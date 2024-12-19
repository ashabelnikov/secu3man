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

/** \file MAPCalcController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "MAPCalcController.h"

#include <math.h>
#include "common/fastdelegate.h"
#include "MAPCalcDlg.h"
#include "Resources/resource.h"
#include "ui-core/MsgBox.h"

using namespace fastdelegate;

//Define maximum and minimum allowed values (in the SI form)
static const float pressMin = 0.0f;       //Pa
static const float voltMin  = 0.0f;       //V
static const float voltMax  = 5.5f;       //V
static const float gradMin  = 1000.0f;    //Pa/V
static const float gradMax  = 500000.0f;  //Pa/V
static const float pressStp = 100.0f;     //pressure step (Pa)
static const float voltStp  = 0.001f;     //voltage step (V)

CMAPCalcController::CMAPCalcController(VIEW* ip_view, float i_offset, float i_gradient)
: mp_view(ip_view)
, m_know_gradient(true)
, m_1stptPressUnit(PU_KPA)
, m_1stptVoltUnit(VU_V)
, m_2ndptPressUnit(PU_KPA)
, m_2ndptVoltUnit(VU_V)
, m_gradUnitN(PU_KPA)
, m_gradUnitD(VU_V)
, m_iniOffset(i_offset)
, m_iniGradient(i_gradient)
, m_pressMax(500000.0f) //Pa
{
 //fill pressure units
 m_p.insert(std::make_pair(PU_PA,  MLL::GetString(IDS_PA_UNIT)));
 m_p.insert(std::make_pair(PU_KPA, MLL::GetString(IDS_KPA_UNIT)));
 m_p.insert(std::make_pair(PU_BAR, MLL::GetString(IDS_BAR_UNIT)));
 m_p.insert(std::make_pair(PU_MMHG,MLL::GetString(IDS_MMHG_UNIT)));
 m_p.insert(std::make_pair(PU_AT,  MLL::GetString(IDS_AT_UNIT)));
 m_p.insert(std::make_pair(PU_PSI, MLL::GetString(IDS_PSI_UNIT)));
 //fill voltage units
 m_v.insert(std::make_pair(VU_MV, MLL::GetString(IDS_MV_UNIT)));
 m_v.insert(std::make_pair(VU_V,  MLL::GetString(IDS_V_UNIT)));
}

/*static*/bool CMAPCalcController::Calculate(float& o_offset, float& o_gradient, float* ip_pressMax /*= NULL*/)
{
 CMAPCalcDlg dlg;
 CMAPCalcController cntr(&dlg, o_offset, o_gradient);
 if (ip_pressMax)
  cntr.m_pressMax = *ip_pressMax;
 dlg.setOnKnowGradientCheck(MakeDelegate(&cntr, &CMAPCalcController::OnKnowGradientCheck));
 dlg.setOnActivate(MakeDelegate(&cntr, &CMAPCalcController::OnActivate));
 dlg.setOn1stptPressUnit(MakeDelegate(&cntr, &CMAPCalcController::On1stptPressUnit));
 dlg.setOn1stptVoltUnit(MakeDelegate(&cntr, &CMAPCalcController::On1stptVoltUnit));
 dlg.setOn2ndptPressUnit(MakeDelegate(&cntr, &CMAPCalcController::On2ndptPressUnit));
 dlg.setOn2ndptVoltUnit(MakeDelegate(&cntr, &CMAPCalcController::On2ndptVoltUnit));
 dlg.setOnGradientUnit(MakeDelegate(&cntr, &CMAPCalcController::OnGradientUnitN), MakeDelegate(&cntr, &CMAPCalcController::OnGradientUnitD));
 dlg.setOnOK(MakeDelegate(&cntr, &CMAPCalcController::OnOK));
 typedef std::pair<float, UnitId> PtValue;
 typedef struct{float grad; UnitId n_unit; UnitId d_unit;} GrValue;

 if (IDOK == dlg.DoModal())
 {
  if (cntr.m_know_gradient)
  {
   PtValue p1(cntr.mp_view->Get1stptPress(), cntr.m_1stptPressUnit);
   PtValue v1(cntr.mp_view->Get1stptVolt(), cntr.m_1stptVoltUnit);
   GrValue g; g.grad = cntr.mp_view->GetGradient(), g.n_unit = cntr.m_gradUnitN, g.d_unit = cntr.m_gradUnitD;  
    
   //first of all, convert gradient to SI form (Pa/V)
   float gradValue;
   ASSERT(g.n_unit != g.d_unit);
   if (cntr._IsPressureUnit(g.n_unit)) //P/V
    gradValue = cntr._ConvertUnit(g.grad, g.n_unit, true) / cntr._ConvertUnit(1.0f, g.d_unit, true);
   else //V/P
    gradValue = cntr._ConvertUnit(1.0f, g.d_unit, true) / cntr._ConvertUnit(g.grad, g.n_unit, true);
   
   //convert pressure and voltage of the 1st point to SI form (Pa,V)
   float p1Press = cntr._ConvertUnit(p1.first, p1.second, true);
   float p1Volt = cntr._ConvertUnit(v1.first, v1.second, true);

   //calculate offset, and convert results to kPa
   o_offset = ((p1Press / gradValue) - p1Volt);
   o_gradient = gradValue / 1000.0f;
  }
  else
  {
   PtValue p1(cntr.mp_view->Get1stptPress(), cntr.m_1stptPressUnit);
   PtValue v1(cntr.mp_view->Get1stptVolt(), cntr.m_1stptVoltUnit);
   PtValue p2(cntr.mp_view->Get2ndptPress(), cntr.m_2ndptPressUnit);
   PtValue v2(cntr.mp_view->Get2ndptVolt(), cntr.m_2ndptVoltUnit);

   //first of all, convert values to SI form  (Pa, V)
   float p1Press = cntr._ConvertUnit(p1.first, p1.second, true);
   float p1Volt = cntr._ConvertUnit(v1.first, v1.second, true);
   float p2Press = cntr._ConvertUnit(p2.first, p2.second, true);
   float p2Volt = cntr._ConvertUnit(v2.first, v2.second, true);
   
   //then, calculate gradient (Pa/V) basing on two points
   float gradValue = (p2Press - p1Press) / (p2Volt - p1Volt);

   //calculate offset, and convert results to kPa
   o_offset = ((p1Press / gradValue) - p1Volt);
   o_gradient = gradValue / 1000.0f;
  }
  return true;
 }
 return false;
}

