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

/** \file ButtonsPanel.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "AutoTuneController.h"
#include "ButtonsPanel.h"
#include "common/dpiaware.h"
#include "common/MathHelpers.h"
#include "common/GDIHelpers.h"
#include "common/chartxdid.h"
#include "DLLLinkedFunctions.h"
#include "GridModeEditorIgnDlg.h"
#include "io-core/secu3io.h"
#include "io-core/MapIds.h"
#include "ui-core/WndScroller.h"
#include "ui-core/MapEditorCtrl.h"
#include "ui-core/ToolTipCtrlEx.h"

#define TIMER_ID 0

static const float splitAngMin = -20.0f;
static const float splitAngMax =  20.0f;

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel dialog

CButtonsPanel::CButtonsPanel(bool enableAutoTune /*=false*/, bool onlineMode /*=false*/, bool disable_vscroll /*= false*/)
: m_en_aa_indication(false)
, m_carb_afr(false)
, m_en_gas_corr(false)
, m_choke_op_enabled(false)
, mp_autoTuneCntr(enableAutoTune ? new CAutoTuneController() : NULL)
, m_onlineMode(onlineMode)
, m_disable_vscroll(disable_vscroll)
, m_it_mode(0)
, m_active_ve(0)
, m_splitAng(false)
, m_ve2_map_func(0)
, m_initialized(false)
{
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
 {
  MapData md;
  md.mp_button = new CButton;
  m_md.insert(std::make_pair(i, md));
 }
 //add two pseudo maps
 for(int i = ETMT_GME_IGN_WND; i <= ETMT_GME_INJ_WND; ++i)
 {
  MapData md;
  md.mp_button = new CButton;
  m_md.insert(std::make_pair(i, md));
 }

 m_ve2_map_load_slots.reserve(32);
 m_ve2_map_load_slots = MathHelpers::BuildGridFromRange(0.0f, 100.0f, 16); 
}

CButtonsPanel::~CButtonsPanel()
{
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  delete m_md[i].mp_button;
 for(int i = ETMT_GME_IGN_WND; i <= ETMT_GME_INJ_WND; ++i)
  delete m_md[i].mp_button;
}

BOOL CButtonsPanel::Create(CWnd* pParentWnd /*= NULL*/)
{
 return Super::Create(IDD_TD_BUTTONS_PANEL, pParentWnd);
}

void CButtonsPanel::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_VIEW_WORK_MAP, *m_md[ETMT_IGN_WORK].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_TEMP_MAP, *m_md[ETMT_IGN_TEMP].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_TEMPI_MAP, *m_md[ETMT_IGN_TEMPI].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_START_MAP, *m_md[ETMT_IGN_START].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IDLE_MAP, *m_md[ETMT_IGN_IDLE].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_VE_MAP, *m_md[ETMT_INJ_VE].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_VE2_MAP, *m_md[ETMT_INJ_VE2].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IVE_MAP, *m_md[ETMT_INJ_IVE].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_AFR_MAP, *m_md[ETMT_INJ_AFR].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_CRNK_MAP, *m_md[ETMT_INJ_CRNK].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_WRMP_MAP, *m_md[ETMT_INJ_WRMP].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_DEAD_MAP, *m_md[ETMT_INJ_DEAD].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IDLR_MAP, *m_md[ETMT_INJ_IDLR].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IDLC_MAP, *m_md[ETMT_INJ_IDLC].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_THRASS_MAP, *m_md[ETMT_INJ_THRASS].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_AETPS_MAP, *m_md[ETMT_INJ_AETPS].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_AEMAP_MAP, *m_md[ETMT_INJ_AEMAP].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_AERPM_MAP, *m_md[ETMT_INJ_AERPM].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_AFTSTR_MAP, *m_md[ETMT_INJ_AFTSTR].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IT_MAP, *m_md[ETMT_INJ_IT].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_ITRPM_MAP, *m_md[ETMT_INJ_ITRPM].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_RIGID_MAP, *m_md[ETMT_INJ_RIGID].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_EGOCRV_MAP, *m_md[ETMT_INJ_EGOCRV].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IACC_MAP, *m_md[ETMT_INJ_IACC].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IACCW_MAP, *m_md[ETMT_INJ_IACCW].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IATCLT_MAP, *m_md[ETMT_INJ_IATCLT].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_TPSSWT_MAP, *m_md[ETMT_INJ_TPSSWT].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_GTSC_MAP, *m_md[ETMT_INJ_GTSC].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_GPSC_MAP, *m_md[ETMT_INJ_GPSC].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_ATSC_MAP, *m_md[ETMT_INJ_ATSC].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_PWM1_MAP, *m_md[ETMT_PWM1].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_PWM2_MAP, *m_md[ETMT_PWM2].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_IACMAT_MAP, *m_md[ETMT_INJ_IACMAT].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_TPSZON_MAP, *m_md[ETMT_INJ_TPSZON].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_CYLMULT_MAP, *m_md[ETMT_INJ_CYLMULT].mp_button);
 DDX_Control(pDX, IDC_TD_VIEW_CYLADD_MAP, *m_md[ETMT_INJ_CYLADD].mp_button);
 DDX_Control(pDX, IDC_TD_GME_IGN_CHECK, *m_md[ETMT_GME_IGN_WND].mp_button); //pseudo map
 DDX_Control(pDX, IDC_TD_GME_INJ_CHECK, *m_md[ETMT_GME_INJ_WND].mp_button); //pseudo map
 DDX_Control(pDX, IDC_TD_SETIDX, m_setidx);
}

