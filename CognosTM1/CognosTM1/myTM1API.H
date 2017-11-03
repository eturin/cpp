
//описание базового объекта Tm1, используемого в API и на сервере
#ifndef TM1_OBJECT_DEFINED
#define TM1_OBJECT_DEFINED
typedef struct TM1_OBJECT{
   unsigned int  major_ix;
   unsigned int  minor_ix;
   unsigned char dbcon_ix;
   unsigned char otype;
   unsigned int  connectionTimestampMillis;
} TM1_OBJECT;
#endif

#ifndef MY_TM1_API_H
#define MY_TM1_API_H

typedef enum{
   TRADITIONAL_SECURITY_MODE = 1,
   MIXED_SECURITY_MODE,
   INTEGRATED_SECURITY_MODE,
   CAM_SECURITY_MODE,
   CAM_SECURITY_MIXED_MODE,
   DISTRIBUTED_SECURITY_MODE,
   SECURITY_MODE_SIZE
} SecurityMode;

typedef enum{
   OWNER_TYPE = 1,
   OWNER,
   EVENT_TYPE,
   EVENT,
   AFFECTED_OBJECT_TYPE,
   AFFECTED_OBJECT,
   AUDIT_QUERY_FILTER_TYPE_SIZE
} TM1AuditQueryFilterTypes;

typedef enum{
   CLIENT_OWNER = 1,
   SCHEDULED_CHORE_OWNER,
   AUDIT_QUERY_OWNER_TYPE_SIZE
} TM1AuditQueryOwnerTypes;

typedef enum{
   SYSTEM_WIDE_EVENT = 1,
   OBJECT_SPECIFIC_EVENT,
   AUDIT_QUERY_EVENT_TYPE_SIZE
} TM1AuditQueryEventTypes;

typedef enum{
   TYPE_STATIC = 1,
   TYPE_SUBSET,
   TYPE_DIMENSION,
   TYPE_NONE
} TM1PickListTypes;


#ifndef TM1_GEN_64
#if defined(GEN_WIN64) || defined(GEN_HP64) || defined(GEN_AIX64) || defined(GEN_LINUX64) || defined(GEN_SUN64)
#define TM1_GEN_64
#endif 
#endif // TM1_GEN_64

#ifdef WIN32
  #ifdef BUILD_TM1SERVER
    #define TM1API __stdcall
    #ifdef TM1IMPORT
      #undef TM1IMPORT
    #endif
    #define TM1IMPORT
  #else
    #define TM1API __stdcall
    #ifndef TM1IMPORT
      #define TM1IMPORT __declspec( dllimport )
    #endif
  #endif
#else
    // Unix
  #define TM1API
  #ifndef TM1IMPORT
    #define TM1IMPORT
  #endif
    typedef char CHAR;
    typedef unsigned char UCHAR;
#endif



//константы TM1 (максимальное кол-во измерений)   
#define TM1_MAXIMUM_DIMENSIONS     256 

//типы TM1
typedef double         TM1_REAL;		// real number 

#ifndef TM1_UTF16_T_DEF
   #define TM1_UTF16_T_DEF 1
   typedef unsigned short  TM1_UTF16_T;	
   typedef unsigned char  TM1_UTF8_T;
#endif 

typedef unsigned char  TM1_BYTE_T;
#ifdef TM1_GEN_64
  typedef unsigned int  TM1_INDEX;      // index 
#else
  typedef unsigned long  TM1_INDEX;      // index 
#endif

typedef unsigned short TM1_BOOL;       // boolean flag


typedef TM1_INDEX    * TM1V;           // дескриптор значения
typedef void         * TM1U;           // дескриптор сессии 
typedef void         * TM1P;           // дескриптор pool-а 

typedef void (TM1API * TM1_HOOK     )( TM1_INDEX message, TM1_INDEX action, TM1_INDEX param, CHAR        * name );
typedef void (TM1API * TM1_HOOK_W   )( TM1_INDEX message, TM1_INDEX action, TM1_INDEX param, TM1_UTF16_T * name );
typedef void (TM1API * TM1_HOOK_UTF8)( TM1_INDEX message, TM1_INDEX action, TM1_INDEX param, TM1_UTF8_T  * name );
#endif

#ifdef NOT_EXT
	#define PRE 
#else
	#define PRE extern
#endif


#ifndef ALL_FUNC
#define ALL_FUNC

//функции из dll
PRE void (WINAPI  *TM1APIInitialize)( void );
PRE void (WINAPI  *TM1APIFinalize)( void );
PRE TM1V (WINAPI  *TM1BlobClose)( TM1P hPool, TM1V hBlob );
PRE TM1V (WINAPI  *TM1BlobCreate)( TM1P hPool, TM1V hServer, TM1V sName );
PRE TM1_INDEX (WINAPI  *TM1BlobGet)( TM1U hUser, TM1V hBlob, TM1_INDEX x, TM1_INDEX n, CHAR * buf );

PRE TM1V (WINAPI  *TM1BlobOpen)( TM1P hPool, TM1V hBlob );
PRE TM1V (WINAPI  *TM1BlobOpenEx)( TM1P hPool, TM1V hBlob, TM1V bOverwrite );
PRE TM1_INDEX (WINAPI  *TM1BlobPut)( TM1U hUser, TM1V hBlob, TM1_INDEX x, TM1_INDEX n, CHAR * buf );

PRE TM1V (WINAPI  *TM1CapabilityGetPolicy)( TM1P hPool, TM1V hClient, TM1V vFeature, TM1V vPermission );
PRE TM1V (WINAPI  *TM1CapabilityGet)( TM1P hPool, TM1V hGroup, TM1V vFeature, TM1V vPermission );
PRE TM1V (WINAPI  *TM1CapabilityGetFeatures)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1CapabilitySet)( TM1P hPool, TM1V hGroup, TM1V vFeature, TM1V vPermission, TM1V vPolicy );
PRE TM1V (WINAPI  *TM1CapabilityGetPermissions)( TM1P hPool, TM1V hServer, TM1V vFeature );

