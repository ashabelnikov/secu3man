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
 int m_GridMapIgnWnd_X;
 int m_GridMapIgnWnd_Y;
 int m_GridMapInjWnd_X;
 int m_GridMapInjWnd_Y;
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
 int m_ITRPMMapWnd_X;
 int m_ITRPMMapWnd_Y;
 int m_RigidMapWnd_X;
 int m_RigidMapWnd_Y;
 int m_EGOCrvMapWnd_X;
 int m_EGOCrvMapWnd_Y;
 int m_IACCMapWnd_X;
 int m_IACCMapWnd_Y;
 int m_IACCWMapWnd_X;
 int m_IACCWMapWnd_Y;
 int m_IATCLTMapWnd_X;
 int m_IATCLTMapWnd_Y;
 int m_BarocorrMapWnd_X;
 int m_BarocorrMapWnd_Y;
 int m_ManIgntimMapWnd_X;
 int m_ManIgntimMapWnd_Y;
 int m_CESettingsWnd_X;
 int m_CESettingsWnd_Y;
 int m_TpsswtMapWnd_X;
 int m_TpsswtMapWnd_Y;
 int m_Tmp2CurveMapWnd_X;
 int m_Tmp2CurveMapWnd_Y;
 int m_GtscMapWnd_X;
 int m_GtscMapWnd_Y;
 int m_GpscMapWnd_X;
 int m_GpscMapWnd_Y;
 int m_AtscMapWnd_X;
 int m_AtscMapWnd_Y;
 int m_CrkTempMapWnd_X;
 int m_CrkTempMapWnd_Y;
 int m_EHPauseMapWnd_X;
 int m_EHPauseMapWnd_Y;
};

struct IndicatorsCfg
{
 int m_optIndRows[2];
 int m_optIndGas_v[2];
 int m_optIndCarb[2];
 int m_optIndIdleValve[2];
 int m_optIndPowerValve[2];
 int m_optStBlock[2];
 int m_optAE[2];
 int m_optCoolingFan[2];
};

struct MetersCfg
{
 int m_optMetRows[2];
 int m_optMetRPM[2];
 int m_optMetMAP[2];
 int m_optMetVBat[2];
 int m_optMetIgnTim[2];
 int m_optMetCLT[2];
 int m_optMetAddI1[2];
 int m_optMetAddI2[2];
 int m_optInjPW[2];
 int m_optMetIAT[2];
 int m_optMetEGOCorr[2];
 int m_optMetTPS[2];
 int m_optMetAirFlow[2];
};

typedef enum EInterLang
{
 IL_ENGLISH = 0,
 IL_RUSSIAN
};

typedef enum ESpeedUnit
{
 SU_KMH = 0,
 SU_MPH
};

//Interface which should be used to access settings' data from usual places of program
class ISettingsData
{
 public:
  virtual const _TSTRING& GetPortName(void) const = 0;
  virtual DWORD GetBaudRateApplication(void) const = 0;
  virtual DWORD GetBaudRateBootloader(void) const = 0;
  virtual const _TSTRING& GetLogFilesFolder(void) const = 0;
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
  virtual int GetTPSAverage(void) const = 0;
  virtual bool GetAllowVisualTheme(void) const = 0;
  virtual bool GetAutoDiagEnter(void) const = 0;
  virtual bool GetSaveWarning(void) const = 0;
  virtual bool GetAutoCERead(void) const = 0;
  virtual bool GetChildCharts(void) const = 0;
  //windows' positions
  virtual void SetWndSettings(const WndSettings& i_wndSettings) = 0;
  virtual void GetWndSettings(WndSettings& o_wndSettings) const = 0;
  //Interface language and SECU-3 platform type
  virtual EInterLang GetInterfaceLanguage(void) const = 0;
  virtual EECUPlatform GetECUPlatformType(void) const = 0;
  //Fixtures's customization settings
  virtual int GetTachometerMax(void) const = 0;
  virtual int GetPressureMax(void) const = 0;
  virtual ESpeedUnit GetSpeedUnit(void) const = 0;
  //Debug features
  virtual bool GetUseDVFeatures(void) const = 0;
  virtual int GetDVDeskUpdatePeriod(void) const = 0;
  virtual bool GetShowToolTips(void) const = 0;
  virtual bool GetShowExFixtures(void) const = 0;
  virtual bool GetHexDataMode(void) const = 0;
  virtual int GetNumPulsesPer1Km(void) const = 0;

  //Indicators
  virtual void GetIndicatorsConfig(IndicatorsCfg& o_cfg) const = 0;
  //Meters
  virtual void GetMetersConfig(MetersCfg& o_cfg) const = 0;
};