BEGIN_MESSAGE_MAP(CButtonsPanel, Super)
 ON_WM_TIMER()
 ON_WM_DESTROY()
 ON_WM_SIZE()

 ON_BN_CLICKED(IDC_TD_VIEW_START_MAP,OnViewStartMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IDLE_MAP, OnViewIdleMap)
 ON_BN_CLICKED(IDC_TD_VIEW_WORK_MAP, OnViewWorkMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TEMP_MAP, OnViewTempMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TEMPI_MAP, OnViewTempIdlMap)
 ON_BN_CLICKED(IDC_TD_VIEW_VE_MAP, OnViewVEMap)
 ON_BN_CLICKED(IDC_TD_VIEW_VE2_MAP, OnViewVE2Map)
 ON_BN_CLICKED(IDC_TD_VIEW_IVE_MAP, OnViewIVEMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AFR_MAP, OnViewAFRMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CRNK_MAP, OnViewCrnkMap)
 ON_BN_CLICKED(IDC_TD_VIEW_WRMP_MAP, OnViewWrmpMap)
 ON_BN_CLICKED(IDC_TD_VIEW_DEAD_MAP, OnViewDeadMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IDLR_MAP, OnViewIdlrMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IDLC_MAP, OnViewIdlcMap)
 ON_BN_CLICKED(IDC_TD_VIEW_THRASS_MAP, OnViewThrassMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AETPS_MAP, OnViewAETPSMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AEMAP_MAP, OnViewAEMAPMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AERPM_MAP, OnViewAERPMMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AFTSTR_MAP, OnViewAftstrMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IT_MAP, OnViewITMap)
 ON_BN_CLICKED(IDC_TD_VIEW_ITRPM_MAP, OnViewITRPMMap)
 ON_BN_CLICKED(IDC_TD_VIEW_RIGID_MAP, OnViewRigidMap)
 ON_BN_CLICKED(IDC_TD_VIEW_EGOCRV_MAP, OnViewEGOCrvMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IACC_MAP, OnViewIACCMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IACCW_MAP, OnViewIACCWMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IATCLT_MAP, OnViewIATCLTMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TPSSWT_MAP, OnViewTpsswtMap)
 ON_BN_CLICKED(IDC_TD_VIEW_GTSC_MAP, OnViewGtscMap)
 ON_BN_CLICKED(IDC_TD_VIEW_GPSC_MAP, OnViewGpscMap)
 ON_BN_CLICKED(IDC_TD_VIEW_ATSC_MAP, OnViewAtscMap)
 ON_BN_CLICKED(IDC_TD_VIEW_PWM1_MAP, OnViewPwm1Map)
 ON_BN_CLICKED(IDC_TD_VIEW_PWM2_MAP, OnViewPwm2Map)
 ON_BN_CLICKED(IDC_TD_VIEW_IACMAT_MAP, OnViewIACMATMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TPSZON_MAP, OnViewTpszonMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CYLMULT_MAP, OnViewCylMultMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CYLADD_MAP, OnViewCylAddMap)
 ON_BN_CLICKED(IDC_TD_GME_IGN_CHECK, OnGridModeEditingIgn) //pseudo map
 ON_BN_CLICKED(IDC_TD_GME_INJ_CHECK, OnGridModeEditingInj) //pseudo map

 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_START_MAP,OnUpdateViewStartMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IDLE_MAP, OnUpdateViewIdleMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_WORK_MAP, OnUpdateViewWorkMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TEMP_MAP, OnUpdateViewTempMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TEMPI_MAP, OnUpdateViewTempIdlMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_VE_MAP, OnUpdateViewVEMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_VE2_MAP, OnUpdateViewVE2Map)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IVE_MAP, OnUpdateViewIVEMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AFR_MAP, OnUpdateViewAFRMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CRNK_MAP, OnUpdateViewCrnkMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_WRMP_MAP, OnUpdateViewWrmpMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_DEAD_MAP, OnUpdateViewDeadMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IDLR_MAP, OnUpdateViewIdlrMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IDLC_MAP, OnUpdateViewIdlcMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_THRASS_MAP, OnUpdateViewThrassMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AETPS_MAP, OnUpdateViewAETPSMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AEMAP_MAP, OnUpdateViewAEMAPMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AERPM_MAP, OnUpdateViewAERPMMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AFTSTR_MAP, OnUpdateViewAftstrMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IT_MAP, OnUpdateViewITMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ITRPM_MAP, OnUpdateViewITRPMMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_RIGID_MAP, OnUpdateViewRigidMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_EGOCRV_MAP, OnUpdateViewEGOCrvMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IACC_MAP, OnUpdateViewIACCMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IACCW_MAP, OnUpdateViewIACCWMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IATCLT_MAP, OnUpdateViewIATCLTMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TPSSWT_MAP, OnUpdateViewTpsswtMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_GTSC_MAP, OnUpdateViewGtscMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_GPSC_MAP, OnUpdateViewGpscMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATSC_MAP, OnUpdateViewAtscMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_PWM1_MAP, OnUpdateViewPwm1Map)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_PWM2_MAP, OnUpdateViewPwm2Map)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IACMAT_MAP, OnUpdateViewIACMATMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TPSZON_MAP, OnUpdateViewTpszonMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CYLMULT_MAP, OnUpdateViewCylMultMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CYLADD_MAP, OnUpdateViewCylAddMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_GME_IGN_CHECK, OnUpdateGridModeEditingIgn) //pseudo map
 ON_UPDATE_COMMAND_UI(IDC_TD_GME_INJ_CHECK, OnUpdateGridModeEditingInj) //pseudo map
 ON_UPDATE_COMMAND_UI(IDC_TD_SETIDX, OnUpdateControls)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel message handlers

BOOL CButtonsPanel::OnInitDialog()
{
 Super::OnInitDialog();

 m_initialized = true;

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_GME_IGN_WND].mp_button, MLL::GetString(IDS_TD_GME_IGN_CHECK_TT))); //pseudo map
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_GME_INJ_WND].mp_button, MLL::GetString(IDS_TD_GME_INJ_CHECK_TT))); //pseudo map
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_IGN_START].mp_button, MLL::GetString(IDS_TD_VIEW_START_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_IGN_IDLE].mp_button, MLL::GetString(IDS_TD_VIEW_IDLE_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_IGN_WORK].mp_button, MLL::GetString(IDS_TD_VIEW_WORK_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_IGN_TEMP].mp_button, MLL::GetString(IDS_TD_VIEW_TEMP_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_IGN_TEMPI].mp_button, MLL::GetString(IDS_TD_VIEW_TEMPI_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_CRNK].mp_button, MLL::GetString(IDS_TD_VIEW_CRNK_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_VE].mp_button, MLL::GetString(IDS_TD_VIEW_VE_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_VE2].mp_button, MLL::GetString(IDS_TD_VIEW_VE2_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_IVE].mp_button, MLL::GetString(IDS_TD_VIEW_IVE_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_AFR].mp_button, MLL::GetString(IDS_TD_VIEW_AFR_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_WRMP].mp_button, MLL::GetString(IDS_TD_VIEW_WRMP_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_ATSC].mp_button, MLL::GetString(IDS_TD_VIEW_ATSC_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_AETPS].mp_button, MLL::GetString(IDS_TD_VIEW_AETPS_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_AEMAP].mp_button, MLL::GetString(IDS_TD_VIEW_AEMAP_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_AERPM].mp_button, MLL::GetString(IDS_TD_VIEW_AERPM_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_AFTSTR].mp_button, MLL::GetString(IDS_TD_VIEW_AFTSTR_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_GTSC].mp_button, MLL::GetString(IDS_TD_VIEW_GTSC_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_GPSC].mp_button, MLL::GetString(IDS_TD_VIEW_GPSC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_DEAD].mp_button, MLL::GetString(IDS_TD_VIEW_DEAD_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_IDLR].mp_button, MLL::GetString(IDS_TD_VIEW_IDLR_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_IDLC].mp_button, MLL::GetString(IDS_TD_VIEW_IDLC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_THRASS].mp_button, MLL::GetString(IDS_TD_VIEW_THRASS_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_IT].mp_button, MLL::GetString(IDS_TD_VIEW_IT_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_ITRPM].mp_button, MLL::GetString(IDS_TD_VIEW_ITRPM_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_RIGID].mp_button, MLL::GetString(IDS_TD_VIEW_RIGID_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_EGOCRV].mp_button, MLL::GetString(IDS_TD_VIEW_EGOCRV_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_IACC].mp_button, MLL::GetString(IDS_TD_VIEW_IACC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_IACCW].mp_button, MLL::GetString(IDS_TD_VIEW_IACCW_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_IATCLT].mp_button, MLL::GetString(IDS_TD_VIEW_IATCLT_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_TPSSWT].mp_button, MLL::GetString(IDS_TD_VIEW_TPSSWT_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_PWM1].mp_button, MLL::GetString(IDS_TD_VIEW_PWM1_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_PWM2].mp_button, MLL::GetString(IDS_TD_VIEW_PWM2_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_IACMAT].mp_button, MLL::GetString(IDS_TD_VIEW_IACMAT_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_TPSZON].mp_button, MLL::GetString(IDS_TD_VIEW_TPSZON_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_CYLMULT].mp_button, MLL::GetString(IDS_TD_VIEW_CYLMULT_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(m_md[ETMT_INJ_CYLADD].mp_button, MLL::GetString(IDS_TD_VIEW_CYLADD_MAP_TT))); 
 
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewStartMap()
{
 MapData &md = m_md[ETMT_IGN_START];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,-15.0,55.0,SECU3IO::start_map_rpm_slots,16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_START_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationStartMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeStartMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle,OnChangeSettingsCME,this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseStartMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_IGN_START);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewIdleMap()
{
 MapData &md = m_md[ETMT_IGN_IDLE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,-15.0,55.0,GetRPMGrid(),16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_IDLE_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationIdleMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeIdleMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseIdleMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_IGN_IDLE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewWorkMap()
{
 MapData &md = m_md[ETMT_IGN_WORK];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active,GetRPMGrid(),16,16,-15.0,55.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_WORK_MAP).c_str());
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationWorkMap, this);
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeWorkMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseWorkMap,this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_IGN_WORK);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewTempMap()
{
 MapData &md = m_md[ETMT_IGN_TEMP];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,-15.0,25.0,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_TEMPCORR_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationTempMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeTempMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseTempMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_IGN_TEMP);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewTempIdlMap()
{
 MapData &md = m_md[ETMT_IGN_TEMPI];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,-15.0,25.0,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_TEMPCORRI_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationTempIdlMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeTempIdlMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseTempIdlMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_IGN_TEMPI);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewVEMap()
{
 MapData &md = m_md[ETMT_INJ_VE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active,GetRPMGrid(),16,16,0.0f,1.99f,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_VE_UNIT).c_str(),
    MLL::GetString(IDS_VE_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationVEMap, this);
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeVEMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseVEMap,this);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_VE);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewVE2Map()
{
 MapData &md = m_md[ETMT_INJ_VE2];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active,GetRPMGrid(),16,16,0.0f,1.99f,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_VE_UNIT).c_str(),
    MLL::GetString(IDS_VE2_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationVE2Map, this);
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeVE2Map,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseVE2Map,this);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_VE2);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewIVEMap()
{
 MapData &md = m_md[ETMT_INJ_IVE];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active,GetIRPMGrid(),8,8,0.0f,1.99f,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_VE_UNIT).c_str(),
    MLL::GetString(IDS_IVE_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationIVEMap, this);
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelIRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeIVEMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseIVEMap,this);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_IVE);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewAFRMap()
{
 MapData &md = m_md[ETMT_INJ_AFR];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active,GetRPMGrid(),16,16,8.0f,22.0f,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFR_UNIT).c_str(),
    MLL::GetString(IDS_AFR_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(md.handle, _T("#00.0"));
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationAFRMap, this);
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeAFRMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseAFRMap,this);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_AFR);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewCrnkMap()
{
 MapData &md = m_md[ETMT_INJ_CRNK];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.25f, 100.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_INJPW_UNIT).c_str(),
    MLL::GetString(IDS_CRNK_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationCrnkMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.01f"));
  DLL::Chart2DSetOnChange(md.handle, OnChangeCrnkMap, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseCrnkMap, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_CRNK);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewWrmpMap()
{
 MapData &md = m_md[ETMT_INJ_WRMP];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.0f,199.0f, GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_WRMP_UNIT).c_str(),
    MLL::GetString(IDS_WRMP_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationWrmpMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeWrmpMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseWrmpMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_WRMP);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewDeadMap()
{
 MapData &md = m_md[ETMT_INJ_DEAD];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -1.0f, 12.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DEAD_UNIT).c_str(),
    MLL::GetString(IDS_DEAD_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationDeadMap,this);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.01f"));
  DLL::Chart2DSetOnChange(md.handle, OnChangeDeadMap, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseDeadMap, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_DEAD);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewIdlrMap()
{
 MapData &md = m_md[ETMT_INJ_IDLR];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.0f,100.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_IAC_UNIT).c_str(),
    MLL::GetString(IDS_IDLR_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationIdlrMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeIdlrMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseIdlrMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_IDLR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewIdlcMap()
{
 MapData &md = m_md[ETMT_INJ_IDLC];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.0f,100.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_IAC_UNIT).c_str(),
    MLL::GetString(IDS_IDLC_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationIdlcMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeIdlcMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseIdlcMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_IDLC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewThrassMap()
{
 MapData &md = m_md[ETMT_INJ_THRASS];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0f, 100.0f, GetRPMGrid(), 16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_IAC_UNIT).c_str(),
    MLL::GetString(IDS_THRASS_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationThrassMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeThrassMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseThrassMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_THRASS);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewAETPSMap()
{
 MapData &md = m_md[ETMT_INJ_AETPS];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[5] = {-1000.0f, 1000.0f, 1.0f, 0.0f, 10.0f}; //min -1000%, max 1000%, inc 1%, 0 dec places, min diff 10%
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,-55.0f,199.0f,bins_lims,8,
    MLL::GetString(IDS_MAPS_DPDT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AETPS_UNIT).c_str(),
    MLL::GetString(IDS_AETPS_MAP).c_str(), CXD_BM_NM); //<-- use horizontal axis bins
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationAETPSMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeAETPSMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAETPSMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_AETPS);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewAEMAPMap()
{
 MapData &md = m_md[ETMT_INJ_AEMAP];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[5] = {-1000.0f, 1000.0f, 1.0f, 0.0f, 10.0f}; //min -1000kPa, max 1000kPa, inc 1kPa, 0 dec places, min diff 10kPa
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,-55.0f,199.0f,bins_lims,8,
    MLL::GetString(IDS_MAPS_DPRDT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AEMAP_UNIT).c_str(),
    MLL::GetString(IDS_AEMAP_MAP).c_str(), CXD_BM_NM); //<-- use horizontal axis bins
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationAEMAPMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeAEMAPMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAEMAPMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_AEMAP);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}