PRE TM1V (WINAPI  *TM1ChoreCreateEmpty)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1ChoreExecute)( TM1P hPool, TM1V hChore );
PRE TM1V (WINAPI  *TM1ClientAdd)( TM1P hPool, TM1V hServer, TM1V sClientName );
PRE TM1V (WINAPI  *TM1ClientGroupAssign)( TM1P hPool, TM1V hClient, TM1V hGroup );
PRE TM1V (WINAPI  *TM1ClientGroupIsAssigned)( TM1P hPool, TM1V hClient, TM1V hGroup );
PRE TM1V (WINAPI  *TM1ClientGroupRemove)( TM1P hPool, TM1V hClient, TM1V hGroup );
PRE TM1V (WINAPI  *TM1ClientHasHolds)( TM1P hPool, TM1V hClient);
PRE TM1V (WINAPI  *TM1ClientPasswordAssign)( TM1P hPool, TM1V hClient, TM1V sPassword );
PRE TM1V (WINAPI  *TM1ConnectionCheck)( TM1P hPool, TM1V hConnection );
PRE TM1V (WINAPI  *TM1ConnectionCreate)( TM1P hPool, TM1V hServer, TM1V sMasterServerName, TM1V sUsername, TM1V sPassword );
PRE TM1V (WINAPI  *TM1ConnectionCreateEx)( TM1P hPool, TM1V hServer, TM1V sMasterServerName, TM1V sUsername, TM1V sPassword, TM1V sNamespace, TM1V bUseIntegratedSecurity );
PRE TM1V (WINAPI  *TM1ConnectionDelete)( TM1P hPool, TM1V hConnection );
PRE TM1V (WINAPI  *TM1ConnectionSynchronize)( TM1P hPool, TM1V hConnection );
PRE TM1V (WINAPI  *TM1CubeCellDrillListGet)( TM1P hPool, TM1V hCube, TM1V hArrayOfKeys );
PRE TM1V (WINAPI  *TM1CubeCellsDrillListGet)( TM1P hPool, TM1V hCube, TM1V hArrayOfArraysOfKeys );
PRE TM1V (WINAPI  *TM1CubeCellDrillObjectBuild)( TM1P hPool, TM1V hCube, TM1V hArrayOfKeys, TM1V sDrillProcessName );
PRE TM1V (WINAPI  *TM1CubeCellValueGet)( TM1P hPool, TM1V hCube, TM1V hArrayOfElements );
PRE TM1V (WINAPI  *TM1CubeCellPickListGet)( TM1P hPool, TM1V hCube, TM1V hArrayOfElements );
PRE TM1V (WINAPI  *TM1CubeCellsPickListGet)( TM1P hPool, TM1V hCube, TM1V hArrayOfCells);
PRE TM1V (WINAPI  *TM1CubeCellPickListExists)( TM1P hPool, TM1V hCube, TM1V hArrayOfElements );
PRE TM1V (WINAPI  *TM1PickListCubeCreate)( TM1P hPool, TM1V hCube );
PRE TM1V (WINAPI  *TM1PickListCubeExists)( TM1P hPool, TM1V hCube );
PRE TM1V (WINAPI  *TM1CubeCellValueSet)( TM1P hPool, TM1V hCube, TM1V hArrayOfElements, TM1V hValue );
PRE TM1V (WINAPI  *TM1CubeCellSpread)( TM1P hPool, TM1V hServer, TM1V vArrayOfCells, TM1V vCellReference, TM1V sSpreadData );
PRE TM1V (WINAPI  *TM1CubeCellSpreadStatusGet)( TM1P hPool, TM1V hServer, TM1V hCells, TM1V hCellRange );
PRE TM1V (WINAPI  *TM1CubeCellSpreadViewArray)( TM1P hPool, TM1V hView, TM1V aCellRange, TM1V aCellRef, TM1V sControl );
PRE TM1V (WINAPI  *TM1CubeCreate)( TM1P hPool, TM1V hServer, TM1V hArrayOfDimensions );
PRE TM1V (WINAPI  *TM1CubePerspectiveCreate)( TM1P hPool, TM1V hCube, TM1V hArrayOfElementTitles );
PRE TM1V (WINAPI  *TM1CubePerspectiveDestroy)( TM1P hPool, TM1V hCube, TM1V hArrayOfElementTitles );
PRE TM1V (WINAPI  *TM1CubeShowsNulls)( TM1P hPool, TM1V hCube );
PRE TM1V (WINAPI  *TM1CubeClearData)( TM1P hPool, TM1V hCube );
PRE TM1V (WINAPI  *TM1DimensionCheck)( TM1P hPool, TM1V hDimension );
PRE TM1V (WINAPI  *TM1DimensionCreateEmpty)( TM1P hPool, TM1V hServer ); 
PRE TM1V (WINAPI  *TM1DimensionElementComponentAdd)( TM1P hPool, TM1V hElement, TM1V hComponent, TM1V rWeight );
PRE TM1V (WINAPI  *TM1DimensionElementComponentDelete)( TM1P hPool, TM1V hCElement, TM1V hElement );
PRE TM1V (WINAPI  *TM1DimensionElementComponentWeightGet)( TM1P hPool, TM1V hCElement, TM1V hElement );
PRE TM1V (WINAPI  *TM1DimensionElementDelete)( TM1P hPool, TM1V hElement );
PRE TM1V (WINAPI  *TM1DimensionElementInsert)( TM1P hPool, TM1V hDimension, TM1V hElementAfter, TM1V sName, TM1V vType ); 
PRE TM1V (WINAPI  *TM1DimensionRootElementsGet)( TM1P hPool, TM1V hDimension );
PRE TM1V (WINAPI  *TM1DimensionRootSubsetGet)( TM1P hPool, TM1V hDimension, TM1V iRight );
PRE TM1V (WINAPI  *TM1DimensionElementParentsSubsetGet)( TM1P hPool, TM1V hElement, TM1V iRight );
PRE TM1V (WINAPI  *TM1DimensionElementChildrenSubsetGet)( TM1P hPool, TM1V hElement, TM1V iRight );
PRE TM1V (WINAPI  *TM1DimensionUpdate)( TM1P hPool, TM1V hOldDimension, TM1V hNewDimension );
PRE TM1V (WINAPI  *TM1GroupAdd)( TM1P hPool, TM1V hServer, TM1V sGroupName );
PRE TM1V (WINAPI  *TM1IsTICubeDimensionOrderMatch)( TM1P hPool, TM1V hProcess, TM1V hCubeName );
PRE TM1V (WINAPI  *TM1ObjectAttributeDelete)( TM1P hPool, TM1V hObject, TM1V hAttribute );
PRE TM1V (WINAPI  *TM1ObjectAttributeInsert)( TM1P hPool, TM1V hObject, TM1V hAttributeBefore, TM1V sName, TM1V vType );
PRE TM1V (WINAPI  *TM1ObjectAttributeValueGet)( TM1P hPool, TM1V hObject, TM1V hAttribute );
PRE TM1V (WINAPI  *TM1ObjectAttributeValueSet)( TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V vValue );
PRE TM1V (WINAPI  *TM1ObjectAttributeLocaleValueGet)( TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V Locale );
PRE TM1V (WINAPI  *TM1ObjectAttributeLocaleValueSet)( TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V Locale, TM1V vValue );
PRE TM1V (WINAPI  *TM1ObjectCopy)( TM1P hPool, TM1V hSrcObject, TM1V hDstObject );
PRE TM1V (WINAPI  *TM1ObjectDelete)( TM1P hPool, TM1V hObject );
PRE TM1V (WINAPI  *TM1ObjectDestroy)( TM1P hPool, TM1V hObject ); 
PRE TM1V (WINAPI  *TM1ObjectDuplicate)( TM1P hPool, TM1V hObject );
PRE TM1V (WINAPI  *TM1ObjectFileDelete)( TM1P hPool, TM1V hObject );
PRE TM1V (WINAPI  *TM1ObjectFileLoad)( TM1P hPool, TM1V hServer, TM1V hParent, TM1V iObjectType, TM1V sObjectName );
PRE TM1V (WINAPI  *TM1ObjectFileSave)( TM1P hPool, TM1V hObject );
PRE TM1V (WINAPI  *TM1ObjectListCountGet)( TM1P hPool, TM1V hObject, TM1V iPropertyList );
PRE TM1V (WINAPI  *TM1ObjectListHandleByIndexGet)( TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V iIndex );
PRE TM1V (WINAPI  *TM1ObjectListHandleByNameGet)( TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V sName );
PRE TM1V (WINAPI  *TM1ObjectPrivateDelete)( TM1P hPool, TM1V hObject );
PRE TM1V (WINAPI  *TM1ObjectPrivateListCountGet)( TM1P hPool, TM1V hObject, TM1V iPropertyList );
PRE TM1V (WINAPI  *TM1ObjectPrivateListHandleByIndexGet)( TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V iIndex );
PRE TM1V (WINAPI  *TM1ObjectPrivateListHandleByNameGet)( TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V sName );
PRE TM1V (WINAPI  *TM1ObjectPrivatePublish)( TM1P hPool, TM1V hObject, TM1V sName );
PRE TM1V (WINAPI  *TM1ObjectPrivateRegister)( TM1P hPool, TM1V hParent, TM1V hObject, TM1V sName );
PRE TM1V (WINAPI  *TM1ObjectPropertyGet)( TM1P hPool, TM1V hObject, TM1V vProperty );
PRE TM1V (WINAPI  *TM1ObjectPropertySet)( TM1P hPool, TM1V hObject, TM1V vProperty, TM1V vValue );
PRE TM1V (WINAPI  *TM1ObjectSubPropertyByRangeGet)( TM1P hPool, TM1V hRootObject, TM1V vObjectType, TM1V vPrivate, TM1V vStartIndex, TM1V vQuantity, TM1V vProperty);
PRE TM1V (WINAPI  *TM1ObjectRegister)( TM1P hPool, TM1V hParent, TM1V hObject, TM1V sName );
PRE TM1V (WINAPI  *TM1ObjectReplicate)( TM1P hPool, TM1V hObject );
PRE TM1V (WINAPI  *TM1ObjectSecurityLock)( TM1P hPool, TM1V hObject ); 
PRE TM1V (WINAPI  *TM1ObjectSecurityRelease)( TM1P hPool, TM1V hObject ); 
PRE TM1V (WINAPI  *TM1ObjectSecurityReserve)( TM1P hPool, TM1V hObject ); 
PRE TM1V (WINAPI  *TM1ObjectSecurityRightGet)( TM1P hPool, TM1V hObject, TM1V hGroup );
PRE TM1V (WINAPI  *TM1ObjectSecurityRightSet)( TM1P hPool, TM1V hObject, TM1V hGroup, TM1V iRight );
PRE TM1V (WINAPI  *TM1ObjectSecurityUnLock)( TM1P hPool, TM1V hObject ); 
PRE TM1V (WINAPI  *TM1ProcessExecute)( TM1P hPool, TM1V hProcess, TM1V hParametersArray );
PRE TM1V (WINAPI  *TM1ProcessCheck)( TM1P hPool, TM1V hProcess );
PRE TM1V (WINAPI  *TM1ProcessCreateEmpty)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1ProcessExecuteEx)( TM1P hPool, TM1V hProcess, TM1V hParametersArray );
PRE TM1V (WINAPI  *TM1ProcessExecuteSQLQuery)( TM1P tPool, TM1V hProcess, TM1V voDatabaseInfoArray);
PRE TM1V (WINAPI  *TM1ProcessVariableNameIsValid)( TM1P hPool, TM1V hProcess, TM1V hVariableName );
PRE TM1V (WINAPI  *TM1RuleAttach)( TM1P hPool, TM1V hRule);
PRE TM1V (WINAPI  *TM1RuleCheck)( TM1P hPool, TM1V hRule);
PRE TM1V (WINAPI  *TM1RuleCreateEmpty)( TM1P hPool, TM1V hCube, TM1V hType); 
PRE TM1V (WINAPI  *TM1RuleDetach)( TM1P hPool, TM1V hRule);
PRE TM1V (WINAPI  *TM1RuleLineGet)( TM1P hPool, TM1V hRule, TM1V iPosition);
PRE TM1V (WINAPI  *TM1RuleLineInsert)( TM1P hPool, TM1V hRule, TM1V iPosition, TM1V sLine);
PRE TM1V (WINAPI  *TM1ServerBatchUpdateStart)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1ServerBatchUpdateIsActive)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1ServerBatchUpdateFinish)( TM1P hPool, TM1V hServer, TM1V bDiscard );
PRE TM1V (WINAPI  *TM1ServerLogClose)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1ServerLogNext)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1ServerLogOpen)( TM1P hPool, TM1V hServer, TM1V sStartTime, TM1V sCubeFilter, TM1V sUserFilter, TM1V sFlagFilter );
PRE TM1V (WINAPI  *TM1AuditLogQueryExecute)( TM1P hPool, TM1V hServer, TM1V sStartTime, TM1V sEndTime, TM1V aAdditionalFilters );
PRE TM1V (WINAPI  *TM1AuditLogResultSetRangeGet)( TM1P hPool, TM1V hServer, TM1V iStartIndex, TM1V iEndIndex );
PRE TM1V (WINAPI  *TM1AuditLogResultSetDestroy)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1AuditLogResultSetDetailsGet)( TM1P hPool, TM1V hServer, TM1V iEventID );
PRE TM1V (WINAPI  *TM1AuditLogResultSetDetailsRangeGet)( TM1P hPool, TM1V hServer, TM1V iEventID, TM1V iStartIndex, TM1V iEndIndex );
PRE TM1V (WINAPI  *TM1AuditLogResultSetDetailsDestroy)( TM1P hPool, TM1V hServer, TM1V iEventID );
PRE TM1V (WINAPI  *TM1AuditLogSystemEventsGet)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1AuditLogObjectEventsGet)( TM1P hPool, TM1V hServer, TM1V iObjectType );
PRE TM1V (WINAPI  *TM1AuditLogRawstoreRoll)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1ServerOpenSQLQuery)( TM1P hPool, TM1V hServer, TM1V hDsnInfo);
PRE TM1V (WINAPI  *TM1ServerPasswordChange)( TM1P hPool, TM1V hServer, TM1V sNewPassword );
PRE TM1V (WINAPI  *TM1ServerSecurityRefresh)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1SQLTableGetNextRows)( TM1P hPool, TM1V hSQLTable, TM1V bColumnSelection );
PRE TM1V (WINAPI  *TM1SubsetAll)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1SubsetRootElementsGet)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1SubsetCreateEmpty)( TM1P hPool, TM1V hDim );
PRE TM1V (WINAPI  *TM1SubsetElementDisplay)( TM1P hPool, TM1V hSubset, TM1V iElement );
PRE TM1_BOOL (WINAPI  *TM1SubsetElementDisplayEll)( TM1U hUser, TM1V vString );
PRE TM1_INDEX (WINAPI  *TM1SubsetElementDisplayLevel)( TM1U hUser, TM1V vString );
PRE TM1_BOOL (WINAPI  *TM1SubsetElementDisplayLine)( TM1U hUser, TM1V vString, TM1_INDEX Index );
PRE TM1_BOOL (WINAPI  *TM1SubsetElementDisplayMinus)( TM1U hUser, TM1V vString );
PRE TM1_BOOL (WINAPI  *TM1SubsetElementDisplayPlus)( TM1U hUser, TM1V vString );
PRE TM1_BOOL (WINAPI  *TM1SubsetElementDisplaySelection)( TM1U hUser, TM1V vString );
PRE TM1_BOOL (WINAPI  *TM1SubsetElementDisplayTee)( TM1U hUser, TM1V vString );
PRE TM1_REAL (WINAPI  *TM1SubsetElementDisplayWeight)( TM1U hUser, TM1V vString );
PRE TM1V (WINAPI  *TM1SubsetInsertElement)( TM1P hPool, TM1V hSubset, TM1V hElement, TM1V iPosition ); 
PRE TM1V (WINAPI  *TM1SubsetInsertSubset)( TM1P hPool, TM1V hSubsetA, TM1V hSubsetB, TM1V iPosition ); 
PRE TM1V (WINAPI  *TM1SubsetSelectByAttribute)( TM1P hPool, TM1V hSubset, TM1V hAlias, TM1V sValueToMatch, TM1V bSelection );
PRE TM1V (WINAPI  *TM1SubsetSelectByIndex)( TM1P hPool, TM1V hSubset, TM1V iPosition, TM1V bSelection );
PRE TM1V (WINAPI  *TM1SubsetMultiSelectByIndex)( TM1P hPool, TM1V hSubset, TM1V iPositionArr, TM1V bSelectionArr );
PRE TM1V (WINAPI  *TM1SubsetSelectByLevel)( TM1P hPool, TM1V hSubset, TM1V iLevel, TM1V bSelection );
PRE TM1V (WINAPI  *TM1SubsetSelectByPattern)( TM1P hPool, TM1V hSubset, TM1V sPattern, TM1V bSelection );
PRE TM1V (WINAPI  *TM1SubsetSelectionDelete)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1SubsetSelectionInsertChildren)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1SubsetSelectionInsertParents)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1SubsetSelectionKeep)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1SubsetSelectNone)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1SubsetSort)( TM1P hPool, TM1V hSubset, TM1V bSortDown );
PRE TM1V (WINAPI  *TM1SubsetSortByHierarchy)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1SubsetUpdate)( TM1P hPool, TM1V hOldSubset, TM1V hNewSubset );
PRE TM1V (WINAPI  *TM1SubsetCreateByExpression)( TM1P hPool, TM1V hServer, TM1V sExpression );
PRE TM1V (WINAPI  *TM1ViewCreateByExpression)(TM1P hPool, TM1V hServer, TM1V sExpression);

