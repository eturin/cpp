#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS



#define SECURITY_WIN32
//SECURITY_KERNEL

#define WINVER 0x0601 /*API Windows 7*/
#define _GNU_SOURCE
#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "Secur32.lib") 
#include <Ntsecapi.h>
#include <schannel.h>
#include <Security.h>

#pragma comment(lib, "Crypt32.lib") 
#include <Wincrypt.h>

#include <locale.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// Функция вывода ошибки полученной при проверке цепочки сертификатов.
static void DisplayWinVerifyTrustError(DWORD Status) {
	LPSTR pszName = NULL;

	switch(Status) {
		case CERT_E_EXPIRED:                pszName = "CERT_E_EXPIRED";                 break;
		case CERT_E_VALIDITYPERIODNESTING:  pszName = "CERT_E_VALIDITYPERIODNESTING";   break;
		case CERT_E_ROLE:                   pszName = "CERT_E_ROLE";                    break;
		case CERT_E_PATHLENCONST:           pszName = "CERT_E_PATHLENCONST";            break;
		case CERT_E_CRITICAL:               pszName = "CERT_E_CRITICAL";                break;
		case CERT_E_PURPOSE:                pszName = "CERT_E_PURPOSE";                 break;
		case CERT_E_ISSUERCHAINING:         pszName = "CERT_E_ISSUERCHAINING";          break;
		case CERT_E_MALFORMED:              pszName = "CERT_E_MALFORMED";               break;
		case CERT_E_UNTRUSTEDROOT:          pszName = "CERT_E_UNTRUSTEDROOT";           break;
		case CERT_E_CHAINING:               pszName = "CERT_E_CHAINING";                break;
		case TRUST_E_FAIL:                  pszName = "TRUST_E_FAIL";                   break;
		case CERT_E_REVOKED:                pszName = "CERT_E_REVOKED";                 break;
		case CERT_E_UNTRUSTEDTESTROOT:      pszName = "CERT_E_UNTRUSTEDTESTROOT";       break;
		case CERT_E_REVOCATION_FAILURE:     pszName = "CERT_E_REVOCATION_FAILURE";      break;
		case CERT_E_CN_NO_MATCH:            pszName = "CERT_E_CN_NO_MATCH";             break;
		case CERT_E_WRONG_USAGE:            pszName = "CERT_E_WRONG_USAGE";             break;
		default:                            pszName = "(unknown)";                      break;
	}

	printf("Ошибка 0x%x (%s) получена из CertVerifyCertificateChainPolicy!\n", Status, pszName);
} 

// Функция проверки сертификата сервера.
static DWORD VerifyServerCertificate(PCCERT_CONTEXT pServerCert, PSTR pszServerName, DWORD dwCertFlags) {
	HRESULT   Status = SEC_E_OK;;

	PWSTR   pwszServerName = NULL;
	PCCERT_CHAIN_CONTEXT     pChainContext = NULL;
		

	do {
		if(pServerCert == NULL) {
			Status = SEC_E_WRONG_PRINCIPAL;
			break;
		}
		
		// Преобразование имени сервера pszServerName в unicode.
		if(pszServerName == NULL || strlen(pszServerName) == 0) {
			Status = SEC_E_WRONG_PRINCIPAL;
			break;
		}

		DWORD   cchServerName;
		cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, NULL, 0);
		pwszServerName = LocalAlloc(LMEM_FIXED, cchServerName * sizeof(WCHAR));
		if(pwszServerName == NULL) {
			Status = SEC_E_INSUFFICIENT_MEMORY;
			break;
		}
		cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, pwszServerName, cchServerName);
		if(cchServerName == 0) {
			Status = SEC_E_WRONG_PRINCIPAL;
			break;
		}


		
		// Построение цепочки сертификатов.
		LPSTR rgszUsages[] = {szOID_PKIX_KP_SERVER_AUTH,
			                  szOID_SERVER_GATED_CRYPTO,
			                  szOID_SGC_NETSCAPE};
		DWORD cUsages = sizeof(rgszUsages) / sizeof(LPSTR);
		CERT_CHAIN_PARA          ChainPara;
		ZeroMemory(&ChainPara, sizeof(ChainPara));
		ChainPara.cbSize = sizeof(ChainPara);
		ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_OR;
		ChainPara.RequestedUsage.Usage.cUsageIdentifier = cUsages;
		ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = rgszUsages;

		if(!CertGetCertificateChain(NULL,
									pServerCert,
									NULL,
									pServerCert->hCertStore,
									&ChainPara,
									0,
									NULL,
									&pChainContext)) {
			Status = GetLastError();
			printf("Ошибка 0x%x получена из CertGetCertificateChain!\n", Status);
			break;
		}


		
		// Проверка цепочки сертификатов.
		HTTPSPolicyCallbackData  polHttps;
		ZeroMemory(&polHttps, sizeof(HTTPSPolicyCallbackData));
		polHttps.cbStruct       = sizeof(HTTPSPolicyCallbackData);
		polHttps.dwAuthType     = AUTHTYPE_SERVER;
		polHttps.fdwChecks      = dwCertFlags;
		polHttps.pwszServerName = pwszServerName;

		CERT_CHAIN_POLICY_PARA   PolicyPara;
		memset(&PolicyPara, 0, sizeof(PolicyPara));
		PolicyPara.cbSize            = sizeof(PolicyPara);
		PolicyPara.pvExtraPolicyPara = &polHttps;

		CERT_CHAIN_POLICY_STATUS PolicyStatus;
		memset(&PolicyStatus, 0, sizeof(PolicyStatus));
		PolicyStatus.cbSize = sizeof(PolicyStatus);

		if(!CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_SSL,
											 pChainContext,
											 &PolicyPara,
											 &PolicyStatus)) {
			Status = GetLastError();
			printf("Ошибка 0x%x получена из CertVerifyCertificateChainPolicy!\n", Status);
			break;
		}

		if(PolicyStatus.dwError) {
			Status = PolicyStatus.dwError;
			DisplayWinVerifyTrustError(Status);
			break;
		}
	} while(FALSE);

	if(pChainContext) 
		CertFreeCertificateChain(pChainContext);
	

	if(pwszServerName) 
		LocalFree(pwszServerName);
	

	return Status;
} // VerifyServerCertificate

