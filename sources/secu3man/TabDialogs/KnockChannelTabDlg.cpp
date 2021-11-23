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

/** \file KnockChannelTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "KnockChannelTabDlg.h"
#include "KnockFrqCalcDlg.h"

#include <math.h>
#include <limits>
#include "about/secu-3about.h"
#include "common/DPIAware.h"
#include "common/FastDelegate.h"
#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
#include "KnockContextMenuManager.h"
#include "ParamDesk/Params/KnockPageDlg.h"
#include "ui-core/Chart2D.h"
#include "ui-core/OScillCtrl.h"
#include "ui-core/ToolTipCtrlEx.h"

using namespace std;
using namespace fastdelegate;

#define TIMER_ID 0

#define K_SIG_MIN 0.0f
#define K_SIG_MAX 5.0f
#define LEVEL_SLIDER_POS_NUM 100
#define RPM_AXIS_MAX  (RPM_AXIS_MIN + ((RPM_KNOCK_SIGNAL_POINTS-1) * RPM_AXIS_STEP))

const UINT CKnockChannelTabDlg::IDD = IDD_KNOCK_CHANNEL;

CKnockChannelTabDlg::CKnockChannelTabDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockChannelTabDlg::IDD, pParent)
, mp_RTChart(new CChart2D())
, mp_knock_parameters_dlg(new CKnockPageDlg())
, mp_knock_frq_calc_dlg(new CKnockFrqCalcDlg())
, mp_OScopeCtrl(new COscillCtrl())
, mp_ContextMenuManager(new CKnockContextMenuManager())
, m_all_enabled(true)
, m_copy_to_attenuator_table_button_state(true)
, m_clear_function_button_state(true)
, m_dlsm_checkbox_state(true)
, m_initialized(false)
{
 mp_RTChart->AddSerie(RPM_KNOCK_SIGNAL_POINTS);    //signal serie
 mp_RTChart->AddSerie(2);      //level serie (horiz. line)
 mp_RTChart->AddSerie(2);      //RPM serie (vert. line)
 mp_RTChart->SetXAxisValueFormat(_T("%.0f")); //integers
 mp_RTChart->SetChartTitle(_T(""));
 mp_RTChart->SetChartLabel(_T(""),_T(""));
 mp_RTChart->SetGridXYNumber(10, 10);
 mp_RTChart->SetRange(RPM_AXIS_MIN, RPM_AXIS_MAX, K_SIG_MIN, K_SIG_MAX);
 mp_RTChart->SetBackColor(RGB(192, 192, 192));
 mp_RTChart->SetSerieColor(0, RGB(80, 80, 200));
 mp_RTChart->SetSerieColor(1, RGB(50, 200, 0));
 mp_RTChart->SetSerieColor(2, RGB(255, 255, 0));

 mp_ContextMenuManager->CreateContent();
 mp_knock_frq_calc_dlg->setOnCalculate(MakeDelegate(this, &CKnockChannelTabDlg::OnFrqCalculate));

 //=================================================================
 if (!CheckBitmaps() || !CheckAppMenu())
  delete this;
 //=================================================================
}

void CKnockChannelTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_KC_SAVE_PARAM_BUTTON, m_param_save_button);
 DDX_Control(pDX, IDC_KC_COPY_TO_ATTENUATOR_TABLE, m_copy_to_attenuator_table_button);
 DDX_Control(pDX, IDC_KC_CLEAR_FUNCTION, m_clear_function_button);
 DDX_Control(pDX, IDC_KC_DESIRED_LEVEL_SLIDER, m_level_slider);
 DDX_Control(pDX, IDC_KC_DLSM_CHECKBOX, m_dlsm_checkbox);
 DDX_Control(pDX, IDC_KC_DESIRED_LEVEL_TEXT, m_level_text);
 DDX_Control(pDX, IDC_KC_REALTIME_LIST, m_RTList);
 DDX_Control(pDX, IDC_KC_LIST_CHECKBOX, m_list_checkbox);
}

LPCTSTR CKnockChannelTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BEGIN_MESSAGE_MAP(CKnockChannelTabDlg, Super)
 ON_WM_DESTROY()
 ON_WM_CONTEXTMENU()
 ON_WM_SIZE()
 ON_BN_CLICKED(IDC_KC_SAVE_PARAM_BUTTON, OnSaveParameters)
 ON_UPDATE_COMMAND_UI(IDC_KC_SAVE_PARAM_BUTTON, OnUpdateControls)
 ON_BN_CLICKED(IDC_KC_COPY_TO_ATTENUATOR_TABLE, OnCopyToAttenuatorTable)
 ON_UPDATE_COMMAND_UI(IDC_KC_COPY_TO_ATTENUATOR_TABLE, OnUpdateCopyToAttenuatorTable)
 ON_BN_CLICKED(IDC_KC_CLEAR_FUNCTION, OnClearFunction)
 ON_UPDATE_COMMAND_UI(IDC_KC_CLEAR_FUNCTION, OnUpdateClearFunction)
 ON_BN_CLICKED(IDC_KC_DLSM_CHECKBOX, OnDLSMCheckbox)
 ON_BN_CLICKED(IDC_KC_LIST_CHECKBOX, OnListCheckbox)
 ON_UPDATE_COMMAND_UI(IDM_KC_LIST_RESET_POINTS, OnUpdateListResetPoints)
 ON_UPDATE_COMMAND_UI(IDM_KC_LIST_NEIGHBOUR_MIDDLE, OnUpdateListNeighbourMiddle)
 ON_COMMAND(IDM_KC_LIST_RESET_POINTS, OnListResetPoints)
 ON_COMMAND(IDM_KC_LIST_NEIGHBOUR_MIDDLE, OnListNeighbourMiddle)
 ON_COMMAND(IDM_KC_LIST_SIGMA_FILTER, OnListSigmaFilter)
 ON_COMMAND(IDM_KC_LIST_LOAD_POINTS, OnListLoadPoints)
 ON_COMMAND(IDM_KC_LIST_SAVE_POINTS, OnListSavePoints)
 ON_WM_TIMER()
 ON_WM_VSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKnockChannelTabDlg message handlers

BOOL CKnockChannelTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_dlsm_checkbox.SetCheck(m_dlsm_checkbox_state ? BST_CHECKED : BST_UNCHECKED);
 m_level_slider.EnableWindow(!m_dlsm_checkbox_state);

 CRect rect;
 GetDlgItem(IDC_KC_PARAMETERS_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);

 //создаем диалог с параметрами ДД
 mp_knock_parameters_dlg->Create(CKnockPageDlg::IDD, this);
 mp_knock_parameters_dlg->SetWindowPos(GetDlgItem(IDC_KC_CLEAR_FUNCTION),rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE);
 mp_knock_parameters_dlg->ShowWindow(SW_SHOWNORMAL);

 GetDlgItem(IDC_KC_KFC_DLG_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 //создаем диалог с калькулятором частоты детонации
 mp_knock_frq_calc_dlg->Create(CKnockFrqCalcDlg::IDD, this);
 mp_knock_frq_calc_dlg->SetWindowPos(GetDlgItem(IDC_KC_COPY_TO_ATTENUATOR_TABLE), rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE);
 mp_knock_frq_calc_dlg->ShowWindow(SW_SHOWNORMAL);

 SetTimer(TIMER_ID, 200, NULL);

 //Инициализируем построитель функций
 _InitializeRPMKnockSignalControl();

 //инициализируем осциллограф
 _InitializeOscilloscopeControl();

 //Initialize list of signal points
 _InitializeRPMKnockSignalList();

 mp_ContextMenuManager->Attach(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_param_save_button, MLL::GetString(IDS_KC_SAVE_PARAM_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_clear_function_button, MLL::GetString(IDS_KC_CLEAR_FUNCTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_copy_to_attenuator_table_button, MLL::GetString(IDS_KC_COPY_TO_ATTENUATOR_TABLE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_level_slider, MLL::GetString(IDS_KC_DESIRED_LEVEL_SLIDER_TT)));
 VERIFY(mp_ttc->AddWindow(&m_dlsm_checkbox, MLL::GetString(IDS_KC_DLSM_CHECKBOX_TT)));
 VERIFY(mp_ttc->AddWindow(&m_list_checkbox, MLL::GetString(IDS_KC_LIST_CHECKBOX_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 m_initialized = true;
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CKnockChannelTabDlg::OnDestroy()
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);
 m_initialized = false;
}

void CKnockChannelTabDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 if (pWnd->m_hWnd == m_RTList.m_hWnd)
  mp_ContextMenuManager->TrackPopupMenu(point.x, point.y);
}

void CKnockChannelTabDlg::OnSaveParameters(void)
{
 if (m_OnSaveParameters)
  m_OnSaveParameters();
}

void CKnockChannelTabDlg::OnDLSMCheckbox()
{
 m_dlsm_checkbox_state = m_dlsm_checkbox.GetCheck()==BST_CHECKED;
 m_level_slider.EnableWindow(!m_dlsm_checkbox_state);
}

void CKnockChannelTabDlg::OnListCheckbox()
{
 bool list_checkbox_state = m_list_checkbox.GetCheck()==BST_CHECKED;
 if (list_checkbox_state)
 {
  m_RTList.ShowWindow(SW_SHOW);
  mp_RTChart->ShowWindow(SW_HIDE);
 }
 else
 {
  m_RTList.ShowWindow(SW_HIDE);
  mp_RTChart->ShowWindow(SW_SHOW);
 }
}

void CKnockChannelTabDlg::EnableAll(bool i_enable)
{
 m_all_enabled = i_enable; //remember state
 mp_OScopeCtrl->EnableWindow(i_enable);
 mp_RTChart->EnableWindow(i_enable);
 UpdateDialogControls(this,TRUE);
}

void CKnockChannelTabDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_all_enabled);
}

void CKnockChannelTabDlg::OnUpdateCopyToAttenuatorTable(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(/*m_all_enabled && */m_copy_to_attenuator_table_button_state);
}

