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

/** \file GridModeEditorIgnDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "common/Dll.h"
#include "common/MathHelpers.h"
#include "common/DPIAware.h"
#include "common/GDIHelpers.h"
#include "GridModeEditorIgnDlg.h"
#include "io-core/SECU3IO.h"
#include "MapIds.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/Label.h"
#include "ui-core/MsgBox.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/CtrlScaler.h"

const UINT CGridModeEditorIgnDlg::IDD = IDD_GRID_MODE_EDITOR_IGN;

/////////////////////////////////////////////////////////////////////////////
// CGridModeEditorIgnDlg dialog

BEGIN_MESSAGE_MAP(CGridModeEditorIgnDlg, Super)
 ON_WM_GETMINMAXINFO()
 ON_WM_SIZE()
 ON_WM_CLOSE()
 ON_BN_CLICKED(IDC_GME_IGN_CLTSEL, OnSelectCLTMap)
 ON_UPDATE_COMMAND_UI(IDC_GME_IGN_STR, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IGN_IDL, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IGN_WRK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IGN_TMP, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IGN_TMPI, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IGN_CLTSEL, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AA_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_WM_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_OC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_TC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_KC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IM_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AA_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AAE_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_WM_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_WMP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_OC_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_OCP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_TC_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_TCP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_KC_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_KCP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IM_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IMP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IC_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_ICP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AC_VALUE, OnUpdateAAControls)   //air temper. corr.
 ON_UPDATE_COMMAND_UI(IDC_GME_AA_UNIT, OnUpdateAAControls)
END_MESSAGE_MAP()

CGridModeEditorIgnDlg::CGridModeEditorIgnDlg(CWnd* pParent /*=NULL*/)
: Super(CGridModeEditorIgnDlg::IDD, pParent)
, mp_startMap(NULL)
, mp_idleMap(NULL)
, mp_workMap(NULL)
, mp_tempMap(NULL)
, mp_rpmGrid(NULL)
, mp_cltGrid(NULL)
, mp_lodGrid(NULL)
, mp_tempIdlMap(NULL)
, m_en_aa_indication(false)
, m_start_map(1, 16, false, false, DLL::GetModuleHandle())
, m_idle_map(1, 16, false, false, DLL::GetModuleHandle())
, m_work_map(16, 16, true, true, DLL::GetModuleHandle(), 3)
, m_temp_map(1, 16, false, false, DLL::GetModuleHandle())
, m_tempi_map(1, 16, false, false, DLL::GetModuleHandle())
, m_ldaxMin(1.0f)
, m_ldaxMax(16.0f)
, m_ldaxNeedsUpdate(false)
, m_ldaxUseTable(false)
, mp_acronLink(new CLabel)
, m_initialized(false)
, mp_cscl(new CtrlScaler)
{
 _ResetUseFlags();
 m_work_map_load_slots.reserve(32);
 m_work_map_load_slots = MathHelpers::BuildGridFromRange(1.0f, 16.0f, 16, true);  //<--reverse order
 m_curDV.baro_press = 101.3f; //sea level atmospheric pressure by default

 wrk_caption_wrk = MLL::GetString(IDS_GME_WM_CAPTION_WRKTEXT);
 wrk_caption_str = MLL::GetString(IDS_GME_WM_CAPTION_STRTEXT);
}

CGridModeEditorIgnDlg::~CGridModeEditorIgnDlg()
{
 //empty
}

void CGridModeEditorIgnDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 //grid controls
 DDX_Control(pDX, IDC_GME_IGN_WRK, m_work_map);
 DDX_Control(pDX, IDC_GME_IGN_IDL, m_idle_map);
 DDX_Control(pDX, IDC_GME_IGN_TMP, m_temp_map);
 DDX_Control(pDX, IDC_GME_IGN_TMPI, m_tempi_map);
 DDX_Control(pDX, IDC_GME_IGN_STR, m_start_map);
 DDX_Control(pDX, IDC_GME_IGN_CLTSEL, m_cltmap_sel);

 //Advance angle indication controls
 DDX_Control(pDX, IDC_GME_AA_VALUE, m_aa_value);
 DDX_Control(pDX, IDC_GME_WM_VALUE, m_wm_value);
 DDX_Control(pDX, IDC_GME_OC_VALUE, m_oc_value);
 DDX_Control(pDX, IDC_GME_TC_VALUE, m_tc_value);
 DDX_Control(pDX, IDC_GME_KC_VALUE, m_kc_value);
 DDX_Control(pDX, IDC_GME_IM_VALUE, m_im_value);
 DDX_Control(pDX, IDC_GME_IC_VALUE, m_ic_value);
 DDX_Control(pDX, IDC_GME_AC_VALUE, m_ac_value);

 DDX_Control(pDX, IDC_GME_ACRONYMS_LINK, *mp_acronLink);

 DDX_Control(pDX, IDC_GME_IDL_TITLE, m_ctrls[0]);
 DDX_Control(pDX, IDC_GME_STR_TITLE, m_ctrls[1]);
 DDX_Control(pDX, IDC_GME_TMP_TITLE, m_ctrls[2]);
 DDX_Control(pDX, IDC_GME_AA_CAPTION, m_ctrls[3]);
 DDX_Control(pDX, IDC_GME_WM_CAPTION, m_ctrls[4]);
 DDX_Control(pDX, IDC_GME_OC_CAPTION, m_ctrls[5]); 
 DDX_Control(pDX, IDC_GME_TC_CAPTION, m_ctrls[6]);
 DDX_Control(pDX, IDC_GME_KC_CAPTION, m_ctrls[7]);
 DDX_Control(pDX, IDC_GME_IM_CAPTION, m_ctrls[8]);
 DDX_Control(pDX, IDC_GME_IC_CAPTION, m_ctrls[9]);
 DDX_Control(pDX, IDC_GME_AC_CAPTION, m_ctrls[10]);
 DDX_Control(pDX, IDC_GME_AAE_TEXT, m_ctrls[11]);
 DDX_Control(pDX, IDC_GME_WMP_TEXT, m_ctrls[12]);
 DDX_Control(pDX, IDC_GME_OCP_TEXT, m_ctrls[13]);
 DDX_Control(pDX, IDC_GME_TCP_TEXT, m_ctrls[14]);
 DDX_Control(pDX, IDC_GME_KCP_TEXT, m_ctrls[15]);
 DDX_Control(pDX, IDC_GME_IMP_TEXT, m_ctrls[16]);
 DDX_Control(pDX, IDC_GME_ICP_TEXT, m_ctrls[17]);
 DDX_Control(pDX, IDC_GME_AA_UNIT, m_ctrls[18]);
}

/////////////////////////////////////////////////////////////////////////////
// CGridModeEditorIgnDlg message handlers

