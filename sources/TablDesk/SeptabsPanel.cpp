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

/** \file SeptabsPanel.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "common/MathHelpers.h"
#include "common/dpiaware.h"
#include "common/GDIHelpers.h"
#include "common/chartxdid.h"
#include "DLLLinkedFunctions.h"
#include "DwellCalcDlg.h"
#include "io-core/secu3io.h"
#include "io-core/MapIds.h"
#include "SeptabsPanel.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

#define TIMER_ID 0

/////////////////////////////////////////////////////////////////////////////
// CSeptabsPanel dialog

CSeptabsPanel::CSeptabsPanel(bool disable_vscroll /*= false*/, bool enable_fwconsts /*= false*/, bool enable_ceerr /*= false*/, bool enable_grids /*= false*/)
: m_dwellcntrl_enabled(false)
, m_cts_curve_enabled(false)
, m_tmp2_curve_enabled(false)
, m_grts_curve_enabled(false)
, m_grheat_duty_enabled(false)
, m_pwmiac_ucoef_enabled(false)
, m_aftstr_strk_enabled(false)
, m_ftls_curve_enabled(false)
, m_egts_curve_enabled(false)
, m_ops_curve_enabled(false)
, m_maninjpwc_enabled(false)
, m_lambdazone_enabled(false)
, m_fts_curve_enabled(false)
, m_xtau_maps_enabled(false)
, m_initialized(false)
, m_disable_vscroll(disable_vscroll)
, m_enable_fwconsts(enable_fwconsts)
, m_enable_ceerr(enable_ceerr)
, m_enable_grids(enable_grids)
{
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
 {
  MapData md;
  md.mp_button = new CButton;
  m_md.insert(std::make_pair(i, md));
 }

 int rpm = 200;
 for(size_t i = 0; i < 128; i++)
 {
  m_attenuator_table_slots[i] = (float)rpm;
  rpm+=60;
 }

/*
 for(size_t i = 0; i < 64; ++i)
  m_mafcurve_slots[i] = (float)((5.0*i) / 63.0);
*/
}

CSeptabsPanel::~CSeptabsPanel()
{
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
  delete m_md[i].mp_button;
}

BOOL CSeptabsPanel::Create(CWnd* pParentWnd /*= NULL*/)
{
 return Super::Create(IDD_TD_SEPTABLES_PANEL, pParentWnd);
}

