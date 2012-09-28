 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "ProgressCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////

CProgressCtrlEx::CProgressCtrlEx()
{
 //empty
}

CProgressCtrlEx::~CProgressCtrlEx()
{
 //empty
}

BEGIN_MESSAGE_MAP(CProgressCtrlEx, CProgressCtrl)
 ON_WM_PAINT()
END_MESSAGE_MAP()

void CProgressCtrlEx::OnPaint()
{
 if (IsWindowEnabled())
  CProgressCtrl::OnPaint();
 else
  CPaintDC dc(this); // device context for painting
}
