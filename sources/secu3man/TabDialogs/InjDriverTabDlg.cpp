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

/** \file InjDriverTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "InjDriverTabDlg.h"

#include "common/DPIAware.h"
#include "common/FastDelegate.h"
#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
#include "ui-core/Chart2D.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/ddx_helpers.h"
//#include "io-core/BitMask.h"

const UINT CInjDriverTabDlg::IDD = IDD_INJ_DRIVER;

//Chart series
#define SERIE_PO 0
#define SERIE_PD 1
#define SERIE_HD 2
#define SERIE_VT 3

const int x_axis_points = SECU3IO::LUTABSIZE;
const float x_axis_min = 5.4f;
const float x_axis_step = 0.8f;
const float x_axis_max = x_axis_min + (x_axis_step * (x_axis_points-1));

const float arrmv_offset[3] = {100.0f, 1.0f, 1.0f};

CInjDriverTabDlg::CInjDriverTabDlg(CWnd* pParent /*=NULL*/)
: Super(CInjDriverTabDlg::IDD, pParent)
, mp_chart(new CChart2D())
, m_initialized(false)
, m_enable(false)
, m_enable_ee_save(false)
, mp_curr_curve(NULL)
, m_set_of_sett_idx(0)
, m_optPeakOnPtMovStep(1.0f)
, m_optPeakDutyPtMovStep(0.1f)
, m_optHoldDutyPtMovStep(0.1f)
, m_y_axis_min(100.0f)
, m_y_axis_max(10000.0f)
, m_pwm_period_edit(CEditEx::MODE_FLOAT, true)
, m_peak_duty_edit(CEditEx::MODE_FLOAT, true)
, m_hold_duty_edit(CEditEx::MODE_FLOAT, true)
, m_peak_on_time_edit(CEditEx::MODE_FLOAT, true)
, m_peak_pwm_time_edit(CEditEx::MODE_FLOAT, true)
, m_pth_pause_edit(CEditEx::MODE_FLOAT, true)
, m_curve_idx(SERIE_PO)
, m_enable_voltage(false)
, m_offline(false)
{

 mp_chart->AddSerie(16);       //peak on serie (see also defines)
 mp_chart->AddSerie(16);       //peak duty serie
 mp_chart->AddSerie(16);       //hold duty serie
 mp_chart->AddSerie(2);        //voltage serie (vert. line)

 for (int i = 0; i < 2; ++i)
 {
  m_params[i].m_pwm_period = .0f;
  m_params[i].m_peak_duty = .0f;
  m_params[i].m_hold_duty = .0f;
  m_params[i].m_peak_on_time = .0f;
  m_params[i].m_peak_pwm_time = .0f;
  m_params[i].m_pth_pause = .0f;
  m_params[i].m_tst_peak_pwm = false;
  m_params[i].m_tst_hold_pwm = false;
  std::fill(m_params[i].m_peak_on_tab, m_params[i].m_peak_on_tab + SECU3IO::LUTABSIZE, .0f);
  std::fill(m_params[i].m_peak_duty_tab, m_params[i].m_peak_duty_tab + SECU3IO::LUTABSIZE, .0f);
  std::fill(m_params[i].m_hold_duty_tab, m_params[i].m_hold_duty_tab + SECU3IO::LUTABSIZE, .0f);
  m_params[i].m_peak_on_usetab = false;
  m_params[i].m_peak_duty_usetab = false;
  m_params[i].m_hold_duty_usetab = false;
  m_params[i].m_tst_peak_pwm = false;
  m_params[i].m_tst_hold_pwm = false;
 }
 mp_curr_curve = m_params[m_set_of_sett_idx].m_peak_on_tab;
}

