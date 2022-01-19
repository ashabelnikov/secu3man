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

/** \file ChokePageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/dpiaware.h"
#include "ChokePageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

const UINT CChokePageDlg::IDD = IDD_PD_CHOKE_PAGE;

BEGIN_MESSAGE_MAP(CChokePageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_EN_CHANGE(IDC_PD_CHOKE_SM_STEPS_NUM_EDIT, OnChangePdChokeSMStepsNumEdit)
 ON_EN_CHANGE(IDC_PD_CHOKE_STRT_ADD_TM1_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CHOKE_STRT_ADD_TM2_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CHOKE_RPMREG_IF_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_CRANKTORUNTIME_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_CHOKE_SM_TEST_CHECK, OnSMTestButton)
 ON_BN_CLICKED(IDC_PD_CHOKE_OFFSTRTADDONGAS_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_CHOKE_OFFRPMREGONGAS_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_CHOKE_USETHROTTLEPOS_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_CHOKE_MAXFREQINIT_CHECK, OnChangeData)

 ON_CBN_SELCHANGE(IDC_PD_CHOKE_SM_FREQ_COMBO, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_TEST_CHECK,OnUpdateChokeTestBtn)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_MANUAL_POS_SPIN,OnUpdateChokeManPosBtn)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_MANUAL_POS_CAPTION,OnUpdateChokeManPosBtn)
 ON_NOTIFY(UDN_DELTAPOS, IDC_PD_CHOKE_MANUAL_POS_SPIN, OnManualDeltapos)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_TM1_EDIT,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_TM1_SPIN,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_TM1_UNIT,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_TM1_CAPTION,OnUpdateControlsCarb)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_TM2_EDIT,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_TM2_SPIN,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_TM2_UNIT,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_TM2_CAPTION,OnUpdateControlsCarb)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_IF_EDIT,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_IF_SPIN,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_IF_CAPTION,OnUpdateControlsCarb)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_TITLE,OnUpdateControlsCarb)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_OFFSTRTADDONGAS_CHECK, OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_OFFRPMREGONGAS_CHECK, OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_USETHROTTLEPOS_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_MAXFREQINIT_CHECK, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_FREQ_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_FREQ_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_CRANKTORUNTIME_SPIN,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_CRANKTORUNTIME_CAPTION,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_CRANKTORUNTIME_UNIT,OnUpdateControlsCarb)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_CRANKTORUNTIME_EDIT,OnUpdateControlsCarb)
END_MESSAGE_MAP()

CChokePageDlg::CChokePageDlg(CWnd* pParent /*=NULL*/)
: Super(CChokePageDlg::IDD, pParent)
, m_enabled(false)
, m_fuel_injection(false)
, m_sm_steps_num_edit(CEditEx::MODE_INT, true)
, m_strt_add_tm1_edit(CEditEx::MODE_FLOAT, true)
, m_strt_add_tm2_edit(CEditEx::MODE_FLOAT, true)
, m_rpmreg_if_edit(CEditEx::MODE_FLOAT, true)
, m_cranktoruntime_edit(CEditEx::MODE_FLOAT, true)
, m_choketst_enabled(false)
, m_chokemanpos_enabled(false)
, m_lock_ui_update(false) //UI update is not locked
, mp_scr(new CWndScroller)
{
 m_params.sm_steps = 700;
 m_params.testing = 0;
 m_params.manual_delta = 0;  //See also code in PMParamsController.cpp
 m_params.choke_rpm_if = 0.05f;
 m_params.choke_corr_time[0] = 5.0f;
 m_params.choke_corr_time[1] = 2.5f;
 m_params.useclrpmreg = false;
 m_params.offrpmreg_ongas = false;
 m_params.usethrottle_pos = false;
 m_params.sm_freq = 0;
 m_params.sm_maxfreqinit = false;
 m_params.inj_cranktorun_time = 3.00f;
}