void CButtonsPanel::OnViewAERPMMap()
{
 MapData &md = m_md[ETMT_INJ_AERPM];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[5] = {0.0f, 25000.0f, 100.0f, 0.0f, 100.0f}; //min 0min-1, max 25000min-1, inc 100min-1, 0 dec places, min diff 100min-1
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.0f,199.0f,bins_lims,4,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AERPM_UNIT).c_str(),
    MLL::GetString(IDS_AERPM_MAP).c_str(), CXD_BM_NM); //<--use horizontal axis bins
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationAERPMMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeAERPMMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAERPMMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_AERPM);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewAftstrMap()
{
 MapData &md = m_md[ETMT_INJ_AFTSTR];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.0f,199.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFTSTR_UNIT).c_str(),
    MLL::GetString(IDS_AFTSTR_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationAftstrMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeAftstrMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAftstrMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_AFTSTR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewITMap()
{
 MapData &md = m_md[ETMT_INJ_IT];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  float y1, y2;
  _GetITModeRange(y1, y2);
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active,GetRPMGrid(),16,16,y1,y2,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString((m_it_mode == 1 || m_it_mode == 3) ? IDS_MAPS_INJTIM_UNIT1 : IDS_MAPS_INJTIM_UNIT).c_str(),
    MLL::GetString(IDS_IT_MAP).c_str());
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationITMap, this);
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangeITMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseITMap,this);
  DLL::Chart3DSetOnValueTransform(md.handle, OnValueTransformITMap, this);
  DLL::Chart3DUpdate(md.handle, NULL, NULL); //<--actuate changes
  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_IT);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewITRPMMap()
{
 MapData &md = m_md[ETMT_INJ_ITRPM];
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
    MLL::GetString(IDS_ITRPM_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelCLT, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationITRPMMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeITRPMMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseITRPMMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_ITRPM);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewRigidMap()
{
 MapData &md = m_md[ETMT_INJ_RIGID];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.0f,5.0f,SECU3IO::rigid_map_slots,8,
    MLL::GetString(IDS_MAPS_RIGID_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_RIGID_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.04f"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationRigidMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeRigidMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseRigidMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_RIGID);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewEGOCrvMap()
{
 MapData &md = m_md[ETMT_INJ_EGOCRV];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[7] = {0.0f, 5.5f, 0.0f, 5.5f, 0.01f, -1.0f, -1.0f}; //min 0, max 5.5, inc 0.01, -1 limit text, -1 dec places
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 6.00, 24.00, bins_lims, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFR_UNIT).c_str(),
    MLL::GetString(IDS_EGO_CURVE_MAP).c_str(), CXD_BM_BE);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationEGOCrvMap, this);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeEGOCrvMap, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseEGOCrvMap, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_EGOCRV);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewIACCMap()
{
 MapData &md = m_md[ETMT_INJ_IACC];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[7] = {0.0f, 100.0f, 0.0f, 100.0f, 0.5f, -1.0f, -1.0f}; //min 0, max 100.0, inc 0.5, -1 limit text, -1 dec places
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.00f, 1.99f, bins_lims, 8,
    MLL::GetString(IDS_MAPS_IACPOS_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_IACC_CURVE_MAP).c_str(), CXD_BM_BE);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationIACCMap, this);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeIACCMap, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseIACCMap, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_IACC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewIACCWMap()
{
 MapData &md = m_md[ETMT_INJ_IACCW];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[7] = {0.0f, 100.0f, 0.0f, 100.0f, 0.5f, -1.0f, -1.0f}; //min 0, max 100.0, inc 0.5, -1 limit text, -1 dec places
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.00, 1.00, bins_lims, 16,
    MLL::GetString(IDS_MAPS_TPSPOS_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_IACCW_CURVE_MAP).c_str(), CXD_BM_BE);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationIACCWMap, this);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisValuesFormat(md.handle, CXD_X_AXIS, _T("%.02f"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeIACCWMap, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseIACCWMap, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_IACCW);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewIATCLTMap()
{
 MapData &md = m_md[ETMT_INJ_IATCLT];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[7] = {.0f, 2000000.0f, .0f, 2000000.0f, 100.0f, 7.0f, .0f};
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.00, 1.00, bins_lims, 8,
    MLL::GetString(IDS_MAPS_AIRFLOW_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_IATCLT_CORR_MAP).c_str(), CXD_BM_BE);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationIATCLTMap, this);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeIATCLTMap, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseIATCLTMap, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_IATCLT);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewTpsswtMap()
{
 MapData &md = m_md[ETMT_INJ_TPSSWT];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active,0.0,100.0,GetRPMGrid(),16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TPSSWT_UNIT).c_str(),
    MLL::GetString(IDS_TPSSWT_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationTpsswtMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeTpsswtMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseTpsswtMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_TPSSWT);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewGtscMap()
{
 MapData &md = m_md[ETMT_INJ_GTSC];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.01f, 1.99f, SECU3IO::temp_map_tmp_slots, 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_GTSC_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationGtscMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeGtscMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseGtscMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_GTSC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewGpscMap()
{
 MapData &md = m_md[ETMT_INJ_GPSC];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  const float bins_lims[7] = {50.0f, 500.0f, 100.0f, 500.0f, 2.0f, 3.0f, .0f}; //min 50, max 500, min 100, max 500, inc 2, 3 limit text, 0 dec places
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.00f, 1.99f, bins_lims, 17,
    MLL::GetString(IDS_MAPS_PRESS_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_GPSC_MAP).c_str(), CXD_BM_BE);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationGpscMap, this);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeGpscMap, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseGpscMap, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_GPSC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewAtscMap()
{
 MapData &md = m_md[ETMT_INJ_ATSC];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.5f, 1.5f, SECU3IO::temp_map_tmp_slots, 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_ATSC_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationAtscMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeAtscMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAtscMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_ATSC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewPwm1Map()
{
 MapData &md = m_md[ETMT_PWM1];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active, GetRPMGrid(), 16, 16, (m_splitAng ? splitAngMin : 0.0f), (m_splitAng ? splitAngMax : 100.0f),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    m_splitAng ? MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str() : MLL::GetString(IDS_MAPS_DUTY_UNIT).c_str(), //unit
    m_splitAng ? MLL::GetString(IDS_SPLIT_ANGLE_MAP).c_str() : MLL::GetString(IDS_PWM1_MAP).c_str()); //title
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationPwm1Map, this);
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangePwm1Map,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnClosePwm1Map,this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_PWM1);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewPwm2Map()
{
 MapData &md = m_md[ETMT_PWM2];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), md.original, md.active, GetRPMGrid(), 16, 16, 0.0, 100.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DUTY_UNIT).c_str(),
    MLL::GetString(IDS_PWM2_MAP).c_str());
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationPwm2Map, this);
  DLL::Chart3DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart3DSetOnChange(md.handle,OnChangePwm2Map,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnClosePwm2Map,this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_PWM2);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewIACMATMap()
{
 MapData &md = m_md[ETMT_INJ_IACMAT];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -25.0f, 25.0f, SECU3IO::temp_map_tmp_slots, 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_CORR_UNIT).c_str(),
    MLL::GetString(IDS_IACMAT_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationIACMATMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeIACMATMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseIACMATMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_IACMAT);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewTpszonMap()
{
 MapData &md = m_md[ETMT_INJ_TPSZON];
 //If button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.0, 100.0, GetRPMGrid(), 16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TPSZON_UNIT).c_str(),
    MLL::GetString(IDS_TPSZON_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationTpszonMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, CXD_X_AXIS, OnGetXAxisLabelRPM, static_cast<CTablesPanelBase*>(this));
  DLL::Chart2DSetOnChange(md.handle,OnChangeTpszonMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseTpszonMap,this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_TPSZON);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewCylMultMap()
{
 MapData &md = m_md[ETMT_INJ_CYLMULT];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, 0.5f, 1.49f, SECU3IO::inj_cyladd_map_slots, 8,
    MLL::GetString(IDS_MAPS_CYLADD_UNIT).c_str(), //x
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),  //y
    MLL::GetString(IDS_INJ_CYLMULT_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationCylMultMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeCylMultMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseCylMultMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_CYLMULT);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CButtonsPanel::OnViewCylAddMap()
{
 MapData &md = m_md[ETMT_INJ_CYLADD];
 //if button was released, then close editor's window
 if (md.mp_button->GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), md.original, md.active, -3.2f, 3.2f, SECU3IO::inj_cyladd_map_slots, 8,
    MLL::GetString(IDS_MAPS_CYLADD_UNIT).c_str(),  //x
    MLL::GetString(IDS_MAPS_CYLADDY_UNIT).c_str(), //y
    MLL::GetString(IDS_INJ_CYLADD_MAP).c_str(), CXD_BM_NO);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationCylAddMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeCylAddMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseCylAddMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, ETMT_INJ_CYLADD);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnGridModeEditingIgn()
{ 
 if (m_md[ETMT_GME_IGN_WND].mp_button->GetCheck()==BST_CHECKED)
 {
  mp_gridModeEditorIgnDlg.reset(new CGridModeEditorIgnDlg());
  mp_gridModeEditorIgnDlg->BindMaps(m_md[ETMT_IGN_START].active, m_md[ETMT_IGN_IDLE].active, m_md[ETMT_IGN_WORK].active, m_md[ETMT_IGN_TEMP].active, m_md[ETMT_IGN_TEMPI].active);
  mp_gridModeEditorIgnDlg->BindMapsOrig(m_md[ETMT_IGN_START].original, m_md[ETMT_IGN_IDLE].original, m_md[ETMT_IGN_WORK].original, m_md[ETMT_IGN_TEMP].original, m_md[ETMT_IGN_TEMPI].original);
  mp_gridModeEditorIgnDlg->BindRPMGrid(GetRPMGrid());
  mp_gridModeEditorIgnDlg->BindCLTGrid(GetCLTGrid());
  mp_gridModeEditorIgnDlg->BindLoadGrid(GetLoadGrid());
  mp_gridModeEditorIgnDlg->setIsAllowed(fastdelegate::MakeDelegate(this, &CButtonsPanel::IsAllowed));
  mp_gridModeEditorIgnDlg->setOnMapChanged(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapChangedIgn));
  mp_gridModeEditorIgnDlg->setOnCloseMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapClosedIgn));
  mp_gridModeEditorIgnDlg->setOnOpenMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnOpenMapWnd));
  mp_gridModeEditorIgnDlg->EnableAdvanceAngleIndication(m_en_aa_indication);
  VERIFY(mp_gridModeEditorIgnDlg->Create(CGridModeEditorIgnDlg::IDD, NULL));
  mp_gridModeEditorIgnDlg->SetLoadAxisCfg(ldaxGetMin(), ldaxGetMax(), ldaxGetCfg(), ldaxIsTable(), false==m_onlineMode); //force update in offline mode
  mp_gridModeEditorIgnDlg->ShowWindow(SW_SHOW);
  m_md[ETMT_GME_IGN_WND].state = 1;
  m_md[ETMT_GME_IGN_WND].handle = mp_gridModeEditorIgnDlg->m_hWnd;
 }
 else
 {
  OnGridMapClosedIgn(mp_gridModeEditorIgnDlg->m_hWnd, ETMT_GME_IGN_WND);
  mp_gridModeEditorIgnDlg->DestroyWindow();
  mp_gridModeEditorIgnDlg.reset(NULL);
  m_md[ETMT_GME_IGN_WND].handle = NULL;
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnGridModeEditingInj()
{
 if (m_md[ETMT_GME_INJ_WND].mp_button->GetCheck()==BST_CHECKED)
 {
  mp_gridModeEditorInjDlg.reset(new CGridModeEditorInjDlg());
  if (mp_autoTuneCntr.get())
  {
   mp_autoTuneCntr->SetView(mp_gridModeEditorInjDlg.get());
   mp_autoTuneCntr->BindRPMGrid(GetRPMGrid());
   mp_autoTuneCntr->BindLoadGrid(GetLoadGrid(), &m_ve2_map_load_slots[0]);
   mp_autoTuneCntr->BindMaps(GetMap(ETMT_INJ_VE, false), GetMap(ETMT_INJ_AFR, false), GetMap(ETMT_INJ_VE2, false));
   mp_autoTuneCntr->SetLoadAxisCfg(ldaxGetMin(), ldaxGetMax(), ldaxGetCfg(), ldaxIsTable());
   mp_autoTuneCntr->SetVE2MapFunc(m_ve2_map_func);
   mp_autoTuneCntr->setOnMapChanged(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapChangedInj));
   mp_autoTuneCntr->Init();
  }
  mp_gridModeEditorInjDlg->BindMaps(GetMap(ETMT_INJ_VE, false), GetMap(ETMT_INJ_AFR, false), GetMap(ETMT_INJ_IT, false), GetMap(ETMT_INJ_IDLC, false), GetMap(ETMT_INJ_IDLR, false), GetMap(ETMT_INJ_ITRPM, false), GetMap(ETMT_INJ_RIGID, false),
                                    GetMap(ETMT_INJ_IACC, false), GetMap(ETMT_INJ_IACCW, false), GetMap(ETMT_INJ_AFTSTR, false), GetMap(ETMT_INJ_WRMP, false), GetMap(ETMT_INJ_AETPS, false), GetMap(ETMT_INJ_AERPM, false),
                                    GetMap(ETMT_INJ_CRNK, false), GetMap(ETMT_INJ_DEAD, false), GetMap(ETMT_INJ_EGOCRV, false), GetMap(ETMT_INJ_IATCLT, false), GetMap(ETMT_INJ_TPSSWT, false), GetMap(ETMT_INJ_ATSC, false),
                                    GetMap(ETMT_INJ_GTSC, false), GetMap(ETMT_INJ_GPSC, false), GetMap(ETMT_PWM1, false), GetMap(ETMT_PWM2, false), GetMap(ETMT_INJ_IACMAT, false), GetMap(ETMT_INJ_VE2, false), GetMap(ETMT_INJ_TPSZON, false),
                                    GetMap(ETMT_INJ_CYLMULT, false), GetMap(ETMT_INJ_CYLADD, false), GetMap(ETMT_INJ_AEMAP, false), GetMap(ETMT_INJ_THRASS, false), GetMap(ETMT_INJ_IVE, false));

  mp_gridModeEditorInjDlg->BindMapsOrig(GetMap(ETMT_INJ_VE, true), GetMap(ETMT_INJ_AFR, true), GetMap(ETMT_INJ_IT, true), GetMap(ETMT_INJ_IDLC, true), GetMap(ETMT_INJ_IDLR, true), GetMap(ETMT_INJ_ITRPM, true), GetMap(ETMT_INJ_RIGID, true),
                                    GetMap(ETMT_INJ_IACC, true), GetMap(ETMT_INJ_IACCW, true), GetMap(ETMT_INJ_AFTSTR, true), GetMap(ETMT_INJ_WRMP, true), GetMap(ETMT_INJ_AETPS, true), GetMap(ETMT_INJ_AERPM, true),
                                    GetMap(ETMT_INJ_CRNK, true), GetMap(ETMT_INJ_DEAD, true), GetMap(ETMT_INJ_EGOCRV, true), GetMap(ETMT_INJ_IATCLT, true), GetMap(ETMT_INJ_TPSSWT, true), GetMap(ETMT_INJ_ATSC, true),
                                    GetMap(ETMT_INJ_GTSC, true), GetMap(ETMT_INJ_GPSC, true), GetMap(ETMT_PWM1, true), GetMap(ETMT_PWM2, true), GetMap(ETMT_INJ_IACMAT, true), GetMap(ETMT_INJ_VE2, true), GetMap(ETMT_INJ_TPSZON, true),
                                    GetMap(ETMT_INJ_CYLMULT, true), GetMap(ETMT_INJ_CYLADD, true), GetMap(ETMT_INJ_AEMAP, true), GetMap(ETMT_INJ_THRASS, true), GetMap(ETMT_INJ_IVE, true));

  mp_gridModeEditorInjDlg->BindRPMGrid(GetRPMGrid(), GetIRPMGrid());
  mp_gridModeEditorInjDlg->BindCLTGrid(GetCLTGrid());
  mp_gridModeEditorInjDlg->BindLoadGrid(GetLoadGrid(), &m_ve2_map_load_slots[0], GetILoadGrid());
  mp_gridModeEditorInjDlg->setIsAllowed(fastdelegate::MakeDelegate(this, &CButtonsPanel::IsAllowed));
  mp_gridModeEditorInjDlg->setOnMapChanged(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapChangedInj));
  mp_gridModeEditorInjDlg->setOnCloseMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapClosedInj));
  mp_gridModeEditorInjDlg->setOnOpenMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnOpenMapWnd));
  mp_gridModeEditorInjDlg->setOnChangeSettings(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnChangeSettingsGME));
  mp_gridModeEditorInjDlg->SetActiveVEMap(m_active_ve); 
  VERIFY(mp_gridModeEditorInjDlg->Create(CGridModeEditorInjDlg::IDD, NULL));
  mp_gridModeEditorInjDlg->SetLoadAxisCfg(ldaxGetMin(), ldaxGetMax(), ldaxGetCfg(), ldaxIsTable(), false==m_onlineMode); //force update in offline mode
  mp_gridModeEditorInjDlg->SetITMode(m_it_mode);
  mp_gridModeEditorInjDlg->SetSplitAngMode(m_splitAng); //splitting
  mp_gridModeEditorInjDlg->ShowWindow(SW_SHOW);

  if (mp_autoTuneCntr.get())
   mp_autoTuneCntr->Activate();
  m_md[ETMT_GME_INJ_WND].state = 1;
  m_md[ETMT_GME_INJ_WND].handle = mp_gridModeEditorInjDlg->m_hWnd;
 }
 else
 {
  OnGridMapClosedInj(mp_gridModeEditorInjDlg->m_hWnd, ETMT_GME_INJ_WND);
  mp_gridModeEditorInjDlg->DestroyWindow();
  mp_gridModeEditorInjDlg.reset(NULL);
  m_md[ETMT_GME_INJ_WND].handle = NULL;
 }
}

