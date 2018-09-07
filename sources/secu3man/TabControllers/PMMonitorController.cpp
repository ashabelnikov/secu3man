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

/** \file PMMonitorController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "PMMonitorController.h"

#include <algorithm>
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "io-core/bitmask.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "MIDesk/CEDeskDlg.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/RSDeskDlg.h"
#include "ParamDesk/Tables/ITablesDeskView.h"
#include "Settings/ISettingsData.h"

using namespace fastdelegate;
using namespace SECU3IO;

CPMMonitorController::CPMMonitorController(VIEW* ip_view, RSDVIEW* ip_rsdview, CEDVIEW* ip_cedview, TDVIEW* ip_tdview, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings)
: Super(ip_view)
, mp_rsdview(ip_rsdview)
, mp_cedview(ip_cedview)
, mp_tdview(ip_tdview)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, mp_settings(ip_settings)
, m_show_raw_sensors(false)
{
 //empty
}

CPMMonitorController::~CPMMonitorController()
{
 //empty
}

void CPMMonitorController::OnSettingsChanged(void)
{
 mp_view->SetUpdatePeriod(mp_settings->GetMIDeskUpdatePeriod());
 //обновляем диапазоны приборов
 mp_view->SetTachometerMax(mp_settings->GetTachometerMax());
 mp_view->SetPressureMax(mp_settings->GetPressureMax());
 mp_view->SetSpeedUnit(mp_settings->GetSpeedUnit());
 mp_view->SetDistanceUnit(mp_settings->GetSpeedUnit()); //same option for distance
 mp_view->ShowSpeedAndDistance(mp_settings->GetNumPulsesPer1Km() > 0); //disabled if pulses = 0
 mp_view->SetRPMAverageNum(mp_settings->GetRPMAverage());
 mp_view->SetVoltAverageNum(mp_settings->GetVoltAverage());
 mp_view->SetMAPAverageNum(mp_settings->GetMAPAverage());
 mp_view->SetAI1AverageNum(mp_settings->GetAI1Average());
 mp_view->SetTPSAverageNum(mp_settings->GetTPSAverage());
}

void CPMMonitorController::OnActivate(void)
{
 mp_view->SetTachometerMax(mp_settings->GetTachometerMax());
 mp_view->SetPressureMax(mp_settings->GetPressureMax());
 mp_view->SetSpeedUnit(mp_settings->GetSpeedUnit());
 mp_view->SetDistanceUnit(mp_settings->GetSpeedUnit()); //same option for distance
 mp_view->SetUpdatePeriod(mp_settings->GetMIDeskUpdatePeriod());
 mp_view->ShowSpeedAndDistance(mp_settings->GetNumPulsesPer1Km() > 0); //disabled if pulses = 0
 mp_view->SetRPMAverageNum(mp_settings->GetRPMAverage());
 mp_view->SetVoltAverageNum(mp_settings->GetVoltAverage());
 mp_view->SetMAPAverageNum(mp_settings->GetMAPAverage());
 mp_view->SetAI1AverageNum(mp_settings->GetAI1Average());
 mp_view->SetTPSAverageNum(mp_settings->GetTPSAverage());
}

void CPMMonitorController::OnDeactivate(void)
{
 //empty
}

void CPMMonitorController::Enable(bool state)
{
 mp_view->Enable(state);
 mp_rsdview->Enable(state);
 mp_cedview->Enable(state);
 mp_tdview->EnableAdvanceAngleIndication(state);
}

void CPMMonitorController::StartDataCollection(void)
{
 Super::StartDataCollection();
}

//возвращает true когда работа автомата завершена
//m_operation_state = 0 для запуска
bool CPMMonitorController::CollectData(const BYTE i_descriptor, const void* i_packet_data)
{
 switch(m_operation_state)
 {
  case 0:
   if (!m_show_raw_sensors)
   {
    //--чекбокс сказал нам что мы в режиме панели приборов--
    if (i_descriptor != SENSOR_DAT)
     mp_comm->m_pControlApp->ChangeContext(SENSOR_DAT);
    else
    {
     SensorDat* sd = (SensorDat*)(i_packet_data);
     //устанавливаем значения приборов, разрешаем их и переходим в основной режим
     mp_view->SetValues(sd);
     mp_cedview->SetValues(sd->ce_errors);
     mp_tdview->SetDynamicValues(sd->frequen, sd->temperat, sd->air_flow, sd->adv_angle, sd->knock_retard, sd->knkret_use, sd->strt_aalt, sd->strt_use,
      sd->idle_aalt, sd->idle_use, sd->work_aalt, sd->work_use, sd->temp_aalt, sd->temp_use, sd->airt_aalt, sd->airt_use,
      sd->idlreg_aac, sd->idlreg_use, sd->octan_aac, sd->octan_use, sd->tps, sd->choke_pos, sd->tpsdot, sd->voltage, sd->add_i1, sd->tmp2, sd->baro_press, sd->load, sd->afr, sd->acceleration, sd->ephh_valve, sd->air_temp);
     mp_view->Enable(mp_comm->m_pControlApp->GetOnlineStatus());
     mp_cedview->Enable(mp_comm->m_pControlApp->GetOnlineStatus());
     mp_tdview->EnableAdvanceAngleIndication(mp_comm->m_pControlApp->GetOnlineStatus());
     m_operation_state = 1; //все готово для отображения данных
    }
   }
   else
   {
    //--чекбокс сказал нам что мы в режиме "сырых" значений--
    if (i_descriptor != ADCRAW_DAT)
     mp_comm->m_pControlApp->ChangeContext(ADCRAW_DAT);
    else
    {
     //устанавливаем значения приборов, разрешаем их и переходим в основной режим
     mp_rsdview->SetValues((RawSensDat*)(i_packet_data));
     mp_rsdview->Enable(mp_comm->m_pControlApp->GetOnlineStatus());
     mp_tdview->EnableAdvanceAngleIndication(false);
     m_operation_state = 1; //все готово для отображения данных
    }
   }   
   break;

  case 1:
   if (!m_show_raw_sensors)
   {
    //--чекбокс сказал нам что мы в режиме панели приборов--
    if (i_descriptor != SENSOR_DAT)
     mp_comm->m_pControlApp->ChangeContext(SENSOR_DAT);
    else
    {
     SensorDat* sd = (SensorDat*)(i_packet_data);
     mp_view->SetValues(sd);
     mp_cedview->SetValues(sd->ce_errors);
     mp_tdview->SetDynamicValues(sd->frequen, sd->temperat, sd->air_flow, sd->adv_angle, sd->knock_retard, sd->knkret_use, sd->strt_aalt, sd->strt_use,
      sd->idle_aalt, sd->idle_use, sd->work_aalt, sd->work_use, sd->temp_aalt, sd->temp_use, sd->airt_aalt, sd->airt_use,
      sd->idlreg_aac, sd->idlreg_use, sd->octan_aac, sd->octan_use, sd->tps, sd->choke_pos, sd->tpsdot, sd->voltage, sd->add_i1, sd->tmp2, sd->baro_press, sd->load, sd->afr, sd->acceleration, sd->ephh_valve, sd->air_temp);
    }
   }
   else
   {
    //--чекбокс сказал нам что мы в режиме "сырых" значений--
    if (i_descriptor != ADCRAW_DAT)
     mp_comm->m_pControlApp->ChangeContext(ADCRAW_DAT);
    else
     mp_rsdview->SetValues((RawSensDat*)(i_packet_data));
   }
   break;
 }

 return false; //КА продолжает работу...
}

void CPMMonitorController::ShowRawSensors(bool show)
{
 if (show)
 {//показывать сырые значения (прячем панель приборов и показываем панель "сырых" значений)
  mp_view->ShowWindow(SW_HIDE);
  mp_cedview->Show(false);
  mp_rsdview->ShowWindow(SW_SHOW);
  StartDataCollection(); //reset finite state machine
 }
 else
 {//показывать панель приборов (прячем панель "сырых" значений и показываем панель приборов)
  mp_view->ShowWindow(SW_SHOW);
  mp_cedview->Show(true);
  mp_rsdview->ShowWindow(SW_HIDE);
  StartDataCollection(); //reset finite state machine
 }
 m_show_raw_sensors = show;
}

void CPMMonitorController::ApplyFWOptions(DWORD opt)
{
 mp_rsdview->EnableSECU3TItems(CHECKBIT32(opt, COPT_SECU3T));
 mp_rsdview->EnableExtraIO(CHECKBIT32(opt, COPT_TPIC8101) && !CHECKBIT32(opt, COPT_SECU3T));
 mp_view->ShowChokePos(CHECKBIT32(opt, COPT_SM_CONTROL));
 mp_view->ShowGDPos(CHECKBIT32(opt, COPT_GD_CONTROL));
}