void CInjDriverTabDlg::DoDataExchange(CDataExchange* pDX)
{
 DDX_CBIndex(pDX, IDC_SET_OF_SETT_COMBO, m_set_of_sett_idx); //must be first!

 DDX_Control(pDX, IDC_PWM_PERIOD_EDIT, m_pwm_period_edit);
 DDX_Control(pDX, IDC_PWM_PERIOD_SPIN, m_pwm_period_spin);

 DDX_Control(pDX, IDC_PEAK_DUTY_EDIT, m_peak_duty_edit);
 DDX_Control(pDX, IDC_PEAK_DUTY_SPIN, m_peak_duty_spin);

 DDX_Control(pDX, IDC_HOLD_DUTY_EDIT, m_hold_duty_edit);
 DDX_Control(pDX, IDC_HOLD_DUTY_SPIN, m_hold_duty_spin);

 DDX_Control(pDX, IDC_PEAK_ON_TIME_EDIT, m_peak_on_time_edit);
 DDX_Control(pDX, IDC_PEAK_ON_TIME_SPIN, m_peak_on_time_spin);

 DDX_Control(pDX, IDC_PEAK_PWM_TIME_EDIT, m_peak_pwm_time_edit);
 DDX_Control(pDX, IDC_PEAK_PWM_TIME_SPIN, m_peak_pwm_time_spin);

 DDX_Control(pDX, IDC_PTH_PAUSE_EDIT, m_pth_pause_edit);
 DDX_Control(pDX, IDC_PTH_PAUSE_SPIN, m_pth_pause_spin);

 DDX_Control(pDX, IDC_EEPROM_SAVE_BUTTON, m_eeprom_save_btn);

 DDX_Control(pDX, IDC_VOLTAGE_PANE, m_voltage_pane);

 DDX_Control(pDX, IDC_PEAK_ON_TABSEL_CHECK, m_peak_on_tabsel_check);
 DDX_Control(pDX, IDC_PEAK_DUTY_TABSEL_CHECK, m_peak_duty_tabsel_check);
 DDX_Control(pDX, IDC_HOLD_DUTY_TABSEL_CHECK, m_hold_duty_tabsel_check);

 DDX_Control(pDX, IDC_PEAK_ON_CHECK, m_peak_on_check);
 DDX_Control(pDX, IDC_PEAK_DUTY_CHECK, m_peak_duty_check);
 DDX_Control(pDX, IDC_HOLD_DUTY_CHECK, m_hold_duty_check);

 DDX_Control(pDX, IDC_CURVE_UP_BUTTON, m_up_arrow);
 DDX_Control(pDX, IDC_CURVE_DOWN_BUTTON, m_down_arrow);

 DDX_Control(pDX, IDC_TST_PEAK_PWM_CHECK, m_tst_peak_pwm_check);
 DDX_Control(pDX, IDC_TST_HOLD_PWM_CHECK, m_tst_hold_pwm_check);

 DDX_Control(pDX, IDC_SET_OF_SETT_COMBO, m_set_of_sett_combo);

 m_pwm_period_edit.DDX_Value(pDX, IDC_PWM_PERIOD_EDIT, m_params[m_set_of_sett_idx].m_pwm_period);
 m_peak_duty_edit.DDX_Value(pDX, IDC_PEAK_DUTY_EDIT, m_params[m_set_of_sett_idx].m_peak_duty);
 m_hold_duty_edit.DDX_Value(pDX, IDC_HOLD_DUTY_EDIT, m_params[m_set_of_sett_idx].m_hold_duty);
 m_peak_on_time_edit.DDX_Value(pDX, IDC_PEAK_ON_TIME_EDIT, m_params[m_set_of_sett_idx].m_peak_on_time);
 m_peak_pwm_time_edit.DDX_Value(pDX, IDC_PEAK_PWM_TIME_EDIT, m_params[m_set_of_sett_idx].m_peak_pwm_time);
 m_pth_pause_edit.DDX_Value(pDX, IDC_PTH_PAUSE_EDIT, m_params[m_set_of_sett_idx].m_pth_pause);

 DDX_Check_bool(pDX, IDC_PEAK_ON_CHECK, m_params[m_set_of_sett_idx].m_peak_on_usetab);
 DDX_Check_bool(pDX, IDC_PEAK_DUTY_CHECK, m_params[m_set_of_sett_idx].m_peak_duty_usetab);
 DDX_Check_bool(pDX, IDC_HOLD_DUTY_CHECK, m_params[m_set_of_sett_idx].m_hold_duty_usetab);

 DDX_Check_bool(pDX, IDC_TST_PEAK_PWM_CHECK, m_params[m_set_of_sett_idx].m_tst_peak_pwm);
 DDX_Check_bool(pDX, IDC_TST_HOLD_PWM_CHECK, m_params[m_set_of_sett_idx].m_tst_hold_pwm);

 Super::DoDataExchange(pDX);
}

