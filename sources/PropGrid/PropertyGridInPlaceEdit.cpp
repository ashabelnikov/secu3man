// InPlaceEdit.cpp : implementation file
//
// Adapted by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// The code contained in this file is based on the original
// CPropertyGridInPlaceEdit from http://www.codeguru.com/listview/edit_subitems.shtml
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name and all copyright
// notices remains intact.
//
// An email letting me know how you are using it would be nice as well.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.10+
//
// History:
//         10 May 1998  Uses GVN_ notifications instead of LVN_,
//                      Sends notification messages to the parent,
//                      instead of the parent's parent.
//         15 May 1998  There was a problem when editing with the in-place editor,
//                      there arises a general protection fault in user.exe, with a
//                      few qualifications:
//                         (1) This only happens with owner-drawn buttons;
//                         (2) This only happens in Win95
//                         (3) This only happens if the handler for the button does not
//                             create a new window (even an AfxMessageBox will avoid the
//                             crash)
//                         (4) This will not happen if Spy++ is running.
//                      PreTranslateMessage was added to route messages correctly.
//                      (Matt Weagle found and fixed this problem)
//         26 Jul 1998  Removed the ES_MULTILINE style - that fixed a few probs!
//          6 Aug 1998  Added nID to the constructor param list
//          6 Sep 1998  Space no longer clears selection when starting edit (Franco Bez)
//         10 Apr 1999  Enter, Tab and Esc key prob fixed (Koay Kah Hoe)
//                      Workaround for bizzare "shrinking window" problem in CE
//
//         30 Dec 2008  Support for spin button control added (Alexey Shabelnikov)
//         18 Dec 2008  UDS_ARROWKEYS style added
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCHAR.h"
#include "PropertyGridInPlaceEdit.h"
#include "wm_messages.h"

using namespace std;

CPropertyGridInPlaceEdit::CPropertyGridInPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID, _TSTRING sInitText, const InplaceEditParamsEx* ip_ex_params /*= NULL*/)
{
 m_sInitText     = sInitText.c_str();
 m_bExitOnArrows = FALSE;
 m_Rect = rect;  // For bizarre CE bug.

 DWORD dwEditStyle = WS_CHILD|ES_AUTOHSCROLL|dwStyle;
 CRect edit_rect;
 edit_rect = rect;

 if (NULL!=ip_ex_params && ip_ex_params->m_spin)
  edit_rect.right -= 20;

 if (!Create(dwEditStyle, edit_rect, pParent, nID))
  return;

 m_clrBack = GetSysColor(COLOR_WINDOW);
 m_clrText = GetSysColor(COLOR_WINDOWTEXT);
 m_Brush.CreateSolidBrush(m_clrBack);

 SetFont(pParent->GetFont());
 SetWindowText(m_sInitText);
 SetFocus();

 if (NULL!=ip_ex_params)
 {
  SetMode(ip_ex_params->m_mode);
  SetDecimalPlaces(ip_ex_params->m_decimal_places);
  if (ip_ex_params->m_limit_text!=-1)
   SetLimitText(ip_ex_params->m_limit_text);

  if (ip_ex_params->m_spin)
  {
   CRect spin_rect;
   spin_rect = rect;
   spin_rect.left = spin_rect.right - 20;
   m_spin.Create(WS_CHILD|WS_VISIBLE|UDS_ARROWKEYS, spin_rect, pParent, nID + 1);
   m_spin.SetBuddy(this);
   m_spin.SetRangeAndDelta(ip_ex_params->m_lower,ip_ex_params->m_upper,ip_ex_params->m_delta);

   if (ip_ex_params->m_decimal_places!=-1)
   {
    float value;
    GetValue(value);
    SetValue(value);
   }
  }
 }

 SetSel(0, -1);
 SetSel(-1, 0);
}

CPropertyGridInPlaceEdit::~CPropertyGridInPlaceEdit()
{
 //empty
}

void CPropertyGridInPlaceEdit::SetColors(COLORREF clrBack, COLORREF clrText)
{
 m_clrBack = clrBack;
 m_clrText = clrText;
 m_Brush.DeleteObject();
 m_Brush.CreateSolidBrush(m_clrBack);
}

