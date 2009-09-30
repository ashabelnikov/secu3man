
#pragma once

#include "ITabController.h"
#include "common/unicodesupport.h"
#include "io-core/ControlAppAdapter.h"
#include "common/ObjectTimer.h"
#include <deque>

class CLogPlayerTabDlg;
class CCommunicationManager;
class CStatusBarManager;
class LogReader;

class CLogPlayerTabController : public ITabController, private IAPPEventHandler 
{
  public:
	CLogPlayerTabController(CLogPlayerTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
	virtual ~CLogPlayerTabController();

  private:
    CLogPlayerTabDlg*  m_view;
	CCommunicationManager* m_comm;
	CStatusBarManager*  m_sbar;    
	
    //появление/закрытие вкладки параметров и монитора
    virtual void OnActivate(void);
	virtual void OnDeactivate(void);
	virtual bool OnClose(void);

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

  private:

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
  
    //хранит последнее направление перемещения по записям
    EDirection m_last_direction; 

    //хранит код текущего фактора времени (ключ для контейнера m_time_factors)
    size_t m_current_time_factor;
};
