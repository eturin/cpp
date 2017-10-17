#define NOT_EXT
#include "Common.h"

//глобальные переменные
HMODULE libibmcogeay32 = NULL;
HMODULE log4cxx        = NULL;
HMODULE sslibmcogeay32 = NULL;
HMODULE TM1ULibDll     = NULL;
HMODULE tm1lib         = NULL;
HMODULE tm1api         = NULL;


BOOL initDLL() {
	BOOL isOk = 1;
	if(libibmcogeay32==NULL && NULL==(libibmcogeay32 = LoadLibrary(R"(C:\Users\etyurin\Documents\Visual Studio 2017\Projects\CognosTM1\CognosTM1\dllx64\libibmcogeay32.dll)")))
		isOk = 0;
	else if(log4cxx==NULL && NULL==(log4cxx = LoadLibrary(R"(C:\Users\etyurin\Documents\Visual Studio 2017\Projects\CognosTM1\CognosTM1\dllx64\log4cxx.dll)")))
		isOk = 0;
	else if (sslibmcogeay32 == NULL && NULL ==(sslibmcogeay32 = LoadLibrary(R"(C:\Users\etyurin\Documents\Visual Studio 2017\Projects\CognosTM1\CognosTM1\dllx64\sslibmcogeay32.dll)")))
		isOk = 0;
	else if (TM1ULibDll == NULL && NULL ==(TM1ULibDll = LoadLibrary(R"(C:\Users\etyurin\Documents\Visual Studio 2017\Projects\CognosTM1\CognosTM1\dllx64\TM1ULibDll.dll)")))
		isOk = 0;	
	else if(tm1lib == NULL && NULL == (tm1lib = LoadLibrary(R"(C:\Users\etyurin\Documents\Visual Studio 2017\Projects\CognosTM1\CognosTM1\dllx64\tm1lib.dll)")))
		isOk = 0;
	else if (tm1api == NULL && NULL == (tm1api = LoadLibrary(R"(C:\Users\etyurin\Documents\Visual Studio 2017\Projects\CognosTM1\CognosTM1\dllx64\tm1api.dll)")))
		isOk = 0;
	else {
		//PRE (.+?)\s\(WINAPI  \*(\w+)\)(\(\s.*\));
		//$2=($1 (WINAPI*)$3)GetProcAddress(tm1api,"$2");
		TM1APIInitialize = (void (WINAPI*)(void))GetProcAddress(tm1api, "TM1APIInitialize");
		TM1APIFinalize = (void (WINAPI*)(void))GetProcAddress(tm1api, "TM1APIFinalize");
		TM1BlobClose = (TM1V(WINAPI*)(TM1P hPool, TM1V hBlob))GetProcAddress(tm1api, "TM1BlobClose");
		TM1BlobCreate = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sName))GetProcAddress(tm1api, "TM1BlobCreate");
		TM1BlobGet = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V hBlob, TM1_INDEX x, TM1_INDEX n, CHAR * buf))GetProcAddress(tm1api, "TM1BlobGet");
		TM1BlobOpen = (TM1V(WINAPI*)(TM1P hPool, TM1V hBlob))GetProcAddress(tm1api, "TM1BlobOpen");
		TM1BlobOpenEx = (TM1V(WINAPI*)(TM1P hPool, TM1V hBlob, TM1V bOverwrite))GetProcAddress(tm1api, "TM1BlobOpenEx");
		TM1BlobPut = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V hBlob, TM1_INDEX x, TM1_INDEX n, CHAR * buf))GetProcAddress(tm1api, "TM1BlobPut");
		TM1CapabilityGetPolicy = (TM1V(WINAPI*)(TM1P hPool, TM1V hClient, TM1V vFeature, TM1V vPermission))GetProcAddress(tm1api, "TM1CapabilityGetPolicy");
		TM1CapabilityGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hGroup, TM1V vFeature, TM1V vPermission))GetProcAddress(tm1api, "TM1CapabilityGet");
		TM1CapabilityGetFeatures = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1CapabilityGetFeatures");
		TM1CapabilitySet = (TM1V(WINAPI*)(TM1P hPool, TM1V hGroup, TM1V vFeature, TM1V vPermission, TM1V vPolicy))GetProcAddress(tm1api, "TM1CapabilitySet");
		TM1CapabilityGetPermissions = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V vFeature))GetProcAddress(tm1api, "TM1CapabilityGetPermissions");
		TM1ChoreCreateEmpty = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ChoreCreateEmpty");
		TM1ChoreExecute = (TM1V(WINAPI*)(TM1P hPool, TM1V hChore))GetProcAddress(tm1api, "TM1ChoreExecute");
		TM1ClientAdd = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sClientName))GetProcAddress(tm1api, "TM1ClientAdd");
		TM1ClientGroupAssign = (TM1V(WINAPI*)(TM1P hPool, TM1V hClient, TM1V hGroup))GetProcAddress(tm1api, "TM1ClientGroupAssign");
		TM1ClientGroupIsAssigned = (TM1V(WINAPI*)(TM1P hPool, TM1V hClient, TM1V hGroup))GetProcAddress(tm1api, "TM1ClientGroupIsAssigned");
		TM1ClientGroupRemove = (TM1V(WINAPI*)(TM1P hPool, TM1V hClient, TM1V hGroup))GetProcAddress(tm1api, "TM1ClientGroupRemove");
		TM1ClientHasHolds = (TM1V(WINAPI*)(TM1P hPool, TM1V hClient))GetProcAddress(tm1api, "TM1ClientHasHolds");
		TM1ClientPasswordAssign = (TM1V(WINAPI*)(TM1P hPool, TM1V hClient, TM1V sPassword))GetProcAddress(tm1api, "TM1ClientPasswordAssign");
		TM1ConnectionCheck = (TM1V(WINAPI*)(TM1P hPool, TM1V hConnection))GetProcAddress(tm1api, "TM1ConnectionCheck");
		TM1ConnectionCreate = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sMasterServerName, TM1V sUsername, TM1V sPassword))GetProcAddress(tm1api, "TM1ConnectionCreate");
		TM1ConnectionCreateEx = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sMasterServerName, TM1V sUsername, TM1V sPassword, TM1V sNamespace, TM1V bUseIntegratedSecurity))GetProcAddress(tm1api, "TM1ConnectionCreateEx");
		TM1ConnectionDelete = (TM1V(WINAPI*)(TM1P hPool, TM1V hConnection))GetProcAddress(tm1api, "TM1ConnectionDelete");
		TM1ConnectionSynchronize = (TM1V(WINAPI*)(TM1P hPool, TM1V hConnection))GetProcAddress(tm1api, "TM1ConnectionSynchronize");
		TM1CubeCellDrillListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfKeys))GetProcAddress(tm1api, "TM1CubeCellDrillListGet");
		TM1CubeCellsDrillListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfArraysOfKeys))GetProcAddress(tm1api, "TM1CubeCellsDrillListGet");
		TM1CubeCellDrillObjectBuild = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfKeys, TM1V sDrillProcessName))GetProcAddress(tm1api, "TM1CubeCellDrillObjectBuild");
		TM1CubeCellValueGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfElements))GetProcAddress(tm1api, "TM1CubeCellValueGet");
		TM1CubeCellPickListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfElements))GetProcAddress(tm1api, "TM1CubeCellPickListGet");
		TM1CubeCellsPickListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfCells))GetProcAddress(tm1api, "TM1CubeCellsPickListGet");
		TM1CubeCellPickListExists = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfElements))GetProcAddress(tm1api, "TM1CubeCellPickListExists");
		TM1PickListCubeCreate = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube))GetProcAddress(tm1api, "TM1PickListCubeCreate");
		TM1PickListCubeExists = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube))GetProcAddress(tm1api, "TM1PickListCubeExists");
		TM1CubeCellValueSet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfElements, TM1V hValue))GetProcAddress(tm1api, "TM1CubeCellValueSet");
		TM1CubeCellSpread = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V vArrayOfCells, TM1V vCellReference, TM1V sSpreadData))GetProcAddress(tm1api, "TM1CubeCellSpread");
		TM1CubeCellSpreadStatusGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V hCells, TM1V hCellRange))GetProcAddress(tm1api, "TM1CubeCellSpreadStatusGet");
		TM1CubeCellSpreadViewArray = (TM1V(WINAPI*)(TM1P hPool, TM1V hView, TM1V aCellRange, TM1V aCellRef, TM1V sControl))GetProcAddress(tm1api, "TM1CubeCellSpreadViewArray");
		TM1CubeCreate = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V hArrayOfDimensions))GetProcAddress(tm1api, "TM1CubeCreate");
		TM1CubePerspectiveCreate = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfElementTitles))GetProcAddress(tm1api, "TM1CubePerspectiveCreate");
		TM1CubePerspectiveDestroy = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfElementTitles))GetProcAddress(tm1api, "TM1CubePerspectiveDestroy");
		TM1CubeShowsNulls = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube))GetProcAddress(tm1api, "TM1CubeShowsNulls");
		TM1CubeClearData = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube))GetProcAddress(tm1api, "TM1CubeClearData");
		TM1DimensionCheck = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension))GetProcAddress(tm1api, "TM1DimensionCheck");
		TM1DimensionCreateEmpty = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1DimensionCreateEmpty");
		TM1DimensionElementComponentAdd = (TM1V(WINAPI*)(TM1P hPool, TM1V hElement, TM1V hComponent, TM1V rWeight))GetProcAddress(tm1api, "TM1DimensionElementComponentAdd");
		TM1DimensionElementComponentDelete = (TM1V(WINAPI*)(TM1P hPool, TM1V hCElement, TM1V hElement))GetProcAddress(tm1api, "TM1DimensionElementComponentDelete");
		TM1DimensionElementComponentWeightGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCElement, TM1V hElement))GetProcAddress(tm1api, "TM1DimensionElementComponentWeightGet");
		TM1DimensionElementDelete = (TM1V(WINAPI*)(TM1P hPool, TM1V hElement))GetProcAddress(tm1api, "TM1DimensionElementDelete");
		TM1DimensionElementInsert = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension, TM1V hElementAfter, TM1V sName, TM1V vType))GetProcAddress(tm1api, "TM1DimensionElementInsert");
		TM1DimensionRootElementsGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension))GetProcAddress(tm1api, "TM1DimensionRootElementsGet");
		TM1DimensionRootSubsetGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension, TM1V iRight))GetProcAddress(tm1api, "TM1DimensionRootSubsetGet");
		TM1DimensionElementParentsSubsetGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hElement, TM1V iRight))GetProcAddress(tm1api, "TM1DimensionElementParentsSubsetGet");
		TM1DimensionElementChildrenSubsetGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hElement, TM1V iRight))GetProcAddress(tm1api, "TM1DimensionElementChildrenSubsetGet");
		TM1DimensionUpdate = (TM1V(WINAPI*)(TM1P hPool, TM1V hOldDimension, TM1V hNewDimension))GetProcAddress(tm1api, "TM1DimensionUpdate");
		TM1GroupAdd = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sGroupName))GetProcAddress(tm1api, "TM1GroupAdd");
		TM1IsTICubeDimensionOrderMatch = (TM1V(WINAPI*)(TM1P hPool, TM1V hProcess, TM1V hCubeName))GetProcAddress(tm1api, "TM1IsTICubeDimensionOrderMatch");
		TM1ObjectAttributeDelete = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hAttribute))GetProcAddress(tm1api, "TM1ObjectAttributeDelete");
		TM1ObjectAttributeInsert = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hAttributeBefore, TM1V sName, TM1V vType))GetProcAddress(tm1api, "TM1ObjectAttributeInsert");
		TM1ObjectAttributeValueGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hAttribute))GetProcAddress(tm1api, "TM1ObjectAttributeValueGet");
		TM1ObjectAttributeValueSet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V vValue))GetProcAddress(tm1api, "TM1ObjectAttributeValueSet");
		TM1ObjectAttributeLocaleValueGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V Locale))GetProcAddress(tm1api, "TM1ObjectAttributeLocaleValueGet");
		TM1ObjectAttributeLocaleValueSet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V Locale, TM1V vValue))GetProcAddress(tm1api, "TM1ObjectAttributeLocaleValueSet");
		TM1ObjectCopy = (TM1V(WINAPI*)(TM1P hPool, TM1V hSrcObject, TM1V hDstObject))GetProcAddress(tm1api, "TM1ObjectCopy");
		TM1ObjectDelete = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectDelete");
		TM1ObjectDestroy = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectDestroy");
		TM1ObjectDuplicate = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectDuplicate");
		TM1ObjectFileDelete = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectFileDelete");
		TM1ObjectFileLoad = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V hParent, TM1V iObjectType, TM1V sObjectName))GetProcAddress(tm1api, "TM1ObjectFileLoad");
		TM1ObjectFileSave = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectFileSave");
		TM1ObjectListCountGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V iPropertyList))GetProcAddress(tm1api, "TM1ObjectListCountGet");
		TM1ObjectListHandleByIndexGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V iIndex))GetProcAddress(tm1api, "TM1ObjectListHandleByIndexGet");
		TM1ObjectListHandleByNameGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V sName))GetProcAddress(tm1api, "TM1ObjectListHandleByNameGet");
		TM1ObjectPrivateDelete = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectPrivateDelete");
		TM1ObjectPrivateListCountGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V iPropertyList))GetProcAddress(tm1api, "TM1ObjectPrivateListCountGet");
		TM1ObjectPrivateListHandleByIndexGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V iIndex))GetProcAddress(tm1api, "TM1ObjectPrivateListHandleByIndexGet");
		TM1ObjectPrivateListHandleByNameGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V sName))GetProcAddress(tm1api, "TM1ObjectPrivateListHandleByNameGet");
		TM1ObjectPrivatePublish = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V sName))GetProcAddress(tm1api, "TM1ObjectPrivatePublish");
		TM1ObjectPrivateRegister = (TM1V(WINAPI*)(TM1P hPool, TM1V hParent, TM1V hObject, TM1V sName))GetProcAddress(tm1api, "TM1ObjectPrivateRegister");
		TM1ObjectPropertyGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V vProperty))GetProcAddress(tm1api, "TM1ObjectPropertyGet");
		TM1ObjectPropertySet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V vProperty, TM1V vValue))GetProcAddress(tm1api, "TM1ObjectPropertySet");
		TM1ObjectSubPropertyByRangeGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hRootObject, TM1V vObjectType, TM1V vPrivate, TM1V vStartIndex, TM1V vQuantity, TM1V vProperty))GetProcAddress(tm1api, "TM1ObjectSubPropertyByRangeGet");
		TM1ObjectRegister = (TM1V(WINAPI*)(TM1P hPool, TM1V hParent, TM1V hObject, TM1V sName))GetProcAddress(tm1api, "TM1ObjectRegister");
		TM1ObjectReplicate = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectReplicate");
		TM1ObjectSecurityLock = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectSecurityLock");
		TM1ObjectSecurityRelease = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectSecurityRelease");
		TM1ObjectSecurityReserve = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectSecurityReserve");
		TM1ObjectSecurityRightGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hGroup))GetProcAddress(tm1api, "TM1ObjectSecurityRightGet");
		TM1ObjectSecurityRightSet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hGroup, TM1V iRight))GetProcAddress(tm1api, "TM1ObjectSecurityRightSet");
		TM1ObjectSecurityUnLock = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject))GetProcAddress(tm1api, "TM1ObjectSecurityUnLock");
		TM1ProcessExecute = (TM1V(WINAPI*)(TM1P hPool, TM1V hProcess, TM1V hParametersArray))GetProcAddress(tm1api, "TM1ProcessExecute");
		TM1ProcessCheck = (TM1V(WINAPI*)(TM1P hPool, TM1V hProcess))GetProcAddress(tm1api, "TM1ProcessCheck");
		TM1ProcessCreateEmpty = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ProcessCreateEmpty");
		TM1ProcessExecuteEx = (TM1V(WINAPI*)(TM1P hPool, TM1V hProcess, TM1V hParametersArray))GetProcAddress(tm1api, "TM1ProcessExecuteEx");
		TM1ProcessExecuteSQLQuery = (TM1V(WINAPI*)(TM1P tPool, TM1V hProcess, TM1V voDatabaseInfoArray))GetProcAddress(tm1api, "TM1ProcessExecuteSQLQuery");
		TM1ProcessVariableNameIsValid = (TM1V(WINAPI*)(TM1P hPool, TM1V hProcess, TM1V hVariableName))GetProcAddress(tm1api, "TM1ProcessVariableNameIsValid");
		TM1RuleAttach = (TM1V(WINAPI*)(TM1P hPool, TM1V hRule))GetProcAddress(tm1api, "TM1RuleAttach");
		TM1RuleCheck = (TM1V(WINAPI*)(TM1P hPool, TM1V hRule))GetProcAddress(tm1api, "TM1RuleCheck");
		TM1RuleCreateEmpty = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hType))GetProcAddress(tm1api, "TM1RuleCreateEmpty");
		TM1RuleDetach = (TM1V(WINAPI*)(TM1P hPool, TM1V hRule))GetProcAddress(tm1api, "TM1RuleDetach");
		TM1RuleLineGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hRule, TM1V iPosition))GetProcAddress(tm1api, "TM1RuleLineGet");
		TM1RuleLineInsert = (TM1V(WINAPI*)(TM1P hPool, TM1V hRule, TM1V iPosition, TM1V sLine))GetProcAddress(tm1api, "TM1RuleLineInsert");
		TM1ServerBatchUpdateStart = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ServerBatchUpdateStart");
		TM1ServerBatchUpdateIsActive = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ServerBatchUpdateIsActive");
		TM1ServerBatchUpdateFinish = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V bDiscard))GetProcAddress(tm1api, "TM1ServerBatchUpdateFinish");
		TM1ServerLogClose = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ServerLogClose");
		TM1ServerLogNext = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ServerLogNext");
		TM1ServerLogOpen = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sStartTime, TM1V sCubeFilter, TM1V sUserFilter, TM1V sFlagFilter))GetProcAddress(tm1api, "TM1ServerLogOpen");
		TM1AuditLogQueryExecute = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sStartTime, TM1V sEndTime, TM1V aAdditionalFilters))GetProcAddress(tm1api, "TM1AuditLogQueryExecute");
		TM1AuditLogResultSetRangeGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V iStartIndex, TM1V iEndIndex))GetProcAddress(tm1api, "TM1AuditLogResultSetRangeGet");
		TM1AuditLogResultSetDestroy = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1AuditLogResultSetDestroy");
		TM1AuditLogResultSetDetailsGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V iEventID))GetProcAddress(tm1api, "TM1AuditLogResultSetDetailsGet");
		TM1AuditLogResultSetDetailsRangeGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V iEventID, TM1V iStartIndex, TM1V iEndIndex))GetProcAddress(tm1api, "TM1AuditLogResultSetDetailsRangeGet");
		TM1AuditLogResultSetDetailsDestroy = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V iEventID))GetProcAddress(tm1api, "TM1AuditLogResultSetDetailsDestroy");
		TM1AuditLogSystemEventsGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1AuditLogSystemEventsGet");
		TM1AuditLogObjectEventsGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V iObjectType))GetProcAddress(tm1api, "TM1AuditLogObjectEventsGet");
		TM1AuditLogRawstoreRoll = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1AuditLogRawstoreRoll");
		TM1ServerOpenSQLQuery = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V hDsnInfo))GetProcAddress(tm1api, "TM1ServerOpenSQLQuery");
		TM1ServerPasswordChange = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sNewPassword))GetProcAddress(tm1api, "TM1ServerPasswordChange");
		TM1ServerSecurityRefresh = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ServerSecurityRefresh");
		TM1SQLTableGetNextRows = (TM1V(WINAPI*)(TM1P hPool, TM1V hSQLTable, TM1V bColumnSelection))GetProcAddress(tm1api, "TM1SQLTableGetNextRows");
		TM1SubsetAll = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1SubsetAll");
		TM1SubsetRootElementsGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1SubsetRootElementsGet");
		TM1SubsetCreateEmpty = (TM1V(WINAPI*)(TM1P hPool, TM1V hDim))GetProcAddress(tm1api, "TM1SubsetCreateEmpty");
		TM1SubsetElementDisplay = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V iElement))GetProcAddress(tm1api, "TM1SubsetElementDisplay");
		TM1SubsetElementDisplayEll = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1SubsetElementDisplayEll");
		TM1SubsetElementDisplayLevel = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1SubsetElementDisplayLevel");
		TM1SubsetElementDisplayLine = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vString, TM1_INDEX Index))GetProcAddress(tm1api, "TM1SubsetElementDisplayLine");
		TM1SubsetElementDisplayMinus = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1SubsetElementDisplayMinus");
		TM1SubsetElementDisplayPlus = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1SubsetElementDisplayPlus");
		TM1SubsetElementDisplaySelection = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1SubsetElementDisplaySelection");
		TM1SubsetElementDisplayTee = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1SubsetElementDisplayTee");
		TM1SubsetElementDisplayWeight = (TM1_REAL(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1SubsetElementDisplayWeight");
		TM1SubsetInsertElement = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V hElement, TM1V iPosition))GetProcAddress(tm1api, "TM1SubsetInsertElement");
		TM1SubsetInsertSubset = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubsetA, TM1V hSubsetB, TM1V iPosition))GetProcAddress(tm1api, "TM1SubsetInsertSubset");
		TM1SubsetSelectByAttribute = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V hAlias, TM1V sValueToMatch, TM1V bSelection))GetProcAddress(tm1api, "TM1SubsetSelectByAttribute");
		TM1SubsetSelectByIndex = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V iPosition, TM1V bSelection))GetProcAddress(tm1api, "TM1SubsetSelectByIndex");
		TM1SubsetMultiSelectByIndex = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V iPositionArr, TM1V bSelectionArr))GetProcAddress(tm1api, "TM1SubsetMultiSelectByIndex");
		TM1SubsetSelectByLevel = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V iLevel, TM1V bSelection))GetProcAddress(tm1api, "TM1SubsetSelectByLevel");
		TM1SubsetSelectByPattern = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V sPattern, TM1V bSelection))GetProcAddress(tm1api, "TM1SubsetSelectByPattern");
		TM1SubsetSelectionDelete = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1SubsetSelectionDelete");
		TM1SubsetSelectionInsertChildren = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1SubsetSelectionInsertChildren");
		TM1SubsetSelectionInsertParents = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1SubsetSelectionInsertParents");
		TM1SubsetSelectionKeep = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1SubsetSelectionKeep");
		TM1SubsetSelectNone = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1SubsetSelectNone");
		TM1SubsetSort = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V bSortDown))GetProcAddress(tm1api, "TM1SubsetSort");
		TM1SubsetSortByHierarchy = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1SubsetSortByHierarchy");
		TM1SubsetUpdate = (TM1V(WINAPI*)(TM1P hPool, TM1V hOldSubset, TM1V hNewSubset))GetProcAddress(tm1api, "TM1SubsetUpdate");
		TM1SubsetCreateByExpression = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sExpression))GetProcAddress(tm1api, "TM1SubsetCreateByExpression");
		TM1SystemAdminHostGet = (CHAR * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemAdminHostGet");
		TM1SystemAdminHostGetW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemAdminHostGetW");
		TM1SystemAdminHostGetUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemAdminHostGetUTF8");
		TM1SystemAdminHostSet = (void (WINAPI*)(TM1U hUser, CHAR * szAdminHosts))GetProcAddress(tm1api, "TM1SystemAdminHostSet");
		TM1SystemAdminHostSetW = (void (WINAPI*)(TM1U hUser, TM1_UTF16_T * szAdminHosts))GetProcAddress(tm1api, "TM1SystemAdminHostSetW");
		TM1SystemAdminHostSetUTF8 = (void (WINAPI*)(TM1U hUser, TM1_UTF8_T * szAdminHosts))GetProcAddress(tm1api, "TM1SystemAdminHostSetUTF8");
		TM1APISetSSLCertVersion = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1_INDEX nCertificateVersion))GetProcAddress(tm1api, "TM1APISetSSLCertVersion");
		TM1SystemSetAdminSSLCertAuthority = (void (WINAPI*)(TM1U hUser, CHAR * szAdminSSLCertAuthority))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertAuthority");
		TM1SystemSetAdminSSLCertAuthorityW = (void (WINAPI*)(TM1U hUser, TM1_UTF16_T * szAdminSSLCertAuthority))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertAuthorityW");
		TM1SystemSetAdminSSLCertAuthorityUTF8 = (void (WINAPI*)(TM1U hUser, TM1_UTF8_T * szAdminSSLCertAuthority))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertAuthorityUTF8");
		TM1SystemGetAdminSSLCertAuthority = (CHAR * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertAuthority");
		TM1SystemGetAdminSSLCertAuthorityW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertAuthorityW");
		TM1SystemGetAdminSSLCertAuthorityUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertAuthorityUTF8");
		TM1ValidateSSLConfig = (int (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1ValidateSSLConfig");
		TM1SystemSetAdminSSLCertRevList = (void (WINAPI*)(TM1U hUser, CHAR * szAdminSSLCertRevList))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertRevList");
		TM1SystemSetAdminSSLCertRevListW = (void (WINAPI*)(TM1U hUser, TM1_UTF16_T * szAdminSSLCertRevList))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertRevListW");
		TM1SystemSetAdminSSLCertRevListUTF8 = (void (WINAPI*)(TM1U hUser, TM1_UTF8_T * szAdminSSLCertRevList))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertRevListUTF8");
		TM1SystemGetAdminSSLCertRevList = (CHAR * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertRevList");
		TM1SystemGetAdminSSLCertRevListW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertRevListW");
		TM1SystemGetAdminSSLCertRevListUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertRevListUTF8");
		TM1SystemSetAdminSSLCertID = (void (WINAPI*)(TM1U hUser, CHAR * szAdminSSLCertID))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertID");
		TM1SystemSetAdminSSLCertIDW = (void (WINAPI*)(TM1U hUser, TM1_UTF16_T * szAdminSSLCertID))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertIDW");
		TM1SystemSetAdminSSLCertIDUTF8 = (void (WINAPI*)(TM1U hUser, TM1_UTF8_T * szAdminSSLCertID))GetProcAddress(tm1api, "TM1SystemSetAdminSSLCertIDUTF8");
		TM1SystemGetAdminSSLCertID = (CHAR * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertID");
		TM1SystemGetAdminSSLCertIDW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertIDW");
		TM1SystemGetAdminSSLCertIDUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetAdminSSLCertIDUTF8");
		TM1SystemSetExportAdminSvrSSLCertFlag = (void (WINAPI*)(TM1U hUser, TM1_BOOL bExportAdminSvrSSLCert))GetProcAddress(tm1api, "TM1SystemSetExportAdminSvrSSLCertFlag");
		TM1SystemSetAdminSvrExportKeyID = (void (WINAPI*)(TM1U hUser, CHAR * szAdminSvrExportKeyID))GetProcAddress(tm1api, "TM1SystemSetAdminSvrExportKeyID");
		TM1SystemSetAdminSvrExportKeyIDW = (void (WINAPI*)(TM1U hUser, TM1_UTF16_T * szAdminSvrExportKeyID))GetProcAddress(tm1api, "TM1SystemSetAdminSvrExportKeyIDW");
		TM1SystemSetAdminSvrExportKeyIDUTF8 = (void (WINAPI*)(TM1U hUser, TM1_UTF8_T * szAdminSvrExportKeyID))GetProcAddress(tm1api, "TM1SystemSetAdminSvrExportKeyIDUTF8");
		TM1SystemSetSSLDirectory = (void (WINAPI*)(TM1U hUser, CHAR * szSSLCertsDirectory))GetProcAddress(tm1api, "TM1SystemSetSSLDirectory");
		TM1SystemSetSSLDirectoryW = (void (WINAPI*)(TM1U hUser, TM1_UTF16_T * szSSLCertsDirectory))GetProcAddress(tm1api, "TM1SystemSetSSLDirectoryW");
		TM1SystemSetSSLDirectoryUTF8 = (void (WINAPI*)(TM1U hUser, TM1_UTF8_T * szSSLCertsDirectory))GetProcAddress(tm1api, "TM1SystemSetSSLDirectoryUTF8");
		TM1SystemGetSSLDirectory = (CHAR * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetSSLDirectory");
		TM1SystemGetSSLDirectoryW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetSSLDirectoryW");
		TM1SystemGetSSLDirectoryUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemGetSSLDirectoryUTF8");
		TM1SystemBuildNumber = (CHAR * (WINAPI*)(void))GetProcAddress(tm1api, "TM1SystemBuildNumber");
		TM1SystemBuildNumberW = (TM1_UTF16_T * (WINAPI*)(void))GetProcAddress(tm1api, "TM1SystemBuildNumberW");
		TM1SystemBuildNumberUTF8 = (TM1_UTF8_T * (WINAPI*)(void))GetProcAddress(tm1api, "TM1SystemBuildNumberUTF8");
		TM1SystemClose = (void (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemClose");
		TM1SystemOpen = (TM1U(WINAPI*)(void))GetProcAddress(tm1api, "TM1SystemOpen");
		TM1SystemProgressHookSet = (void (WINAPI*)(TM1U hUser, TM1_HOOK pHook))GetProcAddress(tm1api, "TM1SystemProgressHookSet");
		TM1SystemProgressHookSetW = (void (WINAPI*)(TM1U hUser, TM1_HOOK_W pHook))GetProcAddress(tm1api, "TM1SystemProgressHookSetW");
		TM1SystemProgressHookSetUTF8 = (void (WINAPI*)(TM1U hUser, TM1_HOOK_UTF8 pHook))GetProcAddress(tm1api, "TM1SystemProgressHookSetUTF8");
		TM1IsProgressHookActive = (TM1_BOOL(WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1IsProgressHookActive");
		TM1SystemServerClientName = (CHAR * (WINAPI*)(TM1U hUser, unsigned index))GetProcAddress(tm1api, "TM1SystemServerClientName");
		TM1SystemServerClientNameW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser, unsigned index))GetProcAddress(tm1api, "TM1SystemServerClientNameW");
		TM1SystemServerClientNameUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser, unsigned index))GetProcAddress(tm1api, "TM1SystemServerClientNameUTF8");
		TM1SystemServerConnect = (TM1V(WINAPI*)(TM1P hPool, TM1V sServerName, TM1V sClientName, TM1V sPassword))GetProcAddress(tm1api, "TM1SystemServerConnect");
		TM1SystemServerLimitedConnect = (TM1V(WINAPI*)(TM1P hPool, TM1V sServerName))GetProcAddress(tm1api, "TM1SystemServerLimitedConnect");
		TM1SystemServerConnectIntegratedLogin = (TM1V(WINAPI*)(TM1P hPool, TM1V sServerName))GetProcAddress(tm1api, "TM1SystemServerConnectIntegratedLogin");
		TM1SystemServerDisconnect = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1SystemServerDisconnect");
		TM1SystemServerHandle = (TM1V(WINAPI*)(TM1U hUser, CHAR * szName))GetProcAddress(tm1api, "TM1SystemServerHandle");
		TM1SystemServerHandleW = (TM1V(WINAPI*)(TM1U hUser, TM1_UTF16_T * szName))GetProcAddress(tm1api, "TM1SystemServerHandleW");
		TM1SystemServerHandleUTF8 = (TM1V(WINAPI*)(TM1U hUser, TM1_UTF8_T * szName))GetProcAddress(tm1api, "TM1SystemServerHandleUTF8");
		TM1SystemServerConnectWithCAMPassport = (TM1V(WINAPI*)(TM1P hPool, TM1V sServerName, TM1V camArgs))GetProcAddress(tm1api, "TM1SystemServerConnectWithCAMPassport");
		TM1SystemServerConnectWithCAMNamespace = (TM1V(WINAPI*)(TM1P hPool, TM1V sServerName, TM1V camArgs))GetProcAddress(tm1api, "TM1SystemServerConnectWithCAMNamespace");
		TM1SystemGetServerConfig = (TM1V(WINAPI*)(TM1P hPool, TM1V sServerName))GetProcAddress(tm1api, "TM1SystemGetServerConfig");
		TM1SystemServerName = (CHAR * (WINAPI*)(TM1U hUser, unsigned index))GetProcAddress(tm1api, "TM1SystemServerName");
		TM1SystemServerNameW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser, unsigned index))GetProcAddress(tm1api, "TM1SystemServerNameW");
		TM1SystemServerNameUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser, unsigned index))GetProcAddress(tm1api, "TM1SystemServerNameUTF8");
		TM1SystemServerNof = (int (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemServerNof");
		TM1SystemServerReload = (void (WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1SystemServerReload");
		TM1SystemServerStart = (TM1_BOOL(WINAPI*)(TM1U hUser, CHAR * szName, CHAR * szDataDirectory, CHAR * szAdminHost, CHAR * szProtocol, int iPortNumber))GetProcAddress(tm1api, "TM1SystemServerStart");
		TM1SystemServerStartW = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1_UTF16_T * szName, TM1_UTF16_T * szDataDirectory, TM1_UTF16_T * szAdminHost, TM1_UTF16_T * szProtocol, int iPortNumber))GetProcAddress(tm1api, "TM1SystemServerStartW");
		TM1SystemServerStartUTF8 = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1_UTF8_T * szName, TM1_UTF8_T * szDataDirectory, TM1_UTF8_T * szAdminHost, TM1_UTF8_T * szProtocol, int iPortNumber))GetProcAddress(tm1api, "TM1SystemServerStartUTF8");
		TM1SystemServerStartEx = (TM1_BOOL(WINAPI*)(TM1U hUser, CHAR * szcmdline))GetProcAddress(tm1api, "TM1SystemServerStartEx");
		TM1SystemServerStartExW = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1_UTF16_T * szcmdline))GetProcAddress(tm1api, "TM1SystemServerStartExW");
		TM1SystemServerStartExUTF8 = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1_UTF8_T * szcmdline))GetProcAddress(tm1api, "TM1SystemServerStartExUTF8");
		TM1SystemServerStop = (TM1_BOOL(WINAPI*)(TM1U hUser, CHAR * szName, TM1_BOOL bSave))GetProcAddress(tm1api, "TM1SystemServerStop");
		TM1SystemServerStopW = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1_UTF16_T * szName, TM1_BOOL bSave))GetProcAddress(tm1api, "TM1SystemServerStopW");
		TM1SystemServerStopUTF8 = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1_UTF8_T * szName, TM1_BOOL bSave))GetProcAddress(tm1api, "TM1SystemServerStopUTF8");
		TM1SystemVersionGet = (int (WINAPI*)(void))GetProcAddress(tm1api, "TM1SystemVersionGet");
		TM1ValArray = (TM1V(WINAPI*)(TM1P hPool, TM1V * InitArray, TM1_INDEX MaxSize))GetProcAddress(tm1api, "TM1ValArray");
		TM1ValArrayGet = (TM1V(WINAPI*)(TM1U hUser, TM1V vArray, TM1_INDEX Index))GetProcAddress(tm1api, "TM1ValArrayGet");
		TM1ValArrayMaxSize = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V vArray))GetProcAddress(tm1api, "TM1ValArrayMaxSize");
		TM1ValArraySet = (void (WINAPI*)(TM1V vArray, TM1V vValue, TM1_INDEX Index))GetProcAddress(tm1api, "TM1ValArraySet");
		TM1ValArraySetSize = (void (WINAPI*)(TM1V vArray, TM1_INDEX Size))GetProcAddress(tm1api, "TM1ValArraySetSize");
		TM1ValBool = (TM1V(WINAPI*)(TM1P hPool, TM1_BOOL InitBool))GetProcAddress(tm1api, "TM1ValBool");
		TM1ValBoolGet = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vBool))GetProcAddress(tm1api, "TM1ValBoolGet");
		TM1ValBoolSet = (void (WINAPI*)(TM1V vBool, TM1_BOOL Bool))GetProcAddress(tm1api, "TM1ValBoolSet");
		TM1ValErrorCode = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V vError))GetProcAddress(tm1api, "TM1ValErrorCode");
		TM1ValErrorString = (CHAR * (WINAPI*)(TM1U hUser, TM1V vValue))GetProcAddress(tm1api, "TM1ValErrorString");
		TM1ValErrorStringW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser, TM1V vValue))GetProcAddress(tm1api, "TM1ValErrorStringW");
		TM1ValErrorStringUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser, TM1V vValue))GetProcAddress(tm1api, "TM1ValErrorStringUTF8");
		TM1ValIndex = (TM1V(WINAPI*)(TM1P hPool, TM1_INDEX InitIndex))GetProcAddress(tm1api, "TM1ValIndex");
		TM1ValIndexGet = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V vIndex))GetProcAddress(tm1api, "TM1ValIndexGet");
		TM1ValIndexSet = (void (WINAPI*)(TM1V vIndex, TM1_INDEX Index))GetProcAddress(tm1api, "TM1ValIndexSet");
		TM1ValIsUndefined = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V Value))GetProcAddress(tm1api, "TM1ValIsUndefined");
		TM1ValIsUpdatable = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V Value))GetProcAddress(tm1api, "TM1ValIsUpdatable");
		TM1ValIsChanged = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V Value))GetProcAddress(tm1api, "TM1ValIsChanged");
		TM1ValObject = (TM1V(WINAPI*)(TM1P hPool, TM1_OBJECT * InitObject))GetProcAddress(tm1api, "TM1ValObject");
		TM1ValObjectCanRead = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vObject))GetProcAddress(tm1api, "TM1ValObjectCanRead");
		TM1ValObjectCanWrite = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V vObject))GetProcAddress(tm1api, "TM1ValObjectCanWrite");
		TM1ValObjectGet = (void (WINAPI*)(TM1U hUser, TM1V vObject, TM1_OBJECT * pObject))GetProcAddress(tm1api, "TM1ValObjectGet");
		TM1ValObjectSet = (void (WINAPI*)(TM1V vObject, TM1_OBJECT * pObject))GetProcAddress(tm1api, "TM1ValObjectSet");
		TM1ValObjectType = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V vObject))GetProcAddress(tm1api, "TM1ValObjectType");
		TM1ValPoolCount = (TM1_INDEX(WINAPI*)(TM1P hPool))GetProcAddress(tm1api, "TM1ValPoolCount");
		TM1ValPoolCreate = (TM1P(WINAPI*)(TM1U hUser))GetProcAddress(tm1api, "TM1ValPoolCreate");
		TM1ValPoolDestroy = (void (WINAPI*)(TM1P hPool))GetProcAddress(tm1api, "TM1ValPoolDestroy");
		TM1ValPoolGet = (TM1V(WINAPI*)(TM1P hPool, TM1_INDEX Index))GetProcAddress(tm1api, "TM1ValPoolGet");
		TM1ValPoolMemory = (unsigned long (WINAPI*)(TM1P hPool))GetProcAddress(tm1api, "TM1ValPoolMemory");
		TM1ValReal = (TM1V(WINAPI*)(TM1P hPool, TM1_REAL InitReal))GetProcAddress(tm1api, "TM1ValReal");
		TM1ValRealGet = (TM1_REAL(WINAPI*)(TM1U hUser, TM1V vReal))GetProcAddress(tm1api, "TM1ValRealGet");
		TM1ValRealSet = (void (WINAPI*)(TM1V vReal, TM1_REAL Real))GetProcAddress(tm1api, "TM1ValRealSet");
		TM1IsStringType = (int (WINAPI*)(int type))GetProcAddress(tm1api, "TM1IsStringType");
		TM1ValString = (TM1V(WINAPI*)(TM1P hPool, CHAR * InitString, TM1_INDEX MaxSize))GetProcAddress(tm1api, "TM1ValString");
		TM1ValStringW = (TM1V(WINAPI*)(TM1P hPool, TM1_UTF16_T * InitString, TM1_INDEX MaxSize))GetProcAddress(tm1api, "TM1ValStringW");
		TM1ValStringUTF8 = (TM1V(WINAPI*)(TM1P hPool, TM1_UTF8_T * InitString, TM1_INDEX MaxSize))GetProcAddress(tm1api, "TM1ValStringUTF8");
		TM1ValStringEncrypt = (TM1V(WINAPI*)(TM1P hPool, CHAR * InitString, TM1_INDEX MaxSize))GetProcAddress(tm1api, "TM1ValStringEncrypt");
		TM1ValStringEncryptW = (TM1V(WINAPI*)(TM1P hPool, TM1_UTF16_T * InitString, TM1_INDEX MaxSize))GetProcAddress(tm1api, "TM1ValStringEncryptW");
		TM1ValStringEncryptUTF8 = (TM1V(WINAPI*)(TM1P hPool, TM1_UTF8_T * InitString, TM1_INDEX MaxSize))GetProcAddress(tm1api, "TM1ValStringEncryptUTF8");
		TM1ValStringGet = (CHAR * (WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1ValStringGet");
		TM1ValStringGetW = (TM1_UTF16_T * (WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1ValStringGetW");
		TM1ValStringGetUTF8 = (TM1_UTF8_T * (WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1ValStringGetUTF8");
		TM1ValStringAsBytesW = (TM1V(WINAPI*)(TM1P hPool, TM1_UTF16_T *InitString))GetProcAddress(tm1api, "TM1ValStringAsBytesW");
		TM1ValStringAsBytesUTF8 = (TM1V(WINAPI*)(TM1P hPool, TM1_UTF8_T *InitString))GetProcAddress(tm1api, "TM1ValStringAsBytesUTF8");
		TM1ValBytesGet = (TM1_BYTE_T * (WINAPI*)(TM1U hUser, TM1V vBytes, TM1_INDEX *dwSize))GetProcAddress(tm1api, "TM1ValBytesGet");
		TM1ValStringMaxSize = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1ValStringMaxSize");
		TM1ValStringWMaxSize = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1ValStringWMaxSize");
		TM1ValStringUTF8MaxSize = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V vString))GetProcAddress(tm1api, "TM1ValStringUTF8MaxSize");
		TM1ValTypeIsString = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V Value))GetProcAddress(tm1api, "TM1ValTypeIsString");
		TM1ValTypeIsBinary = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V Value))GetProcAddress(tm1api, "TM1ValTypeIsBinary");
		TM1ValStringSet = (void (WINAPI*)(TM1V vString, char *String))GetProcAddress(tm1api, "TM1ValStringSet");
		TM1ValStringSetW = (void (WINAPI*)(TM1V vString, TM1_UTF16_T *utf16String))GetProcAddress(tm1api, "TM1ValStringSetW");
		TM1ValStringSetUTF8 = (void (WINAPI*)(TM1V vString, TM1_UTF8_T *utf16String))GetProcAddress(tm1api, "TM1ValStringSetUTF8");
		TM1ValType = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V Value))GetProcAddress(tm1api, "TM1ValType");
		TM1ValTypeEx = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V Value))GetProcAddress(tm1api, "TM1ValTypeEx");
		#ifdef GEN_WIN
			TM1ValTypeWithMsgLoop = (TM1_INDEX(WINAPI*)(TM1U hUser, TM1V Value, HWND cancel_Hwnd))GetProcAddress(tm1api, "TM1ValTypeWithMsgLoop");
		#endif
		TM1ViewArrayColumnsNof = (TM1V(WINAPI*)(TM1P hPool, TM1V hView))GetProcAddress(tm1api, "TM1ViewArrayColumnsNof");
		TM1ViewArrayConstruct = (TM1V(WINAPI*)(TM1P hPool, TM1V hView))GetProcAddress(tm1api, "TM1ViewArrayConstruct");
		TM1ViewArrayDestroy = (TM1V(WINAPI*)(TM1P hPool, TM1V hView))GetProcAddress(tm1api, "TM1ViewArrayDestroy");
		TM1ViewArrayRowsNof = (TM1V(WINAPI*)(TM1P hPool, TM1V hView))GetProcAddress(tm1api, "TM1ViewArrayRowsNof");
		TM1ViewArrayValueGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow))GetProcAddress(tm1api, "TM1ViewArrayValueGet");
		TM1ViewArrayValuePickListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow))GetProcAddress(tm1api, "TM1ViewArrayValuePickListGet");
		TM1ViewArrayValuePickListByRangeGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hView, TM1V iRowStart, TM1V iColStart, TM1V iRowEnd, TM1V iColEnd))GetProcAddress(tm1api, "TM1ViewArrayValuePickListByRangeGet");
		TM1ViewArrayValuePickListExists = (TM1V(WINAPI*)(TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow))GetProcAddress(tm1api, "TM1ViewArrayValuePickListExists");
		TM1ViewCreate = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hTitleSubsetArray, TM1V hColumnSubsetArray, TM1V hRowSubsetArray))GetProcAddress(tm1api, "TM1ViewCreate");
		TM1ViewExtractCreate = (TM1V(WINAPI*)(TM1P hPool, TM1V hView))GetProcAddress(tm1api, "TM1ViewExtractCreate");
		TM1ViewExtractDestroy = (TM1V(WINAPI*)(TM1P hPool, TM1V hView))GetProcAddress(tm1api, "TM1ViewExtractDestroy");
		TM1ViewExtractGetNext = (TM1V(WINAPI*)(TM1P hPool, TM1V hView))GetProcAddress(tm1api, "TM1ViewExtractGetNext");
		TM1CancelClientJob = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V hServer))GetProcAddress(tm1api, "TM1CancelClientJob");
		TM1UserKill = (TM1_BOOL(WINAPI*)(TM1U hUser, TM1V hServer))GetProcAddress(tm1api, "TM1UserKill");
		TM1GetViewByName = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sCube, TM1V sViewName, TM1V bIsPrivate, TM1V iFlag))GetProcAddress(tm1api, "TM1GetViewByName");
		TM1GetViewByHandle = (TM1V(WINAPI*)(TM1P hPool, TM1V hView, TM1V iFlag))GetProcAddress(tm1api, "TM1GetViewByHandle");
		TM1ViewArrayValueByRangeGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hView, TM1V iRowStart, TM1V iColStart, TM1V iRowEnd, TM1V iColEnd))GetProcAddress(tm1api, "TM1ViewArrayValueByRangeGet");
		TM1CubeListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V iFlag))GetProcAddress(tm1api, "TM1CubeListGet");
		TM1CubeListByNamesGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V vCubeNames))GetProcAddress(tm1api, "TM1CubeListByNamesGet");
		TM1ViewListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V iFlag))GetProcAddress(tm1api, "TM1ViewListGet");
		TM1ViewListByNamesGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V vViewNames, TM1V iFlag))GetProcAddress(tm1api, "TM1ViewListByNamesGet");
		TM1ServerDimensionListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V iFlag))GetProcAddress(tm1api, "TM1ServerDimensionListGet");
		TM1ServerDimensionListByNamesGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V vDimensionNames))GetProcAddress(tm1api, "TM1ServerDimensionListByNamesGet");
		TM1CubeDimensionListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube))GetProcAddress(tm1api, "TM1CubeDimensionListGet");
		TM1SubsetListGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension, TM1V iFlag))GetProcAddress(tm1api, "TM1SubsetListGet");
		TM1SubsetListByNamesGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension, TM1V vSubsetNames, TM1V iFlag))GetProcAddress(tm1api, "TM1SubsetListByNamesGet");
		TM1DimensionElementListByNamesGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension, TM1V vElementNames, TM1V iFlag))GetProcAddress(tm1api, "TM1DimensionElementListByNamesGet");
		TM1DimensionElementListByIndexGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension, TM1V iBeginIndex, TM1V iCount, TM1V iFlag))GetProcAddress(tm1api, "TM1DimensionElementListByIndexGet");
		TM1SubsetElementListByIndexGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V iBeginIndex, TM1V iCount))GetProcAddress(tm1api, "TM1SubsetElementListByIndexGet");
		TM1SubsetElementListByIndexGetEx = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V iBeginIndex, TM1V iCount, TM1V vDimName))GetProcAddress(tm1api, "TM1SubsetElementListByIndexGetEx");
		TM1SubsetElementListByNamesGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset, TM1V vElementNames))GetProcAddress(tm1api, "TM1SubsetElementListByNamesGet");
		TM1ViewCellsValueGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hView, TM1V vArrayOfCells))GetProcAddress(tm1api, "TM1ViewCellsValueGet");
		TM1CubeCellsValueGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V vArrayOfCells))GetProcAddress(tm1api, "TM1CubeCellsValueGet");
		TM1CubeCellsValueSet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V vArrayOfCells, TM1V vValues))GetProcAddress(tm1api, "TM1CubeCellsValueSet");
		TM1GetSubsetByHandle = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubset))GetProcAddress(tm1api, "TM1GetSubsetByHandle");
		TM1ElementComponentsGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hElement, TM1V vAliasName, TM1V iStartIndex, TM1V iCount))GetProcAddress(tm1api, "TM1ElementComponentsGet");
		TM1ElementComponentsGetEx = (TM1V(WINAPI*)(TM1P hPool, TM1V hElement, TM1V vAliasName, TM1V iStartIndex, TM1V iCount, TM1V vDimName))GetProcAddress(tm1api, "TM1ElementComponentsGetEx");
		TM1DimensionAttributesGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hDimension))GetProcAddress(tm1api, "TM1DimensionAttributesGet");
		#ifdef GEN_WIN
			TM1ValStringGet_CSH = (void (WINAPI*)(TM1U hUser, TM1V vString, TCHAR ** string))GetProcAddress(tm1api, "TM1ValStringGet_CSH");
		#endif
		TM1ObjectAttributeValuesSet = (TM1V(WINAPI*)(TM1P hPool, TM1V hAttribute, TM1V hObjects, TM1V vValues))GetProcAddress(tm1api, "TM1ObjectAttributeValuesSet");
		TM1ValTypeReal = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeReal");
		TM1ValTypeString = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeString");
		TM1ValTypeStringW = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeStringW");
		TM1ValTypeIndex = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeIndex");
		TM1ValTypeBool = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeBool");
		TM1ValTypeObject = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeObject");
		TM1ValTypeError = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeError");
		TM1ValTypeArray = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeArray");
		TM1ValTypeBinary = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ValTypeBinary");
		TM1ArrayNull = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ArrayNull");
		TM1ObjectNull = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectNull");
		TM1CubeCellValueUndefined = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCellValueUndefined");
		TM1Entry = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1Entry");
		TM1TypeAttribute = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeAttribute");
		TM1TypeBlob = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeBlob");
		TM1TypeClient = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeClient");
		TM1TypeChore = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeChore");
		TM1TypeConnection = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeConnection");
		TM1TypeCube = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeCube");
		TM1TypeDimension = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeDimension");
		TM1TypeElement = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeElement");
		TM1TypeGroup = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeGroup");
		TM1TypeProcess = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeProcess");
		TM1TypeRule = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeRule");
		TM1TypeSubset = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeSubset");
		TM1TypeServer = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeServer");
		TM1TypeSQLNotSupported = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1TypeSQLNotSupported");
		TM1TypeSQLNumericColumn = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1TypeSQLNumericColumn");
		TM1TypeSQLStringColumn = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1TypeSQLStringColumn");
		TM1TypeSQLTable = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1TypeSQLTable");
		TM1TypeView = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeView");
		TM1TypeElementSimple = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeElementSimple");
		TM1TypeElementConsolidated = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeElementConsolidated");
		TM1TypeElementString = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeElementString");
		TM1TypeAttributeAlias = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeAttributeAlias");
		TM1TypeAttributeString = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeAttributeString");
		TM1TypeAttributeNumeric = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeAttributeNumeric");
		TM1TypeRuleCalculation = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeRuleCalculation");
		TM1TypeRuleDrill = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1TypeRuleDrill");
		TM1ObjectAttributes = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectAttributes");
		TM1ObjectChangedSinceLoaded = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectChangedSinceLoaded");
		TM1ObjectLastTimeUpdated = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectLastTimeUpdated");
		TM1ObjectInstanceVersion = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectInstanceVersion");
		TM1ObjectMemoryUsed = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectMemoryUsed");
		TM1ObjectName = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectName");
		TM1ObjectParent = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectParent");
		TM1ObjectSecurityStatus = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectSecurityStatus");
		TM1ObjectSecurityOwner = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectSecurityOwner");
		TM1ObjectType = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectType");
		TM1ObjectRegistration = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectRegistration");
		TM1AttributeType = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1AttributeType");
		TM1ClientPassword = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ClientPassword");
		TM1ChoreActive = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ChoreActive");
		TM1ChoreFrequency = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ChoreFrequency");
		TM1ChoreStartTime = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ChoreStartTime");
		TM1ChoreSteps = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ChoreSteps");
		TM1ChoreExecutionMode = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ChoreExecutionMode");
		TM1ClientStatus = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ClientStatus");
		TM1ServerBuildNumber = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerBuildNumber");
		TM1ServerClients = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerClients");
		TM1ServerDirectories = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerDirectories");
		TM1ServerDimensions = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerDimensions");
		TM1ServerGroups = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerGroups");
		TM1ServerCubes = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerCubes");
		TM1ServerBlobs = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerBlobs");
		TM1ServerLogDirectory = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerLogDirectory");
		TM1ServerNetworkAddress = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerNetworkAddress");
		TM1ServerChores = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerChores");
		TM1ServerProcesses = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerProcesses");
		TM1ServerConnections = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerConnections");
		TM1ServerProcessObjectsSupported = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerProcessObjectsSupported");
		TM1ServerProgressMessageOn = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerProgressMessageOn");
		TM1ServerParallelInteraction = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerParallelInteraction");
		TM1ServerCalculationCachePartitioning = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerCalculationCachePartitioning");
		TM1ServerSession = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ServerSession");
		TM1BlobSize = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1BlobSize");
		TM1ConnectionChoresUsing = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionChoresUsing");
		TM1ConnectionLastSyncTime = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionLastSyncTime");
		TM1ConnectionLastSyncTimeStar = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionLastSyncTimeStar");
		TM1ConnectionLastSyncStarRecord = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionLastSyncStarRecord");
		TM1ConnectionSyncErrorCount = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionSyncErrorCount");
		TM1ConnectionSyncPlanetToStar = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionSyncPlanetToStar");
		TM1ConnectionSyncStarToPlanet = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionSyncStarToPlanet");
		TM1ConnectionUserName = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionUserName");
		TM1ConnectionUseIntegratedSecurity = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionUseIntegratedSecurity");
		TM1ConnectionNamespace = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ConnectionNamespace");
		TM1DimensionElements = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1DimensionElements");
		TM1DimensionNofLevels = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1DimensionNofLevels");
		TM1DimensionSubsets = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1DimensionSubsets");
		TM1DimensionCubesUsing = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1DimensionCubesUsing");
		TM1DimensionWidth = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1DimensionWidth");
		TM1DimensionReplicationSyncSubsets = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1DimensionReplicationSyncSubsets");
		TM1ElementIndex = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ElementIndex");
		TM1ElementLevel = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ElementLevel");
		TM1ElementType = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ElementType");
		TM1ElementComponents = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ElementComponents");
		TM1ElementParents = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ElementParents");
		TM1RuleNofLines = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1RuleNofLines");
		TM1RuleErrorLine = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1RuleErrorLine");
		TM1RuleErrorString = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1RuleErrorString");
		TM1SubsetElements = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SubsetElements");
		TM1SubsetSourceName = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SubsetSourceName");
		TM1SubsetAlias = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SubsetAlias");
		TM1SubsetExpression = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SubsetExpression");
		TM1CubeDimensions = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeDimensions");
		TM1CubeLogChanges = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeLogChanges");
		TM1CubeRule = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeRule");
		TM1CubeViews = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeViews");
		TM1CubePerspectivesMaxMemory = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubePerspectivesMaxMemory");
		TM1CubePerspectivesMinTime = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubePerspectivesMinTime");
		TM1CubeMeasuresDimension = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeMeasuresDimension");
		TM1CubeReplicationSyncRule = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeReplicationSyncRule");
		TM1CubeReplicationSyncViews = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeReplicationSyncViews");
		TM1CubeTimeDimension = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeTimeDimension");
		TM1ViewColumnSubsets = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewColumnSubsets");
		TM1CubeCellSpreadFunctionOk = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCellSpreadFunctionOk");
		TM1CubeCellSpreadNumericSetOk = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCellSpreadNumericSetOk");
		TM1CubeCellSpreadStringSetOk = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCellSpreadStringSetOk");
		TM1CubeCellSpreadStatusHeld = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCellSpreadStatusHeld");
		TM1CubeCellSpreadStatusHeldConsolidation = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCellSpreadStatusHeldConsolidation");
		TM1CubeCellSpreadStatusWritable = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCellSpreadStatusWritable");
		TM1CubeTimeLastInvalidated = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeTimeLastInvalidated");
		TM1CubeDataReservationMode = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeDataReservationMode");
		TM1CubeCellSecurityDefaultValue = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCellSecurityDefaultValue");
		TM1CubeCalculationThreshold = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeCalculationThreshold");
		TM1ViewPreConstruct = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewPreConstruct");
		TM1ViewRowSubsets = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewRowSubsets");
		TM1ViewSuppressZeroes = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewSuppressZeroes");
		TM1ViewTitleElements = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewTitleElements");
		TM1ViewTitleSubsets = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewTitleSubsets");
		TM1ViewFormat = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewFormat");
		TM1ViewShowAutomatically = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewShowAutomatically");
		TM1ViewArrayCellOrdinal = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewArrayCellOrdinal");
		TM1ViewArrayCellValue = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewArrayCellValue");
		TM1ViewArrayCellFormattedValue = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewArrayCellFormattedValue");
		TM1ViewArrayCellFormatString = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewArrayCellFormatString");
		TM1ViewArrayMemberName = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewArrayMemberName");
		TM1ViewArrayMemberType = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewArrayMemberType");
		TM1ViewArrayMemberDescription = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewArrayMemberDescription");
		TM1SecurityRightNone = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SecurityRightNone");
		TM1SecurityRightRead = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SecurityRightRead");
		TM1SecurityRightWrite = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SecurityRightWrite");
		TM1SecurityRightReserve = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SecurityRightReserve");
		TM1SecurityRightLock = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SecurityRightLock");
		TM1SecurityRightAdmin = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SecurityRightAdmin");
		TM1ObjectPublic = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectPublic");
		TM1ObjectPrivate = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectPrivate");
		TM1ObjectUnregistered = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectUnregistered");
		TM1ObjectReplicationConnection = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectReplicationConnection");
		TM1ObjectLastRepSyncChangedTime = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectLastRepSyncChangedTime");
		TM1ObjectReplicationSourceObjectName = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectReplicationSourceObjectName");
		TM1ObjectReplicationStatus = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectReplicationStatus");
		TM1ProcessChoresUsing = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessChoresUsing");
		TM1ProcessComplete = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessComplete");
		TM1ProcessDataProcedure = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataProcedure");
		TM1ProcessDataSourceASCIIDecimalSeparator = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceASCIIDecimalSeparator");
		TM1ProcessDataSourceASCIIDelimiter = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceASCIIDelimiter");
		TM1ProcessDataSourceASCIIHeaderRecords = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceASCIIHeaderRecords");
		TM1ProcessDataSourceASCIIQuoteCharacter = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceASCIIQuoteCharacter");
		TM1ProcessDataSourceASCIIThousandSeparator = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceASCIIThousandSeparator");
		TM1ProcessDataSourceCubeView = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceCubeView");
		TM1ProcessDataSourceDimensionSubset = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceDimensionSubset");
		TM1ProcessDataSourceNameForClient = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceNameForClient");
		TM1ProcessDataSourceNameForServer = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceNameForServer");
		TM1ProcessDataSourceOleDbLocation = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceOleDbLocation");
		TM1ProcessDataSourceOleDbMdp = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceOleDbMdp");
		TM1ProcessDataSourcePassword = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourcePassword");
		TM1ProcessDataSourceQuery = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceQuery");
		TM1ProcessDataSourceType = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceType");
		TM1ProcessDataSourceUserName = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessDataSourceUserName");
		TM1ProcessEpilogProcedure = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessEpilogProcedure");
		TM1ProcessGrantSecurityAccess = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessGrantSecurityAccess");
		TM1ProcessMetaDataProcedure = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessMetaDataProcedure");
		TM1ProcessParametersDefaultValues = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessParametersDefaultValues");
		TM1ProcessParametersNames = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessParametersNames");
		TM1ProcessParametersPromptStrings = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessParametersPromptStrings");
		TM1ProcessParametersTypes = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessParametersTypes");
		TM1ProcessPrologProcedure = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessPrologProcedure");
		TM1ProcessUIData = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessUIData");
		TM1ProcessVariablesEndingBytes = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessVariablesEndingBytes");
		TM1ProcessVariablesNames = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessVariablesNames");
		TM1ProcessVariablesPositions = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessVariablesPositions");
		TM1ProcessVariablesStartingBytes = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessVariablesStartingBytes");
		TM1ProcessVariablesTypes = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessVariablesTypes");
		TM1ProcessVariablesUIData = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProcessVariablesUIData");
		TM1SQLTableColumnNames = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1SQLTableColumnNames");
		TM1SQLTableColumnTypes = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1SQLTableColumnTypes");
		TM1SQLTableNumberOfColumns = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1SQLTableNumberOfColumns");
		TM1SQLTableNumberOfRows = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1SQLTableNumberOfRows");
		TM1SQLTableRowsetSize = (TM1V(WINAPI*)())GetProcAddress(tm1api, "TM1SQLTableRowsetSize");
		TM1ErrorBlobCloseFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorBlobCloseFailed");
		TM1ErrorBlobCreateFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorBlobCreateFailed");
		TM1ErrorBlobGetFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorBlobGetFailed");
		TM1ErrorBlobNotOpen = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorBlobNotOpen");
		TM1ErrorBlobOpenFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorBlobOpenFailed");
		TM1ErrorBlobPutFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorBlobPutFailed");
		TM1ErrorClientPasswordNotDefined = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorClientPasswordNotDefined");
		TM1ErrorClientAlreadyExists = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorClientAlreadyExists");
		TM1ErrorCubeCellValueTypeMismatch = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellValueTypeMismatch");
		TM1ErrorCubeCellWriteStatusCubeNoWriteAccess = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusCubeNoWriteAccess");
		TM1ErrorCubeCellWriteStatusCubeLocked = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusCubeLocked");
		TM1ErrorCubeCellWriteStatusCubeReserved = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusCubeReserved");
		TM1ErrorCubeCellWriteStatusElementIsConsolidated = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusElementIsConsolidated");
		TM1ErrorCubeCellWriteStatusElementLocked = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusElementLocked");
		TM1ErrorCubeCellWriteStatusElementNoWriteAccess = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusElementNoWriteAccess");
		TM1ErrorCubeCellWriteStatusElementReserved = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusElementReserved");
		TM1ErrorCubeCellWriteStatusRuleApplies = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusRuleApplies");
		TM1ErrorCubeCellWriteStatusNoReservation = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusNoReservation");
		TM1ErrorCubeCellWriteStatusCellReserved = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCellWriteStatusCellReserved");
		TM1ErrorCubeCreationFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeCreationFailed");
		TM1ErrorCubeDimensionInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeDimensionInvalid");
		TM1ErrorCubeDrillNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeDrillNotFound");
		TM1ErrorCubeDrillInvalidStructure = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeDrillInvalidStructure");
		TM1ErrorCubeKeyInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeKeyInvalid");
		TM1ErrorCubeNotEnoughDimensions = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeNotEnoughDimensions");
		TM1ErrorCubeNumberOfKeysInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeNumberOfKeysInvalid");
		TM1ErrorCubePerspectiveAllSimpleElements = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubePerspectiveAllSimpleElements");
		TM1ErrorCubePerspectiveCreationFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubePerspectiveCreationFailed");
		TM1ErrorCubeTooManyDimensions = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeTooManyDimensions");
		TM1ErrorDataSpreadFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDataSpreadFailed");
		TM1ErrorDimensionCouldNotBeCompiled = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionCouldNotBeCompiled");
		TM1ErrorDimensionElementAlreadyExists = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionElementAlreadyExists");
		TM1ErrorDimensionElementComponentAlreadyExists = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionElementComponentAlreadyExists");
		TM1ErrorDimensionElementComponentDoesNotExist = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionElementComponentDoesNotExist");
		TM1ErrorDimensionElementComponentNotNumeric = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionElementComponentNotNumeric");
		TM1ErrorDimensionElementDoesNotExist = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionElementDoesNotExist");
		TM1ErrorDimensionElementNotConsolidated = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionElementNotConsolidated");
		TM1ErrorDimensionHasCircularReferences = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionHasCircularReferences");
		TM1ErrorDimensionHasNoElements = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionHasNoElements");
		TM1ErrorDimensionIsBeingUsedByCube = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionIsBeingUsedByCube");
		TM1ErrorDimensionNotChecked = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionNotChecked");
		TM1ErrorGroupAlreadyExists = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorGroupAlreadyExists");
		TM1ErrorGroupMaximunNumberExceeded = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorGroupMaximunNumberExceeded");
		TM1ErrorObjectAttributeNotDefined = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectAttributeNotDefined");
		TM1ErrorObjectAttributeInvalidType = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectAttributeInvalidType");
		TM1ErrorObjectDeleted = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectDeleted");
		TM1ErrorObjectDuplicationFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectDuplicationFailed");
		TM1ErrorObjectFileNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectFileNotFound");
		TM1ErrorObjectFunctionDoesNotApply = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectFunctionDoesNotApply");
		TM1ErrorObjectHandleInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectHandleInvalid");
		TM1ErrorObjectHasNoParent = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectHasNoParent");
		TM1ErrorObjectIndexInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectIndexInvalid");
		TM1ErrorObjectFileInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectFileInvalid");
		TM1ErrorObjectInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectInvalid");
		TM1ErrorObjectIncompatibleTypes = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectIncompatibleTypes");
		TM1ErrorObjectIsRegistered = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectIsRegistered");
		TM1ErrorObjectIsUnregistered = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectIsUnregistered");
		TM1ErrorObjectListIsEmpty = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectListIsEmpty");
		TM1ErrorObjectNameExists = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectNameExists");
		TM1ErrorObjectNameIsBlank = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectNameIsBlank");
		TM1ErrorObjectNameInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectNameInvalid");
		TM1ErrorObjectNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectNotFound");
		TM1ErrorObjectNotLoaded = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectNotLoaded");
		TM1ErrorObjectPropertyIsList = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectPropertyIsList");
		TM1ErrorObjectPropertyNotDefined = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectPropertyNotDefined");
		TM1ErrorObjectPropertyNotList = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectPropertyNotList");
		TM1ErrorObjectRegistrationFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectRegistrationFailed");
		TM1ErrorObjectSecurityIsLocked = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectSecurityIsLocked");
		TM1ErrorObjectSecurityNoAdminRights = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectSecurityNoAdminRights");
		TM1ErrorObjectSecurityNoLockRights = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectSecurityNoLockRights");
		TM1ErrorObjectSecurityNoReadRights = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectSecurityNoReadRights");
		TM1ErrorObjectSecurityNoReserveRights = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectSecurityNoReserveRights");
		TM1ErrorObjectSecurityNoWriteRights = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorObjectSecurityNoWriteRights");
		TM1ErrorRuleCubeHasRuleAttached = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorRuleCubeHasRuleAttached");
		TM1ErrorRuleIsAttached = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorRuleIsAttached");
		TM1ErrorRuleIsNotChecked = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorRuleIsNotChecked");
		TM1ErrorRuleLineNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorRuleLineNotFound");
		TM1ErrorSubsetIsBeingUsedByView = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSubsetIsBeingUsedByView");
		TM1ErrorSystemFunctionObsolete = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemFunctionObsolete");
		TM1ErrorSystemServerClientAlreadyConnected = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemServerClientAlreadyConnected");
		TM1ErrorSystemServerClientNotConnected = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemServerClientNotConnected");
		TM1ErrorSystemServerClientNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemServerClientNotFound");
		SystemServerConnectionFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "SystemServerConnectionFailed");
		TM1ErrorSystemServerClientPasswordInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemServerClientPasswordInvalid");
		TM1ErrorSystemServerNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemServerNotFound");
		TM1ErrorSystemOutOfMemory = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemOutOfMemory");
		TM1ErrorSystemUserHandleInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemUserHandleInvalid");
		TM1ErrorSystemValueInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemValueInvalid");
		TM1ErrorSystemParameterTypeInvalid = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemParameterTypeInvalid");
		TM1ErrorViewHasPrivateSubsets = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorViewHasPrivateSubsets");
		TM1ErrorViewNotConstructed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorViewNotConstructed");
		TM1ErrorDimensionNotRegistered = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionNotRegistered");
		TM1ErrorViewExpressionEmpty = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorViewExpressionEmpty");
		TM1ErrorDimensionUpdateFailedInvalidHierarchies = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorDimensionUpdateFailedInvalidHierarchies");
		TM1ErrorUpdateNonLeafCellValueFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorUpdateNonLeafCellValueFailed");
		TM1ErrorCubeNoTimeDimension = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeNoTimeDimension");
		TM1ErrorCubeMeasuresAndTimeDimensionSame = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCubeMeasuresAndTimeDimensionSame");
		TM1ErrorAuditLogResultSetDoesNotExist = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorAuditLogResultSetDoesNotExist");
		TM1ErrorAuditLogResultSetInvalidRange = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorAuditLogResultSetInvalidRange");
		TM1ErrorAuditLogRecordDoesNotExist = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorAuditLogRecordDoesNotExist");
		TM1ErrorAuditLogResultSetOutOfMemory = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorAuditLogResultSetOutOfMemory");
		TM1ErrorExecutingAuditLogQuery = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorExecutingAuditLogQuery");
		TM1ErrorValueNotInPickList = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorValueNotInPickList");
		TM1ErrorServerInBulkLoadMode = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorServerInBulkLoadMode");
		TM1ErrorTM1PATHEnvVarNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorTM1PATHEnvVarNotFound");
		TM1ErrorCannotCreateAlternateHierarchyFromAlternateHierarchy = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCannotCreateAlternateHierarchyFromAlternateHierarchy");
		TM1ErrorSystemServerClientConnectFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemServerClientConnectFailed");
		TM1ProgressMessageOpening = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressMessageOpening");
		TM1ProgressMessageRunning = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressMessageRunning");
		TM1ProgressMessageClosing = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressMessageClosing");
		TM1ProgressTypePercent = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressTypePercent");
		TM1ProgressTypeCounter = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressTypeCounter");
		TM1ProgressActionLoadingCube = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionLoadingCube");
		TM1ProgressActionLoadingDimension = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionLoadingDimension");
		TM1ProgressActionRunningQuery = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionRunningQuery");
		TM1ProgressActionCalculatingView = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionCalculatingView");
		TM1ProgressActionLoadingSubset = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionLoadingSubset");
		TM1ProgressActionSavingSubset = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionSavingSubset");
		TM1ProgressActionSortingSubset = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionSortingSubset");
		TM1ProgressActionCalculatingSubsetAll = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionCalculatingSubsetAll");
		TM1ProgressActionInsertingSubset = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionInsertingSubset");
		TM1ProgressActionDuplicatingSubset = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionDuplicatingSubset");
		TM1ProgressActionCalculatingSubsetHierarchy = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionCalculatingSubsetHierarchy");
		TM1ProgressActionSelectingSubsetElements = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionSelectingSubsetElements");
		TM1ProgressActionDeletingSelection = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionDeletingSelection");
		TM1ProgressActionKeepingSelection = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ProgressActionKeepingSelection");
		TM1ErrorClientAddedWithErrors = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorClientAddedWithErrors");
		TM1ErrorGroupAddedWithErrors = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorGroupAddedWithErrors");
		TM1ErrorInvalidCapabilityFeature = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorInvalidCapabilityFeature");
		TM1ErrorInvalidCapabilityPermission = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorInvalidCapabilityPermission");
		TM1ErrorInvalidCapabilityPolicy = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorInvalidCapabilityPolicy");
		TM1ErrorUpdateNotReady = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorUpdateNotReady");
		TM1ErrorNoUpdateToProcess = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorNoUpdateToProcess");
		TM1ErrorTUnitRedefined = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorTUnitRedefined");
		TM1ErrorChoreModifiedDuringExecution = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorChoreModifiedDuringExecution");
		TM1ErrorChoreDeleted = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorChoreDeleted");
		TM1ViewExtractComparisonNone = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonNone");
		TM1ViewExtractComparisonEQ_A = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonEQ_A");
		TM1ViewExtractComparisonGE_A = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonGE_A");
		TM1ViewExtractComparisonLE_A = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonLE_A");
		TM1ViewExtractComparisonGT_A = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonGT_A");
		TM1ViewExtractComparisonLT_A = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonLT_A");
		TM1ViewExtractComparisonNE_A = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonNE_A");
		TM1ViewExtractComparisonGE_A_LE_B = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonGE_A_LE_B");
		TM1ViewExtractComparisonGT_A_LT_B = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparisonGT_A_LT_B");
		TM1ViewExtractSkipConsolidatedValues = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractSkipConsolidatedValues");
		TM1ViewExtractSkipRuleValues = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractSkipRuleValues");
		TM1ViewExtractSkipZeroes = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractSkipZeroes");
		TM1ViewExtractRealLimitA = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractRealLimitA");
		TM1ViewExtractRealLimitB = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractRealLimitB");
		TM1ViewExtractStringLimitA = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractStringLimitA");
		TM1ViewExtractStringLimitB = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractStringLimitB");
		TM1ViewExtractComparison = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewExtractComparison");
		TM1ClientAddEx = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sClientName, TM1V sClientDefDisplayValue))GetProcAddress(tm1api, "TM1ClientAddEx");
		TM1ObjAliasControlValueGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hObject, TM1V hType))GetProcAddress(tm1api, "TM1ObjAliasControlValueGet");
		TM1ErrorSystemServerCAMSecurityRequired = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorSystemServerCAMSecurityRequired");
		TM1ErrorCAMDllLoadFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCAMDllLoadFailed");
		TM1ErrorCAMObjectCreateFailed = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorCAMObjectCreateFailed");
		TM1ErrorControlAliasNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorControlAliasNotFound");
		TM1ErrorControlAliasInvalidType = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorControlAliasInvalidType");
		TM1ErrorControlAliasInvalidValueType = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorControlAliasInvalidValueType");
		TM1GroupAddEx = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sGroupDefDisplayValue, TM1V sLangDisplayArr))GetProcAddress(tm1api, "TM1GroupAddEx");
		TM1SecurityObject = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1SecurityObject");
		TM1RawstoreExists = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1RawstoreExists");
		TM1AuditLogExists = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1AuditLogExists");
		TM1EnableBulkLoadMode = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1EnableBulkLoadMode");
		TM1DisableBulkLoadMode = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1DisableBulkLoadMode");
		TM1ChangeSetBegin = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ChangeSetBegin");
		TM1ChangeSetEnd = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer))GetProcAddress(tm1api, "TM1ChangeSetEnd");
		TM1ChangeSetUndo = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sChangeSetId))GetProcAddress(tm1api, "TM1ChangeSetUndo");
		TM1ApplicationFolderContentsGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hFolder, TM1V iDepth))GetProcAddress(tm1api, "TM1ApplicationFolderContentsGet");
		TM1DataReservationAcquire = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hClient, TM1V bForce, TM1V elementArray))GetProcAddress(tm1api, "TM1DataReservationAcquire");
		TM1DataReservationRelease = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray))GetProcAddress(tm1api, "TM1DataReservationRelease");
		TM1DataReservationReleaseAll = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray))GetProcAddress(tm1api, "TM1DataReservationReleaseAll");
		TM1DataReservationGetAll = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hClient))GetProcAddress(tm1api, "TM1DataReservationGetAll");
		TM1DataReservationValidate = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube))GetProcAddress(tm1api, "TM1DataReservationValidate");
		TM1DataReservationGetConflicts = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray))GetProcAddress(tm1api, "TM1DataReservationGetConflicts");
		TM1AssociateCAMIDToGroup = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sCAMID, TM1V sCAMIDDefDisplayValue, TM1V sLangDisplayArr))GetProcAddress(tm1api, "TM1AssociateCAMIDToGroup");
		TM1RemoveCAMIDAssociationFromGroup = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sCAMID))GetProcAddress(tm1api, "TM1RemoveCAMIDAssociationFromGroup");
		TM1RemoveCAMIDAssociation = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sCAMID, TM1V bRemoveCAMID))GetProcAddress(tm1api, "TM1RemoveCAMIDAssociation");
		TM1GetCAMIDsAssociatedWithGroup = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sGroupName))GetProcAddress(tm1api, "TM1GetCAMIDsAssociatedWithGroup");
		TM1GetGroupsAssociatedWithCAMID = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sCAMID))GetProcAddress(tm1api, "TM1GetGroupsAssociatedWithCAMID");
		TM1ErrorAssociateCAMIDToGroup = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorAssociateCAMIDToGroup");
		TM1ErrorRemoveCAMIDAssociationFromGroup = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorRemoveCAMIDAssociationFromGroup");
		TM1ErrorRemoveCAMIDAssociation = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorRemoveCAMIDAssociation");
		TM1ErrorGetCAMIDsAssociatedWithGroup = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorGetCAMIDsAssociatedWithGroup");
		TM1ErrorGetGroupsAssociatedWithCAMID = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorGetGroupsAssociatedWithCAMID");
		TM1ErrorGroupAssociationNotFound = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorGroupAssociationNotFound");
		TM1CreateExpressionBasedView = (TM1V(WINAPI*)(TM1P hPool, TM1V hServer, TM1V sExpression))GetProcAddress(tm1api, "TM1CreateExpressionBasedView");
		TM1CubeExpressionBasedViews = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1CubeExpressionBasedViews");
		TM1ViewMDXExpression = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ViewMDXExpression");
		TM1ErrorElementNameContainsMetacharacters = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorElementNameContainsMetacharacters");
		TM1ErrorAccessDisabledByPolicy = (TM1_INDEX(WINAPI*)(void))GetProcAddress(tm1api, "TM1ErrorAccessDisabledByPolicy");
		TM1CubeCellDrillStringGet = (TM1V(WINAPI*)(TM1P hPool, TM1V hCube, TM1V hArrayOfElements))GetProcAddress(tm1api, "TM1CubeCellDrillStringGet");
		TM1DimensionTopElement = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1DimensionTopElement");
		TM1ObjectReplication = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectReplication");
		TM1ObjectReplicationSourceName = (TM1V(WINAPI*)(void))GetProcAddress(tm1api, "TM1ObjectReplicationSourceName");
		TM1SubsetSubtract = (TM1V(WINAPI*)(TM1P hPool, TM1V hSubsetA, TM1V hSubsetB))GetProcAddress(tm1api, "TM1SubsetSubtract");

	}	


	if(!isOk)
		releasDLL();

	return isOk;
}
BOOL releasDLL() {
	if (libibmcogeay32 != NULL&& FreeLibrary(libibmcogeay32))
		libibmcogeay32 = NULL;

	if (log4cxx != NULL&& FreeLibrary(log4cxx))
		log4cxx = NULL;

	if (sslibmcogeay32 != NULL&& FreeLibrary(sslibmcogeay32))
		sslibmcogeay32 = NULL;

	if (TM1ULibDll != NULL && FreeLibrary(TM1ULibDll))
		TM1ULibDll = NULL;

	if (tm1api != NULL && FreeLibrary(tm1api))
		tm1api = NULL;
	
	if (tm1lib != NULL && FreeLibrary(tm1lib))
		tm1lib = NULL;

	return tm1api == NULL 
		   && tm1lib == NULL 
		   && libibmcogeay32 == NULL 
		   && log4cxx == NULL
		   && sslibmcogeay32 == NULL
		   && TM1ULibDll == NULL;
}

