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

/** \file LdaxCfg.cpp
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "stdafx.h"
#include "LdaxCfg.h"
#include "resource.h"
#include "common/langlayer.h"
#include "common/MathHelpers.h"

LdaxCfg::LdaxCfg()
: m_ldaxUseTable(false)
, m_ldaxMin(1.0f)
, m_ldaxMax(16.0f)
, m_ldaxCfg(0)
, m_ldaxNeedsUpdate(false)
, m_baroPress(101.3f) //sea level atmospheric pressure by default
{
 m_ldaxTitle[0] = MLL::LoadString(IDS_MAPS_LOADKPA_UNIT);
 m_ldaxTitle[1] = MLL::LoadString(IDS_MAPS_LOADPRC_UNIT);

 m_work_map_load_slots.reserve(32);
 m_work_map_load_slots = MathHelpers::BuildGridFromRange(1.0f, 16.0f, 16);
}

void LdaxCfg::SetLoadAxisCfg(float minVal, float maxVal, int ldaxCfg, bool useTable, bool forceUpdate /*= false*/)
{
 if (!MathHelpers::IsEqualFlt(m_ldaxMin, minVal, 0.01f) || !MathHelpers::IsEqualFlt(m_ldaxMax, maxVal, 0.01f) ||
     (m_ldaxCfg != ldaxCfg) || (m_ldaxUseTable != useTable))
  m_ldaxNeedsUpdate = true;

 m_ldaxMin = minVal, m_ldaxMax = maxVal;
 m_ldaxCfg = ldaxCfg;
 m_ldaxUseTable = useTable;
}

bool LdaxCfg::ldaxWatchBaroPress(float newValue)
{
 bool changed = (ldaxIsUseBaroMax() && !MathHelpers::IsEqualFlt(m_baroPress, newValue, 0.01f));
 m_baroPress = newValue;
 return changed;
}