void CButtonsPanel::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsAllowed());
}

void CButtonsPanel::OnUpdateViewStartMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_IGN_START].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdleMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_IGN_IDLE].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewWorkMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_IGN_WORK].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewTempMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_IGN_TEMP].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewTempIdlMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_IGN_TEMPI].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateGridModeEditingIgn(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 pCmdUI->Enable(allowed);
 pCmdUI->SetCheck((mp_gridModeEditorIgnDlg.get() && m_md[ETMT_GME_IGN_WND].state) ? TRUE : FALSE);
}

void CButtonsPanel::OnUpdateGridModeEditingInj(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 pCmdUI->Enable(allowed);
 pCmdUI->SetCheck((mp_gridModeEditorInjDlg.get() && m_md[ETMT_GME_INJ_WND].state) ? TRUE : FALSE);
}

void CButtonsPanel::OnUpdateViewVEMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_VE].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewVE2Map(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_VE2].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIVEMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_IVE].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAFRMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_AFR].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewCrnkMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_CRNK].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewWrmpMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_WRMP].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewDeadMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[ETMT_INJ_DEAD].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdlrMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_choke_op_enabled));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_IDLR].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdlcMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_choke_op_enabled));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_IDLC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewThrassMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[ETMT_INJ_THRASS].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAETPSMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_AETPS].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAEMAPMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_AEMAP].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAERPMMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_AERPM].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAftstrMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_AFTSTR].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewITMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[ETMT_INJ_IT].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewITRPMMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_choke_op_enabled));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_ITRPM].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewRigidMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[ETMT_INJ_RIGID].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewEGOCrvMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose || m_carb_afr));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_EGOCRV].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIACCMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[ETMT_INJ_IACC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIACCWMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[ETMT_INJ_IACCW].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIATCLTMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_IATCLT].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewTpsswtMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable /*&& m_fuel_injection*/); //required by ignition and fuel injection
 pCmdUI->SetCheck( (m_md[ETMT_INJ_TPSSWT].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewGtscMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose) && m_en_gas_corr);
 pCmdUI->SetCheck( (m_md[ETMT_INJ_GTSC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewGpscMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose) && m_en_gas_corr);
 pCmdUI->SetCheck( (m_md[ETMT_INJ_GPSC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAtscMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose || m_choke_op_enabled));
 pCmdUI->SetCheck( (m_md[ETMT_INJ_ATSC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewPwm1Map(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_PWM1].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewPwm2Map(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[ETMT_PWM2].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIACMATMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection));
 pCmdUI->SetCheck((m_md[ETMT_INJ_IACMAT].state) ? TRUE : FALSE);
}

void CButtonsPanel::OnUpdateViewTpszonMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable/* && m_fuel_injection*/); //required by ignition and fuel injection
 pCmdUI->SetCheck((m_md[ETMT_INJ_TPSZON].state) ? TRUE : FALSE);
}

void CButtonsPanel::OnUpdateViewCylMultMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection); //required by fuel injection only
 pCmdUI->SetCheck((m_md[ETMT_INJ_CYLMULT].state) ? TRUE : FALSE);
}

