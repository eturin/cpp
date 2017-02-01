#include "crypt.h"
#include "error.h"
#include "client.h"
#include "server.h"
#include "worker.h"
#include "req.h"

SecurityFunctionTable* tab = NULL;

/* ������� �������� �������� �� ������ ����������� �� �����
1) ������ ���������� ����� � ������ ��������� (����� ���� ������)
makecert - n "CN=ture my" - e 01 / 01 / 5555 - pe - ss my
2) ������������ ���������� ������ � �������� ������ � ���� ��� ������� ��������� (������� ��� ������� � ����������)
3) ������� ���������� �� ��������� (�� �����������)
*/
HRESULT CreateCredentialsFromExportFile(char * path, PCredHandle phCreds) {
	/*��������� ���� �����������*/
	HANDLE hFile = CreateFile(path,                   /*���� � �����*/
							  GENERIC_READ,           /*������������ ������*/
							  FILE_SHARE_READ,        /*��������� ��������� ����� ������ ���� ����*/
							  NULL,                   /*����� ������� �� ���������*/
							  OPEN_EXISTING,          /*�������, ���� ���� ����������� �� �����*/
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		show_err("(master)�� ������� ������� ���� � �������",TRUE);
		return SEC_E_NO_CREDENTIALS;
	}

	/*��������� ���� � ������*/
	DWORD size = 0, load = 0;
	size = GetFileSize(hFile, NULL);
	if(size == INVALID_FILE_SIZE || size == 0) {
		show_err("(master)�� ������� ���������� ������ ����� � �������",TRUE);
		CloseHandle(hFile);
		return SEC_E_NO_CREDENTIALS;
	}

	CRYPT_DATA_BLOB data;
	data.cbData = size;
	data.pbData = malloc(size);
	if(!ReadFile(hFile, data.pbData, size, &load, NULL)) {
		show_err("(master)�� ������� ��������� ����� � �������",TRUE);
		CloseHandle(hFile);
		free(data.pbData);
		return SEC_E_NO_CREDENTIALS;
	}
	CloseHandle(hFile);

	/*����������� �� ����������� �����*/
	HCERTSTORE hCertStore = PFXImportCertStore(&data, /*NULL*/ L"123", 0);
	if(hCertStore == NULL) {
		show_err("(master)�� ������� ������������� �������� � �������� �����",TRUE);
		free(data.pbData);
		return SEC_E_NO_CREDENTIALS;
	}

	/*�������� ��������� ����������� �� ������ �����*/
	PCCERT_CONTEXT pCertContext = CertFindCertificateInStore(hCertStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_ANY, NULL, NULL);
	if(pCertContext == NULL) {
		show_err("(master)�� ������� �������� ��������� ����������� �� ����������� �����",TRUE);
		free(data.pbData);
		return SEC_E_NO_CREDENTIALS;
	}

	/*���������� ��������� Schannel (������������ ������������ �������� � ����������)*/
	SCHANNEL_CRED   SchannelCred;
	memset(&SchannelCred, 0, sizeof(SCHANNEL_CRED));
	SchannelCred.dwVersion             = SCHANNEL_CRED_VERSION;
	SchannelCred.cCreds                = 1;
	SchannelCred.paCred                = &pCertContext;
	//SchannelCred.grbitEnabledProtocols = 0;

	/*�������� SSPI �������*/
	TimeStamp tsExpiry;
	SECURITY_STATUS Status = tab->AcquireCredentialsHandle(NULL,                   // ��� ��������������
														   UNISP_NAME_A,           // ��� ������
														   SECPKG_CRED_INBOUND,    // ����, ������������ �������������
														   NULL,                   // ��������� �� ������������� ������
														   &SchannelCred,          // ������ ������
														   NULL,                   // ��������� �� ������� GetKey()
														   NULL,                   // ��������, ������������ ������� GetKey()
														   phCreds,                // (out) ���������� �������
														   &tsExpiry);             // (out) ������ ������������ (�������������)
	/*������������ ��������� ����������� (� Schannel ��� ������� ��� �����)*/
	CertFreeCertificateContext(pCertContext);
	free(data.pbData);

	switch(Status) {
		case SEC_E_INSUFFICIENT_MEMORY:
			show_err("(master)������ SEC_E_INSUFFICIENT_MEMORY ��� �������� �������", FALSE);
			break;
		case SEC_E_INTERNAL_ERROR:
			show_err("(master)������ SEC_E_INTERNAL_ERROR ��� �������� �������", FALSE);
			break;
		case SEC_E_NO_CREDENTIALS:
			show_err("(master)������ SEC_E_NO_CREDENTIALS ��� �������� �������", FALSE);
			break;
		case SEC_E_NOT_OWNER:
			show_err("(master)������ SEC_E_NOT_OWNER ��� �������� �������", FALSE);
			break;
		case SEC_E_SECPKG_NOT_FOUND:
			show_err("(master)������ SEC_E_SECPKG_NOT_FOUND ��� �������� �������", FALSE);
			break;
		case SEC_E_UNKNOWN_CREDENTIALS:
			show_err("(master)������ SEC_E_UNKNOWN_CREDENTIALS ��� �������� �������", FALSE);
			break;
		default:
			break;
	}

	return Status;
}