void CKnockChannelTabDlg::OnUpdateClearFunction(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(/*m_all_enabled && */m_clear_function_button_state);
}

void CKnockChannelTabDlg::OnUpdateListResetPoints(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_RTList.GetSelectedCount());
}

void CKnockChannelTabDlg::OnUpdateListNeighbourMiddle(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_RTList.GetSelectedCount()==1);
}

void CKnockChannelTabDlg::OnTimer(UINT nIDEvent)
{
 //dirty hack
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);
}

void CKnockChannelTabDlg::AppendPoint(float value)
{
 mp_OScopeCtrl->AppendPoint(value);
}

void CKnockChannelTabDlg::SetRPMKnockSignal(const std::vector<float> &i_values)
{
 _ASSERTE(i_values.size()==RPM_KNOCK_SIGNAL_POINTS);
 for(size_t i = 0; i < i_values.size(); i++)
 {
  mp_RTChart->SetXYValue(0, RPM_AXIS_MIN + (RPM_AXIS_STEP * i), i_values[i], i);
  TCHAR str[32];
  _stprintf(str, _T("%0.3f"), i_values[i]);
  m_RTList.SetItemText(i, 2, str);
 }
 mp_RTChart->Invalidate(TRUE);
}

void CKnockChannelTabDlg::_InitializeRPMKnockSignalControl(void)
{
 //Инициализируем построитель функций
 CRect rect;
 GetDlgItem(IDC_KC_REALTIME_CHART_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 int rpm = RPM_AXIS_MIN;

 mp_RTChart->Create(WS_CHILD | WS_VISIBLE, rect, this, IDC_KC_REALTIME_CHART);

 //First point of desired signal level line
 mp_RTChart->SetXYValue(1, (double)rpm, 0.0, 0);

 for (size_t i = 0; i < RPM_KNOCK_SIGNAL_POINTS; i++)
 {
  mp_RTChart->SetXYValue(0, rpm, 0.0, i);
  rpm+=RPM_AXIS_STEP;
 }

 //Second point of desired signal level line
 mp_RTChart->SetXYValue(1, (double)rpm, 0.0, 1);

 //первая и вторая точки вертикальной линии отображающей обороты
 mp_RTChart->SetXYValue(2, RPM_AXIS_MIN, K_SIG_MIN, 0);
 mp_RTChart->SetXYValue(2, RPM_AXIS_MIN, K_SIG_MAX, 1);

 //инициализация слайдера и установка дефаултного значения уровня
 m_level_slider.SetRange(0, LEVEL_SLIDER_POS_NUM);
 SetDesiredLevel(K_SIG_MIN);
}

void CKnockChannelTabDlg::_InitializeRPMKnockSignalList(void)
{
 m_RTList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
 m_RTList.InsertColumn(0, _T("N"), LVCFMT_LEFT, 35);
 m_RTList.InsertColumn(1, MLL::GetString(IDS_KC_LIST_HDR_RPM).c_str(), LVCFMT_CENTER, 70);
 m_RTList.InsertColumn(2, MLL::GetString(IDS_KC_LIST_HDR_VALUE).c_str(), LVCFMT_CENTER, 160);

 int rpm = RPM_AXIS_MIN;
 //Add items
 for(size_t i = 0; i < RPM_KNOCK_SIGNAL_POINTS; ++i)
 {  
  TCHAR str[32];
  _stprintf(str, _T("%d"), i+1);
  m_RTList.InsertItem(i, str);
  _stprintf(str, _T("%d"), rpm);
  m_RTList.SetItemText(i, 1, str);
  rpm+=RPM_AXIS_STEP;
 }
}

//инициализация осциллографа
void CKnockChannelTabDlg::_InitializeOscilloscopeControl(void)
{
 CRect rect;
 GetDlgItem(IDC_KC_SIGNAL_OSCILLOSCOPE_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 // create the control
 mp_OScopeCtrl->Create(WS_VISIBLE | WS_CHILD, rect, this);

 // customize the control
 mp_OScopeCtrl->SetRange(K_SIG_MIN, K_SIG_MAX, 1);
 mp_OScopeCtrl->SetUnitY(MLL::GetString(IDS_KC_OSCILLOSCOPE_V_UNIT));
 mp_OScopeCtrl->SetUnitX(MLL::GetString(IDS_KC_OSCILLOSCOPE_H_UNIT));
 mp_OScopeCtrl->SetBackgroundColor(RGB(0, 64, 0));
 mp_OScopeCtrl->SetGridColor(RGB(192, 192, 255));
 mp_OScopeCtrl->SetPlotColor(RGB(255, 255, 255));
}

void CKnockChannelTabDlg::setOnSaveParameters(EventHandler OnFunction)
{
 m_OnSaveParameters = OnFunction;
}

void CKnockChannelTabDlg::setOnCopyToAttenuatorTable(EventHandler OnFunction)
{
 m_OnCopyToAttenuatorTable = OnFunction;
}

void CKnockChannelTabDlg::setOnClearFunction(EventHandler OnFunction)
{
 m_OnClearFunction = OnFunction;
}

void CKnockChannelTabDlg::setOnResetPoints(EventIndexes OnFunction)
{
 m_OnResetPoints = OnFunction;
}

void CKnockChannelTabDlg::setOnSigmaFilter(EventHandler OnFunction)
{
 m_OnSigmaFilter = OnFunction;
}

void CKnockChannelTabDlg::setOnNeighbourMiddle(EventIndexes OnFunction)
{
 m_OnNeighbourMiddle = OnFunction;
}

void CKnockChannelTabDlg::setOnLoadPoints(EventHandler OnFunction)
{
 m_OnLoadPoints = OnFunction;
}

void CKnockChannelTabDlg::setOnSavePoints(EventHandler OnFunction)
{
 m_OnSavePoints = OnFunction;
}

void CKnockChannelTabDlg::OnCopyToAttenuatorTable()
{
 if (m_OnCopyToAttenuatorTable)
  m_OnCopyToAttenuatorTable();
}

void CKnockChannelTabDlg::EnableCopyToAttenuatorTableButton(bool i_enable)
{
 m_copy_to_attenuator_table_button_state = i_enable;
}

void CKnockChannelTabDlg::EnableClearFunctionButton(bool i_enable)
{
 m_clear_function_button_state = i_enable;
}

void CKnockChannelTabDlg::OnClearFunction()
{
 if (m_OnClearFunction)
  m_OnClearFunction();
}

static float SliderToLevel(int i_pos)
{
 float value = ((float)i_pos) / (((float)LEVEL_SLIDER_POS_NUM) / (K_SIG_MAX - K_SIG_MIN));
 return K_SIG_MAX - value;
}

void CKnockChannelTabDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
#define _SET_LEVEL(pos) {\
    mp_RTChart->SetXYValue(1, RPM_AXIS_MIN, SliderToLevel((pos)), 0);\
    mp_RTChart->SetXYValue(1, RPM_AXIS_MAX, SliderToLevel((pos)), 1);\
    CString cs; cs.Format(_T("%.02f"), SliderToLevel((pos))); \
    m_level_text.SetWindowText(cs); \
    }

 switch(nSBCode)
 {
  case TB_ENDTRACK:
   break;

  case TB_BOTTOM:
  case TB_TOP:
   _SET_LEVEL(m_level_slider.GetPos());
   break;

  case TB_PAGEDOWN:
  case TB_PAGEUP:
   _SET_LEVEL(m_level_slider.GetPos());
   break;

  case TB_LINEDOWN:
  case TB_LINEUP:
   _SET_LEVEL(m_level_slider.GetPos());
   break;

  case TB_THUMBPOSITION:
  case TB_THUMBTRACK:
   _SET_LEVEL(nPos);
   break;
 }
 #undef _SET_LEVEL
}

