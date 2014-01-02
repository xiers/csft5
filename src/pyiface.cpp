#include "pyiface.h"

#include "pysource.h"
#include "pycsft.h"

//------ Python Configure Block -------


//------ Python Data Source Block -------

// get string
#define LOC_GETS(_arg,_key) \
    if ( hSource.Exists(_key) ) \
        _arg = hSource[_key];

// get array of strings
#define LOC_GETAS(_arg,_key) \
    for ( CSphVariant * pVal = hSource(_key); pVal; pVal = pVal->m_pNext ) \
        _arg.Add ( pVal->cstr() );

#define LOC_CHECK_RET_NULL(_hash,_key,_msg,_add) \
    if (!( _hash.Exists ( _key ) )) \
    { \
        fprintf ( stdout, "ERROR: key '%s' not found " _msg "\n", _key, _add ); \
        return NULL; \
    }

typedef enum columnType_ {
    ECT_INTEGER     = 1510976267,
    ECT_TIMESTAMP   = 2782324286,
    ECT_BOOLEAN     = 2324007016,
    ECT_FLOAT       = 3383058069,
    ECT_LONG        = 999795048,
    ECT_STRING      = 2663297705,
    ECT_POLY2D      = 754820778,
    ECT_FIELD       = 1542800728,
    ECT_JSON        = 1795630405,
    ECT_NONT        = 0
} columnType;

void initColumnInfo(CSphColumnInfo& info, const char* sName, const char* sType){
    info.m_sName = sName;
    //info.m_eAttrType
    uint32_t iType = 0;
    if(sType)
      iType = sphCRC32((const BYTE*)sType);
    switch(iType)
    {
    case ECT_INTEGER: info.m_eAttrType = SPH_ATTR_INTEGER; break;
    case ECT_TIMESTAMP: info.m_eAttrType = SPH_ATTR_TIMESTAMP; break;
    case ECT_BOOLEAN: info.m_eAttrType = SPH_ATTR_BOOL; break;
    case ECT_FLOAT: info.m_eAttrType = SPH_ATTR_FLOAT; break;
    case ECT_LONG: info.m_eAttrType = SPH_ATTR_BIGINT; break;
    case ECT_STRING: info.m_eAttrType = SPH_ATTR_STRING; break;
    case ECT_POLY2D: info.m_eAttrType = SPH_ATTR_POLY2D; break;
    case ECT_JSON: info.m_eAttrType = SPH_ATTR_JSON; break;

    case ECT_FIELD:
    case ECT_NONT:
    default:
        info.m_eAttrType = SPH_ATTR_NONE; break;
    };
}

void setColumnBitCount(CSphColumnInfo& info, int iBitCount){
    info.m_tLocator.m_iBitCount = iBitCount;
}

uint32_t getCRC32(const char* data, size_t iLength)
{
    return sphCRC32((const BYTE*)data, iLength);
}

uint32_t getConfigValues(const CSphConfigSection & hSource, const char* sKey, CSphVector<CSphString>& values){
    // hSource reference as the following.. (hidden)
    int orig_length = values.GetLength();
    LOC_GETAS(values, sKey);
    return values.GetLength() - orig_length;
}

#if USE_PYTHON
CSphSource * SpawnSourcePython ( const CSphConfigSection & hSource, const char * sSourceName )
{
    assert ( hSource["type"]=="python" );

    LOC_CHECK_RET_NULL ( hSource, "name", "in source '%s'.", sSourceName ); //-> should move to setup.

    CSphString	PySourceName;
    LOC_GETS(PySourceName, "name");

    CSphSource_Python2 * pPySource = (CSphSource_Python2*)createPythonDataSourceObject( sSourceName, PySourceName.cstr() );
    if(!pPySource) {
        fprintf ( stdout, "ERROR: Create Python data source failure.\n");
        return NULL;
    }
    if ( !pPySource->Setup ( hSource ) ) {
        if(pPySource->GetErrorMessage().Length())
            fprintf ( stdout, "ERROR: %s\n", pPySource->GetErrorMessage().cstr());
        SafeDelete ( pPySource );
        return NULL;
    }

    return pPySource;
}
#endif

//------ Python Tokenizer Block -------


//------ Python Cache Block -------


//------ Python Query Block -------