LPCTSTR CInjDriverTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BEGIN_MESSAGE_MAP(CInjDriverTabDlg, Super)
 ON_WM_DESTROY()
 ON_WM_INITMENUPOPUP()
 ON_WM_CONTEXTMENU()
 ON_WM_SIZE()

 ON_BN_CLICKED(IDC_EEPROM_SAVE_BUTTON, OnEepromSaveButton)
 ON_BN_CLICKED(IDC_PEAK_ON_TABSEL_CHECK, OnPeakOnTabSelCheck)
 ON_BN_CLICKED(IDC_PEAK_DUTY_TABSEL_CHECK, OnPeakDutyTabSelCheck)
 ON_BN_CLICKED(IDC_HOLD_DUTY_TABSEL_CHECK, OnHoldDutyTabSelCheck)
 ON_BN_CLICKED(IDC_CURVE_UP_BUTTON, OnUpArrowButton)
 ON_BN_CLICKED(IDC_CURVE_DOWN_BUTTON, OnDownArrowButton)
 ON_BN_CLICKED(IDC_PEAK_ON_CHECK, OnChangeDataChecks)
 ON_BN_CLICKED(IDC_PEAK_DUTY_CHECK, OnChangeDataChecks)
 ON_BN_CLICKED(IDC_HOLD_DUTY_CHECK, OnChangeDataChecks)
 ON_BN_CLICKED(IDC_TST_PEAK_PWM_CHECK, OnChangeDataTstPeakPwm)
 ON_BN_CLICKED(IDC_TST_HOLD_PWM_CHECK, OnChangeDataTstHoldPwm)

 ON_CBN_SELENDOK(IDC_SET_OF_SETT_COMBO, OnSelendokSetOfSett)

 ON_EN_CHANGE(IDC_PWM_PERIOD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PEAK_DUTY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_HOLD_DUTY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PEAK_ON_TIME_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PEAK_PWM_TIME_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PTH_PAUSE_EDIT, OnChangeData)

 ON_COMMAND(ID_INJDRV_POPUP_EXPORTTOAFILE, OnExportToAFile)
 ON_COMMAND(ID_INJDRV_POPUP_IMPORTFROMAFILE, OnImportFromAFile)
 ON_COMMAND(ID_INJDRV_POPUP_SAVETOFIRMWARE, OnSaveToFirmware)
 ON_COMMAND(ID_INJDRV_POPUP_LOADFROMFIRMWARE, OnLoadFromFirmware)
 ON_COMMAND(ID_INJDRV_POPUP_SHOWFIRMWAREINFO, OnShowFirmwareInfo)
 ON_COMMAND(ID_INJDRV_POPUP_OFFLINEMODE, OnOfflineModeExit)

 ON_UPDATE_COMMAND_UI(IDC_PEAK_DUTY_EDIT,OnUpdateControlsPD)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_DUTY_SPIN,OnUpdateControlsPD)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_DUTY_CAPTION,OnUpdateControlsPD)

 ON_UPDATE_COMMAND_UI(IDC_HOLD_DUTY_EDIT,OnUpdateControlsHD)
 ON_UPDATE_COMMAND_UI(IDC_HOLD_DUTY_SPIN,OnUpdateControlsHD)
 ON_UPDATE_COMMAND_UI(IDC_HOLD_DUTY_CAPTION,OnUpdateControlsHD)

 ON_UPDATE_COMMAND_UI(IDC_PEAK_ON_TIME_EDIT,OnUpdateControlsPO)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_ON_TIME_SPIN,OnUpdateControlsPO)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_ON_TIME_CAPTION,OnUpdateControlsPO)

 ON_UPDATE_COMMAND_UI(IDC_CURVE_UP_BUTTON, OnUpdateControlsArrowBtns)
 ON_UPDATE_COMMAND_UI(IDC_CURVE_DOWN_BUTTON, OnUpdateControlsArrowBtns)

 ON_UPDATE_COMMAND_UI(IDC_PWM_PERIOD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PWM_PERIOD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PWM_PERIOD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_PWM_TIME_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_PWM_TIME_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_PWM_TIME_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PTH_PAUSE_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PTH_PAUSE_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PTH_PAUSE_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_VCURVES_CHART, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_ON_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_DUTY_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_HOLD_DUTY_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TST_PEAK_PWM_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TST_HOLD_PWM_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_SET_OF_SETT_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_SET_OF_SETT_CAPTION, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_VOLTAGE_PANE,OnUpdateControlsVoltage)
 ON_UPDATE_COMMAND_UI(IDC_VOLTAGE_CAPTION,OnUpdateControlsVoltage)

 ON_UPDATE_COMMAND_UI(IDC_EEPROM_SAVE_BUTTON,OnUpdateControlsEESave)

 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_EXPORTTOAFILE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_IMPORTFROMAFILE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_SAVETOFIRMWARE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_SHOWFIRMWAREINFO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_OFFLINEMODE, OnUpdateOfflineModeExit)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInjDriverTabDlg message handlers