/* ������� �������� �������� �� ������ ����������� ����������� �� pszUserName � �������������� � ��������� my.
makecert -n "CN=ture my" -pe -ss my
*/
HRESULT CreateCredentialsFromSS(char * name, char * path, PCredHandle phCreds) {
	/*��������� ������ ��������� ������������, ���� �� ������� ����*/
	if(strlen(path) == 0)
		strcat(path,"MY");
	
	HCERTSTORE hMyCertStore = CertOpenSystemStore(0, path);
	if(!hMyCertStore) {
		show_err("(master)�� ������� ������� ������ ��������� ������������", TRUE);
		return SEC_E_NO_CREDENTIALS;
	}

	/*����� �����������. � ������ ������� �������������� ����� ����������� �� subject name, ���������� �������� � ��������� ������ ��� ������������.*/
	PCCERT_CONTEXT  pCertContext = CertFindCertificateInStore(hMyCertStore,
															  X509_ASN_ENCODING,
															  0,
															  CERT_FIND_SUBJECT_STR_A,
															  name,
															  NULL);
	CertCloseStore(hMyCertStore, 0);
	if(pCertContext == NULL) {
		show_err("(master)������ ���������� �� ������ � ������ ���������",TRUE);
		return SEC_E_NO_CREDENTIALS;
	}


	/*���������� ��������� Schannel (������������ ������������ �������� � ����������)*/
	SCHANNEL_CRED  SchannelCred;
	memset(&SchannelCred, 0, sizeof(SCHANNEL_CRED));
	SchannelCred.dwVersion             = SCHANNEL_CRED_VERSION;
	SchannelCred.cCreds                = 1;
	SchannelCred.paCred                = &pCertContext;
	//SchannelCred.grbitEnabledProtocols = 0;

	// �������� SSPI �������.
	TimeStamp       tsExpiry;
	SECURITY_STATUS Status = tab->AcquireCredentialsHandle(NULL,                   // ��� ��������������
														   UNISP_NAME_A,           // ��� ������
														   SECPKG_CRED_INBOUND,    // ����, ������������ �������������
														   NULL,                   // ��������� �� ������������� ������
														   &SchannelCred,          // ������ ������
														   NULL,                   // ��������� �� ������� GetKey()
														   NULL,                   // ��������, ������������ ������� GetKey()
														   phCreds,                // (out) ���������� �������
														   &tsExpiry);             // (out) ������ ������������ (�������������)
	/*������������ ��������� ����������� (� Schannel ��� ������� ��� �����)*/
	CertFreeCertificateContext(pCertContext);

	switch(Status) {
		case SEC_E_INSUFFICIENT_MEMORY:
			show_err("(master)������ SEC_E_INSUFFICIENT_MEMORY ��� �������� �������", FALSE);
			break;
		case SEC_E_INTERNAL_ERROR:
			show_err("(master)������ SEC_E_INTERNAL_ERROR ��� �������� �������"     , FALSE);
			break;
		case SEC_E_NO_CREDENTIALS:
			show_err("(master)������ SEC_E_NO_CREDENTIALS ��� �������� �������"     , FALSE);
			break;
		case SEC_E_NOT_OWNER:
			show_err("(master)������ SEC_E_NOT_OWNER ��� �������� �������"          , FALSE);
			break;
		case SEC_E_SECPKG_NOT_FOUND:
			show_err("(master)������ SEC_E_SECPKG_NOT_FOUND ��� �������� �������"   , FALSE);
			break;
		case SEC_E_UNKNOWN_CREDENTIALS:
			show_err("(master)������ SEC_E_UNKNOWN_CREDENTIALS ��� �������� �������", FALSE);
			break;		
		default:
			break;
	}
		
	return Status;
}

