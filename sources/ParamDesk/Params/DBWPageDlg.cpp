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

/** \file DBWPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "DBWPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"
#include "common/dpiaware.h"

BEGIN_MESSAGE_MAP(CDBWPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()

 ON_EN_CHANGE(IDC_PD_DBW_ETCPIDP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_ETCPIDI_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_ETCPIDD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_NMAXDUTY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_PMAXDUTY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_PIDPERIOD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_FRICTORQOP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_FRICTORQCL_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_FRICTORQTH_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_DBW_MAXIACADD_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_DBW_UPDHOMEPOS_BUTTON, OnUpdHomePos)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDP_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDP_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDI_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDI_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDI_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDI_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_ETCPIDD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_NMAXDUTY_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_NMAXDUTY_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_NMAXDUTY_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_NMAXDUTY_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_PMAXDUTY_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_PMAXDUTY_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_PMAXDUTY_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_PMAXDUTY_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_PIDPERIOD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_PIDPERIOD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_PIDPERIOD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_PIDPERIOD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQOP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQOP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQOP_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQOP_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQCL_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQCL_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQCL_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQCL_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQTH_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQTH_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQTH_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_FRICTORQTH_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_MAXIACADD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_MAXIACADD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_MAXIACADD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_MAXIACADD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_HOMEPOS_EDIT,OnUpdateControlsHomepos)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_HOMEPOS_CAPTION,OnUpdateControlsHomepos)
 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_HOMEPOS_UNIT,OnUpdateControlsHomepos)

 ON_UPDATE_COMMAND_UI(IDC_PD_DBW_UPDHOMEPOS_BUTTON,OnUpdateControlsHomepos)
END_MESSAGE_MAP()

CDBWPageDlg::CDBWPageDlg()
: m_enabled(false)
, m_homepos_enabled(false)
, mp_scr(new CWndScroller)
, m_etc_p_edit(CEditEx::MODE_FLOAT, true)
, m_etc_i_edit(CEditEx::MODE_FLOAT, true)
, m_etc_d_edit(CEditEx::MODE_FLOAT, true)
, m_etc_nmax_duty_edit(CEditEx::MODE_FLOAT, true)
, m_etc_pmax_duty_edit(CEditEx::MODE_FLOAT, true)
, m_etc_pidperiod_edit(CEditEx::MODE_FLOAT, true)
, m_etc_frictorqop_edit(CEditEx::MODE_FLOAT, true)
, m_etc_frictorqcl_edit(CEditEx::MODE_FLOAT, true)
, m_etc_frictorqth_edit(CEditEx::MODE_FLOAT, true)
, m_etc_maxiacadd_edit(CEditEx::MODE_FLOAT, true)
, m_etc_homepos_edit(CEditEx::MODE_FLOAT, true)
{
 m_params.etc_p = 0.1f;
 m_params.etc_i = 0.1f;
 m_params.etc_d = 0.1f;
 m_params.etc_nmax_duty = 45.0f;
 m_params.etc_pmax_duty = 45.0f;
 m_params.etc_pid_period = 0.01f;
 m_params.etc_frictorq_op = 6.3f;
 m_params.etc_frictorq_cl = 7.1f;
 m_params.etc_frictorq_thrd = 1.0f;
 m_params.etc_idleadd_max = 5.0f;
 m_params.etc_homepos = 0.0f;
}

CDBWPageDlg::~CDBWPageDlg()
{
 //empty
}

LPCTSTR CDBWPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PD_DBW_PAGE;
}

void CDBWPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX,IDC_PD_DBW_ETCPIDP_EDIT, m_etc_p_edit);
 DDX_Control(pDX,IDC_PD_DBW_ETCPIDP_SPIN, m_etc_p_spin);
 DDX_Control(pDX,IDC_PD_DBW_ETCPIDI_EDIT, m_etc_i_edit);
 DDX_Control(pDX,IDC_PD_DBW_ETCPIDI_SPIN, m_etc_i_spin);
 DDX_Control(pDX,IDC_PD_DBW_ETCPIDD_EDIT, m_etc_d_edit);
 DDX_Control(pDX,IDC_PD_DBW_ETCPIDD_SPIN, m_etc_d_spin);
 DDX_Control(pDX,IDC_PD_DBW_NMAXDUTY_EDIT, m_etc_nmax_duty_edit);
 DDX_Control(pDX,IDC_PD_DBW_NMAXDUTY_SPIN, m_etc_nmax_duty_spin);
 DDX_Control(pDX,IDC_PD_DBW_PMAXDUTY_EDIT, m_etc_pmax_duty_edit);
 DDX_Control(pDX,IDC_PD_DBW_PMAXDUTY_SPIN, m_etc_pmax_duty_spin);
 DDX_Control(pDX,IDC_PD_DBW_PIDPERIOD_EDIT, m_etc_pidperiod_edit);
 DDX_Control(pDX,IDC_PD_DBW_PIDPERIOD_SPIN, m_etc_pidperiod_spin);
 DDX_Control(pDX,IDC_PD_DBW_FRICTORQOP_EDIT, m_etc_frictorqop_edit);
 DDX_Control(pDX,IDC_PD_DBW_FRICTORQOP_SPIN, m_etc_frictorqop_spin);
 DDX_Control(pDX,IDC_PD_DBW_FRICTORQCL_EDIT, m_etc_frictorqcl_edit);
 DDX_Control(pDX,IDC_PD_DBW_FRICTORQCL_SPIN, m_etc_frictorqcl_spin);
 DDX_Control(pDX,IDC_PD_DBW_FRICTORQTH_EDIT, m_etc_frictorqth_edit);
 DDX_Control(pDX,IDC_PD_DBW_FRICTORQTH_SPIN, m_etc_frictorqth_spin);
 DDX_Control(pDX,IDC_PD_DBW_MAXIACADD_EDIT, m_etc_maxiacadd_edit);
 DDX_Control(pDX,IDC_PD_DBW_MAXIACADD_SPIN, m_etc_maxiacadd_spin);
 DDX_Control(pDX,IDC_PD_DBW_HOMEPOS_EDIT, m_etc_homepos_edit);
 DDX_Control(pDX,IDC_PD_DBW_UPDHOMEPOS_BUTTON, m_updhomepos_button);

 m_etc_p_edit.DDX_Value(pDX, IDC_PD_DBW_ETCPIDP_EDIT, m_params.etc_p);
 m_etc_i_edit.DDX_Value(pDX, IDC_PD_DBW_ETCPIDI_EDIT, m_params.etc_i);
 m_etc_d_edit.DDX_Value(pDX, IDC_PD_DBW_ETCPIDD_EDIT, m_params.etc_d);
 m_etc_nmax_duty_edit.DDX_Value(pDX, IDC_PD_DBW_NMAXDUTY_EDIT, m_params.etc_nmax_duty);
 m_etc_pmax_duty_edit.DDX_Value(pDX, IDC_PD_DBW_PMAXDUTY_EDIT, m_params.etc_pmax_duty);
 m_etc_pidperiod_edit.DDX_Value(pDX, IDC_PD_DBW_PIDPERIOD_EDIT, m_params.etc_pid_period);
 m_etc_frictorqop_edit.DDX_Value(pDX, IDC_PD_DBW_FRICTORQOP_EDIT, m_params.etc_frictorq_op);
 m_etc_frictorqcl_edit.DDX_Value(pDX, IDC_PD_DBW_FRICTORQCL_EDIT, m_params.etc_frictorq_cl);
 m_etc_frictorqth_edit.DDX_Value(pDX, IDC_PD_DBW_FRICTORQTH_EDIT, m_params.etc_frictorq_thrd);
 m_etc_maxiacadd_edit.DDX_Value(pDX, IDC_PD_DBW_MAXIACADD_EDIT, m_params.etc_idleadd_max);
 m_etc_homepos_edit.DDX_Value(pDX, IDC_PD_DBW_HOMEPOS_EDIT, m_params.etc_homepos);
}

void CDBWPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CDBWPageDlg::OnUpdateControlsHomepos(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_homepos_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CDBWPageDlg message handlers

BOOL CDBWPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_etc_p_spin.SetBuddy(&m_etc_p_edit);
 m_etc_p_edit.SetLimitText(8);
 m_etc_p_edit.SetDecimalPlaces(5);
 m_etc_p_spin.SetRangeAndDelta(0, 5.0f, 0.00025f);
 m_etc_p_edit.SetRange(0, 5.0f);

 m_etc_i_spin.SetBuddy(&m_etc_i_edit);
 m_etc_i_edit.SetLimitText(8);
 m_etc_i_edit.SetDecimalPlaces(5);
 m_etc_i_spin.SetRangeAndDelta(0, 5.0f, 0.00025f);
 m_etc_i_edit.SetRange(0, 5.0f);

 m_etc_d_spin.SetBuddy(&m_etc_d_edit);
 m_etc_d_edit.SetLimitText(8);
 m_etc_d_edit.SetDecimalPlaces(5);
 m_etc_d_spin.SetRangeAndDelta(0, 5.0f, 0.00025f);
 m_etc_d_edit.SetRange(0, 5.0f);

 m_etc_nmax_duty_spin.SetBuddy(&m_etc_nmax_duty_edit);
 m_etc_nmax_duty_edit.SetLimitText(6);
 m_etc_nmax_duty_edit.SetDecimalPlaces(1);
 m_etc_nmax_duty_spin.SetRangeAndDelta(0, 100.0f, 0.5f);
 m_etc_nmax_duty_edit.SetRange(0, 100.0f);

 m_etc_pmax_duty_spin.SetBuddy(&m_etc_pmax_duty_edit);
 m_etc_pmax_duty_edit.SetLimitText(6);
 m_etc_pmax_duty_edit.SetDecimalPlaces(1);
 m_etc_pmax_duty_spin.SetRangeAndDelta(0, 100.0f, 0.5f);
 m_etc_pmax_duty_edit.SetRange(0, 100.0f);

 m_etc_pidperiod_spin.SetBuddy(&m_etc_pidperiod_edit);
 m_etc_pidperiod_edit.SetLimitText(6);
 m_etc_pidperiod_edit.SetDecimalPlaces(2);
 m_etc_pidperiod_spin.SetRangeAndDelta(0, 0.2f, 0.01f);
 m_etc_pidperiod_edit.SetRange(0, 0.2f);

 m_etc_frictorqop_spin.SetBuddy(&m_etc_frictorqop_edit);
 m_etc_frictorqop_edit.SetLimitText(6);
 m_etc_frictorqop_edit.SetDecimalPlaces(1);
 m_etc_frictorqop_spin.SetRangeAndDelta(0, 15.9f, 0.1f);
 m_etc_frictorqop_edit.SetRange(0, 15.9f);

 m_etc_frictorqcl_spin.SetBuddy(&m_etc_frictorqcl_edit);
 m_etc_frictorqcl_edit.SetLimitText(6);
 m_etc_frictorqcl_edit.SetDecimalPlaces(1);
 m_etc_frictorqcl_spin.SetRangeAndDelta(0, 15.9f, 0.1f);
 m_etc_frictorqcl_edit.SetRange(0, 15.9f);

 m_etc_frictorqth_spin.SetBuddy(&m_etc_frictorqth_edit);
 m_etc_frictorqth_edit.SetLimitText(6);
 m_etc_frictorqth_edit.SetDecimalPlaces(0);
 m_etc_frictorqth_spin.SetRangeAndDelta(0, 25.0f, 1.0f);
 m_etc_frictorqth_edit.SetRange(0, 25.0f);

 m_etc_maxiacadd_spin.SetBuddy(&m_etc_maxiacadd_edit);
 m_etc_maxiacadd_edit.SetLimitText(5);
 m_etc_maxiacadd_edit.SetDecimalPlaces(2);
 m_etc_maxiacadd_spin.SetRangeAndDelta(0, 15.0f, 0.1f);
 m_etc_maxiacadd_edit.SetRange(0, 15.0f);

 m_etc_homepos_edit.SetLimitText(5);
 m_etc_homepos_edit.SetDecimalPlaces(2);
 m_etc_homepos_edit.SetRange(0, 100.0f);

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_etc_p_edit, MLL::GetString(IDS_PD_DBW_ETCPIDP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_p_spin, MLL::GetString(IDS_PD_DBW_ETCPIDP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_i_edit, MLL::GetString(IDS_PD_DBW_ETCPIDI_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_i_spin, MLL::GetString(IDS_PD_DBW_ETCPIDI_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_d_edit, MLL::GetString(IDS_PD_DBW_ETCPIDD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_d_spin, MLL::GetString(IDS_PD_DBW_ETCPIDD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_nmax_duty_edit, MLL::GetString(IDS_PD_DBW_NMAXDUTY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_nmax_duty_spin, MLL::GetString(IDS_PD_DBW_NMAXDUTY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_pmax_duty_edit, MLL::GetString(IDS_PD_DBW_PMAXDUTY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_pmax_duty_spin, MLL::GetString(IDS_PD_DBW_PMAXDUTY_EDIT_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_etc_pidperiod_edit, MLL::GetString(IDS_PD_DBW_PIDPERIOD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_pidperiod_spin, MLL::GetString(IDS_PD_DBW_PIDPERIOD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_frictorqop_edit, MLL::GetString(IDS_PD_DBW_FRICTORQOP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_frictorqop_spin, MLL::GetString(IDS_PD_DBW_FRICTORQOP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_frictorqcl_edit, MLL::GetString(IDS_PD_DBW_FRICTORQCL_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_frictorqcl_spin, MLL::GetString(IDS_PD_DBW_FRICTORQCL_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_frictorqth_edit, MLL::GetString(IDS_PD_DBW_FRICTORQTH_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_frictorqth_spin, MLL::GetString(IDS_PD_DBW_FRICTORQTH_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_maxiacadd_edit, MLL::GetString(IDS_PD_DBW_MAXIACADD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_maxiacadd_spin, MLL::GetString(IDS_PD_DBW_MAXIACADD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_etc_homepos_edit, MLL::GetString(IDS_PD_DBW_HOMEPOS_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_updhomepos_button, MLL::GetString(IDS_PD_DBW_UPDHOMEPOS_BUTTON_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CDBWPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
 m_upd_tmr.KillTimer();
}

void CDBWPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

//Enable/Disable controls (all)
void CDBWPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

bool CDBWPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//Use this function for obtaining data from dialog
void CDBWPageDlg::GetValues(SECU3IO::DBWPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog controls to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::DBWPar));
}

//Use this function to set dialog data
void CDBWPageDlg::SetValues(const SECU3IO::DBWPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::DBWPar));
 UpdateData(FALSE); //copy data from variables to dialog controls
 UpdateDialogControls(this, TRUE);
}

void CDBWPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(300));
}

void CDBWPageDlg::OnUpdHomePos()
{
 if (m_OnRequestDataCollection)
 {
  m_OnRequestDataCollection(1);
  m_upd_tmr.SetTimer(this, &CDBWPageDlg::OnRequestDataCollectionTimer, 300);
 }
}

void CDBWPageDlg::OnRequestDataCollectionTimer(void)
{
 m_upd_tmr.KillTimer();
 if (m_OnRequestDataCollection)
  m_OnRequestDataCollection(0);
}

void CDBWPageDlg::EnableHomePos(bool enable)
{
 if (m_homepos_enabled == enable)
  return; //already has needed state
 m_homepos_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}