BOOL CInjDriverTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //init edit boxes and spin buttons
 m_pwm_period_spin.SetBuddy(&m_pwm_period_edit);
 m_pwm_period_edit.SetLimitText(6);
 m_pwm_period_edit.SetDecimalPlaces(1);
 m_pwm_period_spin.SetRangeAndDelta(20.0f, 3000.0f, 0.1f);
 m_pwm_period_edit.SetRange(20.0f, 9999.0f);

 m_peak_duty_spin.SetBuddy(&m_peak_duty_edit);
 m_peak_duty_edit.SetLimitText(5);
 m_peak_duty_edit.SetDecimalPlaces(1);
 m_peak_duty_spin.SetRangeAndDelta(0.0f, 100.0f, 0.1f);
 m_peak_duty_edit.SetRange(0.0f, 100.0f);

 m_hold_duty_spin.SetBuddy(&m_hold_duty_edit);
 m_hold_duty_edit.SetLimitText(5);
 m_hold_duty_edit.SetDecimalPlaces(1);
 m_hold_duty_spin.SetRangeAndDelta(0.0f, 100.0f, 0.1f);
 m_hold_duty_edit.SetRange(0.0f, 100.0f);

 m_peak_on_time_spin.SetBuddy(&m_peak_on_time_edit);
 m_peak_on_time_edit.SetLimitText(4);
 m_peak_on_time_edit.SetDecimalPlaces(0);
 m_peak_on_time_spin.SetRangeAndDelta(100.0f, 9999.0f, 1.0f);
 m_peak_on_time_edit.SetRange(100.0f, 9999.0f);

 m_peak_pwm_time_spin.SetBuddy(&m_peak_pwm_time_edit);
 m_peak_pwm_time_edit.SetLimitText(4);
 m_peak_pwm_time_edit.SetDecimalPlaces(0);
 m_peak_pwm_time_spin.SetRangeAndDelta(100.0f, 9999.0f, 1.0f);
 m_peak_pwm_time_edit.SetRange(100.0f, 9999.0f);

 m_pth_pause_spin.SetBuddy(&m_pth_pause_edit);
 m_pth_pause_edit.SetLimitText(4);
 m_pth_pause_edit.SetDecimalPlaces(0);
 m_pth_pause_spin.SetRangeAndDelta(1.0f, 1000.0f, 1.0f);
 m_pth_pause_edit.SetRange(1.0f, 1000.0f);

 //Init "set of settings" combo
 m_set_of_sett_combo.AddString(MLL::LoadString(IDS_INJDRV_PETROL_SET));
 m_set_of_sett_combo.AddString(MLL::LoadString(IDS_INJDRV_GAS_SET));

 //init chart
 CRect rect;
 GetDlgItem(IDC_VCURVES_CHART_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_chart->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, rect, this, IDC_VCURVES_CHART);
 mp_chart->SetOnChange(fastdelegate::MakeDelegate(this, &CInjDriverTabDlg::OnChangeChart));
 mp_chart->SetWindowPos(GetDlgItem(IDC_CURVE_DOWN_BUTTON), 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

 mp_chart->SetXAxisValueFormat(_T("%.01f"));
 mp_chart->SetYAxisValueFormat(_T("%.0f")); //integer
 mp_chart->SetChartTitle(MLL::LoadString(IDS_PEAK_ON_TIME_TITLE));
 mp_chart->SetGridXYNumber(x_axis_points-1, 9);
 mp_chart->SetRange(x_axis_min, x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetBackColor(RGB(180, 200, 180));
 mp_chart->SetSerieColor(SERIE_PO, RGB(80, 80, 200));
 mp_chart->SetSerieColor(SERIE_PD, RGB(80, 80, 200));
 mp_chart->SetSerieColor(SERIE_HD, RGB(80, 80, 200));
 mp_chart->SetSerieHandle(SERIE_PO, true);
 mp_chart->SetSerieHandle(SERIE_PD, true);
 mp_chart->SetSerieHandle(SERIE_HD, true);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetPtMovStep(static_cast<double>(m_optPeakOnPtMovStep));

 mp_chart->SetXYValue(SERIE_VT, x_axis_min, m_y_axis_min, 0);
 mp_chart->SetXYValue(SERIE_VT, x_axis_min, m_y_axis_max, 1);

 UpdateChartValues();
 m_peak_on_tabsel_check.SetCheck(BST_CHECKED);

 //arrow buttons
 HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_UP_ARROW), IMAGE_ICON, 0,0, LR_DEFAULTSIZE);
 m_up_arrow.SetIcon(hIcon);
 hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_DOWN_ARROW), IMAGE_ICON, 0,0, LR_DEFAULTSIZE);
 m_down_arrow.SetIcon(hIcon);

 //init tool tips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 VERIFY(mp_ttc->AddWindow(&m_peak_on_check, MLL::GetString(IDS_PEAK_ON_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_duty_check, MLL::GetString(IDS_PEAK_DUTY_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_hold_duty_check, MLL::GetString(IDS_HOLD_DUTY_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tst_peak_pwm_check, MLL::GetString(IDS_TST_PEAK_PWM_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tst_hold_pwm_check, MLL::GetString(IDS_TST_HOLD_PWM_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_set_of_sett_combo, MLL::GetString(IDS_SET_OF_SETT_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_eeprom_save_btn, MLL::GetString(IDS_EEPROM_SAVE_BUTTON_TT)));

 mp_ttc->SetMaxTipWidth(120); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 m_initialized = true;

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CInjDriverTabDlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
 m_enable = false;
}

void CInjDriverTabDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 if (bSysMenu)
  return;     // don't support system menu

 ASSERT(pMenu != NULL);
 // check the enabled state of various menu items

 CCmdUI state;
 state.m_pMenu = pMenu;
 ASSERT(state.m_pOther == NULL);
 ASSERT(state.m_pParentMenu == NULL);

 // determine if menu is popup in top-level menu and set m_pOther to
 //  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
 HMENU hParentMenu;
 if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
  state.m_pParentMenu = pMenu;    // parent == child for tracking popup
 else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
 {
  CWnd* pParent = GetTopLevelParent();
  // child windows don't have menus -- need to go to the top!
  if (pParent != NULL && (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
  {
   int nIndexMax = ::GetMenuItemCount(hParentMenu);
   for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
   {
    if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
    {
     // when popup is found, m_pParentMenu is containing menu
     state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
     break;
    }
   }
  }
 }

 state.m_nIndexMax = pMenu->GetMenuItemCount();
 for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
 {
  state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
  if (state.m_nID == 0)
   continue; // menu separator or invalid cmd - ignore it

  ASSERT(state.m_pOther == NULL);
  ASSERT(state.m_pMenu != NULL);
  if (state.m_nID == (UINT)-1)
  {
   // possibly a popup menu, route to first item of that popup
   state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
   if (state.m_pSubMenu == NULL || (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 || state.m_nID == (UINT)-1)
    continue;       // first item of popup can't be routed to
   state.DoUpdate(this, FALSE);    // popups are never auto disabled
  }
  else
  {
   // normal menu item
   // Auto enable/disable if frame window has 'm_bAutoMenuEnable'
   // set and command is _not_ a system command.
   state.m_pSubMenu = NULL;
   state.DoUpdate(this, TRUE);
  }

  // adjust for menu deletions and additions
  UINT nCount = pMenu->GetMenuItemCount();
  if (nCount < state.m_nIndexMax)
  {
   state.m_nIndex -= (state.m_nIndexMax - nCount);
   while (state.m_nIndex < nCount && pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
   {
    state.m_nIndex++;
   }
  }
  state.m_nIndexMax = nCount;
 }//for
}

void CInjDriverTabDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 CMenu menu;
 VERIFY(menu.LoadMenu(IDR_INJDRV_POPUP_MENU));
 CMenu *pSub = menu.GetSubMenu(0);
 pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CInjDriverTabDlg::OnSize( UINT nType, int cx, int cy )
{
 if (m_initialized)
 {
  CRect rc1, rc2;
  DPIAware da;

  rc1 = GDIHelpers::GetChildWndRect(&m_up_arrow);
  GetClientRect(&rc2);
  m_up_arrow.SetWindowPos(NULL, rc2.right - rc1.Width() - da.ScaleX(3), rc1.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(&m_down_arrow);
  GetClientRect(&rc2);
  int buttonPos = rc2.right - rc1.Width() - da.ScaleX(3);
  m_down_arrow.SetWindowPos(NULL, buttonPos, rc1.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(mp_chart.get());
  mp_chart->SetWindowPos(NULL, 0, 0, buttonPos - rc1.left - da.ScaleX(3) , rc2.bottom - rc1.top - da.ScaleY(5), SWP_NOMOVE | SWP_NOZORDER);
 }

 Super::OnSize(nType, cx, cy);
}

void CInjDriverTabDlg::SetVoltLineColor(DWORD color)
{
 mp_chart->SetSerieColor(SERIE_VT, color);
}

void CInjDriverTabDlg::SetPtMovStep(float peakOn, float peakDuty, float holdDuty)
{
 m_optPeakOnPtMovStep = peakOn;
 m_optPeakDutyPtMovStep = peakDuty;
 m_optHoldDutyPtMovStep = holdDuty;
 //update chart
 if (m_peak_on_tabsel_check.GetCheck() == BST_CHECKED)  
  mp_chart->SetPtMovStep(static_cast<double>(m_optPeakOnPtMovStep));
 else if (m_peak_duty_tabsel_check.GetCheck() == BST_CHECKED)
  mp_chart->SetPtMovStep(static_cast<double>(m_optPeakDutyPtMovStep));
 else if (m_hold_duty_tabsel_check.GetCheck() == BST_CHECKED)
  mp_chart->SetPtMovStep(static_cast<double>(m_optHoldDutyPtMovStep));
}

void CInjDriverTabDlg::OnChangeChart(int serieIdx)
{
 const double *x = mp_chart->GetYValues(serieIdx);
 if (serieIdx == SERIE_PO)
 {
  for(int i = 0; i < x_axis_points; ++i)
   m_params[m_set_of_sett_idx].m_peak_on_tab[i] = (float)x[i];
 }
 else if (serieIdx == SERIE_PD)
 {
  for(int i = 0; i < x_axis_points; ++i)
   m_params[m_set_of_sett_idx].m_peak_duty_tab[i] = (float)x[i]; 
 }
 else if (serieIdx == SERIE_HD)
 {
  for(int i = 0; i < x_axis_points; ++i)
   m_params[m_set_of_sett_idx].m_hold_duty_tab[i] = (float)x[i];
 }
 else
  return; //error

 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::UpdateChartValues(void)
{
 for (int i = 0; i < x_axis_points; ++i)
 {
  mp_chart->SetXYValue(SERIE_PO, x_axis_min + (x_axis_step * i), m_params[m_set_of_sett_idx].m_peak_on_tab[i], i);
  mp_chart->SetXYValue(SERIE_PD, x_axis_min + (x_axis_step * i), m_params[m_set_of_sett_idx].m_peak_duty_tab[i], i);
  mp_chart->SetXYValue(SERIE_HD, x_axis_min + (x_axis_step * i), m_params[m_set_of_sett_idx].m_hold_duty_tab[i], i);
 }
}

void CInjDriverTabDlg::OnEepromSaveButton()
{
 if (m_onSaveButton)
  m_onSaveButton();
}

void CInjDriverTabDlg::OnPeakOnTabSelCheck()
{
 m_peak_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_hold_duty_tabsel_check.SetCheck(BST_UNCHECKED);

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, true);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_PEAK_ON_TIME_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.0f")); //integer
 mp_chart->SetPtMovStep(static_cast<double>(m_optPeakOnPtMovStep));

 m_y_axis_min = 100.0f;
 m_y_axis_max = 10000.0f;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_peak_on_tab;
 m_curve_idx = SERIE_PO;
 mp_chart->SetRange(x_axis_min, x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points-1, 9);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnPeakDutyTabSelCheck()
{
 m_peak_on_tabsel_check.SetCheck(BST_UNCHECKED);
 m_hold_duty_tabsel_check.SetCheck(BST_UNCHECKED);

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, false);
 mp_chart->SetSerieVisibility(SERIE_PD, true);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_PEAK_DUTY_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.01f")); //fractional
 mp_chart->SetPtMovStep(static_cast<double>(m_optPeakDutyPtMovStep));

 m_y_axis_min = 0.0f;
 m_y_axis_max = 100.0f;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_peak_duty_tab;
 m_curve_idx = SERIE_PD;
 mp_chart->SetRange(x_axis_min, x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points-1, 10);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnHoldDutyTabSelCheck()
{
 m_peak_on_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_duty_tabsel_check.SetCheck(BST_UNCHECKED);

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, false);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, true);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_HOLD_DUTY_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.01f")); //fractional
 mp_chart->SetPtMovStep(static_cast<double>(m_optHoldDutyPtMovStep));

 m_y_axis_min = 0.0f;
 m_y_axis_max = 100.0f;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_hold_duty_tab;
 m_curve_idx = SERIE_HD;
 mp_chart->SetRange(x_axis_min, x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points-1, 10);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnUpArrowButton()
{
 if (!mp_curr_curve)
  return;
 for (int i = 0; i < x_axis_points; ++i)
 {
  mp_curr_curve[i]+=arrmv_offset[m_curve_idx];
  if (mp_curr_curve[i] > m_y_axis_max)
   mp_curr_curve[i] = m_y_axis_max;
  mp_chart->SetXYValue(m_curve_idx, x_axis_min + (x_axis_step * i), mp_curr_curve[i], i);
 }

 mp_chart->Invalidate();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnDownArrowButton()
{
 if (!mp_curr_curve)
  return;
 for (int i = 0; i < x_axis_points; ++i)
 {
  mp_curr_curve[i]-=arrmv_offset[m_curve_idx];
  if (mp_curr_curve[i] < m_y_axis_min)
   mp_curr_curve[i] = m_y_axis_min;
  mp_chart->SetXYValue(m_curve_idx, x_axis_min + (x_axis_step * i), mp_curr_curve[i], i);
 }

 mp_chart->Invalidate();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnSelendokSetOfSett()
{
 m_set_of_sett_idx = m_set_of_sett_combo.GetCurSel();

 if (m_curve_idx == SERIE_PO)
  OnPeakOnTabSelCheck();
 else if (m_curve_idx == SERIE_PD)
  OnPeakDutyTabSelCheck();
 else if (m_curve_idx == SERIE_HD)
  OnHoldDutyTabSelCheck();

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::setOnChange(EventHandler onCB)
{
 m_onChange = onCB;
}

void CInjDriverTabDlg::setOnSaveButton(EventHandler onCB)
{
 m_onSaveButton = onCB;
}

void CInjDriverTabDlg::setOnExportToAFile(EventHandler onCB)
{
 m_onExportToAFile = onCB;
}

void CInjDriverTabDlg::setOnImportFromAFile(EventHandler onCB)
{
 m_onImportFromAFile = onCB;
}

void CInjDriverTabDlg::setOnSaveToFirmware(EventHandler onCB)
{
 m_onSaveToFirmware = onCB;
}

void CInjDriverTabDlg::setOnLoadFromFirmware(EventHandler onCB)
{
 m_onLoadFromFirmware = onCB;
}

void CInjDriverTabDlg::setOnShowFirmwareInfo(EventHandler onCB)
{
 m_onShowFirmwareInfo = onCB;
}

void CInjDriverTabDlg::setOnExitOfflineMode(EventHandler onCB)
{
 m_onExitOfflineMode = onCB;
}

void CInjDriverTabDlg::SetValues(SECU3IO::InjDrvPar* ip_data, bool voltage_only /*= false*/)
{
 if (voltage_only)
 {
  CString str;
  str.Format("%5.2f", ip_data->voltage);
  m_voltage_pane.SetWindowText(str);
  SetChartVoltageValue(ip_data->voltage);
  mp_chart->Invalidate(TRUE);
  return;
 }
 int set_idx = ip_data->set_idx;
 m_params[set_idx] = *ip_data; //copy

 CString str;
 str.Format("%5.2f", ip_data->voltage);
 m_voltage_pane.SetWindowText(str);
 SetChartVoltageValue(ip_data->voltage);

 if (set_idx == m_set_of_sett_idx)
 {
  UpdateChartValues();
  mp_chart->Invalidate(TRUE);
  UpdateData(FALSE);
 }
}

void CInjDriverTabDlg::GetValues(SECU3IO::InjDrvPar* op_data)
{
 UpdateData(TRUE);
 int set_idx = op_data->set_idx;

 *op_data = m_params[set_idx]; //copy
}

void CInjDriverTabDlg::SetChartVoltageValue(float voltage)
{
 if (voltage < x_axis_min)
  voltage = x_axis_min;
 if (voltage > x_axis_max)
  voltage = x_axis_max;

 mp_chart->SetXYValue(SERIE_VT, voltage, m_y_axis_min, 0);
 mp_chart->SetXYValue(SERIE_VT, voltage, m_y_axis_max, 1);
}

void CInjDriverTabDlg::OnChangeData()
{
 UpdateData();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnChangeDataChecks()
{
 OnChangeData();
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::OnChangeDataTstPeakPwm()
{
 if (m_tst_peak_pwm_check.GetCheck()==BST_CHECKED)
  m_tst_hold_pwm_check.SetCheck(BST_UNCHECKED);

 UpdateData();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnChangeDataTstHoldPwm()
{
 if (m_tst_hold_pwm_check.GetCheck()==BST_CHECKED)
  m_tst_peak_pwm_check.SetCheck(BST_UNCHECKED);

 UpdateData();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnExportToAFile()
{
 if (m_onExportToAFile)
  m_onExportToAFile();
}

void CInjDriverTabDlg::OnImportFromAFile()
{
 if (m_onImportFromAFile)
  m_onImportFromAFile();
}

void CInjDriverTabDlg::OnSaveToFirmware()
{
 if (m_onSaveToFirmware)
  m_onSaveToFirmware();
}

void CInjDriverTabDlg::OnLoadFromFirmware()
{
 if (m_onLoadFromFirmware)
  m_onLoadFromFirmware();
}

void CInjDriverTabDlg::OnShowFirmwareInfo()
{
 if (m_onShowFirmwareInfo)
  m_onShowFirmwareInfo();
}

void CInjDriverTabDlg::OnUpdateControlsPO(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_peak_on_check.GetCheck() == BST_UNCHECKED);
}

void CInjDriverTabDlg::OnUpdateControlsPD(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_peak_duty_check.GetCheck() == BST_UNCHECKED);
}

void CInjDriverTabDlg::OnUpdateControlsHD(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_hold_duty_check.GetCheck() == BST_UNCHECKED);
}

void CInjDriverTabDlg::OnUpdateControlsEESave(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_enable_ee_save && m_initialized);
}

void CInjDriverTabDlg::OnUpdateControlsArrowBtns(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && mp_curr_curve);
}

void CInjDriverTabDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_initialized);
}

void CInjDriverTabDlg::OnUpdateControlsVoltage(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_initialized && m_enable_voltage);
}

void CInjDriverTabDlg::OnUpdateOfflineModeExit(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_offline);
 pCmdUI->SetCheck(m_offline);
}

void CInjDriverTabDlg::EnableAll(bool enable)
{
 m_enable = enable;
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::EnableVoltage(bool enable)
{
 m_enable_voltage = enable;
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::EnableSaveBtn(bool enable)
{
 m_enable_ee_save = enable;
 UpdateDialogControls(this, TRUE);
}

int CInjDriverTabDlg::GetCurrSetIdx(void)
{
 return m_set_of_sett_idx;
}

void CInjDriverTabDlg::OnOfflineModeExit()
{
 m_offline = false;
 UpdateDialogControls(this, TRUE);
 if (m_onExitOfflineMode)
  m_onExitOfflineMode();
}

void CInjDriverTabDlg::SetOfflineCheck(void)
{
 m_offline = true;
 UpdateDialogControls(this, TRUE);
}
