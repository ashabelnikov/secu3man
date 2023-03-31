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

/** \file GMEInjVEDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjVEDlg.h"
#include "common/fastdelegate.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/CtrlScaler.h"

const UINT CGMEInjVEDlg::IDD = IDD_GME_INJ_VE;

const COLORREF colorBlocked = RGB(100,100,100);

/////////////////////////////////////////////////////////////////////////////
// CGMEInjVEDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjVEDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_VE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_LAMDEL_BTN,OnUpdateControlsAutoTune1)
 ON_BN_CLICKED(IDC_GME_INJ_LAMDEL_BTN,OnLamDelButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_CELWGT_BTN,OnUpdateControlsAutoTune1)
 ON_BN_CLICKED(IDC_GME_INJ_CELWGT_BTN,OnCelWgtButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_STRSTP_BTN,OnUpdateControlsAutoTune1)
 ON_BN_CLICKED(IDC_GME_INJ_STRSTP_BTN,OnStrStpButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_RSTSTT_BTN,OnUpdateControlsAutoTune1)
 ON_BN_CLICKED(IDC_GME_INJ_RSTSTT_BTN,OnRstSttButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_AUTOTUNE_GROUPBOX, OnUpdateControlsAutoTune1)
 ON_BN_CLICKED(IDC_GME_INJ_CELBLK_BTN,OnCelBlkButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_CELBLK_BTN,OnUpdateControlsAutoTune2)
 ON_BN_CLICKED(IDC_GME_INJ_SMOOTH_BTN,OnSmoothButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_SMOOTH_BTN,OnUpdateControlsAutoTune1)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_BLKALL_CHECK,OnUpdateControlsAutoTune2)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_FINISH_CHECK,OnUpdateControlsAutoTune1)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_RSTALL_CHECK,OnUpdateControlsAutoTune1)
 ON_BN_CLICKED(IDC_GME_INJ_VE1_RADIOBOX, OnVE1Button)
 ON_BN_CLICKED(IDC_GME_INJ_VE2_RADIOBOX, OnVE2Button)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_VE1_RADIOBOX, OnUpdateControlsAutoTune3)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_VE2_RADIOBOX, OnUpdateControlsAutoTune3)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

CGMEInjVEDlg::CGMEInjVEDlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjVEDlg::IDD, pParent)
, m_ve_map(16, 16, false, false, NULL, 3) 
, m_ve2_map(16, 16, false, false, NULL, 3) 
, m_lamdel_map(3, 3, false, false, NULL, 3)
, m_celwgt_map(16, 16, false, false, NULL, 3, true) //read-only
, mp_VEMap(NULL)
, mp_VEMap2(NULL)
, mp_rpmGrid(NULL)
, mp_loadGrid(NULL)
, mp_loadGrid2(NULL)
, mp_CelWgtMap(NULL)
, mp_LamDelMap(NULL)
, mp_rpmGridLD(NULL)
, mp_loadGridLD(NULL)
, mp_CelBlkMap(NULL)
, m_active_ve(0) //VE1 is active by default
, mp_cscl(new CtrlScaler)
, m_initialized(false)
{
 //empty
}

CGMEInjVEDlg::~CGMEInjVEDlg()
{
 //empty
}

void CGMEInjVEDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_VE, m_ve_map);
 DDX_Control(pDX, IDC_GME_INJ_VE2, m_ve2_map);
 DDX_Control(pDX, IDC_GME_INJ_LAMDEL_MAP, m_lamdel_map);
 DDX_Control(pDX, IDC_GME_INJ_CELWGT_MAP, m_celwgt_map);

 DDX_Control(pDX, IDC_GME_INJ_LAMDEL_BTN, m_lamdel_button);
 DDX_Control(pDX, IDC_GME_INJ_CELWGT_BTN, m_celwgt_button);
 DDX_Control(pDX, IDC_GME_INJ_STRSTP_BTN, m_strstp_button);
 DDX_Control(pDX, IDC_GME_INJ_RSTSTT_BTN, m_rststt_button);
 DDX_Control(pDX, IDC_GME_INJ_CELBLK_BTN, m_celblk_button);
 DDX_Control(pDX, IDC_GME_INJ_SMOOTH_BTN, m_smooth_button);
 DDX_Control(pDX, IDC_GME_INJ_BLKALL_CHECK, m_blkall_check);
 DDX_Control(pDX, IDC_GME_INJ_FINISH_CHECK, m_finish_check);
 DDX_Control(pDX, IDC_GME_INJ_RSTALL_CHECK, m_rstall_check);

 DDX_Control(pDX, IDC_GME_INJ_STATUS_TEXT, m_status_text);

 DDX_Control(pDX, IDC_GME_INJ_VE1_RADIOBOX, m_ve1_radio);
 DDX_Control(pDX, IDC_GME_INJ_VE2_RADIOBOX, m_ve2_radio);

 DDX_Control(pDX, IDC_GME_AUTOTUNE_GROUPBOX, m_atgroup);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjVEDlg message handlers

BOOL CGMEInjVEDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
  CloneWndFont(this, &m_font, -1, false);

 if (0==m_active_ve)
 { //VE1
  m_ve1_radio.SetCheck(BST_CHECKED);
  m_ve2_radio.SetCheck(BST_UNCHECKED);
 }
 else
 { //VE2
  m_ve1_radio.SetCheck(BST_UNCHECKED);
  m_ve2_radio.SetCheck(BST_CHECKED);
 }

 m_ve_map.SetRange(.0f, 1.99f);
 m_ve_map.AttachMap(mp_VEMap);
 m_ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_ve_map.ShowLabels(true, true);
 m_ve_map.SetDecimalPlaces(3, 0, 0);
 m_ve_map.SetFont(&m_font);
 m_ve_map.EnableAbroadMove(false, false);
 m_ve_map.SetValueIncrement(0.01f);
 m_ve_map.setOnSelChange(fastdelegate::MakeDelegate(this, CGMEInjVEDlg::OnSelChangeVE));
 m_ve_map.SetSelection(0, 0, false);

 if (0==m_active_ve)
  m_ve_map.ShowWindow(SW_SHOW); //show 1st map

 m_ve2_map.SetRange(.0f, 1.99f);
 m_ve2_map.AttachMap(mp_VEMap2);
 m_ve2_map.AttachLabels(mp_rpmGrid, mp_loadGrid2);
 m_ve2_map.ShowLabels(true, true);
 m_ve2_map.SetDecimalPlaces(3, 0, 0);
 m_ve2_map.SetFont(&m_font);
 m_ve2_map.EnableAbroadMove(false, false);
 m_ve2_map.SetValueIncrement(0.01f);
 m_ve2_map.setOnSelChange(fastdelegate::MakeDelegate(this, CGMEInjVEDlg::OnSelChangeVE));
 m_ve2_map.SetSelection(0, 0, false);

 if (1==m_active_ve)
  m_ve2_map.ShowWindow(SW_SHOW); //show 2nd map

 if (m_OnSelectVEMap)
  m_OnSelectVEMap(m_active_ve); //notify controller about selection of certain VE map

 UpdateBlockColor(GetVECtrl()); //update current VE map

 if (mp_LamDelMap)
 {
  m_lamdel_map.SetRange(10.0f, 2000.0f);
  m_lamdel_map.AttachMap(mp_LamDelMap);
  m_lamdel_map.AttachLabels(mp_rpmGridLD, mp_loadGridLD);
  m_lamdel_map.ShowLabels(true, true);
  m_lamdel_map.SetDecimalPlaces(0, 0, 0);
  m_lamdel_map.SetFont(&m_font);
  m_lamdel_map.EnableAbroadMove(false, false);
  m_lamdel_map.SetValueIncrement(1.0f);
  m_lamdel_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjVEDlg::OnChangeLamDel));
 }

 if (mp_CelWgtMap)
 {
  m_celwgt_map.SetRange(.0f, 255.0f);
  m_celwgt_map.AttachMap(mp_CelWgtMap);
  m_celwgt_map.AttachLabels(mp_rpmGrid, (0==m_active_ve ? mp_loadGrid : mp_loadGrid2));
  m_celwgt_map.ShowLabels(true, true);
  m_celwgt_map.SetDecimalPlaces(2, 0, 0);
  m_celwgt_map.SetFont(&m_font);
  m_celwgt_map.EnableAbroadMove(false, false);
  m_celwgt_map.SetValueIncrement(1.0f);
 }

 if (m_OnViewActivate)
  m_OnViewActivate();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_lamdel_button, MLL::GetString(IDS_GME_INJ_LAMDEL_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_celwgt_button, MLL::GetString(IDS_GME_INJ_CELWGT_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_strstp_button, MLL::GetString(IDS_GME_INJ_STRSTP_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rststt_button, MLL::GetString(IDS_GME_INJ_RSTSTT_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rstall_check, MLL::GetString(IDS_GME_INJ_RSTALL_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_celblk_button, MLL::GetString(IDS_GME_INJ_CELBLK_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_blkall_check, MLL::GetString(IDS_GME_INJ_BLKALL_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_smooth_button, MLL::GetString(IDS_GME_INJ_SMOOTH_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_finish_check, MLL::GetString(IDS_GME_INJ_FINISH_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ve1_radio, MLL::GetString(IDS_GME_INJ_VE1_RADIOBOX_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ve2_radio, MLL::GetString(IDS_GME_INJ_VE2_RADIOBOX_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //initialize scaler
 mp_cscl->Init(this);
 mp_cscl->Add(&m_ve_map);
 mp_cscl->Add(&m_ve2_map);
 mp_cscl->Add(&m_lamdel_map);
 mp_cscl->Add(&m_celwgt_map);
 mp_cscl->Add(&m_lamdel_button);
 mp_cscl->Add(&m_celwgt_button);
 mp_cscl->Add(&m_strstp_button);
 mp_cscl->Add(&m_rststt_button);
 mp_cscl->Add(&m_celblk_button);
 mp_cscl->Add(&m_smooth_button);
 mp_cscl->Add(&m_blkall_check);
 mp_cscl->Add(&m_finish_check);
 mp_cscl->Add(&m_rstall_check);
 mp_cscl->Add(&m_status_text);
 mp_cscl->Add(&m_ve1_radio);
 mp_cscl->Add(&m_ve2_radio);
 mp_cscl->Add(&m_atgroup);

 m_initialized = true;

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjVEDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

void CGMEInjVEDlg::OnUpdateControlsAutoTune(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_IsReady && IsWindowEnabled() && (m_IsReady && m_IsReady()));
}

void CGMEInjVEDlg::OnUpdateControlsAutoTune1(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_IsReady && IsWindowEnabled());
}

void CGMEInjVEDlg::OnUpdateControlsAutoTune2(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_IsReady && IsWindowEnabled() && (GetVECtrl().GetSelection().first != -1));
}

void CGMEInjVEDlg::OnUpdateControlsAutoTune3(CCmdUI* pCmdUI)
{
 bool not_tuning_now = true;
 if (m_IsReady)
  not_tuning_now = !m_IsReady();
 pCmdUI->Enable(not_tuning_now && IsWindowEnabled() && !m_celwgt_button.GetCheck()==BST_CHECKED && !m_lamdel_button.GetCheck()==BST_CHECKED);
}

LPCTSTR CGMEInjVEDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGMEInjVEDlg::BindMaps(float* pVE, float* pVE2)
{
 ASSERT(pVE);
 mp_VEMap = pVE;
 ASSERT(pVE2);
 mp_VEMap2 = pVE2;
}

void CGMEInjVEDlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjVEDlg::BindLoadGrid(const float* pGrid, bool updateLabels /*= false*/)
{
 mp_loadGrid = pGrid;
 ASSERT(pGrid);
 if (updateLabels /*&& m_ve1_radio.GetCheck()==BST_CHECKED*/)
 {
  m_ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  m_celwgt_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  if (m_ve_map.GetSafeHwnd())
   m_ve_map.UpdateDisplay(); 
  if (mp_CelWgtMap && m_celwgt_map.GetSafeHwnd())
   m_celwgt_map.UpdateDisplay(); 
 }
}