HRESULT CreateCredentials(char * cert_name, char * cert_path, PCredHandle phCreds) {
	HRESULT ret = SEC_E_NO_CREDENTIALS;
	BOOL isName = strlen(cert_name);
	BOOL isPath = strlen(cert_path);
	
	/*������������� */
	if(tab==NULL)
		tab = InitSecurityInterface();

	if(!isName) {		
		/*����� ������������ ��� ����������, ���� ��� �� ��������*/
		gethostname(cert_name, 256);

		if(isPath) {
			ret = CreateCredentialsFromExportFile(cert_path, phCreds);
			if(ret == SEC_E_NO_CREDENTIALS || ret) {
				show_err("(master)� �������� ����� ��� ����������� ����� ������������ ��� ����������", FALSE);
				ret = CreateCredentialsFromSS(cert_name, cert_path, phCreds);
			}
		} else {
			show_err("(master)� �������� ����� ��� ����������� ����� ������������ ��� ����������", FALSE);
			ret = CreateCredentialsFromSS(cert_name, cert_path, phCreds);
			if(ret == SEC_E_NO_CREDENTIALS || ret) {
				sprintf(cert_path, "%s.pfx", cert_name);
				ret = CreateCredentialsFromExportFile(cert_path, phCreds);
			}
		}		
	} else {
		ret = CreateCredentialsFromSS(cert_name, cert_path, phCreds);
		if(ret == SEC_E_NO_CREDENTIALS || ret)
			if(isPath)
				ret = CreateCredentialsFromExportFile(cert_path, phCreds); 
			else {
				sprintf(cert_path, "%s.pfx", cert_name);
				ret = CreateCredentialsFromExportFile(cert_path, phCreds);
			}				
	}
	
	if(ret != SEC_E_OK)
		ret = SEC_E_NO_CREDENTIALS;

	return ret;
}

void crypt_clear(void * ext) {
	//if(ext != NULL) {
	//	/*������� ����������������� ���������*/
	//	((struct Crypt *)ext)->is_Crypted = FALSE;
	//}
}

