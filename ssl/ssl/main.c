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

// ������� ������ ������ ���������� ��� �������� ������� ������������.
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

	printf("������ 0x%x (%s) �������� �� CertVerifyCertificateChainPolicy!\n", Status, pszName);
} 

// ������� �������� ����������� �������.
static DWORD VerifyServerCertificate(PCCERT_CONTEXT pServerCert, PSTR pszServerName, DWORD dwCertFlags) {
	HRESULT   Status = SEC_E_OK;;

	PWSTR   pwszServerName = NULL;
	PCCERT_CHAIN_CONTEXT     pChainContext = NULL;
		

	do {
		if(pServerCert == NULL) {
			Status = SEC_E_WRONG_PRINCIPAL;
			break;
		}
		
		// �������������� ����� ������� pszServerName � unicode.
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


		
		// ���������� ������� ������������.
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
			printf("������ 0x%x �������� �� CertGetCertificateChain!\n", Status);
			break;
		}


		
		// �������� ������� ������������.
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
			printf("������ 0x%x �������� �� CertVerifyCertificateChainPolicy!\n", Status);
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

// ������� ��������� ������ �������� �������.
static void GetNewClientCredentials(SecurityFunctionTable* tab, CredHandle *phCreds, CtxtHandle *phContext, SCHANNEL_CRED  * pSchannelCred, HCERTSTORE *phMyCertStore) {
	CredHandle hCreds;
	
	PCCERT_CHAIN_CONTEXT pChainContext;
	
	PCCERT_CONTEXT  pCertContext;
	
	// ������ ������ ���������� ��������� �� schannel
	SecPkgContext_IssuerListInfoEx IssuerListInfo;
	SECURITY_STATUS  Status = tab->QueryContextAttributes(phContext, SECPKG_ATTR_ISSUER_LIST_EX, (PVOID)&IssuerListInfo);
	if(Status != SEC_E_OK) {
		printf("������ ������ ������ ���������� ��������� �� schannel 0x%x \n", Status);
		return;
	}

	
	// ������������ ������������ �������.	
	CERT_CHAIN_FIND_BY_ISSUER_PARA FindByIssuerPara;
	ZeroMemory(&FindByIssuerPara, sizeof(FindByIssuerPara));

	FindByIssuerPara.cbSize = sizeof(FindByIssuerPara);
	FindByIssuerPara.pszUsageIdentifier = szOID_PKIX_KP_CLIENT_AUTH;
	FindByIssuerPara.dwKeySpec = 0;
	FindByIssuerPara.cIssuer = IssuerListInfo.cIssuers;
	FindByIssuerPara.rgIssuer = IssuerListInfo.aIssuers;

	pChainContext = NULL;

	while(TRUE) {
		// ����� ������� ������������.
		pChainContext = CertFindChainInStore(*phMyCertStore,
											 X509_ASN_ENCODING,
											 0,
											 CERT_CHAIN_FIND_BY_ISSUER,
											 &FindByIssuerPara,
											 pChainContext);
		if(pChainContext == NULL) {
			printf("������ 0x%x ������ ������� �����������\n", GetLastError());
			break;
		}
		printf("\n������� ����������� �������\n");

		// ��������� ��������� �� ������� ���������-�����.
		pCertContext = pChainContext->rgpChain[0]->rgpElement[0]->pCertContext;

		// �������� schannel �������.
		pSchannelCred->dwVersion = SCHANNEL_CRED_VERSION;
		pSchannelCred->cCreds    = 1;
		pSchannelCred->paCred    = &pCertContext;

		TimeStamp tsExpiry;
		Status = tab->AcquireCredentialsHandleA(NULL,                   // ��� ��������������
			                                    UNISP_NAME_A,           // ��� ������
			                                    SECPKG_CRED_OUTBOUND,   // ����, ������������ �������������
			                                    NULL,                   // ��������� �� ������������� ������
			                                    pSchannelCred,          // ������ ������
			                                    NULL,                   // ��������� �� ������� GetKey()
			                                    NULL,                   // ��������, ������������ ������� GetKey()
			                                    &hCreds,                // (out) ���������� �������
			                                    &tsExpiry);             // (out) ������ ������������ (�������������)
		if(Status != SEC_E_OK) {
			printf("**** ������ 0x%x � AcquireCredentialsHandle\n", Status);
			continue;
		}
		printf("\n����� ���������� ������\n");

		// ����������� ������ ��������.
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
		                | ISC_REQ_ALLOCATE_MEMORY  //SSP ������� ������ ��� �������. �� ���������� ���������� ������� FreeContextBuffer
		                | ISC_REQ_STREAM;

	// ���������� ������ ������.	
	#define IO_BUFFER_SIZE  0x8000
	char* IoBuffer = LocalAlloc(LMEM_FIXED, IO_BUFFER_SIZE);
	if(IoBuffer == NULL) {
		printf("**** �� ���������� ������ (1)\n");
		return SEC_E_INTERNAL_ERROR;
	}
	
	DWORD cbIoBuffer = 0;
	BOOL  fDoRead = fDoInitialRead;
	
	// ���� �� ��� ����, ���� �� ���������� ����� �����������, ���� �� ���������� ������.
	scRet = SEC_I_CONTINUE_NEEDED;
	while(scRet == SEC_I_CONTINUE_NEEDED || scRet == SEC_E_INCOMPLETE_MESSAGE || scRet == SEC_I_INCOMPLETE_CREDENTIALS) {

		
		//������ ������� ������ �� �������
		if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE) {
			if(fDoRead) {
				cbData = recv(Socket, IoBuffer + cbIoBuffer, IO_BUFFER_SIZE - cbIoBuffer, 0);
				if(cbData<=0) {
					printf("**** ������ ������ ������ �� ������� %d\n", WSAGetLastError());
					scRet = SEC_E_INTERNAL_ERROR;
					break;				
				}

				printf("��������� %d ���� � ������ ��������� ����������\n", cbData);
				cbIoBuffer += cbData;
			} else {
				fDoRead = TRUE;
			}
		}


		/*��������� ���� ������� �������:
		  Buffer 0 �������� ������, ���������� �� �������. Schannel ��������� ��������� ��� ��� �� ���. 
		  ���������� ������ (� ����� ������) ������������� �  buffer 1 � �������� ��� ������ SECBUFFER_EXTRA.*/
		InBuffers[0].pvBuffer   = IoBuffer;
		InBuffers[0].cbBuffer   = cbIoBuffer;
		InBuffers[0].BufferType = SECBUFFER_TOKEN;

		InBuffers[1].pvBuffer   = NULL;
		InBuffers[1].cbBuffer   = 0;
		InBuffers[1].BufferType = SECBUFFER_EMPTY;

		InBuffer.cBuffers  = 2;
		InBuffer.pBuffers  = InBuffers;
		InBuffer.ulVersion = SECBUFFER_VERSION;

		
		/*��������� �������� �������:
		  ������������� ������������ ����� �������, ����� pvBuffer �������� NULL. 
		  ��� ������� ��� ����, ����� � ������ ������� �� ���� ������������� ��������� ������������ ������.*/
		OutBuffers[0].pvBuffer = NULL;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = 0;

		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		
		// ����� InitializeSecurityContext.
		scRet = tab->InitializeSecurityContextA(phCreds              /*���������� ������������� �������*/,
												phContext            /*��� ������ ������, ������� NULL. ��� ����������� - ��������� �� ����������, ������������ � phNewContext ����� ������� ������ � ���� �������.*/,
												NULL                 /*���������� ������������� ������� (������������ ��� ������ ������ � ���� ������ ��� �������������� ����������)*/,
												dwSSPIFlags          /*����� ������*/,
												0                    /*��������������. ������ ���� 0*/,
												SECURITY_NATIVE_DREP /*��������������. ������ ���� 0*/,

												&InBuffer            /*��������� �� ��������� SecBufferDesc.
																	  ��� ������ ������ ������ ���� NULL.
																	  ��� ����������� ������� ������ �������� �� ���� �������� SecBuffer.
																	  ������ ����� ������ ����� ��� SECBUFFER_TOKEN, � ��������� �����, ��������� �� �������.
																	  ������ ����� ������ ����� ��� SECBUFFER_EMPTY. � ���� � ������ ������������� ����� �������� ���������� �������������� ������� ������.*/,

												0                    /*��������������. ������ ���� 0*/,
												NULL                 /*��������� �� CtxtHandle.
																	  ��� ������ ������ ���� ����� ������� ���������� ������ ���������.
																	  ��� ����������� ������� ���������� ���������� ������� ���������� ����� �������� phContext, � � phNewContext ���������� NULL*/,

												&OutBuffer           /*��������� �� ��������� SecBufferDesc, ���������� SecBuffer � ����� SECBUFFER_TOKEN.
																	  �� ������ � ���� ����� ����� ������� �����, ������� ����� ��������� �������.
																	  ��� ������� ����� ISC_REQ_ALLOCATE_MEMORY ���� ����� ����� �������.*/,

												&dwSSPIOutFlags      /*��������� �� ULONG, ���� ����� �������� �����, ����������� �������� ���������������� ���������.
																	  ������ ��������� �������� �������� � �������� ��������� fContextReq.
																	  pfContextAttr �������� ��� �� ����� ������, � ������� �������� ISC_REQ �� ISC_RET.
																	  (��������� ������������ �� ������� ��������� �� ���������� ������ �������, ������������� SEC_E_OK.
																	  ��������� ��������, ����� ��� ISC_RET_ALLOCATED_MEMORY ����� ������������ ����� ������� �� ������)*/,

												&tsExpiry             /*����������. ��������� �� ��������� TimeStamp.
																	   ��� ������� ����������� �������, ���� �������� �������� ����� ��������� ����� �����������.
																	   ����� ������������ ������������ ��������� �������� �������. ����� ������������ � UTC*/);

		
		
		// ���� InitializeSecurityContext ������� ��������� (��� ���� ��������� ���� �� ���������������� ������), �� ����������� ������� ��������� ������ �������.
		if(scRet == SEC_E_OK || scRet == SEC_I_CONTINUE_NEEDED || FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR)) {
			if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL) {
				cbData = send(Socket, OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer, 0);
				if(cbData == 0) {
					printf("**** ������ ������ ������ �� ������� %d (2)\n",
						   WSAGetLastError());
					tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
					tab->DeleteSecurityContext(phContext);
					return SEC_E_INTERNAL_ERROR;
				}

				printf("���������� %d ���� � ������ ��������� ��������� ����������\n", cbData);
				
				// ������������ ��������� ������.
				tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
				OutBuffers[0].pvBuffer = NULL;
			}
		}

		// ���� InitializeSecurityContext ������� ������ SEC_E_INCOMPLETE_MESSAGE, ����� ���������� ��������� ������� ���������� ������ �� ������� (��������� ��������) � ��������� ������� �����.		
		if(scRet == SEC_E_INCOMPLETE_MESSAGE) 
			continue;
		
		// ���� InitializeSecurityContext ���������� SEC_E_OK, ����� ����� ������� ���������� �������.
		if(scRet == SEC_E_OK) {
			/*���� "extra" ����� �������� ������, �� ��� �������� ������� ��������� ������������. 
			 �� ���������� ���������. ���������� ����� ���������� �� ��� ������ DecryptMessage.*/
			printf("���������� �����������\n");

			if(InBuffers[1].BufferType == SECBUFFER_EXTRA) {
				pExtraData->pvBuffer = LocalAlloc(LMEM_FIXED, InBuffers[1].cbBuffer);
				if(pExtraData->pvBuffer == NULL) {
					printf("**** �� ���������� ������ (2)\n");
					return SEC_E_INTERNAL_ERROR;
				}

				MoveMemory(pExtraData->pvBuffer, IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer), InBuffers[1].cbBuffer);

				pExtraData->cbBuffer   = InBuffers[1].cbBuffer;
				pExtraData->BufferType = SECBUFFER_TOKEN;

				printf("� \"extra\" ����� ������� %d ����, ���������� ��� ��������� ����������\n", pExtraData->cbBuffer);
			} else {
				pExtraData->pvBuffer = NULL;
				pExtraData->cbBuffer = 0;
				pExtraData->BufferType = SECBUFFER_EMPTY;
			}

			
			// �����
			{
				// ������ ������������� QueryContextAttributes() & native TimeStamp
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
				printf("�������� ���������� ������������ �� (������� �����) %s \n", ctime(&clock_1970));
			}
			{
				// ������ ������������� QueryContextAttributes() & FileTimeToSystemTime()
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
				printf("���������� ������������ � {%x, %x}: %d/%d/%d %d:%d:%d.%03d UTC\n",
					   hi, lo,
					   st.wYear, st.wMonth, st.wDay,
					   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

				hi = ls.tsExpiry.HighPart;
				lo = ls.tsExpiry.LowPart;
				memcpy(&ft, &ls.tsExpiry, sizeof(ft));
				FileTimeToSystemTime(&ft, &st);
				printf("�� {%x, %x}: %d/%d/%d %d:%d:%d.%03d UTC\n",
					   hi, lo,
					   st.wYear, st.wMonth, st.wDay,
					   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			}
			break;
		}


		// �������� �� ������.
		if(FAILED(scRet)) {
			printf("**** �������� ������ 0x%x �� InitializeSecurityContext (2)\n", scRet);
			break;
		}


		
		// ���� InitializeSecurityContext ���������� SEC_I_INCOMPLETE_CREDENTIALS, �� ������ ������ ��� �������� �������������� �������. 
		if(scRet == SEC_I_INCOMPLETE_CREDENTIALS) {
			/*������. ������ �������� �������������� �������, �� ���������� ������ �� �������� ����������� �������.
			 
			 ��� ������� ������ ������ ���������� ������������ ������� ("issuers"), ���������� �� ������� � �������� ����� ���������� ���������� �������. 
			 ���� ��� ������� ��������� �������, �� ���������� ���������� ��� ������ ������ �����������.
			 � ��������� ������ �������������� ������� ���������� ���������� �������� (��������� ������� ������).*/
			

			GetNewClientCredentials(tab, phCreds, phContext,pSchannelCred,phMyCertStore);

			// ��������� �������.
			fDoRead = FALSE;
			scRet = SEC_I_CONTINUE_NEEDED;

			// ���������� ������ Platform SDK!
			// �������, ��� �� ���� ���������� �� ����� ��������� ������
			cbIoBuffer = 0;

			continue;
		}


		
		// ����������� ���� ������ �� "extra" ������ � ��������� �������.
		if(InBuffers[1].BufferType == SECBUFFER_EXTRA) {
			MoveMemory(IoBuffer,IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer), InBuffers[1].cbBuffer);
			cbIoBuffer = InBuffers[1].cbBuffer;
		} else {
			cbIoBuffer = 0;
		}
	}

	// ����������� ��������� ��������� � ������ ������������ ������.
	if(FAILED(scRet)) {
		tab->DeleteSecurityContext(phContext);
	}

	LocalFree(IoBuffer);

	return scRet;
} 

