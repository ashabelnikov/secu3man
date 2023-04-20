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

/** \file GridModeEditorInjDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GridModeEditorInjDlg.h"
#include "common/Dll.h"
#include "common/DPIAware.h"
#include "MapIds.h"
#include "GMEInjVEDlg.h"
#include "GMEInjAFRDlg.h"
#include "GMEInjITDlg.h"
#include "GMEInjIRegDlg.h"
#include "GMEInjEnrDlg.h"
#include "GMEInjOtherDlg.h"
#include "GMEInjOther1Dlg.h"
#include "GMEInjPwm1Dlg.h"
#include "GMEInjPwm2Dlg.h"
#include "io-core/SECU3IO.h"
#include "common/MathHelpers.h"
#include <limits>

#undef max

const UINT CGridModeEditorInjDlg::IDD = IDD_GRID_MODE_EDITOR_INJ;

/////////////////////////////////////////////////////////////////////////////
// CGridModeEditorInjDlg dialog

BEGIN_MESSAGE_MAP(CGridModeEditorInjDlg, Super)
 ON_WM_GETMINMAXINFO()
 ON_WM_CLOSE()
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_TAB_CTRL, OnUpdateControls)
 ON_WM_SIZE()
END_MESSAGE_MAP()

CGridModeEditorInjDlg::CGridModeEditorInjDlg(CWnd* pParent /*=NULL*/)
: Super(CGridModeEditorInjDlg::IDD, pParent)
, m_ldaxMin(1.0f)
, m_ldaxMax(16.0f)
, m_ldaxNeedsUpdate(false)
, m_ldaxUseTable(false)
, m_baro_press(101.3f) //sea level atmospheric pressure by default
, m_pwm1TabIdx(0)
, mp_lodGrid(NULL)
, m_initialized(false)
{
 m_work_map_load_slots.reserve(32);
 m_work_map_load_slots = MathHelpers::BuildGridFromRange(1.0f, 16.0f, 16);
 m_ve2_map_load_slots.reserve(32);
 m_ve2_map_load_slots = MathHelpers::BuildGridFromRange(0.0f, 100.0f, 16);

 m_pVEPageDlg.reset(new CGMEInjVEDlg());
 m_pVEPageDlg->BindLoadGrid(&m_work_map_load_slots[0]);
 m_pVEPageDlg->BindLoadGrid2(&m_ve2_map_load_slots[0]);
 m_pVEPageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangeVE));
 m_pVEPageDlg->setOnChange2(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangeVE2));

 m_pAFRPageDlg.reset(new CGMEInjAFRDlg());
 m_pAFRPageDlg->BindLoadGrid(&m_work_map_load_slots[0]);
 m_pAFRPageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangeAFR));

 m_pITPageDlg.reset(new CGMEInjITDlg());
 m_pITPageDlg->BindLoadGrid(&m_work_map_load_slots[0]);
 m_pITPageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangeIT));

 m_pIRegPageDlg.reset(new CGMEInjIRegDlg());
 m_pIRegPageDlg->BindTemperGrid(const_cast<float*>(SECU3IO::temp_map_tmp_slots));
 m_pIRegPageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangeIReg));

 m_pEnrPageDlg.reset(new CGMEInjEnrDlg());
 m_pEnrPageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangeEnr));

 m_pOtherPageDlg.reset(new CGMEInjOtherDlg());
 m_pOtherPageDlg->BindTemperGrid(const_cast<float*>(SECU3IO::temp_map_tmp_slots));
 m_pOtherPageDlg->BindDeadGrid(const_cast<float*>(SECU3IO::dwellcntrl_map_slots));
 m_pOtherPageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangeOther));

 m_pOther1PageDlg.reset(new CGMEInjOther1Dlg());
 m_pOther1PageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangeOther1));

 m_pPwm1PageDlg.reset(new CGMEInjPwm1Dlg());
 m_pPwm1PageDlg->BindLoadGrid(&m_work_map_load_slots[0]);
 m_pPwm1PageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangePwm1));

 m_pPwm2PageDlg.reset(new CGMEInjPwm2Dlg());
 m_pPwm2PageDlg->BindLoadGrid(&m_work_map_load_slots[0]);
 m_pPwm2PageDlg->setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorInjDlg::OnChangePwm2));
}

