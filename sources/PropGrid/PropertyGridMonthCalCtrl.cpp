// PropertyGridMonthCalCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyGridMonthCalCtrl.h"
#include "wm_messages.h"

// CPropertyGridMonthCalCtrl

CPropertyGridMonthCalCtrl::CPropertyGridMonthCalCtrl()
{
 //empty
}

CPropertyGridMonthCalCtrl::~CPropertyGridMonthCalCtrl()
{
 //empty
}

BEGIN_MESSAGE_MAP(CPropertyGridMonthCalCtrl, Super)
 ON_WM_KILLFOCUS()
 ON_NOTIFY_REFLECT(MCN_SELECT, OnMcnSelect)
 ON_WM_GETDLGCODE()
 ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CPropertyGridMonthCalCtrl message handlers

void CPropertyGridMonthCalCtrl::OnKillFocus(CWnd* pNewWnd)
{
 Super::OnKillFocus(pNewWnd);
 CWnd* pParent = pNewWnd ? pNewWnd->GetParent() : NULL;
 if (pParent != this)
  DestroyWindow();
}

void CPropertyGridMonthCalCtrl::OnMcnSelect(NMHDR *pNMHDR, LRESULT *pResult)
{
 LPNMSELCHANGE pSelChange = reinterpret_cast<LPNMSELCHANGE>(pNMHDR);
 GetOwner()->SendMessage(WM_PG_DATESELCHANGED);
 *pResult = 0;
}

UINT CPropertyGridMonthCalCtrl::OnGetDlgCode()
{
 return DLGC_WANTALLKEYS;
}

void CPropertyGridMonthCalCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 if (nChar == VK_ESCAPE)
 {
  DestroyWindow();
  return;
 }
 else if (nChar == VK_RETURN || nChar == VK_EXECUTE)
 {
  GetOwner()->SendMessage(WM_PG_DATESELCHANGED);
  return;
 }

 Super::OnKeyDown(nChar, nRepCnt, nFlags);
}