void CSeptabsPanel::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_VIEW_ATTENUATOR_MAP, *m_md[ETMT_ATTENUATOR].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_DWELL_CONTROL, *m_md[ETMT_DWELLCNTRL].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_CTS_CURVE, *m_md[ETMT_CTS_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_ATS_CURVE, *m_md[ETMT_ATS_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_ATS_MAP, *m_md[ETMT_ATS_CORR].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_GDP_MAP, *m_md[ETMT_GASDOSE].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_BAROCORR_MAP, *m_md[ETMT_BAROCORR].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_MANIGNTIM_MAP, *m_md[ETMT_MANIGNTIM].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_TMP2_CURVE, *m_md[ETMT_TMP2_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_CRKTEMP_MAP, *m_md[ETMT_CRKCLT_CORR].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_EH_PAUSE_MAP, *m_md[ETMT_EH_PAUSE].mp_button);
 DDX_Control(pDX, IDC_TD_CRANKING_THRD_MAP, *m_md[ETMT_CRANKING_THRD].mp_button);
 DDX_Control(pDX, IDC_TD_CRANKING_TIME_MAP, *m_md[ETMT_CRANKING_TIME].mp_button);
 DDX_Control(pDX, IDC_TD_SMAPABAN_THRD_MAP, *m_md[ETMT_SMAPABAN_THRD].mp_button);
 DDX_Control(pDX, IDC_TD_KNOCK_ZONES_MAP, *m_md[ETMT_KNOCK_ZONE].mp_button);
 DDX_Control(pDX, IDC_TD_LAMBDA_ZONES_MAP, *m_md[ETMT_LAMBDA_ZONE].mp_button);
 DDX_Control(pDX, IDC_TD_GRTS_CURVE, *m_md[ETMT_GRTS_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_GRHEAT_DUTY_MAP, *m_md[ETMT_GRHEAT_DUTY].mp_button);
 DDX_Control(pDX, IDC_TD_PWMIAC_UCOEF_MAP, *m_md[ETMT_PWMIAC_UCOEF].mp_button);
 DDX_Control(pDX, IDC_TD_AFTSTR_STRK0_MAP, *m_md[ETMT_AFTSTR_STRK0].mp_button);
 DDX_Control(pDX, IDC_TD_AFTSTR_STRK1_MAP, *m_md[ETMT_AFTSTR_STRK1].mp_button);
 DDX_Control(pDX, IDC_TD_GRVDELAY_MAP, *m_md[ETMT_GRVDELAY].mp_button);
 DDX_Control(pDX, IDC_TD_FTLS_CURVE, *m_md[ETMT_FTLS_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_EGTS_CURVE, *m_md[ETMT_EGTS_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_OPS_CURVE, *m_md[ETMT_OPS_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_MANINJPWC_MAP, *m_md[ETMT_MANINJPWC].mp_button);
 DDX_Control(pDX, IDC_TD_MAF_CURVE, *m_md[ETMT_MAF_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_FTLSCOR_MAP, *m_md[ETMT_FTLSCOR].mp_button);
 DDX_Control(pDX, IDC_TD_FTS_CURVE, *m_md[ETMT_FTS_CURVE].mp_button);
 DDX_Control(pDX, IDC_TD_FUELDENS_CORR_MAP, *m_md[ETMT_FUELDENS_CORR].mp_button);
 DDX_Control(pDX, IDC_TD_XTAU_XFACC_MAP, *m_md[ETMT_XTAU_XFACC].mp_button);
 DDX_Control(pDX, IDC_TD_XTAU_XFDEC_MAP, *m_md[ETMT_XTAU_XFDEC].mp_button);
 DDX_Control(pDX, IDC_TD_XTAU_TFACC_MAP, *m_md[ETMT_XTAU_TFACC].mp_button);
 DDX_Control(pDX, IDC_TD_XTAU_TFDEC_MAP, *m_md[ETMT_XTAU_TFDEC].mp_button);
 DDX_Control(pDX, IDC_TD_INJNONLINP_MAP, *m_md[ETMT_INJNONLINP].mp_button);
 DDX_Control(pDX, IDC_TD_INJNONLING_MAP, *m_md[ETMT_INJNONLING].mp_button);
 DDX_Control(pDX, IDC_TD_EGO_DELAY_MAP, *m_md[ETMT_EGO_DELAY].mp_button);

 DDX_Control(pDX, IDC_TD_EDIT_CEPAR, m_edit_cesettings_btn);
 DDX_Control(pDX, IDC_TD_DWELL_CALC_BUTTON, m_calc_dwell_btn);
 DDX_Control(pDX, IDC_TD_RPM_GRID_BUTTON, m_rpm_grid_btn);
 DDX_Control(pDX, IDC_TD_FW_CONSTS_BUTTON, m_fw_consts_btn);
}

BEGIN_MESSAGE_MAP(CSeptabsPanel, Super)
 ON_BN_CLICKED(IDC_TD_VIEW_ATTENUATOR_MAP, OnViewAttenuatorMap)
 ON_BN_CLICKED(IDC_TD_VIEW_DWELL_CONTROL, OnViewDwellCntrlMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CTS_CURVE, OnViewCTSCurveMap)
 ON_BN_CLICKED(IDC_TD_DWELL_CALC_BUTTON, OnDwellCalcButton)
 ON_BN_CLICKED(IDC_TD_RPM_GRID_BUTTON, OnRPMGridButton)
 ON_BN_CLICKED(IDC_TD_FW_CONSTS_BUTTON, OnFwConstsButton)
 ON_BN_CLICKED(IDC_TD_VIEW_ATS_CURVE, OnViewATSCurveMap)
 ON_BN_CLICKED(IDC_TD_VIEW_ATS_MAP, OnViewATSAACMap)
 ON_BN_CLICKED(IDC_TD_VIEW_GDP_MAP, OnViewGasdosePosMap)
 ON_BN_CLICKED(IDC_TD_EDIT_CEPAR, OnCESettingsButton)
 ON_BN_CLICKED(IDC_TD_VIEW_BAROCORR_MAP, OnViewBarocorrMap)
 ON_BN_CLICKED(IDC_TD_VIEW_MANIGNTIM_MAP, OnViewManIgntimMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TMP2_CURVE, OnViewTmp2CurveMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CRKTEMP_MAP, OnViewCrkTempMap)
 ON_BN_CLICKED(IDC_TD_VIEW_EH_PAUSE_MAP, OnViewEHPauseMap)
 ON_BN_CLICKED(IDC_TD_CRANKING_THRD_MAP, OnViewCrankingThrdMap)
 ON_BN_CLICKED(IDC_TD_CRANKING_TIME_MAP, OnViewCrankingTimeMap)
 ON_BN_CLICKED(IDC_TD_SMAPABAN_THRD_MAP, OnViewSmapabanThrdMap)
 ON_BN_CLICKED(IDC_TD_KNOCK_ZONES_MAP, OnViewKnockZoneMap)
 ON_BN_CLICKED(IDC_TD_LAMBDA_ZONES_MAP, OnViewLambdaZoneMap)
 ON_BN_CLICKED(IDC_TD_GRTS_CURVE, OnViewGrtsCurveMap)
 ON_BN_CLICKED(IDC_TD_GRHEAT_DUTY_MAP, OnViewGrHeatDutyMap)
 ON_BN_CLICKED(IDC_TD_PWMIAC_UCOEF_MAP, OnViewPwmIacUCoefMap)
 ON_BN_CLICKED(IDC_TD_AFTSTR_STRK0_MAP, OnViewAftstrStrk0Map)
 ON_BN_CLICKED(IDC_TD_AFTSTR_STRK1_MAP, OnViewAftstrStrk1Map)
 ON_BN_CLICKED(IDC_TD_GRVDELAY_MAP, OnViewGrValDelMap)
 ON_BN_CLICKED(IDC_TD_FTLS_CURVE, OnViewFtlsCurveMap)
 ON_BN_CLICKED(IDC_TD_EGTS_CURVE, OnViewEgtsCurveMap)
 ON_BN_CLICKED(IDC_TD_OPS_CURVE, OnViewOpsCurveMap)
 ON_BN_CLICKED(IDC_TD_MANINJPWC_MAP, OnViewManInjPwcMap)
 ON_BN_CLICKED(IDC_TD_MAF_CURVE, OnViewMAFCurveMap)
 ON_BN_CLICKED(IDC_TD_FTLSCOR_MAP, OnViewFtlsCorMap)
 ON_BN_CLICKED(IDC_TD_FTS_CURVE, OnViewFtsCurveMap)
 ON_BN_CLICKED(IDC_TD_FUELDENS_CORR_MAP, OnViewFuelDensCorrMap)
 ON_BN_CLICKED(IDC_TD_XTAU_XFACC_MAP, OnViewXtauXfAccMap)
 ON_BN_CLICKED(IDC_TD_XTAU_XFDEC_MAP, OnViewXtauXfDecMap)
 ON_BN_CLICKED(IDC_TD_XTAU_TFACC_MAP, OnViewXtauTfAccMap)
 ON_BN_CLICKED(IDC_TD_XTAU_TFDEC_MAP, OnViewXtauTfDecMap)
 ON_BN_CLICKED(IDC_TD_INJNONLINP_MAP, OnViewInjNonLinPMap)
 ON_BN_CLICKED(IDC_TD_INJNONLING_MAP, OnViewInjNonLinGMap)
 ON_BN_CLICKED(IDC_TD_EGO_DELAY_MAP, OnViewEGODelayMap)

 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATTENUATOR_MAP, OnUpdateViewAttenuatorMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_DWELL_CONTROL, OnUpdateViewDwellCntrlMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CTS_CURVE, OnUpdateViewCTSCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATS_CURVE, OnUpdateViewATSCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATS_MAP, OnUpdateViewATSAACMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_DWELL_CALC_BUTTON, OnUpdateViewDwellCntrlMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_RPM_GRID_BUTTON, OnUpdateControlsGRD)
 ON_UPDATE_COMMAND_UI(IDC_TD_FW_CONSTS_BUTTON, OnUpdateControlsFWC)
 ON_UPDATE_COMMAND_UI(IDC_TD_FUNSET_LIST, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_MAP_GROUPBOX, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_GDP_MAP, OnUpdateViewGasdosePosMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_EDIT_CEPAR, OnUpdateCESettingsButton)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_BAROCORR_MAP, OnUpdateViewBarocorrMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_MANIGNTIM_MAP, OnUpdateViewManIgntimMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TMP2_CURVE, OnUpdateViewTmp2CurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CRKTEMP_MAP, OnUpdateViewCrkTempMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_EH_PAUSE_MAP, OnUpdateViewEHPauseMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_CRANKING_THRD_MAP, OnUpdateViewCrankingThrdMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_CRANKING_TIME_MAP, OnUpdateViewCrankingTimeMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_SMAPABAN_THRD_MAP, OnUpdateViewSmapabanThrdMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_KNOCK_ZONES_MAP, OnUpdateViewKnockZoneMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_LAMBDA_ZONES_MAP, OnUpdateViewLambdaZoneMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_GRTS_CURVE, OnUpdateViewGrtsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_GRHEAT_DUTY_MAP, OnUpdateViewGrHeatDutyMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_PWMIAC_UCOEF_MAP, OnUpdateViewPwmIacUCoefMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_AFTSTR_STRK0_MAP, OnUpdateViewAftstrStrk0Map)
 ON_UPDATE_COMMAND_UI(IDC_TD_AFTSTR_STRK1_MAP, OnUpdateViewAftstrStrk1Map)
 ON_UPDATE_COMMAND_UI(IDC_TD_GRVDELAY_MAP, OnUpdateViewGrValDelMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_FTLS_CURVE, OnUpdateViewFtlsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_EGTS_CURVE, OnUpdateViewEgtsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_OPS_CURVE, OnUpdateViewOpsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_MANINJPWC_MAP, OnUpdateViewManInjPwcMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_MAF_CURVE, OnUpdateViewMAFCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_FTLSCOR_MAP, OnUpdateViewFtlsCorMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_FTS_CURVE, OnUpdateViewFtsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_FUELDENS_CORR_MAP, OnUpdateViewFuelDensCorrMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_XTAU_XFACC_MAP, OnUpdateViewXtauXfAccMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_XTAU_XFDEC_MAP, OnUpdateViewXtauXfDecMap) 
 ON_UPDATE_COMMAND_UI(IDC_TD_XTAU_TFACC_MAP, OnUpdateViewXtauTfAccMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_XTAU_TFDEC_MAP, OnUpdateViewXtauTfDecMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_INJNONLINP_MAP, OnUpdateViewInjNonLinPMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_INJNONLING_MAP, OnUpdateViewInjNonLinGMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_EGO_DELAY_MAP, OnUpdateViewEGODelayMap)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeptabsPanel message handlers

BOOL CSeptabsPanel::OnInitDialog()
{
 Super::OnInitDialog();

 m_initialized = true;

 //initialize window scroller
 mp_scr->Init(this);

 m_calc_dwell_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                              MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));
 m_rpm_grid_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_GRID_UP), MAKEINTRESOURCE(IDB_GRID_DOWN), 
                              MAKEINTRESOURCE(IDB_GRID_FOCUSED), MAKEINTRESOURCE(IDB_GRID_DISABLED));
 m_fw_consts_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CONST_UP), MAKEINTRESOURCE(IDB_CONST_DOWN), 
                              MAKEINTRESOURCE(IDB_CONST_FOCUSED), MAKEINTRESOURCE(IDB_CONST_DISABLED));
 m_edit_cesettings_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CES_UP), MAKEINTRESOURCE(IDB_CES_DOWN), 
                              MAKEINTRESOURCE(IDB_CES_FOCUSED), MAKEINTRESOURCE(IDB_CES_DISABLED));

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_calc_dwell_btn, MLL::GetString(IDS_TD_DWELL_CALC_BUTTON_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_rpm_grid_btn, MLL::GetString(IDS_TD_RPM_GRID_BUTTON_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_fw_consts_btn, MLL::GetString(IDS_TD_FW_CONSTS_BUTTON_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_edit_cesettings_btn, MLL::GetString(IDS_TD_EDIT_CEPAR_TT)));

 VERIFY(mp_ttc->AddWindow(m_md[ETMT_ATTENUATOR].mp_button, MLL::GetString(IDS_TD_VIEW_ATTENUATOR_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_DWELLCNTRL].mp_button, MLL::GetString(IDS_TD_VIEW_DWELL_CONTROL_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_CRKCLT_CORR].mp_button, MLL::GetString(IDS_TD_VIEW_CRKTEMP_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_GASDOSE].mp_button, MLL::GetString(IDS_TD_VIEW_GDP_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_BAROCORR].mp_button, MLL::GetString(IDS_TD_VIEW_BAROCORR_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_CTS_CURVE].mp_button, MLL::GetString(IDS_TD_VIEW_CTS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_ATS_CURVE].mp_button, MLL::GetString(IDS_TD_VIEW_ATS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_ATS_CORR].mp_button, MLL::GetString(IDS_TD_VIEW_ATS_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_MANIGNTIM].mp_button, MLL::GetString(IDS_TD_VIEW_MANIGNTIM_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_TMP2_CURVE].mp_button, MLL::GetString(IDS_TD_VIEW_TMP2_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_EH_PAUSE].mp_button, MLL::GetString(IDS_TD_VIEW_EH_PAUSE_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_CRANKING_THRD].mp_button, MLL::GetString(IDS_TD_CRANKING_THRD_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_CRANKING_TIME].mp_button, MLL::GetString(IDS_TD_CRANKING_TIME_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_SMAPABAN_THRD].mp_button, MLL::GetString(IDS_TD_SMAPABAN_THRD_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_KNOCK_ZONE].mp_button, MLL::GetString(IDS_TD_VIEW_KNOCK_ZONE_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_LAMBDA_ZONE].mp_button, MLL::GetString(IDS_TD_LAMBDA_ZONES_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_GRTS_CURVE].mp_button, MLL::GetString(IDS_TD_GRTS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_GRHEAT_DUTY].mp_button, MLL::GetString(IDS_TD_GRHEAT_DUTY_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_PWMIAC_UCOEF].mp_button, MLL::GetString(IDS_TD_PWMIAC_UCOEF_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_AFTSTR_STRK0].mp_button, MLL::GetString(IDS_TD_AFTSTR_STRK0_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_AFTSTR_STRK1].mp_button, MLL::GetString(IDS_TD_AFTSTR_STRK1_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_GRVDELAY].mp_button, MLL::GetString(IDS_TD_GRVDELAY_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_FTLS_CURVE].mp_button, MLL::GetString(IDS_TD_FTLS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_EGTS_CURVE].mp_button, MLL::GetString(IDS_TD_EGTS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_OPS_CURVE].mp_button, MLL::GetString(IDS_TD_OPS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_MANINJPWC].mp_button, MLL::GetString(IDS_TD_MANINJPWC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_MAF_CURVE].mp_button, MLL::GetString(IDS_TD_MAF_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_FTLSCOR].mp_button, MLL::GetString(IDS_TD_FTLSCOR_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_FTS_CURVE].mp_button, MLL::GetString(IDS_TD_FTS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_FUELDENS_CORR].mp_button, MLL::GetString(IDS_TD_FUELDENS_CORR_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_XTAU_XFACC].mp_button, MLL::GetString(IDS_TD_XTAU_XFACC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_XTAU_XFDEC].mp_button, MLL::GetString(IDS_TD_XTAU_XFDEC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_XTAU_TFACC].mp_button, MLL::GetString(IDS_TD_XTAU_TFACC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_XTAU_TFDEC].mp_button, MLL::GetString(IDS_TD_XTAU_TFDEC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJNONLINP].mp_button, MLL::GetString(IDS_TD_INJNONLINP_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJNONLING].mp_button, MLL::GetString(IDS_TD_INJNONLING_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_EGO_DELAY].mp_button, MLL::GetString(IDS_TD_EGO_DELAY_MAP_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CSeptabsPanel::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
 m_initialized = false;
 m_btnMovIds.clear();
}

void CSeptabsPanel::OnTimer(UINT nIDEvent)
{
 //I know it is dirty hack, but... :-)
 UpdateDialogControls(this,TRUE);
 _EnableCharts(IsAllowed());
 Super::OnTimer(nIDEvent);
}

void CSeptabsPanel::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 if (m_initialized)
 {
  AlignButtons(this, cx, ETMT_DWELLCNTRL, ETMT_ATTENUATOR, ETMT_GASDOSE, ETMT_SEP_START, ETMT_SEP_END);
 }

 if (mp_scr.get())
 {
  DPIAware da;
  if (m_disable_vscroll)
   mp_scr->SetViewSize(cx, da.ScaleY(1));
  else
   mp_scr->SetViewSize(cx, da.ScaleY(m_btnMovIds.empty() ? 1220 : 670));
 }

}

void CSeptabsPanel::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after, x_pos, y_pos, 0, 0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

void CSeptabsPanel::SetPosition(const CRect& rc, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after, rc.left, rc.top, rc.Width(), rc.Height(), (wnd_insert_after ? 0 : SWP_NOZORDER));
}

void CSeptabsPanel::MakeChartsChildren(bool children)
{
 CTablesPanelBase::MakeChartsChildren(children);
 HWND hwnd;

 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
 {
  hwnd = GetMapWindow(i);
  GDIHelpers::MakeWindowChild(hwnd, children);
 }
}

void CSeptabsPanel::OnUpdateViewAttenuatorMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_ATTENUATOR].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewDwellCntrlMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_dwellcntrl_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_DWELLCNTRL].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewCTSCurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_cts_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_CTS_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewATSCurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_ATS_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewATSAACMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_ATS_CORR].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewGasdosePosMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart3DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_gasdose);
 pCmdUI->SetCheck( (m_md[ETMT_GASDOSE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateCESettingsButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsAllowed() && m_enable_ceerr);
}

void CSeptabsPanel::OnUpdateViewBarocorrMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && (m_gasdose || m_fuel_injection));
 pCmdUI->SetCheck( (m_md[ETMT_BAROCORR].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewManIgntimMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_MANIGNTIM].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewTmp2CurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_tmp2_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_TMP2_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewGrtsCurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_grts_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_GRTS_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewCrkTempMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_CRKCLT_CORR].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewEHPauseMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_EH_PAUSE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewCrankingThrdMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_CRANKING_THRD].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewCrankingTimeMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_CRANKING_TIME].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewSmapabanThrdMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_SMAPABAN_THRD].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewKnockZoneMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart3DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_KNOCK_ZONE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewLambdaZoneMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart3DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_LAMBDA_ZONE].state) ? TRUE : FALSE );
}

