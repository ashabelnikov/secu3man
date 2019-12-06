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
 int m_CrankingThrdMapWnd_X;
 int m_CrankingThrdMapWnd_Y;
 int m_CrankingTimeMapWnd_X;
 int m_CrankingTimeMapWnd_Y;
 int m_SmapabanThrdMapWnd_X;
 int m_SmapabanThrdMapWnd_Y;
};

//Holds windows's size data
struct WndSize
{
 int m_MainFrmWnd_W;
 int m_MainFrmWnd_H;
};

//Holds windows's state data
struct WndState
{
 int m_MainFrmWnd;
};

struct IndicatorsCfg
{
 float m_optIndHeightPercent[2];
 int m_optIndRows[2];
 std::pair<int, COLORREF> m_optIndGas_v[2];
 std::pair<int, COLORREF> m_optIndCarb[2];
 std::pair<int, COLORREF> m_optIndIdleValve[2];
 std::pair<int, COLORREF> m_optIndPowerValve[2];
 std::pair<int, COLORREF> m_optIndStBlock[2];
 std::pair<int, COLORREF> m_optIndAE[2];
 std::pair<int, COLORREF> m_optIndCoolingFan[2];
 std::pair<int, COLORREF> m_optIndCE[2]; 
 std::pair<int, COLORREF> m_optIndFCRevLim[2];
 std::pair<int, COLORREF> m_optIndFloodClear[2];
 std::pair<int, COLORREF> m_optIndSysLocked[2];
 std::pair<int, COLORREF> m_optIndIgn_i[2];
 std::pair<int, COLORREF> m_optIndCond_i[2];
 std::pair<int, COLORREF> m_optIndEpas_i[2];
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
 int m_optMetVehicleSpeed[2];
 int m_optMetTPSDot[2];
 int m_optMetMAP2[2];
 int m_optMetMAPD[2];
 int m_optMetTmp2[2];
 int m_optMetFuelConsum[2];
 int m_optMetKnockRetard[2];
 int m_optMetKnockGraph[2];
 int m_optMetSensAFR[2];
 int m_optMetChokePos[2];
 int m_optMetGDPos[2];
 int m_optMetSynLoad[2];
 int m_optMetInjTimB[2];
 int m_optMetInjTimE[2];
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

struct MapPtMovStep
{
 float m_work_map;
 float m_temp_map;
 float m_start_map;
 float m_idle_map;
 float m_ve_map;
 float m_afr_map;
 float m_crnk_map;
 float m_wrmp_map;
 float m_dead_map;
 float m_idlr_map;
 float m_idlc_map;
 float m_aetps_map;
 float m_aerpm_map;
 float m_aftstr_map;
 float m_it_map;
 float m_itrpm_map;
 float m_rigid_map;
 float m_egocrv_map;
 float m_iacc_map;
 float m_iaccw_map;
 float m_iatclt_map;
 float m_tpsswt_map;
 float m_gtsc_map;
 float m_gpsc_map;
 float m_atsc_map;
 //separate
 float m_attenuator_map;
 float m_dwellcntrl_map;
 float m_cts_curve_map;
 float m_ats_curve_map;
 float m_ats_aac_map;
 float m_gasdose_map;
 float m_cesettings;
 float m_barocorr_map;
 float m_manigntim_map;
 float m_tmp2_curve_map;
 float m_crktemp_map;
 float m_eh_pause_map;
 float m_cranking_thrd_map;
 float m_cranking_time_map;
 float m_smapaban_thrd_map;
};

//Log file's fileds
struct LogFileFields
{
 _TSTRING m_fldTime;
 _TSTRING m_fldRPM;
 _TSTRING m_fldIgnTim;
 _TSTRING m_fldMAP;
 _TSTRING m_fldVBat;
 _TSTRING m_fldCLT;
 _TSTRING m_fldKnock;
 _TSTRING m_fldKnockCorr;
 _TSTRING m_fldLoadCurve;
 _TSTRING m_fldCarb;
 _TSTRING m_fldGas_v;
 _TSTRING m_fldIdleValve;
 _TSTRING m_fldPowerValve;
 _TSTRING m_fldCoolingFan;
 _TSTRING m_fldStBlock;
 _TSTRING m_fldAE;
 _TSTRING m_fldFCRevLim;
 _TSTRING m_fldFloodClear;
 _TSTRING m_fldSysLocked;
 _TSTRING m_fldCE;
 _TSTRING m_fldIgn_i;
 _TSTRING m_fldCond_i;
 _TSTRING m_fldEpas_i;
 _TSTRING m_fldTPS;
 _TSTRING m_fldAdd_i1;
 _TSTRING m_fldAdd_i2;
 _TSTRING m_fldChokePos;
 _TSTRING m_fldGDPos;
 _TSTRING m_fldVehSpeed;
 _TSTRING m_fldPasDist;
 _TSTRING m_fldFuelConsum;
 _TSTRING m_fldFuelConsFreq;
 _TSTRING m_fldIAT;
 _TSTRING m_fldStrtIgnTim;
 _TSTRING m_fldIdleIgnTim;
 _TSTRING m_fldWorkIgnTim;
 _TSTRING m_fldTempIgnTim;
 _TSTRING m_fldIATIgnTim;
 _TSTRING m_fldIdlRegIgnTim;
 _TSTRING m_fldOctanCorr;
 _TSTRING m_fldEGOCorr;
 _TSTRING m_fldInjPW;
 _TSTRING m_fldTPSdot;
 _TSTRING m_fldMAP2;
 _TSTRING m_fldTmp2;
 _TSTRING m_fldDiffMAP;
 _TSTRING m_fldAFR;
 _TSTRING m_fldSynLoad;
 _TSTRING m_fldBaroPress;
 _TSTRING m_fldInjTimBeg;
 _TSTRING m_fldInjTimEnd;
 _TSTRING m_fldLogMarks;
 _TSTRING m_fldCECodes;
};

//Functionality
struct Functionality
{
 bool SM_CONTROL;
 bool GD_CONTROL;
};

//Interface which should be used to access settings' data from usual places of program
class ISettingsData
{
 public:
  virtual const _TSTRING& GetPortName(void) const = 0;
  virtual void SetPortName(const _TSTRING& name) = 0;
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
  virtual bool GetAllowVisualTheme(void) const = 0;
  virtual bool GetAutoDiagEnter(void) const = 0;
  virtual bool GetSaveWarning(void) const = 0;
  virtual bool GetAutoCERead(void) const = 0;
  virtual bool GetChildCharts(void) const = 0;
  virtual void SetChildCharts(bool child) = 0;
  virtual bool GetPortAutoReopen(void) const = 0;
  virtual bool GetToggleMapWnd(void) const = 0;
  virtual int GetParamMonVert(void) const = 0;
  virtual void SetParamMonVert(int pos) = 0;
  virtual bool GetExistingPorts(void) const = 0;