// ������� ������ ���������� � ����������.
void DisplayConnectionInfo(SecurityFunctionTable* tab, CtxtHandle *phContext) {
	
	SecPkgContext_ConnectionInfo ConnectionInfo;

	SECURITY_STATUS Status = tab->QueryContextAttributes(phContext,
											             SECPKG_ATTR_CONNECTION_INFO,
											             (PVOID)&ConnectionInfo);
	if(Status != SEC_E_OK) {
		printf("������ ��������� ���������� � ���������� 0x%x\n", Status);
		return;
	}

	printf("\n");

	switch(ConnectionInfo.dwProtocol) {
		case SP_PROT_TLS1_CLIENT:			
			printf("��������: Transport Layer Security 1.0 client - side.\n");
			break;
		case SP_PROT_TLS1_SERVER:			
			printf("��������: Transport Layer Security 1.0 server - side.\n");
			break;
		case SP_PROT_SSL3_CLIENT:			
			printf("��������: Secure Sockets Layer 3.0 client - side.\n");
			break;
		case SP_PROT_SSL3_SERVER:			
			printf("��������: Secure Sockets Layer 3.0 server - side.\n");
			break;
		case SP_PROT_TLS1_1_CLIENT:			
			printf("��������: Transport Layer Security 1.1 client - side.\n");
			break;
		case SP_PROT_TLS1_1_SERVER:			
			printf("��������: Transport Layer Security 1.1 server - side.\n");
			break;
		case SP_PROT_TLS1_2_CLIENT:			
			printf("��������: Transport Layer Security 1.2 client - side.\n");
			break;
		case SP_PROT_TLS1_2_SERVER:			
			printf("��������: Transport Layer Security 1.2 server - side.\n");
			break;
		case SP_PROT_PCT1_CLIENT:			
			printf("��������: Private Communications Technology 1.0 client - side.Obsolete.\n");
			break;
		case SP_PROT_PCT1_SERVER:			
			printf("��������: Private Communications Technology 1.0 server - side.Obsolete.\n");
			break;
		case SP_PROT_SSL2_CLIENT:			
			printf("��������: Secure Sockets Layer 2.0 client - side.Superseded by SP_PROT_TLS1_CLIENT.\n");
			break;
		case SP_PROT_SSL2_SERVER:			
			printf("��������: Secure Sockets Layer 2.0 server - side.Superseded by SP_PROT_TLS1_SERVER.\n");
			break;
		default:
			printf("��������: 0x%x\n", ConnectionInfo.dwProtocol);
	}

	switch(ConnectionInfo.aiCipher) {
		case CALG_3DES:
			printf("����������: 3DES block encryption algorithm.\n");
			break;
		case CALG_AES_128:
			printf("����������: AES 128 - bit encryption algorithm.\n");
			break;
		case CALG_AES_256:
			printf("����������: AES 256 - bit encryption algorithm.\n");
			break;
		case CALG_DES:
			printf("����������: DES encryption algorithm.\n");
			break;
		case CALG_RC2:
			printf("����������: RC2 block encryption algorithm.\n");
			break;
		case CALG_RC4:
			printf("����������: RC4 stream encryption algorithm.\n");
			break;
		case 0: 
			printf("����������: No encryption.\n");
			break;
		default:
			printf("����������: 0x%x\n", ConnectionInfo.aiCipher);
	}

	printf("��������� ����������: %d\n", ConnectionInfo.dwCipherStrength);

	switch(ConnectionInfo.aiHash) {
		case CALG_MD5:
			printf("�����������: MD5 hashing algorithm.\n");
			break;
		case CALG_SHA:
			printf("�����������: SHA hashing algorithm.\n");
			break;
		default:
			printf("�����������: 0x%x\n", ConnectionInfo.aiHash);
	}

	printf("��������� �����������: %d\n", ConnectionInfo.dwHashStrength);

	switch(ConnectionInfo.aiExch) {
		case CALG_DH_EPHEM:
			printf("����� �������: DH Ephemeral (Diffie - Hellman key exchange)\n");
			break;
		case CALG_RSA_KEYX:
			printf("����� �������: RSA key exchange\n");
			break;				
		default:
			printf("����� �������: 0x%x\n", ConnectionInfo.aiExch);
	}

	printf("��������� �����: %d\n", ConnectionInfo.dwExchStrength);
}

