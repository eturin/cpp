/* @(#) $Id: //bas/710_REL/src/include/saprfc.h#4 $ SAP*/


/*---------------------------------------------------------------------------------
 * R F C  S D K
 *
 *
 *  (c) Copyright SAP AG, Walldorf  1993 - 1999
 *
 *	Portion of this software is based on the work from
 *	Visual Edge Software
 *---------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------
 * internal hint: this file is shipped to customers and must not include or use
 * any SAP internal data types or include files.
 *---------------------------------------------------------------------------------
 */

/*
 * @doc
 *
 * @module Remote Function Call API  |
 *
 * The Remote Function Call API (RFC API) allows -
 * remotely or locally - calling ABAP/4 function
 * modules from C programs as well as receiving call
 * request issued from an ABAP/4 program by the
 * CALL FUNCTION interface.
 *
 * In ABAP/4 function modules are special routines with
 * well defined and documented interfaces, which are
 * developed within a library development workbench.
 *
 * ABAP/4 function modules use named parameter passing.
 * The function module may raise exceptions to indicate 
 * errors. These exceptions can be caught
 * by the caller of a function module.
 *
 * The following functions form the programming interface to
 * call ABAP/4 function modules from a C environment.
 *
 * @group Functions for a RFC client program
 * @flag <f RfcOpenEx>        | opens an RFC connection including SAP logon,
 *                              can also be used for
 *                              non C environments such as Visual Basic.
 *                              This call is recommended instead of RfcOpen,
 *                              RfcOpenExt, RfcOpenExtV3 and RfcConnect.
 * @flag <f RfcOpen>          | opens an RFC connection.
 * @flag <f RfcOpenExt>       | another way to open an RFC connection more
 *                              appropriate for non C environments such as Visual Basic.
 * @flag <f RfcOpenExtV3>     | another way to open an RFC connection more
 *                              appropriate for non C environments such as Visual Basic
 *                              (using RFC Version 3).
 * @flag <f RfcConnect>       | opens an RFC connection via LOAD BALANCING (R/3 only).
 * @flag <f RfcCall>          | Calls an ABAP/4 function module.
 * @flag <f RfcReceive>       | Receives the return values from an
 *                              ABAP/4 function module.
 * @flag <f RfcCallReceive>   | Calling a function module and receiving the
 *                              return values in one step.
 * @flag <f RfcCreateTransID> | Gets a unique Transaction-ID for calling an ABAP/4
 *                              function module using the transactional RFC Interface.
 * @flag <f RfcIndirectCall>  | Calls an ABAP/4 function module using transactional RFC
 *                              Interface in R/3 (implicit confirmation).
 * @flag <f RfcIndirectCallEx> | Calls an ABAP/4 function module using transactional RFC
 *                              Interface in R/3 ( (explicit confirmation).
 * @flag <f RfcConfirmTransID> | Confirms a transaction called by RfcIndirectCallEx.
 * @flag <f RfcCleanupContext> | Cleanup the attached R/3 context
 * @flag <f RfcRegisterPasswordChanger> | Register a function to change expired passwords
 *
 * @flag <f RfcCancel>         | Cancel an rfc call. This function works only with
 *                               RfcCall, RfcListen, RfcReceive construct.
 *
 * @group Functions for a RFC server program
 * @flag <f RfcAccept>                    | accepting a RFC request or registering at a
 *                                          SAP gateway.
 * @flag <f RfcInstallFunction>           | registering functions as callable
 *                                          function modules.
 * @flag <f RfcDispatch>                  | waiting for the next function call.
 * @flag <f RfcGetData>                   | receiving the parameters of a function.
 * @flag <f RfcDynamicGetData>            | receiving the parameters of a function one-by-one.
 * @flag <f RfcSendData>                  | sending back the return values.
 * @flag <f RfcRaise>                     | raising an exception.
 * @flag <f RfcGetName>                   | reading the symbolic function name.
 *                                          (Max. 30 B)
 * @flag <f RfcGetLongName>				  | internal use only. Reading symbolic function name that 
 *                                          are longer than 30 characters.
 * @flag <f RfcGetNameEx>                 | internal use only. Reading the symbolic function name.
 *                                          (Max. 30 B) and returning an according
 *                                          return code to the RFC program after
 *                                          a system call such as RFC_PING for
 *                                          connection test is automatically executed
 *                                          by the RFC library
 * @flag <f RfcGetLongNameEx>			  | reading the symbolic function name longer than 30
 *											and returning code to the RFC program after
 *											a system call such as RFC_PING for
 *                                          connection test is automatically executed
 *											by the RFC library.
 * @flag <f RfcCallReceive>               | Calling a function module and receiving
 *                                          the return values in one step. It can be
 *                                          be used for calling back a function module
 *                                          in R/3 using the same RFC connection while
 *                                          executing a RFC function in this server
 *                                          program. Using <f RfcCall> and <f RfcReceive>
 *                                          is also possible.
 * @flag <f RfcInstallTransactionControl> | registering functions as callable
 *                                          function modules for transactional RFC.
 * @flag <f RfcInstallTransactionControl2>| registering functions as callable
 *                                          function modules for transactional RFC.
 * @flag <f RfcSncMode>                   | getting the <t RFC_SNC_MODE> (ON or OFF)
 * @flag <f RfcSncPartnerName>            | getting the SNC name of the RFC client
 * @flag <f RfcSncPartnerAclKey>          | getting the SNC ACL key of the RFC client
 *
 * @group Special Functions
 * @flag <f RfcGetPatchLevel>     | Retrieve precise version information from the build
 * @flag <f RfcAttachContext>     | Attach a context to a RFC handle
 * @flag <f RfcGetAttachedContext>| Retrieves a previously attached context
 * @flag <f RfcDetachContext>     | Dettach a context from a RFC handle
 * @flag <f RfcListen>            | listen for incoming RFC events.
 * @flag <f RfcWaitForRequest>    | wait for incoming RFC requests.
 *                                  Only available after <f RfcAccept> in
 *                                  register mode.
 * @flag <f RfcGetAttributes>     | get some information about this RFC connection
 * @flag <f RfcGetCodePagesInfo>  | get information on the code page used for the call
 * @flag <f RfcInstallStructure>  | define a structure for working with inhomogeneous
 *                                  structure or internal table in an RFC client or
 *                                  server program (for use in NON-Unicode RFC-Library)
 * @flag <f RfcInstallStructure2>  | define a structure for working with inhomogeneous
 *                                  structure or internal table in an RFC client or
 *                                  server program (for use in NON-Unicode RFC-Library)
 * @flag <f RfcInstallUnicodeStructure> | define a structure for working with inhomogeneous
 *                                       structure or internal table in an RFC client or
 *                                       server program (for use in Unicode and non Unicode RFC-Library)
 * @flag <f RfcExidToRfcType>     | Convert ABAP internal type into RFC_TYPES
 * @flag <f RfcTypeToString>      | Convert RFC_TYPES to strings for traces purpose
 * @flag <f RfcGetStructureInfoAsTable>| Wraper to get structure information at runtime
 * @flag <f RfcGetStructureInfoFlat>   | Wraper to get structure information at runtime
 * @flag <f RfcReleaseStructInfo>| Relase memory containing structure information
 * @flag <f RfcCheckRegisterServer>   | check for registered RFC server at a SAP gateway
 * @flag <f RfcCancelRegisterServer>  | cancel all registered RFC servers at a SAP gateway
 * @flag <f RfcSncNameToAclKey>   | convert an SNC name in an SNC ACL key
 * @flag <f RfcSncAclKeyToName>   | convert an SNC ACL key in an SNC name
 *
 * @flag <f RfcAllowStartProgram> | Allows the RFC library for starting some programs
 *
 * @flag <f RfcSetTrace>          | Activate/Deactivate the RFC trace
 *
 * @flag <f RfcSetParameter>      | Set RFC-Parameter
  *
 * @flag <f RfcQueueInsert>       | Insert the tRFC-call in the R/3 receive queue
 *                                  instead of immediately executing as RfcIndirectCall
 *                                  does.
 *
 * @flag <f RfcIsoLangToSap>	  | Converts 2-byte ISO-Language into 1-byte SAP-Language
 * @flag <f RfcSapLangToIso>	  | Converts 1-byte SAP-Language into 2-byte ISO-Language
 *
 * @flag <f RfcCharConversion>    | Turn off/on the conversion of characters between
 *                                  own codepage and partner's codepage
 *
 * @flag <f RfcConvertA2E>        | convert ascii to ebcdic; only on OS400
 *
 * @flag <f RfcConvertE2A>        | convert ebcdic to ascii; only on OS400
 *
 * @flag <f RfcIsValidHandle>     | Check handle
 *
 * @flag <f RfcGetTicket>         | Retrieve backend generated cookie version 2 after calling RfcOpenEx
 *                                  with flag GETSSO2
 *
 * @flag <f RfcSetCodePage>       | Set codepage for a rfc connection
 *
 * @flag <f RfcSetSystemCodePage> | Change default codepage for all further connections
 *
 * @flag <f RfcSetSystemTimeout>  | Set timeout for all blocking functions
 *
 * @flag <f RfcGetEnqueueKey>     | Retrieves enqueue key sended by R3
 *
 * @flag <f RfcSetEnqueueKey>     | Sets enqueue key, which will be sent to R/3 asap
 *
 * @flag <f RfcGetClientId>       | Retrieves client id sent by R/3
 *
 * @flag <f RfcGetClientIdEx>     | Retrieves client id sent by R/3 in the new format
 *
 * @flag <f RfcSetClientIdEx>     | Sets client id, which will be sent to R/3 asap
 *
 * @flag <f RfcCreateUuid>        | Creates an unique identifier
 *
 * @flag <f RfcGetCallInfo>       | Retrieves RFC call information
 * 
 * @flag <f RfcGetLuwInfo>        | Retrieves LUW call information 
 *
 * @flag <f RfcStatistic>         | Gets available statistic 
 *
 * @flag <f RfcInstallExternalLogonHandler> | Installs logon handler for an external rfc server
 *
 * @flag <f RfcCallEx>            | Calls an ABAP/4 function module.
 *
 * @flag <f RfcReceiveEx>         | Receives the return values from an
 *                                  ABAP/4 function module.
 *
 * @flag <f RfcCallReceiveEx>     | Calling a function module and receiving the
 *                                  return values in one step.
 *
 *
 * @flag <f RfcSetMaxCpicConn>   | Defines the maximal number of active cpic connections.
 *
 * @flag <f RfcUseSideInfo>       | CPIC-parameter. CPIC-layer is allowed to suing sideinfo file.
 */

#ifndef RFC_H
#define RFC_H


#ifndef SAPRFC_H
#define SAPRFC_H

#ifdef __midl
/*
 * this part is used when compiled via MIDL
 */
#define SAPonNT
#else /* C/C++ */
/* wchar_t */
#include<stdlib.h>
/* integers */
#ifndef INT_MAX
#include<limits.h>
#endif

#define RFC_VERSION 3


/*
 * For Windows 3.x _WINDOWS has to be defined, as it is done
 * by MS Visual C++
 */

#ifndef SAPTYPE_H

#ifdef _WIN32
#  ifndef SAPonNT
#    define SAPonNT
#  endif
#endif  /* _WIN32       */

#ifdef _WINDOWS
#  ifndef _WIN32
#    ifndef SAPonWINDOWS
#      define SAPonWINDOWS
#    endif
#  endif  /* _WIN16     */
#endif  /* _WINDOWS     */

#ifdef __OS400__
#  ifndef SAPonOS400
#    define SAPonOS400
#  endif
#endif  /* __OS400__ */

#endif  /* SAPTYPE_H    */


#endif /* MIDL <-> C/C++ */


#if !defined(SAP_FAR)|| !defined(SAP_PASCAL)|| !defined(SAP_EXPORT)|| !defined(SAP_LOADDS) || !defined(SAP_STDCALL)
#   undef SAP_FAR
#   undef SAP_PASCAL
#   undef SAP_EXPORT
#   undef SAP_LOADDS
#   undef SAP_STDCALL
#   if defined(SAPonWINDOWS)
#       define SAP_FAR __far
#       define SAP_PASCAL __pascal
#       define SAP_EXPORT __export
#       define SAP_LOADDS __loadds
#       define SAP_STDCALL
#   elif defined(SAPonNT)
#       define SAP_FAR
#       define SAP_PASCAL
#       define SAP_EXPORT
#       define SAP_LOADDS
#       define SAP_STDCALL _stdcall
#   else
#       define SAP_FAR
#       define SAP_PASCAL
#       define SAP_EXPORT
#       define SAP_LOADDS
#       define SAP_STDCALL
#   endif
#endif

#ifndef DECL_EXP
#   if defined(SAPonLIN) && defined(GCC_HIDDEN_VISIBILITY)
#     define DECL_EXP __attribute__((visibility("default")))
#   else
#     define DECL_EXP
#   endif
#endif

#if !defined(_SYSTEM)
#   if defined(OS2)
#       define _SYSTEM _System
#   else
#       define _SYSTEM
#       define SAP_API SAP_FAR SAP_LOADDS SAP_PASCAL SAP_STDCALL
#   endif
#endif /* _SYSTEM */

#if !defined(SAP_API)
#   if defined(OS2)
#       define SAP_API _SYSTEM
#   endif
#endif

#if !defined(WIN_DLL_EXPORT_FLAGS)
#   define WIN_DLL_EXPORT_FLAGS SAP_API
#endif /* WIN_DLL_EXPORT_FLAGS */


#if !defined(DLL_CALL_BACK_FUNCTION)
#   if defined(SAPonWINDOWS)
#       define DLL_CALL_BACK_FUNCTION SAP_FAR SAP_PASCAL
#   elif defined(SAPonNT)
#       define DLL_CALL_BACK_FUNCTION SAP_STDCALL
#   elif defined(OS2)
#       define DLL_CALL_BACK_FUNCTION _SYSTEM
#   else
#       define DLL_CALL_BACK_FUNCTION
#   endif
#   if defined(OS2)
#      define DLL_CALL_BACK_FUNCTION_PTR * _SYSTEM
#   else
#      define DLL_CALL_BACK_FUNCTION_PTR DLL_CALL_BACK_FUNCTION *
#   endif
#endif /* DLL_CALL_BACK_FUNCTION */


#if ! defined (RUNT) && ! defined (ABAP0LIB)

/* ABAP/4 data types */
#define TYPC            0
#define TYPDATE         1
#define TYPP            2
#define TYPTIME         3
#define TYPX            4
#define TYPTABH         5
#define TYPNUM          6
#define TYPFLOAT        7
#define TYPINT          8
#define TYPINT2         9
#define TYPINT1         10
/*  TYPW = 11 */
#define TYP1            12
#define TYP2            13
#define TYPDECF16       23  /* IEEE 754r decimal floating point 8 bytes  */
#define TYPDECF34       24  /* IEEE 754r decimal floating point 16 bytes */


/* internal hint :
 * ABAP/4 data types must also be defined in abtypes.h
 */

#else
#include "abtypes.h"
#endif /* ! defined (RUNT) && ! defined (ABAP0LIB) */

/* There are several RFC libraries:
 * - ASCII/MBCS libraries, 
 * - UNICODE libraries, 
 * - Libraries that use ASCII for C strings on EBCDC platforms
 *
 * in order to prevent usage of the wrong libraries,
 * the entry points are mangeled with U or A extension:
 */

#ifndef RFC_API_ENTRY
#  if defined(SAPwithUNICODE)

    /* we have UNICODE */
#   define RFC_API_ENTRY(fn)   fn##U

#  elif defined(SAPonOS390) && defined(SAPwithCHAR_ASCII)

    /* OS/390 with ASCII char */
#   define RFC_API_ENTRY(fn)   fn##A

#  else    
    /* default native char type */
#   define RFC_API_ENTRY(fn)   fn

#  endif
#endif

#define RfcInit	                         RFC_API_ENTRY(RfcInit)
#define RfcEnvironment                   RFC_API_ENTRY(RfcEnvironment)
#define RfcLastError                     RFC_API_ENTRY(RfcLastError)
#define RfcOpenExt                       RFC_API_ENTRY(RfcOpenExt)
#define RfcOpen	                         RFC_API_ENTRY(RfcOpen)
#define RfcAccept                        RFC_API_ENTRY(RfcAccept)
#define RfcAcceptExt	                 RFC_API_ENTRY(RfcAcceptExt)
#define RfcClose                         RFC_API_ENTRY(RfcClose)
#define RfcAbort                         RFC_API_ENTRY(RfcAbort)
#define RfcCall	                         RFC_API_ENTRY(RfcCall)
/*#define RfcCallVMC	                     RFC_API_ENTRY(RfcCallVMC)*/
#define RfcReceive                       RFC_API_ENTRY(RfcReceive)
/*#define RfcReceiveVMC                    RFC_API_ENTRY(RfcReceiveVMC)*/
#define RfcCallReceive                   RFC_API_ENTRY(RfcCallReceive)
/*#define RfcCallReceiveVMC                RFC_API_ENTRY(RfcCallReceiveVMC)*/
#define RfcIndirectCall	                 RFC_API_ENTRY(RfcIndirectCall)
#define RfcGetName                       RFC_API_ENTRY(RfcGetName)
#define RfcGetData                       RFC_API_ENTRY(RfcGetData)
#define RfcSendData	                     RFC_API_ENTRY(RfcSendData)
#define RfcRaise                         RFC_API_ENTRY(RfcRaise)
#define RfcListen                        RFC_API_ENTRY(RfcListen)
#define RfcFileArgv    	                 RFC_API_ENTRY(RfcFileArgv)
#define RfcConnArgv	                     RFC_API_ENTRY(RfcConnArgv)
#define RfcSendSystemInfo                RFC_API_ENTRY(RfcSendSystemInfo)
#define RfcInstallFunction               RFC_API_ENTRY(RfcInstallFunction)
#define RfcInstallFunctionExt	         RFC_API_ENTRY(RfcInstallFunctionExt)
#define RfcDispatch                      RFC_API_ENTRY(RfcDispatch)
#define RfcGetTransID	                 RFC_API_ENTRY(RfcGetTransID)
#define RfcCreateTransID                 RFC_API_ENTRY(RfcCreateTransID)
#define RfcCreateLocalTransID	         RFC_API_ENTRY(RfcCreateLocalTransID)
#define RfcGuidToTid	                 RFC_API_ENTRY(RfcGuidToTid)
#define RfcTidToGuid	                 RFC_API_ENTRY(RfcTidToGuid)
#define RfcCheckTransID	                 RFC_API_ENTRY(RfcCheckTransID)
#define RfcConfirmTransID                RFC_API_ENTRY(RfcConfirmTransID)
#define RfcAllocParamSpace               RFC_API_ENTRY(RfcAllocParamSpace)
#define RfcFreeParamSpace                RFC_API_ENTRY(RfcFreeParamSpace)
#define RfcAddExportParam                RFC_API_ENTRY(RfcAddExportParam)
#define RfcAddImportParam                RFC_API_ENTRY(RfcAddImportParam)
#define RfcAddTable                      RFC_API_ENTRY(RfcAddTable)
#define RfcGetTableHandle                RFC_API_ENTRY(RfcGetTableHandle)
#define RfcCallExt                       RFC_API_ENTRY(RfcCallExt)
#define RfcReceiveExt	                 RFC_API_ENTRY(RfcReceiveExt)
#define RfcCallReceiveExt                RFC_API_ENTRY(RfcCallReceiveExt)
#define RfcIndirectCallExt               RFC_API_ENTRY(RfcIndirectCallExt)
#define RfcGetDataExt	                 RFC_API_ENTRY(RfcGetDataExt)
#define RfcSendDataExt	                 RFC_API_ENTRY(RfcSendDataExt)
#define RfcExecProgram	                 RFC_API_ENTRY(RfcExecProgram)
#define RfcGetVersion	                 RFC_API_ENTRY(RfcGetVersion)
#define RfcGetPatchLevel                 RFC_API_ENTRY(RfcGetPatchLevel)
#define RfcCheckVersionsOfUsedLibs       RFC_API_ENTRY(RfcCheckVersionsOfUsedLibs)
#define RfcGetAllLibVersions	         RFC_API_ENTRY(RfcGetAllLibVersions)
#define RfcGetDomainFingerprintHex       RFC_API_ENTRY(RfcGetDomainFingerprintHex)
#define RfcGetUserSidHex                 RFC_API_ENTRY(RfcGetUserSidHex)
#define RfcConnect                       RFC_API_ENTRY(RfcConnect)
#define RfcInstallTransactionControl	 RFC_API_ENTRY(RfcInstallTransactionControl)
#define RfcInstallTransactionControl2	 RFC_API_ENTRY(RfcInstallTransactionControl2)
#define RfcRaiseTables	                 RFC_API_ENTRY(RfcRaiseTables)
#define RfcRaiseErrorMessage	         RFC_API_ENTRY(RfcRaiseErrorMessage)
#define RfcConnArgv3	                 RFC_API_ENTRY(RfcConnArgv3)
#define RfcOpenExtV3	                 RFC_API_ENTRY(RfcOpenExtV3)
#define RfcGetAttributes    	         RFC_API_ENTRY(RfcGetAttributes)
#define RfcGetCodePagesInfo    	         RFC_API_ENTRY(RfcGetCodePagesInfo)
#define RfcWaitForRequest                RFC_API_ENTRY(RfcWaitForRequest)
#define RfcInstallStructure              RFC_API_ENTRY(RfcInstallStructure)
#define RfcInstallStructure2	         RFC_API_ENTRY(RfcInstallStructure2)
#define RfcStructureToFile               RFC_API_ENTRY(RfcStructureToFile)
#define RfcStructureToFileByName         RFC_API_ENTRY(RfcStructureToFileByName)
#define RfcAcceptWithChannel	         RFC_API_ENTRY(RfcAcceptWithChannel)
#define RfcDefineImportParam	         RFC_API_ENTRY(RfcDefineImportParam)
#define RfcGetImportParam                RFC_API_ENTRY(RfcGetImportParam)
#define RfcSncMode                       RFC_API_ENTRY(RfcSncMode)
#define RfcSncPartnerName                RFC_API_ENTRY(RfcSncPartnerName)
#define RfcSncPartnerAclKey              RFC_API_ENTRY(RfcSncPartnerAclKey)
#define RfcSncNameToAclKey               RFC_API_ENTRY(RfcSncNameToAclKey)
#define RfcSncAclKeyToName               RFC_API_ENTRY(RfcSncAclKeyToName)
#define RfcIoCtl                         RFC_API_ENTRY(RfcIoCtl)
#define RfcSetCodePage	                 RFC_API_ENTRY(RfcSetCodePage)
#define RfcSetSystemCodePage	         RFC_API_ENTRY(RfcSetSystemCodePage)
#define RfcGetDriverCallBack	         RFC_API_ENTRY(RfcGetDriverCallBack)
#define RemObjInstallDriver              RFC_API_ENTRY(RemObjInstallDriver)
#define RemObjRegisterObject	         RFC_API_ENTRY(RemObjRegisterObject)
#define RemObjGetObject	                 RFC_API_ENTRY(RemObjGetObject)
#define RemObjReleaseObject              RFC_API_ENTRY(RemObjReleaseObject)
#define RfcCheckRegisterServer	         RFC_API_ENTRY(RfcCheckRegisterServer)
#define RfcCancelRegisterServer	         RFC_API_ENTRY(RfcCancelRegisterServer)
#define RfcOpenEx                        RFC_API_ENTRY(RfcOpenEx)
#define RfcLastErrorEx	                 RFC_API_ENTRY(RfcLastErrorEx)
#define RfcConvertBcdToChar              RFC_API_ENTRY(RfcConvertBcdToChar)
#define RfcConvertCharToBcd              RFC_API_ENTRY(RfcConvertCharToBcd)
#define RfcTxStart                       RFC_API_ENTRY(RfcTxStart)
#define RfcTxPrepareToCommit	         RFC_API_ENTRY(RfcTxPrepareToCommit)
#define RfcTxCommit                      RFC_API_ENTRY(RfcTxCommit)
#define RfcTxAbort                       RFC_API_ENTRY(RfcTxAbort)
#define RfcIndirectCallEx                RFC_API_ENTRY(RfcIndirectCallEx)
#define RfcIndirectCallExExt	         RFC_API_ENTRY(RfcIndirectCallExExt)
#define RfcUninstallFunction	         RFC_API_ENTRY(RfcUninstallFunction)
#define RfcGuiCheck                      RFC_API_ENTRY(RfcGuiCheck)
#define RfcGuiPrepare	                 RFC_API_ENTRY(RfcGuiPrepare)
#define RfcGuiStart                      RFC_API_ENTRY(RfcGuiStart)
#define RfcGuiReturn	                 RFC_API_ENTRY(RfcGuiReturn)
#define DllRegisterServerForced	         RFC_API_ENTRY(DllRegisterServerForced)
#define RfcGetNameEx	                 RFC_API_ENTRY(RfcGetNameEx)
#define RfcResizeBlob	                 RFC_API_ENTRY(RfcResizeBlob)
#define RfcAllowStartProgram	         RFC_API_ENTRY(RfcAllowStartProgram)
#define RfcSetTrace                      RFC_API_ENTRY(RfcSetTrace)
#define RfcSetParameter                  RFC_API_ENTRY(RfcSetParameter)
#define RfcNormalizeTabOrFldName         RFC_API_ENTRY(RfcNormalizeTabOrFldName)
#define RfcQueueInsert	                 RFC_API_ENTRY(RfcQueueInsert)
#define RfcIsValidHandle                 RFC_API_ENTRY(RfcIsValidHandle)
#define RfcHealthCheck	                 RFC_API_ENTRY(RfcHealthCheck)
#define RfcPlayback                      RFC_API_ENTRY(RfcPlayback)
#define RfcOpenAdodb	                 RFC_API_ENTRY(RfcOpenAdodb)
#define RfcAcceptAdodb	                 RFC_API_ENTRY(RfcAcceptAdodb)
#define RfcCleanupContext                RFC_API_ENTRY(RfcCleanupContext)
#define RfcDynamicGetData                RFC_API_ENTRY(RfcDynamicGetData)
#define RfcGetLongName	                 RFC_API_ENTRY(RfcGetLongName)
#define RfcGetLongNameEx                 RFC_API_ENTRY(RfcGetLongNameEx)
#define RfcAllocString	                 RFC_API_ENTRY(RfcAllocString)
#define RfcFreeString	                 RFC_API_ENTRY(RfcFreeString)
#define RfcIsoLangToSap	                 RFC_API_ENTRY(RfcIsoLangToSap)
#define RfcSapLangToIso	                 RFC_API_ENTRY(RfcSapLangToIso)
#define RfcCancel                        RFC_API_ENTRY(RfcCancel)
#define RfcCharConversion                RFC_API_ENTRY(RfcCharConversion)
#define RfcRegisterPasswordChanger       RFC_API_ENTRY(RfcRegisterPasswordChanger)
#define RfcResizeXString                 RFC_API_ENTRY(RfcResizeXString)
#define RfcGetTicket	                 RFC_API_ENTRY(RfcGetTicket)
#define RfcInstallUnicodeStructure       RFC_API_ENTRY(RfcInstallUnicodeStructure)
#define RfcSetSystemTimeout              RFC_API_ENTRY(RfcSetSystemTimeout)
#define RfcSetBasisTraceFile             RFC_API_ENTRY(RfcSetBasisTraceFile)
#define RfcSetBasisTraceLevel            RFC_API_ENTRY(RfcSetBasisTraceLevel)
#define RfcGetEnqueueKey                 RFC_API_ENTRY(RfcGetEnqueueKey)
#define RfcSetEnqueueKey                 RFC_API_ENTRY(RfcSetEnqueueKey)
#define RfcGetClientId                   RFC_API_ENTRY(RfcGetClientId)
#define RfcGetClientIdEx                 RFC_API_ENTRY(RfcGetClientIdEx)
#define RfcSetClientIdEx                 RFC_API_ENTRY(RfcSetClientIdEx)
#define RfcCreateUuid                    RFC_API_ENTRY(RfcCreateUuid)
#define RfcGetCallInfo                   RFC_API_ENTRY(RfcGetCallInfo)
#define RfcStatistic                     RFC_API_ENTRY(RfcStatistic)
#define RfcGetLuwInfo                    RFC_API_ENTRY(RfcGetLuwInfo)
#define RfcExidToRfcType                 RFC_API_ENTRY(RfcExidToRfcType)
#define RfcTypeToString                  RFC_API_ENTRY(RfcTypeToString)
#define RfcGetStructureInfoAsTable       RFC_API_ENTRY(RfcGetStructureInfoAsTable)
#define RfcGetStructureInfoFlat          RFC_API_ENTRY(RfcGetStructureInfoFlat)
#define RfcReleaseStructInfo             RFC_API_ENTRY(RfcReleaseStructInfo)
#define RfcGetFunctionInfoAsTable        RFC_API_ENTRY(RfcGetFunctionInfoAsTable)
#define RfcAlignFunctionInterface        RFC_API_ENTRY(RfcAlignFunctionInterface)
#define RfcInstallExternalLogonHandler   RFC_API_ENTRY(RfcInstallExternalLogonHandler)
#define RfcCallEx                        RFC_API_ENTRY(RfcCallEx)
#define RfcReceiveEx                     RFC_API_ENTRY(RfcReceiveEx)
#define RfcCallReceiveEx                 RFC_API_ENTRY(RfcCallReceiveEx)
#define RfcAttachContext                 RFC_API_ENTRY(RfcAttachContext)
#define RfcGetAttachedContext            RFC_API_ENTRY(RfcGetAttachedContext)
#define RfcDetachContext                 RFC_API_ENTRY(RfcDetachContext)
#define RfcDsrInit                       RFC_API_ENTRY(RfcDsrInit)
#define RfcCreateItabObj                 RFC_API_ENTRY(RfcCreateItabObj)  
#define RfcGetMetaDaten4GUI              RFC_API_ENTRY(RfcGetMetaDaten4GUI)
#define RfcSetLeakTrace                  RFC_API_ENTRY(RfcSetLeakTrace)
#define RfcCallReceiveTimed              RFC_API_ENTRY(RfcCallReceiveTimed)
#define RfcConvertA2E                    RFC_API_ENTRY(RfcConvertA2E)
#define RfcConvertE2A                    RFC_API_ENTRY(RfcConvertE2A)
#define RfcSetMaxCpicConn                RFC_API_ENTRY(RfcSetMaxCpicConn)
#define RfcUseSideInfo                   RFC_API_ENTRY(RfcUseSideInfo)
#define RfcResetTraceDir                 RFC_API_ENTRY(RfcResetTraceDir)
#define RfcExceptionInfo                 RFC_API_ENTRY(RfcExceptionInfo)


#if !defined(SAPTYPE_H)
/*
 * Non-SAP RFC Compile: include the SAP U Interface
 */
#include "sapucx.h"
#ifndef __midl
	/* MIDL doesnt need to see the U interface */
	#include "sapuc.h"
#endif
#endif


/* SAPUNICODEOK */
/* @type rfc_char_t | data type used for API parameters
 *
 * characters encoded with UTF16 in unicode builds
 *
 * characters encoded with the default codepage in multibyte builds
 */
typedef SAP_UC	rfc_char_t;

/* Used for constcorrectness on some compilers           */
/* setting SAP_RFC_WITH_CONST = 1 inserts an additional */
/* const to the RFC_TYPE_ELEMENT structures             */
#ifdef SAP_RFC_WITH_CONST
   #define CONST_RFC const
#else
   #define CONST_RFC
#endif


#ifndef sizeofR
#define sizeofR sizeof
#endif


/* SAPUNICODEOK */
/* @type rfc_byte_t |
 * data type used for binary fields. */
typedef unsigned char rfc_byte_t;

/*
 * @enum RFCTYPE |
 *
 * RFC data types.
 */
typedef enum
{
  RFCTYPE_CHAR   = TYPC,         /* @emem 1-byte or multibyte character.
                                  * fixed sized, blank padded
                                  * ( <t RFC_CHAR> ).
			          */
  RFCTYPE_DATE   = TYPDATE,      /* @emem date ( YYYYYMMDD )
                                  * ( <t RFC_DATE> ).
				  */
  RFCTYPE_BCD    = TYPP,         /* @emem packed number.
				  * bcd number, any length between
				  * 1 and 16 bytes
                                  * ( <t RFC_BCD> ).
				  */
  RFCTYPE_TIME   = TYPTIME,      /* @emem time (HHMMSS)
                                  * ( <t RFC_TIME> ).
				  */
  RFCTYPE_BYTE   = TYPX   ,      /* @emem raw data, binary,
				  * fixed length, zero padded.
                                  * ( <t RFC_BYTE> ).
				  */
  RFCTYPE_ITAB   = TYPTABH,      /* @emem internal table
                                  * ( <t ITAB_H> ).
				  */
  RFCTYPE_NUM    = TYPNUM,       /* @emem digits, fixed size,
				  * '0' padded.
                                  * ( <t RFC_NUM> ).
				  */
  RFCTYPE_FLOAT  = TYPFLOAT,     /* @emem floating point,
				  * double precission
                                  * ( <t RFC_FLOAT> ).
				  */
  RFCTYPE_INT    = TYPINT,       /* @emem 4-byte integer
                                  * ( <t RFC_INT> ).
				  */
  RFCTYPE_INT2   = TYPINT2,      /* @emem 2-byte integer
				  * obsolete,
				  * not directly supported
				  * by ABAP/4.
				  */
  RFCTYPE_INT1   = TYPINT1,      /* @emem 1-byte integer
				  * obsolete,
				  * not directly supported
				  * by ABAP/4.
				  */
  RFCTYPE_DATE_1 = TYP1,         /* @emem old fashioned date,
				  * obsolete,
				  * not directly supported
				  * by ABAP/4.
				  */
  RFCTYPE_DATE_2 = TYP2,         /* @emem old fashioned date,
				  * obsolete,
				  * not directly supported
				  * by ABAP/4.
				  */

  /* starting from here ABAP types and RFC types differ */
  _RFCTYPE_differ_from_abap,

  RFCTYPE_NULL                   /* @emem not supported data type. */
  =   _RFCTYPE_differ_from_abap,

  RFCTYPE_WIDE_2,                /* wide character 2 byte (internal use)*/
  RFCTYPE_WIDE_4,                /* wide character 4 byte (internal use)*/
  RFCTYPE_STRUCTURE,             /* a structure (internal use) */

  RFCTYPE_ABAP4OBJECT,           /* abap/4 object (internal use) */

  RFCTYPE_IUNKNOWN,              /* @emem an IUnknown object
				  * can only be used in a COM/OLE
				  * environment.
                                  * ( <t RFC_IUNKNOWN> ).
			 	  */

  RFCTYPE_WSTRING,               /* @emem a wide character string
                                  * zero terminated, if given size
                                  * is big enough.
                                  * ( <t RFC_WSTRING> ).
 	                          */
  RFCTYPE_SAPAUTOMATION,         /* sap automation object
				  * sapgui only, internal use.
				  */
  RFCTYPE_STUB,			 /* remote object's stub
				  * (internal use)
				  */
  RFCTYPE_WIRE_OBJECT,           /* Object on the wire
                                  * (internal use)
                                  */
  RFCTYPE_BLOB,                  /* @emem binary large object,
				  * address must point to a <t RFC_BLOB>
				  * structure.
				  */
  RFCTYPE_CORBA_OBJECT,          /* a Corba object
				  * (internal use only).
				  */
  RFCTYPE_PADDING,               /* @emem use in dummy fields to ensure proper
                                  * alignment of structure with sub structures
 	                          */
  RFCTYPE_UNICODE,               /* @emem Never used, always use RFCTYPE_CHAR.
			          */

  RFCTYPE_XMLDATA,      /* @emem zero terminated string describing ABAP data
                         * types/structure as XML */

  RFCTYPE_STRING,        /* @emem zero terminated string */

  RFCTYPE_XSTRING,      /* @emem raw string with length field in bytes */

  RFCTYPE_DECF16, /* IEEE 754r decimal floating point 8 bytes  */

  RFCTYPE_DECF34, /* IEEE 754r decimal floating point 16 bytes  */

  _RFCTYPE_max_value,     /* the max. value of RFCTYPEs */

  /* some special values */
#if defined(SAPonRM600) || defined (SAPonPTX)
                 		/* cast required incl. CDS++ 1.0B C1016615 */
   RFCTYPE_WCHAR                  /* @emem wide character string,
                                   * blank padded
                                   * ( <t RFC_WCHAR> ).
                                   */
    = ( sizeofR( wchar_t ) == 2 ) ? (int)RFCTYPE_WIDE_2 : (int)RFCTYPE_WIDE_4,
   _RFCTYPE_illegal_wchar         /* not allowed wide character type */
    = ( sizeofR( wchar_t ) == 2 ) ? (int)RFCTYPE_WIDE_4 : (int)RFCTYPE_WIDE_2,
#else
  RFCTYPE_WCHAR                  /* @emem wide character string,
				  * blank padded
                                  * ( <t RFC_WCHAR> ).
				  */
  = (( sizeofR( wchar_t ) == 2u ) ? RFCTYPE_WIDE_2 : RFCTYPE_WIDE_4),

  _RFCTYPE_illegal_wchar         /* not allowed wide character type */
  = ( sizeofR( wchar_t ) == 2u ) ? RFCTYPE_WIDE_4 : RFCTYPE_WIDE_2,
#endif


  _RFCTYPE_the_last_line_without_colon

}
RFCTYPE;