BOOL CGridModeEditorIgnDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //Set bold font to advance angle indication controls
 CloneWndFont(&m_aa_value, &m_fieldFont, 11, true);
 m_aa_value.SetFont(&m_fieldFont);
 m_wm_value.SetFont(&m_fieldFont);
 m_oc_value.SetFont(&m_fieldFont);
 m_tc_value.SetFont(&m_fieldFont);
 m_kc_value.SetFont(&m_fieldFont);
 m_im_value.SetFont(&m_fieldFont);
 m_ic_value.SetFont(&m_fieldFont);
 m_ac_value.SetFont(&m_fieldFont);

 CloneWndFont(this, &m_font, -1, false);

 //start
 m_start_map.setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnChangeStart));
 m_start_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnAbroadMoveStart));
 m_start_map.SetRange(-15.0f, 55.0f);
 m_start_map.AttachMap(mp_startMap);
 m_start_map.AttachLabels(SECU3IO::start_map_rpm_slots, NULL);
 m_start_map.ShowLabels(true, false);
 m_start_map.SetFont(&m_font);
 m_start_map.EnableAbroadMove(true, false);
 m_start_map.SetValueIncrement(0.5f);

 //idle
 m_idle_map.setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnChangeIdle));
 m_idle_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnAbroadMoveIdle));
 m_idle_map.SetRange(-15.0f, 55.0f);
 m_idle_map.AttachMap(mp_idleMap);
 m_idle_map.AttachLabels(mp_rpmGrid, NULL);
 m_idle_map.ShowLabels(false, false);
 m_idle_map.SetFont(&m_font);
 m_idle_map.EnableAbroadMove(true, true);
 m_idle_map.SetValueIncrement(0.5f);

 //temp
 m_temp_map.setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnChangeTemp));
 m_temp_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnAbroadMoveTemp));
 m_temp_map.SetRange(-15.0f, 25.0f);
 m_temp_map.AttachMap(mp_tempMap);
 m_temp_map.AttachLabels(mp_cltGrid, NULL);
 m_temp_map.ShowLabels(true, false);
 m_temp_map.SetFont(&m_font);
 m_temp_map.EnableAbroadMove(true, true);
 m_temp_map.SetValueIncrement(0.5f);

 //temp (idling)
 m_tempi_map.setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnChangeTempIdl));
 m_tempi_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnAbroadMoveTempIdl));
 m_tempi_map.SetRange(-15.0f, 25.0f);
 m_tempi_map.AttachMap(mp_tempIdlMap);
 m_tempi_map.AttachLabels(mp_cltGrid, NULL);
 m_tempi_map.ShowLabels(true, false);
 m_tempi_map.SetFont(&m_font);
 m_tempi_map.EnableAbroadMove(true, true);
 m_tempi_map.SetValueIncrement(0.5f);

 //work
 m_work_map.setOnChange(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnChangeWork));
 m_work_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGridModeEditorIgnDlg::OnAbroadMoveWork));
 m_work_map.SetRange(-15.0f, 55.00f);
 m_work_map.AttachMap(mp_workMap);
 m_work_map.AttachLabels(mp_rpmGrid, m_ldaxUseTable ? mp_lodGrid : &m_work_map_load_slots[0]);
 m_work_map.ShowLabels(true, true);
 m_work_map.SetDecimalPlaces(2, 0, 0);
 m_work_map.SetFont(&m_font);
 m_work_map.EnableAbroadMove(false, true);
 m_work_map.SetValueIncrement(0.5f);

 SetIcon((HICON)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDI_GRAPH), IMAGE_ICON, 0, 0, LR_SHARED), TRUE);

 //init HTTP links:
 mp_acronLink->SetLink(true);
 mp_acronLink->SetTextColor(RGB(0, 0, 255));
 mp_acronLink->SetFontUnderline(true);
 mp_acronLink->SetLinkCursor((HCURSOR)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));
 mp_acronLink->SetOnClick(fastdelegate::MakeDelegate(this, &CGridModeEditorIgnDlg::OnAcronymsLinkClick));

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_cltmap_sel, MLL::GetString(IDS_GME_IGN_CLTSEL_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 m_initialized = true;

 //initialize scaler
 mp_cscl->Init(this);
 mp_cscl->Add(&m_work_map);
 mp_cscl->Add(&m_idle_map);
 mp_cscl->Add(&m_temp_map);
 mp_cscl->Add(&m_tempi_map);
 mp_cscl->Add(&m_start_map);
 mp_cscl->Add(&m_cltmap_sel);
 mp_cscl->Add(&m_aa_value);
 mp_cscl->Add(&m_wm_value);
 mp_cscl->Add(&m_oc_value);
 mp_cscl->Add(&m_tc_value);
 mp_cscl->Add(&m_kc_value);
 mp_cscl->Add(&m_im_value);
 mp_cscl->Add(&m_ic_value);
 mp_cscl->Add(&m_ac_value);
 mp_cscl->Add(mp_acronLink.get());
 for(int i = 0; i < 19; ++i)
  mp_cscl->Add(&m_ctrls[i]);

 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(this->m_hWnd, TYPE_MAP_GME_IGN_WND);

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGridModeEditorIgnDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool allowed = m_IsAllowed ? m_IsAllowed() : false;
 pCmdUI->Enable(allowed);
}

void CGridModeEditorIgnDlg::OnUpdateAAControls(CCmdUI* pCmdUI)
{
 bool allowed = m_IsAllowed ? m_IsAllowed() : false;
 bool flag = true;

 if (pCmdUI->m_nID == IDC_GME_WM_VALUE) 
 {
  flag = m_curDV.strt_use ? true : m_curDV.work_use;
 }

 if (pCmdUI->m_nID == IDC_GME_WM_CAPTION) 
 {
  pCmdUI->SetText(m_curDV.strt_use ? wrk_caption_str.c_str() : wrk_caption_wrk.c_str());
 }

 else if (pCmdUI->m_nID == IDC_GME_OC_VALUE) flag = m_curDV.octan_use;
 else if (pCmdUI->m_nID == IDC_GME_TC_VALUE) flag = m_curDV.temp_use;
 else if (pCmdUI->m_nID == IDC_GME_KC_VALUE) flag = m_curDV.knkret_use;
 else if (pCmdUI->m_nID == IDC_GME_IM_VALUE) flag = m_curDV.idle_use;
 else if (pCmdUI->m_nID == IDC_GME_IC_VALUE) flag = m_curDV.idlreg_use;
 else if (pCmdUI->m_nID == IDC_GME_AC_VALUE) flag = m_curDV.airt_use;

 pCmdUI->Enable(allowed && m_en_aa_indication && flag);
}

