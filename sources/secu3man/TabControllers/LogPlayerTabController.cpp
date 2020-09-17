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

/** \file LogPlayerTabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <shlwapi.h>
#include <limits>
#include "Resources/resource.h"
#include "LogPlayerTabController.h"

#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "common/MathHelpers.h"
#include "io-core/LogReader.h"
#include "io-core/logWriter.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "MIDesk/CEDeskDlg.h"
#include "MIDesk/LMDeskDlg.h"
#include "MIDesk/MIDeskDlg.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/LogPlayerTabDlg.h"
#include "TabDialogs/LPControlPanelDlg.h"
#include "ui-core/FileDialogEx.h"
#include "ui-core/MsgBox.h"

#undef max

using namespace fastdelegate;
using namespace SECU3IO;

#define QUEUE_SIZE 32   //размер очереди для усреднения периода
#define EHKEY _T("LogPlayerCntr")

//Сравнивает только милисекунды и секунды
//возвращает разницу в миллисекундах.
static unsigned long CalcPeriod(SYSTEMTIME &i_time1, SYSTEMTIME &i_time2)
{
 unsigned long ms1 = (i_time1.wSecond * 1000) + i_time1.wMilliseconds;
 unsigned long ms2 = (i_time2.wSecond * 1000) + i_time2.wMilliseconds;
 return (ms1 <= ms2) ? (ms2 - ms1) : (60000 - ms1) + ms2;
}

//lokup table used to speed up time to string conversion
static const TCHAR lutDigits100[100][3] = 
    {_T("00"),_T("01"),_T("02"),_T("03"),_T("04"),_T("05"),_T("06"),_T("07"),_T("08"),_T("09"),
     _T("10"),_T("11"),_T("12"),_T("13"),_T("14"),_T("15"),_T("16"),_T("17"),_T("18"),_T("19"),
     _T("20"),_T("21"),_T("22"),_T("23"),_T("24"),_T("25"),_T("26"),_T("27"),_T("28"),_T("29"),
     _T("30"),_T("31"),_T("32"),_T("33"),_T("34"),_T("35"),_T("36"),_T("37"),_T("38"),_T("39"),
     _T("40"),_T("41"),_T("42"),_T("43"),_T("44"),_T("45"),_T("46"),_T("47"),_T("48"),_T("49"),
     _T("50"),_T("51"),_T("52"),_T("53"),_T("54"),_T("55"),_T("56"),_T("57"),_T("58"),_T("59"),
     _T("60"),_T("61"),_T("62"),_T("63"),_T("64"),_T("65"),_T("66"),_T("67"),_T("68"),_T("69"),
     _T("70"),_T("71"),_T("72"),_T("73"),_T("74"),_T("75"),_T("76"),_T("77"),_T("78"),_T("79"),
     _T("80"),_T("81"),_T("82"),_T("83"),_T("84"),_T("85"),_T("86"),_T("87"),_T("88"),_T("89"),
     _T("90"),_T("91"),_T("92"),_T("93"),_T("94"),_T("95"),_T("96"),_T("97"),_T("98"),_T("99")
    };

CLogPlayerTabController::CLogPlayerTabController(CLogPlayerTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings, LogWriter* i_pLogWriter)
: mp_view(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, m_pLogWriter(i_pLogWriter)
, mp_settings(ip_settings)
, mp_log_reader(new LogReader)
, m_now_tracking(false)
, m_period_before_tracking(0)
, m_playing(false)
, m_current_time_factor(5) //1:1
{ 
#define _IV(id, name, value) (std::make_pair((id), std::make_pair(_TSTRING(name), (value))))
 m_time_factors.insert(_IV(0, _T("16 : 1"),0.0625f));
 m_time_factors.insert(_IV(1, _T(" 8 : 1"), 0.125f));
 m_time_factors.insert(_IV(2, _T(" 4 : 1"), 0.25f));
 m_time_factors.insert(_IV(3, _T(" 3 : 1"), 0.33f));
 m_time_factors.insert(_IV(4, _T(" 2 : 1"), 0.50f));
 m_time_factors.insert(_IV(5, _T(" 1 : 1"), 1.00f));
 m_time_factors.insert(_IV(6, _T(" 1 : 2"), 2.00f));
 m_time_factors.insert(_IV(7, _T(" 1 : 3"), 3.00f));
 m_time_factors.insert(_IV(8, _T(" 1 : 4"), 4.00f));
 m_time_factors.insert(_IV(9, _T(" 1 : 8"), 8.00f));
#undef _IV

 //устанавливаем делегаты (обработчики событий от представления)
 mp_view->mp_LPPanelDlg->setOnOpenFileButton(MakeDelegate(this,&CLogPlayerTabController::OnOpenFileButton));
 mp_view->mp_LPPanelDlg->setOnPlayButton(MakeDelegate(this,&CLogPlayerTabController::OnPlayButton));
 mp_view->mp_LPPanelDlg->setOnNextButton(MakeDelegate(this,&CLogPlayerTabController::OnNextButton));
 mp_view->mp_LPPanelDlg->setOnPrevButton(MakeDelegate(this,&CLogPlayerTabController::OnPrevButton));
 mp_view->mp_LPPanelDlg->setOnTimeFactorCombo(MakeDelegate(this,&CLogPlayerTabController::OnTimeFactorCombo));
 mp_view->mp_LPPanelDlg->setOnSliderMoved(MakeDelegate(this,&CLogPlayerTabController::OnSliderMoved));
 mp_view->setOnDropFile(MakeDelegate(this,&CLogPlayerTabController::OnDropFile));

 m_timer.SetMsgHandler(this, &CLogPlayerTabController::OnTimer);

 mp_view->mp_MIDeskDlg->setOnMISettingsChanged(MakeDelegate(this, &CLogPlayerTabController::OnMIDeskSettingsChanged));
 mp_view->mp_MIDeskDlg->ShowGraphCursor(mp_settings->GetShowGraphsCursor());
}

CLogPlayerTabController::~CLogPlayerTabController()
{
 //empty
}

//изменились настройки программы!
void CLogPlayerTabController::OnSettingsChanged(int action)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
 mp_view->mp_MIDeskDlg->SetUpdatePeriod(mp_settings->GetMIDeskUpdatePeriod());
 mp_view->mp_MIDeskDlg->SetITMode(mp_settings->GetITEdMode());

 //обновляем диапазоны приборов
 mp_view->mp_MIDeskDlg->SetTachometerMax(mp_settings->GetTachometerMax());
 mp_view->mp_MIDeskDlg->SetPressureMax(mp_settings->GetPressureMax());
 mp_view->mp_MIDeskDlg->SetTemperatureMax(mp_settings->GetTemperatureMax());
 mp_view->mp_MIDeskDlg->SetInjPWMax(mp_settings->GetInjPWMax());
 mp_view->mp_MIDeskDlg->SetSpeedUnit(mp_settings->GetSpeedUnit());
 mp_view->mp_MIDeskDlg->SetDistanceUnit(mp_settings->GetSpeedUnit()); //use same option for distance

 mp_view->mp_MIDeskDlg->SetRPMAverageNum(mp_settings->GetRPMAverage());
 mp_view->mp_MIDeskDlg->SetVoltAverageNum(mp_settings->GetVoltAverage());
 mp_view->mp_MIDeskDlg->SetMAPAverageNum(mp_settings->GetMAPAverage());
 mp_view->mp_MIDeskDlg->SetAI1AverageNum(mp_settings->GetAI1Average());
 mp_view->mp_MIDeskDlg->SetTPSAverageNum(mp_settings->GetTPSAverage());
 mp_view->mp_MIDeskDlg->SetKnockAverageNum(mp_settings->GetKnockAverage());
 mp_view->mp_MIDeskDlg->SetIgnTimAverageNum(mp_settings->GetIgnTimAverage());
 mp_view->mp_MIDeskDlg->SetCLTAverageNum(mp_settings->GetCLTAverage());
 mp_view->mp_MIDeskDlg->SetAddI2AverageNum(mp_settings->GetAddI2Average());
 mp_view->mp_MIDeskDlg->SetInjPWAverageNum(mp_settings->GetInjPWAverage());
 mp_view->mp_MIDeskDlg->SetIATAverageNum(mp_settings->GetIATAverage());
 mp_view->mp_MIDeskDlg->SetEGOCorrAverageNum(mp_settings->GetEGOCorrAverage());
 mp_view->mp_MIDeskDlg->SetAirFlowAverageNum(mp_settings->GetAirFlowAverage());
 mp_view->mp_MIDeskDlg->SetVehicleSpeedAverageNum(mp_settings->GetVehicleSpeedAverage());
 mp_view->mp_MIDeskDlg->SetTPSDotAverageNum(mp_settings->GetTPSDotAverage());
 mp_view->mp_MIDeskDlg->SetMAP2AverageNum(mp_settings->GetMAP2Average());
 mp_view->mp_MIDeskDlg->SetMAPDAverageNum(mp_settings->GetMAPDAverage());
 mp_view->mp_MIDeskDlg->SetTmp2AverageNum(mp_settings->GetTmp2Average());
 mp_view->mp_MIDeskDlg->SetFuelConsumAverageNum(mp_settings->GetFuelConsumAverage());
 mp_view->mp_MIDeskDlg->SetFuelConsumFAverageNum(mp_settings->GetFuelConsumFAverage());
 mp_view->mp_MIDeskDlg->SetKnockRetardAverageNum(mp_settings->GetKnockRetardAverage());
 mp_view->mp_MIDeskDlg->SetSensAFRAverageNum(mp_settings->GetSensAFRAverage());
 mp_view->mp_MIDeskDlg->SetChokePosAverageNum(mp_settings->GetChokePosAverage());
 mp_view->mp_MIDeskDlg->SetGDPosAverageNum(mp_settings->GetGDPosAverage());
 mp_view->mp_MIDeskDlg->SetSynLoadAverageNum(mp_settings->GetSynLoadAverage());
 mp_view->mp_MIDeskDlg->SetInjTimBAverageNum(mp_settings->GetInjTimBAverage());
 mp_view->mp_MIDeskDlg->SetInjTimEAverageNum(mp_settings->GetInjTimEAverage());

 ConfigureIndicators();
 mp_view->Invalidate();
}

//from MainTabController
void CLogPlayerTabController::OnActivate(void)
{
 mp_view->mp_MIDeskDlg->SetGraphShtPixels(mp_settings->GetGraphShtPixels());
 mp_view->mp_MIDeskDlg->SetTachometerMax(mp_settings->GetTachometerMax());
 mp_view->mp_MIDeskDlg->SetPressureMax(mp_settings->GetPressureMax());
 mp_view->mp_MIDeskDlg->SetTemperatureMax(mp_settings->GetTemperatureMax());
 mp_view->mp_MIDeskDlg->SetInjPWMax(mp_settings->GetInjPWMax());
 mp_view->mp_MIDeskDlg->SetSpeedUnit(mp_settings->GetSpeedUnit());
 mp_view->mp_MIDeskDlg->SetDistanceUnit(mp_settings->GetSpeedUnit()); //use same option for distance
 mp_view->mp_MIDeskDlg->SetRPMAverageNum(mp_settings->GetRPMAverage());
 mp_view->mp_MIDeskDlg->SetVoltAverageNum(mp_settings->GetVoltAverage());
 mp_view->mp_MIDeskDlg->SetMAPAverageNum(mp_settings->GetMAPAverage());
 mp_view->mp_MIDeskDlg->SetAI1AverageNum(mp_settings->GetAI1Average());
 mp_view->mp_MIDeskDlg->SetTPSAverageNum(mp_settings->GetTPSAverage());
 mp_view->mp_MIDeskDlg->SetKnockAverageNum(mp_settings->GetKnockAverage());
 mp_view->mp_MIDeskDlg->SetIgnTimAverageNum(mp_settings->GetIgnTimAverage());
 mp_view->mp_MIDeskDlg->SetCLTAverageNum(mp_settings->GetCLTAverage());
 mp_view->mp_MIDeskDlg->SetAddI2AverageNum(mp_settings->GetAddI2Average());
 mp_view->mp_MIDeskDlg->SetInjPWAverageNum(mp_settings->GetInjPWAverage());
 mp_view->mp_MIDeskDlg->SetIATAverageNum(mp_settings->GetIATAverage());
 mp_view->mp_MIDeskDlg->SetEGOCorrAverageNum(mp_settings->GetEGOCorrAverage());
 mp_view->mp_MIDeskDlg->SetAirFlowAverageNum(mp_settings->GetAirFlowAverage());
 mp_view->mp_MIDeskDlg->SetVehicleSpeedAverageNum(mp_settings->GetVehicleSpeedAverage());
 mp_view->mp_MIDeskDlg->SetTPSDotAverageNum(mp_settings->GetTPSDotAverage());
 mp_view->mp_MIDeskDlg->SetMAP2AverageNum(mp_settings->GetMAP2Average());
 mp_view->mp_MIDeskDlg->SetMAPDAverageNum(mp_settings->GetMAPDAverage());
 mp_view->mp_MIDeskDlg->SetTmp2AverageNum(mp_settings->GetTmp2Average());
 mp_view->mp_MIDeskDlg->SetFuelConsumAverageNum(mp_settings->GetFuelConsumAverage());
 mp_view->mp_MIDeskDlg->SetFuelConsumFAverageNum(mp_settings->GetFuelConsumFAverage());
 mp_view->mp_MIDeskDlg->SetKnockRetardAverageNum(mp_settings->GetKnockRetardAverage());
 mp_view->mp_MIDeskDlg->SetSensAFRAverageNum(mp_settings->GetSensAFRAverage());
 mp_view->mp_MIDeskDlg->SetChokePosAverageNum(mp_settings->GetChokePosAverage());
 mp_view->mp_MIDeskDlg->SetGDPosAverageNum(mp_settings->GetGDPosAverage());
 mp_view->mp_MIDeskDlg->SetSynLoadAverageNum(mp_settings->GetSynLoadAverage());
 mp_view->mp_MIDeskDlg->SetInjTimBAverageNum(mp_settings->GetInjTimBAverage());
 mp_view->mp_MIDeskDlg->SetInjTimEAverageNum(mp_settings->GetInjTimEAverage());
 mp_view->mp_MIDeskDlg->SetITMode(mp_settings->GetITEdMode());

 //////////////////////////////////////////////////////////////////
 //Подключаем контроллер к потоку данных от SECU-3
 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->setOnSettingsChanged(MakeDelegate(this,&CLogPlayerTabController::OnSettingsChanged));
 //////////////////////////////////////////////////////////////////

 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = mp_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);

 std::vector<_TSTRING> tf_content;
 tf_content.reserve(16);
 for(size_t i = 0; i < m_time_factors.size(); ++i)
  tf_content.push_back(m_time_factors[i].first.c_str());
 mp_view->mp_LPPanelDlg->FillTimeFactorCombo(tf_content);

 mp_view->mp_LPPanelDlg->SetTimeFactor(m_current_time_factor);

 mp_view->mp_MIDeskDlg->SetUpdatePeriod(mp_settings->GetMIDeskUpdatePeriod());
 mp_view->mp_MIDeskDlg->ShowChokePos(true);
 mp_view->mp_MIDeskDlg->ShowGDPos(true);
 mp_view->mp_MIDeskDlg->ShowSpeedAndDistance(true);

 ConfigureIndicators();

 m_one_shot_timer.SetTimer(this, &CLogPlayerTabController::_OnOneShotTimer, 0);
}

//from MainTabController
void CLogPlayerTabController::OnDeactivate(void)
{
 m_one_shot_timer.KillTimer();
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 mp_sbar->SetInformationText(_T(""));
 _ClosePlayer();
 mp_settings->SetLogPlayerVert(mp_view->GetSplitterPos());
}

//hurrah!!! получен пакет от SECU-3
void CLogPlayerTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET:      //таблицы были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
  }
 }
}

//состояние подключения изменилось!
void CLogPlayerTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(mp_sbar);

 if (i_online) //перешли в онлайн
  state = CStatusBarManager::STATE_ONLINE;
 else
  state = CStatusBarManager::STATE_OFFLINE;

 mp_sbar->SetConnectionState(state);
}

bool CLogPlayerTabController::OnClose(void)
{
 return true;
}

void CLogPlayerTabController::OnCloseNotify(void)
{
 //empty
}

void CLogPlayerTabController::OnFullScreen(bool i_what)
{
 //not used
}

bool CLogPlayerTabController::OnAskChangeTab(void)
{
 return true; //always allows to change tab
}

void CLogPlayerTabController::OnSaveSettings(void)
{
 //empty
}

void CLogPlayerTabController::OnOpenFileButton(void)
{
 if (mp_log_reader->IsOpened())
 {  //нажали второй раз кнопку - закрываем плеер
  _ClosePlayer();
  return;
 }
 _TSTRING fileName = _T("");
 _OpenFile(fileName);
}

void CLogPlayerTabController::OnPlayButton(void)
{
 if (mp_view->mp_LPPanelDlg->GetPlayButtonState()) //start
  _Play(true);
 else //stop
  _Play(false);
}

void CLogPlayerTabController::OnNextButton(void)
{
 _ProcessOneRecord(false, DIR_NEXT);
}

void CLogPlayerTabController::OnPrevButton(void)
{
 _ProcessOneRecord(false, DIR_PREV);
}

void CLogPlayerTabController::OnTimeFactorCombo(size_t i_factor_code)
{ //сохраняем код выбранного фактора времени
 m_current_time_factor = i_factor_code;
}

void CLogPlayerTabController::OnSliderMoved(UINT i_nSBCode, unsigned long i_nPos)
{
#define _START_TRACKING() m_now_tracking = true; \
    m_period_before_tracking = m_timer.GetPeriod();\
    m_timer.KillTimer();

 switch(i_nSBCode)
 {
  case TB_ENDTRACK:
   m_now_tracking = false;
   if (m_playing)
    m_timer.SetTimer(m_period_before_tracking);
   break;

  case TB_BOTTOM:
  case TB_TOP:
   {
   _START_TRACKING();
   long count = ((long)mp_view->mp_LPPanelDlg->GetSliderPosition()) - (long)mp_log_reader->GetCurPos();
   EDirection dir = (count > 0) ? DIR_NEXT : DIR_PREV;
   count = _SkipRecords(dir, abs(count));
   for(long i = 0; i < count; ++i)
    _ProcessOneRecord(false, dir, false);
   }
   break;

  case TB_PAGEDOWN:
  case TB_PAGEUP:
   {
    _START_TRACKING();
    unsigned long count = mp_view->mp_LPPanelDlg->GetSliderPageSize();
    EDirection dir = (i_nSBCode==TB_PAGEDOWN) ? DIR_NEXT : DIR_PREV;
    count = _SkipRecords(dir, count);
    for(unsigned long i = 0; i < count; ++i)
     _ProcessOneRecord(false, dir, false);
   }
   break;

  case TB_LINEDOWN:
  case TB_LINEUP:
   {
    _START_TRACKING();
    unsigned long count = mp_view->mp_LPPanelDlg->GetSliderLineSize();
    EDirection dir = (i_nSBCode==TB_LINEDOWN) ? DIR_NEXT : DIR_PREV;
    count = _SkipRecords(dir, count);
    for(unsigned long i = 0; i < count; ++i)
     _ProcessOneRecord(false, dir, false);
   }
   break;

  case TB_THUMBPOSITION:
  case TB_THUMBTRACK:
   _START_TRACKING();
   long count = (int)i_nPos - (int)mp_log_reader->GetCurPos();
   EDirection dir = (count > 0) ? DIR_NEXT : DIR_PREV;
   count = _SkipRecords(dir, abs(count));
   for(long i = 0; i < count; ++i)
    _ProcessOneRecord(false, dir, false);
   break;
 }
#undef _START_TRACKING
}

void CLogPlayerTabController::OnTimer(void)
{
 if (m_now_tracking || !m_playing)
  return;

 if (!mp_log_reader->IsNextPossible())
 {
  _Play(false); //останавливаем проигрывание если дошли до конца
  return;
 }

 _ProcessOneRecord(true, DIR_NEXT);
}

void CLogPlayerTabController::OnDropFile(_TSTRING fileName)
{
 if (mp_log_reader->IsOpened())
  _ClosePlayer(); //закрываем плеер перед началом следующего проигрывания

 _OpenFile(fileName);
}

void CLogPlayerTabController::_OpenFile(const _TSTRING& fileName)
{
 HANDLE   hFile = NULL;
 static TCHAR BASED_CODE szFilter[] = _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
 CFileDialogEx open(TRUE, NULL, NULL, NULL, szFilter, NULL);

 if (fileName.empty() && open.DoModal() != IDOK)
  return; //пользователь передумал

 mp_log_reader->SetSeparatingSymbol(mp_settings->GetCSVSepSymbol());

 LogReader::FileError error_id;
 _TSTRING file_path = fileName.empty() ? ((LPCTSTR)open.GetPathName()) : fileName;
 AfxGetMainWnd()->BeginWaitCursor();
 mp_sbar->SetInformationText(MLL::LoadString(IDS_LOADING_LOG_FILE));
 bool result = mp_log_reader->OpenFile(file_path, error_id, m_pLogWriter->GetFileHandle(), false); //do not fully check file
 AfxGetMainWnd()->EndWaitCursor();
 mp_sbar->SetInformationText(_T(""));
 if (false==result)
 {
  if (error_id==LogReader::FE_OPEN)
   SECUMessageBox(MLL::LoadString(IDS_LP_CANT_OPEN_FILE));
  else if (error_id==LogReader::FE_PENDING)
   SECUMessageBox(MLL::LoadString(IDS_LP_OPEN_PENDING));
  else if (error_id==LogReader::FE_FORMAT)
  {
   SECUMessageBox(MLL::LoadString(IDS_LP_INCORRECT_FILE_FORMAT));
  }
  else
   ASSERT(0);

  mp_log_reader->CloseFile();
  return; //не можем продолжать, так как произошла ошибка
 }

 ////////////////////////////////////////////////////////////////
 mp_view->mp_LPPanelDlg->SetOpenFileButtonText(MLL::GetString(IDS_LP_CLOSE_FILE));

 //obtain file name from full path
 TCHAR stripped_name[MAX_PATH+1] = {0};
 file_path.copy(stripped_name, file_path.size());
 PathStripPath(stripped_name);

 CString string;
 string.Format(MLL::LoadString(IDS_LP_FILE_INFO_FMT_STRING), stripped_name, mp_log_reader->GetCount());
 mp_view->mp_LPPanelDlg->SetFileIndicator((LPCTSTR)string);

 mp_view->mp_MIDeskDlg->Enable(true);
 mp_view->mp_CEDeskDlg->Enable(true);
 mp_view->mp_LMDeskDlg->Enable(true);
 mp_view->mp_LPPanelDlg->EnableAll(true);
 mp_view->EnableAll(true);


 //инициализируем логику плеера и начинаем сразу проигрывать
 if (mp_log_reader->GetCount() > 0)
 {
  _InitPlayer();
  _Play(true);
 }
 ////////////////////////////////////////////////////////////////
}

void CLogPlayerTabController::_GoNext(void)
{
 //вычисляем период и добавляем его в очередь спереди
 unsigned long period = CalcPeriod(m_prev_record.first, m_curr_record.first);
 m_last_perionds.push_front(period);

 //ограничиваем размер очереди
 if (m_last_perionds.size() > QUEUE_SIZE)
  m_last_perionds.pop_back();

 m_prev_record = m_curr_record;
}

void CLogPlayerTabController::_GoBack(void)
{
 //добавляем в очередь cзади
 unsigned long period = CalcPeriod(m_curr_record.first, m_prev_record.first);
 m_last_perionds.push_back(period);

 //ограничиваем размер очереди
 if (m_last_perionds.size() > QUEUE_SIZE)
  m_last_perionds.pop_front();

 m_prev_record = m_curr_record;
}

void CLogPlayerTabController::_GetRecord(void)
{
 bool status = mp_log_reader->GetRecord(m_curr_record.first, m_curr_record.second, m_curr_marks);
 if (!status)
 {
  SECUMessageBox(MLL::LoadString(IDS_LP_FILE_READ_ERROR), MB_OK);
  _ClosePlayer(); //прекращаем проигрывание
 }
}

unsigned long CLogPlayerTabController::_GetAveragedPeriod(void)
{
 unsigned long sum = 0;
 //find average using last N of samples
 sum = std::accumulate(m_last_perionds.begin(), m_last_perionds.end(), 0);
 if (m_last_perionds.size()!=0)
  return sum / m_last_perionds.size();
 else
  return 10; //not samples, use default value 10ms
}

unsigned long CLogPlayerTabController::_SkipRecords(EDirection i_direction, unsigned long count)
{
 unsigned long m = 2;//(m_lastdir != i_direction) ? 2 : 1; //commented expression is faster but cases a bug, TODO: fix bug and improve speed
 unsigned long space, mincnt = (mp_view->mp_MIDeskDlg->GetGraphSamplesNum() * m) + 1;

 if (i_direction == DIR_NEXT)
  space = (mp_log_reader->GetCount() - 1) - mp_log_reader->GetCurPos();
 else if (i_direction == DIR_PREV)
  space = mp_log_reader->GetCurPos();
 else
 {
  ASSERT(0);
 }
 
 if (count > space)
  count = space;

 if (count <= mincnt)
 {
  return count; //nothing ot skip
 }
 else
 {
  int skip = (count - mincnt);
  if (i_direction == DIR_NEXT)
  {
   mp_log_reader->Next(skip);
  }
  else if (i_direction == DIR_PREV)
  {
   mp_log_reader->Prev(skip);
  }
  int marks;
  mp_log_reader->GetRecord(m_prev_record.first, m_prev_record.second, marks); //init previous record, otherwise bug with incorrect period will appear because we skip records
  return mincnt;
 }
}

void CLogPlayerTabController::_ProcessOneRecord(bool i_set_timer, EDirection i_direction, bool i_set_slider /*= true*/)
{
 if (i_direction == DIR_NEXT)
 {
  if (mp_log_reader->IsNextPossible())
   mp_log_reader->Next();
  else
   return;
 }
 else if (i_direction == DIR_PREV)
 {
  if (mp_log_reader->IsPrevPossible())
   mp_log_reader->Prev();
  else
   return;
 }
 else
 {
  ASSERT(0);
  return;
 }

 _GetRecord();

 if (i_direction == DIR_NEXT)
  _GoNext();
 else if (i_direction == DIR_PREV)
  _GoBack();
 else
 {
  ASSERT(0);
  return;
 }

 _UpdateTimerPeriod(i_set_timer);
 _DisplayCurrentRecord(i_direction);
 _UpdateButtons();

 if (i_set_slider)
  mp_view->mp_LPPanelDlg->SetSliderPosition(mp_log_reader->GetCurPos());

 //Stop playing if mark appears and it is enabled by a check box
 if (m_curr_marks && mp_view->mp_LPPanelDlg->GetStopOnMarksCheck())
 {
  if (mp_view->mp_LPPanelDlg->GetPlayButtonState()) //start
  {
   _Play(false);
   mp_view->mp_LPPanelDlg->SetPlayButtonState(false);
  }
 }
}