/*
 * @enum RFC_BLOB_CLASS |
 *
 * Memory class of a BLOB.
 */
typedef enum
{
    RFC_BLOB_CLASS_ANY    = 0,  /* @emem any memory class (send only) */
    RFC_BLOB_CLASS_MALLOC = 9   /* @emem object is allocated in memory
                                 * using 'malloc' or 'realloc'. Memory has
                                 * to be released by 'free'.
                                 */
}
RFC_BLOB_CLASS;

/* @type RFC_BLOB |
 * a structure describing a BLOB (binary large object)
 */typedef struct

{
    RFC_BLOB_CLASS blobClass;  /* @field type of blob (must be
				* <e RFC_BLOB_CLASS.RFC_BLOB_CLASS_MALLOC>)
				*/
    RFCTYPE        blobType;   /* @field RFC type of blob object,
                                * must be either
                                * <e RFCTYPE.RFCTYPE_CHAR> or
                                * <e RFCTYPE.RFCTYPE_BYTE>.
                                */
    unsigned long  blobSize;       /* @field size in bytes */
    void         * blobAddress;    /* @field address of buffer */
}
RFC_BLOB;

/* @type RFC_TYPEHANDLE |
 * a value describing an ABAP/4 field. It may describe
 * a scalar field like <t RFC_CHAR> (s. enum <t RFCTYPE> )
 * or a structure. A
 * structure description must be installed by the
 * function <f RfcInstallStructure>.
 *
 * @comm
 * In former versions corresponding fields where simply
 * defined as 'unsigned'. Use <t RFC_TYPEHANDLE> instead.
 */
typedef unsigned RFC_TYPEHANDLE;

/* @type RFC_TYPE_ELEMENT |
 * an element of a structure used in an RFC interface.
 * This type is used to describe a structure, which is
 * to be installed by the
 * function <f RfcInstallStructure>.
 */
typedef struct
{
    CONST_RFC rfc_char_t *   name;     /* @field name of the field
                              * (only used for trace).
                              */
    RFC_TYPEHANDLE type;     /* @field (scalar) data type of the element. */
    unsigned       length;   /* @field length of the field in bytes. */
    unsigned       decimals; /* @field decimals
                              * (only bcd fields, TYPP ).
                              */

    unsigned _not_used;
}
RFC_TYPE_ELEMENT;


/* @type RFC_TYPE_ELEMENT2 |
 * an element of a structure used in an RFC interface.
 * This type is used to describe a structure, which is
 * to be installed by the
 * function <f RfcInstallStructure2>.
 */
typedef struct
{
    CONST_RFC rfc_char_t *   name;     /* @field name of the field
                              * (only used for trace).
                              */
    RFC_TYPEHANDLE type;     /* @field (scalar) data type of the element. */
    unsigned       length;   /* @field length of the field in bytes. */
    unsigned       decimals; /* @field decimals
                              * (only bcd fields, TYPP ).
                              */

    unsigned       offset;   /* @field byte offset of the field from */
                             /* the beginning of the structure */ 	
}
RFC_TYPE_ELEMENT2;


/* @type RFC_UNICODE_TYPE_ELEMENT |
 * an element of a structure used in an RFC interface.
 * This type is used to describe a structure, which is
 * to be installed by the
 * functions <f RfcInstallUnicodeStructure> and <f RfcGetStructureInfoFla>.
 *
 * Only types installed by RfcInstallUnicodeStructure can be used
 * in a UNICODE programm that communicates with a non-Unicode system.
 */
typedef struct
{
    CONST_RFC rfc_char_t *   name;     /* @field name of the field
                              * (only used for trace).
                              */
    RFC_TYPEHANDLE type;     /* @field (scalar) data type of the element. */

    unsigned       decimals; /* @field decimals
                              * (only bcd fields, TYPP ).
                              */

    unsigned       c1_length;/* @field length of the field in bytes
                              * in a 1 byte per SAP_CHAR system. 
                              */
    unsigned       c1_offset;/* @field byte offset of the field from 
                              * the beginning of the structure
                              * in a 1 byte per SAP_CHAR system
                              */ 	

    unsigned       c2_length;/* @field length of the field in bytes
                              * in a 2 bytes per SAP_CHAR system. 
                              */
    unsigned       c2_offset;/* @field byte offset of the field from 
                              * the beginning of the structure
                              * in a 2 bytes per SAP_CHAR system
                              */ 	

    unsigned       c4_length;/* @field length of the field in bytes
                              * in a 4 bytes per SAP_CHAR system. 
                              */
    unsigned       c4_offset;/* @field byte offset of the field from 
                              * the beginning of the structure
                              * in a 4 bytes per SAP_CHAR system
                              */ 	
}
RFC_UNICODE_TYPE_ELEMENT;

typedef void *RFC_TYPE_HEAD ;

/* @type RFC_U_FIELDS|
 * type from the lines of the table returned by 
 * <f RfcGetStructureInfoAsTable>.
 */
typedef struct
{
    SAP_CHAR	Tabname[30];    /* @field name of the structure 
                                 *  for comptibilty with DDIC's RFC_FIELDS */
    SAP_CHAR	Fieldname[30];  /* @field name of the field
                                 */
    SAP_INT	Position;       /* @field field position
                                 *  for comptibilty with DDIC's RFC_FIELDS */
    SAP_CHAR	Exid [1];       /* @field ABAP internal type representation
                                 * <f RfcExidToRfcType>.*/
    SAP_INT	Decimals;       /* @field decimals 
                                 *(only bcd fields, RFCTYPE_BCD ).*/

    SAP_INT	Offset_b1;      /* @field field offset in systems with 1 byte per SAP_CHAR */
    SAP_INT	Length_b1;      /* @field field length in systems with 1 byte per SAP_CHAR.
                                 * For all sytems, this is the number of SAP_CHAR from the
                                 * field when the fields is of type "Character", "Numeric",
                                 * "Date" or "Time".
                                 */

    SAP_INT	Offset_b2;      /* @field field offset in systems with 2 bytes per SAP_CHAR */
    SAP_INT	Length_b2;      /* @field field length in systems with 2 bytes per SAP_CHAR */

    SAP_INT	Offset_b4;      /* @field field offset in systems with 4 bytes per SAP_CHAR */
    SAP_INT	Length_b4;      /* @field field length in systems with 4 bytes per SAP_CHAR */

}
RFC_U_FIELDS, *PRFC_U_FIELDS;
typedef const RFC_U_FIELDS *PCRFC_U_FIELDS;

/* @type RFC_U_FUNINT|
 * type from the lines of the table returned by 
 * <f RfcGetFunctionInfoAsTable>.
 */
typedef struct RFC_U_FUNINT
{
    SAP_CHAR    Paramclass [1];  /* @field parameter type :
                                  * I - Importing parameter
                                  * E - Exporting parameter
                                  * C - Chaning parameter
                                  * T - Table parameter
                                  * X - eXception
                                  */
    SAP_CHAR    Parameter [30];  /* @field Parameter name, exception value*/
    SAP_CHAR    Tabname [30];    /* @field Base structure defining the parameter type*/
    SAP_CHAR    Fieldname [30];  /* @field Field defining the parameter type*/
    SAP_CHAR    Exid [1];        /* @field Parameter type*/
    SAP_INT     Position;        /* @field Position*/    
    SAP_INT	Offset_b1;       /* @field field offset in systems with 1 byte per SAP_CHAR */
    SAP_INT	Length_b1;       /* @field field length in systems with 1 byte per SAP_CHAR.
                                  * For all sytems, this is the number of SAP_CHAR from the
                                  * field when the fields is of type "Character", "Numeric",
                                  * "Date" or "Time".
                                  */

    SAP_INT	Offset_b2;       /* @field field offset in systems with 2 bytes per SAP_CHAR */
    SAP_INT	Length_b2;       /* @field field length in systems with 2 bytes per SAP_CHAR */

    SAP_INT	Offset_b4;       /* @field field offset in systems with 4 bytes per SAP_CHAR */
    SAP_INT	Length_b4;       /* @field field length in systems with 4 bytes per SAP_CHAR */
    SAP_INT     Decimals;        /* @field Number of decimals 
                                  *(only bcd fields, RFCTYPE_BCD ).*/
    SAP_CHAR    Default [21];    /* @field Default value from optional parameter
                                  * In some cases this is an ABAP variable such as SY-LANGU
                                  */
    SAP_CHAR    Paramtext [79];  /* @field Short description from parameter*/
    SAP_CHAR    Optional [1];    /* @field Indicate optional parameters*/
}
RFC_U_FUNINT, *PRFC_U_FUNINT;
typedef const RFC_U_FUNINT *PCRFC_U_FUNINT;



/* RFC scalar data types */

/* @type RFC_CHAR |
 * ABAP/4 data type C,
 * blank padded character string of fixed length.
 * Used for data transported by RFC
 *
 * characters are encoded in UTF16 in unicode builds
 *
 * characters are encoded in the system default code page
 * or in the code page selected by <f RfcSetCodePage>, <f RfcSetSystemCodePage>,
 * <f RfcOpenEx> or defined by environment variable SAP_CODEPAGE
 * in multibyte builds. The default codepage is 1100.
 */
/* @type RFC_UNICODE |
 * ABAP/4 data type C,
 * blank padded character string of fixed length in unicode environment.
 */
/* @type RFC_NUM |
 * ABAP/4 data type N,
 * character string of fixed length containing only digits.
 */
/* @type RFC_BYTE |
 * ABAP/4 data type X,
 * raw data.
 */
/* @type RFC_BCD |
 * ABAP/4 data type P,
 * packed number in BCD format (binary coded decimal).
 */
/* @type RFC_INT1 |
 * Data dictionary data type INT1,
 * 1-byte unsigned integer.
 */
/* @type RFC_INT2 |
 * Data dictionary data type INT2,
 * 2-byte signed integer.
 */
/* @type RFC_INT |
 * ABAP/4 data type I,
 * 4-byte signed integer.
 */
/* @type RFC_FLOAT |
 * ABAP/4 data type F,
 * 8-byte IEEE floating point number.
 */
/* @type RFC_DATE |
 * ABAP/4 data type D,
 * 8-byte date (YYYYMMDD).
 */
/* @type RFC_TIME |
 * ABAP/4 data type T,
 * 6-byte time (HHMMSS).
 */
/* @type RFC_WCHAR |
 * ABAP/4 data type W,
 * double-byte, unicode string,
 * blank padded.
 */
/* @type RFC_WSTRING |
 * similiar than ABAP/4 data type W.
 *
 * Double-byte, unicode string,
 * zero terminated
 * (if the given length
 * in <t RFC_PARAMETER>.
 * is big enough )
 *
 * Can be used instead of <t RFC_WCHAR>, if more appropriate.
 */
/* @type RFC_STRING |
 * similar than ABAP/4 data type string with variable length.
 * The difference is RFC_STRING is a zero terminated string and ABAP/4 string
 * is a blank padded string. For better understanding you can consider RFC_STRING
 * like  typedef unsigned char* RFC_STRING.
 * The conversion between zero terminated
 * (extern RFC program) and blank padded (ABAP) is done automatically.
 * All strings are to be handled by sending and receiving in external programs
 * as UTF8 zero terminated strings.
 *
 */
/* @type RFC_XSTRING |
 * raw string with length field in bytes.
 */
/* @type RFC_IUNKNOWN |
 * COM object.
 * Can be mapped to ABAP/4 objects and vice versa.
 * (Available, only if COM adapter is installed)
 */
#ifdef RFC_CHAR_UNSIGNED
/* use RFC_CHAR_UNSIGNED if you have legacy code                */
typedef unsigned char  RFC_CHAR;     /* characters, TYPC        */
#else
typedef SAP_UC         RFC_CHAR;     /* characters, TYPC        */
#endif
typedef RFC_CHAR       RFC_NUM;      /* digits, TYPNUM          */
typedef rfc_byte_t     RFC_BYTE;     /* raw data, TYPX          */
typedef rfc_byte_t     RFC_BCD;      /* packed numbers, TYPP    */
typedef rfc_byte_t     RFC_INT1;     /* 1 byte integer, TYPINT1 */
typedef short          RFC_INT2;     /* 2 byte integer, TYPINT2 */
typedef double         RFC_FLOAT;    /* floating point, TYPFLOAT*/
typedef RFC_CHAR       RFC_DATE[8];  /* date, TYPDATE (YYYYMMDD)*/
typedef RFC_CHAR       RFC_TIME[6];  /* time, TYPTIME (HHMMSS)  */
typedef wchar_t        RFC_WCHAR;    /* wchar_t                 */
typedef wchar_t    *   RFC_WSTRING;  /* wchar_t, zero term.     */
typedef SAP_UTF8   *   RFC_XMLDATA;  /* ABAP structs encoded as XML Document */
typedef SAP_UTF8   *   RFC_STRING;   /* zero terminated string in UTF8-Format */
typedef DecFloat16     RFC_DECF16;   /* IEEE 754r decimal floating point 8 bytes  */
typedef DecFloat34     RFC_DECF34;   /* IEEE 754r decimal floating point 16 bytes  */


#ifdef _OBJBASE_H_
/* only if com header is included */
typedef IUnknown *     RFC_IUNKNOWN; /* IUnknown, COM object    */
#endif

#ifdef __midl
 typedef long int      RFC_INT;
#else

#define RFC_INT_MAX 0x7fffffff
#if INT_MAX == RFC_INT_MAX
 typedef int           RFC_INT;     /* 4 byte integers, TYPINT  */
#elif LONG_MAX == RFC_INT_MAX
 typedef long int      RFC_INT;     /* 4 byte integers, TYPINT  */
#else
#error local integer representation is not supported by RFC API
#endif
#endif /* __midl or C */

typedef struct
{
 RFC_INT            length;
 RFC_BYTE          *content;
} RFC_XSTRING; /* binary string */



#ifdef __cplusplus
extern "C"
{
#endif


/* Initialization */
DECL_EXP void SAP_API RfcInit (void);

/* Environment : memory allocation and error handling */

typedef void* (DLL_CALL_BACK_FUNCTION_PTR RFC_ALLOCATE)
                      ( void * old_ptr,
                        unsigned long size );
typedef void  (DLL_CALL_BACK_FUNCTION_PTR RFC_ERROR_HANDLER)
                      ( CONST_RFC rfc_char_t * error_id );

typedef struct
{
   RFC_ERROR_HANDLER errorhandler;
   RFC_ALLOCATE      allocate;
}
RFC_ENV;

DECL_EXP void SAP_API RfcEnvironment( RFC_ENV * new_env );


 /* @struct RFC_ERROR_INFO |
 *
 *
 * !!! Please use <t RFC_ERROR_INFO_EX> and <f RfcLastErrorEx>
 *     instead of this structure.
 *
 *
 * structure returned by <f RfcLastError> describing
 * the last RFC error that occurred.
 */
typedef struct
#ifdef SAPonOS400
/* to get proj/crfc to compile (they use this struct as a parent class) */
RFC_ERROR_INFO
#endif
{
    RFC_CHAR  key[33];       /* @field error code to identify
                               * the error */
    RFC_CHAR status[128];   /* @field state of the RFC
                               * connection
                               */
    RFC_CHAR message[256];  /* @field text describing the error */
    RFC_CHAR intstat[128];  /* @field internal description of the
                               * RFC connection
                               */
}
RFC_ERROR_INFO;

/*------------------------------------------------------
 * @func
 *
 *
 * !!! Please use <f RfcLastErrorEx> instead of this function.
 *
 *
 * RfcLastError describes the last error reported by
 * some function of the RFC API.
 *
 * Multiple calls of RfcLastError are possible and yield the same error until a new error occurs. 
 *
 * @rdesc
 * returns 1 if no error occurred and 0 elsewhere. 
 *
 * @xref <f RfcLastErrorEx>
 *
 *------------------------------------------------------*/
DECL_EXP int SAP_API RfcLastError(
  RFC_ERROR_INFO * error_info  /* @parm structure
                                * <t RFC_ERROR_INFO> describing the
                                * error.
                                */
                        );


/* @enum RFC_ERROR_GROUP | RFC error group */
typedef enum
{
   RFC_ERROR_PROGRAM               = 101, /* @emem Error in RFC program
                                           */
   RFC_ERROR_COMMUNICATION         = 102, /* @emem Error in Network & Communications
                                           */
   RFC_ERROR_LOGON_FAILURE         = 103, /* @emem SAP logon error
                                           */
   RFC_ERROR_SYSTEM_FAILURE        = 104, /* @emem e.g. SAP system exception raised
                                           */
   RFC_ERROR_APPLICATION_EXCEPTION = 105, /* @emem The called function module raised
                                           * an exception
                                           */
   RFC_ERROR_RESOURCE              = 106, /* @emem Resource not available
                                           * (e.g. memory insufficient,...)
                                           */
   RFC_ERROR_PROTOCOL              = 107, /* @emem RFC Protocol error
                                           */
   RFC_ERROR_INTERNAL              = 108, /* @emem RFC Internal error
                                           */
   RFC_ERROR_CANCELLED             = 109, /* @emem RFC Registered Server was
                                           * cancelled
                                           */
   RFC_ERROR_BUSY                  = 110  /* @emem System is busy, try later
                                           */
}
RFC_ERROR_GROUP;



typedef struct
{
   RFC_CHAR   msgid[21];
   RFC_CHAR   msgty;
   RFC_CHAR   msgno[4];
   RFC_CHAR   msgv1[51];
   RFC_CHAR   msgv2[51];
   RFC_CHAR   msgv3[51];
   RFC_CHAR   msgv4[51];
}  RFC_EXCEPTION_INFO;


int SAP_API RfcExceptionInfo
(
   RFC_EXCEPTION_INFO  *info
);


 /* @struct RFC_ERROR_INFO_EX |
 *
 * structure returned by <f RfcLastErrorEx> describing
 * the last RFC error.
 */
typedef struct
#ifdef SAPonOS400
/* to get proj/crfc to compile (they use this struct as a parent class) */
RFC_ERROR_INFO_EX
#endif
{
    RFC_ERROR_GROUP  group;         /* @field error group <t RFC_ERROR_GROUP> */
    RFC_CHAR       key[33];       /* @field error code to identify the error.
				     * The error group (type integer) and error key
				     * (type string) have the same meaning.
				     * RFC program can better analyze the error
				     * with the error group instead of error key.
				     */
    RFC_CHAR       message[513];  /* @field error text to describe the error */
}
RFC_ERROR_INFO_EX;


/*------------------------------------------------------
 * @func
 *
 * RfcLastErrorEx describes the last error reported by
 * some function of the RFC API.
 *
 * Multiple calls of RfcLastErrorEx are possible and yield the same error until a new error occurs. 
 *
 * @rdesc  Returns one of the following values:
 *
 * returns 1 if no error occurred and 0 elsewhere. 
 *
 * @comm 
 * Do not call this API after call contains
 * <t RFC_ERROR_INFO_EX> structure in the parameter list.
 *
 * In this case 
 *
 * - RfcLastErrorEx returns 1 although previous call failed.
 *
 * - please retrieve the error information from the parameter.
 *
 * An example for API contains <t RFC_ERROR_INFO_EX> in the
 * parameter list is <f RfcOpenEx>.
 *------------------------------------------------------
 */
DECL_EXP int SAP_API RfcLastErrorEx 
(
 RFC_ERROR_INFO_EX * error_info  /* @parm structure
                                   * <t RFC_ERROR_INFO_EX> describing the
                                   * error.
                                   */
);



/* @type RFC_HANDLE | handle for RFC connection
 * @flag RFC_HANDLE_NULL | the RFC handle is not set
 */
typedef unsigned RFC_HANDLE;

/* Null-Value : handle not set */
#define RFC_HANDLE_NULL   0

 /* @struct RFC_ATTRIBUTES |
  * structure returned by <f RfcGetAttributes> describing
  * some information about this RFC connection.
  */
typedef struct
{
    rfc_char_t dest[64+1];              /* @field RFC destination              */
    rfc_char_t own_host[100+1];         /* @field Own host name                */
    rfc_char_t partner_host[100+1];     /* @field Partner host name            */
    rfc_char_t systnr[2+1];             /* @field R/3 system number            */
    rfc_char_t sysid[8+1];              /* @field R/3 system name              */
    rfc_char_t client[3+1];             /* @field Client                       */
    rfc_char_t user[12+1];              /* @field User                         */
    rfc_char_t language[1+1];           /* @field Language                     */
    rfc_char_t trace;                   /* @field ON/OFF: 'X'/' '              */
    rfc_char_t ISO_language[2+1];       /* @field 2-byte ISO-Language          */
    rfc_char_t own_codepage[4+1];       /* @field Own code page                */
    rfc_char_t partner_codepage[4+1];   /* @field Partner code page            */
    rfc_char_t rfc_role;                /* @field C/S: RFC Client / RFC Server */
    rfc_char_t own_type;                /* @field 2/3/E/R: R/2,R/3,Ext,Reg.Ext */
    rfc_char_t own_rel[4+1];            /* @field My system release            */
    rfc_char_t partner_type;            /* @field 2/3/E/R: R/2,R/3,Ext,Reg.Ext */
    rfc_char_t partner_rel[4+1];        /* @field Partner system release       */
    rfc_char_t kernel_rel[4+1];         /* @field Partner kernel release       */
    rfc_char_t CPIC_convid[9];          /* @field CPI-C Conversation ID        */
    rfc_char_t password_sate;           /* @field state from the password      */
                                        /*        see RFC_PASS_*               */
    rfc_char_t own_codepage_pcs[4+1];   /* @field Own codepage in partners 
                                         *        point of view.          
                                         *        Differs from own_codepage 
                                         *        field only if UNICODE and 
                                         *        pcs flag are used 
                                         *        (see <f RfcOpenEx>).
                                         *        Elsewhere this field is 
                                         *        identically with 
                                         *        own_codepage field.
                                         */
    rfc_char_t pcs[1+1];                /* @field Partners Char Size (valid only in Unicode systems) 
                                         * 1/2:  Non Unicode/Unicode
                                         * This information is only valid after succesfull call to the target system.
                                         */
    rfc_char_t real_partner_codepage[4+1];   /* @field Partner real code page.
                                         * This information is valid only after succesfulll call
                                         */
    rfc_char_t progname[40+1];          /* @field Name of the calling program
                                         */
    rfc_char_t reserved[161];           /* @field reserve                       */
}
RFC_ATTRIBUTES;

/* @type RFC_CODE_PAGE | RFC code page number
 *
 * SAP name of a code page.  Use the transaction spad or scp 
 * for a complete list:
 *
 *<nl>1100 SAP internal, like ISO 8859-1         
 *<nl>1103 IBM PC Multilingual 850 (00697/00850)
 *<nl>1127 APPLE 8 Bit ASCII
 *<nl>1160 Microsoft 1252, Superset of ISO 8859-1
 *<nl>1401 SAP internal, like ISO 8859-2
 *<nl>1404 Microsoft Windows 1250
 *<nl>1500 SAP-internal, like ISO 8859-5
 *<nl>1504 Microsoft Windows 1251
 *<nl>1610 SAP-internal, like ISO 8859-9 (Latin-5)
 *<nl>1614 MS Windows 1254
 *<nl>1700 SAP-internal, like ISO 8859-7 (Greek)
 *<nl>1703 IBM PC Code Page 869 Greece
 *<nl>1704 MS-Windows 1253
 *<nl>1800 SAP-internal, like ISO 8859-8 (Hebrew)
 *<nl>8000 Shift JIS
 *<nl>8300 Traditional Chinese
 *<nl>8400 Simplified Chinese
 *<nl>8500 Korean
 *<nl>
 *<nl>0100 IBM EBCDIC Codepage 00697/00273
 *<nl>0120 EBCDIC 0697/0500 Latin-1, Multilangual
 *<nl>0410 EBCDIC 0959/0870 Latin-2, Multilingual
 *<nl>0500 EBCDIC 1150/1025 Cyrillic, Multilingual
 *<nl>0610 EBCDIC 1152/1026 Latin-5, Turkey
 *<nl>0700 EBCDIC 0925/0875 Greece
 *<nl>0800 EBCDIC 0941/0424 Hebrew
 *<nl>
 *<nl>4100 ISO/IEC DIS 10646-1.2 Unicode UCS-2 big-endian
 *<nl>4101 ISO/IEC DIS 10646-1.2 Unicode UCS-2 little-endian
 *<nl>4102 ISO/IEC DIS 10646-1.2 Unicode UTF-16BE
 *<nl>4103 ISO/IEC DIS 10646-1.2 Unicode UTF-16LE
 *<nl>4110 Unicode UTF-8
 *
 */
typedef rfc_char_t RFC_CODE_PAGE[4+1] ;


/*
 * @enum RFC_MODE |
 *
 * Type of connection to be opened by <f RfcOpen>.
 */
typedef enum
{
  RFC_MODE_R3ONLY = 0,      /* @emem Use R/3 protocol and addressing scheme.
                             * Only for R/3 systems. Any kind
                             * of user id (dialog user id, CPI-C user id)
                             * is possible.
                             *
                             * <e RFC_OPTIONS.connopt> must point to a structure
                             * of type <t RFC_CONNOPT_R3ONLY>.
                             *
                             */
  RFC_MODE_CPIC = 1,        /* @emem Use R/2 protocol and addressing scheme.
                             * Must be used for R/2,
                             * cpic user id are only allowed.
                             * (Since an R/3 system is also capable of
                             * understanding the R/2 RFC protocol, you can
                             * reach also an R/3 system with that mode,
                             * you must use a 'sideinfo' file for addressing,
                             * however.
                             *
                             * <e RFC_OPTIONS.connopt> must point to a structure
                             * of type <t RFC_CONNOPT_CPIC>.
                             */
  RFC_MODE_VERSION_3 = 3,   /* @emem Use R/3 protocol Version 3.
                             * The receiving SAP system must have at least
                             * Rel. 3.0C to be able to serve every kind
                             * of options.
                             *
                             * <e RFC_OPTIONS.connopt> must point to a structure
                             * of type <t RFC_CONNOPT_VERSION_3>.
                             */
  RFC_MODE_PARAMETER = 4,   /* @emem Use R/3 protocol Version 3 or R/2 protocol and
                             * addressing scheme.
                             * This mode includes all other modes above and all necessary
                             * parameters will be read from a INI-file (saprfc.ini).
                             * See saprfc.ini for more details
                             *
                             * In some cases the receiving SAP system must have at least
                             * Rel. 3.0C to be able to serve every kind of options.
                             *
                             * <e RFC_OPTIONS.destination> must point to a valid entry
                             * in the saprfc.ini. This file can be in the current directory
                             * where RFC programs are running or defined by the Environment
                             * Variable RFC_INI (e.g. /usr/rfctest/saprfc.ini).
                             *
                             * <e RFC_OPTIONS.connopt> must be set to NULL.
                             */
  RFC_MODE_CPIC_EXT = 5     /* @emem Use R/2 protocol and addressing scheme
                             * for communication between two external programs
                             * without using the 'sideinfo' or the 'saprfc.ini' file.
                             *
                             * <e RFC_OPTIONS.connopt> must point to a structure
                             * of type <t RFC_CONNOPT_CPIC_EXT>.
                             */
}
RFC_MODE;


/* internal use */
#define RFC_OPEN_USE_RFC       'R'
#define RFC_OPEN_USE_CPIC      ' '

/*
 * @enum RFC_SNC_MODE |
 *
 * SNC mode of an RFC connection: ON or OFF
 *
 * The default value is RFC_SNC_MODE_OFF.
 *
 */
typedef enum
{
  RFC_SNC_MODE_OFF = 0,     /* @emem Working without SNC. */

  RFC_SNC_MODE_ON  = 1      /* @emem Working with SNC.    */
}
RFC_SNC_MODE;

/*
 * @enum RFC_SNC_QOP |
 *
 * SNC Quality of service (s. SNC document for more details)
 *
 * The default value is RFC_SNC_QOP_DEFAULT.
 *
 */
typedef enum
{
  RFC_SNC_QOP_INVALID   = 0,    /* @emem plain text, but with authorization */
  RFC_SNC_QOP_OPEN      = 1,    /* @emem plain text, but with authorization */
  RFC_SNC_QOP_INTEGRITY = 2,    /* @emem each packet is integrity protected */
  RFC_SNC_QOP_PRIVACY   = 3,    /* @emem each packet is privacy protected   */

  RFC_SNC_QOP_DEFAULT   = 8,    /* @emem use global configuration defaults  */
  RFC_SNC_QOP_MAX       = 9     /* @emem use maximum available security     */
}
RFC_SNC_QOP;


/* @struct RFC_EXT_DATA |
 * includes extended parameters for opening an RFC connection
 * such as SNC parameters (SNC: Secure Network Communications)
 *
 * If working with SNC and not using the 'saprfc.ini' file,
 * all SNC parameters must be set in this structure.
 */
typedef struct
{
  RFC_SNC_MODE   snc_mode;            /* @field SNC mode for working
                                       * with or without SNC.
                                       */
  RFC_SNC_QOP    snc_qop;             /* @field SNC Quality of service
                                       */
  rfc_char_t         * snc_myname;          /* @field Own SNC name if you don't want
				       * to use the default SNC name
                                       */
  rfc_char_t         * snc_partnername;     /* @field SNC name of partner system
                                       *
                                       */
  rfc_char_t         * snc_lib;             /* @field name of the SNC library
                                       * on the local system
                                       */
  rfc_char_t          xmlflag[2];

  rfc_char_t          reserve[254];        /* @field reserve, must be set to 0
                                       *
                                       */
}
RFC_EXT_DATA;


/* @struct RFC_CONNOPT_CPIC |
 * Options for an R/2 connection (via SAP gateway).
 * connection data must be specified at the SAP gateway,
 * 'destination' is used as key into the 'sideinfo' file there.
 */
typedef struct
{
   rfc_char_t *   gateway_host;       /* @field gateway hostname
                                 */
   rfc_char_t *   gateway_service;    /* @field gateway service
                                 * (in general sapgw00).
                                 */
}
RFC_CONNOPT_CPIC;


/* @struct RFC_CONNOPT_CPIC_EXT |
 * Options for an RFC connection (via SAP gateway)
 * between two external programs without using the 'sideinfo'
 * or the 'saprfc.ini' file.
 */
typedef struct
{
   int      register_mode;      /* @field register mode:
                                 *  0: Server program is to be started by SAP gateway.
                                 *  1: Server program is running in register mode.
                                 */
   rfc_char_t *   gateway_host;       /* @field gateway hostname
                                 */
   rfc_char_t *   gateway_service;    /* @field gateway service
                                 */
   rfc_char_t *   tp_host;            /* @field host name of the partner program
                                 * not needed if register_mode is 1.
                                 */
   rfc_char_t *   tp_name;            /* @field name or program ID of the partner program
                                 * if the parameter 'register_mode' is set to 1.
                                 */
   void * ext_data;             /* @field data for working with extended features
                                 * such as SNC (Secure Network Communications).
                                 * 'ext_data' must be set to NULL or point to a
                                 * structure of type <t RFC_EXT_DATA>.
                                 */
}
RFC_CONNOPT_CPIC_EXT;


/* @struct RFC_CONNOPT_R3ONLY |
 * Options for a connection to an R/3 system.
 */
typedef struct
{
   rfc_char_t *   hostname;           /* @field Host name of target system.
                                 * Host names can be regular host
                                 * names
                                 * defined in a 'hosts' file,
                                 * an IP address like
                                 * 123.123.123.123 or a saprouter
                                 * address as
                                 * /H/hostname/S/port/H/host/S/port/...
                                 */
   int      sysnr;              /* @field system number (0-99).
                                 * This is the number by which
                                 * the target system
                                 * is identified. In general this is 0.
                                 */
   rfc_char_t *   gateway_host;       /* @field gateway hostname.
                                 * If the pointer is NULL, the gateway is
                                 * assumed to run at 'hostname'.
                                 */
   rfc_char_t *   gateway_service;    /* @field gateway service.
                                 * If the pointer is NULL,
                                 * the service "sapgw##" with
                                 * ## = 'sysnr' is assumed.
                                 */
}
RFC_CONNOPT_R3ONLY;


/*--------------------------------------------
 * @struct RFC_CONNOPT_VERSION_3 |
 * Options for a connection to a R/3 system.
 * The target system must be of release 3.0C or later.
 *
 * Since a R/3 system of version 3.0 always has it's
 * own 'gateway' process, no gateway options are necessary
 * any more.
 *-----------------------------------------------------------*/
typedef struct
{
   rfc_char_t *   hostname;           /* @field Host name of target system.
                                 * Host names can be regular host
                                 * names
                                 * defined in a 'hosts' file,
                                 * an IP address like
                                 * 123.123.123.123 or a saprouter
                                 * address as
                                 * /H/hostname/S/port/H/host/S/port/...
                                 *
                                 * If 'use_load_balancing' is set to a non-zero
                                 * value, this field can be NULL.
                                 *
                                 */
   RFC_INT  sysnr;              /* @field system number (0-99).
                                 * This is the number by which
                                 * the target system
                                 * is identified. In general this is 0.
                                 *
                                 * If 'use_load_balancing' is set to a non-zero
                                 * value, this field is ignored.
                                 *
                                 */
   RFC_INT use_load_balancing;  /* @field Use the load balancing features
                                 * of a SAP system.
                                 *
                                 * If you set this to a non-zero value,
                                 * 'hostname' and 'sysnr' are ignored.
                                 *
                                 * You must set the fields 'lb_host'
                                 * and 'lb_system_name' instead.
                                 *
                                 * The target system is determined dynamically
                                 * then.
                                 *
                                 */
   rfc_char_t *  lb_host;             /* @field Host name where the 'message server'
                                 * of the target system is running.
                                 *
                                 * This field must only be filled, if
                                 * 'use_load_balancing' is set to a non-zero
                                 * value.
                                 */
   rfc_char_t *  lb_system_name;      /* @field Name of the target system (C11 i.e.)
                                 *
                                 * This field must only be filled, if
                                 * 'use_load_balancing' is set to a non-zero
                                 * value.
                                 */
   rfc_char_t *  lb_group;            /* @field application server group.
                                 *
                                 * The group of application servers to be used.
                                 *
                                 * This field must only be filled, if
                                 * 'use_load_balancing' is set to a non-zero
                                 * value.
                                 */
   RFC_INT use_sapgui;          /* @field use 'sapgui' process to
                                 * display SAP dynpros and graphics.
                                 *
                                 * Set this to 1 to activate
                                 * this functionality.
                                 *
                                 * Set this to 2 if you want to let SAPGUI
                                 * automatically invisible after each RFC function.
                                 * This functionality requires R/3, Rel. >= 3.0F
                                 * and only available with SAPGUI, Rel. >= 3.0E
                                 * on Windows NT or 95.
                                 *
                                 * Starting sapgui takes some time, so
                                 * you should not set that value, if you do not
                                 * need it.
                                 *
                                 * This field is set implicitly as soon as
                                 * you activate the ABAP/4 debugger by
                                 * entering 'D' in the <e RFC_OPTIONS.trace> field or
                                 * by setting RFC_DEBUG in the system environment.
                                 *
                                 */
    void * ext_data;            /* @field data for working with extended features
                                 * such as SNC (Secure Network Communications).
                                 * 'ext_data' must be set to NULL or point to a
                                 * structure of type <t RFC_EXT_DATA>.
                                 */
}
RFC_CONNOPT_VERSION_3;


/*----------------------------------------------------------------------------
 * @struct RFC_OPTIONS |
 * parameters for <f RfcOpen>.
 *
 * Depending on the type of connection various data have to be supplied
 * to open a RFC connection.
 *
 * There are 3 ways to supply this information:
 *
 * 1. You can enter a destination name pointed to an entry in a 'saprfc.ini'
 *    file which contains the necessary network parameters and RFC specific
 *    parameters for opening the connection at <f RfcOpen>.
 *
 * 2. You can enter a destination name pointed to an entry in a 'sideinfo'
 *    file which only contains the necessary network parameters for opening
 *    the connection at <f RfcOpen>.
 *
 * 3. You supply in your program all the data needed for opening the
 *    connection at <f RfcOpen>.
 *
 *
 *
 * We recommend the first one (working with saprfc.ini) because you can use some
 * RFC features today and in the future without changes in your RFC programs.
 *
 * See saprfc.ini for more details.
 *
 *
 * Fields that are not supplied must be set to NULL.
 *
 * The structure consists of 3 groups.
 *
 * The data needed to establish the physical connection depend on
 * the type of connection (R/2 or R/3 connection, version , ... ).
 * Depending on the contents of the field 'mode', the field 'connopt'
 * must point to different structures.
 *
 * The sign on data ( client, user, password, language ) are needed
 * for authentication. Since <f RfcOpen> only opens the physical connection
 * directly. these data are only checked if the first RFC call is
 * send.
 *
 * The third field contains a trace flag. If not zero, a trace file
 * is written for all the operations corresponding to this connection
 * to ease error analysis.
 *
 * Please make sure, that this structure is completely initialized
 * with 0 to allow adding more connection parameters in the future
 * by SAP.
 *
 * Please make sure you are using language field in SAP format (like E, F, S)
 * If you are using language in ISO-Format (like EN, DE, FR, ES) only first char 
 * will be interpreted. The rest will be truncated without any error or warning.
 *
 * @ex Options for a R/3 connection. |
 *
 * // static = initialized structures
 * static RFC_OPTIONS        options;
 * static RFC_CONNOPT_R3ONLY rfc_connopt_r3only;
 * RFC_HANDLE                handle;
 *
 * options.destination = "TEST";
 * options.mode        = RFC_MODE_R3ONLY;
 * options.client      = "000";
 * options.user        = "SAP*";
 * options.language    = "E";
 * options.password    = "PASS";
 * options.trace       = 0;                    // turn trace off
 *
 * options.connopt     = &rfc_connopt_r3only;
 *
 * rfc_connopt_r3only.hostname = "some_host";  // some host name
 * rfc_connopt_r3only.sysnr    = 0;            // system 00
 *
 * handle = RfcOpen( &options );
 * if ( handle == RFC_HANDLE_NULL )
 * {
 *    .....
 *
 *--------------------------------------------------------------------------------*/
typedef struct
{
   rfc_char_t *   destination;        /* @field name of destination.
                                 * If the connection is not described
                                 * completely, this name is used as a
                                 * key for a 'sideinfo', where the
                                 * connection should be described then.
                                 * You always have to fill this field.
                                 */

   /* connection data */
   RFC_MODE mode;               /* @field connection mode.
                                 * There are two different protocol
                                 * types for RFC, depending on whether
                                 * your target system is a R/2 or a R/3
                                 * or an external system.
                                 *
                                 * If your target system is a SAP system
                                 * of Release 3.0C or later you can use
                                 * various special options, if you enter
                                 * the value <e RFC_MODE.RFC_MODE_VERSION_3>
                                 * here.
                                 *
                                 * Depending on the content of
                                 * this field, connopt must point to
                                 * different structures.
                                 * (s. <t RFC_MODE>).
                                 */
   void *   connopt;            /* @field If connopt is NULL, the 'sideinfo'
                                 * or the 'saprfc.ini' file is used to determine the
                                 * the connection parameters.
                                 *
                                 * Without 'sideinfo' file
                                 * 'connopt' must point to a structure
                                 * of type <t RFC_CONNOPT_R3ONLY>,
                                 * <t RFC_CONNOPT_VERSION_3> or
                                 * <t RFC_CONNOPT_CPIC_EXT> or
                                 * <t RFC_CONNOPT_CPIC> depending
                                 * on the value of 'mode'.
                                 *
                                 */

   /* sign on data */
   rfc_char_t *   client;             /* @field signon data: client   */
   rfc_char_t *   user;               /* @field signon data: user id  */
   rfc_char_t *   password;           /* @field signon data: password */
   rfc_char_t *   language;           /* @field signon data: language in SAP format */

   /* options */
   int      trace;              /* @field trace.
                                 * If 0, no trace is written,
                                 * if not 0, the RFC library traces
                                 * all activities into a file 'dev_rfc'
                                 * in the actual working directory.
                                 *
                                 * If your target system is of release 3.0C
                                 * or later, you can enter the value 'D' here
                                 * to start the ABAP/4 debugger on the target
                                 * system.
                                 *
                                 * The ABAP/4 debugger can also be activated
                                 * by setting the environment variable
                                 * RFC_DEBUG before the call to <f RfcOpen>
                                 * is done.
                                 */
}
RFC_OPTIONS;


#define RFC_HOSTNAME_LN        100
typedef rfc_char_t RFC_HOSTNAME[RFC_HOSTNAME_LN+1];



/*------------------------------------------------------------------
 * @func
 *
 *
 * =====================================================
 *
 *   Please use <f RfcOpenEx> instead of this function
 *
 * =====================================================
 *
 *
 * opens a RFC connection via LOAD BALANCING
 *
 * (only available for R/3 Release 3.0 onwards).
 *
 * With this function the RFC library will try to connect to an
 * application server with the least load (LOAD BALANCING principle)
 * within a group of predefined application servers.
 *
 * This function has following advantages:
 *
 * 1. The load in R/3 system is distributed to different application server
 *
 * 2. RFC connections are thus independent of a specific application server.
 *    (With <f RfcOpen> or <f RfcOpenExt> a RFC connection could only be established
 *    to a predefined application server).
 *
 * 3. Only the host name of the message server and its port number of the
 *    according R/3 are required in the host file and the services file.
 *
 *    Information about SAP gateway, application server, system number,... as
 *    parameter for <f RfcOpen>, <f RfcOpenExt> or as parameter in sideinfo are no longer
 *    necessary. Even the sideinfo is no longer required.
 *
 *
 * @rdesc
 * returns a handle to the RFC connection (<t RFC_HANDLE>) or
 * RFC_HANDLE_NULL, if the connection cannot be opened.
 *
 * @xref <f RfcOpenEx>
 *---------------------------------------------------------------------------------*/
RFC_HANDLE SAP_API RfcConnect
(
 rfc_char_t *       system_name, /* @parm name of the R/3 system */
 rfc_char_t *       ms_hostname, /* @parm hostname of the message server */
 rfc_char_t *       group_name,  /* @parm name of a specific group of application servers */
 rfc_char_t *       client,      /* @parm signon data: client */
 rfc_char_t *       user,        /* @parm signon data: user id */
 rfc_char_t *       password,    /* @parm signon data: password */
 rfc_char_t *       language,    /* @parm signon data: language */
 int          trace,			 /* @parm trace ( 0 / 1 ). */
 RFC_HOSTNAME as_hostname,		 /* @parm name of the connected App. Server (Output parm.) */
 int  *       sysnr				 /* @parm system number of the connected R/3 system (Output parm.) */
);





/*---------------------------------------------------------------------------------------
 *
 * @func
 *
 *
 * =====================================================
 *
 *   Please use <f RfcOpenEx> instead of this function
 *
 * =====================================================
 *
 *
 * opens a RFC connection.

 * all parameters are passed as single fields.
 *
 *
 * @rdesc
 * returns a handle to the RFC connection (<t RFC_HANDLE>) or
 * RFC_HANDLE_NULL.
 *
 * @xref <f RfcOpenEx>
 *--------------------------------------------------------------------*/
RFC_HANDLE SAP_API RfcOpenExt
(
 rfc_char_t*    destination,      /* @parm name of destination. */
 RFC_MODE		mode,			  /* @parm connection mode ( <t RFC_MODE> ). */
 rfc_char_t*    hostname,         /* @parm hostname of target system. */
 int			sysnr,			  /* @parm system number ( 0 - 99 ). */
 rfc_char_t*    gateway_host,	  /* @parm gateway hostname or NULL */
 rfc_char_t*    gateway_service,  /* @parm gateway service or NULL */
 rfc_char_t*    client,           /* @parm signon data: client */
 rfc_char_t*    user,             /* @parm signon data: user id */
 rfc_char_t*    password,         /* @parm signon data: password */
 rfc_char_t*    language,         /* @parm signon data: language */
 int			trace             /* @parm trace ( 0 / 1 ). */
);




/*------------------------------------------------------------------------
 * @func
 *
 *
 * =====================================================
 *
 *   Please use <f RfcOpenEx> instead of this function
 *
 * =====================================================
 *
 *
 * opens a RFC connection.
 *
 * all parameters are passed as single fields.
 * (using the all possible <t RFC_MODE>)
 *
 * Following parameters in this call are always needed:
 *   <t RFC_MODE> mode,
 *   SAP-logon-Info: client, user, password, language
 *   trace
 *   use_sapgui
 *
 * Depending of <t RFC_MODE> mode following parameters are necessary:
 *
 *
 * RFC_MODE_PARAMETER:
 *
 *   - destination must be defined.
 *
 *   - the 'saprfc.ini' file is needed.
 *
 *   - the 'sideinfo' file is NOT necessary.
 *
 * RFC_MODE_CPIC_EXT:
 *
 *   - destination is not necessary.
 *
 *   - gateway_host, gateway_service, tp_host and tp_name must be defined.
 *
 *   - the 'saprfc.ini' and the 'sideinfo' file are NOT necessary.
 *
 * RFC_MODE_CPIC:
 *
 *   - destination must be defined.
 *
 *   - gateway_host and gateway_service can be defined.
 *
 *   - sideinfo is necessary.
 *
 * RFC_MODE_R3ONLY/RFC_MODE_VERSION_3:
 *
 *   - use_load_balancing is 0 (OFF)
 *
 *       destination is not NULL:
 *
 *         gateway_host, gateway_service, hostname and sysnr
 *         can be defined. sideinfo is necessary.
 *
 *       destination is NULL:
 *
 *         gateway_host and gateway_service can be defined.
 *         hostname, sysnr must be defined.
 *
 *   - use_load_balancing is 1 (ON):
 *
 *       destination, gateway_host, gateway_service,
 *       hostname and sysnr will not be evaluated.
 *
 *       lb_host and lb_system_name must be defined.
 *       if lb_group is NULL the group 'PUBLIC' will be used.
 *
 *
 * ATTENTION:
 *
 *   - use_sapgui and ABAP-Debug are only available with R/3 3.0C or later
 *     and not on Windows with 16-bit-RFC-library.
 *     Moreover, the functionality "INVISIBLE SAPGUI" after each RFC-function
 *     requires R/3, Rel. >= 3.0F and only available with SAPGUI,
 *     Rel. >= 3.0E on Windows NT or 95.
 *
 *   - use_sapgui and ABAP-Debug are not available with R/2.
 *
 *
 * @rdesc
 * returns a handle to the RFC connection (<t RFC_HANDLE>) or
 * RFC_HANDLE_NULL.
 *
 * @xref <f RfcOpenEx>
 *------------------------------------------------------------------*/
RFC_HANDLE SAP_API RfcOpenExtV3
(
 rfc_char_t*    destination,  /* @parm name of destination or NULL */
 RFC_MODE mode,         /* @parm connection mode ( <t RFC_MODE> ) */
 rfc_char_t*    hostname,     /* @parm hostname of target system or NULL */
 int      sysnr,        /* @parm system number ( 0 - 99 ) */
 rfc_char_t*    gateway_host, /* @parm gateway hostname or NULL */
 rfc_char_t*    gateway_service,  /* @parm gateway service or NULL */
 rfc_char_t*    client,           /* @parm signon data: client */
 rfc_char_t*    user,             /* @parm signon data: user id */
 rfc_char_t*    password,         /* @parm signon data: password */
 rfc_char_t*    language,         /* @parm signon data: language */
 int      trace,            /* @parm trace (OFF/ON/ABAP-DEBUG: 0/1/2) */
 RFC_INT  use_load_balancing, /* @parm using load balancing feature (OFF/ON: 0/1)*/
 rfc_char_t*    lb_host,          /* @parm Host name of the 'message server' */
 rfc_char_t*    lb_system_name,   /* @parm Name of the target system (C11 i.e.) */
 rfc_char_t*    lb_group,         /* @parm application server group or NULL for PUBLIC */
 RFC_INT  use_sapgui        /* @parm using sapgui to display SAP dynpros and graphics
			    * (OFF/ON/INVISIBLE_SAPGUI after each RFC-function: 0/1/2)
			    */
);



#ifdef SAPwithUNICODE
#define RFC_ROLE_CLIENT        cU('C')
#define RFC_ROLE_SERVER        cU('S')

#define RFC_SERVER_R2          cU('2')
#define RFC_SERVER_R3          cU('3')
#define RFC_SERVER_EXT         cU('E')
#define RFC_SERVER_EXT_REG     cU('R')
#define RFC_SERVER_SHM         cU('F')

#define RFC_PASS_UNKN          cU(' ')
#define RFC_PASS_OK            cU('O')
#define RFC_PASS_MUST_CHNG     cU('M')
#define RFC_PASS_NO_CHNG       cU('N')
#else
#define RFC_ROLE_CLIENT        'C'
#define RFC_ROLE_SERVER        'S'

#define RFC_SERVER_R2          '2'
#define RFC_SERVER_R3          '3'
#define RFC_SERVER_EXT         'E'
#define RFC_SERVER_EXT_REG     'R'
#define RFC_SERVER_SHM         'F'

#define RFC_PASS_UNKN          ' '  /* unknown (RFC server,
                                     * or SAPsystem older than 46C
                                     * or no roundtrip yet)
                                     */
