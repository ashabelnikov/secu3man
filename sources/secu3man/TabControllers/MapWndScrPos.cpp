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

MapWndScrPos::MapWndScrPos(ISettingsData* ip_settings)
: mp_settings(ip_settings)
{
 //empty
}

void MapWndScrPos::OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 if (!i_hwnd)
  return;

 RECT rc;
 GetWindowRect(i_hwnd, &rc);

 WndSettings ws;
 mp_settings->GetWndSettings(ws);

 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   ws.m_StrtMapWnd_X = rc.left;
   ws.m_StrtMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_DA_IDLE:
   ws.m_IdleMapWnd_X = rc.left;
   ws.m_IdleMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_DA_WORK:
   ws.m_WorkMapWnd_X = rc.left;
   ws.m_WorkMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   ws.m_TempMapWnd_X = rc.left;
   ws.m_TempMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_ATTENUATOR:
   ws.m_AttenuatorMapWnd_X = rc.left;
   ws.m_AttenuatorMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_DWELLCNTRL:
   ws.m_DwellCntrlMapWnd_X = rc.left;
   ws.m_DwellCntrlMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_CTS_CURVE:
   ws.m_CTSCurveMapWnd_X = rc.left;
   ws.m_CTSCurveMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_CHOKE_OP:
   ws.m_ChokeOpMapWnd_X = rc.left;
   ws.m_ChokeOpMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_VE:
   ws.m_VEMapWnd_X = rc.left;
   ws.m_VEMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_AFR:
   ws.m_AFRMapWnd_X = rc.left;
   ws.m_AFRMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_CRNK:
   ws.m_CrnkMapWnd_X = rc.left;
   ws.m_CrnkMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_WRMP:
   ws.m_WrmpMapWnd_X = rc.left;
   ws.m_WrmpMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_DEAD:
   ws.m_DeadMapWnd_X = rc.left;
   ws.m_DeadMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_IDLR:
   ws.m_IdlrMapWnd_X = rc.left;
   ws.m_IdlrMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_IDLC:
   ws.m_IdlcMapWnd_X = rc.left;
   ws.m_IdlcMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_AETPS:
   ws.m_AETPSMapWnd_X = rc.left;
   ws.m_AETPSMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_AERPM:
   ws.m_AERPMMapWnd_X = rc.left;
   ws.m_AERPMMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_AFTSTR:
   ws.m_AftstrMapWnd_X = rc.left;
   ws.m_AftstrMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_ATS_CURVE:
   ws.m_ATSCurvMapWnd_X = rc.left;
   ws.m_ATSCurvMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_ATS_CORR:
   ws.m_ATSCorrMapWnd_X = rc.left;
   ws.m_ATSCorrMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_GASDOSE:
   ws.m_GasdoseMapWnd_X = rc.left;
   ws.m_GasdoseMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_IT:
   ws.m_ITMapWnd_X = rc.left;
   ws.m_ITMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_ITRPM:
   ws.m_ITRPMMapWnd_X = rc.left;
   ws.m_ITRPMMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_RIGID:
   ws.m_RigidMapWnd_X = rc.left;
   ws.m_RigidMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_EGOCRV:
   ws.m_EGOCrvMapWnd_X = rc.left;
   ws.m_EGOCrvMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_IACC:
   ws.m_IACCMapWnd_X = rc.left;
   ws.m_IACCMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_IACCW:
   ws.m_IACCWMapWnd_X = rc.left;
   ws.m_IACCWMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_IATCLT:
   ws.m_IATCLTMapWnd_X = rc.left;
   ws.m_IATCLTMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_BAROCORR:
   ws.m_BarocorrMapWnd_X = rc.left;
   ws.m_BarocorrMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_MANIGNTIM:
   ws.m_ManIgntimMapWnd_X = rc.left;
   ws.m_ManIgntimMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_TPSSWT:
   ws.m_TpsswtMapWnd_X = rc.left;
   ws.m_TpsswtMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_TMP2_CURVE:
   ws.m_Tmp2CurveMapWnd_X = rc.left;
   ws.m_Tmp2CurveMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_GTSC:
   ws.m_GtscMapWnd_X = rc.left;
   ws.m_GtscMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_GPSC:
   ws.m_GpscMapWnd_X = rc.left;
   ws.m_GpscMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_INJ_ATSC:
   ws.m_AtscMapWnd_X = rc.left;
   ws.m_AtscMapWnd_Y = rc.top;
   break;

  case TYPE_MAP_GME_WND: //pseudo map
   ws.m_GridMapWnd_X = rc.left;
   ws.m_GridMapWnd_Y = rc.top;
   break;
 };

 mp_settings->SetWndSettings(ws);
}