void CLogPlayerTabController::_UpdateTimerPeriod(bool i_set_timer)
{
 //Вычисляем средний период. Учитываем фактор времени
 unsigned long period = _GetAveragedPeriod();
 period = MathHelpers::Round(((float)period) * m_time_factors[m_current_time_factor].second);

 //перезапускаем таймер если нужно. Если новый период отличается от старого
 //меньше чем на 10%, то не перезапускаем таймер
 if ((i_set_timer) && ((unsigned long)abs(m_timer.GetPeriod() - (int)period) > (period / 10)))
 {
  m_timer.KillTimer();
  m_timer.SetTimer(period);
 }
}

void CLogPlayerTabController::_Play(bool i_play)
{
 if (i_play)
  m_timer.SetTimer(0);
 else
  m_timer.KillTimer();

 m_playing = i_play;
 mp_view->mp_LPPanelDlg->SetPlayButtonState(i_play);
}

void CLogPlayerTabController::_InitPlayer(void)
{
 m_last_perionds.clear();
 mp_view->mp_LPPanelDlg->EnableSlider(mp_log_reader->GetCount() > 1);
 mp_view->mp_LPPanelDlg->SetSliderRange(0, mp_log_reader->GetCount());
 mp_view->mp_LPPanelDlg->SetSliderPosition(mp_log_reader->GetCurPos());
 mp_view->mp_LPPanelDlg->SetSliderPageSize(mp_log_reader->GetCount()/10); //10 pages
 _GetRecord();
 _DisplayCurrentRecord(DIR_NEXT);
 _UpdateButtons();
 m_prev_record = m_curr_record;
}