void CGMEInjVEDlg::BindLoadGrid2(const float* pGrid, bool updateLabels /*= false*/)
{
 mp_loadGrid2 = pGrid;
 ASSERT(pGrid);
 if (updateLabels /*&& m_ve2_radio.GetCheck()==BST_CHECKED*/)
 {
  m_ve2_map.AttachLabels(mp_rpmGrid, mp_loadGrid2);
  m_celwgt_map.AttachLabels(mp_rpmGrid, mp_loadGrid2);
  if (m_ve2_map.GetSafeHwnd())
   m_ve2_map.UpdateDisplay(); 
  if (mp_CelWgtMap && m_celwgt_map.GetSafeHwnd())
   m_celwgt_map.UpdateDisplay(); 
 }
}

void CGMEInjVEDlg::setOnChange(EventHandler OnCB)
{
 m_ve_map.setOnChange(OnCB);
}

void CGMEInjVEDlg::setOnChange2(EventHandler OnCB)
{
 m_ve2_map.setOnChange(OnCB);
}

void CGMEInjVEDlg::UpdateView(bool axisLabels /*= false*/)
{
 CMapEditorCtrl& ve_map = GetVECtrl();
 UpdateBlockColor(ve_map);
 if (axisLabels)
  ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid); //update axis labels
 ve_map.UpdateDisplay();
}

