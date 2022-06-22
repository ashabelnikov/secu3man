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

/** \file Magnitude.h
 * \author Alexey A. Shabelnikov
 */

namespace SECU3IO {

static const float ADC_DISCRETE = 0.0025f;   //Вольт
static const float ANGLE_MULTIPLIER = 32.0f; //коэффициент масштабирования углов поворота коленвала
static const int MAP_PHYSICAL_MAGNITUDE_MULTIPLIER = 64;
static const int UBAT_PHYSICAL_MAGNITUDE_MULTIPLIER = 400;
static const int TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER = 4;
static const int TPS_PHYSICAL_MAGNITUDE_MULTIPLIER  = 2;
static const int CHOKE_PHYSICAL_MAGNITUDE_MULTIPLIER = 2;
static const int GD_PHYSICAL_MAGNITUDE_MULTIPLIER = 2;
static const int AFR_PHYSICAL_MAGNITUDE_MULTIPLIER = 128;
static const int LOAD_PHYSICAL_MAGNITUDE_MULTIPLIER = 64;
static const float PARINJTIM_MULT = 16.0f;
static const float FTLS_MULT = 64.0f;
static const float EGTS_MULT = 4.0f;
static const float OPS_MULT = 256.0f;
static const float INJPWCOEF_MULT = 4096.0f;
static const float MAFS_MULT = 64.0f;

static const float AA_MAPS_M_FACTOR = 2.0f; //scale factor for adv. angle maps (they are stored in integer format)
static const float VE_MAPS_M_FACTOR = 2048.0f;
static const float AFR_MAPS_M_FACTOR = 16.0f;
static const float WRMP_MAPS_M_FACTOR = (128.0f * 0.01f);
static const float IACPOS_MAPS_M_FACTOR = 2.0f;
static const float AETPSB_MAPS_M_FACTOR = 0.1f;
static const float AETPSV_MAPS_ADDER = 55.0f;
static const float AERPMB_MAPS_M_FACTOR = 0.01f;
static const float AERPMV_MAPS_M_FACTOR = (128.0f * 0.01f);
static const float AFTSTR_MAPS_M_FACTOR = (128.0f * 0.01f);
static const float GD_MAPS_M_FACTOR = 2.0f;
static const float EGO_CURVE_M_FACTOR = 128.0f;
static const float IT_MAPS_M_FACTOR = 2.0f;
static const float BAROCORR_MAPS_M_FACTOR = 4096.0f;
static const float BAROCORR_MAPSX_M_FACTOR = 64.0f;
static const float TPSSWT_MAPS_M_FACTOR = 2.0f;
static const float PWMIAC_UCOEF_MAPS_M_FACTOR = 4096.0f;
static const float MAF_FLOW_CURVE_MULT = 64.0f;
static const float TPSZON_MAPS_M_FACTOR = 2.0f;
static const float AEMAPB_MAPS_M_FACTOR = 0.1f;
static const float AEMAPV_MAPS_ADDER = 55.0f;
};