void CLogPlayerTabController::_ClosePlayer(void)
{
 mp_view->mp_LPPanelDlg->SetFileIndicator(_T(""));
 mp_view->mp_LPPanelDlg->SetOpenFileButtonText(MLL::GetString(IDS_LP_OPEN_FILE));
 mp_view->mp_LPPanelDlg->SetSliderPosition(0);
 mp_view->mp_MIDeskDlg->Enable(false);
 mp_view->mp_CEDeskDlg->Enable(false);
 mp_view->mp_LMDeskDlg->Enable(false);
 mp_view->mp_LPPanelDlg->EnableAll(false);
 mp_view->EnableAll(false);
 m_timer.KillTimer();
 mp_log_reader->CloseFile();
 mp_view->mp_LPPanelDlg->SetPositionIndicator(_T(""));
 mp_view->mp_MIDeskDlg->Reset();
 mp_view->mp_LMDeskDlg->SetValues(false, false, false);
}

void CLogPlayerTabController::_DisplayCurrentRecord(EDirection i_direction)
{
 //обновляем приборы, а также обновляем позицию слайдера, если нужно
 mp_view->mp_MIDeskDlg->SetValues(&m_curr_record.second, i_direction == DIR_PREV);

 //обновляем ошибки
 mp_view->mp_CEDeskDlg->SetValues(m_curr_record.second.ce_errors);

 //Обновляем метки лога
 mp_view->mp_LMDeskDlg->SetValues(m_curr_marks & 0x1, m_curr_marks & 0x2, m_curr_marks & 0x4);

 //выводим время записи
/* CString str;
 str.Format(_T("%05d    %02d:%02d:%02d.%02d"),
     mp_log_reader->GetCurPos() + 1,
     m_curr_record.first.wHour,
     m_curr_record.first.wMinute,
     m_curr_record.first.wSecond,
     m_curr_record.first.wMilliseconds / 10);*/

 TCHAR str[23] = "000000    HH:MM:SS.mm";
 //current position
 unsigned long curpos = mp_log_reader->GetCurPos() + 1;
 str[0] = (TCHAR)((curpos / 1000000) % 10 + _T('0'));
 str[1] = (TCHAR)((curpos / 100000) % 10 + _T('0'));
 str[2] = (TCHAR)((curpos / 10000) % 10 + _T('0'));
 str[3] = (TCHAR)((curpos / 1000) % 10 + _T('0'));
 str[4] = (TCHAR)((curpos / 100) % 10 + _T('0'));
 str[5] = (TCHAR)((curpos / 10) % 10 + _T('0'));
 str[6] = (TCHAR)((curpos) % 10 + _T('0'));
 //HH
 str[10] = lutDigits100[m_curr_record.first.wHour][0];
 str[11] = lutDigits100[m_curr_record.first.wHour][1];
 //MM
 str[13] = lutDigits100[m_curr_record.first.wMinute][0];
 str[14] = lutDigits100[m_curr_record.first.wMinute][1];
 //SS
 str[16] = lutDigits100[m_curr_record.first.wSecond][0];
 str[17] = lutDigits100[m_curr_record.first.wSecond][1];
 //mm
 int msd = m_curr_record.first.wMilliseconds / 10;
 str[19] = lutDigits100[msd][0];
 str[20] = lutDigits100[msd][1];

 mp_view->mp_LPPanelDlg->SetPositionIndicator((LPCTSTR)str);
}

