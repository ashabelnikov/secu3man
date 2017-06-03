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

/** \file PMParamsController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "PMParamsController.h"

#include <algorithm>
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "io-core/bitmask.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "ParamDesk/Params/ParamDeskDlg.h"

using namespace fastdelegate;
using namespace SECU3IO;

CPMParamsController::CPMParamsController(VIEW* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, EventHandler RequestDataCollection)
: Super(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, m_parameters_changed(false)
, m_lastSel(0)
, m_RequestDataCollection(RequestDataCollection)
{
 mp_view->SetOnTabActivate(MakeDelegate(this, &CPMParamsController::OnParamDeskTabActivate));
 mp_view->SetOnChangeInTab(MakeDelegate(this, &CPMParamsController::OnParamDeskChangeInTab));
 mp_view->SetOnSaveButton(MakeDelegate(this, &CPMParamsController::OnPDSaveButton));
}

CPMParamsController::~CPMParamsController()
{
 m_pd_changes_timer.KillTimer();
}

void CPMParamsController::OnActivate(void)
{
 //выбираем ранее выбранную вкладку на панели параметров
 bool result = mp_view->SetCurSel(m_lastSel);

 //запускаем таймер по которому будет ограничиваться частота посылки данных в SECU-3
 m_pd_changes_timer.SetTimer(this, &CPMParamsController::OnParamDeskChangesTimer, 250);

 mp_view->EnableBTNameAndPass(true);
}

void CPMParamsController::OnDeactivate(void)
{
 //таймер работает только если мы находимся в контексте "параметров и монитора"
 m_pd_changes_timer.KillTimer();

 //запоминаем номер последней выбранной вкладки на панели параметров
 m_lastSel = mp_view->GetCurSel();
}

void CPMParamsController::Enable(bool state)
{
 mp_view->Enable(state);
}

void CPMParamsController::StartDataCollection(void)
{
 BYTE view_descriptor = mp_view->GetCurrentDescriptor();
 mp_comm->m_pControlApp->ChangeContext(view_descriptor);  //change context!
 Super::StartDataCollection();
}

//возвращает true когда работа автомата завершена
//m_operation_state = 0 для запуска
bool CPMParamsController::CollectData(const BYTE i_descriptor, const void* i_packet_data)
{
 mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_PARAMS));

 switch(m_operation_state)
 {
  case 0:  //ожидаем пакета с указанными нами параметрами
  {
   BYTE view_descriptor = mp_view->GetCurrentDescriptor();
   if (i_descriptor != view_descriptor)
   {
    mp_comm->m_pControlApp->ChangeContext(view_descriptor);
   }
   else
   {//тот что надо!
    mp_view->SetValues(i_descriptor, i_packet_data);

    //процесс инициализации окончен
    m_operation_state = -1; //останов КА - операции выполнены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
    return true; //операции выполнены
   }
  }
  break;
 }//switch

 return false; //КА продолжает работу...
}

void CPMParamsController::SetFunctionsNames(const std::vector<_TSTRING>& i_names)
{
 mp_view->SetFunctionsNames(i_names);
}

void CPMParamsController::ApplyFWOptions(DWORD opt)
{
 mp_view->EnableIgnitionCogs(!CHECKBIT32(opt, COPT_DWELL_CONTROL) && !CHECKBIT32(opt, COPT_CKPS_2CHIGN));
 mp_view->EnableUseVentPwm(CHECKBIT32(opt, COPT_COOLINGFAN_PWM));
 mp_view->EnableUseCTSCurveMap(CHECKBIT32(opt, COPT_THERMISTOR_CS));
 mp_view->EnableHallOutputParams(CHECKBIT32(opt, COPT_HALL_OUTPUT) && !CHECKBIT32(opt, COPT_HALL_SYNC) && !CHECKBIT32(opt, COPT_CKPS_NPLUS1));
 mp_view->EnableSECU3TItems(CHECKBIT32(opt, COPT_SECU3T));
 if (CHECKBIT32(opt, COPT_SECU3T))
  //SECU-3T has two additional ignition outputs
  mp_view->SetMaxCylinders(CHECKBIT32(opt, COPT_PHASED_IGNITION) ? 8 : 8);
 else
  mp_view->SetMaxCylinders(CHECKBIT32(opt, COPT_PHASED_IGNITION) ? 4 : 8);

 //in full-sequential ignition mode odd cylinder number engines are also supported,
 //aslo if hall sensor synchronization is used
 mp_view->EnableOddCylinders(CHECKBIT32(opt, COPT_PHASED_IGNITION) || CHECKBIT32(opt, COPT_HALL_SYNC));

 mp_view->EnableChokeTesting(CHECKBIT32(opt, COPT_SM_CONTROL));
 mp_view->EnableChokeManPos(CHECKBIT32(opt, COPT_SM_CONTROL));
 mp_view->EnableGasdoseTesting(CHECKBIT32(opt, COPT_GD_CONTROL));
 mp_view->EnableGasdoseManPos(CHECKBIT32(opt, COPT_GD_CONTROL));
 mp_view->EnableCKPSItems(!CHECKBIT32(opt, COPT_HALL_SYNC) && !CHECKBIT32(opt, COPT_CKPS_NPLUS1));
 mp_view->EnableHallWndWidth(CHECKBIT32(opt, COPT_HALL_SYNC) || CHECKBIT32(opt, COPT_CKPS_NPLUS1));
 mp_view->EnableInputsMerging(!CHECKBIT32(opt, COPT_CKPS_2CHIGN));
 mp_view->EnableRisingSpark(CHECKBIT32(opt, COPT_DWELL_CONTROL) && !CHECKBIT32(opt, COPT_CKPS_2CHIGN));
 mp_view->EnableFuelInjection(CHECKBIT32(opt, COPT_FUEL_INJECT)); 
 mp_view->EnableLambda(CHECKBIT32(opt, COPT_FUEL_INJECT) || CHECKBIT32(opt, COPT_CARB_AFR) || CHECKBIT32(opt, COPT_GD_CONTROL));
 mp_view->EnableGasdose(CHECKBIT32(opt, COPT_GD_CONTROL)); //GD
 mp_view->EnableChoke(CHECKBIT32(opt, COPT_SM_CONTROL)); 
 mp_view->EnableChokeCtrls(!CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT));
}

//from view. Очередная вкладка активировалась
void CPMParamsController::OnParamDeskTabActivate(void)
{
 //после появления новой вкладки необходимо прочитать для нее параметры из SECU
 m_RequestDataCollection();
}

//from view. Данные на вкладке изменились
void CPMParamsController::OnParamDeskChangeInTab(void)
{
 m_parameters_changed = true;
}

//from view. Нажали кнопку сохранения параметров - надо послать команду сохранения в SECU-3
void CPMParamsController::OnPDSaveButton()
{
 mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_WRITING_PARAMS));
 OPCompNc packet_data;
 packet_data.opcode = OPCODE_EEPROM_PARAM_SAVE;
 mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
}

//передача пакетов с параметрами в SECU будет происходить не чаще чем вызов этого обработчика
void CPMParamsController::OnParamDeskChangesTimer(void)
{
 if (m_parameters_changed)
 {
  //получаем данные от view и сохраняем их во временный буфер
  SECU3Packet packet_data;
  BYTE view_descriptor = mp_view->GetCurrentDescriptor();
  mp_view->GetValues(view_descriptor, &packet_data);

  //послали измененные пользователем данные (эта операция блокирует поток, поэтому за данные в стеке можно не беспокоиться)
  mp_comm->m_pControlApp->SendPacket(view_descriptor, &packet_data);
  if (view_descriptor == CHOKE_PAR)
  {
   packet_data.m_ChokePar.manual_delta = 0; //reset accumulated value
   mp_view->LockUIUpdate(view_descriptor, true);
   mp_view->SetValues(view_descriptor, &packet_data);
   mp_view->LockUIUpdate(view_descriptor, false);
  }
  if (view_descriptor == GASDOSE_PAR)
  {
   packet_data.m_GasdosePar.manual_delta = 0; //reset accumulated value
   mp_view->LockUIUpdate(view_descriptor, true);
   mp_view->SetValues(view_descriptor, &packet_data);
   mp_view->LockUIUpdate(view_descriptor, false);
  }


  m_parameters_changed = false; //обработали событие - сбрасываем признак
 }
}
