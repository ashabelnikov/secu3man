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

const UINT CGMEInjVEDlg::IDD = IDD_GME_INJ_VE;

/////////////////////////////////////////////////////////////////////////////
// CGMEInjVEDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjVEDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_VE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_LAMDEL_BTN,OnUpdateControlsAutoTune)
 ON_BN_CLICKED(IDC_GME_INJ_LAMDEL_BTN,OnLamDelButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_CELWGT_BTN,OnUpdateControlsAutoTune)
 ON_BN_CLICKED(IDC_GME_INJ_CELWGT_BTN,OnCelWgtButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_STRSTP_BTN,OnUpdateControlsAutoTune1)
 ON_BN_CLICKED(IDC_GME_INJ_STRSTP_BTN,OnStrStpButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_RSTSTT_BTN,OnUpdateControlsAutoTune)
 ON_BN_CLICKED(IDC_GME_INJ_RSTSTT_BTN,OnRstSttButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_AUTOTUNE_GROUPBOX, OnUpdateControlsAutoTune1)
 ON_BN_CLICKED(IDC_GME_INJ_CELBLK_BTN,OnCelBlkButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_CELBLK_BTN,OnUpdateControlsAutoTune2)
 ON_BN_CLICKED(IDC_GME_INJ_SMOOTH_BTN,OnSmoothButton)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_SMOOTH_BTN,OnUpdateControlsAutoTune1)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_BLKALL_CHECK,OnUpdateControlsAutoTune2)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_FINISH_CHECK,OnUpdateControlsAutoTune1)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_RSTALL_CHECK,OnUpdateControlsAutoTune)
END_MESSAGE_MAP()

CGMEInjVEDlg::CGMEInjVEDlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjVEDlg::IDD, pParent)
, m_ve_map(16, 16, true, true, NULL, 3) //inverted order of rows and vertical labels' data
, m_lamdel_map(3, 3, true, true, NULL, 3) //inverted order of rows and vertical labels' data
, m_celwgt_map(16, 16, false, true, NULL, 3, true) //normal order of rows, inverted order of vertical labels' data, read-only
, mp_VEMap(NULL)
, mp_rpmGrid(NULL)
, mp_loadGrid(NULL)
, mp_CelWgtMap(NULL)
, mp_LamDelMap(NULL)
, mp_rpmGridLD(NULL)
, mp_loadGridLD(NULL)
, mp_CelBlkMap(NULL)
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
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjVEDlg message handlers

BOOL CGMEInjVEDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
  CloneWndFont(this, &m_font, -1, false);

 m_ve_map.SetRange(.0f, 1.99f);
 m_ve_map.AttachMap(mp_VEMap);
 m_ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_ve_map.ShowLabels(true, true);
 m_ve_map.SetDecimalPlaces(3, 0, 0);
 m_ve_map.SetFont(&m_font);
 m_ve_map.EnableAbroadMove(false, false);
 m_ve_map.SetValueIncrement(0.01f);
 m_ve_map.setOnSelChange(fastdelegate::MakeDelegate(this, CGMEInjVEDlg::OnSelChangeVE));

 if (mp_CelBlkMap)
 {
  for (int l = 0; l < 16; ++l)
   for (int r = 0; r < 16; ++r)
    m_ve_map.SetItemColor(l,r, mp_CelBlkMap[(l * 16) + r] ? RGB(100,100,100) : 0);
 }

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
  m_celwgt_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
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
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

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
 pCmdUI->Enable(m_IsReady && IsWindowEnabled() && (m_ve_map.GetSelection().first != -1));
}

LPCTSTR CGMEInjVEDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGMEInjVEDlg::BindMaps(float* pVE)
{
 ASSERT(pVE);
 mp_VEMap = pVE;
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
 if (updateLabels)
 {
  m_ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  m_celwgt_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  if (m_ve_map.GetSafeHwnd())
   m_ve_map.UpdateDisplay(); 
  if (mp_CelWgtMap && m_celwgt_map.GetSafeHwnd())
   m_celwgt_map.UpdateDisplay(); 
 }
}

void CGMEInjVEDlg::setOnChange(EventHandler OnCB)
{
 m_ve_map.setOnChange(OnCB);
}

void CGMEInjVEDlg::UpdateView(bool axisLabels /*= false*/)
{
 if (mp_CelBlkMap)
 {
  for (int l = 0; l < 16; ++l)
   for (int r = 0; r < 16; ++r)
    m_ve_map.SetItemColor(l, r, mp_CelBlkMap[(l * 16) + r] ? RGB(100, 100, 100) : 0);
 }

 if (axisLabels)
  m_ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid); //update axis labels

 m_ve_map.UpdateDisplay();
}

void CGMEInjVEDlg::SetArguments(int rpm, int air_flow, bool strt_use, float load)
{
 if (m_ve_map.GetSafeHwnd() && m_ve_map.IsWindowVisible())
 {
  m_ve_map.ShowMarkers(!strt_use, true);
  m_ve_map.SetArguments(load, (float)rpm);
 }
 if (m_lamdel_map.GetSafeHwnd() && m_lamdel_map.IsWindowVisible())
 {
  m_lamdel_map.ShowMarkers(true, true);
  m_lamdel_map.SetArguments(load, (float)rpm);
 }
 if (m_celwgt_map.GetSafeHwnd() && m_celwgt_map.IsWindowVisible())
 {
  m_celwgt_map.ShowMarkers(true, true);
  m_celwgt_map.SetArguments(load, (float)rpm);
 }
}

void CGMEInjVEDlg::OnLamDelButton()
{
 bool checked = m_lamdel_button.GetCheck() == BST_CHECKED;
 if (checked)
  m_celwgt_button.SetCheck(BST_UNCHECKED);

 m_ve_map.ShowWindow(checked ? SW_HIDE : SW_SHOW);
 m_lamdel_map.ShowWindow(checked ? SW_SHOW : SW_HIDE);
 m_celwgt_map.ShowWindow(SW_HIDE);
}

void CGMEInjVEDlg::OnCelWgtButton()
{
 bool checked = m_celwgt_button.GetCheck() == BST_CHECKED;
 if (checked)
  m_lamdel_button.SetCheck(BST_UNCHECKED);

 m_ve_map.ShowWindow(checked ? SW_HIDE : SW_SHOW);
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
 std::pair<int, int> sel = m_ve_map.GetSelection();
 bool* p_cell = &mp_CelBlkMap[(sel.first * 16) + sel.second];
 if (*p_cell == true)
 {
  *p_cell = false;
  m_ve_map.SetItemColor(sel.first, sel.second, 0);
  m_celblk_button.SetWindowText(MLL::LoadString(IDS_GME_INJ_CELBLK));
 }
 else
 {
  *p_cell = true;
  m_ve_map.SetItemColor(sel.first, sel.second, RGB(100,100,100));
  m_celblk_button.SetWindowText(MLL::LoadString(IDS_GME_INJ_CELUNBLK));
 }

 if (GetBlkAllCheck())
 { //block/unblock all cells
  for (int l = 0; l < 16; ++l)
  {
   for (int r = 0; r < 16; ++r)
   {
    mp_CelBlkMap[(l * 16) + r] = *p_cell;
    m_ve_map.SetItemColor(l, r, *p_cell ? RGB(100,100,100) : 0);
   }
  }
  m_ve_map.UpdateDisplay(); //all cells
 }
 else
  m_ve_map.UpdateDisplay(sel.first, sel.second);
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
 std::pair<int, int> sel = m_ve_map.GetSelection();
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
 return m_ve_map.GetSelection();
}
