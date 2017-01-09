#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define WINVER 0x0601 /*API Windows 7*/
#define _GNU_SOURCE
#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "Crypt32.lib") 
#include <Wincrypt.h>

//#include <winsock.h>
//#include <wincrypt.h>
#include <wintrust.h>
#include <schannel.h>
#include <time.h>
#define SECURITY_WIN32
#include <security.h>
#include <sspi.h>
#define IS_SOCKET_ERROR(a) (a==SOCKET_ERROR)
#define INVALID_VALUE INVALID_HANDLE_VALUE
#define snprintf _snprintf

#define IO_BUFFER_SIZE  0x10000
#define NT4_DLL_NAME TEXT("Security.dll")
#define OBJECT_NAME_LENGTH_MAX 256

// Опции пользователя.
static INT     iPortNumber = 12443;
static LPSTR   pszUserName = "ture my";
static DWORD   dwProtocol = 0;

static HCERTSTORE  hMyCertStore = 0;

static PSecurityFunctionTable g_pSSPI;

static CHAR IoBuffer[IO_BUFFER_SIZE];
static DWORD cbIoBuffer = 0;

static HRESULT CreateCredentials(LPSTR pszUserName,PCredHandle phCreds);

static void WebServer(CredHandle *phServerCreds);

static BOOL ParseRequest(IN PCHAR InputBuffer,IN INT InputBufferLength,OUT PCHAR ObjectName,OUT DWORD *pcbContentLength);

static BOOL SSPINegotiateLoop(SOCKET Socket, PCtxtHandle phContext, PCredHandle phCred, BOOL fDoInitialRead, BOOL NewContext);

static LONG DisconnectFromClient(SOCKET Socket, PCredHandle     phCreds, CtxtHandle *    phContext);

static void PrintHexDump(DWORD length, PCHAR buffer);

// Функция загрузки библиотеки.
HMODULE g_hSecurity = NULL;


static BOOL LoadSecurityLibrary(void) {
	INIT_SECURITY_INTERFACE         pInitSecurityInterface;

	g_hSecurity = LoadLibrary(NT4_DLL_NAME);
	if(g_hSecurity == NULL) {
		printf("Error 0x%x loading %s.\n", GetLastError(), NT4_DLL_NAME);
		return FALSE;
	}

	pInitSecurityInterface = (INIT_SECURITY_INTERFACE)GetProcAddress(g_hSecurity,"InitSecurityInterfaceA");


	if(pInitSecurityInterface == NULL) {
		printf("Error 0x%x reading InitSecurityInterface entry point.\n",  GetLastError());
		return FALSE;
	}
	g_pSSPI = pInitSecurityInterface();

	if(g_pSSPI == NULL) {
		printf("Error 0x%x reading security interface.\n", GetLastError());
		return FALSE;
	}

	return TRUE;
} 

int main(int argc, char *argv[]) {

	WSADATA WsaData;

	CredHandle hServerCreds;

	INT i;
	INT iOption;
	PCHAR pszOption;
	
	if(!LoadSecurityLibrary()) {
		printf("Error initializing the security library\n");
		return 1;
	}



	// Инициализация подсистемы WinSock.
	if(WSAStartup(0x0101, &WsaData) == SOCKET_ERROR) {
		printf("Error %d returned by WSAStartup\n", GetLastError());
		exit(1);
	}

	// Создание мандатов сервера.
	if(CreateCredentials(pszUserName, &hServerCreds)) {
		printf("Error creating credentials\n");
		exit(1);
	}


	WebServer(&hServerCreds);

	// Освобождение дескриптора SSPI мандатов.
	g_pSSPI->FreeCredentialsHandle(&hServerCreds);


	// Завершение работы подсистемы WinSock.
	WSACleanup();


	// Закрытие хранилища сертификатов "MY".
	if(hMyCertStore) {
		CertCloseStore(hMyCertStore, 0);
	}

	exit(0);
} 

