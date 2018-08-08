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

#include <limits>
#include <algorithm>

#undef max //avoid conflicts with C++

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
, m_optMIDeskUpdatePeriod(_T("MI_Desk_UpdatePeriod"))
, m_optInterfaceLang(_T("InterfaceLanguage"))
, m_optECUPlatformType(_T("ECUPlatformType"))
, m_optUseDVFeatures(_T("UseDVFeatures"))
, m_optShowToolTips(_T("ShowToolTips"))
, m_optShowExFixtures(_T("ExFixtures"))
, m_optHexDataMode(_T("HexDataMode"))
, m_optDVDeskUpdatePeriod(_T("DVDeskUpdatePeriod"))
, m_optCOMPortBother(_T("COMPortBother"))
, m_optUseHotKeys(_T("UseHotKeys"))
, m_optShowWelcome(_T("ShowWelcome"))
, m_optAllowVisualTheme(_T("AllowVisualTheme"))
, m_optAutoDiagEnter(_T("AutoDiagEnter"))
, m_optSaveWarning(_T("SaveWarning"))
, m_optAutoCERead(_T("AutoCERead"))
, m_optChildCharts(_T("ChildCharts"))
//fixtures
, m_Name_Fixtures_Section("Fixtures")
, m_optTachometerMax(_T("Tachometer_Max"))
, m_optPressureMax(_T("Pressure_Max"))
, m_optPulsesPer1Km(_T("PulsesPer1Km"))
, m_optSpeedUnit(_T("SpeedUnit"))
, m_optRPMAverage(_T("RPMAverage"))
, m_optVoltAverage(_T("VoltAverage"))
, m_optMAPAverage(_T("MAPAverage"))
, m_optAI1Average(_T("AI1Average"))
, m_optTPSAverage(_T("TPSAverage"))
//positions of windows
, m_Name_WndSettings_Section(_T("WndSettings"))
, m_optStrtMapWnd(_T("StrtMapWnd"))
, m_optIdleMapWnd(_T("IdleMapWnd"))
, m_optWorkMapWnd(_T("WorkMapWnd"))
, m_optTempMapWnd(_T("TempMapWnd"))
, m_optAttenMapWnd(_T("AttenMapWnd"))
, m_optMainFrmWnd(_T("MainFrmWnd"))
, m_optDwellCntrlMapWnd(_T("DwellCntrlMapWnd"))
, m_optCTSCurveMapWnd(_T("CTSCurveMapWnd"))
, m_optChokeOpMapWnd(_T("ChokeOpMapWnd"))
, m_optGridMapIgnWnd(_T("GridMapIgnWnd"))
, m_optGridMapInjWnd(_T("GridMapInjWnd"))
, m_optVEMapWnd(_T("VEMapWnd"))
, m_optAFRMapWnd(_T("AFRMapWnd"))
, m_optCrnkMapWnd(_T("CrnkMapWnd"))
, m_optWrmpMapWnd(_T("WrmpMapWnd"))
, m_optDeadMapWnd(_T("DeadMapWnd"))
, m_optIdlrMapWnd(_T("IdlrMapWnd"))
, m_optIdlcMapWnd(_T("IdlcMapWnd"))
, m_optATSCurvMapWnd(_T("ATSCurvMapWnd"))
, m_optATSCorrMapWnd(_T("ATSCorrMapWnd"))
, m_optAETPSMapWnd(_T("AETPSMapWnd"))
, m_optAERPMMapWnd(_T("AERPMMapWnd"))
, m_optAftstrMapWnd(_T("AftstrMapWnd"))
, m_optGasdoseMapWnd(_T("GasdoseMapWnd"))
, m_optITMapWnd(_T("ITMapWnd"))
, m_optITRPMMapWnd(_T("ITRPMMapWnd"))
, m_optRigidMapWnd(_T("RigidMapWnd"))
, m_optEGOCrvMapWnd(_T("EGOCrvMapWnd"))
, m_optIACCMapWnd(_T("IACCMapWnd"))
, m_optIACCWMapWnd(_T("IACCWMapWnd"))
, m_optIATCLTMapWnd(_T("IATCLTCorrMapWnd"))
, m_optBarocorrMapWnd(_T("BarocorrMapWnd"))
, m_optManIgntimMapWnd(_T("ManIgntimMapWnd"))
, m_optCESettingsWnd(_T("CESettingsWnd"))
, m_optTpsswtMapWnd(_T("TpsSwtMapWnd"))
, m_optTmp2CurveMapWnd(_T("Tmp2CurveMapWnd"))
, m_optGtscMapWnd(_T("GTSCMapWnd"))
, m_optGpscMapWnd(_T("GPSCMapWnd"))
, m_optAtscMapWnd(_T("AirDenMapWnd"))
, m_optCrkTempMapWnd(_T("CrkTempMapWnd"))
, m_optEHPauseMapWnd(_T("EHPauseMapWnd"))
//size of windows
, m_Name_WndSize_Section(_T("WndSize"))
, m_optMainFrmWndSize(_T("MainFrmWnd"))
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
 }

 m_Name_Meters_Section[0] = _T("Meters");
 m_Name_Meters_Section[1] = _T("MetersEx");

 for(int i = 0; i < 2; ++i)
 {
  m_optMetRows[i].name = _T("MetRows");
  m_optMetRPM[i].name = _T("MetRPM");
  m_optMetMAP[i].name = _T("MetMAP");
  m_optMetVBat[i].name = _T("MetVBat");
  m_optMetIgnTim[i].name = _T("MetIgnTim");
  m_optMetCLT[i].name = _T("MetCLT");
  m_optMetAddI1[i].name = _T("MetAddI1");
  m_optMetAddI2[i].name = _T("MetAddI2");
  m_optMetInjPW[i].name = _T("MetInjPW");
  m_optMetIAT[i].name = _T("MetIAT");
  m_optMetEGOCorr[i].name = _T("MetEGOCorr");
  m_optMetTPS[i].name = _T("MetTPS");
  m_optMetAirFlow[i].name = _T("MetAirFlow");
  m_optMetVehicleSpeed[i].name = _T("MetVehicleSpeed");
  m_optMetTPSDot[i].name = _T("MetTPSDot");
  m_optMetMAP2[i].name = _T("MetMAP2");
  m_optMetMAPD[i].name = _T("MetMAPD");
  m_optMetTmp2[i].name = _T("MetTmp2");
  m_optMetFuelConsum[i].name = _T("MetFuelConsum");
  m_optMetKnockRetard[i].name = _T("MetKnockRetard");
  m_optMetKnockGraph[i].name = _T("MetKnockGraph");
  m_optMetSensAFR[i].name = _T("MetSensAFR");
  m_optMetChokePos[i].name = _T("MetIACPos");
  m_optMetGDPos[i].name = _T("MetGDPos");
  m_optMetSynLoad[i].name = _T("MetSynLoad");
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

 _tcscpy(m_current_directory,_T(""));

 //get current directory name
 GetCurrentDirectory(MAX_PATH, m_current_directory);

 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\",\"  comma")),     ','));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\";\"  semicolon")), ';'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\":\"  colon")),     ':'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"+\"  plus")),      '+'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"-\"  minus")),     '-'));

 m_AllowableLanguages.push_back(std::make_pair(std::make_pair(_TSTRING(_T("English")), _TSTRING(_T("english"))), IL_ENGLISH) );
 m_AllowableLanguages.push_back(std::make_pair(std::make_pair(_TSTRING(_T("Russian")), _TSTRING(_T("russian"))), IL_RUSSIAN) );

 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega64")), _TSTRING(_T("atmega64"))), EP_ATMEGA64) );
 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega644")), _TSTRING(_T("atmega644"))), EP_ATMEGA644) );
 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega128")), _TSTRING(_T("atmega128"))), EP_ATMEGA128) );
 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega1284")), _TSTRING(_T("atmega1284"))), EP_ATMEGA1284) );

 m_AllowableSpeedUnits.push_back(std::make_pair(std::make_pair(_TSTRING(_T("km/h")), _TSTRING(_T("kmh"))), SU_KMH));
 m_AllowableSpeedUnits.push_back(std::make_pair(std::make_pair(_TSTRING(_T("m/h")), _TSTRING(_T("mph"))), SU_MPH));
}