void CKnockChannelTabDlg::SetDesiredLevel(float i_level)
{
 ASSERT(i_level <= K_SIG_MAX);
 ASSERT(i_level >= K_SIG_MIN);
 float factor = (((float)LEVEL_SLIDER_POS_NUM) / (K_SIG_MAX - K_SIG_MIN));
 m_level_slider.SetPos(MathHelpers::Round((K_SIG_MAX - i_level) * factor));
 mp_RTChart->SetXYValue(1, RPM_AXIS_MIN, i_level, 0);
 mp_RTChart->SetXYValue(1, RPM_AXIS_MAX, i_level, 1);
 CString cs; cs.Format(_T("%.02f"), i_level);
 m_level_text.SetWindowText(cs);
}

float CKnockChannelTabDlg::GetDesiredLevel(void)
{
 return SliderToLevel(m_level_slider.GetPos());
}

void CKnockChannelTabDlg::SetDesiredLevelColor(bool color_ok)
{ 
 mp_RTChart->SetSerieColor(1, color_ok ? RGB(50,200,0) : RGB(200,50,0));
}

bool CKnockChannelTabDlg::GetDLSMCheckboxState(void) const
{
 return m_dlsm_checkbox_state;
}

void CKnockChannelTabDlg::SetRPMValue(int rpm)
{
 if (rpm < RPM_AXIS_MIN)
  rpm = RPM_AXIS_MIN;
 if (rpm > RPM_AXIS_MAX)
  rpm = RPM_AXIS_MAX;

 mp_RTChart->SetXYValue(2, rpm, K_SIG_MIN, 0);
 mp_RTChart->SetXYValue(2, rpm, K_SIG_MAX, 1);
}