// Функция создания соединения с web сервером.
static void WebServer(CredHandle *phServerCreds) {
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET Socket = INVALID_SOCKET;
	SOCKADDR_IN address;
	SOCKADDR_IN remoteAddress;
	INT remoteSockaddrLength;
	DWORD cConnections = 0;
	INT err;
	INT i;

	CHAR objectName[OBJECT_NAME_LENGTH_MAX];
	DWORD currentDirectoryLength;

	HANDLE objectHandle = INVALID_VALUE;

	INT bytesSent;
	DWORD bytesRead;
	DWORD cbContentLength;

	CtxtHandle      hContext;
	BOOL            fContextInitialized = FALSE;
	SecBufferDesc   Message;
	SecBuffer       Buffers[4];
	SecBufferDesc   MessageOut;
	SecBuffer       OutBuffers[4];
	SecPkgContext_StreamSizes Sizes;
	SECURITY_STATUS scRet;

#if 1
	HANDLE hUserToken = NULL;
	BOOL fImpersonating = FALSE;
#endif

	//
	// Инициализация структур закрытых буферов
	//

	Message.ulVersion = SECBUFFER_VERSION;
	Message.cBuffers = 4;
	Message.pBuffers = Buffers;

	Buffers[0].BufferType = SECBUFFER_EMPTY;
	Buffers[1].BufferType = SECBUFFER_EMPTY;
	Buffers[2].BufferType = SECBUFFER_EMPTY;
	Buffers[3].BufferType = SECBUFFER_EMPTY;

	MessageOut.ulVersion = SECBUFFER_VERSION;
	MessageOut.cBuffers = 4;
	MessageOut.pBuffers = OutBuffers;

	OutBuffers[0].BufferType = SECBUFFER_EMPTY;
	OutBuffers[1].BufferType = SECBUFFER_EMPTY;
	OutBuffers[2].BufferType = SECBUFFER_EMPTY;
	OutBuffers[3].BufferType = SECBUFFER_EMPTY;




	// Вычисление текущей директории для получения необходимых файлов из нее.
	currentDirectoryLength = GetCurrentDirectory(OBJECT_NAME_LENGTH_MAX, objectName);
	if(currentDirectoryLength == 0) {
		printf("GetCurrentDirectory failed: %ld\n", GetLastError());
		exit(1);
	}



	
	// Настройка сокета, слушающего порт HTTPS.
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(ListenSocket == INVALID_SOCKET) {
		printf("socket() failed for ListenSocket: %ld\n", GetLastError());
		exit(1);
	}

	RtlZeroMemory(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons((short)iPortNumber);    // порт https
	address.sin_addr.s_addr = 0;

	err = bind(ListenSocket, (LPSOCKADDR)&address, sizeof(address));
	if(IS_SOCKET_ERROR(err)) {
		printf("bind failed: %ld\n", GetLastError());
		exit(1);
	}

	err = listen(ListenSocket, 1);
	if(IS_SOCKET_ERROR(err)) {
		printf("listen failed: %ld\n", GetLastError());
		exit(1);
	}


	
	// Цикл по соединениям.
	while(TRUE) {
		PSecBuffer pDataBuffer;
		int filelen;


		fContextInitialized = FALSE;

		objectHandle = INVALID_VALUE;

		
		// В первую очередь получение соединения.
		printf("\nWaiting for connection %d\n", ++cConnections);
		remoteSockaddrLength = sizeof(remoteAddress);
		Socket = accept(ListenSocket, (LPSOCKADDR)&remoteAddress, &remoteSockaddrLength);
		if(Socket == INVALID_SOCKET) {
			printf("accept() failed: %ld\n", GetLastError());
			goto cleanup;
		}

		printf("Socket connection established\n");
				
		// Установление связи
		cbIoBuffer = 0;

		if(!SSPINegotiateLoop(Socket, &hContext, phServerCreds,	TRUE,TRUE)) {
			printf("Couldn't connect\n");
			goto cleanup;
		}

		fContextInitialized = TRUE;


		//
		// Вычисление размера заголовка:
		//

		scRet = g_pSSPI->QueryContextAttributes(&hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);


		if(scRet != SEC_E_OK) {
			printf("Couldn't get Sizes\n");
			goto cleanup;
		}


		//
		// Получение HTTP запроса от клиента.  
		//

		do {
			Buffers[0].pvBuffer = IoBuffer;
			Buffers[0].cbBuffer = cbIoBuffer;
			Buffers[0].BufferType = SECBUFFER_DATA;

			Buffers[1].BufferType = SECBUFFER_EMPTY;
			Buffers[2].BufferType = SECBUFFER_EMPTY;
			Buffers[3].BufferType = SECBUFFER_EMPTY;

			scRet = g_pSSPI->DecryptMessage(&hContext, &Message, 0, NULL);

			if(scRet == SEC_E_INCOMPLETE_MESSAGE) {
				err = recv(Socket, IoBuffer + cbIoBuffer, IO_BUFFER_SIZE - cbIoBuffer, 0);
				if(IS_SOCKET_ERROR(err) || (err == 0)) {
					printf("recv failed: %ld %ld\n", err, GetLastError());
					goto cleanup;
				}

				printf("\nReceived %d (request) bytes from client\n", err);

				PrintHexDump(16, IoBuffer + cbIoBuffer);

				cbIoBuffer += err;
			}
		} while(scRet == SEC_E_INCOMPLETE_MESSAGE);

		if(scRet == SEC_I_CONTEXT_EXPIRED) {
			// Клиент закрыл сессию
			goto cleanup;
		}

		if(scRet != SEC_E_OK) {
			printf("Couldn't decrypt, error %lx\n", scRet);
			goto cleanup;
		}
		cbIoBuffer = 0;

		// Расположение буфера данных.
		pDataBuffer = NULL;
		for(i = 1; i < 4; i++) {
			if(Buffers[i].BufferType == SECBUFFER_DATA) {
				pDataBuffer = &Buffers[i];
				break;
			}
		}
		if(pDataBuffer == NULL) {
			goto cleanup;
		}

		// Проверка того, что данные в выходном буфере являются строкой,
		// заканчивающейся нулем, а также вывод этих данных.
		((CHAR *)pDataBuffer->pvBuffer)[pDataBuffer->cbBuffer] = '\0';
		printf("\nMessage is: '%s'\n", pDataBuffer->pvBuffer);


		// Обработка запроса для определения объекта запроса.

		if(!ParseRequest(
			pDataBuffer->pvBuffer,
			pDataBuffer->cbBuffer,
			objectName + currentDirectoryLength,
			&cbContentLength)) {
			printf("Unable to parse message\n");
			goto cleanup;
		}


#ifdef _WIN32
	{
		BY_HANDLE_FILE_INFORMATION fileInfo;
		objectHandle = CreateFileA(
			objectName,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
		if(objectHandle == INVALID_VALUE) {
			printf("CreateFile(%s) failed: %ld\n", objectName, GetLastError());
			goto cleanup;
		}

		// Определение длины файла.

		if(!GetFileInformationByHandle(objectHandle, &fileInfo)) {
			printf("GetFileInformationByHandle failed: %ld\n", GetLastError());
			goto cleanup;
		}

		//
		// Построение заголовка и отклик HTTP.
		//

		filelen = fileInfo.nFileSizeLow;
	}
#else
	{
		struct stat buf;
		int objectHandle = open(objectName, O_RDONLY);
		if(objectHandle == -1) {
			printf("open(%s) failed: %ld\n", objectName, GetLastError());
			goto cleanup;
		}
		if(fstat(objectHandle, &buf)) {
			printf("fstat failed: %ld\n", GetLastError());
			goto cleanup;
		}
		filelen = buf.st_size;
	}
#endif
	ZeroMemory(IoBuffer, Sizes.cbHeader);
	i = snprintf(IoBuffer + Sizes.cbHeader, IO_BUFFER_SIZE,
				 "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n",
				 filelen);

	//
	// Построение буферов таким образом, чтобы заголовок и содержимое 
	// файла полностью в них поместились.
	//

	Buffers[0].pvBuffer = IoBuffer;
	Buffers[0].cbBuffer = Sizes.cbHeader;
	Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;

	Buffers[1].pvBuffer = IoBuffer + Sizes.cbHeader;
	Buffers[1].cbBuffer = i;
	Buffers[1].BufferType = SECBUFFER_DATA;

	Buffers[2].pvBuffer = IoBuffer + Sizes.cbHeader + i;
	Buffers[2].cbBuffer = Sizes.cbTrailer;
	Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

	Buffers[3].BufferType = SECBUFFER_EMPTY;

	scRet = g_pSSPI->EncryptMessage(&hContext, 0, &Message, 0);

	if(FAILED(scRet)) {
		printf(" EncryptMessage failed with 0x%x\n", scRet);
		goto cleanup;
	}


	err = send(Socket,
			   IoBuffer,
			   Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer,
			   0);

	printf("\nSend %d header bytes to client\n", Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer);
	PrintHexDump(16, IoBuffer);

	if(IS_SOCKET_ERROR(err)) {
		printf("send failed: %ld\n", GetLastError());
		goto cleanup;
	}

	//
	// Чтение и посылка данных из файла.
	//

	for(bytesSent = 0;
		bytesSent < (INT)filelen;
		bytesSent += bytesRead) {

#ifdef _WIN32
		if(!ReadFile(objectHandle,
			IoBuffer + Sizes.cbHeader,
			IO_BUFFER_SIZE - (Sizes.cbHeader + Sizes.cbTrailer),
			&bytesRead,
			NULL)) {
			printf("ReadFile failed: %ld\n", GetLastError());
			break;
		}
#else
		if((bytesRead = read(objectHandle,
			IoBuffer + Sizes.cbHeader,
			IO_BUFFER_SIZE - (Sizes.cbHeader + Sizes.cbTrailer)) == -1)) {
			printf("read failed: %ld\n", GetLastError());
			break;
		}
#endif

		if(bytesRead == 0) {
			printf("zero bytes read\n");
			break;
		}


		Buffers[0].pvBuffer = IoBuffer;
		Buffers[0].cbBuffer = Sizes.cbHeader;
		Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;

		Buffers[1].pvBuffer = IoBuffer + Sizes.cbHeader;
		Buffers[1].cbBuffer = bytesRead;
		Buffers[1].BufferType = SECBUFFER_DATA;

		Buffers[2].pvBuffer = IoBuffer + Sizes.cbHeader + bytesRead;
		Buffers[2].cbBuffer = Sizes.cbTrailer;
		Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

		Buffers[3].BufferType = SECBUFFER_EMPTY;

		scRet = g_pSSPI->EncryptMessage(&hContext,
										0,
										&Message,
										0);

		if(FAILED(scRet)) {
			printf(" EncryptMessage failed with 0x%x\n", scRet);
			goto cleanup;
		}

		err = send(Socket,
				   IoBuffer,
				   Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer,
				   0);

		printf("\nSend %d data bytes to client\n", Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer);
		PrintHexDump(16, IoBuffer);

		if(IS_SOCKET_ERROR(err)) {
			printf("send failed: %ld\n", GetLastError());
			break;
		}
	}

cleanup:


	if(fImpersonating) {
		RevertToSelf();
		fImpersonating = FALSE;
	}

	if(hUserToken) {
		CloseHandle(hUserToken);
		hUserToken = NULL;
	}


	if(fContextInitialized) {
		scRet = DisconnectFromClient(Socket, phServerCreds, &hContext);

		if(scRet == SEC_E_OK) {
			fContextInitialized = FALSE;
			Socket = INVALID_SOCKET;
		} else {
			printf("Error disconnecting from server\n");
		}
	}

	if(objectHandle != INVALID_VALUE) {
		CloseHandle(objectHandle);
	}

	// Освобождение дескриптора SSPI контекста.
	if(fContextInitialized) {
		g_pSSPI->DeleteSecurityContext(&hContext);
	}

	// Закрытие сокета.
	if(Socket != INVALID_SOCKET) {
		closesocket(Socket);
	}
	}

} // WebServer

//-------------------------------------------------------------
// Функция установления связи.

static BOOL SSPINegotiateLoop(SOCKET          Socket,PCtxtHandle     phContext,PCredHandle     phCred,BOOL            fDoInitialRead,BOOL            NewContext) {
	TimeStamp            tsExpiry;
	SECURITY_STATUS      scRet;
	SecBufferDesc        InBuffer;
	SecBufferDesc        OutBuffer;
	SecBuffer            InBuffers[2];
	SecBuffer            OutBuffers[1];
	DWORD                err = 0;

	BOOL                 fDoRead;
	BOOL                 fInitContext = NewContext;

	DWORD                dwSSPIFlags;
	unsigned long        dwSSPIOutFlags;

	fDoRead = fDoInitialRead;

	dwSSPIFlags = ASC_REQ_SEQUENCE_DETECT 
		         | ASC_REQ_REPLAY_DETECT 
				 | ASC_REQ_CONFIDENTIALITY 
				 | ASC_REQ_EXTENDED_ERROR 
				 | ASC_REQ_ALLOCATE_MEMORY 
				 | ASC_REQ_STREAM;

	
	//  Установка OutBuffer для вызова InitializeSecurityContext
	OutBuffer.cBuffers  = 1;
	OutBuffer.pBuffers  = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;
	
	scRet = SEC_I_CONTINUE_NEEDED;
	while(scRet == SEC_I_CONTINUE_NEEDED ||
		  scRet == SEC_E_INCOMPLETE_MESSAGE ||
		  scRet == SEC_I_INCOMPLETE_CREDENTIALS) {

		if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE) {
			if(fDoRead) {
				err = recv(Socket, IoBuffer + cbIoBuffer, IO_BUFFER_SIZE, 0);

				if(IS_SOCKET_ERROR(err) || err == 0) {
					printf(" recv failed: %ld %d\n", err, GetLastError());
					return FALSE;
				} else {
					printf("\nReceived %d (handshake) bytes from client\n", err);
					PrintHexDump(min(16, err), IoBuffer + cbIoBuffer);

					cbIoBuffer += err;
				}
			} else {
				fDoRead = TRUE;
			}
		}

		
		// InBuffers[1] используется для получения дополнительных данные,
		//  которые не были обработаны SSPI/SCHANNEL на данном шаге цикла.
		InBuffers[0].pvBuffer = IoBuffer;
		InBuffers[0].cbBuffer = cbIoBuffer;
		InBuffers[0].BufferType = SECBUFFER_TOKEN;

		InBuffers[1].pvBuffer = NULL;
		InBuffers[1].cbBuffer = 0;
		InBuffers[1].BufferType = SECBUFFER_EMPTY;

		InBuffer.cBuffers = 2;
		InBuffer.pBuffers = InBuffers;
		InBuffer.ulVersion = SECBUFFER_VERSION;

		// Инициализация производится таким образом, чтобы pvBuffer содержал NULL. 
		// Это сделано для того, чтобы 
		// в случае неудачи не было необходимости выполнять
		// освобождение памяти.
		OutBuffers[0].pvBuffer = NULL;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = 0;


		scRet = g_pSSPI->AcceptSecurityContext( phCred,
												(fInitContext ? NULL : phContext),
												&InBuffer,
												dwSSPIFlags,
												SECURITY_NATIVE_DREP,
												(fInitContext ? phContext : NULL),
												&OutBuffer,
												&dwSSPIOutFlags,
												&tsExpiry);
		fInitContext = FALSE;


		if(scRet == SEC_E_OK ||
		   scRet == SEC_I_CONTINUE_NEEDED ||
		   (FAILED(scRet) && (0 != (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR)))) {
			if(OutBuffers[0].cbBuffer != 0 &&
			   OutBuffers[0].pvBuffer != NULL) {
				
				// Посылка ответа клиенту
				err = send(Socket,
						   OutBuffers[0].pvBuffer,
						   OutBuffers[0].cbBuffer,
						   0);

				printf("\nSend %d handshake bytes to client\n", OutBuffers[0].cbBuffer);
				PrintHexDump(16, OutBuffers[0].pvBuffer);

				g_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer);
				OutBuffers[0].pvBuffer = NULL;
			}
		}


		if(scRet == SEC_E_OK) {


			if(InBuffers[1].BufferType == SECBUFFER_EXTRA) {

				memcpy(IoBuffer,
					   (LPBYTE)(IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer)),
					   InBuffers[1].cbBuffer);
				cbIoBuffer = InBuffers[1].cbBuffer;
			} else {
				cbIoBuffer = 0;
			}
			{
				//The sample of using tsExpiry parametre. Valid only on last call 
				//of AcceptSecurityContext
				double hi = tsExpiry.HighPart;
				double lo = tsExpiry.LowPart;
				// Convert 100-ns interval since January 1, 1601 (UTC) 
				// to 1-sec interval science January 1, 1970, UTC
				time_t clock_1970 = (time_t)(
					((ldexp(hi, 32) + lo)*100.e-9)
					- 11644473600. //SystemTimeToFileTime({.wYear = 1970, .wMonth = 1, .wDay = 1}... 
					);
				// Convert UTC time_t to local time string
				printf("Security Context of this session valid till %s (local time)\n", ctime(&clock_1970));
			}
			{
				// Sample for usage QueryContextAttributes() & FileTimeToSystemTime()
				FILETIME   ft;
				SYSTEMTIME st;
				unsigned hi;
				unsigned lo;
				SecPkgContext_Lifespan ls;

				scRet = g_pSSPI->QueryContextAttributes(phContext,
														SECPKG_ATTR_LIFESPAN, &ls);
				hi = ls.tsStart.HighPart;
				lo = ls.tsStart.LowPart;
				memcpy(&ft, &ls.tsStart, sizeof(ft));
				FileTimeToSystemTime(&ft, &st);
				printf("Connection start {%x, %x}: %d/%d/%d %d:%d:%d.%03d UTC\n",
					   hi, lo,
					   st.wYear, st.wMonth, st.wDay,
					   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

				hi = ls.tsExpiry.HighPart;
				lo = ls.tsExpiry.LowPart;
				memcpy(&ft, &ls.tsExpiry, sizeof(ft));
				FileTimeToSystemTime(&ft, &st);
				printf("Connection expiry {%x, %x}: %d/%d/%d %d:%d:%d.%03d UTC\n",
					   hi, lo,
					   st.wYear, st.wMonth, st.wDay,
					   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			}
			return TRUE;
		} else if(FAILED(scRet) && (scRet != SEC_E_INCOMPLETE_MESSAGE)) {

			printf("Accept Security Context Failed with error code %lx\n", scRet);
			return FALSE;

		}



		if(scRet != SEC_E_INCOMPLETE_MESSAGE &&
		   scRet != SEC_I_INCOMPLETE_CREDENTIALS) {


			if(InBuffers[1].BufferType == SECBUFFER_EXTRA) {



				memcpy(IoBuffer,
					   (LPBYTE)(IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer)),
					   InBuffers[1].cbBuffer);
				cbIoBuffer = InBuffers[1].cbBuffer;
			} else {
				//
				// подготовка к следующему получению данных
				//

				cbIoBuffer = 0;
			}
		}
	}

	return FALSE;
} //SSPINegotiateLoop