int crypt_disconect(struct Client * pcln){
	int ret = 0;
			
	if(pcln->ext == NULL) 
		return 0;	

	/*������������� �������������� ���������*/
	struct Crypt * ext = (struct Crypt *)pcln->ext;
	if(!ext->is_Connected || ext->is_Clousing)
		return 0;

	/*�������� schannel � �������� ����������*/
	printf("[%x] ��������� � ��������\n", pcln);
	ext->is_Clousing = TRUE;

	DWORD dwType = SCHANNEL_SHUTDOWN;
	SecBuffer OutBuffers[1];
	OutBuffers[0].pvBuffer = &dwType;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = sizeof(dwType);

	SecBufferDesc   OutBuffer;
	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;
	/*����������*/
	DWORD scRet = tab->ApplyControlToken(&ext->hContext, &OutBuffer);
	if(FAILED(scRet)) {
		printf("[%x] ", pcln);
		show_err_HRESULT("(master)�� ������� ��������� ����� � �������� ����������", scRet);
		release_Client(pcln);
		return 0;
	}

	/*���������� SSL ���������, ����������� ������������ � ��������.*/
	OutBuffers[0].pvBuffer = NULL;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = 0;

	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	DWORD dwSSPIFlags = ASC_REQ_SEQUENCE_DETECT
						| ASC_REQ_REPLAY_DETECT
						| ASC_REQ_CONFIDENTIALITY
						| ASC_REQ_EXTENDED_ERROR
						| ASC_REQ_ALLOCATE_MEMORY //����������� ����� FreeContextBuffer � ������ ������
						| ASC_REQ_STREAM;
	unsigned long   dwSSPIOutFlags;
	TimeStamp       tsExpiry;
	scRet = tab->AcceptSecurityContext(&pcln->psrv->hServerCreds,
									   &ext->hContext,
									   NULL,
									   dwSSPIFlags,
									   SECURITY_NATIVE_DREP,
									   NULL,
									   &OutBuffer,
									   &dwSSPIOutFlags,
									   &tsExpiry);
	if(FAILED(scRet)) {
		printf("[%x] ", pcln);
		show_err_HRESULT("(master)�� ������� ��������� ����� � �������� ����������", scRet);
		release_Client(pcln);
		return 0;
	}

	// ������� ����� ��������� �������.	
	clear_Client(pcln);
	pcln->len = OutBuffers[0].cbBuffer;
	pcln->cur = 0;
	memcpy(pcln->data, OutBuffers[0].pvBuffer, pcln->len);
	tab->FreeContextBuffer(OutBuffers[0].pvBuffer);

	pcln->overlapped_inf.type = WRITE;
	start_async(pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
	/*������ ����������� �������� ��������*/
	return 1;
}

int crypt_check(struct Client *pcln) {
	int ret = 0;

	struct Crypt * ext;
	/*������������� �������������� ���������*/
	if(pcln->ext == NULL) {
		ext = malloc(sizeof(struct Crypt));
		memset(ext, 0, sizeof(struct Crypt));
		pcln->ext = (void*)ext;
		ext->is_InitContext = TRUE;
	} else
		ext = (struct Crypt *)pcln->ext;
		
	if((pcln->overlapped_inf.type == READ || pcln->overlapped_inf.type == READ_TO_SEND_WORKER)
	   && pcln->len == 0) {
		/*������������� �������� �� �������������� ������, ������� ������� �� ����������� ������� ������� � �������*/
		if(ext->IO_Buffer_Size) {
			free(pcln->data);
			pcln->data=ext->IO_Buffer;
			pcln->size = pcln->len = pcln->cur = ext->IO_Buffer_Size;
			
			pcln->DataBuf.buf = pcln->data+pcln->len;
			
			ext->IO_Buffer = NULL;
			ext->IO_Buffer_Size = 0;
		} else if(!ext->is_Connected)
			/*��� ������ ������������*/
			return 0;
	}

	if(pcln->overlapped_inf.type == READ && !ext->is_Connected) {
		/*��������� ��������� �������������� ���������� (handshake)*/
		SecBuffer InBuffers[2];
		/*� ���� ����� �������� ������������� ������*/
		InBuffers[0].pvBuffer = pcln->data;
		InBuffers[0].cbBuffer = pcln->len;
		InBuffers[0].BufferType = SECBUFFER_TOKEN;
		/*���� ����� �������� ���������������� �������� ������, ������� ����� ����� ���������� � ��������� ���*/
		InBuffers[1].pvBuffer = NULL;
		InBuffers[1].cbBuffer = 0;
		InBuffers[1].BufferType = SECBUFFER_EMPTY;

		SecBufferDesc InBuffer;
		InBuffer.cBuffers = 2;
		InBuffer.pBuffers = InBuffers;
		InBuffer.ulVersion = SECBUFFER_VERSION;


		SecBuffer OutBuffers[1];
		/*���� ����� ����� ��������� �������� ��������� �������, ������� ����� ����� ��������� �������*/
		OutBuffers[0].pvBuffer = NULL;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = 0;

		SecBufferDesc OutBuffer;
		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;


		TimeStamp tsExpiry;
		DWORD dwSSPIFlags = ASC_REQ_SEQUENCE_DETECT
							| ASC_REQ_REPLAY_DETECT
							| ASC_REQ_CONFIDENTIALITY
							| ASC_REQ_EXTENDED_ERROR
							| ASC_REQ_ALLOCATE_MEMORY
							| ASC_REQ_STREAM;
		unsigned long dwSSPIOutFlags;
		SECURITY_STATUS scRet = SEC_I_CONTINUE_NEEDED;
		scRet = tab->AcceptSecurityContext(&pcln->psrv->hServerCreds,
										   ext->is_InitContext ? NULL : &ext->hContext,
										   &InBuffer,
										   dwSSPIFlags,
										   SECURITY_NATIVE_DREP,
										   ext->is_InitContext ? &ext->hContext : NULL,
										   &OutBuffer,
										   &dwSSPIOutFlags,
										   &tsExpiry);
		if(scRet != SEC_E_INCOMPLETE_MESSAGE)
			ext->is_InitContext = FALSE;
		else if(ext->is_InitContext) 
			tab->DeleteSecurityContext(&ext->hContext); //������� �� ���� �������� ������ ������� ��������� �� �������			
		

		/*���� ���� �������������� ��������, �� �������� ��� �� �����*/
		if((scRet == SEC_E_OK || scRet != SEC_E_INCOMPLETE_MESSAGE && scRet != SEC_I_INCOMPLETE_CREDENTIALS)
		   && InBuffers[1].BufferType == SECBUFFER_EXTRA) {
			ext->IO_Buffer_Size = InBuffers[1].cbBuffer;
			ext->IO_Buffer      = malloc(ext->IO_Buffer_Size);
			memcpy(ext->IO_Buffer, pcln->data + (pcln->len - ext->IO_Buffer_Size), ext->IO_Buffer_Size);						 
		} 

		printf("[%x] %d <--��������\n", pcln, pcln->len);

		if(scRet == SEC_E_OK) {
			/*��������� ��������� ���������� ���������*/
			ext->is_Connected = TRUE;
			printf("[%x] �������� ���������� �����������\n", pcln);
			/*���������� ������� ���������*/
			scRet = tab->QueryContextAttributes(&ext->hContext, SECPKG_ATTR_STREAM_SIZES, &ext->head_Sizes);
			if(scRet != SEC_E_OK) {
				printf("[%x] ", pcln);
				show_err_HRESULT("(master)�� ������� ��������� ������ ��������� �� ������ �������� ������������ �������", scRet);
				release_Client(pcln);
				return 1;
			}
		}

		

		if(scRet == SEC_E_OK
		   || scRet == SEC_I_CONTINUE_NEEDED
		   || FAILED(scRet) && 0 != (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR)
		   ) {			
			if(OutBuffers[0].cbBuffer != 0 &&
			   OutBuffers[0].pvBuffer != NULL) {
				
				/*��������� ����� �������*/
				free(pcln->data);
				pcln->cur = 0;
				pcln->size = pcln->len = OutBuffers[0].cbBuffer;
				pcln->data = malloc(pcln->size);
				pcln->DataBuf.len = LEN;
				pcln->DataBuf.buf = pcln->data;
				memcpy(pcln->data, OutBuffers[0].pvBuffer, pcln->len);
				ext->is_Crypted = TRUE;
				pcln->overlapped_inf.type = WRITE;
				/*��������� ����������� ��������*/
				DWORD len = 0;
				printf("[%x] ����������--> %d\n", pcln, pcln->len);
				start_async(pcln, len, pcln->psrv->iocp, &pcln->overlapped_inf);
				tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
				/*�� ����� ���������� ������ ������*/
				return 1;
			}
		}

		if(scRet == SEC_E_OK) {				
			pcln->len = pcln->cur = 0;
			pcln->DataBuf.buf = pcln->data;
			if(ext->IO_Buffer_Size) {
				/*�������� �����������*/
				printf("[%x] ���� ������ ������--> %d\n", pcln, ext->IO_Buffer_Size);				
				return crypt_check(pcln);
			} else {
				/*�� ���������� ������, �� ��������� �������� �� �����*/
				pcln->cur = 0;
				pcln->DataBuf.buf = pcln->data;
				return crypt_check(pcln);
			}
		} else if(FAILED(scRet) 
				  && scRet != SEC_E_INCOMPLETE_MESSAGE /*��������� �������� �� �������*/
				  && scRet != SEC_E_ILLEGAL_MESSAGE    /*������ ��������� �������� (������ ��� ��� �� ��� ���������)*/) {
			printf("[%x] len=%d ", pcln, pcln->len);
			show_err_HRESULT("(master)�� ������� ������� �������� ������������ �� �������", scRet);
			return 1;
		}
		/*���������� ������ ��������� �� �������*/
		return 0;
	} else if(pcln->overlapped_inf.type == READ
			  || pcln->overlapped_inf.type == READ_TO_SEND_WORKER) {
		printf("[%x] ������� (������) %d\n", pcln, pcln->len);
		/*��������� ������ ��������� ����� ������������� ����������*/
		SecBuffer Buffers[4];
		/*���� ����� �������� ������������� ������*/
		Buffers[0].pvBuffer   = pcln->data;
		Buffers[0].cbBuffer   = pcln->len;			
		Buffers[0].BufferType = SECBUFFER_DATA;
		/*����� �� �������������� ����� ��������� � ������ ������*/
		Buffers[1].BufferType = SECBUFFER_EMPTY;		
		Buffers[2].BufferType = SECBUFFER_EMPTY;		
		Buffers[3].BufferType = SECBUFFER_EMPTY;
		/*���������������� ���������*/
		SecBufferDesc Message;
		Message.ulVersion = SECBUFFER_VERSION;
		Message.cBuffers  = 4;
		Message.pBuffers  = Buffers;
		/*�����������*/
		SECURITY_STATUS scRet = tab->DecryptMessage(&ext->hContext, &Message, 0, NULL);
		            
		if(scRet == SEC_E_INCOMPLETE_MESSAGE)
			/*���� ��������� �� ������, �� ���������� ����������� ������*/
			return 0;		
		else if(scRet == SEC_I_CONTEXT_EXPIRED) {
			/*������ ������ ����������*/
			release_Client(pcln);
			return 1;
		} else if(scRet != SEC_E_OK) {
			/*�� ������� ������������ ��������� �������*/
			printf("[%x] ������=%d", pcln, pcln->len);
			show_err_HRESULT("(master)�� ������� ������������ ��������� �������", scRet);
			release_Client(pcln);
			return 1;
		}

		/*�������� ������ � �������*/
		BOOL is_ok = FALSE;
		for(int i = 1; i < 4; ++i)
			if(Buffers[i].BufferType == SECBUFFER_DATA) {
				/*��� ������ ������������*/
				pcln->cur = pcln->len = Buffers[i].cbBuffer;
				memcpy(pcln->data, Buffers[i].pvBuffer, pcln->len);
				is_ok = TRUE;				
			} else if(Buffers[i].BufferType == SECBUFFER_EXTRA) {
				/*��� ������ ����� ������������ �� ��������� ����*/
				ext->IO_Buffer_Size = Buffers[i].cbBuffer;
				ext->IO_Buffer = malloc(ext->IO_Buffer_Size);
				memcpy(ext->IO_Buffer, Buffers[i].pvBuffer, ext->IO_Buffer_Size);				
			}

		/*�������� ����� ���������*/
		if(is_ok) {
			if(pcln->overlapped_inf.type == READ) {
				char * begin = strstr(pcln->data, "\r\n\r\n");
				if(pcln->len >= pcln->size || begin != NULL) {
					/*��������� ��������, ������ worker � ����������� ��������*/
					ext->is_Crypted = FALSE;
					work(pcln, pcln->psrv->iocp); //����������� �����								
					return 1;
				}
				/*���������� ������ �� ������*/ 
				return 0;
			} else {
				pcln->preq->cur += pcln->len;
				return 1;
			}
		} else if(pcln->overlapped_inf.type == READ_TO_SEND_WORKER)
			return 0;

		show_err("(master)�� ������� ����� ����� � �������������� ���������� �������", FALSE);
		release_Client(pcln);
		return 1;
	} else if(pcln->overlapped_inf.type == WRITE) {		
		if(!ext->is_Crypted) {
			/*��������� ������, ������� ��������� ���������, � �����*/
			free(ext->IO_Buffer);
			ext->IO_Buffer_Size = pcln->len;
			ext->IO_Buffer      = pcln->data;
			pcln->data = NULL;
			pcln->size = pcln->cur = pcln->len = 0;
			ext->IO_Buffer_cur  = 0;
			ext->is_Response    = TRUE;
		} else if(ext->is_Response && ext->IO_Buffer_Size <= ext->IO_Buffer_cur) {
			/*��� ���������� (������������� � ��������� ������)*/
			free(ext->IO_Buffer);
			ext->IO_Buffer      = NULL;
			ext->IO_Buffer_Size = ext->IO_Buffer_cur = 0;
			ext->is_Response = FALSE;

			//release_Client(pcln);
			clear_Client(pcln);			
			
			//pcln->overlapped_inf.type = WRITE;

			
			//ext->is_Connected   = FALSE;			
			
			///*�������� schannel � �������� ����������*/
			//printf("[%x] ��������� � ��������\n", pcln);
			//ext->is_Clousing    = TRUE;
			//DWORD dwType = SCHANNEL_SHUTDOWN; 
			//SecBuffer OutBuffers[1];
			//OutBuffers[0].pvBuffer   = &dwType;
			//OutBuffers[0].BufferType = SECBUFFER_TOKEN;
			//OutBuffers[0].cbBuffer   = sizeof(dwType);

			//SecBufferDesc   OutBuffer;
			//OutBuffer.cBuffers  = 1;
			//OutBuffer.pBuffers  = OutBuffers;
			//OutBuffer.ulVersion = SECBUFFER_VERSION;
			///*����������*/
			//DWORD scRet = tab->ApplyControlToken(&ext->hContext, &OutBuffer);
			//if(FAILED(scRet)) {
			//	printf("[%x] ", pcln);
			//	show_err_HRESULT("(master)�� ������� ��������� ����� � �������� ����������", scRet);
			//	release_Client(pcln);
			//	return 1;				
			//}

			///*���������� SSL ���������, ����������� ������������ � ��������.*/
			//OutBuffers[0].pvBuffer   = NULL;
			//OutBuffers[0].BufferType = SECBUFFER_TOKEN;
			//OutBuffers[0].cbBuffer   = 0;

			//OutBuffer.cBuffers  = 1;
			//OutBuffer.pBuffers  = OutBuffers;
			//OutBuffer.ulVersion = SECBUFFER_VERSION;
			//
			//DWORD dwSSPIFlags = ASC_REQ_SEQUENCE_DETECT
			//					| ASC_REQ_REPLAY_DETECT
			//					| ASC_REQ_CONFIDENTIALITY
			//					| ASC_REQ_EXTENDED_ERROR
			//					| ASC_REQ_ALLOCATE_MEMORY //����������� ����� FreeContextBuffer � ������ ������
			//					| ASC_REQ_STREAM;
			//unsigned long   dwSSPIOutFlags;
			//TimeStamp       tsExpiry;
			//scRet = tab->AcceptSecurityContext(&pcln->psrv->hServerCreds,
			//									&ext->hContext,
			//									NULL,
			//									dwSSPIFlags,
			//									SECURITY_NATIVE_DREP,
			//									NULL,
			//									&OutBuffer,
			//									&dwSSPIOutFlags,
			//									&tsExpiry);
			//if(FAILED(scRet)) {
			//	printf("[%x] ", pcln);
			//	show_err_HRESULT("(master)�� ������� ��������� ����� � �������� ����������", scRet);
			//	release_Client(pcln);
			//	return 1;
			//}

			//// ������� ����� ��������� �������.	
			//pcln->len = OutBuffers[0].cbBuffer;
			//memcpy(pcln->data, OutBuffers[0].pvBuffer, pcln->len);
			//tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
			
			/*��������� ����������� �������� ������ � �����*/
			return 1;
		} else if(ext->is_Clousing && pcln->cur >= pcln->len) {
			//��� ���������� � ������ �������� �������� ����������
			printf("[%x] ���������� ���������\n", pcln);
			release_Client(pcln);
			/*��������� ����������� �������� ������ �� ������*/
			return 1;
		} else if(!ext->is_Response && pcln->cur >= pcln->len) {
			//��� ���������� � ������ �������� ��������� ���������� 
			clear_Client(pcln);
			/*��������� ����������� �������� ������ �� ������*/
			return 0;
		}

		/*��������� ����������� ��������*/
		if(ext->is_Response && pcln->cur >= pcln->len) {
			if(pcln->cur == 0) {
				/*�������������� �������� �������� ������ ���������*/
				char *b=strstr(ext->IO_Buffer, "\r\n\r\n");
				if(b != NULL)
					pcln->size = b+4 - ext->IO_Buffer;
			} 
			if(pcln->cur!=0 || pcln->size == 0) {
				/*�������� �������� ������������ ������ � ������ ���������, ������ � ��� ������������� ���������*/
				pcln->size = ext->head_Sizes.cbHeader + ext->IO_Buffer_Size + ext->head_Sizes.cbTrailer - ext->IO_Buffer_cur;
				pcln->size = pcln->size > LEN ? LEN : pcln->size;				
			}			
			pcln->len = pcln->size - ext->head_Sizes.cbHeader - ext->head_Sizes.cbTrailer;
			char * data = malloc(pcln->size);
			memset(data, 0, pcln->size);
			memcpy(data + ext->head_Sizes.cbHeader, ext->IO_Buffer + ext->IO_Buffer_cur, pcln->len);
		
			/*�������� ������ ������������� ���������*/
			ext->IO_Buffer_cur += pcln->len;
				
			free(pcln->data);
			pcln->data = data;				
									
			SecBuffer Buffers[4];
			/*����� ���������*/
			Buffers[0].pvBuffer   = pcln->data;
			Buffers[0].cbBuffer   = ext->head_Sizes.cbHeader;
			Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;
			/*����� ������*/
			Buffers[1].pvBuffer   = pcln->data + ext->head_Sizes.cbHeader;
			Buffers[1].cbBuffer   = pcln->len;
			Buffers[1].BufferType = SECBUFFER_DATA;
			/*��������� �����*/
			Buffers[2].pvBuffer   = pcln->data + ext->head_Sizes.cbHeader + pcln->len;
			Buffers[2].cbBuffer   = ext->head_Sizes.cbTrailer;
			Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;
			/*������ �����*/
			Buffers[3].BufferType = SECBUFFER_EMPTY;
			/*������������� ���������*/
			SecBufferDesc Message;
			Message.ulVersion = SECBUFFER_VERSION;
			Message.cBuffers  = 4;
			Message.pBuffers  = Buffers;
			/*����������*/
			SECURITY_STATUS scRet = tab->EncryptMessage(&ext->hContext, 0, &Message, 0);
			if(FAILED(scRet)) {
				printf("[%x] len=%d ", pcln, pcln->len);
				show_err_HRESULT("(master)�� ������� ����������� ��������� �������", scRet);				
				release_Client(pcln);
				return 1;
			}
			ext->is_Crypted = TRUE;
			/*������ ������������� ��������� ������������ ��������� ������� (������� ����� �����������)*/
			pcln->len = Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer;
			pcln->cur = 0;
			pcln->DataBuf.buf = pcln->data;
			printf("[%x] ��������--> %d\n", pcln, pcln->len);
		}
	}
	return ret;
}

void * release_Crypt(struct Client* pcln) {
	if(pcln != NULL && pcln->ext) {		
		if(crypt_disconect(pcln))
			/*� ������ �������� �������� ����������*/
			return pcln->ext;

		struct Crypt * ext = (struct Crypt *)pcln->ext;
		/*�������� ������������ ������� ��� ��������������� (������ �����������)*/
		if(!ext->is_InitContext)
			tab->DeleteSecurityContext(&ext->hContext);
		
		free(ext->IO_Buffer);
		free(ext);
	}

	return NULL;
}