void CGMEInjVEDlg::SetArguments(int rpm, int air_flow, bool strt_use, float load, float tps)
{
 if (m_ve_map.GetSafeHwnd() && m_ve_map.IsWindowVisible())
 {
  m_ve_map.ShowMarkers(!strt_use, true);
  m_ve_map.SetArguments(load, (float)rpm);
 }
 if (m_ve2_map.GetSafeHwnd() && m_ve2_map.IsWindowVisible())
 {
  m_ve2_map.ShowMarkers(!strt_use, true);
  m_ve2_map.SetArguments(tps, (float)rpm);
 }
 if (m_lamdel_map.GetSafeHwnd() && m_lamdel_map.IsWindowVisible())
 {
  m_lamdel_map.ShowMarkers(true, true);
  m_lamdel_map.SetArguments(load, (float)rpm);
 }
 if (m_celwgt_map.GetSafeHwnd() && m_celwgt_map.IsWindowVisible())
 {
  m_celwgt_map.ShowMarkers(true, true);
  m_celwgt_map.SetArguments(0==m_active_ve ? load : tps, (float)rpm);
 }
}

void CGMEInjVEDlg::OnLamDelButton()
{
 bool checked = m_lamdel_button.GetCheck() == BST_CHECKED;
 if (checked)
  m_celwgt_button.SetCheck(BST_UNCHECKED);
 GetVECtrl().ShowWindow(checked ? SW_HIDE : SW_SHOW);
 m_lamdel_map.ShowWindow(checked ? SW_SHOW : SW_HIDE);
 m_celwgt_map.ShowWindow(SW_HIDE);
}