void CGridModeEditorIgnDlg::_ResetUseFlags(void)
{
 m_curDV.strt_use = m_curDV.work_use = m_curDV.idle_use = m_curDV.temp_use =
 m_curDV.airt_use = m_curDV.idlreg_use = m_curDV.octan_use = m_curDV.knkret_use = false;
}

void CGridModeEditorIgnDlg::OnClose()
{
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(this->m_hWnd, TYPE_MAP_GME_IGN_WND);
 Super::OnClose(); //close window
 DestroyWindow();
}

void CGridModeEditorIgnDlg::BindMaps(float* pStart, float* pIdle, float* pWork, float* pTemp, float* pTempIdl)
{
 mp_startMap = pStart;
 mp_idleMap = pIdle;
 mp_workMap = pWork;
 mp_tempMap = pTemp;
 mp_tempIdlMap = pTempIdl;
}

void CGridModeEditorIgnDlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
}

void CGridModeEditorIgnDlg::BindCLTGrid(float* pGrid)
{
 mp_cltGrid = pGrid;
}

void CGridModeEditorIgnDlg::BindLoadGrid(float* pGrid)
{
 mp_lodGrid = pGrid;
}

void CGridModeEditorIgnDlg::UpdateView(bool axisLabels /*= false*/)
{
 if (::IsWindow(this->m_hWnd))
 {
  if (axisLabels)
  {
   m_idle_map.AttachLabels(mp_rpmGrid, NULL);
   m_work_map.AttachLabels(mp_rpmGrid, m_ldaxUseTable ? mp_lodGrid : &m_work_map_load_slots[0]);
   m_temp_map.AttachLabels(mp_cltGrid, NULL);
   m_tempi_map.AttachLabels(mp_cltGrid, NULL);
  }
  m_start_map.UpdateDisplay();
  m_idle_map.UpdateDisplay();
  m_work_map.UpdateDisplay();
  m_temp_map.UpdateDisplay();
  m_tempi_map.UpdateDisplay();
 }
}

void CGridModeEditorIgnDlg::EnableAdvanceAngleIndication(bool i_enable)
{
 m_en_aa_indication = i_enable;
 bool is_wnd = ::IsWindow(this->m_hWnd);

 if (!i_enable)
 {
  _ResetUseFlags();
  if (is_wnd)
   Invalidate();
 }

 if (is_wnd)
  UpdateDialogControls(this, true);
}

void CGridModeEditorIgnDlg::SetDynamicValues(const TablDesk::DynVal& dv)
{
 bool baroChanged = false;
 if (!GetSafeHwnd())
  return;

 m_aa_value.SetValue(dv.adv_ang);
 m_kc_value.SetValue(dv.knock_retard);
 m_im_value.SetValue(dv.idle_aalt);
 m_wm_value.SetValue(dv.strt_use ? dv.strt_aalt : dv.work_aalt);
 m_tc_value.SetValue(dv.temp_aalt);
 m_ac_value.SetValue(dv.airt_aalt);
 m_ic_value.SetValue(dv.idlreg_aac);
 m_oc_value.SetValue(dv.octan_aac);

 if (m_curDV.baro_press != dv.baro_press)
  baroChanged = true;

 m_curDV = dv;
 UpdateDialogControls(this, true);  //todo: check it for perfomance issues

 //updating work points:
 m_start_map.ShowMarkers(dv.strt_use, true);
 m_start_map.SetArguments(0, (float)dv.rpm);

 m_idle_map.ShowMarkers(dv.idle_use, true);
 m_idle_map.SetArguments(0, (float)dv.rpm);

 m_temp_map.ShowMarkers(dv.temp_use && !dv.strt_use && !dv.idlreg_use, true);
 m_temp_map.SetArguments(0, dv.temp);

 m_tempi_map.ShowMarkers(dv.temp_use && !dv.strt_use && dv.idlreg_use, true);
 m_tempi_map.SetArguments(0, dv.temp);

 //Update vertical axis of work map
 bool useBaroMax = (m_ldaxMax == std::numeric_limits<float>::max());
 if (m_ldaxNeedsUpdate || (baroChanged && useBaroMax))
 {
  m_work_map_load_slots = MathHelpers::BuildGridFromRange(m_ldaxMin, useBaroMax ? m_curDV.baro_press : m_ldaxMax, 16, true); //<-- reverse order
  m_work_map.AttachLabels(mp_rpmGrid, m_ldaxUseTable ? mp_lodGrid : &m_work_map_load_slots[0]);
  m_work_map.UpdateDisplay();
  m_ldaxNeedsUpdate = false;
 }

 m_work_map.ShowMarkers(dv.work_use && dv.air_flow, true);
 m_work_map.SetArguments(dv.load, (float)dv.rpm);
}