void CLogPlayerTabController::_UpdateButtons(void)
{
 if (mp_view->mp_LPPanelDlg->IsNextButtonEnabled()!=mp_log_reader->IsNextPossible())
  mp_view->mp_LPPanelDlg->EnableNextButton(mp_log_reader->IsNextPossible());
 if (mp_view->mp_LPPanelDlg->IsPrevButtonEnabled()!=mp_log_reader->IsPrevPossible())
  mp_view->mp_LPPanelDlg->EnablePrevButton(mp_log_reader->IsPrevPossible());
 if (mp_view->mp_LPPanelDlg->IsPlayButtonEnabled()!=mp_log_reader->IsNextPossible())
  mp_view->mp_LPPanelDlg->EnablePlayButton(mp_log_reader->IsNextPossible());
}

void CLogPlayerTabController::ConfigureIndicators(void)
{
 //indicators
 int idx = (int)mp_settings->GetShowExFixtures();
 IndicatorsCfg cfg0;
 mp_settings->GetIndicatorsConfig(cfg0);
 mp_view->mp_MIDeskDlg->SetIndicatorsCfg(cfg0.m_optIndHeightPercent[idx], cfg0.m_optIndRows[idx], cfg0.m_optIndGas_v[idx], cfg0.m_optIndCarb[idx], cfg0.m_optIndIdleValve[idx], cfg0.m_optIndPowerValve[idx], cfg0.m_optIndStBlock[idx], cfg0.m_optIndAE[idx], cfg0.m_optIndCoolingFan[idx], cfg0.m_optIndCE[idx], cfg0.m_optIndFCRevLim[idx], cfg0.m_optIndFloodClear[idx], cfg0.m_optIndSysLocked[idx], cfg0.m_optIndIgn_i[idx], cfg0.m_optIndCond_i[idx], cfg0.m_optIndEpas_i[idx]);

 mp_view->mp_MIDeskDlg->SetIndicatorsDragNDrop(mp_settings->GetIndicatorsDragNDrop());

 //meters
 MetersCfg cfg1[2];
 mp_settings->GetMetersConfig(cfg1);
 mp_view->mp_MIDeskDlg->SetMetersCfg(cfg1[idx].m_optMetRows, cfg1[idx].m_optMetRPM, cfg1[idx].m_optMetMAP, cfg1[idx].m_optMetVBat, cfg1[idx].m_optMetIgnTim, cfg1[idx].m_optMetCLT, cfg1[idx].m_optMetAddI1, cfg1[idx].m_optMetAddI2,
  cfg1[idx].m_optMetInjPW, cfg1[idx].m_optMetIAT, cfg1[idx].m_optMetEGOCorr, cfg1[idx].m_optMetTPS, cfg1[idx].m_optMetAirFlow, cfg1[idx].m_optMetVehicleSpeed, cfg1[idx].m_optMetTPSDot, cfg1[idx].m_optMetMAP2, cfg1[idx].m_optMetMAPD,
  cfg1[idx].m_optMetTmp2, cfg1[idx].m_optMetFuelConsum, cfg1[idx].m_optMetKnockRetard, cfg1[idx].m_optMetKnockGraph, cfg1[idx].m_optMetSensAFR, cfg1[idx].m_optMetChokePos, cfg1[idx].m_optMetGDPos, cfg1[idx].m_optMetSynLoad, cfg1[idx].m_optMetInjTimB, cfg1[idx].m_optMetInjTimE, cfg1[idx].m_optMetFuelConsumF,
  mp_settings->GetTitleFontSize(), mp_settings->GetValueFontSize(), mp_settings->GetPaneFontSize(), mp_settings->GetLabelFontSize());

 mp_view->mp_MIDeskDlg->SetMetersDragNDrop(mp_settings->GetMetersDragNDrop());
}