void CGMEInjVEDlg::OnCelWgtButton()
{
 bool checked = m_celwgt_button.GetCheck() == BST_CHECKED;
 if (checked)
  m_lamdel_button.SetCheck(BST_UNCHECKED);
 GetVECtrl().ShowWindow(checked ? SW_HIDE : SW_SHOW);
 m_lamdel_map.ShowWindow(SW_HIDE);
 m_celwgt_map.ShowWindow(checked ? SW_SHOW : SW_HIDE);
}

void CGMEInjVEDlg::OnStrStpButton()
{
 if (m_OnStrStp)
  m_OnStrStp();
}

void CGMEInjVEDlg::OnRstSttButton()
{
 if (m_OnRstStt)
  m_OnRstStt();
}

void CGMEInjVEDlg::OnCelBlkButton()
{
 if (!mp_CelBlkMap)
  return;
 CMapEditorCtrl& ve_map = GetVECtrl();

 std::pair<int, int> sel = ve_map.GetSelection();
 bool* p_cell = &mp_CelBlkMap[(sel.first * 16) + sel.second];
 if (*p_cell == true)
 {
  *p_cell = false;
  ve_map.SetItemColor(sel.first, sel.second, 0);
  m_celblk_button.SetWindowText(MLL::LoadString(IDS_GME_INJ_CELBLK));
 }
 else
 {
  *p_cell = true;
  ve_map.SetItemColor(sel.first, sel.second, colorBlocked);
  m_celblk_button.SetWindowText(MLL::LoadString(IDS_GME_INJ_CELUNBLK));
 }

 if (GetBlkAllCheck())
 { //block/unblock all cells
  for (int l = 0; l < 16; ++l)
  {
   for (int r = 0; r < 16; ++r)
   {
    mp_CelBlkMap[(l * 16) + r] = *p_cell;
    ve_map.SetItemColor(l, r, *p_cell ? colorBlocked : 0);
   }
  }
  ve_map.UpdateDisplay(); //all cells
 }
 else
  ve_map.UpdateDisplay(sel.first, sel.second);
}