// ������� ��������� ����� ��� ������ Https.
SECURITY_STATUS HttpsGetFile(SecurityFunctionTable* tab, SOCKET Socket, PCredHandle phCreds, CtxtHandle *phContext, LPSTR pszFileName, SCHANNEL_CRED  * pSchannelCred, HCERTSTORE *phMyCertStore) {
	// ������ ������� ��������� ������������.
	SecPkgContext_StreamSizes Sizes;
	SECURITY_STATUS scRet = tab->QueryContextAttributes(phContext,
										                SECPKG_ATTR_STREAM_SIZES,
										                &Sizes);
	if(scRet != SEC_E_OK) {
		printf("**** ������ 0x%x ������  SECPKG_ATTR_STREAM_SIZES\n", scRet);
		return scRet;
	}

	printf("\n������ ���������: %d, ������ ������: %d, ������������ ������ ���� ���������: %d\n",Sizes.cbHeader, Sizes.cbTrailer, Sizes.cbMaximumMessage);

	/*��������� �������� ������. 
	  �������� �����, ������������ EncryptMessage, ������ �� ������������ �� �������� 'Sizes.cbMaximumMessage', 
	  ������� ������ ������ ����� ���� ��������, �������� � ��������� ��������� � �������������� �����.*/
	DWORD cbIoBufferLength = Sizes.cbHeader + Sizes.cbMaximumMessage + Sizes.cbTrailer;// ����� ���� ����� ���������� ������� � ������� ��� ����� ������ ����� cbIoBufferLength ����������� ����� ������� + 2048 ���� ��. RFC 2246

	char * pbIoBuffer = LocalAlloc(LMEM_FIXED, cbIoBufferLength);
	if(pbIoBuffer == NULL) {
		printf("**** �� ���������� ������ (2)\n");
		return SEC_E_INTERNAL_ERROR;
	}
		
	/*���������� HTTP ������� �������.
	 �������� ����������� '/' �� ����� �����, ������ ���� ������ ����.*/
	if(pszFileName && strlen(pszFileName) > 1 && pszFileName[strlen(pszFileName) - 1] == '/') 
		pszFileName[strlen(pszFileName) - 1] = 0;	

	/*���������� HTTP �������, ���������� �� "header size" ���� � ������.
	 ��� ��������� Schannel ��������� �������� ������������.*/
	char * pbMessage = pbIoBuffer + Sizes.cbHeader;

	/*���������� HTTP �������. �� ������, ��� ������������ ������ ���������.
	  ���� ��� �� ���, ��� ���������� break.*/
	sprintf(pbMessage,"GET /%s HTTP/1.1\r\n"
			          "User-Agent: Webclient\r\n"
				      "Accept:*/*\r\n"
					  "\r\n", pszFileName);
	printf("\nHTTP request: \n%s\n", pbMessage);
	DWORD cbMessage = (DWORD)strlen(pbMessage);
	printf("������ �� �������������� �������: %d ����\n", cbMessage);
		
	/*������������ HTTP �������.*/
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

	scRet = tab->EncryptMessage(phContext, 0, &Message, 0); //����������
	if(FAILED(scRet)) {
		printf("**** ������ 0x%x ���������� EncryptMessage\n", scRet);
		return scRet;
	}
	 
	/*�������� ������ � �����*/
	DWORD cbData = send(Socket, pbIoBuffer, Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer, 0);
	if(cbData == 0) {
		printf("**** ������ %d �������� �������������� ��������� ������� (3)\n",  WSAGetLastError());
		tab->DeleteSecurityContext(phContext);
		return SEC_E_INTERNAL_ERROR;
	} else 
		printf("���������� %d ���� �������������� ���������\n", cbData);

	
	DWORD cbIoBuffer = 0; //������ ������������ ���������
	while(TRUE) {
		/*������ �� ������ �������*/
		if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE) {
			cbData = recv(Socket, pbIoBuffer + cbIoBuffer,  cbIoBufferLength - cbIoBuffer,  0);
			if(cbData == 0) {				
				if(cbIoBuffer) {
					printf("**** ������ ���������� �������� ����������\n");
					scRet = SEC_E_INTERNAL_ERROR;
					return scRet;
				} else 
					break;				
			} else {
				printf("�������� %d ���� �������������� ���������\n", cbData);
				cbIoBuffer += cbData;
			}
		}

		
		// ������� ������������� ���������� �� ������� ������.
		Buffers[0].pvBuffer   = pbIoBuffer;
		Buffers[0].cbBuffer   = cbIoBuffer;
		Buffers[0].BufferType = SECBUFFER_DATA;

		Buffers[1].BufferType = Buffers[2].BufferType = Buffers[3].BufferType = SECBUFFER_EMPTY;		

		Message.ulVersion = SECBUFFER_VERSION;
		Message.cBuffers  = 4;
		Message.pBuffers  = Buffers;

		scRet = tab->DecryptMessage(phContext, &Message, 0, NULL); //�����������
		if(scRet == SEC_E_INCOMPLETE_MESSAGE) 
			/*������� ����� �������� ������ �������� ������������� ������. ����������� ����� � ������ ������.*/
			continue;
		else if(scRet == SEC_I_CONTEXT_EXPIRED)
			/*������ �������� ������*/
			break;
		else if(scRet != SEC_E_OK &&
		        scRet != SEC_I_RENEGOTIATE &&
		        scRet != SEC_I_CONTEXT_EXPIRED) {
			printf("**** ������ 0x%x � �������� ����������� (DecryptMessage)\n", scRet);
			return scRet;
		}

		// ������������ ������ � (�������������) ���������� �������.		
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

		// ����� ��� ��������� �������������� ������.
		if(pDataBuffer) 
			printf("������ �������������� ������: %d ����\n", pDataBuffer->cbBuffer);
		

		// ������� ���� "extra" ������ �� ������� �����.
		if(pExtraBuffer) {
			MoveMemory(pbIoBuffer, pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);
			cbIoBuffer = pExtraBuffer->cbBuffer;
		} else 
			cbIoBuffer = 0;
		

		if(scRet == SEC_I_RENEGOTIATE) {			
			printf("������ �������� ���������� ������!\n");
			SecBuffer ExtraBuffer;
			scRet = ClientHandshakeLoop(tab, Socket, phCreds, phContext, FALSE, &ExtraBuffer, pSchannelCred, phMyCertStore);
			if(scRet != SEC_E_OK) 
				return scRet;
			
			// ������� ���� "extra" ������ �� ������� �����.
			if(ExtraBuffer.pvBuffer) {
				MoveMemory(pbIoBuffer, ExtraBuffer.pvBuffer, ExtraBuffer.cbBuffer);
				cbIoBuffer = ExtraBuffer.cbBuffer;
			}
		}
	}

	return SEC_E_OK;
}