void CButtonsPanel::OnUpdateViewCylAddMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection); //required by fuel injection only
 pCmdUI->SetCheck((m_md[ETMT_INJ_CYLADD].state) ? TRUE : FALSE);
}

void CButtonsPanel::OnTimer(UINT nIDEvent)
{
 //I know it is dirty hack, but... :-)
 UpdateDialogControls(this,TRUE);
 _EnableCharts(IsAllowed());
 Super::OnTimer(nIDEvent);
}

void CButtonsPanel::OnDestroy()
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);
 mp_scr->Close();
 m_initialized = false;
 m_btnMovIds.clear();
}

void CButtonsPanel::UpdateOpenedCharts(void)
{
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
 {
  if (!m_md[i].state)
   continue;

  if (Is3DMap(i))
   DLL::Chart3DUpdate(m_md[i].handle, GetMap(i, true), GetMap(i, false));
  else
   DLL::Chart2DUpdate(m_md[i].handle, GetMap(i, true), GetMap(i, false));
 }

 if (mp_gridModeEditorIgnDlg.get() && m_md[ETMT_GME_IGN_WND].state)
  mp_gridModeEditorIgnDlg->UpdateView(true);
 if (mp_gridModeEditorInjDlg.get() && m_md[ETMT_GME_INJ_WND].state)
  mp_gridModeEditorInjDlg->UpdateView(true);
}

