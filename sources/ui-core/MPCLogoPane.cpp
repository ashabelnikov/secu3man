//
// MPCLogoPane.cpp
// 
// (c) Pierre MELLINAND
//
// History
// --------
// v 1.0
//		June 1998:				Created.
// v 1.01
//		October 19, 1999		Added History
//

#include "StdAfx.h"
#include "MPCLogoPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////
// MPCLogoPane
IMPLEMENT_DYNCREATE(MPCLogoPane,CWnd)
BEGIN_MESSAGE_MAP(MPCLogoPane, CWnd)
	//{{AFX_MSG_MAP(MPCLogoPane)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MPCLogoPane::MPCLogoPane()
{
	SetLogoFont("Arial");
}


void MPCLogoPane::SetLogoText(CString sTxt)
{
	m_LogoText=sTxt;
}


void MPCLogoPane::SetLogoFont(CString Name, int nHeight/* = 24*/,
	int nWeight/* = FW_BOLD*/, BYTE bItalic/* = true*/, BYTE bUnderline/* = false*/)
{
	if(m_fontLogo.m_hObject)
		m_fontLogo.Detach();

	m_fontLogo.CreateFont(nHeight, 0, 0, 0, nWeight, bItalic, bUnderline,0,0,0,0,0,0, Name);
}


void MPCLogoPane::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetWindowRect(rect);

	dc.SetBkMode(TRANSPARENT);

	CFont * OldFont = dc.SelectObject(&m_fontLogo);

	// draw text in DC
	COLORREF OldColor = dc.SetTextColor( ::GetSysColor( COLOR_3DHILIGHT));

	rect.right = rect.Width();
	rect.bottom = rect.Height();
	rect.left = rect.top = 0;
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));


	dc.DrawText( m_LogoText, rect + CPoint(1,1), DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	dc.SetTextColor( ::GetSysColor( COLOR_3DSHADOW));
	dc.DrawText( m_LogoText, rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	// restore old text color
	dc.SetTextColor( OldColor);
	// restore old font
	dc.SelectObject(OldFont);
	
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL MPCLogoPane::Create(CString sText,DWORD dwStyle, CWnd * pParent,UINT nID)
{
	BOOL bOk = CWnd::Create(AfxRegisterWndClass(CS_BYTEALIGNCLIENT),
							sText,
							dwStyle,
							CRect(0,0,0,0),
							pParent,nID);
	m_LogoText=sText;
	return bOk;

}
