//////////////////////////////////////////////////////////////////////
// InPlaceEdit.h : header file
//
// MFC Grid Control - inplace editing class
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
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
//         30 Dec 2008  Support for spin button control added (Alexey Shabelnikov)
//         18 Dec 2009  Refactoring and bugfix
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\EditEx.h"
#include "common\unicodesupport.h"

class CPropertyGridInPlaceEdit : public CEditEx
{
  typedef CEditEx Super;
 public:
  //used for edit with spin button
  struct InplaceEditParamsEx
  {
   InplaceEditParamsEx()
   : m_mode(1), m_spin(false)
   , m_decimal_places(-1), m_lower(0),m_upper(100)
   , m_limit_text(256) {}

   DWORD  m_mode;           //mode, e.g. float, integer, hex... (see CEditEx class)
   bool   m_spin;           //spin should be, or not be present
   int    m_decimal_places; //decimal places (float: number of places after point)
   double m_lower;          //range for spin button
   double m_upper;
   double m_delta;          //delta for spin button
   int    m_limit_text;     //maximum number of characters
  };

  CPropertyGridInPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID, _TSTRING sInitText, const InplaceEditParamsEx* ip_ex_params = NULL);
  virtual ~CPropertyGridInPlaceEdit();

  void SetColors(COLORREF clrBack, COLORREF clrText);
  
 protected:
  void CancelEdit();
  void EndEdit();

  // Generated message map functions
 protected:  
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg UINT OnGetDlgCode();
  afx_msg HBRUSH CtlColor (CDC* pDC, UINT nCtlColor);
  DECLARE_MESSAGE_MAP()

 private:
  CString m_sInitText;
  BOOL    m_bExitOnArrows;
  CRect   m_Rect;

  COLORREF m_clrBack;
  COLORREF m_clrText;
  CBrush  m_Brush;
  CSpinButtonCtrlEx m_spin;
};