void CMAPCalcController::OnKnowGradientCheck()
{
 bool value = mp_view->GetKnownGradientCheck();
 if (value)
 {
  mp_view->EnableGradientCtrls(true);
  mp_view->Enable2ndptCtrls(false);
 }
 else
 {
  mp_view->EnableGradientCtrls(false);
  mp_view->Enable2ndptCtrls(true);
 }
 m_know_gradient = value;
}

void CMAPCalcController::OnActivate()
{
 UnitMap m_pv;
 m_pv.insert(m_p.begin(), m_p.end());
 m_pv.insert(m_v.begin(), m_v.end());

 //Fill unit combo boxes
 mp_view->Fill1stptPressUnit(m_p);
 mp_view->Fill1stptVoltUnit(m_v);
 mp_view->Fill2ndptPressUnit(m_p);
 mp_view->Fill2ndptVoltUnit(m_v);
 mp_view->FillGradientUnit(m_pv, m_pv);

 //set default state of "know gradient" check box
 mp_view->SetKnownGradientCheck(m_know_gradient);
 mp_view->EnableGradientCtrls(m_know_gradient);
 mp_view->Enable2ndptCtrls(!m_know_gradient);

 float v = 1.0; //V
 float p = (v + m_iniOffset) * m_iniGradient; //kPa
 mp_view->Set1stptPress(p, m_1stptPressUnit);
 mp_view->Set1stptVolt(v, m_1stptVoltUnit);
 mp_view->Set2ndptPress(p, m_2ndptPressUnit);
 mp_view->Set2ndptVolt(v, m_2ndptVoltUnit);
 mp_view->SetGradient(m_iniGradient, m_gradUnitN, m_gradUnitD);

 //set default limits
 On1stptPressUnit(m_1stptPressUnit);
 On1stptVoltUnit(m_1stptVoltUnit);
 On2ndptPressUnit(m_2ndptPressUnit);
 On2ndptVoltUnit(m_2ndptVoltUnit);
 OnGradientUnitN(m_gradUnitN, m_gradUnitD);
}

bool CMAPCalcController::OnOK()
{
 if ((mp_view->Get1stptVolt() >= mp_view->Get2ndptVolt()) && !m_know_gradient)
 {
  SECUMessageBox(MLL::GetString(IDS_MAP_CALC_1V_ABOVE_2V).c_str());
  return false; //user warned and made to eliminate error, so do not exit
 }
 return true; //ok
}

void CMAPCalcController::On1stptPressUnit(UnitId id)
{
 float minVal, maxVal; int dp;
 _GetLimitAndDP(id, minVal, maxVal, dp);
 mp_view->SetPressLimits(minVal, maxVal, dp);
 m_1stptPressUnit = id;
}
void CMAPCalcController::On1stptVoltUnit(UnitId id)
{
 float minVal, maxVal; int dp;
 _GetLimitAndDP(id, minVal, maxVal, dp);
 mp_view->SetVoltLimits(minVal, maxVal, dp);
 m_1stptVoltUnit = id;
}
void CMAPCalcController::On2ndptPressUnit(UnitId id)
{
 float minVal, maxVal; int dp;
 _GetLimitAndDP(id, minVal, maxVal, dp);
 mp_view->SetPressLimits(minVal, maxVal, dp);
 m_2ndptPressUnit = id;
}
void CMAPCalcController::On2ndptVoltUnit(UnitId id)
{
 float minVal, maxVal; int dp;
 _GetLimitAndDP(id, minVal, maxVal, dp);
 mp_view->SetVoltLimits(minVal, maxVal, dp);
 m_2ndptVoltUnit = id;
}
void CMAPCalcController::OnGradientUnitN(UnitId n_id, UnitId d_id)
{
 if ((_IsPressureUnit(n_id) && _IsPressureUnit(d_id)) || (!_IsPressureUnit(n_id) && !_IsPressureUnit(d_id)))
 {
  mp_view->SetGradientUnit(n_id, m_gradUnitN);
  d_id = m_gradUnitN;
 }
 m_gradUnitN = n_id, m_gradUnitD = d_id;

 float minVal, maxVal; int dp;
 _GetLimitAndDPG(n_id, d_id, minVal, maxVal, dp);
 mp_view->SetGradLimits(minVal, maxVal, dp);
}

