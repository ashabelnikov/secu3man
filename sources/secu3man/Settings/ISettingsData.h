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
 int m_MainFrmWnd_X;  //main window x
 int m_MainFrmWnd_Y;  //main window y
 int m_StrtMapWnd_X;
 int m_StrtMapWnd_Y;
 int m_IdleMapWnd_X;
 int m_IdleMapWnd_Y;
 int m_WorkMapWnd_X;
 int m_WorkMapWnd_Y;
 int m_TempMapWnd_X;
 int m_TempMapWnd_Y;
 int m_TempIdlMapWnd_X;
 int m_TempIdlMapWnd_Y;
 int m_AttenuatorMapWnd_X;
 int m_AttenuatorMapWnd_Y;
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
 int m_VE2MapWnd_X;
 int m_VE2MapWnd_Y;
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
 int m_ThrassMapWnd_X;
 int m_ThrassMapWnd_Y;
 int m_ATSCurvMapWnd_X;
 int m_ATSCurvMapWnd_Y;
 int m_ATSCorrMapWnd_X;
 int m_ATSCorrMapWnd_Y;
 int m_AETPSMapWnd_X;
 int m_AETPSMapWnd_Y;
 int m_AEMAPMapWnd_X;
 int m_AEMAPMapWnd_Y;
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
 int m_TpszonMapWnd_X;
 int m_TpszonMapWnd_Y;
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
 int m_Pwm1MapWnd_X;
 int m_Pwm1MapWnd_Y;
 int m_Pwm2MapWnd_X;
 int m_Pwm2MapWnd_Y;
 int m_KnockZoneMapWnd_X;
 int m_KnockZoneMapWnd_Y;
 int m_GrtsCurveMapWnd_X;
 int m_GrtsCurveMapWnd_Y;
 int m_GrHeatDutyMapWnd_X;
 int m_GrHeatDutyMapWnd_Y;
 int m_PwmIacUCoefMapWnd_X;
 int m_PwmIacUCoefMapWnd_Y;
 int m_AftstrStrk0MapWnd_X;
 int m_AftstrStrk0MapWnd_Y;
 int m_AftstrStrk1MapWnd_X;
 int m_AftstrStrk1MapWnd_Y;
 int m_GrValDelMapWnd_X;
 int m_GrValDelMapWnd_Y;
 int m_IACMATMapWnd_X;
 int m_IACMATMapWnd_Y;
 int m_FtlsCurveMapWnd_X;
 int m_FtlsCurveMapWnd_Y;
 int m_EgtsCurveMapWnd_X;
 int m_EgtsCurveMapWnd_Y;
 int m_OpsCurveMapWnd_X;
 int m_OpsCurveMapWnd_Y;
 int m_ManInjPwcMapWnd_X;
 int m_ManInjPwcMapWnd_Y;
 int m_MAFCurveMapWnd_X;
 int m_MAFCurveMapWnd_Y;
 int m_FtlsCorMapWnd_X;
 int m_FtlsCorMapWnd_Y;
 int m_LambdaZoneMapWnd_X;
 int m_LambdaZoneMapWnd_Y;
 int m_CylMultMapWnd_X;
 int m_CylMultMapWnd_Y;
 int m_CylAddMapWnd_X;
 int m_CylAddMapWnd_Y;
 int m_FtsCurveMapWnd_X;
 int m_FtsCurveMapWnd_Y;
 int m_FuelDensCorrMapWnd_X;
 int m_FuelDensCorrMapWnd_Y;
 int m_XtauXfAccMapWnd_X;
 int m_XtauXfAccMapWnd_Y;
 int m_XtauXfDecMapWnd_X;
 int m_XtauXfDecMapWnd_Y;
 int m_XtauTfAccMapWnd_X;
 int m_XtauTfAccMapWnd_Y;
 int m_XtauTfDecMapWnd_X;
 int m_XtauTfDecMapWnd_Y;
};