// 65001 is utf-8.
wchar_t *CodePageToUnicode(int codePage, const char *src){
	if (!src) return 0;
	int srcLen = (int)strlen(src);
	if (!srcLen){
		wchar_t *w = new wchar_t[1];
		w[0] = 0;
		return w;
	}

	int requiredSize = MultiByteToWideChar(codePage, //кодовая страница
		                                   0,        //нет дополнительных флагов
		                                   src,      //исходная строка
		                                   srcLen,   //длина исходной строки
		                                   nullptr,  //строка назначения (здесь не указана, чтоб посчитать размер)
		                                   0);       //размер аллоцированной строки назначения

	if (!requiredSize)
		return 0;
	

	wchar_t *w = new wchar_t[requiredSize + 1];
	w[requiredSize] = 0;

	int retval = MultiByteToWideChar(codePage,       //кодовая страница
		                             0,              //нет дополнительных флагов
		                             src,            //исходная строка
		                             srcLen,         //длина исходной строки
		                             w,              //строка назначения
		                             requiredSize);  //размер аллоцированной строки назначения
	if (!retval){
		delete[] w;
		return 0;
	}

	return w;
}



char * UnicodeToCodePage(int codePage, const wchar_t *src){
	if (!src) 
		return 0;

	int srcLen = (int)wcslen(src);
	if (!srcLen){
		char *x = new char[1];
		x[0] = '\0';
		return x;
	}

	int requiredSize = WideCharToMultiByte(codePage,  //кодовая страница
		                                   0,         //нет дополнительных флагов
		                                   src,       //исходная строка
		                                   srcLen,    //длина исходной строки
		                                   nullptr,   //строка назначения (здесь не указана, чтоб посчитать размер)
		                                   0,         //размер аллоцированной строки назначения
		                                   0,         //
		                                   0);        //

	if (!requiredSize)
		return 0;
	
	char *x = new char[requiredSize + 1];
	x[requiredSize] = 0;

	int retval = WideCharToMultiByte(codePage,        //кодовая страница
		                             0,               //нет дополнительных флагов
		                             src,             //исходная строка
		                             srcLen,          //длина исходной строки
		                             x,               //строка назначения
		                             requiredSize,    //размер аллоцированной строки назначения
		                             0,               //
		                             0);              //
	if (!retval){
		delete[] x;
		return 0;
	}

	return x;
}


