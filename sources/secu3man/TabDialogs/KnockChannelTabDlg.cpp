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

#include "stdafx.h"
#include "Resources/resource.h"
#include "KnockChannelTabDlg.h"
#include "KnockFrqCalcDlg.h"

#include <math.h>
#include <limits>
#include "common/FastDelegate.h"
#include "common/MathHelpers.h"
#include "HiSCCtrl/sources/ChartCtrl.h"
#include "HiSCCtrl/sources/ChartPointsSerie.h"
#include "HiSCCtrl/sources/ChartLineSerie.h"
#include "KnockContextMenuManager.h"
#include "ParamDesk/Params/KnockPageDlg.h"
#include "ui-core/OScopeCtrl.h"

using namespace std;
using namespace fastdelegate;

#define TIMER_ID 0

#define K_SIG_MIN 0.0f
#define K_SIG_MAX 5.0f
#define LEVEL_SLIDER_POS_NUM 100
#define RPM_AXIS_MAX  (RPM_AXIS_MIN + (RPM_KNOCK_SIGNAL_POINTS * RPM_AXIS_STEP))

const UINT CKnockChannelTabDlg::IDD = IDD_KNOCK_CHANNEL;

CKnockChannelTabDlg::CKnockChannelTabDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockChannelTabDlg::IDD, pParent)
, mp_RTChart(NULL)
, mp_knock_parameters_dlg(new CKnockPageDlg())
, mp_knock_frq_calc_dlg(new CKnockFrqCalcDlg())
, mp_OScopeCtrl(new COScopeCtrl())
, mp_ContextMenuManager(new CKnockContextMenuManager())
, m_all_enabled(true)
, m_pPointSerie(NULL)
, m_pLineSerie(NULL)
, m_pLineSerieLevel(NULL)
, m_pLineSerieRPM(NULL)
, m_copy_to_attenuator_table_button_state(true)
, m_clear_function_button_state(true)
, m_dlsm_checkbox_state(true)
{
 mp_ContextMenuManager->CreateContent();
 mp_knock_frq_calc_dlg->setOnCalculate(MakeDelegate(this, &CKnockChannelTabDlg::OnFrqCalculate));
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
 ON_WM_INITMENUPOPUP()
 ON_WM_CONTEXTMENU()
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

 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CKnockChannelTabDlg::OnDestroy()
{
 Super::OnDestroy();
 delete mp_RTChart;
 KillTimer(TIMER_ID);
}

void CKnockChannelTabDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 mp_ContextMenuManager->OnInitMenuPopup(pMenu, nIndex, bSysMenu);
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
 mp_RTChart->EnableRefresh(false);
 for(size_t i = 0; i < i_values.size(); i++)
 {
  m_pPointSerie->SetYPointValue(i, i_values[i]);
  m_pLineSerie->SetYPointValue(i, i_values[i]);
  TCHAR str[32];
  _stprintf(str, _T("%0.3f"), i_values[i]);
  m_RTList.SetItemText(i, 2, str);
 }
 mp_RTChart->EnableRefresh(true);
}

void CKnockChannelTabDlg::_InitializeRPMKnockSignalControl(void)
{
 //Инициализируем построитель функций
 mp_RTChart = new CChartCtrl();
 CRect rect;
 GetDlgItem(IDC_KC_REALTIME_CHART_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_RTChart->Create(this,rect,IDC_KC_REALTIME_CHART);

 m_pPointSerie = dynamic_cast<CChartPointsSerie*>(mp_RTChart->AddSerie(CChartSerie::stPointsSerie));
 m_pLineSerie = dynamic_cast<CChartLineSerie*>(mp_RTChart->AddSerie(CChartSerie::stLineSerie));
 m_pLineSerieLevel = dynamic_cast<CChartLineSerie*>(mp_RTChart->AddSerie(CChartSerie::stLineSerie));
 m_pLineSerieRPM = dynamic_cast<CChartLineSerie*>(mp_RTChart->AddSerie(CChartSerie::stLineSerie));

 m_pLineSerie->SetColor(RGB(80,80,200));
 m_pLineSerieLevel->SetColor(RGB(50,200,0));
 m_pLineSerieRPM->SetColor(RGB(255,255,0));

 int rpm = RPM_AXIS_MIN;
 int rpm_step = RPM_AXIS_STEP;

 //первая точка линии желаемого уровня сигнала
 m_pLineSerieLevel->AddPoint(rpm, 0);

 // Sets the min and max values of the bottom and left axis.
 mp_RTChart->GetBottomAxis()->SetMinMax(rpm, rpm + (rpm_step * RPM_KNOCK_SIGNAL_POINTS));
 mp_RTChart->GetLeftAxis()->SetMinMax(K_SIG_MIN, K_SIG_MAX);

 for (size_t i = 0; i < RPM_KNOCK_SIGNAL_POINTS; i++)
 {
  m_pPointSerie->AddPoint(rpm, 0.0);
  m_pLineSerie->AddPoint(rpm, 0.0);
  rpm+=rpm_step;
 }

 //вторая точка линии желаемого уровня сигнала
 m_pLineSerieLevel->AddPoint(rpm, 0);

 //первая и вторая точки вертикальной линии отображающей обороты
 m_pLineSerieRPM->AddPoint(RPM_AXIS_MIN, K_SIG_MIN);
 m_pLineSerieRPM->AddPoint(RPM_AXIS_MIN, K_SIG_MAX);

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
 int rpm_step = RPM_AXIS_STEP;
 //Add items
 for(size_t i = 0; i < RPM_KNOCK_SIGNAL_POINTS; ++i)
 {  
  TCHAR str[32];
  _stprintf(str, _T("%d"), i+1);
  m_RTList.InsertItem(i, str);
  _stprintf(str, _T("%d"), rpm);
  m_RTList.SetItemText(i, 1, str);
  rpm+=rpm_step;
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
 mp_OScopeCtrl->SetYUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_V_UNIT));
 mp_OScopeCtrl->SetXUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_H_UNIT));
 mp_OScopeCtrl->SetBackgroundColor(RGB(0, 0, 64));
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
    m_pLineSerieLevel->SetYPointValue(0, SliderToLevel((pos))); \
    m_pLineSerieLevel->SetYPointValue(1, SliderToLevel((pos))); \
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
 m_pLineSerieLevel->SetYPointValue(0, i_level);
 m_pLineSerieLevel->SetYPointValue(1, i_level);
 CString cs; cs.Format(_T("%.02f"), i_level);
 m_level_text.SetWindowText(cs);
}

float CKnockChannelTabDlg::GetDesiredLevel(void)
{
 return SliderToLevel(m_level_slider.GetPos());
}

void CKnockChannelTabDlg::SetDesiredLevelColor(bool color_ok)
{ 
 m_pLineSerieLevel->SetColor(color_ok ? RGB(50,200,0) : RGB(200,50,0));
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

 m_pLineSerieRPM->SetXPointValue(0, rpm);
 m_pLineSerieRPM->SetXPointValue(1, rpm);
}

void CKnockChannelTabDlg::SetRPMVisibility(bool visible)
{
 m_pLineSerieRPM->SetVisible(visible);
}

void CKnockChannelTabDlg::OnFrqCalculate(float frq)
{
 //Ищем блищаюшую частоту к той, что получилась исходя из расчетов
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
 //сохраняем значение
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
