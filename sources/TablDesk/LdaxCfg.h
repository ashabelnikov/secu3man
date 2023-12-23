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

/** \file LdaxCfg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include <vector>

class LdaxCfg
{
 protected:
  LdaxCfg();

  //Updates configuration of the load axis and sets flag if changes of configuration were detected
  virtual void SetLoadAxisCfg(float minVal, float maxVal, int ldaxCfg, bool useTable, bool forceUpdate = false) = 0;

  //Cheks if MAP(baro) was selected as load source.
  //returns true if barometer value is selected for use instead of 'm_ldaxMax' constant)
  bool ldaxIsUseBaroMax(void) { return m_ldaxCfg == 1; }

  //get title depending on current load source (kPa or %)
  const CString& ldaxGetTitle(void) { return m_ldaxTitle[(m_ldaxCfg > 1)]; }

  //Cpmpares new barometer value with stored one and then updated stored value.
  //returns true if barometer value changed. If MAP(baro) was not selected as
  //load source, this function will always return false
  bool ldaxWatchBaroPress(float newValue);

  //Gets maximum value for load axis depending on load source
  //returns current barometer value if MAP(baro) was selected, otherwise returns constant value
  float ldaxGetBaroMax(void) { return ldaxIsUseBaroMax() ? m_baroPress : m_ldaxMax; }

  float ldaxGetMax(void) const { return m_ldaxMax; }
  float ldaxGetMin(void) const { return m_ldaxMin; }
  int   ldaxGetCfg(void) const { return m_ldaxCfg; }
  bool  ldaxIsTable(void) const { return m_ldaxUseTable; }

  bool m_ldaxNeedsUpdate;
  std::vector<float> m_work_map_load_slots; //dynamically generated load grid

 private:
  //load axis
  float m_ldaxMin;        //min. value on the load axis (ignored if table is used)
  float m_ldaxMax;        //max. value on the load axis (ignored if table is used) 
  int   m_ldaxCfg;        //load source selection   
  bool  m_ldaxUseTable;   //use table instead of range

  float m_baroPress;      //current barometer pressure
  CString m_ldaxTitle[2]; //preloaded strings
};