// �������, ����������� ������ ���������� � ��������.
LONG DisconnectFromServer(SecurityFunctionTable* tab, SOCKET Socket, PCredHandle phCreds, CtxtHandle * phContext) {
	// ����������� schannel � �������� ����������.
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

	
	// ���������� SSL ���������, ����������� ������������ � ��������.
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
		
	// ������� ����� ��������� �������.
	if(pbMessage != NULL && cbMessage != 0) {
		DWORD cbData = send(Socket, pbMessage, cbMessage, 0);
		if(cbData == 0) {
			Status = WSAGetLastError();
			printf("**** Error %d sending close notify\n", Status);
			return 1;
		}

		printf("Sending Close Notify\n");
		printf("%d bytes of handshake data sent\n", cbData);


		// ������������ ��������� ������.
		tab->FreeContextBuffer(pbMessage);
	}

	return Status;
}

int main() {
	/*�����������*/
	setlocale(LC_ALL, "russian");
	
	HCERTSTORE hMyCertStore=NULL;
	SOCKET Socket = INVALID_SOCKET;
	SecurityFunctionTable* tab = NULL;
	CredHandle hCredential;
	CtxtHandle hContext;
	memset(&hContext, 0, sizeof(CtxtHandle));
	PCCERT_CONTEXT pRemoteCertContext = NULL;

	do {
		// �������� ��������� ������������ "MY" , � ������� Internet Explorer ������ ����������� �������.
		hMyCertStore = CertOpenSystemStore(0, "MY");
		if(!hMyCertStore) {
			printf("**** ������ �������� ��������� ������������ 0x%x (CertOpenSystemStore)\n",  GetLastError());
			break;
		}

		// ���������� ��������� Schannel ��������. � ������ ������� ������������ ������������ �������� � ����������.
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
		                        | SCH_CRED_MANUAL_CRED_VALIDATION; // ���� SCH_CRED_MANUAL_CRED_VALIDATION ����������, ��������� ��������� � ������ ������� ���������� ������� ����������� "�������". 

		tab = InitSecurityInterface();

		/*��������� ���������� ������������� �������*/
		SCHANNEL_CRED AuthData;
		memset(&AuthData, 0, sizeof(SCHANNEL_CRED));

		AuthData.dwVersion             = SCHANNEL_CRED_VERSION;
		AuthData.dwFlags               = SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_MANUAL_CRED_VALIDATION; //SCH_CRED_MANUAL_CRED_VALIDATION - ��� ������ �������� �����������
		AuthData.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT; /*�������� ����������*/
			
		
		TimeStamp tsExpiry;
		SECURITY_STATUS rc = tab->AcquireCredentialsHandle( NULL                 /*�� ������������, ������ ���� NULL*/,
														    UNISP_NAME_A         /*�������� ������ ������������. ����������� UNISP_NAME_A*/,
															SECPKG_CRED_OUTBOUND /*���� - ��������� ������������� �������������. 
																				 SECPKG_CRED_INBOUND  - ��� ��������� �������� ��������� (������) 
																				 SECPKG_CRED_OUTBOUND - ��� ���������� ��������� ��������� (������)*/,
															NULL                 /*�� ������������, ������ ���� NULL.*/,
															&AuthData            /*������ ��� �������� �������������, ��������� �� ��������� SCHANNEL_CRED*/,
															NULL                 /*�� ������������, ������ ���� NULL*/,
															NULL                 /*�� ������������, ������ ���� NULL*/,
															&hCredential         /*���������� ������������� �������*/,
															&tsExpiry            /*��������� �� ��������� TimeStamp, ���������� ���� �������� ������������� � ��������� �������*/);
		if(rc != SEC_E_OK) {
			perror("�� ������� ������������ ���������� ������������� �������");
			break;
		}

		/*������������� WSA*/
		WSADATA WsaData;
		if(WSAStartup(0x0101, &WsaData) == SOCKET_ERROR) {
			printf("������ ������������� ����� WSA %d (WSAStartup)\n", GetLastError());
			break;
		}

		/*������������*/
		Socket = socket(PF_INET, SOCK_STREAM, 0);
		if(Socket == INVALID_SOCKET) {
			printf("**** ������ �������� ������ %d (socket)\n", WSAGetLastError());
			break;
		}

		int port = 443;
		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons((u_short)port);

		char ServerName[] = "google.ru";
		struct hostent *hp;
		if((hp = gethostbyname(ServerName)) == NULL) { /*�������� ����� �� �����*/
			printf("**** ������ %d ��������� ������ �� ����� �� DNS (gethostbyname)\n", WSAGetLastError());
			break;
		} else {
			memcpy(&sin.sin_addr, hp->h_addr, 4);
		}


		if(0!=connect(Socket, (struct sockaddr *)&sin, sizeof(sin))) {
			printf("**** ������ %d ��������� ���������� � \"%s\" (%s)\n", WSAGetLastError(), ServerName,  inet_ntoa(sin.sin_addr));
			closesocket(Socket);
			break;
		}		

		/*��������� ������������ ����������*/
		DWORD Flags = ISC_REQ_SEQUENCE_DETECT 
			          | ISC_REQ_REPLAY_DETECT 
			          | ISC_REQ_CONFIDENTIALITY 
					  | ISC_RET_EXTENDED_ERROR
			          | ISC_REQ_ALLOCATE_MEMORY  //SSP ������� ������ ��� �������. �� ���������� ���������� ������� FreeContextBuffer
			          | ISC_REQ_STREAM ;
		

		//������������� ���������, ��� ��������� ������
		SecBuffer OutBuffers[1];
		OutBuffers[0].pvBuffer   = NULL;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer   = 0;

		SecBufferDesc OutBuffer;
		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		ULONG SSPIOutFlags= 0;
		rc = tab->InitializeSecurityContextA(&hCredential            /*���������� ������������� �������*/,
									        NULL                     /*��� ������ ������, ������� NULL. ��� ����������� - ��������� �� ����������, ������������ � phNewContext ����� ������� ������ � ���� �������.*/,
									        ServerName               /*���������� ������������� ������� (������������ ��� ������ ������ � ���� ������ ��� �������������� ����������)*/,
									        Flags                    /*����� ������*/,												 
										    0                        /*��������������. ������ ���� 0*/,
											SECURITY_NATIVE_DREP     /*��������������. ������ ���� 0*/,
												 
										    NULL                     /*��������� �� ��������� SecBufferDesc. 
									   		 						 ��� ������ ������ ������ ���� NULL. 
																	 ��� ����������� ������� ������ �������� �� ���� �������� SecBuffer. 
																	 ������ ����� ������ ����� ��� SECBUFFER_TOKEN, � ��������� �����, ��������� �� �������. 
																	 ������ ����� ������ ����� ��� SECBUFFER_EMPTY. � ���� � ������ ������������� ����� �������� ���������� �������������� ������� ������.*/,
												 
										    0                        /*��������������. ������ ���� 0*/,
												 
										    &hContext                /*��������� �� CtxtHandle.
																	 ��� ������ ������ ���� ����� ������� ���������� ������ ���������. 
																	 ��� ����������� ������� ���������� ���������� ������� ���������� ����� �������� phContext, � � phNewContext ���������� NULL*/,
												 
										    &OutBuffer               /*��������� �� ��������� SecBufferDesc, ���������� SecBuffer � ����� SECBUFFER_TOKEN.
																	 �� ������ � ���� ����� ����� ������� �����, ������� ����� ��������� �������. 
																	 ��� ������� ����� ISC_REQ_ALLOCATE_MEMORY ���� ����� ����� �������.*/,
												 
										    &SSPIOutFlags            /*��������� �� ULONG, ���� ����� �������� �����, ����������� �������� ���������������� ���������.
																	 ������ ��������� �������� �������� � �������� ��������� fContextReq. 
																	 pfContextAttr �������� ��� �� ����� ������, � ������� �������� ISC_REQ �� ISC_RET. 
																	 (��������� ������������ �� ������� ��������� �� ���������� ������ �������, ������������� SEC_E_OK. 
																	 ��������� ��������, ����� ��� ISC_RET_ALLOCATED_MEMORY ����� ������������ ����� ������� �� ������)*/,

										    &tsExpiry                /*����������. ��������� �� ��������� TimeStamp. 
																	 ��� ������� ����������� �������, ���� �������� �������� ����� ��������� ����� �����������. 
																	 ����� ������������ ������������ ��������� �������� �������. ����� ������������ � UTC*/);
		if(rc != SEC_I_CONTINUE_NEEDED) {
			perror("�� ������� ���������������� ������� ������������");
			break;
		}

		//���������� ����� �������
		if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL) {
			DWORD cbData = send(Socket, OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer, 0);
			if(cbData <= 0) {
				printf("**** ������ �������� ������ ������� %d (1)\n", WSAGetLastError());
				tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
				tab->DeleteSecurityContext(&hContext);
				break;
			}

			printf("���������� %d ���� � ������ ��������� ����������\n", cbData);

			/*�������� ��������� ������*/
			tab->FreeContextBuffer(OutBuffers[0].pvBuffer);
			OutBuffers[0].pvBuffer = NULL;
		}

		//����� ����������� � �������� (��������� ��������� ����������)
		SecBuffer  ExtraData;
		if(SEC_E_OK != ClientHandshakeLoop(tab, Socket, &hCredential, &hContext, TRUE, &ExtraData, &SchannelCred, &hMyCertStore))
			break;

		// ��������� ����������� �������.
		SECURITY_STATUS Status = tab->QueryContextAttributes(&hContext,
															 SECPKG_ATTR_REMOTE_CERT_CONTEXT,
															 (PVOID)&pRemoteCertContext);
		if(Status != SEC_E_OK) {
			printf("������ ��������� ����������� ������� 0x%x \n", Status);
			break;
		}

		// �������� ���������������� ����������� �������.
		Status = VerifyServerCertificate(pRemoteCertContext,ServerName, 0);
		if(Status) {
			/*���������� ������� �� ������������. �������� ���� ������������
			 ���������� � ������������ ��������, ���� ������������� ����� "��������� ����������".
			 ����� ����� �������� ����������.*/
			printf("**** ������ 0x%x �������� �����������!\n", Status);
			break;
		} else {
			// ������������ ��������� ����������� �������.
			CertFreeCertificateContext(pRemoteCertContext);
			pRemoteCertContext = NULL;
			printf("���������� ������ ��������.\n");
		}
		
		// ����� ���������� � ����������. 
		DisplayConnectionInfo(tab, &hContext);


		/*��������� ����� � �������*/
		LPSTR   pszFileName = "Default.Htm";
		if(HttpsGetFile(tab, Socket, &hCredential, &hContext, pszFileName, &SchannelCred, &hMyCertStore)) {
			printf("������ ��������� ����� � �������\n");
			break;
		}

		/*����������� ������ � ������� ����������*/
		DisconnectFromServer(tab, Socket, &hCredential, &hContext);		
	} while(FALSE);

	/*������������ ��������� ����������� �������*/
	if(pRemoteCertContext)
		CertFreeCertificateContext(pRemoteCertContext);

		
	if(tab != NULL) {
		/*�������� ������������������� ��������*/
		tab->DeleteSecurityContext(&hContext);

		/*�������� ����������� SSPI �������*/
		tab->FreeCredentialsHandle(&hCredential);
	}

	/*�������� ������*/
	if(Socket != INVALID_SOCKET) {
		shutdown(Socket, SD_BOTH);
		closesocket(Socket);
	}

	/*���������� ������ ���������� WinSock*/
	WSACleanup();


	// �������� ��������� ������������ "MY".
	if(hMyCertStore) {
		CertCloseStore(hMyCertStore, 0);
	}

	system("pause");
	return 0;
}