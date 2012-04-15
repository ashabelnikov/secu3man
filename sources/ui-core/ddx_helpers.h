
#pragma once

#ifdef DDX_HELPERS_EXPORTS
 #define DDX_HELPERS_API __declspec(dllexport)
#else
 #define DDX_HELPERS_API __declspec(dllimport)
#endif

void DDX_HELPERS_API DDX_Check_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value );
void DDX_HELPERS_API DDX_CBIndex_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& index);
void DDX_HELPERS_API DDX_Radio_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value);
void DDX_HELPERS_API DDX_Text_Fmt(CDataExchange* pDX, int nIDC, float& value, LPCTSTR OutFmt=_T("%f"), LPCTSTR InFmt=_T("%f"));
void DDX_HELPERS_API DDX_Text_Fmt(CDataExchange* pDX, int nIDC, int& value, LPCTSTR OutFmt=_T("%d"), LPCTSTR InFmt=_T("%d"));