LPCTSTR CChokePageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CChokePageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_CHOKE_SM_STEPS_NUM_SPIN, m_sm_steps_num_spin);
 DDX_Control(pDX, IDC_PD_CHOKE_SM_STEPS_NUM_EDIT, m_sm_steps_num_edit);
 DDX_Control(pDX, IDC_PD_CHOKE_SM_TEST_CHECK, m_sm_test_check);
 DDX_Control(pDX, IDC_PD_CHOKE_MANUAL_POS_SPIN, m_man_ctrl_spin);

 DDX_Control(pDX, IDC_PD_CHOKE_STRT_ADD_TM1_SPIN, m_strt_add_tm1_spin);
 DDX_Control(pDX, IDC_PD_CHOKE_STRT_ADD_TM1_EDIT, m_strt_add_tm1_edit);

 DDX_Control(pDX, IDC_PD_CHOKE_STRT_ADD_TM2_SPIN, m_strt_add_tm2_spin);
 DDX_Control(pDX, IDC_PD_CHOKE_STRT_ADD_TM2_EDIT, m_strt_add_tm2_edit);

 DDX_Control(pDX, IDC_PD_CHOKE_RPMREG_IF_EDIT, m_rpmreg_if_edit);
 DDX_Control(pDX, IDC_PD_CHOKE_RPMREG_IF_SPIN, m_rpmreg_if_spin);

 DDX_Control(pDX, IDC_PD_STARTER_CRANKTORUNTIME_EDIT, m_cranktoruntime_edit);
 DDX_Control(pDX, IDC_PD_STARTER_CRANKTORUNTIME_SPIN, m_cranktoruntime_spin);

 DDX_Control(pDX, IDC_PD_CHOKE_OFFSTRTADDONGAS_CHECK, m_offstrtadd_ongas_check);
 DDX_Control(pDX, IDC_PD_CHOKE_OFFRPMREGONGAS_CHECK, m_offrpmreg_ongas_check);
 DDX_Control(pDX, IDC_PD_CHOKE_USETHROTTLEPOS_CHECK, m_usethrottle_pos_check);
 DDX_Control(pDX, IDC_PD_CHOKE_MAXFREQINIT_CHECK, m_sm_maxfreqinit_check);

 DDX_Control(pDX, IDC_PD_CHOKE_SM_FREQ_COMBO, m_sm_freq_combo);

 m_sm_steps_num_edit.DDX_Value(pDX, IDC_PD_CHOKE_SM_STEPS_NUM_EDIT, m_params.sm_steps);
 m_strt_add_tm1_edit.DDX_Value(pDX, IDC_PD_CHOKE_STRT_ADD_TM1_EDIT, m_params.choke_corr_time[0]);
 m_strt_add_tm2_edit.DDX_Value(pDX, IDC_PD_CHOKE_STRT_ADD_TM2_EDIT, m_params.choke_corr_time[1]);
 m_rpmreg_if_edit.DDX_Value(pDX, IDC_PD_CHOKE_RPMREG_IF_EDIT, m_params.choke_rpm_if);
 m_cranktoruntime_edit.DDX_Value(pDX, IDC_PD_STARTER_CRANKTORUNTIME_EDIT, m_params.inj_cranktorun_time);
 DDX_Check_UCHAR(pDX, IDC_PD_CHOKE_SM_TEST_CHECK, m_params.testing);

 DDX_Check_bool(pDX, IDC_PD_CHOKE_OFFSTRTADDONGAS_CHECK, m_params.useclrpmreg);
 DDX_Check_bool(pDX, IDC_PD_CHOKE_OFFRPMREGONGAS_CHECK, m_params.offrpmreg_ongas);
 DDX_Check_bool(pDX, IDC_PD_CHOKE_USETHROTTLEPOS_CHECK, m_params.usethrottle_pos);
 DDX_Check_bool(pDX, IDC_PD_CHOKE_MAXFREQINIT_CHECK, m_params.sm_maxfreqinit);

 DDX_CBIndex_int(pDX, IDC_PD_CHOKE_SM_FREQ_COMBO, m_params.sm_freq);
}

//Update state of controls
void CChokePageDlg::OnUpdateControlsCarb(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_fuel_injection);
}

//Update TEST button state
void CChokePageDlg::OnUpdateChokeTestBtn(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_choketst_enabled);
}

//Update "Manual postion control"
void CChokePageDlg::OnUpdateChokeManPosBtn(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_chokemanpos_enabled);
}

void CChokePageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CChokePageDlg message handlers