TM1_INDEX getLastError(std::ostringstream & sout, TM1U hUser, TM1V val, bool isShow) noexcept {
	TM1_INDEX rc = TM1ValErrorCode(hUser, val);

	if (rc && isShow){
		char * str = TM1ValErrorString(hUser, val);
		sout << "Error[" << rc << "]: " << str << std::endl;
	}

	return rc;
}

TM1_INDEX getCountObjects(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType){
	//выделяем место в пуле под значение
	TM1V hCount = TM1ObjectListCountGet(hPool, hParrentObject, vType);
	//проверяем успешность получения кол-ва объектов
	if (TM1ValType(hUser, hCount) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "Не удалось получить кол-во объектов:\n\t";
		getLastError(sout, hUser, hCount, true);		
		throw std::exception(sout.str().c_str());
	}else
		return TM1ValIndexGet(hUser, hCount);
}

std::string showObjects(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, const char * str){
	TM1_INDEX cnt = getCountObjects(hUser, hPool, hParrentObject, vType);
	std::ostringstream sout;	
	for (TM1_INDEX i = 0; i < cnt; ++i){
		TM1V hObject = getObjectByIndex(hUser, hPool, hParrentObject, vType, i + 1);
		//проверяем успешность получения объекта по индексу
		if (hObject!=nullptr){			
			TM1V hName = getObjectProperty(hUser, hPool, hObject, TM1ObjectName());
			if (hName)			
				sout << TM1ValStringGet(hUser, hName) << std::endl;
		}
	}
	return sout.str();
}