// Функция получения пновых мандатов клиента.
static void GetNewClientCredentials(SecurityFunctionTable* tab, CredHandle *phCreds, CtxtHandle *phContext, SCHANNEL_CRED  * pSchannelCred, HCERTSTORE *phMyCertStore) {
	CredHandle hCreds;
	
	PCCERT_CHAIN_CONTEXT pChainContext;
	
	PCCERT_CONTEXT  pCertContext;
	
	// Чтение спсика доверенных издателей из schannel
	SecPkgContext_IssuerListInfoEx IssuerListInfo;
	SECURITY_STATUS  Status = tab->QueryContextAttributes(phContext, SECPKG_ATTR_ISSUER_LIST_EX, (PVOID)&IssuerListInfo);
	if(Status != SEC_E_OK) {
		printf("Ошибка чтения списка доверенных издателей из schannel 0x%x \n", Status);
		return;
	}

	
	// Перечисление сертификатов клиента.	
	CERT_CHAIN_FIND_BY_ISSUER_PARA FindByIssuerPara;
	ZeroMemory(&FindByIssuerPara, sizeof(FindByIssuerPara));

	FindByIssuerPara.cbSize = sizeof(FindByIssuerPara);
	FindByIssuerPara.pszUsageIdentifier = szOID_PKIX_KP_CLIENT_AUTH;
	FindByIssuerPara.dwKeySpec = 0;
	FindByIssuerPara.cIssuer = IssuerListInfo.cIssuers;
	FindByIssuerPara.rgIssuer = IssuerListInfo.aIssuers;

	pChainContext = NULL;

	while(TRUE) {
		// Поиск цепочки сертификатов.
		pChainContext = CertFindChainInStore(*phMyCertStore,
											 X509_ASN_ENCODING,
											 0,
											 CERT_CHAIN_FIND_BY_ISSUER,
											 &FindByIssuerPara,
											 pChainContext);
		if(pChainContext == NULL) {
			printf("Ошибка 0x%x поиска цепочки сертификата\n", GetLastError());
			break;
		}
		printf("\nцепочка сертификата найдена\n");

		// Получение указателя на контекс сертифика-листа.
		pCertContext = pChainContext->rgpChain[0]->rgpElement[0]->pCertContext;

		// Создание schannel мандата.
		pSchannelCred->dwVersion = SCHANNEL_CRED_VERSION;
		pSchannelCred->cCreds    = 1;
		pSchannelCred->paCred    = &pCertContext;

		TimeStamp tsExpiry;
		Status = tab->AcquireCredentialsHandleA(NULL,                   // Имя администратора
			                                    UNISP_NAME_A,           // Имя пакета
			                                    SECPKG_CRED_OUTBOUND,   // Флаг, определяющий использование
			                                    NULL,                   // Указатель на идентификатор пароля
			                                    pSchannelCred,          // Данные пакета
			                                    NULL,                   // Указатель на функицю GetKey()
			                                    NULL,                   // Значения, передаваемые функции GetKey()
			                                    &hCreds,                // (out) Даскриптор мандата
			                                    &tsExpiry);             // (out) Период актуальности (необязательно)
		if(Status != SEC_E_OK) {
			printf("**** Ошибка 0x%x в AcquireCredentialsHandle\n", Status);
			continue;
		}
		printf("\nновый дескриптор создан\n");

		// Уничтожение старых мандатов.
		tab->FreeCredentialsHandle(phCreds);

		*phCreds = hCreds;

		break;
	}
}

