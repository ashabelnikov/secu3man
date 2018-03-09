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

/** \file ce_errors.h
 * \author Alexey A. Shabelnikov
 */

#ifndef _CE_ERRORS_H_
#define _CE_ERRORS_H_

//define error bits (Check Engine)
#define ECUERROR_CKPS_MALFUNCTION     0
#define ECUERROR_EEPROM_PARAM_BROKEN  1
#define ECUERROR_PROGRAM_CODE_BROKEN  2
#define ECUERROR_KSP_CHIP_FAILED      3
#define ECUERROR_KNOCK_DETECTED       4
#define ECUERROR_MAP_SENSOR_FAIL      5
#define ECUERROR_TEMP_SENSOR_FAIL     6
#define ECUERROR_VOLT_SENSOR_FAIL     7
#define ECUERROR_DWELL_CONTROL        8
#define ECUERROR_CAMS_MALFUNCTION     9
#define ECUERROR_TPS_SENSOR_FAIL     10
#define ECUERROR_ADD_I1_SENSOR       11
#define ECUERROR_ADD_I2_SENSOR       12
#define ECUERROR_ADD_I3_SENSOR       13
#define ECUERROR_ADD_I4_SENSOR       14
#define ECUERROR_SYS_START           15

const int SECU3_CE_ERRCODES_COUNT   = 16;
 //<bitnumber, error code>
#define _SD std::pair<int, int>
 static std::pair<int, int> secu3_ce_error_codes[SECU3_CE_ERRCODES_COUNT] =
 {
  _SD(ECUERROR_CKPS_MALFUNCTION,    21), //sensor
  _SD(ECUERROR_EEPROM_PARAM_BROKEN, 13),
  _SD(ECUERROR_PROGRAM_CODE_BROKEN, 14),
  _SD(ECUERROR_KSP_CHIP_FAILED,     31),
  _SD(ECUERROR_KNOCK_DETECTED,      32),
  _SD(ECUERROR_MAP_SENSOR_FAIL,     22), //sensor
  _SD(ECUERROR_TEMP_SENSOR_FAIL,    23), //sensor
  _SD(ECUERROR_VOLT_SENSOR_FAIL,    24), //sensor
  _SD(ECUERROR_DWELL_CONTROL,       41),
  _SD(ECUERROR_CAMS_MALFUNCTION,    25), //sensor
  _SD(ECUERROR_TPS_SENSOR_FAIL,     26), //sensor
  _SD(ECUERROR_ADD_I1_SENSOR,       27), //sensor
  _SD(ECUERROR_ADD_I2_SENSOR,       28), //sensor
  _SD(ECUERROR_ADD_I3_SENSOR,       51), //sensor
  _SD(ECUERROR_ADD_I4_SENSOR,       52), //sensor
  _SD(ECUERROR_SYS_START,           00), //not actually an error
 };
#undef _SD

#endif //_CE_ERRORS_H_