CGridModeEditorInjDlg::~CGridModeEditorInjDlg()
{
 //empty
}

void CGridModeEditorInjDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_TAB_CTRL, m_tab_control);
}

/////////////////////////////////////////////////////////////////////////////
// CGridModeEditorInjDlg message handlers

BOOL CGridModeEditorInjDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_tab_control.SetResourceModule(DLL::GetModuleHandle());
 m_tab_control.Init();

 m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_VE_TAB), m_pVEPageDlg.get(), 0);
 m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_AFR_TAB), m_pAFRPageDlg.get(), 0);
 m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_IT_TAB), m_pITPageDlg.get(), 0);
 m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_IREG_TAB), m_pIRegPageDlg.get(), 0);
 m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_ENR_TAB), m_pEnrPageDlg.get(), 0);
 m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_OTHER_TAB), m_pOtherPageDlg.get(), 0);
 m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_OTHER1_TAB), m_pOther1PageDlg.get(), 0);
 m_pwm1TabIdx = m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_PWM1_TAB), m_pPwm1PageDlg.get(), 0);
 m_tab_control.AddPage(MLL::LoadString(IDS_GME_INJ_PWM2_TAB), m_pPwm2PageDlg.get(), 0);

 m_tab_control.SetCurSel(0);

 SetIcon((HICON)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDI_GRAPH), IMAGE_ICON, 0, 0, LR_SHARED), TRUE);

 m_initialized = true;

 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(this->m_hWnd, TYPE_MAP_GME_INJ_WND);

 Super::UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGridModeEditorInjDlg::UpdateDialogControls(void)
{
 Super::UpdateDialogControls(this, true);

 bool allowed = m_IsAllowed ? m_IsAllowed() : false;
 m_tab_control.EnableItem(-1, allowed);

 if (m_pVEPageDlg->GetSafeHwnd())
 {
  m_pVEPageDlg->EnableWindow(allowed);
  m_pVEPageDlg->UpdateDialogControls(this, true);
 }

 if (m_pAFRPageDlg->GetSafeHwnd())
 {
  m_pAFRPageDlg->EnableWindow(allowed);
  m_pAFRPageDlg->UpdateDialogControls(this, true);
 }

 if (m_pITPageDlg->GetSafeHwnd())
 {
  m_pITPageDlg->EnableWindow(allowed);
  m_pITPageDlg->UpdateDialogControls(this, true);
 }

 if (m_pIRegPageDlg->GetSafeHwnd())
 {
  m_pIRegPageDlg->EnableWindow(allowed);
  m_pIRegPageDlg->UpdateDialogControls(this, true);
 }

 if (m_pEnrPageDlg->GetSafeHwnd())
 {
  m_pEnrPageDlg->EnableWindow(allowed);
  m_pEnrPageDlg->UpdateDialogControls(this, true);
 }

 if (m_pOtherPageDlg->GetSafeHwnd())
 {
  m_pOtherPageDlg->EnableWindow(allowed);
  m_pOtherPageDlg->UpdateDialogControls(this, true);
 }

 if (m_pOther1PageDlg->GetSafeHwnd())
 {
  m_pOther1PageDlg->EnableWindow(allowed);
  m_pOther1PageDlg->UpdateDialogControls(this, true);
 }

 if (m_pPwm1PageDlg->GetSafeHwnd())
 {
  m_pPwm1PageDlg->EnableWindow(allowed);
  m_pPwm1PageDlg->UpdateDialogControls(this, true);
 }

 if (m_pPwm2PageDlg->GetSafeHwnd())
 {
  m_pPwm2PageDlg->EnableWindow(allowed);
  m_pPwm2PageDlg->UpdateDialogControls(this, true);
 }
}

void CGridModeEditorInjDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool allowed = m_IsAllowed ? m_IsAllowed() : false;
 pCmdUI->Enable(allowed);
}

void CGridModeEditorInjDlg::OnClose()
{
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(this->m_hWnd, TYPE_MAP_GME_INJ_WND);

 Super::OnClose(); //close window
 DestroyWindow();
}

