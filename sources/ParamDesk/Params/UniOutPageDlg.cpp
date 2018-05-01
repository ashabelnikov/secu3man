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

/** \file UniOutPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/dpiaware.h"
#include "UniOutPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

const UINT CUniOutPageDlg::IDD = IDD_PD_UNIOUT_PAGE;

BEGIN_MESSAGE_MAP(CUniOutPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_BN_CLICKED(IDC_PD_UNIOUT_1_COND1_INV_CHECK, OnChangeInvFlags)
 ON_BN_CLICKED(IDC_PD_UNIOUT_1_COND2_INV_CHECK, OnChangeInvFlags)
 ON_BN_CLICKED(IDC_PD_UNIOUT_2_COND1_INV_CHECK, OnChangeInvFlags)
 ON_BN_CLICKED(IDC_PD_UNIOUT_2_COND2_INV_CHECK, OnChangeInvFlags)
 ON_BN_CLICKED(IDC_PD_UNIOUT_3_COND1_INV_CHECK, OnChangeInvFlags)
 ON_BN_CLICKED(IDC_PD_UNIOUT_3_COND2_INV_CHECK, OnChangeInvFlags)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_1_COND1_COMBO, OnChangeDataOut1Con1)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_1_COND2_COMBO, OnChangeDataOut1Con2)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_2_COND1_COMBO, OnChangeDataOut2Con1)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_2_COND2_COMBO, OnChangeDataOut2Con2)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_3_COND1_COMBO, OnChangeDataOut3Con1)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_3_COND2_COMBO, OnChangeDataOut3Con2)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_1_LOGFUN_COMBO, OnChangeDataLFCombo)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_2_LOGFUN_COMBO, OnChangeDataLFCombo)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_3_LOGFUN_COMBO, OnChangeDataLFCombo)
 ON_CBN_SELCHANGE(IDC_PD_UNIOUT_12_LOGFUN_COMBO, OnChangeDataLF12Combo)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_PD_UNIOUT_1_GROUP, IDC_PD_UNIOUT_1_LOGFUN_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_PD_UNIOUT_2_GROUP, IDC_PD_UNIOUT_2_LOGFUN_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_PD_UNIOUT_3_GROUP, IDC_PD_UNIOUT_3_LOGFUN_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_PD_UNIOUT_1_COND2_COMBO, IDC_PD_UNIOUT_1_COND2_INV_CHECK, OnUpdateControlsOut1Con2)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_PD_UNIOUT_2_COND2_COMBO, IDC_PD_UNIOUT_2_COND2_INV_CHECK, OnUpdateControlsOut2Con2)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_PD_UNIOUT_3_COND2_COMBO, IDC_PD_UNIOUT_3_COND2_INV_CHECK, OnUpdateControlsOut3Con2)
 ON_UPDATE_COMMAND_UI(IDC_PD_UNIOUT_12_LOGFUN_COMBO, OnUpdateControls)
 ON_EN_CHANGE(IDC_PD_UNIOUT_1_COND1_ON_EDIT,  OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_1_COND1_OFF_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_1_COND2_ON_EDIT,  OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_1_COND2_OFF_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_2_COND1_ON_EDIT,  OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_2_COND1_OFF_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_2_COND2_ON_EDIT,  OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_2_COND2_OFF_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_3_COND1_ON_EDIT,  OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_3_COND1_OFF_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_3_COND2_ON_EDIT,  OnChangeData)
 ON_EN_CHANGE(IDC_PD_UNIOUT_3_COND2_OFF_EDIT, OnChangeData)
END_MESSAGE_MAP()

CUniOutPageDlg::CUniOutPageDlg(CWnd* pParent /*=NULL*/)
: Super(CUniOutPageDlg::IDD, pParent)
, m_enabled(false)
, mp_scr(new CWndScroller)
{
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  m_out[i].on_thrd_1_edit.SetOwnDDV(true);
  m_out[i].off_thrd_1_edit.SetOwnDDV(true);
  m_out[i].on_thrd_2_edit.SetOwnDDV(true);
  m_out[i].off_thrd_2_edit.SetOwnDDV(true);
  m_out[i].on_thrd_1_edit.SetMode(CEditEx::MODE_FLOAT);
  m_out[i].off_thrd_1_edit.SetMode(CEditEx::MODE_FLOAT);
  m_out[i].on_thrd_2_edit.SetMode(CEditEx::MODE_FLOAT);
  m_out[i].off_thrd_2_edit.SetMode(CEditEx::MODE_FLOAT);

  //fill parameters with default values
  m_params.out[i].condition1 = SECU3IO::UNIOUT_COND_CTS;
  m_params.out[i].condition2 = SECU3IO::UNIOUT_COND_CTS;
  m_params.out[i].invers_1 = false;
  m_params.out[i].invers_2 = false;
  m_params.out[i].logicFunc = SECU3IO::UNIOUT_LF_OR;
  m_params.out[i].on_thrd_1 = 55.0f;
  m_params.out[i].off_thrd_1 = 50.0f;
  m_params.out[i].on_thrd_2 = 55.0f;
  m_params.out[i].off_thrd_2 = 50.0f;
 }
 m_params.logicFunc12 = SECU3IO::UNIOUT_LF_OR;

 m_lf_str.insert(std::make_pair(SECU3IO::UNIOUT_LF_OR, MLL::GetString(IDS_UNIOUT_LF_OR)));
 m_lf_str.insert(std::make_pair(SECU3IO::UNIOUT_LF_AND, MLL::GetString(IDS_UNIOUT_LF_AND)));
 m_lf_str.insert(std::make_pair(SECU3IO::UNIOUT_LF_XOR, MLL::GetString(IDS_UNIOUT_LF_XOR)));
 m_lf_str.insert(std::make_pair(SECU3IO::UNIOUT_LF_2ND, MLL::GetString(IDS_UNIOUT_LF_2ND)));
 m_lf_str.insert(std::make_pair(SECU3IO::UNIOUT_LF_NONE, MLL::GetString(IDS_UNIOUT_LF_NONE)));

 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_CTS,  CondFmt(6, 2, -40.0f, 180.0f, 0.25f, 55.0f, 50.0f, MLL::GetString(IDS_UNIOUT_COND_CTS), true)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_RPM,  CondFmt(5, 0, 50.0f, 20000.0f, 10.0f, 1500.0f, 1200.0f, MLL::GetString(IDS_UNIOUT_COND_RPM), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_MAP,  CondFmt(5, 2, 0.25f, 500.0f, 0.25f, 95.0f, 90.0f, MLL::GetString(IDS_UNIOUT_COND_MAP), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_UBAT, CondFmt(4, 1, 5.0f, 16.0f, 0.1f, 10.0f, 10.5f, MLL::GetString(IDS_UNIOUT_COND_UBAT), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_CARB, CondFmt(1, 0, .0f, 1.0f, 1.0f, .0f, 1.0f, MLL::GetString(IDS_UNIOUT_COND_CARB), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_VSPD, CondFmt(5, 1, 5.0f, 250.0f, 0.1f, 70.0f, 65.0f, MLL::GetString(IDS_UNIOUT_COND_VSPD), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_AIRFL, CondFmt(2, 0, .0f, 16.0f, 1.0f, 13.0f, 12.0f, MLL::GetString(IDS_UNIOUT_COND_AIRFL), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_TMR, CondFmt(5, 1, .0f, 600.0f, 0.1f, .0f, 5.0f, MLL::GetString(IDS_UNIOUT_COND_TMR), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_ITTMR, CondFmt(5, 1, .0f, 600.0f, 0.1f, .0f, 5.0f, MLL::GetString(IDS_UNIOUT_COND_ITTMR), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_ESTMR, CondFmt(5, 1, .0f, 600.0f, 0.1f, .0f, 5.0f, MLL::GetString(IDS_UNIOUT_COND_ESTMR), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_CPOS, CondFmt(5, 1, .0f, 100.0f, 0.5f, 60.0f, 55.0f, MLL::GetString(IDS_UNIOUT_COND_CPOS), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_AANG, CondFmt(5, 1, -15.0f, 65.0f, 0.1f, 55.0f, 53.0f, MLL::GetString(IDS_UNIOUT_COND_AANG), true)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_KLEV, CondFmt(4, 2, .0f, 5.00f, 0.01f, 2.5f, 2.45f, MLL::GetString(IDS_UNIOUT_COND_KLEV), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_TPS, CondFmt(5, 1, .0f, 100.0f, 0.5f, 30.0f, 29.0f, MLL::GetString(IDS_UNIOUT_COND_TPS), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_ATS, CondFmt(5, 2, -40.0f, 180.0f, 0.25f, 55.0f, 50.0f, MLL::GetString(IDS_UNIOUT_COND_ATS), true)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_AI1, CondFmt(4, 2, .0f, 5.00f, 0.01f, 2.5f, 2.48f, MLL::GetString(IDS_UNIOUT_COND_AI1), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_AI2, CondFmt(4, 2, .0f, 600.0f, 0.1f, 2.5f, 2.48f, MLL::GetString(IDS_UNIOUT_COND_AI2), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_GASV, CondFmt(1, 0, .0f, 1.0f, 1.0f, .0f, 1.0f, MLL::GetString(IDS_UNIOUT_COND_GASV), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_IPW, CondFmt(6, 2, 0.01f, 200.0f, 0.01f, 20.0f, 19.90f, MLL::GetString(IDS_UNIOUT_COND_IPW), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_CE, CondFmt(1, 0, .0f, 1.0f, 1.0f, .0f, 1.0f, MLL::GetString(IDS_UNIOUT_COND_CE), false)));
 m_condFmt.insert(std::make_pair(SECU3IO::UNIOUT_COND_OFTMR, CondFmt(5, 1, .0f, 600.0f, 0.1f, .0f, 5.0f, MLL::GetString(IDS_UNIOUT_COND_OFTMR), false)));
}

LPCTSTR CUniOutPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CUniOutPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  int idoff = i * (IDC_PD_UNIOUT_2_COND1_COMBO - IDC_PD_UNIOUT_1_COND1_COMBO);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND1_COMBO + idoff, m_out[i].cond1_combo);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND1_ON_EDIT + idoff, m_out[i].on_thrd_1_edit);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND1_ON_SPIN + idoff, m_out[i].on_thrd_1_spin);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND1_OFF_EDIT + idoff, m_out[i].off_thrd_1_edit);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND1_OFF_SPIN + idoff, m_out[i].off_thrd_1_spin);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND1_INV_CHECK + idoff, m_out[i].inv1_check);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_LOGFUN_COMBO + idoff, m_out[i].lf_combo); //logic function
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND2_COMBO + idoff, m_out[i].cond2_combo);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND2_ON_EDIT + idoff, m_out[i].on_thrd_2_edit);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND2_ON_SPIN + idoff, m_out[i].on_thrd_2_spin);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND2_OFF_EDIT + idoff, m_out[i].off_thrd_2_edit);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND2_OFF_SPIN + idoff, m_out[i].off_thrd_2_spin);
  DDX_Control(pDX, IDC_PD_UNIOUT_1_COND2_INV_CHECK + idoff, m_out[i].inv2_check);

  m_out[i].on_thrd_1_edit.DDX_Value(pDX, IDC_PD_UNIOUT_1_COND1_ON_EDIT + idoff, m_params.out[i].on_thrd_1);
  m_out[i].off_thrd_1_edit.DDX_Value(pDX, IDC_PD_UNIOUT_1_COND1_OFF_EDIT + idoff, m_params.out[i].off_thrd_1);
  m_out[i].on_thrd_2_edit.DDX_Value(pDX, IDC_PD_UNIOUT_1_COND2_ON_EDIT + idoff, m_params.out[i].on_thrd_2);
  m_out[i].off_thrd_2_edit.DDX_Value(pDX, IDC_PD_UNIOUT_1_COND2_OFF_EDIT + idoff, m_params.out[i].off_thrd_2);

  DDX_Check_bool(pDX, IDC_PD_UNIOUT_1_COND1_INV_CHECK + idoff, m_params.out[i].invers_1);
  DDX_Check_bool(pDX, IDC_PD_UNIOUT_1_COND2_INV_CHECK + idoff, m_params.out[i].invers_2);
 }
 DDX_Control(pDX, IDC_PD_UNIOUT_12_LOGFUN_COMBO, lf12_combo); //logic function for 1st and 2nd outputs
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CUniOutPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CUniOutPageDlg::OnUpdateControlsOut1Con2(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_params.out[0].logicFunc != SECU3IO::UNIOUT_LF_NONE);
}

