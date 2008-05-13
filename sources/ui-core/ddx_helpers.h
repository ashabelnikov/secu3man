
#ifdef DDX_HELPERS_EXPORTS
#define DDX_HELPERS_API __declspec(dllexport)
#else
#define DDX_HELPERS_API __declspec(dllimport)
#endif

void  DDX_HELPERS_API DDX_Check_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value );
void DDX_HELPERS_API  DDX_CBIndex_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& index);
