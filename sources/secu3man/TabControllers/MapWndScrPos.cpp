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

/** \file MapWndScrPos.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <limits>
#include "Resources/resource.h"
#include "MapWndScrPos.h"
#include "Settings/ISettingsData.h"
#include "TablDesk/MapIds.h"

#undef max   //avoid conflicts with C++

MapWndScrPos::MapWndScrPos(ISettingsData* ip_settings, bool online /*=false*/)
: mp_settings(ip_settings)
, m_online(online)
{
 //empty
}

void MapWndScrPos::OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 if (!i_hwnd)
  return;

 WINDOWPLACEMENT wpl;
 GetWindowPlacement(i_hwnd, &wpl);
 RECT rc = wpl.rcNormalPosition;

 WndSettings ws;
 WndSize sz;
 if (m_online)
 {
  mp_settings->GetWndSettings1(ws);
  mp_settings->GetWndSize1(sz);
 }
 else
 {
  mp_settings->GetWndSettings(ws);
  mp_settings->GetWndSize(sz);
 }

 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   ws.m_StrtMapWnd_X = rc.left;
   ws.m_StrtMapWnd_Y = rc.top;
   sz.m_StrtMapWnd_W = rc.right - rc.left;
   sz.m_StrtMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_DA_IDLE:
   ws.m_IdleMapWnd_X = rc.left;
   ws.m_IdleMapWnd_Y = rc.top;
   sz.m_IdleMapWnd_W = rc.right - rc.left;
   sz.m_IdleMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_DA_WORK:
   ws.m_WorkMapWnd_X = rc.left;
   ws.m_WorkMapWnd_Y = rc.top;
   sz.m_WorkMapWnd_W = rc.right - rc.left;
   sz.m_WorkMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   ws.m_TempMapWnd_X = rc.left;
   ws.m_TempMapWnd_Y = rc.top;
   sz.m_TempMapWnd_W = rc.right - rc.left;
   sz.m_TempMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_DA_TEMPI_CORR:
   ws.m_TempIdlMapWnd_X = rc.left;
   ws.m_TempIdlMapWnd_Y = rc.top;
   sz.m_TempIdlMapWnd_W = rc.right - rc.left;
   sz.m_TempIdlMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_ATTENUATOR:
   ws.m_AttenuatorMapWnd_X = rc.left;
   ws.m_AttenuatorMapWnd_Y = rc.top;
   sz.m_AttenuatorMapWnd_W = rc.right - rc.left;
   sz.m_AttenuatorMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_DWELLCNTRL:
   ws.m_DwellCntrlMapWnd_X = rc.left;
   ws.m_DwellCntrlMapWnd_Y = rc.top;
   sz.m_DwellCntrlMapWnd_W = rc.right - rc.left;
   sz.m_DwellCntrlMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_CTS_CURVE:
   ws.m_CTSCurveMapWnd_X = rc.left;
   ws.m_CTSCurveMapWnd_Y = rc.top;
   sz.m_CTSCurveMapWnd_W = rc.right - rc.left;
   sz.m_CTSCurveMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_VE:
   ws.m_VEMapWnd_X = rc.left;
   ws.m_VEMapWnd_Y = rc.top;
   sz.m_VEMapWnd_W = rc.right - rc.left;
   sz.m_VEMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_VE2:
   ws.m_VE2MapWnd_X = rc.left;
   ws.m_VE2MapWnd_Y = rc.top;
   sz.m_VE2MapWnd_W = rc.right - rc.left;
   sz.m_VE2MapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_AFR:
   ws.m_AFRMapWnd_X = rc.left;
   ws.m_AFRMapWnd_Y = rc.top;
   sz.m_AFRMapWnd_W = rc.right - rc.left;
   sz.m_AFRMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_CRNK:
   ws.m_CrnkMapWnd_X = rc.left;
   ws.m_CrnkMapWnd_Y = rc.top;
   sz.m_CrnkMapWnd_W = rc.right - rc.left;
   sz.m_CrnkMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_WRMP:
   ws.m_WrmpMapWnd_X = rc.left;
   ws.m_WrmpMapWnd_Y = rc.top;
   sz.m_WrmpMapWnd_W = rc.right - rc.left;
   sz.m_WrmpMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_DEAD:
   ws.m_DeadMapWnd_X = rc.left;
   ws.m_DeadMapWnd_Y = rc.top;
   sz.m_DeadMapWnd_W = rc.right - rc.left;
   sz.m_DeadMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_IDLR:
   ws.m_IdlrMapWnd_X = rc.left;
   ws.m_IdlrMapWnd_Y = rc.top;
   sz.m_IdlrMapWnd_W = rc.right - rc.left;
   sz.m_IdlrMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_IDLC:
   ws.m_IdlcMapWnd_X = rc.left;
   ws.m_IdlcMapWnd_Y = rc.top;
   sz.m_IdlcMapWnd_W = rc.right - rc.left;
   sz.m_IdlcMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_AETPS:
   ws.m_AETPSMapWnd_X = rc.left;
   ws.m_AETPSMapWnd_Y = rc.top;
   sz.m_AETPSMapWnd_W = rc.right - rc.left;
   sz.m_AETPSMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_AERPM:
   ws.m_AERPMMapWnd_X = rc.left;
   ws.m_AERPMMapWnd_Y = rc.top;
   sz.m_AERPMMapWnd_W = rc.right - rc.left;
   sz.m_AERPMMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_AFTSTR:
   ws.m_AftstrMapWnd_X = rc.left;
   ws.m_AftstrMapWnd_Y = rc.top;
   sz.m_AftstrMapWnd_W = rc.right - rc.left;
   sz.m_AftstrMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_ATS_CURVE:
   ws.m_ATSCurvMapWnd_X = rc.left;
   ws.m_ATSCurvMapWnd_Y = rc.top;
   sz.m_ATSCurvMapWnd_W = rc.right - rc.left;
   sz.m_ATSCurvMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_ATS_CORR:
   ws.m_ATSCorrMapWnd_X = rc.left;
   ws.m_ATSCorrMapWnd_Y = rc.top;
   sz.m_ATSCorrMapWnd_W = rc.right - rc.left;
   sz.m_ATSCorrMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_GASDOSE:
   ws.m_GasdoseMapWnd_X = rc.left;
   ws.m_GasdoseMapWnd_Y = rc.top;
   sz.m_GasdoseMapWnd_W = rc.right - rc.left;
   sz.m_GasdoseMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_IT:
   ws.m_ITMapWnd_X = rc.left;
   ws.m_ITMapWnd_Y = rc.top;
   sz.m_ITMapWnd_W = rc.right - rc.left;
   sz.m_ITMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_ITRPM:
   ws.m_ITRPMMapWnd_X = rc.left;
   ws.m_ITRPMMapWnd_Y = rc.top;
   sz.m_ITRPMMapWnd_W = rc.right - rc.left;
   sz.m_ITRPMMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_RIGID:
   ws.m_RigidMapWnd_X = rc.left;
   ws.m_RigidMapWnd_Y = rc.top;
   sz.m_RigidMapWnd_W = rc.right - rc.left;
   sz.m_RigidMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_EGOCRV:
   ws.m_EGOCrvMapWnd_X = rc.left;
   ws.m_EGOCrvMapWnd_Y = rc.top;
   sz.m_EGOCrvMapWnd_W = rc.right - rc.left;
   sz.m_EGOCrvMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_IACC:
   ws.m_IACCMapWnd_X = rc.left;
   ws.m_IACCMapWnd_Y = rc.top;
   sz.m_IACCMapWnd_W = rc.right - rc.left;
   sz.m_IACCMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_IACCW:
   ws.m_IACCWMapWnd_X = rc.left;
   ws.m_IACCWMapWnd_Y = rc.top;
   sz.m_IACCWMapWnd_W = rc.right - rc.left;
   sz.m_IACCWMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_IATCLT:
   ws.m_IATCLTMapWnd_X = rc.left;
   ws.m_IATCLTMapWnd_Y = rc.top;
   sz.m_IATCLTMapWnd_W = rc.right - rc.left;
   sz.m_IATCLTMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_BAROCORR:
   ws.m_BarocorrMapWnd_X = rc.left;
   ws.m_BarocorrMapWnd_Y = rc.top;
   sz.m_BarocorrMapWnd_W = rc.right - rc.left;
   sz.m_BarocorrMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_MANIGNTIM:
   ws.m_ManIgntimMapWnd_X = rc.left;
   ws.m_ManIgntimMapWnd_Y = rc.top;
   sz.m_ManIgntimMapWnd_W = rc.right - rc.left;
   sz.m_ManIgntimMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_TPSSWT:
   ws.m_TpsswtMapWnd_X = rc.left;
   ws.m_TpsswtMapWnd_Y = rc.top;
   sz.m_TpsswtMapWnd_W = rc.right - rc.left;
   sz.m_TpsswtMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_TMP2_CURVE:
   ws.m_Tmp2CurveMapWnd_X = rc.left;
   ws.m_Tmp2CurveMapWnd_Y = rc.top;
   sz.m_Tmp2CurveMapWnd_W = rc.right - rc.left;
   sz.m_Tmp2CurveMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_GRTS_CURVE:
   ws.m_GrtsCurveMapWnd_X = rc.left;
   ws.m_GrtsCurveMapWnd_Y = rc.top;
   sz.m_GrtsCurveMapWnd_W = rc.right - rc.left;
   sz.m_GrtsCurveMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_GTSC:
   ws.m_GtscMapWnd_X = rc.left;
   ws.m_GtscMapWnd_Y = rc.top;
   sz.m_GtscMapWnd_W = rc.right - rc.left;
   sz.m_GtscMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_GPSC:
   ws.m_GpscMapWnd_X = rc.left;
   ws.m_GpscMapWnd_Y = rc.top;
   sz.m_GpscMapWnd_W = rc.right - rc.left;
   sz.m_GpscMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_ATSC:
   ws.m_AtscMapWnd_X = rc.left;
   ws.m_AtscMapWnd_Y = rc.top;
   sz.m_AtscMapWnd_W = rc.right - rc.left;
   sz.m_AtscMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_CRKCLT_CORR:
   ws.m_CrkTempMapWnd_X = rc.left;
   ws.m_CrkTempMapWnd_Y = rc.top;
   sz.m_CrkTempMapWnd_W = rc.right - rc.left;
   sz.m_CrkTempMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_EH_PAUSE:
   ws.m_EHPauseMapWnd_X = rc.left;
   ws.m_EHPauseMapWnd_Y = rc.top;
   sz.m_EHPauseMapWnd_W = rc.right - rc.left;
   sz.m_EHPauseMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_CRANKING_THRD:
   ws.m_CrankingThrdMapWnd_X = rc.left;
   ws.m_CrankingThrdMapWnd_Y = rc.top;
   sz.m_CrankingThrdMapWnd_W = rc.right - rc.left;
   sz.m_CrankingThrdMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_CRANKING_TIME:
   ws.m_CrankingTimeMapWnd_X = rc.left;
   ws.m_CrankingTimeMapWnd_Y = rc.top;
   sz.m_CrankingTimeMapWnd_W = rc.right - rc.left;
   sz.m_CrankingTimeMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_SMAPABAN_THRD:
   ws.m_SmapabanThrdMapWnd_X = rc.left;
   ws.m_SmapabanThrdMapWnd_Y = rc.top;
   sz.m_SmapabanThrdMapWnd_W = rc.right - rc.left;
   sz.m_SmapabanThrdMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_PWM1:
   ws.m_Pwm1MapWnd_X = rc.left;
   ws.m_Pwm1MapWnd_Y = rc.top;
   sz.m_Pwm1MapWnd_W = rc.right - rc.left;
   sz.m_Pwm1MapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_PWM2:
   ws.m_Pwm2MapWnd_X = rc.left;
   ws.m_Pwm2MapWnd_Y = rc.top;
   sz.m_Pwm2MapWnd_W = rc.right - rc.left;
   sz.m_Pwm2MapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_GME_IGN_WND: //pseudo map
   ws.m_GridMapIgnWnd_X = rc.left;
   ws.m_GridMapIgnWnd_Y = rc.top;
   sz.m_GridMapIgnWnd_W = rc.right - rc.left;
   sz.m_GridMapIgnWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_GME_INJ_WND: //pseudo map
   ws.m_GridMapInjWnd_X = rc.left;
   ws.m_GridMapInjWnd_Y = rc.top;
   sz.m_GridMapInjWnd_W = rc.right - rc.left;
   sz.m_GridMapInjWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_KNOCK_ZONE:
   ws.m_KnockZoneMapWnd_X = rc.left;
   ws.m_KnockZoneMapWnd_Y = rc.top;
   sz.m_KnockZoneMapWnd_W = rc.right - rc.left;
   sz.m_KnockZoneMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_GRHEAT_DUTY:
   ws.m_GrHeatDutyMapWnd_X = rc.left;
   ws.m_GrHeatDutyMapWnd_Y = rc.top;
   sz.m_GrHeatDutyMapWnd_W = rc.right - rc.left;
   sz.m_GrHeatDutyMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_PWMIAC_UCOEF:
   ws.m_PwmIacUCoefMapWnd_X = rc.left;
   ws.m_PwmIacUCoefMapWnd_Y = rc.top;
   sz.m_PwmIacUCoefMapWnd_W = rc.right - rc.left;
   sz.m_PwmIacUCoefMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_AFTSTR_STRK0:
   ws.m_AftstrStrk0MapWnd_X = rc.left;
   ws.m_AftstrStrk0MapWnd_Y = rc.top;
   sz.m_AftstrStrk0MapWnd_W = rc.right - rc.left;
   sz.m_AftstrStrk0MapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_AFTSTR_STRK1:
   ws.m_AftstrStrk1MapWnd_X = rc.left;
   ws.m_AftstrStrk1MapWnd_Y = rc.top;
   sz.m_AftstrStrk1MapWnd_W = rc.right - rc.left;
   sz.m_AftstrStrk1MapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_GRVDELAY:
   ws.m_GrValDelMapWnd_X = rc.left;
   ws.m_GrValDelMapWnd_Y = rc.top;
   sz.m_GrValDelMapWnd_W = rc.right - rc.left;
   sz.m_GrValDelMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_INJ_IACMAT:
   ws.m_IACMATMapWnd_X = rc.left;
   ws.m_IACMATMapWnd_Y = rc.top;
   sz.m_IACMATMapWnd_W = rc.right - rc.left;
   sz.m_IACMATMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_FTLS_CURVE:
   ws.m_FtlsCurveMapWnd_X = rc.left;
   ws.m_FtlsCurveMapWnd_Y = rc.top;
   sz.m_FtlsCurveMapWnd_W = rc.right - rc.left;
   sz.m_FtlsCurveMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_EGTS_CURVE:
   ws.m_EgtsCurveMapWnd_X = rc.left;
   ws.m_EgtsCurveMapWnd_Y = rc.top;
   sz.m_EgtsCurveMapWnd_W = rc.right - rc.left;
   sz.m_EgtsCurveMapWnd_H = rc.bottom - rc.top;
   break;
  case TYPE_MAP_OPS_CURVE:
   ws.m_OpsCurveMapWnd_X = rc.left;
   ws.m_OpsCurveMapWnd_Y = rc.top;
   sz.m_OpsCurveMapWnd_W = rc.right - rc.left;
   sz.m_OpsCurveMapWnd_H = rc.bottom - rc.top;
   break;
 };

 if (m_online)
 {
  mp_settings->SetWndSettings1(ws);
  mp_settings->SetWndSize1(sz);
 }
 else
 {
  mp_settings->SetWndSettings(ws);
  mp_settings->SetWndSize(sz);
 }
}

