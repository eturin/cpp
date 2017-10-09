
/**********************************************************************
 * IBM Confidential
 * 
 * OCO Source Materials
 * 
 * IBM Cognos Products: TM1 Server
 * 
 * (C) Copyright IBM Corp. 2007, 2014
 * 
 * The source code for this program is not published or otherwise divested
 * of its trade secrets, irrespective of what has been deposited with the
 * U.S. Copyright Office.
**********************************************************************/

// 17-dec-04 jon Fix build warnings
// 09-Nov-04 jon Make 256 the standard number of dimensions


// basic Tm1 object definition, processed in the API and the server
#ifndef TM1_OBJECT_DEFINED
typedef struct TM1_OBJECT
{
   unsigned int major_ix;
   unsigned int minor_ix;
   unsigned char dbcon_ix;
   unsigned char otype;
   unsigned int connectionTimestampMillis;
} TM1_OBJECT;
#define TM1_OBJECT_DEFINED
#endif


#ifndef TM1_OBJECT_DEFINITION_ONLY   // {

#ifndef TM1_API_H
#define TM1_API_H

typedef enum{
   TRADITIONAL_SECURITY_MODE = 1,
   MIXED_SECURITY_MODE,
   INTEGRATED_SECURITY_MODE,
   CAM_SECURITY_MODE,
   CAM_SECURITY_MIXED_MODE,
   DISTRIBUTED_SECURITY_MODE,
   SECURITY_MODE_SIZE
}SecurityMode;

typedef enum
{
   OWNER_TYPE = 1,
   OWNER,
   EVENT_TYPE,
   EVENT,
   AFFECTED_OBJECT_TYPE,
   AFFECTED_OBJECT,
   AUDIT_QUERY_FILTER_TYPE_SIZE
} TM1AuditQueryFilterTypes;

typedef enum
{
   CLIENT_OWNER = 1,
   SCHEDULED_CHORE_OWNER,
   AUDIT_QUERY_OWNER_TYPE_SIZE
} TM1AuditQueryOwnerTypes;

typedef enum
{
   SYSTEM_WIDE_EVENT = 1,
   OBJECT_SPECIFIC_EVENT,
   AUDIT_QUERY_EVENT_TYPE_SIZE
} TM1AuditQueryEventTypes;

typedef enum
{
   TYPE_STATIC = 1,
   TYPE_SUBSET,
   TYPE_DIMENSION,
   TYPE_NONE
} TM1PickListTypes;

/***********************************************************************/
  
   // Uncomment the line which matches the system used

   // #define GEN_HP       // HP-UX (32bit)
   // #define GEN_HP64     // HP-UX 11 (64bit)
   // #define GEN_AIX      // IBM-AIX (32bit)
   // #define GEN_AIX64    // IBM-AIX (64bit)
   // #define GEN_LINUX    // Linux (32bit)
   // #define GEN_LINUX64  // Linux (64 bit)
   // #define GEN_SUN      // Sun Solaris (32bit)
   // #define GEN_SUN64    // Sun Solaris (64bit)
   // #define GEN_WIN16    // Micorsoft Windows 16bit
   // #define GEN_WIN32    // Micorsoft Windows 32bit
   // #define GEN_WIN64    // Micorsoft Windows 64bit
/***********************************************************************/


#ifndef TM1_GEN_64
#if defined(GEN_WIN64) || defined(GEN_HP64) || defined(GEN_AIX64) || defined(GEN_LINUX64) || defined(GEN_SUN64)
#define TM1_GEN_64
#endif 
#endif // TM1_GEN_64