SECURITY_STATUS ClientHandshakeLoop(SecurityFunctionTable* tab, SOCKET Socket, PCredHandle phCreds, CtxtHandle * phContext, BOOL fDoInitialRead, SecBuffer * pExtraData, SCHANNEL_CRED  * pSchannelCred, HCERTSTORE *phMyCertStore) {
	
	SecBufferDesc   InBuffer;
	SecBuffer       InBuffers[2];
	
	SecBufferDesc   OutBuffer;
	SecBuffer       OutBuffers[1];
		
	unsigned long   dwSSPIOutFlags;
	TimeStamp       tsExpiry;
	SECURITY_STATUS scRet;
	DWORD           cbData;
	
	DWORD dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT
		                | ISC_REQ_REPLAY_DETECT
		                | ISC_REQ_CONFIDENTIALITY
		                | ISC_RET_EXTENDED_ERROR
		                | ISC_REQ_ALLOCATE_MEMORY  //SSP отводит память для буферов. Ее необходимо освободить вызовом FreeContextBuffer
		                | ISC_REQ_STREAM;

	// Размещение буфера данных.	
	#define IO_BUFFER_SIZE  0x8000
	char* IoBuffer = LocalAlloc(LMEM_FIXED, IO_BUFFER_SIZE);
	if(IoBuffer == NULL) {
		printf("**** Не достаточно памяти (1)\n");
		return SEC_E_INTERNAL_ERROR;
	}
	
	DWORD cbIoBuffer = 0;
	BOOL  fDoRead = fDoInitialRead;
	
	// Цикл до тех пока, пока не закончится обмен сообщениями, либо не произойдет ошибка.
	scRet = SEC_I_CONTINUE_NEEDED;
	while(scRet == SEC_I_CONTINUE_NEEDED || scRet == SEC_E_INCOMPLETE_MESSAGE || scRet == SEC_I_INCOMPLETE_CREDENTIALS) {

		
		//чтение частями данных от сервера
		if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE) {
			if(fDoRead) {
				cbData = recv(Socket, IoBuffer + cbIoBuffer, IO_BUFFER_SIZE - cbIoBuffer, 0);
				if(cbData<=0) {
					printf("**** Ошибка чтения данных от сервера %d\n", WSAGetLastError());
					scRet = SEC_E_INTERNAL_ERROR;
					break;				
				}

				printf("Прочитано %d байт в рамках установки соединения\n", cbData);
				cbIoBuffer += cbData;
			} else {
				fDoRead = TRUE;
			}
		}


		/*Установка двух входных буферов:
		  Buffer 0 содержит данные, получаемые от сервера. Schannel поглощает некоторые или все из них. 
		  Оставшиеся данные (в любом случае) располагаются в  buffer 1 и получают тип буфера SECBUFFER_EXTRA.*/
		InBuffers[0].pvBuffer   = IoBuffer;
		InBuffers[0].cbBuffer   = cbIoBuffer;
		InBuffers[0].BufferType = SECBUFFER_TOKEN;

		InBuffers[1].pvBuffer   = NULL;
		InBuffers[1].cbBuffer   = 0;
		InBuffers[1].BufferType = SECBUFFER_EMPTY;

		InBuffer.cBuffers  = 2;
		InBuffer.pBuffers  = InBuffers;
		InBuffer.ulVersion = SECBUFFER_VERSION;

		
		/*Установка выходных буферов:
		  Инициализация производится таким образом, чтобы pvBuffer содержал NULL. 
		  Это сделано для того, чтобы в случае неудачи не было необходимости выполнять освобождение памяти.*/
		OutBuffers[0].pvBuffer = NULL;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = 0;

		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		
		// Вызов InitializeSecurityContext.
		scRet = tab->InitializeSecurityContextA(phCreds              /*дескриптор удостоверения клиента*/,
												phContext            /*При первом вызове, укажите NULL. При последующих - указатель на дескриптор, возвращенный в phNewContext после первого вызова к этой функции.*/,
												NULL                 /*уникальный идентификатор сервера (используется при поиске сессии в кеше сессий при восстановлении соединения)*/,
												dwSSPIFlags          /*набор флагов*/,
												0                    /*Зарезервирован. Должен быть 0*/,
												SECURITY_NATIVE_DREP /*Зарезервирован. Должен быть 0*/,

												&InBuffer            /*указатель на структуру SecBufferDesc.
																	  При первом вызове должен быть NULL.
																	  При последующих вызовах должен состоять из двух структур SecBuffer.
																	  Первый буфер должен иметь тип SECBUFFER_TOKEN, и содержать пакет, полученый от сервера.
																	  Второй буфер должен иметь тип SECBUFFER_EMPTY. В него в случае необходимости будут помещены оставшиеся необработанные входные данные.*/,

												0                    /*Зарезервирован. Должен быть 0*/,
												NULL                 /*Указатель на CtxtHandle.
																	  При первом вызове сюда будет помещен дескриптор нового контекста.
																	  При последующих вызовах полученный дескриптор следует передавать через параметр phContext, а в phNewContext передавать NULL*/,

												&OutBuffer           /*Указатель на структуру SecBufferDesc, содержащую SecBuffer с типом SECBUFFER_TOKEN.
																	  На выходе в этот буфер будет помещен пакет, который нужно переслать серверу.
																	  При наличии флага ISC_REQ_ALLOCATE_MEMORY этот буфер будет отведен.*/,

												&dwSSPIOutFlags      /*Указатель на ULONG, куда будут записаны флаги, описывающие свойства устанавливаемого контекста.
																	  Список возможных значений приведен в описании параметра fContextReq.
																	  pfContextAttr получает тот же набор флагов, с заменой префикса ISC_REQ на ISC_RET.
																	  (Аттрибуты безопасности не следует проверять до последнего вызова функции, возвращающего SEC_E_OK.
																	  Остальные атрибуты, такие как ISC_RET_ALLOCATED_MEMORY можно использовать после первого же вызова)*/,

												&tsExpiry             /*Опционален. Указатель на структуру TimeStamp.
																	   При наличии сертификата клиента, этот параметр получает время истечения этого сертификата.
																	   Иначе возвращается максимальное возможное значение времени. Врямя возвращается в UTC*/);

		
		
		// Если InitializeSecurityContext успешно выполнена (или если произошла одна из распространенных ошибок), то выполняется посылка выходного буфера серверу.
		if(scRet == SEC_E_OK || scRet == SEC_I_CONTINUE_NEEDED || FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR)) {
			if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL) {
				cbData = send(Socket, OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer, 0);
				if(cbData == 0) {
					printf("**** Ошибка чтения данных от сервера %d (2)\n",
						   WSAGetLastError());
					tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
					tab->DeleteSecurityContext(phContext);
					return SEC_E_INTERNAL_ERROR;
				}

				printf("Отправлено %d байт в рамках процедуры установки соединения\n", cbData);
				
				// Освобождение выходного буфера.
				tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
				OutBuffers[0].pvBuffer = NULL;
			}
		}

		// Если InitializeSecurityContext вернула ошибку SEC_E_INCOMPLETE_MESSAGE, тогда необходимо прочитать большее количество данных от сервера (следующий фрагмент) и повторить попытку снова.		
		if(scRet == SEC_E_INCOMPLETE_MESSAGE) 
			continue;
		
		// Если InitializeSecurityContext возвратила SEC_E_OK, тогда обмен данными завершился успешно.
		if(scRet == SEC_E_OK) {
			/*Если "extra" буфер содержит данные, то они являются данными протокола зашифрования. 
			 Их необходимо сохранить. Приложение позже расшифрует их при помощи DecryptMessage.*/
			printf("Соединение установлено\n");

			if(InBuffers[1].BufferType == SECBUFFER_EXTRA) {
				pExtraData->pvBuffer = LocalAlloc(LMEM_FIXED, InBuffers[1].cbBuffer);
				if(pExtraData->pvBuffer == NULL) {
					printf("**** Не достаточно памяти (2)\n");
					return SEC_E_INTERNAL_ERROR;
				}

				MoveMemory(pExtraData->pvBuffer, IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer), InBuffers[1].cbBuffer);

				pExtraData->cbBuffer   = InBuffers[1].cbBuffer;
				pExtraData->BufferType = SECBUFFER_TOKEN;

				printf("В \"extra\" буфер имеется %d байт, полученных при установке соединения\n", pExtraData->cbBuffer);
			} else {
				pExtraData->pvBuffer = NULL;
				pExtraData->cbBuffer = 0;
				pExtraData->BufferType = SECBUFFER_EMPTY;
			}

			
			// Выход
			{
				// пример использования QueryContextAttributes() & native TimeStamp
				SecPkgContext_Lifespan ls;
				double hi, lo;
				time_t clock_1970;

				scRet = tab->QueryContextAttributes(phContext, SECPKG_ATTR_LIFESPAN, &ls);
				hi = ls.tsExpiry.HighPart;
				lo = ls.tsExpiry.LowPart;
				// Convert 100-ns interval since January 1, 1601 (UTC) 
				// to 1-sec interval science January 1, 1970, UTC
				clock_1970 = (time_t)(
					((ldexp(hi, 32) + lo)*100.e-9)
					- 11644473600. //SystemTimeToFileTime({.wYear = 1970, .wMonth = 1, .wDay = 1}... 
					);
				// Convert UTC time_t to local time string
				printf("Контекст соединения действителен до (местное время) %s \n", ctime(&clock_1970));
			}
			{
				// пример использования QueryContextAttributes() & FileTimeToSystemTime()
				FILETIME   ft;
				SYSTEMTIME st;
				unsigned hi;
				unsigned lo;
				SecPkgContext_Lifespan ls;

				scRet = tab->QueryContextAttributes(phContext, SECPKG_ATTR_LIFESPAN, &ls);
				hi = ls.tsStart.HighPart;
				lo = ls.tsStart.LowPart;
				memcpy(&ft, &ls.tsStart, sizeof(ft));
				FileTimeToSystemTime(&ft, &st);
				printf("Сертификат действителен с {%x, %x}: %d/%d/%d %d:%d:%d.%03d UTC\n",
					   hi, lo,
					   st.wYear, st.wMonth, st.wDay,
					   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

				hi = ls.tsExpiry.HighPart;
				lo = ls.tsExpiry.LowPart;
				memcpy(&ft, &ls.tsExpiry, sizeof(ft));
				FileTimeToSystemTime(&ft, &st);
				printf("по {%x, %x}: %d/%d/%d %d:%d:%d.%03d UTC\n",
					   hi, lo,
					   st.wYear, st.wMonth, st.wDay,
					   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			}
			break;
		}


		// Проверка на ошибки.
		if(FAILED(scRet)) {
			printf("**** Получена ошибка 0x%x из InitializeSecurityContext (2)\n", scRet);
			break;
		}


		
		// Если InitializeSecurityContext возвратила SEC_I_INCOMPLETE_CREDENTIALS, то сервер только что запросил аутентификацию клиента. 
		if(scRet == SEC_I_INCOMPLETE_CREDENTIALS) {
			/*Ошибка. Сервер запросил аутентификацию клиента, но переданный мандат не содержит сертификата клиента.
			 
			 Эта функция читает список доверенных сертификатов авторов ("issuers"), полученный от сервера и пытается найти подходящий сертификат клиента. 
			 Если эта функция выполнена успешно, то происходит соединение при помощи нового сертификата.
			 В противном случае осуществляется попытка произвести соединение анонимно (используя текущий мандат).*/
			

			GetNewClientCredentials(tab, phCreds, phContext,pSchannelCred,phMyCertStore);

			// Повторная попытка.
			fDoRead = FALSE;
			scRet = SEC_I_CONTINUE_NEEDED;

			// Исправляем ошибку Platform SDK!
			// Считаем, что за этим сообщением не может следовать другое
			cbIoBuffer = 0;

			continue;
		}


		
		// Копирование всех данных из "extra" буфера и повторная попытка.
		if(InBuffers[1].BufferType == SECBUFFER_EXTRA) {
			MoveMemory(IoBuffer,IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer), InBuffers[1].cbBuffer);
			cbIoBuffer = InBuffers[1].cbBuffer;
		} else {
			cbIoBuffer = 0;
		}
	}

	// Уничтожение закрытого контекста в случае непоправимой ошибки.
	if(FAILED(scRet)) {
		tab->DeleteSecurityContext(phContext);
	}

	LocalFree(IoBuffer);

	return scRet;
} 

