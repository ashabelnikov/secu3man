 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "ProgressCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cpp

CProgressCtrlEx::CProgressCtrlEx()
{
}

CProgressCtrlEx::~CProgressCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CProgressCtrlEx, CProgressCtrl)
	//{{AFX_MSG_MAP(Cpp)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cpp message handlers

void CProgressCtrlEx::OnPaint() 
{ 
  if (IsWindowEnabled())
	CProgressCtrl::OnPaint();
  else
	CPaintDC dc(this); // device context for painting
}