void CMAPCalcController::OnGradientUnitD(UnitId n_id, UnitId d_id)
{
 if ((_IsPressureUnit(n_id) && _IsPressureUnit(d_id)) || (!_IsPressureUnit(n_id) && !_IsPressureUnit(d_id)))
 {
  mp_view->SetGradientUnit(m_gradUnitD, d_id);
  n_id = m_gradUnitD;  
 }
 m_gradUnitN = n_id, m_gradUnitD = d_id;

 float minVal, maxVal; int dp;
 _GetLimitAndDPG(n_id, d_id, minVal, maxVal, dp);
 mp_view->SetGradLimits(minVal, maxVal, dp);
}

void CMAPCalcController::_GetLimitAndDP(UnitId i_unit, float& o_min, float& o_max, int& o_dp)
{
 if (_IsPressureUnit(i_unit))
 {
  o_min = _ConvertUnit(pressMin, i_unit);
  o_max = _ConvertUnit(m_pressMax, i_unit);
 }
 else //voltage
 {
  o_min = _ConvertUnit(voltMin, i_unit);
  o_max = _ConvertUnit(voltMax, i_unit);
 }
 o_dp  = _GetDecimalPlaces(i_unit);
}

void CMAPCalcController::_GetLimitAndDPG(UnitId i_n_unit, UnitId i_d_unit, float& o_min, float& o_max, int& o_dp)
{
 ASSERT(i_n_unit != i_d_unit);
 if (_IsPressureUnit(i_n_unit)) //P/V
 {
  o_min = _ConvertUnit(gradMin, i_n_unit) / _ConvertUnit(1.0f, i_d_unit);
  o_max = _ConvertUnit(gradMax, i_n_unit) / _ConvertUnit(1.0f, i_d_unit);
  o_dp  = _GetDecimalPlaces(i_n_unit);
  if (i_d_unit == VU_MV) o_dp+=3;
 }
 if (_IsPressureUnit(i_d_unit)) //V/P
 {
  o_max = _ConvertUnit(1.0f, i_n_unit) / _ConvertUnit(gradMin, i_d_unit);
  o_min = _ConvertUnit(1.0f, i_n_unit) / _ConvertUnit(gradMax, i_d_unit);
  o_dp  = _GetDecimalPlaces(i_d_unit);
  if (i_n_unit == VU_MV) o_dp-=3;
  if (o_dp < 0) o_dp = 0;
 }
}

// if dir == true, then value of specified unit will be converted to SI from
// if dir == false, then SI form value will be converted to specified unit
float CMAPCalcController::_ConvertUnit(float i_value, UnitId i_unit, bool dir /*=false*/) const
{
 switch(i_unit)
 {
  case PU_PA:  //Pa   <--> Pa
   return i_value; //nothing to do
  case VU_V:   //V    <--> V
   return i_value; //nothing to do
  case PU_KPA: //kPa  <--> Pa
   return (float)(dir ? (i_value * (1.0 * 1000.0))      : (i_value / (1.0 * 1000.0)));
  case PU_BAR: //bar  <--> Pa
   return (float)(dir ? (i_value * (1.0 * 100000.0))    : (i_value / (1.0 * 100000.0)));
  case PU_MMHG://mmHg <--> Pa
   return (float)(dir ? (i_value / (7.5006 * 0.001))    : (i_value * (7.5006 * 0.001)));
  case PU_AT:  //at   <--> Pa
   return (float)(dir ? (i_value / (10.197 * 0.000001)) : (i_value * (10.197 * 0.000001)));
  case PU_PSI: //psi  <--> Pa
   return (float)(dir ? (i_value / (145.04 * 0.000001)) : (i_value * (145.04 * 0.000001)));
  case VU_MV:  //mV   <--> V
   return (float)(dir ? (i_value / (1.0 * 1000.0))      : (i_value * (1.0 * 1000.0)));
 }
 return .0f; //WTF case
}

int CMAPCalcController::_GetDecimalPlaces(UnitId i_unit) const
{  
 double step, intPart = 0;
 if (_IsPressureUnit(i_unit))
  step = _ConvertUnit(pressStp, i_unit); //pressure
 else
  step = _ConvertUnit(voltStp, i_unit);  //voltage
 //find out number of zero decimal places after the point. (e.g. for 1.045, n = 2)
 double fracPart = modf(step, &intPart);
 int n = 0;
 while(fracPart < 1.0 && n < 10)
 {
  fracPart*=10, ++n;
 }
 return n;
}

bool CMAPCalcController::_IsPressureUnit(UnitId i_unit) const
{ 
 return (m_p.find(i_unit)!=m_p.end());
}
