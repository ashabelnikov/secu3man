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

/** \file ISettingsData.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "io-core/PlatformParamHolder.h"  //only for EECUPlatform enum

//Holds windows's position data
struct WndSettings
{
 int m_StrtMapWnd_X;
 int m_StrtMapWnd_Y;
 int m_IdleMapWnd_X;
 int m_IdleMapWnd_Y;
 int m_WorkMapWnd_X;
 int m_WorkMapWnd_Y;
 int m_TempMapWnd_X;
 int m_TempMapWnd_Y;
 int m_AttenuatorMapWnd_X;
 int m_AttenuatorMapWnd_Y;
 int m_MainFrmWnd_X;
 int m_MainFrmWnd_Y;
 int m_DwellCntrlMapWnd_X;
 int m_DwellCntrlMapWnd_Y;
 int m_CTSCurveMapWnd_X;
 int m_CTSCurveMapWnd_Y;
 int m_ChokeOpMapWnd_X;
 int m_ChokeOpMapWnd_Y;
 int m_GridMapWnd_X;
 int m_GridMapWnd_Y;
 int m_VEMapWnd_X;
 int m_VEMapWnd_Y;
 int m_AFRMapWnd_X;
 int m_AFRMapWnd_Y;
 int m_CrnkMapWnd_X;
 int m_CrnkMapWnd_Y;
 int m_WrmpMapWnd_X;
 int m_WrmpMapWnd_Y;
 int m_DeadMapWnd_X;
 int m_DeadMapWnd_Y;
 int m_IdlrMapWnd_X;
 int m_IdlrMapWnd_Y;
 int m_IdlcMapWnd_X;
 int m_IdlcMapWnd_Y;
 int m_ATSCurvMapWnd_X;
 int m_ATSCurvMapWnd_Y;
 int m_ATSCorrMapWnd_X;
 int m_ATSCorrMapWnd_Y;
 int m_AETPSMapWnd_X;
 int m_AETPSMapWnd_Y;
 int m_AERPMMapWnd_X;
 int m_AERPMMapWnd_Y;
 int m_AftstrMapWnd_X;
 int m_AftstrMapWnd_Y;
 int m_GasdoseMapWnd_X;
 int m_GasdoseMapWnd_Y;
 int m_ITMapWnd_X;
 int m_ITMapWnd_Y;
};

typedef enum EInterLang
{
 IL_ENGLISH = 0,
 IL_RUSSIAN
};

//Interface which should be used to access settings' data from usual places of program
class ISettingsData
{
 public:
  virtual const _TSTRING& GetPortName(void) const = 0;
  virtual DWORD GetBaudRateApplication(void) const = 0;
  virtual DWORD GetBaudRateBootloader(void) const = 0;
  virtual const CString& GetLogFilesFolder(void) const = 0;
  virtual bool  GetUseAppFolder(void) const = 0;
  virtual bool  GetAlwaysWriteLog(void) const = 0;
  virtual char  GetCSVSepSymbol(void) const = 0;
  virtual int   GetMIDeskUpdatePeriod(void) const = 0;
  virtual CString GetAppDirectory(void) const = 0;
  virtual bool GetCOMPortBother(void) const = 0;
  virtual bool GetUseHotKeys(void) const = 0;
  virtual bool GetShowWelcome(void) const = 0;
  virtual int GetRPMAverage(void) const = 0;
  virtual int GetVoltAverage(void) const = 0;
  virtual int GetMAPAverage(void) const = 0;
  virtual int GetAI1Average(void) const = 0;
  virtual bool GetAllowVisualTheme(void) const = 0;
  //windows' positions
  virtual void SetWndSettings(const WndSettings& i_wndSettings) = 0;
  virtual void GetWndSettings(WndSettings& o_wndSettings) const = 0;
  //Interface language and SECU-3 platform type
  virtual EInterLang GetInterfaceLanguage(void) const = 0;
  virtual EECUPlatform GetECUPlatformType(void) const = 0;
  //Fixtures's customization settings
  virtual int GetTachometerMax(void) const = 0;
  virtual int GetPressureMax(void) const = 0;
  //Debug features
  virtual bool GetUseDVFeatures(void) const = 0;
  virtual int GetDVDeskUpdatePeriod(void) const = 0;
  virtual bool GetShowToolTips(void) const = 0;
  virtual bool GetShowExFixtures(void) const = 0;
  virtual bool GetHexDataMode(void) const = 0;
  virtual int GetNumPulsesPer1Km(void) const = 0;
};
