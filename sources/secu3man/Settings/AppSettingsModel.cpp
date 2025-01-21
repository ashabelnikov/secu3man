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

/** \file AppSettingsModel.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "AppSettingsModel.h"
#include "IniFileIO.h"
#include "common/MathHelpers.h"
#include "common/ModuleName.h"
#include "io-core/MapIds.h"

#include <limits>
#include <algorithm>

#undef max //avoid conflicts with C++

#define SECU3_CBR_250000 250000
#define SECU3_CBR_500000 500000

CAppSettingsModel::CAppSettingsModel()
: m_Name_Options_Section(_T("Options"))
, m_optBaudRateApplication(_T("Application_baud_rate"))
, m_optBaudRateBootloader(_T("Boot_loader_baud_rate"))
, m_optPortName(_T("COM_port"))
, m_optLogFilesFolder(_T("LogFilesFolder"))
, m_optUseAppFolder(_T("UseAppFolder"))
, m_optAlwaysWriteLog(_T("AlwaysWriteLog"))
, m_optCSVSepSymbol(_T("CSVSeparatingSymbol"))
, m_optMapCSVSepSymbol(_T("MapCSVSeparatingSymbol"))
, m_optMIDeskUpdatePeriod(_T("MI_Desk_UpdatePeriod"))
, m_optInterfaceLang(_T("InterfaceLanguage"))
, m_optECUPlatformType(_T("ECUPlatformType"))
, m_optUseDVFeatures(_T("UseDVFeatures"))
, m_optShowToolTips(_T("ShowToolTips"))
, m_optShowExFixtures(_T("ExFixtures"))
, m_optShowSpeedAndDist(_T("ShowSpeedAndDist"))
, m_optDVDeskUpdatePeriod(_T("DVDeskUpdatePeriod"))
, m_optCOMPortBother(_T("COMPortBother"))
, m_optUseHotKeys(_T("UseHotKeys"))
, m_optShowWelcome(_T("ShowWelcome"))
, m_optAllowVisualTheme(_T("AllowVisualTheme"))
, m_optAutoDiagEnter(_T("AutoDiagEnter"))
, m_optSaveWarning(_T("SaveWarning"))
, m_optAutoCERead(_T("AutoCERead"))
, m_optChildCharts(_T("ChildCharts"))
, m_optPortAutoReopen(_T("PortAutoReopen"))
, m_optToggleMapWnd(_T("ToggleMapWnd"))
, m_optEmbedMapWnd(_T("EmbedMapWnd"))
, m_optExistingPorts(_T("ExistingPorts"))
, m_optToolTipTime(_T("ToolTipTime"))
, m_optIniEdSyntax(_T("IniEditorSyntax"))
, m_optBldrEEPROMBlocks(_T("BldrEEPROMBlocks"))
, m_optFuelDensity1(_T("FuelDensity1"))
, m_optFuelDensity2(_T("FuelDensity2"))
, m_optLogBinaryFmt(_T("LogBinaryFmt"))
, m_optCreateWindows(_T("CreateWindows"))
, m_optDbgVarsToFile(_T("DbgVarsToFile"))
, m_optClassic2DKeys(_T("Classic2DKeys"))

//fixtures
, m_Name_Fixtures_Section("Fixtures")
, m_optSpeedUnit(_T("SpeedUnit"))
, m_optRPMAverage(_T("RPMAverage"))
, m_optVoltAverage(_T("VoltAverage"))
, m_optMAPAverage(_T("MAPAverage"))
, m_optAddI1Average(_T("AddI1Average"))
, m_optTPSAverage(_T("TPSAverage"))
, m_optKnockAverage(_T("KnockAverage"))
, m_optIgnTimAverage(_T("IgnTimAverage"))
, m_optCLTAverage(_T("CLTAverage"))
, m_optAddI2Average(_T("AddI2Average"))
, m_optInjPWAverage(_T("InjPWAverage"))
, m_optIATAverage(_T("IATAverage"))
, m_optEGOCorrAverage(_T("EGOCorrAverage"))
, m_optAirFlowAverage(_T("AirFlowAverage"))
, m_optVehicleSpeedAverage(_T("VehicleSpeedAverage"))
, m_optTPSDotAverage(_T("TPSDotAverage"))
, m_optMAPDotAverage(_T("MAPDotAverage"))
, m_optMAP2Average(_T("MAP2Average"))
, m_optGPSAverage(_T("GPSAverage"))
, m_optMAPDAverage(_T("MAPDAverage"))
, m_optTmp2Average(_T("Tmp2Average"))
, m_optFuelConsumAverage(_T("FuelConsumAverage"))
, m_optKnockRetardAverage(_T("KnockRetardAverage"))
, m_optSensAFRAverage(_T("SensAFRAverage"))
, m_optChokePosAverage(_T("ChokePosAverage"))
, m_optGDPosAverage(_T("GDPosAverage"))
, m_optSynLoadAverage(_T("SynLoadAverage"))
, m_optInjTimBAverage(_T("InjTimBAverage"))
, m_optInjTimEAverage(_T("InjTimEAverage"))
, m_optGraphShtPixels(_T("GraphShtPixels"))
, m_optGraphShowValue(_T("GraphShowValue"))
, m_optGraphValueHeight(_T("GraphValueHeight"))
, m_optFuelConsumFAverage(_T("FuelConsumFAverage"))
, m_optGrtsAverage(_T("GrtsAverage"))
, m_optFtlsAverage(_T("FtlsAverage"))
, m_optEgtsAverage(_T("EgtsAverage"))
, m_optOpsAverage(_T("OpsAverage"))
, m_optMAFAverage(_T("MAFAverage"))
, m_optFtsAverage(_T("FtsAverage"))
, m_optTargAFRAverage(_T("MapAFRAverage"))
, m_optFPSAverage(_T("FPSAverage"))

, m_optTitleFontSize(_T("TitleFontSize"))
, m_optValueFontSize(_T("ValueFontSize"))
, m_optPaneFontSize(_T("PaneFontSize"))
, m_optLabelFontSize(_T("LabelFontSize"))
, m_optMetersDragNDrop(_T("MetersDragNDrop"))
, m_optIndicatorsDragNDrop(_T("IndicatorsDragNDrop"))
, m_optFFFConst(_T("FFFConst"))
, m_optShowGraphCursor(_T("ShowGraphCursor"))

//positions of windows
, m_Name_WndSettings_Section(_T("WndSettings"))
, m_optMainFrmWnd(_T("MainFrmWnd"))
, m_optStrtMapWnd(_T("StrtMapWnd"))
, m_optIdleMapWnd(_T("IdleMapWnd"))
, m_optWorkMapWnd(_T("WorkMapWnd"))
, m_optTempMapWnd(_T("TempMapWnd"))
, m_optTempIdlMapWnd(_T("TempIdlMapWnd"))
, m_optGridMapIgnWnd(_T("GridMapIgnWnd"))
, m_optGridMapInjWnd(_T("GridMapInjWnd"))
, m_optVEMapWnd(_T("VEMapWnd"))
, m_optVE2MapWnd(_T("VE2MapWnd"))
, m_optVEIMapWnd(_T("VEIMapWnd"))
, m_optAFRMapWnd(_T("AFRMapWnd"))
, m_optCrnkMapWnd(_T("CrnkMapWnd"))
, m_optWrmpMapWnd(_T("WrmpMapWnd"))
, m_optDeadMapWnd(_T("DeadMapWnd"))
, m_optIdlrMapWnd(_T("IdlrMapWnd"))
, m_optIdlcMapWnd(_T("IdlcMapWnd"))
, m_optThrassMapWnd(_T("ThrassMapWnd"))
, m_optAETPSMapWnd(_T("AETPSMapWnd"))
, m_optAEMAPMapWnd(_T("AEMAPMapWnd"))
, m_optAERPMMapWnd(_T("AERPMMapWnd"))
, m_optAftstrMapWnd(_T("AftstrMapWnd"))
, m_optITMapWnd(_T("ITMapWnd"))
, m_optITRPMMapWnd(_T("ITRPMMapWnd"))
, m_optRigidMapWnd(_T("RigidMapWnd"))
, m_optEGOCrvMapWnd(_T("EGOCrvMapWnd"))
, m_optIACCMapWnd(_T("IACCMapWnd"))
, m_optIACCWMapWnd(_T("IACCWMapWnd"))
, m_optIATCLTMapWnd(_T("IATCLTCorrMapWnd"))
, m_optIACMATMapWnd(_T("IACMATCorrMapWnd"))
, m_optTpsswtMapWnd(_T("TpsSwtMapWnd"))
, m_optTpszonMapWnd(_T("TpsMapZonesMapWnd"))
, m_optGtscMapWnd(_T("GTSCMapWnd"))
, m_optGpscMapWnd(_T("GPSCMapWnd"))
, m_optAtscMapWnd(_T("AirDenMapWnd"))
, m_optPwm1MapWnd(_T("Pwm1MapWnd"))
, m_optPwm2MapWnd(_T("Pwm2MapWnd"))
, m_optCylMultMapWnd(_T("InjCylMultMapWnd"))
, m_optCylAddMapWnd(_T("InjCylAddMapWnd"))
//group of separate maps
, m_optAttenuatorMapWnd(_T("AttenMapWnd"))
, m_optDwellCntrlMapWnd(_T("DwellCntrlMapWnd"))
, m_optCTSCurveMapWnd(_T("CTSCurveMapWnd"))
, m_optATSCurvMapWnd(_T("ATSCurvMapWnd"))
, m_optATSCorrMapWnd(_T("ATSCorrMapWnd"))
, m_optGasdoseMapWnd(_T("GasdoseMapWnd"))
, m_optBarocorrMapWnd(_T("BarocorrMapWnd"))
, m_optManIgntimMapWnd(_T("ManIgntimMapWnd"))
, m_optCESettingsWnd(_T("CESettingsWnd"))
, m_optTmp2CurveMapWnd(_T("Tmp2CurveMapWnd"))
, m_optCrkTempMapWnd(_T("CrkTempMapWnd"))
, m_optEHPauseMapWnd(_T("EHPauseMapWnd"))
, m_optCrankingThrdMapWnd(_T("CrankingThrdMapWnd"))
, m_optCrankingTimeMapWnd(_T("CrankingTimeMapWnd"))
, m_optSmapabanThrdMapWnd(_T("SmapabanThrdMapWnd"))
, m_optKnockZoneMapWnd(_T("KnockZoneMapWnd"))
, m_optGrtsCurveMapWnd(_T("GrtsCurveMapWnd"))
, m_optGrHeatDutyMapWnd(_T("GrHeatDutyMapWnd"))
, m_optPwmIacUCoefMapWnd(_T("PwmIacUCoefMapWnd"))
, m_optAftstrStrk0MapWnd(_T("AftstrStrk0MapWnd"))
, m_optAftstrStrk1MapWnd(_T("AftstrStrk1MapWnd"))
, m_optGrValDelMapWnd(_T("GasValDelMapWnd"))
, m_optFtlsCurveMapWnd(_T("FtlsCurveMapWnd"))
, m_optEgtsCurveMapWnd(_T("EgtsCurveMapWnd"))
, m_optOpsCurveMapWnd(_T("OpsCurveMapWnd"))
, m_optManInjPwcMapWnd(_T("ManInjPwcMapWnd"))
, m_optMAFCurveMapWnd(_T("ManMAFCurveMapWnd"))
, m_optFtlsCorMapWnd(_T("FtlsCorrMapWnd"))
, m_optLambdaZoneMapWnd(_T("LambdaZoneMapWnd"))
, m_optFtsCurveMapWnd(_T("FtsCurveMapWnd"))
, m_optFuelDensCorrMapWnd(_T("FuelDensCorrMapWnd"))
, m_optXtauXfAccMapWnd(_T("XtauXfAccMapWnd"))
, m_optXtauXfDecMapWnd(_T("XtauXfDecMapWnd"))
, m_optXtauTfAccMapWnd(_T("XtauTfAccMapWnd"))
, m_optXtauTfDecMapWnd(_T("XtauTfDecMapWnd"))
, m_optInjNonLinPMapWnd(_T("InjNonLinPMapWnd"))
, m_optInjNonLinGMapWnd(_T("InjNonLinGMapWnd"))
, m_optEGODelayMapWnd(_T("EGODelayMapWnd"))
, m_optWUAFR0MapWnd(_T("WUAFR0MapWnd"))
, m_optWUAFR1MapWnd(_T("WUAFR1MapWnd"))
//positions of windows (online tables)
, m_Name_WndSettings_Section1(_T("WndSettingsOnline"))
, m_optStrtMapWnd1(_T("StrtMapWnd"))
, m_optIdleMapWnd1(_T("IdleMapWnd"))
, m_optWorkMapWnd1(_T("WorkMapWnd"))
, m_optTempMapWnd1(_T("TempMapWnd"))
, m_optTempIdlMapWnd1(_T("TempIdlMapWnd"))
, m_optGridMapIgnWnd1(_T("GridMapIgnWnd"))
, m_optGridMapInjWnd1(_T("GridMapInjWnd"))
, m_optVEMapWnd1(_T("VEMapWnd"))
, m_optVE2MapWnd1(_T("VE2MapWnd"))
, m_optVEIMapWnd1(_T("VEIMapWnd"))
, m_optAFRMapWnd1(_T("AFRMapWnd"))
, m_optCrnkMapWnd1(_T("CrnkMapWnd"))
, m_optWrmpMapWnd1(_T("WrmpMapWnd"))
, m_optDeadMapWnd1(_T("DeadMapWnd"))
, m_optIdlrMapWnd1(_T("IdlrMapWnd"))
, m_optIdlcMapWnd1(_T("IdlcMapWnd"))
, m_optThrassMapWnd1(_T("ThrassMapWnd"))
, m_optAETPSMapWnd1(_T("AETPSMapWnd"))
, m_optAEMAPMapWnd1(_T("AEMAPMapWnd"))
, m_optAERPMMapWnd1(_T("AERPMMapWnd"))
, m_optAftstrMapWnd1(_T("AftstrMapWnd"))
, m_optITMapWnd1(_T("ITMapWnd"))
, m_optITRPMMapWnd1(_T("ITRPMMapWnd"))
, m_optRigidMapWnd1(_T("RigidMapWnd"))
, m_optEGOCrvMapWnd1(_T("EGOCrvMapWnd"))
, m_optIACCMapWnd1(_T("IACCMapWnd"))
, m_optIACCWMapWnd1(_T("IACCWMapWnd"))
, m_optIATCLTMapWnd1(_T("IATCLTCorrMapWnd"))
, m_optTpsswtMapWnd1(_T("TpsSwtMapWnd"))
, m_optGtscMapWnd1(_T("GTSCMapWnd"))
, m_optGpscMapWnd1(_T("GPSCMapWnd"))
, m_optAtscMapWnd1(_T("AirDenMapWnd"))
, m_optPwm1MapWnd1(_T("Pwm1MapWnd"))
, m_optPwm2MapWnd1(_T("Pwm2MapWnd"))
, m_optIACMATMapWnd1(_T("IACMATCorrMapWnd"))
, m_optTpszonMapWnd1(_T("TpsMapZonesMapWnd"))
, m_optCylMultMapWnd1(_T("InjCylMultMapWnd"))
, m_optCylAddMapWnd1(_T("InjCylAddMapWnd"))
//group of separate maps
, m_optAttenuatorMapWnd1(_T("AttenMapWnd"))
, m_optDwellCntrlMapWnd1(_T("DwellCntrlMapWnd"))
, m_optCTSCurveMapWnd1(_T("CTSCurveMapWnd"))
, m_optATSCurvMapWnd1(_T("ATSCurvMapWnd"))
, m_optATSCorrMapWnd1(_T("ATSCorrMapWnd"))
, m_optGasdoseMapWnd1(_T("GasdoseMapWnd"))
, m_optBarocorrMapWnd1(_T("BarocorrMapWnd"))
, m_optManIgntimMapWnd1(_T("ManIgntimMapWnd"))
, m_optCESettingsWnd1(_T("CESettingsWnd"))
, m_optTmp2CurveMapWnd1(_T("Tmp2CurveMapWnd"))
, m_optCrkTempMapWnd1(_T("CrkTempMapWnd"))
, m_optEHPauseMapWnd1(_T("EHPauseMapWnd"))
, m_optCrankingThrdMapWnd1(_T("CrankingThrdMapWnd"))
, m_optCrankingTimeMapWnd1(_T("CrankingTimeMapWnd"))
, m_optSmapabanThrdMapWnd1(_T("SmapabanThrdMapWnd"))
, m_optKnockZoneMapWnd1(_T("KnockZoneMapWnd"))
, m_optGrtsCurveMapWnd1(_T("GrtsCurveMapWnd"))
, m_optGrHeatDutyMapWnd1(_T("GrHeatDutyMapWnd"))
, m_optPwmIacUCoefMapWnd1(_T("PwmIacUCoefMapWnd"))
, m_optAftstrStrk0MapWnd1(_T("AftstrStrk0MapWnd"))
, m_optAftstrStrk1MapWnd1(_T("AftstrStrk1MapWnd"))
, m_optGrValDelMapWnd1(_T("GasValDelMapWnd"))
, m_optFtlsCurveMapWnd1(_T("FtlsCurveMapWnd"))
, m_optEgtsCurveMapWnd1(_T("EgtsCurveMapWnd"))
, m_optOpsCurveMapWnd1(_T("OpsCurveMapWnd"))
, m_optManInjPwcMapWnd1(_T("ManInjPwcMapWnd"))
, m_optMAFCurveMapWnd1(_T("ManMAFCurveMapWnd"))
, m_optFtlsCorMapWnd1(_T("FtlsCorrMapWnd"))
, m_optLambdaZoneMapWnd1(_T("LambdaZoneMapWnd"))
, m_optFtsCurveMapWnd1(_T("FtsCurveMapWnd"))
, m_optFuelDensCorrMapWnd1(_T("FuelDensCorrMapWnd"))
, m_optXtauXfAccMapWnd1(_T("XtauXfAccMapWnd"))
, m_optXtauXfDecMapWnd1(_T("XtauXfDecMapWnd"))
, m_optXtauTfAccMapWnd1(_T("XtauTfAccMapWnd"))
, m_optXtauTfDecMapWnd1(_T("XtauTfDecMapWnd"))
, m_optInjNonLinPMapWnd1(_T("InjNonLinPMapWnd"))
, m_optInjNonLinGMapWnd1(_T("InjNonLinGMapWnd"))
, m_optEGODelayMapWnd1(_T("EGODelayMapWnd"))
, m_optWUAFR0MapWnd1(_T("WUAFR0MapWnd"))
, m_optWUAFR1MapWnd1(_T("WUAFR1MapWnd"))
//size of windows
, m_Name_WndSize_Section(_T("WndSize"))
, m_optMainFrmWndSize(_T("MainFrmWnd"))
, m_optStrtMapWndSize(_T("StrtMapWnd"))
, m_optIdleMapWndSize(_T("IdleMapWnd"))
, m_optWorkMapWndSize(_T("WorkMapWnd"))
, m_optTempMapWndSize(_T("TempMapWnd"))
, m_optTempIdlMapWndSize(_T("TempIdlMapWnd"))
, m_optGridMapIgnWndSize(_T("GridMapIgnWnd"))
, m_optGridMapInjWndSize(_T("GridMapInjWnd"))
, m_optVEMapWndSize(_T("VEMapWnd"))
, m_optVE2MapWndSize(_T("VE2MapWnd"))
, m_optVEIMapWndSize(_T("VEIMapWnd"))
, m_optAFRMapWndSize(_T("AFRMapWnd"))
, m_optCrnkMapWndSize(_T("CrnkMapWnd"))
, m_optWrmpMapWndSize(_T("WrmpMapWnd"))
, m_optDeadMapWndSize(_T("DeadMapWnd"))
, m_optIdlrMapWndSize(_T("IdlrMapWnd"))
, m_optIdlcMapWndSize(_T("IdlcMapWnd"))
, m_optThrassMapWndSize(_T("ThrassMapWnd"))
, m_optAETPSMapWndSize(_T("AETPSMapWnd"))
, m_optAEMAPMapWndSize(_T("AEMAPMapWnd"))
, m_optAERPMMapWndSize(_T("AERPMMapWnd"))
, m_optAftstrMapWndSize(_T("AftstrMapWnd"))
, m_optITMapWndSize(_T("ITMapWnd"))
, m_optITRPMMapWndSize(_T("ITRPMMapWnd"))
, m_optRigidMapWndSize(_T("RigidMapWnd"))
, m_optEGOCrvMapWndSize(_T("EGOCrvMapWnd"))
, m_optIACCMapWndSize(_T("IACCMapWnd"))
, m_optIACCWMapWndSize(_T("IACCWMapWnd"))
, m_optIATCLTMapWndSize(_T("IATCLTCorrMapWnd"))
, m_optTpsswtMapWndSize(_T("TpsSwtMapWnd"))
, m_optTpszonMapWndSize(_T("TpsMapZonesMapWnd"))
, m_optGtscMapWndSize(_T("GTSCMapWnd"))
, m_optGpscMapWndSize(_T("GPSCMapWnd"))
, m_optAtscMapWndSize(_T("AirDenMapWnd"))
, m_optPwm1MapWndSize(_T("Pwm1MapWnd"))
, m_optPwm2MapWndSize(_T("Pwm2MapWnd"))
, m_optIACMATMapWndSize(_T("IACMATCorrMapWnd"))
, m_optCylMultMapWndSize(_T("InjCylMultMapWnd"))
, m_optCylAddMapWndSize(_T("InjCylAddMapWnd"))
//group of separate maps
, m_optAttenuatorMapWndSize(_T("AttenMapWnd"))
, m_optDwellCntrlMapWndSize(_T("DwellCntrlMapWnd"))
, m_optCTSCurveMapWndSize(_T("CTSCurveMapWnd"))
, m_optATSCurvMapWndSize(_T("ATSCurvMapWnd"))
, m_optATSCorrMapWndSize(_T("ATSCorrMapWnd"))
, m_optGasdoseMapWndSize(_T("GasdoseMapWnd"))
, m_optBarocorrMapWndSize(_T("BarocorrMapWnd"))
, m_optManIgntimMapWndSize(_T("ManIgntimMapWnd"))
, m_optCESettingsWndSize(_T("CESettingsWnd"))
, m_optTmp2CurveMapWndSize(_T("Tmp2CurveMapWnd"))
, m_optCrkTempMapWndSize(_T("CrkTempMapWnd"))
, m_optEHPauseMapWndSize(_T("EHPauseMapWnd"))
, m_optCrankingThrdMapWndSize(_T("CrankingThrdMapWnd"))
, m_optCrankingTimeMapWndSize(_T("CrankingTimeMapWnd"))
, m_optSmapabanThrdMapWndSize(_T("SmapabanThrdMapWnd"))
, m_optKnockZoneMapWndSize(_T("KnockZoneMapWnd"))
, m_optGrtsCurveMapWndSize(_T("GrtsCurveMapWnd"))
, m_optGrHeatDutyMapWndSize(_T("GrHeatDutyMapWnd"))
, m_optPwmIacUCoefMapWndSize(_T("PwmIacUCoefMapWnd"))
, m_optAftstrStrk0MapWndSize(_T("AftstrStrk0MapWnd"))
, m_optAftstrStrk1MapWndSize(_T("AftstrStrk1MapWnd"))
, m_optGrValDelMapWndSize(_T("GasValDelMapWnd"))
, m_optFtlsCurveMapWndSize(_T("FtlsCurveMapWnd"))
, m_optEgtsCurveMapWndSize(_T("EgtsCurveMapWnd"))
, m_optOpsCurveMapWndSize(_T("OpsCurveMapWnd"))
, m_optManInjPwcMapWndSize(_T("ManInjPwcMapWnd"))
, m_optMAFCurveMapWndSize(_T("MAFCurveMapWnd"))
, m_optFtlsCorMapWndSize(_T("FtlsCorrMapWnd"))
, m_optLambdaZoneMapWndSize(_T("LambdaZoneMapWnd"))
, m_optFtsCurveMapWndSize(_T("FtsCurveMapWnd"))
, m_optFuelDensCorrMapWndSize(_T("FuelDensCorrMapWnd"))
, m_optXtauXfAccMapWndSize(_T("XtauXfAccMapWnd"))
, m_optXtauXfDecMapWndSize(_T("XtauXfDecMapWnd"))
, m_optXtauTfAccMapWndSize(_T("XtauTfAccMapWnd"))
, m_optXtauTfDecMapWndSize(_T("XtauTfDecMapWnd"))
, m_optInjNonLinPMapWndSize(_T("InjNonLinPMapWnd"))
, m_optInjNonLinGMapWndSize(_T("InjNonLinGMapWnd"))
, m_optEGODelayMapWndSize(_T("EGODelayMapWnd"))
, m_optWUAFR0MapWndSize(_T("WUAFR0MapWnd"))
, m_optWUAFR1MapWndSize(_T("WUAFR1MapWnd"))
//sizes of windows (online tables)
, m_Name_WndSize_Section1(_T("WndSizeOnline"))
, m_optStrtMapWndSize1(_T("StrtMapWnd"))
, m_optIdleMapWndSize1(_T("IdleMapWnd"))
, m_optWorkMapWndSize1(_T("WorkMapWnd"))
, m_optTempMapWndSize1(_T("TempMapWnd"))
, m_optTempIdlMapWndSize1(_T("TempIdlMapWnd"))
, m_optGridMapIgnWndSize1(_T("GridMapIgnWnd"))
, m_optGridMapInjWndSize1(_T("GridMapInjWnd"))
, m_optVEMapWndSize1(_T("VEMapWnd"))
, m_optVE2MapWndSize1(_T("VE2MapWnd"))
, m_optVEIMapWndSize1(_T("VEIMapWnd"))
, m_optAFRMapWndSize1(_T("AFRMapWnd"))
, m_optCrnkMapWndSize1(_T("CrnkMapWnd"))
, m_optWrmpMapWndSize1(_T("WrmpMapWnd"))
, m_optDeadMapWndSize1(_T("DeadMapWnd"))
, m_optIdlrMapWndSize1(_T("IdlrMapWnd"))
, m_optIdlcMapWndSize1(_T("IdlcMapWnd"))
, m_optThrassMapWndSize1(_T("ThrassMapWnd"))
, m_optAETPSMapWndSize1(_T("AETPSMapWnd"))
, m_optAEMAPMapWndSize1(_T("AEMAPMapWnd"))
, m_optAERPMMapWndSize1(_T("AERPMMapWnd"))
, m_optAftstrMapWndSize1(_T("AftstrMapWnd"))
, m_optITMapWndSize1(_T("ITMapWnd"))
, m_optITRPMMapWndSize1(_T("ITRPMMapWnd"))
, m_optRigidMapWndSize1(_T("RigidMapWnd"))
, m_optEGOCrvMapWndSize1(_T("EGOCrvMapWnd"))
, m_optIACCMapWndSize1(_T("IACCMapWnd"))
, m_optIACCWMapWndSize1(_T("IACCWMapWnd"))
, m_optIATCLTMapWndSize1(_T("IATCLTCorrMapWnd"))
, m_optTpsswtMapWndSize1(_T("TpsSwtMapWnd"))
, m_optGtscMapWndSize1(_T("GTSCMapWnd"))
, m_optGpscMapWndSize1(_T("GPSCMapWnd"))
, m_optAtscMapWndSize1(_T("AirDenMapWnd"))
, m_optPwm1MapWndSize1(_T("Pwm1MapWnd"))
, m_optPwm2MapWndSize1(_T("Pwm2MapWnd"))
, m_optIACMATMapWndSize1(_T("IACMATCorrMapWnd"))
, m_optTpszonMapWndSize1(_T("TpsMapZonesMapWnd"))
, m_optCylMultMapWndSize1(_T("InjCylMultMapWnd"))
, m_optCylAddMapWndSize1(_T("InjCylAddMapWnd"))
//group of separate maps
, m_optAttenuatorMapWndSize1(_T("AttenMapWnd"))
, m_optDwellCntrlMapWndSize1(_T("DwellCntrlMapWnd"))
, m_optCTSCurveMapWndSize1(_T("CTSCurveMapWnd"))
, m_optATSCurvMapWndSize1(_T("ATSCurvMapWnd"))
, m_optATSCorrMapWndSize1(_T("ATSCorrMapWnd"))
, m_optGasdoseMapWndSize1(_T("GasdoseMapWnd"))
, m_optBarocorrMapWndSize1(_T("BarocorrMapWnd"))
, m_optManIgntimMapWndSize1(_T("ManIgntimMapWnd"))
, m_optCESettingsWndSize1(_T("CESettingsWnd"))
, m_optTmp2CurveMapWndSize1(_T("Tmp2CurveMapWnd"))
, m_optCrkTempMapWndSize1(_T("CrkTempMapWnd"))
, m_optEHPauseMapWndSize1(_T("EHPauseMapWnd"))
, m_optCrankingThrdMapWndSize1(_T("CrankingThrdMapWnd"))
, m_optCrankingTimeMapWndSize1(_T("CrankingTimeMapWnd"))
, m_optSmapabanThrdMapWndSize1(_T("SmapabanThrdMapWnd"))
, m_optKnockZoneMapWndSize1(_T("KnockZoneMapWnd"))
, m_optGrtsCurveMapWndSize1(_T("GrtsCurveMapWnd"))
, m_optGrHeatDutyMapWndSize1(_T("GrHeatDutyMapWnd"))
, m_optPwmIacUCoefMapWndSize1(_T("PwmIacUCoefMapWnd"))
, m_optAftstrStrk0MapWndSize1(_T("AftstrStrk0MapWnd"))
, m_optAftstrStrk1MapWndSize1(_T("AftstrStrk1MapWnd"))
, m_optGrValDelMapWndSize1(_T("GasValDelMapWnd"))
, m_optFtlsCurveMapWndSize1(_T("FtlsCurveMapWnd"))
, m_optEgtsCurveMapWndSize1(_T("EgtsCurveMapWnd"))
, m_optOpsCurveMapWndSize1(_T("OpsCurveMapWnd"))
, m_optManInjPwcMapWndSize1(_T("ManInjPwcMapWnd"))
, m_optMAFCurveMapWndSize1(_T("MAFCurveMapWnd"))
, m_optFtlsCorMapWndSize1(_T("FtlsCorrMapWnd"))
, m_optLambdaZoneMapWndSize1(_T("LambdaZoneMapWnd"))
, m_optFtsCurveMapWndSize1(_T("FtsCurveMapWnd"))
, m_optFuelDensCorrMapWndSize1(_T("FuelDensCorrMapWnd"))
, m_optXtauXfAccMapWndSize1(_T("XtauXfAccMapWnd"))
, m_optXtauXfDecMapWndSize1(_T("XtauXfDecMapWnd"))
, m_optXtauTfAccMapWndSize1(_T("XtauTfAccMapWnd"))
, m_optXtauTfDecMapWndSize1(_T("XtauTfDecMapWnd"))
, m_optInjNonLinPMapWndSize1(_T("InjNonLinPMapWnd"))
, m_optInjNonLinGMapWndSize1(_T("InjNonLinGMapWnd"))
, m_optEGODelayMapWndSize1(_T("EGODelayMapWnd"))
, m_optWUAFR0MapWndSize1(_T("WUAFR0MapWnd"))
, m_optWUAFR1MapWndSize1(_T("WUAFR1MapWnd"))
//state of windows (minimized, normal, maximized)
, m_Name_WndState_Section(_T("WndState"))
, m_optMainFrmWndState(_T("MainFrmWnd"))
//Colors of indicators
, m_Name_IndColors_Section(_T("IndColors"))
, m_optColGas_v(_T("IndGas_v"))
, m_optColCarb(_T("IndThrottle"))
, m_optColIdleValve(_T("IndIdleCutoff"))
, m_optColPowerValve(_T("IndPowerValve"))
, m_optColStBlock(_T("IndStBlock"))
, m_optColAE(_T("IndAE"))
, m_optColCoolingFan(_T("IndCoolingFan"))
, m_optColCE(_T("IndCE"))
, m_optColFCRevLim(_T("IndFCRevLim"))
, m_optColFloodClear(_T("IndFloodClear"))
, m_optColSysLocked(_T("IndSysLocked"))
, m_optColIgn_i(_T("IndIgn_i"))
, m_optColCond_i(_T("IndCond_i"))
, m_optColEpas_i(_T("IndEpas_i"))
, m_optColAftStrEnr(_T("IndAftStrEnr"))
, m_optColIacClLoop(_T("IndIacClLoop"))
, m_optColUniOut1(_T("IndUniOut1"))
, m_optColUniOut2(_T("IndUniOut2"))
, m_optColUniOut3(_T("IndUniOut3"))
, m_optColUniOut4(_T("IndUniOut4"))
, m_optColUniOut5(_T("IndUniOut5"))
, m_optColUniOut6(_T("IndUniOut6"))
, m_optColGpa4_i(_T("IndGpa4_i"))
, m_optColInput1(_T("IndInput1"))
, m_optColInput2(_T("IndInput2"))
, m_optColAuto_i(_T("IndAuto_i"))
, m_optColMapsel0(_T("Mapsel0"))
, m_optColRefprs_i(_T("Refprs_i"))
, m_optColAltrn_i(_T("Altrn_i"))
//Autotune
, m_Name_AutoTune_Section(_T("AutoTune"))
, m_optLambdaDelay(_T("LambdaDelay"))
, m_optLambdaDelayL(_T("LambdaDelayL"))
, m_optLambdaDelayR(_T("LambdaDelayR"))
, m_optAFRError(_T("AFRError"))
, m_optBlockedCells(_T("BlockedCells"))
, m_optStatSize(_T("StatSize"))
, m_optAutoBlockThrd(_T("AutoBlockThrd"))
, m_optGrowingMode(_T("GrowingMode"))
, m_optMinAFR(_T("MinAFR"))
, m_optMaxAFR(_T("MaxAFR"))
, m_optMinDistThrd(_T("MinDistThrd"))
, m_optMinTPS(_T("MinTPS"))
, m_optMaxTPS(_T("MaxTPS"))
, m_optCLTThrd(_T("CLTThrd"))
, m_optTunSoftness(_T("TunSoftness"))
//Map editor
, m_Name_MapEditor_Section(_T("MapEditor"))
, m_optGradSaturation(_T("GradSaturation"))
, m_optGradBrightness(_T("GradBrightness"))
, m_optBoldFont(_T("BoldFont"))
, m_optITEdMode(_T("InjTimEdMode"))
, m_optSpotMarkers(_T("SpotMarkers"))
, m_optSpotMarkersSize(_T("SpotMarkersSize"))
, m_optActiveVEMap(_T("ActiveVEMap"))
, m_optUseMarkers(_T("UseMarkers"))
//Splitters
, m_Name_Splitters_Section(_T("Splitters"))
, m_optParamMonVert(_T("ParamMonVert"))
, m_optLogPlayerVert(_T("LogPlayerVert"))
//Inj.driver
, m_Name_InjDrv_Section(_T("InjDriver"))
, m_optInjDrvTabActive(_T("InjDrvTabActive"))
, m_optVoltLineColor(_T("VoltLineColor"))
, m_optPeakOnPtMovStep(_T("PeakOnPtMovStep"))
, m_optPeakDutyPtMovStep(_T("PeakDutyPtMovStep"))
, m_optHoldDutyPtMovStep(_T("HoldDutyPtMovStep"))
, m_optPeakFullPtMovStep(_T("PeakFullPtMovStep"))
, m_optPthPausePtMovStep(_T("PthPausePtMovStep"))
, m_optShowGraphLabels(_T("ShowGraphLabels"))
, m_optPWMulPtMovStep(_T("PWMulPtMovStep"))
, m_optPWAddPtMovStep(_T("PWAddPtMovStep"))
//Points' moving step
, m_Name_MapPtMovStep_Section(_T("MapPtMovStep"))
, m_optPtMovStepWorkMap(_T("WorkMapWnd"))
, m_optPtMovStepTempMap(_T("TempMapWnd"))
, m_optPtMovStepTempIdlMap(_T("TempIdlMapWnd"))
, m_optPtMovStepStartMap(_T("StrtMapWnd"))
, m_optPtMovStepIdleMap(_T("IdleMapWnd"))
, m_optPtMovStepVeMap(_T("VEMapWnd"))
, m_optPtMovStepVe2Map(_T("VE2MapWnd"))
, m_optPtMovStepVeiMap(_T("VEIMapWnd"))
, m_optPtMovStepAfrMap(_T("AFRMapWnd"))
, m_optPtMovStepCrnkMap(_T("CrnkMapWnd"))
, m_optPtMovStepWrmpMap(_T("WrmpMapWnd"))
, m_optPtMovStepDeadMap(_T("DeadMapWnd"))
, m_optPtMovStepIdlrMap(_T("IdlrMapWnd"))
, m_optPtMovStepIdlcMap(_T("IdlcMapWnd"))
, m_optPtMovStepThrassMap(_T("ThrassMapWnd"))
, m_optPtMovStepAetpsMap(_T("AETPSMapWnd"))
, m_optPtMovStepAemapMap(_T("AEMAPMapWnd"))
, m_optPtMovStepAerpmMap(_T("AERPMMapWnd"))
, m_optPtMovStepAftstrMap(_T("AftstrMapWnd"))
, m_optPtMovStepItMap(_T("ITMapWnd"))
, m_optPtMovStepItrpmMap(_T("ITRPMMapWnd"))
, m_optPtMovStepRigidMap(_T("RigidMapWnd"))
, m_optPtMovStepEgocrvMap(_T("EGOCrvMapWnd"))
, m_optPtMovStepIaccMap(_T("IACCMapWnd"))
, m_optPtMovStepIaccwMap(_T("IACCWMapWnd"))
, m_optPtMovStepIatcltMap(_T("IATCLTCorrMapWnd"))
, m_optPtMovStepTpsswtMap(_T("TpsSwtMapWnd"))
, m_optPtMovStepGtscMap(_T("GTSCMapWnd"))
, m_optPtMovStepGpscMap(_T("GPSCMapWnd"))
, m_optPtMovStepAtscMap(_T("AirDenMapWnd"))
, m_optPtMovStepPwm1Map(_T("Pwm1MapWnd"))
, m_optPtMovStepPwm2Map(_T("Pwm2MapWnd"))
, m_optPtMovStepIACMATMap(_T("IACMATCorrMapWnd"))
, m_optPtMovStepTpszonMap(_T("TpsMapZonesMapWnd"))
, m_optPtMovStepCylMultMap(_T("InjCylMultMapWnd"))
, m_optPtMovStepCylAddMap(_T("InjCylAddMapWnd"))
//separate
, m_optPtMovStepCrkTempMap(_T("CrkTempMapWnd"))
, m_optPtMovStepEHPauseMap(_T("EHPauseMapWnd"))
, m_optPtMovStepAttenuatorMap(_T("AttenMapWnd"))
, m_optPtMovStepDwellCntrlMap(_T("DwellCntrlMapWnd"))
, m_optPtMovStepCTSCurveMap(_T("CTSCurveMapWnd"))
, m_optPtMovStepBarocorrMap(_T("BarocorrMapWnd"))
, m_optPtMovStepManIgntimMap(_T("ManIgntimMapWnd"))
, m_optPtMovStepATSCurvMap(_T("ATSCurvMapWnd"))
, m_optPtMovStepATSCorrMap(_T("ATSCorrMapWnd"))
, m_optPtMovStepGasdoseMap(_T("GasdoseMapWnd"))
, m_optPtMovStepTmp2CurveMap(_T("Tmp2CurveMapWnd"))
, m_optPtMovStepCrankingThrdMap(_T("CrankingThrdMapWnd"))
, m_optPtMovStepCrankingTimeMap(_T("CrankingTimeMapWnd"))
, m_optPtMovStepSmapabanThrdMap(_T("SmapabanThrdMapWnd"))
, m_optPtMovStepKnockZoneMap(_T("KnockZoneMapWnd"))
, m_optPtMovStepGrtsCurveMap(_T("GrtsCurveMapWnd"))
, m_optPtMovStepGrHeatDutyMap(_T("GrHeatDutyMapWnd"))
, m_optPtMovStepPwmIacUCoefMap(_T("PwmIacUCoefMapWnd"))
, m_optPtMovStepAftstrStrk0Map(_T("AftstrStrk0MapWnd"))
, m_optPtMovStepAftstrStrk1Map(_T("AftstrStrk1MapWnd"))
, m_optPtMovStepGrValDelMap(_T("GasValDelMapWnd"))
, m_optPtMovStepFtlsCurveMap(_T("FtlsCurveMapWnd"))
, m_optPtMovStepEgtsCurveMap(_T("EgtsCurveMapWnd"))
, m_optPtMovStepOpsCurveMap(_T("OpsCurveMapWnd"))
, m_optPtMovStepManInjPwcMap(_T("ManInjPwcMapWnd"))
, m_optPtMovStepMAFCurveMap(_T("MAFCurveMapWnd"))
, m_optPtMovStepFtlsCorMap(_T("FtlsCorrMapWnd"))
, m_optPtMovStepLambdaZoneMap(_T("LambdaZoneMapWnd"))
, m_optPtMovStepFtsCurveMap(_T("FtsCurveMapWnd"))
, m_optPtMovStepFuelDensCorrMap(_T("FuelDensCorrMapWnd"))
, m_optPtMovStepXtauXfAccMap(_T("XtauXfAccMapWnd"))
, m_optPtMovStepXtauXfDecMap(_T("XtauXfDecMapWnd"))
, m_optPtMovStepXtauTfAccMap(_T("XtauTfAccMapWnd"))
, m_optPtMovStepXtauTfDecMap(_T("XtauTfDecMapWnd"))
, m_optPtMovStepInjNonLinPMap(_T("InjNonLinPMapWnd"))
, m_optPtMovStepInjNonLinGMap(_T("InjNonLinGMapWnd"))
, m_optPtMovStepEGODelayMap(_T("EGODelayMapWnd"))
, m_optPtMovStepWUAFR0Map(_T("WUAFR0MapWnd"))
, m_optPtMovStepWUAFR1Map(_T("WUAFR1MapWnd"))
//Log file's fileds
, m_Name_LogFileFields_Section(_T("LogFileFields"))
, m_optWriteLogFields(_T("WriteFields"))
, m_optLogFieldTime(_T("Time"))
, m_optLogFieldRPM(_T("RPM"))
, m_optLogFieldIgnTim(_T("IgnTim"))
, m_optLogFieldMAP(_T("Map"))
, m_optLogFieldVBat(_T("VBat"))
, m_optLogFieldCLT(_T("CLT"))
, m_optLogFieldKnock(_T("Knock"))
, m_optLogFieldKnockCorr(_T("KnockCorr"))
, m_optLogFieldLoadCurve(_T("LoadCurve"))
, m_optLogFieldCarb(_T("CarbSw"))
, m_optLogFieldGas_v(_T("Gas_V"))
, m_optLogFieldIdleValve(_T("IdleValve"))
, m_optLogFieldPowerValve(_T("PowerValve"))
, m_optLogFieldCoolingFan(_T("CoolingFan"))
, m_optLogFieldStBlock(_T("StBlock"))
, m_optLogFieldAE(_T("AE"))
, m_optLogFieldFCRevLim(_T("FCRevLim"))
, m_optLogFieldFloodClear(_T("FloodClear"))
, m_optLogFieldSysLocked(_T("SysLocked"))
, m_optLogFieldCE(_T("CE"))
, m_optLogFieldIgn_i(_T("Ign_i"))
, m_optLogFieldCond_i(_T("Cond_i"))
, m_optLogFieldEpas_i(_T("Epas_I"))
, m_optLogFieldGpa4_i(_T("Gpa4_i"))
, m_optLogFieldInput1(_T("Input1"))
, m_optLogFieldInput2(_T("Input2"))
, m_optLogFieldAuto_i(_T("Auto_i"))
, m_optLogFieldMapsel0(_T("Mapsel0"))
, m_optLogFieldRefprs_i(_T("Refprs_i"))
, m_optLogFieldAltrn_i(_T("Altrn_i"))
, m_optLogFieldTPS(_T("TPS"))
, m_optLogFieldAdd_i1(_T("Add_i1"))
, m_optLogFieldAdd_i2(_T("Add_i2"))
, m_optLogFieldChokePos(_T("ChokePos"))
, m_optLogFieldGDPos(_T("GDPos"))
, m_optLogFieldVehSpeed(_T("VehSpeed"))
, m_optLogFieldPasDist(_T("PasDist"))
, m_optLogFieldFuelConsum(_T("FuelConsum"))
, m_optLogFieldFuelConsFreq(_T("FuelConsumF"))
, m_optLogFieldIAT(_T("IAT"))
, m_optLogFieldStrtIgnTim(_T("StrtIgnTim"))
, m_optLogFieldIdleIgnTim(_T("IdleIgnTim"))
, m_optLogFieldWorkIgnTim(_T("WorkIgnTim"))
, m_optLogFieldTempIgnTim(_T("TempIgnTim"))
, m_optLogFieldIATIgnTim(_T("IATIgnTim"))
, m_optLogFieldIdlRegIgnTim(_T("IdlRegIgnTim"))
, m_optLogFieldOctanCorr(_T("IgnTimCorr"))
, m_optLogFieldEGOCorr(_T("EGOcorr"))
, m_optLogFieldInjPW(_T("InjPW"))
, m_optLogFieldTPSdot(_T("TPSdot"))
, m_optLogFieldMAPdot(_T("MAPdot"))
, m_optLogFieldMAP2(_T("MAP2"))
, m_optLogFieldGPS(_T("GPS"))
, m_optLogFieldFPS(_T("FPS"))
, m_optLogFieldTmp2(_T("Tmp2"))
, m_optLogFieldDiffMAP(_T("DiffMAP"))
, m_optLogFieldAFR(_T("AFR"))
, m_optLogFieldSynLoad(_T("SynLoad"))
, m_optLogFieldBaroPress(_T("BaroPress"))
, m_optLogFieldInjTimBeg(_T("InjTimBeg"))
, m_optLogFieldInjTimEnd(_T("InjTimEnd"))
, m_optLogFieldLogMarks(_T("LogMarks"))
, m_optLogFieldCECodes(_T("CECodes"))
, m_optLogFieldGrts(_T("GRTS"))
, m_optLogFieldFtls(_T("FTLS"))
, m_optLogFieldEgts(_T("EGTS"))
, m_optLogFieldOps(_T("OPS"))
, m_optLogFieldAftStrEnr(_T("AftStrEnr"))
, m_optLogFieldIacClLoop(_T("IacClLoop"))
, m_optLogFieldInjDuty(_T("InjDuty"))
, m_optLogFieldRigidArg(_T("RigidArg"))
, m_optLogFieldServFlag(_T("ServFlag"))
, m_optLogFieldRxlaf(_T("Rxlaf"))
, m_optLogFieldMAF(_T("MAF"))
, m_optLogFieldVentDuty(_T("VentDuty"))
, m_optLogFieldUniOuts(_T("UnivOuts"))
, m_optLogFieldFts(_T("FTS"))
, m_optLogFieldConsFuel(_T("FuelConsumed"))
, m_optLogFieldEGOCorr2(_T("EGOcorr2"))
, m_optLogFieldAFR2(_T("AFR2"))
, m_optLogFieldAFRMap(_T("AFRMap"))
, m_optLogFieldTchrg(_T("Tchrg"))
//Functionality section
, m_Name_Functionality_Section(_T("Functionality"))
, m_optFuncSM_CONTROL(_T("SM_CONTROL"))
, m_optFuncGD_CONTROL(_T("GD_CONTROL"))
{
 m_Name_Indicators_Section[0] = _T("Indicators");
 m_Name_Indicators_Section[1] = _T("IndicatorsEx");

 for(int i = 0; i < 2; ++i)
 {
  m_optIndHeightPercent[i].name = _T("IndHeightPercent");
  m_optIndRows[i].name = _T("IndRows");
  m_optIndGas_v[i].name = _T("IndGas_v");;
  m_optIndCarb[i].name = _T("IndThrottle");
  m_optIndIdleValve[i].name = _T("IndIdleCutoff");
  m_optIndPowerValve[i].name = _T("IndPowerValve");
  m_optIndStBlock[i].name = _T("IndStBlock");
  m_optIndAE[i].name = _T("IndAE");
  m_optIndCoolingFan[i].name = _T("IndCoolingFan");
  m_optIndCE[i].name = _T("IndCE");
  m_optIndFCRevLim[i].name = _T("IndFCRevLim");
  m_optIndFloodClear[i].name = _T("IndFloodClear");
  m_optIndSysLocked[i].name = _T("IndSysLocked");
  m_optIndIgn_i[i].name = _T("IndIgn_i");
  m_optIndCond_i[i].name = _T("IndCond_i");
  m_optIndEpas_i[i].name = _T("IndEpas_i");
  m_optIndAftStrEnr[i].name = _T("IndAftStrEnr");
  m_optIndIacClLoop[i].name = _T("IndIacClLoop");
  m_optIndUniOut1[i].name = _T("UnivOut1");
  m_optIndUniOut2[i].name = _T("UnivOut2");
  m_optIndUniOut3[i].name = _T("UnivOut3");
  m_optIndUniOut4[i].name = _T("UnivOut4");
  m_optIndUniOut5[i].name = _T("UnivOut5");
  m_optIndUniOut6[i].name = _T("UnivOut6");
  m_optIndGpa4_i[i].name = _T("IndGpa4_i");
  m_optIndInput1[i].name = _T("IndInput1_i");
  m_optIndInput2[i].name = _T("IndInput2_i");
  m_optIndAuto_i[i].name = _T("IndAuto_i");
  m_optIndMapsel0[i].name = _T("IndMapsel0");
  m_optIndRefprs_i[i].name = _T("IndRefprs_i");
  m_optIndAltrn_i[i].name = _T("IndAltrn_i");
 }

 m_Name_Meters_Section[0] = _T("Meters");
 m_Name_Meters_Section[1] = _T("MetersEx");
 m_Name_MetersSc_Section  = _T("MetersScale");

 for(int i = 0; i < 2; ++i)
 {
  m_optMetRows[i].name = _T("MetRows");
  m_optMetScRPM[0].name = m_optMetRPM[i][0].name = _T("MetRPM");
  m_optMetScRPM[1].name = m_optMetRPM[i][1].name = _T("GrhRPM");
  m_optMetScMAP[0].name = m_optMetMAP[i][0].name = _T("MetMAP");
  m_optMetScMAP[1].name = m_optMetMAP[i][1].name = _T("GrhMAP");
  m_optMetScVBat[0].name = m_optMetVBat[i][0].name = _T("MetVBat");
  m_optMetScVBat[1].name = m_optMetVBat[i][1].name = _T("GrhVBat");
  m_optMetScIgnTim[0].name = m_optMetIgnTim[i][0].name = _T("MetIgnTim");
  m_optMetScIgnTim[1].name = m_optMetIgnTim[i][1].name = _T("GrhIgnTim");
  m_optMetScCLT[0].name = m_optMetCLT[i][0].name = _T("MetCLT");
  m_optMetScCLT[1].name = m_optMetCLT[i][1].name = _T("GrhCLT");
  m_optMetScAddI1[0].name = m_optMetAddI1[i][0].name = _T("MetAddI1");
  m_optMetScAddI1[1].name = m_optMetAddI1[i][1].name = _T("GrhAddI1");
  m_optMetScAddI2[0].name = m_optMetAddI2[i][0].name = _T("MetAddI2");
  m_optMetScAddI2[1].name = m_optMetAddI2[i][1].name = _T("GrhAddI2");
  m_optMetScInjPW[0].name = m_optMetInjPW[i][0].name = _T("MetInjPW");
  m_optMetScInjPW[1].name = m_optMetInjPW[i][1].name = _T("GrhInjPW");
  m_optMetScIAT[0].name = m_optMetIAT[i][0].name = _T("MetIAT");
  m_optMetScIAT[1].name = m_optMetIAT[i][1].name = _T("GrhIAT");
  m_optMetScEGOCorr[0].name = m_optMetEGOCorr[i][0].name = _T("MetEGOCorr");
  m_optMetScEGOCorr[1].name = m_optMetEGOCorr[i][1].name = _T("GrhEGOCorr");
  m_optMetScTPS[0].name = m_optMetTPS[i][0].name = _T("MetTPS");
  m_optMetScTPS[1].name = m_optMetTPS[i][1].name = _T("GrhTPS");
  m_optMetScAirFlow[0].name = m_optMetAirFlow[i][0].name = _T("MetAirFlow");
  m_optMetScAirFlow[1].name = m_optMetAirFlow[i][1].name = _T("GrhAirFlow");
  m_optMetScVehicleSpeed[0].name = m_optMetVehicleSpeed[i][0].name = _T("MetVehicleSpeed");
  m_optMetScVehicleSpeed[1].name = m_optMetVehicleSpeed[i][1].name = _T("GrhVehicleSpeed");
  m_optMetScTPSDot[0].name = m_optMetTPSDot[i][0].name = _T("MetTPSDot");
  m_optMetScTPSDot[1].name = m_optMetTPSDot[i][1].name = _T("GrhTPSDot");
  m_optMetScMAPDot[0].name = m_optMetMAPDot[i][0].name = _T("MetMAPDot");
  m_optMetScMAPDot[1].name = m_optMetMAPDot[i][1].name = _T("GrhMAPDot");
  m_optMetScMAP2[0].name = m_optMetMAP2[i][0].name = _T("MetMAP2");
  m_optMetScMAP2[1].name = m_optMetMAP2[i][1].name = _T("GrhMAP2");
  m_optMetScMAPD[0].name = m_optMetMAPD[i][0].name = _T("MetMAPD");
  m_optMetScMAPD[1].name = m_optMetMAPD[i][1].name = _T("GrhMAPD");
  m_optMetScTmp2[0].name = m_optMetTmp2[i][0].name = _T("MetTmp2");
  m_optMetScTmp2[1].name = m_optMetTmp2[i][1].name = _T("GrhTmp2");
  m_optMetScFuelConsum[0].name = m_optMetFuelConsum[i][0].name = _T("MetFuelConsum");
  m_optMetScFuelConsum[1].name = m_optMetFuelConsum[i][1].name = _T("GrhFuelConsum");
  m_optMetScKnockRetard[0].name = m_optMetKnockRetard[i][0].name = _T("MetKnockRetard");
  m_optMetScKnockRetard[1].name = m_optMetKnockRetard[i][1].name = _T("GrhKnockRetard");
  m_optMetScKnockGraph[0].name = m_optMetKnockGraph[i][0].name = _T("MetKnockGraph");
  m_optMetScKnockGraph[1].name = m_optMetKnockGraph[i][1].name = _T("GrhKnockGraph");
  m_optMetScSensAFR[0].name = m_optMetSensAFR[i][0].name = _T("MetSensAFR");
  m_optMetScSensAFR[1].name = m_optMetSensAFR[i][1].name = _T("GrhSensAFR");
  m_optMetScChokePos[0].name = m_optMetChokePos[i][0].name = _T("MetIACPos");
  m_optMetScChokePos[1].name = m_optMetChokePos[i][1].name = _T("GrhIACPos");
  m_optMetScGDPos[0].name = m_optMetGDPos[i][0].name = _T("MetGDPos");
  m_optMetScGDPos[1].name = m_optMetGDPos[i][1].name = _T("GrhGDPos");
  m_optMetScSynLoad[0].name = m_optMetSynLoad[i][0].name = _T("MetSynLoad");
  m_optMetScSynLoad[1].name = m_optMetSynLoad[i][1].name = _T("GrhSynLoad");
  m_optMetScInjTimB[0].name = m_optMetInjTimB[i][0].name = _T("MetInjTimB"); //0...720
  m_optMetScInjTimB[1].name = m_optMetInjTimB[i][1].name = _T("GrhInjTimB"); //0...720
  m_optMetScInjTimE[0].name = m_optMetInjTimE[i][0].name = _T("MetInjTimE"); //0...720
  m_optMetScInjTimE[1].name = m_optMetInjTimE[i][1].name = _T("GrhInjTimE"); //0...720
  m_optMetScInjTimB1[0].name = _T("MetInjTimB1"); //-360...360
  m_optMetScInjTimB1[1].name = _T("GrhInjTimB1"); //-360...360
  m_optMetScInjTimE1[0].name = _T("MetInjTimE1"); //-360...360
  m_optMetScInjTimE1[1].name = _T("GrhInjTimE1"); //-360...360
  m_optMetScFuelConsumF[0].name = m_optMetFuelConsumF[i][0].name = _T("MetFuelConsumF");
  m_optMetScFuelConsumF[1].name = m_optMetFuelConsumF[i][1].name = _T("GrhFuelConsumF");
  m_optMetScGrts[0].name = m_optMetGrts[i][0].name = _T("MetGrts");
  m_optMetScGrts[1].name = m_optMetGrts[i][1].name = _T("GrhGrts");
  m_optMetScFtls[0].name = m_optMetFtls[i][0].name = _T("MetFtls");
  m_optMetScFtls[1].name = m_optMetFtls[i][1].name = _T("GrhFtls");
  m_optMetScEgts[0].name = m_optMetEgts[i][0].name = _T("MetEgts");
  m_optMetScEgts[1].name = m_optMetEgts[i][1].name = _T("GrhEgts");
  m_optMetScOps[0].name = m_optMetOps[i][0].name = _T("MetOps");
  m_optMetScOps[1].name = m_optMetOps[i][1].name = _T("GrhOps");
  m_optMetScInjDuty[0].name = m_optMetInjDuty[i][0].name = _T("MetInjDuty");
  m_optMetScInjDuty[1].name = m_optMetInjDuty[i][1].name = _T("GrhInjDuty");
  m_optMetScMAF[0].name = m_optMetMAF[i][0].name = _T("MetMAF");
  m_optMetScMAF[1].name = m_optMetMAF[i][1].name = _T("GrhMAF");
  m_optMetScVentDuty[0].name = m_optMetVentDuty[i][0].name = _T("MetVentDuty");
  m_optMetScVentDuty[1].name = m_optMetVentDuty[i][1].name = _T("GrhVentDuty");
  m_optMetScFts[0].name = m_optMetFts[i][0].name = _T("MetFts");
  m_optMetScFts[1].name = m_optMetFts[i][1].name = _T("GrhFts");
  m_optMetScEGOCorr2[0].name = m_optMetEGOCorr2[i][0].name = _T("MetEGOCorr2");
  m_optMetScEGOCorr2[1].name = m_optMetEGOCorr2[i][1].name = _T("GrhEGOCorr2");
  m_optMetScSensAFR2[0].name = m_optMetSensAFR2[i][0].name = _T("MetSensAFR2");
  m_optMetScSensAFR2[1].name = m_optMetSensAFR2[i][1].name = _T("GrhSensAFR2");
  m_optMetScTargAFR[0].name = m_optMetTargAFR[i][0].name = _T("MetMapAFR");
  m_optMetScTargAFR[1].name = m_optMetTargAFR[i][1].name = _T("GrhMapAFR");
  m_optMetScDiffAFR[0].name = m_optMetDiffAFR[i][0].name = _T("MetDiffAFR");
  m_optMetScDiffAFR[1].name = m_optMetDiffAFR[i][1].name = _T("GrhDiffAFR");
  m_optMetScDiffAFR2[0].name = m_optMetDiffAFR2[i][0].name = _T("MetDiffAFR2");
  m_optMetScDiffAFR2[1].name = m_optMetDiffAFR2[i][1].name = _T("GrhDiffAFR2");
  m_optMetScGPS[0].name = m_optMetGPS[i][0].name = _T("MetGPS");
  m_optMetScGPS[1].name = m_optMetGPS[i][1].name = _T("GrhGPS");
  m_optMetScFPS[0].name = m_optMetFPS[i][0].name = _T("MetFPS");
  m_optMetScFPS[1].name = m_optMetFPS[i][1].name = _T("GrhFPS");
 }

 //заполняем базу данных допустимых скоростей для COM-порта
 m_AllowableBaudRates.push_back(CBR_9600);
 m_AllowableBaudRates.push_back(CBR_14400);
 m_AllowableBaudRates.push_back(CBR_19200);
 m_AllowableBaudRates.push_back(CBR_38400);
 m_AllowableBaudRates.push_back(CBR_56000);
 m_AllowableBaudRates.push_back(CBR_57600);
 m_AllowableBaudRates.push_back(CBR_115200);
 m_AllowableBaudRates.push_back(CBR_256000);
 m_AllowableBaudRates.push_back(SECU3_CBR_250000);  //not standard
 m_AllowableBaudRates.push_back(SECU3_CBR_500000);  //not standard
 //fill database with allowed COM-port numbers
 for(int i = 1; i < 256; i++)
 {
  CString str;
  str.Format(_T("COM%d"),i);
  m_AllowablePorts.push_back(_TSTRING(str));
 }
 m_AllowablePorts.push_back(_T("/dev/ttyUSB0"));
 m_AllowablePorts.push_back(_T("/dev/ttyUSB1"));
 m_AllowablePorts.push_back(_T("/dev/rfcomm0")); //bluetooth (sudo rfcomm bind 0 MAC 1) MAC - MAC address of paired bluetooth device
 m_AllowablePorts.push_back(_T("/dev/rfcomm1")); //bluetooth

 _tcscpy(m_app_directory, ModuleName::GetExecDirPath().c_str());

 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\";\"  semicolon")), ';'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"%\"  percent")),   '%'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\",\"  comma")),     ','));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"+\"  plus")),      '+'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"|\"  stick")),     '|'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"*\"  star")),      '*'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"=\"  equals")),    '='));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"!\"  exclamation")), '!'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"#\"  hash")),      '#'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"$\"  dollar")),    '$'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"&\"  ampersand")), '&'));

 m_AllowableLanguages.push_back(std::make_pair(std::make_pair(_TSTRING(_T("English")), _TSTRING(_T("english"))), IL_ENGLISH) );
 m_AllowableLanguages.push_back(std::make_pair(std::make_pair(_TSTRING(_T("Russian")), _TSTRING(_T("russian"))), IL_RUSSIAN) );

 m_AllowableWndStates.push_back(std::make_pair(std::make_pair(_TSTRING(_T("Minimized")), _TSTRING(_T("minimized"))), 0) );
 m_AllowableWndStates.push_back(std::make_pair(std::make_pair(_TSTRING(_T("Normal")), _TSTRING(_T("normal"))), 1) );
 m_AllowableWndStates.push_back(std::make_pair(std::make_pair(_TSTRING(_T("Maximized")), _TSTRING(_T("maximized"))), 2) );

 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega1284")), _TSTRING(_T("atmega1284"))), EP_ATMEGA1284) );
// m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega644")), _TSTRING(_T("atmega644"))), EP_ATMEGA644) );
// m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega128")), _TSTRING(_T("atmega128"))), EP_ATMEGA128) );
// m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega64")), _TSTRING(_T("atmega64"))), EP_ATMEGA64) );

 m_AllowableSpeedUnits.push_back(std::make_pair(std::make_pair(_TSTRING(_T("km/h")), _TSTRING(_T("kmh"))), SU_KMH));
 m_AllowableSpeedUnits.push_back(std::make_pair(std::make_pair(_TSTRING(_T("m/h")), _TSTRING(_T("mph"))), SU_MPH));
}

CAppSettingsModel::~CAppSettingsModel()
{
 //empty
}

CString CAppSettingsModel::GetINIFileFullName(void) const
{
 _TSTRING directory(m_app_directory);
 if (0==directory.size())
  return _T("");

 ModuleName::AddSlashToPath(directory); //add '\' if absent
 return CString(directory.c_str()) + AfxGetApp()->m_pszExeName + _T(".ini");
}

CString CAppSettingsModel::GetAppDirectory(void) const
{
 return m_app_directory;
}

bool CAppSettingsModel::ReadSettings(void)
{
 bool status = true;
 CString IniFileName = GetINIFileFullName();

 //Options
 IniIO os(IniFileName, m_Name_Options_Section);

 os.ReadString(m_optPortName, _T("COM1"));
 os.ReadDword(m_optBaudRateApplication, _T("115200"), m_AllowableBaudRates);
 os.ReadDword(m_optBaudRateBootloader, _T("115200"), m_AllowableBaudRates);
 os.ReadString(m_optLogFilesFolder, m_app_directory); //use app. dir. as default value
 os.ReadChar(m_optCSVSepSymbol, _T(";"), m_AllowaleCSVSepSymbols);
 os.ReadChar(m_optMapCSVSepSymbol, _T(";"), m_AllowaleCSVSepSymbols);
 os.ReadInt(m_optUseAppFolder, _T("1"), 0, 1);
 os.ReadInt(m_optAlwaysWriteLog, _T("1"), 0, 1);
 os.ReadInt(m_optMIDeskUpdatePeriod, _T("40"), 0, 1000); 
 os.ReadEnum(m_optInterfaceLang, 0, m_AllowableLanguages);
 os.ReadEnum(m_optECUPlatformType, 0, m_AllowablePlatforms); //1284
 os.ReadInt(m_optDVDeskUpdatePeriod, _T("40"), 0, 1000);
 os.ReadInt(m_optUseDVFeatures, _T("0"), 0, 1);
 os.ReadInt(m_optShowToolTips, _T("1"), 0, 1);
 os.ReadInt(m_optCOMPortBother, _T("1"), 0, 1);
 os.ReadInt(m_optUseHotKeys, _T("1"), 0, 1);
 os.ReadInt(m_optShowWelcome, _T("1"), 0, 1);
 os.ReadInt(m_optAllowVisualTheme, _T("1"), 0, 1);
 os.ReadInt(m_optAutoDiagEnter, _T("0"), 0, 1);
 os.ReadInt(m_optSaveWarning, _T("1"), 0, 1);
 os.ReadInt(m_optAutoCERead, _T("0"), 0, 1);
 os.ReadInt(m_optChildCharts, _T("1"), 0, 1);
 os.ReadInt(m_optPortAutoReopen, _T("1"), 0, 1);
 os.ReadInt(m_optToggleMapWnd, _T("0"), 0, 1);
 os.ReadInt(m_optEmbedMapWnd, _T("0"), 0, 1);
 os.ReadInt(m_optExistingPorts, _T("1"), 0, 1);
 os.ReadInt(m_optToolTipTime, _T("5000"), 1000, 60000);
 os.ReadInt(m_optIniEdSyntax, _T("1"), 0, 1);
 os.ReadInt(m_optBldrEEPROMBlocks, _T("1"), 0, 1);
 os.ReadFlt(m_optFuelDensity1,_T("0.710"), 0.2f, 2.0f); //1-st fuel (e.g. petrol)
 os.ReadFlt(m_optFuelDensity2,_T("0.536"), 0.2f, 2.0f); //2-nd fuel (e.g. LPG)
 os.ReadInt(m_optLogBinaryFmt, _T("0"), 0, 1);
 os.ReadInt(m_optCreateWindows, _T("1"), 0, 1);
 os.ReadInt(m_optDbgVarsToFile, _T("0"), 0, 1);
 os.ReadInt(m_optClassic2DKeys, _T("0"), 0, 1);

 //fixtures
 IniIO fs(IniFileName, m_Name_Fixtures_Section);

 fs.ReadInt(m_optShowExFixtures, _T("0"), 0, 1);
 fs.ReadInt(m_optShowSpeedAndDist, _T("1"), 0, 1);
 fs.ReadInt(m_optRPMAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optVoltAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optMAPAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optAddI1Average, _T("4"), 0, 16);
 fs.ReadInt(m_optTPSAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optKnockAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optIgnTimAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optCLTAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optAddI2Average, _T("4"), 0, 16);
 fs.ReadInt(m_optInjPWAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optIATAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optEGOCorrAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optAirFlowAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optVehicleSpeedAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optTPSDotAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optMAPDotAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optMAP2Average, _T("4"), 0, 16);
 fs.ReadInt(m_optGPSAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optMAPDAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optTmp2Average, _T("4"), 0, 16);
 fs.ReadInt(m_optFuelConsumAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optKnockRetardAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optSensAFRAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optChokePosAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optGDPosAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optSynLoadAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optInjTimBAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optInjTimEAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optFuelConsumFAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optGrtsAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optFtlsAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optEgtsAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optOpsAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optMAFAverage, _T("0"), 0, 16);
 fs.ReadInt(m_optFtsAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optTargAFRAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optFPSAverage, _T("4"), 0, 16);

 fs.ReadEnum(m_optSpeedUnit, 0, m_AllowableSpeedUnits);
 fs.ReadInt(m_optTitleFontSize, _T("100"), 10, 200);
 fs.ReadInt(m_optValueFontSize, _T("130"), 10, 200);
 fs.ReadInt(m_optPaneFontSize, _T("100"), 10, 200);
 fs.ReadInt(m_optLabelFontSize, _T("100"), 10, 200);
 fs.ReadInt(m_optMetersDragNDrop, _T("1"), 0, 1);
 fs.ReadInt(m_optIndicatorsDragNDrop, _T("1"), 0, 1);
 fs.ReadInt(m_optFFFConst, _T("16000"), 1000, 32000);
 fs.ReadInt(m_optShowGraphCursor, _T("1"), 0, 1);
 fs.ReadInt(m_optGraphShtPixels, _T("2"), 2, 10);
 fs.ReadInt(m_optGraphShowValue, _T("0"), 0, 1);
 fs.ReadInt(m_optGraphValueHeight, _T("100"), 2, 200);

 //Positions of windows
 IniIO ws(IniFileName, m_Name_WndSettings_Section);

 ws.ReadWndPos(m_optMainFrmWnd); //Main frame window
 ws.ReadWndPos(m_optStrtMapWnd);
 ws.ReadWndPos(m_optIdleMapWnd);
 ws.ReadWndPos(m_optWorkMapWnd);
 ws.ReadWndPos(m_optTempMapWnd);
 ws.ReadWndPos(m_optTempIdlMapWnd);
 ws.ReadWndPos(m_optGridMapIgnWnd);
 ws.ReadWndPos(m_optGridMapInjWnd);
 ws.ReadWndPos(m_optVEMapWnd);
 ws.ReadWndPos(m_optVE2MapWnd);
 ws.ReadWndPos(m_optVEIMapWnd);
 ws.ReadWndPos(m_optAFRMapWnd);
 ws.ReadWndPos(m_optCrnkMapWnd);
 ws.ReadWndPos(m_optWrmpMapWnd);
 ws.ReadWndPos(m_optDeadMapWnd);
 ws.ReadWndPos(m_optIdlrMapWnd);
 ws.ReadWndPos(m_optIdlcMapWnd);
 ws.ReadWndPos(m_optThrassMapWnd);
 ws.ReadWndPos(m_optAETPSMapWnd);
 ws.ReadWndPos(m_optAEMAPMapWnd);
 ws.ReadWndPos(m_optAERPMMapWnd);
 ws.ReadWndPos(m_optAftstrMapWnd);
 ws.ReadWndPos(m_optITMapWnd);
 ws.ReadWndPos(m_optITRPMMapWnd);
 ws.ReadWndPos(m_optRigidMapWnd);
 ws.ReadWndPos(m_optEGOCrvMapWnd);
 ws.ReadWndPos(m_optIACCMapWnd);
 ws.ReadWndPos(m_optIACCWMapWnd);
 ws.ReadWndPos(m_optIATCLTMapWnd);
 ws.ReadWndPos(m_optTpsswtMapWnd);
 ws.ReadWndPos(m_optTpszonMapWnd);
 ws.ReadWndPos(m_optGtscMapWnd);
 ws.ReadWndPos(m_optGpscMapWnd);
 ws.ReadWndPos(m_optAtscMapWnd);
 ws.ReadWndPos(m_optPwm1MapWnd);
 ws.ReadWndPos(m_optPwm2MapWnd);
 ws.ReadWndPos(m_optIACMATMapWnd);
 ws.ReadWndPos(m_optCylMultMapWnd);
 ws.ReadWndPos(m_optCylAddMapWnd);
 //group of separate maps
 ws.ReadWndPos(m_optAttenuatorMapWnd); 
 ws.ReadWndPos(m_optDwellCntrlMapWnd);
 ws.ReadWndPos(m_optCTSCurveMapWnd);
 ws.ReadWndPos(m_optATSCurvMapWnd);
 ws.ReadWndPos(m_optATSCorrMapWnd);
 ws.ReadWndPos(m_optGasdoseMapWnd);
 ws.ReadWndPos(m_optBarocorrMapWnd);
 ws.ReadWndPos(m_optManIgntimMapWnd);
 ws.ReadWndPos(m_optCESettingsWnd);
 ws.ReadWndPos(m_optCrkTempMapWnd);
 ws.ReadWndPos(m_optEHPauseMapWnd);
 ws.ReadWndPos(m_optCrankingThrdMapWnd);
 ws.ReadWndPos(m_optCrankingTimeMapWnd);
 ws.ReadWndPos(m_optSmapabanThrdMapWnd);
 ws.ReadWndPos(m_optKnockZoneMapWnd);
 ws.ReadWndPos(m_optGrtsCurveMapWnd);
 ws.ReadWndPos(m_optGrHeatDutyMapWnd);
 ws.ReadWndPos(m_optPwmIacUCoefMapWnd);
 ws.ReadWndPos(m_optAftstrStrk0MapWnd);
 ws.ReadWndPos(m_optAftstrStrk1MapWnd);
 ws.ReadWndPos(m_optGrValDelMapWnd);
 ws.ReadWndPos(m_optFtlsCurveMapWnd);
 ws.ReadWndPos(m_optEgtsCurveMapWnd);
 ws.ReadWndPos(m_optOpsCurveMapWnd);
 ws.ReadWndPos(m_optManInjPwcMapWnd);
 ws.ReadWndPos(m_optMAFCurveMapWnd);
 ws.ReadWndPos(m_optTmp2CurveMapWnd);
 ws.ReadWndPos(m_optFtlsCorMapWnd);
 ws.ReadWndPos(m_optLambdaZoneMapWnd);
 ws.ReadWndPos(m_optFtsCurveMapWnd);
 ws.ReadWndPos(m_optFuelDensCorrMapWnd);
 ws.ReadWndPos(m_optXtauXfAccMapWnd);
 ws.ReadWndPos(m_optXtauXfDecMapWnd);
 ws.ReadWndPos(m_optXtauTfAccMapWnd);
 ws.ReadWndPos(m_optXtauTfDecMapWnd);
 ws.ReadWndPos(m_optInjNonLinPMapWnd);
 ws.ReadWndPos(m_optInjNonLinGMapWnd);
 ws.ReadWndPos(m_optEGODelayMapWnd);
 ws.ReadWndPos(m_optWUAFR0MapWnd);
 ws.ReadWndPos(m_optWUAFR1MapWnd);

 //Positions of windows (online tables)
 IniIO ws1(IniFileName, m_Name_WndSettings_Section1);
 ws1.ReadWndPos(m_optStrtMapWnd1);
 ws1.ReadWndPos(m_optIdleMapWnd1);
 ws1.ReadWndPos(m_optWorkMapWnd1);
 ws1.ReadWndPos(m_optTempMapWnd1);
 ws1.ReadWndPos(m_optTempIdlMapWnd1);
 ws1.ReadWndPos(m_optGridMapIgnWnd1);
 ws1.ReadWndPos(m_optGridMapInjWnd1);
 ws1.ReadWndPos(m_optVEMapWnd1);
 ws1.ReadWndPos(m_optVE2MapWnd1);
 ws1.ReadWndPos(m_optVEIMapWnd1);
 ws1.ReadWndPos(m_optAFRMapWnd1);
 ws1.ReadWndPos(m_optCrnkMapWnd1);
 ws1.ReadWndPos(m_optWrmpMapWnd1);
 ws1.ReadWndPos(m_optDeadMapWnd1);
 ws1.ReadWndPos(m_optIdlrMapWnd1);
 ws1.ReadWndPos(m_optIdlcMapWnd1);
 ws1.ReadWndPos(m_optThrassMapWnd1);
 ws1.ReadWndPos(m_optAETPSMapWnd1);
 ws1.ReadWndPos(m_optAEMAPMapWnd1);
 ws1.ReadWndPos(m_optAERPMMapWnd1);
 ws1.ReadWndPos(m_optAftstrMapWnd1);
 ws1.ReadWndPos(m_optITMapWnd1);
 ws1.ReadWndPos(m_optITRPMMapWnd1);
 ws1.ReadWndPos(m_optRigidMapWnd1);
 ws1.ReadWndPos(m_optEGOCrvMapWnd1);
 ws1.ReadWndPos(m_optIACCMapWnd1);
 ws1.ReadWndPos(m_optIACCWMapWnd1);
 ws1.ReadWndPos(m_optIATCLTMapWnd1);
 ws1.ReadWndPos(m_optTpsswtMapWnd1);
 ws1.ReadWndPos(m_optGtscMapWnd1);
 ws1.ReadWndPos(m_optGpscMapWnd1);
 ws1.ReadWndPos(m_optAtscMapWnd1);
 ws1.ReadWndPos(m_optPwm1MapWnd1);
 ws1.ReadWndPos(m_optPwm2MapWnd1);
 ws1.ReadWndPos(m_optIACMATMapWnd1);
 ws1.ReadWndPos(m_optTpszonMapWnd1);
 ws1.ReadWndPos(m_optCylMultMapWnd1);
 ws1.ReadWndPos(m_optCylAddMapWnd1);
 //group of separate maps
 ws1.ReadWndPos(m_optAttenuatorMapWnd1); 
 ws1.ReadWndPos(m_optDwellCntrlMapWnd1);
 ws1.ReadWndPos(m_optCTSCurveMapWnd1);
 ws1.ReadWndPos(m_optATSCurvMapWnd1);
 ws1.ReadWndPos(m_optATSCorrMapWnd1);
 ws1.ReadWndPos(m_optGasdoseMapWnd1);
 ws1.ReadWndPos(m_optBarocorrMapWnd1);
 ws1.ReadWndPos(m_optManIgntimMapWnd1);
 ws1.ReadWndPos(m_optCESettingsWnd1);
 ws1.ReadWndPos(m_optCrkTempMapWnd1);
 ws1.ReadWndPos(m_optEHPauseMapWnd1);
 ws1.ReadWndPos(m_optCrankingThrdMapWnd1);
 ws1.ReadWndPos(m_optCrankingTimeMapWnd1);
 ws1.ReadWndPos(m_optSmapabanThrdMapWnd1);
 ws1.ReadWndPos(m_optKnockZoneMapWnd1);
 ws1.ReadWndPos(m_optGrtsCurveMapWnd1);
 ws1.ReadWndPos(m_optGrHeatDutyMapWnd1);
 ws1.ReadWndPos(m_optPwmIacUCoefMapWnd1);
 ws1.ReadWndPos(m_optAftstrStrk0MapWnd1);
 ws1.ReadWndPos(m_optAftstrStrk1MapWnd1);
 ws1.ReadWndPos(m_optGrValDelMapWnd1);
 ws1.ReadWndPos(m_optFtlsCurveMapWnd1);
 ws1.ReadWndPos(m_optEgtsCurveMapWnd1);
 ws1.ReadWndPos(m_optOpsCurveMapWnd1);
 ws1.ReadWndPos(m_optManInjPwcMapWnd1);
 ws1.ReadWndPos(m_optMAFCurveMapWnd1);
 ws1.ReadWndPos(m_optTmp2CurveMapWnd1);
 ws1.ReadWndPos(m_optFtlsCorMapWnd1);
 ws1.ReadWndPos(m_optLambdaZoneMapWnd1);
 ws1.ReadWndPos(m_optFtsCurveMapWnd1);
 ws1.ReadWndPos(m_optFuelDensCorrMapWnd1);
 ws1.ReadWndPos(m_optXtauXfAccMapWnd1);
 ws1.ReadWndPos(m_optXtauXfDecMapWnd1);
 ws1.ReadWndPos(m_optXtauTfAccMapWnd1);
 ws1.ReadWndPos(m_optXtauTfDecMapWnd1);
 ws1.ReadWndPos(m_optInjNonLinPMapWnd1);
 ws1.ReadWndPos(m_optInjNonLinGMapWnd1);
 ws1.ReadWndPos(m_optEGODelayMapWnd1);
 ws1.ReadWndPos(m_optWUAFR0MapWnd1);
 ws1.ReadWndPos(m_optWUAFR1MapWnd1);

 //Sizes of windows
 IniIO sz(IniFileName, m_Name_WndSize_Section);
 sz.ReadWndPos(m_optMainFrmWndSize, 0, 10000); //Main frame window
 sz.ReadWndPos(m_optStrtMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optIdleMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optWorkMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optTempMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optTempIdlMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optGridMapIgnWndSize, 0, 10000);
 sz.ReadWndPos(m_optGridMapInjWndSize, 0, 10000);
 sz.ReadWndPos(m_optVEMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optVE2MapWndSize, 0, 10000);
 sz.ReadWndPos(m_optVEIMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optAFRMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optCrnkMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optWrmpMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optDeadMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optIdlrMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optIdlcMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optThrassMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optAETPSMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optAEMAPMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optAERPMMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optAftstrMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optITMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optITRPMMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optRigidMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optEGOCrvMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optIACCMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optIACCWMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optIATCLTMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optTpsswtMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optTpszonMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optGtscMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optGpscMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optAtscMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optPwm1MapWndSize, 0, 10000);
 sz.ReadWndPos(m_optPwm2MapWndSize, 0, 10000);
 sz.ReadWndPos(m_optIACMATMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optCylMultMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optCylAddMapWndSize, 0, 10000);
 //group of separate maps
 sz.ReadWndPos(m_optAttenuatorMapWndSize, 0, 10000); 
 sz.ReadWndPos(m_optDwellCntrlMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optCTSCurveMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optATSCurvMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optATSCorrMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optGasdoseMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optBarocorrMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optManIgntimMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optCESettingsWndSize, 0, 10000);
 sz.ReadWndPos(m_optTmp2CurveMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optCrkTempMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optEHPauseMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optCrankingThrdMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optCrankingTimeMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optSmapabanThrdMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optKnockZoneMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optGrtsCurveMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optGrHeatDutyMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optPwmIacUCoefMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optAftstrStrk0MapWndSize, 0, 10000);
 sz.ReadWndPos(m_optAftstrStrk1MapWndSize, 0, 10000);
 sz.ReadWndPos(m_optGrValDelMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optFtlsCurveMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optEgtsCurveMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optOpsCurveMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optManInjPwcMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optMAFCurveMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optFtlsCorMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optLambdaZoneMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optFtsCurveMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optFuelDensCorrMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optXtauXfAccMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optXtauXfDecMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optXtauTfAccMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optXtauTfDecMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optInjNonLinPMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optInjNonLinGMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optEGODelayMapWndSize, 0, 10000);
 sz.ReadWndPos(m_optWUAFR0MapWndSize, 0, 10000);
 sz.ReadWndPos(m_optWUAFR1MapWndSize, 0, 10000);

 //Positions of windows (online tables)
 IniIO sz1(IniFileName, m_Name_WndSize_Section1);
 sz1.ReadWndPos(m_optStrtMapWndSize1);
 sz1.ReadWndPos(m_optIdleMapWndSize1);
 sz1.ReadWndPos(m_optWorkMapWndSize1);
 sz1.ReadWndPos(m_optTempMapWndSize1);
 sz1.ReadWndPos(m_optTempIdlMapWndSize1);
 sz1.ReadWndPos(m_optGridMapIgnWndSize1);
 sz1.ReadWndPos(m_optGridMapInjWndSize1);
 sz1.ReadWndPos(m_optVEMapWndSize1);
 sz1.ReadWndPos(m_optVE2MapWndSize1);
 sz1.ReadWndPos(m_optVEIMapWndSize1);
 sz1.ReadWndPos(m_optAFRMapWndSize1);
 sz1.ReadWndPos(m_optCrnkMapWndSize1);
 sz1.ReadWndPos(m_optWrmpMapWndSize1);
 sz1.ReadWndPos(m_optDeadMapWndSize1);
 sz1.ReadWndPos(m_optIdlrMapWndSize1);
 sz1.ReadWndPos(m_optIdlcMapWndSize1);
 sz1.ReadWndPos(m_optThrassMapWndSize1);
 sz1.ReadWndPos(m_optAETPSMapWndSize1);
 sz1.ReadWndPos(m_optAEMAPMapWndSize1);
 sz1.ReadWndPos(m_optAERPMMapWndSize1);
 sz1.ReadWndPos(m_optAftstrMapWndSize1);
 sz1.ReadWndPos(m_optITMapWndSize1);
 sz1.ReadWndPos(m_optITRPMMapWndSize1);
 sz1.ReadWndPos(m_optRigidMapWndSize1);
 sz1.ReadWndPos(m_optEGOCrvMapWndSize1);
 sz1.ReadWndPos(m_optIACCMapWndSize1);
 sz1.ReadWndPos(m_optIACCWMapWndSize1);
 sz1.ReadWndPos(m_optIATCLTMapWndSize1);
 sz1.ReadWndPos(m_optTpsswtMapWndSize1);
 sz1.ReadWndPos(m_optGtscMapWndSize1);
 sz1.ReadWndPos(m_optGpscMapWndSize1);
 sz1.ReadWndPos(m_optAtscMapWndSize1);
 sz1.ReadWndPos(m_optPwm1MapWndSize1);
 sz1.ReadWndPos(m_optPwm2MapWndSize1);
 sz1.ReadWndPos(m_optIACMATMapWndSize1);
 sz1.ReadWndPos(m_optTpszonMapWndSize1);
 sz1.ReadWndPos(m_optCylMultMapWndSize1);
 sz1.ReadWndPos(m_optCylAddMapWndSize1);
 //group of separate maps
 sz1.ReadWndPos(m_optAttenuatorMapWndSize1, 0, 10000); 
 sz1.ReadWndPos(m_optDwellCntrlMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optCTSCurveMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optATSCurvMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optATSCorrMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optGasdoseMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optBarocorrMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optManIgntimMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optCESettingsWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optTmp2CurveMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optCrkTempMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optEHPauseMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optCrankingThrdMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optCrankingTimeMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optSmapabanThrdMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optKnockZoneMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optGrtsCurveMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optGrHeatDutyMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optPwmIacUCoefMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optAftstrStrk0MapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optAftstrStrk1MapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optGrValDelMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optFtlsCurveMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optEgtsCurveMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optOpsCurveMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optManInjPwcMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optMAFCurveMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optFtlsCorMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optLambdaZoneMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optFtsCurveMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optFuelDensCorrMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optXtauXfAccMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optXtauXfDecMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optXtauTfAccMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optXtauTfDecMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optInjNonLinPMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optInjNonLinGMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optEGODelayMapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optWUAFR0MapWndSize1, 0, 10000);
 sz1.ReadWndPos(m_optWUAFR1MapWndSize1, 0, 10000);

 //State of windows
 IniIO sw(IniFileName, m_Name_WndState_Section);
 sw.ReadEnum(m_optMainFrmWndState, 1, m_AllowableWndStates); //Main frame window

 //Indicators
 for(int i = 0; i < 2; ++i)
 {
  IniIO ii(IniFileName, m_Name_Indicators_Section[i]);
  ii.ReadFlt(m_optIndHeightPercent[i],_T("10.0"), 1.0, 100.0);
  ii.ReadInt(m_optIndRows[i],_T("1"), 1, 10);
  ii.ReadInt(m_optIndGas_v[i],_T("0"), 0, 32, true);
  ii.ReadInt(m_optIndCarb[i],_T("1"), 0, 32, true);
  ii.ReadInt(m_optIndIdleValve[i],_T("2"), 0, 32, true);
  ii.ReadInt(m_optIndPowerValve[i],_T("3"), 0, 32, true);
  ii.ReadInt(m_optIndStBlock[i],_T("4"), 0, 32, true);
  ii.ReadInt(m_optIndAE[i],_T("5"), 0, 32, true);
  ii.ReadInt(m_optIndCoolingFan[i],_T("6"), 0, 32, true);
  ii.ReadInt(m_optIndCE[i],_T("7"), 0, 32, true);
  ii.ReadInt(m_optIndFCRevLim[i],_T("8"), 0, 32, true);
  ii.ReadInt(m_optIndFloodClear[i],_T("9"), 0, 32, true);
  ii.ReadInt(m_optIndSysLocked[i],_T("10"), 0, 32, true);
  ii.ReadInt(m_optIndIgn_i[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndCond_i[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndEpas_i[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndAftStrEnr[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndIacClLoop[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndUniOut1[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndUniOut2[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndUniOut3[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndUniOut4[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndUniOut5[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndUniOut6[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndGpa4_i[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndInput1[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndInput2[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndAuto_i[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndMapsel0[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndRefprs_i[i],_T(""), 0, 32, true);
  ii.ReadInt(m_optIndAltrn_i[i],_T(""), 0, 32, true);
 }

 IniIO ic(IniFileName, m_Name_IndColors_Section);
 ic.ReadColor(m_optColGas_v,_T("00FF00"));
 ic.ReadColor(m_optColCarb,_T("00FF00"));
 ic.ReadColor(m_optColIdleValve,_T("00FF00"));
 ic.ReadColor(m_optColPowerValve,_T("00FF00"));
 ic.ReadColor(m_optColStBlock,_T("00FF00"));
 ic.ReadColor(m_optColAE,_T("00FF00"));
 ic.ReadColor(m_optColCoolingFan,_T("00FF00"));
 ic.ReadColor(m_optColCE,_T("00FF00"));
 ic.ReadColor(m_optColFCRevLim,_T("00FF00"));
 ic.ReadColor(m_optColFloodClear,_T("00FF00"));
 ic.ReadColor(m_optColSysLocked,_T("00FF00"));
 ic.ReadColor(m_optColIgn_i,_T("00FF00"));
 ic.ReadColor(m_optColCond_i,_T("00FF00"));
 ic.ReadColor(m_optColEpas_i,_T("00FF00"));
 ic.ReadColor(m_optColAftStrEnr,_T("00FF00"));
 ic.ReadColor(m_optColIacClLoop,_T("00FF00"));
 ic.ReadColor(m_optColUniOut1,_T("00FF00"));
 ic.ReadColor(m_optColUniOut2,_T("00FF00"));
 ic.ReadColor(m_optColUniOut3,_T("00FF00"));
 ic.ReadColor(m_optColUniOut4,_T("00FF00"));
 ic.ReadColor(m_optColUniOut5,_T("00FF00"));
 ic.ReadColor(m_optColUniOut6,_T("00FF00"));
 ic.ReadColor(m_optColGpa4_i,_T("00FF00"));
 ic.ReadColor(m_optColInput1,_T("00FF00"));
 ic.ReadColor(m_optColInput2,_T("00FF00"));
 ic.ReadColor(m_optColAuto_i,_T("00FF00"));
 ic.ReadColor(m_optColMapsel0,_T("00FF00"));
 ic.ReadColor(m_optColRefprs_i,_T("00FF00"));
 ic.ReadColor(m_optColAltrn_i,_T("00FF00"));

 //Meters
 const TCHAR* metDef[2][43*2] = {{_T("0"),_T("1"),_T("2"),_T("3"),_T("4"),_T("5"),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T("")},
                                 {_T("0"),_T("1"),_T("2"),_T("5"),_T("6"),_T("7"),_T("3"),_T(""),_T("4"),_T("8"),_T("9"),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T("")}};
 for(int i = 0; i < 2; ++i)
 {
  IniIO mm(IniFileName, m_Name_Meters_Section[i]);
  mm.ReadInt(m_optMetRows[i],_T("2"), 1, 10);
  int vmax = 45;
  for (int g = 0, d = 0; g < 2; ++g)
  {
   mm.ReadInt(m_optMetRPM[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetMAP[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetVBat[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetIgnTim[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetTPS[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetCLT[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetAddI1[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetAddI2[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetInjPW[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetIAT[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetEGOCorr[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetAirFlow[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetVehicleSpeed[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetTPSDot[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetMAPDot[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetMAP2[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetMAPD[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetTmp2[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetFuelConsum[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetKnockRetard[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetKnockGraph[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetSensAFR[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetChokePos[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetGDPos[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetSynLoad[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetInjTimB[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetInjTimE[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetFuelConsumF[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetGrts[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetFtls[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetEgts[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetOps[i][g], metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetInjDuty[i][g], metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetMAF[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetVentDuty[i][g], metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetFts[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetEGOCorr2[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetSensAFR2[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetTargAFR[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetDiffAFR[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetDiffAFR2[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetGPS[i][g],metDef[i][d++], 0, vmax, true);
   mm.ReadInt(m_optMetFPS[i][g],metDef[i][d++], 0, vmax, true);
  }
 }

 const TCHAR* metScDef[2][45] = {
                             {_T("16, 0, 8000, 0,4000,64FF64, 4000,6000,FFFF64, 6000,8000,FF6464"),  //RPM
                              _T("20, 10, 110, 10,30,B48282, 30,90,78C878, 90,110,B48282"),          //MAP
                              _T("16, 2,  18, 2.0,10.0,FF8080, 10.0,15.0,80FF80, 15.0,18.0,FF0000"), //Vbat
                              _T("16,-15, 65, -15.0,0.0,B48282, 0.0,45.0,DCDC78, 45.0,65.0,FF6464"), //Ign.tim.
                              _T("16, 0,  100, 0,5,8282B4, 5,75,78B496, 75,100,E68282"),             //TPS
                              _T("16,-40, 120, -40,50,8282B4, 50,100,787878, 100,120,E68282"),       //CLT
                              _T("20, 0,  5, 0,5,B4B4E6"), //ADD_I1
                              _T("20, 0,  5, 0,5,B4B4E6"), //ADD_I2
                              _T("24, 0,  24, 0,1,FF6464, 1,10,64FF64, 10,24,9696FA"), //Inj.PW
                              _T("16,-40, 120, -40,50,8282B4, 50,100,787878, 100,120,E68282"), //IAT
                              _T("16,-50, 50, -50,-20,E68282, -20,20,78BE78, 20,50,E68282"),  //EGO corr.
                              _T("16, 0,  16, 0,1,BE9696, 1,2,B496A0, 2,3,AA96AA, 3,4,A096B4, 4,5,9696BE, 5,6,9696C8, 6,7,9696D2, 7,8,9696DC, 8,9,9696E6, 9,10,9696F0, 10,11,9696FA, 11,12,9696FF, 12,13,9696FF, 13,14,9696FF, 14,15,9696FF, 15,16,9696FF"),  //AirFlow
                              _T("22, 0,  220, 0,60,64FF64, 60,120,FAE664, 120,180,E68264, 180,220,FF7878"), //Vehicle seed
                              _T("10,-500,500,-500,-200,FA6464, -200,-50,64FA64, -50,0,6464FA, 0,50,6464FA, 50,200,64FA64, 200,500,FA6464"), //TPSdot
                              _T("10,-500,500,-500,-200,FA6464, -200,-50,64FA64, -50,0,6464FA, 0,50,6464FA, 50,200,64FA64, 200,500,FA6464 "), //MAPdot
                              _T("20, 0,  400, 0,100,B48282, 100,400,78C878"), //MAP2
                              _T("20, 0,  400, 0,100,B48282, 100,400,78C878"), //MAPD
                              _T("16,-40, 120, -40,50,8282B4, 50,100,787878, 100,120,E68282"), //TEMP2
                              _T("25, 0,  50, 0,10,64DC64, 10,20,B4B464, 20,50,DC6464"),  //FuelConsum
                              _T("15, 0,  30, 0,10,DCDC78, 10,30,DC7878"),  //KnockRetard
                              _T("21, 0,  5, 0,0.5,2850DC, 0.5,1.0,3C64B4, 1.0,1.5,5078A0, 1.5,2.0,648C8C, 2.0,2.5,78A078, 2.5,3.0,8CB464, 3.0,3.5,A0A064, 3.5,4.0,B48C64, 4.0,4.5,C87864, 4.5,5.0,E66464"),   //Knock signal
                              _T("18, 6,  24, 6,10,FF8282, 10,14.55,F0F082, 14.55,14.85,82FF82, 14.85,15.45,F0F078, 15.45,15.75,82FF82, 15.75,20,F0F082, 20,24,FF8282"),  //Sens AFR
                              _T("20, 0,  100, 0,100,00E6FA"), //Choke pos.
                              _T("20, 0,  100, 0,100,00E6FA"), //GD pos.
                              _T("20, 0,  250, 0,100,82FA82, 100,250,FA8282"), //SynLoad
                              _T("16, 0,  720,  0,720,FAFAFA"), //InjTimB
                              _T("16, 0,  720,  0,720,FAFAFA"), //InjTimE
                              _T("16,-360,360, -360,360,FAFAFA"), //InjTimB1
                              _T("16,-360,360, -360,360,FAFAFA"), //InjTimE1
                              _T("20, 0,  250, 0,10,828296, 10,70,64F064, 70,150,E6E664, 150,250,F06464"), //FuelConsumF
                              _T("16,-40, 120, -40,50,8282B4, 50,100,787878, 100,120,E68282"), //GRTS
                              _T("20, 0,  100, 0,10,FA6464, 10,50,DCDC64, 50,100,64DC64"), //FTLS
                              _T("16, 0, 1100, 0,400,6464FF, 400,950,C8C878, 950,1100,FF6464"), //EGTS
                              _T("12, 0,  6, 0,1,8282BE, 1,3.5,78F078, 3.5,5.0,F0F078, 5.0,6.0,FF8282"),   //OPS
                              _T("21, 0,  100, 0,10,6464FF, 10,70,64FF64, 70,90,F0FF64, 90,100,FF6464"), //InjDuty
                              _T("20, 0,  600, 0,100,8282B4, 100,300,78C878, 300,500,C8C878, 500,600,C87878"), //MAF
                              _T("21, 0,  100, 0,10,6464FF, 10,70,64FF64, 70,90,F0FF64, 90,100,FF6464"), //VentDuty
                              _T("16,-40, 120, -40,50,8282B4, 50,100,787878, 100,120,E68282"), //FTS
                              _T("16,-50, 50, -50,-20,E68282, -20,20,78BE78, 20,50,E68282"),  //EGO Corr.2
                              _T("18, 6,  24, 6,10,FF8282, 10,14.55,F0F082, 14.55,14.85,82FF82, 14.85,15.45,F0F078, 15.45,15.75,82FF82, 15.75,20,F0F082, 20,24,FF8282"),  //Sens AFR2
                              _T("18, 6,  24, 6,10,FF8282, 10,14.55,F0F082, 14.55,14.85,82FF82, 14.85,15.45,F0F078, 15.45,15.75,82FF82, 15.75,20,F0F082, 20,24,FF8282"),  //TargAFR
                              _T("18,-10, 10, -9,-2,8282FF, -2,-1,F0F082, -1,1,82FF82, 1,2,F0F082, 2,9,FF8282"),  //DiffAFR
                              _T("18,-10, 10, -9,-2,8282FF, -2,-1,F0F082, -1,1,82FF82, 1,2,F0F082, 2,9,FF8282"),  //DiffAFR2
                              _T("20, 0,  400, 0,100,B48282, 100,400,78C878"), //GPS
                              _T("20, 0,  700, 0,300,B48282, 300,700,78C878")},//FPS
                             //graphs
                             {_T("8, 0,  8000"), //RPM
                              _T("10, 10, 110"), //MAP
                              _T("8, 2,  18"),   //VBat
                              _T("10,-15, 65"),  //Ign.tim.
                              _T("10, 0,  100"), //TPS
                              _T("8,-40, 120"),  //CLT
                              _T("10, 0,  5"),   //ADD_I1
                              _T("10, 0,  5"),   //ADD_i2
                              _T("8, 0,  24"),   //InjPW
                              _T("8,-40, 120"),  //IAT
                              _T("10,-50, 50"),  //EGO corr.
                              _T("16, 0,  16"),  //Air flow
                              _T("11, 0,  220"), //vehicle speed
                              _T("10,-500,500"), //TPSdot
                              _T("10,-500,500"), //MAPdot
                              _T("10, 0,  400"), //MAP2
                              _T("10, 0,  400"), //MAPD
                              _T("8,-40, 120"),  //Tmp2
                              _T("10, 0,  50"),  //FuelConsum
                              _T("10, 0,  30"),  //knockRetard
                              _T("10, 0,  5"),   //Knock signal
                              _T("9,  6,  24"),  //SensAFR
                              _T("10, 0,  100"), //Choke pos.
                              _T("10, 0,  100"), //GD pos.
                              _T("10, 0,  250"), //SynLoad
                              _T("16, 0,  720"), //InjTimB
                              _T("16, 0,  720"), //InjTimE
                              _T("16,-360,360"), //InjTimB1
                              _T("16,-360,360"), //InjTimE1
                              _T("20, 0,  250"), //FuelConsumF
                              _T("8,-40, 120"),  //GRTS
                              _T("8, 0,  100"),  //FTLS
                              _T("8, 0, 1100"),  //EGTS
                              _T("12, 0,  6"),   //OPS
                              _T("10, 0,  100"), //InjDuty
                              _T("10, 0,  600"), //MAF
                              _T("10, 0,  100"), //VentDuty
                              _T("8,-40, 120"),  //FTS
                              _T("10,-50, 50"),  //EGO corr. 2
                              _T("9,  6,  24"),  //SensAFR2
                              _T("9, 6,  24"),   //TargAFR
                              _T("10,-10, 10"),  //DiffAFR
                              _T("18,-10, 10"),  //DiffAFR2
                              _T("10, 0, 400"),  //GPS
                              _T("10, 0, 700")}  //FPS
                             };

 IniIO sc(IniFileName, m_Name_MetersSc_Section);
 for (int g = 0; g < 2; ++g)
 {
  int d = 0;
  sc.ReadScale(m_optMetScRPM[g],     metScDef[g][d++],  0.0f,  30000.0f, 100);
  sc.ReadScale(m_optMetScMAP[g],     metScDef[g][d++],  0.0f,  500.0f, 100);
  sc.ReadScale(m_optMetScVBat[g],    metScDef[g][d++],  0.0f,  30.0f, 100);
  sc.ReadScale(m_optMetScIgnTim[g],  metScDef[g][d++], -20.0f, 90.0f, 100);
  sc.ReadScale(m_optMetScTPS[g],     metScDef[g][d++],  0.0f,  100.0f, 100);
  sc.ReadScale(m_optMetScCLT[g],     metScDef[g][d++], -60.0f, 300.0f, 100);
  sc.ReadScale(m_optMetScAddI1[g],   metScDef[g][d++],  0.0f,  5.5f, 100);
  sc.ReadScale(m_optMetScAddI2[g],   metScDef[g][d++],  0.0f,  5.5f, 100);
  sc.ReadScale(m_optMetScInjPW[g],   metScDef[g][d++],  0.0f,  100.0f, 100);
  sc.ReadScale(m_optMetScIAT[g],     metScDef[g][d++], -60.0f, 300.0f, 100);
  sc.ReadScale(m_optMetScEGOCorr[g], metScDef[g][d++],-75.0f, 75.0f, 100);
  sc.ReadScale(m_optMetScAirFlow[g], metScDef[g][d++],  0.0f, 16.0f, 100);
  sc.ReadScale(m_optMetScVehicleSpeed[g],metScDef[g][d++], 0.0f, 300.0f, 100);
  sc.ReadScale(m_optMetScTPSDot[g],  metScDef[g][d++], -1000.0f, 1000.0f, 100);
  sc.ReadScale(m_optMetScMAPDot[g],  metScDef[g][d++], -1000.0f, 1000.0f, 100);
  sc.ReadScale(m_optMetScMAP2[g],    metScDef[g][d++], 0.0f, 500.0f, 100);
  sc.ReadScale(m_optMetScMAPD[g],    metScDef[g][d++], 0.0f, 500.0f, 100);
  sc.ReadScale(m_optMetScTmp2[g],    metScDef[g][d++], -60.0f, 300.0f, 100);
  sc.ReadScale(m_optMetScFuelConsum[g],metScDef[g][d++], 0.0f, 500.0f, 100);
  sc.ReadScale(m_optMetScKnockRetard[g],metScDef[g][d++], 0.0f, 50.0f, 100);
  sc.ReadScale(m_optMetScKnockGraph[g],metScDef[g][d++], 0.0f, 5.5f, 100);
  sc.ReadScale(m_optMetScSensAFR[g], metScDef[g][d++], 6.0f, 24.0f, 100);
  sc.ReadScale(m_optMetScChokePos[g],metScDef[g][d++], 0.0f, 100.0f, 100);
  sc.ReadScale(m_optMetScGDPos[g],   metScDef[g][d++], 0.0f, 100.0f, 100);
  sc.ReadScale(m_optMetScSynLoad[g], metScDef[g][d++], 0.0f, 500.0f, 100);
  sc.ReadScale(m_optMetScInjTimB[g], metScDef[g][d++],0.0f, 720.0f, 100);  //0...720
  sc.ReadScale(m_optMetScInjTimE[g], metScDef[g][d++],0.0f, 720.0f, 100);  //0...720
  sc.ReadScale(m_optMetScInjTimB1[g], metScDef[g][d++],-360.0f, 360.0f, 100); //-360...360
  sc.ReadScale(m_optMetScInjTimE1[g], metScDef[g][d++],-360.0f, 360.0f, 100); //-360...360
  sc.ReadScale(m_optMetScFuelConsumF[g],metScDef[g][d++], 0.0f, 500.0f, 100);
  sc.ReadScale(m_optMetScGrts[g],    metScDef[g][d++],-60.0f,300.0f, 100);
  sc.ReadScale(m_optMetScFtls[g],    metScDef[g][d++], 0.0f, 300.0f, 100);
  sc.ReadScale(m_optMetScEgts[g],    metScDef[g][d++], 0.0f, 1500.0f, 100);
  sc.ReadScale(m_optMetScOps[g],     metScDef[g][d++], 0.0f, 10.0f, 100);
  sc.ReadScale(m_optMetScInjDuty[g], metScDef[g][d++], 0.0f, 100.0f, 100);
  sc.ReadScale(m_optMetScMAF[g],     metScDef[g][d++], 0.0f, 1000.0f, 100);
  sc.ReadScale(m_optMetScVentDuty[g], metScDef[g][d++], 0.0f, 100.0f, 100);
  sc.ReadScale(m_optMetScFts[g],     metScDef[g][d++],-60.0f,200.0f, 100);
  sc.ReadScale(m_optMetScEGOCorr2[g],metScDef[g][d++], -75.0f, 75.0f, 100);
  sc.ReadScale(m_optMetScSensAFR2[g],metScDef[g][d++], 6.0f, 24.0f, 100);
  sc.ReadScale(m_optMetScTargAFR[g], metScDef[g][d++], 6.0f, 24.0f, 100);
  sc.ReadScale(m_optMetScDiffAFR[g], metScDef[g][d++], -10.0f, 10.0f, 100);
  sc.ReadScale(m_optMetScDiffAFR2[g],metScDef[g][d++], -10.0f, 10.0f, 100);
  sc.ReadScale(m_optMetScGPS[g],     metScDef[g][d++], 0.0f, 500.0f, 100);
  sc.ReadScale(m_optMetScFPS[g],     metScDef[g][d++], 0.0f, 1000.0f, 100);
 }

 //Auto tune
 IniIO at(IniFileName, m_Name_AutoTune_Section);
 at.ReadVector(m_optLambdaDelay,_T("200,70,40,250,150,100,350,300,200"),10, 3000, 9);
 at.ReadVector(m_optLambdaDelayL,_T("30,115,200"), 5, 500, 3);
 at.ReadVector(m_optLambdaDelayR,_T("500,4000,7500"), 100, 15000, 3);
 at.ReadFlt(m_optAFRError,_T("0.1"), 0.01f, 10.0f);
 at.ReadVector(m_optBlockedCells,_T(""), 0, 255, -1);
 at.ReadInt(m_optStatSize, _T("16"), 0, 64, false);
 at.ReadInt(m_optAutoBlockThrd, _T("50"), 0, 255, false);
 at.ReadInt(m_optGrowingMode, _T("0"), 0, 1);
 at.ReadFlt(m_optMinAFR,_T("8.0"), 8.0f, 22.0f);
 at.ReadFlt(m_optMaxAFR,_T("22.0"), 8.0f, 22.0f);
 at.ReadInt(m_optMinDistThrd, _T("10"), 0, 255, false);
 at.ReadFlt(m_optMinTPS,_T("0.0"), 0.0f, 100.0f);
 at.ReadFlt(m_optMaxTPS,_T("100.0"), 0.0f, 100.0f);
 at.ReadFlt(m_optCLTThrd,_T("70.0"), 0.0f, 180.0f);
 at.ReadFlt(m_optTunSoftness,_T("1.0"), 0.5f, 5.0f);
 //Map editor
 IniIO me(IniFileName, m_Name_MapEditor_Section);
 me.ReadInt(m_optGradSaturation, _T("120"), 0, 255);
 me.ReadInt(m_optGradBrightness, _T("255"), 0, 255);
 me.ReadInt(m_optBoldFont, _T("0"), 0, 1);
 me.ReadInt(m_optITEdMode, _T("0"), 0, 3);
 me.ReadInt(m_optSpotMarkers, _T("1"), 0, 1);
 me.ReadFlt(m_optSpotMarkersSize,_T("1.0"), 0.1f, 3.0f);
 me.ReadInt(m_optActiveVEMap, _T("0"), 0, 2);
 me.ReadInt(m_optUseMarkers, _T("1"), 0, 1);

 //Splitters
 IniIO sp(IniFileName, m_Name_Splitters_Section);
 sp.ReadInt(m_optParamMonVert, _T("279"), 0, 10000, true);
 sp.ReadInt(m_optLogPlayerVert, _T("243"), 0, 10000, true);

 //Inj. driver
 IniIO dr(IniFileName, m_Name_InjDrv_Section);
 dr.ReadInt(m_optInjDrvTabActive, _T("0"), 0, 1);
 dr.ReadColor(m_optVoltLineColor, _T("FFFF00"));
 dr.ReadFlt(m_optPeakOnPtMovStep, _T("1.0"), 1.0f, 10000.0f);
 dr.ReadFlt(m_optPeakDutyPtMovStep, _T("0.1"), 0.1f, 10.0f);
 dr.ReadFlt(m_optHoldDutyPtMovStep, _T("0.1"), 0.1f, 10.0f);
 dr.ReadFlt(m_optPeakFullPtMovStep, _T("1.0"), 1.0f, 10000.0f);
 dr.ReadFlt(m_optPthPausePtMovStep, _T("1.0"), 1.0f, 10000.0f);
 dr.ReadInt(m_optShowGraphLabels, _T("1"), 0, 1);
 dr.ReadFlt(m_optPWMulPtMovStep, _T("0.1"), 0.0f, 5.0f); //%
 dr.ReadFlt(m_optPWAddPtMovStep, _T("10"), 0.0f, 2500.0f); //us
 
 //Moving step of points in maps' editing windows
 IniIO ms(IniFileName, m_Name_MapPtMovStep_Section);
 ms.ReadFlt(m_optPtMovStepWorkMap,  _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepTempMap,  _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepTempIdlMap,  _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepStartMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepIdleMap,  _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepVeMap,   _T("0.05"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepVe2Map,   _T("0.05"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepVeiMap,   _T("0.05"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepAfrMap, _T("0.1"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepCrnkMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepWrmpMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepDeadMap, _T("0.1"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepIdlrMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepIdlcMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepThrassMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepAetpsMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepAemapMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepAerpmMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepAftstrMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepItMap, _T("1.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepItrpmMap, _T("10.0"), 0.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepRigidMap, _T("0.01"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepEgocrvMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepIaccMap, _T("0.05"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepIaccwMap, _T("0.05"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepIatcltMap, _T("0.05"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepTpsswtMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepGtscMap, _T("0.01"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepGpscMap, _T("0.01"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepAtscMap, _T("0.01"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepPwm1Map,  _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepPwm2Map,  _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepIACMATMap, _T("0.25"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepTpszonMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepCylMultMap, _T("0.1"), 0.005f, 1.0f);
 ms.ReadFlt(m_optPtMovStepCylAddMap, _T("0.1"), 0.025f, 1.0f);
 //separate
 ms.ReadFlt(m_optPtMovStepCrkTempMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepEHPauseMap, _T("0.01"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepAttenuatorMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepDwellCntrlMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepCTSCurveMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepBarocorrMap, _T("0.1"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepManIgntimMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepATSCurvMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepATSCorrMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepGasdoseMap, _T("0.5"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepTmp2CurveMap, _T("0.25"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepCrankingThrdMap, _T("100.0"), 0.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepCrankingTimeMap, _T("10.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepSmapabanThrdMap, _T("100.0"), 0.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepKnockZoneMap, _T("1.0"), 0.0f, 1.0f);
 ms.ReadFlt(m_optPtMovStepGrtsCurveMap, _T("0.25"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepGrHeatDutyMap, _T("5.0"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepPwmIacUCoefMap, _T("0.01"), 0.001f, 1.0f);
 ms.ReadFlt(m_optPtMovStepAftstrStrk0Map, _T("10"), 1.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepAftstrStrk1Map, _T("10"), 1.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepGrValDelMap, _T("0.1"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepFtlsCurveMap, _T("0.1"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepEgtsCurveMap, _T("5.0"), 0.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepOpsCurveMap, _T("0.1"), 0.0f, 1.0f);
 ms.ReadFlt(m_optPtMovStepManInjPwcMap, _T("0.01"), 0.0f, 0.1f);
 ms.ReadFlt(m_optPtMovStepMAFCurveMap, _T("0.01"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepFtlsCorMap, _T("0.01"), 0.001f, 1.0f);
 ms.ReadFlt(m_optPtMovStepLambdaZoneMap, _T("1.0"), 0.0f, 1.0f);
 ms.ReadFlt(m_optPtMovStepFtsCurveMap, _T("1.0"), 0.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepFuelDensCorrMap, _T("0.001"), 0.0001f, 0.1f);
 ms.ReadFlt(m_optPtMovStepXtauXfAccMap, _T("1.0"), 0.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepXtauXfDecMap, _T("1.0"), 0.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepXtauTfAccMap, _T("1.0"), 0.0f, 1000.0f);
 ms.ReadFlt(m_optPtMovStepXtauTfDecMap, _T("1.0"), 0.0f, 1000.0f);
 ms.ReadFlt(m_optPtMovStepInjNonLinPMap, _T("0.001"), 0.0001f, 0.1f);
 ms.ReadFlt(m_optPtMovStepInjNonLinGMap, _T("0.001"), 0.0001f, 0.1f);
 ms.ReadFlt(m_optPtMovStepEGODelayMap, _T("10"), 1.0f, 100.0f);
 ms.ReadFlt(m_optPtMovStepWUAFR0Map, _T("0.1"), 0.0f, 10.0f);
 ms.ReadFlt(m_optPtMovStepWUAFR1Map, _T("0.1"), 0.0f, 10.0f);

 //Log file's fileds
 IniIO lf(IniFileName, m_Name_LogFileFields_Section);
 lf.ReadInt(m_optWriteLogFields, _T("0"), 0, 1);
 lf.ReadString(m_optLogFieldTime, _T("Time"));
 lf.ReadString(m_optLogFieldRPM, _T("RPM"));
 lf.ReadString(m_optLogFieldIgnTim, _T("IgnTim"));
 lf.ReadString(m_optLogFieldMAP, _T("Map"));
 lf.ReadString(m_optLogFieldVBat, _T("VBat"));
 lf.ReadString(m_optLogFieldCLT, _T("CLT"));
 lf.ReadString(m_optLogFieldKnock, _T("Knock"));
 lf.ReadString(m_optLogFieldKnockCorr, _T("KnockCorr"));
 lf.ReadString(m_optLogFieldLoadCurve, _T("LoadCurve"));
 lf.ReadString(m_optLogFieldCarb, _T("CarbSw"));
 lf.ReadString(m_optLogFieldGas_v, _T("Gas_V"));
 lf.ReadString(m_optLogFieldIdleValve, _T("IdleValve"));
 lf.ReadString(m_optLogFieldPowerValve, _T("PowerValve"));
 lf.ReadString(m_optLogFieldCoolingFan, _T("CoolingFan"));
 lf.ReadString(m_optLogFieldStBlock, _T("StBlock"));
 lf.ReadString(m_optLogFieldAE, _T("AE"));
 lf.ReadString(m_optLogFieldFCRevLim, _T("FCRevLim"));
 lf.ReadString(m_optLogFieldFloodClear, _T("FloodClear"));
 lf.ReadString(m_optLogFieldSysLocked, _T("SysLocked"));
 lf.ReadString(m_optLogFieldCE, _T("CE"));
 lf.ReadString(m_optLogFieldIgn_i, _T("Ign_i"));
 lf.ReadString(m_optLogFieldCond_i, _T("Cond_i"));
 lf.ReadString(m_optLogFieldEpas_i, _T("Epas_i"));
 lf.ReadString(m_optLogFieldGpa4_i, _T("Gpa4_i"));
 lf.ReadString(m_optLogFieldInput1, _T("Input1"));
 lf.ReadString(m_optLogFieldInput2, _T("Input2"));
 lf.ReadString(m_optLogFieldAuto_i, _T("Auto_i"));
 lf.ReadString(m_optLogFieldMapsel0, _T("Mapsel0"));
 lf.ReadString(m_optLogFieldRefprs_i, _T("Refprs_i"));
 lf.ReadString(m_optLogFieldAltrn_i, _T("Altrn_i"));
 lf.ReadString(m_optLogFieldTPS, _T("TPS"));
 lf.ReadString(m_optLogFieldAdd_i1, _T("Add_i1"));
 lf.ReadString(m_optLogFieldAdd_i2, _T("Add_i2"));
 lf.ReadString(m_optLogFieldChokePos, _T("ChokePos"));
 lf.ReadString(m_optLogFieldGDPos, _T("GDPos"));
 lf.ReadString(m_optLogFieldVehSpeed, _T("VehSpeed"));
 lf.ReadString(m_optLogFieldPasDist, _T("PasDist"));
 lf.ReadString(m_optLogFieldFuelConsum, _T("FuelConsum"));
 lf.ReadString(m_optLogFieldFuelConsFreq, _T("FuelConsumF"));
 lf.ReadString(m_optLogFieldIAT, _T("IAT"));
 lf.ReadString(m_optLogFieldStrtIgnTim, _T("StrtIgnTim"));
 lf.ReadString(m_optLogFieldIdleIgnTim, _T("IdleIgnTim"));
 lf.ReadString(m_optLogFieldWorkIgnTim, _T("WorkIgnTim"));
 lf.ReadString(m_optLogFieldTempIgnTim, _T("TempIgnTim"));
 lf.ReadString(m_optLogFieldIATIgnTim, _T("IATIgnTim"));
 lf.ReadString(m_optLogFieldIdlRegIgnTim, _T("IdlRegIgnTim"));
 lf.ReadString(m_optLogFieldOctanCorr, _T("IgnTimCorr"));
 lf.ReadString(m_optLogFieldEGOCorr, _T("EGOcorr"));
 lf.ReadString(m_optLogFieldInjPW, _T("InjPW"));
 lf.ReadString(m_optLogFieldTPSdot, _T("TPSdot"));
 lf.ReadString(m_optLogFieldMAP2, _T("MAP2"));
 lf.ReadString(m_optLogFieldTmp2, _T("Tmp2"));
 lf.ReadString(m_optLogFieldDiffMAP, _T("DiffMAP"));
 lf.ReadString(m_optLogFieldAFR, _T("AFR"));
 lf.ReadString(m_optLogFieldSynLoad, _T("SynLoad"));
 lf.ReadString(m_optLogFieldBaroPress, _T("BaroPress"));
 lf.ReadString(m_optLogFieldInjTimBeg, _T("InjTimBeg"));
 lf.ReadString(m_optLogFieldInjTimEnd, _T("InjTimEnd"));
 lf.ReadString(m_optLogFieldLogMarks, _T("LogMarks"));
 lf.ReadString(m_optLogFieldCECodes, _T("CECodes"));
 lf.ReadString(m_optLogFieldGrts, _T("GRTS"));
 lf.ReadString(m_optLogFieldFtls, _T("FTLS"));
 lf.ReadString(m_optLogFieldEgts, _T("EGTS"));
 lf.ReadString(m_optLogFieldOps, _T("OPS"));
 lf.ReadString(m_optLogFieldAftStrEnr, _T("AftStrEnr"));
 lf.ReadString(m_optLogFieldIacClLoop, _T("IacClLoop"));
 lf.ReadString(m_optLogFieldInjDuty, _T("InjDuty"));
 lf.ReadString(m_optLogFieldRigidArg, _T("RigidArg"));
 lf.ReadString(m_optLogFieldServFlag, _T("ServFlag"));
 lf.ReadString(m_optLogFieldRxlaf, _T("Rxlaf"));
 lf.ReadString(m_optLogFieldMAF, _T("MAF"));
 lf.ReadString(m_optLogFieldVentDuty, _T("VentDuty"));
 lf.ReadString(m_optLogFieldUniOuts, _T("UnivOuts"));
 lf.ReadString(m_optLogFieldMAPdot, _T("MAPdot"));
 lf.ReadString(m_optLogFieldFts, _T("FTS"));
 lf.ReadString(m_optLogFieldConsFuel, _T("FuelConsumed"));
 lf.ReadString(m_optLogFieldEGOCorr2, _T("EGOcorr2"));
 lf.ReadString(m_optLogFieldAFR2, _T("AFR2"));
 lf.ReadString(m_optLogFieldAFRMap, _T("AFRMap"));
 lf.ReadString(m_optLogFieldTchrg, _T("Tchrg"));
 lf.ReadString(m_optLogFieldGPS, _T("GPS"));
 lf.ReadString(m_optLogFieldFPS, _T("FPS"));

 //Functionality
 IniIO fn(IniFileName, m_Name_Functionality_Section);
 fn.ReadInt(m_optFuncSM_CONTROL, _T("1"), 0, 1);
 fn.ReadInt(m_optFuncGD_CONTROL, _T("1"), 0, 1);

 return status;
}

bool CAppSettingsModel::WriteSettings(void)
{
 bool status = true;
 CString IniFileName = GetINIFileFullName();
 IniIO writer(IniFileName); //set locale, open file and start

 bool en = (m_optInterfaceLang.value == IL_ENGLISH);
 const TCHAR* strMetRPM = en ? _T("RPM, speed, distance") : _T("Обороты, скорость, расстояние");
 const TCHAR* strMetMAP = en ? _T("MAP sensor") : _T("ДАД");
 const TCHAR* strMetVBat = en ? _T("Board voltage") :_T("Напряжение бортовой сети");
 const TCHAR* strMetIgnTim = en ? _T("Ignition timing") : _T("УОЗ");                                          
 const TCHAR* strMetTPS = en ? _T("TPS sensor") : _T("ДПДЗ, расход воздуха");
 const TCHAR* strMetCLT = en ? _T("CLT sensor") : _T("ДТОЖ");
 const TCHAR* strMetAddI1 = en ? _T("ADD_I1 input") : _T("Вход ADD_I1");
 const TCHAR* strMetAddI2 = en ? _T("ADD_I2 input") : _T("Вход ADD_I2");
 const TCHAR* strMetInjPW = en ? _T("Injection pulse width") : _T("Длительность впрыска");
 const TCHAR* strMetIAT = en ? _T("MAT sensor") : _T("ДТВ");
 const TCHAR* strMetEGOCorr = en ? _T("Lambda correction") : _T("Лямбда коррекция");
 const TCHAR* strMetAirFlow = en ? _T("Air flow") : _T("Расход воздуха");
 const TCHAR* strMetVehicleSpeed = en ? _T("Vehicle spped") : _T("Скорость авто.");
 const TCHAR* strMetTPSDot = en ? _T("TPS dot") : _T("Скорость ДПДЗ (d%/dt)");
 const TCHAR* strMetMAPDot = en ? _T("MAP dot") : _T("Скорость ДАД (dP/dt)");
 const TCHAR* strMetMAP2 = en ? _T("MAP2 sensor") : _T("Датчик MAP2");
 const TCHAR* strMetGPS = en ? _T("GP sensor") : _T("Датчик ДГ");
 const TCHAR* strMetMAPD = en ? _T("Diff. pressure") : _T("Дифф. давление");
 const TCHAR* strMetTmp2 = en ? _T("TMP2 sensor") : _T("Датчик TMP2");
 const TCHAR* strMetFuelConsum = en ? _T("Fuel consumption") : _T("Расход топлива");
 const TCHAR* strMetKnockRetard = en ? _T("Knock retard") : _T("Коррекция УОЗ по ДД");
 const TCHAR* strMetKnockGraph = en ? _T("Knock signal") : _T("Сигнал детонации");
 const TCHAR* strMetSensAFR = en ? _T("AFR sensor") : _T("Воздух/топливо с ДК");
 const TCHAR* strMetChokePos = en ? _T("Choke/IAC position") : _T("Положение ВЗ/РДВ");
 const TCHAR* strMetGDPos = en ? _T("Gas valve position") : _T("Положение дозатора газа");
 const TCHAR* strMetSynLoad = en ? _T("Synthetic load") : _T("Синтетическая нагрузка");
 const TCHAR* strMetInjTimB = en ? _T("Begin phase of injection pulse") : _T("Фаза начала впрыска");
 const TCHAR* strMetInjTimE = en ? _T("End phase of injection pulse") : _T("Фаза конца впрыска");
 const TCHAR* strMetFuelConsumF = en ? _T("Fuel consumption (Hz)") : _T("Расход топлива (Гц)");
 const TCHAR* strMetGrts = en ? _T("GRTEMP sensor") : _T("Датчик GRTEMP");
 const TCHAR* strMetFtls = en ? _T("Fuel tank level") : _T("Датчик уровня топлива");
 const TCHAR* strMetEgts = en ? _T("Exhaust gas temperature") : _T("Температура выхлопных газов");
 const TCHAR* strMetOps = en ? _T("Oil pressure") : _T("Давление масла");
 const TCHAR* strMetInjDuty = en ? _T("Injector's duty") : _T("Загрузка форсунок");
 const TCHAR* strMetMAF = en ? _T("MAF sensor") : _T("Датчик ДМРВ");
 const TCHAR* strMetVentDuty = en ? _T("Cooling fan's PWM duty") : _T("Скважность ШИМ вентилятора охлаждения");
 const TCHAR* strMetFts = en ? _T("Temperature in the fuel ramp") : _T("Температура в топливной рампе");
 const TCHAR* strMetEGOCorr2 = en ? _T("Lambda correction 2") : _T("Лямбда коррекция 2");
 const TCHAR* strMetSensAFR2 = en ? _T("AFR sensor 2") : _T("Воздух/топливо с ДК 2");
 const TCHAR* strMetTargAFR = en ? _T("Target AFR") : _T("Воздух/топливо целевое");
 const TCHAR* strMetDiffAFR = en ? _T("AFR difference") : _T("Разность Воздух/топливо");
 const TCHAR* strMetDiffAFR2 = en ? _T("AFR difference2") : _T("Разность Воздух/топливо2");
 const TCHAR* strMetFPS = en ? _T("Fuel pressure sensor") : _T("Датчик давления топлива");

 IniIO &os = writer; 
 if (m_optInterfaceLang.value == IL_ENGLISH)
  VERIFY(os.WriteComment(_T("*** General settings ***"), true));
 else
  VERIFY(os.WriteComment(_T("*** Общие настройки ***"), true));
 os.CreateSection(m_Name_Options_Section);
 
 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Name of COM port or a device (e.g. /dev/ttyUSB0)"));
 else
  os.WriteComment(_T("Имя COM порта или устройства (e.g. /dev/ttyUSB0)"));
 os.WriteString(m_optPortName);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Show only really existing ports in the COM port selection drop down list"));
 else
  os.WriteComment(_T("Показывать только существующие порты в выпадающем списке COM портов"));
 os.WriteInt(m_optExistingPorts); 
 
 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("COM port speed for firmware (baud)"));
 else
  os.WriteComment(_T("Скорость COM порта для связи с прошивкой (бод)"));
 os.WriteDword(m_optBaudRateApplication);
 
 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("COM port speed for boot loader (baud)"));
 else
  os.WriteComment(_T("Скорость COM порта для связи с загрузчиком (бод)"));
 os.WriteDword(m_optBaudRateBootloader);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Write log files in binary format"));
 else
  os.WriteComment(_T("Записывать лог файл в бинарном формате"));
 os.WriteInt(m_optLogBinaryFmt); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Create all windows on start up of the programm. This will speed up the programm, but will take more memory and resources."));
 else
  os.WriteComment(_T("Создавать все окна программы в момент ее запуска. Это ускорит работу программы, но потребуется больше памяти и ресурсов."));
 os.WriteInt(m_optCreateWindows); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Full path to folder for writing CSV log files. UseAppFolder must be set to 0."));
 else
  os.WriteComment(_T("Полный путь к папке для записи CSV лог файлов. UseAppFolder должен быть установлен в 0, иначе этот параметр игнорируется"));
 os.WriteString(m_optLogFilesFolder);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("If set to 1, then log files will be created in the folder containing application. Set to 0 if you want to use LogFilesFolder"));
 else
  os.WriteComment(_T("Если установлен в 1, то лог файлы будут создаваться в папке с исполняемым файлом. Установите в 0, если вы хотите использовать LogFilesFolder"));
 os.WriteInt(m_optUseAppFolder);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Automatically write log files. Set to 0 for disabling this feature."));
 else
  os.WriteComment(_T("Автоматически записывать логи. Установите в 0 чтобы запретить эту функцию."));
 os.WriteInt(m_optAlwaysWriteLog);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Symbol used to separate fields in the CSV file. Accepted values: ;%,+|*=!#$&"));
 else
  os.WriteComment(_T("Символ, используемый для разделения полей в CSV файле. Допустимые значения: ;%,+|*=!#$&"));
 os.WriteChar(m_optCSVSepSymbol);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Symbol used to separate fields in the Map CSV file. Accepted values: ;%,+|*=!#$&"));
 else
  os.WriteComment(_T("Символ, используемый для разделения полей в CSV файлах таблиц. Допустимые значения: ;%,+|*=!#$&"));
 os.WriteChar(m_optMapCSVSepSymbol);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Specifies how frequently virtual gauges and indicators will be updated (redraw). Value in the milliseconds"));
 else
  os.WriteComment(_T("Задает период обновления панели приборов и индикаторов (перерисовка). Значение в миллисекундах"));
 os.WriteInt(m_optMIDeskUpdatePeriod);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Interface language. Set to english for English and set to russian for Russian. Default is english."));
 else
  os.WriteComment(_T("Язык интерфейса. Установите в english для Английского или в russian для Русского. По умолчанию english."));
 os.WriteEnum(m_optInterfaceLang, m_AllowableLanguages);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Specify model of processor of your SECU-3 unit. Supported values are: atmega64, atmega128, atmega644, atmega1284."));
 else
  os.WriteComment(_T("Укажите модель просессора вашего блока SECU-3. Поддерживаемые значения: atmega64, atmega128, atmega644, atmega1284."));
 os.WriteEnum(m_optECUPlatformType, m_AllowablePlatforms);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Use debug features (for developers). If set to 1, the small floating window will be displayed inside the main window. See also DVDeskUpdatePeriod."));
 else
  os.WriteComment(_T("Использовать средства отладки (для разработчиков). Если установлено в 1, внутри главного окна будет отображаться небольшое плавающее окно. См. также DVDeskUpdatePeriod."));
 os.WriteInt(m_optUseDVFeatures);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Write values of debug variables into a file. File 'dbgvar.csv' will be placed in the directory of programm."));
 else
  os.WriteComment(_T("Писать значения отладочных переменных в файл. Файл 'dbgvar.csv' будет находиться в папке с файлами программы."));
 os.WriteInt(m_optDbgVarsToFile); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Specifies how frequently debug floating window will be updated (redraw). Value in the milliseconds"));
 else
  os.WriteComment(_T("Задает период обновления плавающего окна отладки (перерисовка). Значение в миллисекундах"));
 os.WriteInt(m_optDVDeskUpdatePeriod);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Controls displaying of tool tips (hints). Set to 0 if you want to disable showing of tool tips."));
 else
  os.WriteComment(_T("Управление отображением всплывающих подсказок. Установите в 0, если вы не хотите чтобы отображались всплывающие подсказки."));
 os.WriteInt(m_optShowToolTips); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Set to 0 if you don't want to be bothred by error messages related to COM port. Recommended only for experienced users."));
 else
  os.WriteComment(_T("Установите в 0, если вас раздражают сообщения об ошибках COM порта (не будут показываться). Рекомендуется только для опытных пользователей."));
 os.WriteInt(m_optCOMPortBother); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Use or not to use hot keys. Set to 0 if you want to disable hot keys."));
 else
  os.WriteComment(_T("Использовать горячие клавиши. Установите в 0, если вы хотите запретить горячие клавиши."));
 os.WriteInt(m_optUseHotKeys); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Show welcome"));
 else
  os.WriteComment(_T("Показывать приветствие"));
 m_optShowWelcome.value = 0; //<--Will be not shown next time
 os.WriteInt(m_optShowWelcome); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Allow visual theme (appeared since Windows XP). Set to 0 if you want ot disable using of visual themes."));
 else
  os.WriteComment(_T("Разрешить визуальные темы (появились в Windows XP). Установите в 0, если вы хотите запретить визуальные темы."));
 os.WriteInt(m_optAllowVisualTheme); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Set to 1 if you want to automatically enter diagnostic mode after opening of \"Diagnostics\" tab. For experienced users only."));
 else
  os.WriteComment(_T("Установите в 1, если вы хотите, чтобы вход в режим диагностики производился автоматически при открытии вкладки \"Диагностика\". Только для опытных пользователей."));
 os.WriteInt(m_optAutoDiagEnter);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Set to 0 if you don't want to be bothered by warning messages related to saving of files."));
 else
  os.WriteComment(_T("Установите в 0, если вас раздражают сообщения, предупреждающие о несохраненных файлах."));
 os.WriteInt(m_optSaveWarning); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Set to 1 if you want to automatically read CE errors from firmware after opening of \"Check engine\" tab."));
 else
  os.WriteComment(_T("Установите в 1, если вы хотите, чтобы коды ошибок автоматически считывались при открытии вкладки \"Check Engine\"."));
 os.WriteInt(m_optAutoCERead); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Enable/disable children windows of 2D and 3D chards. If set to 1, then mentioned windows will belong to main window (parent). Set to 0 and windows will belong to desktop."));
 else
  os.WriteComment(_T("Разрешить/запретить дочерние окна 2D и 3D графиков таблиц. Установите в 0 и эти плавающие окна будут принадлежать рабочему столу, а не главному окну программы."));
 os.WriteInt(m_optChildCharts); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Automatically reopen COM port after plugging of USB cable or Bluetooth. Set to 0 to disable this feature."));
 else
  os.WriteComment(_T("Автоматически переоткрывать COM порт после подключения по USB кабелю или Bluetooth. Установите в 0 для запрещения этой функции."));
 os.WriteInt(m_optPortAutoReopen); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("When set to 1, only one 2D or 3D chart window will be displayed at a time (buttons will be toggled). By default is set to 0 (disabled)."));
 else
  os.WriteComment(_T("Когда установлено в 1, только одно окно 2D или 3D графиков таблиц будет отображаться одновременно (кнопки будут переключаться). По умолчанию установленов в 0 (отключено)."));
 os.WriteInt(m_optToggleMapWnd); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("When set to 1, windows of maps will be embedded into free space of main window (only for 'Firmware data' and 'EEPROM' tabs). By default is set to 0 (disabled)."));
 else
  os.WriteComment(_T("Когда установлено в 1, окна таблиц будут встраиваться в свободное пространство главного окна (только для вкладок 'Данные прошивки' и 'EEPROM'). По умолчанию установленов в 0 (отключено)."));
 os.WriteInt(m_optEmbedMapWnd); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("The time that the tooltips are displayed. Increase if you do not have time to read the tips"));
 else
  os.WriteComment(_T("Время, на протяжении которого отображаются всплывающие подсказки. Увеличьте, если не успеваете читать подсказки"));
 os.WriteInt(m_optToolTipTime); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Control of syntax highlighting in program settings file editor. 0 - do not highlight, 1 - highlight."));
 else
  os.WriteComment(_T("Управление подсветкой синтаксиса в редакторе файла настроек программы. 0 - не подсвечивать, 1 - подсвечивать."));
 os.WriteInt(m_optIniEdSyntax); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Use block data transfer mode for communication with boot loader when reading/writing EEPROM. 0 - don't use block data transfer, 1 - use block data transfer."));
 else
  os.WriteComment(_T("Использовать блочный режим обмена данными с загрузчиком при чтении/записи EEPROM. 0 - не использовать блочный режим, 1 - использовать блочный режим."));
 os.WriteInt(m_optBldrEEPROMBlocks); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Density of fuel 1 (e.g. petrol), g/cc. Used when GAS_V = 0. Take into account! For the new density value to take effect, you must change the value of one or more parameters on the Injection tab!"));
 else
  os.WriteComment(_T("Плотность топлива 1 (бензин), г/см3. Используется когда GAS_V = 0. Обратите внимание! Чтобы новое значение плотности вступило в силу, необходимо изменить значение одного или нескольких параметров на вкладке Впрыск!"));
 os.WriteFlt(m_optFuelDensity1, 4);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Density of fuel 2 (e.g. petrol), g/cc. Used when GAS_V = 1. Take into account! For the new density value to take effect, you must change the value of one or more parameters on the Injection tab!"));
 else
  os.WriteComment(_T("Плотность топлива 2 (бензин), г/см3. Используется когда GAS_V = 1. Обратите внимание! Чтобы новое значение плотности вступило в силу, необходимо изменить значение одного или нескольких параметров на вкладке Впрыск!"));
 os.WriteFlt(m_optFuelDensity2, 4);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  os.WriteComment(_T("Use 'up' and 'down' arrow keys for changing values in 2D mode of table editor."));
 else
  os.WriteComment(_T("Использовать клавиши 'вверх' и 'вниз' для изменения значений в 2D режиме редактора таблиц"));
 os.WriteInt(m_optClassic2DKeys); 

 IniIO &fs = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("*** General settings for virtual fixtures and indicators ***"), true);
 else
  fs.WriteComment(_T("*** Общие настройки для виртуальных приборов и индикаторов ***"), true);
 fs.CreateSection(m_Name_Fixtures_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Switch between two separately configured profiles of virtual gauges and indicators (See Meters and MetersEx sections, see also Indicators and IndicatorsEx). Allowed values: 0 and 1"));
 else
  fs.WriteComment(_T("Переключение между двумя независимо конфигурируемыми профилями приборов и индикаторов (См. секции Meters и MetersEx, см. также Indicators и IndicatorsEx). Допустимые значения: 0 или 1"));
 fs.WriteInt(m_optShowExFixtures); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Display vehicle spped and passed distance in uppers corners of the 'Speed' gauge. Allowed values: 0 and 1"));
 else
  fs.WriteComment(_T("Отображать скорость и пройденное расстояние в верхних углах прибора 'Обороты'. Допустимые значения: 0 или 1"));
 fs.WriteInt(m_optShowSpeedAndDist); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for RPM values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений оборотов. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optRPMAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for voltage values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений напряжения борт.сети. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optVoltAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for MAP values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДАД. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optMAPAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for ADD_I1 values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ADD_I1. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optAddI1Average); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for TPS values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДПДЗ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optTPSAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for knock values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений сигнала детонации. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optKnockAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for ignition timing values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений УОЗ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optIgnTimAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for CLT values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДТОЖ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optCLTAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for ADD_I2 values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ADD_I2. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optAddI2Average); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for inj. PW values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений длит. впрыска. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optInjPWAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for IAT values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДТВ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optIATAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for lambda correction values (sensors 1 and 2). Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений коррекции по ДК1 и ДК2. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optEGOCorrAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for air flow values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений расхода воздуха. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optAirFlowAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for VSS values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений скорости автомобиля. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optVehicleSpeedAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for TPS dot values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений скорости изменения положения ДЗ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optTPSDotAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for MAP dot values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений скорости изменения ДАД. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optMAPDotAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for MAP2 values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДАД2. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optMAP2Average); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for GPS values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДДГ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optGPSAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for diff. pressure values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений дифф. давления. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optMAPDAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for gas temperature values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДТГ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optTmp2Average); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for fuel consumption values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений расхода топлива. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optFuelConsumAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for knock retard values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений коррекции УОЗ по детонации. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optKnockRetardAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for AFR values 1 and 2. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений состава смеси 1 и 2. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optSensAFRAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for choke position values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений положения ВЗ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optChokePosAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for gas dispenser position values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений положения газового дозатора. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optGDPosAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for synthetic load values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений синтетической нагрузки. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optSynLoadAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for begin inj. timing values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений фазы начала впрыска. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optInjTimBAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for end inj. timing values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений фазы конца впрыска. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optInjTimEAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for MAF values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДМРВ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optMAFAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for AFR value from map (target AFR). Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений состава смеси из таблицы (целевое значение). Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optTargAFRAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for FPS values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДДТ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optFPSAverage); 

 //---------------------------
 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Defines which unit should be used for displaying of vehicle speed and distance. Supported values are: kmh and mph"));
 else
  fs.WriteComment(_T("Указывает в каких единицах измерения должна отображаеться скорость и расстояние. Допустимые значения: kmh and mph"));
 fs.WriteEnum(m_optSpeedUnit, m_AllowableSpeedUnits);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of font used for displaying of titles in virtual gauges. Default is 100% (normal traditional size)"));
 else
  fs.WriteComment(_T("Размер шрифта, используемого для отображения заголовков виртуальных приборов. По умолчанию 100% (традиционный размер)"));
 fs.WriteInt(m_optTitleFontSize); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of font used for displaying of digital values at the bottom of virtual gauges. Default is 130%"));
 else
  fs.WriteComment(_T("Размер шрифта, используемого для отображения цифровых значений в виртуальных приборах. По умолчанию 100% (традиционный размер)"));
 fs.WriteInt(m_optValueFontSize); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of font used for displaying of digital values at the left-top and right-top corners of virtual gauges. Default is 100%"));
 else
  fs.WriteComment(_T("Размер шрифта, используемого для отображения цифровых значений в левом-верхнем и правом-верхнем углах приборов. По умолчанию 100% (традиционный размер)"));
 fs.WriteInt(m_optPaneFontSize); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of font used for displaying of grid labels and units in virtual gauges. Default is 100%"));
 else
  fs.WriteComment(_T("Размер шрифта, используемого для отображения надписей и единиц измерений в виртуальных приборах. По умолчанию 100% (традиционный размер)"));
 fs.WriteInt(m_optLabelFontSize); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Enable drag and drop of virtual gauges. Enabled by default, set to 0 if you want to disable this function."));
 else
  fs.WriteComment(_T("Разрешить перетаскивание виртуальных приборов. По умолчанию разрешено. Установите в 0, если хотите запретить."));
 fs.WriteInt(m_optMetersDragNDrop); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Enable drag and drop of virtual indicators. Enabled by default, set to 0 if you want to disable this function."));
 else
  fs.WriteComment(_T("Разрешить перетаскивание виртуальных индикаторов. По умолчанию разрешено. Установите в 0, если хотите запретить."));
 fs.WriteInt(m_optIndicatorsDragNDrop); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Constant (num. of pulses per 1L of burnt fuel) for calculation of fuel consumption (L/100km) using value send by firmware."));
 else
  fs.WriteComment(_T("Константа (число импульсов на 1Л сгоревшего топлива), используемая при вычислении расхода топлива (Л/100км) на основе значения, передаваемого прошивкой."));
 fs.WriteInt(m_optFFFConst); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Display cursor on graphics in the log player. 0 - don't display, 1 - display"));
 else
  fs.WriteComment(_T("Отображать курсор на графиках в проигрывателе. 0 - не отображать, 1 - отображать"));
 fs.WriteInt(m_optShowGraphCursor); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Number of pixels which graphs shifted for when adding each new sample"));
 else
  fs.WriteComment(_T("Кол-во пикселей, на которое сдвигаются графики при добавлени новой выборки"));
 fs.WriteInt(m_optGraphShtPixels); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Show digital value on graphs. 0 - don't show value (default), 1 - show value"));
 else
  fs.WriteComment(_T("Показывать числовые значения на графиках. 0 - не показывать (по умолчанию), 1 - показывать"));
 fs.WriteInt(m_optGraphShowValue); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Font height of digital value on graphs. Value of height in %"));
 else
  fs.WriteComment(_T("Высота шрифта числовых значений на графиках. Значение высоты в %"));
 fs.WriteInt(m_optGraphValueHeight); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for fuel consumption values (Hz). Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений расхода топлива (Гц). Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optFuelConsumFAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for gas reducer's temperature values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений температуры газового редуктора. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optGrtsAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for FTLS values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДУТ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optFtlsAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for EGTS values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДТВГ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optEgtsAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for OPS values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДДМ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optOpsAverage); 

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fs.WriteComment(_T("Size of the moving average filter used for FTS values. Set to non-zero value if you want avaraging to be performed in the SECU-3 Manager."));
 else
  fs.WriteComment(_T("Размер ядра фильтра \"скользящее среднее\" используемого для усреднения значений ДТТ. Установите значение больше 0, если вы хотите, чтобы усреднение производилось в SECU-3 Manager."));
 fs.WriteInt(m_optFtsAverage); 

 //Positions of windows
 IniIO &ws = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteComment(_T("*** Remembered positions of floating windows on the screen (opened from the \"Firmware data\" and \"EEPROM\" tabs), including position of the main window ***"), true);
 else
  ws.WriteComment(_T("*** Запомненные положения плавающих окон на экране (открывающихся с вкладок \"Данные прошивки\" и \"EEPROM\"), включая положение главного окна ***"), true);
 ws.CreateSection(m_Name_WndSettings_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optMainFrmWnd, _T("Main window"));
 else
  ws.WriteWndPos(m_optMainFrmWnd, _T("Главное окно"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optStrtMapWnd, _T("Cranking ignition timing map"));
 else
  ws.WriteWndPos(m_optStrtMapWnd, _T("Таблица УОЗ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optIdleMapWnd, _T("Idling ignition timing map"));
 else
  ws.WriteWndPos(m_optIdleMapWnd, _T("Таблица УОЗ на ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optWorkMapWnd, _T("Work ignition timing map"));
 else
  ws.WriteWndPos(m_optWorkMapWnd, _T("Рабочая карта УОЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optTempMapWnd, _T("Ign. timing vs CLT correction map"));
 else
  ws.WriteWndPos(m_optTempMapWnd, _T("Коррекция УОЗ по ДТОЖ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optTempIdlMapWnd, _T("Ign. timing vs CLT correction map (idling)"));
 else
  ws.WriteWndPos(m_optTempIdlMapWnd, _T("Коррекция УОЗ по ДТОЖ (для ХХ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optGridMapIgnWnd, _T("Grid editing window for ign.timing maps"));
 else
  ws.WriteWndPos(m_optGridMapIgnWnd, _T("Редактирование в виде таблиц (зажигание)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optGridMapInjWnd, _T("Grid editing window for injection maps"));
 else
  ws.WriteWndPos(m_optGridMapInjWnd, _T("Редактирование в виде таблиц (впрыск)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optVEMapWnd, _T("VE map"));
 else
  ws.WriteWndPos(m_optVEMapWnd, _T("Наполнение (объемный КПД)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optVE2MapWnd, _T("VE map 2"));
 else
  ws.WriteWndPos(m_optVE2MapWnd, _T("Наполнение (объемный КПД) 2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optVEIMapWnd, _T("VE map for idling"));
 else
  ws.WriteWndPos(m_optVEIMapWnd, _T("Наполнение (объемный КПД) для ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAFRMapWnd, _T("AFR map"));
 else
  ws.WriteWndPos(m_optAFRMapWnd, _T("Соотношение Воздух/топливо"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optCrnkMapWnd, _T("Injection PW on cranking"));
 else
  ws.WriteWndPos(m_optCrnkMapWnd, _T("Длительность впрыска на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optWrmpMapWnd, _T("Warmup enrichment map"));
 else
  ws.WriteWndPos(m_optWrmpMapWnd, _T("Обогащение при прогреве"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optDeadMapWnd, _T("Injector's lag map"));
 else
  ws.WriteWndPos(m_optDeadMapWnd, _T("Лаг форсунки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optIdlrMapWnd, _T("IAC position (working)"));
 else
  ws.WriteWndPos(m_optIdlrMapWnd, _T("Положение РДВ (рабочее)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optIdlcMapWnd, _T("IAC position (cranking)"));
 else
  ws.WriteWndPos(m_optIdlcMapWnd, _T("Положение РДВ (на пуске)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optThrassMapWnd, _T("Throttle assist"));
 else
  ws.WriteWndPos(m_optThrassMapWnd, _T("Сопроводитель дросселя"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAETPSMapWnd, _T("Enrichment vs throttle speed map"));
 else
  ws.WriteWndPos(m_optAETPSMapWnd, _T("Обогащение по скорости ДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAEMAPMapWnd, _T("Enrichment vs MAPdot map"));
 else
  ws.WriteWndPos(m_optAEMAPMapWnd, _T("Обогащение по скорости ДАД"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAERPMMapWnd, _T("Enrichment vs engine speed map"));
 else
  ws.WriteWndPos(m_optAERPMMapWnd, _T("Обогащение по оборотам"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAftstrMapWnd, _T("Afterstart enrichment map"));
 else
  ws.WriteWndPos(m_optAftstrMapWnd, _T("Обогащение после пуска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optITMapWnd, _T("Injection timing map"));
 else
  ws.WriteWndPos(m_optITMapWnd, _T("Фаза впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optITRPMMapWnd, _T("Target idling RPM"));
 else
  ws.WriteWndPos(m_optITRPMMapWnd, _T("Целевые обороты ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optRigidMapWnd, _T("Idling regulator's rigidity map"));
 else
  ws.WriteWndPos(m_optRigidMapWnd, _T("Жесткость РХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optEGOCrvMapWnd, _T("EGO sensor curve"));
 else
  ws.WriteWndPos(m_optEGOCrvMapWnd, _T("Кривая датчика кислорода"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optIACCMapWnd, _T("Mixture correction vs IAC pos"));
 else
  ws.WriteWndPos(m_optIACCMapWnd, _T("Коррекция смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optIACCWMapWnd, _T("Weight of mixture correction vs IAC pos"));
 else
  ws.WriteWndPos(m_optIACCWMapWnd, _T("Вес коррекции смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optIATCLTMapWnd, _T("CLT to MAT influence factor"));
 else
  ws.WriteWndPos(m_optIATCLTMapWnd, _T("Коэффициент влияния ТОЖ на ТВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optTpsswtMapWnd, _T("TPS switch point map"));
 else
  ws.WriteWndPos(m_optTpsswtMapWnd, _T("Точка переключения по ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optGtscMapWnd, _T("Gas temperature correction map"));
 else
  ws.WriteWndPos(m_optGtscMapWnd, _T("Коррекция смеси по температуре газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optGpscMapWnd, _T("Gas pressure correction map"));
 else
  ws.WriteWndPos(m_optGpscMapWnd, _T("Коррекция смеси по давлению газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAtscMapWnd, _T("Air density correction map"));
 else
  ws.WriteWndPos(m_optAtscMapWnd, _T("Коррекция смеси по плотности воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optPwm1MapWnd, _T("PWM1 duty map"));
 else
  ws.WriteWndPos(m_optPwm1MapWnd, _T("Карта скважности ШИМ на выходе PWM1"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optPwm2MapWnd, _T("PWM2 duty map"));
 else
  ws.WriteWndPos(m_optPwm2MapWnd, _T("Карта скважности ШИМ на выходе PWM2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optIACMATMapWnd, _T("IAC position's correction vs MAT map"));
 else
  ws.WriteWndPos(m_optIACMATMapWnd, _T("Коррекция положения РДВ от температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optTpszonMapWnd, _T("Load axis allocation between MAP and TPS"));
 else
  ws.WriteWndPos(m_optTpszonMapWnd, _T("Распределение оси нагрузки между ДАД и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optCylMultMapWnd, _T("Per cylinder injection PW multiplier"));
 else
  ws.WriteWndPos(m_optCylMultMapWnd, _T("Поцилиндровый множитель длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optCylAddMapWnd, _T("Per cylinder injection PW addition"));
 else
  ws.WriteWndPos(m_optCylAddMapWnd, _T("Поцилиндровая добавка к длительности впрыска"));

 //group of separate maps
 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAttenuatorMapWnd, _T("Attenuator's curve of amplification"));
 else
  ws.WriteWndPos(m_optAttenuatorMapWnd, _T("Таблица функции аттенюатора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optDwellCntrlMapWnd, _T("Dwell time map"));
 else
  ws.WriteWndPos(m_optDwellCntrlMapWnd, _T("Время накопления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optCTSCurveMapWnd, _T("Coolant temperature sensor table"));
 else
  ws.WriteWndPos(m_optCTSCurveMapWnd, _T("Кривая датчика температуры охлажд.житкости"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optATSCurvMapWnd, _T("MAT sensor table"));
 else
  ws.WriteWndPos(m_optATSCurvMapWnd, _T("Кривая датчика температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optATSCorrMapWnd, _T("Ign.timing correction vs MAT map"));
 else
  ws.WriteWndPos(m_optATSCorrMapWnd, _T("Коррекция УОЗ по температуре воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optGasdoseMapWnd, _T("Gas valve position vs RPM,TPS map"));
 else
  ws.WriteWndPos(m_optGasdoseMapWnd, _T("Положение дозатора газа от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optBarocorrMapWnd, _T("Barometric correction map"));
 else
  ws.WriteWndPos(m_optBarocorrMapWnd, _T("Барометрическая коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optManIgntimMapWnd, _T("Manual ignition timing corr. map"));
 else
  ws.WriteWndPos(m_optManIgntimMapWnd, _T("Ручная октан-коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optCESettingsWnd, _T("CE settings window"));
 else
  ws.WriteWndPos(m_optCESettingsWnd, _T("Настройки СЕ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optTmp2CurveMapWnd, _T("TMP2 sensor table"));
 else
  ws.WriteWndPos(m_optTmp2CurveMapWnd, _T("Кривая датчика температуры на входе TMP2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optCrkTempMapWnd, _T("Ign.timing vs CLT correction on cranking"));
 else
  ws.WriteWndPos(m_optCrkTempMapWnd, _T("Коррекция УОЗ по ДТОЖ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optEHPauseMapWnd, _T("EGO heater's pause map (for PWM)"));
 else
  ws.WriteWndPos(m_optEHPauseMapWnd, _T("Время паузы подогрева ДК (для ШИМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optCrankingThrdMapWnd, _T("Starter's blocking RPM"));
 else
  ws.WriteWndPos(m_optCrankingThrdMapWnd, _T("Обороты блокировки стартера"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optCrankingTimeMapWnd, _T("Starter's blocking delay (strokes)"));
 else
  ws.WriteWndPos(m_optCrankingTimeMapWnd, _T("Время до блокировки стартера после превышения порога оборотов (такты)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optSmapabanThrdMapWnd, _T("RPM threshold for abandon from start map"));
 else
  ws.WriteWndPos(m_optSmapabanThrdMapWnd, _T("Обороты перехода с пусковой карты"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optKnockZoneMapWnd, _T("Knock zones vs RPM,TPS map"));
 else
  ws.WriteWndPos(m_optKnockZoneMapWnd, _T("Зоны детонации в зависимости от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optGrtsCurveMapWnd, _T("Gas reducer's temperature sensor table"));
 else
  ws.WriteWndPos(m_optGrtsCurveMapWnd, _T("Кривая датчика температуры на входе GRTEMP"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optGrHeatDutyMapWnd, _T("PWM duty map for controlling of gas reducer's heater"));
 else
  ws.WriteWndPos(m_optGrHeatDutyMapWnd, _T("Скважность ШИМ для управления нагревателем газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optPwmIacUCoefMapWnd, _T("PWM IAC duty coefficient vs voltage"));
 else
  ws.WriteWndPos(m_optPwmIacUCoefMapWnd, _T("Коэффициент скважности ШИМ РДВ от напряжения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAftstrStrk0MapWnd, _T("After start enrichment strokes vs coolant temperature (petrol)"));
 else
  ws.WriteWndPos(m_optAftstrStrk0MapWnd, _T("Число тактов обогащения после пуска от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optAftstrStrk1MapWnd, _T("After start enrichment strokes vs coolant temperature (gas)"));
 else
  ws.WriteWndPos(m_optAftstrStrk1MapWnd, _T("Число тактов обогащения после пуска от температуры двигателя (gas)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optGrValDelMapWnd, _T("Gas valve opening delay vs gas reducer's temperature map"));
 else
  ws.WriteWndPos(m_optGrValDelMapWnd, _T("Задержка включения газового клапана от температуры газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optFtlsCurveMapWnd, _T("Fuel tank level sensor's table"));
 else
  ws.WriteWndPos(m_optFtlsCurveMapWnd, _T("Кривая датчика уровня топлива (ДУТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optEgtsCurveMapWnd, _T("Exhaust gas temperature sensor's table"));
 else
  ws.WriteWndPos(m_optEgtsCurveMapWnd, _T("Кривая датчика температуры выхлопных газов (ДТВГ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optOpsCurveMapWnd, _T("Oil pressure sensor's table"));
 else
  ws.WriteWndPos(m_optOpsCurveMapWnd, _T("Кривая датчика давления масла (ДДМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optManInjPwcMapWnd, _T("Map for manual correction of injection PW"));
 else
  ws.WriteWndPos(m_optManInjPwcMapWnd, _T("Таблица для ручной коррекции длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optMAFCurveMapWnd, _T("MAF flow curve"));
 else
  ws.WriteWndPos(m_optMAFCurveMapWnd, _T("Кривая расхода ДМРВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optFtlsCorMapWnd, _T("Fuel tank level sensor's correction vs board voltage table"));
 else
  ws.WriteWndPos(m_optFtlsCorMapWnd, _T("Таблица коррекции датчика уровня топлива (ДУТ) от напряжения бортовой сети"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optLambdaZoneMapWnd, _T("EGO correction zones vs RPM,load map"));
 else
  ws.WriteWndPos(m_optLambdaZoneMapWnd, _T("Зоны лямбда-коррекции в зависимости от оборотов и нагрузки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optFtsCurveMapWnd, _T("Fuel temperature sensor's table"));
 else
  ws.WriteWndPos(m_optFtsCurveMapWnd, _T("Кривая датчика температуры топлива (ДТТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optFuelDensCorrMapWnd, _T("Fuel density correction map"));
 else
  ws.WriteWndPos(m_optFuelDensCorrMapWnd, _T("Таблица коррекции плотности топлива"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optXtauXfAccMapWnd, _T("Coefficient of injected fuel which goes into port wall's film (acceleration)"));
 else
  ws.WriteWndPos(m_optXtauXfAccMapWnd, _T("Коэффициент топлива выпадающего в пленку для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optXtauXfDecMapWnd, _T("Coefficient of injected fuel which goes into port wall's film (deceleration)"));
 else
  ws.WriteWndPos(m_optXtauXfDecMapWnd, _T("Коэффициент топлива выпадающего в пленку для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optXtauTfAccMapWnd, _T("Constant of fuel's film dissipation time (accelaration)"));
 else
  ws.WriteWndPos(m_optXtauTfAccMapWnd, _T("Постоянная времени испарения плeнки для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optXtauTfDecMapWnd, _T("Constant of fuel's film dissipation time (decelaration)"));
 else
  ws.WriteWndPos(m_optXtauTfDecMapWnd, _T("Постоянная времени испарения плeнки для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optInjNonLinPMapWnd, _T("Injector's non-linearity correction map used on small inj. PWs (petrol)"));
 else
  ws.WriteWndPos(m_optInjNonLinPMapWnd, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optInjNonLinGMapWnd, _T("Injector's non-linearity correction map used on small inj. PWs (gas)"));
 else
  ws.WriteWndPos(m_optInjNonLinGMapWnd, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (газ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optEGODelayMapWnd, _T("Number of strokes of EGO delay vs engine load"));
 else
  ws.WriteWndPos(m_optEGODelayMapWnd, _T("Число тактов задержки ДК в зависимости от нагрузки на двигатель"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optWUAFR0MapWnd, _T("AFR vs coolant temperature (petrol)"));
 else
  ws.WriteWndPos(m_optWUAFR0MapWnd, _T("Соотношение Воздух/топливо от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws.WriteWndPos(m_optWUAFR1MapWnd, _T("AFR vs coolant temperature (gas)"));
 else
  ws.WriteWndPos(m_optWUAFR1MapWnd, _T("Соотношение Воздух/топливо от температуры двигателя (gas)"));

 //Positions of windows
 IniIO &ws1 = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteComment(_T("*** Remembered positions of floating windows on the screen (opened from the \"Parameters and monitor\" tab) ***"), true);
 else
  ws1.WriteComment(_T("*** Запомненные положения плавающих окон на экране (открываются с вкладки \"Параметры и монитор\") ***"), true);
 ws1.CreateSection(m_Name_WndSettings_Section1);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optStrtMapWnd1, _T("Cranking ignition timing map"));
 else
  ws1.WriteWndPos(m_optStrtMapWnd1, _T("Таблица УОЗ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optIdleMapWnd1, _T("Idling ignition timing map"));
 else
  ws1.WriteWndPos(m_optIdleMapWnd1, _T("Таблица УОЗ на ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optWorkMapWnd1, _T("Work ignition timing map"));
 else
  ws1.WriteWndPos(m_optWorkMapWnd1, _T("Рабочая карта УОЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optTempMapWnd1, _T("Ign. timing vs CLT correction map"));
 else
  ws1.WriteWndPos(m_optTempMapWnd1, _T("Коррекция УОЗ по ДТОЖ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optTempIdlMapWnd1, _T("Ign. timing vs CLT correction map (idling)"));
 else
  ws1.WriteWndPos(m_optTempIdlMapWnd1, _T("Коррекция УОЗ по ДТОЖ (для ХХ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optGridMapIgnWnd1, _T("Grid editing window for ign.timing maps"));
 else
  ws1.WriteWndPos(m_optGridMapIgnWnd1, _T("Редактирование в виде таблиц (зажигание)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optGridMapInjWnd1, _T("Grid editing window for injection maps"));
 else
  ws1.WriteWndPos(m_optGridMapInjWnd1, _T("Редактирование в виде таблиц (впрыск)"));
 
 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optVEMapWnd1, _T("VE map"));
 else
  ws1.WriteWndPos(m_optVEMapWnd1, _T("Наполнение (объемный КПД)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optVE2MapWnd1, _T("VE map 2"));
 else
  ws1.WriteWndPos(m_optVE2MapWnd1, _T("Наполнение (объемный КПД) 2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optVEIMapWnd1, _T("VE map for idling"));
 else
  ws1.WriteWndPos(m_optVEIMapWnd1, _T("Наполнение (объемный КПД) для ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAFRMapWnd1, _T("AFR map"));
 else
  ws1.WriteWndPos(m_optAFRMapWnd1, _T("Соотношение Воздух/топливо"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optCrnkMapWnd1, _T("Injection PW on cranking"));
 else
  ws1.WriteWndPos(m_optCrnkMapWnd1, _T("Длительность впрыска на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optWrmpMapWnd1, _T("Warmup enrichment map"));
 else
  ws1.WriteWndPos(m_optWrmpMapWnd1, _T("Обогащение при прогреве"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optDeadMapWnd1, _T("Injector's lag map"));
 else
  ws1.WriteWndPos(m_optDeadMapWnd1, _T("Лаг форсунки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optIdlrMapWnd1, _T("IAC position (working)"));
 else
  ws1.WriteWndPos(m_optIdlrMapWnd1, _T("Положение РДВ (рабочее)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optIdlcMapWnd1, _T("IAC position (cranking)"));
 else
  ws1.WriteWndPos(m_optIdlcMapWnd1, _T("Положение РДВ (на пуске)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optThrassMapWnd1, _T("Throttle assist"));
 else
  ws1.WriteWndPos(m_optThrassMapWnd1, _T("Сопроводитель дросселя"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAETPSMapWnd1, _T("Enrichment vs throttle speed map"));
 else
  ws1.WriteWndPos(m_optAETPSMapWnd1, _T("Обогащение по скорости ДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAEMAPMapWnd1, _T("Enrichment vs MAPdot map"));
 else
  ws1.WriteWndPos(m_optAEMAPMapWnd1, _T("Обогащение по скорости ДАД"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAERPMMapWnd1, _T("Enrichment vs engine speed"));
 else
  ws1.WriteWndPos(m_optAERPMMapWnd1, _T("Обогащение по оборотам"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAftstrMapWnd1, _T("Afterstart enrichment map"));
 else
  ws1.WriteWndPos(m_optAftstrMapWnd1, _T("Обогащение после пуска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optITMapWnd1, _T("Injection timing map"));
 else
  ws1.WriteWndPos(m_optITMapWnd1, _T("Фаза впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optITRPMMapWnd1, _T("Target idling RPM"));
 else
  ws1.WriteWndPos(m_optITRPMMapWnd1, _T("Целевые обороты ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optRigidMapWnd1, _T("Idling regulator's rigidity map"));
 else
  ws1.WriteWndPos(m_optRigidMapWnd1, _T("Жесткость РХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optEGOCrvMapWnd1, _T("EGO sensor curve"));
 else
  ws1.WriteWndPos(m_optEGOCrvMapWnd1, _T("Кривая датчика кислорода"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optIACCMapWnd1, _T("Mixture correction vs IAC pos"));
 else
  ws1.WriteWndPos(m_optIACCMapWnd1, _T("Коррекция смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optIACCWMapWnd1, _T("Weight of mixture correction vs IAC pos"));
 else
  ws1.WriteWndPos(m_optIACCWMapWnd1, _T("Вес коррекции смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optIATCLTMapWnd1, _T("CLT to MAT influence factor"));
 else
  ws1.WriteWndPos(m_optIATCLTMapWnd1, _T("Коэффициент влияния ТОЖ на ТВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optTpsswtMapWnd1, _T("TPS switch point map"));
 else
  ws1.WriteWndPos(m_optTpsswtMapWnd1, _T("Точка переключения по ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optGtscMapWnd1, _T("Gas temperature correction map"));
 else
  ws1.WriteWndPos(m_optGtscMapWnd1, _T("Коррекция смеси по температуре газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optGpscMapWnd1, _T("Gas pressure correction map"));
 else
  ws1.WriteWndPos(m_optGpscMapWnd1, _T("Коррекция смеси по давлению газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAtscMapWnd1, _T("Air density correction map"));
 else
  ws1.WriteWndPos(m_optAtscMapWnd1, _T("Коррекция смеси по плотности воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optPwm1MapWnd1, _T("PWM1 duty map"));
 else
  ws1.WriteWndPos(m_optPwm1MapWnd1, _T("Карта скважности ШИМ на выходе PWM1"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optPwm2MapWnd1, _T("PWM2 duty map"));
 else
  ws1.WriteWndPos(m_optPwm2MapWnd1, _T("Карта скважности ШИМ на выходе PWM2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optIACMATMapWnd1, _T("IAC position's correction vs MAT map"));
 else
  ws1.WriteWndPos(m_optIACMATMapWnd1, _T("Коррекция положения РДВ от температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optTpszonMapWnd1, _T("Load axis allocation between MAP and TPS"));
 else
  ws1.WriteWndPos(m_optTpszonMapWnd1, _T("Распределение оси нагрузки между ДАД и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optCylMultMapWnd1, _T("Per cylinder injection PW multiplier"));
 else
  ws1.WriteWndPos(m_optCylMultMapWnd1, _T("Поцилиндровый множитель длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optCylAddMapWnd1, _T("Per cylinder injection PW addition"));
 else
  ws1.WriteWndPos(m_optCylAddMapWnd1, _T("Поцилиндровая добавка к длительности впрыска"));

 //group of separate maps
 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAttenuatorMapWnd1, _T("Attenuator's curve of amplification"));
 else
  ws1.WriteWndPos(m_optAttenuatorMapWnd1, _T("Таблица функции аттенюатора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optDwellCntrlMapWnd1, _T("Dwell time map"));
 else
  ws1.WriteWndPos(m_optDwellCntrlMapWnd1, _T("Время накопления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optCTSCurveMapWnd1, _T("Coolant temperature sensor table"));
 else
  ws1.WriteWndPos(m_optCTSCurveMapWnd1, _T("Кривая датчика температуры охлажд.житкости"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optATSCurvMapWnd1, _T("MAT sensor table"));
 else
  ws1.WriteWndPos(m_optATSCurvMapWnd1, _T("Кривая датчика температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optATSCorrMapWnd1, _T("Ign.timing correction vs MAT map"));
 else
  ws1.WriteWndPos(m_optATSCorrMapWnd1, _T("Коррекция УОЗ по температуре воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optGasdoseMapWnd1, _T("Gas valve position vs RPM,TPS map"));
 else
  ws1.WriteWndPos(m_optGasdoseMapWnd1, _T("Положение дозатора газа от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optBarocorrMapWnd1, _T("Barometric correction map"));
 else
  ws1.WriteWndPos(m_optBarocorrMapWnd1, _T("Барометрическая коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optManIgntimMapWnd1, _T("Manual ignition timing corr. map"));
 else
  ws1.WriteWndPos(m_optManIgntimMapWnd1, _T("Ручная октан-коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optCESettingsWnd1, _T("CE settings window"));
 else
  ws1.WriteWndPos(m_optCESettingsWnd1, _T("Настройки СЕ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optTmp2CurveMapWnd1, _T("TMP2 sensor table"));
 else
  ws1.WriteWndPos(m_optTmp2CurveMapWnd1, _T("Кривая датчика температуры на входе TMP2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optCrkTempMapWnd1, _T("Ign.timing vs CLT correction on cranking"));
 else
  ws1.WriteWndPos(m_optCrkTempMapWnd1, _T("Коррекция УОЗ по ДТОЖ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optEHPauseMapWnd1, _T("EGO heater's pause map (for PWM)"));
 else
  ws1.WriteWndPos(m_optEHPauseMapWnd1, _T("Время паузы подогрева ДК (для ШИМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optCrankingThrdMapWnd1, _T("Starter's blocking RPM"));
 else
  ws1.WriteWndPos(m_optCrankingThrdMapWnd1, _T("Обороты блокировки стартера"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optCrankingTimeMapWnd1, _T("Starter's blocking delay (strokes)"));
 else
  ws1.WriteWndPos(m_optCrankingTimeMapWnd1, _T("Время до блокировки стартера после превышения порога оборотов (такты)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optSmapabanThrdMapWnd1, _T("RPM threshold for abandon from start map"));
 else
  ws1.WriteWndPos(m_optSmapabanThrdMapWnd1, _T("Обороты перехода с пусковой карты"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optKnockZoneMapWnd1, _T("Knock zones vs RPM,TPS map"));
 else
  ws1.WriteWndPos(m_optKnockZoneMapWnd1, _T("Зоны детонации в зависимости от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optGrtsCurveMapWnd1, _T("Gas reducer's temperature sensor table"));
 else
  ws1.WriteWndPos(m_optGrtsCurveMapWnd1, _T("Кривая датчика температуры на входе GRTEMP"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optGrHeatDutyMapWnd1, _T("PWM duty map for controlling of gas reducer's heater"));
 else
  ws1.WriteWndPos(m_optGrHeatDutyMapWnd1, _T("Скважность ШИМ для управления нагревателем газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optPwmIacUCoefMapWnd1, _T("PWM IAC duty coefficient vs voltage"));
 else
  ws1.WriteWndPos(m_optPwmIacUCoefMapWnd1, _T("Коэффициент скважности ШИМ РДВ от напряжения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAftstrStrk0MapWnd1, _T("After start enrichment strokes vs coolant temperature (petrol)"));
 else
  ws1.WriteWndPos(m_optAftstrStrk0MapWnd1, _T("Число тактов обогащения после пуска от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optAftstrStrk1MapWnd1, _T("After start enrichment strokes vs coolant temperature (gas)"));
 else
  ws1.WriteWndPos(m_optAftstrStrk1MapWnd1, _T("Число тактов обогащения после пуска от температуры двигателя (gas)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optGrValDelMapWnd1, _T("Gas valve opening delay vs gas reducer's temperature map"));
 else
  ws1.WriteWndPos(m_optGrValDelMapWnd1, _T("Задержка включения газового клапана от температуры газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optFtlsCurveMapWnd1, _T("Fuel tank level sensor's table"));
 else
  ws1.WriteWndPos(m_optFtlsCurveMapWnd1, _T("Кривая датчика уровня топлива (ДУТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optEgtsCurveMapWnd1, _T("Exhaust gas temperature sensor's table"));
 else
  ws1.WriteWndPos(m_optEgtsCurveMapWnd1, _T("Кривая датчика температуры выхлопных газов (ДТВГ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optOpsCurveMapWnd1, _T("Oil pressure sensor's table"));
 else
  ws1.WriteWndPos(m_optOpsCurveMapWnd1, _T("Кривая датчика давления масла (ДДМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optManInjPwcMapWnd1, _T("Map for manual correction of injection PW"));
 else
  ws1.WriteWndPos(m_optManInjPwcMapWnd1, _T("Таблица для ручной коррекции длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optMAFCurveMapWnd1, _T("MAF flow curve"));
 else
  ws1.WriteWndPos(m_optMAFCurveMapWnd1, _T("Кривая расхода ДМРВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optFtlsCorMapWnd1, _T("Fuel tank level sensor's correction vs board voltage table"));
 else
  ws1.WriteWndPos(m_optFtlsCorMapWnd1, _T("Таблица коррекции датчика уровня топлива (ДУТ) от напряжения бортовой сети"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optLambdaZoneMapWnd1, _T("EGO correction zones vs RPM,load map"));
 else
  ws1.WriteWndPos(m_optLambdaZoneMapWnd1, _T("Зоны лямбда-коррекции в зависимости от оборотов и нагрузки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optFtsCurveMapWnd1, _T("Fuel temperature sensor's table"));
 else
  ws1.WriteWndPos(m_optFtsCurveMapWnd1, _T("Кривая датчика температуры топлива (ДТТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optFuelDensCorrMapWnd1, _T("Fuel density correction map"));
 else
  ws1.WriteWndPos(m_optFuelDensCorrMapWnd1, _T("Таблица коррекции плотности топлива"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optXtauXfAccMapWnd1, _T("Coefficient of injected fuel which goes into port wall's film (acceleration)"));
 else
  ws1.WriteWndPos(m_optXtauXfAccMapWnd1, _T("Коэффициент топлива выпадающего в пленку для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optXtauXfDecMapWnd1, _T("Coefficient of injected fuel which goes into port wall's film (deceleration)"));
 else
  ws1.WriteWndPos(m_optXtauXfDecMapWnd1, _T("Коэффициент топлива выпадающего в пленку для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optXtauTfAccMapWnd1, _T("Constant of fuel's film dissipation time (accelaration)"));
 else
  ws1.WriteWndPos(m_optXtauTfAccMapWnd1, _T("Постоянная времени испарения плeнки для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optXtauTfDecMapWnd1, _T("Constant of fuel's film dissipation time (decelaration)"));
 else
  ws1.WriteWndPos(m_optXtauTfDecMapWnd1, _T("Постоянная времени испарения плeнки для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optInjNonLinPMapWnd1, _T("Injector's non-linearity correction map used on small inj. PWs (petrol)"));
 else
  ws1.WriteWndPos(m_optInjNonLinPMapWnd1, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optInjNonLinGMapWnd1, _T("Injector's non-linearity correction map used on small inj. PWs (gas)"));
 else
  ws1.WriteWndPos(m_optInjNonLinGMapWnd1, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (газ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optEGODelayMapWnd1, _T("Number of strokes of EGO delay vs engine load"));
 else
  ws1.WriteWndPos(m_optEGODelayMapWnd1, _T("Число тактов задержки ДК в зависимости от нагрузки на двигатель"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optWUAFR0MapWnd1, _T("AFR vs coolant temperature (petrol)"));
 else
  ws1.WriteWndPos(m_optWUAFR0MapWnd1, _T("Соотношение Воздух/топливо от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ws1.WriteWndPos(m_optWUAFR1MapWnd1, _T("AFR vs coolant temperature (gas)"));
 else
  ws1.WriteWndPos(m_optWUAFR1MapWnd1, _T("Соотношение Воздух/топливо от температуры двигателя (газ)"));

 //Sizes of windows
 IniIO &sz = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteComment(_T("*** Remembered sizes of windows ***"), true);
 else
  sz.WriteComment(_T("*** Запомненные размеры окон ***"), true);
 sz.CreateSection(m_Name_WndSize_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optMainFrmWndSize, _T("Main window"));
 else
  sz.WriteWndPos(m_optMainFrmWndSize, _T("Главное окно"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optStrtMapWndSize, _T("Cranking ignition timing map"));
 else
  sz.WriteWndPos(m_optStrtMapWndSize, _T("Таблица УОЗ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optIdleMapWndSize, _T("Idling ignition timing map"));
 else
  sz.WriteWndPos(m_optIdleMapWndSize, _T("Таблица УОЗ на ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optWorkMapWndSize, _T("Work ignition timing map"));
 else
  sz.WriteWndPos(m_optWorkMapWndSize, _T("Рабочая карта УОЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optTempMapWndSize, _T("Ign. timing vs CLT correction map"));
 else
  sz.WriteWndPos(m_optTempMapWndSize, _T("Коррекция УОЗ по ДТОЖ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optTempIdlMapWndSize, _T("Ign. timing vs CLT correction map (idling)"));
 else
  sz.WriteWndPos(m_optTempIdlMapWndSize, _T("Коррекция УОЗ по ДТОЖ (для ХХ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optGridMapIgnWndSize, _T("Grid editing window for ign.timing maps"));
 else
  sz.WriteWndPos(m_optGridMapIgnWndSize, _T("Редактирование в виде таблиц (зажигание)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optGridMapInjWndSize, _T("Grid editing window for injection maps"));
 else
  sz.WriteWndPos(m_optGridMapInjWndSize, _T("Редактирование в виде таблиц (впрыск)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optVEMapWndSize, _T("VE map"));
 else
  sz.WriteWndPos(m_optVEMapWndSize, _T("Наполнение (объемный КПД)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optVE2MapWndSize, _T("VE map 2"));
 else
  sz.WriteWndPos(m_optVE2MapWndSize, _T("Наполнение (объемный КПД) 2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optVEIMapWndSize, _T("VE map for idling"));
 else
  sz.WriteWndPos(m_optVEIMapWndSize, _T("Наполнение (объемный КПД) для ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAFRMapWndSize, _T("AFR map"));
 else
  sz.WriteWndPos(m_optAFRMapWndSize, _T("Соотношение Воздух/топливо"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optCrnkMapWndSize, _T("Injection PW on cranking"));
 else
  sz.WriteWndPos(m_optCrnkMapWndSize, _T("Длительность впрыска на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optWrmpMapWndSize, _T("Warmup enrichment map"));
 else
  sz.WriteWndPos(m_optWrmpMapWndSize, _T("Обогащение при прогреве"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optDeadMapWndSize, _T("Injector's lag map"));
 else
  sz.WriteWndPos(m_optDeadMapWndSize, _T("Лаг форсунки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optIdlrMapWndSize, _T("IAC position (working)"));
 else
  sz.WriteWndPos(m_optIdlrMapWndSize, _T("Положение РДВ (рабочее)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optIdlcMapWndSize, _T("IAC position (cranking)"));
 else
  sz.WriteWndPos(m_optIdlcMapWndSize, _T("Положение РДВ (на пуске)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optThrassMapWndSize, _T("Throttle assist"));
 else
  sz.WriteWndPos(m_optThrassMapWndSize, _T("Сопроводитель дросселя"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAETPSMapWndSize, _T("Enrichment vs throttle speed map"));
 else
  sz.WriteWndPos(m_optAETPSMapWndSize, _T("Обогащение по скорости ДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAEMAPMapWndSize, _T("Enrichment vs MAPdot"));
 else
  sz.WriteWndPos(m_optAEMAPMapWndSize, _T("Обогащение по скорости ДАД"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAERPMMapWndSize, _T("Enrichment vs engine speed map"));
 else
  sz.WriteWndPos(m_optAERPMMapWndSize, _T("Обогащение по оборотам"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAftstrMapWndSize, _T("Afterstart enrichment map"));
 else
  sz.WriteWndPos(m_optAftstrMapWndSize, _T("Обогащение после пуска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optITMapWndSize, _T("Injection timing map"));
 else
  sz.WriteWndPos(m_optITMapWndSize, _T("Фаза впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optITRPMMapWndSize, _T("Target idling RPM"));
 else
  sz.WriteWndPos(m_optITRPMMapWndSize, _T("Целевые обороты ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optRigidMapWndSize, _T("Idling regulator's rigidity map"));
 else
  sz.WriteWndPos(m_optRigidMapWndSize, _T("Жесткость РХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optEGOCrvMapWndSize, _T("EGO sensor curve"));
 else
  sz.WriteWndPos(m_optEGOCrvMapWndSize, _T("Кривая датчика кислорода"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optIACCMapWndSize, _T("Mixture correction vs IAC pos"));
 else
  sz.WriteWndPos(m_optIACCMapWndSize, _T("Коррекция смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optIACCWMapWndSize, _T("Weight of mixture correction vs IAC pos"));
 else
  sz.WriteWndPos(m_optIACCWMapWndSize, _T("Вес коррекции смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optIATCLTMapWndSize, _T("CLT to MAT influence factor"));
 else
  sz.WriteWndPos(m_optIATCLTMapWndSize, _T("Коэффициент влияния ТОЖ на ТВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optTpsswtMapWndSize, _T("TPS switch point map"));
 else
  sz.WriteWndPos(m_optTpsswtMapWndSize, _T("Точка переключения по ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optGtscMapWndSize, _T("Gas temperature correction map"));
 else
  sz.WriteWndPos(m_optGtscMapWndSize, _T("Коррекция смеси по температуре газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optGpscMapWndSize, _T("Gas pressure correction map"));
 else
  sz.WriteWndPos(m_optGpscMapWndSize, _T("Коррекция смеси по давлению газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAtscMapWndSize, _T("Air density correction map"));
 else
  sz.WriteWndPos(m_optAtscMapWndSize, _T("Коррекция смеси по плотности воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optPwm1MapWndSize, _T("PWM1 duty map"));
 else
  sz.WriteWndPos(m_optPwm1MapWndSize, _T("Карта скважности на выходе PWM1"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optPwm2MapWndSize, _T("PWM2 duty map"));
 else
  sz.WriteWndPos(m_optPwm2MapWndSize, _T("Карта скважности на выходе PWM2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optIACMATMapWndSize, _T("IAC position's correction vs MAT map"));
 else
  sz.WriteWndPos(m_optIACMATMapWndSize, _T("Коррекция положения РДВ от температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optTpszonMapWndSize, _T("Load axis allocation between MAP and TPS"));
 else
  sz.WriteWndPos(m_optTpszonMapWndSize, _T("Распределение оси нагрузки между ДАД и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optCylMultMapWndSize, _T("Per cylinder injection PW multiplier"));
 else
  sz.WriteWndPos(m_optCylMultMapWndSize, _T("Поцилиндровый множитель длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optCylAddMapWndSize, _T("Per cylinder injection PW addition"));
 else
  sz.WriteWndPos(m_optCylAddMapWndSize, _T("Поцилиндровая добавка к длительности впрыска"));

 //group of separate maps
 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAttenuatorMapWndSize, _T("Attenuator's curve of amplification"));
 else
  sz.WriteWndPos(m_optAttenuatorMapWndSize, _T("Таблица функции аттенюатора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optDwellCntrlMapWndSize, _T("Dwell time map"));
 else
  sz.WriteWndPos(m_optDwellCntrlMapWndSize, _T("Время накопления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optCTSCurveMapWndSize, _T("Coolant temperature sensor table"));
 else
  sz.WriteWndPos(m_optCTSCurveMapWndSize, _T("Кривая датчика температуры охлажд.житкости"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optATSCurvMapWndSize, _T("MAT sensor table"));
 else
  sz.WriteWndPos(m_optATSCurvMapWndSize, _T("Кривая датчика температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optATSCorrMapWndSize, _T("Ign.timing correction vs MAT map"));
 else
  sz.WriteWndPos(m_optATSCorrMapWndSize, _T("Коррекция УОЗ по температуре воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optGasdoseMapWndSize, _T("Gas valve position vs RPM,TPS map"));
 else
  sz.WriteWndPos(m_optGasdoseMapWndSize, _T("Положение дозатора газа от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optBarocorrMapWndSize, _T("Barometric correction map"));
 else
  sz.WriteWndPos(m_optBarocorrMapWndSize, _T("Барометрическая коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optManIgntimMapWndSize, _T("Manual ignition timing corr. map"));
 else
  sz.WriteWndPos(m_optManIgntimMapWndSize, _T("Ручная октан-коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optCESettingsWndSize, _T("CE settings window"));
 else
  sz.WriteWndPos(m_optCESettingsWndSize, _T("Настройки СЕ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optTmp2CurveMapWndSize, _T("TMP2 sensor table"));
 else
  sz.WriteWndPos(m_optTmp2CurveMapWndSize, _T("Кривая датчика температуры на входе TMP2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optCrkTempMapWndSize, _T("Ign.timing vs CLT correction on cranking"));
 else
  sz.WriteWndPos(m_optCrkTempMapWndSize, _T("Коррекция УОЗ по ДТОЖ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optEHPauseMapWndSize, _T("EGO heater's pause map (for PWM)"));
 else
  sz.WriteWndPos(m_optEHPauseMapWndSize, _T("Время паузы подогрева ДК (для ШИМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optCrankingThrdMapWndSize, _T("Starter's blocking RPM"));
 else
  sz.WriteWndPos(m_optCrankingThrdMapWndSize, _T("Обороты блокировки стартера"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optCrankingTimeMapWndSize, _T("Starter's blocking delay (strokes)"));
 else
  sz.WriteWndPos(m_optCrankingTimeMapWndSize, _T("Время до блокировки стартера после превышения порога оборотов (такты)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optSmapabanThrdMapWndSize, _T("RPM threshold for abandon from start map"));
 else
  sz.WriteWndPos(m_optSmapabanThrdMapWndSize, _T("Обороты перехода с пусковой карты"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optKnockZoneMapWndSize, _T("Knock zones vs RPM,TPS map"));
 else
  sz.WriteWndPos(m_optKnockZoneMapWndSize, _T("Зоны детонации в зависимости от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optGrtsCurveMapWndSize, _T("GRTEMP sensor table"));
 else
  sz.WriteWndPos(m_optGrtsCurveMapWndSize, _T("Кривая датчика температуры на входе GRTEMP"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optGrHeatDutyMapWndSize, _T("PWM duty map for controlling of gas reducer's heater"));
 else
  sz.WriteWndPos(m_optGrHeatDutyMapWndSize, _T("Скважность ШИМ для управления нагревателем газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optPwmIacUCoefMapWndSize, _T("PWM IAC duty coefficient vs voltage"));
 else
  sz.WriteWndPos(m_optPwmIacUCoefMapWndSize, _T("Коэффициент скважности ШИМ РДВ от напряжения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAftstrStrk0MapWndSize, _T("After start enrichment strokes vs coolant temperature (petrol)"));
 else
  sz.WriteWndPos(m_optAftstrStrk0MapWndSize, _T("Число тактов обогащения после пуска от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optAftstrStrk1MapWndSize, _T("After start enrichment strokes vs coolant temperature (gas)"));
 else
  sz.WriteWndPos(m_optAftstrStrk1MapWndSize, _T("Число тактов обогащения после пуска от температуры двигателя (газ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optGrValDelMapWndSize, _T("Gas valve opening delay vs gas reducer's temperature map"));
 else
  sz.WriteWndPos(m_optGrValDelMapWndSize, _T("Задержка включения газового клапана от температуры газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optFtlsCurveMapWndSize, _T("Fuel tank level sensor's table"));
 else
  sz.WriteWndPos(m_optFtlsCurveMapWndSize, _T("Кривая датчика уровня топлива (ДУТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optEgtsCurveMapWndSize, _T("Exhaust gas temperature sensor's table"));
 else
  sz.WriteWndPos(m_optEgtsCurveMapWndSize, _T("Кривая датчика температуры выхлопных газов (ДТВГ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optOpsCurveMapWndSize, _T("Oil pressure sensor's table"));
 else
  sz.WriteWndPos(m_optOpsCurveMapWndSize, _T("Кривая датчика давления масла (ДДМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optManInjPwcMapWndSize, _T("Map for manual correction of injection PW"));
 else
  sz.WriteWndPos(m_optManInjPwcMapWndSize, _T("Таблица для ручной коррекции длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optMAFCurveMapWndSize, _T("MAF flow curve"));
 else
  sz.WriteWndPos(m_optMAFCurveMapWndSize, _T("Кривая расхода ДМРВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optFtlsCorMapWndSize, _T("Fuel tank level sensor's correction vs board voltage table"));
 else
  sz.WriteWndPos(m_optFtlsCorMapWndSize, _T("Таблица коррекции датчика уровня топлива (ДУТ) от напряжения бортовой сети"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optLambdaZoneMapWndSize, _T("EGO correction zones vs RPM,load map"));
 else
  sz.WriteWndPos(m_optLambdaZoneMapWndSize, _T("Зоны лямбда-коррекции в зависимости от оборотов и нагрузки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optFtsCurveMapWndSize, _T("Fuel temperature sensor's table"));
 else
  sz.WriteWndPos(m_optFtsCurveMapWndSize, _T("Кривая датчика температуры топлива (ДТТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optFuelDensCorrMapWndSize, _T("Fuel density correction map"));
 else
  sz.WriteWndPos(m_optFuelDensCorrMapWndSize, _T("Таблица коррекции плотности топлива"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optXtauXfAccMapWndSize, _T("Coefficient of injected fuel which goes into port wall's film (acceleration)"));
 else
  sz.WriteWndPos(m_optXtauXfAccMapWndSize, _T("Коэффициент топлива выпадающего в пленку для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optXtauXfDecMapWndSize, _T("Coefficient of injected fuel which goes into port wall's film (deceleration)"));
 else
  sz.WriteWndPos(m_optXtauXfDecMapWndSize, _T("Коэффициент топлива выпадающего в пленку для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optXtauTfAccMapWndSize, _T("Constant of fuel's film dissipation time (accelaration)"));
 else
  sz.WriteWndPos(m_optXtauTfAccMapWndSize, _T("Постоянная времени испарения плeнки для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optXtauTfDecMapWndSize, _T("Constant of fuel's film dissipation time (decelaration)"));
 else
  sz.WriteWndPos(m_optXtauTfDecMapWndSize, _T("Постоянная времени испарения плeнки для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optInjNonLinPMapWndSize, _T("Injector's non-linearity correction map used on small inj. PWs (petrol)"));
 else
  sz.WriteWndPos(m_optInjNonLinPMapWndSize, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optInjNonLinGMapWndSize, _T("Injector's non-linearity correction map used on small inj. PWs (gas)"));
 else
  sz.WriteWndPos(m_optInjNonLinGMapWndSize, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (газ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optEGODelayMapWndSize, _T("Number of strokes of EGO delay vs engine load"));
 else
  sz.WriteWndPos(m_optEGODelayMapWndSize, _T("Число тактов задержки ДК в зависимости от нагрузки на двигатель"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optWUAFR0MapWndSize, _T("AFR vs coolant temperature (petrol)"));
 else
  sz.WriteWndPos(m_optWUAFR0MapWndSize, _T("Соотношение Воздух/топливо от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz.WriteWndPos(m_optWUAFR1MapWndSize, _T("AFR vs coolant temperature (gas)"));
 else
  sz.WriteWndPos(m_optWUAFR1MapWndSize, _T("Соотношение Воздух/топливо от температуры двигателя (газ)"));

 //Sizes of windows (online)
 IniIO &sz1 = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteComment(_T("*** Remembered sizes of windows (opened from the \"Parameters and monitor\" tab) ***"), true);
 else
  sz1.WriteComment(_T("*** Запомненные размеры окон (открываются с вкладки \"Параметры и монитор\") ***"), true);
 sz1.CreateSection(m_Name_WndSize_Section1);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optStrtMapWndSize1, _T("Cranking ignition timing map"));
 else
  sz1.WriteWndPos(m_optStrtMapWndSize1, _T("Таблица УОЗ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optIdleMapWndSize1, _T("Idling ignition timing map"));
 else
  sz1.WriteWndPos(m_optIdleMapWndSize1, _T("Таблица УОЗ на ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optWorkMapWndSize1, _T("Work ignition timing map"));
 else
  sz1.WriteWndPos(m_optWorkMapWndSize1, _T("Рабочая карта УОЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optTempMapWndSize1, _T("Ign. timing vs CLT correction map"));
 else
  sz1.WriteWndPos(m_optTempMapWndSize1, _T("Коррекция УОЗ по ДТОЖ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optTempIdlMapWndSize1, _T("Ign. timing vs CLT correction map (idling)"));
 else
  sz1.WriteWndPos(m_optTempIdlMapWndSize1, _T("Коррекция УОЗ по ДТОЖ (для ХХ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optGridMapIgnWndSize1, _T("Grid editing window for ign.timing maps"));
 else
  sz1.WriteWndPos(m_optGridMapIgnWndSize1, _T("Редактирование в виде таблиц (зажигание)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optGridMapInjWndSize1, _T("Grid editing window for injection maps"));
 else
  sz1.WriteWndPos(m_optGridMapInjWndSize1, _T("Редактирование в виде таблиц (впрыск)"));
 
 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optVEMapWndSize1, _T("VE map"));
 else
  sz1.WriteWndPos(m_optVEMapWndSize1, _T("Наполнение (объемный КПД)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optVE2MapWndSize1, _T("VE map 2"));
 else
  sz1.WriteWndPos(m_optVE2MapWndSize1, _T("Наполнение (объемный КПД) 2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optVEIMapWndSize1, _T("VE map for idling"));
 else
  sz1.WriteWndPos(m_optVEIMapWndSize1, _T("Наполнение (объемный КПД) для ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAFRMapWndSize1, _T("AFR map"));
 else
  sz1.WriteWndPos(m_optAFRMapWndSize1, _T("Соотношение Воздух/топливо"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optCrnkMapWndSize1, _T("Injection PW on cranking"));
 else
  sz1.WriteWndPos(m_optCrnkMapWndSize1, _T("Длительность впрыска на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optWrmpMapWndSize1, _T("Warmup enrichment map"));
 else
  sz1.WriteWndPos(m_optWrmpMapWndSize1, _T("Обогащение при прогреве"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optDeadMapWndSize1, _T("Injector's lag map"));
 else
  sz1.WriteWndPos(m_optDeadMapWndSize1, _T("Лаг форсунки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optIdlrMapWndSize1, _T("IAC position (working)"));
 else
  sz1.WriteWndPos(m_optIdlrMapWndSize1, _T("Положение РДВ (рабочее)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optIdlcMapWndSize1, _T("IAC position (cranking)"));
 else
  sz1.WriteWndPos(m_optIdlcMapWndSize1, _T("Положение РДВ (на пуске)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optThrassMapWndSize1, _T("Throttle assist"));
 else
  sz1.WriteWndPos(m_optThrassMapWndSize1, _T("Сопроводитель дросселя"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAETPSMapWndSize1, _T("Enrichment vs throttle speed map"));
 else
  sz1.WriteWndPos(m_optAETPSMapWndSize1, _T("Обогащение по скорости ДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAEMAPMapWndSize1, _T("Enrichment vs MAPdot"));
 else
  sz1.WriteWndPos(m_optAEMAPMapWndSize1, _T("Обогащение по скорости ДАД"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAERPMMapWndSize1, _T("Enrichment vs engine speed"));
 else
  sz1.WriteWndPos(m_optAERPMMapWndSize1, _T("Обогащение по оборотам"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAftstrMapWndSize1, _T("Afterstart enrichment map"));
 else
  sz1.WriteWndPos(m_optAftstrMapWndSize1, _T("Обогащение после пуска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optITMapWndSize1, _T("Injection timing map"));
 else
  sz1.WriteWndPos(m_optITMapWndSize1, _T("Фаза впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optITRPMMapWndSize1, _T("Target idling RPM"));
 else
  sz1.WriteWndPos(m_optITRPMMapWndSize1, _T("Целевые обороты ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optRigidMapWndSize1, _T("Idling regulator's rigidity map"));
 else
  sz1.WriteWndPos(m_optRigidMapWndSize1, _T("Жесткость РХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optEGOCrvMapWndSize1, _T("EGO sensor curve"));
 else
  sz1.WriteWndPos(m_optEGOCrvMapWndSize1, _T("Кривая датчика кислорода"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optIACCMapWndSize1, _T("Mixture correction vs IAC pos"));
 else
  sz1.WriteWndPos(m_optIACCMapWndSize1, _T("Коррекция смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optIACCWMapWndSize1, _T("Weight of mixture correction vs IAC pos"));
 else
  sz1.WriteWndPos(m_optIACCWMapWndSize1, _T("Вес коррекции смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optIATCLTMapWndSize1, _T("CLT to MAT influence factor"));
 else
  sz1.WriteWndPos(m_optIATCLTMapWndSize1, _T("Коэффициент влияния ТОЖ на ТВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optTpsswtMapWndSize1, _T("TPS switch point map"));
 else
  sz1.WriteWndPos(m_optTpsswtMapWndSize1, _T("Точка переключения по ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optGtscMapWndSize1, _T("Gas temperature correction map"));
 else
  sz1.WriteWndPos(m_optGtscMapWndSize1, _T("Коррекция смеси по температуре газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optGpscMapWndSize1, _T("Gas pressure correction map"));
 else
  sz1.WriteWndPos(m_optGpscMapWndSize1, _T("Коррекция смеси по давлению газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAtscMapWndSize1, _T("Air density correction map"));
 else
  sz1.WriteWndPos(m_optAtscMapWndSize1, _T("Коррекция смеси по плотности воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optPwm1MapWndSize1, _T("PWM1 duty map"));
 else
  sz1.WriteWndPos(m_optPwm1MapWndSize1, _T("Карта скважности на выходе PWM1"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optPwm2MapWndSize1, _T("PWM2 duty map"));
 else
  sz1.WriteWndPos(m_optPwm2MapWndSize1, _T("Карта скважности на выходе PWM2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optIACMATMapWndSize1, _T("IAC position's correction vs MAT map"));
 else
  sz1.WriteWndPos(m_optIACMATMapWndSize1, _T("Коррекция положения РДВ от температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optTpszonMapWndSize1, _T("Load axis allocation between MAP and TPS"));
 else
  sz1.WriteWndPos(m_optTpszonMapWndSize1, _T("Распределение оси нагрузки между ДАД и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optCylMultMapWndSize1, _T("Per cylinder injection PW multiplier"));
 else
  sz1.WriteWndPos(m_optCylMultMapWndSize1, _T("Поцилиндровый множитель длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optCylAddMapWndSize1, _T("Per cylinder injection PW addition"));
 else
  sz1.WriteWndPos(m_optCylAddMapWndSize1, _T("Поцилиндровая добавка к длительности впрыска"));

 //group of separate maps
 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAttenuatorMapWndSize1, _T("Attenuator's curve of amplification"));
 else
  sz1.WriteWndPos(m_optAttenuatorMapWndSize1, _T("Таблица функции аттенюатора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optDwellCntrlMapWndSize1, _T("Dwell time map"));
 else
  sz1.WriteWndPos(m_optDwellCntrlMapWndSize1, _T("Время накопления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optCTSCurveMapWndSize1, _T("Coolant temperature sensor table"));
 else
  sz1.WriteWndPos(m_optCTSCurveMapWndSize1, _T("Кривая датчика температуры охлажд.житкости"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optATSCurvMapWndSize1, _T("MAT sensor table"));
 else
  sz1.WriteWndPos(m_optATSCurvMapWndSize1, _T("Кривая датчика температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optATSCorrMapWndSize1, _T("Ign.timing correction vs MAT map"));
 else
  sz1.WriteWndPos(m_optATSCorrMapWndSize1, _T("Коррекция УОЗ по температуре воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optGasdoseMapWndSize1, _T("Gas valve position vs RPM,TPS map"));
 else
  sz1.WriteWndPos(m_optGasdoseMapWndSize1, _T("Положение дозатора газа от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optBarocorrMapWndSize1, _T("Barometric correction map"));
 else
  sz1.WriteWndPos(m_optBarocorrMapWndSize1, _T("Барометрическая коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optManIgntimMapWndSize1, _T("Manual ignition timing corr. map"));
 else
  sz1.WriteWndPos(m_optManIgntimMapWndSize1, _T("Ручная октан-коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optCESettingsWndSize1, _T("CE settings window"));
 else
  sz1.WriteWndPos(m_optCESettingsWndSize1, _T("Настройки СЕ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optTmp2CurveMapWndSize1, _T("TMP2 sensor table"));
 else
  sz1.WriteWndPos(m_optTmp2CurveMapWndSize1, _T("Кривая датчика температуры на входе TMP2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optCrkTempMapWndSize1, _T("Ign.timing vs CLT correction on cranking"));
 else
  sz1.WriteWndPos(m_optCrkTempMapWndSize1, _T("Коррекция УОЗ по ДТОЖ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optEHPauseMapWndSize1, _T("EGO heater's pause map (for PWM)"));
 else
  sz1.WriteWndPos(m_optEHPauseMapWndSize1, _T("Время паузы подогрева ДК (для ШИМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optCrankingThrdMapWndSize1, _T("Starter's blocking RPM"));
 else
  sz1.WriteWndPos(m_optCrankingThrdMapWndSize1, _T("Обороты блокировки стартера"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optCrankingTimeMapWndSize1, _T("Starter's blocking delay (strokes)"));
 else
  sz1.WriteWndPos(m_optCrankingTimeMapWndSize1, _T("Время до блокировки стартера после превышения порога оборотов (такты)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optSmapabanThrdMapWndSize1, _T("RPM threshold for abandon from start map"));
 else
  sz1.WriteWndPos(m_optSmapabanThrdMapWndSize1, _T("Обороты перехода с пусковой карты"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optKnockZoneMapWndSize1, _T("Knock zones vs RPM,TPS map"));
 else
  sz1.WriteWndPos(m_optKnockZoneMapWndSize1, _T("Зоны детонации в зависимости от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optGrtsCurveMapWndSize1, _T("GRTEMP sensor table"));
 else
  sz1.WriteWndPos(m_optGrtsCurveMapWndSize1, _T("Кривая датчика температуры на входе GRTEMP"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optGrHeatDutyMapWndSize1, _T("PWM duty map for controlling of gas reducer's heater"));
 else
  sz1.WriteWndPos(m_optGrHeatDutyMapWndSize1, _T("Скважность ШИМ для управления нагревателем газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optPwmIacUCoefMapWndSize1, _T("PWM IAC duty coefficient vs voltage"));
 else
  sz1.WriteWndPos(m_optPwmIacUCoefMapWndSize1, _T("Коэффициент скважности ШИМ РДВ от напряжения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAftstrStrk0MapWndSize1, _T("After start enrichment strokes vs coolant temperature (petrol)"));
 else
  sz1.WriteWndPos(m_optAftstrStrk0MapWndSize1, _T("Число тактов обогащения после пуска от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optAftstrStrk1MapWndSize1, _T("After start enrichment strokes vs coolant temperature (gas)"));
 else
  sz1.WriteWndPos(m_optAftstrStrk1MapWndSize1, _T("Число тактов обогащения после пуска от температуры двигателя (газ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optGrValDelMapWndSize1, _T("Gas valve opening delay vs gas reducer's temperature map"));
 else
  sz1.WriteWndPos(m_optGrValDelMapWndSize1, _T("Задержка включения газового клапана от температуры газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optFtlsCurveMapWndSize1, _T("Fuel tank level sensor's table"));
 else
  sz1.WriteWndPos(m_optFtlsCurveMapWndSize1, _T("Кривая датчика уровня топлива (ДУТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optEgtsCurveMapWndSize1, _T("Exhaust gas temperature sensor's table"));
 else
  sz1.WriteWndPos(m_optEgtsCurveMapWndSize1, _T("Кривая датчика температуры выхлопных газов (ДТВГ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optOpsCurveMapWndSize1, _T("Oil pressure sensor's table"));
 else
  sz1.WriteWndPos(m_optOpsCurveMapWndSize1, _T("Кривая датчика давления масла (ДДМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optManInjPwcMapWndSize1, _T("Map for manual correction of injection PW"));
 else
  sz1.WriteWndPos(m_optManInjPwcMapWndSize1, _T("Таблица для ручной коррекции длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optMAFCurveMapWndSize1, _T("MAF flow curve"));
 else
  sz1.WriteWndPos(m_optMAFCurveMapWndSize1, _T("Кривая расхода ДМРВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optFtlsCorMapWndSize1, _T("Fuel tank level sensor's correction vs board voltage table"));
 else
  sz1.WriteWndPos(m_optFtlsCorMapWndSize1, _T("Таблица коррекции датчика уровня топлива (ДУТ) от напряжения бортовой сети"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optLambdaZoneMapWndSize1, _T("EGO correction zones vs RPM,load map"));
 else
  sz1.WriteWndPos(m_optLambdaZoneMapWndSize1, _T("Зоны лямбда-коррекции в зависимости от оборотов и нагрузки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optFtsCurveMapWndSize1, _T("Fuel temperature sensor's table"));
 else
  sz1.WriteWndPos(m_optFtsCurveMapWndSize1, _T("Кривая датчика температуры топлива (ДТТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optFuelDensCorrMapWndSize1, _T("Fuel density correction map"));
 else
  sz1.WriteWndPos(m_optFuelDensCorrMapWndSize1, _T("Таблица коррекции плотности топлива"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optXtauXfAccMapWndSize1, _T("Coefficient of injected fuel which goes into port wall's film (acceleration)"));
 else
  sz1.WriteWndPos(m_optXtauXfAccMapWndSize1, _T("Коэффициент топлива выпадающего в пленку для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optXtauXfDecMapWndSize1, _T("Coefficient of injected fuel which goes into port wall's film (deceleration)"));
 else
  sz1.WriteWndPos(m_optXtauXfDecMapWndSize1, _T("Коэффициент топлива выпадающего в пленку для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optXtauTfAccMapWndSize1, _T("Constant of fuel's film dissipation time (accelaration)"));
 else
  sz1.WriteWndPos(m_optXtauTfAccMapWndSize1, _T("Постоянная времени испарения плeнки для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optXtauTfDecMapWndSize1, _T("Constant of fuel's film dissipation time (decelaration)"));
 else
  sz1.WriteWndPos(m_optXtauTfDecMapWndSize1, _T("Постоянная времени испарения плeнки для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optInjNonLinPMapWndSize1, _T("Injector's non-linearity correction map used on small inj. PWs (petrol)"));
 else
  sz1.WriteWndPos(m_optInjNonLinPMapWndSize1, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optInjNonLinGMapWndSize1, _T("Injector's non-linearity correction map used on small inj. PWs (gas)"));
 else
  sz1.WriteWndPos(m_optInjNonLinGMapWndSize1, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (газ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optEGODelayMapWndSize1, _T("Number of strokes of EGO delay vs engine load"));
 else
  sz1.WriteWndPos(m_optEGODelayMapWndSize1, _T("Число тактов задержки ДК в зависимости от нагрузки на двигатель"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optWUAFR0MapWndSize1, _T("AFR vs coolant temperature (petrol)"));
 else
  sz1.WriteWndPos(m_optWUAFR0MapWndSize1, _T("Соотношение Воздух/топливо от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sz1.WriteWndPos(m_optWUAFR1MapWndSize1, _T("AFR vs coolant temperature (gas)"));
 else
  sz1.WriteWndPos(m_optWUAFR1MapWndSize1, _T("Соотношение Воздух/топливо от температуры двигателя (газ)"));

 //States of windows
 IniIO &sw = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  sw.WriteComment(_T("*** Remembered states of windows ***"), true);
 else
  sw.WriteComment(_T("*** Запомненные состояние окон ***"), true);
 sw.CreateSection(m_Name_WndState_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sw.WriteEnum(m_optMainFrmWndState, m_AllowableWndStates, _T("State of the main window. Allowed values are: minimized, normal and maximized"));
 else
  sw.WriteEnum(m_optMainFrmWndState, m_AllowableWndStates, _T("Состояние главного окна. Допустимые значения: minimized, normal and maximized"));

 TCHAR* ii_comment[2];
 if (m_optInterfaceLang.value == IL_ENGLISH)
 {
  ii_comment[0] = _T("*** Configuration of virtual On\\Off indicators (1-st profile) ***\r\n; Use IndRows to specify number of rows will be used for grouping of virtual indicators\r\n; Set index number for each field (order). Indicator fileds will be displayed in direction from left-top to right-bottom according to set order.");
  ii_comment[1] = _T("*** Configuration of virtual On\\Off indicators (2-nd profile) ***");
 }
 else
 {
  ii_comment[0] = _T("*** Конфигурация виртуальных индикаторов (1-й профиль) ***\r\n; Используйте IndRows чтобы указать кол-во строк, используемых для группировки индикаторов.\r\n; Установите порядковый номер для каждой ячейки. Ячейки индикаторов будут отображаться в направлении с левого-верхнего до правого-нижнего углов в соответствии с установленным порядком.");
  ii_comment[1] = _T("*** Конфигурация виртуальных индикаторов (2-й профиль) ***"); 
 }

 //Indicators
 for(int i = 0; i < 2; ++i)
 {
  IniIO &ii = writer;
  ii.WriteComment(ii_comment[i], true);
  ii.CreateSection(m_Name_Indicators_Section[i]);

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteFlt(m_optIndHeightPercent[i], 2, _T("Percent of height which indicators' panel will occupy"));
  else
   ii.WriteFlt(m_optIndHeightPercent[i], 2, _T("Процент высоты, которую будет занимать панель индикаторов"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndRows[i], _T("Number of rows in the virtual indicators panel"));
  else
   ii.WriteInt(m_optIndRows[i], _T("Кол-во строк в панели виртуальных индикаторов"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndGas_v[i], _T("GAS_V input"));
  else
   ii.WriteInt(m_optIndGas_v[i], _T("Вход GAS_V"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndCarb[i], _T("CARB input"));
  else
   ii.WriteInt(m_optIndCarb[i], _T("Вход CARB"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndIdleValve[i], _T("Idle cutoff valve"));
  else
   ii.WriteInt(m_optIndIdleValve[i], _T("Клапан ЭПХХ"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndPowerValve[i], _T("Power valve"));
  else
   ii.WriteInt(m_optIndPowerValve[i], _T("Клапан ЭМР"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndStBlock[i], _T("Starter relay"));
  else
   ii.WriteInt(m_optIndStBlock[i], _T("Реле блокировки стартера"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndAE[i], _T("Acceleration enrichment"));
  else
   ii.WriteInt(m_optIndAE[i], _T("Обогащение при ускорении"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndCoolingFan[i], _T("Cooling fan"));
  else
   ii.WriteInt(m_optIndCoolingFan[i], _T("Вентилятор охлаждения"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndCE[i], _T("Check Engine lamp"));
  else
   ii.WriteInt(m_optIndCE[i], _T("Лампа Check Engine"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndFCRevLim[i], _T("Rev. limitter"));
  else
   ii.WriteInt(m_optIndFCRevLim[i], _T("Ограничение оборотов"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndFloodClear[i], _T("Flood clear"));
  else
   ii.WriteInt(m_optIndFloodClear[i], _T("Режим продувки"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndSysLocked[i], _T("System locked"));
  else
   ii.WriteInt(m_optIndSysLocked[i], _T("Система заблокирована"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndIgn_i[i], _T("IGN_I input"));
  else
   ii.WriteInt(m_optIndIgn_i[i], _T("Вход IGN_I"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndCond_i[i], _T("COND_I input"));
  else
   ii.WriteInt(m_optIndCond_i[i], _T("Вход COND_I"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndEpas_i[i], _T("EPAS_I input"));
  else
   ii.WriteInt(m_optIndEpas_i[i], _T("Вход EPAS_I"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndAftStrEnr[i], _T("Afterstart enrichment"));
  else
   ii.WriteInt(m_optIndAftStrEnr[i], _T("Обогащение после пуска"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndIacClLoop[i], _T("IAC closed loop"));
  else
   ii.WriteInt(m_optIndIacClLoop[i], _T("РХХ closed loop"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndUniOut1[i], _T("Universal output 1"));
  else
   ii.WriteInt(m_optIndUniOut1[i], _T("Универсальный выход 1"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndUniOut2[i], _T("Universal output 2"));
  else
   ii.WriteInt(m_optIndUniOut2[i], _T("Универсальный выход 2"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndUniOut3[i], _T("Universal output 3"));
  else
   ii.WriteInt(m_optIndUniOut3[i], _T("Универсальный выход 3"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndUniOut4[i], _T("Universal output 4"));
  else
   ii.WriteInt(m_optIndUniOut4[i], _T("Универсальный выход 4"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndUniOut5[i], _T("Universal output 5"));
  else
   ii.WriteInt(m_optIndUniOut5[i], _T("Универсальный выход 5"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndUniOut6[i], _T("Universal output 6"));
  else
   ii.WriteInt(m_optIndUniOut6[i], _T("Универсальный выход 6"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndGpa4_i[i], _T("GPA4_I input"));
  else
   ii.WriteInt(m_optIndGpa4_i[i], _T("Вход GPA4_I"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndInput1[i], _T("INPUT1 input"));
  else
   ii.WriteInt(m_optIndInput1[i], _T("Вход INPUT1"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndInput2[i], _T("INPUT2 input"));
  else
   ii.WriteInt(m_optIndInput2[i], _T("Вход INPUT2"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndAuto_i[i], _T("AUTO_I input"));
  else
   ii.WriteInt(m_optIndAuto_i[i], _T("Вход AUTO_I"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndMapsel0[i], _T("MAPSEL0 input"));
  else
   ii.WriteInt(m_optIndMapsel0[i], _T("Вход MAPSEL0"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndRefprs_i[i], _T("REFPRS_I input"));
  else
   ii.WriteInt(m_optIndRefprs_i[i], _T("Вход REFPRS_I"));

  if (m_optInterfaceLang.value == IL_ENGLISH)
   ii.WriteInt(m_optIndAltrn_i[i], _T("ALTRN_I input"));
  else
   ii.WriteInt(m_optIndAltrn_i[i], _T("Вход ALTRN_I"));
 }

 IniIO &ic = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteComment(_T("*** Colors for indicators (colors of the \"On\" state). Values are RGB in hex format ***\r\n; You can obtain any color by combining different values of R, G and B components. For example: FF0000 - red, 00FF00 - green, D3D350 - dim yellow. https://www.w3schools.com/colors/colors_picker.asp"), true);
 else
  ic.WriteComment(_T("*** Цвета индикаторов (цвет включенного состояния). RGB значения в hex формате ***\r\n; Вы можете получить любой цвет путем комбинирования значений R, G и B компонент. For example: FF0000 - красный, 00FF00 - зеленый, D3D350 - тусклый желтый. https://www.w3schools.com/colors/colors_picker.asp"), true);
 ic.CreateSection(m_Name_IndColors_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColGas_v, _T("GAS_V input"));
 else
  ic.WriteColor(m_optColGas_v, _T("Вход GAS_V"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColCarb, _T("CARB input"));
 else
  ic.WriteColor(m_optColCarb, _T("Вход CARB"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColIdleValve, _T("Idle cutoff valve"));
 else
  ic.WriteColor(m_optColIdleValve, _T("Клапан ЭПХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColPowerValve, _T("Power valve"));
 else
  ic.WriteColor(m_optColPowerValve, _T("Клапан ЭМР"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColStBlock, _T("Starter relay"));
 else
  ic.WriteColor(m_optColStBlock, _T("Реле блокировки стартера"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColAE, _T("Acceleration enrichment"));
 else
  ic.WriteColor(m_optColAE, _T("Обогащение при ускорении"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColCoolingFan, _T("Cooling fan"));
 else
  ic.WriteColor(m_optColCoolingFan, _T("Вентилятор охлаждения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColCE, _T("Check Engine"));
 else
  ic.WriteColor(m_optColCE, _T("Лампа Check Engine"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColFCRevLim, _T("Rev. limitter"));
 else
  ic.WriteColor(m_optColFCRevLim, _T("Ограничение оборотов"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColFloodClear, _T("Flood clear"));
 else
  ic.WriteColor(m_optColFloodClear, _T("Режим продувки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColSysLocked, _T("System locked"));
 else
  ic.WriteColor(m_optColSysLocked, _T("Система заблокирована"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColIgn_i, _T("IGN_I input"));
 else
  ic.WriteColor(m_optColIgn_i, _T("Вход IGN_I"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColCond_i, _T("COND_I input"));
 else
  ic.WriteColor(m_optColCond_i, _T("Вход COND_I"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColEpas_i, _T("EPAS_I input"));
 else
  ic.WriteColor(m_optColEpas_i, _T("Вход EPAS_I"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColAftStrEnr, _T("Afterstart enrichment"));
 else
  ic.WriteColor(m_optColAftStrEnr, _T("Обогащение после пуска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColIacClLoop, _T("IAC closed loop"));
 else
  ic.WriteColor(m_optColIacClLoop, _T("РХХ closed loop"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColUniOut1, _T("Universal output 1"));
 else
  ic.WriteColor(m_optColUniOut1, _T("Универсальный выход 1"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColUniOut2, _T("Universal output 2"));
 else
  ic.WriteColor(m_optColUniOut2, _T("Универсальный выход 2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColUniOut3, _T("Universal output 3"));
 else
  ic.WriteColor(m_optColUniOut3, _T("Универсальный выход 3"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColUniOut4, _T("Universal output 4"));
 else
  ic.WriteColor(m_optColUniOut4, _T("Универсальный выход 4"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColUniOut5, _T("Universal output 5"));
 else
  ic.WriteColor(m_optColUniOut5, _T("Универсальный выход 5"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColUniOut6, _T("Universal output 6"));
 else
  ic.WriteColor(m_optColUniOut6, _T("Универсальный выход 6"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColGpa4_i, _T("GPA4_I input"));
 else
  ic.WriteColor(m_optColGpa4_i, _T("Вход GPA4_I"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColInput1, _T("INPUT1 input"));
 else
  ic.WriteColor(m_optColInput1, _T("Вход INPUT1"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColInput2, _T("INPUT2 input"));
 else
  ic.WriteColor(m_optColInput2, _T("Вход INPUT2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColAuto_i, _T("AUTO_I input"));
 else
  ic.WriteColor(m_optColAuto_i, _T("Вход AUTO_I"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColMapsel0, _T("MAPSEL0 input"));
 else
  ic.WriteColor(m_optColMapsel0, _T("Вход MAPSEL0"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColRefprs_i, _T("REFPRS_I input"));
 else
  ic.WriteColor(m_optColRefprs_i, _T("Вход REFPRS_I"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ic.WriteColor(m_optColAltrn_i, _T("ALTRN_I input"));
 else
  ic.WriteColor(m_optColAltrn_i, _T("Вход ALTRN_I"));

 TCHAR* mm_comment[2];
 if (m_optInterfaceLang.value == IL_ENGLISH)
 {
  mm_comment[0] = _T("*** Configuration of virtual gauges (1-st profile) ***\r\n; Use MetRows to specify number of rows will be used for grouping of virtual gauges\r\n; Set index number for each gauge (order). Gauges will be displayed in direction from left-top to right-bottom according to set order.");
  mm_comment[1] = _T("*** Configuration of virtual gauges (2-nd profile) ***");
 }
 else
 {
  mm_comment[0] = _T("*** Конфигурация виртуальных приборов (1-й профиль) ***\r\n; Используйте MetRows, чтобы задать число строк, которое будет использовано для группировки приборов\r\n; Установите порядковый номер для каждого прибора. Приборы будут отображаться в направлении с левого-верхнего до правого-нижнего углов в соответствии с установленным порядком.");
  mm_comment[1] = _T("*** Конфигурация виртуальных приборов (2-й профиль) ***");
 }

 //Meters
 for(int i = 0; i < 2; ++i)
 {
  IniIO &mm = writer;
  mm.WriteComment(mm_comment[i], true);
  mm.CreateSection(m_Name_Meters_Section[i]);

  if (m_optInterfaceLang.value == IL_ENGLISH)
   mm.WriteInt(m_optMetRows[i], _T("Number of rows in the virtual gauges panel"));
  else
   mm.WriteInt(m_optMetRows[i], _T("Кол-во строк в панели виртуальных приборов"));

  for(int g = 0; g < 2; ++g)
  {
   mm.WriteInt(m_optMetRPM[i][g], strMetRPM);
   mm.WriteInt(m_optMetMAP[i][g], strMetMAP);
   mm.WriteInt(m_optMetVBat[i][g], strMetVBat);
   mm.WriteInt(m_optMetIgnTim[i][g], strMetIgnTim);
   mm.WriteInt(m_optMetTPS[i][g], strMetTPS);
   mm.WriteInt(m_optMetCLT[i][g], strMetCLT);
   mm.WriteInt(m_optMetAddI1[i][g], strMetAddI1);
   mm.WriteInt(m_optMetAddI2[i][g], strMetAddI2);
   mm.WriteInt(m_optMetInjPW[i][g], strMetInjPW);
   mm.WriteInt(m_optMetIAT[i][g], strMetIAT);
   mm.WriteInt(m_optMetEGOCorr[i][g], strMetEGOCorr);
   mm.WriteInt(m_optMetAirFlow[i][g], strMetAirFlow);
   mm.WriteInt(m_optMetVehicleSpeed[i][g], strMetVehicleSpeed);
   mm.WriteInt(m_optMetTPSDot[i][g], strMetTPSDot);
   mm.WriteInt(m_optMetMAPDot[i][g], strMetMAPDot);
   mm.WriteInt(m_optMetMAP2[i][g], strMetMAP2);
   mm.WriteInt(m_optMetGPS[i][g], strMetGPS);
   mm.WriteInt(m_optMetMAPD[i][g], strMetMAPD);
   mm.WriteInt(m_optMetTmp2[i][g], strMetTmp2);
   mm.WriteInt(m_optMetFuelConsum[i][g], strMetFuelConsum);
   mm.WriteInt(m_optMetKnockRetard[i][g], strMetKnockRetard);
   mm.WriteInt(m_optMetKnockGraph[i][g], strMetKnockGraph);
   mm.WriteInt(m_optMetSensAFR[i][g], strMetSensAFR);
   mm.WriteInt(m_optMetChokePos[i][g], strMetChokePos);
   mm.WriteInt(m_optMetGDPos[i][g], strMetGDPos);
   mm.WriteInt(m_optMetSynLoad[i][g], strMetSynLoad);
   mm.WriteInt(m_optMetInjTimB[i][g], strMetInjTimB);
   mm.WriteInt(m_optMetInjTimE[i][g], strMetInjTimE);
   mm.WriteInt(m_optMetFuelConsumF[i][g], strMetFuelConsumF);
   mm.WriteInt(m_optMetGrts[i][g], strMetGrts);
   mm.WriteInt(m_optMetFtls[i][g], strMetFtls);
   mm.WriteInt(m_optMetEgts[i][g], strMetEgts);
   mm.WriteInt(m_optMetOps[i][g], strMetOps);
   mm.WriteInt(m_optMetInjDuty[i][g], strMetInjDuty);
   mm.WriteInt(m_optMetMAF[i][g], strMetMAF);
   mm.WriteInt(m_optMetVentDuty[i][g], strMetVentDuty);
   mm.WriteInt(m_optMetFts[i][g], strMetFts);
   mm.WriteInt(m_optMetEGOCorr2[i][g], strMetEGOCorr2);
   mm.WriteInt(m_optMetSensAFR2[i][g], strMetSensAFR2);
   mm.WriteInt(m_optMetTargAFR[i][g], strMetTargAFR);
   mm.WriteInt(m_optMetDiffAFR[i][g], strMetDiffAFR);
   mm.WriteInt(m_optMetDiffAFR2[i][g], strMetDiffAFR2);
   mm.WriteInt(m_optMetFPS[i][g], strMetFPS);
  }
 }

 //Meters' scales
 IniIO &sc = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  sc.WriteComment(_T("*** Ranges of scales of virtual gauges ***\r\n; Two values per line - first for minimum, second for maximum"), true);
 else
  sc.WriteComment(_T("*** Диапазоны шкал виртуальных приборов ***\r\n; По два значения в каждой строке - первое для минимума, второе для максимума"), true);

 sc.CreateSection(m_Name_MetersSc_Section);

 for(int g = 0; g < 2; ++g)
 {
  sc.WriteScale(m_optMetScRPM[g], 0, strMetRPM);
  sc.WriteScale(m_optMetScMAP[g], 0, strMetMAP);
  sc.WriteScale(m_optMetScVBat[g], 1, strMetVBat);
  sc.WriteScale(m_optMetScIgnTim[g], 1, strMetIgnTim);
  sc.WriteScale(m_optMetScTPS[g], 1, strMetTPS);
  sc.WriteScale(m_optMetScCLT[g], 1, strMetCLT);
  sc.WriteScale(m_optMetScAddI1[g], 1, strMetAddI1);
  sc.WriteScale(m_optMetScAddI2[g], 1, strMetAddI2);
  sc.WriteScale(m_optMetScInjPW[g], 1, strMetInjPW);
  sc.WriteScale(m_optMetScIAT[g], 1, strMetIAT);
  sc.WriteScale(m_optMetScEGOCorr[g], 1, strMetEGOCorr);
  sc.WriteScale(m_optMetScAirFlow[g], 0, strMetAirFlow);
  sc.WriteScale(m_optMetScVehicleSpeed[g], 1, strMetVehicleSpeed);
  sc.WriteScale(m_optMetScTPSDot[g], 0, strMetTPSDot);
  sc.WriteScale(m_optMetScMAPDot[g], 0, strMetMAPDot);
  sc.WriteScale(m_optMetScMAP2[g], 1, strMetMAP2);
  sc.WriteScale(m_optMetScGPS[g], 1, strMetGPS);
  sc.WriteScale(m_optMetScMAPD[g], 1, strMetMAPD);
  sc.WriteScale(m_optMetScTmp2[g], 1, strMetTmp2);
  sc.WriteScale(m_optMetScFuelConsum[g], 1, strMetFuelConsum);
  sc.WriteScale(m_optMetScKnockRetard[g], 1, strMetKnockRetard);
  sc.WriteScale(m_optMetScKnockGraph[g], 1, strMetKnockGraph);
  sc.WriteScale(m_optMetScSensAFR[g], 1, strMetSensAFR);
  sc.WriteScale(m_optMetScChokePos[g], 1, strMetChokePos);
  sc.WriteScale(m_optMetScGDPos[g], 1, strMetGDPos);
  sc.WriteScale(m_optMetScSynLoad[g], 1, strMetSynLoad);
  sc.WriteScale(m_optMetScInjTimB[g], 1, strMetInjTimB);  //0...720
  sc.WriteScale(m_optMetScInjTimE[g], 1, strMetInjTimE);  //0...720
  sc.WriteScale(m_optMetScInjTimB1[g], 1, strMetInjTimB); //-360...360
  sc.WriteScale(m_optMetScInjTimE1[g], 1, strMetInjTimE); //-360...360
  sc.WriteScale(m_optMetScFuelConsumF[g], 1, strMetFuelConsumF);
  sc.WriteScale(m_optMetScGrts[g], 1, strMetGrts);
  sc.WriteScale(m_optMetScFtls[g], 1, strMetFtls);
  sc.WriteScale(m_optMetScEgts[g], 1, strMetEgts);
  sc.WriteScale(m_optMetScOps[g], 1, strMetOps);
  sc.WriteScale(m_optMetScInjDuty[g], 1, strMetInjDuty);
  sc.WriteScale(m_optMetScMAF[g], 1, strMetMAF);
  sc.WriteScale(m_optMetScVentDuty[g], 1, strMetVentDuty);
  sc.WriteScale(m_optMetScFts[g], 1, strMetFts);
  sc.WriteScale(m_optMetScEGOCorr2[g], 1, strMetEGOCorr2);
  sc.WriteScale(m_optMetScSensAFR2[g], 1, strMetSensAFR2);
  sc.WriteScale(m_optMetScTargAFR[g], 1, strMetTargAFR);
  sc.WriteScale(m_optMetScDiffAFR[g], 1, strMetDiffAFR);
  sc.WriteScale(m_optMetScDiffAFR2[g], 1, strMetDiffAFR2);
  sc.WriteScale(m_optMetScFPS[g], 1, strMetFPS);
 }

 IniIO &at = writer;

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("*** VE autotune settings ***"), true);
 else
  at.WriteComment(_T("*** Параметры автонастройки VE ***"), true);
 at.CreateSection(m_Name_AutoTune_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Lambda delay 3x3 map, packed in rows"));
 else
  at.WriteComment(_T("Таблица задержки лямбды 3x3, запакована построчно"));
 at.WriteVector(m_optLambdaDelay);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Load bins for lambda delay map"));
 else
  at.WriteComment(_T("Ячейки по оси нагрузки в таблице задержки лямбды"));
 at.WriteVector(m_optLambdaDelayL);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("RPM bins for lambda delay map"));
 else
  at.WriteComment(_T("Ячейки по оси оборотов в таблице задержки лямбды"));
 at.WriteVector(m_optLambdaDelayR);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Target AFR error for autotune algothithm"));
 else
  at.WriteComment(_T("Целевое соотношение ошибки воздух/топливо для алгоритма атонастройки"));
 at.WriteFlt(m_optAFRError, 2);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("List of blocked cells. Numbering starts from 0 (value at the left-bottom corner of table in the user's interface)"));
 else
  at.WriteComment(_T("Список заблокированных ячеек. Нумерация начинается с 0 (значение в левом-верхнем углу таблицы в окне автонастройки)"));
 at.WriteVector(m_optBlockedCells);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Number of sample points (statistics) per cell"));
 else
  at.WriteComment(_T("Число выборок на одну ячейку (статистика)"));
 at.WriteInt(m_optStatSize);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Cells will be automatically blocked if theirs weight is greater or equal to the specified value. Set to zero if you want to disable this feature. "));
 else
  at.WriteComment(_T("Ячейки будут автоматически блокироваться, если их веса больше или равны указанному значению. Установите в 0, если вы хотите запретить эту функцию."));
 at.WriteInt(m_optAutoBlockThrd);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("If set to 1, then statistics will be gathered inly when RPM is rising. Set to 0 for disabling of this feature."));
 else
  at.WriteComment(_T("Если установлено в 1, то статистика будет собираться только если обороты растут. Установите в 0 для запрещения этой функции."));
 at.WriteInt(m_optGrowingMode);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Sample points (statistics) will be gathered only if measured AFR if greater or equal to the specified value"));
 else
  at.WriteComment(_T("Статистика будет собираться только если фактическое соотношение воздух/топливо больше или равно указанному значению"));
 at.WriteFlt(m_optMinAFR, 1);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Sample points (statistics) will be gathered only if measured AFR if less or equal to the specified value"));
 else
  at.WriteComment(_T("Статистика будет собираться только если фактическое соотношение воздух/топливо меньше или равно указанному значению"));
 at.WriteFlt(m_optMaxAFR, 1);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Threshold for the number of matches between real and target AFR, after reaching which cell's statistics will updated only if the average distance of new statistics points is less than the existing ones."));
 else
  at.WriteComment(_T("Порог количества совпадений между фактическим и целевым соотношениями воздух/топливо, после достижения которого статистика ячейки будет обновляться если среднее расстояние новых точек статистики меньше чем то, которое уже имеет ячейка."));
 at.WriteInt(m_optMinDistThrd);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Sample points (statistics) will be gathered only if TPS if greater or equal to the specified value"));
 else
  at.WriteComment(_T("Статистика будет собираться только если ДПДЗ больше или равно указанному значению"));
 at.WriteFlt(m_optMinTPS, 1);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Sample points (statistics) will be gathered only if TPS if less or equal to the specified value"));
 else
  at.WriteComment(_T("Статистика будет собираться только если ДПДЗ меньше или равно указанному значению"));
 at.WriteFlt(m_optMaxTPS, 1);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("Automatic tune will not start while CLT is less than following value."));
 else
  at.WriteComment(_T("Автонастройка не начнется пока ДТОЖ меньше чем указанное значение."));
 at.WriteFlt(m_optCLTThrd, 1);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  at.WriteComment(_T("The softness coefficient of automatic tuning the values in table. The higher the value, the slower the auto-tuning takes place, but the higher the error tolerance."));
 else
  at.WriteComment(_T("Коэффициент мягкости откатки значений в таблице. Чем выше значение, тем медленнее происходит автонастройка, но при этом стойкость к ошибкам выше."));
 at.WriteFlt(m_optTunSoftness, 2);

 //Map editor
 IniIO &me = writer;

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("*** Grid map editors ***"), true);
 else
  me.WriteComment(_T("*** Редактирование в виде таблиц ***"), true);
 me.CreateSection(m_Name_MapEditor_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("Color saturation of gradient"));
 else
  me.WriteComment(_T("Насыщенность цвета градиента"));
 me.WriteInt(m_optGradSaturation);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("Brightness of gradient"));
 else
  me.WriteComment(_T("Яркость градиента"));
 me.WriteInt(m_optGradBrightness);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("Use bold fonts. Set to 1 for bold fonts, set to 0 for normal fonts."));
 else
  me.WriteComment(_T("Использовать жирные шрифты. Установите в 1 для жирных шрифтов или в 0 для обычных"));
 me.WriteInt(m_optBoldFont);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("Editing mode of the inj. timing map. 0 - [0...720 BTDC], 1 - [0...720 ATDC], 2 - [-360...+360 BTDC], 3 - [-360...+360 ATDC]"));
 else
  me.WriteComment(_T("Режим редактирования таблицы фазы впрыска. 0 - [0...720 до ВМТ], 1 - [0...720 после ВМТ], 2 - [-360...360 до ВМТ], 3 - [-360...360 после ВМТ]"));
 me.WriteInt(m_optITEdMode);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("Use spot markers instead of rectangles in grid mode map eitors. 0 - use rectangles, 1 - use spots"));
 else
  me.WriteComment(_T("Использовать пятно для индикации рабочей точки вместо рамок в окнах табличного редактирования. 0 - использовать рамки, 1 - использовать пятно"));
 me.WriteInt(m_optSpotMarkers);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("Size of spot used for indication of working point in grid editors"));
 else
  me.WriteComment(_T("Размер пятна используемого для индикации рабочей точки в окнах табличного редактирования"));
 me.WriteFlt(m_optSpotMarkersSize, 2);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("Current VE map selected for editing in the grid mode map editor's window. 0 - VE1, 1 - VE2, 2 - Idling VE"));
 else
  me.WriteComment(_T("Таблица наполнения, которая выбрана для редактирования в окне табличного редактирования карт. 0 - VE1, 1 - VE2, 2 - VE ХХ"));
 me.WriteInt(m_optActiveVEMap);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  me.WriteComment(_T("Use markers (spot or rectangle) to indicate current working point in grid mode map eitors. 0 - don't use, 1 - use."));
 else
  me.WriteComment(_T("Использовать индикацию рабочей точки (пятно или рамка) в окнах табличного редактирования. 0 - не использовать, 1 - использовать."));
 me.WriteInt(m_optUseMarkers);

 //Splitters
 IniIO &sp = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  sp.WriteComment(_T("*** Remembered positions of splitters ***"), true);
 else
  sp.WriteComment(_T("*** Запомненные положения сплиттеров ***"), true);
 sp.CreateSection(m_Name_Splitters_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sp.WriteComment(_T("Vertical splitter on the \"Parameters and monitor\" tab"));
 else
  sp.WriteComment(_T("Вертикальный сплиттер на вкладке \"Параметры и монитор\""));
 sp.WriteInt(m_optParamMonVert);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  sp.WriteComment(_T("Vertical splitter on the \"Player\" tab"));
 else
  sp.WriteComment(_T("Вертикальный сплиттер на вкладке \"Проигрыватель\""));
 sp.WriteInt(m_optLogPlayerVert);

 //Inj.driver
 IniIO &dr = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteComment(_T("*** Settings related to \"Injector Driver\" tab ***"), true);
 else
  dr.WriteComment(_T("*** Настройки для вкладки \"Драйвер форсунок\" ***"), true);
 dr.CreateSection(m_Name_InjDrv_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteInt(m_optInjDrvTabActive, _T("Enable \"Inj. driver\" tab. Set to 1 if you want to enable mentioned tab, 0 - disable."));
 else
  dr.WriteInt(m_optInjDrvTabActive, _T("Разрешение вкладки \"Драйвер форсунок\". Поставьте 1 для разрешения вкладки, 0 - запрещение."));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteColor(m_optVoltLineColor, _T("Color of the voltage line on graphs"));
 else
  dr.WriteColor(m_optVoltLineColor, _T("Цвет линии напряжения на графиках"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteFlt(m_optPeakOnPtMovStep, 0, _T("Points moving step for the \"Peak on time\" graph"));
 else
  dr.WriteFlt(m_optPeakOnPtMovStep, 0, _T("Шаг смещения точек для графика \"Время полного включения\""));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteFlt(m_optPeakDutyPtMovStep, 1, _T("Points moving step for the \"Peak PWM duty\" graph"));
 else
  dr.WriteFlt(m_optPeakDutyPtMovStep, 1, _T("Шаг смещения точек для графика \"Скважность полного включения\""));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteFlt(m_optHoldDutyPtMovStep, 1, _T("Points moving step for the \"Hold PWM duty\" graph"));
 else
  dr.WriteFlt(m_optHoldDutyPtMovStep, 1, _T("Шаг смещения точек для графика \"Скважность удерживания\""));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteFlt(m_optPeakFullPtMovStep, 0, _T("Points moving step for the \"Peak ON + PWM time\" graph"));
 else
  dr.WriteFlt(m_optPeakFullPtMovStep, 0, _T("Шаг смещения точек для графика \"Время Вкл + Время ШИМ\""));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteFlt(m_optPthPausePtMovStep, 0, _T("Points moving step for the \"Peak-to-hold pause\" graph"));
 else
  dr.WriteFlt(m_optPthPausePtMovStep, 0, _T("Шаг смещения точек для графика \"Пауза включ. - удерж.\""));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteInt(m_optShowGraphLabels, _T("Show labels over handles on graphs. 1 - show, 0 - don't show."));
 else
  dr.WriteInt(m_optShowGraphLabels, _T("Показывать надписи над узловыми точками графиков. 1 - показывать, 0 - не показывать."));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteFlt(m_optPWMulPtMovStep, 2, _T("Points moving step for the \"PW mul\" graph"));
 else
  dr.WriteFlt(m_optPWMulPtMovStep, 2, _T("Шаг смещения точек для графика \"Множитель\""));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  dr.WriteFlt(m_optPWAddPtMovStep, 1, _T("Points moving step for the \"PW add\" graph"));
 else
  dr.WriteFlt(m_optPWAddPtMovStep, 1, _T("Шаг смещения точек для графика \"Добавка\""));

 //Moving step of points in maps' editing windows
 IniIO &ms = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteComment(_T("*** Step of points' moving in map editor windows ***"), true);
 else
  ms.WriteComment(_T("*** Шаг смещения точек в окнах редактирования таблиц ***"), true);
 ms.CreateSection(m_Name_MapPtMovStep_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepWorkMap, 3, _T("Work ignition timing map"));
 else
  ms.WriteFlt(m_optPtMovStepWorkMap, 3, _T("Рабочая карта УОЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepTempMap, 3, _T("Ign. timing vs CLT correction map"));
 else
  ms.WriteFlt(m_optPtMovStepTempMap, 3, _T("Коррекция УОЗ по ДТОЖ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepTempIdlMap, 3, _T("Ign. timing vs CLT correction map (idling)"));
 else
  ms.WriteFlt(m_optPtMovStepTempIdlMap, 3, _T("Коррекция УОЗ по ДТОЖ (для ХХ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepStartMap, 3, _T("Cranking ignition timing map"));
 else
  ms.WriteFlt(m_optPtMovStepStartMap, 3, _T("Таблица УОЗ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepIdleMap, 3, _T("Idling ignition timing map"));
 else
  ms.WriteFlt(m_optPtMovStepIdleMap, 3, _T("Таблица УОЗ на ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepVeMap, 3, _T("VE map"));
 else
  ms.WriteFlt(m_optPtMovStepVeMap, 3, _T("Наполнение (объемный КПД)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepVe2Map, 3, _T("VE map 2"));
 else
  ms.WriteFlt(m_optPtMovStepVe2Map, 3, _T("Наполнение (объемный КПД) 2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepVeiMap, 3, _T("VE map for idling"));
 else
  ms.WriteFlt(m_optPtMovStepVeiMap, 3, _T("Наполнение (объемный КПД) для ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAfrMap, 3, _T("AFR map"));
 else
  ms.WriteFlt(m_optPtMovStepAfrMap, 3, _T("Соотношение Воздух/топливо"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepCrnkMap, 3, _T("Injection PW on cranking"));
 else
  ms.WriteFlt(m_optPtMovStepCrnkMap, 3, _T("Длительность впрыска на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepWrmpMap, 3, _T("Warmup enrichment map"));
 else
  ms.WriteFlt(m_optPtMovStepWrmpMap, 3, _T("Обогащение при прогреве"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepDeadMap, 3, _T("Injector's lag map"));
 else
  ms.WriteFlt(m_optPtMovStepWrmpMap, 3, _T("Лаг форсунки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepIdlrMap, 3, _T("IAC position (working)"));
 else
  ms.WriteFlt(m_optPtMovStepIdlrMap, 3, _T("Положение РДВ (рабочее)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepIdlcMap, 3, _T("IAC position (cranking)"));
 else
  ms.WriteFlt(m_optPtMovStepIdlcMap, 3, _T("Положение РДВ (на пуске)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepThrassMap, 3, _T("Throttle assist"));
 else
  ms.WriteFlt(m_optPtMovStepThrassMap, 3, _T("Сопроводитель дросселя"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAetpsMap, 3, _T("Enrichment vs throttle speed map"));
 else
  ms.WriteFlt(m_optPtMovStepAetpsMap, 3, _T("Обогащение по скорости ДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAemapMap, 3, _T("Enrichment vs MAPdot"));
 else
  ms.WriteFlt(m_optPtMovStepAemapMap, 3, _T("Обогащение по скорости ДАД"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAerpmMap, 3, _T("Enrichment vs engine speed"));
 else
  ms.WriteFlt(m_optPtMovStepAerpmMap, 3, _T("Обогащение по оборотам"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAftstrMap, 3, _T("Afterstart enrichment map"));
 else
  ms.WriteFlt(m_optPtMovStepAftstrMap, 3, _T("Обогащение после пуска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepItMap, 3, _T("Injection timing map"));
 else
  ms.WriteFlt(m_optPtMovStepItMap, 3, _T("Фаза впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepItrpmMap, 3, _T("Target idling RPM"));
 else
  ms.WriteFlt(m_optPtMovStepItrpmMap, 3, _T("Целевые обороты ХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepRigidMap, 3, _T("Idling regulator's rigidity map"));
 else
  ms.WriteFlt(m_optPtMovStepRigidMap, 3, _T("Жесткость РХХ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepEgocrvMap, 3, _T("EGO sensor curve"));
 else
  ms.WriteFlt(m_optPtMovStepEgocrvMap, 3, _T("Кривая датчика кислорода"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepIaccMap, 3, _T("Mixture correction vs IAC pos"));
 else
  ms.WriteFlt(m_optPtMovStepIaccMap, 3, _T("Коррекция смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepIaccwMap, 3, _T("Weight of mixture correction vs IAC pos"));
 else
  ms.WriteFlt(m_optPtMovStepIaccwMap, 3, _T("Вес коррекции смеси по положению РДВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepIatcltMap, 3, _T("CLT to MAT influence factor"));
 else
  ms.WriteFlt(m_optPtMovStepIatcltMap, 3, _T("Коэффициент влияния ТОЖ на ТВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepTpsswtMap, 3, _T("TPS switch point map"));
 else
  ms.WriteFlt(m_optPtMovStepTpsswtMap, 3, _T("Точка переключения по ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepGtscMap, 3, _T("Gas temperature correction map"));
 else
  ms.WriteFlt(m_optPtMovStepGtscMap, 3, _T("Коррекция смеси по температуре газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepGpscMap, 3, _T("Gas pressure correction map"));
 else
  ms.WriteFlt(m_optPtMovStepGpscMap, 3, _T("Коррекция смеси по давлению газа"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAtscMap, 3, _T("Air density correction map"));
 else
  ms.WriteFlt(m_optPtMovStepAtscMap, 3, _T("Коррекция смеси по плотности воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepPwm1Map, 3, _T("PWM1 duty map"));
 else
  ms.WriteFlt(m_optPtMovStepPwm1Map, 3, _T("Скважность ШИМ на выходе PWM1"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepPwm2Map, 3, _T("PWM2 duty map"));
 else
  ms.WriteFlt(m_optPtMovStepPwm2Map, 3, _T("Скважность ШИМ на выходе PWM2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepIACMATMap, 3, _T("IAC position's correction vs MAT map"));
 else
  ms.WriteFlt(m_optPtMovStepIACMATMap, 3, _T("Коррекция положения РДВ от температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepTpszonMap, 3, _T("Load axis allocation between MAP and TPS"));
 else
  ms.WriteFlt(m_optPtMovStepTpszonMap, 3, _T("Распределение оси нагрузки между ДАД и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepCylMultMap, 3, _T("Per cylinder injection PW multiplier"));
 else
  ms.WriteFlt(m_optPtMovStepCylMultMap, 3, _T("Поцилиндровый множитель длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepCylAddMap, 3, _T("Per cylinder injection PW addition"));
 else
  ms.WriteFlt(m_optPtMovStepCylAddMap, 3, _T("Поцилиндровая добавка к длительности впрыска"));

 //separate maps
 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepCrkTempMap, 3, _T("Ign.timing vs CLT correction on cranking"));
 else
  ms.WriteFlt(m_optPtMovStepCrkTempMap, 3, _T("Коррекция УОЗ по ДТОЖ на пуске"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepEHPauseMap, 3, _T("EGO heater's pause map (for PWM)"));
 else
  ms.WriteFlt(m_optPtMovStepEHPauseMap, 3, _T("Время паузы подогрева ДК (для ШИМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAttenuatorMap, 3, _T("Attenuator's curve of amplification"));
 else
  ms.WriteFlt(m_optPtMovStepAttenuatorMap, 3, _T("Таблица функции аттенюатора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepDwellCntrlMap, 3, _T("Dwell time map"));
 else
  ms.WriteFlt(m_optPtMovStepDwellCntrlMap, 3, _T("Время накопления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepCTSCurveMap, 3, _T("Coolant temperature sensor table"));
 else
  ms.WriteFlt(m_optPtMovStepCTSCurveMap, 3, _T("Кривая датчика температуры охлажд.житкости"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepBarocorrMap, 3, _T("Barometric correction map"));
 else
  ms.WriteFlt(m_optPtMovStepBarocorrMap, 3, _T("Барометрическая коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepManIgntimMap, 3, _T("Manual ignition timing corr. map"));
 else
  ms.WriteFlt(m_optPtMovStepManIgntimMap, 3, _T("Ручная октан-коррекция"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepATSCurvMap, 3, _T("MAT sensor table"));
 else
  ms.WriteFlt(m_optPtMovStepATSCurvMap, 3, _T("Кривая датчика температуры воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepATSCorrMap, 3, _T("Ign.timing correction vs MAT map"));
 else
  ms.WriteFlt(m_optPtMovStepATSCorrMap, 3, _T("Коррекция УОЗ по температуре воздуха"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepGasdoseMap, 3, _T("Gas valve position vs RPM,TPS map"));
 else
  ms.WriteFlt(m_optPtMovStepGasdoseMap, 3, _T("Положение дозатора газа от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepTmp2CurveMap, 3, _T("TMP2 sensor table"));
 else
  ms.WriteFlt(m_optPtMovStepTmp2CurveMap, 3, _T("Кривая датчика температуры на входе TMP2"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepCrankingThrdMap, 3, _T("Starter's bloking RPM"));
 else
  ms.WriteFlt(m_optPtMovStepCrankingThrdMap, 3, _T("Обороты блокировки стартера"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepCrankingTimeMap, 3, _T("Starter's blocking delay (strokes)"));
 else
  ms.WriteFlt(m_optPtMovStepCrankingTimeMap, 3, _T("Время до блокировки стартера после превышения порога оборотов (такты)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepSmapabanThrdMap, 3, _T("RPM threshold for abandon from start map"));
 else
  ms.WriteFlt(m_optPtMovStepSmapabanThrdMap, 3, _T("Обороты перехода с пусковой карты"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepKnockZoneMap, 3, _T("Knock zones vs RPM,TPS map"));
 else
  ms.WriteFlt(m_optPtMovStepKnockZoneMap, 3, _T("Зоны детонации в зависимости от оборотов и ДПДЗ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepGrtsCurveMap, 3, _T("GRTEMP sensor table"));
 else
  ms.WriteFlt(m_optPtMovStepGrtsCurveMap, 3, _T("Кривая датчика температуры на входе GRTEMP"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepGrHeatDutyMap, 3, _T("PWM duty map for controlling of gas reducer's heater"));
 else
  ms.WriteFlt(m_optPtMovStepGrHeatDutyMap, 3, _T("Скважность ШИМ для управления нагревателем газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepPwmIacUCoefMap, 3, _T("PWM IAC duty coefficient vs voltage"));
 else
  ms.WriteFlt(m_optPtMovStepPwmIacUCoefMap, 3, _T("Коэффициент скважности ШИМ РДВ от напряжения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAftstrStrk0Map, 3, _T("After start enrichment strokes vs coolant temperature (petrol)"));
 else
  ms.WriteFlt(m_optPtMovStepAftstrStrk0Map, 3, _T("Число тактов обогащения после пуска от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepAftstrStrk1Map, 3, _T("After start enrichment strokes vs coolant temperature (gas)"));
 else
  ms.WriteFlt(m_optPtMovStepAftstrStrk1Map, 3, _T("Число тактов обогащения после пуска от температуры двигателя (газ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepGrValDelMap, 2, _T("Gas valve opening delay vs gas reducer's temperature map"));
 else
  ms.WriteFlt(m_optPtMovStepGrValDelMap, 2, _T("Задержка включения газового клапана от температуры газового редуктора"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepFtlsCurveMap, 2, _T("Fuel tank level sensor's table"));
 else
  ms.WriteFlt(m_optPtMovStepFtlsCurveMap, 2, _T("Кривая датчика уровня топлива (ДУТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepEgtsCurveMap, 1, _T("Exhaust gas temperature sensor's table"));
 else
  ms.WriteFlt(m_optPtMovStepEgtsCurveMap, 1, _T("Кривая датчика температуры выхлопных газов (ДТВГ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepOpsCurveMap, 2, _T("Oil pressure sensor's table"));
 else
  ms.WriteFlt(m_optPtMovStepOpsCurveMap, 2, _T("Кривая датчика давления масла (ДДМ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepManInjPwcMap, 3, _T("Map for manual correction of injection PW"));
 else
  ms.WriteFlt(m_optPtMovStepManInjPwcMap, 3, _T("Таблица для ручной коррекции длительности впрыска"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepMAFCurveMap, 3, _T("MAF flow curve"));
 else
  ms.WriteFlt(m_optPtMovStepMAFCurveMap, 3, _T("Кривая расхода ДМРВ"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepFtlsCorMap, 2, _T("Fuel tank level sensor's correction vs board voltge table"));
 else
  ms.WriteFlt(m_optPtMovStepFtlsCorMap, 2, _T("Таблица коррекции датчика уровня топлива (ДУТ) от напряжения бортовой сети"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepLambdaZoneMap, 3, _T("EGO correction zones vs RPM,load map"));
 else
  ms.WriteFlt(m_optPtMovStepLambdaZoneMap, 3, _T("Зоны лямбда-коррекции в зависимости от оборотов и нагрузки"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepFtsCurveMap, 1, _T("Fuel temperature sensor's table"));
 else
  ms.WriteFlt(m_optPtMovStepFtsCurveMap, 1, _T("Кривая датчика температуры топлива (ДТТ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepFuelDensCorrMap, 1, _T("Fuel density correction map"));
 else
  ms.WriteFlt(m_optPtMovStepFuelDensCorrMap, 1, _T("Таблица коррекции плотности топлива"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepXtauXfAccMap, 1, _T("Coefficient of injected fuel which goes into port wall's film (acceleration)"));
 else
  ms.WriteFlt(m_optPtMovStepXtauXfAccMap, 1, _T("Коэффициент топлива выпадающего в пленку для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepXtauXfDecMap, 1, _T("Coefficient of injected fuel which goes into port wall's film (deceleration)"));
 else
  ms.WriteFlt(m_optPtMovStepXtauXfDecMap, 1, _T("Коэффициент топлива выпадающего в пленку для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepXtauTfAccMap, 1, _T("Constant of fuel's film dissipation time (accelaration)"));
 else
  ms.WriteFlt(m_optPtMovStepXtauTfAccMap, 1, _T("Постоянная времени испарения плeнки для режима ускорения"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepXtauTfDecMap, 1, _T("Constant of fuel's film dissipation time (decelaration)"));
 else
  ms.WriteFlt(m_optPtMovStepXtauTfDecMap, 1, _T("Постоянная времени испарения плeнки для режима замедления"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepInjNonLinPMap, 1, _T("Injector's non-linearity correction map used on small inj. PWs (petrol)"));
 else
  ms.WriteFlt(m_optPtMovStepInjNonLinPMap, 1, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepInjNonLinGMap, 1, _T("Injector's non-linearity correction map used on small inj. PWs (gas)"));
 else
  ms.WriteFlt(m_optPtMovStepInjNonLinGMap, 1, _T("Таблица коррекции нелинейности форсунки при малых длительностях впрыска (газ)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepEGODelayMap, 3, _T("Number of strokes of EGO delay vs engine load"));
 else
  ms.WriteFlt(m_optPtMovStepEGODelayMap, 3, _T("Число тактов задержки ДК в зависимости от нагрузки на двигатель"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepWUAFR0Map, 3, _T("AFR vs coolant temperature (petrol)"));
 else
  ms.WriteFlt(m_optPtMovStepWUAFR0Map, 3, _T("Соотношение Воздух/топливо от температуры двигателя (бензин)"));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  ms.WriteFlt(m_optPtMovStepWUAFR1Map, 3, _T("AFR vs coolant temperature (gas)"));
 else
  ms.WriteFlt(m_optPtMovStepWUAFR1Map, 3, _T("Соотношение Воздух/топливо от температуры двигателя (газ)"));

 //Log file's fileds
 IniIO &lf = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  lf.WriteComment(_T("*** Titles of fileds of the log file, which are written into the first line ***"), true);
 else
  lf.WriteComment(_T("*** Названия полей в лог файле, которые пишутся в первую строку ***"), true);
 lf.CreateSection(m_Name_LogFileFields_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  lf.WriteInt(m_optWriteLogFields, _T("Write title into a first line of the log file. Set to 1 to enable this."));
 else
  lf.WriteInt(m_optWriteLogFields, _T("Записывать заголовок в первую строку лог файла. Установите в 1 для разрешения."));

 lf.WriteString(m_optLogFieldTime);
 lf.WriteString(m_optLogFieldRPM);
 lf.WriteString(m_optLogFieldIgnTim);
 lf.WriteString(m_optLogFieldMAP);
 lf.WriteString(m_optLogFieldVBat);
 lf.WriteString(m_optLogFieldCLT);
 lf.WriteString(m_optLogFieldKnock);
 lf.WriteString(m_optLogFieldKnockCorr);
 lf.WriteString(m_optLogFieldLoadCurve);
 lf.WriteString(m_optLogFieldCarb);
 lf.WriteString(m_optLogFieldGas_v);
 lf.WriteString(m_optLogFieldIdleValve);
 lf.WriteString(m_optLogFieldPowerValve);
 lf.WriteString(m_optLogFieldCoolingFan);
 lf.WriteString(m_optLogFieldStBlock);
 lf.WriteString(m_optLogFieldAE);
 lf.WriteString(m_optLogFieldFCRevLim);
 lf.WriteString(m_optLogFieldFloodClear);
 lf.WriteString(m_optLogFieldSysLocked);
 lf.WriteString(m_optLogFieldCE);
 lf.WriteString(m_optLogFieldIgn_i);
 lf.WriteString(m_optLogFieldCond_i);
 lf.WriteString(m_optLogFieldEpas_i);
 lf.WriteString(m_optLogFieldGpa4_i);
 lf.WriteString(m_optLogFieldInput1); 
 lf.WriteString(m_optLogFieldInput2);
 lf.WriteString(m_optLogFieldAuto_i);
 lf.WriteString(m_optLogFieldMapsel0);
 lf.WriteString(m_optLogFieldRefprs_i);
 lf.WriteString(m_optLogFieldAltrn_i);
 lf.WriteString(m_optLogFieldAftStrEnr);
 lf.WriteString(m_optLogFieldIacClLoop);
 lf.WriteString(m_optLogFieldTPS);
 lf.WriteString(m_optLogFieldAdd_i1);
 lf.WriteString(m_optLogFieldAdd_i2);
 lf.WriteString(m_optLogFieldChokePos);
 lf.WriteString(m_optLogFieldGDPos);
 lf.WriteString(m_optLogFieldVehSpeed);
 lf.WriteString(m_optLogFieldPasDist);
 lf.WriteString(m_optLogFieldFuelConsum);
 lf.WriteString(m_optLogFieldFuelConsFreq);
 lf.WriteString(m_optLogFieldIAT);
 lf.WriteString(m_optLogFieldStrtIgnTim);
 lf.WriteString(m_optLogFieldIdleIgnTim);
 lf.WriteString(m_optLogFieldWorkIgnTim);
 lf.WriteString(m_optLogFieldTempIgnTim);
 lf.WriteString(m_optLogFieldIATIgnTim);
 lf.WriteString(m_optLogFieldIdlRegIgnTim);
 lf.WriteString(m_optLogFieldOctanCorr);
 lf.WriteString(m_optLogFieldEGOCorr);
 lf.WriteString(m_optLogFieldInjPW);
 lf.WriteString(m_optLogFieldTPSdot);
 lf.WriteString(m_optLogFieldMAP2);
 lf.WriteString(m_optLogFieldTmp2);
 lf.WriteString(m_optLogFieldDiffMAP);
 lf.WriteString(m_optLogFieldAFR);
 lf.WriteString(m_optLogFieldSynLoad);
 lf.WriteString(m_optLogFieldBaroPress);
 lf.WriteString(m_optLogFieldInjTimBeg);
 lf.WriteString(m_optLogFieldInjTimEnd);
 lf.WriteString(m_optLogFieldGrts);
 lf.WriteString(m_optLogFieldFtls);
 lf.WriteString(m_optLogFieldEgts);
 lf.WriteString(m_optLogFieldOps);
 lf.WriteString(m_optLogFieldInjDuty);
 lf.WriteString(m_optLogFieldRigidArg);
 lf.WriteString(m_optLogFieldRxlaf);
 lf.WriteString(m_optLogFieldMAF);
 lf.WriteString(m_optLogFieldVentDuty);
 lf.WriteString(m_optLogFieldUniOuts);
 lf.WriteString(m_optLogFieldMAPdot);
 lf.WriteString(m_optLogFieldFts);
 lf.WriteString(m_optLogFieldConsFuel);
 lf.WriteString(m_optLogFieldEGOCorr2);
 lf.WriteString(m_optLogFieldAFR2);
 lf.WriteString(m_optLogFieldAFRMap);
 lf.WriteString(m_optLogFieldTchrg);
 lf.WriteString(m_optLogFieldLogMarks);
 lf.WriteString(m_optLogFieldServFlag);
 lf.WriteString(m_optLogFieldCECodes);
 lf.WriteString(m_optLogFieldGPS);
 lf.WriteString(m_optLogFieldFPS);
 //Functionality
 IniIO &fn = writer;
 if (m_optInterfaceLang.value == IL_ENGLISH)
  fn.WriteComment(_T("*** Control of functionality (overlapping of firmware options) ***"), true);
 else
  fn.WriteComment(_T("*** Управление функциональностью (дублируются опции прошивки) ***"), true);
 fn.CreateSection(m_Name_Functionality_Section);

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fn.WriteInt(m_optFuncSM_CONTROL, _T("Enable automatic choke functionality. Set to 1 to enable (0 - for disabling)"));
 else
  fn.WriteInt(m_optFuncSM_CONTROL, _T("Разрешение функциональности воздушной заслонки. Установите в 1 для разрешения (0 для запрещения)."));

 if (m_optInterfaceLang.value == IL_ENGLISH)
  fn.WriteInt(m_optFuncGD_CONTROL, _T("Enable gas dispenser functionality. Set to 1 to enable (0 - for disabling)"));
 else
  fn.WriteInt(m_optFuncGD_CONTROL, _T("Разрешение функциональности дозатора газа. Установите в 1 для разрешения (0 для запрещения)."));

 return status;
}

const _TSTRING& CAppSettingsModel::GetPortName(void) const
{
 return m_optPortName.value;
}

void CAppSettingsModel::SetPortName(const _TSTRING& name)
{
 m_optPortName.value = name;
}

DWORD CAppSettingsModel::GetBaudRateApplication(void) const
{
 return m_optBaudRateApplication.value;
}

DWORD CAppSettingsModel::GetBaudRateBootloader(void) const
{
 return m_optBaudRateBootloader.value;
}

const _TSTRING& CAppSettingsModel::GetLogFilesFolder(void) const
{
 return m_optLogFilesFolder.value;
}

bool CAppSettingsModel::GetUseAppFolder(void) const
{
 return m_optUseAppFolder.value;
}

bool CAppSettingsModel::GetAlwaysWriteLog(void) const
{
 return m_optAlwaysWriteLog.value;
}

char CAppSettingsModel::GetCSVSepSymbol(void) const
{
 return m_optCSVSepSymbol.value;
}

char CAppSettingsModel::GetMapCSVSepSymbol(void) const
{
 return m_optMapCSVSepSymbol.value;
}

int CAppSettingsModel::GetMIDeskUpdatePeriod(void) const
{
 return m_optMIDeskUpdatePeriod.value;
}

void CAppSettingsModel::SetWndSettings(const WndSettings& i_wndSettings)
{
 m_optMainFrmWnd.value.x = i_wndSettings.m_MainFrmWnd_X;
 m_optMainFrmWnd.value.y = i_wndSettings.m_MainFrmWnd_Y;
 m_optStrtMapWnd.value.x = i_wndSettings.m_StrtMapWnd_X;
 m_optStrtMapWnd.value.y = i_wndSettings.m_StrtMapWnd_Y;
 m_optIdleMapWnd.value.x = i_wndSettings.m_IdleMapWnd_X;
 m_optIdleMapWnd.value.y = i_wndSettings.m_IdleMapWnd_Y;
 m_optWorkMapWnd.value.x = i_wndSettings.m_WorkMapWnd_X;
 m_optWorkMapWnd.value.y = i_wndSettings.m_WorkMapWnd_Y;
 m_optTempMapWnd.value.x = i_wndSettings.m_TempMapWnd_X;
 m_optTempMapWnd.value.y = i_wndSettings.m_TempMapWnd_Y;
 m_optTempIdlMapWnd.value.x = i_wndSettings.m_TempIdlMapWnd_X;
 m_optTempIdlMapWnd.value.y = i_wndSettings.m_TempIdlMapWnd_Y;
 m_optGridMapIgnWnd.value.x = i_wndSettings.m_GridMapIgnWnd_X;
 m_optGridMapIgnWnd.value.y = i_wndSettings.m_GridMapIgnWnd_Y;
 m_optGridMapInjWnd.value.x = i_wndSettings.m_GridMapInjWnd_X;
 m_optGridMapInjWnd.value.y = i_wndSettings.m_GridMapInjWnd_Y;
 m_optVEMapWnd.value.x = i_wndSettings.m_VEMapWnd_X;
 m_optVEMapWnd.value.y = i_wndSettings.m_VEMapWnd_Y;
 m_optVE2MapWnd.value.x = i_wndSettings.m_VE2MapWnd_X;
 m_optVE2MapWnd.value.y = i_wndSettings.m_VE2MapWnd_Y;
 m_optVEIMapWnd.value.x = i_wndSettings.m_VEIMapWnd_X;
 m_optVEIMapWnd.value.y = i_wndSettings.m_VEIMapWnd_Y;
 m_optAFRMapWnd.value.x = i_wndSettings.m_AFRMapWnd_X;
 m_optAFRMapWnd.value.y = i_wndSettings.m_AFRMapWnd_Y;
 m_optCrnkMapWnd.value.x = i_wndSettings.m_CrnkMapWnd_X;
 m_optCrnkMapWnd.value.y = i_wndSettings.m_CrnkMapWnd_Y;
 m_optWrmpMapWnd.value.x = i_wndSettings.m_WrmpMapWnd_X;
 m_optWrmpMapWnd.value.y = i_wndSettings.m_WrmpMapWnd_Y;
 m_optDeadMapWnd.value.x = i_wndSettings.m_DeadMapWnd_X;
 m_optDeadMapWnd.value.y = i_wndSettings.m_DeadMapWnd_Y;
 m_optIdlrMapWnd.value.x = i_wndSettings.m_IdlrMapWnd_X;
 m_optIdlrMapWnd.value.y = i_wndSettings.m_IdlrMapWnd_Y;
 m_optIdlcMapWnd.value.x = i_wndSettings.m_IdlcMapWnd_X;
 m_optIdlcMapWnd.value.y = i_wndSettings.m_IdlcMapWnd_Y;
 m_optThrassMapWnd.value.x = i_wndSettings.m_ThrassMapWnd_X;
 m_optThrassMapWnd.value.y = i_wndSettings.m_ThrassMapWnd_Y;
 m_optAETPSMapWnd.value.x = i_wndSettings.m_AETPSMapWnd_X;
 m_optAETPSMapWnd.value.y = i_wndSettings.m_AETPSMapWnd_Y; 
 m_optAEMAPMapWnd.value.x = i_wndSettings.m_AEMAPMapWnd_X;
 m_optAEMAPMapWnd.value.y = i_wndSettings.m_AEMAPMapWnd_Y; 
 m_optAERPMMapWnd.value.x = i_wndSettings.m_AERPMMapWnd_X;
 m_optAERPMMapWnd.value.y = i_wndSettings.m_AERPMMapWnd_Y; 
 m_optAftstrMapWnd.value.x = i_wndSettings.m_AftstrMapWnd_X;
 m_optAftstrMapWnd.value.y = i_wndSettings.m_AftstrMapWnd_Y; 
 m_optITMapWnd.value.x = i_wndSettings.m_ITMapWnd_X;
 m_optITMapWnd.value.y = i_wndSettings.m_ITMapWnd_Y;
 m_optITRPMMapWnd.value.x = i_wndSettings.m_ITRPMMapWnd_X;
 m_optITRPMMapWnd.value.y = i_wndSettings.m_ITRPMMapWnd_Y;
 m_optRigidMapWnd.value.x = i_wndSettings.m_RigidMapWnd_X;
 m_optRigidMapWnd.value.y = i_wndSettings.m_RigidMapWnd_Y;
 m_optEGOCrvMapWnd.value.x = i_wndSettings.m_EGOCrvMapWnd_X;
 m_optEGOCrvMapWnd.value.y = i_wndSettings.m_EGOCrvMapWnd_Y;
 m_optIACCMapWnd.value.x = i_wndSettings.m_IACCMapWnd_X;
 m_optIACCMapWnd.value.y = i_wndSettings.m_IACCMapWnd_Y;
 m_optIACCWMapWnd.value.x = i_wndSettings.m_IACCWMapWnd_X;
 m_optIACCWMapWnd.value.y = i_wndSettings.m_IACCWMapWnd_Y;
 m_optIATCLTMapWnd.value.x = i_wndSettings.m_IATCLTMapWnd_X;
 m_optIATCLTMapWnd.value.y = i_wndSettings.m_IATCLTMapWnd_Y;
 m_optTpsswtMapWnd.value.x = i_wndSettings.m_TpsswtMapWnd_X;
 m_optTpsswtMapWnd.value.y = i_wndSettings.m_TpsswtMapWnd_Y;
 m_optGtscMapWnd.value.x = i_wndSettings.m_GtscMapWnd_X;
 m_optGtscMapWnd.value.y = i_wndSettings.m_GtscMapWnd_Y;
 m_optGpscMapWnd.value.x = i_wndSettings.m_GpscMapWnd_X;
 m_optGpscMapWnd.value.y = i_wndSettings.m_GpscMapWnd_Y;
 m_optAtscMapWnd.value.x = i_wndSettings.m_AtscMapWnd_X;
 m_optAtscMapWnd.value.y = i_wndSettings.m_AtscMapWnd_Y;
 m_optPwm1MapWnd.value.x = i_wndSettings.m_Pwm1MapWnd_X;
 m_optPwm1MapWnd.value.y = i_wndSettings.m_Pwm1MapWnd_Y;
 m_optPwm2MapWnd.value.x = i_wndSettings.m_Pwm2MapWnd_X;
 m_optPwm2MapWnd.value.y = i_wndSettings.m_Pwm2MapWnd_Y;
 m_optIACMATMapWnd.value.x = i_wndSettings.m_IACMATMapWnd_X;
 m_optIACMATMapWnd.value.y = i_wndSettings.m_IACMATMapWnd_Y;
 m_optTpszonMapWnd.value.x = i_wndSettings.m_TpszonMapWnd_X;
 m_optTpszonMapWnd.value.y = i_wndSettings.m_TpszonMapWnd_Y;
 m_optCylMultMapWnd.value.x = i_wndSettings.m_CylMultMapWnd_X;
 m_optCylMultMapWnd.value.y = i_wndSettings.m_CylMultMapWnd_Y;
 m_optCylAddMapWnd.value.x = i_wndSettings.m_CylAddMapWnd_X;
 m_optCylAddMapWnd.value.y = i_wndSettings.m_CylAddMapWnd_Y;
 //group of separate maps
 m_optAttenuatorMapWnd.value.x = i_wndSettings.m_AttenuatorMapWnd_X;
 m_optAttenuatorMapWnd.value.y = i_wndSettings.m_AttenuatorMapWnd_Y;
 m_optDwellCntrlMapWnd.value.x = i_wndSettings.m_DwellCntrlMapWnd_X;
 m_optDwellCntrlMapWnd.value.y = i_wndSettings.m_DwellCntrlMapWnd_Y;
 m_optCTSCurveMapWnd.value.x = i_wndSettings.m_CTSCurveMapWnd_X;
 m_optCTSCurveMapWnd.value.y = i_wndSettings.m_CTSCurveMapWnd_Y;
 m_optATSCurvMapWnd.value.x = i_wndSettings.m_ATSCurvMapWnd_X;
 m_optATSCurvMapWnd.value.y = i_wndSettings.m_ATSCurvMapWnd_Y;
 m_optATSCorrMapWnd.value.x = i_wndSettings.m_ATSCorrMapWnd_X;
 m_optATSCorrMapWnd.value.y = i_wndSettings.m_ATSCorrMapWnd_Y; 
 m_optGasdoseMapWnd.value.x = i_wndSettings.m_GasdoseMapWnd_X;
 m_optGasdoseMapWnd.value.y = i_wndSettings.m_GasdoseMapWnd_Y; 
 m_optBarocorrMapWnd.value.x = i_wndSettings.m_BarocorrMapWnd_X;
 m_optBarocorrMapWnd.value.y = i_wndSettings.m_BarocorrMapWnd_Y; 
 m_optManIgntimMapWnd.value.x = i_wndSettings.m_ManIgntimMapWnd_X;
 m_optManIgntimMapWnd.value.y = i_wndSettings.m_ManIgntimMapWnd_Y; 
 m_optCESettingsWnd.value.x = i_wndSettings.m_CESettingsWnd_X;
 m_optCESettingsWnd.value.y = i_wndSettings.m_CESettingsWnd_Y; 
 m_optTmp2CurveMapWnd.value.x = i_wndSettings.m_Tmp2CurveMapWnd_X;
 m_optTmp2CurveMapWnd.value.y = i_wndSettings.m_Tmp2CurveMapWnd_Y; 
 m_optCrkTempMapWnd.value.x = i_wndSettings.m_CrkTempMapWnd_X;
 m_optCrkTempMapWnd.value.y = i_wndSettings.m_CrkTempMapWnd_Y;
 m_optEHPauseMapWnd.value.x = i_wndSettings.m_EHPauseMapWnd_X;
 m_optEHPauseMapWnd.value.y = i_wndSettings.m_EHPauseMapWnd_Y;
 m_optCrankingThrdMapWnd.value.x = i_wndSettings.m_CrankingThrdMapWnd_X;
 m_optCrankingThrdMapWnd.value.y = i_wndSettings.m_CrankingThrdMapWnd_Y;
 m_optCrankingTimeMapWnd.value.x = i_wndSettings.m_CrankingTimeMapWnd_X;
 m_optCrankingTimeMapWnd.value.y = i_wndSettings.m_CrankingTimeMapWnd_Y;
 m_optSmapabanThrdMapWnd.value.x = i_wndSettings.m_SmapabanThrdMapWnd_X;
 m_optSmapabanThrdMapWnd.value.y = i_wndSettings.m_SmapabanThrdMapWnd_Y;
 m_optKnockZoneMapWnd.value.x = i_wndSettings.m_KnockZoneMapWnd_X;
 m_optKnockZoneMapWnd.value.y = i_wndSettings.m_KnockZoneMapWnd_Y; 
 m_optGrtsCurveMapWnd.value.x = i_wndSettings.m_GrtsCurveMapWnd_X;
 m_optGrtsCurveMapWnd.value.y = i_wndSettings.m_GrtsCurveMapWnd_Y; 
 m_optGrHeatDutyMapWnd.value.x = i_wndSettings.m_GrHeatDutyMapWnd_X;
 m_optGrHeatDutyMapWnd.value.y = i_wndSettings.m_GrHeatDutyMapWnd_Y;
 m_optPwmIacUCoefMapWnd.value.x = i_wndSettings.m_PwmIacUCoefMapWnd_X;
 m_optPwmIacUCoefMapWnd.value.y = i_wndSettings.m_PwmIacUCoefMapWnd_Y;
 m_optAftstrStrk0MapWnd.value.x = i_wndSettings.m_AftstrStrk0MapWnd_X;
 m_optAftstrStrk0MapWnd.value.y = i_wndSettings.m_AftstrStrk0MapWnd_Y;
 m_optAftstrStrk1MapWnd.value.x = i_wndSettings.m_AftstrStrk1MapWnd_X;
 m_optAftstrStrk1MapWnd.value.y = i_wndSettings.m_AftstrStrk1MapWnd_Y;
 m_optGrValDelMapWnd.value.x = i_wndSettings.m_GrValDelMapWnd_X;
 m_optGrValDelMapWnd.value.y = i_wndSettings.m_GrValDelMapWnd_Y;
 m_optFtlsCurveMapWnd.value.x = i_wndSettings.m_FtlsCurveMapWnd_X;
 m_optFtlsCurveMapWnd.value.y = i_wndSettings.m_FtlsCurveMapWnd_Y; 
 m_optEgtsCurveMapWnd.value.x = i_wndSettings.m_EgtsCurveMapWnd_X;
 m_optEgtsCurveMapWnd.value.y = i_wndSettings.m_EgtsCurveMapWnd_Y; 
 m_optOpsCurveMapWnd.value.x = i_wndSettings.m_OpsCurveMapWnd_X;
 m_optOpsCurveMapWnd.value.y = i_wndSettings.m_OpsCurveMapWnd_Y;
 m_optManInjPwcMapWnd.value.x = i_wndSettings.m_ManInjPwcMapWnd_X;
 m_optManInjPwcMapWnd.value.y = i_wndSettings.m_ManInjPwcMapWnd_Y; 
 m_optMAFCurveMapWnd.value.x = i_wndSettings.m_MAFCurveMapWnd_X;
 m_optMAFCurveMapWnd.value.y = i_wndSettings.m_MAFCurveMapWnd_Y; 
 m_optFtlsCorMapWnd.value.x = i_wndSettings.m_FtlsCorMapWnd_X;
 m_optFtlsCorMapWnd.value.y = i_wndSettings.m_FtlsCorMapWnd_Y; 
 m_optLambdaZoneMapWnd.value.x = i_wndSettings.m_LambdaZoneMapWnd_X;
 m_optLambdaZoneMapWnd.value.y = i_wndSettings.m_LambdaZoneMapWnd_Y; 
 m_optFtsCurveMapWnd.value.x = i_wndSettings.m_FtsCurveMapWnd_X;
 m_optFtsCurveMapWnd.value.y = i_wndSettings.m_FtsCurveMapWnd_Y; 
 m_optFuelDensCorrMapWnd.value.x = i_wndSettings.m_FuelDensCorrMapWnd_X;
 m_optFuelDensCorrMapWnd.value.y = i_wndSettings.m_FuelDensCorrMapWnd_Y; 
 m_optXtauXfAccMapWnd.value.x = i_wndSettings.m_XtauXfAccMapWnd_X;
 m_optXtauXfAccMapWnd.value.y = i_wndSettings.m_XtauXfAccMapWnd_Y; 
 m_optXtauXfDecMapWnd.value.x = i_wndSettings.m_XtauXfDecMapWnd_X;
 m_optXtauXfDecMapWnd.value.y = i_wndSettings.m_XtauXfDecMapWnd_Y; 
 m_optXtauTfAccMapWnd.value.x = i_wndSettings.m_XtauTfAccMapWnd_X;
 m_optXtauTfAccMapWnd.value.y = i_wndSettings.m_XtauTfAccMapWnd_Y; 
 m_optXtauTfDecMapWnd.value.x = i_wndSettings.m_XtauTfDecMapWnd_X;
 m_optXtauTfDecMapWnd.value.y = i_wndSettings.m_XtauTfDecMapWnd_Y; 
 m_optInjNonLinPMapWnd.value.x = i_wndSettings.m_InjNonLinPMapWnd_X;
 m_optInjNonLinPMapWnd.value.y = i_wndSettings.m_InjNonLinPMapWnd_Y; 
 m_optInjNonLinGMapWnd.value.x = i_wndSettings.m_InjNonLinGMapWnd_X;
 m_optInjNonLinGMapWnd.value.y = i_wndSettings.m_InjNonLinGMapWnd_Y; 
 m_optEGODelayMapWnd.value.x = i_wndSettings.m_EGODelayMapWnd_X;
 m_optEGODelayMapWnd.value.y = i_wndSettings.m_EGODelayMapWnd_Y;
 m_optWUAFR0MapWnd.value.x = i_wndSettings.m_WUAFR0MapWnd_X;
 m_optWUAFR0MapWnd.value.y = i_wndSettings.m_WUAFR0MapWnd_Y;
 m_optWUAFR1MapWnd.value.x = i_wndSettings.m_WUAFR1MapWnd_X;
 m_optWUAFR1MapWnd.value.y = i_wndSettings.m_WUAFR1MapWnd_Y;
}

void CAppSettingsModel::GetWndSettings(WndSettings& o_wndSettings) const
{
 o_wndSettings.m_MainFrmWnd_X = m_optMainFrmWnd.value.x;
 o_wndSettings.m_MainFrmWnd_Y = m_optMainFrmWnd.value.y;
 o_wndSettings.m_StrtMapWnd_X = m_optStrtMapWnd.value.x;
 o_wndSettings.m_StrtMapWnd_Y = m_optStrtMapWnd.value.y;
 o_wndSettings.m_IdleMapWnd_X = m_optIdleMapWnd.value.x;
 o_wndSettings.m_IdleMapWnd_Y = m_optIdleMapWnd.value.y;
 o_wndSettings.m_WorkMapWnd_X = m_optWorkMapWnd.value.x;
 o_wndSettings.m_WorkMapWnd_Y = m_optWorkMapWnd.value.y;
 o_wndSettings.m_TempMapWnd_X = m_optTempMapWnd.value.x;
 o_wndSettings.m_TempMapWnd_Y = m_optTempMapWnd.value.y;
 o_wndSettings.m_TempIdlMapWnd_X = m_optTempIdlMapWnd.value.x;
 o_wndSettings.m_TempIdlMapWnd_Y = m_optTempIdlMapWnd.value.y;
 o_wndSettings.m_GridMapIgnWnd_X = m_optGridMapIgnWnd.value.x;
 o_wndSettings.m_GridMapIgnWnd_Y = m_optGridMapIgnWnd.value.y;
 o_wndSettings.m_GridMapInjWnd_X = m_optGridMapInjWnd.value.x;
 o_wndSettings.m_GridMapInjWnd_Y = m_optGridMapInjWnd.value.y;
 o_wndSettings.m_VEMapWnd_X = m_optVEMapWnd.value.x;
 o_wndSettings.m_VEMapWnd_Y = m_optVEMapWnd.value.y;
 o_wndSettings.m_VE2MapWnd_X = m_optVE2MapWnd.value.x;
 o_wndSettings.m_VE2MapWnd_Y = m_optVE2MapWnd.value.y;
 o_wndSettings.m_VEIMapWnd_X = m_optVEIMapWnd.value.x;
 o_wndSettings.m_VEIMapWnd_Y = m_optVEIMapWnd.value.y;
 o_wndSettings.m_AFRMapWnd_X = m_optAFRMapWnd.value.x;
 o_wndSettings.m_AFRMapWnd_Y = m_optAFRMapWnd.value.y;
 o_wndSettings.m_CrnkMapWnd_X = m_optCrnkMapWnd.value.x;
 o_wndSettings.m_CrnkMapWnd_Y = m_optCrnkMapWnd.value.y;
 o_wndSettings.m_WrmpMapWnd_X = m_optWrmpMapWnd.value.x;
 o_wndSettings.m_WrmpMapWnd_Y = m_optWrmpMapWnd.value.y;
 o_wndSettings.m_DeadMapWnd_X = m_optDeadMapWnd.value.x;
 o_wndSettings.m_DeadMapWnd_Y = m_optDeadMapWnd.value.y;
 o_wndSettings.m_IdlrMapWnd_X = m_optIdlrMapWnd.value.x;
 o_wndSettings.m_IdlrMapWnd_Y = m_optIdlrMapWnd.value.y;
 o_wndSettings.m_IdlcMapWnd_X = m_optIdlcMapWnd.value.x;
 o_wndSettings.m_IdlcMapWnd_Y = m_optIdlcMapWnd.value.y;
 o_wndSettings.m_ThrassMapWnd_X = m_optThrassMapWnd.value.x;
 o_wndSettings.m_ThrassMapWnd_Y = m_optThrassMapWnd.value.y;
 o_wndSettings.m_AETPSMapWnd_X = m_optAETPSMapWnd.value.x;
 o_wndSettings.m_AETPSMapWnd_Y = m_optAETPSMapWnd.value.y;
 o_wndSettings.m_AEMAPMapWnd_X = m_optAEMAPMapWnd.value.x;
 o_wndSettings.m_AEMAPMapWnd_Y = m_optAEMAPMapWnd.value.y;
 o_wndSettings.m_AERPMMapWnd_X = m_optAERPMMapWnd.value.x;
 o_wndSettings.m_AERPMMapWnd_Y = m_optAERPMMapWnd.value.y;
 o_wndSettings.m_AftstrMapWnd_X = m_optAftstrMapWnd.value.x;
 o_wndSettings.m_AftstrMapWnd_Y = m_optAftstrMapWnd.value.y;
 o_wndSettings.m_ITMapWnd_X = m_optITMapWnd.value.x;
 o_wndSettings.m_ITMapWnd_Y = m_optITMapWnd.value.y;
 o_wndSettings.m_ITRPMMapWnd_X = m_optITRPMMapWnd.value.x;
 o_wndSettings.m_ITRPMMapWnd_Y = m_optITRPMMapWnd.value.y;
 o_wndSettings.m_RigidMapWnd_X = m_optRigidMapWnd.value.x;
 o_wndSettings.m_RigidMapWnd_Y = m_optRigidMapWnd.value.y;
 o_wndSettings.m_EGOCrvMapWnd_X = m_optEGOCrvMapWnd.value.x;
 o_wndSettings.m_EGOCrvMapWnd_Y = m_optEGOCrvMapWnd.value.y;
 o_wndSettings.m_IACCMapWnd_X = m_optIACCMapWnd.value.x;
 o_wndSettings.m_IACCMapWnd_Y = m_optIACCMapWnd.value.y;
 o_wndSettings.m_IACCWMapWnd_X = m_optIACCWMapWnd.value.x;
 o_wndSettings.m_IACCWMapWnd_Y = m_optIACCWMapWnd.value.y;
 o_wndSettings.m_IATCLTMapWnd_X = m_optIATCLTMapWnd.value.x;
 o_wndSettings.m_IATCLTMapWnd_Y = m_optIATCLTMapWnd.value.y;
 o_wndSettings.m_TpsswtMapWnd_X = m_optTpsswtMapWnd.value.x;
 o_wndSettings.m_TpsswtMapWnd_Y = m_optTpsswtMapWnd.value.y;
 o_wndSettings.m_GtscMapWnd_X = m_optGtscMapWnd.value.x;
 o_wndSettings.m_GtscMapWnd_Y = m_optGtscMapWnd.value.y;
 o_wndSettings.m_GpscMapWnd_X = m_optGpscMapWnd.value.x;
 o_wndSettings.m_GpscMapWnd_Y = m_optGpscMapWnd.value.y;
 o_wndSettings.m_AtscMapWnd_X = m_optAtscMapWnd.value.x;
 o_wndSettings.m_AtscMapWnd_Y = m_optAtscMapWnd.value.y;
 o_wndSettings.m_Pwm1MapWnd_X = m_optPwm1MapWnd.value.x;
 o_wndSettings.m_Pwm1MapWnd_Y = m_optPwm1MapWnd.value.y;
 o_wndSettings.m_Pwm2MapWnd_X = m_optPwm2MapWnd.value.x;
 o_wndSettings.m_Pwm2MapWnd_Y = m_optPwm2MapWnd.value.y;
 o_wndSettings.m_IACMATMapWnd_X = m_optIACMATMapWnd.value.x;
 o_wndSettings.m_IACMATMapWnd_Y = m_optIACMATMapWnd.value.y;
 o_wndSettings.m_TpszonMapWnd_X = m_optTpszonMapWnd.value.x;
 o_wndSettings.m_TpszonMapWnd_Y = m_optTpszonMapWnd.value.y;
 o_wndSettings.m_CylMultMapWnd_X = m_optCylMultMapWnd.value.x;
 o_wndSettings.m_CylMultMapWnd_Y = m_optCylMultMapWnd.value.y;
 o_wndSettings.m_CylAddMapWnd_X = m_optCylAddMapWnd.value.x;
 o_wndSettings.m_CylAddMapWnd_Y = m_optCylAddMapWnd.value.y;
 //group of separate maps
 o_wndSettings.m_AttenuatorMapWnd_X = m_optAttenuatorMapWnd.value.x;
 o_wndSettings.m_AttenuatorMapWnd_Y = m_optAttenuatorMapWnd.value.y;
 o_wndSettings.m_DwellCntrlMapWnd_X = m_optDwellCntrlMapWnd.value.x;
 o_wndSettings.m_DwellCntrlMapWnd_Y = m_optDwellCntrlMapWnd.value.y;
 o_wndSettings.m_CTSCurveMapWnd_X = m_optCTSCurveMapWnd.value.x;
 o_wndSettings.m_CTSCurveMapWnd_Y = m_optCTSCurveMapWnd.value.y;
 o_wndSettings.m_ATSCurvMapWnd_X = m_optATSCurvMapWnd.value.x;
 o_wndSettings.m_ATSCurvMapWnd_Y = m_optATSCurvMapWnd.value.y;
 o_wndSettings.m_ATSCorrMapWnd_X = m_optATSCorrMapWnd.value.x;
 o_wndSettings.m_ATSCorrMapWnd_Y = m_optATSCorrMapWnd.value.y;
 o_wndSettings.m_GasdoseMapWnd_X = m_optGasdoseMapWnd.value.x;
 o_wndSettings.m_GasdoseMapWnd_Y = m_optGasdoseMapWnd.value.y;
 o_wndSettings.m_BarocorrMapWnd_X = m_optBarocorrMapWnd.value.x;
 o_wndSettings.m_BarocorrMapWnd_Y = m_optBarocorrMapWnd.value.y;
 o_wndSettings.m_ManIgntimMapWnd_X = m_optManIgntimMapWnd.value.x;
 o_wndSettings.m_ManIgntimMapWnd_Y = m_optManIgntimMapWnd.value.y;
 o_wndSettings.m_CESettingsWnd_X = m_optCESettingsWnd.value.x;
 o_wndSettings.m_CESettingsWnd_Y = m_optCESettingsWnd.value.y;
 o_wndSettings.m_Tmp2CurveMapWnd_X = m_optTmp2CurveMapWnd.value.x;
 o_wndSettings.m_Tmp2CurveMapWnd_Y = m_optTmp2CurveMapWnd.value.y;
 o_wndSettings.m_CrkTempMapWnd_X = m_optCrkTempMapWnd.value.x;
 o_wndSettings.m_CrkTempMapWnd_Y = m_optCrkTempMapWnd.value.y;
 o_wndSettings.m_EHPauseMapWnd_X = m_optEHPauseMapWnd.value.x;
 o_wndSettings.m_EHPauseMapWnd_Y = m_optEHPauseMapWnd.value.y;
 o_wndSettings.m_CrankingThrdMapWnd_X = m_optCrankingThrdMapWnd.value.x;
 o_wndSettings.m_CrankingThrdMapWnd_Y = m_optCrankingThrdMapWnd.value.y;
 o_wndSettings.m_CrankingTimeMapWnd_X = m_optCrankingTimeMapWnd.value.x;
 o_wndSettings.m_CrankingTimeMapWnd_Y = m_optCrankingTimeMapWnd.value.y;
 o_wndSettings.m_SmapabanThrdMapWnd_X = m_optSmapabanThrdMapWnd.value.x;
 o_wndSettings.m_SmapabanThrdMapWnd_Y = m_optSmapabanThrdMapWnd.value.y;
 o_wndSettings.m_KnockZoneMapWnd_X = m_optKnockZoneMapWnd.value.x;
 o_wndSettings.m_KnockZoneMapWnd_Y = m_optKnockZoneMapWnd.value.y;
 o_wndSettings.m_GrtsCurveMapWnd_X = m_optGrtsCurveMapWnd.value.x;
 o_wndSettings.m_GrtsCurveMapWnd_Y = m_optGrtsCurveMapWnd.value.y;
 o_wndSettings.m_GrHeatDutyMapWnd_X = m_optGrHeatDutyMapWnd.value.x;
 o_wndSettings.m_GrHeatDutyMapWnd_Y = m_optGrHeatDutyMapWnd.value.y;
 o_wndSettings.m_PwmIacUCoefMapWnd_X = m_optPwmIacUCoefMapWnd.value.x;
 o_wndSettings.m_PwmIacUCoefMapWnd_Y = m_optPwmIacUCoefMapWnd.value.y;
 o_wndSettings.m_AftstrStrk0MapWnd_X = m_optAftstrStrk0MapWnd.value.x;
 o_wndSettings.m_AftstrStrk0MapWnd_Y = m_optAftstrStrk0MapWnd.value.y;
 o_wndSettings.m_AftstrStrk1MapWnd_X = m_optAftstrStrk1MapWnd.value.x;
 o_wndSettings.m_AftstrStrk1MapWnd_Y = m_optAftstrStrk1MapWnd.value.y;
 o_wndSettings.m_GrValDelMapWnd_X = m_optGrValDelMapWnd.value.x;
 o_wndSettings.m_GrValDelMapWnd_Y = m_optGrValDelMapWnd.value.y;
 o_wndSettings.m_FtlsCurveMapWnd_X = m_optFtlsCurveMapWnd.value.x;
 o_wndSettings.m_FtlsCurveMapWnd_Y = m_optFtlsCurveMapWnd.value.y;
 o_wndSettings.m_EgtsCurveMapWnd_X = m_optEgtsCurveMapWnd.value.x;
 o_wndSettings.m_EgtsCurveMapWnd_Y = m_optEgtsCurveMapWnd.value.y;
 o_wndSettings.m_OpsCurveMapWnd_X = m_optOpsCurveMapWnd.value.x;
 o_wndSettings.m_OpsCurveMapWnd_Y = m_optOpsCurveMapWnd.value.y;
 o_wndSettings.m_ManInjPwcMapWnd_X = m_optManInjPwcMapWnd.value.x;
 o_wndSettings.m_ManInjPwcMapWnd_Y = m_optManInjPwcMapWnd.value.y;
 o_wndSettings.m_MAFCurveMapWnd_X = m_optMAFCurveMapWnd.value.x;
 o_wndSettings.m_MAFCurveMapWnd_Y = m_optMAFCurveMapWnd.value.y;
 o_wndSettings.m_FtlsCorMapWnd_X = m_optFtlsCorMapWnd.value.x;
 o_wndSettings.m_FtlsCorMapWnd_Y = m_optFtlsCorMapWnd.value.y;
 o_wndSettings.m_LambdaZoneMapWnd_X = m_optLambdaZoneMapWnd.value.x;
 o_wndSettings.m_LambdaZoneMapWnd_Y = m_optLambdaZoneMapWnd.value.y;
 o_wndSettings.m_FtsCurveMapWnd_X = m_optFtsCurveMapWnd.value.x;
 o_wndSettings.m_FtsCurveMapWnd_Y = m_optFtsCurveMapWnd.value.y;
 o_wndSettings.m_FuelDensCorrMapWnd_X = m_optFuelDensCorrMapWnd.value.x;
 o_wndSettings.m_FuelDensCorrMapWnd_Y = m_optFuelDensCorrMapWnd.value.y;
 o_wndSettings.m_XtauXfAccMapWnd_X = m_optXtauXfAccMapWnd.value.x;
 o_wndSettings.m_XtauXfAccMapWnd_Y = m_optXtauXfAccMapWnd.value.y;
 o_wndSettings.m_XtauXfDecMapWnd_X = m_optXtauXfDecMapWnd.value.x;
 o_wndSettings.m_XtauXfDecMapWnd_Y = m_optXtauXfDecMapWnd.value.y;
 o_wndSettings.m_XtauTfAccMapWnd_X = m_optXtauTfAccMapWnd.value.x;
 o_wndSettings.m_XtauTfAccMapWnd_Y = m_optXtauTfAccMapWnd.value.y;
 o_wndSettings.m_XtauTfDecMapWnd_X = m_optXtauTfDecMapWnd.value.x;
 o_wndSettings.m_XtauTfDecMapWnd_Y = m_optXtauTfDecMapWnd.value.y;
 o_wndSettings.m_InjNonLinPMapWnd_X = m_optInjNonLinPMapWnd.value.x;
 o_wndSettings.m_InjNonLinPMapWnd_Y = m_optInjNonLinPMapWnd.value.y;
 o_wndSettings.m_InjNonLinGMapWnd_X = m_optInjNonLinGMapWnd.value.x;
 o_wndSettings.m_InjNonLinGMapWnd_Y = m_optInjNonLinGMapWnd.value.y;
 o_wndSettings.m_EGODelayMapWnd_X = m_optEGODelayMapWnd.value.x;
 o_wndSettings.m_EGODelayMapWnd_Y = m_optEGODelayMapWnd.value.y;
 o_wndSettings.m_WUAFR0MapWnd_X = m_optWUAFR0MapWnd.value.x;
 o_wndSettings.m_WUAFR0MapWnd_Y = m_optWUAFR0MapWnd.value.y;
 o_wndSettings.m_WUAFR1MapWnd_X = m_optWUAFR1MapWnd.value.x;
 o_wndSettings.m_WUAFR1MapWnd_Y = m_optWUAFR1MapWnd.value.y;
}

void CAppSettingsModel::SetWndSettings1(const WndSettings& i_wndSettings)
{
 m_optStrtMapWnd1.value.x = i_wndSettings.m_StrtMapWnd_X;
 m_optStrtMapWnd1.value.y = i_wndSettings.m_StrtMapWnd_Y;
 m_optIdleMapWnd1.value.x = i_wndSettings.m_IdleMapWnd_X;
 m_optIdleMapWnd1.value.y = i_wndSettings.m_IdleMapWnd_Y;
 m_optWorkMapWnd1.value.x = i_wndSettings.m_WorkMapWnd_X;
 m_optWorkMapWnd1.value.y = i_wndSettings.m_WorkMapWnd_Y;
 m_optTempMapWnd1.value.x = i_wndSettings.m_TempMapWnd_X;
 m_optTempMapWnd1.value.y = i_wndSettings.m_TempMapWnd_Y;
 m_optTempIdlMapWnd1.value.x = i_wndSettings.m_TempIdlMapWnd_X;
 m_optTempIdlMapWnd1.value.y = i_wndSettings.m_TempIdlMapWnd_Y;
 m_optGridMapIgnWnd1.value.x = i_wndSettings.m_GridMapIgnWnd_X;
 m_optGridMapIgnWnd1.value.y = i_wndSettings.m_GridMapIgnWnd_Y;
 m_optGridMapInjWnd1.value.x = i_wndSettings.m_GridMapInjWnd_X;
 m_optGridMapInjWnd1.value.y = i_wndSettings.m_GridMapInjWnd_Y;
 m_optVEMapWnd1.value.x = i_wndSettings.m_VEMapWnd_X;
 m_optVEMapWnd1.value.y = i_wndSettings.m_VEMapWnd_Y;
 m_optVE2MapWnd1.value.x = i_wndSettings.m_VE2MapWnd_X;
 m_optVE2MapWnd1.value.y = i_wndSettings.m_VE2MapWnd_Y;
 m_optVEIMapWnd1.value.x = i_wndSettings.m_VEIMapWnd_X;
 m_optVEIMapWnd1.value.y = i_wndSettings.m_VEIMapWnd_Y;
 m_optAFRMapWnd1.value.x = i_wndSettings.m_AFRMapWnd_X;
 m_optAFRMapWnd1.value.y = i_wndSettings.m_AFRMapWnd_Y;
 m_optCrnkMapWnd1.value.x = i_wndSettings.m_CrnkMapWnd_X;
 m_optCrnkMapWnd1.value.y = i_wndSettings.m_CrnkMapWnd_Y;
 m_optWrmpMapWnd1.value.x = i_wndSettings.m_WrmpMapWnd_X;
 m_optWrmpMapWnd1.value.y = i_wndSettings.m_WrmpMapWnd_Y;
 m_optDeadMapWnd1.value.x = i_wndSettings.m_DeadMapWnd_X;
 m_optDeadMapWnd1.value.y = i_wndSettings.m_DeadMapWnd_Y;
 m_optIdlrMapWnd1.value.x = i_wndSettings.m_IdlrMapWnd_X;
 m_optIdlrMapWnd1.value.y = i_wndSettings.m_IdlrMapWnd_Y;
 m_optIdlcMapWnd1.value.x = i_wndSettings.m_IdlcMapWnd_X;
 m_optIdlcMapWnd1.value.y = i_wndSettings.m_IdlcMapWnd_Y;
 m_optThrassMapWnd1.value.x = i_wndSettings.m_ThrassMapWnd_X;
 m_optThrassMapWnd1.value.y = i_wndSettings.m_ThrassMapWnd_Y;
 m_optAETPSMapWnd1.value.x = i_wndSettings.m_AETPSMapWnd_X;
 m_optAETPSMapWnd1.value.y = i_wndSettings.m_AETPSMapWnd_Y; 
 m_optAEMAPMapWnd1.value.x = i_wndSettings.m_AEMAPMapWnd_X;
 m_optAEMAPMapWnd1.value.y = i_wndSettings.m_AEMAPMapWnd_Y; 
 m_optAERPMMapWnd1.value.x = i_wndSettings.m_AERPMMapWnd_X;
 m_optAERPMMapWnd1.value.y = i_wndSettings.m_AERPMMapWnd_Y; 
 m_optAftstrMapWnd1.value.x = i_wndSettings.m_AftstrMapWnd_X;
 m_optAftstrMapWnd1.value.y = i_wndSettings.m_AftstrMapWnd_Y; 
 m_optITMapWnd1.value.x = i_wndSettings.m_ITMapWnd_X;
 m_optITMapWnd1.value.y = i_wndSettings.m_ITMapWnd_Y;
 m_optITRPMMapWnd1.value.x = i_wndSettings.m_ITRPMMapWnd_X;
 m_optITRPMMapWnd1.value.y = i_wndSettings.m_ITRPMMapWnd_Y;
 m_optRigidMapWnd1.value.x = i_wndSettings.m_RigidMapWnd_X;
 m_optRigidMapWnd1.value.y = i_wndSettings.m_RigidMapWnd_Y;
 m_optEGOCrvMapWnd1.value.x = i_wndSettings.m_EGOCrvMapWnd_X;
 m_optEGOCrvMapWnd1.value.y = i_wndSettings.m_EGOCrvMapWnd_Y;
 m_optIACCMapWnd1.value.x = i_wndSettings.m_IACCMapWnd_X;
 m_optIACCMapWnd1.value.y = i_wndSettings.m_IACCMapWnd_Y;
 m_optIACCWMapWnd1.value.x = i_wndSettings.m_IACCWMapWnd_X;
 m_optIACCWMapWnd1.value.y = i_wndSettings.m_IACCWMapWnd_Y;
 m_optIATCLTMapWnd1.value.x = i_wndSettings.m_IATCLTMapWnd_X;
 m_optIATCLTMapWnd1.value.y = i_wndSettings.m_IATCLTMapWnd_Y;
 m_optTpsswtMapWnd1.value.x = i_wndSettings.m_TpsswtMapWnd_X;
 m_optTpsswtMapWnd1.value.y = i_wndSettings.m_TpsswtMapWnd_Y;
 m_optGtscMapWnd1.value.x = i_wndSettings.m_GtscMapWnd_X;
 m_optGtscMapWnd1.value.y = i_wndSettings.m_GtscMapWnd_Y;
 m_optGpscMapWnd1.value.x = i_wndSettings.m_GpscMapWnd_X;
 m_optGpscMapWnd1.value.y = i_wndSettings.m_GpscMapWnd_Y;
 m_optAtscMapWnd1.value.x = i_wndSettings.m_AtscMapWnd_X;
 m_optAtscMapWnd1.value.y = i_wndSettings.m_AtscMapWnd_Y;
 m_optPwm1MapWnd1.value.x = i_wndSettings.m_Pwm1MapWnd_X;
 m_optPwm1MapWnd1.value.y = i_wndSettings.m_Pwm1MapWnd_Y;
 m_optPwm2MapWnd1.value.x = i_wndSettings.m_Pwm2MapWnd_X;
 m_optPwm2MapWnd1.value.y = i_wndSettings.m_Pwm2MapWnd_Y;
 m_optIACMATMapWnd1.value.x = i_wndSettings.m_IACMATMapWnd_X;
 m_optIACMATMapWnd1.value.y = i_wndSettings.m_IACMATMapWnd_Y;
 m_optTpszonMapWnd1.value.x = i_wndSettings.m_TpszonMapWnd_X;
 m_optTpszonMapWnd1.value.y = i_wndSettings.m_TpszonMapWnd_Y;
 m_optCylMultMapWnd1.value.x = i_wndSettings.m_CylMultMapWnd_X;
 m_optCylMultMapWnd1.value.y = i_wndSettings.m_CylMultMapWnd_Y;
 m_optCylAddMapWnd1.value.x = i_wndSettings.m_CylAddMapWnd_X;
 m_optCylAddMapWnd1.value.y = i_wndSettings.m_CylAddMapWnd_Y;
 //group of separate maps
 m_optAttenuatorMapWnd1.value.x = i_wndSettings.m_AttenuatorMapWnd_X;
 m_optAttenuatorMapWnd1.value.y = i_wndSettings.m_AttenuatorMapWnd_Y;
 m_optDwellCntrlMapWnd1.value.x = i_wndSettings.m_DwellCntrlMapWnd_X;
 m_optDwellCntrlMapWnd1.value.y = i_wndSettings.m_DwellCntrlMapWnd_Y;
 m_optCTSCurveMapWnd1.value.x = i_wndSettings.m_CTSCurveMapWnd_X;
 m_optCTSCurveMapWnd1.value.y = i_wndSettings.m_CTSCurveMapWnd_Y;
 m_optATSCurvMapWnd1.value.x = i_wndSettings.m_ATSCurvMapWnd_X;
 m_optATSCurvMapWnd1.value.y = i_wndSettings.m_ATSCurvMapWnd_Y;
 m_optATSCorrMapWnd1.value.x = i_wndSettings.m_ATSCorrMapWnd_X;
 m_optATSCorrMapWnd1.value.y = i_wndSettings.m_ATSCorrMapWnd_Y; 
 m_optGasdoseMapWnd1.value.x = i_wndSettings.m_GasdoseMapWnd_X;
 m_optGasdoseMapWnd1.value.y = i_wndSettings.m_GasdoseMapWnd_Y; 
 m_optBarocorrMapWnd1.value.x = i_wndSettings.m_BarocorrMapWnd_X;
 m_optBarocorrMapWnd1.value.y = i_wndSettings.m_BarocorrMapWnd_Y; 
 m_optManIgntimMapWnd1.value.x = i_wndSettings.m_ManIgntimMapWnd_X;
 m_optManIgntimMapWnd1.value.y = i_wndSettings.m_ManIgntimMapWnd_Y; 
 m_optCESettingsWnd1.value.x = i_wndSettings.m_CESettingsWnd_X;
 m_optCESettingsWnd1.value.y = i_wndSettings.m_CESettingsWnd_Y; 
 m_optTmp2CurveMapWnd1.value.x = i_wndSettings.m_Tmp2CurveMapWnd_X;
 m_optTmp2CurveMapWnd1.value.y = i_wndSettings.m_Tmp2CurveMapWnd_Y; 
 m_optCrkTempMapWnd1.value.x = i_wndSettings.m_CrkTempMapWnd_X;
 m_optCrkTempMapWnd1.value.y = i_wndSettings.m_CrkTempMapWnd_Y;
 m_optEHPauseMapWnd1.value.x = i_wndSettings.m_EHPauseMapWnd_X;
 m_optEHPauseMapWnd1.value.y = i_wndSettings.m_EHPauseMapWnd_Y;
 m_optCrankingThrdMapWnd1.value.x = i_wndSettings.m_CrankingThrdMapWnd_X;
 m_optCrankingThrdMapWnd1.value.y = i_wndSettings.m_CrankingThrdMapWnd_Y;
 m_optCrankingTimeMapWnd1.value.x = i_wndSettings.m_CrankingTimeMapWnd_X;
 m_optCrankingTimeMapWnd1.value.y = i_wndSettings.m_CrankingTimeMapWnd_Y;
 m_optSmapabanThrdMapWnd1.value.x = i_wndSettings.m_SmapabanThrdMapWnd_X;
 m_optSmapabanThrdMapWnd1.value.y = i_wndSettings.m_SmapabanThrdMapWnd_Y;
 m_optKnockZoneMapWnd1.value.x = i_wndSettings.m_KnockZoneMapWnd_X;
 m_optKnockZoneMapWnd1.value.y = i_wndSettings.m_KnockZoneMapWnd_Y; 
 m_optGrtsCurveMapWnd1.value.x = i_wndSettings.m_GrtsCurveMapWnd_X;
 m_optGrtsCurveMapWnd1.value.y = i_wndSettings.m_GrtsCurveMapWnd_Y; 
 m_optGrHeatDutyMapWnd1.value.x = i_wndSettings.m_GrHeatDutyMapWnd_X;
 m_optGrHeatDutyMapWnd1.value.y = i_wndSettings.m_GrHeatDutyMapWnd_Y;
 m_optPwmIacUCoefMapWnd1.value.x = i_wndSettings.m_PwmIacUCoefMapWnd_X;
 m_optPwmIacUCoefMapWnd1.value.y = i_wndSettings.m_PwmIacUCoefMapWnd_Y;
 m_optAftstrStrk0MapWnd1.value.x = i_wndSettings.m_AftstrStrk0MapWnd_X;
 m_optAftstrStrk0MapWnd1.value.y = i_wndSettings.m_AftstrStrk0MapWnd_Y;
 m_optAftstrStrk1MapWnd1.value.x = i_wndSettings.m_AftstrStrk1MapWnd_X;
 m_optAftstrStrk1MapWnd1.value.y = i_wndSettings.m_AftstrStrk1MapWnd_Y;
 m_optGrValDelMapWnd1.value.x = i_wndSettings.m_GrValDelMapWnd_X;
 m_optGrValDelMapWnd1.value.y = i_wndSettings.m_GrValDelMapWnd_Y;
 m_optFtlsCurveMapWnd1.value.x = i_wndSettings.m_FtlsCurveMapWnd_X;
 m_optFtlsCurveMapWnd1.value.y = i_wndSettings.m_FtlsCurveMapWnd_Y; 
 m_optEgtsCurveMapWnd1.value.x = i_wndSettings.m_EgtsCurveMapWnd_X;
 m_optEgtsCurveMapWnd1.value.y = i_wndSettings.m_EgtsCurveMapWnd_Y; 
 m_optOpsCurveMapWnd1.value.x = i_wndSettings.m_OpsCurveMapWnd_X;
 m_optOpsCurveMapWnd1.value.y = i_wndSettings.m_OpsCurveMapWnd_Y;
 m_optManInjPwcMapWnd1.value.x = i_wndSettings.m_ManInjPwcMapWnd_X;
 m_optManInjPwcMapWnd1.value.y = i_wndSettings.m_ManInjPwcMapWnd_Y; 
 m_optMAFCurveMapWnd1.value.x = i_wndSettings.m_MAFCurveMapWnd_X;
 m_optMAFCurveMapWnd1.value.y = i_wndSettings.m_MAFCurveMapWnd_Y; 
 m_optFtlsCorMapWnd1.value.x = i_wndSettings.m_FtlsCorMapWnd_X;
 m_optFtlsCorMapWnd1.value.y = i_wndSettings.m_FtlsCorMapWnd_Y; 
 m_optLambdaZoneMapWnd1.value.x = i_wndSettings.m_LambdaZoneMapWnd_X;
 m_optLambdaZoneMapWnd1.value.y = i_wndSettings.m_LambdaZoneMapWnd_Y; 
 m_optFtsCurveMapWnd1.value.x = i_wndSettings.m_FtsCurveMapWnd_X;
 m_optFtsCurveMapWnd1.value.y = i_wndSettings.m_FtsCurveMapWnd_Y; 
 m_optFuelDensCorrMapWnd1.value.x = i_wndSettings.m_FuelDensCorrMapWnd_X;
 m_optFuelDensCorrMapWnd1.value.y = i_wndSettings.m_FuelDensCorrMapWnd_Y; 
 m_optXtauXfAccMapWnd1.value.x = i_wndSettings.m_XtauXfAccMapWnd_X;
 m_optXtauXfAccMapWnd1.value.y = i_wndSettings.m_XtauXfAccMapWnd_Y; 
 m_optXtauXfDecMapWnd1.value.x = i_wndSettings.m_XtauXfDecMapWnd_X;
 m_optXtauXfDecMapWnd1.value.y = i_wndSettings.m_XtauXfDecMapWnd_Y; 
 m_optXtauTfAccMapWnd1.value.x = i_wndSettings.m_XtauTfAccMapWnd_X;
 m_optXtauTfAccMapWnd1.value.y = i_wndSettings.m_XtauTfAccMapWnd_Y; 
 m_optXtauTfDecMapWnd1.value.x = i_wndSettings.m_XtauTfDecMapWnd_X;
 m_optXtauTfDecMapWnd1.value.y = i_wndSettings.m_XtauTfDecMapWnd_Y; 
 m_optInjNonLinPMapWnd1.value.x = i_wndSettings.m_InjNonLinPMapWnd_X;
 m_optInjNonLinPMapWnd1.value.y = i_wndSettings.m_InjNonLinPMapWnd_Y; 
 m_optInjNonLinGMapWnd1.value.x = i_wndSettings.m_InjNonLinGMapWnd_X;
 m_optInjNonLinGMapWnd1.value.y = i_wndSettings.m_InjNonLinGMapWnd_Y; 
 m_optEGODelayMapWnd1.value.x = i_wndSettings.m_EGODelayMapWnd_X;
 m_optEGODelayMapWnd1.value.y = i_wndSettings.m_EGODelayMapWnd_Y;
 m_optWUAFR0MapWnd1.value.x = i_wndSettings.m_WUAFR0MapWnd_X;
 m_optWUAFR0MapWnd1.value.y = i_wndSettings.m_WUAFR0MapWnd_Y;
 m_optWUAFR1MapWnd1.value.x = i_wndSettings.m_WUAFR1MapWnd_X;
 m_optWUAFR1MapWnd1.value.y = i_wndSettings.m_WUAFR1MapWnd_Y;
}

void CAppSettingsModel::GetWndSettings1(WndSettings& o_wndSettings) const
{
 o_wndSettings.m_StrtMapWnd_X = m_optStrtMapWnd1.value.x;
 o_wndSettings.m_StrtMapWnd_Y = m_optStrtMapWnd1.value.y;
 o_wndSettings.m_IdleMapWnd_X = m_optIdleMapWnd1.value.x;
 o_wndSettings.m_IdleMapWnd_Y = m_optIdleMapWnd1.value.y;
 o_wndSettings.m_WorkMapWnd_X = m_optWorkMapWnd1.value.x;
 o_wndSettings.m_WorkMapWnd_Y = m_optWorkMapWnd1.value.y;
 o_wndSettings.m_TempMapWnd_X = m_optTempMapWnd1.value.x;
 o_wndSettings.m_TempMapWnd_Y = m_optTempMapWnd1.value.y;
 o_wndSettings.m_TempIdlMapWnd_X = m_optTempIdlMapWnd1.value.x;
 o_wndSettings.m_TempIdlMapWnd_Y = m_optTempIdlMapWnd1.value.y;
 o_wndSettings.m_GridMapIgnWnd_X = m_optGridMapIgnWnd1.value.x;
 o_wndSettings.m_GridMapIgnWnd_Y = m_optGridMapIgnWnd1.value.y;
 o_wndSettings.m_GridMapInjWnd_X = m_optGridMapInjWnd1.value.x;
 o_wndSettings.m_GridMapInjWnd_Y = m_optGridMapInjWnd1.value.y;
 o_wndSettings.m_VEMapWnd_X = m_optVEMapWnd1.value.x;
 o_wndSettings.m_VEMapWnd_Y = m_optVEMapWnd1.value.y;
 o_wndSettings.m_VE2MapWnd_X = m_optVE2MapWnd1.value.x;
 o_wndSettings.m_VE2MapWnd_Y = m_optVE2MapWnd1.value.y;
 o_wndSettings.m_VEIMapWnd_X = m_optVEIMapWnd1.value.x;
 o_wndSettings.m_VEIMapWnd_Y = m_optVEIMapWnd1.value.y;
 o_wndSettings.m_AFRMapWnd_X = m_optAFRMapWnd1.value.x;
 o_wndSettings.m_AFRMapWnd_Y = m_optAFRMapWnd1.value.y;
 o_wndSettings.m_CrnkMapWnd_X = m_optCrnkMapWnd1.value.x;
 o_wndSettings.m_CrnkMapWnd_Y = m_optCrnkMapWnd1.value.y;
 o_wndSettings.m_WrmpMapWnd_X = m_optWrmpMapWnd1.value.x;
 o_wndSettings.m_WrmpMapWnd_Y = m_optWrmpMapWnd1.value.y;
 o_wndSettings.m_DeadMapWnd_X = m_optDeadMapWnd1.value.x;
 o_wndSettings.m_DeadMapWnd_Y = m_optDeadMapWnd1.value.y;
 o_wndSettings.m_IdlrMapWnd_X = m_optIdlrMapWnd1.value.x;
 o_wndSettings.m_IdlrMapWnd_Y = m_optIdlrMapWnd1.value.y;
 o_wndSettings.m_IdlcMapWnd_X = m_optIdlcMapWnd1.value.x;
 o_wndSettings.m_IdlcMapWnd_Y = m_optIdlcMapWnd1.value.y;
 o_wndSettings.m_ThrassMapWnd_X = m_optThrassMapWnd1.value.x;
 o_wndSettings.m_ThrassMapWnd_Y = m_optThrassMapWnd1.value.y;
 o_wndSettings.m_AETPSMapWnd_X = m_optAETPSMapWnd1.value.x;
 o_wndSettings.m_AETPSMapWnd_Y = m_optAETPSMapWnd1.value.y;
 o_wndSettings.m_AEMAPMapWnd_X = m_optAEMAPMapWnd1.value.x;
 o_wndSettings.m_AEMAPMapWnd_Y = m_optAEMAPMapWnd1.value.y;
 o_wndSettings.m_AERPMMapWnd_X = m_optAERPMMapWnd1.value.x;
 o_wndSettings.m_AERPMMapWnd_Y = m_optAERPMMapWnd1.value.y;
 o_wndSettings.m_AftstrMapWnd_X = m_optAftstrMapWnd1.value.x;
 o_wndSettings.m_AftstrMapWnd_Y = m_optAftstrMapWnd1.value.y;
 o_wndSettings.m_ITMapWnd_X = m_optITMapWnd1.value.x;
 o_wndSettings.m_ITMapWnd_Y = m_optITMapWnd1.value.y;
 o_wndSettings.m_ITRPMMapWnd_X = m_optITRPMMapWnd1.value.x;
 o_wndSettings.m_ITRPMMapWnd_Y = m_optITRPMMapWnd1.value.y;
 o_wndSettings.m_RigidMapWnd_X = m_optRigidMapWnd1.value.x;
 o_wndSettings.m_RigidMapWnd_Y = m_optRigidMapWnd1.value.y;
 o_wndSettings.m_EGOCrvMapWnd_X = m_optEGOCrvMapWnd1.value.x;
 o_wndSettings.m_EGOCrvMapWnd_Y = m_optEGOCrvMapWnd1.value.y;
 o_wndSettings.m_IACCMapWnd_X = m_optIACCMapWnd1.value.x;
 o_wndSettings.m_IACCMapWnd_Y = m_optIACCMapWnd1.value.y;
 o_wndSettings.m_IACCWMapWnd_X = m_optIACCWMapWnd1.value.x;
 o_wndSettings.m_IACCWMapWnd_Y = m_optIACCWMapWnd1.value.y;
 o_wndSettings.m_IATCLTMapWnd_X = m_optIATCLTMapWnd1.value.x;
 o_wndSettings.m_IATCLTMapWnd_Y = m_optIATCLTMapWnd1.value.y;
 o_wndSettings.m_TpsswtMapWnd_X = m_optTpsswtMapWnd1.value.x;
 o_wndSettings.m_TpsswtMapWnd_Y = m_optTpsswtMapWnd1.value.y;
 o_wndSettings.m_GtscMapWnd_X = m_optGtscMapWnd1.value.x;
 o_wndSettings.m_GtscMapWnd_Y = m_optGtscMapWnd1.value.y;
 o_wndSettings.m_GpscMapWnd_X = m_optGpscMapWnd1.value.x;
 o_wndSettings.m_GpscMapWnd_Y = m_optGpscMapWnd1.value.y;
 o_wndSettings.m_AtscMapWnd_X = m_optAtscMapWnd1.value.x;
 o_wndSettings.m_AtscMapWnd_Y = m_optAtscMapWnd1.value.y;
 o_wndSettings.m_Pwm1MapWnd_X = m_optPwm1MapWnd1.value.x;
 o_wndSettings.m_Pwm1MapWnd_Y = m_optPwm1MapWnd1.value.y;
 o_wndSettings.m_Pwm2MapWnd_X = m_optPwm2MapWnd1.value.x;
 o_wndSettings.m_Pwm2MapWnd_Y = m_optPwm2MapWnd1.value.y;
 o_wndSettings.m_IACMATMapWnd_X = m_optIACMATMapWnd1.value.x;
 o_wndSettings.m_IACMATMapWnd_Y = m_optIACMATMapWnd1.value.y;
 o_wndSettings.m_TpszonMapWnd_X = m_optTpszonMapWnd1.value.x;
 o_wndSettings.m_TpszonMapWnd_Y = m_optTpszonMapWnd1.value.y;
 o_wndSettings.m_CylMultMapWnd_X = m_optCylMultMapWnd1.value.x;
 o_wndSettings.m_CylMultMapWnd_Y = m_optCylMultMapWnd1.value.y;
 o_wndSettings.m_CylAddMapWnd_X = m_optCylAddMapWnd1.value.x;
 o_wndSettings.m_CylAddMapWnd_Y = m_optCylAddMapWnd1.value.y;
 //group of separate maps
 o_wndSettings.m_AttenuatorMapWnd_X = m_optAttenuatorMapWnd1.value.x;
 o_wndSettings.m_AttenuatorMapWnd_Y = m_optAttenuatorMapWnd1.value.y;
 o_wndSettings.m_DwellCntrlMapWnd_X = m_optDwellCntrlMapWnd1.value.x;
 o_wndSettings.m_DwellCntrlMapWnd_Y = m_optDwellCntrlMapWnd1.value.y;
 o_wndSettings.m_CTSCurveMapWnd_X = m_optCTSCurveMapWnd1.value.x;
 o_wndSettings.m_CTSCurveMapWnd_Y = m_optCTSCurveMapWnd1.value.y;
 o_wndSettings.m_ATSCurvMapWnd_X = m_optATSCurvMapWnd1.value.x;
 o_wndSettings.m_ATSCurvMapWnd_Y = m_optATSCurvMapWnd1.value.y;
 o_wndSettings.m_ATSCorrMapWnd_X = m_optATSCorrMapWnd1.value.x;
 o_wndSettings.m_ATSCorrMapWnd_Y = m_optATSCorrMapWnd1.value.y;
 o_wndSettings.m_GasdoseMapWnd_X = m_optGasdoseMapWnd1.value.x;
 o_wndSettings.m_GasdoseMapWnd_Y = m_optGasdoseMapWnd1.value.y;
 o_wndSettings.m_BarocorrMapWnd_X = m_optBarocorrMapWnd1.value.x;
 o_wndSettings.m_BarocorrMapWnd_Y = m_optBarocorrMapWnd1.value.y;
 o_wndSettings.m_ManIgntimMapWnd_X = m_optManIgntimMapWnd1.value.x;
 o_wndSettings.m_ManIgntimMapWnd_Y = m_optManIgntimMapWnd1.value.y;
 o_wndSettings.m_CESettingsWnd_X = m_optCESettingsWnd1.value.x;
 o_wndSettings.m_CESettingsWnd_Y = m_optCESettingsWnd1.value.y;
 o_wndSettings.m_Tmp2CurveMapWnd_X = m_optTmp2CurveMapWnd1.value.x;
 o_wndSettings.m_Tmp2CurveMapWnd_Y = m_optTmp2CurveMapWnd1.value.y;
 o_wndSettings.m_CrkTempMapWnd_X = m_optCrkTempMapWnd1.value.x;
 o_wndSettings.m_CrkTempMapWnd_Y = m_optCrkTempMapWnd1.value.y;
 o_wndSettings.m_EHPauseMapWnd_X = m_optEHPauseMapWnd1.value.x;
 o_wndSettings.m_EHPauseMapWnd_Y = m_optEHPauseMapWnd1.value.y;
 o_wndSettings.m_CrankingThrdMapWnd_X = m_optCrankingThrdMapWnd1.value.x;
 o_wndSettings.m_CrankingThrdMapWnd_Y = m_optCrankingThrdMapWnd1.value.y;
 o_wndSettings.m_CrankingTimeMapWnd_X = m_optCrankingTimeMapWnd1.value.x;
 o_wndSettings.m_CrankingTimeMapWnd_Y = m_optCrankingTimeMapWnd1.value.y;
 o_wndSettings.m_SmapabanThrdMapWnd_X = m_optSmapabanThrdMapWnd1.value.x;
 o_wndSettings.m_SmapabanThrdMapWnd_Y = m_optSmapabanThrdMapWnd1.value.y;
 o_wndSettings.m_KnockZoneMapWnd_X = m_optKnockZoneMapWnd1.value.x;
 o_wndSettings.m_KnockZoneMapWnd_Y = m_optKnockZoneMapWnd1.value.y;
 o_wndSettings.m_GrtsCurveMapWnd_X = m_optGrtsCurveMapWnd1.value.x;
 o_wndSettings.m_GrtsCurveMapWnd_Y = m_optGrtsCurveMapWnd1.value.y;
 o_wndSettings.m_GrHeatDutyMapWnd_X = m_optGrHeatDutyMapWnd1.value.x;
 o_wndSettings.m_GrHeatDutyMapWnd_Y = m_optGrHeatDutyMapWnd1.value.y;
 o_wndSettings.m_PwmIacUCoefMapWnd_X = m_optPwmIacUCoefMapWnd1.value.x;
 o_wndSettings.m_PwmIacUCoefMapWnd_Y = m_optPwmIacUCoefMapWnd1.value.y;
 o_wndSettings.m_AftstrStrk0MapWnd_X = m_optAftstrStrk0MapWnd1.value.x;
 o_wndSettings.m_AftstrStrk0MapWnd_Y = m_optAftstrStrk0MapWnd1.value.y;
 o_wndSettings.m_AftstrStrk1MapWnd_X = m_optAftstrStrk1MapWnd1.value.x;
 o_wndSettings.m_AftstrStrk1MapWnd_Y = m_optAftstrStrk1MapWnd1.value.y;
 o_wndSettings.m_GrValDelMapWnd_X = m_optGrValDelMapWnd1.value.x;
 o_wndSettings.m_GrValDelMapWnd_Y = m_optGrValDelMapWnd1.value.y;
 o_wndSettings.m_FtlsCurveMapWnd_X = m_optFtlsCurveMapWnd1.value.x;
 o_wndSettings.m_FtlsCurveMapWnd_Y = m_optFtlsCurveMapWnd1.value.y;
 o_wndSettings.m_EgtsCurveMapWnd_X = m_optEgtsCurveMapWnd1.value.x;
 o_wndSettings.m_EgtsCurveMapWnd_Y = m_optEgtsCurveMapWnd1.value.y;
 o_wndSettings.m_OpsCurveMapWnd_X = m_optOpsCurveMapWnd1.value.x;
 o_wndSettings.m_OpsCurveMapWnd_Y = m_optOpsCurveMapWnd1.value.y;
 o_wndSettings.m_ManInjPwcMapWnd_X = m_optManInjPwcMapWnd1.value.x;
 o_wndSettings.m_ManInjPwcMapWnd_Y = m_optManInjPwcMapWnd1.value.y;
 o_wndSettings.m_MAFCurveMapWnd_X = m_optMAFCurveMapWnd1.value.x;
 o_wndSettings.m_MAFCurveMapWnd_Y = m_optMAFCurveMapWnd1.value.y;
 o_wndSettings.m_FtlsCorMapWnd_X = m_optFtlsCorMapWnd1.value.x;
 o_wndSettings.m_FtlsCorMapWnd_Y = m_optFtlsCorMapWnd1.value.y;
 o_wndSettings.m_LambdaZoneMapWnd_X = m_optLambdaZoneMapWnd1.value.x;
 o_wndSettings.m_LambdaZoneMapWnd_Y = m_optLambdaZoneMapWnd1.value.y;
 o_wndSettings.m_FtsCurveMapWnd_X = m_optFtsCurveMapWnd1.value.x;
 o_wndSettings.m_FtsCurveMapWnd_Y = m_optFtsCurveMapWnd1.value.y;
 o_wndSettings.m_FuelDensCorrMapWnd_X = m_optFuelDensCorrMapWnd1.value.x;
 o_wndSettings.m_FuelDensCorrMapWnd_Y = m_optFuelDensCorrMapWnd1.value.y;
 o_wndSettings.m_XtauXfAccMapWnd_X = m_optXtauXfAccMapWnd1.value.x;
 o_wndSettings.m_XtauXfAccMapWnd_Y = m_optXtauXfAccMapWnd1.value.y;
 o_wndSettings.m_XtauXfDecMapWnd_X = m_optXtauXfDecMapWnd1.value.x;
 o_wndSettings.m_XtauXfDecMapWnd_Y = m_optXtauXfDecMapWnd1.value.y;
 o_wndSettings.m_XtauTfAccMapWnd_X = m_optXtauTfAccMapWnd1.value.x;
 o_wndSettings.m_XtauTfAccMapWnd_Y = m_optXtauTfAccMapWnd1.value.y;
 o_wndSettings.m_XtauTfDecMapWnd_X = m_optXtauTfDecMapWnd1.value.x;
 o_wndSettings.m_XtauTfDecMapWnd_Y = m_optXtauTfDecMapWnd1.value.y;
 o_wndSettings.m_InjNonLinPMapWnd_X = m_optInjNonLinPMapWnd1.value.x;
 o_wndSettings.m_InjNonLinPMapWnd_Y = m_optInjNonLinPMapWnd1.value.y;
 o_wndSettings.m_InjNonLinGMapWnd_X = m_optInjNonLinGMapWnd1.value.x;
 o_wndSettings.m_InjNonLinGMapWnd_Y = m_optInjNonLinGMapWnd1.value.y;
 o_wndSettings.m_EGODelayMapWnd_X = m_optEGODelayMapWnd1.value.x;
 o_wndSettings.m_EGODelayMapWnd_Y = m_optEGODelayMapWnd1.value.y;
 o_wndSettings.m_WUAFR0MapWnd_X = m_optWUAFR0MapWnd1.value.x;
 o_wndSettings.m_WUAFR0MapWnd_Y = m_optWUAFR0MapWnd1.value.y;
 o_wndSettings.m_WUAFR1MapWnd_X = m_optWUAFR1MapWnd1.value.x;
 o_wndSettings.m_WUAFR1MapWnd_Y = m_optWUAFR1MapWnd1.value.y;
}

void CAppSettingsModel::SetWndSize(const WndSize& i_wndSize)
{
 m_optMainFrmWndSize.value.x = i_wndSize.m_MainFrmWnd_W; //main window
 m_optMainFrmWndSize.value.y = i_wndSize.m_MainFrmWnd_H; //main window
 m_optStrtMapWndSize.value.x = i_wndSize.m_StrtMapWnd_W;
 m_optStrtMapWndSize.value.y = i_wndSize.m_StrtMapWnd_H;
 m_optIdleMapWndSize.value.x = i_wndSize.m_IdleMapWnd_W;
 m_optIdleMapWndSize.value.y = i_wndSize.m_IdleMapWnd_H;
 m_optWorkMapWndSize.value.x = i_wndSize.m_WorkMapWnd_W;
 m_optWorkMapWndSize.value.y = i_wndSize.m_WorkMapWnd_H;
 m_optTempMapWndSize.value.x = i_wndSize.m_TempMapWnd_W;
 m_optTempMapWndSize.value.y = i_wndSize.m_TempMapWnd_H;
 m_optTempIdlMapWndSize.value.x = i_wndSize.m_TempIdlMapWnd_W;
 m_optTempIdlMapWndSize.value.y = i_wndSize.m_TempIdlMapWnd_H;
 m_optGridMapIgnWndSize.value.x = i_wndSize.m_GridMapIgnWnd_W;
 m_optGridMapIgnWndSize.value.y = i_wndSize.m_GridMapIgnWnd_H;
 m_optGridMapInjWndSize.value.x = i_wndSize.m_GridMapInjWnd_W;
 m_optGridMapInjWndSize.value.y = i_wndSize.m_GridMapInjWnd_H;
 m_optVEMapWndSize.value.x = i_wndSize.m_VEMapWnd_W;
 m_optVEMapWndSize.value.y = i_wndSize.m_VEMapWnd_H;
 m_optVE2MapWndSize.value.x = i_wndSize.m_VE2MapWnd_W;
 m_optVE2MapWndSize.value.y = i_wndSize.m_VE2MapWnd_H;
 m_optVEIMapWndSize.value.x = i_wndSize.m_VEIMapWnd_W;
 m_optVEIMapWndSize.value.y = i_wndSize.m_VEIMapWnd_H;
 m_optAFRMapWndSize.value.x = i_wndSize.m_AFRMapWnd_W;
 m_optAFRMapWndSize.value.y = i_wndSize.m_AFRMapWnd_H;
 m_optCrnkMapWndSize.value.x = i_wndSize.m_CrnkMapWnd_W;
 m_optCrnkMapWndSize.value.y = i_wndSize.m_CrnkMapWnd_H;
 m_optWrmpMapWndSize.value.x = i_wndSize.m_WrmpMapWnd_W;
 m_optWrmpMapWndSize.value.y = i_wndSize.m_WrmpMapWnd_H;
 m_optDeadMapWndSize.value.x = i_wndSize.m_DeadMapWnd_W;
 m_optDeadMapWndSize.value.y = i_wndSize.m_DeadMapWnd_H;
 m_optIdlrMapWndSize.value.x = i_wndSize.m_IdlrMapWnd_W;
 m_optIdlrMapWndSize.value.y = i_wndSize.m_IdlrMapWnd_H;
 m_optIdlcMapWndSize.value.x = i_wndSize.m_IdlcMapWnd_W;
 m_optIdlcMapWndSize.value.y = i_wndSize.m_IdlcMapWnd_H;
 m_optThrassMapWndSize.value.x = i_wndSize.m_ThrassMapWnd_W;
 m_optThrassMapWndSize.value.y = i_wndSize.m_ThrassMapWnd_H;
 m_optAETPSMapWndSize.value.x = i_wndSize.m_AETPSMapWnd_W;
 m_optAETPSMapWndSize.value.y = i_wndSize.m_AETPSMapWnd_H; 
 m_optAEMAPMapWndSize.value.x = i_wndSize.m_AEMAPMapWnd_W;
 m_optAEMAPMapWndSize.value.y = i_wndSize.m_AEMAPMapWnd_H; 
 m_optAERPMMapWndSize.value.x = i_wndSize.m_AERPMMapWnd_W;
 m_optAERPMMapWndSize.value.y = i_wndSize.m_AERPMMapWnd_H; 
 m_optAftstrMapWndSize.value.x = i_wndSize.m_AftstrMapWnd_W;
 m_optAftstrMapWndSize.value.y = i_wndSize.m_AftstrMapWnd_H; 
 m_optITMapWndSize.value.x = i_wndSize.m_ITMapWnd_W;
 m_optITMapWndSize.value.y = i_wndSize.m_ITMapWnd_H;
 m_optITRPMMapWndSize.value.x = i_wndSize.m_ITRPMMapWnd_W;
 m_optITRPMMapWndSize.value.y = i_wndSize.m_ITRPMMapWnd_H;
 m_optRigidMapWndSize.value.x = i_wndSize.m_RigidMapWnd_W;
 m_optRigidMapWndSize.value.y = i_wndSize.m_RigidMapWnd_H;
 m_optEGOCrvMapWndSize.value.x = i_wndSize.m_EGOCrvMapWnd_W;
 m_optEGOCrvMapWndSize.value.y = i_wndSize.m_EGOCrvMapWnd_H;
 m_optIACCMapWndSize.value.x = i_wndSize.m_IACCMapWnd_W;
 m_optIACCMapWndSize.value.y = i_wndSize.m_IACCMapWnd_H;
 m_optIACCWMapWndSize.value.x = i_wndSize.m_IACCWMapWnd_W;
 m_optIACCWMapWndSize.value.y = i_wndSize.m_IACCWMapWnd_H;
 m_optIATCLTMapWndSize.value.x = i_wndSize.m_IATCLTMapWnd_W;
 m_optIATCLTMapWndSize.value.y = i_wndSize.m_IATCLTMapWnd_H;
 m_optTpsswtMapWndSize.value.x = i_wndSize.m_TpsswtMapWnd_W;
 m_optTpsswtMapWndSize.value.y = i_wndSize.m_TpsswtMapWnd_H;
 m_optTpszonMapWndSize.value.x = i_wndSize.m_TpszonMapWnd_W;
 m_optTpszonMapWndSize.value.y = i_wndSize.m_TpszonMapWnd_H;
 m_optGtscMapWndSize.value.x = i_wndSize.m_GtscMapWnd_W;
 m_optGtscMapWndSize.value.y = i_wndSize.m_GtscMapWnd_H;
 m_optGpscMapWndSize.value.x = i_wndSize.m_GpscMapWnd_W;
 m_optGpscMapWndSize.value.y = i_wndSize.m_GpscMapWnd_H;
 m_optAtscMapWndSize.value.x = i_wndSize.m_AtscMapWnd_W;
 m_optAtscMapWndSize.value.y = i_wndSize.m_AtscMapWnd_H;
 m_optPwm1MapWndSize.value.x = i_wndSize.m_Pwm1MapWnd_W;
 m_optPwm1MapWndSize.value.y = i_wndSize.m_Pwm1MapWnd_H;
 m_optPwm2MapWndSize.value.x = i_wndSize.m_Pwm2MapWnd_W;
 m_optPwm2MapWndSize.value.y = i_wndSize.m_Pwm2MapWnd_H;
 m_optIACMATMapWndSize.value.x = i_wndSize.m_IACMATMapWnd_W;
 m_optIACMATMapWndSize.value.y = i_wndSize.m_IACMATMapWnd_H;
 m_optCylMultMapWndSize.value.x = i_wndSize.m_CylMultMapWnd_W;
 m_optCylMultMapWndSize.value.y = i_wndSize.m_CylMultMapWnd_H;
 m_optCylAddMapWndSize.value.x = i_wndSize.m_CylAddMapWnd_W;
 m_optCylAddMapWndSize.value.y = i_wndSize.m_CylAddMapWnd_H;
 //group of separate maps
 m_optAttenuatorMapWndSize.value.x = i_wndSize.m_AttenuatorMapWnd_W;
 m_optAttenuatorMapWndSize.value.y = i_wndSize.m_AttenuatorMapWnd_H;
 m_optDwellCntrlMapWndSize.value.x = i_wndSize.m_DwellCntrlMapWnd_W;
 m_optDwellCntrlMapWndSize.value.y = i_wndSize.m_DwellCntrlMapWnd_H;
 m_optCTSCurveMapWndSize.value.x = i_wndSize.m_CTSCurveMapWnd_W;
 m_optCTSCurveMapWndSize.value.y = i_wndSize.m_CTSCurveMapWnd_H;
 m_optATSCurvMapWndSize.value.x = i_wndSize.m_ATSCurvMapWnd_W;
 m_optATSCurvMapWndSize.value.y = i_wndSize.m_ATSCurvMapWnd_H;
 m_optATSCorrMapWndSize.value.x = i_wndSize.m_ATSCorrMapWnd_W;
 m_optATSCorrMapWndSize.value.y = i_wndSize.m_ATSCorrMapWnd_H; 
 m_optGasdoseMapWndSize.value.x = i_wndSize.m_GasdoseMapWnd_W;
 m_optGasdoseMapWndSize.value.y = i_wndSize.m_GasdoseMapWnd_H; 
 m_optBarocorrMapWndSize.value.x = i_wndSize.m_BarocorrMapWnd_W;
 m_optBarocorrMapWndSize.value.y = i_wndSize.m_BarocorrMapWnd_H; 
 m_optManIgntimMapWndSize.value.x = i_wndSize.m_ManIgntimMapWnd_W;
 m_optManIgntimMapWndSize.value.y = i_wndSize.m_ManIgntimMapWnd_H; 
 m_optCESettingsWndSize.value.x = i_wndSize.m_CESettingsWnd_W;
 m_optCESettingsWndSize.value.y = i_wndSize.m_CESettingsWnd_H; 
 m_optTmp2CurveMapWndSize.value.x = i_wndSize.m_Tmp2CurveMapWnd_W;
 m_optTmp2CurveMapWndSize.value.y = i_wndSize.m_Tmp2CurveMapWnd_H; 
 m_optCrkTempMapWndSize.value.x = i_wndSize.m_CrkTempMapWnd_W;
 m_optCrkTempMapWndSize.value.y = i_wndSize.m_CrkTempMapWnd_H;
 m_optEHPauseMapWndSize.value.x = i_wndSize.m_EHPauseMapWnd_W;
 m_optEHPauseMapWndSize.value.y = i_wndSize.m_EHPauseMapWnd_H;
 m_optCrankingThrdMapWndSize.value.x = i_wndSize.m_CrankingThrdMapWnd_W;
 m_optCrankingThrdMapWndSize.value.y = i_wndSize.m_CrankingThrdMapWnd_H;
 m_optCrankingTimeMapWndSize.value.x = i_wndSize.m_CrankingTimeMapWnd_W;
 m_optCrankingTimeMapWndSize.value.y = i_wndSize.m_CrankingTimeMapWnd_H;
 m_optSmapabanThrdMapWndSize.value.x = i_wndSize.m_SmapabanThrdMapWnd_W;
 m_optSmapabanThrdMapWndSize.value.y = i_wndSize.m_SmapabanThrdMapWnd_H;
 m_optKnockZoneMapWndSize.value.x = i_wndSize.m_KnockZoneMapWnd_W;
 m_optKnockZoneMapWndSize.value.y = i_wndSize.m_KnockZoneMapWnd_H; 
 m_optGrtsCurveMapWndSize.value.x = i_wndSize.m_GrtsCurveMapWnd_W;
 m_optGrtsCurveMapWndSize.value.y = i_wndSize.m_GrtsCurveMapWnd_H; 
 m_optGrHeatDutyMapWndSize.value.x = i_wndSize.m_GrHeatDutyMapWnd_W;
 m_optGrHeatDutyMapWndSize.value.y = i_wndSize.m_GrHeatDutyMapWnd_H;
 m_optPwmIacUCoefMapWndSize.value.x = i_wndSize.m_PwmIacUCoefMapWnd_W;
 m_optPwmIacUCoefMapWndSize.value.y = i_wndSize.m_PwmIacUCoefMapWnd_H;
 m_optAftstrStrk0MapWndSize.value.x = i_wndSize.m_AftstrStrk0MapWnd_W;
 m_optAftstrStrk0MapWndSize.value.y = i_wndSize.m_AftstrStrk0MapWnd_H;
 m_optAftstrStrk1MapWndSize.value.x = i_wndSize.m_AftstrStrk1MapWnd_W;
 m_optAftstrStrk1MapWndSize.value.y = i_wndSize.m_AftstrStrk1MapWnd_H;
 m_optGrValDelMapWndSize.value.x = i_wndSize.m_GrValDelMapWnd_W;
 m_optGrValDelMapWndSize.value.y = i_wndSize.m_GrValDelMapWnd_H;
 m_optFtlsCurveMapWndSize.value.x = i_wndSize.m_FtlsCurveMapWnd_W;
 m_optFtlsCurveMapWndSize.value.y = i_wndSize.m_FtlsCurveMapWnd_H; 
 m_optEgtsCurveMapWndSize.value.x = i_wndSize.m_EgtsCurveMapWnd_W;
 m_optEgtsCurveMapWndSize.value.y = i_wndSize.m_EgtsCurveMapWnd_H; 
 m_optOpsCurveMapWndSize.value.x = i_wndSize.m_OpsCurveMapWnd_W;
 m_optOpsCurveMapWndSize.value.y = i_wndSize.m_OpsCurveMapWnd_H; 
 m_optManInjPwcMapWndSize.value.x = i_wndSize.m_ManInjPwcMapWnd_W;
 m_optManInjPwcMapWndSize.value.y = i_wndSize.m_ManInjPwcMapWnd_H; 
 m_optMAFCurveMapWndSize.value.x = i_wndSize.m_MAFCurveMapWnd_W;
 m_optMAFCurveMapWndSize.value.y = i_wndSize.m_MAFCurveMapWnd_H; 
 m_optFtlsCorMapWndSize.value.x = i_wndSize.m_FtlsCorMapWnd_W;
 m_optFtlsCorMapWndSize.value.y = i_wndSize.m_FtlsCorMapWnd_H; 
 m_optLambdaZoneMapWndSize.value.x = i_wndSize.m_LambdaZoneMapWnd_W;
 m_optLambdaZoneMapWndSize.value.y = i_wndSize.m_LambdaZoneMapWnd_H; 
 m_optFtsCurveMapWndSize.value.x = i_wndSize.m_FtsCurveMapWnd_W;
 m_optFtsCurveMapWndSize.value.y = i_wndSize.m_FtsCurveMapWnd_H; 
 m_optFuelDensCorrMapWndSize.value.x = i_wndSize.m_FuelDensCorrMapWnd_W;
 m_optFuelDensCorrMapWndSize.value.y = i_wndSize.m_FuelDensCorrMapWnd_H; 
 m_optXtauXfAccMapWndSize.value.x = i_wndSize.m_XtauXfAccMapWnd_W;
 m_optXtauXfAccMapWndSize.value.y = i_wndSize.m_XtauXfAccMapWnd_H; 
 m_optXtauXfDecMapWndSize.value.x = i_wndSize.m_XtauXfDecMapWnd_W;
 m_optXtauXfDecMapWndSize.value.y = i_wndSize.m_XtauXfDecMapWnd_H; 
 m_optXtauTfAccMapWndSize.value.x = i_wndSize.m_XtauTfAccMapWnd_W;
 m_optXtauTfAccMapWndSize.value.y = i_wndSize.m_XtauTfAccMapWnd_H; 
 m_optXtauTfDecMapWndSize.value.x = i_wndSize.m_XtauTfDecMapWnd_W;
 m_optXtauTfDecMapWndSize.value.y = i_wndSize.m_XtauTfDecMapWnd_H; 
 m_optInjNonLinPMapWndSize.value.x = i_wndSize.m_InjNonLinPMapWnd_W;
 m_optInjNonLinPMapWndSize.value.y = i_wndSize.m_InjNonLinPMapWnd_H; 
 m_optInjNonLinGMapWndSize.value.x = i_wndSize.m_InjNonLinGMapWnd_W;
 m_optInjNonLinGMapWndSize.value.y = i_wndSize.m_InjNonLinGMapWnd_H;
 m_optEGODelayMapWndSize.value.x = i_wndSize.m_EGODelayMapWnd_W;
 m_optEGODelayMapWndSize.value.y = i_wndSize.m_EGODelayMapWnd_H;
 m_optWUAFR0MapWndSize.value.x = i_wndSize.m_WUAFR0MapWnd_W;
 m_optWUAFR0MapWndSize.value.y = i_wndSize.m_WUAFR0MapWnd_H;
 m_optWUAFR1MapWndSize.value.x = i_wndSize.m_WUAFR1MapWnd_W;
 m_optWUAFR1MapWndSize.value.y = i_wndSize.m_WUAFR1MapWnd_H;
}

void CAppSettingsModel::GetWndSize(WndSize& o_wndSize) const
{
 o_wndSize.m_MainFrmWnd_W = m_optMainFrmWndSize.value.x;
 o_wndSize.m_MainFrmWnd_H = m_optMainFrmWndSize.value.y;
 o_wndSize.m_StrtMapWnd_W = m_optStrtMapWndSize.value.x;
 o_wndSize.m_StrtMapWnd_H = m_optStrtMapWndSize.value.y;
 o_wndSize.m_IdleMapWnd_W = m_optIdleMapWndSize.value.x;
 o_wndSize.m_IdleMapWnd_H = m_optIdleMapWndSize.value.y;
 o_wndSize.m_WorkMapWnd_W = m_optWorkMapWndSize.value.x;
 o_wndSize.m_WorkMapWnd_H = m_optWorkMapWndSize.value.y;
 o_wndSize.m_TempMapWnd_W = m_optTempMapWndSize.value.x;
 o_wndSize.m_TempMapWnd_H = m_optTempMapWndSize.value.y;
 o_wndSize.m_TempIdlMapWnd_W = m_optTempIdlMapWndSize.value.x;
 o_wndSize.m_TempIdlMapWnd_H = m_optTempIdlMapWndSize.value.y;
 o_wndSize.m_GridMapIgnWnd_W = m_optGridMapIgnWndSize.value.x;
 o_wndSize.m_GridMapIgnWnd_H = m_optGridMapIgnWndSize.value.y;
 o_wndSize.m_GridMapInjWnd_W = m_optGridMapInjWndSize.value.x;
 o_wndSize.m_GridMapInjWnd_H = m_optGridMapInjWndSize.value.y;
 o_wndSize.m_VEMapWnd_W = m_optVEMapWndSize.value.x;
 o_wndSize.m_VEMapWnd_H = m_optVEMapWndSize.value.y;
 o_wndSize.m_VE2MapWnd_W = m_optVE2MapWndSize.value.x;
 o_wndSize.m_VE2MapWnd_H = m_optVE2MapWndSize.value.y;
 o_wndSize.m_VEIMapWnd_W = m_optVEIMapWndSize.value.x;
 o_wndSize.m_VEIMapWnd_H = m_optVEIMapWndSize.value.y;
 o_wndSize.m_AFRMapWnd_W = m_optAFRMapWndSize.value.x;
 o_wndSize.m_AFRMapWnd_H = m_optAFRMapWndSize.value.y;
 o_wndSize.m_CrnkMapWnd_W = m_optCrnkMapWndSize.value.x;
 o_wndSize.m_CrnkMapWnd_H = m_optCrnkMapWndSize.value.y;
 o_wndSize.m_WrmpMapWnd_W = m_optWrmpMapWndSize.value.x;
 o_wndSize.m_WrmpMapWnd_H = m_optWrmpMapWndSize.value.y;
 o_wndSize.m_DeadMapWnd_W = m_optDeadMapWndSize.value.x;
 o_wndSize.m_DeadMapWnd_H = m_optDeadMapWndSize.value.y;
 o_wndSize.m_IdlrMapWnd_W = m_optIdlrMapWndSize.value.x;
 o_wndSize.m_IdlrMapWnd_H = m_optIdlrMapWndSize.value.y;
 o_wndSize.m_IdlcMapWnd_W = m_optIdlcMapWndSize.value.x;
 o_wndSize.m_IdlcMapWnd_H = m_optIdlcMapWndSize.value.y;
 o_wndSize.m_ThrassMapWnd_W = m_optThrassMapWndSize.value.x;
 o_wndSize.m_ThrassMapWnd_H = m_optThrassMapWndSize.value.y;
 o_wndSize.m_AETPSMapWnd_W = m_optAETPSMapWndSize.value.x;
 o_wndSize.m_AETPSMapWnd_H = m_optAETPSMapWndSize.value.y;
 o_wndSize.m_AEMAPMapWnd_W = m_optAEMAPMapWndSize.value.x;
 o_wndSize.m_AEMAPMapWnd_H = m_optAEMAPMapWndSize.value.y;
 o_wndSize.m_AERPMMapWnd_W = m_optAERPMMapWndSize.value.x;
 o_wndSize.m_AERPMMapWnd_H = m_optAERPMMapWndSize.value.y;
 o_wndSize.m_AftstrMapWnd_W = m_optAftstrMapWndSize.value.x;
 o_wndSize.m_AftstrMapWnd_H = m_optAftstrMapWndSize.value.y;
 o_wndSize.m_ITMapWnd_W = m_optITMapWndSize.value.x;
 o_wndSize.m_ITMapWnd_H = m_optITMapWndSize.value.y;
 o_wndSize.m_ITRPMMapWnd_W = m_optITRPMMapWndSize.value.x;
 o_wndSize.m_ITRPMMapWnd_H = m_optITRPMMapWndSize.value.y;
 o_wndSize.m_RigidMapWnd_W = m_optRigidMapWndSize.value.x;
 o_wndSize.m_RigidMapWnd_H = m_optRigidMapWndSize.value.y;
 o_wndSize.m_EGOCrvMapWnd_W = m_optEGOCrvMapWndSize.value.x;
 o_wndSize.m_EGOCrvMapWnd_H = m_optEGOCrvMapWndSize.value.y;
 o_wndSize.m_IACCMapWnd_W = m_optIACCMapWndSize.value.x;
 o_wndSize.m_IACCMapWnd_H = m_optIACCMapWndSize.value.y;
 o_wndSize.m_IACCWMapWnd_W = m_optIACCWMapWndSize.value.x;
 o_wndSize.m_IACCWMapWnd_H = m_optIACCWMapWndSize.value.y;
 o_wndSize.m_IATCLTMapWnd_W = m_optIATCLTMapWndSize.value.x;
 o_wndSize.m_IATCLTMapWnd_H = m_optIATCLTMapWndSize.value.y;
 o_wndSize.m_TpsswtMapWnd_W = m_optTpsswtMapWndSize.value.x;
 o_wndSize.m_TpsswtMapWnd_H = m_optTpsswtMapWndSize.value.y;
 o_wndSize.m_GtscMapWnd_W = m_optGtscMapWndSize.value.x;
 o_wndSize.m_GtscMapWnd_H = m_optGtscMapWndSize.value.y;
 o_wndSize.m_GpscMapWnd_W = m_optGpscMapWndSize.value.x;
 o_wndSize.m_GpscMapWnd_H = m_optGpscMapWndSize.value.y;
 o_wndSize.m_AtscMapWnd_W = m_optAtscMapWndSize.value.x;
 o_wndSize.m_AtscMapWnd_H = m_optAtscMapWndSize.value.y;
 o_wndSize.m_Pwm1MapWnd_W = m_optPwm1MapWndSize.value.x;
 o_wndSize.m_Pwm1MapWnd_H = m_optPwm1MapWndSize.value.y;
 o_wndSize.m_Pwm2MapWnd_W = m_optPwm2MapWndSize.value.x;
 o_wndSize.m_Pwm2MapWnd_H = m_optPwm2MapWndSize.value.y;
 o_wndSize.m_IACMATMapWnd_W = m_optIACMATMapWndSize.value.x;
 o_wndSize.m_IACMATMapWnd_H = m_optIACMATMapWndSize.value.y;
 o_wndSize.m_TpszonMapWnd_W = m_optTpszonMapWndSize.value.x;
 o_wndSize.m_TpszonMapWnd_H = m_optTpszonMapWndSize.value.y;
 o_wndSize.m_CylMultMapWnd_W = m_optCylMultMapWndSize.value.x;
 o_wndSize.m_CylMultMapWnd_H = m_optCylMultMapWndSize.value.y;
 o_wndSize.m_CylAddMapWnd_W = m_optCylAddMapWndSize.value.x;
 o_wndSize.m_CylAddMapWnd_H = m_optCylAddMapWndSize.value.y;
 //group of separate maps
 o_wndSize.m_AttenuatorMapWnd_W = m_optAttenuatorMapWndSize.value.x;
 o_wndSize.m_AttenuatorMapWnd_H = m_optAttenuatorMapWndSize.value.y;
 o_wndSize.m_DwellCntrlMapWnd_W = m_optDwellCntrlMapWndSize.value.x;
 o_wndSize.m_DwellCntrlMapWnd_H = m_optDwellCntrlMapWndSize.value.y;
 o_wndSize.m_CTSCurveMapWnd_W = m_optCTSCurveMapWndSize.value.x;
 o_wndSize.m_CTSCurveMapWnd_H = m_optCTSCurveMapWndSize.value.y;
 o_wndSize.m_ATSCurvMapWnd_W = m_optATSCurvMapWndSize.value.x;
 o_wndSize.m_ATSCurvMapWnd_H = m_optATSCurvMapWndSize.value.y;
 o_wndSize.m_ATSCorrMapWnd_W = m_optATSCorrMapWndSize.value.x;
 o_wndSize.m_ATSCorrMapWnd_H = m_optATSCorrMapWndSize.value.y;
 o_wndSize.m_GasdoseMapWnd_W = m_optGasdoseMapWndSize.value.x;
 o_wndSize.m_GasdoseMapWnd_H = m_optGasdoseMapWndSize.value.y;
 o_wndSize.m_BarocorrMapWnd_W = m_optBarocorrMapWndSize.value.x;
 o_wndSize.m_BarocorrMapWnd_H = m_optBarocorrMapWndSize.value.y;
 o_wndSize.m_ManIgntimMapWnd_W = m_optManIgntimMapWndSize.value.x;
 o_wndSize.m_ManIgntimMapWnd_H = m_optManIgntimMapWndSize.value.y;
 o_wndSize.m_CESettingsWnd_W = m_optCESettingsWndSize.value.x;
 o_wndSize.m_CESettingsWnd_H = m_optCESettingsWndSize.value.y;
 o_wndSize.m_Tmp2CurveMapWnd_W = m_optTmp2CurveMapWndSize.value.x;
 o_wndSize.m_Tmp2CurveMapWnd_H = m_optTmp2CurveMapWndSize.value.y;
 o_wndSize.m_CrkTempMapWnd_W = m_optCrkTempMapWndSize.value.x;
 o_wndSize.m_CrkTempMapWnd_H = m_optCrkTempMapWndSize.value.y;
 o_wndSize.m_EHPauseMapWnd_W = m_optEHPauseMapWndSize.value.x;
 o_wndSize.m_EHPauseMapWnd_H = m_optEHPauseMapWndSize.value.y;
 o_wndSize.m_CrankingThrdMapWnd_W = m_optCrankingThrdMapWndSize.value.x;
 o_wndSize.m_CrankingThrdMapWnd_H = m_optCrankingThrdMapWndSize.value.y;
 o_wndSize.m_CrankingTimeMapWnd_W = m_optCrankingTimeMapWndSize.value.x;
 o_wndSize.m_CrankingTimeMapWnd_H = m_optCrankingTimeMapWndSize.value.y;
 o_wndSize.m_SmapabanThrdMapWnd_W = m_optSmapabanThrdMapWndSize.value.x;
 o_wndSize.m_SmapabanThrdMapWnd_H = m_optSmapabanThrdMapWndSize.value.y;
 o_wndSize.m_KnockZoneMapWnd_W = m_optKnockZoneMapWndSize.value.x;
 o_wndSize.m_KnockZoneMapWnd_H = m_optKnockZoneMapWndSize.value.y;
 o_wndSize.m_GrtsCurveMapWnd_W = m_optGrtsCurveMapWndSize.value.x;
 o_wndSize.m_GrtsCurveMapWnd_H = m_optGrtsCurveMapWndSize.value.y;
 o_wndSize.m_GrHeatDutyMapWnd_W = m_optGrHeatDutyMapWndSize.value.x;
 o_wndSize.m_GrHeatDutyMapWnd_H = m_optGrHeatDutyMapWndSize.value.y;
 o_wndSize.m_PwmIacUCoefMapWnd_W = m_optPwmIacUCoefMapWndSize.value.x;
 o_wndSize.m_PwmIacUCoefMapWnd_H = m_optPwmIacUCoefMapWndSize.value.y;
 o_wndSize.m_AftstrStrk0MapWnd_W = m_optAftstrStrk0MapWndSize.value.x;
 o_wndSize.m_AftstrStrk0MapWnd_H = m_optAftstrStrk0MapWndSize.value.y;
 o_wndSize.m_AftstrStrk1MapWnd_W = m_optAftstrStrk1MapWndSize.value.x;
 o_wndSize.m_AftstrStrk1MapWnd_H = m_optAftstrStrk1MapWndSize.value.y;
 o_wndSize.m_GrValDelMapWnd_W = m_optGrValDelMapWndSize.value.x;
 o_wndSize.m_GrValDelMapWnd_H = m_optGrValDelMapWndSize.value.y;
 o_wndSize.m_FtlsCurveMapWnd_W = m_optFtlsCurveMapWndSize.value.x;
 o_wndSize.m_FtlsCurveMapWnd_H = m_optFtlsCurveMapWndSize.value.y;
 o_wndSize.m_EgtsCurveMapWnd_W = m_optEgtsCurveMapWndSize.value.x;
 o_wndSize.m_EgtsCurveMapWnd_H = m_optEgtsCurveMapWndSize.value.y;
 o_wndSize.m_OpsCurveMapWnd_W = m_optOpsCurveMapWndSize.value.x;
 o_wndSize.m_OpsCurveMapWnd_H = m_optOpsCurveMapWndSize.value.y;
 o_wndSize.m_ManInjPwcMapWnd_W = m_optManInjPwcMapWndSize.value.x;
 o_wndSize.m_ManInjPwcMapWnd_H = m_optManInjPwcMapWndSize.value.y;
 o_wndSize.m_MAFCurveMapWnd_W = m_optMAFCurveMapWndSize.value.x;
 o_wndSize.m_MAFCurveMapWnd_H = m_optMAFCurveMapWndSize.value.y;
 o_wndSize.m_FtlsCorMapWnd_W = m_optFtlsCorMapWndSize.value.x;
 o_wndSize.m_FtlsCorMapWnd_H = m_optFtlsCorMapWndSize.value.y;
 o_wndSize.m_LambdaZoneMapWnd_W = m_optLambdaZoneMapWndSize.value.x;
 o_wndSize.m_LambdaZoneMapWnd_H = m_optLambdaZoneMapWndSize.value.y;
 o_wndSize.m_FtsCurveMapWnd_W = m_optFtsCurveMapWndSize.value.x;
 o_wndSize.m_FtsCurveMapWnd_H = m_optFtsCurveMapWndSize.value.y;
 o_wndSize.m_FuelDensCorrMapWnd_W = m_optFuelDensCorrMapWndSize.value.x;
 o_wndSize.m_FuelDensCorrMapWnd_H = m_optFuelDensCorrMapWndSize.value.y;
 o_wndSize.m_XtauXfAccMapWnd_W = m_optXtauXfAccMapWndSize.value.x;
 o_wndSize.m_XtauXfAccMapWnd_H = m_optXtauXfAccMapWndSize.value.y;
 o_wndSize.m_XtauXfDecMapWnd_W = m_optXtauXfDecMapWndSize.value.x;
 o_wndSize.m_XtauXfDecMapWnd_H = m_optXtauXfDecMapWndSize.value.y;
 o_wndSize.m_XtauTfAccMapWnd_W = m_optXtauTfAccMapWndSize.value.x;
 o_wndSize.m_XtauTfAccMapWnd_H = m_optXtauTfAccMapWndSize.value.y;
 o_wndSize.m_XtauTfDecMapWnd_W = m_optXtauTfDecMapWndSize.value.x;
 o_wndSize.m_XtauTfDecMapWnd_H = m_optXtauTfDecMapWndSize.value.y;
 o_wndSize.m_InjNonLinPMapWnd_W = m_optInjNonLinPMapWndSize.value.x;
 o_wndSize.m_InjNonLinPMapWnd_H = m_optInjNonLinPMapWndSize.value.y;
 o_wndSize.m_InjNonLinGMapWnd_W = m_optInjNonLinGMapWndSize.value.x;
 o_wndSize.m_InjNonLinGMapWnd_H = m_optInjNonLinGMapWndSize.value.y;
 o_wndSize.m_EGODelayMapWnd_W = m_optEGODelayMapWndSize.value.x;
 o_wndSize.m_EGODelayMapWnd_H = m_optEGODelayMapWndSize.value.y;
 o_wndSize.m_WUAFR0MapWnd_W = m_optWUAFR0MapWndSize.value.x;
 o_wndSize.m_WUAFR0MapWnd_H = m_optWUAFR0MapWndSize.value.y;
 o_wndSize.m_WUAFR1MapWnd_W = m_optWUAFR1MapWndSize.value.x;
 o_wndSize.m_WUAFR1MapWnd_H = m_optWUAFR1MapWndSize.value.y;
}

void CAppSettingsModel::SetWndSize1(const WndSize& i_wndSize)
{
 m_optStrtMapWndSize1.value.x = i_wndSize.m_StrtMapWnd_W;
 m_optStrtMapWndSize1.value.y = i_wndSize.m_StrtMapWnd_H;
 m_optIdleMapWndSize1.value.x = i_wndSize.m_IdleMapWnd_W;
 m_optIdleMapWndSize1.value.y = i_wndSize.m_IdleMapWnd_H;
 m_optWorkMapWndSize1.value.x = i_wndSize.m_WorkMapWnd_W;
 m_optWorkMapWndSize1.value.y = i_wndSize.m_WorkMapWnd_H;
 m_optTempMapWndSize1.value.x = i_wndSize.m_TempMapWnd_W;
 m_optTempMapWndSize1.value.y = i_wndSize.m_TempMapWnd_H;
 m_optTempIdlMapWndSize1.value.x = i_wndSize.m_TempIdlMapWnd_W;
 m_optTempIdlMapWndSize1.value.y = i_wndSize.m_TempIdlMapWnd_H;
 m_optGridMapIgnWndSize1.value.x = i_wndSize.m_GridMapIgnWnd_W;
 m_optGridMapIgnWndSize1.value.y = i_wndSize.m_GridMapIgnWnd_H;
 m_optGridMapInjWndSize1.value.x = i_wndSize.m_GridMapInjWnd_W;
 m_optGridMapInjWndSize1.value.y = i_wndSize.m_GridMapInjWnd_H;
 m_optVEMapWndSize1.value.x = i_wndSize.m_VEMapWnd_W;
 m_optVEMapWndSize1.value.y = i_wndSize.m_VEMapWnd_H;
 m_optVE2MapWndSize1.value.x = i_wndSize.m_VE2MapWnd_W;
 m_optVE2MapWndSize1.value.y = i_wndSize.m_VE2MapWnd_H;
 m_optVEIMapWndSize1.value.x = i_wndSize.m_VEIMapWnd_W;
 m_optVEIMapWndSize1.value.y = i_wndSize.m_VEIMapWnd_H;
 m_optAFRMapWndSize1.value.x = i_wndSize.m_AFRMapWnd_W;
 m_optAFRMapWndSize1.value.y = i_wndSize.m_AFRMapWnd_H;
 m_optCrnkMapWndSize1.value.x = i_wndSize.m_CrnkMapWnd_W;
 m_optCrnkMapWndSize1.value.y = i_wndSize.m_CrnkMapWnd_H;
 m_optWrmpMapWndSize1.value.x = i_wndSize.m_WrmpMapWnd_W;
 m_optWrmpMapWndSize1.value.y = i_wndSize.m_WrmpMapWnd_H;
 m_optDeadMapWndSize1.value.x = i_wndSize.m_DeadMapWnd_W;
 m_optDeadMapWndSize1.value.y = i_wndSize.m_DeadMapWnd_H;
 m_optIdlrMapWndSize1.value.x = i_wndSize.m_IdlrMapWnd_W;
 m_optIdlrMapWndSize1.value.y = i_wndSize.m_IdlrMapWnd_H;
 m_optIdlcMapWndSize1.value.x = i_wndSize.m_IdlcMapWnd_W;
 m_optIdlcMapWndSize1.value.y = i_wndSize.m_IdlcMapWnd_H;
 m_optThrassMapWndSize1.value.x = i_wndSize.m_ThrassMapWnd_W;
 m_optThrassMapWndSize1.value.y = i_wndSize.m_ThrassMapWnd_H;
 m_optAETPSMapWndSize1.value.x = i_wndSize.m_AETPSMapWnd_W;
 m_optAETPSMapWndSize1.value.y = i_wndSize.m_AETPSMapWnd_H; 
 m_optAEMAPMapWndSize1.value.x = i_wndSize.m_AEMAPMapWnd_W;
 m_optAEMAPMapWndSize1.value.y = i_wndSize.m_AEMAPMapWnd_H; 
 m_optAERPMMapWndSize1.value.x = i_wndSize.m_AERPMMapWnd_W;
 m_optAERPMMapWndSize1.value.y = i_wndSize.m_AERPMMapWnd_H; 
 m_optAftstrMapWndSize1.value.x = i_wndSize.m_AftstrMapWnd_W;
 m_optAftstrMapWndSize1.value.y = i_wndSize.m_AftstrMapWnd_H; 
 m_optITMapWndSize1.value.x = i_wndSize.m_ITMapWnd_W;
 m_optITMapWndSize1.value.y = i_wndSize.m_ITMapWnd_H;
 m_optITRPMMapWndSize1.value.x = i_wndSize.m_ITRPMMapWnd_W;
 m_optITRPMMapWndSize1.value.y = i_wndSize.m_ITRPMMapWnd_H;
 m_optRigidMapWndSize1.value.x = i_wndSize.m_RigidMapWnd_W;
 m_optRigidMapWndSize1.value.y = i_wndSize.m_RigidMapWnd_H;
 m_optEGOCrvMapWndSize1.value.x = i_wndSize.m_EGOCrvMapWnd_W;
 m_optEGOCrvMapWndSize1.value.y = i_wndSize.m_EGOCrvMapWnd_H;
 m_optIACCMapWndSize1.value.x = i_wndSize.m_IACCMapWnd_W;
 m_optIACCMapWndSize1.value.y = i_wndSize.m_IACCMapWnd_H;
 m_optIACCWMapWndSize1.value.x = i_wndSize.m_IACCWMapWnd_W;
 m_optIACCWMapWndSize1.value.y = i_wndSize.m_IACCWMapWnd_H;
 m_optIATCLTMapWndSize1.value.x = i_wndSize.m_IATCLTMapWnd_W;
 m_optIATCLTMapWndSize1.value.y = i_wndSize.m_IATCLTMapWnd_H;
 m_optTpsswtMapWndSize1.value.x = i_wndSize.m_TpsswtMapWnd_W;
 m_optTpsswtMapWndSize1.value.y = i_wndSize.m_TpsswtMapWnd_H;
 m_optGtscMapWndSize1.value.x = i_wndSize.m_GtscMapWnd_W;
 m_optGtscMapWndSize1.value.y = i_wndSize.m_GtscMapWnd_H;
 m_optGpscMapWndSize1.value.x = i_wndSize.m_GpscMapWnd_W;
 m_optGpscMapWndSize1.value.y = i_wndSize.m_GpscMapWnd_H;
 m_optAtscMapWndSize1.value.x = i_wndSize.m_AtscMapWnd_W;
 m_optAtscMapWndSize1.value.y = i_wndSize.m_AtscMapWnd_H;
 m_optPwm1MapWndSize1.value.x = i_wndSize.m_Pwm1MapWnd_W;
 m_optPwm1MapWndSize1.value.y = i_wndSize.m_Pwm1MapWnd_H;
 m_optPwm2MapWndSize1.value.x = i_wndSize.m_Pwm2MapWnd_W;
 m_optPwm2MapWndSize1.value.y = i_wndSize.m_Pwm2MapWnd_H;
 m_optIACMATMapWndSize1.value.x = i_wndSize.m_IACMATMapWnd_W;
 m_optIACMATMapWndSize1.value.y = i_wndSize.m_IACMATMapWnd_H;
 m_optTpszonMapWndSize1.value.x = i_wndSize.m_TpszonMapWnd_W;
 m_optTpszonMapWndSize1.value.y = i_wndSize.m_TpszonMapWnd_H;
 m_optCylMultMapWndSize1.value.x = i_wndSize.m_CylMultMapWnd_W;
 m_optCylMultMapWndSize1.value.y = i_wndSize.m_CylMultMapWnd_H;
 m_optCylAddMapWndSize1.value.x = i_wndSize.m_CylAddMapWnd_W;
 m_optCylAddMapWndSize1.value.y = i_wndSize.m_CylAddMapWnd_H;
 //group of separate maps
 m_optAttenuatorMapWndSize1.value.x = i_wndSize.m_AttenuatorMapWnd_W;
 m_optAttenuatorMapWndSize1.value.y = i_wndSize.m_AttenuatorMapWnd_H;
 m_optDwellCntrlMapWndSize1.value.x = i_wndSize.m_DwellCntrlMapWnd_W;
 m_optDwellCntrlMapWndSize1.value.y = i_wndSize.m_DwellCntrlMapWnd_H;
 m_optCTSCurveMapWndSize1.value.x = i_wndSize.m_CTSCurveMapWnd_W;
 m_optCTSCurveMapWndSize1.value.y = i_wndSize.m_CTSCurveMapWnd_H;
 m_optATSCurvMapWndSize1.value.x = i_wndSize.m_ATSCurvMapWnd_W;
 m_optATSCurvMapWndSize1.value.y = i_wndSize.m_ATSCurvMapWnd_H;
 m_optATSCorrMapWndSize1.value.x = i_wndSize.m_ATSCorrMapWnd_W;
 m_optATSCorrMapWndSize1.value.y = i_wndSize.m_ATSCorrMapWnd_H; 
 m_optGasdoseMapWndSize1.value.x = i_wndSize.m_GasdoseMapWnd_W;
 m_optGasdoseMapWndSize1.value.y = i_wndSize.m_GasdoseMapWnd_H; 
 m_optBarocorrMapWndSize1.value.x = i_wndSize.m_BarocorrMapWnd_W;
 m_optBarocorrMapWndSize1.value.y = i_wndSize.m_BarocorrMapWnd_H; 
 m_optManIgntimMapWndSize1.value.x = i_wndSize.m_ManIgntimMapWnd_W;
 m_optManIgntimMapWndSize1.value.y = i_wndSize.m_ManIgntimMapWnd_H; 
 m_optCESettingsWndSize1.value.x = i_wndSize.m_CESettingsWnd_W;
 m_optCESettingsWndSize1.value.y = i_wndSize.m_CESettingsWnd_H; 
 m_optTmp2CurveMapWndSize1.value.x = i_wndSize.m_Tmp2CurveMapWnd_W;
 m_optTmp2CurveMapWndSize1.value.y = i_wndSize.m_Tmp2CurveMapWnd_H; 
 m_optCrkTempMapWndSize1.value.x = i_wndSize.m_CrkTempMapWnd_W;
 m_optCrkTempMapWndSize1.value.y = i_wndSize.m_CrkTempMapWnd_H;
 m_optEHPauseMapWndSize1.value.x = i_wndSize.m_EHPauseMapWnd_W;
 m_optEHPauseMapWndSize1.value.y = i_wndSize.m_EHPauseMapWnd_H;
 m_optCrankingThrdMapWndSize1.value.x = i_wndSize.m_CrankingThrdMapWnd_W;
 m_optCrankingThrdMapWndSize1.value.y = i_wndSize.m_CrankingThrdMapWnd_H;
 m_optCrankingTimeMapWndSize1.value.x = i_wndSize.m_CrankingTimeMapWnd_W;
 m_optCrankingTimeMapWndSize1.value.y = i_wndSize.m_CrankingTimeMapWnd_H;
 m_optSmapabanThrdMapWndSize1.value.x = i_wndSize.m_SmapabanThrdMapWnd_W;
 m_optSmapabanThrdMapWndSize1.value.y = i_wndSize.m_SmapabanThrdMapWnd_H;
 m_optKnockZoneMapWndSize1.value.x = i_wndSize.m_KnockZoneMapWnd_W;
 m_optKnockZoneMapWndSize1.value.y = i_wndSize.m_KnockZoneMapWnd_H; 
 m_optGrtsCurveMapWndSize1.value.x = i_wndSize.m_GrtsCurveMapWnd_W;
 m_optGrtsCurveMapWndSize1.value.y = i_wndSize.m_GrtsCurveMapWnd_H; 
 m_optGrHeatDutyMapWndSize1.value.x = i_wndSize.m_GrHeatDutyMapWnd_W;
 m_optGrHeatDutyMapWndSize1.value.y = i_wndSize.m_GrHeatDutyMapWnd_H;
 m_optPwmIacUCoefMapWndSize1.value.x = i_wndSize.m_PwmIacUCoefMapWnd_W;
 m_optPwmIacUCoefMapWndSize1.value.y = i_wndSize.m_PwmIacUCoefMapWnd_H;
 m_optAftstrStrk0MapWndSize1.value.x = i_wndSize.m_AftstrStrk0MapWnd_W;
 m_optAftstrStrk0MapWndSize1.value.y = i_wndSize.m_AftstrStrk0MapWnd_H;
 m_optAftstrStrk1MapWndSize1.value.x = i_wndSize.m_AftstrStrk1MapWnd_W;
 m_optAftstrStrk1MapWndSize1.value.y = i_wndSize.m_AftstrStrk1MapWnd_H;
 m_optGrValDelMapWndSize1.value.x = i_wndSize.m_GrValDelMapWnd_W;
 m_optGrValDelMapWndSize1.value.y = i_wndSize.m_GrValDelMapWnd_H;
 m_optFtlsCurveMapWndSize1.value.x = i_wndSize.m_FtlsCurveMapWnd_W;
 m_optFtlsCurveMapWndSize1.value.y = i_wndSize.m_FtlsCurveMapWnd_H; 
 m_optEgtsCurveMapWndSize1.value.x = i_wndSize.m_EgtsCurveMapWnd_W;
 m_optEgtsCurveMapWndSize1.value.y = i_wndSize.m_EgtsCurveMapWnd_H; 
 m_optOpsCurveMapWndSize1.value.x = i_wndSize.m_OpsCurveMapWnd_W;
 m_optOpsCurveMapWndSize1.value.y = i_wndSize.m_OpsCurveMapWnd_H; 
 m_optManInjPwcMapWndSize1.value.x = i_wndSize.m_ManInjPwcMapWnd_W;
 m_optManInjPwcMapWndSize1.value.y = i_wndSize.m_ManInjPwcMapWnd_H; 
 m_optMAFCurveMapWndSize1.value.x = i_wndSize.m_MAFCurveMapWnd_W;
 m_optMAFCurveMapWndSize1.value.y = i_wndSize.m_MAFCurveMapWnd_H; 
 m_optFtlsCorMapWndSize1.value.x = i_wndSize.m_FtlsCorMapWnd_W;
 m_optFtlsCorMapWndSize1.value.y = i_wndSize.m_FtlsCorMapWnd_H; 
 m_optLambdaZoneMapWndSize1.value.x = i_wndSize.m_LambdaZoneMapWnd_W;
 m_optLambdaZoneMapWndSize1.value.y = i_wndSize.m_LambdaZoneMapWnd_H; 
 m_optFtsCurveMapWndSize1.value.x = i_wndSize.m_FtsCurveMapWnd_W;
 m_optFtsCurveMapWndSize1.value.y = i_wndSize.m_FtsCurveMapWnd_H; 
 m_optFuelDensCorrMapWndSize1.value.x = i_wndSize.m_FuelDensCorrMapWnd_W;
 m_optFuelDensCorrMapWndSize1.value.y = i_wndSize.m_FuelDensCorrMapWnd_H; 
 m_optXtauXfAccMapWndSize1.value.x = i_wndSize.m_XtauXfAccMapWnd_W;
 m_optXtauXfAccMapWndSize1.value.y = i_wndSize.m_XtauXfAccMapWnd_H; 
 m_optXtauXfDecMapWndSize1.value.x = i_wndSize.m_XtauXfDecMapWnd_W;
 m_optXtauXfDecMapWndSize1.value.y = i_wndSize.m_XtauXfDecMapWnd_H; 
 m_optXtauTfAccMapWndSize1.value.x = i_wndSize.m_XtauTfAccMapWnd_W;
 m_optXtauTfAccMapWndSize1.value.y = i_wndSize.m_XtauTfAccMapWnd_H; 
 m_optXtauTfDecMapWndSize1.value.x = i_wndSize.m_XtauTfDecMapWnd_W;
 m_optXtauTfDecMapWndSize1.value.y = i_wndSize.m_XtauTfDecMapWnd_H; 
 m_optInjNonLinPMapWndSize1.value.x = i_wndSize.m_InjNonLinPMapWnd_W;
 m_optInjNonLinPMapWndSize1.value.y = i_wndSize.m_InjNonLinPMapWnd_H; 
 m_optInjNonLinGMapWndSize1.value.x = i_wndSize.m_InjNonLinGMapWnd_W;
 m_optInjNonLinGMapWndSize1.value.y = i_wndSize.m_InjNonLinGMapWnd_H;
 m_optEGODelayMapWndSize1.value.x = i_wndSize.m_EGODelayMapWnd_W;
 m_optEGODelayMapWndSize1.value.y = i_wndSize.m_EGODelayMapWnd_H;
 m_optWUAFR0MapWndSize1.value.x = i_wndSize.m_WUAFR0MapWnd_W;
 m_optWUAFR0MapWndSize1.value.y = i_wndSize.m_WUAFR0MapWnd_H;
 m_optWUAFR1MapWndSize1.value.x = i_wndSize.m_WUAFR1MapWnd_W;
 m_optWUAFR1MapWndSize1.value.y = i_wndSize.m_WUAFR1MapWnd_H;
}

void CAppSettingsModel::GetWndSize1(WndSize& o_wndSize) const
{
 o_wndSize.m_StrtMapWnd_W = m_optStrtMapWndSize1.value.x;
 o_wndSize.m_StrtMapWnd_H = m_optStrtMapWndSize1.value.y;
 o_wndSize.m_IdleMapWnd_W = m_optIdleMapWndSize1.value.x;
 o_wndSize.m_IdleMapWnd_H = m_optIdleMapWndSize1.value.y;
 o_wndSize.m_WorkMapWnd_W = m_optWorkMapWndSize1.value.x;
 o_wndSize.m_WorkMapWnd_H = m_optWorkMapWndSize1.value.y;
 o_wndSize.m_TempMapWnd_W = m_optTempMapWndSize1.value.x;
 o_wndSize.m_TempMapWnd_H = m_optTempMapWndSize1.value.y;
 o_wndSize.m_TempIdlMapWnd_W = m_optTempIdlMapWndSize1.value.x;
 o_wndSize.m_TempIdlMapWnd_H = m_optTempIdlMapWndSize1.value.y;
 o_wndSize.m_GridMapIgnWnd_W = m_optGridMapIgnWndSize1.value.x;
 o_wndSize.m_GridMapIgnWnd_H = m_optGridMapIgnWndSize1.value.y;
 o_wndSize.m_GridMapInjWnd_W = m_optGridMapInjWndSize1.value.x;
 o_wndSize.m_GridMapInjWnd_H = m_optGridMapInjWndSize1.value.y;
 o_wndSize.m_VEMapWnd_W = m_optVEMapWndSize1.value.x;
 o_wndSize.m_VEMapWnd_H = m_optVEMapWndSize1.value.y;
 o_wndSize.m_VE2MapWnd_W = m_optVE2MapWndSize1.value.x;
 o_wndSize.m_VE2MapWnd_H = m_optVE2MapWndSize1.value.y;
 o_wndSize.m_VEIMapWnd_W = m_optVEIMapWndSize1.value.x;
 o_wndSize.m_VEIMapWnd_H = m_optVEIMapWndSize1.value.y;
 o_wndSize.m_AFRMapWnd_W = m_optAFRMapWndSize1.value.x;
 o_wndSize.m_AFRMapWnd_H = m_optAFRMapWndSize1.value.y;
 o_wndSize.m_CrnkMapWnd_W = m_optCrnkMapWndSize1.value.x;
 o_wndSize.m_CrnkMapWnd_H = m_optCrnkMapWndSize1.value.y;
 o_wndSize.m_WrmpMapWnd_W = m_optWrmpMapWndSize1.value.x;
 o_wndSize.m_WrmpMapWnd_H = m_optWrmpMapWndSize1.value.y;
 o_wndSize.m_DeadMapWnd_W = m_optDeadMapWndSize1.value.x;
 o_wndSize.m_DeadMapWnd_H = m_optDeadMapWndSize1.value.y;
 o_wndSize.m_IdlrMapWnd_W = m_optIdlrMapWndSize1.value.x;
 o_wndSize.m_IdlrMapWnd_H = m_optIdlrMapWndSize1.value.y;
 o_wndSize.m_IdlcMapWnd_W = m_optIdlcMapWndSize1.value.x;
 o_wndSize.m_IdlcMapWnd_H = m_optIdlcMapWndSize1.value.y;
 o_wndSize.m_ThrassMapWnd_W = m_optThrassMapWndSize1.value.x;
 o_wndSize.m_ThrassMapWnd_H = m_optThrassMapWndSize1.value.y;
 o_wndSize.m_AETPSMapWnd_W = m_optAETPSMapWndSize1.value.x;
 o_wndSize.m_AETPSMapWnd_H = m_optAETPSMapWndSize1.value.y;
 o_wndSize.m_AEMAPMapWnd_W = m_optAEMAPMapWndSize1.value.x;
 o_wndSize.m_AEMAPMapWnd_H = m_optAEMAPMapWndSize1.value.y;
 o_wndSize.m_AERPMMapWnd_W = m_optAERPMMapWndSize1.value.x;
 o_wndSize.m_AERPMMapWnd_H = m_optAERPMMapWndSize1.value.y;
 o_wndSize.m_AftstrMapWnd_W = m_optAftstrMapWndSize1.value.x;
 o_wndSize.m_AftstrMapWnd_H = m_optAftstrMapWndSize1.value.y;
 o_wndSize.m_ITMapWnd_W = m_optITMapWndSize1.value.x;
 o_wndSize.m_ITMapWnd_H = m_optITMapWndSize1.value.y;
 o_wndSize.m_ITRPMMapWnd_W = m_optITRPMMapWndSize1.value.x;
 o_wndSize.m_ITRPMMapWnd_H = m_optITRPMMapWndSize1.value.y;
 o_wndSize.m_RigidMapWnd_W = m_optRigidMapWndSize1.value.x;
 o_wndSize.m_RigidMapWnd_H = m_optRigidMapWndSize1.value.y;
 o_wndSize.m_EGOCrvMapWnd_W = m_optEGOCrvMapWndSize1.value.x;
 o_wndSize.m_EGOCrvMapWnd_H = m_optEGOCrvMapWndSize1.value.y;
 o_wndSize.m_IACCMapWnd_W = m_optIACCMapWndSize1.value.x;
 o_wndSize.m_IACCMapWnd_H = m_optIACCMapWndSize1.value.y;
 o_wndSize.m_IACCWMapWnd_W = m_optIACCWMapWndSize1.value.x;
 o_wndSize.m_IACCWMapWnd_H = m_optIACCWMapWndSize1.value.y;
 o_wndSize.m_IATCLTMapWnd_W = m_optIATCLTMapWndSize1.value.x;
 o_wndSize.m_IATCLTMapWnd_H = m_optIATCLTMapWndSize1.value.y;
 o_wndSize.m_TpsswtMapWnd_W = m_optTpsswtMapWndSize1.value.x;
 o_wndSize.m_TpsswtMapWnd_H = m_optTpsswtMapWndSize1.value.y;
 o_wndSize.m_GtscMapWnd_W = m_optGtscMapWndSize1.value.x;
 o_wndSize.m_GtscMapWnd_H = m_optGtscMapWndSize1.value.y;
 o_wndSize.m_GpscMapWnd_W = m_optGpscMapWndSize1.value.x;
 o_wndSize.m_GpscMapWnd_H = m_optGpscMapWndSize1.value.y;
 o_wndSize.m_AtscMapWnd_W = m_optAtscMapWndSize1.value.x;
 o_wndSize.m_AtscMapWnd_H = m_optAtscMapWndSize1.value.y;
 o_wndSize.m_Pwm1MapWnd_W = m_optPwm1MapWndSize1.value.x;
 o_wndSize.m_Pwm1MapWnd_H = m_optPwm1MapWndSize1.value.y;
 o_wndSize.m_Pwm2MapWnd_W = m_optPwm2MapWndSize1.value.x;
 o_wndSize.m_Pwm2MapWnd_H = m_optPwm2MapWndSize1.value.y;
 o_wndSize.m_IACMATMapWnd_W = m_optIACMATMapWndSize1.value.x;
 o_wndSize.m_IACMATMapWnd_H = m_optIACMATMapWndSize1.value.y;
 o_wndSize.m_TpszonMapWnd_W = m_optTpszonMapWndSize1.value.x;
 o_wndSize.m_TpszonMapWnd_H = m_optTpszonMapWndSize1.value.y;
 o_wndSize.m_CylMultMapWnd_W = m_optCylMultMapWndSize1.value.x;
 o_wndSize.m_CylMultMapWnd_H = m_optCylMultMapWndSize1.value.y;
 o_wndSize.m_CylAddMapWnd_W = m_optCylAddMapWndSize1.value.x;
 o_wndSize.m_CylAddMapWnd_H = m_optCylAddMapWndSize1.value.y;
 //group of separate maps
 o_wndSize.m_AttenuatorMapWnd_W = m_optAttenuatorMapWndSize1.value.x;
 o_wndSize.m_AttenuatorMapWnd_H = m_optAttenuatorMapWndSize1.value.y;
 o_wndSize.m_DwellCntrlMapWnd_W = m_optDwellCntrlMapWndSize1.value.x;
 o_wndSize.m_DwellCntrlMapWnd_H = m_optDwellCntrlMapWndSize1.value.y;
 o_wndSize.m_CTSCurveMapWnd_W = m_optCTSCurveMapWndSize1.value.x;
 o_wndSize.m_CTSCurveMapWnd_H = m_optCTSCurveMapWndSize1.value.y;
 o_wndSize.m_ATSCurvMapWnd_W = m_optATSCurvMapWndSize1.value.x;
 o_wndSize.m_ATSCurvMapWnd_H = m_optATSCurvMapWndSize1.value.y;
 o_wndSize.m_ATSCorrMapWnd_W = m_optATSCorrMapWndSize1.value.x;
 o_wndSize.m_ATSCorrMapWnd_H = m_optATSCorrMapWndSize1.value.y;
 o_wndSize.m_GasdoseMapWnd_W = m_optGasdoseMapWndSize1.value.x;
 o_wndSize.m_GasdoseMapWnd_H = m_optGasdoseMapWndSize1.value.y;
 o_wndSize.m_BarocorrMapWnd_W = m_optBarocorrMapWndSize1.value.x;
 o_wndSize.m_BarocorrMapWnd_H = m_optBarocorrMapWndSize1.value.y;
 o_wndSize.m_ManIgntimMapWnd_W = m_optManIgntimMapWndSize1.value.x;
 o_wndSize.m_ManIgntimMapWnd_H = m_optManIgntimMapWndSize1.value.y;
 o_wndSize.m_CESettingsWnd_W = m_optCESettingsWndSize1.value.x;
 o_wndSize.m_CESettingsWnd_H = m_optCESettingsWndSize1.value.y;
 o_wndSize.m_Tmp2CurveMapWnd_W = m_optTmp2CurveMapWndSize1.value.x;
 o_wndSize.m_Tmp2CurveMapWnd_H = m_optTmp2CurveMapWndSize1.value.y;
 o_wndSize.m_CrkTempMapWnd_W = m_optCrkTempMapWndSize1.value.x;
 o_wndSize.m_CrkTempMapWnd_H = m_optCrkTempMapWndSize1.value.y;
 o_wndSize.m_EHPauseMapWnd_W = m_optEHPauseMapWndSize1.value.x;
 o_wndSize.m_EHPauseMapWnd_H = m_optEHPauseMapWndSize1.value.y;
 o_wndSize.m_CrankingThrdMapWnd_W = m_optCrankingThrdMapWndSize1.value.x;
 o_wndSize.m_CrankingThrdMapWnd_H = m_optCrankingThrdMapWndSize1.value.y;
 o_wndSize.m_CrankingTimeMapWnd_W = m_optCrankingTimeMapWndSize1.value.x;
 o_wndSize.m_CrankingTimeMapWnd_H = m_optCrankingTimeMapWndSize1.value.y;
 o_wndSize.m_SmapabanThrdMapWnd_W = m_optSmapabanThrdMapWndSize1.value.x;
 o_wndSize.m_SmapabanThrdMapWnd_H = m_optSmapabanThrdMapWndSize1.value.y;
 o_wndSize.m_KnockZoneMapWnd_W = m_optKnockZoneMapWndSize1.value.x;
 o_wndSize.m_KnockZoneMapWnd_H = m_optKnockZoneMapWndSize1.value.y;
 o_wndSize.m_GrtsCurveMapWnd_W = m_optGrtsCurveMapWndSize1.value.x;
 o_wndSize.m_GrtsCurveMapWnd_H = m_optGrtsCurveMapWndSize1.value.y;
 o_wndSize.m_GrHeatDutyMapWnd_W = m_optGrHeatDutyMapWndSize1.value.x;
 o_wndSize.m_GrHeatDutyMapWnd_H = m_optGrHeatDutyMapWndSize1.value.y;
 o_wndSize.m_PwmIacUCoefMapWnd_W = m_optPwmIacUCoefMapWndSize1.value.x;
 o_wndSize.m_PwmIacUCoefMapWnd_H = m_optPwmIacUCoefMapWndSize1.value.y;
 o_wndSize.m_AftstrStrk0MapWnd_W = m_optAftstrStrk0MapWndSize1.value.x;
 o_wndSize.m_AftstrStrk0MapWnd_H = m_optAftstrStrk0MapWndSize1.value.y;
 o_wndSize.m_AftstrStrk1MapWnd_W = m_optAftstrStrk1MapWndSize1.value.x;
 o_wndSize.m_AftstrStrk1MapWnd_H = m_optAftstrStrk1MapWndSize1.value.y;
 o_wndSize.m_GrValDelMapWnd_W = m_optGrValDelMapWndSize1.value.x;
 o_wndSize.m_GrValDelMapWnd_H = m_optGrValDelMapWndSize1.value.y;
 o_wndSize.m_FtlsCurveMapWnd_W = m_optFtlsCurveMapWndSize1.value.x;
 o_wndSize.m_FtlsCurveMapWnd_H = m_optFtlsCurveMapWndSize1.value.y;
 o_wndSize.m_EgtsCurveMapWnd_W = m_optEgtsCurveMapWndSize1.value.x;
 o_wndSize.m_EgtsCurveMapWnd_H = m_optEgtsCurveMapWndSize1.value.y;
 o_wndSize.m_OpsCurveMapWnd_W = m_optOpsCurveMapWndSize1.value.x;
 o_wndSize.m_OpsCurveMapWnd_H = m_optOpsCurveMapWndSize1.value.y;
 o_wndSize.m_ManInjPwcMapWnd_W = m_optManInjPwcMapWndSize1.value.x;
 o_wndSize.m_ManInjPwcMapWnd_H = m_optManInjPwcMapWndSize1.value.y;
 o_wndSize.m_MAFCurveMapWnd_W = m_optMAFCurveMapWndSize1.value.x;
 o_wndSize.m_MAFCurveMapWnd_H = m_optMAFCurveMapWndSize1.value.y;
 o_wndSize.m_FtlsCorMapWnd_W = m_optFtlsCorMapWndSize1.value.x;
 o_wndSize.m_FtlsCorMapWnd_H = m_optFtlsCorMapWndSize1.value.y;
 o_wndSize.m_LambdaZoneMapWnd_W = m_optLambdaZoneMapWndSize1.value.x;
 o_wndSize.m_LambdaZoneMapWnd_H = m_optLambdaZoneMapWndSize1.value.y;
 o_wndSize.m_FtsCurveMapWnd_W = m_optFtsCurveMapWndSize1.value.x;
 o_wndSize.m_FtsCurveMapWnd_H = m_optFtsCurveMapWndSize1.value.y;
 o_wndSize.m_FuelDensCorrMapWnd_W = m_optFuelDensCorrMapWndSize1.value.x;
 o_wndSize.m_FuelDensCorrMapWnd_H = m_optFuelDensCorrMapWndSize1.value.y;
 o_wndSize.m_XtauXfAccMapWnd_W = m_optXtauXfAccMapWndSize1.value.x;
 o_wndSize.m_XtauXfAccMapWnd_H = m_optXtauXfAccMapWndSize1.value.y;
 o_wndSize.m_XtauXfDecMapWnd_W = m_optXtauXfDecMapWndSize1.value.x;
 o_wndSize.m_XtauXfDecMapWnd_H = m_optXtauXfDecMapWndSize1.value.y;
 o_wndSize.m_XtauTfAccMapWnd_W = m_optXtauTfAccMapWndSize1.value.x;
 o_wndSize.m_XtauTfAccMapWnd_H = m_optXtauTfAccMapWndSize1.value.y;
 o_wndSize.m_XtauTfDecMapWnd_W = m_optXtauTfDecMapWndSize1.value.x;
 o_wndSize.m_XtauTfDecMapWnd_H = m_optXtauTfDecMapWndSize1.value.y;
 o_wndSize.m_InjNonLinPMapWnd_W = m_optInjNonLinPMapWndSize1.value.x;
 o_wndSize.m_InjNonLinPMapWnd_H = m_optInjNonLinPMapWndSize1.value.y;
 o_wndSize.m_InjNonLinGMapWnd_W = m_optInjNonLinGMapWndSize1.value.x;
 o_wndSize.m_InjNonLinGMapWnd_H = m_optInjNonLinGMapWndSize1.value.y;
 o_wndSize.m_EGODelayMapWnd_W = m_optEGODelayMapWndSize1.value.x;
 o_wndSize.m_EGODelayMapWnd_H = m_optEGODelayMapWndSize1.value.y;
 o_wndSize.m_WUAFR0MapWnd_W = m_optWUAFR0MapWndSize1.value.x;
 o_wndSize.m_WUAFR0MapWnd_H = m_optWUAFR0MapWndSize1.value.y;
 o_wndSize.m_WUAFR1MapWnd_W = m_optWUAFR1MapWndSize1.value.x;
 o_wndSize.m_WUAFR1MapWnd_H = m_optWUAFR1MapWndSize1.value.y;
}

void CAppSettingsModel::SetWndState(const WndState& i_wndState)
{
 m_optMainFrmWndState.value = i_wndState.m_MainFrmWnd;
}

void CAppSettingsModel::GetWndState(WndState& o_wndState) const
{
 o_wndState.m_MainFrmWnd = m_optMainFrmWndState.value;
}

EInterLang CAppSettingsModel::GetInterfaceLanguage(void) const
{
 return (EInterLang)m_optInterfaceLang.value;
}

EECUPlatform CAppSettingsModel::GetECUPlatformType(void) const
{
 return (EECUPlatform)m_optECUPlatformType.value;
}

ESpeedUnit CAppSettingsModel::GetSpeedUnit(void) const
{
 return (ESpeedUnit)m_optSpeedUnit.value;
}

bool CAppSettingsModel::GetUseDVFeatures(void) const
{
 return m_optUseDVFeatures.value;
}

int CAppSettingsModel::GetDVDeskUpdatePeriod(void) const
{
 return m_optDVDeskUpdatePeriod.value;
}

bool CAppSettingsModel::GetShowToolTips(void) const
{
 return m_optShowToolTips.value;
}

bool CAppSettingsModel::GetShowExFixtures(void) const
{
 return m_optShowExFixtures.value;
}

bool CAppSettingsModel::GetShowSpeedAndDist(void) const
{
 return m_optShowSpeedAndDist.value;
}

bool CAppSettingsModel::GetCOMPortBother(void) const
{
 return m_optCOMPortBother.value;
}

bool CAppSettingsModel::GetUseHotKeys(void) const
{
 return m_optUseHotKeys.value;
}

bool CAppSettingsModel::GetShowWelcome(void) const
{
 return m_optShowWelcome.value;
}

bool CAppSettingsModel::GetAllowVisualTheme(void) const
{
 return m_optAllowVisualTheme.value;
}

bool CAppSettingsModel::GetAutoDiagEnter(void) const
{
 return m_optAutoDiagEnter.value;
}

bool CAppSettingsModel::GetSaveWarning(void) const
{
 return m_optSaveWarning.value;
}

bool CAppSettingsModel::GetAutoCERead(void) const
{
 return m_optAutoCERead.value;
}

bool CAppSettingsModel::GetChildCharts(void) const
{
 return m_optChildCharts.value;
}

void CAppSettingsModel::SetChildCharts(bool child)
{
 m_optChildCharts.value = child;
}

void CAppSettingsModel::GetIndicatorsConfig(IndicatorsCfg& o_cfg) const
{
 for(int i = 0; i < 2; ++i)
 {
  o_cfg.m_optIndHeightPercent[i] = m_optIndHeightPercent[i].value;
  o_cfg.m_optIndRows[i] = m_optIndRows[i].value;
  o_cfg.m_optIndGas_v[i] = std::make_pair(m_optIndGas_v[i].value, m_optColGas_v.value);
  o_cfg.m_optIndCarb[i] = std::make_pair(m_optIndCarb[i].value, m_optColCarb.value);
  o_cfg.m_optIndIdleValve[i] = std::make_pair(m_optIndIdleValve[i].value, m_optColIdleValve.value);
  o_cfg.m_optIndPowerValve[i] = std::make_pair(m_optIndPowerValve[i].value, m_optColPowerValve.value);
  o_cfg.m_optIndStBlock[i] = std::make_pair(m_optIndStBlock[i].value, m_optColStBlock.value);
  o_cfg.m_optIndAE[i] = std::make_pair(m_optIndAE[i].value, m_optColAE.value);
  o_cfg.m_optIndCoolingFan[i] = std::make_pair(m_optIndCoolingFan[i].value, m_optColCoolingFan.value);
  o_cfg.m_optIndCE[i] = std::make_pair(m_optIndCE[i].value, m_optColCE.value);
  o_cfg.m_optIndFCRevLim[i] = std::make_pair(m_optIndFCRevLim[i].value, m_optColFCRevLim.value);
  o_cfg.m_optIndFloodClear[i] = std::make_pair(m_optIndFloodClear[i].value, m_optColFloodClear.value);
  o_cfg.m_optIndSysLocked[i] = std::make_pair(m_optIndSysLocked[i].value, m_optColSysLocked.value);
  o_cfg.m_optIndIgn_i[i] = std::make_pair(m_optIndIgn_i[i].value, m_optColIgn_i.value);
  o_cfg.m_optIndCond_i[i] = std::make_pair(m_optIndCond_i[i].value, m_optColCond_i.value);
  o_cfg.m_optIndEpas_i[i] = std::make_pair(m_optIndEpas_i[i].value, m_optColEpas_i.value);
  o_cfg.m_optIndAftStrEnr[i] = std::make_pair(m_optIndAftStrEnr[i].value, m_optColAftStrEnr.value);
  o_cfg.m_optIndIacClLoop[i] = std::make_pair(m_optIndIacClLoop[i].value, m_optColIacClLoop.value);
  o_cfg.m_optIndUniOut1[i] = std::make_pair(m_optIndUniOut1[i].value, m_optColUniOut1.value);
  o_cfg.m_optIndUniOut2[i] = std::make_pair(m_optIndUniOut2[i].value, m_optColUniOut2.value);
  o_cfg.m_optIndUniOut3[i] = std::make_pair(m_optIndUniOut3[i].value, m_optColUniOut3.value);
  o_cfg.m_optIndUniOut4[i] = std::make_pair(m_optIndUniOut4[i].value, m_optColUniOut4.value);
  o_cfg.m_optIndUniOut5[i] = std::make_pair(m_optIndUniOut5[i].value, m_optColUniOut5.value);
  o_cfg.m_optIndUniOut6[i] = std::make_pair(m_optIndUniOut6[i].value, m_optColUniOut6.value);
  o_cfg.m_optIndGpa4_i[i] = std::make_pair(m_optIndGpa4_i[i].value, m_optColGpa4_i.value);
  o_cfg.m_optIndInput1[i] = std::make_pair(m_optIndInput1[i].value, m_optColInput1.value);
  o_cfg.m_optIndInput2[i] = std::make_pair(m_optIndInput2[i].value, m_optColInput2.value);
  o_cfg.m_optIndAuto_i[i] = std::make_pair(m_optIndAuto_i[i].value, m_optColAuto_i.value);
  o_cfg.m_optIndMapsel0[i] = std::make_pair(m_optIndMapsel0[i].value, m_optColMapsel0.value);
  o_cfg.m_optIndRefprs_i[i] = std::make_pair(m_optIndRefprs_i[i].value, m_optColRefprs_i.value);
  o_cfg.m_optIndAltrn_i[i] = std::make_pair(m_optIndAltrn_i[i].value, m_optColAltrn_i.value);
 }
}

void CAppSettingsModel::SetIndicatorsConfig(const IndicatorsCfg& i_cfg)
{
 for(int i = 0; i < 2; ++i)
 {
  m_optIndHeightPercent[i].value = i_cfg.m_optIndHeightPercent[i];
  m_optIndRows[i].value = i_cfg.m_optIndRows[i];
  m_optIndGas_v[i].value = i_cfg.m_optIndGas_v[i].first, m_optColGas_v.value = i_cfg.m_optIndGas_v[i].second;
  m_optIndCarb[i].value = i_cfg.m_optIndCarb[i].first, m_optColCarb.value = i_cfg.m_optIndCarb[i].second;
  m_optIndIdleValve[i].value = i_cfg.m_optIndIdleValve[i].first, m_optColIdleValve.value = i_cfg.m_optIndIdleValve[i].second;
  m_optIndPowerValve[i].value = i_cfg.m_optIndPowerValve[i].first, m_optColPowerValve.value = i_cfg.m_optIndPowerValve[i].second;
  m_optIndStBlock[i].value = i_cfg.m_optIndStBlock[i].first, m_optColStBlock.value = i_cfg.m_optIndStBlock[i].second;
  m_optIndAE[i].value = i_cfg.m_optIndAE[i].first, m_optColAE.value = i_cfg.m_optIndAE[i].second;
  m_optIndCoolingFan[i].value = i_cfg.m_optIndCoolingFan[i].first, m_optColCoolingFan.value = i_cfg.m_optIndCoolingFan[i].second;
  m_optIndCE[i].value = i_cfg.m_optIndCE[i].first, m_optColCE.value = i_cfg.m_optIndCE[i].second;
  m_optIndFCRevLim[i].value = i_cfg.m_optIndFCRevLim[i].first, m_optColFCRevLim.value = i_cfg.m_optIndFCRevLim[i].second;
  m_optIndFloodClear[i].value = i_cfg.m_optIndFloodClear[i].first, m_optColFloodClear.value = i_cfg.m_optIndFloodClear[i].second;
  m_optIndSysLocked[i].value = i_cfg.m_optIndSysLocked[i].first, m_optColSysLocked.value = i_cfg.m_optIndSysLocked[i].second;
  m_optIndIgn_i[i].value = i_cfg.m_optIndIgn_i[i].first, m_optColIgn_i.value = i_cfg.m_optIndIgn_i[i].second;
  m_optIndCond_i[i].value = i_cfg.m_optIndCond_i[i].first, m_optColCond_i.value = i_cfg.m_optIndCond_i[i].second;
  m_optIndEpas_i[i].value = i_cfg.m_optIndEpas_i[i].first, m_optColEpas_i.value = i_cfg.m_optIndEpas_i[i].second;
  m_optIndAftStrEnr[i].value = i_cfg.m_optIndAftStrEnr[i].first, m_optColAftStrEnr.value = i_cfg.m_optIndAftStrEnr[i].second;
  m_optIndIacClLoop[i].value = i_cfg.m_optIndIacClLoop[i].first, m_optColIacClLoop.value = i_cfg.m_optIndIacClLoop[i].second;
  m_optIndUniOut1[i].value = i_cfg.m_optIndUniOut1[i].first, m_optColUniOut1.value = i_cfg.m_optIndUniOut1[i].second;
  m_optIndUniOut2[i].value = i_cfg.m_optIndUniOut2[i].first, m_optColUniOut2.value = i_cfg.m_optIndUniOut2[i].second;
  m_optIndUniOut3[i].value = i_cfg.m_optIndUniOut3[i].first, m_optColUniOut3.value = i_cfg.m_optIndUniOut3[i].second;
  m_optIndUniOut4[i].value = i_cfg.m_optIndUniOut4[i].first, m_optColUniOut4.value = i_cfg.m_optIndUniOut4[i].second;
  m_optIndUniOut5[i].value = i_cfg.m_optIndUniOut5[i].first, m_optColUniOut5.value = i_cfg.m_optIndUniOut5[i].second;
  m_optIndUniOut6[i].value = i_cfg.m_optIndUniOut6[i].first, m_optColUniOut6.value = i_cfg.m_optIndUniOut6[i].second;
  m_optIndGpa4_i[i].value = i_cfg.m_optIndGpa4_i[i].first, m_optColGpa4_i.value = i_cfg.m_optIndGpa4_i[i].second;
  m_optIndInput1[i].value = i_cfg.m_optIndInput1[i].first, m_optColInput1.value = i_cfg.m_optIndInput1[i].second;
  m_optIndInput2[i].value = i_cfg.m_optIndInput2[i].first, m_optColInput2.value = i_cfg.m_optIndInput2[i].second;
  m_optIndAuto_i[i].value = i_cfg.m_optIndAuto_i[i].first, m_optColAuto_i.value = i_cfg.m_optIndAuto_i[i].second;
  m_optIndMapsel0[i].value = i_cfg.m_optIndMapsel0[i].first, m_optColMapsel0.value = i_cfg.m_optIndMapsel0[i].second;
  m_optIndRefprs_i[i].value = i_cfg.m_optIndRefprs_i[i].first, m_optColRefprs_i.value = i_cfg.m_optIndRefprs_i[i].second;
  m_optIndAltrn_i[i].value = i_cfg.m_optIndAltrn_i[i].first, m_optColAltrn_i.value = i_cfg.m_optIndAltrn_i[i].second;
 }
}

static void _cpyMetersConfig(MetCfg* opt, const OptField_t<int>* mbr, const OptField_t<SclCfg>* scale)
{
 for(int g = 0; g < 2; ++g)
 {
  opt[g].position = mbr[g].value;
  opt[g].ticksNum = scale[g].value.ticksNum;
  opt[g].scaleMin = scale[g].value.scaleMin;
  opt[g].scaleMax = scale[g].value.scaleMax;
  opt[g].alezn = scale[g].value.alezn;
 }
}
static void _cpyMetersConfig(const MetCfg* opt, OptField_t<int>* mbr, OptField_t<SclCfg>* scale)
{
 for(int g = 0; g < 2; ++g)
 {
  if (mbr)
   mbr[g].value = opt[g].position;
  scale[g].value.ticksNum = opt[g].ticksNum;
  scale[g].value.scaleMin = opt[g].scaleMin;
  scale[g].value.scaleMax = opt[g].scaleMax;
  scale[g].value.alezn = opt[g].alezn;
 }
}

void CAppSettingsModel::GetMetersConfig(MetersCfg* o_cfg) const
{
 for(int i = 0; i < 2; ++i)
 {
  o_cfg[i].m_optMetRows = m_optMetRows[i].value;
  _cpyMetersConfig(o_cfg[i].m_optMetRPM, &m_optMetRPM[i][0], &m_optMetScRPM[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetMAP, &m_optMetMAP[i][0], &m_optMetScMAP[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetVBat, &m_optMetVBat[i][0], &m_optMetScVBat[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetIgnTim, &m_optMetIgnTim[i][0], &m_optMetScIgnTim[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetCLT, &m_optMetCLT[i][0], &m_optMetScCLT[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetAddI1, &m_optMetAddI1[i][0], &m_optMetScAddI1[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetAddI2, &m_optMetAddI2[i][0], &m_optMetScAddI2[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetInjPW, &m_optMetInjPW[i][0], &m_optMetScInjPW[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetIAT, &m_optMetIAT[i][0], &m_optMetScIAT[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetEGOCorr, &m_optMetEGOCorr[i][0], &m_optMetScEGOCorr[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetTPS, &m_optMetTPS[i][0], &m_optMetScTPS[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetAirFlow, &m_optMetAirFlow[i][0], &m_optMetScAirFlow[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetVehicleSpeed, &m_optMetVehicleSpeed[i][0], &m_optMetScVehicleSpeed[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetTPSDot, &m_optMetTPSDot[i][0], &m_optMetScTPSDot[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetMAPDot, &m_optMetMAPDot[i][0], &m_optMetScMAPDot[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetMAP2, &m_optMetMAP2[i][0], &m_optMetScMAP2[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetGPS, &m_optMetGPS[i][0], &m_optMetScGPS[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetMAPD, &m_optMetMAPD[i][0], &m_optMetScMAPD[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetTmp2, &m_optMetTmp2[i][0], &m_optMetScTmp2[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetFuelConsum, &m_optMetFuelConsum[i][0], &m_optMetScFuelConsum[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetKnockRetard, &m_optMetKnockRetard[i][0], &m_optMetScKnockRetard[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetKnockGraph, &m_optMetKnockGraph[i][0], &m_optMetScKnockGraph[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetSensAFR, &m_optMetSensAFR[i][0], &m_optMetScSensAFR[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetChokePos, &m_optMetChokePos[i][0], &m_optMetScChokePos[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetGDPos, &m_optMetGDPos[i][0], &m_optMetScGDPos[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetSynLoad, &m_optMetSynLoad[i][0], &m_optMetScSynLoad[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetInjTimB, &m_optMetInjTimB[i][0], &m_optMetScInjTimB[0]); //0...720
  _cpyMetersConfig(o_cfg[i].m_optMetInjTimE, &m_optMetInjTimE[i][0], &m_optMetScInjTimE[0]); //0...720
  _cpyMetersConfig(o_cfg[i].m_optMetInjTimB1, &m_optMetInjTimB[i][0], &m_optMetScInjTimB1[0]); //-360...360
  _cpyMetersConfig(o_cfg[i].m_optMetInjTimE1, &m_optMetInjTimE[i][0], &m_optMetScInjTimE1[0]); //-360...360
  _cpyMetersConfig(o_cfg[i].m_optMetFuelConsumF, &m_optMetFuelConsumF[i][0], &m_optMetScFuelConsumF[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetGrts, &m_optMetGrts[i][0], &m_optMetScGrts[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetFtls, &m_optMetFtls[i][0], &m_optMetScFtls[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetEgts, &m_optMetEgts[i][0], &m_optMetScEgts[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetOps, &m_optMetOps[i][0], &m_optMetScOps[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetInjDuty, &m_optMetInjDuty[i][0], &m_optMetScInjDuty[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetMAF, &m_optMetMAF[i][0], &m_optMetScMAF[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetVentDuty, &m_optMetVentDuty[i][0], &m_optMetScVentDuty[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetFts, &m_optMetFts[i][0], &m_optMetScFts[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetEGOCorr2, &m_optMetEGOCorr2[i][0], &m_optMetScEGOCorr2[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetSensAFR2, &m_optMetSensAFR2[i][0], &m_optMetScSensAFR2[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetTargAFR, &m_optMetTargAFR[i][0], &m_optMetScTargAFR[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetDiffAFR, &m_optMetDiffAFR[i][0], &m_optMetScDiffAFR[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetDiffAFR2, &m_optMetDiffAFR2[i][0], &m_optMetScDiffAFR2[0]);
  _cpyMetersConfig(o_cfg[i].m_optMetFPS, &m_optMetFPS[i][0], &m_optMetScFPS[0]);
 }
}

void CAppSettingsModel::SetMetersConfig(const MetersCfg* i_cfg)
{
 for(int i = 0; i < 2; ++i)
 {
  m_optMetRows[i].value = i_cfg[i].m_optMetRows;
  _cpyMetersConfig(i_cfg[i].m_optMetRPM, &m_optMetRPM[i][0], &m_optMetScRPM[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetMAP, &m_optMetMAP[i][0], &m_optMetScMAP[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetVBat, &m_optMetVBat[i][0], &m_optMetScVBat[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetIgnTim, &m_optMetIgnTim[i][0], &m_optMetScIgnTim[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetCLT, &m_optMetCLT[i][0], &m_optMetScCLT[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetAddI1, &m_optMetAddI1[i][0], &m_optMetScAddI1[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetAddI2, &m_optMetAddI2[i][0], &m_optMetScAddI2[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetInjPW, &m_optMetInjPW[i][0], &m_optMetScInjPW[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetIAT, &m_optMetIAT[i][0], &m_optMetScIAT[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetEGOCorr, &m_optMetEGOCorr[i][0], &m_optMetScEGOCorr[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetTPS, &m_optMetTPS[i][0], &m_optMetScTPS[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetAirFlow, &m_optMetAirFlow[i][0], &m_optMetScAirFlow[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetVehicleSpeed, &m_optMetVehicleSpeed[i][0], &m_optMetScVehicleSpeed[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetTPSDot, &m_optMetTPSDot[i][0], &m_optMetScTPSDot[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetMAPDot, &m_optMetMAPDot[i][0], &m_optMetScMAPDot[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetMAP2, &m_optMetMAP2[i][0], &m_optMetScMAP2[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetGPS, &m_optMetGPS[i][0], &m_optMetScGPS[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetMAPD, &m_optMetMAPD[i][0], &m_optMetScMAPD[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetTmp2, &m_optMetTmp2[i][0], &m_optMetScTmp2[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetFuelConsum, &m_optMetFuelConsum[i][0], &m_optMetScFuelConsum[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetKnockRetard, &m_optMetKnockRetard[i][0], &m_optMetScKnockRetard[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetKnockGraph, &m_optMetKnockGraph[i][0], &m_optMetScKnockGraph[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetSensAFR, &m_optMetSensAFR[i][0], &m_optMetScSensAFR[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetChokePos, &m_optMetChokePos[i][0], &m_optMetScChokePos[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetGDPos, &m_optMetGDPos[i][0], &m_optMetScGDPos[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetSynLoad, &m_optMetSynLoad[i][0], &m_optMetScSynLoad[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetInjTimB, &m_optMetInjTimB[i][0], &m_optMetScInjTimB[0]); //0...720
  _cpyMetersConfig(i_cfg[i].m_optMetInjTimE, &m_optMetInjTimE[i][0], &m_optMetScInjTimE[0]); //0...720
  _cpyMetersConfig(i_cfg[i].m_optMetInjTimB1, NULL, &m_optMetScInjTimB1[0]); //-360...360
  _cpyMetersConfig(i_cfg[i].m_optMetInjTimE1, NULL, &m_optMetScInjTimE1[0]); //-360...360
  _cpyMetersConfig(i_cfg[i].m_optMetFuelConsumF, &m_optMetFuelConsumF[i][0], &m_optMetScFuelConsumF[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetGrts, &m_optMetGrts[i][0], &m_optMetScGrts[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetFtls, &m_optMetFtls[i][0], &m_optMetScFtls[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetEgts, &m_optMetEgts[i][0], &m_optMetScEgts[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetOps, &m_optMetOps[i][0], &m_optMetScOps[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetInjDuty, &m_optMetInjDuty[i][0], &m_optMetScInjDuty[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetMAF, &m_optMetMAF[i][0], &m_optMetScMAF[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetVentDuty, &m_optMetVentDuty[i][0], &m_optMetScVentDuty[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetFts, &m_optMetFts[i][0], &m_optMetScFts[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetEGOCorr2, &m_optMetEGOCorr2[i][0], &m_optMetScEGOCorr2[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetSensAFR2, &m_optMetSensAFR2[i][0], &m_optMetScSensAFR2[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetTargAFR, &m_optMetTargAFR[i][0], &m_optMetScTargAFR[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetDiffAFR, &m_optMetDiffAFR[i][0], &m_optMetScDiffAFR[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetDiffAFR2, &m_optMetDiffAFR2[i][0], &m_optMetScDiffAFR2[0]);
  _cpyMetersConfig(i_cfg[i].m_optMetFPS, &m_optMetFPS[i][0], &m_optMetScFPS[0]);
 }
}

void CAppSettingsModel::SetLamDelMap(float* map, float* rb, float* lb)
{
 for(size_t i = 0; i < m_optLambdaDelay.value.size(); ++i)
  m_optLambdaDelay.value[i] = MathHelpers::Round(MathHelpers::RevLineGet<float, 3, 3>(map, i)); //convert from normal order to reverse order
 for(size_t i = 0; i < m_optLambdaDelayL.value.size(); ++i)
  m_optLambdaDelayL.value[i] = MathHelpers::Round(lb[i]); 
 for(size_t i = 0; i < m_optLambdaDelayR.value.size(); ++i)
  m_optLambdaDelayR.value[i] = MathHelpers::Round(rb[i]); 
}

void CAppSettingsModel::GetLamDelMap(float* map, float* rb, float* lb)
{
 for(size_t i = 0; i < m_optLambdaDelay.value.size(); ++i)
  map[i] = (float)MathHelpers::RevLineGet<int, 3, 3>(&m_optLambdaDelay.value[0], i); //convert from reverse order to normal order
 for(size_t i = 0; i < m_optLambdaDelayL.value.size(); ++i)
  lb[i] = (float)m_optLambdaDelayL.value[i];
 for(size_t i = 0; i < m_optLambdaDelayR.value.size(); ++i)
  rb[i] = (float)m_optLambdaDelayR.value[i];
}

float CAppSettingsModel::GetAFRError(void)
{
 return m_optAFRError.value;
}

void CAppSettingsModel::SetBlockedCells(const bool* blList)
{
 m_optBlockedCells.value.clear();
 for(size_t i = 0; i < (16*16); ++i)
 {
  if (true == blList[i])
   m_optBlockedCells.value.push_back(i);
 }
}

void CAppSettingsModel::GetBlockedCells(bool* blList)
{
 for(size_t i = 0; i < (16*16); ++i)
  blList[i] = false;
 for(size_t i = 0; i < m_optBlockedCells.value.size(); ++i)
  blList[m_optBlockedCells.value[i]] = true;
}

int CAppSettingsModel::GetStatSize(void)
{
 return m_optStatSize.value;
}

int CAppSettingsModel::GetAutoBlockThrd(void)
{
 return m_optAutoBlockThrd.value;
}

bool CAppSettingsModel::GetGrowingMode(void)
{
 return m_optGrowingMode.value;
}

float CAppSettingsModel::GetMinAFR(void)
{
 return m_optMinAFR.value;
}

float CAppSettingsModel::GetMaxAFR(void)
{
 return m_optMaxAFR.value;
}

int CAppSettingsModel::GetMinDistThrd(void)
{
 return m_optMinDistThrd.value;
}

float CAppSettingsModel::GetMinTPS(void)
{
 return m_optMinTPS.value;
}

float CAppSettingsModel::GetMaxTPS(void)
{
 return m_optMaxTPS.value;
}

float CAppSettingsModel::GetCLTThrd(void)
{
 return m_optCLTThrd.value;
}

float CAppSettingsModel::GetTunSoftness(void)
{
 return m_optTunSoftness.value;
}

void CAppSettingsModel::SetShowExFixtures(bool i_show)
{
 m_optShowExFixtures.value = i_show;
}

void CAppSettingsModel::SetShowSpeedAndDist(bool i_show)
{
 m_optShowSpeedAndDist.value = i_show;
}

bool CAppSettingsModel::GetPortAutoReopen(void) const
{
 return m_optPortAutoReopen.value;
}

int CAppSettingsModel::GetGradSaturation(void)
{
 return m_optGradSaturation.value;
}

int CAppSettingsModel::GetGradBrightness(void)
{
 return m_optGradBrightness.value;
}

int CAppSettingsModel::GetBoldFont(void)
{
 return m_optBoldFont.value;
}

bool CAppSettingsModel::GetToggleMapWnd(void) const
{
 return m_optToggleMapWnd.value;
}

void CAppSettingsModel::SetToggleMapWnd(bool toggle)
{
 m_optToggleMapWnd.value = toggle;
}

bool CAppSettingsModel::GetEmbedMapWnd(void) const
{
 return m_optEmbedMapWnd.value;
}

void CAppSettingsModel::SetEmbedMapWnd(bool embed)
{
 m_optEmbedMapWnd.value = embed;
}

int CAppSettingsModel::GetParamMonVert(void) const
{
 return m_optParamMonVert.value;
}

void CAppSettingsModel::SetParamMonVert(int pos)
{
 m_optParamMonVert.value = pos;
}

int CAppSettingsModel::GetLogPlayerVert(void) const
{
 return m_optLogPlayerVert.value;
}

void CAppSettingsModel::SetLogPlayerVert(int pos)
{
 m_optLogPlayerVert.value = pos;
}

int CAppSettingsModel::GetTitleFontSize(void) const
{
 return m_optTitleFontSize.value;
}

int CAppSettingsModel::GetValueFontSize(void) const
{
 return m_optValueFontSize.value;
}

int CAppSettingsModel::GetPaneFontSize(void) const
{
 return m_optPaneFontSize.value;
}

int CAppSettingsModel::GetLabelFontSize(void) const
{
 return m_optLabelFontSize.value;
}

bool CAppSettingsModel::GetExistingPorts(void) const
{
 return m_optExistingPorts.value;
}

void CAppSettingsModel::SetTitleFontSize(int size)
{
 m_optTitleFontSize.value = size;
}

void CAppSettingsModel::SetValueFontSize(int size)
{
 m_optValueFontSize.value = size;
}

void CAppSettingsModel::SetPaneFontSize(int size)
{
 m_optPaneFontSize.value = size;
}

void CAppSettingsModel::SetLabelFontSize(int size)
{
 m_optLabelFontSize.value = size;
}

bool CAppSettingsModel::GetMetersDragNDrop(void) const
{
 return m_optMetersDragNDrop.value;
}

void CAppSettingsModel::SetMetersDragNDrop(bool enable)
{
 m_optMetersDragNDrop.value = enable;
}

bool CAppSettingsModel::GetIndicatorsDragNDrop(void) const
{
 return m_optIndicatorsDragNDrop.value;
}

void CAppSettingsModel::SetIndicatorsDragNDrop(bool enable)
{
 m_optIndicatorsDragNDrop.value = enable;
}

int CAppSettingsModel::GetFFFConst(void) const
{
 return m_optFFFConst.value;
}

COLORREF CAppSettingsModel::GetVoltLineColor(void) const
{
 return m_optVoltLineColor.value;
}

float CAppSettingsModel::GetPeakOnPtMovStep(void) const
{
 return m_optPeakOnPtMovStep.value;
}

float CAppSettingsModel::GetPeakDutyPtMovStep(void) const
{
 return m_optPeakDutyPtMovStep.value;
}

float CAppSettingsModel::GetHoldDutyPtMovStep(void) const
{
 return m_optHoldDutyPtMovStep.value;
}

float CAppSettingsModel::GetPeakFullPtMovStep(void) const
{
 return m_optPeakFullPtMovStep.value;
}

float CAppSettingsModel::GetPthPausePtMovStep(void) const
{
 return m_optPthPausePtMovStep.value;
}

bool CAppSettingsModel::GetInjDrvTabActive(void) const
{
 return m_optInjDrvTabActive.value;
}

float CAppSettingsModel::GetPWMulPtMovStep(void) const
{
 return m_optPWMulPtMovStep.value;
}

float CAppSettingsModel::GetPWAddPtMovStep(void) const
{
 return m_optPWAddPtMovStep.value;
}

int CAppSettingsModel::GetITEdMode(void) const
{
 return m_optITEdMode.value;
}

void CAppSettingsModel::SetITEdMode(int mode)
{
 m_optITEdMode.value = mode;
}

int CAppSettingsModel::GetActiveVEMap(void) const
{
 return m_optActiveVEMap.value;
}

void CAppSettingsModel::SetActiveVEMap(int id)
{
 m_optActiveVEMap.value = id;
}

void CAppSettingsModel::SetMapPtMovStep(const MapPtMovStep& i_ptMovStep)
{
 m_optPtMovStepWorkMap.value = i_ptMovStep.m_value[ETMT_IGN_WORK];
 m_optPtMovStepTempMap.value = i_ptMovStep.m_value[ETMT_IGN_TEMP];
 m_optPtMovStepTempIdlMap.value = i_ptMovStep.m_value[ETMT_IGN_TEMPI];
 m_optPtMovStepStartMap.value = i_ptMovStep.m_value[ETMT_IGN_START];
 m_optPtMovStepIdleMap.value = i_ptMovStep.m_value[ETMT_IGN_IDLE];
 m_optPtMovStepVeMap.value = i_ptMovStep.m_value[ETMT_INJ_VE];
 m_optPtMovStepVe2Map.value = i_ptMovStep.m_value[ETMT_INJ_VE2];
 m_optPtMovStepVeiMap.value = i_ptMovStep.m_value[ETMT_INJ_IVE];
 m_optPtMovStepAfrMap.value = i_ptMovStep.m_value[ETMT_INJ_AFR];
 m_optPtMovStepCrnkMap.value = i_ptMovStep.m_value[ETMT_INJ_CRNK];
 m_optPtMovStepWrmpMap.value = i_ptMovStep.m_value[ETMT_INJ_WRMP];
 m_optPtMovStepDeadMap.value = i_ptMovStep.m_value[ETMT_INJ_DEAD];
 m_optPtMovStepIdlrMap.value = i_ptMovStep.m_value[ETMT_INJ_IDLR];
 m_optPtMovStepIdlcMap.value = i_ptMovStep.m_value[ETMT_INJ_IDLC];
 m_optPtMovStepThrassMap.value = i_ptMovStep.m_value[ETMT_INJ_THRASS];
 m_optPtMovStepAetpsMap.value = i_ptMovStep.m_value[ETMT_INJ_AETPS];
 m_optPtMovStepAemapMap.value = i_ptMovStep.m_value[ETMT_INJ_AEMAP];
 m_optPtMovStepAerpmMap.value = i_ptMovStep.m_value[ETMT_INJ_AERPM];
 m_optPtMovStepAftstrMap.value = i_ptMovStep.m_value[ETMT_INJ_AFTSTR];
 m_optPtMovStepItMap.value = i_ptMovStep.m_value[ETMT_INJ_IT];
 m_optPtMovStepItrpmMap.value = i_ptMovStep.m_value[ETMT_INJ_ITRPM];
 m_optPtMovStepRigidMap.value = i_ptMovStep.m_value[ETMT_INJ_RIGID];
 m_optPtMovStepEgocrvMap.value = i_ptMovStep.m_value[ETMT_INJ_EGOCRV];
 m_optPtMovStepIaccMap.value = i_ptMovStep.m_value[ETMT_INJ_IACC];
 m_optPtMovStepIaccwMap.value = i_ptMovStep.m_value[ETMT_INJ_IACCW];
 m_optPtMovStepIatcltMap.value = i_ptMovStep.m_value[ETMT_INJ_IATCLT];
 m_optPtMovStepTpsswtMap.value = i_ptMovStep.m_value[ETMT_INJ_TPSSWT];
 m_optPtMovStepGtscMap.value = i_ptMovStep.m_value[ETMT_INJ_GTSC];
 m_optPtMovStepGpscMap.value = i_ptMovStep.m_value[ETMT_INJ_GPSC];
 m_optPtMovStepAtscMap.value = i_ptMovStep.m_value[ETMT_INJ_ATSC];
 m_optPtMovStepPwm1Map.value = i_ptMovStep.m_value[ETMT_PWM1];
 m_optPtMovStepPwm2Map.value = i_ptMovStep.m_value[ETMT_PWM2];
 m_optPtMovStepIACMATMap.value = i_ptMovStep.m_value[ETMT_INJ_IACMAT];
 m_optPtMovStepTpszonMap.value = i_ptMovStep.m_value[ETMT_INJ_TPSZON];
 m_optPtMovStepCylMultMap.value = i_ptMovStep.m_value[ETMT_INJ_CYLMULT];
 m_optPtMovStepCylAddMap.value = i_ptMovStep.m_value[ETMT_INJ_CYLADD];
 //separate
 m_optPtMovStepAttenuatorMap.value = i_ptMovStep.m_value[ETMT_ATTENUATOR];
 m_optPtMovStepDwellCntrlMap.value = i_ptMovStep.m_value[ETMT_DWELLCNTRL];
 m_optPtMovStepCTSCurveMap.value = i_ptMovStep.m_value[ETMT_CTS_CURVE];
 m_optPtMovStepATSCurvMap.value = i_ptMovStep.m_value[ETMT_ATS_CURVE];
 m_optPtMovStepATSCorrMap.value = i_ptMovStep.m_value[ETMT_ATS_CORR];
 m_optPtMovStepGasdoseMap.value = i_ptMovStep.m_value[ETMT_GASDOSE];
 m_optPtMovStepBarocorrMap.value = i_ptMovStep.m_value[ETMT_BAROCORR];
 m_optPtMovStepManIgntimMap.value = i_ptMovStep.m_value[ETMT_MANIGNTIM];
 m_optPtMovStepTmp2CurveMap.value = i_ptMovStep.m_value[ETMT_TMP2_CURVE];
 m_optPtMovStepCrkTempMap.value = i_ptMovStep.m_value[ETMT_CRKCLT_CORR];
 m_optPtMovStepEHPauseMap.value = i_ptMovStep.m_value[ETMT_EH_PAUSE];
 m_optPtMovStepCrankingThrdMap.value = i_ptMovStep.m_value[ETMT_CRANKING_THRD];
 m_optPtMovStepCrankingTimeMap.value = i_ptMovStep.m_value[ETMT_CRANKING_TIME];
 m_optPtMovStepSmapabanThrdMap.value = i_ptMovStep.m_value[ETMT_SMAPABAN_THRD];
 m_optPtMovStepKnockZoneMap.value = i_ptMovStep.m_value[ETMT_KNOCK_ZONE];
 m_optPtMovStepGrtsCurveMap.value = i_ptMovStep.m_value[ETMT_GRTS_CURVE];
 m_optPtMovStepGrHeatDutyMap.value = i_ptMovStep.m_value[ETMT_GRHEAT_DUTY];
 m_optPtMovStepPwmIacUCoefMap.value = i_ptMovStep.m_value[ETMT_PWMIAC_UCOEF];
 m_optPtMovStepAftstrStrk0Map.value = i_ptMovStep.m_value[ETMT_AFTSTR_STRK0];
 m_optPtMovStepAftstrStrk1Map.value = i_ptMovStep.m_value[ETMT_AFTSTR_STRK1];
 m_optPtMovStepGrValDelMap.value = i_ptMovStep.m_value[ETMT_GRVDELAY];
 m_optPtMovStepFtlsCurveMap.value = i_ptMovStep.m_value[ETMT_FTLS_CURVE];
 m_optPtMovStepEgtsCurveMap.value = i_ptMovStep.m_value[ETMT_EGTS_CURVE];
 m_optPtMovStepOpsCurveMap.value = i_ptMovStep.m_value[ETMT_OPS_CURVE];
 m_optPtMovStepManInjPwcMap.value = i_ptMovStep.m_value[ETMT_MANINJPWC];
 m_optPtMovStepMAFCurveMap.value = i_ptMovStep.m_value[ETMT_MAF_CURVE];
 m_optPtMovStepFtlsCorMap.value = i_ptMovStep.m_value[ETMT_FTLSCOR];
 m_optPtMovStepLambdaZoneMap.value = i_ptMovStep.m_value[ETMT_LAMBDA_ZONE];
 m_optPtMovStepFtsCurveMap.value = i_ptMovStep.m_value[ETMT_FTS_CURVE];
 m_optPtMovStepFuelDensCorrMap.value = i_ptMovStep.m_value[ETMT_FUELDENS_CORR];
 m_optPtMovStepXtauXfAccMap.value = i_ptMovStep.m_value[ETMT_XTAU_XFACC];
 m_optPtMovStepXtauXfDecMap.value = i_ptMovStep.m_value[ETMT_XTAU_XFDEC];
 m_optPtMovStepXtauTfAccMap.value = i_ptMovStep.m_value[ETMT_XTAU_TFACC];
 m_optPtMovStepXtauTfDecMap.value = i_ptMovStep.m_value[ETMT_XTAU_TFDEC];
 m_optPtMovStepInjNonLinPMap.value = i_ptMovStep.m_value[ETMT_INJNONLINP];
 m_optPtMovStepInjNonLinGMap.value = i_ptMovStep.m_value[ETMT_INJNONLING];
 m_optPtMovStepEGODelayMap.value = i_ptMovStep.m_value[ETMT_EGO_DELAY];
 m_optPtMovStepWUAFR0Map.value = i_ptMovStep.m_value[ETMT_WU_AFR0];
 m_optPtMovStepWUAFR1Map.value = i_ptMovStep.m_value[ETMT_WU_AFR1];
}

void CAppSettingsModel::GetMapPtMovStep(MapPtMovStep& o_ptMovStep) const
{
 o_ptMovStep.m_value[ETMT_IGN_WORK] = m_optPtMovStepWorkMap.value;
 o_ptMovStep.m_value[ETMT_IGN_TEMP] = m_optPtMovStepTempMap.value;
 o_ptMovStep.m_value[ETMT_IGN_TEMPI] = m_optPtMovStepTempIdlMap.value;
 o_ptMovStep.m_value[ETMT_IGN_START] = m_optPtMovStepStartMap.value;
 o_ptMovStep.m_value[ETMT_IGN_IDLE] = m_optPtMovStepIdleMap.value;
 o_ptMovStep.m_value[ETMT_INJ_VE] = m_optPtMovStepVeMap.value;
 o_ptMovStep.m_value[ETMT_INJ_VE2] = m_optPtMovStepVe2Map.value;
 o_ptMovStep.m_value[ETMT_INJ_IVE] = m_optPtMovStepVeiMap.value;
 o_ptMovStep.m_value[ETMT_INJ_AFR] = m_optPtMovStepAfrMap.value;
 o_ptMovStep.m_value[ETMT_INJ_CRNK] = m_optPtMovStepCrnkMap.value;
 o_ptMovStep.m_value[ETMT_INJ_WRMP] = m_optPtMovStepWrmpMap.value;
 o_ptMovStep.m_value[ETMT_INJ_DEAD] = m_optPtMovStepDeadMap.value;
 o_ptMovStep.m_value[ETMT_INJ_IDLR] = m_optPtMovStepIdlrMap.value;
 o_ptMovStep.m_value[ETMT_INJ_IDLC] = m_optPtMovStepIdlcMap.value;
 o_ptMovStep.m_value[ETMT_INJ_THRASS] = m_optPtMovStepThrassMap.value;
 o_ptMovStep.m_value[ETMT_INJ_AETPS] = m_optPtMovStepAetpsMap.value;
 o_ptMovStep.m_value[ETMT_INJ_AEMAP] = m_optPtMovStepAemapMap.value;
 o_ptMovStep.m_value[ETMT_INJ_AERPM] = m_optPtMovStepAerpmMap.value;
 o_ptMovStep.m_value[ETMT_INJ_AFTSTR] = m_optPtMovStepAftstrMap.value;
 o_ptMovStep.m_value[ETMT_INJ_IT] = m_optPtMovStepItMap.value;
 o_ptMovStep.m_value[ETMT_INJ_ITRPM] = m_optPtMovStepItrpmMap.value;
 o_ptMovStep.m_value[ETMT_INJ_RIGID] = m_optPtMovStepRigidMap.value;
 o_ptMovStep.m_value[ETMT_INJ_EGOCRV] = m_optPtMovStepEgocrvMap.value;
 o_ptMovStep.m_value[ETMT_INJ_IACC] = m_optPtMovStepIaccMap.value;
 o_ptMovStep.m_value[ETMT_INJ_IACCW] = m_optPtMovStepIaccwMap.value;
 o_ptMovStep.m_value[ETMT_INJ_IATCLT] = m_optPtMovStepIatcltMap.value;
 o_ptMovStep.m_value[ETMT_INJ_TPSSWT] = m_optPtMovStepTpsswtMap.value;
 o_ptMovStep.m_value[ETMT_INJ_GTSC] = m_optPtMovStepGtscMap.value;
 o_ptMovStep.m_value[ETMT_INJ_GPSC] = m_optPtMovStepGpscMap.value;
 o_ptMovStep.m_value[ETMT_INJ_ATSC] = m_optPtMovStepAtscMap.value;
 o_ptMovStep.m_value[ETMT_PWM1] = m_optPtMovStepPwm1Map.value;
 o_ptMovStep.m_value[ETMT_PWM2] = m_optPtMovStepPwm2Map.value;
 o_ptMovStep.m_value[ETMT_INJ_IACMAT] = m_optPtMovStepIACMATMap.value;
 o_ptMovStep.m_value[ETMT_INJ_TPSZON] = m_optPtMovStepTpszonMap.value;
 o_ptMovStep.m_value[ETMT_INJ_CYLMULT] = m_optPtMovStepCylMultMap.value;
 o_ptMovStep.m_value[ETMT_INJ_CYLADD] = m_optPtMovStepCylAddMap.value;

 //separate
 o_ptMovStep.m_value[ETMT_ATTENUATOR] = m_optPtMovStepAttenuatorMap.value;
 o_ptMovStep.m_value[ETMT_DWELLCNTRL] = m_optPtMovStepDwellCntrlMap.value;
 o_ptMovStep.m_value[ETMT_CTS_CURVE] = m_optPtMovStepCTSCurveMap.value;
 o_ptMovStep.m_value[ETMT_ATS_CURVE] = m_optPtMovStepATSCurvMap.value;
 o_ptMovStep.m_value[ETMT_ATS_CORR] = m_optPtMovStepATSCorrMap.value;
 o_ptMovStep.m_value[ETMT_GASDOSE] = m_optPtMovStepGasdoseMap.value;
 o_ptMovStep.m_value[ETMT_BAROCORR] = m_optPtMovStepBarocorrMap.value;
 o_ptMovStep.m_value[ETMT_MANIGNTIM] = m_optPtMovStepManIgntimMap.value;
 o_ptMovStep.m_value[ETMT_TMP2_CURVE] = m_optPtMovStepTmp2CurveMap.value;
 o_ptMovStep.m_value[ETMT_CRKCLT_CORR] = m_optPtMovStepCrkTempMap.value;
 o_ptMovStep.m_value[ETMT_EH_PAUSE] = m_optPtMovStepEHPauseMap.value;
 o_ptMovStep.m_value[ETMT_CRANKING_THRD] = m_optPtMovStepCrankingThrdMap.value;
 o_ptMovStep.m_value[ETMT_CRANKING_TIME] = m_optPtMovStepCrankingTimeMap.value;
 o_ptMovStep.m_value[ETMT_SMAPABAN_THRD] = m_optPtMovStepSmapabanThrdMap.value;
 o_ptMovStep.m_value[ETMT_KNOCK_ZONE] = m_optPtMovStepKnockZoneMap.value;
 o_ptMovStep.m_value[ETMT_GRTS_CURVE] = m_optPtMovStepGrtsCurveMap.value;
 o_ptMovStep.m_value[ETMT_GRHEAT_DUTY] = m_optPtMovStepGrHeatDutyMap.value;
 o_ptMovStep.m_value[ETMT_PWMIAC_UCOEF]= m_optPtMovStepPwmIacUCoefMap.value;
 o_ptMovStep.m_value[ETMT_AFTSTR_STRK0] = m_optPtMovStepAftstrStrk0Map.value;
 o_ptMovStep.m_value[ETMT_AFTSTR_STRK1] = m_optPtMovStepAftstrStrk1Map.value;
 o_ptMovStep.m_value[ETMT_GRVDELAY] = m_optPtMovStepGrValDelMap.value;
 o_ptMovStep.m_value[ETMT_FTLS_CURVE] = m_optPtMovStepFtlsCurveMap.value;
 o_ptMovStep.m_value[ETMT_EGTS_CURVE] = m_optPtMovStepEgtsCurveMap.value;
 o_ptMovStep.m_value[ETMT_OPS_CURVE] = m_optPtMovStepOpsCurveMap.value;
 o_ptMovStep.m_value[ETMT_MANINJPWC] = m_optPtMovStepManInjPwcMap.value;
 o_ptMovStep.m_value[ETMT_MAF_CURVE] = m_optPtMovStepMAFCurveMap.value;
 o_ptMovStep.m_value[ETMT_FTLSCOR] = m_optPtMovStepFtlsCorMap.value;
 o_ptMovStep.m_value[ETMT_LAMBDA_ZONE] = m_optPtMovStepLambdaZoneMap.value;
 o_ptMovStep.m_value[ETMT_FTS_CURVE] = m_optPtMovStepFtsCurveMap.value;
 o_ptMovStep.m_value[ETMT_FUELDENS_CORR] = m_optPtMovStepFuelDensCorrMap.value;
 o_ptMovStep.m_value[ETMT_XTAU_XFACC] = m_optPtMovStepXtauXfAccMap.value;
 o_ptMovStep.m_value[ETMT_XTAU_XFDEC] = m_optPtMovStepXtauXfDecMap.value;
 o_ptMovStep.m_value[ETMT_XTAU_TFACC] = m_optPtMovStepXtauTfAccMap.value;
 o_ptMovStep.m_value[ETMT_XTAU_TFDEC] = m_optPtMovStepXtauTfDecMap.value;
 o_ptMovStep.m_value[ETMT_INJNONLINP] = m_optPtMovStepInjNonLinPMap.value;
 o_ptMovStep.m_value[ETMT_INJNONLING] = m_optPtMovStepInjNonLinGMap.value;
 o_ptMovStep.m_value[ETMT_EGO_DELAY] = m_optPtMovStepEGODelayMap.value;
 o_ptMovStep.m_value[ETMT_WU_AFR0] = m_optPtMovStepWUAFR0Map.value;
 o_ptMovStep.m_value[ETMT_WU_AFR1] = m_optPtMovStepWUAFR1Map.value;
}

void CAppSettingsModel::SetLogFileFields(const LogFileFields& i_flds)
{
 m_optLogFieldTime.value = i_flds.m_fldTime;
 m_optLogFieldRPM.value = i_flds.m_fldRPM;
 m_optLogFieldIgnTim.value = i_flds.m_fldIgnTim;
 m_optLogFieldMAP.value = i_flds.m_fldMAP;
 m_optLogFieldVBat.value = i_flds.m_fldVBat;
 m_optLogFieldCLT.value = i_flds.m_fldCLT;
 m_optLogFieldKnock.value = i_flds.m_fldKnock;
 m_optLogFieldKnockCorr.value = i_flds.m_fldKnockCorr;
 m_optLogFieldLoadCurve.value = i_flds.m_fldLoadCurve;
 m_optLogFieldCarb.value = i_flds.m_fldCarb;
 m_optLogFieldGas_v.value = i_flds.m_fldGas_v;
 m_optLogFieldIdleValve.value = i_flds.m_fldIdleValve;
 m_optLogFieldPowerValve.value = i_flds.m_fldPowerValve;
 m_optLogFieldCoolingFan.value = i_flds.m_fldCoolingFan;
 m_optLogFieldStBlock.value = i_flds.m_fldStBlock;
 m_optLogFieldAE.value = i_flds.m_fldAE;
 m_optLogFieldFCRevLim.value = i_flds.m_fldFCRevLim;
 m_optLogFieldFloodClear.value = i_flds.m_fldFloodClear;
 m_optLogFieldSysLocked.value = i_flds.m_fldSysLocked;
 m_optLogFieldCE.value = i_flds.m_fldCE;
 m_optLogFieldIgn_i.value = i_flds.m_fldIgn_i;
 m_optLogFieldCond_i.value = i_flds.m_fldCond_i;
 m_optLogFieldEpas_i.value = i_flds.m_fldEpas_i;
 m_optLogFieldGpa4_i.value = i_flds.m_fldGpa4_i;
 m_optLogFieldInput1.value = i_flds.m_fldInput1;
 m_optLogFieldInput2.value = i_flds.m_fldInput2;
 m_optLogFieldAuto_i.value = i_flds.m_fldAuto_i;
 m_optLogFieldMapsel0.value = i_flds.m_fldMapsel0;
 m_optLogFieldRefprs_i.value = i_flds.m_fldRefprs_i;
 m_optLogFieldAltrn_i.value = i_flds.m_fldAltrn_i;
 m_optLogFieldTPS.value = i_flds.m_fldTPS;
 m_optLogFieldAdd_i1.value = i_flds.m_fldAdd_i1;
 m_optLogFieldAdd_i2.value = i_flds.m_fldAdd_i2;
 m_optLogFieldChokePos.value = i_flds.m_fldChokePos;
 m_optLogFieldGDPos.value = i_flds.m_fldGDPos;
 m_optLogFieldVehSpeed.value = i_flds.m_fldVehSpeed;
 m_optLogFieldPasDist.value = i_flds.m_fldPasDist;
 m_optLogFieldFuelConsum.value = i_flds.m_fldFuelConsum;
 m_optLogFieldFuelConsFreq.value = i_flds.m_fldFuelConsFreq;
 m_optLogFieldIAT.value = i_flds.m_fldIAT;
 m_optLogFieldStrtIgnTim.value = i_flds.m_fldStrtIgnTim;
 m_optLogFieldIdleIgnTim.value = i_flds.m_fldIdleIgnTim;
 m_optLogFieldWorkIgnTim.value = i_flds.m_fldWorkIgnTim;
 m_optLogFieldTempIgnTim.value = i_flds.m_fldTempIgnTim;
 m_optLogFieldIATIgnTim.value = i_flds.m_fldIATIgnTim;
 m_optLogFieldIdlRegIgnTim.value = i_flds.m_fldIdlRegIgnTim;
 m_optLogFieldOctanCorr.value = i_flds.m_fldOctanCorr;
 m_optLogFieldEGOCorr.value = i_flds.m_fldEGOCorr;
 m_optLogFieldEGOCorr2.value = i_flds.m_fldEGOCorr2;
 m_optLogFieldInjPW.value = i_flds.m_fldInjPW;
 m_optLogFieldTPSdot.value = i_flds.m_fldTPSdot;
 m_optLogFieldMAP2.value = i_flds.m_fldMAP2;
 m_optLogFieldTmp2.value = i_flds.m_fldTmp2;
 m_optLogFieldDiffMAP.value = i_flds.m_fldDiffMAP;
 m_optLogFieldAFR.value = i_flds.m_fldAFR;
 m_optLogFieldAFR2.value = i_flds.m_fldAFR2;
 m_optLogFieldSynLoad.value = i_flds.m_fldSynLoad;
 m_optLogFieldBaroPress.value = i_flds.m_fldBaroPress;
 m_optLogFieldInjTimBeg.value = i_flds.m_fldInjTimBeg;
 m_optLogFieldInjTimEnd.value = i_flds.m_fldInjTimEnd;
 m_optLogFieldLogMarks.value = i_flds.m_fldLogMarks;
 m_optLogFieldCECodes.value = i_flds.m_fldCECodes;
 m_optLogFieldGrts.value = i_flds.m_fldGrts;
 m_optLogFieldFtls.value = i_flds.m_fldFtls;
 m_optLogFieldEgts.value = i_flds.m_fldEgts;
 m_optLogFieldOps.value = i_flds.m_fldOps;
 m_optLogFieldAftStrEnr.value = i_flds.m_fldAftStrEnr;
 m_optLogFieldIacClLoop.value = i_flds.m_fldIacClLoop;
 m_optLogFieldInjDuty.value = i_flds.m_fldInjDuty;
 m_optLogFieldRigidArg.value = i_flds.m_fldRigidArg;
 m_optLogFieldServFlag.value = i_flds.m_fldServFlag;
 m_optLogFieldRxlaf.value = i_flds.m_fldRxlaf;
 m_optLogFieldMAF.value = i_flds.m_fldMAF;
 m_optLogFieldVentDuty.value = i_flds.m_fldVentDuty;
 m_optLogFieldUniOuts.value = i_flds.m_fldUniOuts;
 m_optLogFieldMAPdot.value = i_flds.m_fldMAPdot;
 m_optLogFieldFts.value = i_flds.m_fldFts;
 m_optLogFieldConsFuel.value = i_flds.m_fldConsFuel;
 m_optLogFieldAFRMap.value = i_flds.m_fldAFRMap;
 m_optLogFieldTchrg.value = i_flds.m_fldTchrg;
 m_optLogFieldGPS.value = i_flds.m_fldGPS;
 m_optLogFieldFPS.value = i_flds.m_fldFPS;
}

void CAppSettingsModel::GetLogFileFields(LogFileFields& o_flds) const
{
 o_flds.m_fldTime = m_optLogFieldTime.value;
 o_flds.m_fldRPM = m_optLogFieldRPM.value;
 o_flds.m_fldIgnTim = m_optLogFieldIgnTim.value;
 o_flds.m_fldMAP = m_optLogFieldMAP.value;
 o_flds.m_fldVBat = m_optLogFieldVBat.value;
 o_flds.m_fldCLT = m_optLogFieldCLT.value;
 o_flds.m_fldKnock = m_optLogFieldKnock.value;
 o_flds.m_fldKnockCorr = m_optLogFieldKnockCorr.value;
 o_flds.m_fldLoadCurve = m_optLogFieldLoadCurve.value;
 o_flds.m_fldCarb = m_optLogFieldCarb.value;
 o_flds.m_fldGas_v = m_optLogFieldGas_v.value;
 o_flds.m_fldIdleValve = m_optLogFieldIdleValve.value;
 o_flds.m_fldPowerValve = m_optLogFieldPowerValve.value;
 o_flds.m_fldCoolingFan = m_optLogFieldCoolingFan.value;
 o_flds.m_fldStBlock = m_optLogFieldStBlock.value;
 o_flds.m_fldAE = m_optLogFieldAE.value;
 o_flds.m_fldFCRevLim = m_optLogFieldFCRevLim.value;
 o_flds.m_fldFloodClear = m_optLogFieldFloodClear.value;
 o_flds.m_fldSysLocked = m_optLogFieldSysLocked.value;
 o_flds.m_fldCE = m_optLogFieldCE.value;
 o_flds.m_fldIgn_i = m_optLogFieldIgn_i.value;
 o_flds.m_fldCond_i = m_optLogFieldCond_i.value;
 o_flds.m_fldEpas_i = m_optLogFieldEpas_i.value;
 o_flds.m_fldGpa4_i = m_optLogFieldGpa4_i.value;
 o_flds.m_fldInput1 = m_optLogFieldInput1.value;
 o_flds.m_fldInput2 = m_optLogFieldInput2.value;
 o_flds.m_fldAuto_i = m_optLogFieldAuto_i.value;
 o_flds.m_fldMapsel0 = m_optLogFieldMapsel0.value;
 o_flds.m_fldRefprs_i = m_optLogFieldRefprs_i.value;
 o_flds.m_fldAltrn_i = m_optLogFieldAltrn_i.value;
 o_flds.m_fldTPS = m_optLogFieldTPS.value;
 o_flds.m_fldAdd_i1 = m_optLogFieldAdd_i1.value;
 o_flds.m_fldAdd_i2 = m_optLogFieldAdd_i2.value;
 o_flds.m_fldChokePos = m_optLogFieldChokePos.value;
 o_flds.m_fldGDPos = m_optLogFieldGDPos.value;
 o_flds.m_fldVehSpeed = m_optLogFieldVehSpeed.value;
 o_flds.m_fldPasDist = m_optLogFieldPasDist.value;
 o_flds.m_fldFuelConsum = m_optLogFieldFuelConsum.value;
 o_flds.m_fldFuelConsFreq = m_optLogFieldFuelConsFreq.value;
 o_flds.m_fldIAT = m_optLogFieldIAT.value;
 o_flds.m_fldStrtIgnTim = m_optLogFieldStrtIgnTim.value;
 o_flds.m_fldIdleIgnTim = m_optLogFieldIdleIgnTim.value;
 o_flds.m_fldWorkIgnTim = m_optLogFieldWorkIgnTim.value;
 o_flds.m_fldTempIgnTim = m_optLogFieldTempIgnTim.value;
 o_flds.m_fldIATIgnTim = m_optLogFieldIATIgnTim.value;
 o_flds.m_fldIdlRegIgnTim = m_optLogFieldIdlRegIgnTim.value;
 o_flds.m_fldOctanCorr = m_optLogFieldOctanCorr.value;
 o_flds.m_fldEGOCorr = m_optLogFieldEGOCorr.value;
 o_flds.m_fldEGOCorr2 = m_optLogFieldEGOCorr2.value;
 o_flds.m_fldInjPW = m_optLogFieldInjPW.value;
 o_flds.m_fldTPSdot = m_optLogFieldTPSdot.value;
 o_flds.m_fldMAP2 = m_optLogFieldMAP2.value;
 o_flds.m_fldTmp2 = m_optLogFieldTmp2.value;
 o_flds.m_fldDiffMAP = m_optLogFieldDiffMAP.value;
 o_flds.m_fldAFR = m_optLogFieldAFR.value;
 o_flds.m_fldAFR2 = m_optLogFieldAFR2.value;
 o_flds.m_fldSynLoad = m_optLogFieldSynLoad.value;
 o_flds.m_fldBaroPress = m_optLogFieldBaroPress.value;
 o_flds.m_fldInjTimBeg = m_optLogFieldInjTimBeg.value;
 o_flds.m_fldInjTimEnd = m_optLogFieldInjTimEnd.value;
 o_flds.m_fldLogMarks = m_optLogFieldLogMarks.value;
 o_flds.m_fldCECodes = m_optLogFieldCECodes.value;
 o_flds.m_fldGrts = m_optLogFieldGrts.value;
 o_flds.m_fldFtls = m_optLogFieldFtls.value;
 o_flds.m_fldEgts = m_optLogFieldEgts.value;
 o_flds.m_fldOps = m_optLogFieldOps.value;
 o_flds.m_fldAftStrEnr = m_optLogFieldAftStrEnr.value;
 o_flds.m_fldIacClLoop = m_optLogFieldIacClLoop.value;
 o_flds.m_fldInjDuty = m_optLogFieldInjDuty.value;
 o_flds.m_fldRigidArg = m_optLogFieldRigidArg.value;
 o_flds.m_fldServFlag = m_optLogFieldServFlag.value;
 o_flds.m_fldRxlaf = m_optLogFieldRxlaf.value;
 o_flds.m_fldMAF = m_optLogFieldMAF.value;
 o_flds.m_fldVentDuty = m_optLogFieldVentDuty.value;
 o_flds.m_fldUniOuts = m_optLogFieldUniOuts.value;
 o_flds.m_fldMAPdot = m_optLogFieldMAPdot.value;
 o_flds.m_fldFts = m_optLogFieldFts.value;
 o_flds.m_fldConsFuel = m_optLogFieldConsFuel.value;
 o_flds.m_fldAFRMap = m_optLogFieldAFRMap.value;
 o_flds.m_fldTchrg = m_optLogFieldTchrg.value;
 o_flds.m_fldGPS = m_optLogFieldGPS.value;
 o_flds.m_fldFPS = m_optLogFieldFPS.value;
}

bool CAppSettingsModel::GetWriteLogFields(void) const
{
 return m_optWriteLogFields.value;
}

void CAppSettingsModel::SetWriteLogFields(bool value)
{
 m_optWriteLogFields.value = value;
}

void CAppSettingsModel::GetFunctionality(Functionality& o_fnc) const
{
 o_fnc.SM_CONTROL = m_optFuncSM_CONTROL.value;
 o_fnc.GD_CONTROL = m_optFuncGD_CONTROL.value;
}

bool CAppSettingsModel::GetShowGraphLabels(void) const
{
 return m_optShowGraphLabels.value;
}

bool CAppSettingsModel::GetShowGraphsCursor(void)
{
 return m_optShowGraphCursor.value;
}

int CAppSettingsModel::GetGraphShtPixels(void)
{
 return m_optGraphShtPixels.value;
}

int CAppSettingsModel::GetGraphShowValue(void)
{
 return m_optGraphShowValue.value;
}

int CAppSettingsModel::GetGraphValueHeight(void)
{
 return m_optGraphValueHeight.value;
}

int CAppSettingsModel::GetToolTipTime(void) const
{
 return m_optToolTipTime.value;
}

int CAppSettingsModel::GetIniEditorSyntax(void) const
{
 return m_optIniEdSyntax.value;
}

void CAppSettingsModel::GetMetAverage(MetAverage& ma)
{
 ma.avrRPM = m_optRPMAverage.value;
 ma.avrVolt = m_optVoltAverage.value;
 ma.avrMAP = m_optMAPAverage.value;
 ma.avrAddI1 = m_optAddI1Average.value;
 ma.avrTPS = m_optTPSAverage.value;
 ma.avrKnock = m_optKnockAverage.value;
 ma.avrIgnTim = m_optIgnTimAverage.value;
 ma.avrCLT = m_optCLTAverage.value;
 ma.avrAddI2 = m_optAddI2Average.value;
 ma.avrInjPW = m_optInjPWAverage.value;
 ma.avrIAT = m_optIATAverage.value;
 ma.avrEGOCorr = m_optEGOCorrAverage.value;
 ma.avrAirFlow = m_optAirFlowAverage.value;
 ma.avrVehicleSpeed = m_optVehicleSpeedAverage.value;
 ma.avrTPSDot = m_optTPSDotAverage.value;
 ma.avrMAPDot = m_optMAPDotAverage.value;
 ma.avrMAP2 = m_optMAP2Average.value;
 ma.avrGPS = m_optGPSAverage.value;
 ma.avrMAPD = m_optMAPDAverage.value;
 ma.avrTmp2 = m_optTmp2Average.value;
 ma.avrFuelConsum = m_optFuelConsumAverage.value;
 ma.avrFuelConsumF = m_optFuelConsumFAverage.value;
 ma.avrKnockRetard = m_optKnockRetardAverage.value;
 ma.avrSensAFR = m_optSensAFRAverage.value;
 ma.avrChokePos = m_optChokePosAverage.value;
 ma.avrGDPos = m_optGDPosAverage.value;
 ma.avrSynLoad = m_optSynLoadAverage.value;
 ma.avrInjTimB = m_optInjTimBAverage.value;
 ma.avrInjTimE = m_optInjTimEAverage.value;
 ma.avrMAF = m_optMAFAverage.value;
 ma.avrTargAFR = m_optTargAFRAverage.value;
 ma.avrGrts = m_optGrtsAverage.value;
 ma.avrFtls = m_optFtlsAverage.value;
 ma.avrEgts = m_optEgtsAverage.value;
 ma.avrOps = m_optOpsAverage.value;
 ma.avrFts = m_optFtsAverage.value;
 ma.avrFPS = m_optFPSAverage.value;
}

bool CAppSettingsModel::GetSpotMarkers(void) const
{
 return m_optSpotMarkers.value;
}

float CAppSettingsModel::GetSpotMarkersSize(void) const
{
 return m_optSpotMarkersSize.value;
}

int CAppSettingsModel::GetBldrEEPROMBlocks(void) const
{
 return m_optBldrEEPROMBlocks.value;
}

float CAppSettingsModel::GetFuelDensity1(void) const
{
 return m_optFuelDensity1.value;
}

float CAppSettingsModel::GetFuelDensity2(void) const
{
 return m_optFuelDensity2.value;
}

bool CAppSettingsModel::GetLogBinaryFmt(void) const
{
 return m_optLogBinaryFmt.value;
}

bool CAppSettingsModel::GetCreateWindows(void) const
{
 return m_optCreateWindows.value;
}

bool CAppSettingsModel::GetDbgVarsToFile(void) const
{
 return m_optDbgVarsToFile.value;
}

bool CAppSettingsModel::GetClassic2DKeys(void) const
{
 return m_optClassic2DKeys.value;
}

bool CAppSettingsModel::GetUseMarkers(void) const
{
 return m_optUseMarkers.value;
}
