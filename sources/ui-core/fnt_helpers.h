
#pragma once

#ifdef FNT_HELPERS_EXPORTS
 #define FNT_HELPERS_API __declspec(dllexport)
#else
 #define FNT_HELPERS_API __declspec(dllimport)
#endif

// wnd - Window to clone font from
// newFont - pointer to objects which will receive new font
// height - height of a new font
// bold - new font will be bold if true
bool FNT_HELPERS_API CloneWndFont(CWnd* wnd, CFont* newFnt, LONG height, bool bold = false);