// Функция вывода информации о соединении.
void DisplayConnectionInfo(SecurityFunctionTable* tab, CtxtHandle *phContext) {
	
	SecPkgContext_ConnectionInfo ConnectionInfo;

	SECURITY_STATUS Status = tab->QueryContextAttributes(phContext,
											             SECPKG_ATTR_CONNECTION_INFO,
											             (PVOID)&ConnectionInfo);
	if(Status != SEC_E_OK) {
		printf("Ошибка получения информации о соединение 0x%x\n", Status);
		return;
	}

	printf("\n");

	switch(ConnectionInfo.dwProtocol) {
		case SP_PROT_TLS1_CLIENT:			
			printf("Протокол: Transport Layer Security 1.0 client - side.\n");
			break;
		case SP_PROT_TLS1_SERVER:			
			printf("Протокол: Transport Layer Security 1.0 server - side.\n");
			break;
		case SP_PROT_SSL3_CLIENT:			
			printf("Протокол: Secure Sockets Layer 3.0 client - side.\n");
			break;
		case SP_PROT_SSL3_SERVER:			
			printf("Протокол: Secure Sockets Layer 3.0 server - side.\n");
			break;
		case SP_PROT_TLS1_1_CLIENT:			
			printf("Протокол: Transport Layer Security 1.1 client - side.\n");
			break;
		case SP_PROT_TLS1_1_SERVER:			
			printf("Протокол: Transport Layer Security 1.1 server - side.\n");
			break;
		case SP_PROT_TLS1_2_CLIENT:			
			printf("Протокол: Transport Layer Security 1.2 client - side.\n");
			break;
		case SP_PROT_TLS1_2_SERVER:			
			printf("Протокол: Transport Layer Security 1.2 server - side.\n");
			break;
		case SP_PROT_PCT1_CLIENT:			
			printf("Протокол: Private Communications Technology 1.0 client - side.Obsolete.\n");
			break;
		case SP_PROT_PCT1_SERVER:			
			printf("Протокол: Private Communications Technology 1.0 server - side.Obsolete.\n");
			break;
		case SP_PROT_SSL2_CLIENT:			
			printf("Протокол: Secure Sockets Layer 2.0 client - side.Superseded by SP_PROT_TLS1_CLIENT.\n");
			break;
		case SP_PROT_SSL2_SERVER:			
			printf("Протокол: Secure Sockets Layer 2.0 server - side.Superseded by SP_PROT_TLS1_SERVER.\n");
			break;
		default:
			printf("Протокол: 0x%x\n", ConnectionInfo.dwProtocol);
	}

	switch(ConnectionInfo.aiCipher) {
		case CALG_3DES:
			printf("Шифрование: 3DES block encryption algorithm.\n");
			break;
		case CALG_AES_128:
			printf("Шифрование: AES 128 - bit encryption algorithm.\n");
			break;
		case CALG_AES_256:
			printf("Шифрование: AES 256 - bit encryption algorithm.\n");
			break;
		case CALG_DES:
			printf("Шифрование: DES encryption algorithm.\n");
			break;
		case CALG_RC2:
			printf("Шифрование: RC2 block encryption algorithm.\n");
			break;
		case CALG_RC4:
			printf("Шифрование: RC4 stream encryption algorithm.\n");
			break;
		case 0: 
			printf("Шифрование: No encryption.\n");
			break;
		default:
			printf("Шифрование: 0x%x\n", ConnectionInfo.aiCipher);
	}

	printf("Прочность шифрования: %d\n", ConnectionInfo.dwCipherStrength);

	switch(ConnectionInfo.aiHash) {
		case CALG_MD5:
			printf("Хеширование: MD5 hashing algorithm.\n");
			break;
		case CALG_SHA:
			printf("Хеширование: SHA hashing algorithm.\n");
			break;
		default:
			printf("Хеширование: 0x%x\n", ConnectionInfo.aiHash);
	}

	printf("Прочность хеширования: %d\n", ConnectionInfo.dwHashStrength);

	switch(ConnectionInfo.aiExch) {
		case CALG_DH_EPHEM:
			printf("Обмен ключами: DH Ephemeral (Diffie - Hellman key exchange)\n");
			break;
		case CALG_RSA_KEYX:
			printf("Обмен ключами: RSA key exchange\n");
			break;				
		default:
			printf("Обмен ключами: 0x%x\n", ConnectionInfo.aiExch);
	}

	printf("Прочность ключа: %d\n", ConnectionInfo.dwExchStrength);
}