void CButtonsPanel::EnableAdvanceAngleIndication(bool i_enable)
{
 m_en_aa_indication = i_enable;
 if (mp_gridModeEditorIgnDlg.get())
  mp_gridModeEditorIgnDlg->EnableAdvanceAngleIndication(m_en_aa_indication);
}

void CButtonsPanel::SetDynamicValues(const SECU3IO::SensorDat& dv)
{
 if (mp_gridModeEditorIgnDlg.get())
  mp_gridModeEditorIgnDlg->SetDynamicValues(dv);
 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->SetDynamicValues(dv);
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetDynamicValues(dv);
}

void CButtonsPanel::EnableFuelInjection(bool i_enable)
{
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_INJ_VE].state && ::IsWindow(m_md[ETMT_INJ_VE].handle))
  DLL::Chart3DEnable(m_md[ETMT_INJ_VE].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_VE2].state && ::IsWindow(m_md[ETMT_INJ_VE2].handle))
  DLL::Chart3DEnable(m_md[ETMT_INJ_VE2].handle, (i_enable) && IsAllowed());
 if (m_md[ETMT_INJ_IVE].state && ::IsWindow(m_md[ETMT_INJ_IVE].handle))
  DLL::Chart3DEnable(m_md[ETMT_INJ_IVE].handle, (i_enable) && IsAllowed());
 if (m_md[ETMT_INJ_AFR].state && ::IsWindow(m_md[ETMT_INJ_AFR].handle))
  DLL::Chart3DEnable(m_md[ETMT_INJ_AFR].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_CRNK].state && ::IsWindow(m_md[ETMT_INJ_CRNK].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_CRNK].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_WRMP].state && ::IsWindow(m_md[ETMT_INJ_WRMP].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_WRMP].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_DEAD].state && ::IsWindow(m_md[ETMT_INJ_DEAD].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_DEAD].handle, i_enable && IsAllowed());
 if (m_md[ETMT_INJ_IDLR].state && ::IsWindow(m_md[ETMT_INJ_IDLR].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IDLR].handle, (i_enable || m_choke_op_enabled) && IsAllowed());
 if (m_md[ETMT_INJ_IDLC].state && ::IsWindow(m_md[ETMT_INJ_IDLC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IDLC].handle, (i_enable || m_choke_op_enabled) && IsAllowed());
 if (m_md[ETMT_INJ_THRASS].state && ::IsWindow(m_md[ETMT_INJ_THRASS].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_THRASS].handle, i_enable && IsAllowed());
 if (m_md[ETMT_INJ_AETPS].state && ::IsWindow(m_md[ETMT_INJ_AETPS].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_AETPS].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_AEMAP].state && ::IsWindow(m_md[ETMT_INJ_AEMAP].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_AEMAP].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_AERPM].state && ::IsWindow(m_md[ETMT_INJ_AERPM].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_AERPM].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_AFTSTR].state && ::IsWindow(m_md[ETMT_INJ_AFTSTR].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_AFTSTR].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_IT].state && ::IsWindow(m_md[ETMT_INJ_IT].handle))
  DLL::Chart3DEnable(m_md[ETMT_INJ_IT].handle, i_enable && IsAllowed());
 if (m_md[ETMT_INJ_ITRPM].state && ::IsWindow(m_md[ETMT_INJ_ITRPM].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_ITRPM].handle, (i_enable || m_choke_op_enabled) && IsAllowed());
 if (m_md[ETMT_INJ_RIGID].state && ::IsWindow(m_md[ETMT_INJ_RIGID].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_RIGID].handle, i_enable && IsAllowed());
 if (m_md[ETMT_INJ_EGOCRV].state && ::IsWindow(m_md[ETMT_INJ_EGOCRV].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_EGOCRV].handle, (i_enable || m_gasdose || m_carb_afr) && IsAllowed());
 if (m_md[ETMT_INJ_IACC].state && ::IsWindow(m_md[ETMT_INJ_IACC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IACC].handle, i_enable && IsAllowed());
 if (m_md[ETMT_INJ_IACCW].state && ::IsWindow(m_md[ETMT_INJ_IACCW].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IACCW].handle, i_enable && IsAllowed());
 if (m_md[ETMT_INJ_IATCLT].state && ::IsWindow(m_md[ETMT_INJ_IATCLT].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IATCLT].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[ETMT_INJ_TPSSWT].state && ::IsWindow(m_md[ETMT_INJ_TPSSWT].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_TPSSWT].handle, i_enable && IsAllowed());
 if (m_md[ETMT_INJ_GTSC].state && ::IsWindow(m_md[ETMT_INJ_GTSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_GTSC].handle, ((i_enable || m_gasdose) && m_en_gas_corr) && IsAllowed());
 if (m_md[ETMT_INJ_GPSC].state && ::IsWindow(m_md[ETMT_INJ_GPSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_GPSC].handle, ((i_enable || m_gasdose) && m_en_gas_corr) && IsAllowed());
 if (m_md[ETMT_INJ_ATSC].state && ::IsWindow(m_md[ETMT_INJ_ATSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_ATSC].handle, (i_enable || m_gasdose || m_choke_op_enabled) && IsAllowed());
 if (m_md[ETMT_INJ_IACMAT].state && ::IsWindow(m_md[ETMT_INJ_IACMAT].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IACMAT].handle, (i_enable) && IsAllowed());
 if (m_md[ETMT_INJ_TPSZON].state && ::IsWindow(m_md[ETMT_INJ_TPSZON].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_TPSZON].handle, (i_enable) && IsAllowed());
 if (m_md[ETMT_INJ_CYLMULT].state && ::IsWindow(m_md[ETMT_INJ_CYLMULT].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_CYLMULT].handle, (i_enable) && IsAllowed());
 if (m_md[ETMT_INJ_CYLADD].state && ::IsWindow(m_md[ETMT_INJ_CYLADD].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_CYLADD].handle, (i_enable) && IsAllowed());
}

void CButtonsPanel::EnableGasdose(bool i_enable)
{
 m_gasdose = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_INJ_AETPS].state && ::IsWindow(m_md[ETMT_INJ_AETPS].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_AETPS].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_AEMAP].state && ::IsWindow(m_md[ETMT_INJ_AEMAP].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_AEMAP].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_AERPM].state && ::IsWindow(m_md[ETMT_INJ_AERPM].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_AERPM].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_VE].state && ::IsWindow(m_md[ETMT_INJ_VE].handle))
  DLL::Chart3DEnable(m_md[ETMT_INJ_VE].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_AFR].state && ::IsWindow(m_md[ETMT_INJ_AFR].handle))
  DLL::Chart3DEnable(m_md[ETMT_INJ_AFR].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_WRMP].state && ::IsWindow(m_md[ETMT_INJ_WRMP].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_WRMP].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_CRNK].state && ::IsWindow(m_md[ETMT_INJ_CRNK].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_CRNK].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_AFTSTR].state && ::IsWindow(m_md[ETMT_INJ_AFTSTR].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_AFTSTR].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_ATSC].state && ::IsWindow(m_md[ETMT_INJ_ATSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_ATSC].handle, (i_enable || m_fuel_injection || m_choke_op_enabled) && IsAllowed());
 if (m_md[ETMT_INJ_IATCLT].state && ::IsWindow(m_md[ETMT_INJ_IATCLT].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IATCLT].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_EGOCRV].state && ::IsWindow(m_md[ETMT_INJ_EGOCRV].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_EGOCRV].handle, (i_enable || m_fuel_injection || m_carb_afr) && IsAllowed());
 if (m_md[ETMT_INJ_GTSC].state && ::IsWindow(m_md[ETMT_INJ_GTSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_GTSC].handle, (m_en_gas_corr && (i_enable || m_fuel_injection)) && IsAllowed());
 if (m_md[ETMT_INJ_GPSC].state && ::IsWindow(m_md[ETMT_INJ_GPSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_GPSC].handle, (m_en_gas_corr && (i_enable || m_fuel_injection)) && IsAllowed());
}

void CButtonsPanel::EnableCarbAfr(bool i_enable)
{
 m_carb_afr = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);

 if (m_md[ETMT_INJ_EGOCRV].state && ::IsWindow(m_md[ETMT_INJ_EGOCRV].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_EGOCRV].handle, (i_enable || m_fuel_injection || m_gasdose) && IsAllowed());
}