//Holds windows's size data
struct WndSize
{
 int m_MainFrmWnd_W; //main window width
 int m_MainFrmWnd_H; //main window height
 int m_StrtMapWnd_W;
 int m_StrtMapWnd_H;
 int m_IdleMapWnd_W;
 int m_IdleMapWnd_H;
 int m_WorkMapWnd_W;
 int m_WorkMapWnd_H;
 int m_TempMapWnd_W;
 int m_TempMapWnd_H;
 int m_TempIdlMapWnd_W;
 int m_TempIdlMapWnd_H;
 int m_AttenuatorMapWnd_W;
 int m_AttenuatorMapWnd_H;
 int m_DwellCntrlMapWnd_W;
 int m_DwellCntrlMapWnd_H;
 int m_CTSCurveMapWnd_W;
 int m_CTSCurveMapWnd_H;
 int m_GridMapIgnWnd_W;
 int m_GridMapIgnWnd_H;
 int m_GridMapInjWnd_W;
 int m_GridMapInjWnd_H;
 int m_VEMapWnd_W;
 int m_VEMapWnd_H;
 int m_VE2MapWnd_W;
 int m_VE2MapWnd_H;
 int m_AFRMapWnd_W;
 int m_AFRMapWnd_H;
 int m_CrnkMapWnd_W;
 int m_CrnkMapWnd_H;
 int m_WrmpMapWnd_W;
 int m_WrmpMapWnd_H;
 int m_DeadMapWnd_W;
 int m_DeadMapWnd_H;
 int m_IdlrMapWnd_W;
 int m_IdlrMapWnd_H;
 int m_IdlcMapWnd_W;
 int m_IdlcMapWnd_H;
 int m_ThrassMapWnd_W;
 int m_ThrassMapWnd_H;
 int m_ATSCurvMapWnd_W;
 int m_ATSCurvMapWnd_H;
 int m_ATSCorrMapWnd_W;
 int m_ATSCorrMapWnd_H;
 int m_AETPSMapWnd_W;
 int m_AETPSMapWnd_H;
 int m_AEMAPMapWnd_W;
 int m_AEMAPMapWnd_H;
 int m_AERPMMapWnd_W;
 int m_AERPMMapWnd_H;
 int m_AftstrMapWnd_W;
 int m_AftstrMapWnd_H;
 int m_GasdoseMapWnd_W;
 int m_GasdoseMapWnd_H;
 int m_ITMapWnd_W;
 int m_ITMapWnd_H;
 int m_ITRPMMapWnd_W;
 int m_ITRPMMapWnd_H;
 int m_RigidMapWnd_W;
 int m_RigidMapWnd_H;
 int m_EGOCrvMapWnd_W;
 int m_EGOCrvMapWnd_H;
 int m_IACCMapWnd_W;
 int m_IACCMapWnd_H;
 int m_IACCWMapWnd_W;
 int m_IACCWMapWnd_H;
 int m_IATCLTMapWnd_W;
 int m_IATCLTMapWnd_H;
 int m_BarocorrMapWnd_W;
 int m_BarocorrMapWnd_H;
 int m_ManIgntimMapWnd_W;
 int m_ManIgntimMapWnd_H;
 int m_CESettingsWnd_W;
 int m_CESettingsWnd_H;
 int m_TpsswtMapWnd_W;
 int m_TpsswtMapWnd_H;
 int m_TpszonMapWnd_W;
 int m_TpszonMapWnd_H;
 int m_Tmp2CurveMapWnd_W;
 int m_Tmp2CurveMapWnd_H;
 int m_GtscMapWnd_W;
 int m_GtscMapWnd_H;
 int m_GpscMapWnd_W;
 int m_GpscMapWnd_H;
 int m_AtscMapWnd_W;
 int m_AtscMapWnd_H;
 int m_CrkTempMapWnd_W;
 int m_CrkTempMapWnd_H;
 int m_EHPauseMapWnd_W;
 int m_EHPauseMapWnd_H;
 int m_CrankingThrdMapWnd_W;
 int m_CrankingThrdMapWnd_H;
 int m_CrankingTimeMapWnd_W;
 int m_CrankingTimeMapWnd_H;
 int m_SmapabanThrdMapWnd_W;
 int m_SmapabanThrdMapWnd_H;
 int m_Pwm1MapWnd_W;
 int m_Pwm1MapWnd_H;
 int m_Pwm2MapWnd_W;
 int m_Pwm2MapWnd_H;
 int m_KnockZoneMapWnd_W;
 int m_KnockZoneMapWnd_H;
 int m_GrtsCurveMapWnd_W;
 int m_GrtsCurveMapWnd_H;
 int m_GrHeatDutyMapWnd_W;
 int m_GrHeatDutyMapWnd_H;
 int m_PwmIacUCoefMapWnd_W;
 int m_PwmIacUCoefMapWnd_H;
 int m_AftstrStrk0MapWnd_W;
 int m_AftstrStrk0MapWnd_H;
 int m_AftstrStrk1MapWnd_W;
 int m_AftstrStrk1MapWnd_H;
 int m_GrValDelMapWnd_W;
 int m_GrValDelMapWnd_H;
 int m_IACMATMapWnd_W;
 int m_IACMATMapWnd_H;
 int m_FtlsCurveMapWnd_W;
 int m_FtlsCurveMapWnd_H;
 int m_EgtsCurveMapWnd_W;
 int m_EgtsCurveMapWnd_H;
 int m_OpsCurveMapWnd_W;
 int m_OpsCurveMapWnd_H;
 int m_ManInjPwcMapWnd_W;
 int m_ManInjPwcMapWnd_H;
 int m_MAFCurveMapWnd_W;
 int m_MAFCurveMapWnd_H;
 int m_FtlsCorMapWnd_W;
 int m_FtlsCorMapWnd_H;
 int m_LambdaZoneMapWnd_W;
 int m_LambdaZoneMapWnd_H;
 int m_CylMultMapWnd_W;
 int m_CylMultMapWnd_H;
 int m_CylAddMapWnd_W;
 int m_CylAddMapWnd_H;
 int m_FtsCurveMapWnd_W;
 int m_FtsCurveMapWnd_H;
 int m_FuelDensCorrMapWnd_W;
 int m_FuelDensCorrMapWnd_H;
 int m_XtauXfAccMapWnd_W;
 int m_XtauXfAccMapWnd_H;
 int m_XtauXfDecMapWnd_W;
 int m_XtauXfDecMapWnd_H;
 int m_XtauTfAccMapWnd_W;
 int m_XtauTfAccMapWnd_H;
 int m_XtauTfDecMapWnd_W;
 int m_XtauTfDecMapWnd_H;
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
 std::pair<int, COLORREF> m_optIndAftStrEnr[2];
 std::pair<int, COLORREF> m_optIndIacClLoop[2];
 std::pair<int, COLORREF> m_optIndUniOut1[2];
 std::pair<int, COLORREF> m_optIndUniOut2[2];
 std::pair<int, COLORREF> m_optIndUniOut3[2];
 std::pair<int, COLORREF> m_optIndUniOut4[2];
 std::pair<int, COLORREF> m_optIndUniOut5[2];
 std::pair<int, COLORREF> m_optIndUniOut6[2];
};