// Функция получения файла при помощи Https.
SECURITY_STATUS HttpsGetFile(SecurityFunctionTable* tab, SOCKET Socket, PCredHandle phCreds, CtxtHandle *phContext, LPSTR pszFileName, SCHANNEL_CRED  * pSchannelCred, HCERTSTORE *phMyCertStore) {
	// Чтение свойств поточного зашифрования.
	SecPkgContext_StreamSizes Sizes;
	SECURITY_STATUS scRet = tab->QueryContextAttributes(phContext,
										                SECPKG_ATTR_STREAM_SIZES,
										                &Sizes);
	if(scRet != SEC_E_OK) {
		printf("**** Ошибка 0x%x чтения  SECPKG_ATTR_STREAM_SIZES\n", scRet);
		return scRet;
	}

	printf("\nРазмер заголовка: %d, размер хвоста: %d, максимальный размер тела сообщения: %d\n",Sizes.cbHeader, Sizes.cbTrailer, Sizes.cbMaximumMessage);

	/*Выделение рабочего буфера. 
	  Открытый текст, передаваемый EncryptMessage, должен ен провосходить по размерам 'Sizes.cbMaximumMessage', 
	  поэтому размер буфера равен этой величене, сложеной с размерами заголовка и заключительной части.*/
	DWORD cbIoBufferLength = Sizes.cbHeader + Sizes.cbMaximumMessage + Sizes.cbTrailer;// Могут быть такие реализации сервера и клиента что длина буфера будет cbIoBufferLength вычисленная таким образом + 2048 байт см. RFC 2246

	char * pbIoBuffer = LocalAlloc(LMEM_FIXED, cbIoBufferLength);
	if(pbIoBuffer == NULL) {
		printf("**** Не достаточно памяти (2)\n");
		return SEC_E_INTERNAL_ERROR;
	}
		
	/*Построение HTTP запроса серверу.
	 Удаление завершающих '/' из имени файла, должен быть только один.*/
	if(pszFileName && strlen(pszFileName) > 1 && pszFileName[strlen(pszFileName) - 1] == '/') 
		pszFileName[strlen(pszFileName) - 1] = 0;	

	/*Построение HTTP запроса, сдвинутого на "header size" байт в буфере.
	 Это позволяет Schannel выполнять операцию зашифрования.*/
	char * pbMessage = pbIoBuffer + Sizes.cbHeader;

	/*Построение HTTP запроса. Он меньше, чем максимальный размер сообщения.
	  Если это не так, что происходит break.*/
	sprintf(pbMessage,"GET /%s HTTP/1.1\r\n"
			          "User-Agent: Webclient\r\n"
				      "Accept:*/*\r\n"
					  "\r\n", pszFileName);
	printf("\nHTTP request: \n%s\n", pbMessage);
	DWORD cbMessage = (DWORD)strlen(pbMessage);
	printf("Размер не зашифрованного запроса: %d байт\n", cbMessage);
		
	/*Зашифрование HTTP запроса.*/
	SecBuffer  Buffers[4];
	Buffers[0].pvBuffer   = pbIoBuffer;
	Buffers[0].cbBuffer   = Sizes.cbHeader;
	Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;

	Buffers[1].pvBuffer   = pbMessage;
	Buffers[1].cbBuffer   = cbMessage;
	Buffers[1].BufferType = SECBUFFER_DATA;

	Buffers[2].pvBuffer   = pbMessage + cbMessage;
	Buffers[2].cbBuffer   = Sizes.cbTrailer;
	Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

	Buffers[3].BufferType = SECBUFFER_EMPTY;

	SecBufferDesc   Message;
	Message.ulVersion = SECBUFFER_VERSION;
	Message.cBuffers  = 4;
	Message.pBuffers  = Buffers;

	scRet = tab->EncryptMessage(phContext, 0, &Message, 0); //зашифровка
	if(FAILED(scRet)) {
		printf("**** Ошибка 0x%x шифрования EncryptMessage\n", scRet);
		return scRet;
	}
	 
	/*отправка данных в сокет*/
	DWORD cbData = send(Socket, pbIoBuffer, Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer, 0);
	if(cbData == 0) {
		printf("**** Ошибка %d отправки зашифрованного сообщения серверу (3)\n",  WSAGetLastError());
		tab->DeleteSecurityContext(phContext);
		return SEC_E_INTERNAL_ERROR;
	} else 
		printf("Отправлено %d байт зашифрованного сообщения\n", cbData);

	
	DWORD cbIoBuffer = 0; //размер прочитанного сообщения
	while(TRUE) {
		/*чтение из сокета сервера*/
		if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE) {
			cbData = recv(Socket, pbIoBuffer + cbIoBuffer,  cbIoBufferLength - cbIoBuffer,  0);
			if(cbData == 0) {				
				if(cbIoBuffer) {
					printf("**** Сервер неожиданно разорвал соединение\n");
					scRet = SEC_E_INTERNAL_ERROR;
					return scRet;
				} else 
					break;				
			} else {
				printf("Получено %d байт зашифрованного сообщения\n", cbData);
				cbIoBuffer += cbData;
			}
		}

		
		// Попытка расшифрования полученных от сервера данных.
		Buffers[0].pvBuffer   = pbIoBuffer;
		Buffers[0].cbBuffer   = cbIoBuffer;
		Buffers[0].BufferType = SECBUFFER_DATA;

		Buffers[1].BufferType = Buffers[2].BufferType = Buffers[3].BufferType = SECBUFFER_EMPTY;		

		Message.ulVersion = SECBUFFER_VERSION;
		Message.cBuffers  = 4;
		Message.pBuffers  = Buffers;

		scRet = tab->DecryptMessage(phContext, &Message, 0, NULL); //расшифровка
		if(scRet == SEC_E_INCOMPLETE_MESSAGE) 
			/*Входной буфер содержит только фрагмент зашифрованных данных. Продолжение цикла и чтение данных.*/
			continue;
		else if(scRet == SEC_I_CONTEXT_EXPIRED)
			/*Сервер завершил сессию*/
			break;
		else if(scRet != SEC_E_OK &&
		        scRet != SEC_I_RENEGOTIATE &&
		        scRet != SEC_I_CONTEXT_EXPIRED) {
			printf("**** Ошибка 0x%x в процессе расшифровки (DecryptMessage)\n", scRet);
			return scRet;
		}

		// Расположение данных и (необязательно) добавление буферов.		
		SecBuffer * pDataBuffer  = NULL;
		SecBuffer * pExtraBuffer = NULL;
		for(int i = 1; i < 4; i++) {
			if(pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA) {
				pDataBuffer = &Buffers[i];
				printf("Buffers[%d].BufferType = SECBUFFER_DATA\n", i);
			}
			if(pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA) 
				pExtraBuffer = &Buffers[i];
			
		}

		// Вывод или обработка расшифрованных данных.
		if(pDataBuffer) 
			printf("Размер расшифрованных данных: %d байт\n", pDataBuffer->cbBuffer);
		

		// Перенос всех "extra" данных во входной буфер.
		if(pExtraBuffer) {
			MoveMemory(pbIoBuffer, pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);
			cbIoBuffer = pExtraBuffer->cbBuffer;
		} else 
			cbIoBuffer = 0;
		

		if(scRet == SEC_I_RENEGOTIATE) {			
			printf("Сервер запросил обновление ключей!\n");
			SecBuffer ExtraBuffer;
			scRet = ClientHandshakeLoop(tab, Socket, phCreds, phContext, FALSE, &ExtraBuffer, pSchannelCred, phMyCertStore);
			if(scRet != SEC_E_OK) 
				return scRet;
			
			// Перенос всех "extra" данных во входной буфер.
			if(ExtraBuffer.pvBuffer) {
				MoveMemory(pbIoBuffer, ExtraBuffer.pvBuffer, ExtraBuffer.cbBuffer);
				cbIoBuffer = ExtraBuffer.cbBuffer;
			}
		}
	}

	return SEC_E_OK;
}