//-------------------------------------------------------------
// Функция обработки запроса.
static BOOL ParseRequest(IN PCHAR InputBuffer,IN INT InputBufferLength,OUT PCHAR ObjectName,OUT DWORD *pcbContentLength) {
	PCHAR s = InputBuffer;
	DWORD i;

	*pcbContentLength = 0;

	while((INT)(s - InputBuffer) < InputBufferLength) {

		// Обработка команды

		//
		// Первоначально определяется начинается ли данная строчка 
		// с командны GET.
		//

		while(*s != '\0' && *s != ' ' && *s != '\t') {
			*s = (CHAR)toupper(*s);
			s++;
		}

		if(*s == '\0' || *s == ' ' || *s == '\0') {
			*s = '\0';
			//            printf("Verb is :%s\n", InputBuffer);

			//
			// Это команда GET. Пропуск пробелов и символов табуляции.
			//
			for(s++; *s == ' ' || *s == '\t'; s++);

			//
			// Получение имени объекта.
			//

			for(i = 0; *s != 0xA && *s != 0xD && *s != ' ' && *s != '\0'; s++, i++) {
				ObjectName[i] = *s;
			}

			ObjectName[i] = '\0';

			//
			// Разбор завершен.
			//

			if(strcmp(ObjectName, "/") == 0) {
				strncpy(ObjectName, "/Default.Htm", OBJECT_NAME_LENGTH_MAX);
			}
			if(strcmp(InputBuffer, "POST") == 0) {
				char * content_length;
				DWORD cbContent = 0;
				// Поиск длины содержимого;
				content_length = strstr(s, "Content-Length: ");
				if(content_length) {
					cbContent = atoi(content_length + 16);
					printf("Content Length is %d\n", cbContent);
					*pcbContentLength = cbContent;
				}
			}
			return TRUE;
		}

		//
		// Пропус концы строки и продолжение разбора.
		//

		while(*s != 0xA && *s != 0xD) {
			s++;
		}

		s++;

		if(*s == 0xD || *s == 0xA) {
			s++;
		}
	}

	return FALSE;

} // ParseRequest