void CButtonsPanel::EnableGasCorr(bool i_enable)
{
 m_en_gas_corr = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);

 if (m_md[ETMT_INJ_GTSC].state && ::IsWindow(m_md[ETMT_INJ_GTSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_GTSC].handle, (i_enable && (m_gasdose || m_fuel_injection)) && IsAllowed());
 if (m_md[ETMT_INJ_GPSC].state && ::IsWindow(m_md[ETMT_INJ_GPSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_GPSC].handle, (i_enable && (m_gasdose || m_fuel_injection)) && IsAllowed());
}

void CButtonsPanel::EnableChokeOp(bool enable)
{
 m_choke_op_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[ETMT_INJ_ATSC].state && ::IsWindow(m_md[ETMT_INJ_ATSC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_ATSC].handle, (enable || m_gasdose || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_IDLR].state && ::IsWindow(m_md[ETMT_INJ_IDLR].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IDLR].handle, (enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_IDLC].state && ::IsWindow(m_md[ETMT_INJ_IDLC].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_IDLC].handle, (enable || m_fuel_injection) && IsAllowed());
 if (m_md[ETMT_INJ_ITRPM].state && ::IsWindow(m_md[ETMT_INJ_ITRPM].handle))
  DLL::Chart2DEnable(m_md[ETMT_INJ_ITRPM].handle, (enable || m_fuel_injection) && IsAllowed());
}

float* CButtonsPanel::GetMap(int mapId, bool i_original)
{
 return i_original ? m_md[mapId].original : m_md[mapId].active;
}

HWND CButtonsPanel::GetMapWindow(int wndType)
{
 if (wndType >= ETMT_SET_START && wndType <= ETMT_SET_END)
  return m_md[wndType].state ? m_md[wndType].handle : NULL;
 else if (wndType == ETMT_GME_IGN_WND) //pseudo map
  return (mp_gridModeEditorIgnDlg.get() && m_md[ETMT_GME_IGN_WND].state) ? mp_gridModeEditorIgnDlg->m_hWnd : NULL; 
 else if (wndType == ETMT_GME_INJ_WND) //pseudo map
  return (mp_gridModeEditorInjDlg.get() && m_md[ETMT_GME_INJ_WND].state) ? mp_gridModeEditorInjDlg->m_hWnd : NULL; 
 else
  return NULL;
}

void CButtonsPanel::_EnableCharts(bool enable)
{
 if (m_charts_enabled != (int)enable)
 {//ignition
  for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  {
   if (m_md[i].state && ::IsWindow(m_md[i].handle))
   {
    if (Is3DMap(i))
     DLL::Chart3DEnable(m_md[i].handle, enable && IsAllowed());
    else
     DLL::Chart2DEnable(m_md[i].handle, enable && IsAllowed());
   }
  }
  //and don't forget two pseudo maps:
  if (mp_gridModeEditorIgnDlg.get() && m_md[ETMT_GME_IGN_WND].state && ::IsWindow(mp_gridModeEditorIgnDlg->m_hWnd))
   mp_gridModeEditorIgnDlg->UpdateDialogControls(mp_gridModeEditorIgnDlg.get(), TRUE);
  if (mp_gridModeEditorInjDlg.get() && m_md[ETMT_GME_INJ_WND].state && ::IsWindow(mp_gridModeEditorInjDlg->m_hWnd))
   mp_gridModeEditorInjDlg->UpdateDialogControls();
 }

 m_charts_enabled = enable;
}

void CButtonsPanel::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after, x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

void CButtonsPanel::SetPosition(const CRect& rc, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after, rc.left, rc.top, rc.Width(), rc.Height(), (wnd_insert_after ? 0 : SWP_NOZORDER));
}

void CButtonsPanel::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 if (m_initialized)
 {
  AlignButtons(this, cx, ETMT_IGN_START, ETMT_IGN_IDLE, ETMT_INJ_IDLR, ETMT_SET_START, ETMT_SET_END);
 }

 if (mp_scr.get())
 {
  DPIAware da;
  if (m_disable_vscroll)
   mp_scr->SetViewSize(cx, da.ScaleY(1));
  else
   mp_scr->SetViewSize(cx, da.ScaleY(m_btnMovIds.empty() ? 1200 : 650));
 }
}

void CButtonsPanel::SetLoadAxisCfg(float minVal, float maxVal, int ldaxCfg, bool useTable, bool forceUpdate /*= false*/)
{
 LdaxCfg::SetLoadAxisCfg(minVal, maxVal, ldaxCfg, useTable, forceUpdate); //save values

 if (mp_gridModeEditorIgnDlg.get())
  mp_gridModeEditorIgnDlg->SetLoadAxisCfg(minVal, maxVal, ldaxCfg, useTable, forceUpdate);

 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->SetLoadAxisCfg(minVal, maxVal, ldaxCfg, useTable, forceUpdate);

 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetLoadAxisCfg(minVal, maxVal, ldaxCfg, useTable);
}

void CButtonsPanel::SetVE2MapFunc(int func)
{
 m_ve2_map_func = func;
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetVE2MapFunc(m_ve2_map_func);
}

float* CButtonsPanel::GetLamDelMap(int id)
{
 ASSERT(mp_autoTuneCntr.get());
 return mp_autoTuneCntr->GetLamDelMap(id);
}

