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

/** \file MapIds.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

enum EMapTypes
{
 ETMT_NAME_STR      = 0,  //name of tables's set, not a map

 ETMT_IGN_START     = 1,  //ignition
 ETMT_IGN_IDLE      = 2,  //ignition
 ETMT_IGN_WORK      = 3,  //ignition
 ETMT_IGN_TEMP      = 4,  //ignition
 ETMT_IGN_TEMPI     = 5,  //ignition temp.corr. map (idling)

 ETMT_INJ_VE        = 6,  // VE
 ETMT_INJ_AFR       = 7,  // AFR
 ETMT_INJ_CRNK      = 8,  // Cranking PW
 ETMT_INJ_WRMP      = 9,  // Warmup enrichment
 ETMT_INJ_DEAD      = 10, // Injector dead time
 ETMT_INJ_IDLR      = 11, // IAC run pos
 ETMT_INJ_IDLC      = 12, // IAC cranking pos
 ETMT_INJ_AETPS     = 13, // AE TPS map
 ETMT_INJ_AERPM     = 14, // AE RPM map
 ETMT_INJ_AFTSTR    = 15, // afterstart enrichment
 ETMT_INJ_IT        = 16, // injection timing
 ETMT_INJ_ITRPM     = 17, // Idling RPM
 ETMT_INJ_RIGID     = 18, // Idl. regulator's rigidity
 ETMT_INJ_EGOCRV    = 19, // EGO curve (WBO sensor emulation)
 ETMT_INJ_IACC      = 20, // mixture correction vs IAC pos.
 ETMT_INJ_IACCW     = 21, // weight of mixture correction vs IAC pos.
 ETMT_INJ_IATCLT    = 22, // IAT/CLT correction vs air flow
 ETMT_INJ_TPSSWT    = 23, // MAP/TPS switch point vs RPM
 ETMT_INJ_GTSC      = 24, // Inj. PW correction coefficient vs gas temperature
 ETMT_INJ_GPSC      = 25, // Inj. PW correction coefficient vs gas pressure
 ETMT_INJ_ATSC      = 26, // Inj. PW correction coefficient vs air temperature
 ETMT_PWM1          = 27, // PWM1 duty
 ETMT_PWM2          = 28, // PWM2 duty
 ETMT_INJ_IACMAT    = 29, // IAC position's correction vs MAT
 ETMT_INJ_VE2       = 30, // Secondary VE map
 ETMT_INJ_TPSZON    = 31, // MAP/TPS load axis allocation map
 ETMT_INJ_CYLMULT   = 32, // Inj. Multiplier
 ETMT_INJ_CYLADD    = 33, // Inj. Addition
 ETMT_INJ_AEMAP     = 34, // AE MAP map
 ETMT_INJ_THRASS    = 35, // Throttle assistant map
 ETMT_INJ_IVE       = 36,  // Idling VE

 //separate maps
 ETMT_ATTENUATOR    = 37, //
 ETMT_DWELLCNTRL    = 38, //
 ETMT_CTS_CURVE     = 39, //
 ETMT_ATS_CURVE     = 40, //
 ETMT_ATS_CORR      = 41, //
 ETMT_GASDOSE       = 42, //
 ETMT_BAROCORR      = 43, //
 ETMT_MANIGNTIM     = 44, //
 ETMT_TMP2_CURVE    = 45, //
 ETMT_CRKCLT_CORR   = 46, //
 ETMT_EH_PAUSE      = 47, //
 ETMT_CRANKING_THRD = 48, //
 ETMT_CRANKING_TIME = 49, //
 ETMT_SMAPABAN_THRD = 50, //
 ETMT_KNOCK_ZONE    = 51, //
 ETMT_GRTS_CURVE    = 52, //
 ETMT_GRHEAT_DUTY   = 53, //
 ETMT_PWMIAC_UCOEF  = 54, //
 ETMT_AFTSTR_STRK0  = 55, //
 ETMT_AFTSTR_STRK1  = 56, //
 ETMT_GRVDELAY      = 57, //
 ETMT_FTLS_CURVE    = 58, //
 ETMT_EGTS_CURVE    = 59, //
 ETMT_OPS_CURVE     = 60, //
 ETMT_MANINJPWC     = 61, //
 ETMT_MAF_CURVE     = 62, //
 ETMT_FTLSCOR       = 63, //
 ETMT_LAMBDA_ZONE   = 64, //
 ETMT_FTS_CURVE     = 65, //
 ETMT_FUELDENS_CORR = 66, //
 ETMT_XTAU_XFACC    = 67, //
 ETMT_XTAU_XFDEC    = 68, //
 ETMT_XTAU_TFACC    = 69, //
 ETMT_XTAU_TFDEC    = 70, //
 ETMT_INJNONLINP    = 71, //
 ETMT_INJNONLING    = 72, //
 ETMT_EGO_DELAY     = 73, //
 ETMT_WU_AFR0       = 74, //
 ETMT_WU_AFR1       = 75, //
 ETMT_ETC_SPRPREL   = 76, //
 ETMT_ETC_ACCEERR   = 77, //
 ETMT_ETC_THROPOS   = 78, //
 ETMT_OTS_CURVE     = 79, // oil temperature sensor's curve

 ETMT_GME_IGN_WND   = 80, //pseudo map, grid mode editing window for ignition maps
 ETMT_GME_INJ_WND   = 81, //pseudo map, grid mode editing window for injection maps

 ETMT_IG_START      = 1,  //set ignition
 ETMT_IG_END        = 5,
 ETMT_IN_START      = 6,  //set injection
 ETMT_IN_END        = 36,
 ETMT_SET_START     = 1,  //set  ignition + injection
 ETMT_SET_END       = 36,
 ETMT_SEP_START     = 37, //separate
 ETMT_SEP_END       = 79,
 ETMT_ALL_START     = 1,  //all (including GME)
 ETMT_ALL_END       = 81
};
