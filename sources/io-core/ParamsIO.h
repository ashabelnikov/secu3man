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

/** \file ParamsIO.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include "PlatformParamHolder.h"

#ifdef PARAMSIO_EXPORTS
#define PARAMSIO_API __declspec(dllexport)
#else
#define PARAMSIO_API __declspec(dllimport)
#endif

namespace SECU3IO { struct params_t; }

class PARAMSIO_API ParamsIO
{
public:
 ParamsIO();
 virtual ~ParamsIO();

 //-----------------------------------------------------------------------
 virtual bool SetDefParamValues(BYTE i_descriptor, const void* ip_values);
 virtual bool GetDefParamValues(BYTE i_descriptor, void* op_values);
 //-----------------------------------------------------------------------

 void SetNumPulsesPer1Km(int pp1km);
 void SetQuartzFrq(long frq);

protected:
 virtual SECU3IO::params_t* GetParamsPtr(void) = 0;
 virtual EECUPlatform GetPlatformId(void) = 0;

 float m_period_distance;              //distance of one period in meters (speed sensor), used in calculations
 long m_quartz_frq;                    //MCU clock frequency
};