// Функция создания мандатов.
static HRESULT CreateCredentials(LPSTR pszUserName,PCredHandle phCreds) {
	SCHANNEL_CRED   SchannelCred;
	TimeStamp       tsExpiry;
	SECURITY_STATUS Status;
	PCCERT_CONTEXT  pCertContext = NULL;

	if(pszUserName == NULL || strlen(pszUserName) == 0) {
		printf("**** No user name specified!\n");
		return SEC_E_NO_CREDENTIALS;
	}

	// Открытие хранилища сертификатов "MY".
	hMyCertStore = CertOpenSystemStore(0, "MY");

	if(!hMyCertStore) {
		printf("**** Error 0x%x returned by CertOpenSystemStore\n",   GetLastError());
		return SEC_E_NO_CREDENTIALS;
	}

	/*Поиск сертификата. В данном примере осуществляется поиск сертификата по subject name, содержащем заданное в командной строке имя пользователя.*/
	pCertContext = CertFindCertificateInStore(hMyCertStore,
											  X509_ASN_ENCODING,
											  0,
											  CERT_FIND_SUBJECT_STR_A,
											  pszUserName,
											  NULL);
	if(pCertContext == NULL) {
		printf("**** Error 0x%x returned by CertFindCertificateInStore\n",
			   GetLastError());
		return SEC_E_NO_CREDENTIALS;
	}


	/*Построение структуры Schannel мандатов. 
      В данном примере определяются используемые протокол и сертификат.*/

	ZeroMemory(&SchannelCred, sizeof(SchannelCred));

	SchannelCred.dwVersion             = SCHANNEL_CRED_VERSION;
	SchannelCred.cCreds                = 1;
	SchannelCred.paCred                = &pCertContext;
	SchannelCred.grbitEnabledProtocols = dwProtocol;
	
	// Создание SSPI мандата.
	Status = g_pSSPI->AcquireCredentialsHandle( NULL,                   // Имя администратора
												UNISP_NAME_A,           // Имя пакета
												SECPKG_CRED_INBOUND,    // Флаг, определяющий использование
												NULL,                   // Указатель на идентификатор пароля
												&SchannelCred,          // Данные пакета
												NULL,                   // Указатель на функицю GetKey()
												NULL,                   // Значения, передаваемые функции GetKey()
												phCreds,                // (out) Даскриптор мандата
												&tsExpiry);             // (out) Период актуальности (необязательно)
	if(Status != SEC_E_OK) {
		printf("**** Error 0x%x returned by AcquireCredentialsHandle\n", Status);
		return Status;
	}

	// Освобождение контекста сертификата. В Schannel уже создана его копия.
	if(pCertContext) {
		CertFreeCertificateContext(pCertContext);
	}


	return SEC_E_OK;
}