void CKnockChannelTabDlg::SetRPMVisibility(bool visible)
{
 mp_RTChart->SetSerieVisibility(2, visible);
}

void CKnockChannelTabDlg::OnFrqCalculate(float frq)
{
 //Find frequency which is nearest to calculated one
 float smaller_diff = FLT_MAX;
 size_t index = 0;
 for (size_t i = 0; i < SECU3IO::GAIN_FREQUENCES_SIZE; i++)
 {
  float diff = fabs(SECU3IO::hip9011_gain_frequences[i] - frq);
  if (diff < smaller_diff)
  {
   smaller_diff = diff;
   index = i;
  }
 }
 //save value
 SECU3IO::KnockPar values;
 mp_knock_parameters_dlg->GetValues(&values);
 values.knock_bpf_frequency = index;
 mp_knock_parameters_dlg->SetValues(&values);
 //apply changes
 mp_knock_parameters_dlg->ForceOnChangeNotify();
}

void CKnockChannelTabDlg::OnListResetPoints()
{
 std::vector<int> pointIndexes;
 UINT i, uSelectedCount = m_RTList.GetSelectedCount();
 int  nItem = -1;
 if (uSelectedCount > 0)
 {
  for (i = 0; i < uSelectedCount; i++)
  {
   nItem = m_RTList.GetNextItem(nItem, LVNI_SELECTED);
   ASSERT(nItem != -1);
   pointIndexes.push_back(nItem);
  }
 } 
 if (m_OnResetPoints)
  m_OnResetPoints(pointIndexes);
}