PRE CHAR * (WINAPI  *TM1SystemAdminHostGet)( TM1U hUser );
PRE TM1_UTF16_T * (WINAPI  *TM1SystemAdminHostGetW)( TM1U hUser );
PRE TM1_UTF8_T * (WINAPI  *TM1SystemAdminHostGetUTF8)( TM1U hUser );

PRE void (WINAPI  *TM1SystemAdminHostSet)( TM1U hUser, CHAR * szAdminHosts );
PRE void (WINAPI  *TM1SystemAdminHostSetW)( TM1U hUser, TM1_UTF16_T * szAdminHosts );
PRE void (WINAPI  *TM1SystemAdminHostSetUTF8)( TM1U hUser, TM1_UTF8_T * szAdminHosts );

PRE TM1_BOOL (WINAPI  *TM1APISetSSLCertVersion)( TM1U hUser, TM1_INDEX nCertificateVersion );

PRE void (WINAPI  *TM1SystemSetAdminSSLCertAuthority)( TM1U hUser, CHAR * szAdminSSLCertAuthority );
PRE void (WINAPI  *TM1SystemSetAdminSSLCertAuthorityW)( TM1U hUser, TM1_UTF16_T * szAdminSSLCertAuthority );
PRE void (WINAPI  *TM1SystemSetAdminSSLCertAuthorityUTF8)( TM1U hUser, TM1_UTF8_T * szAdminSSLCertAuthority );

PRE CHAR * (WINAPI  *TM1SystemGetAdminSSLCertAuthority)( TM1U hUser );
PRE TM1_UTF16_T * (WINAPI  *TM1SystemGetAdminSSLCertAuthorityW)( TM1U hUser );
PRE TM1_UTF8_T * (WINAPI  *TM1SystemGetAdminSSLCertAuthorityUTF8)( TM1U hUser );

PRE int (WINAPI  *TM1ValidateSSLConfig)( TM1U hUser );
PRE void (WINAPI  *TM1SystemSetAdminSSLCertRevList)( TM1U hUser, CHAR * szAdminSSLCertRevList );
PRE void (WINAPI  *TM1SystemSetAdminSSLCertRevListW)( TM1U hUser, TM1_UTF16_T * szAdminSSLCertRevList );
PRE void (WINAPI  *TM1SystemSetAdminSSLCertRevListUTF8)( TM1U hUser, TM1_UTF8_T * szAdminSSLCertRevList );

PRE CHAR * (WINAPI  *TM1SystemGetAdminSSLCertRevList)( TM1U hUser );
PRE TM1_UTF16_T * (WINAPI  *TM1SystemGetAdminSSLCertRevListW)( TM1U hUser );
PRE TM1_UTF8_T * (WINAPI  *TM1SystemGetAdminSSLCertRevListUTF8)( TM1U hUser );

