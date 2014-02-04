/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include "stdafx.h"
#include "Resources/resource.h"
#include "ChokePageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CChokePageDlg::IDD = IDD_PD_CHOKE_PAGE;

BEGIN_MESSAGE_MAP(CChokePageDlg, Super)
 ON_EN_CHANGE(IDC_PD_CHOKE_SM_STEPS_NUM_EDIT, OnChangePdChokeSMStepsNumEdit)
 ON_EN_CHANGE(IDC_PD_CHOKE_STRT_ADD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CHOKE_RPMREG_1STPT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CHOKE_RPMREG_2NDPT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CHOKE_RPMREG_IF_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_CHOKE_SM_TEST_CHECK, OnSMTestButton)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_TEST_CHECK,OnUpdateChokeTestBtn)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_MANUAL_POS_SPIN,OnUpdateChokeManPosBtn)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_MANUAL_POS_CAPTION,OnUpdateChokeManPosBtn)
 ON_NOTIFY(UDN_DELTAPOS, IDC_PD_CHOKE_MANUAL_POS_SPIN, OnManualDeltapos)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_STRT_ADD_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_1STPT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_1STPT_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_1STPT_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_1STPT_TEMP,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_2NDPT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_2NDPT_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_2NDPT_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_2NDPT_TEMP,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_IF_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_IF_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_IF_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_RPMREG_TITLE,OnUpdateControls)
END_MESSAGE_MAP()

CChokePageDlg::CChokePageDlg(CWnd* pParent /*=NULL*/)
: Super(CChokePageDlg::IDD, pParent)
, m_enabled(false)
, m_sm_steps_num_edit(CEditEx::MODE_INT)
, m_strt_add_edit(CEditEx::MODE_FLOAT)
, m_rpmreg_1stpt_edit(CEditEx::MODE_INT)
, m_rpmreg_2ndpt_edit(CEditEx::MODE_INT)
, m_rpmreg_if_edit(CEditEx::MODE_FLOAT)
, m_choketst_enabled(false)
, m_chokemanpos_enabled(false)
{
 m_params.sm_steps = 700;
 m_params.testing = 0;
 m_params.manual_delta = 0;
 m_params.strt_add = 10.0f; //10%
 m_params.choke_rpm[0] = 2000;
 m_params.choke_rpm[1] = 1000;
 m_params.choke_rpm_if = 0.05f;
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

 DDX_Control(pDX, IDC_PD_CHOKE_STRT_ADD_SPIN, m_strt_add_spin);
 DDX_Control(pDX, IDC_PD_CHOKE_STRT_ADD_EDIT, m_strt_add_edit);
 DDX_Control(pDX, IDC_PD_CHOKE_RPMREG_1STPT_SPIN, m_rpmreg_1stpt_spin);
 DDX_Control(pDX, IDC_PD_CHOKE_RPMREG_1STPT_EDIT, m_rpmreg_1stpt_edit);
 DDX_Control(pDX, IDC_PD_CHOKE_RPMREG_2NDPT_SPIN, m_rpmreg_2ndpt_spin);
 DDX_Control(pDX, IDC_PD_CHOKE_RPMREG_2NDPT_EDIT, m_rpmreg_2ndpt_edit);
 DDX_Control(pDX, IDC_PD_CHOKE_RPMREG_IF_EDIT, m_rpmreg_if_edit);
 DDX_Control(pDX, IDC_PD_CHOKE_RPMREG_IF_SPIN, m_rpmreg_if_spin);

 m_sm_steps_num_edit.DDX_Value(pDX, IDC_PD_CHOKE_SM_STEPS_NUM_EDIT, m_params.sm_steps);
 m_strt_add_edit.DDX_Value(pDX, IDC_PD_CHOKE_STRT_ADD_EDIT, m_params.strt_add);
 m_rpmreg_1stpt_edit.DDX_Value(pDX, IDC_PD_CHOKE_RPMREG_1STPT_EDIT, m_params.choke_rpm[0]);
 m_rpmreg_2ndpt_edit.DDX_Value(pDX, IDC_PD_CHOKE_RPMREG_2NDPT_EDIT, m_params.choke_rpm[1]);
 m_rpmreg_if_edit.DDX_Value(pDX, IDC_PD_CHOKE_RPMREG_IF_EDIT, m_params.choke_rpm_if);
 DDX_Check_UCHAR(pDX, IDC_PD_CHOKE_SM_TEST_CHECK, m_params.testing);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CChokePageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CChokePageDlg::OnUpdateChokeTestBtn(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_choketst_enabled);
}

void CChokePageDlg::OnUpdateChokeManPosBtn(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_chokemanpos_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CStarterPageDlg message handlers

BOOL CChokePageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_sm_steps_num_edit.SetLimitText(4);
 m_sm_steps_num_spin.SetBuddy(&m_sm_steps_num_edit);
 m_sm_steps_num_spin.SetRangeAndDelta(50, 2000, 1);

 m_strt_add_edit.SetLimitText(5);
 m_strt_add_spin.SetBuddy(&m_strt_add_edit);
 m_strt_add_edit.SetDecimalPlaces(1);
 m_strt_add_spin.SetRangeAndDelta(0.0f, 100.0f, 0.5f);

 m_rpmreg_1stpt_edit.SetLimitText(4);
 m_rpmreg_1stpt_spin.SetBuddy(&m_rpmreg_1stpt_edit);
 m_rpmreg_1stpt_spin.SetRangeAndDelta(0, 3000, 10);

 m_rpmreg_2ndpt_edit.SetLimitText(4);
 m_rpmreg_2ndpt_spin.SetBuddy(&m_rpmreg_2ndpt_edit);
 m_rpmreg_2ndpt_spin.SetRangeAndDelta(500, 3000, 10);

 m_rpmreg_if_edit.SetLimitText(4);
 m_rpmreg_if_spin.SetBuddy(&m_rpmreg_if_edit);
 m_rpmreg_if_edit.SetDecimalPlaces(2);
 m_rpmreg_if_spin.SetRangeAndDelta(0.01f, 10.0f, 0.01f);

 m_man_ctrl_spin.SetBuddy(&m_man_ctrl_spin); //loves himself

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_sm_steps_num_edit, MLL::GetString(IDS_PD_CHOKE_SM_STEPS_NUM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_sm_steps_num_spin, MLL::GetString(IDS_PD_CHOKE_SM_STEPS_NUM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_sm_test_check, MLL::GetString(IDS_PD_CHOKE_SM_TEST_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_man_ctrl_spin, MLL::GetString(IDS_PD_CHOKE_MANUAL_POS_SPIN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_strt_add_edit, MLL::GetString(IDS_PD_CHOKE_STRT_ADD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_strt_add_spin, MLL::GetString(IDS_PD_CHOKE_STRT_ADD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rpmreg_1stpt_edit, MLL::GetString(IDS_PD_CHOKE_RPMREG_1STPT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rpmreg_1stpt_spin, MLL::GetString(IDS_PD_CHOKE_RPMREG_1STPT_EDIT_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
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

//эту функцию необходимо использовать когда надо получить данные из диалога
void CChokePageDlg::GetValues(SECU3IO::ChokePar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::ChokePar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CChokePageDlg::SetValues(const SECU3IO::ChokePar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::ChokePar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