CAppSettingsModel::~CAppSettingsModel()
{
 //empty
}

CString CAppSettingsModel::GetINIFileFullName(void) const
{
 CString directory(m_current_directory);
 if (directory.IsEmpty())
  return _T("");

 CString last_char = directory.Right(1);
 if (last_char != _T("\\")) //если корневой каталог, то '\' уже есть
  directory+=_T("\\");

 return directory + AfxGetApp()->m_pszExeName + _T(".ini");
}

CString CAppSettingsModel::GetAppDirectory(void) const
{
 return m_current_directory;
}

bool CAppSettingsModel::ReadSettings(void)
{
 CString IniFileName = GetINIFileFullName();
 bool status = true;

 //Options
 IniIO os(IniFileName, m_Name_Options_Section);

 os.ReadString(m_optPortName, _T("COM1"));
 os.ReadDword(m_optBaudRateApplication, _T("57600"), m_AllowableBaudRates);
 os.ReadDword(m_optBaudRateBootloader, _T("57600"), m_AllowableBaudRates);
 os.ReadString(m_optLogFilesFolder, m_current_directory);
 os.ReadInt(m_optCSVSepSymbol, _T("44"), m_AllowaleCSVSepSymbols);
 os.ReadInt(m_optUseAppFolder, _T("1"), 0, 1);
 os.ReadInt(m_optAlwaysWriteLog, _T("1"), 0, 1);
 os.ReadInt(m_optMIDeskUpdatePeriod, _T("40"), 0, 1000); 
 os.ReadEnum(m_optInterfaceLang, 0, m_AllowableLanguages);
 os.ReadEnum(m_optECUPlatformType, 1, m_AllowablePlatforms); //644
 os.ReadInt(m_optDVDeskUpdatePeriod, _T("40"), 0, 1000);
 os.ReadInt(m_optUseDVFeatures, _T("0"), 0, 1);
 os.ReadInt(m_optShowToolTips, _T("1"), 0, 1);
 os.ReadInt(m_optHexDataMode, _T("0"), 0, 1);
 os.ReadInt(m_optCOMPortBother, _T("1"), 0, 1);
 os.ReadInt(m_optUseHotKeys, _T("1"), 0, 1);
 os.ReadInt(m_optShowWelcome, _T("1"), 0, 1);
 os.ReadInt(m_optAllowVisualTheme, _T("1"), 0, 1);
 os.ReadInt(m_optAutoDiagEnter, _T("0"), 0, 1);
 os.ReadInt(m_optSaveWarning, _T("1"), 0, 1);
 os.ReadInt(m_optAutoCERead, _T("0"), 0, 1);
 os.ReadInt(m_optChildCharts, _T("1"), 0, 1);

 //fixtures
 IniIO fs(IniFileName, m_Name_Fixtures_Section);

 fs.ReadInt(m_optShowExFixtures, _T("1"), 0, 1);
 fs.ReadInt(m_optRPMAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optVoltAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optMAPAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optAI1Average, _T("4"), 0, 16);
 fs.ReadInt(m_optTPSAverage, _T("4"), 0, 16);
 fs.ReadInt(m_optTachometerMax, _T("8000"), 0, 15000);
 fs.ReadInt(m_optPressureMax, _T("110"), 0, 500);
 fs.ReadInt(m_optPulsesPer1Km, _T("6000"), 0, 60000);
 fs.ReadEnum(m_optSpeedUnit, 0, m_AllowableSpeedUnits);

 //Positions of windows
 IniIO ws(IniFileName, m_Name_WndSettings_Section);

 ws.ReadWndPos(m_optStrtMapWnd);
 ws.ReadWndPos(m_optIdleMapWnd);
 ws.ReadWndPos(m_optWorkMapWnd);
 ws.ReadWndPos(m_optTempMapWnd);
 ws.ReadWndPos(m_optAttenMapWnd); 
 ws.ReadWndPos(m_optMainFrmWnd); //Main frame window
 ws.ReadWndPos(m_optDwellCntrlMapWnd);
 ws.ReadWndPos(m_optCTSCurveMapWnd);
 ws.ReadWndPos(m_optChokeOpMapWnd);
 ws.ReadWndPos(m_optGridMapIgnWnd);
 ws.ReadWndPos(m_optGridMapInjWnd);
 ws.ReadWndPos(m_optVEMapWnd);
 ws.ReadWndPos(m_optAFRMapWnd);
 ws.ReadWndPos(m_optCrnkMapWnd);
 ws.ReadWndPos(m_optWrmpMapWnd);
 ws.ReadWndPos(m_optDeadMapWnd);
 ws.ReadWndPos(m_optIdlrMapWnd);
 ws.ReadWndPos(m_optIdlcMapWnd);
 ws.ReadWndPos(m_optAETPSMapWnd);
 ws.ReadWndPos(m_optAERPMMapWnd);
 ws.ReadWndPos(m_optAftstrMapWnd);
 ws.ReadWndPos(m_optATSCurvMapWnd);
 ws.ReadWndPos(m_optATSCorrMapWnd);
 ws.ReadWndPos(m_optGasdoseMapWnd);
 ws.ReadWndPos(m_optITMapWnd);
 ws.ReadWndPos(m_optITRPMMapWnd);
 ws.ReadWndPos(m_optRigidMapWnd);
 ws.ReadWndPos(m_optEGOCrvMapWnd);
 ws.ReadWndPos(m_optIACCMapWnd);
 ws.ReadWndPos(m_optIACCWMapWnd);
 ws.ReadWndPos(m_optIATCLTMapWnd);
 ws.ReadWndPos(m_optBarocorrMapWnd);
 ws.ReadWndPos(m_optManIgntimMapWnd);
 ws.ReadWndPos(m_optCESettingsWnd);
 ws.ReadWndPos(m_optTpsswtMapWnd);
 ws.ReadWndPos(m_optTmp2CurveMapWnd);
 ws.ReadWndPos(m_optGtscMapWnd);
 ws.ReadWndPos(m_optGpscMapWnd);
 ws.ReadWndPos(m_optAtscMapWnd);
 ws.ReadWndPos(m_optCrkTempMapWnd);
 ws.ReadWndPos(m_optEHPauseMapWnd);

 //Sizes of windows
 IniIO sz(IniFileName, m_Name_WndSize_Section);
 sz.ReadWndPos(m_optMainFrmWndSize, 0, 10000); //Main frame window

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

 //Meters
 for(int i = 0; i < 2; ++i)
 {
  IniIO mm(IniFileName, m_Name_Meters_Section[i]);
  mm.ReadInt(m_optMetRows[i],_T("2"), 1, 10);
  mm.ReadInt(m_optMetRPM[i],_T("0"), 0, 32, true);
  mm.ReadInt(m_optMetMAP[i],_T("1"), 0, 32, true);
  mm.ReadInt(m_optMetVBat[i],_T("2"), 0, 32, true);
  mm.ReadInt(m_optMetIgnTim[i],_T("3"), 0, 32, true);
  mm.ReadInt(m_optMetTPS[i],_T("4"), 0, 32, true);
  mm.ReadInt(m_optMetCLT[i],_T("5"), 0, 32, true);
  mm.ReadInt(m_optMetAddI1[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetAddI2[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetInjPW[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetIAT[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetEGOCorr[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetAirFlow[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetVehicleSpeed[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetTPSDot[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetMAP2[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetMAPD[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetTmp2[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetFuelConsum[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetKnockRetard[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetKnockGraph[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetSensAFR[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetChokePos[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetGDPos[i],_T(""), 0, 32, true);
  mm.ReadInt(m_optMetSynLoad[i],_T(""), 0, 32, true);
 }

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

 return status;
}

bool CAppSettingsModel::WriteSettings(void)
{
 CString IniFileName = GetINIFileFullName();
 bool status = true;

 IniIO os(IniFileName, m_Name_Options_Section);
 os.CreateSection(); //create section

 os.WriteString(m_optPortName);
 os.WriteDword(m_optBaudRateApplication);
 os.WriteDword(m_optBaudRateBootloader);
 os.WriteString(m_optLogFilesFolder);
 os.WriteInt(m_optUseAppFolder);
 os.WriteInt(m_optAlwaysWriteLog);
 os.WriteInt(m_optCSVSepSymbol);
 os.WriteInt(m_optMIDeskUpdatePeriod);
 os.WriteEnum(m_optInterfaceLang, m_AllowableLanguages);
 os.WriteEnum(m_optECUPlatformType, m_AllowablePlatforms);
 os.WriteInt(m_optUseDVFeatures);
 os.WriteInt(m_optDVDeskUpdatePeriod);
 os.WriteInt(m_optShowToolTips); 
 os.WriteInt(m_optHexDataMode); 
 os.WriteInt(m_optCOMPortBother); 
 os.WriteInt(m_optUseHotKeys); 
 m_optShowWelcome.value = 0; //<--Will be not shown next time
 os.WriteInt(m_optShowWelcome); 
 os.WriteInt(m_optAllowVisualTheme); 
 os.WriteInt(m_optAutoDiagEnter); 
 os.WriteInt(m_optSaveWarning); 
 os.WriteInt(m_optAutoCERead); 
 os.WriteInt(m_optChildCharts); 

 IniIO fs(IniFileName, m_Name_Fixtures_Section);
 fs.CreateSection();

 fs.WriteInt(m_optShowExFixtures); 
 fs.WriteInt(m_optRPMAverage); 
 fs.WriteInt(m_optVoltAverage); 
 fs.WriteInt(m_optMAPAverage); 
 fs.WriteInt(m_optAI1Average); 
 fs.WriteInt(m_optTPSAverage); 
 fs.WriteInt(m_optTachometerMax); 
 fs.WriteInt(m_optPressureMax); 
 fs.WriteInt(m_optPulsesPer1Km); 
 fs.WriteEnum(m_optSpeedUnit, m_AllowableSpeedUnits);

 //Positions of windows
 IniIO ws(IniFileName, m_Name_WndSettings_Section);
 ws.CreateSection();

 ws.WriteWndPos(m_optStrtMapWnd);
 ws.WriteWndPos(m_optIdleMapWnd);
 ws.WriteWndPos(m_optWorkMapWnd);
 ws.WriteWndPos(m_optTempMapWnd);
 ws.WriteWndPos(m_optAttenMapWnd);
 ws.WriteWndPos(m_optMainFrmWnd);
 ws.WriteWndPos(m_optDwellCntrlMapWnd);
 ws.WriteWndPos(m_optCTSCurveMapWnd);
 ws.WriteWndPos(m_optChokeOpMapWnd);
 ws.WriteWndPos(m_optGridMapIgnWnd);
 ws.WriteWndPos(m_optGridMapInjWnd);
 ws.WriteWndPos(m_optVEMapWnd);
 ws.WriteWndPos(m_optAFRMapWnd);
 ws.WriteWndPos(m_optCrnkMapWnd);
 ws.WriteWndPos(m_optWrmpMapWnd);
 ws.WriteWndPos(m_optDeadMapWnd);
 ws.WriteWndPos(m_optIdlrMapWnd);
 ws.WriteWndPos(m_optIdlcMapWnd);
 ws.WriteWndPos(m_optAETPSMapWnd);
 ws.WriteWndPos(m_optAERPMMapWnd);
 ws.WriteWndPos(m_optAftstrMapWnd);
 ws.WriteWndPos(m_optATSCurvMapWnd);
 ws.WriteWndPos(m_optATSCorrMapWnd);
 ws.WriteWndPos(m_optGasdoseMapWnd);
 ws.WriteWndPos(m_optITMapWnd);
 ws.WriteWndPos(m_optITRPMMapWnd);
 ws.WriteWndPos(m_optRigidMapWnd);
 ws.WriteWndPos(m_optEGOCrvMapWnd);
 ws.WriteWndPos(m_optIACCMapWnd);
 ws.WriteWndPos(m_optIACCWMapWnd);
 ws.WriteWndPos(m_optIATCLTMapWnd);
 ws.WriteWndPos(m_optBarocorrMapWnd);
 ws.WriteWndPos(m_optManIgntimMapWnd);
 ws.WriteWndPos(m_optCESettingsWnd);
 ws.WriteWndPos(m_optTpsswtMapWnd);
 ws.WriteWndPos(m_optTmp2CurveMapWnd);
 ws.WriteWndPos(m_optGtscMapWnd);
 ws.WriteWndPos(m_optGpscMapWnd);
 ws.WriteWndPos(m_optAtscMapWnd);
 ws.WriteWndPos(m_optCrkTempMapWnd);
 ws.WriteWndPos(m_optEHPauseMapWnd);

 //Sizes of windows
 IniIO sz(IniFileName, m_Name_WndSize_Section);
 sz.CreateSection();

 sz.WriteWndPos(m_optMainFrmWndSize);

 //Indicators
 for(int i = 0; i < 2; ++i)
 {
  IniIO ii(IniFileName, m_Name_Indicators_Section[i]);
  ii.CreateSection();
  ii.WriteFlt(m_optIndHeightPercent[i], 2);
  ii.WriteInt(m_optIndRows[i]);
  ii.WriteInt(m_optIndGas_v[i]);
  ii.WriteInt(m_optIndCarb[i]);
  ii.WriteInt(m_optIndIdleValve[i]);
  ii.WriteInt(m_optIndPowerValve[i]);
  ii.WriteInt(m_optIndStBlock[i]);
  ii.WriteInt(m_optIndAE[i]);
  ii.WriteInt(m_optIndCoolingFan[i]);
  ii.WriteInt(m_optIndCE[i]);
  ii.WriteInt(m_optIndFCRevLim[i]);
  ii.WriteInt(m_optIndFloodClear[i]);
  ii.WriteInt(m_optIndSysLocked[i]);
 }

 IniIO ic(IniFileName, m_Name_IndColors_Section);
 ic.WriteColor(m_optColGas_v);
 ic.WriteColor(m_optColCarb);
 ic.WriteColor(m_optColIdleValve);
 ic.WriteColor(m_optColPowerValve);
 ic.WriteColor(m_optColStBlock);
 ic.WriteColor(m_optColAE);
 ic.WriteColor(m_optColCoolingFan);
 ic.WriteColor(m_optColCE);
 ic.WriteColor(m_optColFCRevLim);
 ic.WriteColor(m_optColFloodClear);
 ic.WriteColor(m_optColSysLocked);

 //Meters
 for(int i = 0; i < 2; ++i)
 {
  IniIO mm(IniFileName, m_Name_Meters_Section[i]);
  mm.CreateSection();
  mm.WriteInt(m_optMetRows[i]);
  mm.WriteInt(m_optMetRPM[i]);
  mm.WriteInt(m_optMetMAP[i]);
  mm.WriteInt(m_optMetVBat[i]);
  mm.WriteInt(m_optMetIgnTim[i]);
  mm.WriteInt(m_optMetTPS[i]);
  mm.WriteInt(m_optMetCLT[i]);
  mm.WriteInt(m_optMetAddI1[i]);
  mm.WriteInt(m_optMetAddI2[i]);
  mm.WriteInt(m_optMetInjPW[i]);
  mm.WriteInt(m_optMetIAT[i]);
  mm.WriteInt(m_optMetEGOCorr[i]);
  mm.WriteInt(m_optMetAirFlow[i]);
  mm.WriteInt(m_optMetVehicleSpeed[i]);
  mm.WriteInt(m_optMetTPSDot[i]);
  mm.WriteInt(m_optMetMAP2[i]);
  mm.WriteInt(m_optMetMAPD[i]);
  mm.WriteInt(m_optMetTmp2[i]);
  mm.WriteInt(m_optMetFuelConsum[i]);
  mm.WriteInt(m_optMetKnockRetard[i]);
  mm.WriteInt(m_optMetKnockGraph[i]);
  mm.WriteInt(m_optMetSensAFR[i]);
  mm.WriteInt(m_optMetChokePos[i]);
  mm.WriteInt(m_optMetGDPos[i]);
  mm.WriteInt(m_optMetSynLoad[i]);
 }

 IniIO at(IniFileName, m_Name_AutoTune_Section);
 at.WriteVector(m_optLambdaDelay);
 at.WriteVector(m_optLambdaDelayL);
 at.WriteVector(m_optLambdaDelayR);
 at.WriteFlt(m_optAFRError, 2);
 at.WriteVector(m_optBlockedCells);
 at.WriteInt(m_optStatSize);
 at.WriteInt(m_optAutoBlockThrd);
 at.WriteInt(m_optGrowingMode);
 at.WriteFlt(m_optMinAFR, 1);
 at.WriteFlt(m_optMaxAFR, 1);
 at.WriteInt(m_optMinDistThrd);

 return status;
}

const _TSTRING& CAppSettingsModel::GetPortName(void) const
{
 return m_optPortName.value;
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

int  CAppSettingsModel::GetMIDeskUpdatePeriod(void) const
{
 return m_optMIDeskUpdatePeriod.value;
}

void CAppSettingsModel::SetWndSettings(const WndSettings& i_wndSettings)
{
 m_optStrtMapWnd.value.x = i_wndSettings.m_StrtMapWnd_X;
 m_optStrtMapWnd.value.y = i_wndSettings.m_StrtMapWnd_Y;
 m_optIdleMapWnd.value.x = i_wndSettings.m_IdleMapWnd_X;
 m_optIdleMapWnd.value.y = i_wndSettings.m_IdleMapWnd_Y;
 m_optWorkMapWnd.value.x = i_wndSettings.m_WorkMapWnd_X;
 m_optWorkMapWnd.value.y = i_wndSettings.m_WorkMapWnd_Y;
 m_optTempMapWnd.value.x = i_wndSettings.m_TempMapWnd_X;
 m_optTempMapWnd.value.y = i_wndSettings.m_TempMapWnd_Y;
 m_optAttenMapWnd.value.x = i_wndSettings.m_AttenuatorMapWnd_X;
 m_optAttenMapWnd.value.y = i_wndSettings.m_AttenuatorMapWnd_Y;
 m_optMainFrmWnd.value.x = i_wndSettings.m_MainFrmWnd_X;
 m_optMainFrmWnd.value.y = i_wndSettings.m_MainFrmWnd_Y;
 m_optDwellCntrlMapWnd.value.x = i_wndSettings.m_DwellCntrlMapWnd_X;
 m_optDwellCntrlMapWnd.value.y = i_wndSettings.m_DwellCntrlMapWnd_Y;
 m_optCTSCurveMapWnd.value.x = i_wndSettings.m_CTSCurveMapWnd_X;
 m_optCTSCurveMapWnd.value.y = i_wndSettings.m_CTSCurveMapWnd_Y;
 m_optChokeOpMapWnd.value.x = i_wndSettings.m_ChokeOpMapWnd_X;
 m_optChokeOpMapWnd.value.y = i_wndSettings.m_ChokeOpMapWnd_Y;
 m_optGridMapIgnWnd.value.x = i_wndSettings.m_GridMapIgnWnd_X;
 m_optGridMapIgnWnd.value.y = i_wndSettings.m_GridMapIgnWnd_Y;
 m_optGridMapInjWnd.value.x = i_wndSettings.m_GridMapInjWnd_X;
 m_optGridMapInjWnd.value.y = i_wndSettings.m_GridMapInjWnd_Y;
 m_optVEMapWnd.value.x = i_wndSettings.m_VEMapWnd_X;
 m_optVEMapWnd.value.y = i_wndSettings.m_VEMapWnd_Y;
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
 m_optAETPSMapWnd.value.x = i_wndSettings.m_AETPSMapWnd_X;
 m_optAETPSMapWnd.value.y = i_wndSettings.m_AETPSMapWnd_Y; 
 m_optAERPMMapWnd.value.x = i_wndSettings.m_AERPMMapWnd_X;
 m_optAERPMMapWnd.value.y = i_wndSettings.m_AERPMMapWnd_Y; 
 m_optAftstrMapWnd.value.x = i_wndSettings.m_AftstrMapWnd_X;
 m_optAftstrMapWnd.value.y = i_wndSettings.m_AftstrMapWnd_Y; 
 m_optATSCurvMapWnd.value.x = i_wndSettings.m_ATSCurvMapWnd_X;
 m_optATSCurvMapWnd.value.y = i_wndSettings.m_ATSCurvMapWnd_Y;
 m_optATSCorrMapWnd.value.x = i_wndSettings.m_ATSCorrMapWnd_X;
 m_optATSCorrMapWnd.value.y = i_wndSettings.m_ATSCorrMapWnd_Y; 
 m_optGasdoseMapWnd.value.x = i_wndSettings.m_GasdoseMapWnd_X;
 m_optGasdoseMapWnd.value.y = i_wndSettings.m_GasdoseMapWnd_Y; 
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
 m_optBarocorrMapWnd.value.x = i_wndSettings.m_BarocorrMapWnd_X;
 m_optBarocorrMapWnd.value.y = i_wndSettings.m_BarocorrMapWnd_Y; 
 m_optManIgntimMapWnd.value.x = i_wndSettings.m_ManIgntimMapWnd_X;
 m_optManIgntimMapWnd.value.y = i_wndSettings.m_ManIgntimMapWnd_Y; 
 m_optCESettingsWnd.value.x = i_wndSettings.m_CESettingsWnd_X;
 m_optCESettingsWnd.value.y = i_wndSettings.m_CESettingsWnd_Y; 
 m_optTpsswtMapWnd.value.x = i_wndSettings.m_TpsswtMapWnd_X;
 m_optTpsswtMapWnd.value.y = i_wndSettings.m_TpsswtMapWnd_Y;
 m_optTmp2CurveMapWnd.value.x = i_wndSettings.m_Tmp2CurveMapWnd_X;
 m_optTmp2CurveMapWnd.value.y = i_wndSettings.m_Tmp2CurveMapWnd_Y; 
 m_optGtscMapWnd.value.x = i_wndSettings.m_GtscMapWnd_X;
 m_optGtscMapWnd.value.y = i_wndSettings.m_GtscMapWnd_Y;
 m_optGpscMapWnd.value.x = i_wndSettings.m_GpscMapWnd_X;
 m_optGpscMapWnd.value.y = i_wndSettings.m_GpscMapWnd_Y;
 m_optAtscMapWnd.value.x = i_wndSettings.m_AtscMapWnd_X;
 m_optAtscMapWnd.value.y = i_wndSettings.m_AtscMapWnd_Y;
 m_optCrkTempMapWnd.value.x = i_wndSettings.m_CrkTempMapWnd_X;
 m_optCrkTempMapWnd.value.y = i_wndSettings.m_CrkTempMapWnd_Y;
 m_optEHPauseMapWnd.value.x = i_wndSettings.m_EHPauseMapWnd_X;
 m_optEHPauseMapWnd.value.y = i_wndSettings.m_EHPauseMapWnd_Y;
}

void CAppSettingsModel::GetWndSettings(WndSettings& o_wndSettings) const
{
 o_wndSettings.m_StrtMapWnd_X = m_optStrtMapWnd.value.x;
 o_wndSettings.m_StrtMapWnd_Y = m_optStrtMapWnd.value.y;
 o_wndSettings.m_IdleMapWnd_X = m_optIdleMapWnd.value.x;
 o_wndSettings.m_IdleMapWnd_Y = m_optIdleMapWnd.value.y;
 o_wndSettings.m_WorkMapWnd_X = m_optWorkMapWnd.value.x;
 o_wndSettings.m_WorkMapWnd_Y = m_optWorkMapWnd.value.y;
 o_wndSettings.m_TempMapWnd_X = m_optTempMapWnd.value.x;
 o_wndSettings.m_TempMapWnd_Y = m_optTempMapWnd.value.y;
 o_wndSettings.m_AttenuatorMapWnd_X = m_optAttenMapWnd.value.x;
 o_wndSettings.m_AttenuatorMapWnd_Y = m_optAttenMapWnd.value.y;
 o_wndSettings.m_MainFrmWnd_X = m_optMainFrmWnd.value.x;
 o_wndSettings.m_MainFrmWnd_Y = m_optMainFrmWnd.value.y;
 o_wndSettings.m_DwellCntrlMapWnd_X = m_optDwellCntrlMapWnd.value.x;
 o_wndSettings.m_DwellCntrlMapWnd_Y = m_optDwellCntrlMapWnd.value.y;
 o_wndSettings.m_CTSCurveMapWnd_X = m_optCTSCurveMapWnd.value.x;
 o_wndSettings.m_CTSCurveMapWnd_Y = m_optCTSCurveMapWnd.value.y;
 o_wndSettings.m_ChokeOpMapWnd_X = m_optChokeOpMapWnd.value.x;
 o_wndSettings.m_ChokeOpMapWnd_Y = m_optChokeOpMapWnd.value.y;
 o_wndSettings.m_GridMapIgnWnd_X = m_optGridMapIgnWnd.value.x;
 o_wndSettings.m_GridMapIgnWnd_Y = m_optGridMapIgnWnd.value.y;
 o_wndSettings.m_GridMapInjWnd_X = m_optGridMapInjWnd.value.x;
 o_wndSettings.m_GridMapInjWnd_Y = m_optGridMapInjWnd.value.y;
 o_wndSettings.m_VEMapWnd_X = m_optVEMapWnd.value.x;
 o_wndSettings.m_VEMapWnd_Y = m_optVEMapWnd.value.y;
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
 o_wndSettings.m_AETPSMapWnd_X = m_optAETPSMapWnd.value.x;
 o_wndSettings.m_AETPSMapWnd_Y = m_optAETPSMapWnd.value.y;
 o_wndSettings.m_AERPMMapWnd_X = m_optAERPMMapWnd.value.x;
 o_wndSettings.m_AERPMMapWnd_Y = m_optAERPMMapWnd.value.y;
 o_wndSettings.m_AftstrMapWnd_X = m_optAftstrMapWnd.value.x;
 o_wndSettings.m_AftstrMapWnd_Y = m_optAftstrMapWnd.value.y;
 o_wndSettings.m_ATSCurvMapWnd_X = m_optATSCurvMapWnd.value.x;
 o_wndSettings.m_ATSCurvMapWnd_Y = m_optATSCurvMapWnd.value.y;
 o_wndSettings.m_ATSCorrMapWnd_X = m_optATSCorrMapWnd.value.x;
 o_wndSettings.m_ATSCorrMapWnd_Y = m_optATSCorrMapWnd.value.y;
 o_wndSettings.m_GasdoseMapWnd_X = m_optGasdoseMapWnd.value.x;
 o_wndSettings.m_GasdoseMapWnd_Y = m_optGasdoseMapWnd.value.y;
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
 o_wndSettings.m_BarocorrMapWnd_X = m_optBarocorrMapWnd.value.x;
 o_wndSettings.m_BarocorrMapWnd_Y = m_optBarocorrMapWnd.value.y;
 o_wndSettings.m_ManIgntimMapWnd_X = m_optManIgntimMapWnd.value.x;
 o_wndSettings.m_ManIgntimMapWnd_Y = m_optManIgntimMapWnd.value.y;
 o_wndSettings.m_CESettingsWnd_X = m_optCESettingsWnd.value.x;
 o_wndSettings.m_CESettingsWnd_Y = m_optCESettingsWnd.value.y;
 o_wndSettings.m_TpsswtMapWnd_X = m_optTpsswtMapWnd.value.x;
 o_wndSettings.m_TpsswtMapWnd_Y = m_optTpsswtMapWnd.value.y;
 o_wndSettings.m_Tmp2CurveMapWnd_X = m_optTmp2CurveMapWnd.value.x;
 o_wndSettings.m_Tmp2CurveMapWnd_Y = m_optTmp2CurveMapWnd.value.y;
 o_wndSettings.m_GtscMapWnd_X = m_optGtscMapWnd.value.x;
 o_wndSettings.m_GtscMapWnd_Y = m_optGtscMapWnd.value.y;
 o_wndSettings.m_GpscMapWnd_X = m_optGpscMapWnd.value.x;
 o_wndSettings.m_GpscMapWnd_Y = m_optGpscMapWnd.value.y;
 o_wndSettings.m_AtscMapWnd_X = m_optAtscMapWnd.value.x;
 o_wndSettings.m_AtscMapWnd_Y = m_optAtscMapWnd.value.y;
 o_wndSettings.m_CrkTempMapWnd_X = m_optCrkTempMapWnd.value.x;
 o_wndSettings.m_CrkTempMapWnd_Y = m_optCrkTempMapWnd.value.y;
 o_wndSettings.m_EHPauseMapWnd_X = m_optEHPauseMapWnd.value.x;
 o_wndSettings.m_EHPauseMapWnd_Y = m_optEHPauseMapWnd.value.y;
}

void CAppSettingsModel::SetWndSize(const WndSize& i_wndSize)
{
 m_optMainFrmWndSize.value.x = i_wndSize.m_MainFrmWnd_W;
 m_optMainFrmWndSize.value.y = i_wndSize.m_MainFrmWnd_H;
}

void CAppSettingsModel::GetWndSize(WndSize& o_wndSize) const
{
 o_wndSize.m_MainFrmWnd_W = m_optMainFrmWndSize.value.x;
 o_wndSize.m_MainFrmWnd_H = m_optMainFrmWndSize.value.y;
}

EInterLang CAppSettingsModel::GetInterfaceLanguage(void) const
{
 return (EInterLang)m_optInterfaceLang.value;
}

EECUPlatform CAppSettingsModel::GetECUPlatformType(void) const
{
 return (EECUPlatform)m_optECUPlatformType.value;
}

int CAppSettingsModel::GetTachometerMax(void) const
{
 return m_optTachometerMax.value;
}

int CAppSettingsModel::GetPressureMax(void) const
{
 return m_optPressureMax.value;
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

bool CAppSettingsModel::GetHexDataMode(void) const
{
 return m_optHexDataMode.value;
}

int CAppSettingsModel::GetNumPulsesPer1Km(void) const
{
 return m_optPulsesPer1Km.value;
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

int CAppSettingsModel::GetRPMAverage(void) const
{
 return m_optRPMAverage.value;
}

int CAppSettingsModel::GetVoltAverage(void) const
{
 return m_optVoltAverage.value;
}

int CAppSettingsModel::GetMAPAverage(void) const
{
 return m_optMAPAverage.value;
}

int CAppSettingsModel::GetAI1Average(void) const
{
 return m_optAI1Average.value;
}

int CAppSettingsModel::GetTPSAverage(void) const
{
 return m_optTPSAverage.value;
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
 }
}

void CAppSettingsModel::GetMetersConfig(MetersCfg& o_cfg) const
{
 for(int i = 0; i < 2; ++i)
 {
  o_cfg.m_optMetRows[i] = m_optMetRows[i].value; 
  o_cfg.m_optMetRPM[i] = m_optMetRPM[i].value;
  o_cfg.m_optMetMAP[i] = m_optMetMAP[i].value;
  o_cfg.m_optMetVBat[i] = m_optMetVBat[i].value;
  o_cfg.m_optMetIgnTim[i] = m_optMetIgnTim[i].value;
  o_cfg.m_optMetCLT[i] = m_optMetCLT[i].value;
  o_cfg.m_optMetAddI1[i] = m_optMetAddI1[i].value;
  o_cfg.m_optMetAddI2[i] = m_optMetAddI2[i].value;
  o_cfg.m_optInjPW[i] = m_optMetInjPW[i].value;
  o_cfg.m_optMetIAT[i] = m_optMetIAT[i].value;
  o_cfg.m_optMetEGOCorr[i] = m_optMetEGOCorr[i].value;
  o_cfg.m_optMetTPS[i] = m_optMetTPS[i].value;
  o_cfg.m_optMetAirFlow[i] = m_optMetAirFlow[i].value;
  o_cfg.m_optMetVehicleSpeed[i] = m_optMetVehicleSpeed[i].value;
  o_cfg.m_optMetTPSDot[i] = m_optMetTPSDot[i].value;
  o_cfg.m_optMetMAP2[i] = m_optMetMAP2[i].value;
  o_cfg.m_optMetMAPD[i] = m_optMetMAPD[i].value;
  o_cfg.m_optMetTmp2[i] = m_optMetTmp2[i].value;
  o_cfg.m_optMetFuelConsum[i] = m_optMetFuelConsum[i].value;
  o_cfg.m_optMetKnockRetard[i] = m_optMetKnockRetard[i].value;
  o_cfg.m_optMetKnockGraph[i] = m_optMetKnockGraph[i].value;
  o_cfg.m_optMetSensAFR[i] = m_optMetSensAFR[i].value;
  o_cfg.m_optMetChokePos[i] = m_optMetChokePos[i].value;
  o_cfg.m_optMetGDPos[i] = m_optMetGDPos[i].value;
  o_cfg.m_optMetSynLoad[i] = m_optMetSynLoad[i].value;
 }
}

void CAppSettingsModel::SetLamDelMap(float* map, float* rb, float* lb)
{
 for(size_t i = 0; i < m_optLambdaDelay.value.size(); ++i)
  m_optLambdaDelay.value[i] = MathHelpers::Round(map[i]); 
 for(size_t i = 0; i < m_optLambdaDelayL.value.size(); ++i)
  m_optLambdaDelayL.value[i] = MathHelpers::Round(lb[i]); 
 for(size_t i = 0; i < m_optLambdaDelayR.value.size(); ++i)
  m_optLambdaDelayR.value[i] = MathHelpers::Round(rb[i]); 
}

void CAppSettingsModel::GetLamDelMap(float* map, float* rb, float* lb)
{
 for(size_t i = 0; i < m_optLambdaDelay.value.size(); ++i)
  map[i] = (float)m_optLambdaDelay.value[i];
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