PRE void (WINAPI  *TM1SystemSetAdminSSLCertID)( TM1U hUser, CHAR * szAdminSSLCertID );
PRE void (WINAPI  *TM1SystemSetAdminSSLCertIDW)( TM1U hUser, TM1_UTF16_T * szAdminSSLCertID );
PRE void (WINAPI  *TM1SystemSetAdminSSLCertIDUTF8)( TM1U hUser, TM1_UTF8_T * szAdminSSLCertID );

PRE CHAR * (WINAPI  *TM1SystemGetAdminSSLCertID)( TM1U hUser );
PRE TM1_UTF16_T * (WINAPI  *TM1SystemGetAdminSSLCertIDW)( TM1U hUser );
PRE TM1_UTF8_T * (WINAPI  *TM1SystemGetAdminSSLCertIDUTF8)( TM1U hUser );

PRE void (WINAPI  *TM1SystemSetExportAdminSvrSSLCertFlag)( TM1U hUser, TM1_BOOL bExportAdminSvrSSLCert );

PRE void (WINAPI  *TM1SystemSetAdminSvrExportKeyID)( TM1U hUser, CHAR * szAdminSvrExportKeyID );
PRE void (WINAPI  *TM1SystemSetAdminSvrExportKeyIDW)( TM1U hUser, TM1_UTF16_T * szAdminSvrExportKeyID );
PRE void (WINAPI  *TM1SystemSetAdminSvrExportKeyIDUTF8)( TM1U hUser, TM1_UTF8_T * szAdminSvrExportKeyID );

PRE void (WINAPI  *TM1SystemSetSSLDirectory)( TM1U hUser, CHAR * szSSLCertsDirectory );
PRE void (WINAPI  *TM1SystemSetSSLDirectoryW)( TM1U hUser, TM1_UTF16_T * szSSLCertsDirectory );
PRE void (WINAPI  *TM1SystemSetSSLDirectoryUTF8)( TM1U hUser, TM1_UTF8_T * szSSLCertsDirectory );

PRE CHAR * (WINAPI  *TM1SystemGetSSLDirectory)( TM1U hUser );
PRE TM1_UTF16_T * (WINAPI  *TM1SystemGetSSLDirectoryW)( TM1U hUser );
PRE TM1_UTF8_T * (WINAPI  *TM1SystemGetSSLDirectoryUTF8)( TM1U hUser );

PRE CHAR * (WINAPI  *TM1SystemBuildNumber)( void );
PRE TM1_UTF16_T * (WINAPI  *TM1SystemBuildNumberW)( void );
PRE TM1_UTF8_T * (WINAPI  *TM1SystemBuildNumberUTF8)( void );

PRE void (WINAPI  *TM1SystemClose)( TM1U hUser );
PRE TM1U (WINAPI  *TM1SystemOpen)( void );
PRE void (WINAPI  *TM1SystemProgressHookSet)( TM1U hUser, TM1_HOOK pHook );
PRE void (WINAPI  *TM1SystemProgressHookSetW)( TM1U hUser, TM1_HOOK_W pHook );
PRE void (WINAPI  *TM1SystemProgressHookSetUTF8)( TM1U hUser, TM1_HOOK_UTF8 pHook);
PRE TM1_BOOL (WINAPI  *TM1IsProgressHookActive)( TM1U hUser );

PRE CHAR * (WINAPI  *TM1SystemServerClientName)( TM1U hUser, unsigned index );
PRE TM1_UTF16_T * (WINAPI  *TM1SystemServerClientNameW)( TM1U hUser, unsigned index );
PRE TM1_UTF8_T * (WINAPI  *TM1SystemServerClientNameUTF8)( TM1U hUser, unsigned index );

PRE TM1V (WINAPI  *TM1SystemServerConnect)( TM1P hPool, TM1V sServerName, TM1V sClientName, TM1V sPassword );
PRE TM1V (WINAPI  *TM1SystemServerLimitedConnect)( TM1P hPool, TM1V sServerName );
PRE TM1V (WINAPI  *TM1SystemServerConnectIntegratedLogin)( TM1P hPool, TM1V sServerName);
PRE TM1V (WINAPI  *TM1SystemServerDisconnect)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1SystemServerHandle)( TM1U hUser, CHAR * szName );
PRE TM1V (WINAPI  *TM1SystemServerHandleW)( TM1U hUser, TM1_UTF16_T * szName );
PRE TM1V (WINAPI  *TM1SystemServerHandleUTF8)( TM1U hUser, TM1_UTF8_T * szName );
PRE TM1V (WINAPI  *TM1SystemServerConnectWithCAMPassport)( TM1P hPool, TM1V sServerName, TM1V camArgs );
PRE TM1V (WINAPI  *TM1SystemServerConnectWithCAMNamespace)( TM1P hPool, TM1V sServerName, TM1V camArgs );
PRE TM1V (WINAPI  *TM1SystemGetServerConfig)( TM1P hPool, TM1V sServerName );

PRE CHAR * (WINAPI  *TM1SystemServerName)( TM1U hUser, unsigned index );
PRE TM1_UTF16_T * (WINAPI  *TM1SystemServerNameW)( TM1U hUser, unsigned index );
PRE TM1_UTF8_T * (WINAPI  *TM1SystemServerNameUTF8)( TM1U hUser, unsigned index );

PRE int (WINAPI  *TM1SystemServerNof)( TM1U hUser );
PRE void (WINAPI  *TM1SystemServerReload)( TM1U hUser );
PRE TM1_BOOL (WINAPI  *TM1SystemServerStart)( TM1U hUser, CHAR * szName, CHAR * szDataDirectory, CHAR * szAdminHost, CHAR * szProtocol, int iPortNumber );
PRE TM1_BOOL (WINAPI  *TM1SystemServerStartW)( TM1U hUser, TM1_UTF16_T * szName, TM1_UTF16_T * szDataDirectory, TM1_UTF16_T * szAdminHost, TM1_UTF16_T * szProtocol, int iPortNumber );
PRE TM1_BOOL (WINAPI  *TM1SystemServerStartUTF8)( TM1U hUser, TM1_UTF8_T * szName, TM1_UTF8_T * szDataDirectory, TM1_UTF8_T * szAdminHost, TM1_UTF8_T * szProtocol, int iPortNumber );

PRE TM1_BOOL (WINAPI  *TM1SystemServerStartEx)( TM1U hUser, CHAR * szcmdline );
PRE TM1_BOOL (WINAPI  *TM1SystemServerStartExW)( TM1U hUser, TM1_UTF16_T * szcmdline );
PRE TM1_BOOL (WINAPI  *TM1SystemServerStartExUTF8)( TM1U hUser, TM1_UTF8_T * szcmdline );

PRE TM1_BOOL (WINAPI  *TM1SystemServerStop)( TM1U hUser, CHAR * szName, TM1_BOOL bSave );
PRE TM1_BOOL (WINAPI  *TM1SystemServerStopW)( TM1U hUser, TM1_UTF16_T * szName, TM1_BOOL bSave );
PRE TM1_BOOL (WINAPI  *TM1SystemServerStopUTF8)( TM1U hUser, TM1_UTF8_T * szName, TM1_BOOL bSave );

PRE int (WINAPI  *TM1SystemVersionGet)( void );
PRE TM1V (WINAPI  *TM1ValArray)( TM1P hPool, TM1V * InitArray, TM1_INDEX MaxSize );
PRE TM1V (WINAPI  *TM1ValArrayGet)( TM1U hUser, TM1V vArray, TM1_INDEX Index );
PRE TM1_INDEX (WINAPI  *TM1ValArrayMaxSize)( TM1U hUser, TM1V vArray );
PRE void (WINAPI  *TM1ValArraySet)( TM1V vArray, TM1V vValue, TM1_INDEX Index );
PRE void (WINAPI  *TM1ValArraySetSize)( TM1V vArray, TM1_INDEX Size );
PRE TM1V (WINAPI  *TM1ValBool)( TM1P hPool, TM1_BOOL InitBool );
PRE TM1_BOOL (WINAPI  *TM1ValBoolGet)( TM1U hUser, TM1V vBool );
PRE void (WINAPI  *TM1ValBoolSet)( TM1V vBool, TM1_BOOL Bool );
PRE TM1_INDEX (WINAPI  *TM1ValErrorCode)( TM1U hUser, TM1V vError );

PRE CHAR * (WINAPI  *TM1ValErrorString)( TM1U hUser, TM1V vValue );
PRE TM1_UTF16_T * (WINAPI  *TM1ValErrorStringW)( TM1U hUser, TM1V vValue );
PRE TM1_UTF8_T * (WINAPI  *TM1ValErrorStringUTF8)( TM1U hUser, TM1V vValue );

PRE TM1V (WINAPI  *TM1ValIndex)( TM1P hPool, TM1_INDEX InitIndex );
PRE TM1_INDEX (WINAPI  *TM1ValIndexGet)( TM1U hUser, TM1V vIndex );
PRE void (WINAPI  *TM1ValIndexSet)( TM1V vIndex, TM1_INDEX Index );
PRE TM1_BOOL (WINAPI  *TM1ValIsUndefined)( TM1U hUser, TM1V Value );
PRE TM1_BOOL (WINAPI  *TM1ValIsUpdatable)( TM1U hUser, TM1V Value );
PRE TM1_BOOL (WINAPI  *TM1ValIsChanged)( TM1U hUser, TM1V Value );

