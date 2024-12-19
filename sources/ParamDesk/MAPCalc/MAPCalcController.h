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

/** \file MAPCalcController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>

class CMAPCalcDlg;

class CMAPCalcController
{
  typedef CMAPCalcDlg VIEW;
  typedef int UnitId;
  typedef std::map<UnitId, _TSTRING> UnitMap;

  //units (pressure and voltage)
  enum Unit
  {
   PU_PA   = 0,  //Pa
   PU_KPA  = 1,  //kPa
   PU_BAR  = 2,  //bar
   PU_MMHG = 3,  //mm Hg
   PU_AT   = 4,  //at
   PU_PSI  = 5,  //psi
   VU_MV   = 6,  //mV
   VU_V    = 7   //V
  };

 public:
  CMAPCalcController(VIEW* ip_view, float i_offset, float i_gradient);

  //This function will always return offset - in Volts, gradient - in kPa/V
  //Return values: true - user accepted calculation, false - user canceled calculation
  static bool Calculate(float& o_offset, float& o_gradient, float* ip_pressMax = NULL);

 private:
  void OnKnowGradientCheck();
  void OnActivate();
  bool OnOK();
  void On1stptPressUnit(UnitId id);
  void On1stptVoltUnit(UnitId id);
  void On2ndptPressUnit(UnitId id);
  void On2ndptVoltUnit(UnitId id);
  void OnGradientUnitN(UnitId n_id, UnitId d_id);
  void OnGradientUnitD(UnitId n_id, UnitId d_id);
  
  void _GetLimitAndDP(UnitId i_unit, float& o_min, float& o_max, int& o_dp);
  void _GetLimitAndDPG(UnitId i_n_unit, UnitId i_d_unit, float& o_min, float& o_max, int& o_dp);
  float _ConvertUnit(float i_value, UnitId i_Unit, bool dir = false) const;
  int _GetDecimalPlaces(UnitId i_unit) const;
  bool _IsPressureUnit(UnitId i_unit) const;

  CMAPCalcDlg* mp_view;
  UnitMap m_p; //pressure units
  UnitMap m_v; //voltage units
  UnitId m_gradUnitN;
  UnitId m_gradUnitD;
  UnitId m_1stptPressUnit;
  UnitId m_1stptVoltUnit;
  UnitId m_2ndptPressUnit;
  UnitId m_2ndptVoltUnit;
  bool m_know_gradient;
  const float m_iniOffset;   //initial offset (V)
  const float m_iniGradient; //initial gradient (kPa/V)
  float m_pressMax;
};