void CGMEInjVEDlg::OnSmoothButton()
{
 if (m_OnSmooth)
  m_OnSmooth();
}

void CGMEInjVEDlg::OnSelChangeVE(void)
{
 if (!mp_CelBlkMap)
  return;
 CMapEditorCtrl& ve_map = GetVECtrl();
 std::pair<int, int> sel = ve_map.GetSelection();
 bool* p_cell = &mp_CelBlkMap[(sel.first * 16) + sel.second];

 UpdateDialogControls(this, true);

 if (*p_cell == true)
  m_celblk_button.SetWindowText(MLL::LoadString(IDS_GME_INJ_CELUNBLK));
 else
  m_celblk_button.SetWindowText(MLL::LoadString(IDS_GME_INJ_CELBLK));
}

void CGMEInjVEDlg::BindLamDelMap(float* p_LamDelMap, float* p_rpmGridLD, float* p_loadGridLD)
{
 mp_LamDelMap = p_LamDelMap;
 mp_rpmGridLD = p_rpmGridLD;
 mp_loadGridLD = p_loadGridLD;
}

void CGMEInjVEDlg::BindCelWgtMap(float* p_CelWgtMap)
{
 mp_CelWgtMap = p_CelWgtMap;
}

void CGMEInjVEDlg::BindCelBlkMap(bool *p_CelBlkMap)
{
 mp_CelBlkMap = p_CelBlkMap;
}

void CGMEInjVEDlg::OnChangeLamDel(void)
{
 if (m_OnChangeLamDel)
  m_OnChangeLamDel();
}

void CGMEInjVEDlg::setOnStrStp(EventHandler OnCB)
{
 m_OnStrStp = OnCB;
}

void CGMEInjVEDlg::setOnRstStt(EventHandler OnCB)
{
 m_OnRstStt = OnCB;
}

void CGMEInjVEDlg::setIsReady(EventResult OnCB)
{
 m_IsReady = OnCB;
}

void CGMEInjVEDlg::setOnChangeLamDel(EventHandler OnCB)
{
 m_OnChangeLamDel = OnCB;
}

void CGMEInjVEDlg::setOnViewActivate(EventHandler OnCB)
{
 m_OnViewActivate = OnCB;
}

void CGMEInjVEDlg::setOnSmooth(EventHandler OnCB)
{
 m_OnSmooth = OnCB;
}

void CGMEInjVEDlg::SetStatusText(const _TSTRING& str)
{
 if (m_IsReady && (m_status_cache != str) && m_status_text.GetSafeHwnd())
 {
  m_status_text.SetWindowText(str.c_str());
  m_status_cache = str;
 }
}

void CGMEInjVEDlg::SetStrStpCaption(const _TSTRING& str)
{
 m_strstp_button.SetWindowText(str.c_str());
}

void CGMEInjVEDlg::UpdateCelWgtMapCell(int l, int r)
{
 if (m_celwgt_map.GetSafeHwnd())
  m_celwgt_map.UpdateDisplay(l, r);
}

bool CGMEInjVEDlg::GetBlkAllCheck(void)
{
 return m_blkall_check.GetCheck() == BST_CHECKED;
}