void CGridModeEditorInjDlg::BindMaps(float* pVE, float* pAFR, float* pIT, float* pIdlc, float* pIdlr, float* pITRPM, float* pRigid, float* pIACC, float* pIACCW, float* pAftstr, float* pWrmp, float* pAETPS, float* pAERPM, float* pCrnk, float* pDead, float* pEGOCrv, float* pIATCLT, float* pTpsswt, float* pAtsc, float* pGtsc, float* pGpsc, float* pPwm1, float* pPwm2, float* pIACMAT, float* pVE2, float* pTpszon, float* pCylMult, float* pCylAdd, float* pAEMAP, float* pThrass)
{
 m_pVEPageDlg->BindMaps(pVE, pVE2);
 m_pAFRPageDlg->BindMaps(pAFR);
 m_pITPageDlg->BindMaps(pIT);
 m_pIRegPageDlg->BindMaps(pIdlc, pIdlr, pITRPM, pRigid, pIACC, pIACCW, pIACMAT);
 m_pEnrPageDlg->BindMaps(pAftstr, pWrmp, pAETPS, pAERPM, pAEMAP);
 m_pOtherPageDlg->BindMaps(pCrnk, pDead, pEGOCrv, pIATCLT, pAtsc, pGtsc, pGpsc);
 m_pOther1PageDlg->BindMaps(pTpsswt, pTpszon, pCylMult, pCylAdd, pThrass);
 m_pPwm1PageDlg->BindMaps(pPwm1);
 m_pPwm2PageDlg->BindMaps(pPwm2);
}

void CGridModeEditorInjDlg::BindRPMGrid(float* pGrid)
{
 m_pVEPageDlg->BindRPMGrid(pGrid);
 m_pAFRPageDlg->BindRPMGrid(pGrid);
 m_pITPageDlg->BindRPMGrid(pGrid);
 m_pOtherPageDlg->BindRPMGrid(pGrid);
 m_pOther1PageDlg->BindRPMGrid(pGrid);
 m_pPwm1PageDlg->BindRPMGrid(pGrid);
 m_pPwm2PageDlg->BindRPMGrid(pGrid);
}

void CGridModeEditorInjDlg::BindCLTGrid(float* pGrid)
{
 m_pIRegPageDlg->BindCLTGrid(pGrid);
 m_pEnrPageDlg->BindCLTGrid(pGrid);
 m_pOtherPageDlg->BindCLTGrid(pGrid);
}

void CGridModeEditorInjDlg::BindLoadGrid(float* pGrid, float* pGrid2)
{
 mp_lodGrid = pGrid; //save to use later
 m_work_map_load_slots = MathHelpers::BuildGridFromRange(m_ldaxMin, m_ldaxMax, 16);
 const float* pLoadGrid = m_ldaxUseTable ? mp_lodGrid : &m_work_map_load_slots[0];
 m_pVEPageDlg->BindLoadGrid(pLoadGrid);
 m_pVEPageDlg->BindLoadGrid2(pGrid2);
 m_pAFRPageDlg->BindLoadGrid(pLoadGrid);
 m_pITPageDlg->BindLoadGrid(pLoadGrid);
 m_pPwm1PageDlg->BindLoadGrid(pLoadGrid);
 m_pPwm2PageDlg->BindLoadGrid(pLoadGrid); 
}

void CGridModeEditorInjDlg::UpdateView(bool axisLabels /*= fasle*/)
{
 if (::IsWindow(this->m_hWnd))
 {
  if (::IsWindow(m_pVEPageDlg->m_hWnd))
   m_pVEPageDlg->UpdateView(axisLabels); 
  if (::IsWindow(m_pAFRPageDlg->m_hWnd))
   m_pAFRPageDlg->UpdateView(axisLabels); 
  if (::IsWindow(m_pITPageDlg->m_hWnd))
   m_pITPageDlg->UpdateView(axisLabels); 
  if (::IsWindow(m_pIRegPageDlg->m_hWnd))
   m_pIRegPageDlg->UpdateView(axisLabels); 
  if (::IsWindow(m_pEnrPageDlg->m_hWnd))
   m_pEnrPageDlg->UpdateView(axisLabels); 
  if (::IsWindow(m_pOtherPageDlg->m_hWnd))
   m_pOtherPageDlg->UpdateView(axisLabels); 
  if (::IsWindow(m_pOther1PageDlg->m_hWnd))
   m_pOther1PageDlg->UpdateView(axisLabels); 
  if (::IsWindow(m_pPwm1PageDlg->m_hWnd))
   m_pPwm1PageDlg->UpdateView(axisLabels); 
  if (::IsWindow(m_pPwm2PageDlg->m_hWnd))
   m_pPwm2PageDlg->UpdateView(axisLabels); 
 } 
}

