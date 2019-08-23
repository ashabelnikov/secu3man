/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file DynFieldsDialog.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include "common/unicodesupport.h"
#include <list>
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"

class CWndScroller;

class AFX_EXT_CLASS CDynFieldsDialog : public CDialog
{
 typedef CDialog Super;

 public:
  CDynFieldsDialog(CWnd* pParentWnd, const _TSTRING& caption, int height);
 ~CDynFieldsDialog();

  bool AppendItem(const _TSTRING& caption, const _TSTRING& unit, int vMin, int vMax, int vStp, int decPls, int* p_value);
  bool AppendItem(const _TSTRING& caption, const _TSTRING& unit, float vMin, float vMax, float vStp, int decPls, float* p_value);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
  afx_msg void OnChangeData(UINT nID);
  DECLARE_MESSAGE_MAP()

  void _UpdateScrlViewSize(int cx, int cy);

 private:
  const _TSTRING m_caption;
  bool m_initialized;
  CSize m_createSize;

  struct ItemData
  {
   ItemData()
   : p_edit(NULL), p_spin(NULL), p_capt(NULL), p_unit(NULL), intVal(NULL), fltVal(NULL)
   {
   }

   ~ItemData()
   {
    delete p_edit;
    delete p_spin;
    delete p_capt;
    delete p_unit;
   }

   bool Init(void)
   {
    if (intVal)
    {
     p_edit = new CEditEx(CEditEx::MODE_INT, true);
     intValm = *intVal;
    }
    else if (fltVal)
    {
     p_edit = new CEditEx(CEditEx::MODE_FLOAT, true);
     fltValm = *fltVal;
    }
    else
    {
     ASSERT(0);
    }
    p_spin = new CSpinButtonCtrlEx;
    p_capt = new CStatic;
    p_unit = new CStatic;
    return true;
   }

   bool Create(CWnd* parent)
   {
    ASSERT(parent);

    //NOTE: values are in dialog units (not screen pixels)
    int leftOffset = 5;
    int topOffset = 5;
    int groupHeight = 16;
    int captWidth = 125;
    int editWidth = 38;
    int spinWidth = 13;
    int unitWidth = 20;
    int vertClearance = 3;
    //define coordinates
    CRect captRect(leftOffset, topOffset+(groupIdx*(groupHeight+vertClearance)), leftOffset+captWidth, topOffset+(groupIdx*(groupHeight+vertClearance))+groupHeight);
    CRect editRect(captRect.right, captRect.top, captRect.right + editWidth, captRect.bottom);
    CRect spinRect(editRect.right+1, editRect.top, editRect.right + spinWidth, captRect.bottom);
    CRect unitRect(spinRect.right+1, spinRect.top, spinRect.right + unitWidth, captRect.bottom);
    //caption
    ::MapDialogRect(parent->GetSafeHwnd(), &captRect);
    VERIFY(p_capt->Create(caption,WS_CHILD | WS_VISIBLE, captRect, parent, idCntr++));
    p_capt->SetWindowText(caption);
    p_capt->SetFont(parent->GetFont());
    //edit
    ::MapDialogRect(parent->GetSafeHwnd(), &editRect);
    p_edit->CreateEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP|ES_AUTOHSCROLL,
    editRect, parent, idCntr++, NULL); 
    p_edit->ShowWindow(SW_SHOW); 
    p_edit->SetFont(parent->GetFont());
    //spin
    ::MapDialogRect(parent->GetSafeHwnd(), &spinRect);
    VERIFY(p_spin->Create(WS_CHILD|WS_VISIBLE|UDS_ARROWKEYS|UDS_HOTTRACK, spinRect, parent, idCntr++));
    p_spin->ShowWindow(SW_SHOW);
    //unit
    ::MapDialogRect(parent->GetSafeHwnd(), &unitRect);
    VERIFY(p_unit->Create(caption,WS_CHILD | WS_VISIBLE, unitRect, parent, idCntr++));
    p_unit->SetWindowText(unit);
    p_unit->ShowWindow(SW_SHOW);
    p_unit->SetFont(parent->GetFont());
    //increment group index
    groupIdx++;
    //set buddy and limits
    p_spin->SetBuddy(p_edit);
    p_edit->SetLimitText(10);
    p_edit->SetDecimalPlaces(decPls);
    p_spin->SetRangeAndDelta(vMin, vMax, vStp);
    p_edit->SetRange(vMin, vMax);
    return true;
   }

   void DDX_Value(CDataExchange* pDX)
   {
    if (intVal)
     p_edit->DDX_Value(pDX, p_edit->GetDlgCtrlID(), intValm);
    else if (fltVal)
     p_edit->DDX_Value(pDX, p_edit->GetDlgCtrlID(), fltValm);
    else {ASSERT(0);}
   }

   void Apply(void)
   {
    if (intVal)
     *intVal = intValm;
    if (fltVal)
     *fltVal = fltValm;
   }

   CEditEx* p_edit;
   CSpinButtonCtrlEx* p_spin;
   CStatic* p_capt;
   CStatic* p_unit;

   CString caption;
   CString unit;
   int*   intVal;
   float* fltVal;
   int   intValm;
   float fltValm;

   float vMin;
   float vMax;
   float vStp;
   int decPls;

   static int idCntr;
   static int groupIdx;
  };

  std::list<ItemData> m_fl;
  std::auto_ptr<CWndScroller> mp_scr;

  int m_height;
  int m_contentHeight;
};