#define RFC_PASS_OK            'O'  /* OK */
#define RFC_PASS_MUST_CHNG     'M'  /* expired must be changed */
#define RFC_PASS_NO_CHNG       'N'  /* cannot be changed: on this SAP system,
                                     * password can only be changed once a day
                                     */

#endif


/*---------------------------------------------------------------------------------
 * @func
 *
 * With this function, an RFC connection will be opened and if the RFC server
 * is an R/2 or R/3 system, it will try to login onto the according
 * SAP system. There are some advantages with this feature:
 *
 * 1. The structure of parameters in this call need not be changed for
 *    new functionalities in future.
 *
 * 2. SAP logon check at OPEN-time and not at the first call of a
 *    function module as usual.
 *
 * 3. Better performance if the RFC server is an R/3 from 3.0 onwards,
 *    because data will be compressed before transfer and not after the
 *    first call as usual.
 *
 *
 * This call should be used instead of RfcOpen, RfcOpenExt, RfcOpenExtV3
 * and RfcConnect.
 *
 *
 * @head2 Structure and contents of the 'Connect_param' |
 * This parameter is a zero terminated string and must have the following
 * structure:
 *
 * @head3 ID=value ID=value ... ID=value |
 *
 * Pay attention that the ID's are not case-sensitive but the values are
 * case-sensitive and must be set in "..." if blanks are included.
 *
 * Depend on the RFC server (R/2, R/3 or External system), some of
 * these ID's are fix and some of them are optional.
 *
 * Following ID's are available:
 *
 *
 * TYPE             RFC server type, 2/3/E: R/2 or R/3 or External System
 *                  (Default: 3)
 *
 * CLIENT           SAP logon client
 *
 * USER             SAP logon user
 *
 * PASSWD           SAP logon password
 *
 *
 * LANG             SAP logon language (1-byte SAP language or 2-byte ISO language)
 *
 * CODEPAGE         The given  codepage is to be used for this connection (Default is 
 *                  either 1100 or set by <f RfcSetSystemCodepage> or is set by SAP_CODEPAGE
 *                  environment variable). Could be rather useful if the sapgui should be started 
 *                  with codepage differs from 1100. 
 *
 * LCHECK           Logon check at OPEN time (0/1: without/with check, Def. 1)
 *
 * NEWPASS          Changes the password during the logon
 *                  !!!On SAP system kernel older than 46C the password is sent
 *                  in clear text on the network!!!!
 *
 * TRACE            RFC trace (0/1: without/with trace, Def. 0)
 *
 * DEST             Destination in saprfc.ini if working with saprfc.ini.
 *                  If the RFC server is an R/2 system this destination must
 *                  also be defined in the 'sideinfo' for the SAP gateway.
 *
 * GWHOST           Host name of the SAP gateway (if server is R/2 or External)
 *
 * GWSERV           Service of the SAP gateway   (if server is R/2 or External)
 *
 * MSHOST           Host name of the Message Server (if using Load Balancing)
 *
 * MSSERV           Service of the Message Server (if using Load Balancing)
 *
 * R3NAME           Name of the R/3 system (if using Load Balancing)
 *
 * GROUP            Name of the group of application servers (if using Load Balancing)
 *
 * ASHOST           Host name of a specific application server (R/3, No Load Balancing)
 *
 * SYSNR            R/3 system number (R/3, No Load Balancing)
 *
 * TPHOST           Host name of the external RFC server program
 *
 * TPNAME           Path and name of the external RFC server program
 *                  or Program ID of an registered RFC server program.
 *
 * USE_SAPGUI       RFC with SAPGUI, 0/1/2: without/with/invisible SAPGUI
 *                  between two RFC functions (Default: 0). If the sapgui
 *                  is to be started with codepage differs from 1100, please
 *                  use option CODEPAGE to define the codepage you need.
 *
 * ABAP_DEBUG       RFC with ABAP debugger, 0/1: without/with ABAP debugger
 *                  (Default: 0)
 *
 * SNC_MODE         Working with SNC, 0/1: without/with SNC
 *                  (Default: 0, s. <t RFC_SNC_MODE>)
 *
 * SNC_QOP          SNC Quality of service
 *                  (Default: 8 (RFC_SNC_QOP_DEFAULT), s. <t RFC_SNC_QOP>)
 *
 * SNC_MYNAME       Own SNC name if you don't want to use the default SNC name
 *
 * SNC_PARTNERNAME  SNC name of the SNC partner (RFC server)
 *                  or SNC name of the message server (Load Balancing)
 *
 * SNC_LIB          Path and name of the SNC-library
 *
 * SAPLOGON_ID      String defined for SAPLOGON on 32-bit Windows
 *
 * GRT_DATA         SAProuter connect data for SAPGUI when using RFC with SAPGUI.
 *                  /H/......  : the whole router string for SAPGUI.
 *                  /P/password: If the password for the SAPGUI connection is not
 *                              the same as the one for the RFC connection.
 *
 * PCS              Partner's Char Size. The RFC-library determines automatically the partner's char size
 *                  at open time (using logon check) or at first call time (without logon check).
 *                  This flag tells directly the unicode RFC library to open a connection to a system with 
 *                  size of char given by this value. Currently are two values valid 1/2 : Non Unicode/Unicode
 *                  (Default: 1).
 *                  If the partner is not an unicode system but the value of the PCS flag is 2 an error will occur 
 *                  (Runtime exception in the remote system). If the partner is an unicode system but the value
 *                  of the PCS flag is 1 the connection kind will be switched automatically.
 *                  This field works only with unicode library.
 *
 * COMM_CP          When communication has to be established between an Unicode Library and a Non Unicode system,
 *                  all char like data will be converted into codepage which matched to logon language before send them. 
 *                  This codepage is called communication codepage. The effect of this method is that the Non Unicode 
 *                  System is sure to talk an system with communication codepage and not with an unicode system. 
 *                  Usualy the RFC Library determines  automatically the communication codepage. Using this option 
 *                  it is possible for the programer to set the communication codepage directly. 
 *                  This option is only active in the unicode version of the RFC library. 
 *
 * ICCE             Ignore Character Conversion Errors, 0/1: Do not ignore/Ignore (Default 1 if not redefined by environment
 *                  variable RFC_IGNORE_CONV_ERROR). The value set by environment variable can be overloaded by connect string.
 *                  This flag determines the runtime behaviour of the RFC library concerning character conversion.
 *                  If this flag is 1, the concerned API will not exit with error, but replace the character 
 *                  which could not be converted with CFIT defined token.
 *
 * CFIT             Conversion Fault Indicator Token. 
 *                  This flag determines substitute symbol for received unicode characters, which could not 
 *                  be converted by the RFC library (Default: non unicode systems 0x23, unicode systems 0xfffd,
 *                  or defined by environment variable RFC_REPL_CHAR). The code point has to be set using
 *                  hex format (0x23, 0x30 etc). The value set by environment variable 
 *                  can be overloaded using connect string.
 *
 * WAN_CONN         RFC via Wide Area Network, 0 = LAN, 1 = WAN (Default: 0).
 *                  If LAN is used, all tables bigger than 8000 Bytes will be compressed before sent.
 *                  If WAN is used, all tables bigger than 250 Bytes (or value defined by 
 *                  environment variable RFC_WAN_THRESHOLD) will be compressed before sent.
 *                  The table size will be calculated as follows: table_length * number_of_rows.
 *
 * IDLE_TIMEOUT     Inform the Web Application Server to close the connection after idle time in seconds.
 *
 * TOUPPER          Do not convert user and password to upper case for sending to Web Application Server: 0/1 (Default 1, i.e. convert to upper case).
 *
 * Alternative logon possibilities:
 *
 * ALIAS_USER       An alias user name, could used instead of user or even together with USER.
 *                  If both USER and ALIAS_USER are used, than theay have to be match.
 *
 * MYSAPSSO         SAP Cookie Version 1. Will be used instead of user and password for logon to backend
 *
 * MYSAPSSO2        SAP Cookie Version 2. Will be used instead of password for logon to backend. 
 *                  In this case, user name is optional.
 *
 * X509CERT         An X.509 certificate will be used instead of password to logon to R/3 System.
 *                  In this case, user name is optional.
 *
 * EXTIDDATA        Contains valid external user's ID of an external authentification system. 
 *                  User name is optional. 
 *                  External ID is to be defined in the backend (SAP-System). 
 *
 * EXTIDTYPE        Defines the kind of external identity. Valid only with EXTIDDATA.
 *                  Follow values are not allowed: ID, NT; DN, CA, X, HX.
 *
 * Additionally, RFC Library provides the feature to retrieve MYSAPSSO2 certificate from the backend
 * after successful logon:
 * 
 * GETSSO2          Request to create a cookie version 2 using given password and user name.
 *                  If the value is 1, the cookie will be generated from user and password values
 *                  given by USER=user and PASSWORD=password in the same connect_param string.
 *                  Instead, user and password X.509 certificate could be used.
 *                  If the RfcOpenEx call ended successfully, the generated SAP cookie version 2 
 *                  can be retrieved via <f RfcGetTicket> API.
 *
 *
 * @head2 EXAMPLES |
 *
 * Some examples for working with these ID's.
 *
 * @head3 1. RFC server is an R/2 system |
 *
 *    Following data are fix:  TYPE=2 DEST=... CLIENT=... USER=... PASSWD=... LANG=D
 *
 *    no additional info necessary if using saprfc.ini
 *
 *    additional info if not using saprfc.ini:   GWHOST=ihsap4 GWSERV=sapgw00
 *
 *    additional info if using SAPLOGON on 32-bit Windows:  SAPLOGON_ID=...
 *
 *    In all cases, the sideinfo for the according SAP gateway must
 *    have an entry with the destination above.
 *
 *
 * @head3 2. RFC server is an R/3 system |
 *
 *    Following data are fix:  CLIENT=... USER=... PASSWD=... LANG=D
 *
 *    additional info if using saprfc.ini    :   DEST=BIN
 *
 *    additional info if not using saprfc.ini
 *
 *      + Connect to a specific application server: ASHOST=hs0311 SYSNR=53
 *
 *      + Connect using Load Balancing: MSHOST=hs0311 R3NAME=BIN GROUP=PUBLIC
 *
 *        If no info about MSHOST, the RFC library will try to get this
 *        host name in the 'sapmsg.ini' customized for SAPLOGON on Windows
 *        (in Windows-directory). This file can also be copied and used on
 *        a Non-Windows-Host. It must be in the working directory of the RFC
 *        client or its path name must be defined via the environment
 *        'RFC_LOGON_INI_PATH' (e.g. /usr/rfctest).
 *        The SAProuter string, if exists, must be defined in the R/3 name.
 *
 *        e.g.  R3NAME=/H/sapgate1/S/3297/LOI where LOI is the R/3 system name.
 *
 *        or    R3NAME=/SAP_test/LOI    or    R3NAME='/SAP America/LOI'
 *              where 'SAP_test' and 'SAP America' is the router name defined
 *              in the 'saproute.ini' for SAPLOGON on Windows.
 *
 *    additional info if using SAPLOGON on 32-bit Windows:  SAPLOGON_ID=...
 *
 *
 * @head3 3. RFC server is an external program |
 *
 *    Following data are fix:  TYPE=E
 *
 *    additional info if using saprfc.ini    :   DEST=RFCEXT
 *
 *    additional info if not using saprfc.ini
 *
 *      In this case, the info about TPHOST determines whether your RFC
 *      client will connect to an RFC server started by the SAP gateway
 *      or it will connect to an already registered RFC server.
 *
 *      + RFC server will be started by the SAP gateway
 *
 *        GWHOST=hs0311 GWSERV=sapgw53 TPHOST=hs0311 TPNAME=/sap/srfcserv
 *
 *        If GWHOST and TPHOST are different, make sure that the SAP gateway
 *        has the access to start the server program via remote shell.
 *
 *
 *      + RFC server is already registered at the SAP gateway
 *
 *        GWHOST=hs0311 GWSERV=sapgw53 TPNAME=hw1145.srfcserv
 *
 *
 * See the delivered program srfctest.c for more details.
 *
 *
 *
 * @head2 ATTENTION |
 *
 *   - USE_SAPGUI and ABAP_DEBUG are only available with R/3 3.0C or later
 *     and not on Windows with 16-bit-RFC-library.
 *     Moreover, the functionality "INVISIBLE SAPGUI" after each RFC-function
 *     requires R/3 from 3.0F onwards and only available with SAPGUI from 3.0E
 *     onwards on Windows NT/95 or with SAPGUI from 3.0F onwards on UNIX.
 *
 *   - USE_SAPGUI and ABAP_DEBUG are not available with R/2.
 *
 *   - SAPLOGON_ID is only available on 32-bit Windows where SAPLOGON is also installed.
 *     Using this feature, all parameters needed for open a connection to an SAP system
 *     such as DEST, GWHOST, GWSERV, ASHOST, SYSNR, MSHOST, R3NAME, GROUP and SNC-parameters
 *     are not necessary because the RFC library will get this information from the
 *     SAPLOGON data files.
 *
 *
 * @rdesc
 * returns a valid handle to the RFC connection (<t RFC_HANDLE>) or
 * RFC_HANDLE_NULL if connection could not be established. In the
 * case of failure please interprete the <p error_info> structure for more 
 * information.
 *
 *
 * @comm
 * 1. Every connection will be established either
 * with default code page 1100 or 
 * according environment variable SAP_CODEPAGE or
 * according id CODEPAGE in <p connect_param> string of this function.
 *
 * 2. Do not call <f RfcLastErrorEx> if RFC_HANDLE_NULL was returned but
 * extract the error information from <p error_info>.
 *
 * 3. The UNICODE RFC Library determines automatically, which kind connection
 * is to be established: unicode or non unicode. This will be done at open time 
 * if RfcOpenEx was called with logon check or at the first rfc call time without
 * logon check (s. LCHECK filed for more information).
 *
 *---------------------------------------------------------------------------*/
DECL_EXP RFC_HANDLE SAP_API RfcOpenEx 
(
 rfc_char_t         * connect_param, /* @parm connection parameters.*/
 RFC_ERROR_INFO_EX  * error_info     /* @parm <t RFC_ERROR_INFO_EX> describing the error.*/
);





/*----------------------------------------------------------------------------------------
 * @func
 *
 *
 * =====================================================
 *
 *   Please use <f RfcOpenEx> instead of this function
 *
 * =====================================================
 *
 *
 * opens a connection according to the given options.
 *
 * The structure <t RFC_OPTIONS>
 * contains the necessary data for opening the connection
 *
 *
 * @rdesc
 * returns a handle to the RFC connection (<t RFC_HANDLE>) or
 * RFC_HANDLE_NULL, if the connection cannot be opened.
 *
 * @xref <f RfcOpenEx>
 *---------------------------------------------------------------*/
RFC_HANDLE SAP_API RfcOpen
(
  RFC_OPTIONS * options  /* @parm connection parameters as described
                          * at structure <t RFC_OPTIONS>.
                          */
);



/*--------------------------------------------------------------
 * @func
 * accepts an incoming connection and has to be used, if the program
 * was started by a RFC call issued by a SAP system or an other
 * program using this API. The command line (argv) has to be passed
 * to this function.
 *
 * With Release 3.0C onwards this function can be used to register at
 * a SAP gateway and the server program can wait for next RFC request
 * by issueing <f RfcDispatch> or <f RfcWaitForRequest> or <f RfcListen>
 * or <f RfcGetName>.
 *
 * Using this functionality a RFC server program can now already run
 * and work as a RFC daemon. A start of any server program by a R/3
 * application server, by SAPGUI or via remote shell by a SAP gateway
 * is no longer necessary.
 *
 * There are 2 ways to define the input parameter 'argv':
 *
 * 1. Working with 'saprfc.ini'-file:
 *
 *    -D<lt>destination pointed to an entry in 'saprfc.ini'<gt>
 *
 *    -t      Set RFC-Trace on
 *
 *
 * 2. Working without 'saprfc.ini'-file:
 *
 *    -a<lt>program ID<gt> e.g. own_host_name.program_name
 *
 *    -g<lt>host name of the SAP gateway<gt>
 *
 *    -x<lt>service of the SAP gateway<gt>, e.g. sapgw00
 *
 *    -t      Set RFC-Trace on
 *
 *
 *
 * We recommend the first one because you can use some RFC features today
 * and in the future without changes in your RFC programs.
 *
 * See saprfc.ini for more details.
 *
 *
 *
 * The 3 parameters above must fit with the configuration in R/3.
 * (via SM59, Connection type T and Register Mode)
 *
 * For working with register mode there are no other changes in RFC
 * server programs necessary.
 *
 * Be careful by using only the program name as program ID because
 * there might be more programs (e.g. rfcexec) registered at the
 * same SAP gateway. Please use at least the host name of the computer where
 * your RFC server program is running as a part of the program ID.
 *
 * Example: The well-known rfcexec program can be started from the
 *          command line with RFC-trace as follows:
 *
 *          rfcexec -ap10234.rfcexec -ghs0311 -xsapgw53 -t
 *
 *       or
 *
 *          rfcexec -Drfctest
 *
 *          and an entry in saprfc.ini can be defined as follows:
 *
 *          DEST=rfctest
 *
 *          TYPE=R
 *
 *          PROGID=p10234.rfcexec
 *
 *          GWHOST=hs0311
 *
 *          GWSERV=sapgw53
 *
 *          RFC_TRACE = 1
 *
 *
 *
 * RFC server programs working with this functionality will be called
 * RFC server programs running in register mode.
 *
 *
 * If an RFC server program is working with SNC (Secure Network
 * Communications) it can be started with the following SNC parameters:
 *
 *    -L<lt>SNC library name on the local system<gt> e.g. /krb5/hpux/lib/libkrb5.sl
 *
 *    -S<lt>SNC name of this local instance<gt>
 *
 *
 * ATTENTION:
 *
 * RFC program can use <f RfcCheckRegisterServer> to test whether
 * and how many server programs are registered at a SAP gateway with a
 * defined program ID. Moreover, it can use <f RfcCancelRegisterServer>
 * to cancel all RFC servers which are registered at a SAP gateway with a
 * defined program ID.
 *
 *
 * @rdesc
 * returns a valid <t RFC_HANDLE> or RFC_HANDLE_NULL
 *
 * @comm
 * An unicode rfc server switchs automatically to unicode/non-unicode mode
 * depends from a client (unicode/non unicode).
 *--------------------------------------------------------------*/
DECL_EXP RFC_HANDLE SAP_API RfcAccept
(
  rfc_char_t ** argv /* @parm command line (argv), the program is started with.*/
);




