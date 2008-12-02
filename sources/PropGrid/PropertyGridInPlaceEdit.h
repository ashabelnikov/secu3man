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
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"
#include "common/unicodesupport.h"

#define WM_PG_ENDLABELEDIT WM_USER+488

class CPropertyGridInPlaceEdit : public CEditEx
{
 typedef CEditEx Super;
  // Construction
public:

	//used for edit with spin button
	struct InplaceEditParamsEx
	{	  
	  InplaceEditParamsEx() : m_mode(1), m_spin(false)
		, m_decimal_places(-1), m_lower(0),m_upper(100)
		, m_limit_text(256) {}
	  void operator=(const InplaceEditParamsEx& i_other)
	  {
	   m_mode = i_other.m_mode;
	   m_spin = i_other.m_spin;
	   m_decimal_places = i_other.m_decimal_places;
	   m_lower = i_other.m_lower;
	   m_upper = i_other.m_upper;
	   m_delta = i_other.m_delta;
	   m_limit_text = i_other.m_limit_text;
	  }
	  DWORD  m_mode;           //mode, e.g. float, integer, hex... (see CEditEx class) 	  
	  bool   m_spin;	       //spin should be, or not be present
	  int    m_decimal_places; //decimal places (float: number of places after point)
	  double m_lower;          //range for spin button
	  double m_upper;
	  double m_delta;          //delta for spin button
	  int    m_limit_text;     //maximum number of characters
	};

  CPropertyGridInPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID, _TSTRING sInitText, const InplaceEditParamsEx* ip_ex_params = NULL);
  void SetColors(COLORREF clrBack, COLORREF clrText);

  // Attributes
public:

  // Operations
public:
  void CancelEdit();
  void EndEdit();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPropertyGridInPlaceEdit)
public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CPropertyGridInPlaceEdit();

  // Generated message map functions
protected:
  //{{AFX_MSG(CPropertyGridInPlaceEdit)
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg UINT OnGetDlgCode();
  afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
  //}}AFX_MSG
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

/////////////////////////////////////////////////////////////////////////////