void CButtonsPanel::SetAFRError(float afrError)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetAFRError(afrError);
}

bool* CButtonsPanel::GetBlockedCells(void)
{
 ASSERT(mp_autoTuneCntr.get());
 return mp_autoTuneCntr->GetBlockedCells();
}

void CButtonsPanel::SetStatSize(int statSize)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetStatSize(statSize);
}

void CButtonsPanel::SetAutoBlockThrd(int thrd)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetAutoBlockThrd(thrd);
}

void CButtonsPanel::SetGrowingMode(bool growing)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetGrowingMode(growing);
}

void CButtonsPanel::SetMinAFR(float afr)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMinAFR(afr);
}

void CButtonsPanel::SetMaxAFR(float afr)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMaxAFR(afr);
}

void CButtonsPanel::SetMinDistThrd(int thrd)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMinDistThrd(thrd);
}

void CButtonsPanel::SetMinTPS(float tps)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMinTPS(tps);
}

void CButtonsPanel::SetMaxTPS(float tps)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMaxTPS(tps);
}

void CButtonsPanel::SetCLTThrd(float clt)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetCLTThrd(clt);
}

void CButtonsPanel::SetTunSoftness(float soft)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetTunSoftness(soft);
}

void CButtonsPanel::SetMapEditorSettings(int gradSat, int gradBrt, bool bold, bool spotMarkers, float spotMarkersSize)
{
 CMapEditorCtrl::SetSettings(gradSat, gradBrt, bold, spotMarkers, spotMarkersSize);
}

void CButtonsPanel::MakeChartsChildren(bool children)
{
 CTablesPanelBase::MakeChartsChildren(children);
 HWND hwnd;

 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
 {
  hwnd = GetMapWindow(i);
  GDIHelpers::MakeWindowChild(hwnd, children);
 }

 hwnd = GetMapWindow(ETMT_GME_IGN_WND); //pseudo map
 GDIHelpers::MakeWindowChild(hwnd, children);

 hwnd = GetMapWindow(ETMT_GME_INJ_WND); //pseudo map
 GDIHelpers::MakeWindowChild(hwnd, children);
}

void CButtonsPanel::CloseCharts(void)
{
 HWND hwnd;
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
 {
  hwnd = GetMapWindow(i);
  if (hwnd)
   ::SendMessage(hwnd, WM_CLOSE, 0, 0);
 }
 hwnd = GetMapWindow(ETMT_GME_IGN_WND);
 if (hwnd) ::SendMessage(hwnd, WM_CLOSE, 0, 0);
 hwnd = GetMapWindow(ETMT_GME_INJ_WND);
 if (hwnd) ::SendMessage(hwnd, WM_CLOSE, 0, 0);
}

void CButtonsPanel::ShowOpenedCharts(bool i_show)
{
 HWND hwnd;
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
 {
  hwnd = GetMapWindow(i);
  if (hwnd)
   ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 }

 hwnd = GetMapWindow(ETMT_GME_IGN_WND);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);

 hwnd = GetMapWindow(ETMT_GME_INJ_WND);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
}

void CButtonsPanel::SetITEdMode(int mode)
{
 m_it_mode = mode;
 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->SetITMode(mode); 

 float y1, y2;
 _GetITModeRange(y1, y2);
 if ((m_md[ETMT_INJ_IT].state)&&(DLL::Chart3DCreate))
 {
  DLL::Chart3DSetFncRange(m_md[ETMT_INJ_IT].handle, y1, y2);
  DLL::Chart3DUpdate(m_md[ETMT_INJ_IT].handle, GetMap(ETMT_INJ_IT, true), GetMap(ETMT_INJ_IT, false));
  DLL::Chart3DSetAxisTitle(m_md[ETMT_INJ_IT].handle, CXD_Y_AXIS, MLL::GetString((m_it_mode == 1 || m_it_mode == 3) ? IDS_MAPS_INJTIM_UNIT1 : IDS_MAPS_INJTIM_UNIT).c_str());
 }
}

int CButtonsPanel::GetITEdMode(void) const
{
 return m_it_mode;
}

void CButtonsPanel::SetActiveVEMap(int id)
{
 m_active_ve = id;
 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->SetActiveVEMap(id); 
}

int CButtonsPanel::GetActiveVEMap(void) const
{
 return m_active_ve;
}

void CButtonsPanel::OnChangeSettingsGME(void)
{
 m_it_mode = mp_gridModeEditorInjDlg->GetITMode();

 float y1, y2;
 _GetITModeRange(y1, y2);
 if ((m_md[ETMT_INJ_IT].state)&&(DLL::Chart3DCreate))
 {
  DLL::Chart3DSetFncRange(m_md[ETMT_INJ_IT].handle, y1, y2);
  DLL::Chart3DUpdate(m_md[ETMT_INJ_IT].handle, GetMap(ETMT_INJ_IT, true), GetMap(ETMT_INJ_IT, false));
  DLL::Chart3DSetAxisTitle(m_md[ETMT_INJ_IT].handle, CXD_Y_AXIS, MLL::GetString((m_it_mode == 1 || m_it_mode == 3) ? IDS_MAPS_INJTIM_UNIT1 : IDS_MAPS_INJTIM_UNIT).c_str());
 }

 m_active_ve = mp_gridModeEditorInjDlg->GetActiveVEMap();

 if (m_OnChangeSettings)
  m_OnChangeSettings();
}

void CButtonsPanel::_GetITModeRange(float& y1, float& y2)
{
 y1 = (m_it_mode < 2) ? .0f : -360.0f;
 y2 = (m_it_mode < 2) ? 720.0f : 360.0f;
}

void CButtonsPanel::TransformValues(void)
{
 float* p_map = m_md[ETMT_INJ_IT].active;
 for (int i = 0; i < 16*16; ++i)
  p_map[i] = OnValueTransformITMap(this, OnValueTransformITMap(this, p_map[i], 0), 1);
}

void CButtonsPanel::SetSplitAngMode(bool mode)
{
 m_splitAng = mode; 

 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->SetSplitAngMode(m_splitAng);

 if (m_splitAng)
 {
  DLL::Chart3DSetFncRange(m_md[ETMT_PWM1].handle, splitAngMin, splitAngMax); //split angle
  DLL::Chart3DSetAxisTitle(m_md[ETMT_PWM1].handle, CXD_Y_AXIS, MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str());
  DLL::Chart3DSetTitle(m_md[ETMT_PWM1].handle, MLL::GetString(IDS_SPLIT_ANGLE_MAP).c_str());
  m_md[ETMT_PWM1].mp_button->SetWindowText(MLL::GetString(IDS_TD_VIEW_SPLIT_MAP).c_str());
 }
 else
 {
  DLL::Chart3DSetFncRange(m_md[ETMT_PWM1].handle, 0.0f, 100.0f);  //PWM duty
  DLL::Chart3DSetAxisTitle(m_md[ETMT_PWM1].handle, CXD_Y_AXIS, MLL::GetString(IDS_MAPS_DUTY_UNIT).c_str());
  DLL::Chart3DSetTitle(m_md[ETMT_PWM1].handle, MLL::GetString(IDS_PWM1_MAP).c_str());
  m_md[ETMT_PWM1].mp_button->SetWindowText(MLL::GetString(IDS_TD_VIEW_PWM1_MAP).c_str());
 }
}

void CButtonsPanel::OnShow(bool show)
{
 if (show)
  SetTimer(TIMER_ID, 250, NULL);
 else
  KillTimer(TIMER_ID);
}

void CButtonsPanel::EnableEmbedMapWnd(bool embed, const CRect& rc)
{
 CTablesPanelBase::EnableEmbedMapWnd(embed, rc, ETMT_SET_START, ETMT_SET_END);
 CTablesPanelBase::EnableEmbedMapWnd(embed, rc, ETMT_GME_IGN_WND, ETMT_GME_INJ_WND);
}

void CButtonsPanel::SetMapSetIndex(int idx)
{
 CString cs;
 if (idx > 0)
  cs.Format(_T("%d"), idx);
 else
  cs = _T("");
 m_setidx.SetWindowText(cs);
}
