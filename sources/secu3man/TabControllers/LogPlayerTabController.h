/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#pragma once

#include <deque>
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"
#include "io-core/ControlAppAdapter.h"
#include "TabsManagement/ITabController.h"

class CCommunicationManager;
class CLogPlayerTabDlg;
class CStatusBarManager;
class ISettingsData;
class LogReader;

class CLogPlayerTabController : public ITabController, private IAPPEventHandler
{
 public:
  CLogPlayerTabController(CLogPlayerTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings);
  virtual ~CLogPlayerTabController();

 private:
  //появление/закрытие вкладки проигрывателя
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what, const CRect& i_rect);

  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //от представления (диалога)
  void OnSettingsChanged(void);
  void OnOpenFileButton(void);
  void OnPlayButton(void);
  void OnNextButton(void);
  void OnPrevButton(void);
  void OnTimeFactorCombo(size_t i_factor_code);
  void OnSliderMoved(UINT i_nSBCode, unsigned long i_nPos);

  //от таймера
  void OnTimer(void);

 private:
  //вспомогательные функции
  void _GoNext(void);
  void _GoBack(void);
  void _GetRecord(void);
  unsigned long _GetAveragedPeriod(void);

  enum EDirection
  {
   DIR_NEXT,
   DIR_PREV,
   DIR_NA
  };

  void _ProcessOneRecord(bool i_set_timer, EDirection i_direction, bool i_set_slider = true);

  //начало/остановка проигрывания
  void _Play(bool i_play);

  //инициализация плеера
  void _InitPlayer(void);

  //прекращение работы плеера
  void _ClosePlayer(void);

  //вывод текущей записи на приборы и обновление индикатора времени
  void _DisplayCurrentRecord(EDirection i_direction);

  //разрешение/запрещение кнопок в зависимости от текущего положения
  void _UpdateButtons(void);

  //Переустанавливет период таймера, если нужно
  void _UpdateTimerPeriod(bool i_set_timer);

 private:
  CLogPlayerTabDlg*  m_view;
  CCommunicationManager* m_comm;
  CStatusBarManager*  m_sbar;
  ISettingsData* mp_settings;

  //<factor id, <name, value> >
  std::map<size_t, std::pair<_TSTRING, float> > m_time_factors;

  //отвечает за итерироване по записям, которые находятся в файле
  std::auto_ptr<LogReader> mp_log_reader;

  CObjectTimer<CLogPlayerTabController> m_timer;

  //хранит последние периоды ежду записями для усреднения
  std::deque<unsigned long> m_last_perionds;

  //хранят предыдушую и текушую записи
  typedef std::pair<SYSTEMTIME, SECU3IO::SensorDat> RECORD_INFO;
  RECORD_INFO m_prev_record;
  RECORD_INFO m_curr_record;

  //сохраняет период таймера на время работы со слайдером
  unsigned long m_period_before_tracking;

  //true - если в данный момент происходит работа со слайдером
  bool m_now_tracking;

  //true - если в данный момент происходит проигрывание, false - если пауза
  bool m_playing;

  //хранит код текущего фактора времени (ключ для контейнера m_time_factors)
  size_t m_current_time_factor;
};