//-------------------------------------------------------------
// Функция, выполняющая разрыв соединения с клиентом.
static LONG DisconnectFromClient(SOCKET          Socket,PCredHandle     phCreds,CtxtHandle *    phContext) {
	DWORD           dwType;
	PCHAR           pbMessage;
	DWORD           cbMessage;
	DWORD           cbData;

	SecBufferDesc   OutBuffer;
	SecBuffer       OutBuffers[1];
	DWORD           dwSSPIFlags;
	unsigned long   dwSSPIOutFlags;
	TimeStamp       tsExpiry;
	DWORD           Status;

	//
	// Уведомление schannel о закрытии соединения.
	//

	dwType = SCHANNEL_SHUTDOWN;

	OutBuffers[0].pvBuffer = &dwType;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = sizeof(dwType);

	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	Status = g_pSSPI->ApplyControlToken(phContext, &OutBuffer);

	if(FAILED(Status)) {
		printf("**** Error 0x%x returned by ApplyControlToken\n", Status);
		goto cleanup;
	}

	//
	// Построение SSL сообщения, являющегося уведомлением о закрытии.
	//

	dwSSPIFlags = ASC_REQ_SEQUENCE_DETECT |
		ASC_REQ_REPLAY_DETECT |
		ASC_REQ_CONFIDENTIALITY |
		ASC_REQ_EXTENDED_ERROR |
		ASC_REQ_ALLOCATE_MEMORY |
		ASC_REQ_STREAM;

	OutBuffers[0].pvBuffer = NULL;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = 0;

	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	Status = g_pSSPI->AcceptSecurityContext(
		phCreds,
		phContext,
		NULL,
		dwSSPIFlags,
		SECURITY_NATIVE_DREP,
		NULL,
		&OutBuffer,
		&dwSSPIOutFlags,
		&tsExpiry);

	if(FAILED(Status)) {
		printf("**** Error 0x%x returned by AcceptSecurityContext\n", Status);
		goto cleanup;
	}

	pbMessage = OutBuffers[0].pvBuffer;
	cbMessage = OutBuffers[0].cbBuffer;


	//
	// Посылка этого сообщения клиенту.
	//

	if(pbMessage != NULL && cbMessage != 0) {
		cbData = send(Socket, pbMessage, cbMessage, 0);
		if(IS_SOCKET_ERROR(cbData) || cbData == 0) {
			Status = WSAGetLastError();
			printf("**** Error %d sending close notify\n", Status);
			goto cleanup;
		}

		printf("\n%d bytes of handshake data sent\n", cbData);

		PrintHexDump(min(16, cbData), pbMessage);

		// Освобождение выходного буфера.
		g_pSSPI->FreeContextBuffer(pbMessage);
	}


cleanup:

	// Освобождение закрытого контекста.
	g_pSSPI->DeleteSecurityContext(phContext);

	// Закрытие сокета.
	closesocket(Socket);

	return Status;
} //DisconnectFromClient