RFC_HANDLE SAP_API RfcAcceptExt( rfc_char_t* CommandLine );




/*----------------------------------------------------------------------------
 * @func
 *
 * closes a RFC connection or all RFC connections.
 *
 * @comm
 * Be carefully. If parameter is RFC_HANDLE_NULL, all existing and in the same thread last active connections
 * will be closed. This could leads to an unexpected behaviour of your application.
 * It could be usefull to test whether the connection have been already closed,
 * before call this API
 *
 * ...
 * if (handle != RFC_HANDLE_NULL)
 *  RfcClose (handle);
 * ...
 *
 * A connection could already be closed after an unsuccessful
 * RfcCall for example. 
 *----------------------------------------------------------------------------*/
DECL_EXP void SAP_API RfcClose 
(
    RFC_HANDLE handle   /* @parm handle to RFC connection.
				         *
                         * If handle is RFC_HANDLE_NULL, all opened and in the same thread last active connections
                         * will be closed.
                         */
);


/*----------------------------------------------------------------------------
 * @func
 *
 * sends a error message, if possible, and closes the connection.
 *
 * A given error message cannot be send, if the receiver is not in state,
 * where it expects some RFC data to receive.
 *
 *----------------------------------------------------------------------------*/
DECL_EXP void SAP_API RfcAbort 
(
    RFC_HANDLE handle,  /* @parm handle to RFC connection */
    rfc_char_t * text   /* @parm error message.
                         *
                         * If this field is NULL, the connection is
                         * only closed.
                         */
);


/*---------------------------------------------------------------
 * @enum RFC_RC | RFC return codes
 *---------------------------------------------------------------*/
typedef enum
{
    RFC_OK,                     /* @emem O.K.                                                   */

    RFC_FAILURE,                /* @emem Error occurred                                         */

    RFC_EXCEPTION,              /* @emem Exception raised                                       */

    RFC_SYS_EXCEPTION,          /* @emem System exception raised, connection closed             */

    RFC_CALL,                   /* @emem Call received                                          */

    RFC_INTERNAL_COM,           /* @emem Internal communication, repeat (internal use only      */

    RFC_CLOSED,                 /* @emem Connection closed by the other side                    */

    RFC_RETRY,                  /* @emem No data yet (RfcListen or RfcWaitForRequest only)      */

    RFC_NO_TID,                 /* @emem No Transaction ID available                            */

    RFC_EXECUTED,               /* @emem Function already executed                              */

    RFC_SYNCHRONIZE,            /* @emem Synchronous Call in Progress (only for Windows)        */

    RFC_MEMORY_INSUFFICIENT,    /* @emem Memory insufficient                                    */

    RFC_VERSION_MISMATCH,       /* @emem Version mismatch                                       */

    RFC_NOT_FOUND,              /* @emem Function not found (internal use only)                 */
    
    RFC_CALL_NOT_SUPPORTED,     /* @emem This call is not supported                             */

    RFC_NOT_OWNER,              /* @emem Caller does not own the specified handle               */

    RFC_NOT_INITIALIZED,        /* @emem RFC not yet initialized                                */

    RFC_SYSTEM_CALLED,          /* @emem A system call such as RFC_PING for connection
                                 *       test is executed
                                 */
    RFC_INVALID_HANDLE,         /* @emem An invalid handle was passed to an API call
                                 */
    RFC_INVALID_PARAMETER,      /* @emem An invalid parameter was passed to an API call or
                                 *       parameter length mismatch (expected and received) has
                                 *       been detected.
                                 */
    RFC_CANCELED,               /* @emem An rfc call has been canceled by user                  */

    RFC_CONVERSION,             /* @emem An error during conversion has been detected           */

    RFC_INVALID_PROTOCOL,       /* @emem The received data has an unsupported format.
                                 *       e.g.: non unicode application receives data sended 
                                 *       in unicode format
                                 */
	RFC_TIMEOUT
}
RFC_RC;




/*------------------------------------------------------------------
 * @struct RFC_PARAMETER |
 * RFC parameters.
 *
 * Describes 'Exporting', 'Importing', 'Changing' parameters of the
 * interface of a function module.
 *
 * @xref <t RFC_CHAR>, <t RFC_NUM>, <t RFC_BYTE>, <t RFC_BCD>,
 *       <t RFC_INT1>, <t RFC_INT2>, <t RFC_INT>, <t RFC_FLOAT>,
 *       <t RFC_DATE>, <t RFC_TIME> <t RFC_STRING>.
 *------------------------------------------------------------------*/
typedef struct
{
#ifdef RFCwithVOID_NAMES
   const void *   name;  /* @field Name of the field (in the interface
                    * definition of the function).
                    */
#else
   const RFC_CHAR *name;
#endif
   unsigned nlen;  /* @field Length of the name
                    * (should be strlen(name)).
                    */
   RFC_TYPEHANDLE type;  /* @field Data type of the field.
                          * In former versions this field was simply
                          * defined as 'unsigned'.
                          */
   /*SAPUNICODEOK_RAWLEN*/unsigned leng;  /* @field Length of the field in Bytes.
                    */
   void *   addr;  /* @field Address of the field to be exported
                    * or imported.
                    */
}
RFC_PARAMETER;




#ifndef SAPITAB_H
typedef void * ITAB_H;
#endif



/*-------------------------------------------------------------------
 *
 * @enum RFC_ITMODE | mode how to pass internal table
 *
 *--------------------------------------------------------------------*/
typedef enum
{
   RFC_ITMODE_BYREFERENCE,  /* @emem Table is passed by reference
                             * (ALLWAYS USE THIS)
                             */
   RFC_ITMODE_BYVALUE,      /* @emem Table is passed by value, changes
                             * are not transported back
                             * (internal use only !)
                             */
   RFC_ITMODE_KEEPALIVE     /* @emem Table is passed by reference, but
                             * is kept alive after returning,
                             * (i.e. after RfcSendData, internal use
                             * only )
                             */
}
RFC_ITMODE;



/*--------------------------------------------------------
 *
 * @struct RFC_TABLE |
 * RFC tables.
 *
 * Describes Tables parameters of the interface of a function module.
 *
 *-----------------------------------------------------------------------*/
typedef struct
{
#ifdef RFCwithVOID_NAMES
  const void *   name;      /* @field Name of the table (in the interface
                        * definition of the function).
                        */
#else
   const RFC_CHAR * name;
#endif
   unsigned nlen;      /* @field Length of the name
                        * (should be strlen(name)).
                        */
   RFC_TYPEHANDLE type;      /* @field Data type of the lines of the table.
                              * In former versions this field was simply
                              * defined as 'unsigned'.
                              */
   unsigned leng;      /* @field length of a row in bytes.
                        */
   ITAB_H   ithandle;  /* @field Table handle <t ITAB_H>,
                        * i.e. the address of
                        * the control structure of the
                        * internal table.
                        *
                        * This is an input field at <f RfcCall> and
                        * an output field at <f RfcGetData>.
                        */
   RFC_ITMODE itmode;  /* @field mode ( <t RFC_ITMODE> ),
                        * how this table has to be received :
                        * 'call by reference' or 'call by value'.
                        * (only for RfcGetData, has no effect
                        *  in RfcCall).
                        */
   int      newitab;   /* @field If the value is not 0 after RfcGetData,
                        * the table was created by RfcGetData.
                        * Internal use only. This field must not
                        * be modified
                        * between <f RfcGetData> and <f RfcSendData> !
                        */
}
RFC_TABLE;



/*------------------------------------------------------
 *
 * @func
 *
 *  Function to allocate, reallocate or free a <t RFC_BLOB>.
 *
 *  @rdesc
 *  returns <e RFC_RC.RFC_OK> if no error occurred and
 *  <e RFC_RC.RFC_FAILURE> elsewhere.
 *
 *  @xref <f RFC_BLOB>
 *
 *------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcResizeBlob 
(
   RFC_BLOB * pBlob, /* @parm pointer to BLOB. */
   unsigned   leng   /* @parm new size of BLOB. 
                      * If leng is 0, the BLOB is freed.
		              */
);

/*----------------------------------------------------------------------------
 * @func
 *
 * The RFC library determines the own codepage for every RFC
 * connection at open time. 
 *
 * Every connection will be established either
 * with default code page (1100) or 
 * according environment variable SAP_CODEPAGE or
 * according option CODEPAGE in <f RfcOpenEx> or according
 * new default codepage set by <f RfcSetSystemCodePage>.
 * 
 * With this call, a RFC client or server program can set an own
 * codepage for a RFC connections to runtime. In this case, this call must 
 * be issued after <f RfcOpen>, <f RfcOpenEx> or <f RfcAccept>. 
 * 
 * Alternatively one can use option CODEPAGE
 * of <f RfcOpenEx> to define the codepage for a RFC connection to open time. 
 * 
 * If one wants to change the default codepage, <f RfcSetSystemCodePage> must
 * be called.
 * 
 * @xref <f RfcOpenEx>, <f RfcOpen>, <f RfcSetSystemCodepage>, <f RfcCharConversion>.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>              | If call successful
 * @flag <e RFC_RC.RFC_INVALID_HANDLE>  | If handle is invalid
 * @flag <e RFC_RC.RFC_NOT_OWNER>       | If caller does not own the specified handle
 * @flag <e RFC_RC.RFC_NOT_INITIALISED> | If initialization has not be done or failed
 *----------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcSetCodePage 
(
  RFC_HANDLE   handle,      /* @parm handle to RFC connection. */
  rfc_char_t * codepage     /* @parm own code page (e.g. "1800") */
);

/*----------------------------------------------------------------------------
 * @func
 * 
 * With this call, a RFC client or server process can change the default codepage.
 * The call may be called to any time. All connections opened after this call,
 * will be established with new default codepage.
 * 
 * Alternatively one can use option CODEPAGE
 * of <f RfcOpenEx> to set the codepage for a RFC connection 
 * at open time (in this case the default codepage will be ignored).
 * 
 * @xref <f RfcOpenEx>, <f RfcOpen>, <f RfcSetCodepage>, <f RfcCharConversion>.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>              | If call successful
 * @flag <e RFC_RC.RFC_NOT_INITIALISED> | If initialization was not be done or failed.
 *
 * @comm 
 * - Please notice that the codepage's change is valid only for all connections,
 * which will be established after this call. All already existing connections
 * are not concerned.
 *
 * - Every connection, which will be established after this call, will be done either
 * with new default code page or according option CODEPAGE in <f RfcOpenEx>.
 *----------------------------------------------------------------------------*/
RFC_RC SAP_API RfcSetSystemCodePage 
(
  rfc_char_t * codepage  /* @parm codepage (e.g "8000") */
);

/*------------------------------------------------------
 * @func
 *
 * RfcGetAttributes returns some information about this
 * RFC connection.
 *
 * The RFC library can only set these attributes after the
 * RFC connection is established and at least one RFC function
 * is called.
 *
 * Therefore, it should be called after <f RfcOpenEx> with
 * SAP-logon check at OPEN-time or after the first <f RfcReceive>
 * in an RFC client program or after <f RfcGetData> in an
 * RFC server program.
 *
 *
 * @rdesc
 * returns 0 if no error occurred and 1 elsewhere.
 *------------------------------------------------------
 */
DECL_EXP int SAP_API RfcGetAttributes 
(
    RFC_HANDLE       handle,            /* @parm valid connection handle. */
    RFC_ATTRIBUTES * rfc_attributes     /* @parm structure
                                         * <t RFC_ATTRIBUTES> describing some
                                         * information about this RFC connection.
                                         */
);

/*------------------------------------------------------
 * @func
 *
 * RfcGetCodePagesInfo returns information about the
 * code pages and conversions flags used for this RFC connection.
 *
 * The RFC library can only set these attributes after the
 * RFC connection is established and at least one RFC function
 * has been called.
 *
 * Therefore, it should be called after <f RfcOpenEx> with
 * SAP-logon check at OPEN-time or after the first <f RfcReceive>
 * in an RFC client program or after <f RfcGetData> in an
 * RFC server program.
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>              | If call successful
 * @flag <e RFC_RC.RFC_INVALID_HANDLE>  | If handle is invalid
 * @flag <e RFC_RC.RFC_NOT_OWNER>       | If caller does not own the specified handle
 * @flag <e RFC_RC.RFC_NOT_INITIALISED> | If initialization has not be done or failed
 *
 * @xref <t RFC_CODE_PAGE>
 *------------------------------------------------------
 */
DECL_EXP RFC_RC SAP_API RfcGetCodePagesInfo 
(
    RFC_HANDLE      handle,         /* @parm valid connection handle. */
    RFC_CODE_PAGE   pOwnCP,         /* @parm Own code page, 
                                     * the local code page*/
    RFC_CODE_PAGE   pRemoteCP,      /* @parm Remote code page,
                                     * the code page from the remote system*/
    RFC_CODE_PAGE   pCommCP,        /* @parm Communication code page,
                                     * the code page used on the wire.
                                     * For client, this differs from 
                                     * pOwnCP when a UNICODE system 
                                     * communicates with a non-UNICODE system.
                                     * For servers, this differs from pRemoteCP
                                     * when a UNICODE system communicates with
                                     * a non-UNICODE system
                                     */
    SAP_CHAR      * pReplChar,      /* @parm Replacement character. */
    int           * pIgnore,        /* @parm Ignore conversion error. */
    int           * pcs             /* @parm Actual character size from the partner */

);

/*----------------------------------------------------------------------------
 *
 * @func
 *
 * An RFC program can use this call to check whether and how
 * many RFC server programs are registered at a SAP gateway with a defined
 * program ID.
 *
 * This call is only available with a SAP gateway from 3.1G onwards.
 *
 * An RFC server program which registers at a SAP gateway and then waits for
 * RFC requests from any R/2 or R/3 or another external program can have
 * one of the following 3 states:
 *
 * INIT:  RFC server is only registered at the SAP gateway, it doesn't wait
 *        at this moment for RFC request. An RFC server enters in this state
 *        after <f RfcAccept> in registered mode.
 *
 * READY: RFC server enters in this state after <f RfcListen>,
 *        <f RfcWaitForRequest>, <f RfcGetName> or <f RfcDispatch>.
 *        It is now ready to process incoming RFC requests. Be careful,
 *        RfcGetName and RfcDispatch are blocking calls. They will
 *        be returned when an RFC request is incoming.
 *        Make sure that the time between two RfcListen or RfcWaitForRequest
 *        is not longer than the CPIC_TIMEOUT defined for the SAP gateway.
 *        This is necessary because the SAP gateway will only queue the RFC
 *        request within the CPIC time above. If this time is elapsed the
 *        RFC request will be returned back to the RFC client with an
 *        according error.
 *
 * BUSY:  RFC server is processing an RFC request and is not available
 *        for other ones.
 *
 *
 * @xref <f RfcCancelRegisterServer>.
 *
 * @rdesc
 * <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE>.
 *
 *----------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcCheckRegisterServer
(
  rfc_char_t      * tpid,       /* @parm program ID of the registered RFC server program*/
  rfc_char_t      * gwhost,     /* @parm host name of the SAP gateway  */
  rfc_char_t      * gwserv,     /* @parm service of the SAP gateway    */
  RFC_INT		  * ntotal,     /* @parm total number of all registered RFC server programs
			                     *  (Sum of ninit, nready and nbusy)
			                     */
  RFC_INT		  * ninit,      /* @parm number of registered RFC server programs
			                     * in INIT-state with the program ID above.
			                     */
  RFC_INT		  * nready,     /* @parm number of registered RFC server programs
							     * in READY-state with the program ID above.
			                     */
  RFC_INT		  * nbusy,      /* @parm number of registered RFC server programs
			                     * in BUSY-state with the program ID above.
			                     */
  RFC_ERROR_INFO_EX * error_info    /* @parm structure
				                     * <t RFC_ERROR_INFO_EX> describing the error
                                     */
);




/*----------------------------------------------------------------------------
 *
 * @func
 *
 * An RFC program can use this call to cancel all registered RFC servers
 * at a SAP gateway with a defined program ID.
 *
 * Pay attention that RFC servers in BUSY state cannot be canceled.
 * In this case you will have a difference between the output parameters
 * ntotal and ncancel returned by this call.
 *
 * This call is only available with a SAP gateway from 3.1H onwards.
 *
 *
 * @xref <f RfcCheckRegisterServer>.
 *
 * @rdesc
 * <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE>.
 *
 *----------------------------------------------------------------------------*/
RFC_RC SAP_API RfcCancelRegisterServer
(
  rfc_char_t      * tpid,        /* @parm program ID of the registered RFC server program */
  rfc_char_t      * gwhost,      /* @parm host name of the SAP gateway  */
  rfc_char_t      * gwserv,      /* @parm service of the SAP gateway	   */
  RFC_INT		  * ntotal,      /* @parm total number of all registered RFC server programs */
  RFC_INT		  * ncancel,     /* @parm total number of all canceled RFC server programs   */
  RFC_ERROR_INFO_EX * error_info /* @parm structure <t RFC_ERROR_INFO_EX> describing the error */
);




/*----------------------------------------------------------------------------
 * @func
 *
 * Cleanup the attached R/3 context
 *
 * Resets the state acquired by the R/3 user context as well as
 * local cache information.  This call is intended for client programs
 *
 * This is only useful when running against an R/3 release 40A
 * and above.
 *
 * If the remote system is not an R/3 release 40A and above,
 * RfcCleanupContext calls <f RfcClose> internally and returns
 * RFC_CLOSE,
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | The context has been reset.
 * @flag <e RFC_RC.RFC_CLOSED>  | The connection has been closed internally
 *                                The caller shall call RfcOpen* to obtain a
 *                                new handle.
 * @flag <e RFC_RC.RFC_FAILURE> | An error occurred. The connection is lost.
 *----------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcCleanupContext 
(
 RFC_HANDLE handle  /* @parm handle to connection that must be cleaned */
);



/*---------------------------------------------------------------
 * @enum RFC_PASS_RC | ON_RFC_PASSWORD_EXPIRED reason
 * This value is passed as reason code to the ON_RFC_PASSWORD_EXPIRED
 * call back.  This is helpful to handle system policies
 *---------------------------------------------------------------*/
typedef enum
{
   RFC_PASS_EXPIRED,      /* @emem First call: password is expired
                           */
   RFC_PASS_REFUSED       /* @emem nth call: Password supplied
                           *       by the previous call refused
                           *       (too short, had already been
                           *       used previously, not enough
                           *       uppercase, not enough digits...)
                           *
                           */
} RFC_PASS_RC;


/*-----------------------------------------------------------------------
 * @cb  RFC_RC |  ON_RFC_PASSWORD_EXPIRED |
 *
 * function to be installed by <f RfcRegisterPasswordChanger>.
 *
 * The function is called when an expired password is reported
 * by R/3 and when LCHECK=1 was specified in <f RfcOpenEx>
 * (this is the default).
 *
 * The function shall write the new password in the buffer
 * pointed by <p NewPassword>.
 * The buffer is guarantied to be <p cntByte> long.
 *
 * The function may return an empty string (filled with zeros) to signal that
 * the user does not wish to change the password.
 *
 * @comm
 * Only SAP system running a kernel 46C and higher will report expired password.
 *------------------------------------------------------------------------------*/
typedef RFC_RC (DLL_CALL_BACK_FUNCTION_PTR ON_RFC_PASSWORD_EXPIRED)
(
 RFC_PASS_RC     Reason,     /* @parm [in]  Reason for this call    */
 RFC_CHAR      * NewPassword,/* @parm [out] The new password to use */
 RFC_INT         cntByte     /* @parm [in]  The size of NewPassword */
);




/*----------------------------------------------------------------------------
 * @func
 *
 * Registers a function that will be called during <f RfcOpenEx> if the
 * systems reports that the password is expired.
 *
 * If RfcRegisterPasswordChanger() is not called, then <f RfcOpenEx> will
 * not report expired password.
 *
 * The function is called when an expired password is reported
 * by R/3 and when LCHECK=1 was specified in <f RfcOpenEx>
 * (this is the default).
 *
 *
 * Only SAP system running a kernel 46C and higher will report expired password.
 *
 * @xref <f RfcCancelRegisterServer>.
 *
 * @rdesc
 * <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE>.
 *
 *-------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcRegisterPasswordChanger
(
 ON_RFC_PASSWORD_EXPIRED pCbChangePassword	   /* @parm a call back function that will be called
												* if a SAP system reports an expired password
    									        */
);


/*----------------------------------------------------------------------------
 * In order to provide the correct alignment on various UNICODE platforms
 * as in the ABAP-VM, pading bytes are generated in structures
 * The amount of padding bytes is dependant of the UNICODE support of the
 * System
 *
 * Please use the tool genh found in .../rfcsdk/bin
 * to generate the header for your structure.
 *
 * rfc_padd_4_1BperC	//padding in a one byte per character environment
 * rfc_padd_4_2BperC	//padding in a two bytes per character environment
 * rfc_padd_4_4BperC	//padding in a four bytes per character environment
 * 
 * rfc_padd_4_1BperCF	//final padding in a one byte per character environment
 * rfc_padd_4_2BperCF	//final padding in a two bytes per character environment
 * rfc_padd_4_4BperCF	//final padding in a four bytes per character environment
 *
 * Please note that this macros contain the final ';' terminating the statement
 *
 *
 *----------------------------------------------------------------------------
 */
#ifdef SAPwithUNICODE
#if defined SAP_UC_is_4B 	/* unknown future */
#   define rfc_padd_4_1bperC(n)	
#   define rfc_padd_4_2bperC(n)
#   define rfc_padd_4_4bperC(n) SAP_RAW __rfc_padding4_4c##n ;
#   define rfc_padd_4_1bperCF(n,s)	
#   define rfc_padd_4_2bperCF(n,s)
#   define rfc_padd_4_4bperCF(n,s) SAP_RAW __rfc_padding4_4c##n [s];
# else				/* current SAP_UC */
#   define rfc_padd_4_1bperC(n)	
#   define rfc_padd_4_2bperC(n)	SAP_RAW __rfc_padding4_2c##n ;
#   define rfc_padd_4_4bperC(n)
#   define rfc_padd_4_1bperCF(n,s)	
#   define rfc_padd_4_2bperCF(n,s) SAP_RAW __rfc_padding4_2c##n [s];
#   define rfc_padd_4_4bperCF(n,s)
# endif
#else				/* normal char */
# define rfc_padd_4_1bperC(n)	SAP_RAW __rfc_padding4_1c##n ;
# define rfc_padd_4_2bperC(n)
# define rfc_padd_4_4bperC(n)
# define rfc_padd_4_1bperCF(n,s) SAP_RAW __rfc_padding4_1c##n [s];	
# define rfc_padd_4_2bperCF(n,s)
# define rfc_padd_4_4bperCF(n,s)
#endif


/*----------------------------------------------------------------------------
 * @func
 *
 * installs a description for a structure being used in an RFC interface.
 *
 * For functions with structured parameters or tables it is necessary to
 * install a description of the structures used to allow automatic conversion
 * of different data representations by the RFC library.
 *
 * The description must contain all (scalar) fields of the structure in correct
 * order. A type handle is returned which can be used in <t RFC_PARAMETER> and
 * <t RFC_TABLE> descriptions.
 *
 * The function will detect multiple instalation of the same structure.
 * In this case, it return the previously returned handle.
 *
 * The memory pointed to by <p elements> can be released as soon as the
 * function returns.
 *
 * Types that are instaled by <f RfcInstallStructure>/<f RfcInstallStructure2>
 * can only be used to communicate with systems that use the same # of bytes
 * per SAP_UC.
 *
 * Please <f RfcInstalUnicodeStructure> if you need cross platform connectivity.
 *
 * -i We recommend to use this function only with the NON-Unicode RFC-Library
 * -i This function will be removed from the Unicode RFC-Library in future Versions.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag  <e RFC_RC.RFC_OK> | The structure was successfully installed. The returned
 *                            type handle (<t RFC_TYPEHANDLE>) can be used in 
 *                            <t RFC_PARAMETER> or <t RFC_TABLE> arrays.
 *
 * @flag  <e RFC_RC.RFC_MEMORY_INSUFFICIENT> | Not enough memory.
 *
 *----------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcInstallStructure 
(
    CONST_RFC rfc_char_t  * name,     /* @parm the name of the structure. 
								               * Is used onlyin trace file.*/
    RFC_TYPE_ELEMENT      * elements, /* @parm description of the elements of
                                       * the structure */
    unsigned                entries,  /* @parm count of (scalar) elements in
                                       * the structure. 'elements' points to
                                       * an array of size
                                       * 'entries' * sizeof(RFC_TYPE_ELEMENT).*/
    RFC_TYPEHANDLE * pTypeHandle /* @parm returned type handle. */
);


/*----------------------------------------------------------------------------
 * @func
 *
 * same as <f RfcInstallStructure> but uses the information returned by the
 * function module RFC_GET_STRUCTURE_DEFINITION
 *
 * This is helpful for programs that do not use RFC_TYPE_ELEMENT arrays
 * that where gnerated by  rfcsdk's genh.
 * In particular, this is helpful for structures using .INCLUDE 
 * in the data dictionary.
 *
 * -i We recommend to use this function only with the NON-Unicode RFC-Library
 * -i This function will be removed from the Unicode RFC-Library in future Versions.
 *
 *----------------------------------------------------------------------------*/
RFC_RC SAP_API RfcInstallStructure2
(
    CONST_RFC rfc_char_t  * name,  /* @parm the name of the structure. Is used only
                                    * in trace file.*/
    RFC_TYPE_ELEMENT2 * elements, /* @parm description of the elements of
                                  * the structure
                                  */
    unsigned      entries,		 /* @parm count of (scalar) elements in
								  * the structure. 'elements' points to
                                  * an array of size
                                  * 'entries' * sizeof(RFC_TYPE_ELEMENT).
                                  */
    RFC_TYPEHANDLE * pTypeHandle /* @parm returned type handle. */
);


typedef RFC_BYTE RFC_STRUCT_TYPE_ID[16];
/*----------------------------------------------------------------------------
 * @func
 *
 * Installs a description for a structure being used in an RFC interface.
 *
 * For functions with structured parameters or tables it is necessary to
 * install a description of the structures used to allow automatic conversion
 * of different data representations by the RFC library.
 *
 * The description must contain all (scalar) fields of the structure in correct
 * order. A type handle is returned which can be used in <t RFC_PARAMETER> and
 * <t RFC_TABLE> descriptions.
 *
 * The function will detect multiple instalation of the same structure.
 * In this case, it return the previously returned handle.
 *
 * The memory pointed to by <p elements> can be released as soon as the
 * function returns.
 *
 * Only types installed by RfcInstallUnicodeStructure can be used
 * in an UNICODE program that communicates with a non-Unicode system.
 *
 * Types that are instaled by <f RfcInstallStructure>/<f RfcInstallStructure2>
 * can only be used to communicate with systems that use the same # of bytes
 * per SAP_UC.
 *
 * -i We recommend to use this function only with the Unicode RFC-Library
 * -i This function will be removed from the NON-Unicode RFC-Library in future Versions.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>                 | The structure was successfully installed. 
 *                                           The returned type handle (<t RFC_TYPEHANDLE>) 
 *                                           can be used in <t RFC_PARAMETER>
 *                                           or <t RFC_TABLE> arrays.
 *
 * @flag <e RFC_RC.RFC_MEMORY_INSUFFICIENT> | Not enough memory.
 *
 *----------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcInstallUnicodeStructure 
(
    const     rfc_char_t        * name,         /* @parm the name of the structure. Is used only
                                                 *       in trace file.
                                                 */
    RFC_UNICODE_TYPE_ELEMENT    * elements,     /* @parm description of the fields of
                                                 *       the structure
                                                 */
    unsigned                      entries,      /* @parm count of (scalar) elements in
                                                 *       the structure. 'elements' points to
                                                 *       an array of size
                                                 *       'entries' * sizeof(RFC_UNICODE_TYPE_ELEMENT).
                                                 */
    unsigned                      flag,         /* @parm Must be 0.
                                                 */
    RFC_STRUCT_TYPE_ID            identifier,   /* @parm optional parameter.
                                                         If the value of this parameter is not NULL, the detection
                                                         of already installed structures of the same type
                                                         is done using this value as search key and not using type description 
                                                         in 'entries' field as search key.
                                                 */
    RFC_TYPEHANDLE              * pTypeHandle  /* @parm  returned type handle. */
);

/*----------------------------------------------------------------------------
 * @func
 *
 * Convert ABAP internal type identifier into RFC type identifier.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>                 | The <p pType> contains the RFC type that
 *                                           match <p Exid>.
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER>  | <p pType> is NULL.
 * @flag <e RFC_RC.RFC_FAILURE>            | <p Exid> is invalid.
 * @flag <e RFC_RC.RFC_CALL_NOT_SUPPORTED> | <p Exid> cannot be maped to a supported
 *                                           RFCTYPE.
 *
 *----------------------------------------------------------------------------*/
RFC_RC SAP_API RfcExidToRfcType
(
    SAP_CHAR Exid,  /*@parm [in]  Internal ABAP type id*/
    RFCTYPE *pType  /*@parm [out] Matching RFCTYPE     */
);

/*----------------------------------------------------------------------------
 * @func
 *
 * Converts an RFC type identifier into a string.
 * The buffer provided to receive the value must be 
 * larger than 21.
 *
 * Installed types are displayed as hexadecimal values
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>                |  Success
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER> |  <p pString> is NULL. or ccCnt is smaller than 21
 * @flag <e RFC_RC.RFC_FAILURE>           | <p RfcType> is invalid.
 *
 *----------------------------------------------------------------------------*/
RFC_RC SAP_API RfcTypeToString
(
    RFC_TYPEHANDLE      RfcType,  /*@parm [in]  RFC type id*/
    rfc_char_t        * pString,  /*@parm [out] Buffer into which the value is writen*/
    unsigned            ccCnt     /*@parm [in]  Size from <p pString> buffer*/   
);


/* @func
 * Wraper to get structure information at runtime
 *
 * <p hRfc> must be a valid RFC_HANDLE to a connection in
 * that has the call token.  This can be a client connection
 * outside of the RfcCall/RfcReceive span, this can be a 
 * server connection within the RfcGetData/RfcSendData span.
 *
 * <p StructName> must be a structure that does not contain
 * RFCTYPE_STRING, RFCTYPE_XSTRING, RFCTYPE_XMLDATA or RFCTYPE_ITAB
 * that is it must be an ABAP TYP1 structure.
 *
 * On successfull return <p pItabStruc> will contain an ITab
 * that contains the description of the structure.
 * The caller must free that ITab when it is done with it.
 * The raws of the table have the structure <t RFC_U_FIELDS>
 *
 * This function calls the Abap-functionmodule RFC_GET_UNICODE_STRUCTURE
 * of the function group RFCH. So the user which is used by the 
 * handle hRfc should have the authorization to these function group.
 * See note 93254 for more information about the authorization.
 * 
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK> | The <p pType> contains the RFC type that match <p Exid>.
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER> | One of the [out] parameter is NULL, 
 *                     <p hRfc> is invalid.
 * @flag <e RFC_RC.RFC_EXCEPTION> | The callee has raised an exception.
 *                     The field '*exception' points to the name 
 *                     of the exception.
 * @flag <e RFC_RC.RFC_FAILURE> | An error occured, check the trace files.
 * @flag <e RFC_RC.RFC_CALL_NOT_SUPPORTED> | <p StructName> Cannot be described.
 *                     It is not active, or contains deep fields.
 *                     Check the trace files for details 
 */
RFC_RC SAP_API RfcGetStructureInfoAsTable
(
    RFC_HANDLE hRfc,         /*@parm [in]  Connection to system to query */
    SAP_CHAR  *StructName,   /*@parm [in]  The structure to describe     */
    ITAB_H    *pItabStruct,  /*@parm [out] returns an handle to an ITab 
                              * that contains the description of the structure
                              */
    ITAB_H    *pItabHeads,   /*@parm [out] reserved may be NULL.
                              */
    RFCTYPE   *pTypeKind,    /*@parm [out] If *pTypeKind is RFCTYPE_STRUCTURE,
                              *            then StrucName is an ordinary Structure.
                              *            If *pTypeKind is RFCTYPE_ITAB,
                              *            then StructName is a table type.
                              *            *pItabStruct contains the fields
                              *            from the table.
                              */
    unsigned  *pB1Slen,      /*@parm [out] Size from the structure in 
                               *            systems with 1 byte per SAP_CHAR
                              */
    unsigned  *pB2Slen,      /*@parm [out] Size from the structure in 
                              *            systems with 2 bytes per SAP_CHAR
                              */
    unsigned  *pB4Slen,      /*@parm [out] Size from the structure in 
                              *            systems with 4 bytes per SAP_CHAR
                              */
    RFC_STRUCT_TYPE_ID *pSId,/*@parm [out]16 bytes structure identifier
                              */           
    rfc_char_t **exception   /* @parm [out]pointer to exception string.
                              * only set, if the underlying calls to the SAP system
                              * returned <e RFC_RC.RFC_EXCEPTION> or
                              * <e RFC_RC.RFC_SYS_EXCEPTION>.
                              */
  
);

/* @func
 * Wraper to get structure information at runtime
 *
 * <p hRfc> must be a valid RFC_HANDLE to a connection in
 * that has the call token.  This can be a client connection
 * outside of the RfcCall/RfcReceive span, this can be a 
 * server connection within the RfcGetData/RfcSendData span.
 *
 * <p StructName> must be a structure that does not contain
 * RFCTYPE_STRING, RFCTYPE_XSTRING, RFCTYPE_XMLDATA or RFCTYPE_ITAB
 * that is it must be an ABAP TYP1 structure.
 *
 * On successfull return <p Fields> will contain an array
 * that contains the description of the structure.
 * The caller must free this array with <f RfcReleaseStructInfo>
 *
 * When accessing type 2 structures, then <p pHeads> describes the
 * the hierarchy of fields
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK> | The <p pType> contains the RFC type that
 *                     match <p Exid>.
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER> | One of the [out] parameter
                       is NULL, <p hRfc> is invalid.
 * @flag <e RFC_RC.RFC_EXCEPTION> | The callee has raised an exception.
 *                     The field '*exception' points to the name
 *                     of the exception.
 * @flag <e RFC_RC.RFC_FAILURE> | An error occured, check the trace files.
 * @flag <e RFC_RC.RFC_CALL_NOT_SUPPORTED> | <p StructName> Cannot be described.
 *                     It is not active, or contains deep fields.
 *                     Check the trace files for details 
 */
