 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
 //na
}

CChildView::~CChildView()
{
 //na
}

BEGIN_MESSAGE_MAP(CChildView,CWnd )
 ON_WM_PAINT()
 ON_WM_SHOWWINDOW()
 ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
 if (!CWnd::PreCreateWindow(cs))
  return FALSE;

 cs.dwExStyle |= WS_EX_CLIENTEDGE;
 cs.style &= ~WS_BORDER;
 cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
  ::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

 return TRUE;
}

void CChildView::OnPaint() 
{
 CPaintDC dc(this); // device context for painting		
 // Do not call CWnd::OnPaint() for painting messages
}


BOOL CChildView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{	
 BOOL result = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
 return result;
}

void CChildView::PostNcDestroy() 
{
 CWnd::PostNcDestroy();
}


void CChildView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{	
 CWnd::CalcWindowRect(lpClientRect, nAdjustType);
}

void CChildView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 CWnd::OnShowWindow(bShow, nStatus);		 
}

void CChildView::PreSubclassWindow() 
{
 CWnd::PreSubclassWindow();
}

void CChildView::OnSize(UINT nType, int cx, int cy) 
{
 CWnd::OnSize(nType, cx, cy);	
}