//Updates controls which state depends on whether or not data is
void CSeptabsPanel::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsAllowed());
}

void CSeptabsPanel::OnUpdateControlsFWC(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsAllowed() && m_enable_fwconsts);
}

void CSeptabsPanel::OnUpdateControlsGRD(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsAllowed() && m_enable_grids);
}

void CSeptabsPanel::OnUpdateViewGrHeatDutyMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_grheat_duty_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_GRHEAT_DUTY].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewPwmIacUCoefMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_pwmiac_ucoef_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_PWMIAC_UCOEF].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewAftstrStrk0Map(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_aftstr_strk_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_AFTSTR_STRK0].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewAftstrStrk1Map(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_aftstr_strk_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_AFTSTR_STRK1].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewGrValDelMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_grheat_duty_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_GRVDELAY].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewFtlsCurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_ftls_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_FTLS_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewFtlsCorMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_ftls_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_FTLSCOR].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewEgtsCurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_egts_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_EGTS_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewOpsCurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_ops_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_OPS_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewManInjPwcMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_maninjpwc_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_MANINJPWC].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewMAFCurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_MAF_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewFtsCurveMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_fts_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_FTS_CURVE].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewFuelDensCorrMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_fts_curve_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_FUELDENS_CORR].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewXtauXfAccMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_xtau_maps_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_XTAU_XFACC].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewXtauXfDecMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_xtau_maps_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_XTAU_XFDEC].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewXtauTfAccMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_xtau_maps_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_XTAU_TFACC].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewXtauTfDecMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_xtau_maps_enabled);
 pCmdUI->SetCheck( (m_md[ETMT_XTAU_TFDEC].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewInjNonLinPMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[ETMT_INJNONLINP].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewInjNonLinGMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[ETMT_INJNONLING].state) ? TRUE : FALSE );
}