RFC_RC SAP_API RfcGetStructureInfoFlat
(
    RFC_HANDLE hRfc,         /*@parm [in]  Connection to system to query */
    SAP_CHAR  *StructName,   /*@parm [in]  The structure to describe     */
    int        bInstall,     /*@parm [in]  If not 0, RfcGetStructureInfoFlat
                              *            installs the structure and returns the 
                              *            type handle in <p phType>
                              */

    unsigned  *pCntHeads,    /*@parm [out] reserved may be NULL. */
    RFC_TYPE_HEAD **pHeads,  /*@parm [out] reserved may be NULL. */

    unsigned  *pCntFields,   /*@parm [out] Total number of fields. 
                              *            number of lines in <p Fields>.
                              */
    RFC_UNICODE_TYPE_ELEMENT ** Fields, /*@parm [out] Fields Description
                              *            There are <p pCntFields> elements
                              */
    RFCTYPE   *pTypeKind,    /*@parm [out] If *pTypeKind is RFCTYPE_STRUCTURE,
                              *            then StrucName is an ordinary Structure.
                              *            If *pTypeKind is RFCTYPE_ITAB,
                              *            then StructName is a table type.
                              *            *pItabStruct contains the fields
                              *            from the table.
                              */
    unsigned  *pB1Slen,      /*@parm [out] Size from the structure in 
                               *            systems with 1 byte per SAP_CHAR
                              */
    unsigned  *pB2Slen,      /*@parm [out] Size from the structure in 
                              *            systems with 2 bytes per SAP_CHAR
                              */
    unsigned  *pB4Slen,      /*@parm [out] Size from the structure in 
                              *            systems with 4 bytes per SAP_CHAR
                              */
    RFC_STRUCT_TYPE_ID *pSId,/*@parm [out]16 bytes structure identifier
                              */
    RFC_TYPEHANDLE *phType,  /*@parm [out] A type handlde describing <p StructName>
                              */
    rfc_char_t **exception   /* @parm [out]pointer to exception string.
                              * only set, if the underlying calls to the SAP system
                              * returned <e RFC_RC.RFC_EXCEPTION> or
                              * <e RFC_RC.RFC_SYS_EXCEPTION>.
                              */
  
);


/* @func
 * Wraper to get function information at runtime
 *
 * please note that this is only a wraper to handle the various memory
 * layout encountered behind this function, (RFC_FUNINT vares with releases)
 * In particular the offset values ar only meaningfull for scalar parameters
 * defined with TYPE.  If you need precise and efficient aligment figures
 * please do update the fields Length_b1, Lenght_b2, Length_b4 in the table
 * and pass the table to <f RfcAlignFunctionInterface> to update the 
 * Offset_b1, Offset_b2 and Offset_b4 fields.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK> | The <p pType> contains the RFC type that match <p Exid>.
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER> | one of the [out] parameter is NULL, 
 *                     <p hRfc> is invalid.
 * @flag <e RFC_RC.RFC_EXCEPTION> | The callee has raised an exception.
 *                     The field '*exception' points to the name of the exception.
 * @flag <e RFC_RC.RFC_FAILURE>  | An error occured, check the trace files.
 */
RFC_RC SAP_API RfcGetFunctionInfoAsTable
(
    RFC_HANDLE hRfc,        /*@parm [in]  Connection to system to query */
    SAP_CHAR  *FunctionName,/*@parm [in]  The function to describe     */
    ITAB_H    *pItabFunInfo,/*@parm [out] returns an handle to an ITab 
                             * that contains the description of the function
                             * the line of the function are of type <t RFC_U_FUNINT>
                             */
    rfc_char_t **exception  /* @parm [out]pointer to exception string.
                             * only set, if the underlying calls to the SAP system
                             * returned <e RFC_RC.RFC_EXCEPTION> or
                             * <e RFC_RC.RFC_SYS_EXCEPTION>.*/
);

/*@func 
 * Release the memory alocated by <f RfcGetStructureInfoFlat>
 */
void SAP_API RfcReleaseStructInfo
(
    RFC_TYPE_HEAD            *pHeads, /*@parm [in] array of headers allocated by
                                       * <f RfcGetStructureInfoFlat>. 
                                       *May be NULL */
    RFC_UNICODE_TYPE_ELEMENT *pFields /*@parm [in] array of fields allocated by
                                       * <f RfcGetStructureInfoFlat>
                                       * May be NULL */
);

/* @func
 * Helper to finalize scratch buffer memory layout
 *
 * Some applications use the field OFFSET from the RFC_FUNINT returned
 * by RFC_GET_FUNCTION_INTERFACE to layout a scatch buffer to hold the 
 * values passed to the RFC_PARAMETER arrays.  
 *
 * Since RFC_GET_FUNCTION_INTERFACE only partially fills the RFC_FUNINT
 * table, the application must fill the EXID and the Length_b1, Lenght_b2,
 * and fields.  The application can then pass the table to 
 * RfcAlignFunctionInterface to have the fields Offset_b1, Offset_b2 and 
 * finalized.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK> | The <p pType> contains the RFC type that 
 *                           match <p Exid>.
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER> | One of the [out] parameter 
 *                           is NULL, <p hRfc> is invalid.
 * @flag <e RFC_RC.RFC_FAILURE> | An error occured, check the trace files.
 */
RFC_RC SAP_API RfcAlignFunctionInterface
(
    RFC_HANDLE hRfc,        /*@parm [in] Connection to system to query */
    SAP_CHAR  *FunctionName,/*@parm [in]  The function to describe     */
    ITAB_H     ItabFunInfo  /*@parm [in] A table of <t RFC_U_FUNINT>
                             * with completed Exid, Length_b1, Lenght_b2,
                             * and Length_b4 fields.
                             */    
);


/* -------------------------------------------------------------------
 * security and automatic (re)connection
 * ------------------------------------------------------------------*/

typedef RFC_BYTE RFC_TICKET_CHECKSUM[20];   /* SHA checksum */
typedef RFC_BYTE RFC_TICKET_SYSNAME[8];

typedef struct
{
                                        /* position     */
   RFC_BYTE     head    [4];            /* 0-4          */
   RFC_BYTE     unused  [8];            /* 4-12         */
   RFC_BYTE     ip      [4];            /* 12-16        */
   RFC_BYTE     sysnr   [1];            /* 16-17        */
   RFC_BYTE     client  [3];            /* 17-20        */
   RFC_TICKET_SYSNAME   sysname;        /* 20-28        */
   RFC_BYTE     userid  [14];           /* 28-42        */
   RFC_BYTE     valid   [2];            /* 42-44        */
   RFC_TICKET_CHECKSUM  check;          /* 44-64        */
                                        /* size 64      */
}
RFC_TICKET;

/* for internal use only */
#define NEWITAB_SHARE   0
#define NEWITAB_POP     1
#define NEWITAB_KEEP    2
#define NEWITAB_BYVALUE 3
#define NEWITAB_COPY    4


/* @type RFC_FUNCTIONNAME | RFC function name.
 * Character field big enough containing the name of any
 * ABAP/4 function module.
 */
#define RFC_FUNCTIONNAME_LN   31
typedef rfc_char_t RFC_FUNCTIONNAME[RFC_FUNCTIONNAME_LN];


/* RFC Transaction Identifier (TID) */
#define RFC_TID_LN_CHAR         24
#define RFC_TID_LN_RAW          RFC_TID_LN_CHAR*sizeofR(SAP_UC)
#define RFC_TID_LN              RFC_TID_LN_CHAR           /* Offsetof (ARFCSDATA, arfcdest)    */

#define RFC_CALLID_LN           64                        /* Offsetof (ARFCSDATA, arfcblcnt)   */
#define ARFCSDATA_HEADER_LN     68                        /* Offsetof (ARFCSDATA, arfcdata01)  */

/* @type RFC_TID |
 * zero terminated string giving  an RFC transaction a globally unique
 * identifier.
 */
typedef rfc_char_t RFC_TID[RFC_TID_LN+1];

#define RFC_QNAME_LN          24*sizeofR(SAP_UC)
#define RFC_QNAME_LN_CHAR     24
#define RFC_QNAME_LN_RAW      RFC_QNAME_LN_CHAR*sizeofR(SAP_UC)


#define RFC_QCOUNT_LN         24*sizeofR(SAP_UC)
#define RFC_QCOUNT_LN_CHAR    24
#define RFC_QCOUNT_LN_RAW     RFC_QCOUN_LN_CHAR*sizeofR(SAP_UC)

/* @type RFC_QNAME |
 * zero terminated string giving a queue name for using the receive queue in
 * SAP system.
 */
typedef rfc_char_t            RFC_QNAME[RFC_QNAME_LN_CHAR+1];

typedef rfc_char_t            RFC_QCOUNT[RFC_QCOUNT_LN_CHAR+1];


/*----------------------------------------------------------------------
 * @func
 * After being started, an RFC server program can issue this call
 * to check whether it is running in an SNC environment.
 *
 * If working with SNC an RFC server program can issue
 * <f RfcSncPartnerName> or <f RfcSncPartnerAclKey> to get the
 * SNC name or the SNC ACL key of the RFC client and can then
 * decide to accept the communication with this partner.
 *
 * An RFC server program can only
 * issue this call after a successful return of <f RfcListen> or
 * <f RfcWaitForRequest> or <f RfcGetName> or <f RfcDispatch>.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If call successful.
 * @flag <e RFC_RC.RFC_FAILURE> | If any error has occurred.
 *
 */
DECL_EXP RFC_RC SAP_API RfcSncMode
(
 RFC_HANDLE       handle,          /* @parm connection handle.*/
 RFC_SNC_MODE *   snc_mode         /* @parm SNC mode for working with or without SNC */
);




/*----------------------------------------------------------------------
 * @func
 *
 * If the <t RFC_SNC_MODE> is on, an RFC server program can issue
 * this call to get the SNC name of the RFC client.
 * It can then decide itself whether it can communicate with this
 * RFC client.
 *
 * An RFC server program can only
 * issue this call after a successful return of <f RfcListen> or
 * <f RfcWaitForRequest> or <f RfcGetName> or <f RfcDispatch>.
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If successful.
 * @flag <e RFC_RC.RFC_FAILURE> | If any error has occurred.
 *
 *----------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcSncPartnerName
(
 RFC_HANDLE       handle,				/* @parm connection handle.*/
 rfc_char_t *     partner_name,			/* @parm buffer for SNC name of
										 * the RFC client.
                                         */
 RFC_INT          maxlen				/* @parm Max. length of buffer.*/
);




/*----------------------------------------------------------------------
 * @func
 * Instead of getting the SNC name an RFC server program can issue
 * this call to get the SNC ACL key of the RFC client.
 *
 * See <f RfcSncPartnerName> for more details.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If call successful
 * @flag <e RFC_RC.RFC_FAILURE> | Otherwise.
 *
 *------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcSncPartnerAclKey
(
 RFC_HANDLE       handle,          /* @parm connection handle. */
 RFC_BYTE *       partner_aclkey,  /* @parm buffer for SNC ACL key of
									* the RFC client.
                                    */
 RFC_INT          maxlen,          /* @parm Max. length of buffer.*/
 RFC_INT *        aclkey_len       /* @parm Real length of the SNC ACL key.*/
);


/*----------------------------------------------------------------------
 * @func
 * An administrative program can issue this call to convert an SNC name
 * in an SNC ACL key without working with RFC.
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If call successful
 * @flag <e RFC_RC.RFC_FAILURE> | Otherwise.
 *
 *---------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcSncNameToAclKey
(
 rfc_char_t            * snc_lib,         /* @parm name of the SNC library
										   * on the local system.
										   */
 rfc_char_t            * snc_name,        /* @parm SNC name to be converted */
 RFC_BYTE        * snc_aclkey,			  /* @parm buffer for SNC ACL key */
 RFC_INT           maxlen,				  /* @parm Max. length of buffer. */
 RFC_INT         * aclkey_len,			  /* @parm Real length of the SNC ACL key.*/
 RFC_ERROR_INFO_EX * error_info           /* @parm structure <t RFC_ERROR_INFO_EX>
										   * describing the error
                                           */
);




/*----------------------------------------------------------------------
 * @func
 * An administrative program can issue this call to convert an
 * SNC ACL key in an SNC name without working with RFC.
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If call successful
 * @flag <e RFC_RC.RFC_FAILURE> | Otherwise.
 *
 *--------------------------------------------------------------------------*/
RFC_RC SAP_API RfcSncAclKeyToName
(
 rfc_char_t      * snc_lib,         /* @parm name of the SNC library on the local system.*/
 RFC_BYTE        * snc_aclkey,      /* @parm SNC ACL key to be converted */
 RFC_INT           aclkey_len,      /* @parm Length of the SNC ACL key. */
 rfc_char_t      * snc_name,        /* @parm SNC name to be converted */
 RFC_INT           maxlen,          /* @parm Max. length of buffer. */
 RFC_ERROR_INFO_EX * error_info     /* @parm structure
				                     * <t RFC_ERROR_INFO_EX> describing the error
                                     */
);



/* ------------------------------------------------------------------
 * Standard RFC interface
 * ----------------------------------------------------------------*/


/* -----------------------------------------
 * calling function modules
 * ---------------------------------------*/

/* @func
 *
 * Obsolet. Please use <f RfcCallEx> instead.
 *
 * Calls an ABAP/4 function module via RFC.
 *
 * The structures <t RFC_PARAMETER> and  <t RFC_TABLE> are the
 * importing ( 'exporting'  in ABAP convention) parameters and tables of the called function module.
 * The function returns immediately after the call request is send.
 * If the function returns RFC_OK, there is no guaranty, that
 * the call was already received by the target system.
 *
 * @rdesc
 * returns RFC_OK or RFC_FAILURE.
 *
 * @xref <f RfcCallEx>, <f RfcCallReceive>, <f RfcCallReceiveEx>, <f RfcReceive> <f RfcReceiveEx>.
 *
 * @ex calling the function module  RFC_SYSTEM_INFO. |
 *
 *   RFC_RC             rfc_rc;
 *   RFC_PARAMETER      exporting[32];
 *   RFC_TABLE          tables[32];
 *
 *   exporting[0].name = NULL;
 *   tables[0].name    = NULL;
 *
 *   rfc_rc = RfcCall( handle, "RFC_SYSTEM_INFO",
 *                                  exporting,
 *                                  tables );
 *   if( rfc_rc != RFC_OK )
 *   {
 *      ....
 *
 *------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcCall 
(
 RFC_HANDLE            handle,    /* @parm connection handle.*/
 CONST_RFC rfc_char_t *function,  /* @parm function module to call.*/
 RFC_PARAMETER        *parameters,/* @parm 'exporting' parameters. */
 RFC_TABLE            *tables     /* @parm 'tables' parameters.*/
);


/*----------------------------------------------------------------------
 * @func
 *
 * Obsolete. Please use <f RfcReceiveEx> instead.
 *
 * receives the return values from a function call issued by <f RfcCall>.
 *
 * allows to receive the answer to a RFC call and must be called
 * after RfcCall was issued. The tables' description ( <t RFC_TABLE> )
 * must be identical to the one used in RfcCall.
 *
 * The function RfcReceive waits till the answer is received.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK> | The call was successfully completed and
 *       the values of the returned parameters were filled into the
 *       fields being supplied by the <t RFC_PARAMETER> array.
 * @flag <e RFC_RC.RFC_FAILURE> | An internal error has occurred.
 *       <f RfcLastError> or <f RfcLastErrorEx> may give more information.
 * @flag <e RFC_RC.RFC_EXCEPTION> | The callee has raised an exception.
 *       The field '*exception' points to the name of the exception. No
 *       data were transported.
 * @flag <e RFC_RC.RFC_SYS_EXCEPTION> | The local or remote RFC system has
 *       raised an exception. The parameter '*exception' points to the name of
 *       the exception. The connection was automatically closed by the
 *       system and <f RfcLastError> or <f RfcLastErrorEx> may give more
 *       information on the origin
 *       of the error. Two exceptions may occur now: SYSTEM_FAILURE and
 *       COMMUNICATION_FAILURE.
 * @flag <e RFC_RC.RFC_CALL> | The callee has issued a RFC call to the
 *       caller of RfcReceive. No data are transported. The call request
 *       must be handled by using the functions <f RfcDispatch> or by
 *       <f RfcGetName>, <f RfcGetData> and <f RfcSendData>
 *       before another call to RfcReceive can be done.
 *
 * @xref <f RfcCall>, <f RfcCallReceive>, <f RfcCallEx>, <f RfcReceiveEx>, <f RfcCallReceiveEx>.
 */
DECL_EXP RFC_RC SAP_API RfcReceive
(
  RFC_HANDLE       handle,   /* @parm connection handle.
                              */
  RFC_PARAMETER *  parameters,/* @parm 'importing' parameters.
                               */
  RFC_TABLE     *  tables,   /* @parm 'tables' parameters.
                              */
  rfc_char_t **    exception /* @parm output parameter:
                              * pointer to exception string.
                              * only set, if <e RFC_RC.RFC_EXCEPTION> or
                              * <e RFC_RC.RFC_SYS_EXCEPTION> is returned.
                              */
 );


/*---------------------------------------------------------------------
 *
 * @func
 *
 * Obsolete. Please use <f RfcCallReceiveEx> instead.
 *
 * calls an ABAP/4 function module and receives the return values
 * in one function call.
 *
 * @rdesc Returns one of the following values:
 *
 *
 * @flag <e RFC_RC.RFC_OK> |
 *       The call was successfully completed and
 *       the values of the returned parameters were filled into the
 *       fields being supplied by the <t RFC_PARAMETER> array.
 *
 * @flag <e RFC_RC.RFC_FAILURE> |
 *       An internal error has occurred.
 *       <f RfcLastError> or <f RfcLastErrorEx> may give more information.
 *
 * @flag <e RFC_RC.RFC_EXCEPTION> |
 *       The callee has raised an exception.
 *       The field '*exception' points to the name of the exception. No
 *       data were transported.
 *
 * @flag <e RFC_RC.RFC_SYS_EXCEPTION> |
 *       The local or remote RFC system has
 *       raised an exception. The parameter '*exception' points to the name of
 *       the exception. The connection was automatically closed by the
 *       system and <f RfcLastError>  or <f RfcLastErrorEx> may give more
 *       information on the origin
 *       of the error. Two exceptions may occur now: SYSTEM_FAILURE and
 *       COMMUNICATION_FAILURE.
 *
 * @flag <e RFC_RC.RFC_CALL> |
 *       The callee has issued an RFC call to the
 *       caller of RfcReceive. No data are transported. The call request
 *       must be handled by using the functions <f RfcDispatch> or by
 *       <f RfcGetName>, <f RfcGetData> and <f RfcSendData>
 *       before an other call to RfcReceive can be done.
 *
 *
 * @xref <f RfcCall>, <f RfcCallReceive>, <f RfcCallEx>, <f RfcReceiveEx>, <f RfcCallReceiveEx>.
 *
 *---------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcCallReceive (
 RFC_HANDLE       handle,    /* @parm connection handle.
                              */
 CONST_RFC rfc_char_t  *function,  /* @parm function module to call.
                              */
 RFC_PARAMETER *  exporting, /* @parm 'exporting' parameters.
                              */
 RFC_PARAMETER *  importing,/* @parm 'importing' parameters.
                              */
 RFC_TABLE     *  tables,   /* @parm 'tables' parameters.
                             */
 rfc_char_t   **  exception /* @parm output parameter:
                             * pointer to exception string.
                             * only set, if <e RFC_RC.RFC_EXCEPTION> or
                             * <e RFC_RC.RFC_SYS_EXCEPTION> is returned.
                             */
 );


/*--------------------------------------------------------------------
 * @func
 *
 *
 * ===========================================================
 *
 *   Please use <f RfcIndirectCallEx> instead of this function.
 *  
 *   Unicode RFC Library does not support this call.
 *
 * ===========================================================
 *
 *
 * calls a function module in R/3 indirectly.
 *
 * With this function the call of a function module in R/3 will use the
 * transactional RFC interface in R/3.
 *
 * Importing parameters are not supported.
 *
 * If an error occurs (almost communication error) the RFC client program
 * has to reconnect to R/3 later and repeat this RFC call with the specific
 * TransID. It must NOT create a new TransID via <f RfcCreateTransID>.
 *
 * If the tRFC client program takes care of the TID management and repeats
 * this RFC call in error cases the function module will be executed
 * exactly once in R/3.
 *
 * @xref <f RfcCreateTransID>, <f RfcIndirectCallEx>
 *-------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcIndirectCall
(
 RFC_HANDLE       handle,    /* @parm connection handle        */
 rfc_char_t *     function,  /* @parm function module to call  */
 RFC_PARAMETER *  exporting, /* @parm 'exporting' parameters   */
 RFC_TABLE     *  tables,    /* @parm 'tables' parameters      */
 RFC_TID          tid        /* @parm <t RFC_TID>              */
);

/*--------------------------------------------------------------------------------------------
 *
 * @func
 *
 * calls a function module in R/3 indirectly (using the tRFC interface
 * in R/3.
 *
 * Importing parameters are not supported.
 *
 * In error cases, the tRFC client
 * has to reconnect to R/3 later and repeat this RFC call with the specific
 * TransID. It must NOT create a new TransID via <f RfcCreateTransID>.
 *
 * After this call is returned without any error, the tRFC client must
 * issue <f RfcConfirmTransID> to confirm the executed function module.
 *
 * If the tRFC client program takes care of the TID management and repeats
 * this RFC call in error cases, the according function module will be executed
 * exactly once in R/3.
 *
 * Even if the tRFC client is broken down after this call is
 * successfully returned and before the update in the own TID management,
 * it can repeat this call with the same TID. The R/3 system will check
 * whether this transaction is already executed or not before the according
 * function module will be called.
 *
 * This is why we recommend you to use this new call instead of
 * <f RfcIndirectCall>.
 *
 * @xref <f RfcCreateTransID>
 *-------------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcIndirectCallEx
(
 RFC_HANDLE       handle,    /* @parm connection handle        */
 rfc_char_t *     function,  /* @parm function module to call  */
 RFC_PARAMETER *  exporting, /* @parm 'exporting' parameters   */
 RFC_TABLE     *  tables,    /* @parm 'tables' parameters      */
 RFC_TID          tid        /* @parm <t RFC_TID>              */
);



/*---------------------------------------------------------------------------------
 *
 * @func
 *
 * When using this function instead of <f RfcIndirectCallEx> the called
 * function module in R/3 should not be immediately executed, it will be inserted in
 * the receive queue from within R/3 defined by the parameter qname.
 *
 * Backend defines qcounter based on the arrival time of the request. 
 *
 * The processing of the queue can be activated by registering of the queuename 
 * using transaction "smqr" in the backend-system 
 * Furthermore it can be started using R/3 function module 
 * "TRFC_QIN_RESTART" either from within R/3 or from a remote system via RFC, then and only then
 * if queue name is not registered in "smqr".
 *
 * Calling the function module "TRFC_QIN_RESTART" from a batch job, you can let
 * all according function modules in a receive queue run in a batch process and
 * you won't have problem with the max. ABAP-timeout in a dialog work process when
 * using RFC or tRFC with RfcIndirectCall or RfcIndirectCallEx.
 *
 * Importing parameters are not supported.
 *
 * In error cases, the tRFC client has to reconnect to R/3 later and repeat this
 * RFC call with the specific TransID. It must NOT create a new TransID via
 * <f RfcCreateTransID>.
 *
 * After this call is returned without any error, the tRFC client must
 * issue <f RfcConfirmTransID> to confirm the executed function module.
 *
 * If the tRFC client program takes care of the TID management and repeats
 * this RFC call in error cases, the according function module will be executed
 * exactly once in R/3.
 *
 * Even if the tRFC client is broken down after this call is
 * successfully returned and before the update in the own TID management,
 * it can repeat this call with the same TID. The R/3 system will check
 * whether this transaction is already executed or not before the according
 * function module will be called.
 *
 * Warning: Parameter qcount does not impact the processing of the call. Backend simple ignore it.
 *
 * @xref <f RfcCreateTransID>
 *---------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcQueueInsert(
 RFC_HANDLE       handle,    /* @parm connection handle        */
 rfc_char_t *     function,  /* @parm function module to call  */
 RFC_PARAMETER *  exporting, /* @parm 'exporting' parameters   */
 RFC_TABLE     *  tables,    /* @parm 'tables' parameters      */
 RFC_QNAME        qname,     /* @parm Queue name               */
 RFC_INT          qcount,    /* @parm Obsolete                 */
 RFC_TID          tid        /* @parm <t RFC_TID>              */
 );



/*----------------------------------------------------------------------
 *
 * @func
 * The function RfcGetName can be used to get the name of the called function
 * directly without function registration (via <f RfcInstallFunction>) and without
 * call dispatching (via <f RfcDispatch>). 
 * The calling program then has to determine the interface of the
 * requested function module and to receive the parameters as within
 * a function being installed via <f RfcInstallFunction>.
 *
 * If no error occurs, the returned function name will be a zero
 * terminated string.
 *
 * In register mode (s. <f RfcAccept>) the server program will wait for
 * the next RFC requests.
 *
 * @comm
 *
 * With RfcGetName it is not possible to receive a RFC function name longer
 * that 30 chars. For longer function name use <f RfcGetLongName> instead.
 *
 * RfcGetName can also be used within the callback function "%%USER_GLOBAL_SERVER"
 * (s. <f RfcInstallFunction>) to find the name of the called function.
 *
 * RfcGetName is a blocking call in "register"-mode.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If successful
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 * @xref <f RfcGetNameEx>, <f RfcDispatch>, <f RfcInstallFunction>, <f RfcGetData>, <f RfcSendData>
 *---------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcGetName
(
  RFC_HANDLE   handle,           /* @parm RFC connection handle */
  RFC_FUNCTIONNAME functionname  /* @parm output parameter,
                                  * name of the called function module
                                  * ( <t RFC_FUNCTIONNAME> ).
                                  */
);



/*----------------------------------------------------------------------
 * @func
 *
 * The function RfcGetLongName can be used to get the name of the called function
 * directly without function registration (via <f RfcInstallFunction>) and without
 * call dispatching (via <f RfcDispatch>). 
 * Unlike <f RfcGetName>, RfcGetLongName also works
 * with called function names longer that 30 chars.
 * The calling program then has to determine the interface of the
 * requested function module and to receive the parameters as within
 * a function being installed via <f RfcInstallFunction>.
 *
 * If no error occurs, the returned function name will be a zero
 * terminated string.
 *
 * In register mode (s. <f RfcAccept>) the server program will wait for
 * the next RFC requests.
 *
 * @comm
 *
 * RfcGetLongName can not be used inside of the callback function
 * "%%USER_GLOBAL_SERVER". Use instead <f RfcGetName>.
 *
 * RfcGetLongName is a blocking call in "register"-mode.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If successful
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 * @xref <f RfcGetLongNameEx>, <f RfcDispatch>, <f RfcInstallFunction>, <f RfcGetData>, <f RfcSendData>
 *---------------------------------------------------------------------*/
RFC_RC SAP_API RfcGetLongName
(
  RFC_HANDLE   handle,           /* @parm RFC connection handle */
  rfc_char_t*  functionname, 	 /* @parm output parameter,
                                  * name of the called function module
                                  * ( <t RFC_FUNCTIONNAME> ).
                                  */
  unsigned	   fn_length	     /* @parm input parameter length of functionname */
);



/*----------------------------------------------------------------------
 * @func
 * RfcGetNameEx acts like <f RfcGetName> but it returns the control
 * back after each RFC system call.
 * If a RFC system call such as RFC_PING for connection test
 * is incoming, it will be automatically executed by the RFC library, and 
 * after that RfcGetNameEx does return the control to the RFC program.
 * In this case, <e RFC_RC.RFC_SYSTEM_CALLED> will be returned
 * and the RFC program must check this code and repeat the call.
 *
 * @rdesc
 * <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE> or <e RFC_RC.RFC_SYSTEM_CALLED>.
 *
 * @xref <f RfcGetName>, <f RfcDispatch>, <f RfcInstallFunction>, <f RfcGetData>, <f RfcSendData>
 *---------------------------------------------------------------------*/
RFC_RC SAP_API RfcGetNameEx
(
  RFC_HANDLE   handle,           /* @parm RFC connection handle */
  RFC_FUNCTIONNAME functionname  /* @parm output parameter,
                                  * name of the called function module
                                  * ( <t RFC_FUNCTIONNAME> ).
                                  */
);



/*----------------------------------------------------------------------
 * @func
 * RfcGetLongNameEx acts like <f RfcGetLongName> but it returns the control
 * back after each RFC system call.
 * If a RFC system call such as RFC_PING for connection test
 * is incoming, it will be automatically executed by the RFC library, and 
 * after that RfcGetLongNameEx does return the control to the RFC program.
 * In this case, <e RFC_RC.RFC_SYSTEM_CALLED> will be returned
 * and the RFC program must check this code and repeat the call.
 *
 * @rdesc
 * <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE> or <e RFC_RC.RFC_SYSTEM_CALLED>.
 *
 * @xref <f RfcLongGetName>, <f RfcDispatch>, <f RfcInstallFunction>, <f RfcGetData>, <f RfcSendData>
 *---------------------------------------------------------------------*/
RFC_RC SAP_API RfcGetLongNameEx
(
  RFC_HANDLE   handle,           /* @parm RFC connection handle */
  rfc_char_t*  functionname,     /* @parm output parameter,
                                  * name of the called function module
                                  * ( <t RFC_FUNCTIONNAME> ).
                                  */
 unsigned fn_length		         /* @parm length of function name */
);



/*---------------------------------------------------------------------
 * @func
 *
 * Within a function registered via <f RfcInstallFunction>
 * or after receiving the name of the
 * called function by <f RfcGetName> the function RfcGetData can be
 * used to receive the parameters of the function call.
 *
 * Here the <t ITAB_H> field in the <t RFC_TABLE> record has to be
 * initialized to NULL. The function RfcGetData fills in the
 * corresponding table handle, which is either a newly created table
 * or an already existing table, which was send to the caller via
 * another RFC call.  The field itmode in an <t RFC_TABLE> record
 * determines if a received table is passed by reference or by
 * value.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If successful
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 *---------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcGetData 
(
  RFC_HANDLE   handle,          /* @parm RFC connection handle */
  RFC_PARAMETER *  parameters,  /* @parm 'importing' parameters ( <t RFC_PARAMETER> ).*/
  RFC_TABLE     *  tables       /* @parm 'tables' parameters ( <t RFC_TABLE> ). */
);



/*---------------------------------------------------------------------
 * @enum RFC_ONGETPARAMETER_ACTION |
 *
 *   Possibly return values for <f RFC_ONGETPARAMETER> function.
 *
 * @xref <f RFC_ONGETPARAMETER_ACTION>, <f RfcDynamicGetData>.
 *---------------------------------------------------------------------*/
typedef enum
{
    RFC_ONGETPARAMETER_ACTION_NOT_FOUND,        /* @emem any error has  occurred */
    RFC_ONGETPARAMETER_ACTION_SINGLE_PARAMETER, /* @emem a single parameter have been received */
    RFC_ONGETPARAMETER_ACTION_SINGLE_TABLE,     /* @emem a single table have been received */
    RFC_ONGETPARAMETER_ACTION_ALL               /* @emem all parameters have been received */
} RFC_ONGETPARAMETER_ACTION;




/*------------------------------------------------------------------------
 * @cb RFC_ONGET_PARAMETER_ACTION |		RFC_ONGETPARAMETER |
 *
 * Prototype for callback function, pointer on which should be used as one of parameters
 * by <f RfcDynamicGetData>.
 *------------------------------------------------------------------------*/
typedef RFC_ONGETPARAMETER_ACTION ( DLL_CALL_BACK_FUNCTION_PTR RFC_ONGETPARAMETER )
        (
		  void           *context,             /* @parm pointer on a data area contains parameter */
          rfc_char_t     *parameterName,       /* @parm name of parameter, restored by <f RfcDynamicGetData> */
          unsigned        parameterNameLength, /* @parm name's length of parameter, restored by <f RfcDynamicGetData> */
          RFC_PARAMETER **parameters,          /* @parm pointer on parameters array */
          RFC_TABLE     **tables               /* @parm pointer on tables array */
		);