TM1V getObjectByIndex(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1_INDEX i) noexcept {
	//выделяем место в пуле под значение
	TM1V ind = TM1ValIndex(hPool, i);
	if (ind == 0) {
		//Не удалось получить индекс в пуле 
		return nullptr;
	}

	TM1V hObject = TM1ObjectListHandleByIndexGet(hPool, hParrentObject, vType, ind);
	//проверяем успешность получения объекта по индексу
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "Не удалось получить объект по индексу:" << i << "\n\t";
		getLastError(sout, hUser, hObject, true);
		return nullptr;
	}
	return hObject;
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, const char *strName, TM1V & vName, TM1_INDEX NameLen) noexcept {
	//выделяем место в пуле под значения
	vName = TM1ValString(hPool, const_cast<char*>(strName), NameLen);
	if (vName == 0) {
		//Не удалось получить имя в пуле
		return nullptr;
	}
				
	return getObjectByName(hUser,hPool,hParrentObject, vType,vName);
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V vName) noexcept {
	TM1V hObject = TM1ObjectListHandleByNameGet(hPool, hParrentObject, vType, vName);
	//проверяем успешность получения объекта по имени
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "Не удалось получить объект по имени:" << (vName != nullptr ? TM1ValStringGet(hUser, vName):"") << "\n\t";
		getLastError(sout, hUser, hObject, true);
		return nullptr;
	}

	return hObject;
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, double val, TM1V & vVal) noexcept {
	//выделяем место в пуле под значения
	vVal = TM1ValReal(hPool, val);
	if (vVal == 0) {
		//Не удалось получить имя в пуле 
		vVal = nullptr;
		return nullptr;
	}

	TM1V hObject = TM1ObjectListHandleByNameGet(hPool, hParrentObject, vType, vVal);
	//проверяем успешность получения объекта по имени
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "Не удалось получить объект по имени:" << val << "\n\t";
		getLastError(sout, hUser, hObject, true);
		return nullptr;
	}

	return hObject;
}

