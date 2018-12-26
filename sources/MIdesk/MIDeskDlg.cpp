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

/** \file MIDeskDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include <algorithm>
#include <numeric>
#include <limits>
#include "MIDeskDlg.h"

#include "common/Dll.h"
#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
#include "common/DPIAware.h"
#include "ui-core/AnalogMeterCtrl.h"

#include "MIAirFlow.h"
#include "MIDwellAngle.h"
#include "MIEGOCorr.h"
#include "MIIAT.h"
#include "MIInjPW.h"
#include "MIPressure.h"
#include "MITachometer.h"
#include "MITemperature.h"
#include "MIThrottleGate.h"
#include "MIVoltage.h"
#include "MIVoltmeter.h"
#include "MIVehicleSpeed.h"
#include "MITPSDot.h"
#include "MIMap2.h"
#include "MIMapD.h"
#include "MITemp2.h"
#include "MIFuelConsum.h"
#include "MIKnockRetard.h"
#include "MIKnockGraph.h"
#include "MISensAFR.h"
#include "MIChokePos.h"
#include "MISynLoad.h"
#include "ContextMenuManager.h"

#undef max

BEGIN_MESSAGE_MAP(CMIDeskDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_WM_PAINT()
 ON_WM_LBUTTONDOWN()
 ON_WM_MOUSEMOVE()
 ON_WM_LBUTTONUP()
 ON_WM_SETCURSOR()
 ON_WM_CONTEXTMENU()
 ON_WM_INITMENUPOPUP()
 ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
 ON_COMMAND(IDM_MI_MET_DEL_GAUGE, OnMetDeleteGauge)
 ON_UPDATE_COMMAND_UI(IDM_MI_MET_DEL_GAUGE, OnUpdateMetDelete)
 ON_COMMAND_RANGE(IDM_MI_MET_RPM, IDM_MI_MET_SYNLOAD, OnMetAddGauge)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_RPM, IDM_MI_MET_SYNLOAD, OnUpdateMetAddGauge)
 ON_COMMAND_RANGE(IDM_MI_MET_ROWS1, IDM_MI_MET_ROWS4, OnMetNumOfRows)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_ROWS1, IDM_MI_MET_ROWS4, OnUpdateMetNumOfRows)
 ON_COMMAND_RANGE(IDM_MI_MET_TITLE_FONT050, IDM_MI_MET_TITLE_FONT150, OnMetTitleFont)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_TITLE_FONT050, IDM_MI_MET_TITLE_FONT150, OnUpdateMetTitleFont)
 ON_COMMAND_RANGE(IDM_MI_MET_VALUE_FONT050, IDM_MI_MET_VALUE_FONT150, OnMetValueFont)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_VALUE_FONT050, IDM_MI_MET_VALUE_FONT150, OnUpdateMetValueFont)
 ON_COMMAND_RANGE(IDM_MI_MET_PANE_FONT050, IDM_MI_MET_PANE_FONT150, OnMetPaneFont)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_PANE_FONT050, IDM_MI_MET_PANE_FONT150, OnUpdateMetPaneFont)
 ON_COMMAND_RANGE(IDM_MI_MET_LABEL_FONT050, IDM_MI_MET_LABEL_FONT150, OnMetLabelFont)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_LABEL_FONT050, IDM_MI_MET_LABEL_FONT150, OnUpdateMetLabelFont)
 ON_COMMAND(IDM_MI_MET_ENDRAGNDROP, OnMetEnDragNDrop)
 ON_UPDATE_COMMAND_UI(IDM_MI_MET_ENDRAGNDROP, OnUpdateMetEnDragNDrop)
 ON_COMMAND_RANGE(IDM_MI_IND_ROWS1, IDM_MI_IND_ROWS4, OnIndNumOfRows)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_IND_ROWS1, IDM_MI_IND_ROWS4, OnUpdateIndNumOfRows)
 ON_COMMAND_RANGE(IDM_MI_IND_PERCHEIGHT03, IDM_MI_IND_PERCHEIGHT15, OnIndPercHeight)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_IND_PERCHEIGHT03, IDM_MI_IND_PERCHEIGHT15, OnUpdateIndPercHeight)
 ON_COMMAND(IDM_MI_IND_ENDRAGNDROP, OnIndEnDragNDrop)
 ON_UPDATE_COMMAND_UI(IDM_MI_IND_ENDRAGNDROP, OnUpdateIndEnDragNDrop)
 ON_COMMAND(IDM_MI_IND_DEL_INDICATOR, OnIndDelete)
 ON_UPDATE_COMMAND_UI(IDM_MI_IND_DEL_INDICATOR, OnUpdateIndDelete)
 ON_COMMAND_RANGE(IDM_MI_IND_GAS_V, IDM_MI_IND_SYSLOCKED, OnIndAddIndicator)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_IND_GAS_V, IDM_MI_IND_SYSLOCKED, OnUpdateIndAddIndicator)
 ON_COMMAND(IDM_MI_IND_SET_COLOR, OnIndSetColor)
 ON_UPDATE_COMMAND_UI(IDM_MI_IND_SET_COLOR, OnUpdateIndSetColor)
END_MESSAGE_MAP()

const UINT CMIDeskDlg::IDD = IDD_MEAS_INSTRUMENT_DESK;

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg dialog

CMIDeskDlg::CMIDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CMIDeskDlg::IDD, pParent)
, m_update_period(100)
, m_was_initialized(false)
, m_enabled(-1)
, m_metRows(0)
, m_indRows(0)
, m_showSpeedAndDistance(false)
, m_showChokePos(false)
, m_showGDPos(false)
, m_air_flow(0)
, m_tps_dot(0)
, mp_miTemperat(NULL)
, m_leds(DLL::GetModuleHandle())
, m_hMoveCursor(NULL)
, m_defCursor(NULL)
, m_draggingMet(false)
, m_draggingInd(false)
, mp_ctxMenuMgrMet(new CMetContextMenuManager())
, mp_ctxMenuMgrInd(new CIndContextMenuManager())
, m_metDragNDrop(false)
, m_indDragNDrop(false)
{
 mp_ctxMenuMgrMet->CreateContent();
 mp_ctxMenuMgrInd->CreateContent();

 memset(&m_values, 0, sizeof(SECU3IO::SensorDat));
 m_hMoveCursor = (HCURSOR)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND_MOVE), IMAGE_CURSOR, 0, 0, 0);
 VERIFY(m_hMoveCursor);
}

CMIDeskDlg::~CMIDeskDlg()
{
 _MetCleanUp();
 VERIFY(DestroyCursor(m_hMoveCursor));
}

void CMIDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_MI_INDICATORS, m_leds);
}

BOOL CMIDeskDlg::Create(UINT nIDTemplate, CWnd* pParentWnd, CRect& rect)
{
 BOOL result = Super::Create(nIDTemplate, pParentWnd);
 if (!result)
  return FALSE;

 SetWindowPos(NULL, rect.TopLeft().x,rect.TopLeft().y, rect.Width(), rect.Height(), SWP_NOZORDER);

 m_was_initialized = true;

 return result;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg message handlers

BOOL CMIDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();

 mp_ctxMenuMgrMet->Attach(this);
 mp_ctxMenuMgrInd->Attach(this);

 if (!m_font.GetSafeHandle())
 {
  LOGFONT LogFont;
  memset(&LogFont, 0x00, sizeof(LogFont));
  _tcsncpy(LogFont.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE);
  LogFont.lfHeight = 8;
  m_font.CreateFontIndirect(&LogFont);
 }

 m_leds.SetFont(&m_font);
 
 Enable(false);

 m_update_timer.SetTimer(this, &CMIDeskDlg::OnUpdateTimer, m_update_period);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CMIDeskDlg::OnDestroy()
{
 m_was_initialized = false;
 _MetCleanUp();
 Super::OnDestroy();
 m_update_timer.KillTimer();
 m_enabled = -1;
}

//разрешение/запрещение приборов
void CMIDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;
 //meters
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Enable(enable);
 //indicators
 m_leds.EnableWindow(enable);
}

void CMIDeskDlg::Show(bool show)
{
 //meters
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Show(show);
 //indicators
 m_leds.ShowWindow(show ? SW_SHOW : SW_HIDE);
}

using namespace SECU3IO;

void CMIDeskDlg::SetValues(const SensorDat* i_values)
{
 m_values = *i_values;
 m_air_flow = (float)i_values->air_flow;
 m_tps_dot = (float)i_values->tpsdot;

 m_ringRPM.Append((float)i_values->frequen);
 m_ringVBat.Append(i_values->voltage);
 m_ringMAP.Append(i_values->pressure);
 m_ringAddI1.Append(i_values->add_i1);
 m_ringTPS.Append(i_values->tps);
}

void CMIDeskDlg::OnUpdateTimer(void)
{
 if (!m_was_initialized)
  return;

 m_ringRPM.Calculate();
 m_ringVBat.Calculate();
 m_ringMAP.Calculate();
 m_ringAddI1.Calculate();
 m_ringTPS.Calculate();

 //meters
 MetFields_t::iterator mm;
 for(mm = m_metFields.begin(); mm != m_metFields.end(); ++mm)
  mm->second->SetValues();

 //indicators
 IndFields_t::iterator it;
 int idx = 0;
 for(it = m_indFields.begin(); it != m_indFields.end(); ++it, ++idx)
  m_leds.SetItemState(idx, *it->second.p_value, true);
}

void CMIDeskDlg::SetUpdatePeriod(unsigned int i_period)
{
 m_update_period = i_period;
 if (m_was_initialized)
 {
  m_update_timer.KillTimer();
  m_update_timer.SetTimer(this,&CMIDeskDlg::OnUpdateTimer, m_update_period);
 }
}

void CMIDeskDlg::SetTachometerMax(int i_max)
{
 m_tachoMax = i_max;
}

void CMIDeskDlg::SetPressureMax(int i_max)
{
 m_pressMax = i_max;
}

void CMIDeskDlg::SetSpeedUnit(int i_unit)
{
 m_speedUnit = (i_unit == 0) ? MLL::GetString(IDS_MI_KM_H) : MLL::GetString(IDS_MI_MP_H);
}

void CMIDeskDlg::SetDistanceUnit(int i_unit)
{
 m_distanceUnit = (i_unit == 0) ? MLL::GetString(IDS_MI_KM) : MLL::GetString(IDS_MI_MI);
}

void CMIDeskDlg::_Resize(void)
{
 bool redraw = false;
 CRect rect;
 GetClientRect(&rect);
 if (!rect.IsRectNull())
 {
  //meters
  MetFields_t::iterator it;
  for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
   it->second->Resize(_GetMetItemRect(it->first), redraw);

  //Indicators
  m_leds.MoveWindow(_GetGaugesRect(false), redraw);

  RedrawWindow();
 }
}

void CMIDeskDlg::ShowChokePos(bool i_show)
{
 m_showChokePos = i_show;
 if (mp_miTemperat!=NULL)
  mp_miTemperat->ShowTRP(i_show);
}

void CMIDeskDlg::ShowGDPos(bool i_show)
{
 m_showGDPos = i_show;
 if (mp_miTemperat!=NULL)
  mp_miTemperat->ShowTLP(i_show);
}

void CMIDeskDlg::ShowSpeedAndDistance(bool i_show)
{
 m_showSpeedAndDistance = i_show;
}

void CMIDeskDlg::SetRPMAverageNum(int avnum)
{
 m_ringRPM.m_avnum = avnum;
}

void CMIDeskDlg::SetVoltAverageNum(int avnum)
{
 m_ringVBat.m_avnum = avnum;
}

void CMIDeskDlg::SetMAPAverageNum(int avnum)
{
 m_ringMAP.m_avnum = avnum;
}

void CMIDeskDlg::SetAI1AverageNum(int avnum)
{
 m_ringAddI1.m_avnum = avnum;
}

void CMIDeskDlg::SetTPSAverageNum(int avnum)
{
 m_ringTPS.m_avnum = avnum;
}

void CMIDeskDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_was_initialized)
  _Resize();
}

void CMIDeskDlg::SetIndicatorsCfg(float IndHeingtPercent, int IndRows, IndCfg_t IndGas_v, IndCfg_t IndCarb, IndCfg_t IndIdleValve, IndCfg_t IndPowerValve, IndCfg_t IndStBlock, IndCfg_t IndAE, IndCfg_t IndCoolingFan, IndCfg_t IndCE, IndCfg_t IndFCRevLim, IndCfg_t IndFloodClear, IndCfg_t IndSysLocked)
{
 m_indCfg.clear();
 m_indCfg.insert(std::make_pair(IDM_MI_IND_GAS_V, IndGas_v));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_CARB, IndCarb));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_IDLEVALVE, IndIdleValve));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_POWERVALVE, IndPowerValve));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_STBLOCK, IndStBlock));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_AE, IndAE));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_COOLINGFAN, IndCoolingFan));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_CE, IndCE));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_FCREVLIM, IndFCRevLim));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_FLOODCLEAR, IndFloodClear));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_SYSLOCKED, IndSysLocked));

 m_indRows = IndRows;
 m_IndHeightCoeff = IndHeingtPercent / 100.0f;

 m_leds.SetNumRows(IndRows);
 m_indFields.clear();

 for (std::map<UINT, IndCfg_t>::iterator fi = m_indCfg.begin(); fi != m_indCfg.end(); ++fi)
  _IndFactory(fi->first);

 _CreateLEDs();
}

void CMIDeskDlg::SetMetersCfg(int MetRows, int MetRPM, int MetMAP, int MetVBat, int MetIgnTim, int MetCLT, int MetAddI1, int MetAddI2,
                              int MetInjPW, int MetIAT, int MetEGOCorr, int MetTPS, int MetAirFlow, int MetVehicleSpeed, int MetTPSDot,
                              int MetMAP2, int MetMAPD, int MetTmp2, int MetFuelConsum, int MetKnockRetard, int MetKnockGraph,
                              int MetSensAFR, int MetChokePos, int MetGDPos, int MetSynLoad, int TitleFontSize, int ValueFontSize, int PaneFontSize, int LabelFontSize)
{
 m_metCfg.clear();
 m_metCfg.insert(std::make_pair(IDM_MI_MET_RPM, MetRPM));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAP, MetMAP));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_VBAT, MetVBat));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_IGNTIM, MetIgnTim));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_CLT, MetCLT));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_ADDI1, MetAddI1));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_ADDI2, MetAddI2));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJPW, MetInjPW));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_IAT, MetIAT));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_EGOCORR, MetEGOCorr));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TPS, MetTPS));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_AIRFLOW, MetAirFlow));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_VEHSPD, MetVehicleSpeed));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TPSDOT, MetTPSDot));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAP2, MetMAP2));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAPD, MetMAPD));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TMP2, MetTmp2));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FUELCONSUM, MetFuelConsum));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_KNOCKRETARD, MetKnockRetard));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_KNOCKGRAPH, MetKnockGraph));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_SENSAFR, MetSensAFR));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_CHOKEPOS, MetChokePos));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_GDPOS, MetGDPos));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_SYNLOAD, MetSynLoad));

 m_metRows = MetRows;
 m_TitleFontSize = TitleFontSize;
 m_ValueFontSize = ValueFontSize;
 m_PaneFontSize = PaneFontSize;
 m_LabelFontSize = LabelFontSize;

 //remember color to have ability to detect changing of system colors (see OnPaint)
 m_COLOR_BTNFACE = GetSysColor(COLOR_BTNFACE);

 //destroy current widgets and clear list
 _MetCleanUp();

 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);

 _MetRearrangeKeys();

 //enable/disable
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Enable(m_enabled, false);

 _Resize();
}

void CMIDeskDlg::_MetCleanUp(void)
{
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  delete it->second;
 m_metFields.clear();
 mp_miTemperat = NULL;
}

CRect CMIDeskDlg::_GetMetItemRect(int idx)
{
 //calculate number of columns from number of items and rows
 int cols = ((int)m_metFields.size() / m_metRows) + ((int)m_metFields.size() % m_metRows ? 1 : 0);
 int i = idx / cols;
 int j = idx % cols;
 CRect rc = _GetGaugesRect(true); 
 int space  = 0;
 float width  = ((float)(rc.right - rc.left) - ((float)cols)*space) / ((float)cols);
 float height = ((float)(rc.bottom - rc.top) - ((float)m_metRows)*space) / ((float)m_metRows);
 return CRect(MathHelpers::Round(j*(width+space)), MathHelpers::Round(i*(height+space)), MathHelpers::Round(j*(width+space) + width), MathHelpers::Round(i*(height+space) + height));
}

void CMIDeskDlg::_MetRearrangeKeys(void)
{
 int i = 0;
 MetFields_t new_map;
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
 {
  new_map.insert(std::make_pair(i, it->second));
  m_metCfg[it->second->m_uiID] = i++;
 }
 m_metFields = new_map;
}

void CMIDeskDlg::_IndRearrangeKeys(void)
{
 int i = 0;
 IndFields_t new_map;
 IndFields_t::iterator it;
 for(it = m_indFields.begin(); it != m_indFields.end(); ++it)
 {
  new_map.insert(std::make_pair(i, it->second));
  m_indCfg[it->second.m_uiID].first = i++;
 }
 m_indFields = new_map;
}

void CMIDeskDlg::OnPaint() 
{
 CPaintDC dc(this); // device context for painting
 COLORREF newcolor = GetSysColor(COLOR_BTNFACE);
 if (newcolor != m_COLOR_BTNFACE)
 {
  m_COLOR_BTNFACE = newcolor;
  for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
   it->second->UpdateColors();
 }
}

void CMIDeskDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
 //gauges
 for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
 {
  CRect rc = it->second->GetWindowRect();
  if (rc.PtInRect(point) && m_metDragNDrop)
  {
   m_defCursor = SetCursor(m_hMoveCursor);
   m_draggingMet = true;
   m_dragItemMet = it;
  }
 }

 //indicators
 CPoint ptscr = point;
 ClientToScreen(&ptscr);
 for(IndFields_t::iterator it = m_indFields.begin(); it != m_indFields.end(); ++it)  
 {
  CRect rc = m_leds.GetItemRect(std::distance(m_indFields.begin(), it));
  m_leds.ClientToScreen(&rc);
  if (rc.PtInRect(ptscr) && m_indDragNDrop)
  {
   m_defCursor = SetCursor(m_hMoveCursor);
   m_draggingInd = true;
   m_dragItemInd = it;
  }
 }

 Super::OnLButtonDown(nFlags, point);
}

void CMIDeskDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
 //meters
 if (m_draggingMet && m_metDragNDrop)
 {
  for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
  {
   CRect rc = it->second->GetWindowRect();
   if (rc.PtInRect(point))
   {
    if (m_dragItemMet != it)
    {
     //swap gauges
     MeasInstrBase* widget = m_dragItemMet->second;
     m_dragItemMet->second = it->second;
     it->second = widget;

     //update order in settings
     m_metCfg[m_dragItemMet->second->m_uiID] = m_dragItemMet->first;
     m_metCfg[it->second->m_uiID] = it->first; 

     //update positions of widgets
     _Resize();

     if (m_OnMISettingsChanged)
      m_OnMISettingsChanged();

     break;
    }
   }
  }
 }


 //indicators
 if (m_draggingInd && m_indDragNDrop)
 {
  CPoint ptscr = point;
  ClientToScreen(&ptscr);
  for(IndFields_t::iterator it = m_indFields.begin(); it != m_indFields.end(); ++it)  
  {
   CRect rc = m_leds.GetItemRect(std::distance(m_indFields.begin(), it));
   m_leds.ClientToScreen(&rc);
   if (rc.PtInRect(ptscr))
   {
    if (m_dragItemInd != it)
    {
     //swap indicators
     IndFieldData ind = m_dragItemInd->second;
     m_dragItemInd->second = it->second;
     it->second = ind;

     //update order in settings
     m_indCfg[m_dragItemInd->second.m_uiID].first = m_dragItemInd->first;
     m_indCfg[it->second.m_uiID].first = it->first; 

     _CreateLEDs();

     m_leds.Invalidate();

     if (m_OnMISettingsChanged)
      m_OnMISettingsChanged();

     break;
    }
   }
  }
 }
 if (m_defCursor)
  SetCursor(m_defCursor);
 m_draggingMet = false;
 m_draggingInd = false;
 Super::OnLButtonUp(nFlags, point);
}

void CMIDeskDlg::OnMouseMove(UINT nFlags, CPoint point)
{
 if (!_GetGaugesRect(true).PtInRect(point))
  m_draggingMet = false;

 if (!_GetGaugesRect(false).PtInRect(point))
  m_draggingInd = false;

 if (m_draggingMet || m_draggingInd)
  SetCursor(m_hMoveCursor);

 //this code tell system to call OnMouseLeave when mouse leaves window
 TRACKMOUSEEVENT tme;
 tme.cbSize = sizeof(TRACKMOUSEEVENT);
 tme.dwFlags = TME_LEAVE;
 tme.hwndTrack = GetSafeHwnd();
 TrackMouseEvent(&tme);

 Super::OnMouseMove(nFlags, point);
}

BOOL CMIDeskDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
 if (m_draggingMet || m_draggingInd)
  return FALSE;

 return Super::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CMIDeskDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
 m_draggingMet = false;
 m_draggingInd = false;
 return TRUE;
}

void CMIDeskDlg::GetMetersCfg(int &MetRows, int &MetRPM, int &MetMAP, int &MetVBat, int &MetIgnTim, int &MetCLT, int &MetAddI1, int &MetAddI2,
                    int &MetInjPW, int &MetIAT, int &MetEGOCorr, int &MetTPS, int &MetAirFlow, int &MetVehicleSpeed, int &MetTPSDot, int &MetMAP2,
                    int &MetMapD, int &MetTmp2, int &MetFuelConsum, int &MetKnockRetard, int &MetKnockGraph, int &MetSensAFR, int &MetChokePos,
                    int &MetGDPos, int &MetSynLoad, int &TitleFontSize, int &ValueFontSize, int &PaneFontSize, int &LabelFontSize)
{

 MetRows = m_metRows;
 TitleFontSize = m_TitleFontSize, ValueFontSize = m_ValueFontSize, PaneFontSize = m_PaneFontSize, LabelFontSize = m_LabelFontSize;

 MetRPM = m_metCfg[IDM_MI_MET_RPM];
 MetMAP = m_metCfg[IDM_MI_MET_MAP];
 MetVBat = m_metCfg[IDM_MI_MET_VBAT];
 MetIgnTim = m_metCfg[IDM_MI_MET_IGNTIM];
 MetCLT = m_metCfg[IDM_MI_MET_CLT];
 MetAddI1 = m_metCfg[IDM_MI_MET_ADDI1];
 MetAddI2 = m_metCfg[IDM_MI_MET_ADDI2];
 MetInjPW = m_metCfg[IDM_MI_MET_INJPW];
 MetIAT = m_metCfg[IDM_MI_MET_IAT];
 MetEGOCorr = m_metCfg[IDM_MI_MET_EGOCORR];
 MetTPS = m_metCfg[IDM_MI_MET_TPS];
 MetAirFlow = m_metCfg[IDM_MI_MET_AIRFLOW];
 MetVehicleSpeed = m_metCfg[IDM_MI_MET_VEHSPD];
 MetTPSDot = m_metCfg[IDM_MI_MET_TPSDOT];
 MetMAP2 = m_metCfg[IDM_MI_MET_MAP2];
 MetMapD = m_metCfg[IDM_MI_MET_MAPD];
 MetTmp2 = m_metCfg[IDM_MI_MET_TMP2];
 MetFuelConsum = m_metCfg[IDM_MI_MET_FUELCONSUM];
 MetKnockRetard = m_metCfg[IDM_MI_MET_KNOCKRETARD];
 MetKnockGraph = m_metCfg[IDM_MI_MET_KNOCKGRAPH];
 MetSensAFR = m_metCfg[IDM_MI_MET_SENSAFR];
 MetChokePos = m_metCfg[IDM_MI_MET_CHOKEPOS];
 MetGDPos= m_metCfg[IDM_MI_MET_GDPOS];
 MetSynLoad = m_metCfg[IDM_MI_MET_SYNLOAD];
}

void CMIDeskDlg::GetIndicatorsCfg(float &IndHeingtPercent, int &IndRows, IndCfg_t &IndGas_v, IndCfg_t &IndCarb, IndCfg_t &IndIdleValve, IndCfg_t &IndPowerValve, IndCfg_t &IndStBlock, IndCfg_t &IndAE,
                      IndCfg_t &IndCoolingFan, IndCfg_t &IndCE, IndCfg_t &IndFCRevLim, IndCfg_t &IndFloodClear, IndCfg_t &IndSysLocked)
{
 IndHeingtPercent = m_IndHeightCoeff * 100.0f;
 IndRows = m_indRows;
 
 IndGas_v = m_indCfg[IDM_MI_IND_GAS_V];
 IndCarb = m_indCfg[IDM_MI_IND_CARB];
 IndIdleValve = m_indCfg[IDM_MI_IND_IDLEVALVE];
 IndPowerValve = m_indCfg[IDM_MI_IND_POWERVALVE];
 IndStBlock = m_indCfg[IDM_MI_IND_STBLOCK];
 IndAE = m_indCfg[IDM_MI_IND_AE];
 IndCoolingFan = m_indCfg[IDM_MI_IND_COOLINGFAN];
 IndCE = m_indCfg[IDM_MI_IND_CE];
 IndFCRevLim = m_indCfg[IDM_MI_IND_FCREVLIM];
 IndFloodClear = m_indCfg[IDM_MI_IND_FLOODCLEAR];
 IndSysLocked = m_indCfg[IDM_MI_IND_SYSLOCKED];
}

void CMIDeskDlg::setOnMISettingsChanged(EventHandler i_Function)
{
 m_OnMISettingsChanged = i_Function;
}

void CMIDeskDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 mp_ctxMenuMgrMet->OnInitMenuPopup(pMenu, nIndex, bSysMenu);
 mp_ctxMenuMgrInd->OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}

void CMIDeskDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 //display menu for gauges
 CRect rc = _GetGaugesRect(true, true); 
 if (rc.PtInRect(point))
 {
  if (m_metFields.size())
  {
   m_dragItemMet = m_metFields.end();
   for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
   {
    CRect rc = it->second->GetWindowRect(true);
    if (rc.PtInRect(point))
    {
     m_dragItemMet = it;
     break;
    }
   }
  }
  else
  {
   m_dragItemMet = m_metFields.end();
  }
  mp_ctxMenuMgrMet->TrackPopupMenu(point.x, point.y);
 }
 //display menu for indicators
 rc = _GetGaugesRect(false, true);
 if (rc.PtInRect(point))
 {
  if (m_indFields.size())
  {
   m_dragItemInd = m_indFields.end();
   for(IndFields_t::iterator it = m_indFields.begin(); it != m_indFields.end(); ++it)  
   {
    CRect rc = m_leds.GetItemRect(std::distance(m_indFields.begin(), it));
    m_leds.ClientToScreen(&rc);
    if (rc.PtInRect(point))
    {
     m_dragItemInd = it;
     break;
    }
   }
  }
  else
  {
   m_dragItemInd = m_indFields.end();
  }
  mp_ctxMenuMgrInd->TrackPopupMenu(point.x, point.y);
 }
}

void CMIDeskDlg::OnMetDeleteGauge()
{
 m_metCfg[m_dragItemMet->second->m_uiID] = std::numeric_limits<int>::max(); 
 delete m_dragItemMet->second;
 m_metFields.erase(m_dragItemMet);

 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetDelete(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_metFields.size() && m_dragItemMet != m_metFields.end());
}

void CMIDeskDlg::OnMetAddGauge(UINT nID)
{
 if (m_dragItemMet == m_metFields.end())
  m_metCfg[nID] = m_metFields.size() ? m_metFields.rbegin()->first : 0;
 else
  m_metCfg[nID] = m_dragItemMet->first;

 _MetFactory(nID);

 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetAddGauge(CCmdUI* pCmdUI)
{
 bool already_add = m_metCfg[pCmdUI->m_nID] != std::numeric_limits<int>::max();
 pCmdUI->Enable(!already_add);
 pCmdUI->SetCheck(already_add);
}

void CMIDeskDlg::_MetFactory(UINT uiID)
{
 DPIAware da;
 int TitleFontSize = da.UnScaleY(m_TitleFontSize);
 int ValueFontSize = da.UnScaleY(m_ValueFontSize);
 int PaneFontSize = da.UnScaleY(m_PaneFontSize);
 int LabelFontSize = da.UnScaleY(m_LabelFontSize);

 switch(uiID)
 {
  case IDM_MI_MET_RPM:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITachometer* widget = new CMITachometer();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ringRPM.m_result, &m_values.speed, &m_values.distance);
   widget->ShowTLP(m_showSpeedAndDistance);
   widget->ShowTRP(m_showSpeedAndDistance);
   widget->SetPaneUnit(m_speedUnit, m_distanceUnit);
   widget->SetLimits(0, (float)m_tachoMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_MAP:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIPressure* widget = new CMIPressure();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ringMAP.m_result, NULL, NULL);
   widget->SetLimits(10, (float)m_pressMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_VBAT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIVoltmeter* widget = new CMIVoltmeter();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ringVBat.m_result, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_IGNTIM:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIDwellAngle* widget = new CMIDwellAngle();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.adv_angle, NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_CLT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
   {
    mp_miTemperat = NULL;
    break;
   }
   CMITemperature* widget = new CMITemperature();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.temperat, &m_values.gasdose_pos, &m_values.choke_pos);  
   widget->ShowTLP(m_showGDPos);
   widget->ShowTRP(m_showChokePos);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   mp_miTemperat = widget;
   break;
  }
  case IDM_MI_MET_ADDI1:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAddI1* widget = new CMIAddI1();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ringAddI1.m_result, NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_ADDI2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAddI2* widget = new CMIAddI2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.add_i2, NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_INJPW:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjPW* widget = new CMIInjPW();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.inj_pw, NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_IAT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIIAT* widget = new CMIIAT();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.air_temp, NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_EGOCORR:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIEGOCorr* widget = new CMIEGOCorr();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.lambda_corr, NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_TPS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIThrottleGate* widget = new CMIThrottleGate();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ringTPS.m_result, &m_air_flow, NULL);
   widget->ShowTLP(true);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_AIRFLOW:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAirFlow* widget = new CMIAirFlow();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_air_flow, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_VEHSPD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIVehicleSpeed* widget = new CMIVehicleSpeed();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.speed, NULL, NULL);
   widget->SetMeterUnit(m_speedUnit);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_TPSDOT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITPSDot* widget = new CMITPSDot();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_tps_dot, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_MAP2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMap2* widget = new CMIMap2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.map2, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_MAPD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMapD* widget = new CMIMapD();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.mapd, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_TMP2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITemp2* widget = new CMITemp2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.tmp2, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_FUELCONSUM:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIFuelConsum* widget = new CMIFuelConsum();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.inj_ffd, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_KNOCKRETARD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIKnockRetard* widget = new CMIKnockRetard();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.knock_retard, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_KNOCKGRAPH:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIKnockGraph* widget = new CMIKnockGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.knock_k, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_SENSAFR:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMISensAFR* widget = new CMISensAFR();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.afr, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_CHOKEPOS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIChokePos* widget = new CMIChokePos();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.choke_pos, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_GDPOS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIGDPos* widget = new CMIGDPos();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.gasdose_pos, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
  case IDM_MI_MET_SYNLOAD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMISynLoad* widget = new CMISynLoad();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_values.load, NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   break;
  }
 }
}

void CMIDeskDlg::OnMetNumOfRows(UINT nID)
{
 m_metRows = (nID - IDM_MI_MET_ROWS1) + 1;

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetNumOfRows(CCmdUI* pCmdUI)
{
 pCmdUI->SetCheck(m_metRows == (pCmdUI->m_nID - IDM_MI_MET_ROWS1) + 1);
}

void CMIDeskDlg::OnMetTitleFont(UINT nID)
{
 m_TitleFontSize = ((nID - IDM_MI_MET_TITLE_FONT050) * 10) + 50;

 _MetCleanUp();
 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);
 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetTitleFont(CCmdUI* pCmdUI)
{
 int fontSize = ((pCmdUI->m_nID - IDM_MI_MET_TITLE_FONT050) * 10) + 50;
 pCmdUI->SetCheck(m_TitleFontSize == fontSize);
}

void CMIDeskDlg::OnMetValueFont(UINT nID)
{
 m_ValueFontSize = ((nID - IDM_MI_MET_VALUE_FONT050) * 10) + 50;

 _MetCleanUp();
 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);
 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetValueFont(CCmdUI* pCmdUI)
{
 int fontSize = ((pCmdUI->m_nID - IDM_MI_MET_VALUE_FONT050) * 10) + 50;
 pCmdUI->SetCheck(m_ValueFontSize == fontSize);
}

void CMIDeskDlg::OnMetPaneFont(UINT nID)
{
 m_PaneFontSize = ((nID - IDM_MI_MET_PANE_FONT050) * 10) + 50;

 _MetCleanUp();
 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);
 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetPaneFont(CCmdUI* pCmdUI)
{
 int fontSize = ((pCmdUI->m_nID - IDM_MI_MET_PANE_FONT050) * 10) + 50;
 pCmdUI->SetCheck(m_PaneFontSize == fontSize);
}

void CMIDeskDlg::OnMetLabelFont(UINT nID)
{
 m_LabelFontSize = ((nID - IDM_MI_MET_LABEL_FONT050) * 10) + 50;

 _MetCleanUp();
 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);
 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetLabelFont(CCmdUI* pCmdUI)
{
 int fontSize = ((pCmdUI->m_nID - IDM_MI_MET_LABEL_FONT050) * 10) + 50;
 pCmdUI->SetCheck(m_LabelFontSize == fontSize);
}

void CMIDeskDlg::SetMetersDragNDrop(bool enable)
{
 m_metDragNDrop = enable;
}

bool CMIDeskDlg::GetMetersDragNDrop(void) const
{
 return m_metDragNDrop;
}

void CMIDeskDlg::SetIndicatorsDragNDrop(bool enable)
{
 m_indDragNDrop = enable;
}

bool CMIDeskDlg::GetIndicatorsDragNDrop(void) const
{
 return m_indDragNDrop;
}

void CMIDeskDlg::OnMetEnDragNDrop()
{
 m_metDragNDrop = !m_metDragNDrop;

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetEnDragNDrop(CCmdUI* pCmdUI)
{
 pCmdUI->SetCheck(m_metDragNDrop);
}

void CMIDeskDlg::OnIndNumOfRows(UINT nID)
{
 m_indRows = (nID - IDM_MI_IND_ROWS1) + 1;
 
 m_leds.SetNumRows(m_indRows);
 m_leds.Invalidate();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndNumOfRows(CCmdUI* pCmdUI)
{
 pCmdUI->SetCheck(m_indRows == (pCmdUI->m_nID - IDM_MI_IND_ROWS1) + 1);
}

float CMIDeskDlg::_HeightCoeffFromID(UINT nID)
{
 switch(nID)
 {
  case IDM_MI_IND_PERCHEIGHT03: return 0.03f;
  case IDM_MI_IND_PERCHEIGHT05: return 0.05f;
  case IDM_MI_IND_PERCHEIGHT07: return 0.07f;
  case IDM_MI_IND_PERCHEIGHT10: return 0.10f;
  case IDM_MI_IND_PERCHEIGHT15: return 0.15f;
 }
 return 0.10f;
}

void CMIDeskDlg::OnIndPercHeight(UINT nID)
{ 
 m_IndHeightCoeff = _HeightCoeffFromID(nID);

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndPercHeight(CCmdUI* pCmdUI)
{
 float coeff = _HeightCoeffFromID(pCmdUI->m_nID);
 pCmdUI->SetCheck(m_IndHeightCoeff == coeff);
}

void CMIDeskDlg::OnIndEnDragNDrop()
{
 m_indDragNDrop = !m_indDragNDrop;

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndEnDragNDrop(CCmdUI* pCmdUI)
{
 pCmdUI->SetCheck(m_indDragNDrop);
}

void CMIDeskDlg::OnIndDelete()
{
 m_indCfg[m_dragItemInd->second.m_uiID].first = std::numeric_limits<int>::max(); 
 m_indFields.erase(m_dragItemInd);

 _CreateLEDs();

 m_leds.Invalidate();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndDelete(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_indFields.size() && m_dragItemInd != m_indFields.end());
}

void CMIDeskDlg::OnIndAddIndicator(UINT nID)
{
 if (m_dragItemInd == m_indFields.end())
  m_indCfg[nID].first = m_indFields.size() ? m_indFields.rbegin()->first : 0;
 else
  m_indCfg[nID].first = m_dragItemInd->first;

 _IndFactory(nID);

 _IndRearrangeKeys();

 _CreateLEDs();

 m_leds.Invalidate();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndAddIndicator(CCmdUI* pCmdUI)
{
 bool already_add = m_indCfg[pCmdUI->m_nID].first != std::numeric_limits<int>::max();
 pCmdUI->Enable(!already_add);
 pCmdUI->SetCheck(already_add);
}

void CMIDeskDlg::_IndFactory(UINT uiID)
{
 switch(uiID)
 {
  case IDM_MI_IND_GAS_V:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_GAS_V), m_indCfg[uiID].second, &m_values.gas, IDM_MI_IND_GAS_V)));
   break;
  case IDM_MI_IND_CARB:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_CARB), m_indCfg[uiID].second, &m_values.carb, IDM_MI_IND_CARB)));
   break;
  case IDM_MI_IND_IDLEVALVE: 
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_IDLEVALVE), m_indCfg[uiID].second, &m_values.ephh_valve, IDM_MI_IND_IDLEVALVE)));
   break;
  case IDM_MI_IND_POWERVALVE:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_POWERVALVE), m_indCfg[uiID].second, &m_values.epm_valve, IDM_MI_IND_POWERVALVE)));
   break;
  case IDM_MI_IND_STBLOCK:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_STBLOCK), m_indCfg[uiID].second, &m_values.st_block, IDM_MI_IND_STBLOCK)));
   break;
  case IDM_MI_IND_AE:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_AE), m_indCfg[uiID].second, &m_values.acceleration, IDM_MI_IND_AE)));
   break;
  case IDM_MI_IND_COOLINGFAN:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_COOLINGFAN), m_indCfg[uiID].second, &m_values.cool_fan, IDM_MI_IND_COOLINGFAN)));
   break;
  case IDM_MI_IND_CE:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_CE), m_indCfg[uiID].second, &m_values.ce_state, IDM_MI_IND_CE)));
   break;
  case IDM_MI_IND_FCREVLIM:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_FCREVLIM), m_indCfg[uiID].second, &m_values.fc_revlim, IDM_MI_IND_FCREVLIM)));
   break;
  case IDM_MI_IND_FLOODCLEAR:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_FLOODCLEAR), m_indCfg[uiID].second, &m_values.floodclear, IDM_MI_IND_FLOODCLEAR)));
   break;
  case IDM_MI_IND_SYSLOCKED:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_SYSLOCKED), m_indCfg[uiID].second, &m_values.sys_locked, IDM_MI_IND_SYSLOCKED)));
   break;
 }
}

void CMIDeskDlg::OnIndSetColor()
{
 CColorDialog dlg; 
 if (dlg.DoModal() == IDOK) 
 { 
  COLORREF color = dlg.GetColor(); 

  m_dragItemInd->second.color = color;
  m_indCfg[m_dragItemInd->second.m_uiID].second = color;

  m_leds.SetItemColor(std::distance(m_indFields.begin(), m_dragItemInd), color, true);

  if (m_OnMISettingsChanged)
   m_OnMISettingsChanged();
 }
}

void CMIDeskDlg::OnUpdateIndSetColor(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_indFields.size() && m_dragItemInd != m_indFields.end());
}

void CMIDeskDlg::_CreateLEDs(void)
{
 m_leds.Clear();
 IndFields_t::iterator it;
 for(it = m_indFields.begin(); it != m_indFields.end(); ++it)
  m_leds.AddItem(it->second.caption.c_str(), it->second.color);
}

CRect CMIDeskDlg::_GetGaugesRect(bool meters, bool screenCoord /*= false*/)
{
 CRect rc;
 GetClientRect(&rc);
 if (meters)
  rc.bottom = rc.bottom - MathHelpers::Round(rc.Height() * m_IndHeightCoeff);
 else
  rc.top = rc.bottom - MathHelpers::Round(rc.Height() * m_IndHeightCoeff);
 if (screenCoord)
  ClientToScreen(&rc);
 return rc;
}