void CLogPlayerTabController::OnMIDeskSettingsChanged(void)
{
 int idx = (int)mp_settings->GetShowExFixtures();

 //indicators
 IndicatorsCfg cfg0;
 mp_settings->GetIndicatorsConfig(cfg0);

 float IndHeightPercent; int IndRows; CMIDeskDlg::IndCfg_t IndGas_v, IndCarb, IndIdleValve, IndPowerValve, IndStBlock, IndAE,
 IndCoolingFan, IndCE, IndFCRevLim, IndFloodClear, IndSysLocked, IndIgn_i, IndCond_i, IndEpas_i;

 mp_view->mp_MIDeskDlg->GetIndicatorsCfg(IndHeightPercent, IndRows, IndGas_v, IndCarb, IndIdleValve, IndPowerValve, IndStBlock, IndAE, IndCoolingFan, IndCE, IndFCRevLim, IndFloodClear, IndSysLocked, IndIgn_i, IndCond_i, IndEpas_i);

 cfg0.m_optIndHeightPercent[idx] = IndHeightPercent, cfg0.m_optIndRows[idx] = IndRows, cfg0.m_optIndGas_v[idx] = IndGas_v, cfg0.m_optIndCarb[idx] = IndCarb, cfg0.m_optIndIdleValve[idx] = IndIdleValve, cfg0.m_optIndPowerValve[idx] = IndPowerValve, cfg0.m_optIndStBlock[idx] = IndStBlock, cfg0.m_optIndAE[idx] = IndAE, cfg0.m_optIndCoolingFan[idx] = IndCoolingFan, cfg0.m_optIndCE[idx] = IndCE, cfg0.m_optIndFCRevLim[idx] = IndFCRevLim, cfg0.m_optIndFloodClear[idx] = IndFloodClear, cfg0.m_optIndSysLocked[idx] = IndSysLocked, cfg0.m_optIndIgn_i[idx] = IndIgn_i, cfg0.m_optIndCond_i[idx] = IndCond_i, cfg0.m_optIndEpas_i[idx] = IndEpas_i;

 for (int ii = 0; ii < 2; ++ii) //copy colors, redundant because we have single color set in ini file
  cfg0.m_optIndGas_v[ii].second = IndGas_v.second, cfg0.m_optIndCarb[ii].second = IndCarb.second, cfg0.m_optIndIdleValve[ii].second = IndIdleValve.second, cfg0.m_optIndPowerValve[ii].second = IndPowerValve.second, cfg0.m_optIndStBlock[ii].second = IndStBlock.second, cfg0.m_optIndAE[ii].second = IndAE.second, cfg0.m_optIndCoolingFan[ii].second = IndCoolingFan.second, cfg0.m_optIndCE[ii].second = IndCE.second, cfg0.m_optIndFCRevLim[ii].second = IndFCRevLim.second, cfg0.m_optIndFloodClear[ii].second = IndFloodClear.second, cfg0.m_optIndSysLocked[ii].second = IndSysLocked.second, cfg0.m_optIndIgn_i[ii].second = IndIgn_i.second, cfg0.m_optIndCond_i[ii].second = IndCond_i.second, cfg0.m_optIndEpas_i[ii].second = IndEpas_i.second;

 mp_settings->SetIndicatorsConfig(cfg0);

 mp_settings->SetIndicatorsDragNDrop(mp_view->mp_MIDeskDlg->GetIndicatorsDragNDrop());

 //meters
 MetersCfg cfg1[2];
 mp_settings->GetMetersConfig(cfg1);

 int  TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize;

 mp_view->mp_MIDeskDlg->GetMetersCfg(cfg1[idx].m_optMetRows, cfg1[idx].m_optMetRPM, cfg1[idx].m_optMetMAP, cfg1[idx].m_optMetVBat, cfg1[idx].m_optMetIgnTim, cfg1[idx].m_optMetCLT, cfg1[idx].m_optMetAddI1, cfg1[idx].m_optMetAddI2,
  cfg1[idx].m_optMetInjPW, cfg1[idx].m_optMetIAT, cfg1[idx].m_optMetEGOCorr, cfg1[idx].m_optMetTPS, cfg1[idx].m_optMetAirFlow, cfg1[idx].m_optMetVehicleSpeed, cfg1[idx].m_optMetTPSDot, cfg1[idx].m_optMetMAP2,
  cfg1[idx].m_optMetMAPD, cfg1[idx].m_optMetTmp2, cfg1[idx].m_optMetFuelConsum, cfg1[idx].m_optMetKnockRetard, cfg1[idx].m_optMetKnockGraph, cfg1[idx].m_optMetSensAFR, cfg1[idx].m_optMetChokePos,
  cfg1[idx].m_optMetGDPos, cfg1[idx].m_optMetSynLoad, cfg1[idx].m_optMetInjTimB, cfg1[idx].m_optMetInjTimE, cfg1[idx].m_optMetFuelConsumF, TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);

 mp_settings->SetMetersConfig(cfg1);
 mp_settings->SetTitleFontSize(TitleFontSize);
 mp_settings->SetValueFontSize(ValueFontSize);
 mp_settings->SetPaneFontSize(PaneFontSize);
 mp_settings->SetLabelFontSize(LabelFontSize);

 mp_settings->SetMetersDragNDrop(mp_view->mp_MIDeskDlg->GetMetersDragNDrop());
}

void CLogPlayerTabController::_OnOneShotTimer(void)
{
 m_one_shot_timer.KillTimer();
 if (mp_settings->GetLogPlayerVert() != std::numeric_limits<int>::max())
  mp_view->SetSplitterPos(mp_settings->GetLogPlayerVert());
}