TM1V getObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType){
	//выделяем место в пуле под значение
	TM1V hProperty = TM1ObjectPropertyGet(hPool, hParrentObject, vType);
	//проверяем успешность получения дескриптора свойства
	if (TM1ValType(hUser, hProperty) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "Не удалось получить свойство:\n\t";
		getLastError(sout, hUser, hProperty, true);
		throw std::exception(sout.str().c_str());
	}
	return hProperty;
}

bool setObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V val){
	TM1V vOk = TM1ObjectPropertySet(hPool, hParrentObject, vType, val);
	//проверяем успешность установки значения свойства
	if (TM1ValType(hUser, vOk) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "Не удалось установить свойство:\n\t";
		getLastError(sout, hUser, vOk, true);
		throw std::exception(sout.str().c_str());
	}
	return true;
}

TM1V registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, const char *strName, TM1V & vName, TM1_INDEX NameLen) {
	//выделяем место в пуле под значения
	vName = TM1ValString(hPool, const_cast<char*>(strName), NameLen);
	if (vName == 0) 
		throw std::exception("Не удалось получить имя в пуле");	

	return registerObject(hUser, hPool, hParrentObject, hObject, vName);
}

TM1V registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, TM1V vName) {
	//выделяем место в пуле под значение
	TM1V hNewObject = TM1ObjectRegister(hPool, hParrentObject, hObject, vName);
	//проверяем успешность установки значения свойства
	if (TM1ValType(hUser, hNewObject) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "Не удалось опубликовать объект:\n\t";
		getLastError(sout, hUser, hNewObject, true);
		throw std::exception(sout.str().c_str());
	}
	return hNewObject;
}