void MapWndScrPos::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 if (!i_hwnd)
  return;

 WndSettings ws;
 mp_settings->GetWndSettings(ws);

 int X = 0, Y = 0;

 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   X = ws.m_StrtMapWnd_X, Y = ws.m_StrtMapWnd_Y;
   break;
  case TYPE_MAP_DA_IDLE:
   X = ws.m_IdleMapWnd_X, Y = ws.m_IdleMapWnd_Y;
   break;
  case TYPE_MAP_DA_WORK:
   X = ws.m_WorkMapWnd_X, Y = ws.m_WorkMapWnd_Y;
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   X = ws.m_TempMapWnd_X, Y = ws.m_TempMapWnd_Y;
   break;
  case TYPE_MAP_ATTENUATOR:
   X = ws.m_AttenuatorMapWnd_X, Y = ws.m_AttenuatorMapWnd_Y;
   break;
  case TYPE_MAP_DWELLCNTRL:
   X = ws.m_DwellCntrlMapWnd_X, Y = ws.m_DwellCntrlMapWnd_Y;
   break;
  case TYPE_MAP_CTS_CURVE:
   X = ws.m_CTSCurveMapWnd_X, Y = ws.m_CTSCurveMapWnd_Y;
   break;
  case TYPE_MAP_CHOKE_OP:
   X = ws.m_ChokeOpMapWnd_X, Y = ws.m_ChokeOpMapWnd_Y;
   break;
  case TYPE_MAP_INJ_VE:
   X = ws.m_VEMapWnd_X, Y = ws.m_VEMapWnd_Y;
   break;
  case TYPE_MAP_INJ_AFR:
   X = ws.m_AFRMapWnd_X, Y = ws.m_AFRMapWnd_Y;
   break;
  case TYPE_MAP_INJ_CRNK:
   X = ws.m_CrnkMapWnd_X, Y = ws.m_CrnkMapWnd_Y;
   break;
  case TYPE_MAP_INJ_WRMP:
   X = ws.m_WrmpMapWnd_X, Y = ws.m_WrmpMapWnd_Y;
   break;
  case TYPE_MAP_INJ_DEAD:
   X = ws.m_DeadMapWnd_X, Y = ws.m_DeadMapWnd_Y;
   break;
  case TYPE_MAP_INJ_IDLR:
   X = ws.m_IdlrMapWnd_X, Y = ws.m_IdlrMapWnd_Y;
   break;
  case TYPE_MAP_INJ_IDLC:
   X = ws.m_IdlcMapWnd_X, Y = ws.m_IdlcMapWnd_Y;
   break;
  case TYPE_MAP_INJ_AETPS:
   X = ws.m_AETPSMapWnd_X, Y = ws.m_AETPSMapWnd_Y;
   break;
  case TYPE_MAP_INJ_AERPM:
   X = ws.m_AERPMMapWnd_X, Y = ws.m_AERPMMapWnd_Y;
   break;
  case TYPE_MAP_INJ_AFTSTR:
   X = ws.m_AftstrMapWnd_X, Y = ws.m_AftstrMapWnd_Y;
   break;
  case TYPE_MAP_ATS_CURVE:
   X = ws.m_ATSCurvMapWnd_X, Y = ws.m_ATSCurvMapWnd_Y;
   break;
  case TYPE_MAP_ATS_CORR:
   X = ws.m_ATSCorrMapWnd_X, Y = ws.m_ATSCorrMapWnd_Y;
   break;
  case TYPE_MAP_GASDOSE:
   X = ws.m_GasdoseMapWnd_X, Y = ws.m_GasdoseMapWnd_Y;
   break;
  case TYPE_MAP_INJ_IT:
   X = ws.m_ITMapWnd_X, Y = ws.m_ITMapWnd_Y;
   break;
  case TYPE_MAP_INJ_ITRPM:
   X = ws.m_ITRPMMapWnd_X, Y = ws.m_ITRPMMapWnd_Y;
   break;
  case TYPE_MAP_INJ_RIGID:
   X = ws.m_RigidMapWnd_X, Y = ws.m_RigidMapWnd_Y;
   break;
  case TYPE_MAP_INJ_EGOCRV:
   X = ws.m_EGOCrvMapWnd_X, Y = ws.m_EGOCrvMapWnd_Y;
   break;
  case TYPE_MAP_INJ_IACC:
   X = ws.m_IACCMapWnd_X, Y = ws.m_IACCMapWnd_Y;
   break;
  case TYPE_MAP_INJ_IACCW:
   X = ws.m_IACCWMapWnd_X, Y = ws.m_IACCWMapWnd_Y;
   break;
  case TYPE_MAP_INJ_IATCLT:
   X = ws.m_IATCLTMapWnd_X, Y = ws.m_IATCLTMapWnd_Y;
   break;
  case TYPE_MAP_BAROCORR:
   X = ws.m_BarocorrMapWnd_X, Y = ws.m_BarocorrMapWnd_Y;
   break;
  case TYPE_MAP_MANIGNTIM:
   X = ws.m_ManIgntimMapWnd_X, Y = ws.m_ManIgntimMapWnd_Y;
   break;
  case TYPE_MAP_INJ_TPSSWT:
   X = ws.m_TpsswtMapWnd_X, Y = ws.m_TpsswtMapWnd_Y;
   break;
  case TYPE_MAP_TMP2_CURVE:
   X = ws.m_Tmp2CurveMapWnd_X, Y = ws.m_Tmp2CurveMapWnd_Y;
   break;
  case TYPE_MAP_INJ_GTSC:
   X = ws.m_GtscMapWnd_X, Y = ws.m_GtscMapWnd_Y;
   break;
  case TYPE_MAP_INJ_GPSC:
   X = ws.m_GpscMapWnd_X, Y = ws.m_GpscMapWnd_Y;
   break;
  case TYPE_MAP_INJ_ATSC:
   X = ws.m_AtscMapWnd_X, Y = ws.m_AtscMapWnd_Y;
   break;

  case TYPE_MAP_GME_WND:
   X = ws.m_GridMapWnd_X, Y = ws.m_GridMapWnd_Y;
   break;
  default:
   return; //undefined case...
 };

 if (X != std::numeric_limits<int>::max() && Y != std::numeric_limits<int>::max())
  SetWindowPos(i_hwnd, NULL, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