struct MetersCfg
{
 int m_optMetRows;
 int m_optMetRPM[2];
 int m_optMetMAP[2];
 int m_optMetVBat[2];
 int m_optMetIgnTim[2];
 int m_optMetCLT[2];
 int m_optMetAddI1[2];
 int m_optMetAddI2[2];
 int m_optMetInjPW[2];
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
 int m_optMetFuelConsumF[2];
 int m_optMetGrts[2];
 int m_optMetFtls[2];
 int m_optMetEgts[2];
 int m_optMetOps[2];
 int m_optMetInjDuty[2];
 int m_optMetMAF[2];
 int m_optMetVentDuty[2];
 int m_optMetMAPDot[2];
 int m_optMetFts[2];
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
 float m_tempi_map;
 float m_start_map;
 float m_idle_map;
 float m_ve_map;
 float m_ve2_map;
 float m_afr_map;
 float m_crnk_map;
 float m_wrmp_map;
 float m_dead_map;
 float m_idlr_map;
 float m_idlc_map;
 float m_thrass_map;
 float m_aetps_map;
 float m_aemap_map;
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
 float m_pwm1_map;
 float m_pwm2_map;
 float m_iacmat_map;
 float m_tpszon_map;
 float m_cylmult_map;
 float m_cyladd_map;
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
 float m_knock_zone_map;
 float m_grts_curve_map;
 float m_grheat_duty_map;
 float m_pwmiac_ucoef_map;
 float m_aftstr_strk0_map;
 float m_aftstr_strk1_map;
 float m_grvaldel_map;
 float m_ftls_curve_map;
 float m_egts_curve_map;
 float m_ops_curve_map;
 float m_maninjpwc_map;
 float m_mafcurve_map;
 float m_ftlscor_map;
 float m_lambda_zone_map;
 float m_fts_curve_map;
 float m_fueldens_corr_map;
 float m_xtau_xfacc_map;
 float m_xtau_xfdec_map;
 float m_xtau_tfacc_map;
 float m_xtau_tfdec_map;
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
 _TSTRING m_fldGrts;
 _TSTRING m_fldFtls;
 _TSTRING m_fldEgts;
 _TSTRING m_fldOps;
 _TSTRING m_fldAftStrEnr;
 _TSTRING m_fldIacClLoop;
 _TSTRING m_fldInjDuty;
 _TSTRING m_fldRigidArg;
 _TSTRING m_fldServFlag;
 _TSTRING m_fldRxlaf;
 _TSTRING m_fldMAF;
 _TSTRING m_fldVentDuty;
 _TSTRING m_fldUniOuts;
 _TSTRING m_fldMAPdot;
 _TSTRING m_fldFts;
 _TSTRING m_fldConsFuel;
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
  virtual CString GetINIFileFullName(void) const = 0;