void CGridModeEditorIgnDlg::SetLoadAxisCfg(float minVal, float maxVal, bool useTable, bool forceUpdate /*=false*/)
{
 if ((m_ldaxMin != minVal) || (m_ldaxMax != maxVal) || (m_ldaxUseTable != useTable))
  m_ldaxNeedsUpdate = true;
 m_ldaxMin = minVal;
 m_ldaxMax = maxVal;
 m_ldaxUseTable = useTable;

 if (m_ldaxNeedsUpdate && forceUpdate)
 {
  m_work_map_load_slots = MathHelpers::BuildGridFromRange(m_ldaxMin, m_ldaxMax, 16, true); //<-- reverse order
  m_work_map.AttachLabels(mp_rpmGrid, m_ldaxUseTable ? mp_lodGrid : &m_work_map_load_slots[0]);
  m_work_map.UpdateDisplay(); 
  m_ldaxNeedsUpdate = false;
 }
}

void CGridModeEditorIgnDlg::setIsAllowed(EventResult IsFunction)
{m_IsAllowed = IsFunction;}

void CGridModeEditorIgnDlg::setOnMapChanged(EventWithCode OnFunction)
{m_OnMapChanged = OnFunction;}

void CGridModeEditorIgnDlg::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CGridModeEditorIgnDlg::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CGridModeEditorIgnDlg::OnChangeStart(void)
{
 m_OnMapChanged(TYPE_MAP_DA_START);
}

void CGridModeEditorIgnDlg::OnChangeIdle(void)
{
 m_OnMapChanged(TYPE_MAP_DA_IDLE);
}

void CGridModeEditorIgnDlg::OnChangeWork(void)
{
 m_OnMapChanged(TYPE_MAP_DA_WORK);
}

void CGridModeEditorIgnDlg::OnChangeTemp(void)
{
 m_OnMapChanged(TYPE_MAP_DA_TEMP_CORR);
}

void CGridModeEditorIgnDlg::OnChangeTempIdl(void)
{
 m_OnMapChanged(TYPE_MAP_DA_TEMPI_CORR);
}

void CGridModeEditorIgnDlg::OnAbroadMoveStart(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
 {
  if (m_cltmap_sel.GetCheck()==BST_CHECKED)
   m_tempi_map.SetSelection(0, column);
  else
   m_temp_map.SetSelection(0, column);
 }
}

void CGridModeEditorIgnDlg::OnAbroadMoveIdle(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_work_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
 {
  if (m_cltmap_sel.GetCheck()==BST_CHECKED)
   m_tempi_map.SetSelection(0, column);
  else
   m_temp_map.SetSelection(0, column);
 }
}

void CGridModeEditorIgnDlg::OnAbroadMoveWork(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_idle_map.SetSelection(0, column);
}

void CGridModeEditorIgnDlg::OnAbroadMoveTemp(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_idle_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_start_map.SetSelection(0, column);
}

void CGridModeEditorIgnDlg::OnAbroadMoveTempIdl(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_idle_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_start_map.SetSelection(0, column);
}

void CGridModeEditorIgnDlg::OnAcronymsLinkClick()
{
 SECUMessageBox(IDS_GME_DESCR_ABBREV, MB_OK | MB_ICONINFORMATION);
}

void CGridModeEditorIgnDlg::OnSelectCLTMap()
{
 bool idl = m_cltmap_sel.GetCheck()==BST_CHECKED;
 m_temp_map.ShowWindow(idl ? SW_HIDE : SW_SHOW);
 m_tempi_map.ShowWindow(idl ? SW_SHOW : SW_HIDE);
}

void CGridModeEditorIgnDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  mp_cscl->Scale();
 }
}

void CGridModeEditorIgnDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
 if (m_initialized)
 {
  DPIAware dpi;
  lpMMI->ptMinTrackSize.x = dpi.ScaleX(600);
  lpMMI->ptMinTrackSize.y = dpi.ScaleY(450);
 }
}