void MapWndScrPos::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 if (!i_hwnd)
  return;

 WndSettings ws;
 WndSize sz;
 if (m_online)
 {
  mp_settings->GetWndSettings1(ws);
  mp_settings->GetWndSize1(sz);
 }
 else
 {
  mp_settings->GetWndSettings(ws);
  mp_settings->GetWndSize(sz);
 }

 int X = std::numeric_limits<int>::max(), Y = std::numeric_limits<int>::max(), W = std::numeric_limits<int>::max(), H = std::numeric_limits<int>::max();

 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   X = ws.m_StrtMapWnd_X, Y = ws.m_StrtMapWnd_Y;
   W = sz.m_StrtMapWnd_W, H = sz.m_StrtMapWnd_H;
   break;
  case TYPE_MAP_DA_IDLE:
   X = ws.m_IdleMapWnd_X, Y = ws.m_IdleMapWnd_Y;
   W = sz.m_IdleMapWnd_W, H = sz.m_IdleMapWnd_H;
   break;
  case TYPE_MAP_DA_WORK:
   X = ws.m_WorkMapWnd_X, Y = ws.m_WorkMapWnd_Y;
   W = sz.m_WorkMapWnd_W, H = sz.m_WorkMapWnd_H;
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   X = ws.m_TempMapWnd_X, Y = ws.m_TempMapWnd_Y;
   W = sz.m_TempMapWnd_W, H = sz.m_TempMapWnd_H;
   break;
  case TYPE_MAP_DA_TEMPI_CORR:
   X = ws.m_TempIdlMapWnd_X, Y = ws.m_TempIdlMapWnd_Y;
   W = sz.m_TempIdlMapWnd_W, H = sz.m_TempIdlMapWnd_H;
   break;
  case TYPE_MAP_ATTENUATOR:
   X = ws.m_AttenuatorMapWnd_X, Y = ws.m_AttenuatorMapWnd_Y;
   W = sz.m_AttenuatorMapWnd_W, H = sz.m_AttenuatorMapWnd_H;
   break;
  case TYPE_MAP_DWELLCNTRL:
   X = ws.m_DwellCntrlMapWnd_X, Y = ws.m_DwellCntrlMapWnd_Y;
   W = sz.m_DwellCntrlMapWnd_W, H = sz.m_DwellCntrlMapWnd_H;
   break;
  case TYPE_MAP_CTS_CURVE:
   X = ws.m_CTSCurveMapWnd_X, Y = ws.m_CTSCurveMapWnd_Y;
   W = sz.m_CTSCurveMapWnd_W, H = sz.m_CTSCurveMapWnd_H;
   break;
  case TYPE_MAP_INJ_VE:
   X = ws.m_VEMapWnd_X, Y = ws.m_VEMapWnd_Y;
   W = sz.m_VEMapWnd_W, H = sz.m_VEMapWnd_H;
   break;
  case TYPE_MAP_INJ_VE2:
   X = ws.m_VE2MapWnd_X, Y = ws.m_VE2MapWnd_Y;
   W = sz.m_VE2MapWnd_W, H = sz.m_VE2MapWnd_H;
   break;
  case TYPE_MAP_INJ_AFR:
   X = ws.m_AFRMapWnd_X, Y = ws.m_AFRMapWnd_Y;
   W = sz.m_AFRMapWnd_W, H = sz.m_AFRMapWnd_H;
   break;
  case TYPE_MAP_INJ_CRNK:
   X = ws.m_CrnkMapWnd_X, Y = ws.m_CrnkMapWnd_Y;
   W = sz.m_CrnkMapWnd_W, H = sz.m_CrnkMapWnd_H;
   break;
  case TYPE_MAP_INJ_WRMP:
   X = ws.m_WrmpMapWnd_X, Y = ws.m_WrmpMapWnd_Y;
   W = sz.m_WrmpMapWnd_W, H = sz.m_WrmpMapWnd_H;
   break;
  case TYPE_MAP_INJ_DEAD:
   X = ws.m_DeadMapWnd_X, Y = ws.m_DeadMapWnd_Y;
   W = sz.m_DeadMapWnd_W, H = sz.m_DeadMapWnd_H;
   break;
  case TYPE_MAP_INJ_IDLR:
   X = ws.m_IdlrMapWnd_X, Y = ws.m_IdlrMapWnd_Y;
   W = sz.m_IdlrMapWnd_W, H = sz.m_IdlrMapWnd_H;
   break;
  case TYPE_MAP_INJ_IDLC:
   X = ws.m_IdlcMapWnd_X, Y = ws.m_IdlcMapWnd_Y;
   W = sz.m_IdlcMapWnd_W, H = sz.m_IdlcMapWnd_H;
   break;
  case TYPE_MAP_INJ_AETPS:
   X = ws.m_AETPSMapWnd_X, Y = ws.m_AETPSMapWnd_Y;
   W = sz.m_AETPSMapWnd_W, H = sz.m_AETPSMapWnd_H;
   break;
  case TYPE_MAP_INJ_AERPM:
   X = ws.m_AERPMMapWnd_X, Y = ws.m_AERPMMapWnd_Y;
   W = sz.m_AERPMMapWnd_W, H = sz.m_AERPMMapWnd_H;
   break;
  case TYPE_MAP_INJ_AFTSTR:
   X = ws.m_AftstrMapWnd_X, Y = ws.m_AftstrMapWnd_Y;
   W = sz.m_AftstrMapWnd_W, H = sz.m_AftstrMapWnd_H;
   break;
  case TYPE_MAP_ATS_CURVE:
   X = ws.m_ATSCurvMapWnd_X, Y = ws.m_ATSCurvMapWnd_Y;
   W = sz.m_ATSCurvMapWnd_W, H = sz.m_ATSCurvMapWnd_H;
   break;
  case TYPE_MAP_ATS_CORR:
   X = ws.m_ATSCorrMapWnd_X, Y = ws.m_ATSCorrMapWnd_Y;
   W = sz.m_ATSCurvMapWnd_W, H = sz.m_ATSCurvMapWnd_H;
   break;
  case TYPE_MAP_GASDOSE:
   X = ws.m_GasdoseMapWnd_X, Y = ws.m_GasdoseMapWnd_Y;
   W = sz.m_GasdoseMapWnd_W, H = sz.m_GasdoseMapWnd_H;
   break;
  case TYPE_MAP_INJ_IT:
   X = ws.m_ITMapWnd_X, Y = ws.m_ITMapWnd_Y;
   W = sz.m_ITMapWnd_W, H = sz.m_ITMapWnd_H;
   break;
  case TYPE_MAP_INJ_ITRPM:
   X = ws.m_ITRPMMapWnd_X, Y = ws.m_ITRPMMapWnd_Y;
   W = sz.m_ITRPMMapWnd_W, H = sz.m_ITRPMMapWnd_H;
   break;
  case TYPE_MAP_INJ_RIGID:
   X = ws.m_RigidMapWnd_X, Y = ws.m_RigidMapWnd_Y;
   W = sz.m_RigidMapWnd_W, H = sz.m_RigidMapWnd_H;
   break;
  case TYPE_MAP_INJ_EGOCRV:
   X = ws.m_EGOCrvMapWnd_X, Y = ws.m_EGOCrvMapWnd_Y;
   W = sz.m_EGOCrvMapWnd_W, H = sz.m_EGOCrvMapWnd_H;
   break;
  case TYPE_MAP_INJ_IACC:
   X = ws.m_IACCMapWnd_X, Y = ws.m_IACCMapWnd_Y;
   W = sz.m_IACCMapWnd_W, H = sz.m_IACCMapWnd_H;
   break;
  case TYPE_MAP_INJ_IACCW:
   X = ws.m_IACCWMapWnd_X, Y = ws.m_IACCWMapWnd_Y;
   W = sz.m_IACCWMapWnd_W, H = sz.m_IACCWMapWnd_H;
   break;
  case TYPE_MAP_INJ_IATCLT:
   X = ws.m_IATCLTMapWnd_X, Y = ws.m_IATCLTMapWnd_Y;
   W = sz.m_IATCLTMapWnd_W, H = sz.m_IATCLTMapWnd_H;
   break;
  case TYPE_MAP_BAROCORR:
   X = ws.m_BarocorrMapWnd_X, Y = ws.m_BarocorrMapWnd_Y;
   W = sz.m_BarocorrMapWnd_W, H = sz.m_BarocorrMapWnd_H;
   break;
  case TYPE_MAP_MANIGNTIM:
   X = ws.m_ManIgntimMapWnd_X, Y = ws.m_ManIgntimMapWnd_Y;
   W = sz.m_ManIgntimMapWnd_W, H = sz.m_ManIgntimMapWnd_H;
   break;
  case TYPE_MAP_INJ_TPSSWT:
   X = ws.m_TpsswtMapWnd_X, Y = ws.m_TpsswtMapWnd_Y;
   W = sz.m_TpsswtMapWnd_W, H = sz.m_TpsswtMapWnd_H;
   break;
  case TYPE_MAP_TMP2_CURVE:
   X = ws.m_Tmp2CurveMapWnd_X, Y = ws.m_Tmp2CurveMapWnd_Y;
   W = sz.m_Tmp2CurveMapWnd_W, H = sz.m_Tmp2CurveMapWnd_H;
   break;
  case TYPE_MAP_GRTS_CURVE:
   X = ws.m_GrtsCurveMapWnd_X, Y = ws.m_GrtsCurveMapWnd_Y;
   W = sz.m_GrtsCurveMapWnd_W, H = sz.m_GrtsCurveMapWnd_H;
   break;
  case TYPE_MAP_INJ_GTSC:
   X = ws.m_GtscMapWnd_X, Y = ws.m_GtscMapWnd_Y;
   W = sz.m_GtscMapWnd_W, H = sz.m_GtscMapWnd_H;
   break;
  case TYPE_MAP_INJ_GPSC:
   X = ws.m_GpscMapWnd_X, Y = ws.m_GpscMapWnd_Y;
   W = sz.m_GpscMapWnd_W, H = sz.m_GpscMapWnd_H;
   break;
  case TYPE_MAP_INJ_ATSC:
   X = ws.m_AtscMapWnd_X, Y = ws.m_AtscMapWnd_Y;
   W = sz.m_AtscMapWnd_W, H = sz.m_AtscMapWnd_H;
   break;
  case TYPE_MAP_CRKCLT_CORR:
   X = ws.m_CrkTempMapWnd_X, Y = ws.m_CrkTempMapWnd_Y;
   W = sz.m_CrkTempMapWnd_W, H = sz.m_CrkTempMapWnd_H;
   break;
  case TYPE_MAP_EH_PAUSE:
   X = ws.m_EHPauseMapWnd_X, Y = ws.m_EHPauseMapWnd_Y;
   W = sz.m_EHPauseMapWnd_W, H = sz.m_EHPauseMapWnd_H;
   break;
  case TYPE_MAP_CRANKING_THRD:
   X = ws.m_CrankingThrdMapWnd_X, Y = ws.m_CrankingThrdMapWnd_Y;
   W = sz.m_CrankingThrdMapWnd_W, H = sz.m_CrankingThrdMapWnd_H;
   break;
  case TYPE_MAP_CRANKING_TIME:
   X = ws.m_CrankingTimeMapWnd_X, Y = ws.m_CrankingTimeMapWnd_Y;
   W = sz.m_CrankingTimeMapWnd_W, H = sz.m_CrankingTimeMapWnd_H;
   break;
  case TYPE_MAP_SMAPABAN_THRD:
   X = ws.m_SmapabanThrdMapWnd_X, Y = ws.m_SmapabanThrdMapWnd_Y;
   W = sz.m_SmapabanThrdMapWnd_W, H = sz.m_SmapabanThrdMapWnd_H;
   break;
  case TYPE_MAP_PWM1:
   X = ws.m_Pwm1MapWnd_X, Y = ws.m_Pwm1MapWnd_Y;
   W = sz.m_Pwm1MapWnd_W, H = sz.m_Pwm1MapWnd_H;
   break;
  case TYPE_MAP_PWM2:
   X = ws.m_Pwm2MapWnd_X, Y = ws.m_Pwm2MapWnd_Y;
   W = sz.m_Pwm2MapWnd_W, H = sz.m_Pwm2MapWnd_H;
   break;
  case TYPE_MAP_KNOCK_ZONE:
   X = ws.m_KnockZoneMapWnd_X, Y = ws.m_KnockZoneMapWnd_Y;
   W = sz.m_KnockZoneMapWnd_W, H = sz.m_KnockZoneMapWnd_H;
   break;
  case TYPE_MAP_GRHEAT_DUTY:
   X = ws.m_GrHeatDutyMapWnd_X, Y = ws.m_GrHeatDutyMapWnd_Y;
   W = sz.m_GrHeatDutyMapWnd_W, H = sz.m_GrHeatDutyMapWnd_H;
   break;
  case TYPE_MAP_GME_IGN_WND:
   X = ws.m_GridMapIgnWnd_X, Y = ws.m_GridMapIgnWnd_Y;
   W = sz.m_GridMapIgnWnd_W, H = sz.m_GridMapIgnWnd_H;
   break;
  case TYPE_MAP_GME_INJ_WND:
   X = ws.m_GridMapInjWnd_X, Y = ws.m_GridMapInjWnd_Y;
   W = sz.m_GridMapInjWnd_W, H = sz.m_GridMapInjWnd_H;
   break;
  case TYPE_MAP_PWMIAC_UCOEF:
   X = ws.m_PwmIacUCoefMapWnd_X, Y = ws.m_PwmIacUCoefMapWnd_Y;
   W = sz.m_PwmIacUCoefMapWnd_W, H = sz.m_PwmIacUCoefMapWnd_H;
   break;
  case TYPE_MAP_AFTSTR_STRK0:
   X = ws.m_AftstrStrk0MapWnd_X, Y = ws.m_AftstrStrk0MapWnd_Y;
   W = sz.m_AftstrStrk0MapWnd_W, H = sz.m_AftstrStrk0MapWnd_H;
   break;
  case TYPE_MAP_AFTSTR_STRK1:
   X = ws.m_AftstrStrk1MapWnd_X, Y = ws.m_AftstrStrk1MapWnd_Y;
   W = sz.m_AftstrStrk1MapWnd_W, H = sz.m_AftstrStrk1MapWnd_H;
   break;
  case TYPE_MAP_GRVDELAY:
   X = ws.m_GrValDelMapWnd_X, Y = ws.m_GrValDelMapWnd_Y;
   W = sz.m_GrValDelMapWnd_W, H = sz.m_GrValDelMapWnd_H;
   break;
  case TYPE_MAP_INJ_IACMAT:
   X = ws.m_IACMATMapWnd_X, Y = ws.m_IACMATMapWnd_Y;
   W = sz.m_IACMATMapWnd_W, H = sz.m_IACMATMapWnd_H;
   break;
  case TYPE_MAP_FTLS_CURVE:
   X = ws.m_FtlsCurveMapWnd_X, Y = ws.m_FtlsCurveMapWnd_Y;
   W = sz.m_FtlsCurveMapWnd_W, H = sz.m_FtlsCurveMapWnd_H;
   break;
  case TYPE_MAP_EGTS_CURVE:
   X = ws.m_EgtsCurveMapWnd_X, Y = ws.m_EgtsCurveMapWnd_Y;
   W = sz.m_EgtsCurveMapWnd_W, H = sz.m_EgtsCurveMapWnd_H;
   break;
  case TYPE_MAP_OPS_CURVE:
   X = ws.m_OpsCurveMapWnd_X, Y = ws.m_OpsCurveMapWnd_Y;
   W = sz.m_OpsCurveMapWnd_W, H = sz.m_OpsCurveMapWnd_H;
   break;

  default:
   return; //undefined case...
 };

 WINDOWPLACEMENT wpl;
 GetWindowPlacement(i_hwnd, &wpl);

 bool update = false; 
 if (X != std::numeric_limits<int>::max() && Y != std::numeric_limits<int>::max())
 { //change position, left size intact
  wpl.rcNormalPosition = CRect(X, Y, X + (wpl.rcNormalPosition.right - wpl.rcNormalPosition.left), Y + (wpl.rcNormalPosition.bottom - wpl.rcNormalPosition.top));
  update = true;
 }
 if (W != std::numeric_limits<int>::max() && H != std::numeric_limits<int>::max())
 { //left position intact, change size
  wpl.rcNormalPosition = CRect(wpl.rcNormalPosition.left, wpl.rcNormalPosition.top, wpl.rcNormalPosition.left + W, wpl.rcNormalPosition.top + H);
  update = true;
 }
 if (update)
  SetWindowPlacement(i_hwnd, &wpl);
}
