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

/** \file KnockFrqCalcDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "KnockFrqCalcDlg.h"

#include "common/FastDelegate.h"
#include "common/DPIaware.h"
#include "ui-core/DDX_helpers.h"
#include "ui-core/EditEx.h"
#include "ui-core/ToolTipCtrlEx.h"

using namespace fastdelegate;

const UINT CKnockFrqCalcDlg::IDD = IDD_KC_KNOCK_FRQ_CALC;

const float MIN_CYL_DIAMETER = 28.7f;   //mm
const float MAX_CYL_DIAMETER = 470.0f;  //mm

//I hate math.h
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

//We need this descendant to have ability to handle Enter key press
class CEditExWithEnter : public CEditEx
{
  typedef fastdelegate::FastDelegate0<> EventHandler;
  void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
   CEditEx::OnChar(nChar, nRepCnt, nFlags);
   if (nChar==VK_RETURN)
    m_onEnter();
  }

  UINT OnGetDlgCode()
  {
   const MSG *pMsg = GetCurrentMessage();
   pMsg = ((const MSG *)pMsg->lParam);

   if (pMsg != NULL)
   {
    if (pMsg->message == WM_KEYDOWN)
    {
     if (pMsg->wParam == VK_TAB) //don't eat TAB key, othewise we will brake TABSTOP mechanism
      return 0;
    }
   }
   return CEditEx::OnGetDlgCode() | DLGC_WANTALLKEYS;
  }

  EventHandler m_onEnter;
  DECLARE_MESSAGE_MAP()
 public:
  CEditExWithEnter(const EventHandler& OnEnter) : CEditEx(CEditEx::MODE_FLOAT), m_onEnter(OnEnter) {}
};

BEGIN_MESSAGE_MAP(CEditExWithEnter, CEditEx)
 ON_WM_CHAR()
 ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

//We only save value of this pointer (we do not access members), so we can ignore warning.
#pragma warning( disable : 4355 ) // : 'this' : used in base member initializer list

CKnockFrqCalcDlg::CKnockFrqCalcDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockFrqCalcDlg::IDD, pParent)
, mp_cyl_d_edit(new CEditExWithEnter(MakeDelegate(this, &CKnockFrqCalcDlg::OnFrqCalcButton)))
, m_cyl_d(80.0f)
{
 //empty
}

void CKnockFrqCalcDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_KC_KNOCK_FRQ_CALC_BTN, m_calc_frq_btn);
 DDX_Control(pDX, IDC_KC_KNOCK_FRQ_CALC_EDIT, *mp_cyl_d_edit);

 mp_cyl_d_edit->DDX_Value(pDX, IDC_KC_KNOCK_FRQ_CALC_EDIT, m_cyl_d);
}

BEGIN_MESSAGE_MAP(CKnockFrqCalcDlg, Super)
 ON_BN_CLICKED(IDC_KC_KNOCK_FRQ_CALC_BTN, OnFrqCalcButton)
END_MESSAGE_MAP()

BOOL CKnockFrqCalcDlg::OnInitDialog()
{
 Super::OnInitDialog();
 DPIAware da;
 int dpi = da.GetDPIX();
 if (dpi >= 192) //192 DPI or more
 {
  m_calc_frq_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP200), MAKEINTRESOURCE(IDB_CALC_DOWN200), 
                             MAKEINTRESOURCE(IDB_CALC_FOCUSED200), MAKEINTRESOURCE(IDB_CALC_DISABLED200));
 }
 else if (dpi >= 144)  //144 to 192 DPI
 {
  m_calc_frq_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP150), MAKEINTRESOURCE(IDB_CALC_DOWN150), 
                             MAKEINTRESOURCE(IDB_CALC_FOCUSED150), MAKEINTRESOURCE(IDB_CALC_DISABLED150));
 }
 else //below 144 DPI
 {
  m_calc_frq_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                             MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));
 }
 mp_cyl_d_edit->SetDecimalPlaces(1);
 mp_cyl_d_edit->SetLimitText(5);
 mp_cyl_d_edit->SetValue(m_cyl_d);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(this, MLL::GetString(IDS_KC_KNOCK_FRQ_CALC_TT)));
 VERIFY(mp_ttc->AddWindow(mp_cyl_d_edit.get(), MLL::GetString(IDS_KC_KNOCK_FRQ_CALC_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_calc_frq_btn, MLL::GetString(IDS_KC_KNOCK_FRQ_CALC_BTN_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;
}

void CKnockFrqCalcDlg::OnFrqCalcButton()
{
 UpdateData(TRUE);
 //check limits and correct value if required
 if (m_cyl_d > MAX_CYL_DIAMETER)
 {
  m_cyl_d = MAX_CYL_DIAMETER;
  UpdateData(FALSE);
 }
 if (m_cyl_d < MIN_CYL_DIAMETER)
 {
  m_cyl_d = MIN_CYL_DIAMETER;
  UpdateData(FALSE);
 }
 //calculate approximate frequency
 float frq = 900.0f / ((float)M_PI * (m_cyl_d / 2.0f));
 //delegate result
 if (m_OnCalculate)
  m_OnCalculate(frq);
}

void CKnockFrqCalcDlg::setOnCalculate(EventHandler OnFunction)
{
 m_OnCalculate = OnFunction;
}