void CGridModeEditorInjDlg::OnChangeVE(void)
{
 if (m_OnMapChanged)
  m_OnMapChanged(TYPE_MAP_INJ_VE);
}

void CGridModeEditorInjDlg::OnChangeVE2(void)
{
 if (m_OnMapChanged)
  m_OnMapChanged(TYPE_MAP_INJ_VE2);
}

void CGridModeEditorInjDlg::OnChangeAFR(void)
{
 if (m_OnMapChanged)
  m_OnMapChanged(TYPE_MAP_INJ_AFR);
}

void CGridModeEditorInjDlg::OnChangeIT(void)
{
 if (m_OnMapChanged)
  m_OnMapChanged(TYPE_MAP_INJ_IT);
}

void CGridModeEditorInjDlg::OnChangeIReg(int mapId)
{
 if (m_OnMapChanged)
  m_OnMapChanged(mapId);
}

void CGridModeEditorInjDlg::OnChangeEnr(int mapId)
{
 if (m_OnMapChanged)
  m_OnMapChanged(mapId);
}

void CGridModeEditorInjDlg::OnChangeOther(int mapId)
{
 if (m_OnMapChanged)
  m_OnMapChanged(mapId);
}

void CGridModeEditorInjDlg::OnChangeOther1(int mapId)
{
 if (m_OnMapChanged)
  m_OnMapChanged(mapId);
}

void CGridModeEditorInjDlg::OnChangePwm1(void)
{
 if (m_OnMapChanged)
  m_OnMapChanged(TYPE_MAP_PWM1);
}

void CGridModeEditorInjDlg::OnChangePwm2(void)
{
 if (m_OnMapChanged)
  m_OnMapChanged(TYPE_MAP_PWM2);
}

void CGridModeEditorInjDlg::setIsAllowed(EventResult IsFunction)
{m_IsAllowed = IsFunction;}

void CGridModeEditorInjDlg::setOnMapChanged(EventWithCode OnFunction)
{
 m_OnMapChanged = OnFunction;
}

void CGridModeEditorInjDlg::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CGridModeEditorInjDlg::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CGridModeEditorInjDlg::SetDynamicValues(const TablDesk::DynVal& dv)
{
 //Update vertical axis of work map
 bool useBaroMax = (m_ldaxMax == std::numeric_limits<float>::max());
 if (m_ldaxNeedsUpdate || ((m_baro_press != dv.baro_press) && useBaroMax))
 {
  m_work_map_load_slots = MathHelpers::BuildGridFromRange(m_ldaxMin, useBaroMax ? dv.baro_press : m_ldaxMax, 16);
  const float* pLoadGrid = m_ldaxUseTable ? mp_lodGrid : &m_work_map_load_slots[0];
  m_pVEPageDlg->BindLoadGrid(pLoadGrid, true);
  m_pAFRPageDlg->BindLoadGrid(pLoadGrid, true);
  m_pITPageDlg->BindLoadGrid(pLoadGrid, true);
  m_pPwm1PageDlg->BindLoadGrid(pLoadGrid, true);
  m_pPwm2PageDlg->BindLoadGrid(pLoadGrid, true);
  m_ldaxNeedsUpdate = false;
 }

 m_baro_press = dv.baro_press;

 m_pVEPageDlg->SetArguments(dv.rpm, dv.air_flow, dv.strt_use, dv.load, dv.tps);
 m_pAFRPageDlg->SetArguments(dv.rpm, dv.air_flow, dv.strt_use, dv.load);
 m_pITPageDlg->SetArguments(dv.rpm, dv.air_flow, dv.strt_use, dv.load);
 m_pIRegPageDlg->SetArguments(dv.strt_use, dv.temp, dv.tps, dv.iac_pos, dv.rigid_arg, dv.rigid_use, dv.air_temp);
 m_pEnrPageDlg->SetArguments(dv.strt_use, dv.temp, dv.tpsdot, dv.rpm, dv.aftstr_enr, dv.mapdot);
 m_pOtherPageDlg->SetArguments(dv.strt_use, dv.temp, dv.voltage, dv.lambda_mx, dv.rpm, dv.tmp2, dv.air_temp, dv.map2, dv.rxlaf);
 m_pOther1PageDlg->SetArguments(dv.strt_use, dv.rpm);
 m_pPwm1PageDlg->SetArguments(dv.rpm, dv.air_flow, dv.strt_use, dv.load);
 m_pPwm2PageDlg->SetArguments(dv.rpm, dv.air_flow, dv.strt_use, dv.load);
}