//-------------------------------------------------------------
// Функция перевода в 16-ую систему исчесления.
static void PrintHexDump(DWORD length, PCHAR buffer) {
	DWORD i, count, index;
	CHAR rgbDigits[] = "0123456789abcdef";
	CHAR rgbLine[100];
	char cbLine;

	for(index = 0; length; length -= count, buffer += count, index += count) {
		count = (length > 16) ? 16 : length;

		snprintf(rgbLine, 100, "%4.4x  ", index);
		cbLine = 6;

		for(i = 0; i<count; i++) {
			rgbLine[cbLine++] = rgbDigits[buffer[i] >> 4];
			rgbLine[cbLine++] = rgbDigits[buffer[i] & 0x0f];
			if(i == 7) {
				rgbLine[cbLine++] = ':';
			} else {
				rgbLine[cbLine++] = ' ';
			}
		}
		for(; i < 16; i++) {
			rgbLine[cbLine++] = ' ';
			rgbLine[cbLine++] = ' ';
			rgbLine[cbLine++] = ' ';
		}

		rgbLine[cbLine++] = ' ';

		for(i = 0; i < count; i++) {
			if(buffer[i] < 32 || buffer[i] > 126 || buffer[i] == '%') {
				rgbLine[cbLine++] = '.';
			} else {
				rgbLine[cbLine++] = buffer[i];
			}
		}

		rgbLine[cbLine] = 0;
		printf("%s\n", rgbLine);
	}
}