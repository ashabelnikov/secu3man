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

/** \file CarburPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "CarburPageDlg.h"
#include "common/dpiaware.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

const UINT CCarburPageDlg::IDD = IDD_PD_CARBUR_PAGE;

BEGIN_MESSAGE_MAP(CCarburPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_EPM_ON_THRESHOLD_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_CARBUR_INVERSE_SWITCH, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT_G, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT_G, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_DELAY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_TPS_THRESHOLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_FC_MAP_THRD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_FC_CTS_THRD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_REVLIM_HI_THRESHOLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_REVLIM_LO_THRESHOLD_EDIT, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_INVERSE_SWITCH,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_EPM_ON_THRESHOLD_EDIT,OnUpdateCarburetorControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_EPM_ON_THRESHOLD_SPIN,OnUpdateCarburetorControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_EPM_ON_THRESHOLD_CAPTION,OnUpdateCarburetorControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_EPM_ON_THRESHOLD_UNIT,OnUpdateCarburetorControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT_G,OnUpdateGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN_G,OnUpdateGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_CAPTION_G,OnUpdateGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_UNIT_G,OnUpdateGasControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT_G,OnUpdateGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN_G,OnUpdateGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_CAPTION_G,OnUpdateGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_UNIT_G,OnUpdateGasControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_DELAY_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_DELAY_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_DELAY_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_DELAY_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_TPS_THRESHOLD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_TPS_THRESHOLD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_TPS_THRESHOLD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_TPS_THRESHOLD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_FC_MAP_THRD_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_FC_MAP_THRD_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_FC_MAP_THRD_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_FC_MAP_THRD_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_FC_CTS_THRD_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_FC_CTS_THRD_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_FC_CTS_THRD_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_FC_CTS_THRD_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_REVLIM_LO_THRESHOLD_EDIT,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_REVLIM_LO_THRESHOLD_SPIN,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_REVLIM_LO_THRESHOLD_CAPTION,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_REVLIM_LO_THRESHOLD_UNIT,OnUpdateInjGasControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_REVLIM_HI_THRESHOLD_EDIT,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_REVLIM_HI_THRESHOLD_SPIN,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_REVLIM_HI_THRESHOLD_CAPTION,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_REVLIM_HI_THRESHOLD_UNIT,OnUpdateInjGasControls)

END_MESSAGE_MAP()

CCarburPageDlg::CCarburPageDlg(CWnd* pParent /*=NULL*/)
: Super(CCarburPageDlg::IDD, pParent)
, m_enabled(false)
, m_fuel_injection(false)
, m_gasdose(false)
, m_shutoff_lo_threshold_edit(CEditEx::MODE_INT, true)
, m_shutoff_hi_threshold_edit(CEditEx::MODE_INT, true)
, m_epm_on_threshold_edit(CEditEx::MODE_FLOAT, true)
, m_shutoff_lo_threshold_edit_g(CEditEx::MODE_INT, true)
, m_shutoff_hi_threshold_edit_g(CEditEx::MODE_INT, true)
, m_shutoff_delay_edit(CEditEx::MODE_FLOAT, true)
, m_tps_threshold_edit(CEditEx::MODE_FLOAT, true)
, m_fuelcut_map_thrd_edit(CEditEx::MODE_FLOAT, true)
, m_fuelcut_cts_thrd_edit(CEditEx::MODE_FLOAT, true)
, m_revlim_lo_threshold_edit(CEditEx::MODE_INT, true)
, m_revlim_hi_threshold_edit(CEditEx::MODE_INT, true)
, mp_scr(new CWndScroller)
{
 m_params.ephh_lot = 1250;
 m_params.ephh_hit = 1500;
 m_params.carb_invers = 0;
 m_params.epm_ont = 6.25f;
 m_params.ephh_lot_g = 1250;
 m_params.ephh_hit_g = 1500;
 m_params.shutoff_delay = 0.0f;
 m_params.tps_threshold = 0.0f;
 m_params.fuelcut_map_thrd = 20.0f;
 m_params.fuelcut_cts_thrd = 15.0f;
 m_params.revlim_lot = 8000;
 m_params.revlim_hit = 8100;
}