bool CGMEInjVEDlg::GetFinishCheck(void)
{
 return m_finish_check.GetCheck() == BST_CHECKED;
}

bool CGMEInjVEDlg::GetRstAllCheck(void)
{
 return m_rstall_check.GetCheck() == BST_CHECKED;
}

std::pair<int, int> CGMEInjVEDlg::GetVESelection(void)
{
 CMapEditorCtrl& ve_map = GetVECtrl();
 return ve_map.GetSelection();
}

void CGMEInjVEDlg::OnVE1Button()
{
 m_ve_map.ShowWindow(SW_SHOW); //show 1st map
 m_ve2_map.ShowWindow(SW_HIDE);
 m_ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_celwgt_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 UpdateBlockColor(m_ve_map);
 OnSelChangeVE(); //simulate event to update button
 if (m_ve_map.GetSafeHwnd())
  m_ve_map.UpdateDisplay(); 
 if (mp_CelWgtMap && m_celwgt_map.GetSafeHwnd())
  m_celwgt_map.UpdateDisplay();

 if (m_OnSelectVEMap)
  m_OnSelectVEMap(0); //notify controller about selection of 1st VE map

 m_active_ve = 0;

 if (m_on_change_sett)
  m_on_change_sett();
}

void CGMEInjVEDlg::OnVE2Button()
{
 m_ve_map.ShowWindow(SW_HIDE);
 m_ve2_map.ShowWindow(SW_SHOW); //show 2nd map
 m_ve2_map.AttachLabels(mp_rpmGrid, mp_loadGrid2);
 m_celwgt_map.AttachLabels(mp_rpmGrid, mp_loadGrid2);
 UpdateBlockColor(m_ve2_map);
 OnSelChangeVE(); //simulate event to update button
 if (m_ve2_map.GetSafeHwnd())
  m_ve2_map.UpdateDisplay(); 
 if (mp_CelWgtMap && m_celwgt_map.GetSafeHwnd())
  m_celwgt_map.UpdateDisplay(); 

 if (m_OnSelectVEMap)
  m_OnSelectVEMap(1); //notify controller about selection of 2nd VE map

 m_active_ve = 1;

 if (m_on_change_sett)
  m_on_change_sett();
}

CMapEditorCtrl& CGMEInjVEDlg::GetVECtrl(void)
{
 if (m_ve1_radio.GetCheck()==BST_CHECKED)
  return m_ve_map;
 else if (m_ve2_radio.GetCheck()==BST_CHECKED)
  return m_ve2_map;
 else
 {
  ASSERT(0);
  return m_ve_map;
 }
}

void CGMEInjVEDlg::UpdateBlockColor(CMapEditorCtrl& ve_map)
{
 if (mp_CelBlkMap)
 {
  for (int l = 0; l < 16; ++l)
   for (int r = 0; r < 16; ++r)
    ve_map.SetItemColor(l,r, mp_CelBlkMap[(l * 16) + r] ? colorBlocked : 0);
 }
}

void CGMEInjVEDlg::setOnSelectVEMap(EventWithCode OnCB)
{
 m_OnSelectVEMap = OnCB;
}

void CGMEInjVEDlg::SetActiveVEMap(int vemapid)
{
 m_active_ve = vemapid;
 if (!GetSafeHwnd())
  return;

 if (0==m_active_ve)
 { //VE1
  m_ve1_radio.SetCheck(BST_CHECKED);
  m_ve2_radio.SetCheck(BST_UNCHECKED);
  OnVE1Button();
 }
 else
 { //VE2
  m_ve1_radio.SetCheck(BST_UNCHECKED);
  m_ve2_radio.SetCheck(BST_CHECKED);
  OnVE2Button();
 }
}

int CGMEInjVEDlg::GetActiveVEMap(void) const
{
 return m_active_ve;
}

void CGMEInjVEDlg::setOnChangeSettings(EventHandler OnCB)
{
 m_on_change_sett = OnCB;
}

void CGMEInjVEDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  mp_cscl->Scale();
 }
}

void CGMEInjVEDlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}