void CSeptabsPanel::OnUpdateViewEGODelayMap(CCmdUI* pCmdUI)
{
 BOOL enable = (DLL::Chart2DCreate!=NULL) && IsAllowed();
 pCmdUI->Enable(enable && (m_fuel_injection));
 pCmdUI->SetCheck( (m_md[ETMT_EGO_DELAY].state) ? TRUE : FALSE );
}

void CSeptabsPanel::UpdateOpenedCharts(void)
{
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
 {
  if (!m_md[i].state)
   continue;

  if (Is3DMap(i))
   DLL::Chart3DUpdate(m_md[i].handle, GetMap(i, true), GetMap(i, false));
  else
   DLL::Chart2DUpdate(m_md[i].handle, GetMap(i, true), GetMap(i, false));


  if (i==ETMT_CTS_CURVE)
  {
   DLL::Chart2DUpdateYRange(m_md[i].handle, GetCLTGrid()[0], GetCLTGrid()[15]);
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[16], GetMap(i, false)[16+1]);
  }

  if (i==ETMT_GRTS_CURVE)
  {
   DLL::Chart2DUpdateYRange(m_md[i].handle, GetCLTGrid()[0], GetCLTGrid()[15]);
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[16], GetMap(i, false)[16+1]);
  }

  if (i==ETMT_BAROCORR)
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[9], GetMap(i, false)[9+1]);
  if (i==ETMT_ATS_CURVE)
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[16], GetMap(i, false)[16+1]);
  if (i==ETMT_TMP2_CURVE)
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[16], GetMap(i, false)[16+1]);
  if (i==ETMT_FTLS_CURVE)
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[17], GetMap(i, false)[17+1]);
  if (i==ETMT_EGTS_CURVE)
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[17], GetMap(i, false)[17+1]);
  if (i==ETMT_OPS_CURVE)
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[17], GetMap(i, false)[17+1]);
  if (i==ETMT_FTS_CURVE)
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[17], GetMap(i, false)[17+1]);
  if (i==ETMT_MAF_CURVE)
   DLL::Chart2DUpdateAxisEdits(m_md[i].handle, 1, GetMap(i, false)[64+1], GetMap(i, false)[64+2]);
 }
}