LPCTSTR CCarburPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CCarburPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_CARBUR_INVERSE_SWITCH, m_inverse_throttle_switch);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN, m_shutoff_lo_threshold_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN, m_shutoff_hi_threshold_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_EPM_ON_THRESHOLD_SPIN, m_epm_on_threshold_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_TPS_THRESHOLD_SPIN, m_tps_threshold_spin);

 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, m_shutoff_lo_threshold_edit);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, m_shutoff_hi_threshold_edit);
 DDX_Control(pDX, IDC_PD_CARBUR_EPM_ON_THRESHOLD_EDIT, m_epm_on_threshold_edit);
 DDX_Control(pDX, IDC_PD_CARBUR_TPS_THRESHOLD_EDIT, m_tps_threshold_edit);

 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN_G, m_shutoff_lo_threshold_spin_g);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN_G, m_shutoff_hi_threshold_spin_g);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_DELAY_SPIN, m_shutoff_delay_spin);

 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT_G, m_shutoff_lo_threshold_edit_g);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT_G, m_shutoff_hi_threshold_edit_g);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_DELAY_EDIT, m_shutoff_delay_edit);

 DDX_Control(pDX, IDC_PD_CARBUR_FC_MAP_THRD_SPIN, m_fuelcut_map_thrd_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_FC_CTS_THRD_SPIN, m_fuelcut_cts_thrd_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_FC_MAP_THRD_EDIT, m_fuelcut_map_thrd_edit);
 DDX_Control(pDX, IDC_PD_CARBUR_FC_CTS_THRD_EDIT, m_fuelcut_cts_thrd_edit);

 DDX_Control(pDX, IDC_PD_CARBUR_REVLIM_LO_THRESHOLD_SPIN, m_revlim_lo_threshold_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_REVLIM_HI_THRESHOLD_SPIN, m_revlim_hi_threshold_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_REVLIM_LO_THRESHOLD_EDIT, m_revlim_lo_threshold_edit);
 DDX_Control(pDX, IDC_PD_CARBUR_REVLIM_HI_THRESHOLD_EDIT, m_revlim_hi_threshold_edit);

 m_shutoff_lo_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, m_params.ephh_lot);
 m_shutoff_hi_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, m_params.ephh_hit);

 m_shutoff_lo_threshold_edit_g.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT_G, m_params.ephh_lot_g);
 m_shutoff_hi_threshold_edit_g.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT_G, m_params.ephh_hit_g);
 m_shutoff_delay_edit.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_DELAY_EDIT, m_params.shutoff_delay);

 DDX_Check_UCHAR(pDX, IDC_PD_CARBUR_INVERSE_SWITCH, m_params.carb_invers);
 m_epm_on_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_EPM_ON_THRESHOLD_EDIT, m_params.epm_ont);
 m_tps_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_TPS_THRESHOLD_EDIT, m_params.tps_threshold);

 m_fuelcut_map_thrd_edit.DDX_Value(pDX, IDC_PD_CARBUR_FC_MAP_THRD_EDIT, m_params.fuelcut_map_thrd);
 m_fuelcut_cts_thrd_edit.DDX_Value(pDX, IDC_PD_CARBUR_FC_CTS_THRD_EDIT, m_params.fuelcut_cts_thrd);

 m_revlim_lo_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_REVLIM_LO_THRESHOLD_EDIT, m_params.revlim_lot);
 m_revlim_hi_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_REVLIM_HI_THRESHOLD_EDIT, m_params.revlim_hit);
}

/////////////////////////////////////////////////////////////////////////////
// CCarburPageDlg message handlers

void CCarburPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CCarburPageDlg::OnUpdateGasControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (!m_fuel_injection || m_gasdose));
}

void CCarburPageDlg::OnUpdateInjGasControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (m_fuel_injection || m_gasdose));
}

void CCarburPageDlg::OnUpdateFuelInjectionControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_fuel_injection);
}

void CCarburPageDlg::OnUpdateCarburetorControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (!m_fuel_injection || m_gasdose));
}