  virtual const _TSTRING& GetPortName(void) const = 0;
  virtual void SetPortName(const _TSTRING& name) = 0;
  virtual DWORD GetBaudRateApplication(void) const = 0;
  virtual DWORD GetBaudRateBootloader(void) const = 0;
  virtual const _TSTRING& GetLogFilesFolder(void) const = 0;
  virtual bool  GetUseAppFolder(void) const = 0;
  virtual bool  GetAlwaysWriteLog(void) const = 0;
  virtual char  GetCSVSepSymbol(void) const = 0;
  virtual char  GetMapCSVSepSymbol(void) const = 0;
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
  virtual void SetToggleMapWnd(bool toggle) = 0;
  virtual int GetParamMonVert(void) const = 0;
  virtual void SetParamMonVert(int pos) = 0;
  virtual int GetLogPlayerVert(void) const = 0;
  virtual void SetLogPlayerVert(int pos) = 0;
  virtual bool GetExistingPorts(void) const = 0;
  virtual int GetToolTipTime(void) const = 0;
  virtual int GetIniEditorSyntax(void) const = 0;
  virtual int GetBldrEEPROMBlocks(void) const = 0;
  virtual float GetFuelDensity1(void) const = 0;
  virtual float GetFuelDensity2(void) const = 0;
  virtual bool GetLogBinaryFmt(void) const = 0;