void CSeptabsPanel::EnableDwellControl(bool enable)
{
 m_dwellcntrl_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_DWELLCNTRL].state && ::IsWindow(m_md[ETMT_DWELLCNTRL].handle))
  DLL::Chart2DEnable(m_md[ETMT_DWELLCNTRL].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableCTSCurve(bool enable)
{
 m_cts_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_CTS_CURVE].state && ::IsWindow(m_md[ETMT_CTS_CURVE].handle))
  DLL::Chart2DEnable(m_md[ETMT_CTS_CURVE].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableFuelInjection(bool i_enable)
{
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_BAROCORR].state && ::IsWindow(m_md[ETMT_BAROCORR].handle))
  DLL::Chart2DEnable(m_md[ETMT_BAROCORR].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJNONLINP].state && ::IsWindow(m_md[ETMT_INJNONLINP].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJNONLINP].handle, i_enable && IsAllowed());
 if (m_md[ETMT_INJNONLING].state && ::IsWindow(m_md[ETMT_INJNONLING].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJNONLING].handle, i_enable && IsAllowed());
 if (m_md[ETMT_EGO_DELAY].state && ::IsWindow(m_md[ETMT_EGO_DELAY].handle))
  DLL::Chart2DEnable(m_md[ETMT_EGO_DELAY].handle, i_enable && IsAllowed());
}

void CSeptabsPanel::EnableGasdose(bool enable)
{
 m_gasdose = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_GASDOSE].state && ::IsWindow(m_md[ETMT_GASDOSE].handle))
  DLL::Chart3DEnable(m_md[ETMT_GASDOSE].handle, enable && IsAllowed());
 if (m_md[ETMT_BAROCORR].state && ::IsWindow(m_md[ETMT_BAROCORR].handle))
  DLL::Chart2DEnable(m_md[ETMT_BAROCORR].handle, (enable || m_fuel_injection) && IsAllowed());
}

void CSeptabsPanel::EnableTmp2Curve(bool enable)
{
 m_tmp2_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_TMP2_CURVE].state && ::IsWindow(m_md[ETMT_TMP2_CURVE].handle))
  DLL::Chart2DEnable(m_md[ETMT_TMP2_CURVE].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableGrtsCurve(bool enable)
{
 m_grts_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_GRTS_CURVE].state && ::IsWindow(m_md[ETMT_GRTS_CURVE].handle))
  DLL::Chart2DEnable(m_md[ETMT_GRTS_CURVE].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableGrHeatDutyMap(bool enable)
{
 m_grheat_duty_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_GRHEAT_DUTY].state && ::IsWindow(m_md[ETMT_GRHEAT_DUTY].handle))
  DLL::Chart2DEnable(m_md[ETMT_GRHEAT_DUTY].handle, enable && IsAllowed());
 if (m_md[ETMT_GRVDELAY].state && ::IsWindow(m_md[ETMT_GRVDELAY].handle))
  DLL::Chart2DEnable(m_md[ETMT_GRVDELAY].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnablePwmIacUCoefMap(bool enable)
{
 m_pwmiac_ucoef_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_PWMIAC_UCOEF].state && ::IsWindow(m_md[ETMT_PWMIAC_UCOEF].handle))
  DLL::Chart2DEnable(m_md[ETMT_PWMIAC_UCOEF].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableAftstrStrkMap(bool enable)
{
 m_aftstr_strk_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_AFTSTR_STRK0].state && ::IsWindow(m_md[ETMT_AFTSTR_STRK0].handle))
  DLL::Chart2DEnable(m_md[ETMT_AFTSTR_STRK0].handle, enable && IsAllowed());
 if (m_md[ETMT_AFTSTR_STRK1].state && ::IsWindow(m_md[ETMT_AFTSTR_STRK1].handle))
  DLL::Chart2DEnable(m_md[ETMT_AFTSTR_STRK1].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableFtlsCurve(bool enable)
{
 m_ftls_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_FTLS_CURVE].state && ::IsWindow(m_md[ETMT_FTLS_CURVE].handle))
  DLL::Chart2DEnable(m_md[ETMT_FTLS_CURVE].handle, enable && IsAllowed());
 if (m_md[ETMT_FTLSCOR].state && ::IsWindow(m_md[ETMT_FTLSCOR].handle))
  DLL::Chart2DEnable(m_md[ETMT_FTLSCOR].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableEgtsCurve(bool enable)
{
 m_egts_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_EGTS_CURVE].state && ::IsWindow(m_md[ETMT_EGTS_CURVE].handle))
  DLL::Chart2DEnable(m_md[ETMT_EGTS_CURVE].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableOpsCurve(bool enable)
{
 m_ops_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_OPS_CURVE].state && ::IsWindow(m_md[ETMT_OPS_CURVE].handle))
  DLL::Chart2DEnable(m_md[ETMT_OPS_CURVE].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableManInjPwc(bool enable)
{
 m_maninjpwc_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_MANINJPWC].state && ::IsWindow(m_md[ETMT_MANINJPWC].handle))
  DLL::Chart2DEnable(m_md[ETMT_MANINJPWC].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableLambdaZones(bool enable)
{
 m_lambdazone_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_LAMBDA_ZONE].state && ::IsWindow(m_md[ETMT_LAMBDA_ZONE].handle))
  DLL::Chart3DEnable(m_md[ETMT_LAMBDA_ZONE].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableFtsCurve(bool enable)
{
 m_fts_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_FTS_CURVE].state && ::IsWindow(m_md[ETMT_FTS_CURVE].handle))
  DLL::Chart2DEnable(m_md[ETMT_FTS_CURVE].handle, enable && IsAllowed());
 if (m_md[ETMT_FUELDENS_CORR].state && ::IsWindow(m_md[ETMT_FUELDENS_CORR].handle))
  DLL::Chart2DEnable(m_md[ETMT_FUELDENS_CORR].handle, enable && IsAllowed());
}

void CSeptabsPanel::EnableXtauMaps(bool enable)
{
 m_xtau_maps_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_XTAU_XFACC].state && ::IsWindow(m_md[ETMT_XTAU_XFACC].handle))
  DLL::Chart2DEnable(m_md[ETMT_XTAU_XFACC].handle, enable && IsAllowed());
 if (m_md[ETMT_XTAU_XFDEC].state && ::IsWindow(m_md[ETMT_XTAU_XFDEC].handle))
  DLL::Chart2DEnable(m_md[ETMT_XTAU_XFDEC].handle, enable && IsAllowed());
 if (m_md[ETMT_XTAU_TFACC].state && ::IsWindow(m_md[ETMT_XTAU_TFACC].handle))
  DLL::Chart2DEnable(m_md[ETMT_XTAU_TFACC].handle, enable && IsAllowed());
 if (m_md[ETMT_XTAU_TFDEC].state && ::IsWindow(m_md[ETMT_XTAU_TFDEC].handle))
  DLL::Chart2DEnable(m_md[ETMT_XTAU_TFDEC].handle, enable && IsAllowed());
}