BOOL CCarburPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_shutoff_lo_threshold_edit.SetLimitText(4);
 m_shutoff_lo_threshold_spin.SetBuddy(&m_shutoff_lo_threshold_edit);
 m_shutoff_lo_threshold_spin.SetRangeAndDelta(250,7500,10);
 m_shutoff_lo_threshold_edit.SetRange(250,7500);

 m_shutoff_hi_threshold_edit.SetLimitText(4);
 m_shutoff_hi_threshold_spin.SetBuddy(&m_shutoff_hi_threshold_edit);
 m_shutoff_hi_threshold_spin.SetRangeAndDelta(250,7500,10);
 m_shutoff_hi_threshold_edit.SetRange(250,7500);

 m_epm_on_threshold_spin.SetBuddy(&m_epm_on_threshold_edit);
 m_epm_on_threshold_edit.SetLimitText(4);
 m_epm_on_threshold_edit.SetDecimalPlaces(2);
 m_epm_on_threshold_spin.SetRangeAndDelta(0.0f,50.0f,0.1f);
 m_epm_on_threshold_edit.SetRange(0.0f,50.0f);

 m_shutoff_lo_threshold_edit_g.SetLimitText(4);
 m_shutoff_lo_threshold_spin_g.SetBuddy(&m_shutoff_lo_threshold_edit_g);
 m_shutoff_lo_threshold_spin_g.SetRangeAndDelta(250,15000,10);
 m_shutoff_lo_threshold_edit_g.SetRange(250,15000);

 m_shutoff_hi_threshold_edit_g.SetLimitText(4);
 m_shutoff_hi_threshold_spin_g.SetBuddy(&m_shutoff_hi_threshold_edit_g);
 m_shutoff_hi_threshold_spin_g.SetRangeAndDelta(250,15000,10);
 m_shutoff_hi_threshold_edit_g.SetRange(250,15000);

 m_shutoff_delay_spin.SetBuddy(&m_shutoff_delay_edit);
 m_shutoff_delay_edit.SetLimitText(4);
 m_shutoff_delay_edit.SetDecimalPlaces(2);
 m_shutoff_delay_spin.SetRangeAndDelta(0.0f,2.5f,0.01f);
 m_shutoff_delay_edit.SetRange(0.0f,2.5f);

 m_tps_threshold_spin.SetBuddy(&m_tps_threshold_edit);
 m_tps_threshold_edit.SetLimitText(5);
 m_tps_threshold_edit.SetDecimalPlaces(1);
 m_tps_threshold_spin.SetRangeAndDelta(0.0f, 100.0f, 0.5f);
 m_tps_threshold_edit.SetRange(0.0f, 100.0f);

 m_fuelcut_map_thrd_spin.SetBuddy(&m_fuelcut_map_thrd_edit);
 m_fuelcut_map_thrd_edit.SetLimitText(5);
 m_fuelcut_map_thrd_edit.SetDecimalPlaces(1);
 m_fuelcut_map_thrd_spin.SetRangeAndDelta(0.0f, 250.0f, 0.5f);
 m_fuelcut_map_thrd_edit.SetRange(0.0f, 250.0f);

 m_fuelcut_cts_thrd_spin.SetBuddy(&m_fuelcut_cts_thrd_edit);
 m_fuelcut_cts_thrd_edit.SetLimitText(5);
 m_fuelcut_cts_thrd_edit.SetDecimalPlaces(1);
 m_fuelcut_cts_thrd_spin.SetRangeAndDelta(-30.0f, 180.0f, 0.25f);
 m_fuelcut_cts_thrd_edit.SetRange(-30.0f, 180.0f);

 m_revlim_lo_threshold_edit.SetLimitText(5);
 m_revlim_lo_threshold_spin.SetBuddy(&m_revlim_lo_threshold_edit);
 m_revlim_lo_threshold_spin.SetRangeAndDelta(500,16000,50);
 m_revlim_lo_threshold_edit.SetRange(500,16000);

 m_revlim_hi_threshold_edit.SetLimitText(5);
 m_revlim_hi_threshold_spin.SetBuddy(&m_revlim_hi_threshold_edit);
 m_revlim_hi_threshold_spin.SetRangeAndDelta(500,16000,50);
 m_revlim_hi_threshold_edit.SetRange(500,16000);

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 VERIFY(mp_ttc->AddWindow(&m_epm_on_threshold_edit, MLL::GetString(IDS_PD_CARBUR_EPM_ON_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_epm_on_threshold_spin, MLL::GetString(IDS_PD_CARBUR_EPM_ON_THRESHOLD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_tps_threshold_edit, MLL::GetString(IDS_PD_CARBUR_TPS_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_threshold_spin, MLL::GetString(IDS_PD_CARBUR_TPS_THRESHOLD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_shutoff_delay_edit, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_DELAY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_shutoff_delay_spin, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_DELAY_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_shutoff_lo_threshold_edit, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_THRESHOLD_LO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_shutoff_lo_threshold_spin, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_THRESHOLD_LO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_shutoff_hi_threshold_edit, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_THRESHOLD_HI_TT)));
 VERIFY(mp_ttc->AddWindow(&m_shutoff_hi_threshold_spin, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_THRESHOLD_HI_TT)));

 VERIFY(mp_ttc->AddWindow(&m_shutoff_lo_threshold_edit_g, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_THRESHOLD_LO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_shutoff_lo_threshold_spin_g, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_THRESHOLD_LO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_shutoff_hi_threshold_edit_g, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_THRESHOLD_HI_TT)));
 VERIFY(mp_ttc->AddWindow(&m_shutoff_hi_threshold_spin_g, MLL::GetString(IDS_PD_CARBUR_SHUTOFF_THRESHOLD_HI_TT)));

 VERIFY(mp_ttc->AddWindow(&m_inverse_throttle_switch, MLL::GetString(IDS_PD_CARBUR_INVERSE_SWITCH_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CCarburPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CCarburPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//Enable/disable all controls
void CCarburPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//get state of controls
bool CCarburPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//This function is used to obtain data from dialog
void CCarburPageDlg::GetValues(SECU3IO::CarburPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::CarburPar));
}

//This function is used to set data in dialog
void CCarburPageDlg::SetValues(const SECU3IO::CarburPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::CarburPar));
 UpdateData(FALSE); //copy data from variables to dialog
}

void CCarburPageDlg::EnableFuelInjection(bool i_enable)
{
 if (m_fuel_injection == i_enable)
  return; //already has needed state
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CCarburPageDlg::EnableGasdose(bool i_enable)
{
 if (m_gasdose == i_enable)
  return; //already has needed state
 m_gasdose = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CCarburPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(440));
}