PRE TM1V (WINAPI  *TM1ValObject)( TM1P hPool, TM1_OBJECT * InitObject );
PRE TM1_BOOL (WINAPI  *TM1ValObjectCanRead)( TM1U hUser, TM1V vObject );
PRE TM1_BOOL (WINAPI  *TM1ValObjectCanWrite)( TM1U hUser, TM1V vObject );
PRE void (WINAPI  *TM1ValObjectGet)( TM1U hUser, TM1V vObject, TM1_OBJECT * pObject );
PRE void (WINAPI  *TM1ValObjectSet)( TM1V vObject, TM1_OBJECT * pObject );
PRE TM1_INDEX (WINAPI  *TM1ValObjectType)( TM1U hUser, TM1V vObject );
PRE TM1_INDEX (WINAPI  *TM1ValPoolCount)( TM1P hPool );
PRE TM1P (WINAPI  *TM1ValPoolCreate)( TM1U hUser );
PRE void (WINAPI  *TM1ValPoolDestroy)( TM1P hPool );
PRE TM1V (WINAPI  *TM1ValPoolGet)( TM1P hPool, TM1_INDEX Index );
PRE unsigned long  (WINAPI  *TM1ValPoolMemory)( TM1P hPool );
PRE TM1V (WINAPI  *TM1ValReal)( TM1P hPool, TM1_REAL InitReal );
PRE TM1_REAL (WINAPI  *TM1ValRealGet)( TM1U hUser, TM1V vReal );
PRE void (WINAPI  *TM1ValRealSet)( TM1V vReal, TM1_REAL Real );

PRE int (WINAPI  *TM1IsStringType)( int type );
PRE TM1V (WINAPI  *TM1ValString)( TM1P hPool, CHAR * InitString, TM1_INDEX MaxSize );
PRE TM1V (WINAPI  *TM1ValStringW)( TM1P hPool, TM1_UTF16_T * InitString, TM1_INDEX MaxSize );
PRE TM1V (WINAPI  *TM1ValStringUTF8)( TM1P hPool, TM1_UTF8_T * InitString, TM1_INDEX MaxSize );

PRE TM1V (WINAPI  *TM1ValStringEncrypt)( TM1P hPool, CHAR * InitString, TM1_INDEX MaxSize );
PRE TM1V (WINAPI  *TM1ValStringEncryptW)( TM1P hPool, TM1_UTF16_T * InitString, TM1_INDEX MaxSize );
PRE TM1V (WINAPI  *TM1ValStringEncryptUTF8)( TM1P hPool, TM1_UTF8_T * InitString, TM1_INDEX MaxSize );

PRE CHAR * (WINAPI  *TM1ValStringGet)( TM1U hUser, TM1V vString );
PRE TM1_UTF16_T * (WINAPI  *TM1ValStringGetW)( TM1U hUser, TM1V vString );
PRE TM1_UTF8_T * (WINAPI  *TM1ValStringGetUTF8)( TM1U hUser, TM1V vString );

PRE TM1V (WINAPI  *TM1ValStringAsBytesW)( TM1P hPool, TM1_UTF16_T *InitString );
PRE TM1V (WINAPI  *TM1ValStringAsBytesUTF8)( TM1P hPool, TM1_UTF8_T *InitString );

PRE TM1_BYTE_T * (WINAPI  *TM1ValBytesGet)( TM1U hUser, TM1V vBytes, TM1_INDEX *dwSize );

PRE TM1_INDEX (WINAPI  *TM1ValStringMaxSize)( TM1U hUser, TM1V vString );
PRE TM1_INDEX (WINAPI  *TM1ValStringWMaxSize)( TM1U hUser, TM1V vString );
PRE TM1_INDEX (WINAPI  *TM1ValStringUTF8MaxSize)( TM1U hUser, TM1V vString );
PRE TM1_INDEX (WINAPI  *TM1ValTypeIsString)( TM1U hUser, TM1V Value );
PRE TM1_INDEX (WINAPI  *TM1ValTypeIsBinary)( TM1U hUser, TM1V Value );
PRE void (WINAPI  *TM1ValStringSet)( TM1V vString, char *String );
PRE void (WINAPI  *TM1ValStringSetW)( TM1V vString, TM1_UTF16_T *utf16String );
PRE void (WINAPI  *TM1ValStringSetUTF8)( TM1V vString, TM1_UTF8_T *utf16String );
PRE TM1_INDEX (WINAPI  *TM1ValType)( TM1U hUser, TM1V Value );
PRE TM1_INDEX (WINAPI  *TM1ValTypeEx)( TM1U hUser, TM1V Value );
#ifdef GEN_WIN
PRE TM1_INDEX (WINAPI  *TM1ValTypeWithMsgLoop)( TM1U hUser, TM1V Value, HWND cancel_Hwnd );
#endif
PRE TM1V (WINAPI  *TM1ViewArrayColumnsNof)( TM1P hPool, TM1V hView );
PRE TM1V (WINAPI  *TM1ViewArrayConstruct)( TM1P hPool, TM1V hView );
PRE TM1V (WINAPI  *TM1ViewArrayDestroy)( TM1P hPool, TM1V hView );
PRE TM1V (WINAPI  *TM1ViewArrayRowsNof)( TM1P hPool, TM1V hView );
PRE TM1V (WINAPI  *TM1ViewArrayValueGet)( TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow );
PRE TM1V (WINAPI  *TM1ViewArrayValuePickListGet)( TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow );
PRE TM1V (WINAPI  *TM1ViewArrayValuePickListByRangeGet)( TM1P hPool, TM1V hView, TM1V iRowStart, TM1V iColStart, TM1V iRowEnd, TM1V iColEnd );
PRE TM1V (WINAPI  *TM1ViewArrayValuePickListExists)( TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow );
PRE TM1V (WINAPI  *TM1ViewCreate)( TM1P hPool, TM1V hCube, TM1V hTitleSubsetArray, TM1V hColumnSubsetArray, TM1V hRowSubsetArray );
PRE TM1V (WINAPI  *TM1ViewExtractCreate)( TM1P hPool, TM1V hView );
PRE TM1V (WINAPI  *TM1ViewExtractDestroy)( TM1P hPool, TM1V hView );
PRE TM1V (WINAPI  *TM1ViewExtractGetNext)( TM1P hPool, TM1V hView );

PRE TM1_BOOL (WINAPI  *TM1CancelClientJob)( TM1U hUser, TM1V hServer );
PRE TM1_BOOL (WINAPI  *TM1UserKill)( TM1U hUser, TM1V hServer );

PRE TM1V (WINAPI  *TM1GetViewByName)( TM1P hPool, TM1V hServer, TM1V sCube, TM1V sViewName, TM1V bIsPrivate, TM1V iFlag);
PRE TM1V (WINAPI  *TM1GetViewByHandle)( TM1P hPool, TM1V hView, TM1V iFlag);
PRE TM1V (WINAPI  *TM1ViewArrayValueByRangeGet)( TM1P hPool, TM1V hView, TM1V iRowStart, TM1V iColStart, TM1V iRowEnd, TM1V iColEnd);
PRE TM1V (WINAPI  *TM1CubeListGet)( TM1P hPool, TM1V hServer, TM1V iFlag);
PRE TM1V (WINAPI  *TM1CubeListByNamesGet)( TM1P hPool, TM1V hServer, TM1V vCubeNames);
PRE TM1V (WINAPI  *TM1ViewListGet)( TM1P hPool, TM1V hCube, TM1V iFlag);
PRE TM1V (WINAPI  *TM1ViewListByNamesGet)( TM1P hPool, TM1V hCube, TM1V vViewNames, TM1V iFlag);
PRE TM1V (WINAPI  *TM1ServerDimensionListGet)( TM1P hPool, TM1V hServer, TM1V iFlag);
PRE TM1V (WINAPI  *TM1ServerDimensionListByNamesGet)( TM1P hPool, TM1V hServer, TM1V vDimensionNames);
PRE TM1V (WINAPI  *TM1CubeDimensionListGet)( TM1P hPool, TM1V hCube);
PRE TM1V (WINAPI  *TM1SubsetListGet)( TM1P hPool, TM1V hDimension, TM1V iFlag);
PRE TM1V (WINAPI  *TM1SubsetListByNamesGet)( TM1P hPool, TM1V hDimension, TM1V vSubsetNames, TM1V iFlag);
PRE TM1V (WINAPI  *TM1DimensionElementListByNamesGet)( TM1P hPool, TM1V hDimension, TM1V vElementNames, TM1V iFlag);
PRE TM1V (WINAPI  *TM1DimensionElementListByIndexGet)( TM1P hPool, TM1V hDimension, TM1V iBeginIndex, TM1V iCount, TM1V iFlag);
PRE TM1V (WINAPI  *TM1SubsetElementListByIndexGet)( TM1P hPool, TM1V hSubset, TM1V iBeginIndex, TM1V iCount);
PRE TM1V (WINAPI  *TM1SubsetElementListByIndexGetEx)( TM1P hPool, TM1V hSubset, TM1V iBeginIndex, TM1V iCount, TM1V vDimName);
PRE TM1V (WINAPI  *TM1SubsetElementListByNamesGet)( TM1P hPool, TM1V hSubset, TM1V vElementNames);
PRE TM1V (WINAPI  *TM1ViewCellsValueGet)( TM1P hPool, TM1V hView, TM1V vArrayOfCells);
PRE TM1V (WINAPI  *TM1CubeCellsValueGet)( TM1P hPool, TM1V hCube, TM1V vArrayOfCells);
PRE TM1V (WINAPI  *TM1CubeCellsValueSet)( TM1P hPool, TM1V hCube, TM1V vArrayOfCells, TM1V vValues);
PRE TM1V (WINAPI  *TM1GetSubsetByHandle)( TM1P hPool, TM1V hSubset );
PRE TM1V (WINAPI  *TM1ElementComponentsGet)( TM1P hPool, TM1V hElement, TM1V vAliasName, TM1V iStartIndex, TM1V iCount );
PRE TM1V (WINAPI  *TM1ElementComponentsGetEx)( TM1P hPool, TM1V hElement, TM1V vAliasName, TM1V iStartIndex, TM1V iCount, TM1V vDimName );
PRE TM1V (WINAPI  *TM1DimensionAttributesGet)( TM1P hPool, TM1V hDimension );

 #ifdef GEN_WIN