TM1V duplicateObject(TM1U hUser, TM1P hPool, TM1V hObject) {
	//выделяем место в пуле под значение
	TM1V hDuplicate = TM1ObjectDuplicate(hPool, hObject);
	//проверяем успешность создания копии
	if (TM1ValType(hUser, hDuplicate) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "Не удалось скопировать объект:\n\t";
		getLastError(sout, hUser, hDuplicate, true);
		throw std::exception(sout.str().c_str());
	}
	return hDuplicate;
}

bool deleteObject(TM1U hUser, TM1P hPool, TM1V hObject) noexcept {
	//удаляем объект
	TM1V vBool=TM1ObjectDelete(hPool, hObject);
	//проверяем успешность удаления
	if (TM1ValType(hUser, vBool) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "Не удалось удалить объект:\n\t";
		getLastError(sout, hUser, vBool, true);
		return false;
	}else 
		return TM1ValBoolGet(hUser, vBool);
}

TM1V makeArray(TM1U hUser, TM1P hPool, TM1_INDEX arraySize, TM1V* initArray) {
	//выделяем место в пуле под значение
	TM1V vArrayElements = TM1ValArray(hPool, initArray, arraySize);
	//проверяем успешность создания массива
	if (TM1ValType(hUser, vArrayElements) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "Не удалось создать массив размера:"<< arraySize <<"\n\t";
		getLastError(sout, hUser, vArrayElements, true);
		throw std::exception(sout.str().c_str());
	}else if (TM1ValType(hUser, vArrayElements) == TM1ValTypeBool()) {
		std::ostringstream sout;
		sout << "Не удалось создать массив размера:" << arraySize << "\n";
		getLastError(sout, hUser, vArrayElements, true);
		throw std::exception(sout.str().c_str());
	}else
		return vArrayElements;
}