void CKnockChannelTabDlg::OnListNeighbourMiddle()
{
 std::vector<int> pointIndexes;
 if (m_RTList.GetSelectedCount() > 0)
  pointIndexes.push_back(m_RTList.GetNextItem(-1, LVNI_SELECTED));
 if (m_OnNeighbourMiddle)
  m_OnNeighbourMiddle(pointIndexes);
}

void CKnockChannelTabDlg::OnListSigmaFilter()
{
 if (m_OnSigmaFilter)
  m_OnSigmaFilter();
}

void CKnockChannelTabDlg::OnListLoadPoints()
{
 if (m_OnLoadPoints)
  m_OnLoadPoints();
}

void CKnockChannelTabDlg::OnListSavePoints()
{
 if (m_OnSavePoints)
  m_OnSavePoints();
}

void CKnockChannelTabDlg::OnSize( UINT nType, int cx, int cy )
{
 if (m_initialized)
 {
  CRect rc1, rc2;

  rc1 = GDIHelpers::GetChildWndRect(mp_knock_parameters_dlg.get());
  GetClientRect(&rc2);
  mp_knock_parameters_dlg->SetWindowPos(NULL, 0, 0, rc1.Width(), rc2.bottom - rc1.top, SWP_NOMOVE | SWP_NOZORDER);

  DPIAware da;
  rc1 = GDIHelpers::GetChildWndRect(mp_OScopeCtrl.get());
  GetClientRect(&rc2);
  mp_OScopeCtrl->SetWindowPos(NULL, 0, 0, rc2.right - rc1.left - da.ScaleX(3), rc2.bottom - rc1.top  - da.ScaleY(3), SWP_NOMOVE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(mp_RTChart.get());
  GetClientRect(&rc2);
  mp_RTChart->SetWindowPos(NULL, 0, 0, rc2.right - rc1.left - da.ScaleX(3), rc1.Height(), SWP_NOMOVE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(&m_RTList);
  GetClientRect(&rc2);
  m_RTList.SetWindowPos(NULL, 0, 0, rc2.right - rc1.left - da.ScaleX(3), rc1.Height(), SWP_NOMOVE | SWP_NOZORDER);
 }

 Super::OnSize(nType, cx, cy);
}

void CKnockChannelTabDlg::ResetOscilloscope(void)
{
 mp_OScopeCtrl->Reset();
}

void CKnockChannelTabDlg::SetGraphShtPixels(int n)
{
 mp_OScopeCtrl->SetShtPixels(n);
}

void CKnockChannelTabDlg::SetGraphShowValue(bool show)
{
 mp_OScopeCtrl->ShowValue(show);
}