/*--------------------------------------------------------------------
 * @func
 *
 * A callback in order to support filtering/preprocessing of parameters.
 * This API retrieves parameters one by one.
 *
 * When you first call this API you pass it a callback function. As the library is
 * unmarshaling parameters and tables, it calls the callback function with the
 * parameter name, and the callback function essentially returns an associated
 * RFC_PARAMETER or RFC_TABLE.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If call succesful.
 * @flag <e RFC_RC.RFC_FAILURE> | If call fails.
 *
 * @xref
 * <f RFC_ONGETPARAMETER>, <f RFC_ONGETPARAMETER_ACTION>.
 *
 * @ex Using RfcDynamicGetData |
 *
 *   ....
 *
 * typedef struct
 * {
 *  RFC_INT	sum1;
 *  RFC_INT	sum2;
 *
 *  RFC_PARAMETER  rp;
 *  RFC_TABLE	tables[2];
 *
 * } PT_CTX;
 *
 * static RFC_ONGETPARAMETER_ACTION onGetParameter
 *                                   (void*, char*, unsigned, RFC_PARAMETER**, RFC_TABLE**);
 *
 * static RFC_RC SAP_API abc_function(RFC_HANDLE rfc_handle);
 *
 *  ....
 *
 * int main (int argc, char** argv)
 * {
 *  RFC_HANDLE 	rfc_handle;
 *  RFC_RC		rfc_rc;
 *  char*		docu;
 *
 *
 *
 *  printf ("Accept Rfc Connection ... ");
 *  rfc_handle = RfcAccept (argv);
 *  if (rfc_handle == RFC_HANDLE_NULL) rfc_error ("RfcAccept");
 *  printf ("Ok\n");
 *
 *
 *
 *  printf ("RfcInstallFunction ABCD ...");
 *  rfc_rc = RfcInstallFunction ("ABCD", abc_function, docu);
 *  if (rfc_rc != RFC_OK) rfc_error("Install Function ABC ");
 *  printf ("Ok\n");
 *
 *
 *  do
 *  {
 *    if (rfc_rc == RFC_OK) printf ("RfcDispatch called\n");
 *      rfc_rc = RfcDispatch (rfc_handle);
 *  } while (rfc_rc == RFC_OK);
 *
 *  RfcClose (rfc_handle);
 *
 *  printf ("Server Ok\n");
 *
 *  return 0;
 * }
 *
 * static RFC_RC abc_function(RFC_HANDLE rfc_handle)
 * {
 *  RFC_RC rfc_rc;
 *  RFC_PARAMETER  export[2];
 *  RFC_INT	result;
 *  PT_CTX		ctx;
 *
 *  int i;
 *
 *
 *  printf ("RfcDynamicGetData ... ");
 *  fflush(stdout);
 *  rfc_rc = RfcDynamicGetData (rfc_handle,
 *                      	     onGetParameter,
 *                       	     (void*) &ctx);
 *  if (rfc_rc != RFC_OK)
 *  {
 *   rfc_error ("RfcDynamicGetData");
 *   exit (1);
 *  }
 *  printf ("OK\n");
 *
 *
 *  printf ("Received import parameter sum1 has value:  %d\n", ctx.sum1);
 *  printf ("Received import parameter sum2 has value:  %d\n", ctx.sum2);
 *  printf ("Received table has name: %s \n", ctx.tables[0].name);
 *  printf ("Received table has content:\n");
 *  display_table (ctx.tables[0].ithandle);
 *
 *
 *  printf ("Assign to parameter result the value of (sum1 + sum2)\n");
 *  result = ctx.sum1 + ctx.sum2;
 *
 *  printf ("Insert new entries into table %s ... ", ctx.tables[0].name);
 *  update_table(ctx.tables[0].ithandle);
 *  printf ("Ok\n");
 *
 *  ctx.tables[1].name = NULL;
 *
 *  export[0].name  = "RESULT";
 *  export[0].nlen  = strlen (export[0].name);
 *  export[0].type  = RFCTYPE_INT;
 *  export[0].addr  = &result;
 *  export[0].leng  = sizeof (result);
 *
 *  export[1].name = NULL;
 *
 *
 *  printf ("RfcSendData ... ");
 *  rfc_rc = RfcSendData (rfc_handle,
 * 		       export,
 * 		       ctx.tables);
 *  if (rfc_rc != RFC_OK)
 *  {
 *   rfc_error ("RfcSendData");
 *   exit (1);
 *  }
 *  printf ("Ok\n");
 *
 *
 *  return RFC_OK;
 * }
 *
 *
 * static RFC_ONGETPARAMETER_ACTION onGetParameter
 * ( void* context,
 *   char* parameterName,
 *   unsigned leng,
 *   RFC_PARAMETER** parameters,
 *   RFC_TABLE** tables
 * )
 * {
 *  PT_CTX 	*pCTX;
 *  RFC_PARAMETER	**ppParam;
 *  RFC_TABLE      **ppTable;
 *
 *  pCTX = (PT_CTX*) context;
 *
 *  if ( (memcmp ("sum1", parameterName, leng) == 0) || (memcmp ("SUM1", parameterName, leng) == 0) )
 *  {
 *   *parameters = &(pCTX->rp);
 *   *tables = NULL;
 *
 *   pCTX -> rp.name = "sum1";
 *   pCTX -> rp.nlen = 4;
 *   pCTX -> rp.type = TYPINT;
 *   pCTX -> rp.addr = &(pCTX->sum1);
 *   pCTX -> rp.leng = sizeof (TYPINT);
 *
 *   return RFC_ONGETPARAMETER_ACTION_SINGLE_PARAMETER;
 *  }
 *
 *  if ( (memcmp ("sum2", parameterName, leng) == 0) || (memcmp ("SUM2", parameterName, leng) == 0) )
 *  {
 *   *parameters = &(pCTX->rp);
 *   *tables = NULL;
 *
 *   pCTX -> rp.name = "sum2";
 *   pCTX -> rp.nlen = 5;
 *   pCTX -> rp.type = TYPINT;
 *   pCTX -> rp.addr = &(pCTX->sum2);
 *   pCTX -> rp.leng = sizeof (TYPINT);
 *
 *   return RFC_ONGETPARAMETER_ACTION_SINGLE_PARAMETER;
 *  }
 *
 *  if (memcmp ("ITAB1000", parameterName, leng) == 0)
 *  {
 *   *tables = pCTX->tables;
 *   *parameters = NULL;
 *
 *   pCTX -> tables[0].name = "ITAB1000";
 *   pCTX -> tables[0].nlen = 8;
 *   pCTX -> tables[0].type = TYPC;
 *   pCTX -> tables[0].leng = 1000;
 *   pCTX -> tables[0].ithandle = ITAB_NULL;
 *   pCTX -> tables[0].itmode = RFC_ITMODE_BYREFERENCE;
 *
 *   return RFC_ONGETPARAMETER_ACTION_SINGLE_TABLE;
 *  }
 *
 *  return RFC_ONGETPARAMETER_ACTION_NOT_FOUND;
 *
 * }
 *
 *
 *----------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcDynamicGetData 
(
 RFC_HANDLE           handle,         /* @parm RFC connection handle. */
 RFC_ONGETPARAMETER   onGetParameter, /* @parm pointer on user implemented
									   * function which restores the parameter
									   * according their names.
									   */
 void                *context		  /* @parm data area containing parameter have to be received. */
);








/*------------------------------------------------------------------------
 *
 * @func
 *
 * This function is used to send back the answer of a Remote Function Call
 * to a caller .
 *
 * The tables description ( <t RFC_TABLE> ) must be the same than in the
 * previous RfcGetData call.
 *
 * @rdesc
 *
 * <e RFC_RC.RFC_OK> if call successful or <e RFC_RC.RFC_FAILURE> elsewhere.
 *------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcSendData 
(
  RFC_HANDLE   handle,			/* @parm RFC connection handle  */
  RFC_PARAMETER *  parameters,	/* @parm 'exporting' parameters */
  RFC_TABLE     *  tables		/* @parm 'tables' parameters.
                                 * Must be the same structure as passed to
                                 * <f RfcGetData>.
                                 */
);



/*------------------------------------------------------------------------
 * @func
 * This function is used to raise an exception while processing a
 * Remote Function Call.
 *
 * If you are having tables parameters in your function module, use
 * <f RfcRaiseTables> instead.
 *
 * @rdesc
 * <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE>.
 *
 * @xref
 * <f RfcRaiseTables>
 *------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcRaise 
(
 RFC_HANDLE      handle,     /* @parm RFC connection handle */
 rfc_char_t *    exception   /* @parm exception to be raised
                              * (zero terminated string).
                              */
);




/*------------------------------------------------------------------------
 * @func
 * This function is used to raise an exception while processing a
 * Remote Function Call, if the function module being called has
 * tables parameters.
 *
 * @rdesc
 * <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE>.
 *
 * @xref
 * <f RfcRaise>
 *------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcRaiseTables 
(
 RFC_HANDLE      handle,	/* @parm RFC connection handle */
 rfc_char_t *    exception, /* @parm exception to be raised
							 * (zero terminated string).
                             */
 RFC_TABLE  *    tables     /* @parm tables parameters as received by
                             * <f RfcGetData>.
                             */
);

/*------------------------------------------------------------------------
 * @func
 * This function is used to raise an exception and send back an error message. 
 *
 * @rdesc
 * <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE>.
 *
 * @xref
 * <f RfcRaiseTables>
 *------------------------------------------------------------------------*/
RFC_RC SAP_API RfcRaiseErrorMessage
(
 RFC_HANDLE      handle,	/* @parm RFC connection handle */
 rfc_char_t *    exception, /* @parm exception to be raised
							 * (zero terminated string).
                             */
 RFC_TABLE  *    tables,    /* @parm tables parameters as received by
                             * <f RfcGetData>.
                             */
 rfc_char_t * szErrorMsg    /* @parm Error message to be sent to the caller. The
                             * maximum length of the message is 200.
							 */                         

);


/*-----------------------------------------------------------------------------
 * @func
 *
 * can be used to check if there is a RFC request available.
 *
 * If one does not always want to wait for the answer to an RFC call
 * by <f RfcReceive>  one has to call the function RfcListen
 * to listen for incoming RFC events.
 *
 * The function always returns immediately.
 * If RfcListen returns <e RFC_RC.RFC_OK>, RfcReceive has to be called to
 * handle the event. It only is possible  to listen for one incoming
 * RFC message at a time.
 *
 * It is also possible to use RfcListen while waiting for a new RFC request
 * by <f RfcDispatch> with or without register mode (s. <f RfcAccept>). Because
 * RfcListen will immediately return, it is recommended to use <f RfcWaitForRequest>
 * to wait for new RFC requests.
 *
 * @rdesc
 *
 * The function returns  <e RFC_RC.RFC_OK>,
 * if there is an RFC event pending (call or return) and  <e RFC_RC.RFC_RETRY>,
 * if nothing has arrived yet. The function returns <e RFC_RC.RFC_FAILURE>, if
 * an error has occurred .
 *
 * @ex Using RfcListen while waiting for the answer of a remote function call. |
 *
 *  ....
 *
 * RFC_RC rc;
 *
 * rc = RfcCall( handle, .... );
 *
 * do
 * {
 *    rc = RfcListen( handle );
 *    if( rc == RFC_RETRY )
 *    {
 *       // while waiting for the RFC answer, do something...
 *       ...
 *    }
 * } while ( rc == RFC_RETRY );
 *
 * if( rc == RFC_OK )
 *    rc = RfcReceive( handle, .... );
 * ...
 *
 * @ex Using RfcListen and RfcDispatch. |
 *
 * RFC_RC     rc;
 * RFC_HANDLE handle = RfcAccept( argv );
 * ...
 * do
 * {
 *   // Waiting for the next RFC request
 *   for( rc = RFC_RETRY; rc == RFC_RETRY; )
 *   {
 *     rc = RfcListen( handle );
 *     if( rc == RFC_RETRY )
 *     {
 *       // while waiting for the next RFC request, do something...
 *       ....
 *     }
 *   }
 *   if (rc != RFC_OK)
 *     break;
 *
 *   rc = RfcDispatch( handle );
 *
 * } while( rc == RFC_OK );
 *
 * RfcClose( handle );
 * exit( 0 );
 *
 *
 *-----------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcListen 
(
  RFC_HANDLE handle  /* @parm RFC connection handle */
);



/*-----------------------------------------------------------------------------
 * @func
 *
 * After <f RfcAccept> using register mode, this function can be used to wait
 * for a defined time for new RFC requests.
 *
 * The time must be defined in sec (-1: infinit).
 *
 * If the RFC server program doesn't run in register mode, this call has
 * the same functionality as RfcListen (immediately returning).
 *
 *-----------------------------------------------------------------------------*/
RFC_RC SAP_API RfcWaitForRequest
(
  RFC_HANDLE handle, /* @parm RFC connection handle */
  RFC_INT    wtime   /* @parm Wait time             */
);



RFC_RC SAP_API RfcFileArgv(int* argc, rfc_char_t*** PntArgv);




/*---------------------------------------------------------------------------
 * @func
 *
 * This function can be used to build up the structures <t RFC_OPTIONS>,
 * <t RFC_CONNOPT_R3ONLY>, <t RFC_CONNOPT_VERSION_3> or
 * <t RFC_CONNOPT_CPIC> from the command line.
 *
 * @comm
 *
 * The following tokens are recognized in the argv array, which must
 * be terminated by a NULL entry :
 *
 * @flag -d <lt>Destination<gt>  | the name of the destination
 * @flag -c <lt>NNN<gt>          | client (sign on data)
 * @flag -u <lt>User Id<gt>      | user id
 * @flag -p <lt>Password<gt>     | password
 * @flag -l <lt>language<gt>     | language
 * @flag -3                      | R/3 mode
 * @flag -2                      | CPIC mode
 * @flag -t                      | turn trace on
 * @flag -h <lt>Hostname<gt>        | the name of the target host
 * @flag -s <lt>NN<gt>              | the system number of the target SAP system
 * @flag -g <lt>Gateway Host<gt>    | the gateway host (if not specified, the
 *			                          -h option is used)
 * @flag -x <lt>Gateway Service<gt> | the TCP/IP service of the gateway (
 *                              default is sapgwNN, where NN is
 *                              the system number (-s)
 * @flag -gui | start 'sapgui' to be able to display SAP Dynpros or
 *              Graphics (R/3 mode only).
 * @flag -debug | start communication in debug mode (R/3 mode only).
 *
 * All tokens that were interpreted by RfcConnArgv3 are removed from
 * the argv array.
 *
 * @rdesc returns 0.
 *
 * @xref <f RfcConnArgv>
 *
 *---------------------------------------------------------------------------*/
int    SAP_API RfcConnArgv3
(
 rfc_char_t **                 argv,        /* @parm command line to be parsed. */
 RFC_OPTIONS *           rfc_opt,			/* @parm option structure to
											 * be filled.
											 */
 RFC_CONNOPT_CPIC *      connopt_cpic,		/* @parm option extension for R/2 connection
											 * (a valid pointer must be supplied
											 *  even if you are not using a R/2
											 *  system).
											 */
 RFC_CONNOPT_R3ONLY *    connopt_r3only,	/* @parm option extension for R/3 connection
                                             * (a valid pointer must be supplied
                                             *  all the time).
                                             */
 RFC_CONNOPT_VERSION_3 * connopt_version_3	/* @parm option extension for a
                                             * connection
                                             * to a R/3 system of Rel. 3.0
                                             * or later
                                             * (a valid pointer must be supplied
                                             *  all the time).
                                             */
);





/*---------------------------------------------------------------------------
 * @func
 *
 * This function was used to build up the structures <t RFC_OPTIONS> ,
 * <t RFC_CONNOPT_R3ONLY> or
 * <t RFC_CONNOPT_CPIC> from the command line in former releases and
 * is included for backward compatibility.
 *
 * It offers the same functionality as <f RfcConnArgv3> allocating
 * a static instance of <t RFC_CONNOPT_VERSION_3> internally, however.
 *
 * Use the function <f RfcConnArgv3> instead, if you are not working
 * on 16-bit Windows.
 *
 * On 16-bit Windows only RfcConnArgv is supported.
 *
 * @rdesc returns 0.
 *
 * @xref <f RfcConnArgv3>
 *
 *---------------------------------------------------------------------------*/
int    SAP_API RfcConnArgv
(
 rfc_char_t **                 argv,   /* @parm command line to be parsed. */
 RFC_OPTIONS *           rfc_opt,      /* @parm option structure to be filled. */
 RFC_CONNOPT_CPIC *      connopt_cpic, /* option extension for R/2 connection
                                        * (a valid pointer must be supplied
                                        *  even if you are not using a R/2
                                        *  system).
                                        */
 RFC_CONNOPT_R3ONLY *    connopt_r3only /* @parm option extension for R/3 connection
                                         * (a valid pointer must be supplied
                                         *  all the time).
                                         */
);


RFC_RC SAP_API RfcSendSystemInfo( RFC_HANDLE handle );
RFC_RC SAP_API RfcSendPing      ( RFC_HANDLE handle );
RFC_RC SAP_API RfcSendDocu      ( RFC_HANDLE handle );


/* @cb  RFC_RC |  RFC_ONCALL |
 *
 * type of C function to be installed by <f RfcInstallFunction>
 * or <f RfcInstallFunctionExt>.
 *
 */
typedef RFC_RC ( DLL_CALL_BACK_FUNCTION_PTR RFC_ONCALL ) (
   RFC_HANDLE handle /* @parm Handle to RFC connection, from where the RFC parameters
                      * must be read and to which the parameters must be returned.
                      */
               );

/*--------------------------------------------------------------------------
 * @func
 *
 * Used to install a function to be callable as RFC function module.
 *
 * For receiving an RFC call there are two possible ways. The most
 * simple way to receive an RFC call in an external program is to
 * register a C function to be called if a call request is received.
 *
 * The function RfcInstallFunction registers a C function to be
 * called when receiving the request for an RFC call.
 *
 * The function pointer points to a function of type <f RFC_ONCALL>,
 * which contains the functionality being offered as an RFC function
 * module. 'functionname' is the name of the offered RFC function
 * module and description should contain a description of  the
 * functionality as well as a description of the interface. Newline
 * characters can be used to start new lines.
 *
 * The descriptions of the registered functions can be requested by
 * calling the standard function module RFC_DOCU being available in
 * every RFC server program using <f RfcDispatch> interface.
 *
 * After <f RfcAccept> or after receiving the return code <e RFC_RC.RFC_CALL>,
 * when calling <f RfcReceive> the program has to call <f RfcDispatch>,
 * which internally calls the corresponding registered function.
 *
 * @rdesc
 * returns <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE>, if there is no memory available
 * to register the function.
 *
 * @xref <f RfcDispatch>, <f RfcGetName>
 *
 * @comm
 * This function is not available for 16-Bit Windows (Windows 3.x).
 * Use <f RfcInstallFunctionExt> instead.
*-------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcInstallFunction 
(
 RFC_FUNCTIONNAME functionname,  /* @parm name of function as it can be called
                                  * from ABAP/4 environment. Zero terminated
                                  * string.
                                  */
 RFC_ONCALL       f_ptr,         /* @parm function to be called. Must be of type
                                  * <f RFC_ONCALL>.
                                  */
 rfc_char_t *           docu           /* @parm text describing the functionality and
                                  * the parameters of the function module.
                                  */
);



/*-------------------------------------------------------------------------------
 *
 * @func
 *
 * Alternate function replacing <f RfcInstallFunction> for Windows 3.x (16-Bit).
 * The functions needs a valid RFC handle as an additional parameter.
 *
 * The function module is installed only for the given RFC connection.
 *
 * @rdesc
 * returns <e RFC_RC.RFC_OK> or <e RFC_RC.RFC_FAILURE>, if there is no memory available
 * to register the function.
 *
 * @xref <f RfcInstallFunction>, <f RfcDispatch>, <f RfcGetName>
 *
 * @comm
 * The function is available on all supported platforms to ease porting.
 *-----------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcInstallFunctionExt
(
 RFC_HANDLE handle,              /* @parm valid RFC connection handle,
                                  * as returned by <f RfcAccept>.
                                  */
 RFC_FUNCTIONNAME functionname,  /* @parm name of function as it can be called
                                  * from ABAP/4 environment. Zero terminated
                                  * string.
                                  */
 RFC_ONCALL       f_ptr,         /* @parm function to be called. Must be of type
                                  * <f RFC_ONCALL>.
                                  */
 rfc_char_t *           docu           /* @parm text describing the functionality and
                                  * the parameters of the function module.
                                  */

);



/*--------------------------------------------------------------------------------
 * @func
 * function to be called to process the next RFC request.
 *
 * After <f RfcAccept> or after receiving the return code
 * <e RFC_RC.RFC_CALL>, when calling <f RfcReceive> or
 * <f RfcCallReceive> the program has to call this function
 * to process the pending RFC request.
 *
 * This function internally calls
 * the registered function corresponding to the RFC call.
 * The return code of the registered function is again
 * returned by RfcDispatch.
 *
 * @comm
 * There are some function modules, which are always
 * offered automatically, when using RfcDispatch.
 *
 * These are
 * @flag RFC_DOCU | get of list of the installed function
 *                  modules.
 * @flag RFC_PING | do nothing, used for connection test.
 * @flag RFC_SYSTEM_INFO | deliver some information on
 *                         the RFC library used.
 *
 * @rdesc
 * returns <e RFC_RC.RFC_OK> or other <t RFC_RC> return code.
 *
 * @ex a typical RFC server |
 *
 * int main ( int argc, char ** argv )
 * {
 *   RFC_HANDLE handle;
 *   RFC_RC     rc;
 *
 *   handle = RfcAccept( argv );
 *   if ( handle == RFC_HANDLE_NULL )
 *   {
 *      ... error processing
 *      return 1;
 *   }
 *
 *   rc = RfcInstallFunction( ..... );
 *   if( rc != RFC_OK )
 *   {
 *      ... error processing
 *      return 1;
 *   }
 *
 *   do
 *   {
 *      rc = RfcDispatch( handle );
 *   }
 *   while( rc == RFC_OK );
 *
 *   RfcClose( handle );
 *
 *   return 0;
 * }
 *
 */
DECL_EXP RFC_RC SAP_API RfcDispatch 
(
  RFC_HANDLE  handle   /* @parm handle to RFC connection */
);


/*-----------------------------------------------------
 * @cb  int |  RFC_ON_CHECK_TID |
 *
 * function to be installed by <f RfcInstallTransactionControl>.
 *
 * The function is called when a local transaction is starting.
 * Since a transactional RFC call can be issued many times
 * by the client system, the function is responsible for storing
 * the transaction ID in permanent storage.
 * If the client system
 * tries starting the same transaction
 * a second time, the function has to return 1.
 *
 * @comm
 * If this functions has access to a SQL database,
 * it should perform a operation like
 *
 *    Insert Into SomeTable values ( :transactionId );
 *
 * where the table 'SomeTable' must have a unique index
 * over the transaction ID. If another process has also inserted
 * the same transaction ID, the second process has to wait until
 * the transaction is completed by the first process. If the
 * transaction is completed by another process (The Insert command
 * returns with some 'Duplicate Record' error code, the function must
 * return 1 to indicate to skip the transaction.
 *
 * @rdesc Returns one of the following values:
 * @flag 0      | transaction ID stored, transaction can be started.
 * @flag 1      | transaction already done, skip the request.
 * @flag <lt> 0 | cannot lock transaction, internal error.
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 *       <f RfcInstallTransactionControl2>
 */
typedef int ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_CHECK_TID )
(
  RFC_TID transactionId  /* @parm <t RFC_TID> */
);



/*----------------------------------------------------------------
 * @cb  int |  RFC_ON_COMMIT |
 *
 * function to be installed by <f RfcInstallTransactionControl>.
 *
 * The function is called when a local transaction ends.
 *
 * The function is to be used to commit the local transaction,
 * if necessary.
 *
 * @comm
 * If this functions has access to a SQL database,
 * it should perform a operation like
 *
 *    Commit work;
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 *       <f RfcInstallTransactionControl2>
 *--------------------------------------------------------------*/
typedef void ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_COMMIT )
(
    RFC_TID transactionId  /* @parm <t RFC_TID> */
);




/*--------------------------------------------------------
 * @cb  int |  RFC_ON_ROLLBACK |
 *
 * function to be installed by <f RfcInstallTransactionControl>.
 *
 * The function is called when a local transaction ends with failure.
 *
 * The function is to be used to roll back the local transaction,
 * if necessary.
 *
 * @comm
 * If this functions has access to a SQL database,
 * it should perform a operation like
 *
 *    Rollback work;
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 *       <f RfcInstallTransactionControl2>
 *-------------------------------------------------------------------------*/
typedef void ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_ROLLBACK )
(
    RFC_TID transactionId  /* @parm <t RFC_TID> */
);




/*-----------------------------------------------------------------
 * @cb  void |  RFC_ON_CONFIRM_TID |
 *
 * function to be installed by <f RfcInstallTransactionControl>.
 *
 * The function is called when a local transaction is completed.
 * All information stored about that transaction can be discarded
 * by the server.
 *
 * In general this function can be used to delete the transaction
 * ID from permanent storage.
 *
 * @comm
 * If this functions has access to a SQL database,
 * it should perform a operation like
 *
 *    Delete From SomeTable where key = :transactionId; Commit Work;
 *
 * where the table 'SomeTable' should have a unique index
 * over the transaction ID.
 *
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 *       <f RfcInstallTransactionControl2>
 *
 */
typedef void ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_CONFIRM_TID )
(
    RFC_TID transactionId  /* @parm <t RFC_TID> */
);

/*---------------------------------------------------------------------
 * @func
 *
 * installs 4 functions to control transactional behavior.
 * Only to be used with transactional RFC.
 *
 * Must be called by
 * RFC server program before the RfcDispatch loop is entered,
 * if this program wants to receive transactional RFC calls,
 * i.e. calls being issued by 'Call Function ... In Background Task'
 * and must ensure, that RFC calls are done exactly once.
 *
 * Without installing these functions it can only be guaranteed,
 * that an RFC function call issued by
 * 'Call Function ... In Background Task'
 * is done at least once. Then all function modules offered by such a server
 * program, which are called via 'Call Function ... In Background Task'
 * must cope with being called more then once.
 *
 * If installed, the first function onCheckTid is called, if a transactional
 * RFC is to be called. The actual Transaction ID
 * is passed ( <f RFC_ON_CHECK_TID> ).
 * The function has to store this transaction id in permanent
 * storage and return 0 .
 * If the same function will be called later again with the same
 * Transaction Id, it has to make sure that it will
 * return a non-zero value.
 * If the same transaction is already
 * running by another process, but is not completed,
 * the function has to wait, until
 * the transaction completes.
 *
 * The second function is called, if all the RFC function module
 * calls are done and the local transaction can be completed.
 * It should be used, to locally commit the transaction, if necessary
 * ( <f RFC_ON_COMMIT> ).  This function is 'void' .
 *
 * The third function is called instead of the 2nd function,
 * if, from the point of view of the RFC library,
 * there occurred an error while processing the local transaction.
 * This function can be used to roll back the local transaction
 * ( <f RFC_ON_ROLLBACK> ). This function is 'void' .
 *
 * The 4th function will be called, if the local transaction is
 * completed from the point of view of the calling system
 * and all information on this transaction Id can be
 * discarded. This function is 'void' .
 * <f RFC_ON_CONFIRM_TID>
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl2>
 */
void SAP_API RfcInstallTransactionControl
(
 RFC_ON_CHECK_TID   onCheckTid, /* @parm function to be called, when
                                 * local transaction starts. Must be used
                                 * to check, if the transaction is already
                                 * running or was already completed.
                                 */
 RFC_ON_COMMIT      onCommit,   /* @parm function to be called, when
                                 * local transaction ends.
                                 * Should be used to commit the
                                 * transaction locally.
                                 */
 RFC_ON_ROLLBACK    onRollback, /* @parm function to be called, if
                                 * local transaction fails due to an
                                 * error found while the processing
                                 * is done inside the RFC library.
                                 * Should be used to roll back
                                 * a local transaction.
                                 */
 RFC_ON_CONFIRM_TID onConfirmTid /* @parm function to be called, when
                                  * local transaction is confirmed.
                                  * All information stored locally about
                                  * this transaction can be deleted.
                                  */
);

/*-----------------------------------------------------
 * @cb  int |  RFC_ON_CHECK_TID_EX |
 *
 * function to be installed by <f RfcInstallTransactionControl2>.
 *
 * The function is called when a local transaction is starting.
 * Since a transactional RFC call can be issued many times
 * by the client system, the function is responsible for storing
 * the transaction ID in permanent storage.
 * If the client system
 * tries starting the same transaction
 * a second time, the function has to return 1.
 *
 * @comm
 * If this functions has access to a SQL database,
 * it should perform a operation like
 *
 *    Insert Into SomeTable values ( :transactionId );
 *
 * where the table 'SomeTable' must have a unique index
 * over the transaction ID. If another process has also inserted
 * the same transaction ID, the second process has to wait until
 * the transaction is completed by the first process. If the
 * transaction is completed by another process (The Insert command
 * returns with some 'Duplicate Record' error code, the function must
 * return 1 to indicate to skip the transaction.
 *
 * @rdesc Returns one of the following values:
 * @flag 0      | transaction ID stored, transaction can be started.
 * @flag 1      | transaction already done, skip the request.
 * @flag <lt> 0 | cannot lock transaction, internal error.
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 *       <f RfcInstallTransactionControl2>
 *
 */
typedef int ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_CHECK_TID_EX )
(
  RFC_TID    transactionId, /* @parm <t RFC_TID> */
  RFC_HANDLE handle         /* @parm The handle of the connection that 
                             * dispatches the calls
                             */
);

/*----------------------------------------------------------------
 * @cb  int |  RFC_ON_COMMIT_EX |
 *
 * function to be installed by <f RfcInstallTransactionControl2>.
 *
 * The function is called when a local transaction ends.
 *
 * The function is to be used to commit the local transaction,
 * if necessary.
 *
 * @comm
 * If this functions has access to a SQL database,
 * it should perform a operation like
 *
 *    Commit work;
 *
 * @rdesc Returns one of the following values:
 * @flag 0      | Commit was success
 * @flag !0     | Commit FAILED, the call shall be redispatched
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 *       <f RfcInstallTransactionControl2>
 *--------------------------------------------------------------*/
typedef int ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_COMMIT_EX )
(
    RFC_TID transactionId,  /* @parm <t RFC_TID> */
    RFC_HANDLE handle       /* @parm The handle of the connection that
                             * dispatches the calls
                             */
);

/*--------------------------------------------------------
 * @cb  int |  RFC_ON_ROLLBACK_EX |
 *
 * function to be installed by <f RfcInstallTransactionControl2>.
 *
 * The function is called when a local transaction ends with failure.
 *
 * The function is to be used to roll back the local transaction,
 * if necessary.
 *
 * @comm
 * If this functions has access to a SQL database,
 * it should perform a operation like
 *
 *    Rollback work;
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 *       <f RfcInstallTransactionControl2> 
 *-------------------------------------------------------------------------*/
typedef void ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_ROLLBACK_EX )
(
    RFC_TID transactionId,  /* @parm <t RFC_TID> */
    RFC_HANDLE hRfc,        /* @parm The handle of the connection that
                             * dispatches the calls
                             */
    int        bValidHandle /* @parm 1: The handle <p hRfc> is valid<nl>
                             * 0: The handle is the handle of the connection
                             *    that transported the call, yet this handle
                             *    is now broken and is only passed for 
                             *    information purpose.  It cannot be used
                             *    in any API call.
                             */

);

/*-----------------------------------------------------------------
 * @cb  void |  RFC_ON_CONFIRM_TID_EX |
 *
 * function to be installed by <f RfcInstallTransactionControl>.
 *
 * The function is called when a local transaction is completed.
 * All information stored about that transaction can be discarded
 * by the server.
 *
 * In general this function can be used to delete the transaction
 * ID from permanent storage.
 *
 * NOTE:<nl>
 * The handle that dispatch onConfirmTidEx is not always the handle
 * that was passed to onCheckTidEx and onCommitTidEx
 *
 *
 * @comm
 * If this functions has access to a SQL database,
 * it should perform a operation like
 *
 *    Delete From SomeTable where key = :transactionId; Commit Work;
 *
 * where the table 'SomeTable' should have a unique index
 * over the transaction ID.
 *
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 *       <f RfcInstallTransactionControl2>
 *
 */
typedef void ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_CONFIRM_TID_EX )
(
    RFC_TID transactionId,  /* @parm <t RFC_TID> */
    RFC_HANDLE handle       /* @parm The handle of the connection that
                             * dispatches the calls
                             */
);

/*---------------------------------------------------------------------
 * @func
 *
 * installs 4 functions to control transactional behavior.
 * Only to be used with transactional RFC.
 *
 * Must be called by
 * RFC server program before the RfcDispatch loop is entered,
 * if this program wants to receive transactional RFC calls,
 * i.e. calls being issued by 'Call Function ... In Background Task'
 * and must ensure, that RFC calls are done exactly once.
 *
 * Without installing these functions it can only be guaranteed,
 * that an RFC function call issued by
 * 'Call Function ... In Background Task'
 * is done at least once. Then all function modules offered by such a server
 * program, which are called via 'Call Function ... In Background Task'
 * must cope with being called more then once.
 *
 * If installed, the first function onCheckTid is called, if a transactional
 * RFC is to be called. The actual Transaction ID
 * is passed ( <f RFC_ON_CHECK_TID_EX> ).
 * The function has to store this transaction id in permanent
 * storage and return 0 .
 * If the same function will be called later again with the same
 * Transaction Id, it has to make sure that it will
 * return a non-zero value.
 * If the same transaction is already
 * running by another process, but is not completed,
 * the function has to wait, until
 * the transaction completes.
 *
 * The second function is called, if all the RFC function module
 * calls are done and the local transaction can be completed.
 * It should be used, to locally commit the transaction, if necessary
 * ( <f RFC_ON_COMMIT_EX> ).  This function is 'void' .
 *
 * The third function is called instead of the 2nd function,
 * if, from the point of view of the RFC library,
 * there occurred an error while processing the local transaction.
 * This function can be used to roll back the local transaction
 * ( <f RFC_ON_ROLLBACK_EX> ). This function is 'void' .
 *
 * The 4th function will be called, if the local transaction is
 * completed from the point of view of the calling system
 * and all information on this transaction Id can be
 * discarded. This function is 'void' .
 * <f RFC_ON_CONFIRM_TID_EX>
 *
 * @xref <t RFC_TID>
 *       <f RfcInstallTransactionControl>
 */
void SAP_API RfcInstallTransactionControl2
(
 RFC_ON_CHECK_TID_EX onCheckTid,/* @parm function to be called, when
                                 * local transaction starts. Must be used
                                 * to check, if the transaction is already
                                 * running or was already completed.
                                 */
 RFC_ON_COMMIT_EX    onCommit,  /* @parm function to be called, when
                                 * local transaction ends.
                                 * Should be used to commit the
                                 * transaction locally.
                                 */
 RFC_ON_ROLLBACK_EX onRollback, /* @parm function to be called, if
                                 * local transaction fails due to an
                                 * error found while the processing
                                 * is done inside the RFC library.
                                 * Should be used to roll back
                                 * a local transaction.
                                 */
 RFC_ON_CONFIRM_TID_EX onConfirmTid /* @parm function to be called, when
                                  * local transaction is confirmed.
                                  * All information stored locally about
                                  * this transaction can be deleted.
                                  */
);



/* SAP->API: info transact. rfc */

RFC_RC SAP_API RfcGetTrfcInfo( rfc_char_t * user, rfc_char_t * dest );


/* SAP->API: get transactionID and set f_ptr for confirmation */

RFC_RC SAP_API RfcGetTransID( RFC_RC (* _SYSTEM f_ptr)( rfc_char_t * ),
                                           RFC_TID tid );

/* @func
 *
 * Gets a Transaction-ID for a following call of a function module in R/3
 * using the transactional RFC interface in R/3
 *
 * With this function a new TransID will be produced from the R/3 system
 * the RFC client program has to call a function module in R/3 via
 * <f RfcIndirectCall> or <f RfcIndirectCallEx> with this TransID.
 *
 * If an error occurs (e.g. communication error) during the call of a function
 * module in R/3 via RfcIndirectCall the RFC client program has to reconnect
 * the RFC connection and repeat the RfcIndirectCall/RfcIndirectCallEx without
 * creating a new TransID.
 *
 * @xref <f RfcIndirectCall>, <f RfcIndirectCallEx>
 *
 */
RFC_RC SAP_API RfcCreateTransID(
 RFC_HANDLE       handle,    /* @parm connection handle. */
 RFC_TID          tid        /* @parm <t RFC_TID>        */
 );
/* API->SAP: check transID already is enqueued                */

RFC_RC SAP_API RfcCheckTransID(   RFC_HANDLE  handle,
                                  RFC_TID     tid       );

/* @func
 *
 * explizitely confirms a transaction using by transactional RFC.
 *
 * This call is only for use with <f RfcIndirectCallEx> and NOT
 * with <f RfcIndirectCall>.
 *
 * The function module called by <f RfcIndirectCallEx> is already
 * executed. This call is only necessary to delete some entries
 * for this TransID in SAP database. A repeat of this call in error
 * cases (almost communication error) is also available.
 *
 * If you don't use this call the SAP database will grow up.
 *
 * @xref <f RfcIndirectCallEx>
 *
 */
RFC_RC SAP_API RfcConfirmTransID(
 RFC_HANDLE       handle,    /* @parm connection handle. */
 RFC_TID          tid        /* @parm <t RFC_TID>        */
 );

/* @func
 *
 * explicitly allows the RFC library to start the programs described
 * in the list as input parameter if this is required by the RFC partner.
 *
 * This call can be used either in RFC client or RFC server program.
 *
 */
RFC_RC SAP_API RfcAllowStartProgram(
 rfc_char_t           * pname_list /* @parm zero terminated string.
			      * This is a list of programs which can
			      * be started by the RFC library by the
			      * RFC partner if required. A program
			      * name can have max. 512 B. They must
			      * be separated by ;.
			      * NULL: any program is allowed.
			      */
 );