  //Fixtures's customization settings
  virtual int GetTachometerMax(void) const = 0;
  virtual int GetTemperatureMax(void) const = 0;
  virtual int GetPressureMax(void) const = 0;
  virtual int GetInjPWMax(void) const = 0;
  virtual ESpeedUnit GetSpeedUnit(void) const = 0;
  virtual int GetRPMAverage(void) const = 0;
  virtual int GetVoltAverage(void) const = 0;
  virtual int GetMAPAverage(void) const = 0;
  virtual int GetAI1Average(void) const = 0;
  virtual int GetTPSAverage(void) const = 0;
  virtual int GetKnockAverage(void) const = 0;
  virtual int GetIgnTimAverage(void) const = 0;
  virtual int GetCLTAverage(void) const = 0;
  virtual int GetAddI2Average(void) const = 0;
  virtual int GetInjPWAverage(void) const = 0;
  virtual int GetIATAverage(void) const = 0;
  virtual int GetEGOCorrAverage(void) const = 0;
  virtual int GetAirFlowAverage(void) const = 0;
  virtual int GetVehicleSpeedAverage(void) const = 0;
  virtual int GetTPSDotAverage(void) const = 0;
  virtual int GetMAP2Average(void) const = 0;
  virtual int GetMAPDAverage(void) const = 0;
  virtual int GetTmp2Average(void) const = 0;
  virtual int GetFuelConsumAverage(void) const = 0;
  virtual int GetFuelConsumFAverage(void) const = 0;
  virtual int GetKnockRetardAverage(void) const = 0;
  virtual int GetSensAFRAverage(void) const = 0;
  virtual int GetChokePosAverage(void) const = 0;
  virtual int GetGDPosAverage(void) const = 0;
  virtual int GetSynLoadAverage(void) const = 0;
  virtual int GetInjTimBAverage(void) const = 0;
  virtual int GetInjTimEAverage(void) const = 0;
  virtual int GetGrtsAverage(void) const = 0;
  virtual int GetFtlsAverage(void) const = 0;
  virtual int GetEgtsAverage(void) const = 0;
  virtual int GetOpsAverage(void) const = 0;
  virtual int GetMAFAverage(void) const = 0;
  virtual int GetFtsAverage(void) const = 0;

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
  virtual bool GetShowGraphsCursor(void) = 0;
  virtual int GetGraphShtPixels(void) = 0;
  virtual int GetGraphShowValue(void) = 0;
  virtual int GetGraphValueHeight(void) = 0;
  
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
  virtual bool GetShowSpeedAndDist(void) const = 0;
  virtual void SetShowSpeedAndDist(bool i_show) = 0;

  //Indicators
  virtual void GetIndicatorsConfig(IndicatorsCfg& o_cfg) const = 0;
  virtual void SetIndicatorsConfig(const IndicatorsCfg& i_cfg) = 0;
  //Meters
  virtual void GetMetersConfig(MetersCfg* o_cfg) const = 0;
  virtual void SetMetersConfig(const MetersCfg* i_cfg) = 0;

  virtual void SetWndSize(const WndSize& i_wndSize) = 0;
  virtual void GetWndSize(WndSize& o_wndSize) const = 0;
  virtual void SetWndSize1(const WndSize& i_wndSize) = 0;
  virtual void GetWndSize1(WndSize& o_wndSize) const = 0;

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
  virtual float GetTunSoftness(void) = 0;

  virtual int GetGradSaturation(void) = 0;
  virtual int GetGradBrightness(void) = 0;
  virtual int GetBoldFont(void) = 0;
  virtual int GetITEdMode(void) const = 0;
  virtual void SetITEdMode(int mode) = 0;
  virtual int GetActiveVEMap(void) const = 0;
  virtual void SetActiveVEMap(int id) = 0;

  //Inj.driver
  virtual bool GetInjDrvTabActive(void) const = 0;
  virtual COLORREF GetVoltLineColor(void) const = 0;
  virtual float GetPeakOnPtMovStep(void) const = 0;
  virtual float GetPeakDutyPtMovStep(void) const = 0;
  virtual float GetHoldDutyPtMovStep(void) const = 0;
  virtual float GetPeakFullPtMovStep(void) const = 0;
  virtual float GetPthPausePtMovStep(void) const = 0;
  virtual float GetPWMulPtMovStep(void) const = 0;
  virtual float GetPWAddPtMovStep(void) const = 0;
  virtual bool GetShowGraphLabels(void) const = 0;

  virtual void SetMapPtMovStep(const MapPtMovStep& i_ptMovStep) = 0;
  virtual void GetMapPtMovStep(MapPtMovStep& o_ptMovStep) const = 0;

  virtual void SetLogFileFields(const LogFileFields& i_flds) = 0;
  virtual void GetLogFileFields(LogFileFields& o_flds) const = 0;

  virtual bool GetWriteLogFields(void) const = 0;
  virtual void SetWriteLogFields(bool value) = 0;

  virtual void GetFunctionality(Functionality& o_fnc) const = 0;

  virtual bool GetSpotMarkers(void) const = 0;
  virtual float GetSpotMarkersSize(void) const = 0;
};