BOOL CChokePageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_sm_steps_num_edit.SetLimitText(4);
 m_sm_steps_num_spin.SetBuddy(&m_sm_steps_num_edit);
 m_sm_steps_num_spin.SetRangeAndDelta(50, 2000, 1);
 m_sm_steps_num_edit.SetRange(50, 2000);

 m_strt_add_tm1_edit.SetLimitText(4);
 m_strt_add_tm1_spin.SetBuddy(&m_strt_add_tm1_edit);
 m_strt_add_tm1_edit.SetDecimalPlaces(1);
 m_strt_add_tm1_spin.SetRangeAndDelta(0.1f, 10.0f, 0.1f);
 m_strt_add_tm1_edit.SetRange(0.1f, 10.0f);

 m_strt_add_tm2_edit.SetLimitText(4);
 m_strt_add_tm2_spin.SetBuddy(&m_strt_add_tm2_edit);
 m_strt_add_tm2_edit.SetDecimalPlaces(1);
 m_strt_add_tm2_spin.SetRangeAndDelta(0.1f, 10.0f, 0.1f);
 m_strt_add_tm2_edit.SetRange(0.1f, 10.0f);

 m_rpmreg_if_edit.SetLimitText(4);
 m_rpmreg_if_spin.SetBuddy(&m_rpmreg_if_edit);
 m_rpmreg_if_edit.SetDecimalPlaces(2);
 m_rpmreg_if_spin.SetRangeAndDelta(0.01f, 1.0f, 0.01f);
 m_rpmreg_if_edit.SetRange(0.01f, 1.0f);

 m_cranktoruntime_edit.SetLimitText(5);
 m_cranktoruntime_spin.SetBuddy(&m_cranktoruntime_edit);
 m_cranktoruntime_edit.SetDecimalPlaces(2);
 m_cranktoruntime_spin.SetRangeAndDelta(0.10f,99.00f, 0.01f);
 m_cranktoruntime_edit.SetRange(0.10f, 99.00f);

 m_man_ctrl_spin.SetBuddy(&m_man_ctrl_spin); //loves himself

 //-----------------------------------------------------------------
 //fill combobox containing STEP frequences
 m_sm_freq_combo.AddString(_T("300"));
 m_sm_freq_combo.AddString(_T("150"));
 m_sm_freq_combo.AddString(_T("100"));
 m_sm_freq_combo.AddString(_T("75"));
 //-----------------------------------------------------------------

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_sm_steps_num_edit, MLL::GetString(IDS_PD_CHOKE_SM_STEPS_NUM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_sm_steps_num_spin, MLL::GetString(IDS_PD_CHOKE_SM_STEPS_NUM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_sm_test_check, MLL::GetString(IDS_PD_CHOKE_SM_TEST_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_man_ctrl_spin, MLL::GetString(IDS_PD_CHOKE_MANUAL_POS_SPIN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_strt_add_tm1_edit, MLL::GetString(IDS_PD_CHOKE_STRT_ADD_TM1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_strt_add_tm1_spin, MLL::GetString(IDS_PD_CHOKE_STRT_ADD_TM1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_strt_add_tm2_edit, MLL::GetString(IDS_PD_CHOKE_STRT_ADD_TM2_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_strt_add_tm2_spin, MLL::GetString(IDS_PD_CHOKE_STRT_ADD_TM2_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rpmreg_if_edit, MLL::GetString(IDS_PD_CHOKE_RPMREG_IF_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rpmreg_if_spin, MLL::GetString(IDS_PD_CHOKE_RPMREG_IF_EDIT_TT)));
 //check boxes
 VERIFY(mp_ttc->AddWindow(&m_offstrtadd_ongas_check, MLL::GetString(IDS_PD_CHOKE_OFFSTRTADDONGAS_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_offrpmreg_ongas_check, MLL::GetString(IDS_PD_CHOKE_OFFRPMREGONGAS_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_usethrottle_pos_check, MLL::GetString(IDS_PD_CHOKE_USETHROTTLEPOS_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_sm_maxfreqinit_check, MLL::GetString(IDS_PD_CHOKE_MAXFREQINIT_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_sm_freq_combo, MLL::GetString(IDS_PD_CHOKE_SM_FREQ_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cranktoruntime_edit, MLL::GetString(IDS_PD_STARTER_CRANKTORUNTIME1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cranktoruntime_spin, MLL::GetString(IDS_PD_STARTER_CRANKTORUNTIME1_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CChokePageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CChokePageDlg::OnChangePdChokeSMStepsNumEdit()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CChokePageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify();
}

void CChokePageDlg::OnSMTestButton()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CChokePageDlg::OnManualDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
 NM_UPDOWN* pUD = (NM_UPDOWN*)pNMHDR;
 m_man_ctrl_spin.SetFocus();

 int accDelta = m_params.manual_delta - pUD->iDelta;
 if (accDelta > 127)  accDelta = 127; //because signed char
 if (accDelta < -127) accDelta = -127;
 m_params.manual_delta = accDelta;

 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
 *pResult = 0;
}

//разрешение/запрещение контроллов (всех поголовно)
void CChokePageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CChokePageDlg::IsEnabled(void)
{
 return m_enabled;
}

void CChokePageDlg::EnableChokeTesting(bool enable)
{
 if (m_choketst_enabled == enable)
  return; //already has needed state
 m_choketst_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CChokePageDlg::EnableChokeManPos(bool enable)
{
 if (m_chokemanpos_enabled == enable)
  return; //already has needed state
 m_chokemanpos_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//this function is used to obtain data from a dialog
void CChokePageDlg::GetValues(SECU3IO::ChokePar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::ChokePar));
}

//This function is used to set dialog data
void CChokePageDlg::SetValues(const SECU3IO::ChokePar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::ChokePar));
 if (!m_lock_ui_update)
  UpdateData(FALSE); //copy data from variables to dialog
}

void CChokePageDlg::LockUIUpdate(bool lock)
{
 m_lock_ui_update = lock;
}

void CChokePageDlg::EnableFuelInjection(bool enable)
{
 if (m_fuel_injection == enable)
  return; //already has needed state
 m_fuel_injection = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CChokePageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(460));
}
