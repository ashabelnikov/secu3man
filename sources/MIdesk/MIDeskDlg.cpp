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

#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
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

#undef max

static const float IndHeightCoeff = 0.10f;

BEGIN_MESSAGE_MAP(CMIDeskDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
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
, m_showSpeedAndDistance(false)
, m_showChokePos(false)
, m_showGDPos(false)
, m_air_flow(0)
, m_tps_dot(0)
, mp_miTemperat(NULL)
{
 memset(&m_values, 0, sizeof(SECU3IO::SensorDat));
}

CMIDeskDlg::~CMIDeskDlg()
{
 _MetCleanUp();
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
  m_leds.SetItemState(idx, *it->second.second, true);
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

void CMIDeskDlg::Resize(const CRect& i_rect)
{
 bool redraw = false;
 
 //meters
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Resize(_GetMetItemRect(it->first), redraw);

 //Indicators
 CRect rect = i_rect;
 rect.top = rect.bottom - MathHelpers::Round(rect.Height() * IndHeightCoeff);
 m_leds.MoveWindow(rect, redraw);

 RedrawWindow();
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
 {
  CRect rect;
  GetClientRect(&rect);
  if (!rect.IsRectNull())
   Resize(rect);
 }
}

void CMIDeskDlg::SetIndicatorsCfg(int IndRows, int IndGas_v, int IndCarb, int IndIdleValve, int IndPowerValve, int StBlock, int AE, int CoolingFan)
{
 m_leds.SetNumRows(IndRows);
 m_indFields.clear();

 if (IndGas_v != std::numeric_limits<int>::max())
  m_indFields.insert(std::make_pair(IndGas_v, std::make_pair(MLL::GetString(IDS_MI_IND_GAS_V), &m_values.gas)));

 if (IndCarb != std::numeric_limits<int>::max())
  m_indFields.insert(std::make_pair(IndCarb, std::make_pair(MLL::GetString(IDS_MI_IND_CARB), &m_values.carb)));

 if (IndIdleValve != std::numeric_limits<int>::max())
  m_indFields.insert(std::make_pair(IndIdleValve, std::make_pair(MLL::GetString(IDS_MI_IND_IDLEVALVE), &m_values.ephh_valve)));

 if (IndPowerValve != std::numeric_limits<int>::max())
  m_indFields.insert(std::make_pair(IndPowerValve, std::make_pair(MLL::GetString(IDS_MI_IND_POWERVALVE), &m_values.epm_valve)));

 if (StBlock != std::numeric_limits<int>::max())
  m_indFields.insert(std::make_pair(StBlock, std::make_pair(MLL::GetString(IDS_MI_IND_STBLOCK), &m_values.st_block)));

 if (AE != std::numeric_limits<int>::max())
  m_indFields.insert(std::make_pair(AE, std::make_pair(MLL::GetString(IDS_MI_IND_AE), &m_values.acceleration)));

 if (CoolingFan != std::numeric_limits<int>::max())
  m_indFields.insert(std::make_pair(CoolingFan, std::make_pair(MLL::GetString(IDS_MI_IND_COOLINGFAN), &m_values.cool_fan)));

 m_leds.Clear();
 IndFields_t::iterator it;
 for(it = m_indFields.begin(); it != m_indFields.end(); ++it)
  m_leds.AddItem(it->second.first.c_str());
}

void CMIDeskDlg::SetMetersCfg(int MetRows, int MetRPM, int MetMAP, int MetVBat, int MetIgnTim, int MetCLT, int MetAddI1, int MetAddI2,
                              int MetInjPW, int MetIAT, int MetEGOCorr, int MetTPS, int MetAirFlow, int MetVehicleSpeed, int MetTPSDot,
                              int MetMAP2, int MetMAPD, int MetTmp2, int MetFuelConsum, int MetKnockRetard, int MetKnockGraph)
{
 m_metRows = MetRows;

 //destroy current widgets and clear list
 _MetCleanUp();
 
 if (MetRPM != std::numeric_limits<int>::max())
 {
  CMITachometer* widget = new CMITachometer();
  widget->Create(this);
  widget->BindVars(&m_ringRPM.m_result, &m_values.speed, &m_values.distance);
  widget->ShowTLP(m_showSpeedAndDistance);
  widget->ShowTRP(m_showSpeedAndDistance);
  widget->SetPaneUnit(m_speedUnit, m_distanceUnit);
  widget->SetLimits(0, (float)m_tachoMax);
  m_metFields.insert(std::make_pair(MetRPM, widget));
 }

 if (MetMAP != std::numeric_limits<int>::max())
 {
  CMIPressure* widget = new CMIPressure();
  widget->Create(this);
  widget->BindVars(&m_ringMAP.m_result, NULL, NULL);
  widget->SetLimits(10, (float)m_pressMax);
  m_metFields.insert(std::make_pair(MetMAP, widget));
 }

 if (MetVBat != std::numeric_limits<int>::max())
 {
  CMIVoltmeter* widget = new CMIVoltmeter();
  widget->Create(this);
  widget->BindVars(&m_ringVBat.m_result, NULL, NULL);
  m_metFields.insert(std::make_pair(MetVBat, widget));
 }

 if (MetIgnTim != std::numeric_limits<int>::max())
 {
  CMIDwellAngle* widget = new CMIDwellAngle();
  widget->Create(this);
  widget->BindVars(&m_values.adv_angle, NULL, NULL);  
  m_metFields.insert(std::make_pair(MetIgnTim, widget));
 }

 if (MetCLT != std::numeric_limits<int>::max())
 {
  CMITemperature* widget = new CMITemperature();
  widget->Create(this);
  widget->BindVars(&m_values.temperat, &m_values.gasdose_pos, &m_values.choke_pos);  
  widget->ShowTLP(m_showGDPos);
  widget->ShowTRP(m_showChokePos);
  m_metFields.insert(std::make_pair(MetCLT, widget));
  mp_miTemperat = widget;
 }
 else
  mp_miTemperat = NULL;

 if (MetAddI1 != std::numeric_limits<int>::max())
 {
  CMIAddI1* widget = new CMIAddI1();
  widget->Create(this);
  widget->BindVars(&m_ringAddI1.m_result, NULL, NULL);  
  m_metFields.insert(std::make_pair(MetAddI1, widget));
 }

 if (MetAddI2 != std::numeric_limits<int>::max())
 {
  CMIAddI2* widget = new CMIAddI2();
  widget->Create(this);
  widget->BindVars(&m_values.add_i2, NULL, NULL);  
  m_metFields.insert(std::make_pair(MetAddI2, widget));
 }

 if (MetInjPW != std::numeric_limits<int>::max())
 {
  CMIInjPW* widget = new CMIInjPW();
  widget->Create(this);
  widget->BindVars(&m_values.inj_pw, NULL, NULL);  
  m_metFields.insert(std::make_pair(MetInjPW, widget));
 }

 if (MetIAT != std::numeric_limits<int>::max())
 {
  CMIIAT* widget = new CMIIAT();
  widget->Create(this);
  widget->BindVars(&m_values.air_temp, NULL, NULL);  
  m_metFields.insert(std::make_pair(MetIAT, widget));
 }

 if (MetEGOCorr != std::numeric_limits<int>::max())
 {
  CMIEGOCorr* widget = new CMIEGOCorr();
  widget->Create(this);
  widget->BindVars(&m_values.lambda_corr, NULL, NULL);  
  m_metFields.insert(std::make_pair(MetEGOCorr, widget));
 }

 if (MetTPS != std::numeric_limits<int>::max())
 {
  CMIThrottleGate* widget = new CMIThrottleGate();
  widget->Create(this);
  widget->BindVars(&m_ringTPS.m_result, &m_air_flow, NULL);
  widget->ShowTLP(true);
  m_metFields.insert(std::make_pair(MetTPS, widget));
 }

 if (MetAirFlow != std::numeric_limits<int>::max())
 {
  CMIAirFlow* widget = new CMIAirFlow();
  widget->Create(this);
  widget->BindVars(&m_air_flow, NULL, NULL);
  m_metFields.insert(std::make_pair(MetAirFlow, widget));
 }

 if (MetVehicleSpeed != std::numeric_limits<int>::max())
 {
  CMIVehicleSpeed* widget = new CMIVehicleSpeed();
  widget->Create(this);
  widget->BindVars(&m_values.speed, NULL, NULL);
  widget->SetMeterUnit(m_speedUnit);
  m_metFields.insert(std::make_pair(MetVehicleSpeed, widget));
 }

 if (MetTPSDot != std::numeric_limits<int>::max())
 {
  CMITPSDot* widget = new CMITPSDot();
  widget->Create(this);
  widget->BindVars(&m_tps_dot, NULL, NULL);
  m_metFields.insert(std::make_pair(MetTPSDot, widget));
 }

 if (MetMAP2 != std::numeric_limits<int>::max())
 {
  CMIMap2* widget = new CMIMap2();
  widget->Create(this);
  widget->BindVars(&m_values.map2, NULL, NULL);
  m_metFields.insert(std::make_pair(MetMAP2, widget));
 }

 if (MetMAPD != std::numeric_limits<int>::max())
 {
  CMIMapD* widget = new CMIMapD();
  widget->Create(this);
  widget->BindVars(&m_values.mapd, NULL, NULL);
  m_metFields.insert(std::make_pair(MetMAPD, widget));
 }

 if (MetTmp2 != std::numeric_limits<int>::max())
 {
  CMITemp2* widget = new CMITemp2();
  widget->Create(this);
  widget->BindVars(&m_values.tmp2, NULL, NULL);
  m_metFields.insert(std::make_pair(MetTmp2, widget));
 }

 if (MetFuelConsum != std::numeric_limits<int>::max())
 {
  CMIFuelConsum* widget = new CMIFuelConsum();
  widget->Create(this);
  widget->BindVars(&m_values.inj_ffd, NULL, NULL);
  m_metFields.insert(std::make_pair(MetFuelConsum, widget));
 }

 if (MetKnockRetard != std::numeric_limits<int>::max())
 {
  CMIKnockRetard* widget = new CMIKnockRetard();
  widget->Create(this);
  widget->BindVars(&m_values.knock_retard, NULL, NULL);
  m_metFields.insert(std::make_pair(MetKnockRetard, widget));
 }

 if (MetKnockGraph != std::numeric_limits<int>::max())
 {
  CMIKnockGraph* widget = new CMIKnockGraph();
  widget->Create(this);
  widget->BindVars(&m_values.knock_k, NULL, NULL);
  m_metFields.insert(std::make_pair(MetKnockGraph, widget));
 }

 //enable/disable
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Enable(m_enabled, false);

 CRect rect;
 GetClientRect(&rect);
 Resize(rect);
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
 CRect rc;
 GetClientRect(&rc);
 rc.bottom = rc.bottom - MathHelpers::Round(rc.Height() * IndHeightCoeff);
 int space  = 0;
 float width  = ((float)(rc.right - rc.left) - ((float)cols)*space) / ((float)cols);
 float height = ((float)(rc.bottom - rc.top) - ((float)m_metRows)*space) / ((float)m_metRows);
 return CRect(MathHelpers::Round(j*(width+space)), MathHelpers::Round(i*(height+space)), MathHelpers::Round(j*(width+space) + width), MathHelpers::Round(i*(height+space) + height));
}