BEGIN_MESSAGE_MAP(CPropertyGridInPlaceEdit, CEdit)
 ON_WM_KILLFOCUS()
 ON_WM_CHAR()
 ON_WM_KEYDOWN()
 ON_WM_GETDLGCODE()
 ON_WM_CREATE()
 ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CPropertyGridInPlaceEdit message handlers

// If an arrow key (or associated) is pressed, then exit if
//  a) The Ctrl key was down, or
//  b) m_bExitOnArrows == TRUE
void CPropertyGridInPlaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
      nChar == VK_DOWN  || nChar == VK_UP   ||
      nChar == VK_RIGHT || nChar == VK_LEFT) &&
     (m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
 {
  GetParent()->SetFocus();
  return;
 }

 Super::OnKeyDown(nChar, nRepCnt, nFlags);
}

// As soon as this edit loses focus, kill it.
void CPropertyGridInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
 Super::OnKillFocus(pNewWnd);
 EndEdit();
}

void CPropertyGridInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 if (nChar == VK_TAB || nChar == VK_RETURN)
 {
  GetParent()->SetFocus();    // This will destroy this window
  return;
 }
 if (nChar == VK_ESCAPE)
 {
  CancelEdit();
  return;
 }

 Super::OnChar(nChar, nRepCnt, nFlags);

 //// Resize edit control if needed
 //
 //// Get text extent
 //CString str;
 //GetWindowText( str );

 //// add some extra buffer
 //str += _T("  ");
 //
 //CWindowDC dc(this);
 //CFont *pFontDC = dc.SelectObject(GetFont());
 //CSize size = dc.GetTextExtent( str );
 //dc.SelectObject( pFontDC );
 //
 //// Get client rect
 //CRect ParentRect;
 //GetParent()->GetClientRect( &ParentRect );
 //
 //// Check whether control needs to be resized
 //// and whether there is space to grow
 //if (size.cx > m_Rect.Width())
 //{
 //    if( size.cx + m_Rect.left < ParentRect.right )
 //        m_Rect.right = m_Rect.left + size.cx;
 //    else
 //        m_Rect.right = ParentRect.right;
 //    MoveWindow( &m_Rect );
 //}
}

UINT CPropertyGridInPlaceEdit::OnGetDlgCode()
{
 return DLGC_WANTALLKEYS;
}

////////////////////////////////////////////////////////////////////////////
// CPropertyGridInPlaceEdit overrides

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL CPropertyGridInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
 // Catch the Alt key so we don't choke if focus is going to an owner drawn button
 if (pMsg->message == WM_SYSCHAR)
  return TRUE;
 return Super::PreTranslateMessage(pMsg);
}

////////////////////////////////////////////////////////////////////////////
// CPropertyGridInPlaceEdit implementation

void CPropertyGridInPlaceEdit::CancelEdit()
{
 // restore previous text
 if (IsWindow(GetSafeHwnd()))
 {
  SetWindowText(m_sInitText);
  SendMessage(WM_CLOSE, 0, 0);
 }
}

void CPropertyGridInPlaceEdit::EndEdit()
{
 CString str;

 // EFW - BUG FIX - Clicking on a grid scroll bar in a derived class
 // that validates input can cause this to get called multiple times
 // causing assertions because the edit control goes away the first time.
 static BOOL bAlreadyEnding = FALSE;

 if(bAlreadyEnding)
  return;

 bAlreadyEnding = TRUE;
 GetWindowText(str);

 CWnd* pOwner = GetOwner();
 if (pOwner)
  pOwner->SendMessage(WM_PG_ENDLABELEDIT, (WPARAM) LPCTSTR(str), NULL );

 // Close this window (PostNcDestroy will delete this)
 if (IsWindow(GetSafeHwnd()))
  SendMessage(WM_CLOSE, 0, 0);
 bAlreadyEnding = FALSE;
}

HBRUSH CPropertyGridInPlaceEdit::CtlColor ( CDC* pDC, UINT nCtlColor )
{
 pDC->SetTextColor(m_clrText);
 pDC->SetBkColor(m_clrBack);
 return m_Brush;
}