#ifdef __cplusplus
	extern "C"	{
#endif // __cplusplus

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


//*********************************************************************
// TM1 Constants
#if BLD_16_DIMENSIONS
   #define TM1_MAXIMUM_DIMENSIONS     16
#else
   // default is 256 dimensions
   #define TM1_MAXIMUM_DIMENSIONS     256
#endif

//*********************************************************************
// TM1 Type Definitions
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


typedef TM1_INDEX    * TM1V;           // value handle
typedef void         * TM1U;           // user  handle 
typedef void         * TM1P;           // pool  handle   

typedef void (TM1API * TM1_HOOK)( TM1_INDEX message, TM1_INDEX action, TM1_INDEX param, CHAR * name );
typedef void (TM1API * TM1_HOOK_W)( TM1_INDEX message, TM1_INDEX action, TM1_INDEX param, TM1_UTF16_T * name );
typedef void (TM1API * TM1_HOOK_UTF8)( TM1_INDEX message, TM1_INDEX action, TM1_INDEX param, TM1_UTF8_T * name );

//*********************************************************************
// Functions
TM1IMPORT void TM1API TM1APIInitialize( void );
TM1IMPORT void TM1API TM1APIFinalize( void );
TM1IMPORT TM1V TM1API TM1BlobClose( TM1P hPool, TM1V hBlob );
TM1IMPORT TM1V TM1API TM1BlobCreate( TM1P hPool, TM1V hServer, TM1V sName );
TM1IMPORT TM1_INDEX TM1API TM1BlobGet( TM1U hUser, TM1V hBlob, TM1_INDEX x, TM1_INDEX n, CHAR * buf );

TM1IMPORT TM1V TM1API TM1BlobOpen( TM1P hPool, TM1V hBlob );
TM1IMPORT TM1V TM1API TM1BlobOpenEx( TM1P hPool, TM1V hBlob, TM1V bOverwrite );
TM1IMPORT TM1_INDEX TM1API TM1BlobPut( TM1U hUser, TM1V hBlob, TM1_INDEX x, TM1_INDEX n, CHAR * buf );

TM1IMPORT TM1V TM1API TM1CapabilityGetPolicy( TM1P hPool, TM1V hClient, TM1V vFeature, TM1V vPermission );
TM1IMPORT TM1V TM1API TM1CapabilityGet( TM1P hPool, TM1V hGroup, TM1V vFeature, TM1V vPermission );
TM1IMPORT TM1V TM1API TM1CapabilityGetFeatures( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1CapabilitySet( TM1P hPool, TM1V hGroup, TM1V vFeature, TM1V vPermission, TM1V vPolicy );
TM1IMPORT TM1V TM1API TM1CapabilityGetPermissions( TM1P hPool, TM1V hServer, TM1V vFeature );

TM1IMPORT TM1V TM1API TM1ChoreCreateEmpty( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1ChoreExecute( TM1P hPool, TM1V hChore );
TM1IMPORT TM1V TM1API TM1ClientAdd( TM1P hPool, TM1V hServer, TM1V sClientName );
TM1IMPORT TM1V TM1API TM1ClientGroupAssign( TM1P hPool, TM1V hClient, TM1V hGroup );
TM1IMPORT TM1V TM1API TM1ClientGroupIsAssigned( TM1P hPool, TM1V hClient, TM1V hGroup );
TM1IMPORT TM1V TM1API TM1ClientGroupRemove( TM1P hPool, TM1V hClient, TM1V hGroup );
TM1IMPORT TM1V TM1API TM1ClientHasHolds( TM1P hPool, TM1V hClient);
TM1IMPORT TM1V TM1API TM1ClientPasswordAssign( TM1P hPool, TM1V hClient, TM1V sPassword );
TM1IMPORT TM1V TM1API TM1ConnectionCheck( TM1P hPool, TM1V hConnection );
TM1IMPORT TM1V TM1API TM1ConnectionCreate( TM1P hPool, TM1V hServer, TM1V sMasterServerName, TM1V sUsername, TM1V sPassword );
TM1IMPORT TM1V TM1API TM1ConnectionCreateEx( TM1P hPool, TM1V hServer, TM1V sMasterServerName, TM1V sUsername, TM1V sPassword, TM1V sNamespace, TM1V bUseIntegratedSecurity );
TM1IMPORT TM1V TM1API TM1ConnectionDelete( TM1P hPool, TM1V hConnection );
TM1IMPORT TM1V TM1API TM1ConnectionSynchronize( TM1P hPool, TM1V hConnection );
TM1IMPORT TM1V TM1API TM1CubeCellDrillListGet( TM1P hPool, TM1V hCube, TM1V hArrayOfKeys );
TM1IMPORT TM1V TM1API TM1CubeCellsDrillListGet( TM1P hPool, TM1V hCube, TM1V hArrayOfArraysOfKeys );
TM1IMPORT TM1V TM1API TM1CubeCellDrillObjectBuild( TM1P hPool, TM1V hCube, TM1V hArrayOfKeys, TM1V sDrillProcessName );
TM1IMPORT TM1V TM1API TM1CubeCellValueGet( TM1P hPool, TM1V hCube, TM1V hArrayOfElements );
TM1IMPORT TM1V TM1API TM1CubeCellPickListGet( TM1P hPool, TM1V hCube, TM1V hArrayOfElements );
TM1IMPORT TM1V TM1API TM1CubeCellsPickListGet( TM1P hPool, TM1V hCube, TM1V hArrayOfCells);
TM1IMPORT TM1V TM1API TM1CubeCellPickListExists( TM1P hPool, TM1V hCube, TM1V hArrayOfElements );
TM1IMPORT TM1V TM1API TM1PickListCubeCreate( TM1P hPool, TM1V hCube );
TM1IMPORT TM1V TM1API TM1PickListCubeExists( TM1P hPool, TM1V hCube );
TM1IMPORT TM1V TM1API TM1CubeCellValueSet( TM1P hPool, TM1V hCube, TM1V hArrayOfElements, TM1V hValue );
TM1IMPORT TM1V TM1API TM1CubeCellSpread( TM1P hPool, TM1V hServer, TM1V vArrayOfCells, TM1V vCellReference, TM1V sSpreadData );
TM1IMPORT TM1V TM1API TM1CubeCellSpreadStatusGet( TM1P hPool, TM1V hServer, TM1V hCells, TM1V hCellRange );
TM1IMPORT TM1V TM1API TM1CubeCellSpreadViewArray( TM1P hPool, TM1V hView, TM1V aCellRange, TM1V aCellRef, TM1V sControl );
TM1IMPORT TM1V TM1API TM1CubeCreate( TM1P hPool, TM1V hServer, TM1V hArrayOfDimensions );
TM1IMPORT TM1V TM1API TM1CubePerspectiveCreate( TM1P hPool, TM1V hCube, TM1V hArrayOfElementTitles ); 
TM1IMPORT TM1V TM1API TM1CubePerspectiveDestroy( TM1P hPool, TM1V hCube, TM1V hArrayOfElementTitles ); 
TM1IMPORT TM1V TM1API TM1CubeShowsNulls( TM1P hPool, TM1V hCube );
TM1IMPORT TM1V TM1API TM1CubeClearData( TM1P hPool, TM1V hCube );
TM1IMPORT TM1V TM1API TM1DimensionCheck( TM1P hPool, TM1V hDimension );
TM1IMPORT TM1V TM1API TM1DimensionCreateEmpty( TM1P hPool, TM1V hServer ); 
TM1IMPORT TM1V TM1API TM1DimensionElementComponentAdd( TM1P hPool, TM1V hElement, TM1V hComponent, TM1V rWeight );
TM1IMPORT TM1V TM1API TM1DimensionElementComponentDelete( TM1P hPool, TM1V hCElement, TM1V hElement );
TM1IMPORT TM1V TM1API TM1DimensionElementComponentWeightGet( TM1P hPool, TM1V hCElement, TM1V hElement );
TM1IMPORT TM1V TM1API TM1DimensionElementDelete( TM1P hPool, TM1V hElement );
TM1IMPORT TM1V TM1API TM1DimensionElementInsert( TM1P hPool, TM1V hDimension, TM1V hElementAfter, TM1V sName, TM1V vType ); 
TM1IMPORT TM1V TM1API TM1DimensionRootElementsGet( TM1P hPool, TM1V hDimension );
TM1IMPORT TM1V TM1API TM1DimensionRootSubsetGet( TM1P hPool, TM1V hDimension, TM1V iRight );
TM1IMPORT TM1V TM1API TM1DimensionElementParentsSubsetGet( TM1P hPool, TM1V hElement, TM1V iRight );
TM1IMPORT TM1V TM1API TM1DimensionElementChildrenSubsetGet( TM1P hPool, TM1V hElement, TM1V iRight );
TM1IMPORT TM1V TM1API TM1DimensionUpdate( TM1P hPool, TM1V hOldDimension, TM1V hNewDimension );
TM1IMPORT TM1V TM1API TM1GroupAdd( TM1P hPool, TM1V hServer, TM1V sGroupName );
TM1IMPORT TM1V TM1API TM1IsTICubeDimensionOrderMatch( TM1P hPool, TM1V hProcess, TM1V hCubeName );
TM1IMPORT TM1V TM1API TM1ObjectAttributeDelete( TM1P hPool, TM1V hObject, TM1V hAttribute );
TM1IMPORT TM1V TM1API TM1ObjectAttributeInsert( TM1P hPool, TM1V hObject, TM1V hAttributeBefore, TM1V sName, TM1V vType );
TM1IMPORT TM1V TM1API TM1ObjectAttributeValueGet( TM1P hPool, TM1V hObject, TM1V hAttribute );
TM1IMPORT TM1V TM1API TM1ObjectAttributeValueSet( TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V vValue );
TM1IMPORT TM1V TM1API TM1ObjectAttributeLocaleValueGet( TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V Locale );
TM1IMPORT TM1V TM1API TM1ObjectAttributeLocaleValueSet( TM1P hPool, TM1V hObject, TM1V hAttribute, TM1V Locale, TM1V vValue );
TM1IMPORT TM1V TM1API TM1ObjectCopy( TM1P hPool, TM1V hSrcObject, TM1V hDstObject );
TM1IMPORT TM1V TM1API TM1ObjectDelete( TM1P hPool, TM1V hObject );
TM1IMPORT TM1V TM1API TM1ObjectDestroy( TM1P hPool, TM1V hObject ); 
TM1IMPORT TM1V TM1API TM1ObjectDuplicate( TM1P hPool, TM1V hObject );
TM1IMPORT TM1V TM1API TM1ObjectFileDelete( TM1P hPool, TM1V hObject );
TM1IMPORT TM1V TM1API TM1ObjectFileLoad( TM1P hPool, TM1V hServer, TM1V hParent, TM1V iObjectType, TM1V sObjectName );
TM1IMPORT TM1V TM1API TM1ObjectFileSave( TM1P hPool, TM1V hObject );
TM1IMPORT TM1V TM1API TM1ObjectListCountGet( TM1P hPool, TM1V hObject, TM1V iPropertyList );
TM1IMPORT TM1V TM1API TM1ObjectListHandleByIndexGet( TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V iIndex );
TM1IMPORT TM1V TM1API TM1ObjectListHandleByNameGet( TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V sName );
TM1IMPORT TM1V TM1API TM1ObjectPrivateDelete( TM1P hPool, TM1V hObject );
TM1IMPORT TM1V TM1API TM1ObjectPrivateListCountGet( TM1P hPool, TM1V hObject, TM1V iPropertyList );
TM1IMPORT TM1V TM1API TM1ObjectPrivateListHandleByIndexGet( TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V iIndex );
TM1IMPORT TM1V TM1API TM1ObjectPrivateListHandleByNameGet( TM1P hPool, TM1V hObject, TM1V iPropertyList, TM1V sName );
TM1IMPORT TM1V TM1API TM1ObjectPrivatePublish( TM1P hPool, TM1V hObject, TM1V sName );
TM1IMPORT TM1V TM1API TM1ObjectPrivateRegister( TM1P hPool, TM1V hParent, TM1V hObject, TM1V sName );
TM1IMPORT TM1V TM1API TM1ObjectPropertyGet( TM1P hPool, TM1V hObject, TM1V vProperty );
TM1IMPORT TM1V TM1API TM1ObjectPropertySet( TM1P hPool, TM1V hObject, TM1V vProperty, TM1V vValue );
TM1IMPORT TM1V TM1API TM1ObjectSubPropertyByRangeGet( TM1P hPool, TM1V hRootObject, TM1V vObjectType, TM1V vPrivate, TM1V vStartIndex, TM1V vQuantity, TM1V vProperty);
TM1IMPORT TM1V TM1API TM1ObjectRegister( TM1P hPool, TM1V hParent, TM1V hObject, TM1V sName );
TM1IMPORT TM1V TM1API TM1ObjectReplicate( TM1P hPool, TM1V hObject );
TM1IMPORT TM1V TM1API TM1ObjectSecurityLock( TM1P hPool, TM1V hObject ); 
TM1IMPORT TM1V TM1API TM1ObjectSecurityRelease( TM1P hPool, TM1V hObject ); 
TM1IMPORT TM1V TM1API TM1ObjectSecurityReserve( TM1P hPool, TM1V hObject ); 
TM1IMPORT TM1V TM1API TM1ObjectSecurityRightGet( TM1P hPool, TM1V hObject, TM1V hGroup );
TM1IMPORT TM1V TM1API TM1ObjectSecurityRightSet( TM1P hPool, TM1V hObject, TM1V hGroup, TM1V iRight );
TM1IMPORT TM1V TM1API TM1ObjectSecurityUnLock( TM1P hPool, TM1V hObject ); 
TM1IMPORT TM1V TM1API TM1ProcessExecute( TM1P hPool, TM1V hProcess, TM1V hParametersArray );
TM1IMPORT TM1V TM1API TM1ProcessCheck( TM1P hPool, TM1V hProcess );
TM1IMPORT TM1V TM1API TM1ProcessCreateEmpty( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1ProcessExecuteEx( TM1P hPool, TM1V hProcess, TM1V hParametersArray );
TM1IMPORT TM1V TM1API TM1ProcessExecuteSQLQuery(TM1P tPool, TM1V hProcess, TM1V voDatabaseInfoArray);
TM1IMPORT TM1V TM1API TM1ProcessVariableNameIsValid( TM1P hPool, TM1V hProcess, TM1V hVariableName );
TM1IMPORT TM1V TM1API TM1RuleAttach( TM1P hPool, TM1V hRule);
TM1IMPORT TM1V TM1API TM1RuleCheck( TM1P hPool, TM1V hRule);
TM1IMPORT TM1V TM1API TM1RuleCreateEmpty( TM1P hPool, TM1V hCube, TM1V hType); 
TM1IMPORT TM1V TM1API TM1RuleDetach( TM1P hPool, TM1V hRule);
TM1IMPORT TM1V TM1API TM1RuleLineGet( TM1P hPool, TM1V hRule, TM1V iPosition);
TM1IMPORT TM1V TM1API TM1RuleLineInsert( TM1P hPool, TM1V hRule, TM1V iPosition, TM1V sLine);
TM1IMPORT TM1V TM1API TM1ServerBatchUpdateStart( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1ServerBatchUpdateIsActive( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1ServerBatchUpdateFinish( TM1P hPool, TM1V hServer, TM1V bDiscard );
TM1IMPORT TM1V TM1API TM1ServerLogClose( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1ServerLogNext( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1ServerLogOpen( TM1P hPool, TM1V hServer, TM1V sStartTime, TM1V sCubeFilter, TM1V sUserFilter, TM1V sFlagFilter );
TM1IMPORT TM1V TM1API TM1AuditLogQueryExecute( TM1P hPool, TM1V hServer, TM1V sStartTime, TM1V sEndTime, TM1V aAdditionalFilters );
TM1IMPORT TM1V TM1API TM1AuditLogResultSetRangeGet( TM1P hPool, TM1V hServer, TM1V iStartIndex, TM1V iEndIndex );
TM1IMPORT TM1V TM1API TM1AuditLogResultSetDestroy( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1AuditLogResultSetDetailsGet( TM1P hPool, TM1V hServer, TM1V iEventID );
TM1IMPORT TM1V TM1API TM1AuditLogResultSetDetailsRangeGet( TM1P hPool, TM1V hServer, TM1V iEventID, TM1V iStartIndex, TM1V iEndIndex );
TM1IMPORT TM1V TM1API TM1AuditLogResultSetDetailsDestroy( TM1P hPool, TM1V hServer, TM1V iEventID );
TM1IMPORT TM1V TM1API TM1AuditLogSystemEventsGet( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1AuditLogObjectEventsGet( TM1P hPool, TM1V hServer, TM1V iObjectType );
TM1IMPORT TM1V TM1API TM1AuditLogRawstoreRoll( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1ServerOpenSQLQuery(TM1P hPool, TM1V hServer, TM1V hDsnInfo);
TM1IMPORT TM1V TM1API TM1ServerPasswordChange( TM1P hPool, TM1V hServer, TM1V sNewPassword );
TM1IMPORT TM1V TM1API TM1ServerSecurityRefresh( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1SQLTableGetNextRows( TM1P hPool, TM1V hSQLTable, TM1V bColumnSelection );
TM1IMPORT TM1V TM1API TM1SubsetAll( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1SubsetRootElementsGet( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1SubsetCreateEmpty( TM1P hPool, TM1V hDim );
TM1IMPORT TM1V TM1API TM1SubsetElementDisplay( TM1P hPool, TM1V hSubset, TM1V iElement );
TM1IMPORT TM1_BOOL TM1API TM1SubsetElementDisplayEll( TM1U hUser, TM1V vString );
TM1IMPORT TM1_INDEX TM1API TM1SubsetElementDisplayLevel( TM1U hUser, TM1V vString );
TM1IMPORT TM1_BOOL TM1API TM1SubsetElementDisplayLine( TM1U hUser, TM1V vString, TM1_INDEX Index );
TM1IMPORT TM1_BOOL TM1API TM1SubsetElementDisplayMinus( TM1U hUser, TM1V vString );
TM1IMPORT TM1_BOOL TM1API TM1SubsetElementDisplayPlus( TM1U hUser, TM1V vString );
TM1IMPORT TM1_BOOL TM1API TM1SubsetElementDisplaySelection( TM1U hUser, TM1V vString );
TM1IMPORT TM1_BOOL TM1API TM1SubsetElementDisplayTee( TM1U hUser, TM1V vString );
TM1IMPORT TM1_REAL TM1API TM1SubsetElementDisplayWeight( TM1U hUser, TM1V vString );
TM1IMPORT TM1V TM1API TM1SubsetInsertElement( TM1P hPool, TM1V hSubset, TM1V hElement, TM1V iPosition ); 
TM1IMPORT TM1V TM1API TM1SubsetInsertSubset( TM1P hPool, TM1V hSubsetA, TM1V hSubsetB, TM1V iPosition ); 
TM1IMPORT TM1V TM1API TM1SubsetSelectByAttribute( TM1P hPool, TM1V hSubset, TM1V hAlias, TM1V sValueToMatch, TM1V bSelection );
TM1IMPORT TM1V TM1API TM1SubsetSelectByIndex( TM1P hPool, TM1V hSubset, TM1V iPosition, TM1V bSelection );
TM1IMPORT TM1V TM1API TM1SubsetMultiSelectByIndex( TM1P hPool, TM1V hSubset, TM1V iPositionArr, TM1V bSelectionArr );
TM1IMPORT TM1V TM1API TM1SubsetSelectByLevel( TM1P hPool, TM1V hSubset, TM1V iLevel, TM1V bSelection );
TM1IMPORT TM1V TM1API TM1SubsetSelectByPattern( TM1P hPool, TM1V hSubset, TM1V sPattern, TM1V bSelection );
TM1IMPORT TM1V TM1API TM1SubsetSelectionDelete( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1SubsetSelectionInsertChildren( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1SubsetSelectionInsertParents( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1SubsetSelectionKeep( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1SubsetSelectNone( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1SubsetSort( TM1P hPool, TM1V hSubset, TM1V bSortDown );
TM1IMPORT TM1V TM1API TM1SubsetSortByHierarchy( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1SubsetUpdate( TM1P hPool, TM1V hOldSubset, TM1V hNewSubset );
TM1IMPORT TM1V TM1API TM1SubsetCreateByExpression( TM1P hPool, TM1V hServer, TM1V sExpression );

TM1IMPORT CHAR * TM1API TM1SystemAdminHostGet( TM1U hUser );
TM1IMPORT TM1_UTF16_T * TM1API TM1SystemAdminHostGetW( TM1U hUser );
TM1IMPORT TM1_UTF8_T * TM1API TM1SystemAdminHostGetUTF8( TM1U hUser );

TM1IMPORT void TM1API TM1SystemAdminHostSet( TM1U hUser, CHAR * szAdminHosts );
TM1IMPORT void TM1API TM1SystemAdminHostSetW( TM1U hUser, TM1_UTF16_T * szAdminHosts );
TM1IMPORT void TM1API TM1SystemAdminHostSetUTF8( TM1U hUser, TM1_UTF8_T * szAdminHosts );

TM1IMPORT TM1_BOOL TM1API TM1APISetSSLCertVersion( TM1U hUser, TM1_INDEX nCertificateVersion ); 

TM1IMPORT void TM1API TM1SystemSetAdminSSLCertAuthority( TM1U hUser, CHAR * szAdminSSLCertAuthority );
TM1IMPORT void TM1API TM1SystemSetAdminSSLCertAuthorityW( TM1U hUser, TM1_UTF16_T * szAdminSSLCertAuthority );
TM1IMPORT void TM1API TM1SystemSetAdminSSLCertAuthorityUTF8( TM1U hUser, TM1_UTF8_T * szAdminSSLCertAuthority );

TM1IMPORT CHAR * TM1API TM1SystemGetAdminSSLCertAuthority( TM1U hUser );
TM1IMPORT TM1_UTF16_T * TM1API TM1SystemGetAdminSSLCertAuthorityW( TM1U hUser );
TM1IMPORT TM1_UTF8_T * TM1API TM1SystemGetAdminSSLCertAuthorityUTF8( TM1U hUser );

TM1IMPORT int TM1API TM1ValidateSSLConfig( TM1U hUser );
TM1IMPORT void TM1API TM1SystemSetAdminSSLCertRevList( TM1U hUser, CHAR * szAdminSSLCertRevList );
TM1IMPORT void TM1API TM1SystemSetAdminSSLCertRevListW( TM1U hUser, TM1_UTF16_T * szAdminSSLCertRevList );
TM1IMPORT void TM1API TM1SystemSetAdminSSLCertRevListUTF8( TM1U hUser, TM1_UTF8_T * szAdminSSLCertRevList );

TM1IMPORT CHAR * TM1API TM1SystemGetAdminSSLCertRevList( TM1U hUser );
TM1IMPORT TM1_UTF16_T * TM1API TM1SystemGetAdminSSLCertRevListW( TM1U hUser );
TM1IMPORT TM1_UTF8_T * TM1API TM1SystemGetAdminSSLCertRevListUTF8( TM1U hUser );

TM1IMPORT void TM1API TM1SystemSetAdminSSLCertID( TM1U hUser, CHAR * szAdminSSLCertID );
TM1IMPORT void TM1API TM1SystemSetAdminSSLCertIDW( TM1U hUser, TM1_UTF16_T * szAdminSSLCertID );
TM1IMPORT void TM1API TM1SystemSetAdminSSLCertIDUTF8( TM1U hUser, TM1_UTF8_T * szAdminSSLCertID );

TM1IMPORT CHAR * TM1API TM1SystemGetAdminSSLCertID( TM1U hUser );
TM1IMPORT TM1_UTF16_T * TM1API TM1SystemGetAdminSSLCertIDW( TM1U hUser );
TM1IMPORT TM1_UTF8_T * TM1API TM1SystemGetAdminSSLCertIDUTF8( TM1U hUser );

TM1IMPORT void TM1API TM1SystemSetExportAdminSvrSSLCertFlag( TM1U hUser, TM1_BOOL bExportAdminSvrSSLCert );

TM1IMPORT void TM1API TM1SystemSetAdminSvrExportKeyID( TM1U hUser, CHAR * szAdminSvrExportKeyID );
TM1IMPORT void TM1API TM1SystemSetAdminSvrExportKeyIDW( TM1U hUser, TM1_UTF16_T * szAdminSvrExportKeyID );
TM1IMPORT void TM1API TM1SystemSetAdminSvrExportKeyIDUTF8( TM1U hUser, TM1_UTF8_T * szAdminSvrExportKeyID );

TM1IMPORT void TM1API TM1SystemSetSSLDirectory( TM1U hUser, CHAR * szSSLCertsDirectory );
TM1IMPORT void TM1API TM1SystemSetSSLDirectoryW( TM1U hUser, TM1_UTF16_T * szSSLCertsDirectory );
TM1IMPORT void TM1API TM1SystemSetSSLDirectoryUTF8( TM1U hUser, TM1_UTF8_T * szSSLCertsDirectory );

TM1IMPORT CHAR * TM1API TM1SystemGetSSLDirectory( TM1U hUser );
TM1IMPORT TM1_UTF16_T * TM1API TM1SystemGetSSLDirectoryW( TM1U hUser );
TM1IMPORT TM1_UTF8_T * TM1API TM1SystemGetSSLDirectoryUTF8( TM1U hUser );

TM1IMPORT CHAR * TM1API TM1SystemBuildNumber( void );
TM1IMPORT TM1_UTF16_T * TM1API TM1SystemBuildNumberW( void );
TM1IMPORT TM1_UTF8_T * TM1API TM1SystemBuildNumberUTF8( void );

TM1IMPORT void TM1API TM1SystemClose( TM1U hUser ); 
TM1IMPORT TM1U TM1API TM1SystemOpen( void ); 
TM1IMPORT void TM1API TM1SystemProgressHookSet( TM1U hUser, TM1_HOOK pHook );
TM1IMPORT void TM1API TM1SystemProgressHookSetW( TM1U hUser, TM1_HOOK_W pHook );
TM1IMPORT void TM1API TM1SystemProgressHookSetUTF8( TM1U hUser, TM1_HOOK_UTF8 pHook);
TM1IMPORT TM1_BOOL TM1API TM1IsProgressHookActive( TM1U hUser );

TM1IMPORT CHAR * TM1API TM1SystemServerClientName( TM1U hUser, unsigned index );
TM1IMPORT TM1_UTF16_T * TM1API TM1SystemServerClientNameW( TM1U hUser, unsigned index );
TM1IMPORT TM1_UTF8_T * TM1API TM1SystemServerClientNameUTF8( TM1U hUser, unsigned index );

TM1IMPORT TM1V TM1API TM1SystemServerConnect( TM1P hPool, TM1V sServerName, TM1V sClientName, TM1V sPassword );
TM1IMPORT TM1V TM1API TM1SystemServerLimitedConnect( TM1P hPool, TM1V sServerName );
TM1IMPORT TM1V TM1API TM1SystemServerConnectIntegratedLogin(TM1P hPool, TM1V sServerName);
TM1IMPORT TM1V TM1API TM1SystemServerDisconnect( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1SystemServerHandle( TM1U hUser, CHAR * szName );
TM1IMPORT TM1V TM1API TM1SystemServerHandleW( TM1U hUser, TM1_UTF16_T * szName );
TM1IMPORT TM1V TM1API TM1SystemServerHandleUTF8( TM1U hUser, TM1_UTF8_T * szName );
TM1IMPORT TM1V TM1API TM1SystemServerConnectWithCAMPassport( TM1P hPool, TM1V sServerName, TM1V camArgs );
TM1IMPORT TM1V TM1API TM1SystemServerConnectWithCAMNamespace( TM1P hPool, TM1V sServerName, TM1V camArgs );
TM1IMPORT TM1V TM1API TM1SystemGetServerConfig (TM1P hPool, TM1V sServerName );

TM1IMPORT CHAR * TM1API TM1SystemServerName( TM1U hUser, unsigned index );
TM1IMPORT TM1_UTF16_T * TM1API TM1SystemServerNameW( TM1U hUser, unsigned index );
TM1IMPORT TM1_UTF8_T * TM1API TM1SystemServerNameUTF8( TM1U hUser, unsigned index );

TM1IMPORT int TM1API TM1SystemServerNof( TM1U hUser );
TM1IMPORT void TM1API TM1SystemServerReload( TM1U hUser );
TM1IMPORT TM1_BOOL TM1API TM1SystemServerStart( TM1U hUser, CHAR * szName, CHAR * szDataDirectory, CHAR * szAdminHost, CHAR * szProtocol, int iPortNumber );
TM1IMPORT TM1_BOOL TM1API TM1SystemServerStartW( TM1U hUser, TM1_UTF16_T * szName, TM1_UTF16_T * szDataDirectory, TM1_UTF16_T * szAdminHost, TM1_UTF16_T * szProtocol, int iPortNumber );
TM1IMPORT TM1_BOOL TM1API TM1SystemServerStartUTF8( TM1U hUser, TM1_UTF8_T * szName, TM1_UTF8_T * szDataDirectory, TM1_UTF8_T * szAdminHost, TM1_UTF8_T * szProtocol, int iPortNumber );

TM1IMPORT TM1_BOOL TM1API TM1SystemServerStartEx( TM1U hUser, CHAR * szcmdline );
TM1IMPORT TM1_BOOL TM1API TM1SystemServerStartExW( TM1U hUser, TM1_UTF16_T * szcmdline );
TM1IMPORT TM1_BOOL TM1API TM1SystemServerStartExUTF8( TM1U hUser, TM1_UTF8_T * szcmdline );

TM1IMPORT TM1_BOOL TM1API TM1SystemServerStop( TM1U hUser, CHAR * szName, TM1_BOOL bSave );
TM1IMPORT TM1_BOOL TM1API TM1SystemServerStopW( TM1U hUser, TM1_UTF16_T * szName, TM1_BOOL bSave );
TM1IMPORT TM1_BOOL TM1API TM1SystemServerStopUTF8( TM1U hUser, TM1_UTF8_T * szName, TM1_BOOL bSave );

TM1IMPORT int TM1API TM1SystemVersionGet( void );
TM1IMPORT TM1V TM1API TM1ValArray( TM1P hPool, TM1V * InitArray, TM1_INDEX MaxSize );
TM1IMPORT TM1V TM1API TM1ValArrayGet( TM1U hUser, TM1V vArray, TM1_INDEX Index );
TM1IMPORT TM1_INDEX TM1API TM1ValArrayMaxSize( TM1U hUser, TM1V vArray );
TM1IMPORT void TM1API TM1ValArraySet( TM1V vArray, TM1V vValue, TM1_INDEX Index );
TM1IMPORT void TM1API TM1ValArraySetSize( TM1V vArray, TM1_INDEX Size );
TM1IMPORT TM1V TM1API TM1ValBool( TM1P hPool, TM1_BOOL InitBool );
TM1IMPORT TM1_BOOL TM1API TM1ValBoolGet( TM1U hUser, TM1V vBool );
TM1IMPORT void TM1API TM1ValBoolSet( TM1V vBool, TM1_BOOL Bool );
TM1IMPORT TM1_INDEX TM1API TM1ValErrorCode( TM1U hUser, TM1V vError );

TM1IMPORT CHAR * TM1API TM1ValErrorString( TM1U hUser, TM1V vValue );
TM1IMPORT TM1_UTF16_T * TM1API TM1ValErrorStringW( TM1U hUser, TM1V vValue );
TM1IMPORT TM1_UTF8_T * TM1API TM1ValErrorStringUTF8( TM1U hUser, TM1V vValue );

TM1IMPORT TM1V TM1API TM1ValIndex( TM1P hPool, TM1_INDEX InitIndex ); 
TM1IMPORT TM1_INDEX TM1API TM1ValIndexGet( TM1U hUser, TM1V vIndex );
TM1IMPORT void TM1API TM1ValIndexSet( TM1V vIndex, TM1_INDEX Index );
TM1IMPORT TM1_BOOL TM1API TM1ValIsUndefined( TM1U hUser, TM1V Value ); 
TM1IMPORT TM1_BOOL TM1API TM1ValIsUpdatable( TM1U hUser, TM1V Value );
TM1IMPORT TM1_BOOL TM1API TM1ValIsChanged( TM1U hUser, TM1V Value );

TM1IMPORT TM1V TM1API TM1ValObject( TM1P hPool, TM1_OBJECT * InitObject ); 
TM1IMPORT TM1_BOOL TM1API TM1ValObjectCanRead( TM1U hUser, TM1V vObject );
TM1IMPORT TM1_BOOL TM1API TM1ValObjectCanWrite( TM1U hUser, TM1V vObject );
TM1IMPORT void TM1API TM1ValObjectGet( TM1U hUser, TM1V vObject, TM1_OBJECT * pObject );
TM1IMPORT void TM1API TM1ValObjectSet( TM1V vObject, TM1_OBJECT * pObject );
TM1IMPORT TM1_INDEX TM1API TM1ValObjectType( TM1U hUser, TM1V vObject );
TM1IMPORT TM1_INDEX TM1API TM1ValPoolCount( TM1P hPool );
TM1IMPORT TM1P TM1API TM1ValPoolCreate( TM1U hUser );
TM1IMPORT void TM1API TM1ValPoolDestroy( TM1P hPool );
TM1IMPORT TM1V TM1API TM1ValPoolGet( TM1P hPool, TM1_INDEX Index );
TM1IMPORT unsigned long  TM1API TM1ValPoolMemory( TM1P hPool );
TM1IMPORT TM1V TM1API TM1ValReal( TM1P hPool, TM1_REAL InitReal ); 
TM1IMPORT TM1_REAL TM1API TM1ValRealGet( TM1U hUser, TM1V vReal );
TM1IMPORT void TM1API TM1ValRealSet( TM1V vReal, TM1_REAL Real );

TM1IMPORT int TM1API TM1IsStringType( int type );
TM1IMPORT TM1V TM1API TM1ValString( TM1P hPool, CHAR * InitString, TM1_INDEX MaxSize );
TM1IMPORT TM1V TM1API TM1ValStringW( TM1P hPool, TM1_UTF16_T * InitString, TM1_INDEX MaxSize );
TM1IMPORT TM1V TM1API TM1ValStringUTF8( TM1P hPool, TM1_UTF8_T * InitString, TM1_INDEX MaxSize );

TM1IMPORT TM1V TM1API TM1ValStringEncrypt( TM1P hPool, CHAR * InitString, TM1_INDEX MaxSize );
TM1IMPORT TM1V TM1API TM1ValStringEncryptW( TM1P hPool, TM1_UTF16_T * InitString, TM1_INDEX MaxSize );
TM1IMPORT TM1V TM1API TM1ValStringEncryptUTF8( TM1P hPool, TM1_UTF8_T * InitString, TM1_INDEX MaxSize );

TM1IMPORT CHAR * TM1API TM1ValStringGet( TM1U hUser, TM1V vString );
TM1IMPORT TM1_UTF16_T * TM1API TM1ValStringGetW( TM1U hUser, TM1V vString );
TM1IMPORT TM1_UTF8_T * TM1API TM1ValStringGetUTF8( TM1U hUser, TM1V vString );

TM1IMPORT TM1V TM1API TM1ValStringAsBytesW( TM1P hPool, TM1_UTF16_T *InitString );
TM1IMPORT TM1V TM1API TM1ValStringAsBytesUTF8( TM1P hPool, TM1_UTF8_T *InitString );

TM1IMPORT TM1_BYTE_T * TM1API TM1ValBytesGet( TM1U hUser, TM1V vBytes, TM1_INDEX *dwSize );

TM1IMPORT TM1_INDEX TM1API TM1ValStringMaxSize( TM1U hUser, TM1V vString );
TM1IMPORT TM1_INDEX TM1API TM1ValStringWMaxSize( TM1U hUser, TM1V vString );
TM1IMPORT TM1_INDEX TM1API TM1ValStringUTF8MaxSize( TM1U hUser, TM1V vString );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeIsString( TM1U hUser, TM1V Value );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeIsBinary( TM1U hUser, TM1V Value );
TM1IMPORT void TM1API TM1ValStringSet( TM1V vString, char *String );
TM1IMPORT void TM1API TM1ValStringSetW( TM1V vString, TM1_UTF16_T *utf16String );
TM1IMPORT void TM1API TM1ValStringSetUTF8( TM1V vString, TM1_UTF8_T *utf16String );
TM1IMPORT TM1_INDEX TM1API TM1ValType( TM1U hUser, TM1V Value );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeEx( TM1U hUser, TM1V Value );
#ifdef GEN_WIN
TM1IMPORT TM1_INDEX TM1API TM1ValTypeWithMsgLoop( TM1U hUser, TM1V Value, HWND cancel_Hwnd );
#endif
TM1IMPORT TM1V TM1API TM1ViewArrayColumnsNof( TM1P hPool, TM1V hView );
TM1IMPORT TM1V TM1API TM1ViewArrayConstruct( TM1P hPool, TM1V hView );
TM1IMPORT TM1V TM1API TM1ViewArrayDestroy( TM1P hPool, TM1V hView );
TM1IMPORT TM1V TM1API TM1ViewArrayRowsNof( TM1P hPool, TM1V hView );
TM1IMPORT TM1V TM1API TM1ViewArrayValueGet( TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow );
TM1IMPORT TM1V TM1API TM1ViewArrayValuePickListGet( TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow );
TM1IMPORT TM1V TM1API TM1ViewArrayValuePickListByRangeGet( TM1P hPool, TM1V hView, TM1V iRowStart, TM1V iColStart, TM1V iRowEnd, TM1V iColEnd );
TM1IMPORT TM1V TM1API TM1ViewArrayValuePickListExists( TM1P hPool, TM1V hView, TM1V iColumn, TM1V iRow );
TM1IMPORT TM1V TM1API TM1ViewCreate( TM1P hPool, TM1V hCube, TM1V hTitleSubsetArray, TM1V hColumnSubsetArray, TM1V hRowSubsetArray );
TM1IMPORT TM1V TM1API TM1ViewExtractCreate( TM1P hPool, TM1V hView );
TM1IMPORT TM1V TM1API TM1ViewExtractDestroy( TM1P hPool, TM1V hView );
TM1IMPORT TM1V TM1API TM1ViewExtractGetNext( TM1P hPool, TM1V hView );

TM1IMPORT TM1_BOOL TM1API TM1CancelClientJob( TM1U hUser, TM1V hServer );
TM1IMPORT TM1_BOOL TM1API TM1UserKill( TM1U hUser, TM1V hServer );

TM1IMPORT TM1V TM1API TM1GetViewByName(TM1P hPool, TM1V hServer, TM1V sCube, TM1V sViewName, TM1V bIsPrivate, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1GetViewByHandle(TM1P hPool, TM1V hView, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1ViewArrayValueByRangeGet(TM1P hPool, TM1V hView, TM1V iRowStart, TM1V iColStart, TM1V iRowEnd, TM1V iColEnd);
TM1IMPORT TM1V TM1API TM1CubeListGet(TM1P hPool, TM1V hServer, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1CubeListByNamesGet(TM1P hPool, TM1V hServer, TM1V vCubeNames);
TM1IMPORT TM1V TM1API TM1ViewListGet(TM1P hPool, TM1V hCube, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1ViewListByNamesGet(TM1P hPool, TM1V hCube, TM1V vViewNames, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1ServerDimensionListGet(TM1P hPool, TM1V hServer, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1ServerDimensionListByNamesGet(TM1P hPool, TM1V hServer, TM1V vDimensionNames);
TM1IMPORT TM1V TM1API TM1CubeDimensionListGet(TM1P hPool, TM1V hCube);
TM1IMPORT TM1V TM1API TM1SubsetListGet(TM1P hPool, TM1V hDimension, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1SubsetListByNamesGet(TM1P hPool, TM1V hDimension, TM1V vSubsetNames, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1DimensionElementListByNamesGet(TM1P hPool, TM1V hDimension, TM1V vElementNames, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1DimensionElementListByIndexGet(TM1P hPool, TM1V hDimension, TM1V iBeginIndex, TM1V iCount, TM1V iFlag);
TM1IMPORT TM1V TM1API TM1SubsetElementListByIndexGet(TM1P hPool, TM1V hSubset, TM1V iBeginIndex, TM1V iCount);
TM1IMPORT TM1V TM1API TM1SubsetElementListByIndexGetEx(TM1P hPool, TM1V hSubset, TM1V iBeginIndex, TM1V iCount, TM1V vDimName);
TM1IMPORT TM1V TM1API TM1SubsetElementListByNamesGet(TM1P hPool, TM1V hSubset, TM1V vElementNames);
TM1IMPORT TM1V TM1API TM1ViewCellsValueGet(TM1P hPool, TM1V hView, TM1V vArrayOfCells);
TM1IMPORT TM1V TM1API TM1CubeCellsValueGet(TM1P hPool, TM1V hCube, TM1V vArrayOfCells);
TM1IMPORT TM1V TM1API TM1CubeCellsValueSet(TM1P hPool, TM1V hCube, TM1V vArrayOfCells, TM1V vValues);
TM1IMPORT TM1V TM1API TM1GetSubsetByHandle( TM1P hPool, TM1V hSubset );
TM1IMPORT TM1V TM1API TM1ElementComponentsGet( TM1P hPool, TM1V hElement, TM1V vAliasName, TM1V iStartIndex, TM1V iCount );
TM1IMPORT TM1V TM1API TM1ElementComponentsGetEx( TM1P hPool, TM1V hElement, TM1V vAliasName, TM1V iStartIndex, TM1V iCount, TM1V vDimName );
TM1IMPORT TM1V TM1API TM1DimensionAttributesGet( TM1P hPool, TM1V hDimension );

 #ifdef GEN_WIN
// I think this one we can just over-write - .NET only
TM1IMPORT void  TM1API TM1ValStringGet_CSH( TM1U hUser, TM1V vString, TCHAR ** string  );
#endif

TM1IMPORT TM1V TM1API TM1ObjectAttributeValuesSet( TM1P hPool, TM1V hAttribute, TM1V hObjects, TM1V vValues );
//*********************************************************************
//  Constant Functions
TM1IMPORT TM1_INDEX TM1API TM1ValTypeReal( void );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeString( void );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeStringW( void );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeIndex( void );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeBool( void );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeObject( void );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeError( void );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeArray( void );
TM1IMPORT TM1_INDEX TM1API TM1ValTypeBinary( void );
TM1IMPORT TM1V TM1API TM1ArrayNull( void );
TM1IMPORT TM1V TM1API TM1ObjectNull( void );
TM1IMPORT TM1V TM1API TM1CubeCellValueUndefined( void );
TM1IMPORT TM1V TM1API TM1Entry(void);
TM1IMPORT TM1V TM1API TM1TypeAttribute( void );
TM1IMPORT TM1V TM1API TM1TypeBlob( void );
TM1IMPORT TM1V TM1API TM1TypeClient( void );
TM1IMPORT TM1V TM1API TM1TypeChore( void );
TM1IMPORT TM1V TM1API TM1TypeConnection( void );
TM1IMPORT TM1V TM1API TM1TypeCube( void );
TM1IMPORT TM1V TM1API TM1TypeDimension( void );
TM1IMPORT TM1V TM1API TM1TypeElement( void );
TM1IMPORT TM1V TM1API TM1TypeGroup( void );
TM1IMPORT TM1V TM1API TM1TypeProcess( void );
TM1IMPORT TM1V TM1API TM1TypeRule( void );
TM1IMPORT TM1V TM1API TM1TypeSubset( void );
TM1IMPORT TM1V TM1API TM1TypeServer( void );
TM1IMPORT TM1V TM1API TM1TypeSQLNotSupported();
TM1IMPORT TM1V TM1API TM1TypeSQLNumericColumn();
TM1IMPORT TM1V TM1API TM1TypeSQLStringColumn();
TM1IMPORT TM1V TM1API TM1TypeSQLTable();
TM1IMPORT TM1V TM1API TM1TypeView( void );
TM1IMPORT TM1V TM1API TM1TypeElementSimple( void );
TM1IMPORT TM1V TM1API TM1TypeElementConsolidated( void );
TM1IMPORT TM1V TM1API TM1TypeElementString( void );
TM1IMPORT TM1V TM1API TM1TypeAttributeAlias( void );
TM1IMPORT TM1V TM1API TM1TypeAttributeString( void );
TM1IMPORT TM1V TM1API TM1TypeAttributeNumeric( void );
TM1IMPORT TM1V TM1API TM1TypeRuleCalculation( void );
TM1IMPORT TM1V TM1API TM1TypeRuleDrill( void );
TM1IMPORT TM1V TM1API TM1ObjectAttributes( void );      
TM1IMPORT TM1V TM1API TM1ObjectChangedSinceLoaded( void );                         
TM1IMPORT TM1V TM1API TM1ObjectLastTimeUpdated( void ); 
TM1IMPORT TM1V TM1API TM1ObjectInstanceVersion( void ); 
TM1IMPORT TM1V TM1API TM1ObjectMemoryUsed( void );      
TM1IMPORT TM1V TM1API TM1ObjectName( void );            
TM1IMPORT TM1V TM1API TM1ObjectParent( void );          
TM1IMPORT TM1V TM1API TM1ObjectSecurityStatus( void );  
TM1IMPORT TM1V TM1API TM1ObjectSecurityOwner( void );   
TM1IMPORT TM1V TM1API TM1ObjectType( void );            
TM1IMPORT TM1V TM1API TM1ObjectRegistration( void );
TM1IMPORT TM1V TM1API TM1AttributeType( void );
TM1IMPORT TM1V TM1API TM1ClientPassword( void );                            
TM1IMPORT TM1V TM1API TM1ChoreActive( void );
TM1IMPORT TM1V TM1API TM1ChoreFrequency( void );
TM1IMPORT TM1V TM1API TM1ChoreStartTime( void );
TM1IMPORT TM1V TM1API TM1ChoreSteps( void );
TM1IMPORT TM1V TM1API TM1ChoreExecutionMode( void );
TM1IMPORT TM1V TM1API TM1ClientStatus( void ); 
TM1IMPORT TM1V TM1API TM1ServerBuildNumber( void );
TM1IMPORT TM1V TM1API TM1ServerClients( void );                           
TM1IMPORT TM1V TM1API TM1ServerDirectories( void );  
TM1IMPORT TM1V TM1API TM1ServerDimensions( void );    
TM1IMPORT TM1V TM1API TM1ServerGroups( void );                            
TM1IMPORT TM1V TM1API TM1ServerCubes( void );         
TM1IMPORT TM1V TM1API TM1ServerBlobs( void );
TM1IMPORT TM1V TM1API TM1ServerLogDirectory( void );   
TM1IMPORT TM1V TM1API TM1ServerNetworkAddress( void );
TM1IMPORT TM1V TM1API TM1ServerChores( void );
TM1IMPORT TM1V TM1API TM1ServerProcesses( void ); 
TM1IMPORT TM1V TM1API TM1ServerConnections( void );                      
TM1IMPORT TM1V TM1API TM1ServerProcessObjectsSupported( void );
TM1IMPORT TM1V TM1API TM1ServerProgressMessageOn(void);
TM1IMPORT TM1V TM1API TM1ServerParallelInteraction( void );
TM1IMPORT TM1V TM1API TM1ServerCalculationCachePartitioning( void );
TM1IMPORT TM1V TM1API TM1ServerSession( void );
TM1IMPORT TM1V TM1API TM1BlobSize( void );
TM1IMPORT TM1V TM1API TM1ConnectionChoresUsing( void ); 
TM1IMPORT TM1V TM1API TM1ConnectionLastSyncTime( void ); 
TM1IMPORT TM1V TM1API TM1ConnectionLastSyncTimeStar( void );
TM1IMPORT TM1V TM1API TM1ConnectionLastSyncStarRecord( void );         
TM1IMPORT TM1V TM1API TM1ConnectionSyncErrorCount( void ); 
TM1IMPORT TM1V TM1API TM1ConnectionSyncPlanetToStar( void ); 
TM1IMPORT TM1V TM1API TM1ConnectionSyncStarToPlanet( void ); 
TM1IMPORT TM1V TM1API TM1ConnectionUserName( void );          
TM1IMPORT TM1V TM1API TM1ConnectionUseIntegratedSecurity( void );          
TM1IMPORT TM1V TM1API TM1ConnectionNamespace( void );          
TM1IMPORT TM1V TM1API TM1DimensionElements( void );     
TM1IMPORT TM1V TM1API TM1DimensionNofLevels( void );    
TM1IMPORT TM1V TM1API TM1DimensionSubsets( void );      
TM1IMPORT TM1V TM1API TM1DimensionCubesUsing( void );      
TM1IMPORT TM1V TM1API TM1DimensionWidth( void );        
TM1IMPORT TM1V TM1API TM1DimensionReplicationSyncSubsets( void );
TM1IMPORT TM1V TM1API TM1ElementIndex( void );          
TM1IMPORT TM1V TM1API TM1ElementLevel( void );          
TM1IMPORT TM1V TM1API TM1ElementType( void );           
TM1IMPORT TM1V TM1API TM1ElementComponents( void );     
TM1IMPORT TM1V TM1API TM1ElementParents( void );        
TM1IMPORT TM1V TM1API TM1RuleNofLines( void );
TM1IMPORT TM1V TM1API TM1RuleErrorLine( void );
TM1IMPORT TM1V TM1API TM1RuleErrorString( void );
TM1IMPORT TM1V TM1API TM1SubsetElements( void ); 
TM1IMPORT TM1V TM1API TM1SubsetSourceName( void);
TM1IMPORT TM1V TM1API TM1SubsetAlias( void );   
TM1IMPORT TM1V TM1API TM1SubsetExpression( void );                            
TM1IMPORT TM1V TM1API TM1CubeDimensions( void );       
TM1IMPORT TM1V TM1API TM1CubeLogChanges( void );                           
TM1IMPORT TM1V TM1API TM1CubeRule( void );                                 
TM1IMPORT TM1V TM1API TM1CubeViews( void );                                
TM1IMPORT TM1V TM1API TM1CubePerspectivesMaxMemory( void );
TM1IMPORT TM1V TM1API TM1CubePerspectivesMinTime( void );
TM1IMPORT TM1V TM1API TM1CubeMeasuresDimension( void );
TM1IMPORT TM1V TM1API TM1CubeReplicationSyncRule( void );
TM1IMPORT TM1V TM1API TM1CubeReplicationSyncViews( void );
TM1IMPORT TM1V TM1API TM1CubeTimeDimension( void );
TM1IMPORT TM1V TM1API TM1ViewColumnSubsets( void );                         
TM1IMPORT TM1V TM1API TM1CubeCellSpreadFunctionOk(void);
TM1IMPORT TM1V TM1API TM1CubeCellSpreadNumericSetOk(void);
TM1IMPORT TM1V TM1API TM1CubeCellSpreadStringSetOk(void);
TM1IMPORT TM1V TM1API TM1CubeCellSpreadStatusHeld(void);
TM1IMPORT TM1V TM1API TM1CubeCellSpreadStatusHeldConsolidation(void);
TM1IMPORT TM1V TM1API TM1CubeCellSpreadStatusWritable(void);
TM1IMPORT TM1V TM1API TM1CubeTimeLastInvalidated(void);
TM1IMPORT TM1V TM1API TM1CubeDataReservationMode(void);
TM1IMPORT TM1V TM1API TM1CubeCellSecurityDefaultValue(void);
TM1IMPORT TM1V TM1API TM1CubeCalculationThreshold( void );
TM1IMPORT TM1V TM1API TM1ViewPreConstruct( void );                          
TM1IMPORT TM1V TM1API TM1ViewRowSubsets( void );                            
TM1IMPORT TM1V TM1API TM1ViewSuppressZeroes( void );                        
TM1IMPORT TM1V TM1API TM1ViewTitleElements( void );                         
TM1IMPORT TM1V TM1API TM1ViewTitleSubsets( void );                          
TM1IMPORT TM1V TM1API TM1ViewFormat( void );                                
TM1IMPORT TM1V TM1API TM1ViewShowAutomatically( void ); 
TM1IMPORT TM1V TM1API TM1ViewArrayCellOrdinal( void );                          
TM1IMPORT TM1V TM1API TM1ViewArrayCellValue( void );                                
TM1IMPORT TM1V TM1API TM1ViewArrayCellFormattedValue( void );                     
TM1IMPORT TM1V TM1API TM1ViewArrayCellFormatString( void );                     
TM1IMPORT TM1V TM1API TM1ViewArrayMemberName( void );                                
TM1IMPORT TM1V TM1API TM1ViewArrayMemberType( void );                     
TM1IMPORT TM1V TM1API TM1ViewArrayMemberDescription( void );                     
TM1IMPORT TM1V TM1API TM1SecurityRightNone( void );
TM1IMPORT TM1V TM1API TM1SecurityRightRead( void );
TM1IMPORT TM1V TM1API TM1SecurityRightWrite( void );
TM1IMPORT TM1V TM1API TM1SecurityRightReserve( void );
TM1IMPORT TM1V TM1API TM1SecurityRightLock( void );
TM1IMPORT TM1V TM1API TM1SecurityRightAdmin( void );
TM1IMPORT TM1V TM1API TM1ObjectPublic( void );
TM1IMPORT TM1V TM1API TM1ObjectPrivate( void );
TM1IMPORT TM1V TM1API TM1ObjectUnregistered( void );
TM1IMPORT TM1V TM1API TM1ObjectReplicationConnection( void );
TM1IMPORT TM1V TM1API TM1ObjectLastRepSyncChangedTime( void );
TM1IMPORT TM1V TM1API TM1ObjectReplicationSourceObjectName( void );
TM1IMPORT TM1V TM1API TM1ObjectReplicationStatus( void );
TM1IMPORT TM1V TM1API TM1ProcessChoresUsing(void);
TM1IMPORT TM1V TM1API TM1ProcessComplete(void);
TM1IMPORT TM1V TM1API TM1ProcessDataProcedure(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceASCIIDecimalSeparator(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceASCIIDelimiter(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceASCIIHeaderRecords(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceASCIIQuoteCharacter(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceASCIIThousandSeparator(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceCubeView(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceDimensionSubset(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceNameForClient(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceNameForServer(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceOleDbLocation(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceOleDbMdp(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourcePassword(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceQuery(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceType(void);
TM1IMPORT TM1V TM1API TM1ProcessDataSourceUserName(void);
TM1IMPORT TM1V TM1API TM1ProcessEpilogProcedure(void);
TM1IMPORT TM1V TM1API TM1ProcessGrantSecurityAccess(void);
TM1IMPORT TM1V TM1API TM1ProcessMetaDataProcedure(void);
TM1IMPORT TM1V TM1API TM1ProcessParametersDefaultValues(void);
TM1IMPORT TM1V TM1API TM1ProcessParametersNames(void);
TM1IMPORT TM1V TM1API TM1ProcessParametersPromptStrings(void);
TM1IMPORT TM1V TM1API TM1ProcessParametersTypes(void);
TM1IMPORT TM1V TM1API TM1ProcessPrologProcedure(void);
TM1IMPORT TM1V TM1API TM1ProcessUIData(void);
TM1IMPORT TM1V TM1API TM1ProcessVariablesEndingBytes(void);
TM1IMPORT TM1V TM1API TM1ProcessVariablesNames(void);
TM1IMPORT TM1V TM1API TM1ProcessVariablesPositions(void);
TM1IMPORT TM1V TM1API TM1ProcessVariablesStartingBytes(void);
TM1IMPORT TM1V TM1API TM1ProcessVariablesTypes(void);
TM1IMPORT TM1V TM1API TM1ProcessVariablesUIData(void);
TM1IMPORT TM1V TM1API TM1SQLTableColumnNames();
TM1IMPORT TM1V TM1API TM1SQLTableColumnTypes();
TM1IMPORT TM1V TM1API TM1SQLTableNumberOfColumns();
TM1IMPORT TM1V TM1API TM1SQLTableNumberOfRows();
TM1IMPORT TM1V TM1API TM1SQLTableRowsetSize();
TM1IMPORT TM1_INDEX TM1API TM1ErrorBlobCloseFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorBlobCreateFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorBlobGetFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorBlobNotOpen( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorBlobOpenFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorBlobPutFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorClientPasswordNotDefined( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorClientAlreadyExists( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellValueTypeMismatch( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusCubeNoWriteAccess(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusCubeLocked(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusCubeReserved(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusElementIsConsolidated(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusElementLocked(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusElementNoWriteAccess(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusElementReserved(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusRuleApplies(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusNoReservation(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCellWriteStatusCellReserved(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeCreationFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeDimensionInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeDrillNotFound( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeDrillInvalidStructure( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeKeyInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeNotEnoughDimensions( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeNumberOfKeysInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubePerspectiveAllSimpleElements( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubePerspectiveCreationFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeTooManyDimensions( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDataSpreadFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionCouldNotBeCompiled( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionElementAlreadyExists( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionElementComponentAlreadyExists( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionElementComponentDoesNotExist( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionElementComponentNotNumeric( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionElementDoesNotExist( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionElementNotConsolidated( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionHasCircularReferences( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionHasNoElements( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionIsBeingUsedByCube( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionNotChecked( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorGroupAlreadyExists( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorGroupMaximunNumberExceeded( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectAttributeNotDefined( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectAttributeInvalidType( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectDeleted( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectDuplicationFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectFileNotFound( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectFunctionDoesNotApply( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectHandleInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectHasNoParent( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectIndexInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectFileInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectIncompatibleTypes( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectIsRegistered( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectIsUnregistered( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectListIsEmpty( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectNameExists( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectNameIsBlank( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectNameInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectNotFound( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectNotLoaded( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectPropertyIsList( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectPropertyNotDefined( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectPropertyNotList( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectRegistrationFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectSecurityIsLocked( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectSecurityNoAdminRights( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectSecurityNoLockRights( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectSecurityNoReadRights( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectSecurityNoReserveRights( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorObjectSecurityNoWriteRights( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorRuleCubeHasRuleAttached( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorRuleIsAttached( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorRuleIsNotChecked( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorRuleLineNotFound( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSubsetIsBeingUsedByView( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemFunctionObsolete(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemServerClientAlreadyConnected( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemServerClientNotConnected( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemServerClientNotFound( void );
TM1IMPORT TM1_INDEX TM1API SystemServerConnectionFailed(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemServerClientPasswordInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemServerNotFound( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemOutOfMemory( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemUserHandleInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemValueInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemParameterTypeInvalid( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorViewHasPrivateSubsets( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorViewNotConstructed(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionNotRegistered(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorViewExpressionEmpty(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorDimensionUpdateFailedInvalidHierarchies(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorUpdateNonLeafCellValueFailed(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeNoTimeDimension(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorCubeMeasuresAndTimeDimensionSame(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorAuditLogResultSetDoesNotExist(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorAuditLogResultSetInvalidRange(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorAuditLogRecordDoesNotExist(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorAuditLogResultSetOutOfMemory(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorExecutingAuditLogQuery(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorValueNotInPickList(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorServerInBulkLoadMode(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorTM1PATHEnvVarNotFound( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCannotCreateAlternateHierarchyFromAlternateHierarchy( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemServerClientConnectFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressMessageOpening( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressMessageRunning( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressMessageClosing( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressTypePercent( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressTypeCounter( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionLoadingCube( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionLoadingDimension( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionRunningQuery( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionCalculatingView( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionLoadingSubset( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionSavingSubset( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionSortingSubset( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionCalculatingSubsetAll( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionInsertingSubset( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionDuplicatingSubset( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionCalculatingSubsetHierarchy( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionSelectingSubsetElements( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionDeletingSelection( void );
TM1IMPORT TM1_INDEX TM1API TM1ProgressActionKeepingSelection( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorClientAddedWithErrors(void);
TM1IMPORT TM1_INDEX TM1API TM1ErrorGroupAddedWithErrors( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorInvalidCapabilityFeature( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorInvalidCapabilityPermission( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorInvalidCapabilityPolicy( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorUpdateNotReady( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorNoUpdateToProcess( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorTUnitRedefined( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorChoreModifiedDuringExecution( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorChoreDeleted( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonNone( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonEQ_A( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonGE_A( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonLE_A( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonGT_A( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonLT_A( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonNE_A( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonGE_A_LE_B( void );
TM1IMPORT TM1V TM1API TM1ViewExtractComparisonGT_A_LT_B( void );
TM1IMPORT TM1V TM1API TM1ViewExtractSkipConsolidatedValues( void );           
TM1IMPORT TM1V TM1API TM1ViewExtractSkipRuleValues( void );           
TM1IMPORT TM1V TM1API TM1ViewExtractSkipZeroes( void );                     
TM1IMPORT TM1V TM1API TM1ViewExtractRealLimitA( void );                     
TM1IMPORT TM1V TM1API TM1ViewExtractRealLimitB( void );                     
TM1IMPORT TM1V TM1API TM1ViewExtractStringLimitA( void );                   
TM1IMPORT TM1V TM1API TM1ViewExtractStringLimitB( void );                   
TM1IMPORT TM1V TM1API TM1ViewExtractComparison( void );   
TM1IMPORT TM1V TM1API TM1ClientAddEx( TM1P hPool, TM1V hServer, TM1V sClientName, TM1V sClientDefDisplayValue );
TM1IMPORT TM1V TM1API TM1ObjAliasControlValueGet( TM1P hPool, TM1V hObject, TM1V hType );
TM1IMPORT TM1_INDEX TM1API TM1ErrorSystemServerCAMSecurityRequired( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCAMDllLoadFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorCAMObjectCreateFailed( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorControlAliasNotFound( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorControlAliasInvalidType( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorControlAliasInvalidValueType( void );
TM1IMPORT TM1V TM1API TM1GroupAddEx( TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sGroupDefDisplayValue, TM1V sLangDisplayArr);
TM1IMPORT TM1V TM1API TM1SecurityObject( void );                        
TM1IMPORT TM1V TM1API TM1RawstoreExists( void );
TM1IMPORT TM1V TM1API TM1AuditLogExists( void );
TM1IMPORT TM1V TM1API TM1EnableBulkLoadMode( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1DisableBulkLoadMode( TM1P hPool, TM1V hServer );
TM1IMPORT TM1V TM1API TM1ChangeSetBegin(TM1P hPool, TM1V hServer);
TM1IMPORT TM1V TM1API TM1ChangeSetEnd(TM1P hPool, TM1V hServer);
TM1IMPORT TM1V TM1API TM1ChangeSetUndo(TM1P hPool, TM1V hServer, TM1V sChangeSetId);
TM1IMPORT TM1V TM1API TM1ApplicationFolderContentsGet(TM1P hPool, TM1V hFolder, TM1V iDepth);
TM1IMPORT TM1V TM1API TM1DataReservationAcquire(TM1P hPool, TM1V hCube, TM1V hClient, TM1V bForce, TM1V elementArray);
TM1IMPORT TM1V TM1API TM1DataReservationRelease(TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray);
TM1IMPORT TM1V TM1API TM1DataReservationReleaseAll(TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray);
TM1IMPORT TM1V TM1API TM1DataReservationGetAll(TM1P hPool, TM1V hCube, TM1V hClient);
TM1IMPORT TM1V TM1API TM1DataReservationValidate(TM1P hPool, TM1V hCube);
TM1IMPORT TM1V TM1API TM1DataReservationGetConflicts(TM1P hPool, TM1V hCube, TM1V hClient, TM1V elementArray);
TM1IMPORT TM1V TM1API TM1AssociateCAMIDToGroup( TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sCAMID, TM1V sCAMIDDefDisplayValue, TM1V sLangDisplayArr);
TM1IMPORT TM1V TM1API TM1RemoveCAMIDAssociationFromGroup( TM1P hPool, TM1V hServer, TM1V sGroupName, TM1V sCAMID);
TM1IMPORT TM1V TM1API TM1RemoveCAMIDAssociation( TM1P hPool, TM1V hServer, TM1V sCAMID, TM1V bRemoveCAMID);
TM1IMPORT TM1V TM1API TM1GetCAMIDsAssociatedWithGroup( TM1P hPool, TM1V hServer, TM1V sGroupName);
TM1IMPORT TM1V TM1API TM1GetGroupsAssociatedWithCAMID( TM1P hPool, TM1V hServer, TM1V sCAMID);
TM1IMPORT TM1_INDEX TM1API TM1ErrorAssociateCAMIDToGroup( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorRemoveCAMIDAssociationFromGroup( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorRemoveCAMIDAssociation( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorGetCAMIDsAssociatedWithGroup( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorGetGroupsAssociatedWithCAMID( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorGroupAssociationNotFound( void );
TM1IMPORT TM1V TM1API TM1CreateExpressionBasedView( TM1P hPool, TM1V hServer, TM1V sExpression );
TM1IMPORT TM1V TM1API TM1CubeExpressionBasedViews( void );   
TM1IMPORT TM1V TM1API TM1ViewMDXExpression( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorElementNameContainsMetacharacters( void );
TM1IMPORT TM1_INDEX TM1API TM1ErrorAccessDisabledByPolicy( void );


// Obsolete functions
TM1IMPORT TM1V TM1API TM1CubeCellDrillStringGet( TM1P hPool, TM1V hCube, TM1V hArrayOfElements );
TM1IMPORT TM1V TM1API TM1DimensionTopElement( void );   
TM1IMPORT TM1V TM1API TM1ObjectReplication( void );
TM1IMPORT TM1V TM1API TM1ObjectReplicationSourceName( void );
TM1IMPORT TM1V TM1API TM1SubsetSubtract( TM1P hPool, TM1V hSubsetA, TM1V hSubsetB );
//*********************************************************************



#ifdef __cplusplus
		};
#endif // __cplusplus



#endif        // }  ifndef TM1_OBJECT_DEFINITION_ONLY

#endif // TM1_API_H