/* @func
 *
 * activates the RFC trace for a specified connection or for all RFC
 * connections (RFC_HANDLE_NULL as handle).
 *
 * This call can be used either in RFC client or RFC server program.
 *
 */
void SAP_API RfcSetTrace(
 RFC_HANDLE       handle,      /* @parm connection handle
				* or RFC_HANDLE_NULL
				*/
 int              trace_level  /* @parm trace_level
				*  0: deactivate the RFC trace
				*  1: activate the RFC trace
				*/
 );

void SAP_API RfcSetParameter(
 rfc_char_t* Name,      /* @parm parameter name
  				        */
 rfc_char_t* Value  /* @parm parameter value
    				*/
 );


/*
    New API functions that omit the use of structs.
    d002076, 28.1.94
*/
typedef struct
{
        RFC_INT MaxNo;
    void* Params;
}RFC_PARAMETER_STACK;

typedef struct
{
    RFC_PARAMETER_STACK Exports;
    RFC_PARAMETER_STACK Imports;
    RFC_PARAMETER_STACK Tables;
} RFC_PARAMETER_ADMIN;

typedef RFC_PARAMETER_ADMIN* RFC_PARAM_SPACE;

RFC_PARAM_SPACE SAP_API RfcAllocParamSpace(unsigned MaxEx,
                                 unsigned MaxIm,
                                 unsigned MaxTab);

RFC_RC SAP_API RfcFreeParamSpace(RFC_PARAM_SPACE PSpace);

RFC_RC SAP_API RfcAddExportParam(RFC_PARAM_SPACE PSpace,
                                 unsigned ParamNo,
                                 void* name,
                                 unsigned nlen,
                                 unsigned type,
                                 unsigned leng,
                                 void* addr);

RFC_RC SAP_API RfcAddImportParam(RFC_PARAM_SPACE PSpace,
                                 unsigned ParamNo,
                                 void* name,
                                 unsigned nlen,
                                 unsigned type,
                                 unsigned leng,
                                 void* addr);

/* @func
 *
 * Special call for Visual Basic 4.0/5.0.
 * Instead of RfcAddImportParam() you must use RfcDefineImportParam()
 * to define all IMPORTING-parameters.
 * After RfcCallReceiveExt() or RfcReceiveExt() or RfcGetDataExt()
 * you have to issue RfcGetImportParam() for each IMPORTING-parameter
 * to get the contents of this parameter.
 *
 * @xref <f RfcGetImportParam>
 *
 */
RFC_RC SAP_API RfcDefineImportParam(
  RFC_PARAM_SPACE   PSpace,  /* @parm Pointer returned by RfcAllocParam */
  unsigned          ParamNo, /* @parm Parameter number (0, 1, 2, ...)   */
  void            * name,    /* @parm Parameter name                    */
  unsigned          nlen,    /* @parm Length of parameter name          */
  unsigned          type,    /* @parm Parameter type                    */
  unsigned          leng     /* @parm Length of parameter value         */
  );

/* @func
 *
 * Special call for Visual Basic 4.0/5.0.
 * Instead of RfcAddImportParam() you must use RfcDefineImportParam()
 * to define all IMPORTING-parameters. After RfcCallReceiveExt()
 * or RfcReceiveExt() or RfcGetDataExt() you have to issue
 * RfcGetImportParam() for each IMPORTING-parameter to get the
 * contents of this parameter.
 *
 * @xref <f RfcDefineImportParam>
 *
 */
RFC_RC SAP_API RfcGetImportParam(
  RFC_PARAM_SPACE   PSpace,  /* @parm Pointer returned by RfcAllocParam */
  unsigned          ParamNo, /* @parm Parameter number (0, 1, 2, ...)   */
  void            * addr     /* @parm Addr. for parameter value         */
  );

RFC_RC SAP_API RfcAddTable ( RFC_PARAM_SPACE PSpace,
                                 unsigned TableNo,
                                 void* name,
                                 unsigned nlen,
                                 unsigned type,
                                 unsigned leng,
                                 ITAB_H ithandle);

ITAB_H SAP_API RfcGetTableHandle(RFC_PARAM_SPACE PSpace,
                                 unsigned TableNo);

RFC_RC SAP_API RfcCallExt ( RFC_HANDLE handle,
			    RFC_PARAM_SPACE PSpace,
			    CONST_RFC rfc_char_t* function);

RFC_RC SAP_API RfcReceiveExt(RFC_HANDLE handle,
			     RFC_PARAM_SPACE PSpace,
			     rfc_char_t* exception);

RFC_RC SAP_API RfcCallReceiveExt(RFC_HANDLE handle,
                                 RFC_PARAM_SPACE PSpace,
                       CONST_RFC rfc_char_t* function,
                                 rfc_char_t* exception);

RFC_RC SAP_API RfcIndirectCallExt(RFC_HANDLE handle,
                                  RFC_PARAM_SPACE PSpace,
                        CONST_RFC rfc_char_t* function,
                                  RFC_TID tid);

RFC_RC SAP_API RfcIndirectCallExExt(RFC_HANDLE handle,
                                    RFC_PARAM_SPACE PSpace,
                                    rfc_char_t* function,
                                    RFC_TID tid);

RFC_RC SAP_API RfcGetDataExt(RFC_HANDLE handle,
                                 RFC_PARAM_SPACE PSpace);

RFC_RC SAP_API RfcSendDataExt(RFC_HANDLE handle,
                                 RFC_PARAM_SPACE PSpace);

int SAP_API RfcExecProgram( rfc_char_t * c,
                            rfc_char_t * et,
                            unsigned leng );

/* @func
 *
 * Returns the RFC protocol version
 *
 */
RFC_INT SAP_API RfcGetVersion(void);

/* @func
 *
 * Returns the SAP system version, The patch level and the
 * patch number of the RFC library
 */
void SAP_API RfcGetPatchLevel
(   
    int *pSapRelease,  /* @parm [out] SAP release of the build */
    int *pPatchLevel,  /* @parm [out] Patch level of the build */
    int *pPatchNumber  /* @parm [out] Patch number of the build */
);


RFC_RC SAP_API RfcCheckVersionsOfUsedLibs(void);
DECL_EXP RFC_RC SAP_API RfcGetAllLibVersions(rfc_char_t* buffer,
					     int BufferLength);
RFC_RC SAP_API RfcStructureToFile( RFC_HANDLE handle,
				   rfc_char_t * tableName,
				   void * file /* must be FILE * ! */);

RFC_RC SAP_API RfcStructureToFileByName( RFC_HANDLE handle,
				         rfc_char_t * tableName,
				         rfc_char_t * filename );

/* @func
 *
 * The RFC library supports 2 functions to convert encoded decimal
 * numbers (BCD number) as defined in SAP systems (Type P) to a
 * character string and vice versa RfcConvertCharToBcd and
 * RfcConvertBcdToChar.
 *
 * A BCD number is a 16 byte structure which contains 31 digits and
 * the sign. The structure is aligned in a long-word boundary.
 *
 * The sign can have the values '+' or '-'. Analogous to SAP system,
 * the hex value 'C' and 'D' are used as positive and negative sign.
 *
 * This function converts a character string to a BCD number.
 *
 * @rdesc Returns one of the following values:
 * @flag 0      | OK
 * @flag 2      | "out_len" too small
 * @flag 3      | Invalid character in "in"
 * @flag 4      | Invalid "in_len"
 *
 * @xref <f RfcConvertBcdToChar>
 *
 */
DECL_EXP int SAP_API RfcConvertCharToBcd(
  RFC_CHAR * in,       /* @parm Input data to convert */
  int        in_len,   /* @parm Length of input data  */
  int      * pdecs,    /* @parm No. of digits after , */
  RFC_BCD  * out,      /* @parm Output data           */
  int        out_len   /* @parm Length of output data */
  );

/* @func
 *
 * converts a BCD number into a character string.
 *
 * See <f RfcConvertCharToBcd> for more details.
 *
 *
 * @rdesc Returns one of the following values:
 * @flag 0      | OK
 * @flag 2      | "out_len" too small
 * @flag 3      | Invalid character in "in"
 * @flag 4      | Invalid "in_len"
 *
 */
DECL_EXP int SAP_API RfcConvertBcdToChar(
  RFC_BCD  * in,       /* @parm Input data to convert */
  int        in_len,   /* @parm Length of input data  */
  int        decs,     /* @parm No. of digits after , */
  RFC_CHAR * out,      /* @parm Output data           */
  int        out_len   /* @parm Length of output data */
  );


/* 2-PC-RFC Identifier (TXID) */
#define RFC_SAP_TXID_LN_CHAR        128
#define RFC_SAP_TXID_LN_RAW         RFC_SAP_TXID_LN_CHAR*sizeofR(SAP_UC)
#define RFC_SAP_TXID_LN             RFC_SAP_TXID_LN_CHAR


#define RFC_EXT_TXID_LN_CHAR        32
#define RFC_EXT_TXID_LN_RAW         RFC_EXT_TXID_LN_CHAR*sizeofR(SAP_UC)
#define RFC_EXT_TXID_LN             RFC_EXT_TXID_LN_CHAR


/* @type RFC_SAP_TXID |
 * zero terminated string giving a 2-Phaser-Commit-RFC
 * globally unique identifier.
 *
 * ATTENTION:  Don't mix up this ID with the <t RFC_TID> using
 *             in transactional RFC.
 *
 */
typedef rfc_char_t RFC_SAP_TXID[RFC_SAP_TXID_LN_CHAR+1];

/* @func
 *
 * RFC with 2-PHASE-COMMIT (2-PC-RFC)
 *
 * Only available with R/3 from 4.0C onwards and the called function
 * module in R/3 must use the Update Task for database updating.
 *
 *
 * The RFC library supports 4 functions for working with 2-PC-RFC:
 *
 * RfcTxStart             Start a transaction
 *
 * RfcTxPrepareToCommit   Prepare to commit the current transaction
 *
 * RfcTxCommit            Commit a transaction
 *
 * RfcTxAbort             Abort a transaction
 *
 *
 * @xref <f RfcTxPrepareToCommit>, <f RfcTxCommit>, <f RfcTxAbort>
 *
 */
RFC_RC SAP_API RfcTxStart(
 RFC_HANDLE          handle,       /* @parm connection handle.       */
 void *              ext_txid,     /* @parm External Transaction ID. */
 int                 ext_txid_len, /* @parm Length of the External Transaction ID in char, 
                                    * Max. 32 CHAR.
			   	                    */
 RFC_SAP_TXID        sap_txid,     /* @parm <t RFC_SAP_TXID>         */
 RFC_ERROR_INFO_EX * error_info    /* @parm <t RFC_ERROR_INFO_EX>
			                        * describing the error
				                    */
);



/* @func
 *
 *
 * see <f RfcTxStart> for more details
 *
 *
 */
RFC_RC SAP_API RfcTxPrepareToCommit(
 RFC_HANDLE          handle,       /* @parm connection handle.       */
 void *              ext_txid,     /* @parm External Transaction ID. */
 int                 ext_txid_len, /* @parm Length of the External Transaction ID in chars, 
                                    * Max. 32 charachters.
                                    */
 RFC_SAP_TXID        sap_txid,     /* @parm <t RFC_SAP_TXID>         */
 RFC_ERROR_INFO_EX * error_info    /* @parm <t RFC_ERROR_INFO_EX> describing the error */
 );



/* @func
 *
 *
 * see <f RfcTxStart> for more details
 *
 *
 */
RFC_RC SAP_API RfcTxCommit(
 RFC_HANDLE          handle,       /* @parm connection handle.       */
 void *              ext_txid,     /* @parm External Transaction ID. */
 int                 ext_txid_len, /* @parm Length of the External Transaction ID in chars, 
                                    * Max. 32 charachters. */
 RFC_SAP_TXID        sap_txid,     /* @parm <t RFC_SAP_TXID>         */
 RFC_ERROR_INFO_EX * error_info    /* @parm <t RFC_ERROR_INFO_EX> describing the error */
);



/* @func
 *
 *
 * see <f RfcTxStart> for more details
 *
 *
 */
RFC_RC SAP_API RfcTxAbort(
 RFC_HANDLE          handle,       /* @parm connection handle.       */
 void *              ext_txid,     /* @parm External Transaction ID. */
 int                 ext_txid_len, /* @parm Length of the External Transaction ID in chars, 
                                    * Max. 32 charachters.
                                    */
 RFC_SAP_TXID        sap_txid,     /* @parm <t RFC_SAP_TXID>         */
 RFC_ERROR_INFO_EX * error_info    /* @parm <t RFC_ERROR_INFO_EX> describing the error */
);



/* @func
 *
 * uninstalls an RFC function.
 *
 *
 * @xref <f RfcInstallFunction>
 *
 */
DECL_EXP RFC_RC SAP_API RfcUninstallFunction(
 RFC_FUNCTIONNAME    f_name        /* @parm Name of an RFC function
                                    * which should be uninstalled.
                                    */
 );


/* @type RFC_GUI_START_PARAM |
 *
 * zero terminated string giving parameters which SAPGUI should be
 * started with (returned by <f RfcGuiPrepare> and should be used
 * in <f RfcGuiStart>.
 *
 */
#define RFC_GUI_START_PARAM_LN   512
typedef rfc_char_t RFC_GUI_START_PARAM[RFC_GUI_START_PARAM_LN+1];


/* @type RFC_GUI_RETURN_PARAM |
 *
 * zero terminated string giving information by SAPGUI after
 * being started. This information must be sent to R/3 via the
 * RFC library using <f RfcGuiReturn>.
 *
 */
#define RFC_GUI_RETURN_PARAM_LN   128
typedef rfc_char_t RFC_GUI_RETURN_PARAM[RFC_GUI_RETURN_PARAM_LN+1];


/*---------------------------------------------------------------------
 * @type RFC_GUI_ERROR |
 *
 * zero terminated string giving error message by unsuccessful start
 * of SAPGUI. This error message must be sent to R/3 via the
 * RFC library using <f RfcGuiReturn>.
 *
 * If no error occurs this error message must be an empty string
 * (string length 0).
 *
 *----------------------------------------------------------------------*/
#define RFC_GUI_ERROR_LN   128
typedef rfc_char_t RFC_GUI_ERROR[RFC_GUI_ERROR_LN+1];


/*------------------------------------------------------------------------------
 *
 * @func
 *
 * Additional to the automatic start of SAPGUI at the connection open time,
 * the RFC library supports now 4 more functions for working with SAPGUI.
 *
 * Using these new functions, SAPGUI will be started when needed
 * and not necessary at the connection open time.
 *
 * RfcGuiCheck     checks whether SAPGUI is already running.
 *
 * RfcGuiPrepare   informs the RFC library about working with SAPGUI.
 *                 As output, it will return some parameters which SAPGUI
 *                 should be started with.
 *
 * RfcGuiStart     starts SAPGUI via the RFC library.
 *
 * RfcGuiReturn    sends the information returned by SAPGUI after being
 *                 started to R/3.
 *
 *
 * Start SAPGUI on the same computer as RFC client (locale):
 *
 * If you only want to start SAPGUI on the same computer where your RFC
 * client is running but not at the OPEN-time, you only need to use the
 * function RfcGuiPrepare. In this case, the parameter remote_gui must
 * be set to 0.
 *
 *
 * Start SAPGUI not on the same computer as RFC client (remote):
 *
 * There are 2 different ways how to work with this.
 *
 *
 * 1. Start SAPGUI in your responsibility:
 *
 *    If your RFC client can send and receive with another program
 *    running on the remote computer, it must issue RfcGuiPrepare to get the
 *    parameters needed for starting SAPGUI. The program on the remote
 *    computer has then to start SAPGUI with these parameters using the
 *    function RfcGuiStart.
 *
 *    After SAPGUI is started, it will return some information to R/3, the
 *    program on the remote computer must send this information to your
 *    RFC client and it must then send this information to R/3 using
 *    RfcGuiReturn.
 *
 *    Using this functionality, you are responsible for sending
 *    START-parameters returned by RfcGuiPrepare to the remote computer
 *    for starting SAPGUI and sending information returned by SAPGUI after
 *    being started from the remote computer to R/3 via your RFC client and
 *    the RFC library.
 *
 *
 * 2. Using any RFC server in registered mode on the remote computer:
 *
 *    After starting an RFC server such as rfcexec or srfcserv delivered
 *    with the RFC SDK in registered mode on the remote computer, your RFC
 *    client only needs to issue RfcGuiPrepare with the information about the SAP
 *    gateway and the program ID which the RFC server is registered with.
 *    The parameter register_info contains this information must be in the
 *    form as defined in the following example:
 *
 *    GWHOST=hs0311 GWSERV=sapgw53 PROGID=hw1145.srfcserv
 *
 *    The RFC library on both computers will start SAPGUI on the remote
 *    computer as required.
 *
 *    At the end of your work, your RFC client can use <f RfcCancelRegisterServer>
 *    to normally end the registered RFC server on the remote computer.
 *
 *
 * @xref <f RfcGuiCheck>, <f RfcGuiStart>, <f RfcGuiReturn>
 *
 *-----------------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcGuiPrepare(
 RFC_HANDLE            handle,     /* @parm connection handle               */
 RFC_INT               remote_gui, /* @parm 0/1: locale/remote SAPGUI       */
 RFC_INT               debug,      /* @parm 0/1: without/with ABAP-debugger */
 RFC_INT               sapgui,     /* @parm 1/2: without/with invisible
                                    * SAPGUI after each RFC call
                                    */
 rfc_char_t                * register_info, /* @parm Info about the registered server
                                       * on the remote computer.
                                       * If using a registered server, the
                                       * parameter remote_gui must be set to 1.
                                       * If not, this parameter must be NULL or
                                       * an empty string (length 0)
                                       */
 RFC_GUI_START_PARAM  start_param, /* @parm <t RFC_GUI_START_PARAM>         */
 RFC_ERROR_INFO_EX  * error_info   /* @parm <t RFC_ERROR_INFO_EX>           */
 );


/*---------------------------------------------------------------------------
 * @func
 *
 *
 * Check whether SAPGUI is already running for this RFC connection.
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag 0 | SAPGUI is not running.
 * @flag 1 | SAPGUI is already running.
 *
 *--------------------------------------------------------------------------------*/
int SAP_API RfcGuiCheck(
 RFC_HANDLE            handle      /* @parm connection handle               */
 );



/*------------------------------------------------------------------------------
 * @func
 *
 *
 * see <f RfcGuiPrepare> for more details
 *
 *
 *------------------------------------------------------------------------------*/
void SAP_API RfcGuiStart(
 RFC_GUI_START_PARAM     start_param,    /* @parm <t RFC_GUI_START_PARAM>  */
 RFC_GUI_ERROR           error_message,  /* @parm <t RFC_GUI_ERROR>        */
 RFC_GUI_RETURN_PARAM    return_param    /* @parm <t RFC_GUI_RETURN_PARAM> */
 );


/*-------------------------------------------------------------------------
 * @func
 *
 *
 * see <f RfcGuiPrepare> for more details.
 *
 *
 *--------------------------------------------------------------------------*/
RFC_RC SAP_API RfcGuiReturn(
 RFC_HANDLE             handle,         /* @parm connection handle         */
 RFC_GUI_ERROR          error_message,  /* @parm <t RFC_GUI_ERROR>         */
 RFC_GUI_RETURN_PARAM   return_param,   /* @parm <t RFC_GUI_RETURN_PARAM>  */
 RFC_ERROR_INFO_EX    * error_info      /* @parm <t RFC_ERROR_INFO_EX>     */
);



/*------------------------------------------------------------------------------
 *
 * @func
 *
 * Check RFC_HANDLE for validity.
 *
 * RFC_ERROR_INFO may be NULL.
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>             | Handle is valid
 * @flag <e RFC_RC.RFC_INVALID_HANDLE> | Handle is invalid
 * @flag <e RFC_RC.RFC_CLOSED>         | Handle is valid but already disconnected
 *                                       RfcClose must be called to release resources
 * @flag <e RFC_RC.RFC_NOT_OWNER>      | Handle used in Wrong Thread
 *                                       (when this check is enabled)
 *
 * @ex
 * This comes handy in
 *
 *  assert(RFC_OK == RfcIsValidHandle(hRfc, NULL));
 *
 *  or in
 *
 *  assert(  (RFC_OK     == RfcIsValidHandle(hRfc, NULL)
 *        || (RFC_CLOSED == RfcIsValidHandle(hRfc, NULL)
 *        );
 *
 *
 *
 *
 *-----------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcIsValidHandle 
(
 RFC_HANDLE             handle,         /* @parm connection handle.         */
 RFC_ERROR_INFO_EX    * error_info      /* @parm <t RFC_ERROR_INFO_EX>. A Description of
                                         * from the error.
                                         */
);



/*-----------------------------------------------------------------------------
 * @func
 *
 * Let the RFC lib check it's internal structures for integrity.
 *
 * RFC_ERROR_INFO_EX may be NULL. This comes handy in
 * assert(RFC_OK == RfcHealthCheck(NULL));
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | No errors detected.
 * @flag <e RFC_RC.RFC_FAILURE> | Some systems area are damaged.
 *
 *----------------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcHealthCheck
(
 RFC_ERROR_INFO_EX    * error_info      /* @parm <t RFC_ERROR_INFO_EX> A Description of
                                         * from the error.
										 */
);


/*----------------------------------------------------------------
 * @cb  int |  RFC_ON_DISCONNECT |
 *
 * Function to be installed by <f RfcAttachContext>.
 *
 * The function is called once,  when the handle is being closed.
 * That is when the connection to the peer is discarded
 *
 * The function is called by the thread that perform the action that
 * leads to the closure of the handle.
 *
 *--------------------------------------------------------------*/
typedef void ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_DISCONNECT )
(
    RFC_HANDLE   hRfc,      /* @parm The handle being closed */
    unsigned int iReserved, /* @parm reserved. */
    void         *pContext  /* @parm The context installed by <f RfcAttachContext> */
);

/*----------------------------------------------------------------
 * @cb  int |  RFC_ON_FREE |
 *
 * Function to be installed by <f RfcAttachContext>.
 *
 * The function is called once,  when the handle is being discarded.
 * That is when the memory associated with the handle is released.
 *
 * The function is called by the thread that perform the action that
 * leads to the release of the handle.
 *
 *--------------------------------------------------------------*/
typedef void ( DLL_CALL_BACK_FUNCTION_PTR RFC_ON_FREE )
(
    RFC_HANDLE   hRfc,      /* @parm The handle being closed */
    unsigned int iReserved, /* @parm reserved. */
    void         *pContext  /* @parm The context installed by <f RfcAttachContext> */
);


/*-----------------------------------------------------------------------------
 * @func
 *
 * Let the application attach a context to a RFC handle.
 *
 * The application may:
 *
 * Just install a context <p pUserContext> that i will retrieve
 * via <f RfcGetAttachedContext>.
 *
 * Install an event handler that will be notified when the handle
 * closes. The handler, is passed the RFC handle being closed, and
 * the context that was passed to RfcAttachContext.  This context
 * may be NULL is RfcAttachContext is passed a NULL pointer.
 *
 * RfcAttachContext may be called only once per handle.  
 *
 * RfcAttachContext performs no reference counting on the attached
 * context.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>                | No errors detected.
 * @flag <e RFC_RC.RFC_INVALID_HANDLE>    | <p hRfc> is not a valid handle.
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER> | A context has allready been attached
 *                                          to this handle
 *
 * @xref <f RfcDetachContext>, <f RfcGetAttachedContext>
 *
 *----------------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcAttachContext
(
    RFC_HANDLE         hRfc,         /* @parm The handle being closed */    
    RFC_ON_DISCONNECT  onDisconnect, /* @parm An handler that is nottifed
                                      * when the handle hRfc closes.
                                      * A NULL pointer can also be passed
                                      */ 
    RFC_ON_FREE        onFree,       /* @parm An handler that is nottifed
                                      * when the handle hRfc is released,
                                      * A NULL pointer can also be passed
                                      */ 
    void              *pUserContext  /* @parm A context that will be passed
                                      * to the onDisconnect handler, 
                                      * when the RFC handle hRfc closes
                                      */                                        
);

/*-----------------------------------------------------------------------------
 * @func
 *
 * Retrieves the context registered by RfcAttachContext.
 *
 *
 * If no context was attached, or if RfcDettachContext has already been
 * called, the function returns RFC_FAILURE.
 *
 * When handle is a so called play-back handle, that is when it is the
 * handle that is passed to an installed function when processing a
 * tRFC call in a RFC server, RfcGetAttachedContext first tries to retrieve a
 * context attached to this handle.  If this fails, then it search if a 
 * context is attached to the handle that was returned by RfcAccept, and 
 * caries the network connection.  If an attached context is found in the
 * this accept_handle, then this context is returned, and <p *pFromCarrier>
 * is set to a non zero value.
 *
 * If RfcGetAttachedContext can retrieve an attached context without
 * attempting to search for a carrier handle, then <p *pFromCarrier>
 * is set to zero.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | No errors detected.
 * @flag <e RFC_RC.RFC_INVALID_HANDLE> | <p hRfc> is not a valid handle.
 * @flag <e RFC_RC.RFC_FAILURE> | No Context is attached.
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER> | <p ppUserContext> is NULL
 *
 * @xref <f RfcAttachContext>, <f RfcDetachContext>
 *
 *----------------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcGetAttachedContext
(
    RFC_HANDLE        hRfc,          /* @parm The handle being closed */    
    void            **ppUserContext, /* @parm Receives the comtex that was previously
                                      * installed.
                                      */
    int              *pFromCarrier   /* @parm The returned context, is the context
                                      * attachhed to the cpi-c handle that caries the 
                                      * communication.
                                      * A NULL pointer may be passed
                                      * 
                                      */    

);


/*-----------------------------------------------------------------------------
 * @func
 *
 * Explicitely dettach a context from an RFC handle.
 *
 * The previoulsy installed onDisconnect handler will not be called
 * when the RFC handle closes.
 *
 * If no context was attached, or if RfcDettachContext has already been
 * called, the function returns RFC_FAILURE.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | No errors detected.
 * @flag <e RFC_RC.RFC_INVALID_HANDLE> | <p hRfc> is not a valid handle.
 * @flag <e RFC_RC.RFC_FAILURE> | No Context is attached.
 * @flag <e RFC_RC.RFC_INVALID_PARAMETER> | one of the out parameter is NULL
 *
 * @xref <f RfcAttachContext>, <f RfcGetAttachedContext>
 *
 *----------------------------------------------------------------------------------------*/
RFC_RC SAP_API RfcDetachContext
(
    RFC_HANDLE         hRfc,          /* @parm The handle being closed */    
    RFC_ON_DISCONNECT *pOnDisconnect, /* @parm Receives the address of the handler
                                       * that was previously installed                                       
                                       */
    RFC_ON_FREE       *pOnFree,       /* @parm Receives the address of the handler
                                       * that was previously installed
                                       */                                                                                      
    void             **ppUserContext  /* @parm Receives the comtex that was previously
                                       * installed.
                                       */                                        
);


/*---------------------------------------------------------------------
 * @enum RFC_CTL |
 *
 *   Possibly control request values for <f RfcIoCtl> function.
 *
 *---------------------------------------------------------------------*/
/* @emem any error has been occured */
typedef enum {
    RFC_CTL_NONE = 0,   /* @emem NOP */
    RFC_CTL_SET_MODNO,  /* @emem [in] int, internal use, sapgui only,
			 * pass emode to the rfc handler
			 */
    RFC_CTL_GET_MODNO,  /* @emem [out] int, internal use, sapgui only,
			 * retrieve emode modus
			 */
    RFC_CTL_GET_TRACE,  /* @emem [out] TraceOn, C(1), ' ' or 'X' */
    RFC_CTL_GET_CARRIER,/* @emem [out] RFC_HANDLE,
			 * in tRFC servers, retrieves handle from connection
			 * that transport the calls
			 */
    RFC_CTL_SET_DELTA_COMPACT, /* @emem [in] int, turns on the recycling
				* delta manager handles.
				* handy on very long running session
				*/
    RFC_CTL_SET_CRM_MODE, /* @emem [in] sets the OLE-DB Provider's converter in
			   * SAP-CRM mode
			   */
    RFC_CTL_GET_MEM_STATS,/* @emem [out] RFC_INT[4], retrieves memory usage statistics
			   * ((RFC_INT*)pOut)   = current number of allocated blocks
			   * ((RFC_INT*)pOut)+1 = current number of allocated bytes
			   * ((RFC_INT*)pOut)+2 = max number of allocated blocks so far
			   * ((RFC_INT*)pOut)+3 = max number of allocated bytes so far
			   */
    RFC_CTL_GET_TYPE_MEM_STATS,/* @emem [out] Reserved*/

    RFC_CTL_SET_CSLIB_LOCK, /* @emem [in] Reserved */

    RFC_CTL_SET_RABAX_HOOK, /* @emem [in] Reserved sapgui only */
    RFC_CTL_IS_VALID_GUI_HANDLE,

    RFC_CTL_MAX /*@emem unused end of enum marker */
} RFC_CTL;



/* @func
 * general purpose helper function to retrieve internal information
 *
 * <p handle> handle from which the information is requested.
 * For connection independent information, this may be RFC_HANDLE_NULL.
 * For connection dependant information, this must be a valid handle.
 *
 * <p ctlReq> the information to retrieve.
 *
 * use the following call:
 *
 *
 * { RFC_HANDLE hRfcCarrier;
 *   RFC_RC = RfcIoCtl( hRfctRfcCall, RFC_CTL_GET_CARRIER, 0, NULL, sizeof(hRfcCarrier), &hRfcCarrier);
 * }
 * { RFC_INT fOn = 1;
 *   RFC_RC = RfcIoCtl(  RFC_HANDLE_NULL, RFC_CTL_SET_DELTA_COMPACT, sizeof(fOn), &fOn, 0, NULL);
 * }
 * { RFC_INT rngStats[4];
 *   RFC_RC = RfcIoCtl(  RFC_HANDLE_NULL, RFC_CTL_GET_MEM_STATS, 0, NULL, sizeof(rngStats), rngStats);
 * }
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | No errors detected
 * @flag <e RFC_RC.RFC_FAILURE> | Not implemented or not available
 */
DECL_EXP RFC_RC SAP_API RfcIoCtl 
( 
 RFC_HANDLE     handle,     /* @parm handle of rfc connection */
 RFC_CTL        ctlReq,     /* @parm information to retrieve */
 int            sizeofpIn,  /* @parm size of area pointed by pIN */
 void    *      pIn,        /* @parm pointer to input area */
 int            sizeofpOut, /* @parm size of output area pointed by pOut */
 void    *      pOut        /* @parm pointer to output area */
);

/*
 * For internal use only
 *
 */

int SAP_API RfcNormalizeTabOrFldName(SAP_CHAR * szOrgName,
                                     int uOrgSize,
                                     SAP_CHAR * szNewName,
                                     int uNewSize);

RFC_RC SAP_API RfcCreateLocalTransID(RFC_TID tid);
RFC_RC SAP_API RfcGuidToTid(SAP_UUID *pGuid, RFC_TID Tid);
RFC_RC SAP_API RfcTidToGuid(RFC_TID Tid, SAP_UUID *pGuid);

#ifdef SAPonNT
RFC_RC SAP_API RfcGetDomainFingerprintHex
(
	SAP_CHAR * szDomain,
	unsigned long  cbDomain,
	SAP_CHAR * szHexFP,
	unsigned long cbHexFP
);



RFC_RC SAP_API RfcGetUserSidHex
(
    SAP_CHAR * szUser,
	SAP_CHAR * szDomain,
    SAP_CHAR * szUserSidHex,
    unsigned long cbUserSidHex
);

#endif /*Define SAPonNT*/

#ifdef SAPGUI
/* ------------------------------------------------------------------
 *  functions must only be used by 'sapgui'
 *	and therefore not released to customers
 * ----------------------------------------------------------------*/
#include "saprfctr.h"
#endif /*SAPGUI*/




/*------------------------------------------------------
 *
 * @func
 *
 * Function to allocate memory for a <f RFC_STRING>.
 * The new allocated memory will be initialized with nulls. The space for terminating zero
 * will be allocated too.
 *
 * For better understanding one can consider RFC_STRING like  typedef unsigned char* RFC_STRING.
 * The standard C routines like strleng, strcpy etc. could be used to handle <t RFC_STRING>'s.
 *
 *
 * Warning:
 *
 *          1. Please use only this function for allocating
 *             of memory for RFC_STRING's.
 *             Do not use the functions malloc, realloc, free
 *             or C++ operators new/delete for string's memory management.
 *             You have to free strings created either by this function or by another 
 *             API'S like RfcGetData, RfcReceive, only using <f RfcFreeString>.
 *
 *          2. Be carefully  strlen(RfcAllocString(x)) returns always 0.
 *
 * @rdesc
 *
 * a RFC_STRING if no error occurred
 *
 * NULL if call fails.
 *
 *
 * @xref <f RFC_STRING>, <f RfcFreeString>.
 *
 *
 * @ex  Using RfcAllocString and RFC_STRING data type |
 *
 *  RFC_STRING         question,
 *                     answer;
 *  RFC_RC             rfc_rc;
 *  RFC_PARAMETER      exporting[2],
 *                     importing[2];
 *  RFC_TABLE          tables[1];
 *  rfc_char_t         exception[512];
 *  rfc_char_t       * except_ptr = exception;
 *
 *  
 *  // Allocate only export parameter
 *  // Allocation of import parameter will be done by the rfc library
 *  question = RfcAllocString (11); // allocs 12 bytes and fills them with NULL's
 *
 *  strncpyR (quetsion, cR("How are you"), 11); // we are UTF8 
 *
 *  exporting[0].name = cU("QUESTION");
 *  exporting[0].nlen = strlenU (exporting[0].name);
 *  exporting[0].type = RFCTYPE_STRING;
 *  exporting[0].addr = &question;
 *  exporting[0].leng = strlenR (question); // does not matter the value, 
 *                                          // because rfc lib restores this value 
 *                                          // from .addr field using strlenR () 
 *  exporting[1].name = NULL;
 * 
 *  importing[0].name = cU("MYANSWER");
 *  importing[0].nlen = strlenU (importing[0].name);
 *  importing[0].type = RFCTYPE_STRING;
 *  importing[0].addr = &answer;
 *  importing[0].leng = 0; // you do not know how long is the received string.
 *                         // The value of the field will be set by the RFC library
 *  importing[1].name = NULL;
 *
 *  tables[0].name = NULL
 *
 *  rfc_rc = RfcCallReceive (handle, cU("STFC_STRING"),
 *                           exporting,
 *                           importing,
 *                           tables,
 *                           &except_ptr);
 *  if( rfc_rc != RFC_OK )
 *  {
 *     ....
 *  }
 *   
 *
 *  // Free both import and export strings to avoid memory leaks
 *  RfcFreeString (question);
 *  RfcFreeString (answer);
 *
 *------------------------------------------------------*/
DECL_EXP RFC_STRING SAP_API RfcAllocString 
(
  unsigned int  size    /* @parm  size of string. The allocated memory have size
                         * and is initialized with nulls.
						 * The call RfcAllocString(0) allocs 1 byte memory
						 * and is initialized it with null.
                         */

);