void CGridModeEditorInjDlg::SetLoadAxisCfg(float minVal, float maxVal, bool useTable, bool forceUpdate /*=false*/)
{
 if ((m_ldaxMin != minVal) || (m_ldaxMax != maxVal) || (m_ldaxUseTable != useTable))
  m_ldaxNeedsUpdate = true;
 m_ldaxMin = minVal;
 m_ldaxMax = maxVal;
 m_ldaxUseTable = useTable;

 if (m_ldaxNeedsUpdate && forceUpdate)
 {
  if (m_ldaxMax > 500.0f) //prevent program crach
   m_ldaxMax = 500.0f; //kPa
  m_work_map_load_slots = MathHelpers::BuildGridFromRange(m_ldaxMin, m_ldaxMax, 16);
  const float* pLoadGrid = m_ldaxUseTable ? mp_lodGrid : &m_work_map_load_slots[0];
  m_pVEPageDlg->BindLoadGrid(pLoadGrid, true);
  m_pAFRPageDlg->BindLoadGrid(pLoadGrid, true);
  m_pITPageDlg->BindLoadGrid(pLoadGrid, true);
  m_pPwm1PageDlg->BindLoadGrid(pLoadGrid, true);
  m_pPwm2PageDlg->BindLoadGrid(pLoadGrid, true); 
  m_ldaxNeedsUpdate = false;
 }
}

void CGridModeEditorInjDlg::SetITMode(int mode)
{
 m_pITPageDlg->SetITMode(mode);
}

int CGridModeEditorInjDlg::GetITMode(void) const
{
 return m_pITPageDlg->GetITMode();
}

void CGridModeEditorInjDlg::SetActiveVEMap(int vemapid)
{
 m_pVEPageDlg->SetActiveVEMap(vemapid);
}

int CGridModeEditorInjDlg::GetActiveVEMap(void) const
{
 return m_pVEPageDlg->GetActiveVEMap();
}

void CGridModeEditorInjDlg::setOnChangeSettings(EventHandler OnCB)
{
 m_pITPageDlg->setOnChangeSettings(OnCB);
 m_pVEPageDlg->setOnChangeSettings(OnCB);
}

void CGridModeEditorInjDlg::SetSplitAngMode(bool mode)
{
 m_pPwm1PageDlg->SetSplitAngMode(mode);

 if (m_tab_control.GetSafeHwnd())
 {
  if (mode)
   m_tab_control.SetPageCaption(m_pwm1TabIdx, MLL::LoadString(IDS_GME_INJ_SPLIT_TAB)); //split angle
  else
   m_tab_control.SetPageCaption(m_pwm1TabIdx, MLL::LoadString(IDS_GME_INJ_PWM1_TAB));  //PWM duty
 }
}

void CGridModeEditorInjDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  if (m_tab_control.GetSafeHwnd())
  {
   DPIAware dpi;
   m_tab_control.SetWindowPos(NULL, 0,0, cx - dpi.ScaleX(10), cy - dpi.ScaleY(10), SWP_NOMOVE | SWP_NOZORDER);
   m_tab_control.UpdateWindow();
  }
 }
}

void CGridModeEditorInjDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
 if (m_initialized)
 {
  DPIAware dpi;
  lpMMI->ptMinTrackSize.x = dpi.ScaleX(600);
  lpMMI->ptMinTrackSize.y = dpi.ScaleY(450);
 }
}
