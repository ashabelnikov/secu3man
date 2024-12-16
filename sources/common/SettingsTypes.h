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

/** \file SettingsTypes.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "UnicodeSupport.h"

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

//Holds windows's position data
struct WndSettings
{
 int m_MainFrmWnd_X;  //main window x
 int m_MainFrmWnd_Y;  //main window y
 //set of tables
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
 int m_VEMapWnd_X;
 int m_VEMapWnd_Y;
 int m_VE2MapWnd_X;
 int m_VE2MapWnd_Y;
 int m_VEIMapWnd_X;
 int m_VEIMapWnd_Y;
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
 int m_AETPSMapWnd_X;
 int m_AETPSMapWnd_Y;
 int m_AEMAPMapWnd_X;
 int m_AEMAPMapWnd_Y;
 int m_AERPMMapWnd_X;
 int m_AERPMMapWnd_Y;
 int m_AftstrMapWnd_X;
 int m_AftstrMapWnd_Y;
 int m_ITMapWnd_X;
 int m_ITMapWnd_Y;
 int m_ITRPMMapWnd_X;
 int m_ITRPMMapWnd_Y;
 int m_RigidMapWnd_X;
 int m_RigidMapWnd_Y;
 int m_EGOCrvMapWnd_X;
 int m_EGOCrvMapWnd_Y;
 int m_GtscMapWnd_X;
 int m_GtscMapWnd_Y;
 int m_GpscMapWnd_X;
 int m_GpscMapWnd_Y;
 int m_TpsswtMapWnd_X;
 int m_TpsswtMapWnd_Y;
 int m_TpszonMapWnd_X;
 int m_TpszonMapWnd_Y;
 int m_AtscMapWnd_X;
 int m_AtscMapWnd_Y;
 int m_IATCLTMapWnd_X;
 int m_IATCLTMapWnd_Y;
 int m_IACMATMapWnd_X;
 int m_IACMATMapWnd_Y;
 int m_IACCMapWnd_X;
 int m_IACCMapWnd_Y;
 int m_IACCWMapWnd_X;
 int m_IACCWMapWnd_Y;
 int m_Pwm1MapWnd_X;
 int m_Pwm1MapWnd_Y;
 int m_Pwm2MapWnd_X;
 int m_Pwm2MapWnd_Y;
 int m_CylMultMapWnd_X;
 int m_CylMultMapWnd_Y;
 int m_CylAddMapWnd_X;
 int m_CylAddMapWnd_Y;
 int m_GridMapIgnWnd_X; //group of ign. maps x
 int m_GridMapIgnWnd_Y; //group of ign. maps y
 int m_GridMapInjWnd_X; //group of inj. maps x
 int m_GridMapInjWnd_Y; //group of inj. maps y
 //group of separate maps
 int m_AttenuatorMapWnd_X;
 int m_AttenuatorMapWnd_Y;
 int m_DwellCntrlMapWnd_X;
 int m_DwellCntrlMapWnd_Y;
 int m_CTSCurveMapWnd_X;
 int m_CTSCurveMapWnd_Y;
 int m_ATSCurvMapWnd_X;
 int m_ATSCurvMapWnd_Y;
 int m_ATSCorrMapWnd_X;
 int m_ATSCorrMapWnd_Y;
 int m_GasdoseMapWnd_X;
 int m_GasdoseMapWnd_Y;
 int m_BarocorrMapWnd_X;
 int m_BarocorrMapWnd_Y;
 int m_ManIgntimMapWnd_X;
 int m_ManIgntimMapWnd_Y;
 int m_CESettingsWnd_X;
 int m_CESettingsWnd_Y;
 int m_Tmp2CurveMapWnd_X;
 int m_Tmp2CurveMapWnd_Y;
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
 int m_InjNonLinPMapWnd_X;
 int m_InjNonLinPMapWnd_Y;
 int m_InjNonLinGMapWnd_X;
 int m_InjNonLinGMapWnd_Y;
 int m_EGODelayMapWnd_X;
 int m_EGODelayMapWnd_Y;
 int m_WUAFR0MapWnd_X;
 int m_WUAFR0MapWnd_Y;
 int m_WUAFR1MapWnd_X;
 int m_WUAFR1MapWnd_Y;
};

//Holds windows's size data
struct WndSize
{
 int m_MainFrmWnd_W; //main window width
 int m_MainFrmWnd_H; //main window height
 //set of maps
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
 int m_VEMapWnd_W;
 int m_VEMapWnd_H;
 int m_VE2MapWnd_W;
 int m_VE2MapWnd_H;
 int m_VEIMapWnd_W;
 int m_VEIMapWnd_H;
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
 int m_AETPSMapWnd_W;
 int m_AETPSMapWnd_H;
 int m_AEMAPMapWnd_W;
 int m_AEMAPMapWnd_H;
 int m_AERPMMapWnd_W;
 int m_AERPMMapWnd_H;
 int m_AftstrMapWnd_W;
 int m_AftstrMapWnd_H;
 int m_ITMapWnd_W;
 int m_ITMapWnd_H;
 int m_ITRPMMapWnd_W;
 int m_ITRPMMapWnd_H;
 int m_RigidMapWnd_W;
 int m_RigidMapWnd_H;
 int m_EGOCrvMapWnd_W;
 int m_EGOCrvMapWnd_H;
 int m_IACMATMapWnd_W;
 int m_IACMATMapWnd_H;
 int m_IACCMapWnd_W;
 int m_IACCMapWnd_H;
 int m_IACCWMapWnd_W;
 int m_IACCWMapWnd_H;
 int m_IATCLTMapWnd_W;
 int m_IATCLTMapWnd_H;
 int m_TpsswtMapWnd_W;
 int m_TpsswtMapWnd_H;
 int m_TpszonMapWnd_W;
 int m_TpszonMapWnd_H;
 int m_GtscMapWnd_W;
 int m_GtscMapWnd_H;
 int m_GpscMapWnd_W;
 int m_GpscMapWnd_H;
 int m_AtscMapWnd_W;
 int m_AtscMapWnd_H;
 int m_Pwm1MapWnd_W;
 int m_Pwm1MapWnd_H;
 int m_Pwm2MapWnd_W;
 int m_Pwm2MapWnd_H;
 int m_CylMultMapWnd_W;
 int m_CylMultMapWnd_H;
 int m_CylAddMapWnd_W;
 int m_CylAddMapWnd_H;
 int m_GridMapIgnWnd_W; //group of ign. maps W
 int m_GridMapIgnWnd_H; //group of ign. maps H
 int m_GridMapInjWnd_W; //group of inj. maps W
 int m_GridMapInjWnd_H; //group of inj. maps H
 //group of separate maps
 int m_AttenuatorMapWnd_W;
 int m_AttenuatorMapWnd_H;
 int m_DwellCntrlMapWnd_W;
 int m_DwellCntrlMapWnd_H;
 int m_CTSCurveMapWnd_W;
 int m_CTSCurveMapWnd_H;
 int m_ATSCurvMapWnd_W;
 int m_ATSCurvMapWnd_H;
 int m_ATSCorrMapWnd_W;
 int m_ATSCorrMapWnd_H;
 int m_GasdoseMapWnd_W;
 int m_GasdoseMapWnd_H;
 int m_BarocorrMapWnd_W;
 int m_BarocorrMapWnd_H;
 int m_ManIgntimMapWnd_W;
 int m_ManIgntimMapWnd_H;
 int m_CESettingsWnd_W;
 int m_CESettingsWnd_H;
 int m_Tmp2CurveMapWnd_W;
 int m_Tmp2CurveMapWnd_H;
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
 int m_InjNonLinPMapWnd_W;
 int m_InjNonLinPMapWnd_H;
 int m_InjNonLinGMapWnd_W;
 int m_InjNonLinGMapWnd_H;
 int m_EGODelayMapWnd_W;
 int m_EGODelayMapWnd_H;
 int m_WUAFR0MapWnd_W;
 int m_WUAFR0MapWnd_H;
 int m_WUAFR1MapWnd_W;
 int m_WUAFR1MapWnd_H;
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
 std::pair<int, COLORREF> m_optIndGpa4_i[2];
 std::pair<int, COLORREF> m_optIndInput1[2];
 std::pair<int, COLORREF> m_optIndInput2[2];
 std::pair<int, COLORREF> m_optIndAuto_i[2];
 std::pair<int, COLORREF> m_optIndMapsel0[2];
 std::pair<int, COLORREF> m_optIndRefprs_i[2];
 std::pair<int, COLORREF> m_optIndAltrn_i[2];
};

struct MetCfg
{
 int position;
 float scaleMin;
 float scaleMax;
};

struct MetersCfg
{
 int m_optMetRows;
 MetCfg m_optMetRPM[2];
 MetCfg m_optMetMAP[2];
 MetCfg m_optMetVBat[2];
 MetCfg m_optMetIgnTim[2];
 MetCfg m_optMetCLT[2];
 MetCfg m_optMetAddI1[2];
 MetCfg m_optMetAddI2[2];
 MetCfg m_optMetInjPW[2];
 MetCfg m_optMetIAT[2];
 MetCfg m_optMetEGOCorr[2];
 MetCfg m_optMetTPS[2];
 MetCfg m_optMetAirFlow[2];
 MetCfg m_optMetVehicleSpeed[2];
 MetCfg m_optMetTPSDot[2];
 MetCfg m_optMetMAP2[2];
 MetCfg m_optMetMAPD[2];
 MetCfg m_optMetTmp2[2];
 MetCfg m_optMetFuelConsum[2];
 MetCfg m_optMetKnockRetard[2];
 MetCfg m_optMetKnockGraph[2];
 MetCfg m_optMetSensAFR[2];
 MetCfg m_optMetChokePos[2];
 MetCfg m_optMetGDPos[2];
 MetCfg m_optMetSynLoad[2];
 MetCfg m_optMetInjTimB[2];
 MetCfg m_optMetInjTimE[2];
 MetCfg m_optMetFuelConsumF[2];
 MetCfg m_optMetGrts[2];
 MetCfg m_optMetFtls[2];
 MetCfg m_optMetEgts[2];
 MetCfg m_optMetOps[2];
 MetCfg m_optMetInjDuty[2];
 MetCfg m_optMetMAF[2];
 MetCfg m_optMetVentDuty[2];
 MetCfg m_optMetMAPDot[2];
 MetCfg m_optMetFts[2];
 MetCfg m_optMetEGOCorr2[2];
 MetCfg m_optMetSensAFR2[2];
 MetCfg m_optMetTargAFR[2];
 MetCfg m_optMetDiffAFR[2];
 MetCfg m_optMetDiffAFR2[2];
 MetCfg m_optMetGPS[2];
};

struct MapPtMovStep
{
 float m_value[80];
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
 _TSTRING m_fldGpa4_i;
 _TSTRING m_fldInput1;
 _TSTRING m_fldInput2;
 _TSTRING m_fldAuto_i;
 _TSTRING m_fldMapsel0;
 _TSTRING m_fldRefprs_i;
 _TSTRING m_fldAltrn_i;
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
 _TSTRING m_fldEGOCorr2;
 _TSTRING m_fldAFR2;
 _TSTRING m_fldAFRMap;
 _TSTRING m_fldTchrg;
 _TSTRING m_fldGPS;
};

struct MetAverage
{
 int avrRPM;
 int avrVolt;
 int avrMAP;
 int avrAddI1;
 int avrTPS;
 int avrKnock;
 int avrIgnTim;
 int avrCLT;
 int avrAddI2;
 int avrInjPW;
 int avrIAT;
 int avrEGOCorr;
 int avrAirFlow;
 int avrVehicleSpeed;
 int avrTPSDot;
 int avrMAPDot;
 int avrMAP2;
 int avrGPS;
 int avrMAPD;
 int avrTmp2;
 int avrFuelConsum;
 int avrKnockRetard;
 int avrSensAFR;
 int avrChokePos;
 int avrGDPos;
 int avrSynLoad;
 int avrInjTimB;
 int avrInjTimE;
 int avrFuelConsumF;
 int avrGrts;
 int avrFtls;
 int avrEgts;
 int avrOps;
 int avrMAF;
 int avrFts;
 int avrTargAFR;
};

//Functionality
struct Functionality
{
 bool SM_CONTROL;
 bool GD_CONTROL;
};