// Функция, выполняющая разрыв соединения с сервером.
LONG DisconnectFromServer(SecurityFunctionTable* tab, SOCKET Socket, PCredHandle phCreds, CtxtHandle * phContext) {
	// Уведомление schannel о закрытии соединения.
	DWORD dwType = SCHANNEL_SHUTDOWN;

	SecBuffer OutBuffers[1];
	OutBuffers[0].pvBuffer   = &dwType;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer   = sizeof(dwType);

	SecBufferDesc OutBuffer;
	OutBuffer.cBuffers  = 1;
	OutBuffer.pBuffers  = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	DWORD Status = tab->ApplyControlToken(phContext, &OutBuffer);

	if(FAILED(Status)) {
		printf("**** Error 0x%x returned by ApplyControlToken\n", Status);
		return 1;
	}

	
	// Построение SSL сообщения, являющегося уведомлением о закрытии.
	DWORD dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT
			            | ISC_REQ_REPLAY_DETECT 
						| ISC_REQ_CONFIDENTIALITY 
						| ISC_RET_EXTENDED_ERROR 
						| ISC_REQ_ALLOCATE_MEMORY 
						| ISC_REQ_STREAM;

	OutBuffers[0].pvBuffer   = NULL;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer   = 0;

	OutBuffer.cBuffers  = 1;
	OutBuffer.pBuffers  = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	unsigned long dwSSPIOutFlags;
	TimeStamp tsExpiry;
	Status = tab->InitializeSecurityContextA(phCreds,
		                                     phContext,
											 NULL,
											 dwSSPIFlags,
											 0,
											 SECURITY_NATIVE_DREP,
											 NULL,
											 0,
											 phContext,
											 &OutBuffer,
											 &dwSSPIOutFlags,
											 &tsExpiry);

	if(FAILED(Status)) {
		printf("**** Error 0x%x returned by InitializeSecurityContext\n", Status);
		return 1;
	}

	char * pbMessage = OutBuffers[0].pvBuffer;
	DWORD  cbMessage = OutBuffers[0].cbBuffer;
		
	// Посылка этого сообщения серверу.
	if(pbMessage != NULL && cbMessage != 0) {
		DWORD cbData = send(Socket, pbMessage, cbMessage, 0);
		if(cbData == 0) {
			Status = WSAGetLastError();
			printf("**** Error %d sending close notify\n", Status);
			return 1;
		}

		printf("Sending Close Notify\n");
		printf("%d bytes of handshake data sent\n", cbData);


		// Освобождение выходного буфера.
		tab->FreeContextBuffer(pbMessage);
	}

	return Status;
}