void CUniOutPageDlg::OnUpdateControlsOut2Con2(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_params.out[1].logicFunc != SECU3IO::UNIOUT_LF_NONE);
}

void CUniOutPageDlg::OnUpdateControlsOut3Con2(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_params.out[2].logicFunc != SECU3IO::UNIOUT_LF_NONE);
}

/////////////////////////////////////////////////////////////////////////////
// CUniOutPageDlg message handlers

BOOL CUniOutPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 _FillConditionComboBoxes();
 _FillLogicFuncComboBoxes();
 _FillLogicFunc12ComboBox();

 _SetCondComboBoxesSel();
 _SetLogicFuncComboBoxesSel();
 _SetLogicFunc12ComboBoxSel();

 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  m_out[i].on_thrd_1_spin.SetBuddy(&m_out[i].on_thrd_1_edit);
  m_out[i].off_thrd_1_spin.SetBuddy(&m_out[i].off_thrd_1_edit);
  m_out[i].on_thrd_2_spin.SetBuddy(&m_out[i].on_thrd_2_edit);
  m_out[i].off_thrd_2_spin.SetBuddy(&m_out[i].off_thrd_2_edit);
 }

 //initialize window scroller
 mp_scr->Init(this);

//create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  VERIFY(mp_ttc->AddWindow(&m_out[i].on_thrd_1_edit, MLL::GetString(IDS_PD_UNIOUT_COND_ON_EDIT_DEF_TT)));
  VERIFY(mp_ttc->AddWindow(&m_out[i].off_thrd_1_edit, MLL::GetString(IDS_PD_UNIOUT_COND_OFF_EDIT_DEF_TT)));
  VERIFY(mp_ttc->AddWindow(&m_out[i].on_thrd_2_edit, MLL::GetString(IDS_PD_UNIOUT_COND_ON_EDIT_DEF_TT)));
  VERIFY(mp_ttc->AddWindow(&m_out[i].off_thrd_2_edit, MLL::GetString(IDS_PD_UNIOUT_COND_OFF_EDIT_DEF_TT)));
  VERIFY(mp_ttc->AddWindow(&m_out[i].on_thrd_1_spin, MLL::GetString(IDS_PD_UNIOUT_COND_ON_EDIT_DEF_TT)));
  VERIFY(mp_ttc->AddWindow(&m_out[i].off_thrd_1_spin, MLL::GetString(IDS_PD_UNIOUT_COND_OFF_EDIT_DEF_TT)));
  VERIFY(mp_ttc->AddWindow(&m_out[i].on_thrd_2_spin, MLL::GetString(IDS_PD_UNIOUT_COND_ON_EDIT_DEF_TT)));
  VERIFY(mp_ttc->AddWindow(&m_out[i].off_thrd_2_spin, MLL::GetString(IDS_PD_UNIOUT_COND_OFF_EDIT_DEF_TT)));

  VERIFY(mp_ttc->AddWindow(&m_out[i].lf_combo, MLL::GetString(IDS_PD_UNIOUT_LOGFUN_COMBO_TT)));

  //finally, set condition format
  _SetCondInputFormat(i, false), _SetCondInputFormat(i, true);
 }

 VERIFY(mp_ttc->AddWindow(&lf12_combo, MLL::GetString(IDS_PD_UNIOUT_12_LOGFUN_COMBO_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CUniOutPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CUniOutPageDlg::OnChangeInvFlags(void)
{
 UpdateData();
 OnChangeNotify();
}

void CUniOutPageDlg::OnChangeDataOut1Con1(void)
{
 _SetCondInputFormat(0, false);
 UpdateData(FALSE);
 UpdateData();
 OnChangeNotify();
}
void CUniOutPageDlg::OnChangeDataOut1Con2(void)
{
 _SetCondInputFormat(0, true);
 UpdateData(FALSE);
 UpdateData();
 OnChangeNotify();
}
void CUniOutPageDlg::OnChangeDataOut2Con1(void)
{
 _SetCondInputFormat(1, false);
 UpdateData(FALSE);
 UpdateData();
 OnChangeNotify();
}
void CUniOutPageDlg::OnChangeDataOut2Con2(void)
{
 _SetCondInputFormat(1, true);
 UpdateData(FALSE);
 UpdateData();
 OnChangeNotify();
}
void CUniOutPageDlg::OnChangeDataOut3Con1(void)
{
 _SetCondInputFormat(2, false);
 UpdateData(FALSE);
 UpdateData();
 OnChangeNotify();
}
void CUniOutPageDlg::OnChangeDataOut3Con2(void)
{
 _SetCondInputFormat(2, true);
 UpdateData(FALSE);
 UpdateData();
 OnChangeNotify();
}

void CUniOutPageDlg::OnChangeDataLFCombo(void)
{
 UpdateData();
 _GetLogicFuncComboBoxesSel();
 OnChangeNotify();
 UpdateDialogControls(this, TRUE);
}

void CUniOutPageDlg::OnChangeDataLF12Combo(void)
{
 UpdateData();
 _GetLogicFunc12ComboBoxSel();
 OnChangeNotify();
 UpdateDialogControls(this, TRUE);
}

void CUniOutPageDlg::OnChangeData(void)
{
 UpdateData();
 OnChangeNotify();
}

//разрешение/запрещение контроллов (всех поголовно)
void CUniOutPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CUniOutPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CUniOutPageDlg::GetValues(SECU3IO::UniOutPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog tp variables
 _GetCondComboBoxesSel();
 _GetLogicFuncComboBoxesSel();
 _GetLogicFunc12ComboBoxSel();
 memcpy(o_values, &m_params, sizeof(SECU3IO::UniOutPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CUniOutPageDlg::SetValues(const SECU3IO::UniOutPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params, i_values, sizeof(SECU3IO::UniOutPar));

 _SetCondComboBoxesSel();
 _SetLogicFuncComboBoxesSel();
 _SetLogicFunc12ComboBoxSel();

 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {_SetCondInputFormat(i, false, false); _SetCondInputFormat(i, true, false);}

 UpdateData(FALSE); //copy data from variables to dialog
 UpdateDialogControls(this, TRUE);
}

void CUniOutPageDlg::_FillConditionComboBoxes(void)
{
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  std::map<int, CondFmt>::const_iterator it = m_condFmt.begin();
  for(int index; it != m_condFmt.end(); ++it)
  {
   if (it->first != SECU3IO::UNIOUT_COND_TMR)
   {
    index = m_out[i].cond1_combo.AddString(it->second.condstr.c_str());
    if (index==CB_ERR)
    {
     ASSERT(0);
     continue;
    }
    m_out[i].cond1_combo.SetItemData(index, it->first);
   }

   index = m_out[i].cond2_combo.AddString(it->second.condstr.c_str());
   if (index==CB_ERR)
   {
    ASSERT(0);
    continue;
   }
   m_out[i].cond2_combo.SetItemData(index, it->first);
  }
 }
}

void CUniOutPageDlg::_FillLogicFuncComboBoxes(void)
{
for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  std::map<int, _TSTRING>::const_iterator it = m_lf_str.begin();
  for(; it != m_lf_str.end(); ++it)
  {
   int index = m_out[i].lf_combo.AddString(it->second.c_str());
   if (index==CB_ERR)
   {
    ASSERT(0);
    continue;
   }
   m_out[i].lf_combo.SetItemData(index, it->first);
  }
 }
}

void CUniOutPageDlg::_FillLogicFunc12ComboBox(void)
{
 std::map<int, _TSTRING>::const_iterator it = m_lf_str.begin();
 for(; it != m_lf_str.end(); ++it)
 {
  int index = lf12_combo.AddString(it->second.c_str());
  if (index==CB_ERR)
  {
   ASSERT(0);
   continue;
  }
  lf12_combo.SetItemData(index, it->first);
 }
}

void CUniOutPageDlg::_SetCondInputFormat(int outIndex, bool cond, bool setdefval /*= true*/)
{
 CComboBox& cb = cond ? m_out[outIndex].cond2_combo : m_out[outIndex].cond1_combo;
 int cb_index = cb.GetCurSel();
 if (cb_index == CB_ERR)
 {
  ASSERT(0);
  return;
 }
 int condition = cb.GetItemData(cb_index);

 std::map<int, CondFmt>::const_iterator it = m_condFmt.find(condition);
 if (it == m_condFmt.end())
  return;

 (cond?m_out[outIndex].on_thrd_2_edit:m_out[outIndex].on_thrd_1_edit).SetLimitText(it->second.textlim);
 (cond?m_out[outIndex].on_thrd_2_edit:m_out[outIndex].on_thrd_1_edit).SetDecimalPlaces(it->second.decplaces);
 (cond?m_out[outIndex].on_thrd_2_edit:m_out[outIndex].on_thrd_1_edit).SetMode(CEditEx::MODE_FLOAT | (it->second.sign?CEditEx::MODE_SIGNED:0));
 (cond?m_out[outIndex].on_thrd_2_spin:m_out[outIndex].on_thrd_1_spin).SetRangeAndDelta(it->second.minimum, it->second.maximum, it->second.delta);
 (cond?m_out[outIndex].on_thrd_2_edit:m_out[outIndex].on_thrd_1_edit).SetRange(it->second.minimum, it->second.maximum);

 (cond?m_out[outIndex].off_thrd_2_edit:m_out[outIndex].off_thrd_1_edit).SetLimitText(it->second.textlim);
 (cond?m_out[outIndex].off_thrd_2_edit:m_out[outIndex].off_thrd_1_edit).SetDecimalPlaces(it->second.decplaces);
 (cond?m_out[outIndex].off_thrd_2_edit:m_out[outIndex].off_thrd_1_edit).SetMode(CEditEx::MODE_FLOAT | (it->second.sign?CEditEx::MODE_SIGNED:0));
 (cond?m_out[outIndex].off_thrd_2_spin:m_out[outIndex].off_thrd_1_spin).SetRangeAndDelta(it->second.minimum, it->second.maximum, it->second.delta);
 (cond?m_out[outIndex].off_thrd_2_edit:m_out[outIndex].off_thrd_1_edit).SetRange(it->second.minimum, it->second.maximum);

 if (setdefval)
 {
 (cond?m_params.out[outIndex].on_thrd_2:m_params.out[outIndex].on_thrd_1) = it->second.defval_on;
 (cond?m_params.out[outIndex].off_thrd_2:m_params.out[outIndex].off_thrd_1) = it->second.defval_off;
 }

 //Change default tooltip text depending on selected condition
 if (true==cond && condition==SECU3IO::UNIOUT_COND_TMR)
 {
  mp_ttc->UpdateText(&m_out[outIndex].off_thrd_2_edit, MLL::GetString(IDS_PD_UNIOUT_COND_TMR_OFF_EDIT_TT));
  mp_ttc->UpdateText(&m_out[outIndex].off_thrd_2_spin, MLL::GetString(IDS_PD_UNIOUT_COND_TMR_OFF_EDIT_TT));
 }
}

void CUniOutPageDlg::_SetCondComboBoxesSel(void)
{
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  int count = m_out[i].cond1_combo.GetCount();
  for(int ii = 0; ii < count; ++ii)
  {
   if (m_out[i].cond1_combo.GetItemData(ii) == m_params.out[i].condition1)
    m_out[i].cond1_combo.SetCurSel(ii);
  }
  count = m_out[i].cond2_combo.GetCount();
  for(int ii = 0; ii < count; ++ii)
  {
   if (m_out[i].cond2_combo.GetItemData(ii) == m_params.out[i].condition2)
    m_out[i].cond2_combo.SetCurSel(ii);
  }
 }
}

void CUniOutPageDlg::_GetCondComboBoxesSel(void)
{
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  int index = m_out[i].cond1_combo.GetCurSel();
  m_params.out[i].condition1 = (unsigned char)m_out[i].cond1_combo.GetItemData(index);
  index = m_out[i].cond2_combo.GetCurSel();
  m_params.out[i].condition2 = (unsigned char)m_out[i].cond2_combo.GetItemData(index);
 }
}

void CUniOutPageDlg::_SetLogicFuncComboBoxesSel(void)
{
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  int count = m_out[i].lf_combo.GetCount();
  for(int ii = 0; ii < count; ++ii)
  {
   if (m_out[i].lf_combo.GetItemData(ii) == m_params.out[i].logicFunc)
    m_out[i].lf_combo.SetCurSel(ii);
  }
 }
}

void CUniOutPageDlg::_GetLogicFuncComboBoxesSel(void)
{
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  int index = m_out[i].lf_combo.GetCurSel();
  m_params.out[i].logicFunc = (unsigned char)m_out[i].lf_combo.GetItemData(index);
 }
}

void CUniOutPageDlg::_SetLogicFunc12ComboBoxSel(void)
{
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  int count = lf12_combo.GetCount();
  for(int ii = 0; ii < count; ++ii)
  {
   if (lf12_combo.GetItemData(ii) == m_params.logicFunc12)
    lf12_combo.SetCurSel(ii);
  }
 }
}

void CUniOutPageDlg::_GetLogicFunc12ComboBoxSel(void)
{
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
 {
  int index = lf12_combo.GetCurSel();
  m_params.logicFunc12 = (unsigned char)lf12_combo.GetItemData(index);
 }
}

void CUniOutPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(520));
}
