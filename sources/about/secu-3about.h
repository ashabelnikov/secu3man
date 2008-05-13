
#ifdef ABOUT_EXPORTS
#define ABOUT_API __declspec(dllexport)
#else
#define ABOUT_API __declspec(dllimport)
#endif

void ABOUT_API DisplayAbout(CWnd* i_pParent);