// I think this one we can just over-write - .NET only
PRE void  (WINAPI  *TM1ValStringGet_CSH)( TM1U hUser, TM1V vString, TCHAR ** string  );
#endif

PRE TM1V (WINAPI  *TM1ObjectAttributeValuesSet)( TM1P hPool, TM1V hAttribute, TM1V hObjects, TM1V vValues );
//*********************************************************************
//  Constant Functions
PRE TM1_INDEX (WINAPI  *TM1ValTypeReal)( void );   //1
PRE TM1_INDEX (WINAPI  *TM1ValTypeString)( void ); //2
PRE TM1_INDEX (WINAPI  *TM1ValTypeStringW)( void );//14
PRE TM1_INDEX (WINAPI  *TM1ValTypeIndex)( void );  //3
PRE TM1_INDEX (WINAPI  *TM1ValTypeBool)( void );   //4
PRE TM1_INDEX (WINAPI  *TM1ValTypeObject)( void ); //5
PRE TM1_INDEX (WINAPI  *TM1ValTypeError)( void );  //6
PRE TM1_INDEX (WINAPI  *TM1ValTypeArray)( void );  //7
PRE TM1_INDEX (WINAPI  *TM1ValTypeBinary)( void );
PRE TM1V (WINAPI  *TM1ArrayNull)( void );
PRE TM1V (WINAPI  *TM1ObjectNull)( void );
PRE TM1V (WINAPI  *TM1CubeCellValueUndefined)( void );
PRE TM1V (WINAPI  *TM1Entry)( void);         
PRE TM1V (WINAPI  *TM1TypeAttribute)( void );  //18
PRE TM1V (WINAPI  *TM1TypeBlob)( void );       //10
PRE TM1V (WINAPI  *TM1TypeClient)( void );     //1
PRE TM1V (WINAPI  *TM1TypeChore)( void );      //15
PRE TM1V (WINAPI  *TM1TypeConnection)( void ); //6
PRE TM1V (WINAPI  *TM1TypeCube)( void );       //8
PRE TM1V (WINAPI  *TM1TypeDimension)( void );  //4
PRE TM1V (WINAPI  *TM1TypeElement)( void );    //20
PRE TM1V (WINAPI  *TM1TypeGroup)( void );      //2
PRE TM1V (WINAPI  *TM1TypeProcess)( void );    //16
PRE TM1V (WINAPI  *TM1TypeRule)( void );       //12
PRE TM1V (WINAPI  *TM1TypeSubset)( void );     //7
PRE TM1V (WINAPI  *TM1TypeServer)( void );     //3
PRE TM1V (WINAPI  *TM1TypeSQLNotSupported)( ); //39
PRE TM1V (WINAPI  *TM1TypeSQLNumericColumn)( );//37
PRE TM1V (WINAPI  *TM1TypeSQLStringColumn)( ); //38
PRE TM1V (WINAPI  *TM1TypeSQLTable)( );        //36
PRE TM1V (WINAPI  *TM1TypeView)( void );       //9
PRE TM1V (WINAPI  *TM1TypeElementSimple)( void );//21
PRE TM1V (WINAPI  *TM1TypeElementConsolidated)( void );//22
PRE TM1V (WINAPI  *TM1TypeElementString)( void );//23
PRE TM1V (WINAPI  *TM1TypeAttributeAlias)( void );//24
PRE TM1V (WINAPI  *TM1TypeAttributeString)( void );//25
PRE TM1V (WINAPI  *TM1TypeAttributeNumeric)( void );//26
PRE TM1V (WINAPI  *TM1TypeRuleCalculation)( void );//13
PRE TM1V (WINAPI  *TM1TypeRuleDrill)( void );//14
PRE TM1V (WINAPI  *TM1ObjectAttributes)( void );      
PRE TM1V (WINAPI  *TM1ObjectChangedSinceLoaded)( void );                         
PRE TM1V (WINAPI  *TM1ObjectLastTimeUpdated)( void ); 
PRE TM1V (WINAPI  *TM1ObjectInstanceVersion)( void ); 
PRE TM1V (WINAPI  *TM1ObjectMemoryUsed)( void );      
PRE TM1V (WINAPI  *TM1ObjectName)( void );            
PRE TM1V (WINAPI  *TM1ObjectParent)( void );          
PRE TM1V (WINAPI  *TM1ObjectSecurityStatus)( void );  
PRE TM1V (WINAPI  *TM1ObjectSecurityOwner)( void );   
PRE TM1V (WINAPI  *TM1ObjectType)( void );            
PRE TM1V (WINAPI  *TM1ObjectRegistration)( void );
PRE TM1V (WINAPI  *TM1AttributeType)( void );
PRE TM1V (WINAPI  *TM1ClientPassword)( void );                            
PRE TM1V (WINAPI  *TM1ChoreActive)( void );
PRE TM1V (WINAPI  *TM1ChoreFrequency)( void );
PRE TM1V (WINAPI  *TM1ChoreStartTime)( void );
PRE TM1V (WINAPI  *TM1ChoreSteps)( void );
PRE TM1V (WINAPI  *TM1ChoreExecutionMode)( void );
PRE TM1V (WINAPI  *TM1ClientStatus)( void ); 
PRE TM1V (WINAPI  *TM1ServerBuildNumber)( void );
PRE TM1V (WINAPI  *TM1ServerClients)( void );                           
PRE TM1V (WINAPI  *TM1ServerDirectories)( void );  
PRE TM1V (WINAPI  *TM1ServerDimensions)( void );    
PRE TM1V (WINAPI  *TM1ServerGroups)( void );                            
PRE TM1V (WINAPI  *TM1ServerCubes)( void );         
PRE TM1V (WINAPI  *TM1ServerBlobs)( void );
PRE TM1V (WINAPI  *TM1ServerLogDirectory)( void );   
PRE TM1V (WINAPI  *TM1ServerNetworkAddress)( void );
PRE TM1V (WINAPI  *TM1ServerChores)( void );
PRE TM1V (WINAPI  *TM1ServerProcesses)( void ); 
PRE TM1V (WINAPI  *TM1ServerConnections)( void );                      
PRE TM1V (WINAPI  *TM1ServerProcessObjectsSupported)( void );
PRE TM1V (WINAPI  *TM1ServerProgressMessageOn)( void );
PRE TM1V (WINAPI  *TM1ServerParallelInteraction)( void );
PRE TM1V (WINAPI  *TM1ServerCalculationCachePartitioning)( void );
PRE TM1V (WINAPI  *TM1ServerSession)( void );
PRE TM1V (WINAPI  *TM1BlobSize)( void );
PRE TM1V (WINAPI  *TM1ConnectionChoresUsing)( void ); 
PRE TM1V (WINAPI  *TM1ConnectionLastSyncTime)( void ); 
PRE TM1V (WINAPI  *TM1ConnectionLastSyncTimeStar)( void );
PRE TM1V (WINAPI  *TM1ConnectionLastSyncStarRecord)( void );         
PRE TM1V (WINAPI  *TM1ConnectionSyncErrorCount)( void ); 
PRE TM1V (WINAPI  *TM1ConnectionSyncPlanetToStar)( void ); 
PRE TM1V (WINAPI  *TM1ConnectionSyncStarToPlanet)( void ); 
PRE TM1V (WINAPI  *TM1ConnectionUserName)( void );          
PRE TM1V (WINAPI  *TM1ConnectionUseIntegratedSecurity)( void );          
PRE TM1V (WINAPI  *TM1ConnectionNamespace)( void );          
PRE TM1V (WINAPI  *TM1DimensionElements)( void );     
PRE TM1V (WINAPI  *TM1DimensionNofLevels)( void );    
PRE TM1V (WINAPI  *TM1DimensionSubsets)( void );      
PRE TM1V (WINAPI  *TM1DimensionCubesUsing)( void );      
PRE TM1V (WINAPI  *TM1DimensionWidth)( void );        
PRE TM1V (WINAPI  *TM1DimensionReplicationSyncSubsets)( void );
PRE TM1V (WINAPI  *TM1ElementIndex)( void );          
PRE TM1V (WINAPI  *TM1ElementLevel)( void );          
PRE TM1V (WINAPI  *TM1ElementType)( void );           
PRE TM1V (WINAPI  *TM1ElementComponents)( void );     
PRE TM1V (WINAPI  *TM1ElementParents)( void );        
PRE TM1V (WINAPI  *TM1RuleNofLines)( void );
PRE TM1V (WINAPI  *TM1RuleErrorLine)( void );
PRE TM1V (WINAPI  *TM1RuleErrorString)( void );
PRE TM1V (WINAPI  *TM1SubsetElements)( void ); 
PRE TM1V (WINAPI  *TM1SubsetSourceName)( void);
PRE TM1V (WINAPI  *TM1SubsetAlias)( void );   
PRE TM1V (WINAPI  *TM1SubsetExpression)( void );                            
PRE TM1V (WINAPI  *TM1CubeDimensions)( void );       
PRE TM1V (WINAPI  *TM1CubeLogChanges)( void );                           
PRE TM1V (WINAPI  *TM1CubeRule)( void );                                 
PRE TM1V (WINAPI  *TM1CubeViews)( void ); 
PRE TM1V (WINAPI  *TM1CubePerspectivesMaxMemory)( void );
PRE TM1V (WINAPI  *TM1CubePerspectivesMinTime)( void );
PRE TM1V (WINAPI  *TM1CubeMeasuresDimension)( void );
PRE TM1V (WINAPI  *TM1CubeReplicationSyncRule)( void );
PRE TM1V (WINAPI  *TM1CubeReplicationSyncViews)( void );
PRE TM1V (WINAPI  *TM1CubeTimeDimension)( void );
PRE TM1V (WINAPI  *TM1ViewColumnSubsets)( void );                         
PRE TM1V (WINAPI  *TM1CubeCellSpreadFunctionOk)( void );
PRE TM1V (WINAPI  *TM1CubeCellSpreadNumericSetOk)( void );
PRE TM1V (WINAPI  *TM1CubeCellSpreadStringSetOk)( void );
PRE TM1V (WINAPI  *TM1CubeCellSpreadStatusHeld)( void );
PRE TM1V (WINAPI  *TM1CubeCellSpreadStatusHeldConsolidation)( void );
PRE TM1V (WINAPI  *TM1CubeCellSpreadStatusWritable)( void );
PRE TM1V (WINAPI  *TM1CubeTimeLastInvalidated)( void );
PRE TM1V (WINAPI  *TM1CubeDataReservationMode)( void );
PRE TM1V (WINAPI  *TM1CubeCellSecurityDefaultValue)( void );
PRE TM1V (WINAPI  *TM1CubeCalculationThreshold)( void );
PRE TM1V (WINAPI  *TM1ViewPreConstruct)( void );                          
PRE TM1V (WINAPI  *TM1ViewRowSubsets)( void );                            
PRE TM1V (WINAPI  *TM1ViewSuppressZeroes)( void );                        
PRE TM1V (WINAPI  *TM1ViewTitleElements)( void );                         
PRE TM1V (WINAPI  *TM1ViewTitleSubsets)( void );                          
PRE TM1V (WINAPI  *TM1ViewFormat)( void );                                
PRE TM1V (WINAPI  *TM1ViewShowAutomatically)( void ); 
PRE TM1V (WINAPI  *TM1ViewArrayCellOrdinal)( void );                          
PRE TM1V (WINAPI  *TM1ViewArrayCellValue)( void );                                
PRE TM1V (WINAPI  *TM1ViewArrayCellFormattedValue)( void );                     
PRE TM1V (WINAPI  *TM1ViewArrayCellFormatString)( void );                     
PRE TM1V (WINAPI  *TM1ViewArrayMemberName)( void );                                
PRE TM1V (WINAPI  *TM1ViewArrayMemberType)( void );                     
PRE TM1V (WINAPI  *TM1ViewArrayMemberDescription)( void );                     
PRE TM1V (WINAPI  *TM1SecurityRightNone)( void );
PRE TM1V (WINAPI  *TM1SecurityRightRead)( void );
PRE TM1V (WINAPI  *TM1SecurityRightWrite)( void );
PRE TM1V (WINAPI  *TM1SecurityRightReserve)( void );
PRE TM1V (WINAPI  *TM1SecurityRightLock)( void );
PRE TM1V (WINAPI  *TM1SecurityRightAdmin)( void );
PRE TM1V (WINAPI  *TM1ObjectPublic)( void );
PRE TM1V (WINAPI  *TM1ObjectPrivate)( void );
PRE TM1V (WINAPI  *TM1ObjectUnregistered)( void );
PRE TM1V (WINAPI  *TM1ObjectReplicationConnection)( void );
PRE TM1V (WINAPI  *TM1ObjectLastRepSyncChangedTime)( void );
PRE TM1V (WINAPI  *TM1ObjectReplicationSourceObjectName)( void );
PRE TM1V (WINAPI  *TM1ObjectReplicationStatus)( void );
PRE TM1V (WINAPI  *TM1ProcessChoresUsing)( void );
PRE TM1V (WINAPI  *TM1ProcessComplete)( void );
PRE TM1V (WINAPI  *TM1ProcessDataProcedure)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceASCIIDecimalSeparator)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceASCIIDelimiter)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceASCIIHeaderRecords)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceASCIIQuoteCharacter)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceASCIIThousandSeparator)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceCubeView)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceDimensionSubset)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceNameForClient)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceNameForServer)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceOleDbLocation)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceOleDbMdp)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourcePassword)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceQuery)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceType)( void );
PRE TM1V (WINAPI  *TM1ProcessDataSourceUserName)( void );
PRE TM1V (WINAPI  *TM1ProcessEpilogProcedure)( void );
PRE TM1V (WINAPI  *TM1ProcessGrantSecurityAccess)( void );
PRE TM1V (WINAPI  *TM1ProcessMetaDataProcedure)( void );
PRE TM1V (WINAPI  *TM1ProcessParametersDefaultValues)( void );
PRE TM1V (WINAPI  *TM1ProcessParametersNames)( void );
PRE TM1V (WINAPI  *TM1ProcessParametersPromptStrings)( void );
PRE TM1V (WINAPI  *TM1ProcessParametersTypes)( void );
PRE TM1V (WINAPI  *TM1ProcessPrologProcedure)( void );
PRE TM1V (WINAPI  *TM1ProcessUIData)( void );
PRE TM1V (WINAPI  *TM1ProcessVariablesEndingBytes)( void );
PRE TM1V (WINAPI  *TM1ProcessVariablesNames)( void );
PRE TM1V (WINAPI  *TM1ProcessVariablesPositions)( void );
PRE TM1V (WINAPI  *TM1ProcessVariablesStartingBytes)( void );
PRE TM1V (WINAPI  *TM1ProcessVariablesTypes)( void );
PRE TM1V (WINAPI  *TM1ProcessVariablesUIData)( void );
PRE TM1V (WINAPI  *TM1SQLTableColumnNames)( );
PRE TM1V (WINAPI  *TM1SQLTableColumnTypes)(  );
PRE TM1V (WINAPI  *TM1SQLTableNumberOfColumns)(  );
PRE TM1V (WINAPI  *TM1SQLTableNumberOfRows)( );
PRE TM1V (WINAPI  *TM1SQLTableRowsetSize)( );
PRE TM1_INDEX (WINAPI  *TM1ErrorBlobCloseFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorBlobCreateFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorBlobGetFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorBlobNotOpen)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorBlobOpenFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorBlobPutFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorClientPasswordNotDefined)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorClientAlreadyExists)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellValueTypeMismatch)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusCubeNoWriteAccess)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusCubeLocked)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusCubeReserved)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusElementIsConsolidated)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusElementLocked)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusElementNoWriteAccess)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusElementReserved)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusRuleApplies)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusNoReservation)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCellWriteStatusCellReserved)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeCreationFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeDimensionInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeDrillNotFound)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeDrillInvalidStructure)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeKeyInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeNotEnoughDimensions)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeNumberOfKeysInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubePerspectiveAllSimpleElements)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubePerspectiveCreationFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeTooManyDimensions)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDataSpreadFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionCouldNotBeCompiled)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionElementAlreadyExists)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionElementComponentAlreadyExists)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionElementComponentDoesNotExist)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionElementComponentNotNumeric)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionElementDoesNotExist)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionElementNotConsolidated)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionHasCircularReferences)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionHasNoElements)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionIsBeingUsedByCube)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionNotChecked)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorGroupAlreadyExists)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorGroupMaximunNumberExceeded)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectAttributeNotDefined)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectAttributeInvalidType)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectDeleted)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectDuplicationFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectFileNotFound)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectFunctionDoesNotApply)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectHandleInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectHasNoParent)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectIndexInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectFileInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectIncompatibleTypes)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectIsRegistered)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectIsUnregistered)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectListIsEmpty)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectNameExists)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectNameIsBlank)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectNameInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectNotFound)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectNotLoaded)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectPropertyIsList)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectPropertyNotDefined)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectPropertyNotList)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectRegistrationFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectSecurityIsLocked)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectSecurityNoAdminRights)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectSecurityNoLockRights)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectSecurityNoReadRights)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectSecurityNoReserveRights)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorObjectSecurityNoWriteRights)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorRuleCubeHasRuleAttached)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorRuleIsAttached)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorRuleIsNotChecked)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorRuleLineNotFound)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSubsetIsBeingUsedByView)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemFunctionObsolete)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemServerClientAlreadyConnected)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemServerClientNotConnected)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemServerClientNotFound)( void );
PRE TM1_INDEX (WINAPI  *SystemServerConnectionFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemServerClientPasswordInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemServerNotFound)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemOutOfMemory)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemUserHandleInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemValueInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemParameterTypeInvalid)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorViewHasPrivateSubsets)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorViewNotConstructed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionNotRegistered)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorViewExpressionEmpty)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorDimensionUpdateFailedInvalidHierarchies)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorUpdateNonLeafCellValueFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeNoTimeDimension)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCubeMeasuresAndTimeDimensionSame)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorAuditLogResultSetDoesNotExist)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorAuditLogResultSetInvalidRange)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorAuditLogRecordDoesNotExist)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorAuditLogResultSetOutOfMemory)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorExecutingAuditLogQuery)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorValueNotInPickList)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorServerInBulkLoadMode)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorTM1PATHEnvVarNotFound)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCannotCreateAlternateHierarchyFromAlternateHierarchy)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemServerClientConnectFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressMessageOpening)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressMessageRunning)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressMessageClosing)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressTypePercent)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressTypeCounter)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionLoadingCube)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionLoadingDimension)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionRunningQuery)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionCalculatingView)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionLoadingSubset)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionSavingSubset)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionSortingSubset)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionCalculatingSubsetAll)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionInsertingSubset)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionDuplicatingSubset)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionCalculatingSubsetHierarchy)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionSelectingSubsetElements)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionDeletingSelection)( void );
PRE TM1_INDEX (WINAPI  *TM1ProgressActionKeepingSelection)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorClientAddedWithErrors)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorGroupAddedWithErrors)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorInvalidCapabilityFeature)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorInvalidCapabilityPermission)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorInvalidCapabilityPolicy)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorUpdateNotReady)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorNoUpdateToProcess)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorTUnitRedefined)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorChoreModifiedDuringExecution)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorChoreDeleted)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonNone)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonEQ_A)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonGE_A)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonLE_A)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonGT_A)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonLT_A)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonNE_A)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonGE_A_LE_B)( void );
PRE TM1V (WINAPI  *TM1ViewExtractComparisonGT_A_LT_B)( void );
PRE TM1V (WINAPI  *TM1ViewExtractSkipConsolidatedValues)( void );           
PRE TM1V (WINAPI  *TM1ViewExtractSkipRuleValues)( void );           
PRE TM1V (WINAPI  *TM1ViewExtractSkipZeroes)( void );                     
PRE TM1V (WINAPI  *TM1ViewExtractRealLimitA)( void );                     
PRE TM1V (WINAPI  *TM1ViewExtractRealLimitB)( void );                     
PRE TM1V (WINAPI  *TM1ViewExtractStringLimitA)( void );                   
PRE TM1V (WINAPI  *TM1ViewExtractStringLimitB)( void );                   
PRE TM1V (WINAPI  *TM1ViewExtractComparison)( void );   
PRE TM1V (WINAPI  *TM1ClientAddEx)( TM1P hPool, TM1V hServer, TM1V sClientName, TM1V sClientDefDisplayValue );
PRE TM1V (WINAPI  *TM1ObjAliasControlValueGet)( TM1P hPool, TM1V hObject, TM1V hType );
PRE TM1_INDEX (WINAPI  *TM1ErrorSystemServerCAMSecurityRequired)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCAMDllLoadFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorCAMObjectCreateFailed)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorControlAliasNotFound)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorControlAliasInvalidType)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorControlAliasInvalidValueType)( void );
PRE TM1V (WINAPI  *TM1GroupAddEx)( TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sGroupDefDisplayValue, TM1V sLangDisplayArr);
PRE TM1V (WINAPI  *TM1SecurityObject)( void );
PRE TM1V (WINAPI  *TM1RawstoreExists)( void );
PRE TM1V (WINAPI  *TM1AuditLogExists)( void );
PRE TM1V (WINAPI  *TM1EnableBulkLoadMode)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1DisableBulkLoadMode)( TM1P hPool, TM1V hServer );
PRE TM1V (WINAPI  *TM1ChangeSetBegin)( TM1P hPool, TM1V hServer);
PRE TM1V (WINAPI  *TM1ChangeSetEnd)( TM1P hPool, TM1V hServer);
PRE TM1V (WINAPI  *TM1ChangeSetUndo)( TM1P hPool, TM1V hServer, TM1V sChangeSetId);
PRE TM1V (WINAPI  *TM1ApplicationFolderContentsGet)( TM1P hPool, TM1V hFolder, TM1V iDepth);
PRE TM1V (WINAPI  *TM1DataReservationAcquire)( TM1P hPool, TM1V hCube, TM1V hClient, TM1V bForce, TM1V elementArray);
PRE TM1V (WINAPI  *TM1DataReservationRelease)( TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray);
PRE TM1V (WINAPI  *TM1DataReservationReleaseAll)( TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray);
PRE TM1V (WINAPI  *TM1DataReservationGetAll)( TM1P hPool, TM1V hCube, TM1V hClient);
PRE TM1V (WINAPI  *TM1DataReservationValidate)( TM1P hPool, TM1V hCube);
PRE TM1V (WINAPI  *TM1DataReservationGetConflicts)( TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray);
PRE TM1V (WINAPI  *TM1AssociateCAMIDToGroup)( TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sCAMID, TM1V sCAMIDDefDisplayValue, TM1V sLangDisplayArr);
PRE TM1V (WINAPI  *TM1RemoveCAMIDAssociationFromGroup)( TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sCAMID);
PRE TM1V (WINAPI  *TM1RemoveCAMIDAssociation)( TM1P hPool, TM1V hServer, TM1V sCAMID, TM1V bRemoveCAMID);
PRE TM1V (WINAPI  *TM1GetCAMIDsAssociatedWithGroup)( TM1P hPool, TM1V hServer, TM1V sGroupName);
PRE TM1V (WINAPI  *TM1GetGroupsAssociatedWithCAMID)( TM1P hPool, TM1V hServer, TM1V sCAMID);
PRE TM1_INDEX (WINAPI  *TM1ErrorAssociateCAMIDToGroup)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorRemoveCAMIDAssociationFromGroup)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorRemoveCAMIDAssociation)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorGetCAMIDsAssociatedWithGroup)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorGetGroupsAssociatedWithCAMID)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorGroupAssociationNotFound)( void );
PRE TM1V (WINAPI  *TM1CreateExpressionBasedView)( TM1P hPool, TM1V hServer, TM1V sExpression );
PRE TM1V (WINAPI  *TM1CubeExpressionBasedViews)( void );
PRE TM1V (WINAPI  *TM1ViewMDXExpression)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorElementNameContainsMetacharacters)( void );
PRE TM1_INDEX (WINAPI  *TM1ErrorAccessDisabledByPolicy)( void );


//устаревшие функции
PRE TM1V (WINAPI  *TM1CubeCellDrillStringGet)( TM1P hPool, TM1V hCube, TM1V hArrayOfElements );
PRE TM1V (WINAPI  *TM1DimensionTopElement)( void );
PRE TM1V (WINAPI  *TM1ObjectReplication)( void );
PRE TM1V (WINAPI  *TM1ObjectReplicationSourceName)( void );
PRE TM1V (WINAPI  *TM1SubsetSubtract)( TM1P hPool, TM1V hSubsetA, TM1V hSubsetB );
#endif