int main() {
	/*локализация*/
	setlocale(LC_ALL, "russian");
	
	HCERTSTORE hMyCertStore=NULL;
	SOCKET Socket = INVALID_SOCKET;
	SecurityFunctionTable* tab = NULL;
	CredHandle hCredential;
	CtxtHandle hContext;
	memset(&hContext, 0, sizeof(CtxtHandle));
	PCCERT_CONTEXT pRemoteCertContext = NULL;

	do {
		// Открытие хранилища сертификатов "MY" , в котором Internet Explorer хранит сертификаты клиента.
		hMyCertStore = CertOpenSystemStore(0, "MY");
		if(!hMyCertStore) {
			printf("**** Ошибка открытия хранилища сертификатов 0x%x (CertOpenSystemStore)\n",  GetLastError());
			break;
		}

		// Построение структуры Schannel мандатов. В данном примере определяются используемые протокол и сертификат.
		SCHANNEL_CRED   SchannelCred;
		ZeroMemory(&SchannelCred, sizeof(SchannelCred));

		SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;
		/*PCCERT_CONTEXT  pCertContext = NULL;
		if(pCertContext) {
			SchannelCred.cCreds = 1;
			SchannelCred.paCred = &pCertContext;
		}*/
		DWORD   dwProtocol = 0;
		SchannelCred.grbitEnabledProtocols = dwProtocol;

		/*DWORD           cSupportedAlgs = 0;
		ALG_ID          rgbSupportedAlgs[16];
		if(cSupportedAlgs) {
			SchannelCred.cSupportedAlgs = cSupportedAlgs;
			SchannelCred.palgSupportedAlgs = rgbSupportedAlgs;
		}*/

		SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS
		                        | SCH_CRED_MANUAL_CRED_VALIDATION; // Флаг SCH_CRED_MANUAL_CRED_VALIDATION установлен, поскольку поскольку в данном примере сертификат сервера проверяется "вручную". 

		tab = InitSecurityInterface();

		/*формируем дескриптор удостоверения клиента*/
		SCHANNEL_CRED AuthData;
		memset(&AuthData, 0, sizeof(SCHANNEL_CRED));

		AuthData.dwVersion             = SCHANNEL_CRED_VERSION;
		AuthData.dwFlags               = SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_MANUAL_CRED_VALIDATION; //SCH_CRED_MANUAL_CRED_VALIDATION - это ручная проверка сертификата
		AuthData.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT; /*протокол шифрования*/
			
		
		TimeStamp tsExpiry;
		SECURITY_STATUS rc = tab->AcquireCredentialsHandle( NULL                 /*Не используется, должен быть NULL*/,
														    UNISP_NAME_A         /*Название пакета безопасности. Используйте UNISP_NAME_A*/,
															SECPKG_CRED_OUTBOUND /*Флаг - индикатор использования удостоверения. 
																				 SECPKG_CRED_INBOUND  - для обработки входящих сообщений (сервер) 
																				 SECPKG_CRED_OUTBOUND - для подготовки исходящих сообщений (клиент)*/,
															NULL                 /*Не используется, должен быть NULL.*/,
															&AuthData            /*Данные для создания удостоверения, указатель на структуру SCHANNEL_CRED*/,
															NULL                 /*Не используется, должен быть NULL*/,
															NULL                 /*Не используется, должен быть NULL*/,
															&hCredential         /*дескриптор удостоверения клиента*/,
															&tsExpiry            /*Указатель на структуру TimeStamp, содержащую срок действия удостоверения в локальном времени*/);
		if(rc != SEC_E_OK) {
			perror("Не удалось сформировать дескриптор удостоверения клиента");
			break;
		}

		/*инициализация WSA*/
		WSADATA WsaData;
		if(WSAStartup(0x0101, &WsaData) == SOCKET_ERROR) {
			printf("Ошибка инициализации среды WSA %d (WSAStartup)\n", GetLastError());
			break;
		}

		/*подключаемся*/
		Socket = socket(PF_INET, SOCK_STREAM, 0);
		if(Socket == INVALID_SOCKET) {
			printf("**** Ошибка создания сокета %d (socket)\n", WSAGetLastError());
			break;
		}

		int port = 443;
		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons((u_short)port);

		char ServerName[] = "google.ru";
		struct hostent *hp;
		if((hp = gethostbyname(ServerName)) == NULL) { /*получаем адрес по имени*/
			printf("**** Ошибка %d получения адреса по имени из DNS (gethostbyname)\n", WSAGetLastError());
			break;
		} else {
			memcpy(&sin.sin_addr, hp->h_addr, 4);
		}


		if(0!=connect(Socket, (struct sockaddr *)&sin, sizeof(sin))) {
			printf("**** Ошибка %d установки соединения с \"%s\" (%s)\n", WSAGetLastError(), ServerName,  inet_ntoa(sin.sin_addr));
			closesocket(Socket);
			break;
		}		

		/*установка шифрованного соединения*/
		DWORD Flags = ISC_REQ_SEQUENCE_DETECT 
			          | ISC_REQ_REPLAY_DETECT 
			          | ISC_REQ_CONFIDENTIALITY 
					  | ISC_RET_EXTENDED_ERROR
			          | ISC_REQ_ALLOCATE_MEMORY  //SSP отводит память для буферов. Ее необходимо освободить вызовом FreeContextBuffer
			          | ISC_REQ_STREAM ;
		

		//инициализация сообщения, для генерации токена
		SecBuffer OutBuffers[1];
		OutBuffers[0].pvBuffer   = NULL;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer   = 0;

		SecBufferDesc OutBuffer;
		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		ULONG SSPIOutFlags= 0;
		rc = tab->InitializeSecurityContextA(&hCredential            /*дескриптор удостоверения клиента*/,
									        NULL                     /*При первом вызове, укажите NULL. При последующих - указатель на дескриптор, возвращенный в phNewContext после первого вызова к этой функции.*/,
									        ServerName               /*уникальный идентификатор сервера (используется при поиске сессии в кеше сессий при восстановлении соединения)*/,
									        Flags                    /*набор флагов*/,												 
										    0                        /*Зарезервирован. Должен быть 0*/,
											SECURITY_NATIVE_DREP     /*Зарезервирован. Должен быть 0*/,
												 
										    NULL                     /*указатель на структуру SecBufferDesc. 
									   		 						 При первом вызове должен быть NULL. 
																	 При последующих вызовах должен состоять из двух структур SecBuffer. 
																	 Первый буфер должен иметь тип SECBUFFER_TOKEN, и содержать пакет, полученый от сервера. 
																	 Второй буфер должен иметь тип SECBUFFER_EMPTY. В него в случае необходимости будут помещены оставшиеся необработанные входные данные.*/,
												 
										    0                        /*Зарезервирован. Должен быть 0*/,
												 
										    &hContext                /*Указатель на CtxtHandle.
																	 При первом вызове сюда будет помещен дескриптор нового контекста. 
																	 При последующих вызовах полученный дескриптор следует передавать через параметр phContext, а в phNewContext передавать NULL*/,
												 
										    &OutBuffer               /*Указатель на структуру SecBufferDesc, содержащую SecBuffer с типом SECBUFFER_TOKEN.
																	 На выходе в этот буфер будет помещен пакет, который нужно переслать серверу. 
																	 При наличии флага ISC_REQ_ALLOCATE_MEMORY этот буфер будет отведен.*/,
												 
										    &SSPIOutFlags            /*Указатель на ULONG, куда будут записаны флаги, описывающие свойства устанавливаемого контекста.
																	 Список возможных значений приведен в описании параметра fContextReq. 
																	 pfContextAttr получает тот же набор флагов, с заменой префикса ISC_REQ на ISC_RET. 
																	 (Аттрибуты безопасности не следует проверять до последнего вызова функции, возвращающего SEC_E_OK. 
																	 Остальные атрибуты, такие как ISC_RET_ALLOCATED_MEMORY можно использовать после первого же вызова)*/,

										    &tsExpiry                /*Опционален. Указатель на структуру TimeStamp. 
																	 При наличии сертификата клиента, этот параметр получает время истечения этого сертификата. 
																	 Иначе возвращается максимальное возможное значение времени. Врямя возвращается в UTC*/);
		if(rc != SEC_I_CONTINUE_NEEDED) {
			perror("Не удалось инициализировать контент безопасности");
			break;
		}

		//отправляем ответ серверу
		if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL) {
			DWORD cbData = send(Socket, OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer, 0);
			if(cbData <= 0) {
				printf("**** Ошибка отправки данных серверу %d (1)\n", WSAGetLastError());
				tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
				tab->DeleteSecurityContext(&hContext);
				break;
			}

			printf("Отправлено %d байт в рамках установки соединения\n", cbData);

			/*удаление выходного буфера*/
			tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
			OutBuffers[0].pvBuffer = NULL;
		}

		//обмен сообщениями с сервером (процедуры установки соединения)
		SecBuffer  ExtraData;
		if(SEC_E_OK != ClientHandshakeLoop(tab, Socket, &hCredential, &hContext, TRUE, &ExtraData, &SchannelCred, &hMyCertStore))
			break;

		// Получение сертификата сервера.
		SECURITY_STATUS Status = tab->QueryContextAttributes(&hContext,
															 SECPKG_ATTR_REMOTE_CERT_CONTEXT,
															 (PVOID)&pRemoteCertContext);
		if(Status != SEC_E_OK) {
			printf("Ошибка получения сертификата сервера 0x%x \n", Status);
			break;
		}

		// Проверка действительности сертификата сервера.
		Status = VerifyServerCertificate(pRemoteCertContext,ServerName, 0);
		if(Status) {
			/*Сертификат сервера не действителен. Возможно было осуществлено
			 соединение с недопустимым сервером, либо производилась атака "противник посередине".
			 Лучше всего прервать соединение.*/
			printf("**** Ошибка 0x%x проверки сертификата!\n", Status);
			break;
		} else {
			// Освобождение контекста сертификата сервера.
			CertFreeCertificateContext(pRemoteCertContext);
			pRemoteCertContext = NULL;
			printf("Сертификат прошел проверку.\n");
		}
		
		// Вывод информации о соединении. 
		DisplayConnectionInfo(tab, &hContext);


		/*получение файла с сервера*/
		LPSTR   pszFileName = "Default.Htm";
		if(HttpsGetFile(tab, Socket, &hCredential, &hContext, pszFileName, &SchannelCred, &hMyCertStore)) {
			printf("Ошибка получения файла с сервера\n");
			break;
		}

		/*уведобмляем сервер о разрыве соединения*/
		DisconnectFromServer(tab, Socket, &hCredential, &hContext);		
	} while(FALSE);

	/*Освобождение контекста сертификата сервера*/
	if(pRemoteCertContext)
		CertFreeCertificateContext(pRemoteCertContext);

		
	if(tab != NULL) {
		/*удаление инициализированного контента*/
		tab->DeleteSecurityContext(&hContext);

		/*удаление дескриптора SSPI клиента*/
		tab->FreeCredentialsHandle(&hCredential);
	}

	/*Закрытие сокета*/
	if(Socket != INVALID_SOCKET) {
		shutdown(Socket, SD_BOTH);
		closesocket(Socket);
	}

	/*Завершение работы подсистемы WinSock*/
	WSACleanup();


	// Закрытие хранилища сертификатов "MY".
	if(hMyCertStore) {
		CertCloseStore(hMyCertStore, 0);
	}

	system("pause");
	return 0;
}