/*------------------------------------------------------
 * @func
 *
 * Function to free a <t RFC_STRING> allocated by <f RfcAllocString>.
 * Warning: Please use only this function for freeing
 *          of memory for RFC_STRING's allocated by <f RfcAllocString>.
 *          Do not use the functions malloc, realloc, free
 *          or C++ operators new/delete for string's memory managment.
 *          You have to use this function only for strings allocated by <f RfcAllocString>.
 *
 * @rdesc
 *
 * No returns
 *
 * @xref <f RFC_STRING>, <f RfcAllocString>.
 *
 *------------------------------------------------------*/
DECL_EXP void SAP_API RfcFreeString 
(
 RFC_STRING string /* @parm RFC_STRING allocated previously by <f RfcAllocString>.
				    *
                    * If string is NULL, the function simply returns.
                    */
);



/*------------------------------------------------------
 * @func
 *
 * Function to convert 2 Byte ISO-Language in 1-byte SAP-Language.
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag 0 | OK
 * @flag 1 | ISO-language not found
 *
 * @xref <f RfcOpenEx>, <f RfcSapLangToIso>.
 *
 *------------------------------------------------------*/
DECL_EXP int SAP_API RfcIsoLangToSap 
(
 rfc_char_t	*isoLang,	/* @parm String ISO language */
 rfc_char_t	*sapLang	/* @parm String SAP language */
);




/*------------------------------------------------------
 * @func
 *
 * Function to convert 1-byte SAP-Language into 2 Byte ISO-Language.
 *
 *
 * @rdesc
 *
 * always 0 (OK).
 *
 * If saplang could not be found, isoLang = isoLang & blank.
 *
 * @xref <f RfcOpenEx>, <f RfcIsoLangToSap>.
 *
 *------------------------------------------------------*/
int SAP_API RfcSapLangToIso
(
 rfc_char_t		*sapLang,	/* @parm String ISO language */
 rfc_char_t		*isoLang	/* @parm String SAP language */
);


/*-------------------------------------------------------------------------
 *
 * @enum RFC_CANCEL_TYPE |
 * cancel mode to be set by <f RfcCancel> API.
 *
 * @xref <f RfcCancel>.
 *
 *--------------------------------------------------------------------------*/
typedef enum
{
 RFC_CANCEL_HARD = 0, /* @emem Break the already running rfc function on ABAP-Side,
                       *        destroy user context and keep rfc connection open.
                       */

 RFC_CANCEL_SOFT = 1, /*  @emem Break the already running rfc function on ABAP-Side,
                       *         try to hold user context on the ABAP-Side and
                       *         keep rfc connection open.
                       */

 RFC_CANCEL_CLOSE = 2 /*  @emem Break the already running rfc function on ABAP-Side,
                       *        destroy user context on the ABAP-Side and
                       *         close the rfc connection.
                       */

} RFC_CANCEL_TYPE;


/*-----------------------------------------------------------------------
 * @func
 *
 * Function to cancel a RfcCall. This function can be used only by a rfc client running in
 * non blocking RfcCall mode (s. <f RfcListen>). The function itself is a blocking rfc call.
 *
 * If you try to cancel a rfc function running on any external server, the call fails
 * and server exits.
 *
 *
 * @rdesc Returns one of the following values:
 *
 *
 * @flag RFC_CANCELED | If current running ABAP-Function could be canceled.
 *                            If parameter <e RFC_CANCEL_TYPE.RFC_CANCEL_SOFT>
 *                            was used, the user context in abap function was hold.
 *                            Rfc connection is not closed.
 *
 * @flag RFC_CLOSED    | After canceling the current running ABAP-Funktion,
 *                            the rfc connection have been closed. This is the case if
 *                            the second input parameter is <e RFC_CANCEL_TYPE.RFC_CANCEL_CLOSE>
 *                            or partner closed rfc connection.
 *
 * @flag RFC_FAILURE    | If any error has occurred.
 *
 *
 * @xref <f RFC_CANCEL_TYPE>, <f RfcCall>, <f RfcListen>, <f RfcReceive>.
 *
 * @ex using RfcCancel |
 *
 * RFC_RC rc;
 * RFC_HANDLE handle;
 *
 * ...
 *
 * rc = RfcCall( handle, .... );
 *
 * do
 * {
 *  rc = RfcListen( handle );
 *  if( rc == RFC_RETRY )
 *  {
 *   if (somethingHappened() == TRUE)
 *     rc = RfcCancel ( handle, RFC_CANCEL_SOFT);
 *
 *	  ....
 *
 *	doSomething();
 *  }
 * } while ( rc == RFC_RETRY );
 *
 * if( rc == RFC_OK )
 *  rc = RfcReceive( handle, .... );
 * ...
 *
 *----------------------------------------------------------------------*/
RFC_RC SAP_API RfcCancel
(
 RFC_HANDLE             rfc_handle,          /* @parm connection handle. */
 RFC_CANCEL_TYPE        cancel_type          /* @parm cancel type. */
);





/*-------------------------------------------------------------------------
 * @enum RFC_CONVERSION_FLAG |
 *
 * Flag to turn off/on the conversion for <f RFC_CHAR>'s between own and partners codepage.
 * Will be used by <f RfcCharConversion> .
 *
 * @xref <f RfcCharConversion>
 *
 *--------------------------------------------------------------------------*/
typedef enum
{
  On  = 0,	/* @emem Turn conversion for <f RFC_CHAR> between own and partners codepage on. */
  Off = 1	/* @emem Turn conversion for <f RFC_CHAR> between own and partners codepage off. */
} RFC_CONVERSION_FLAG;





/*------------------------------------------------------
 * @func
 * Function to turn On/Off the conversion for <f RFC_CHAR> between own and partners codepage.
 *
 * If flag <f RFC_CONVERSION_FLAG> is set to Off, the RFC_CHAR data will be
 * handled like row data.
 * 	
 * Attention: This feature is to be used only in exception situations
 * if using of <f RFC_BYTE> is impossible.
 *
 * Important: this feature is only available from 4.6C release onwards.
 *
 * Warning: If both client and server call this API, the incoming data
 *          will be handled according the call made locally. This feature can be used,
 *          to create follow situation: Server does not convert incoming data and client
 *          does. To allow this the server should call RfcCharConversion (handle, Off) and the
 *          client should call RfcCharConversion (handle, On).
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If successful.
 * @flag <e RFC_RC.RFC_FAILURE> | If any error has occurred.
 *
 * @xref <f RFC_CHAR>, <f RfcSetCodePage>, <f RFC_CONVERSION_FLAG>.
 *
 *------------------------------------------------------*/
RFC_RC SAP_API RfcCharConversion
(
   RFC_HANDLE           rfc_handle, /* @parm RFC handle */
   RFC_CONVERSION_FLAG  flag        /* @parm Flag to turn the conversion of char's on or off.
			       	     *
				     *  If <e RFC_CONVERSION_FLAG.Off>, RFC_CHAR will be
                                     *  managed like binary data type.
				     *
				     *  If <e RFC_CONVERSION_FLAG.On>, RFC_CHAR will be
				     *  managed as usual.
                                     */
);


#if   defined (SAPonOS400)
 
/* CCQ_CCU_OFF */

/*------------------------------------------------------
 * @func
 * Function to convert ascii to ebcdic in an environment were an
 * external ebcdic program links to the ascii librfc
 * 	
 * @rdesc
 *
 * <e RFC_RC.RFC_OK> if successfull.
 *
 * <e RFC_RC.RFC_FAILURE> if any error has occured.
 *
 *
 * @xref <f RfcConvertE2A>.
 *
 *------------------------------------------------------*/
 RFC_RC SAP_API RfcConvertA2E(char *		 org_dst,
                              char *		 src,
                              int            dst_size,
                              int            src_size,
                              int            fill_char,
                              int            sap_codepage);

/*------------------------------------------------------
 * @func
 * Function to convert ebcdic to ascii in an environment were an
 * external ebcdic program links to the ascii librfc
 * 	
 * @rdesc
 *
 * <e RFC_RC.RFC_OK> if successfull.
 *
 * <e RFC_RC.RFC_FAILURE> if any error has occured.
 *
 *
 * @xref <f RfcConvertA2E>.
 *
 *------------------------------------------------------*/
 RFC_RC SAP_API RfcConvertE2A(char *      org_dst,
                              char *      src,
                              int         dst_size,
                              int         src_size,
                              int         fill_char,
                              int         sap_codepage);

/* CCQ_CCU_ON*/

#endif


/*-----------------------------------------------------------------------------------------
 * @func
 *
 * Copies the received cookie version 2 ticket into given user allocated memory area.
 * The request for the ticket generation has to be done by <f RfcOpenEx> function with flag GETSSO2
 * before calling this API.
 *
 * Features:
 *
 * This api can be called only once after <f RfcOpenEx>.
 * The second call of this api for the same handle fails.
 *
 * The call without request in <f RfcOpenEx> fails.
 *
 * Warning: The caller is responsible for memory allocation/deallocation for ticket.
 * The memory is to be allocated before api is called.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>             | If call succesful
 * @flag <e RFC_RC.RFC_INVALID_HANDLE> | If handle is invalid
 * @flag <e RFC_RC.RFC_FAILURE>        | If call fails.
 *
 * @xref <f RfcOpenEx>.
 *-------------------------------------------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcGetTicket 
(
 RFC_HANDLE handle,	/*@parm rfc handle */
 RFC_CHAR*  ticket  /*@parm  Pointer on memory area allocated for ticket by caller. Must be long enough to receive the ticket*/
);


/*------------------------------------------------------
 *
 * @func
 *
 *  Function to allocate, reallocate or free a <t RFC_XSTRING>.
 *  
 *  It is necessary to initialize the defined xstring variable,
 *  before using them at first time.
 * 
 *  If the initialization has not be done, the behaviour of this
 *  API is undefined.
 *
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 *  @xref <f RFC_BLOB> <f RFC_XSTRING>
 *
 *  @ex  Using RfcResizeXString and RFC_XSTRING |
 *
 *  RFC_XSTRING        question,
 *                     answer;
 *  RFC_RC             rfc_rc;
 *  RFC_PARAMETER      exporting[2],
 *                     importing[2];
 *  RFC_TABLE          tables[1];
 *  rfc_char_t         exception[512];
 *  rfc_char_t       * except_ptr = exception;
 *
 *
 *  // initialise xstrings
 *  question.content = NULL;
 *  question.length = 0;
 *
 *  answer.content = NULL,
 *  answer.length = 0;
 *  
 *  // Allocate only export parameter
 *  // Allocation of import parameter will be done by the rfc library
 *  RfcResizeXString (&question, 2000);
 *
 *  exporting[0].name = cU("QUESTION");
 *  exporting[0].nlen = strlenU (exporting[0].name);
 *  exporting[0].type = RFCTYPE_XSTRING;
 *  exporting[0].addr = &question;
 *  exporting[0].leng = question.length; // does not matter the value, 
 *                                       // because rfc lib restores this value 
 *                                       // from RFC_XSTRING.length field of the data type. 
 *  exporting[1].name = NULL;
 * 
 *  importing[0].name = cU("MYANSWER");
 *  importing[0].nlen = strlenU (importing[0].name);
 *  importing[0].type = RFCTYPE_XSTRING;
 *  importing[0].addr = &answer;
 *  importing[0].leng = 0; // you do not know how long is the received string.
 *                         // The value of the answer.length field will be set by the RFC library
 *  importing[1].name = NULL;
 *
 *  tables[0].name = NULL
 *
 *  rfc_rc = RfcCallReceive (handle, cU("STFC_XSTRING"),
 *                           exporting,
 *                           importing,
 *                           tables,
 *                           &except_ptr);
 *  if( rfc_rc != RFC_OK )
 *  {
 *     ....
 *  }
 *   
 *
 *  // Free both import and export xstrings to avoid memory leaks
 *  RfcResizeXString (&question, 0);
 *  RfcResizeXString (&answer, 0);
 *------------------------------------------------------*/
DECL_EXP RFC_RC SAP_API RfcResizeXString 
(
    RFC_XSTRING    *xstring, /*@parm pointer to xstring to be resized */
    RFC_INT        length    /*@parm new length*/
);




/*--------------------------------------------------------------
 *
 * @func
 *
 * When initiating a connection, the underlying CPI-C layer waits 
 * by default 60 seconds for the connection to succeed.
 *
 * This can be controlled by environment variable CPIC_CONNECT_TIMEOUT,
 * environment variable CPIC_CONNECT_TIMEOUT,<nl>
 * or by the CPI-C API SAP_CMCONNECTTIMEOUT().
 * (see documentation of SAP CPIC gateway). 
 *
 * The API RfcSetSystemTimeout allows to control this value at the RFC-API
 * level.
 *
 * With this call an RFC process is able to overwrite the value set by the
 * environment variable CPIC_CONNECT_TIMEOUT.
 *
 * Warning: This call does not set the timeout for maximal RFC call processing time. 
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 *  @xref <f RfcOpenEx>, <f RfcAccept>, <f RfcCallRecieve>
 * 
 */
RFC_RC SAP_API RfcSetSystemTimeout
(
 RFC_INT    timeout  /*@parm timeout in milliseconds */
);


/*--------------------------------------------------------------
 *
 * @func
 *
 * Function to retrieve enqueue key sent by R/3.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 */
RFC_RC SAP_API RfcGetEnqueueKey 
(
    RFC_HANDLE    handle,                   /*@parm valid rfc handle */                 
    rfc_char_t  * enqueue_key,              /*@parm area to be filled*/
    unsigned      enqueue_key_size          /*@parm size of enqueue_key in chars (including term zero) */
);

/*--------------------------------------------------------------
 *
 * @func
 *
 * Function to set enqueue key.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 */
RFC_RC SAP_API RfcSetEnqueueKey 
(
    RFC_HANDLE    handle,       /*@parm valid rfc handle */                 
    rfc_char_t  * enqueue_key   /*@parm key value */
);



/*--------------------------------------------------------------
 *
 * @func
 *
 * Function to retrieve client id sent by R/3.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 */
RFC_RC SAP_API RfcGetClientId
(
    RFC_HANDLE    handle,                 /*@parm valid rfc handle */
    
    SAP_RAW     * client_id,              /*@parm area to be filled*/

    unsigned    * client_id_size          /*@parm in: size of enqueue_key in bytes 
                                           *      out: size of retrieved client id
                                           */
);



/*--------------------------------------------------------------
 *
 * @func
 *
 * Function to retrieve client id sent by R/3 but in the new format.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 */
RFC_RC SAP_API RfcGetClientIdEx
(
    RFC_HANDLE    handle,                 /*@parm valid rfc handle */
    
    SAP_RAW     * client_id,              /*@parm area to be filled*/

    unsigned    * client_id_size          /*@parm in: size of enqueue_key in bytes 
                                           *      out: size of retrieved client id.
                                           *           if 0, nothing received.
                                           */
);



/*--------------------------------------------------------------
 *
 * @func
 *
 *
 * Function to set the client id which wiil be sent to R/3.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 */
DECL_EXP RFC_RC SAP_API RfcSetClientIdEx 
(
    RFC_HANDLE    handle,                 /*@parm valid rfc handle */
    
    SAP_RAW     * client_id,              /*@parm client_id to be set*/

    unsigned      client_id_size          /*@parm size of client_id in bytes */
);



/*-----------------------------------------------------------------------
 * 
 * @func
 *
 * A UUID (universal unique identifier) is a 16-byte binary
 * quantity which is guaranteed to be unique in space and time
 * while generating a UUID never needs any centralized look up.
 *
 * This API creates an uuid as described.
 *
 * On NT we use the RPC uuid generator, on the other platforms
 * an own implementation.
 *
 * There is no need to take care of the internal structure of uuid
 * The id generated are unique even binary, so that
 * they can be transported across platforms as X(16).
 *  
 * Warning: The caller is responsible for memory allocation/deallocation for uuid.
 * The memory is to be allocated before api is called.
 * 
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 */
RFC_RC SAP_API RfcCreateUuid 
(
    RFC_BYTE * uuid /* @parm pointer to memory area of 16 bytes*/ 
);



/* 
 * @struct RFC_CALL_INFO |
 * structure returned by <f RfcGetCallInfo> or <f RfcGetLuwInfo> describing some metadata of this rfc connection.
 * 
 * Information for currently proceeded rfc call at server side.
 *  
 */
typedef struct
{
    RFC_FUNCTIONNAME    function_name;                  /* @field proceeded function*/
    rfc_char_t          function_counter[8];            /* @field which function in LUW is already proceeded*/
    RFC_TID             tid;                            /* @field transaction id */
    RFC_QNAME           qname;                          /* @field queue name*/
    RFC_QCOUNT          qcounter;                       /* @field queue counter */
    rfc_char_t          call_type;                      /* @field synchron: 'S', 
                                                                  transactional: 'T', 
                                                                  outbound queue 'O'*/
} RFC_CALL_INFO;



/*--------------------------------------------------------------------------------------------------------
 * @func
 *
 * This API retrieves at server side available information for currently proceeded rfc call.
 *
 * Information could be retrieved for:
 *
 *  1. synchron rfc call:       call_type = 'S'. Function name will be retrieved
 *  2. transactional rfc call:  call_type = 'T'. Additionally transaction id and function counter are available
 *  3. queued rfc call:         call_type = 'O'. Additionally outbound queue name and queue counter are available 
 *
 * @comm Please notice: his call have to be called at rfc server side. If the API is called
 *       at client side, RFC_FAILURE will be returned.
 * 
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 * @xref <f RfcGetLuwInfo>
 *
 *-----------------------------------------------------------------------------------------------------*/
RFC_RC  SAP_API RfcGetCallInfo 
(
    RFC_HANDLE      handle,         /* @parm In:  valid rfc handle */  
    RFC_CALL_INFO * rfc_call_info   /* @parm Out: available call information */
);


#ifndef SAP_CONV_ID_LN
#define RFC_CONV_ID_LN  8
#else
#define RFC_CONV_ID_LN  SAP_CONV_ID_LN
#endif

/*
 * @struct SAP_RFC_STATISTIC |
 * structure returned by <f RfcStatistic> describing
 * some statistical information about this RFC connection.
 * 
 * Please notice that some fields are only valid either at server or at client side.
 * Some data is available only for some kinds of rfc like trfc.
 */
typedef struct
{
    unsigned    statistic;      /* @field rfc statistic ON=0:OFF=1, if OFF no warranty for statistic data*/

    double      data_rcv;       /* @field present received data                 */

    double      dsum_rcv;       /* @field number of received data for handle    */
    double      sum_rcv;        /* @field number of received data for function  */

    double      data_snd;       /* @field present sent data                     */
    double      sum_snd;        /* @field number of sent data for function      */
    double      dsum_snd;       /* @field number of sent data for handle        */
 
    double      sum_rexe;       /* @field execution time at receiver for function */
    double      sum_exe;        /* @field execution time at sender for function */

    double      sum_call;       /* @field calling time for function call        */
    double      dsum_call;      /* @field calling time for handle               */

    double      sum_wtime;      /* @field overall writing time for function     */
    double      dsum_wtime;     /* @field overall writing time for handle       */

    double      sum_rtime;      /* @field overall reading time for function     */
    double      dsum_rtime;     /* @field overall reading time for handle       */

    double      calls;          /* @field number of calls                       */

    rfc_char_t  ip[46];         /* @field my ip address                         */
    rfc_char_t  rip[46];        /* @field remote ip address                     */
    rfc_char_t  dest[33];       /* @field my destination id                     */
    rfc_char_t  rdest[33];      /* @field remote destination id                 */
    rfc_char_t  progname[128];  /* @field calling program                       */
    
    RFC_FUNCTIONNAME    funcname;/* @field called function                      */
    
    rfc_char_t  userid[13];     /* @field user id                               */                       
    rfc_char_t  rfc_caller[13]; /* @field rfc caller (whoami)                   */
    rfc_char_t  rfc_cclient[13];/* @field caller client (sender)                */
    rfc_char_t  start[48];      /* @field start time&date&vers.                 */
    RFC_TID     rfc_tid;        /* @field Transaction ID                        */
    
    RFC_BYTE    conversation_id[RFC_CONV_ID_LN]; /* @field conversation id      */
}
SAP_RFC_STATISTIC;



/*------------------------------------------------------------------------------------------
 * @func
 *
 * This API retrieves some rfc statistic available for the handle
 *
 * 
 *
 * @comm Some statistic data is writen to trace file too if trace is on
 * and environment variable RFC_TRACE_STATISTIC is set with 1
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 *---------------------------------------------------------------------------*/
RFC_RC  SAP_API RfcStatistic
(
    RFC_HANDLE          rfc_handle, /* @parm valid rfc handle */
    SAP_RFC_STATISTIC * statistic   /* @parm struct to be filled */
);



/*--------------------------------------------------------------------------------------
 * @func
 * 
 * Retrieves at server side available information for already proceeded LUW.
 *
 * A row of retrieved table has format <t RFC_CALL_INFO>.
 *
 * The output table has to be created before calling this API.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 * @xref <f RfcGetCallInfo>
 *
 *
 * @ex using RfcGetLuwInfo: |
 *
 * RFC_CALL_INFO  *  call_info;
 *
 *  ...
 *
 * itab = ItCreate (NULL, sizeofR(rfc_call_info),0, 0);
 *  
 * ...
 * 
 * rfc_rc = RfcGetLuwInfo (rfc_handle, itab);
 * 
 * if (rfc_rc != RFC_OK)
 * { ... }
 * 
 * ...
 * 
 * call_info = (RFC_CALL_INFO*) ItGetLine (itab, 1);
 * 
 * ...
 * 
 * ItFree (itab);
 *
 *------------------------------------------------------------------------------------*/
RFC_RC  SAP_API RfcGetLuwInfo
(
    RFC_HANDLE  handle, /* @parm In:  valid rfc handle */
    ITAB_H      itab    /* @parm Out: available luw info with <t RFC_CALL_INFO> format*/
);





/*---------------------------------------------------------------------
 * @enum RFC_EXTERNAL_LOGON_RC |
 *
 *   Possibly return values for <f RFC_EXTERNAL_LOGON> function.
 *
 * @xref <f RFC_EXTERNAL_LOGON>.
 *---------------------------------------------------------------------*/
typedef enum
{
    RFC_EXTERNAL_LOGON_OK = 0,      /* @emem logon succesfull */
    RFC_EXTERNAL_LOGON_FAILED = 1   /* @emem logon failed */
} RFC_EXTERNAL_LOGON_RC;


#define RFC_CLIENT_ID_LN_CHAR       3+1
#define RFC_USER_ID_LN_CHAR         12+1
#define RFC_TCODE_LN_CHAR           20+1
/* @type RFC_EXTERNAL_LOGON_CNTL |
 * a structure containing logon data received by RFC-Lib
 */
typedef struct
{
    RFC_FUNCTIONNAME    function_name;  /* @field called function*/
    rfc_char_t          client  [RFC_CLIENT_ID_LN_CHAR];
    rfc_char_t          language[1+1];
    rfc_char_t          user    [RFC_USER_ID_LN_CHAR];
    rfc_char_t          tcode   [RFC_TCODE_LN_CHAR];
    rfc_char_t          reserved[512+1];
} RFC_EXTERNAL_LOGON_CNTL;


/*------------------------------------------------------------------------
 * @cb RFC_EXTERNAL_LOGON_RC |		RFC_EXTERNAL_LOGON |
 *
 * Prototype for use implemented logon handler.
 *
 * This interface allowes to secure the incoming rfc calls for external rfc servers.
 *
 * For example it is possible,
 * that logon handler permits for user XYZ to calling the function ABCD.
 * 
 * Function valid to this interface makes checks whether the
 * received call have to be procced or have to be permitted.
 *
 * If user function returns RFC_EXTERNAL_LOGON_FAILED 
 * rfc library calls RfcAbort with a message.
 * 
 * The message can be written in case of failure.
 *
 * If no message has been written by user function, rfc library creates a default message.
 *
 * 
 *
 * @ ex using <f RFC_EXTERNAL_LOGON>
 * 
 * // simple logon handler
 * static RFC_EXTERNAL_LOGON_RC DLL_CALL_BACK_FUNCTION rfc_logon_check 
 *              (RFC_HANDLE handle, RFC_EXTERNAL_LOGON_CNTL * logon_cntl, rfc_char_t              * message)
 * {
 *  // this logon handler permits for user XYZ to call function ABCD
 *  if (memcmpU (logon_cntl->user, cU("XYZ"), 3) == 0) &&
 *      memcmpU (logon_cntl->function_name, cU("ABCD"), 4) == 0)
 *  {
 *      strcpyU (message, cU("No autority for user XYZ to calling ABCD"));
 *
 *      return RFC_EXTERNAL_LOGON_FAILED;
 *  }
 *
 *  return RFC_EXTERNAL_LOGON_OK;
 * }
 *
 * ...
 * 
 * mainU (int argc, rfc_char_t **argv)
 * {
 *  RFC_HANDLE  handle;
 *  RFC_RC      rfc_rc = RFC_OK;
 *       
 *  handle = RfcAccept (argv);
 *  
 *  ...
 *  
 *  rfc_rc = RfcInstallExternalLogonHandler (rfc_handle, rfc_logon_check);
 *  
 *  ...
 *  
 *  rfc_rc = RfcDispatch (handle);
 * 
 *  ...
 * }
 *------------------------------------------------------------------------*/
typedef RFC_EXTERNAL_LOGON_RC (DLL_CALL_BACK_FUNCTION_PTR RFC_EXTERNAL_LOGON)
(
          RFC_HANDLE                  handle,       /*@ parm handle */
		  RFC_EXTERNAL_LOGON_CNTL   * logon_cntl,   /* @parm In: pointer on logon data structure*/
          rfc_char_t                * error_message /* @parm Out: pointer to area which can take max 256 chars.
                                                     * 
                                                     */
);


/*--------------------------------------------------------------------------------------
 * @func
 * 
 * Installs user implemented logon handler.
 *
 * Activ only for rfc server.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred and
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 * @xref <f RFC_EXTERNAL_LOGON>
 *
 */
RFC_RC SAP_API RfcInstallExternalLogonHandler 
(
    RFC_HANDLE          handle,     /*@parm valid accepted rfc handle*/ 
    RFC_EXTERNAL_LOGON  function    /*@parm user implemented logn handler */
);






/* @func
 * Calls an ABAP/4 function module via RFC.
 *
 * The structures <t RFC_PARAMETER> and  <t RFC_TABLE> contain
 * name and description of the 'exporting' und 'changing' parameters 
 * and tables (internal ABAP/4 table) of the function's interface.
 * The function returns after the call request is send.
 * If the function returns RFC_OK, there is no guaranty, that
 * the call was already received by the target system.
 *
 * @rdesc
 * returns RFC_OK or RFC_FAILURE.
 *
 * @xref <f RfcReceiveEx>, <f RfcCallReceiveEx>.
 *
 * @ex calling using RfcCallEx. |
 *
 *   RFC_RC             rfc_rc;
 *   RFC_PARAMETER      exporting[32];
 *   RFC_PARAMETER      changing[32]
 *   RFC_TABLE          tables[32];
 *
 *   exporting[0].name = NULL;
 *   changing[0].name  = NULL;
 *   tables[0].name    = NULL;
 *
 *   ....
 *   
 *   rfc_rc = RfcCall (handle, "RFC_AN_EXAMPLE",
 *                                  exporting,
 *                                  changing,
 *                                  tables);
 *   if( rfc_rc != RFC_OK )
 *   {
 *      ....
 *
 *------------------------------------------------------------------*/
RFC_RC SAP_API RfcCallEx
(
    RFC_HANDLE      handle,    /* @parm connection handle.      */
    CONST_RFC rfc_char_t *function,  /* @parm function module to call.*/
    RFC_PARAMETER * exporting, /* @parm 'exporting' parameters. */
    RFC_PARAMETER * changing,  /* @parm 'changing' parameters   */
    RFC_TABLE     * tables     /* @parm 'tables' parameters.    */
);




/*----------------------------------------------------------------------
 * @func
 * receives the return values from a function call issued by <f RfcCallEx>.
 *
 * allows to receive the answer to a RFC call and must be called
 * after RfcCallEx was issued. The tables' description ( <t RFC_TABLE> )
 * must be identical to the one used in RfcCallEx.
 *
 * The function RfcReceiveEx waits till the answer is received.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK> | The call was successfully completed and
 *       the values of the returned parameters were filled into the
 *       fields being supplied by the <t RFC_PARAMETER> array.
 * @flag <e RFC_RC.RFC_FAILURE> | An internal error has occurred.
 *       <f RfcLastError> or <f RfcLastErrorEx> may give more information.
 * @flag <e RFC_RC.RFC_EXCEPTION> | The callee has raised an exception.
 *       The field '*exception' points to the name of the exception. No
 *       data were transported.
 * @flag <e RFC_RC.RFC_SYS_EXCEPTION> | The local or remote RFC system has
 *       raised an exception. The parameter '*exception' points to the name of
 *       the exception. The connection was automatically closed by the
 *       system and <f RfcLastError> or <f RfcLastErrorEx> may give more
 *       information on the origin
 *       of the error. Two exceptions may occur now: SYSTEM_FAILURE and
 *       COMMUNICATION_FAILURE.
 * @flag <e RFC_RC.RFC_CALL> | The callee has issued a RFC call to the
 *       caller of RfcReceive. No data are transported. The call request
 *       must be handled by using the functions <f RfcDispatch> or by
 *       <f RfcGetName>, <f RfcGetData> and <f RfcSendData>
 *       before another call to RfcReceive can be done.
 *
 * @xref <f RfcCallEx> <f RfcCallReceiveEx>.
 */
RFC_RC SAP_API RfcReceiveEx
(
  RFC_HANDLE        handle,      /* @parm connection handle.     */
  RFC_PARAMETER   * importing,   /* @parm 'importing' parameters.*/
  RFC_PARAMETER   * changing,    /* @parm 'changing' parameters  */
  RFC_TABLE       * tables,      /* @parm 'tables' parameters.   */
  rfc_char_t     ** exception    /* @parm output parameter:
                                  * pointer to exception string.
                                  * only set, if <e RFC_RC.RFC_EXCEPTION> or
                                  * <e RFC_RC.RFC_SYS_EXCEPTION> is returned.
                                  */
);





/*---------------------------------------------------------------------
 *
 * @func
 *
 * calls an ABAP/4 function module and receives the return values
 * in one function call.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK> |
 *       The call was successfully completed and
 *       the values of the returned parameters were filled into the
 *       fields being supplied by the <t RFC_PARAMETER> array.
 *
 * @flag <e RFC_RC.RFC_FAILURE> |
 *       An internal error has occurred.
 *       <f RfcLastError> or <f RfcLastErrorEx> may give more information.
 *
 * @flag <e RFC_RC.RFC_EXCEPTION> |
 *       The callee has raised an exception.
 *       The field '*exception' points to the name of the exception. No
 *       data were transported.
 *
 * @flag <e RFC_RC.RFC_SYS_EXCEPTION> |
 *       The local or remote RFC system has
 *       raised an exception. The parameter '*exception' points to the name of
 *       the exception. The connection was automatically closed by the
 *       system and <f RfcLastError>  or <f RfcLastErrorEx> may give more
 *       information on the origin
 *       of the error. Two exceptions may occur now: SYSTEM_FAILURE and
 *       COMMUNICATION_FAILURE.
 *
 * @flag <e RFC_RC.RFC_CALL> |
 *       The callee has issued an RFC call to the
 *       caller of RfcReceive. No data are transported. The call request
 *       must be handled by using the functions <f RfcDispatch> or by
 *       <f RfcGetName>, <f RfcGetData> and <f RfcSendData>
 *       before an other call to RfcReceive can be done.
 *
 *
 * @xref <f RfcCallEx> <f RfcReceiveEx>.
 *
 *---------------------------------------------------------------------------*/
RFC_RC SAP_API RfcCallReceiveEx
(
    RFC_HANDLE      handle,     /* @parm connection handle.         */
    CONST_RFC rfc_char_t *function,   /* @parm function module to call    */
    RFC_PARAMETER * exporting,  /* @parm 'exporting' parameters.    */
    RFC_PARAMETER * importing,  /* @parm 'importing' parameters.    */
    RFC_PARAMETER * changing,   /* @parm 'changing' parameters.     */
    RFC_TABLE     * tables,     /* @parm 'tables' parameters.       */
    rfc_char_t   ** exception   /* @parm output parameter:
                                 * pointer to exception string.
                                 * only set, if <e RFC_RC.RFC_EXCEPTION> or
                                 * <e RFC_RC.RFC_SYS_EXCEPTION> is returned.
                                 */
);

/*---------------------------------------------------------------------
 *
 * @func
 *
 * defines the maximal number of active CPIC connections. In this case 
 * the environment variable CPIC_MAX_CONV will never be used. You have to
 * call this API before any another RFC-API.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 * @xref <f RfcCallEx> <f RfcReceiveEx>.
 *
 *---------------------------------------------------------------------------*/
RFC_RC SAP_API RfcSetMaxCpicConn 
(
    unsigned maximalNumberOfConnections /* @parm maximal connections number */
);


/*---------------------------------------------------------------------
 *
 * @func
 *
 * This api admits the CPIC layer for using of sideinfo file.
 * More details are contained in the CPIC documentation
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *
 *
 *---------------------------------------------------------------------------*/
RFC_RC SAP_API RfcUseSideInfo 
(
    unsigned use /* @parm using allowed */
);

/*---------------------------------------------------------------------
 *
 * @func
 *
 * This api admits the RFC-Library to reset the current trace directory.
 * In case of calling of this API the value of the environment variable 
 * RFC_TRACE_DIR will be overwritten.
 *
 * If the directory does not exist the call will be ignored.
 *
 * @rdesc Returns one of the following values:
 *
 * @flag <e RFC_RC.RFC_OK>      | If no error occurred
 * @flag <e RFC_RC.RFC_FAILURE> | Elsewhere.
 *---------------------------------------------------------------------------*/
RFC_RC SAP_API RfcResetTraceDir 
(
    const rfc_char_t * newTraceDirectory /* @parm Path to the new trace location */
);


/*
 * For internal use only
 * Never use these API in your application
 */
void SAP_API RfcSetBasisTraceFile (void* f);
void SAP_API RfcSetBasisTraceLevel (int level);
RFC_RC SAP_API RfcDsrInit (rfc_char_t * appName,
                           unsigned     statOn,
                           unsigned     userB,
                           void       * statData,
                           rfc_char_t * custTracePath);

RFC_RC SAP_API RfcCallReceiveTimed (RFC_HANDLE  handle,
									rfc_char_t    *  function,
									RFC_PARAMETER *  exporting,
									RFC_PARAMETER *  importing,
									RFC_TABLE     *  tables,
									rfc_char_t   **  exception_ptr,
									unsigned         timeout);
#ifdef __cplusplus
}
#endif /*cplusplus*/

#endif /* SAPRFC_H */
#endif /* RFC_H */