  //Fixtures's customization settings
  virtual int GetTachometerMax(void) const = 0;
  virtual int GetPressureMax(void) const = 0;
  virtual ESpeedUnit GetSpeedUnit(void) const = 0;
  virtual int GetRPMAverage(void) const = 0;
  virtual int GetVoltAverage(void) const = 0;
  virtual int GetMAPAverage(void) const = 0;
  virtual int GetAI1Average(void) const = 0;
  virtual int GetTPSAverage(void) const = 0;
  virtual int GetTitleFontSize(void) const = 0;
  virtual int GetValueFontSize(void) const = 0;
  virtual int GetPaneFontSize(void) const = 0;
  virtual int GetLabelFontSize(void) const = 0;

  virtual void SetTitleFontSize(int size) = 0;
  virtual void SetValueFontSize(int size) = 0;
  virtual void SetPaneFontSize(int size) = 0;
  virtual void SetLabelFontSize(int size) = 0;

  virtual bool GetMetersDragNDrop(void) const = 0;
  virtual void SetMetersDragNDrop(bool enable) = 0;

  virtual bool GetIndicatorsDragNDrop(void) const = 0;
  virtual void SetIndicatorsDragNDrop(bool enable) = 0;

  virtual int GetFFFConst(void) const = 0;
  