void CSeptabsPanel::OnViewAttenuatorMap()
{
 MapData &md = m_md[ETMT_ATTENUATOR];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0f, 63, m_attenuator_table_slots, 128,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ATTENUATOR_GAIN_UNIT).c_str(),
    MLL::GetString(IDS_ATTENUATOR_MAP).c_str(), false);
  DLL::Chart2DSetMarksVisible(md.handle,1, false); //пр€чем надписи над узловыми точками функции
  DLL::Chart2DSetOnChange(md.handle,OnChangeAttenuatorTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAttenuatorTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationAttenuatorTable, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_Y_AXIS, OnGetAttenuatorYAxisLabel, this);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_Y_AXIS, _T("#0.00"));
  DLL::Chart2DInverseAxis(md.handle, CXD_Y_AXIS, true);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_ATTENUATOR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewDwellCntrlMap()
{
 MapData &md = m_md[ETMT_DWELLCNTRL];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.25f, 16.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DWELLCNTRL_UNIT).c_str(),
    MLL::GetString(IDS_DWELLCNTRL_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.01f"));
  DLL::Chart2DSetOnChange(md.handle, OnChangeDwellCntrlTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseDwellCntrlTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationDwellCntrlTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_DWELLCNTRL);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewCTSCurveMap()
{
 MapData &md = m_md[ETMT_CTS_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, GetCLTGrid()[0], GetCLTGrid()[15], NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_CTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeCTSXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeCTSCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseCTSCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationCTSCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[16], md.active[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_CTS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewATSCurveMap()
{
 MapData &md = m_md[ETMT_ATS_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_ATS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeATSXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeATSCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseATSCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationATSCurveTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[16], md.active[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_ATS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewATSAACMap()
{
 MapData &md = m_md[ETMT_ATS_CORR];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -15.0, 25.0, SECU3IO::temp_map_tmp_slots, 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_ATSCORR_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(md.handle,OnChangeATSAACTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseATSAACTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationATSAACTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_ATS_CORR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewGasdosePosMap()
{
 MapData &md = m_md[ETMT_GASDOSE];
 //if button has been turned off, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active, GetRPMGrid(), 16, 16, 0, 100.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_GDP_UNIT).c_str(),
    MLL::GetString(IDS_GDP_MAP).c_str());
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeGasdoseTable,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseGasdoseTable,this);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationGasdoseTable, this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_GASDOSE);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewBarocorrMap()
{
 MapData &md = m_md[ETMT_BAROCORR];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 80.0, 120.0, NULL, 9,
    MLL::GetString(IDS_MAPS_ATMOPRESS_UNIT).c_str(),  //horizontal axis
    MLL::GetString(IDS_MAPS_COEFFP_UNIT).c_str(),     //vertical axis
    MLL::GetString(IDS_BAROCORR_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 60.0f, 110.0f, 60.0f, 110.0f, 0.1f, 5, 1, OnChangeBarocorrXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeBarocorrTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseBarocorrTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationBarocorrTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[9], md.active[9+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_BAROCORR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewTmp2CurveMap()
{
 MapData &md = m_md[ETMT_TMP2_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_TMP2_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeTmp2CurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeTmp2CurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseTmp2CurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationTmp2CurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[16], md.active[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_TMP2_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewGrtsCurveMap()
{
 MapData &md = m_md[ETMT_GRTS_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_GRTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeGrtsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeGrtsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseGrtsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationGrtsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[16], md.active[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_GRTS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewManIgntimMap()
{
 MapData &md = m_md[ETMT_MANIGNTIM];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -15.0, 15.0, SECU3IO::manigntim_map_slots, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_MANIGNTIM_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.0"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnChange(md.handle,OnChangeManIgntimTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseManIgntimTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationManIgntimTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_MANIGNTIM);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewCrkTempMap()
{
 MapData &md = m_md[ETMT_CRKCLT_CORR];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,-15.0,25.0, GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_CRKTEMP_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(md.handle,OnChangeCrkTempTable,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseCrkTempTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationCrkTempTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_CRKCLT_CORR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewEHPauseMap()
{
 MapData &md = m_md[ETMT_EH_PAUSE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.01f, 2.55f, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_EH_PAUSE_UNIT).c_str(),
    MLL::GetString(IDS_EH_PAUSE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.01f"));
  DLL::Chart2DSetOnChange(md.handle, OnChangeEHPauseTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseEHPauseTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationEHPauseTable, this);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_EH_PAUSE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewCrankingThrdMap()
{
 MapData &md = m_md[ETMT_CRANKING_THRD];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.0f,2500.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_CRANKING_THRD_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationCrankingThrdMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeCrankingThrdMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseCrankingThrdMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_CRANKING_THRD);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewCrankingTimeMap()
{
 MapData &md = m_md[ETMT_CRANKING_TIME];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0f, 255.0f, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_STROKE_UNIT).c_str(),
    MLL::GetString(IDS_CRANKING_TIME_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationCrankingTimeMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeCrankingTimeMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseCrankingTimeMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_CRANKING_TIME);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewSmapabanThrdMap()
{
 MapData &md = m_md[ETMT_SMAPABAN_THRD];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0f, 2500.0f, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_SMAPABAN_THRD_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationSmapabanThrdMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeSmapabanThrdMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseSmapabanThrdMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_SMAPABAN_THRD);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewKnockZoneMap()
{
 MapData &md = m_md[ETMT_KNOCK_ZONE];
 //if button has been turned off, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active, GetRPMGrid(), 16, 16, 0, 1.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_KNOCK_ZONE_UNIT).c_str(),
    MLL::GetString(IDS_KNOCK_ZONE_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeKnockZoneMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseKnockZoneMap,this);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationKnockZoneMap, this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_KNOCK_ZONE);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewLambdaZoneMap()
{
 MapData &md = m_md[ETMT_LAMBDA_ZONE];
 //if button has been turned off, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active, GetRPMGrid(), 16, 16, 0, 1.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_LAMBDA_ZONE_UNIT).c_str(),
    MLL::GetString(IDS_LAMBDA_ZONE_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeLambdaZoneMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseLambdaZoneMap,this);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationLambdaZoneMap, this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_LAMBDA_ZONE);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewGrHeatDutyMap()
{
 MapData &md = m_md[ETMT_GRHEAT_DUTY];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 100.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DUTY_UNIT).c_str(),
    MLL::GetString(IDS_GRHEAT_DUTY_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(md.handle,OnChangeGrHeatDutyTable,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseGrHeatDutyTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationGrHeatDutyTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_GRHEAT_DUTY);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewPwmIacUCoefMap()
{
 MapData &md = m_md[ETMT_PWMIAC_UCOEF];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 3.0, SECU3IO::voltage_map_slots, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_PWMIAC_UCOEF_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.01f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetOnChange(md.handle,OnChangePwmIacUCoefMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnClosePwmIacUCoefMap,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationPwmIacUCoefMap, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_PWMIAC_UCOEF);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewAftstrStrk0Map()
{
 MapData &md = m_md[ETMT_AFTSTR_STRK0];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 1.0, 2000.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(), //x unit
    MLL::GetString(IDS_MAPS_STROKE_UNIT).c_str(),      //y unit
    MLL::GetString(IDS_AFTSTR_STRK0_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.0f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeAftstrStrk0Map,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAftstrStrk0Map,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationAftstrStrk0Map, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_AFTSTR_STRK0);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(m_md[ETMT_AFTSTR_STRK0].handle);
 }
}

void CSeptabsPanel::OnViewAftstrStrk1Map()
{
 MapData &md = m_md[ETMT_AFTSTR_STRK1];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 1.0, 2000.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(), //x unit
    MLL::GetString(IDS_MAPS_STROKE_UNIT).c_str(),      //y unit
    MLL::GetString(IDS_AFTSTR_STRK1_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.0f"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnChange(md.handle,OnChangeAftstrStrk1Map,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAftstrStrk1Map,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationAftstrStrk1Map, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_AFTSTR_STRK1);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewGrValDelMap()
{
 MapData &md = m_md[ETMT_GRVDELAY];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 120.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_GRVDELAY_UNIT).c_str(),
    MLL::GetString(IDS_GRVDELAY_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(md.handle,OnChangeGrValDelMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseGrValDelMap,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationGrValDelMap, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_GRVDELAY);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewFtlsCurveMap()
{
 MapData &md = m_md[ETMT_FTLS_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 100.0, NULL, 17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_FUELTANK_UNIT).c_str(),
    MLL::GetString(IDS_FTLS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeFtlsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeFtlsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseFtlsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationFtlsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[17], md.active[17+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_FTLS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewFtlsCorMap()
{
 MapData &md = m_md[ETMT_FTLSCOR];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 3.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_FTLSCOR_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.01f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetOnChange(md.handle,OnChangeFtlsCorTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseFtlsCorTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationFtlsCorTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_FTLSCOR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewEgtsCurveMap()
{
 MapData &md = m_md[ETMT_EGTS_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 1100.0, NULL, 17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_EGTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.0"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeEgtsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeEgtsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseEgtsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationEgtsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[17], md.active[17+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_EGTS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewFtsCurveMap()
{
 MapData &md = m_md[ETMT_FTS_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -40.0, 120.0, NULL, 17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_FTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.0"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeFtsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeFtsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseFtsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationFtsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[17], md.active[17+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_FTS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}


void CSeptabsPanel::OnViewOpsCurveMap()
{
 MapData &md = m_md[ETMT_OPS_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 6.0, NULL, 17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_PRESSUREKG_UNIT).c_str(),
    MLL::GetString(IDS_OPS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeOpsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeOpsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseOpsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationOpsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[17], md.active[17+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_OPS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewManInjPwcMap()
{
 MapData &md = m_md[ETMT_MANINJPWC];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.5, 1.5, SECU3IO::maninjpwc_map_slots, 17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_MANINJPWC_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.03f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnChange(md.handle,OnChangeManInjPwcTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseManInjPwcTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationManInjPwcTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_MANINJPWC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewMAFCurveMap()
{
 MapData &md = m_md[ETMT_MAF_CURVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 650.0, /*m_mafcurve_slots*/NULL, 64,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAF_UNIT).c_str(),
    MLL::GetString(IDS_MAF_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.03f"));
  DLL::Chart2DSetAxisEdits(md.handle, CXD_X_AXIS, true, 0, 5.00f, 0, 5.00f, 0.01f, -1, -1, OnChangeMAFCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnChange(md.handle,OnChangeMAFCurveTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseMAFCurveTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationMAFCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, CXD_X_AXIS, md.active[64+1], md.active[64+2]);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_MAF_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewFuelDensCorrMap()
{
 MapData &md = m_md[ETMT_FUELDENS_CORR];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.75, 1.25, SECU3IO::temp_map_tmp_slots, 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_FUELDENS_CORR_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.0000"));
  DLL::Chart2DSetOnChange(md.handle,OnChangeFuelDensCorrTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseFuelDensCorrTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationFuelDensCorrTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_FUELDENS_CORR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CSeptabsPanel::OnViewXtauXfAccMap()
{
 MapData &md = m_md[ETMT_XTAU_XFACC];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 5.0, 95.0, GetRPMGrid(), 16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_XTAU_XF_UNIT).c_str(),
    MLL::GetString(IDS_TD_XTAU_XFACC_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationXtauXfAccMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeXtauXfAccMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseXtauXfAccMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening this window
  OnOpenMapWnd(md.handle, ETMT_XTAU_XFACC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CSeptabsPanel::OnViewXtauXfDecMap()
{
 MapData &md = m_md[ETMT_XTAU_XFDEC];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 5.0, 95.0, GetRPMGrid(), 16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_XTAU_XF_UNIT).c_str(),
    MLL::GetString(IDS_TD_XTAU_XFDEC_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationXtauXfDecMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeXtauXfDecMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseXtauXfDecMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening this window
  OnOpenMapWnd(md.handle, ETMT_XTAU_XFDEC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CSeptabsPanel::OnViewXtauTfAccMap()
{
 MapData &md = m_md[ETMT_XTAU_TFACC];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 2.0, 6000.0, GetRPMGrid(), 16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_XTAU_TF_UNIT).c_str(),
    MLL::GetString(IDS_TD_XTAU_TFACC_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationXtauTfAccMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeXtauTfAccMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseXtauTfAccMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening this window
  OnOpenMapWnd(md.handle, ETMT_XTAU_TFACC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CSeptabsPanel::OnViewXtauTfDecMap()
{
 MapData &md = m_md[ETMT_XTAU_TFDEC];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 2.0, 6000.0, GetRPMGrid(), 16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_XTAU_TF_UNIT).c_str(),
    MLL::GetString(IDS_TD_XTAU_TFDEC_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationXtauTfDecMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeXtauTfDecMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseXtauTfDecMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening this window
  OnOpenMapWnd(md.handle, ETMT_XTAU_TFDEC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewInjNonLinPMap()
{
 MapData &md = m_md[ETMT_INJNONLINP];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[5] = {0.0f, 3.0f, 0.1f, 3.0f, 0.05f}; //min 0ms, max 3ms, inc 0.1, 3 dec places, min diff 0.05ms
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 3.0, bins_lims, 8,
    MLL::GetString(IDS_MAPS_INJPW_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_INJPW_UNIT).c_str(),
    MLL::GetString(IDS_INJNONLINP_MAP).c_str(), true);  //use bins on horizontal axis
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.03f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetOnChange(md.handle,OnChangeInjNonLinPTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseInjNonLinPTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationInjNonLinPTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJNONLINP);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewInjNonLinGMap()
{
 MapData &md = m_md[ETMT_INJNONLING];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[5] = {0.0f, 3.0f, 0.1f, 3.0f, 0.05f}; //min 0ms, max 3ms, inc 0.1, 3 dec places, min diff 0.05ms
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 3.0, bins_lims, 8,
    MLL::GetString(IDS_MAPS_INJPW_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_INJPW_UNIT).c_str(),
    MLL::GetString(IDS_INJNONLING_MAP).c_str(), true); //use bins on horizontal axis
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.03f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetOnChange(md.handle,OnChangeInjNonLinGTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseInjNonLinGTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationInjNonLinGTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJNONLING);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnViewEGODelayMap()
{
 MapData &md = m_md[ETMT_EGO_DELAY];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  if (!ldaxIsUseBaroMax())
   m_work_map_load_slots = MathHelpers::BuildGridFromRange(ldaxGetMin(), ldaxGetMax(), 16);
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 1.0, 2000.0, GetLoadGrid(), 16,
    ldaxGetTitle(), //x unit
    MLL::GetString(IDS_MAPS_STROKE_UNIT).c_str(),  //y unit
    MLL::GetString(IDS_EGO_DELAY_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.0f"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelLoad, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnChange(md.handle,OnChangeEGODelayMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseEGODelayMap,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationEGODelayMap, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_EGO_DELAY);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CSeptabsPanel::OnDwellCalcButton()
{
 CDwellCalcDlg dialog;
 dialog.SetVoltageValues(SECU3IO::dwellcntrl_map_slots, 32);
 dialog.SetLimits(0.25f, 16.0f);
 if (dialog.DoModal()==IDOK)
 {
  //copy results
  std::copy(dialog.GetTimeValues(), dialog.GetTimeValues() + 32, GetMap(ETMT_DWELLCNTRL, false));
  if (DLL::Chart2DCreate)
  {//Update charts and notify about changes
   UpdateOpenedCharts();
   if (this->m_OnMapChanged)
    this->m_OnMapChanged(ETMT_DWELLCNTRL);
  }
 }
}

void CSeptabsPanel::OnRPMGridButton()
{
 if (m_OnRPMGridEditButton)
  m_OnRPMGridEditButton();
}

void CSeptabsPanel::OnFwConstsButton()
{
 if (m_OnFwConstsButton)
  m_OnFwConstsButton();
}

void CSeptabsPanel::OnCESettingsButton()
{
 if (m_OnCESettingsButton)
  m_OnCESettingsButton();
 m_edit_cesettings_btn.SetCheck(BST_UNCHECKED);
}

float* CSeptabsPanel::GetMap(int mapId, bool i_original)
{
 return i_original ? m_md[mapId].original : m_md[mapId].active;
}

HWND CSeptabsPanel::GetMapWindow(int wndType)
{
 if (wndType >= ETMT_SEP_START && wndType <= ETMT_SEP_END)
  return m_md[wndType].state ? m_md[wndType].handle : NULL;
 else
  return NULL;
}

void CSeptabsPanel::setOnRPMGridEditButton(EventHandler OnFunction)
{m_OnRPMGridEditButton = OnFunction;}

void CSeptabsPanel::setOnFwConstsButton(EventHandler OnFunction)
{m_OnFwConstsButton = OnFunction;}

void CSeptabsPanel::setOnCESettingsButton(EventHandler OnFunction)
{m_OnCESettingsButton = OnFunction;}

void CSeptabsPanel::CloseCharts(void)
{
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
 {
  HWND hwnd = GetMapWindow(i);
  if (hwnd)
   ::SendMessage(hwnd, WM_CLOSE, 0, 0);
 }
}

void CSeptabsPanel::ShowOpenedCharts(bool i_show)
{
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
 {
  HWND hwnd = GetMapWindow(i);
  if (hwnd)
   ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 }
}

void CSeptabsPanel::OnShow(bool show)
{
 if (show)
  SetTimer(TIMER_ID, 250, NULL);
 else
  KillTimer(TIMER_ID);
}

void CSeptabsPanel::_EnableCharts(bool enable)
{
 if (m_charts_enabled != (int)enable)
 {
  for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
  {
   if (m_md[i].state && ::IsWindow(m_md[i].handle))
   {
    if (Is3DMap(i))
     DLL::Chart3DEnable(m_md[i].handle, enable && IsAllowed());
    else
     DLL::Chart2DEnable(m_md[i].handle, enable && IsAllowed());
   }
  }
 }

 m_charts_enabled = enable;
}

void CSeptabsPanel::EnableEmbedMapWnd(bool embed, const CRect& rc)
{
 CTablesPanelBase::EnableEmbedMapWnd(embed, rc, ETMT_SEP_START, ETMT_SEP_END);
}

void CSeptabsPanel::SetLoadAxisCfg(float minVal, float maxVal, int ldaxCfg, bool useTable, bool forceUpdate /*= false*/)
{
 bool updateTitle = (ldaxCfg != ldaxGetCfg() && ::IsWindow(m_md[ETMT_EGO_DELAY].handle));
  
 LdaxCfg::SetLoadAxisCfg(minVal, maxVal, ldaxCfg, useTable);

 if (updateTitle)
 {
  DLL::Chart2DSetAxisTitle(m_md[ETMT_EGO_DELAY].handle, CXD_X_AXIS, ldaxGetTitle());
 }

 //do not update if MAP(baro) is selected! Because if MAP(baro) is selected, upper pressure will be updated in SetDynamicValues() method
 if ((m_ldaxNeedsUpdate && !ldaxIsUseBaroMax()) || forceUpdate)
 {
  m_work_map_load_slots = MathHelpers::BuildGridFromRange(ldaxGetMin(), ldaxGetBaroMax(), 16);
  if (::IsWindow(m_md[ETMT_EGO_DELAY].handle))
   DLL::Chart2DRefresh(m_md[ETMT_EGO_DELAY].handle); //make chart to refresh axis
  m_ldaxNeedsUpdate = false;
 }
}

void CSeptabsPanel::SetDynamicValues(const SECU3IO::SensorDat& dv)
{
 //Update load axis either if axis configuration has been changed or a barometer value has been changed
 if (m_ldaxNeedsUpdate || ldaxWatchBaroPress(dv.baro_press))
 {
  m_work_map_load_slots = MathHelpers::BuildGridFromRange(ldaxGetMin(), ldaxGetBaroMax(), 16);
  if (::IsWindow(m_md[ETMT_EGO_DELAY].handle))
   DLL::Chart2DRefresh(m_md[ETMT_EGO_DELAY].handle); //make chart to refresh axis
  m_ldaxNeedsUpdate = false;
 }
}