  //windows' positions
  virtual void SetWndSettings(const WndSettings& i_wndSettings) = 0;
  virtual void GetWndSettings(WndSettings& o_wndSettings) const = 0;
  virtual void SetWndSettings1(const WndSettings& i_wndSettings) = 0; //online
  virtual void GetWndSettings1(WndSettings& o_wndSettings) const = 0; //online

  //Interface language and SECU-3 platform type
  virtual EInterLang GetInterfaceLanguage(void) const = 0;
  virtual EECUPlatform GetECUPlatformType(void) const = 0;
  //Debug features
  virtual bool GetUseDVFeatures(void) const = 0;
  virtual int GetDVDeskUpdatePeriod(void) const = 0;
  virtual bool GetShowToolTips(void) const = 0;
  virtual bool GetShowExFixtures(void) const = 0;
  virtual void SetShowExFixtures(bool i_show) = 0;
  virtual bool GetHexDataMode(void) const = 0;
  virtual int GetNumPulsesPer1Km(void) const = 0;

  //Indicators
  virtual void GetIndicatorsConfig(IndicatorsCfg& o_cfg) const = 0;
  virtual void SetIndicatorsConfig(const IndicatorsCfg& i_cfg) = 0;
  //Meters
  virtual void GetMetersConfig(MetersCfg& o_cfg) const = 0;
  virtual void SetMetersConfig(const MetersCfg& i_cfg) = 0;

  virtual void SetWndSize(const WndSize& i_wndSize) = 0;
  virtual void GetWndSize(WndSize& o_wndSize) const = 0;

  virtual void SetWndState(const WndState& i_wndState) = 0;
  virtual void GetWndState(WndState& o_wndState) const = 0;

  virtual void SetLamDelMap(float* map, float* rb, float* lb) = 0;
  virtual void GetLamDelMap(float* map, float* rb, float* lb) = 0;
  virtual float GetAFRError(void) = 0;
  virtual void SetBlockedCells(const bool* blList) = 0;
  virtual void GetBlockedCells(bool* blList) = 0;
  virtual int GetStatSize(void) = 0;
  virtual int GetAutoBlockThrd(void) = 0;
  virtual bool GetGrowingMode(void) = 0;
  virtual float GetMinAFR(void) = 0;
  virtual float GetMaxAFR(void) = 0;
  virtual int GetMinDistThrd(void) = 0;
  virtual float GetMinTPS(void) = 0;
  virtual float GetMaxTPS(void) = 0;
  virtual float GetCLTThrd(void) = 0;

  virtual int GetGradSaturation(void) = 0;
  virtual int GetGradBrightness(void) = 0;
  virtual int GetBoldFont(void) = 0;
  virtual int GetITEdMode(void) const = 0;
  virtual void SetITEdMode(int mode) = 0;

  //Inj.driver
  virtual bool GetInjDrvTabActive(void) const = 0;
  virtual COLORREF GetVoltLineColor(void) const = 0;
  virtual float GetPeakOnPtMovStep(void) const = 0;
  virtual float GetPeakDutyPtMovStep(void) const = 0;
  virtual float GetHoldDutyPtMovStep(void) const = 0;
  virtual float GetPeakFullPtMovStep(void) const = 0;
  virtual float GetPthPausePtMovStep(void) const = 0;
  virtual bool GetShowGraphLabels(void) const = 0;

  virtual void SetMapPtMovStep(const MapPtMovStep& i_ptMovStep) = 0;
  virtual void GetMapPtMovStep(MapPtMovStep& o_ptMovStep) const = 0;

  virtual void SetLogFileFields(const LogFileFields& i_flds) = 0;
  virtual void GetLogFileFields(LogFileFields& o_flds) const = 0;

  virtual bool GetWriteLogFields(void) const = 0;
  virtual void SetWriteLogFields(bool value) = 0;

  virtual void GetFunctionality(Functionality& o_fnc) const = 0;
};
