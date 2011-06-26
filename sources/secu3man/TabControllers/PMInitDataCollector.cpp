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

#include "stdafx.h"
#include "Resources/resource.h"
#include "PMInitDataCollector.h"

#include <algorithm>
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"

using namespace fastdelegate;
using namespace SECU3IO;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPMInitDataCollector::CPMInitDataCollector(CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar)
: mp_comm(ip_comm)
, mp_sbar(ip_sbar)
{
 //empty
}

CPMInitDataCollector::~CPMInitDataCollector()
{
 //empty
}

const std::vector<_TSTRING>& CPMInitDataCollector::GetFNNames(void) const
{
 return fn_names;
}

const DWORD& CPMInitDataCollector::GetFWOptions(void) const
{
 return fw_options;
}

void CPMInitDataCollector::StartDataCollection(void)
{
 mp_comm->m_pControlApp->ChangeContext(FNNAME_DAT); //change context!
 Super::StartDataCollection();
}

//возвращает true когда работа автомата завершена
//m_operation_state = 0 для запуска
bool CPMInitDataCollector::CollectData(const BYTE i_descriptor, const void* i_packet_data)
{
 const FnNameDat* fn_data = NULL;

 //state machine
 switch(m_operation_state)
 {
  case 0: //пытаемся добиться от SECU-3 чтобы оно посылало пакеты с информацией о семействах характеристик
  {
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_CONFIGURATION));

   if (i_descriptor!=FNNAME_DAT)
   {
    mp_comm->m_pControlApp->ChangeContext(FNNAME_DAT);
   }
   else
   { //контроллер шлет информацию о семействах характеристик
    m_operation_state = 1;

    fn_data = (FnNameDat*)i_packet_data;

    fn_names.clear();
    fn_indexes.clear();
    fn_names.resize(fn_data->tables_num);

    //данные в SECU хранятся в виде ASCII, их надо преобразовать в UNICODE
    TCHAR name_string[256];
    OemToChar(fn_data->name,name_string);
    fn_names[fn_data->index] = name_string;

    fn_indexes.push_back(fn_data->index);
   }
  }
  break;

  case 1: //принимает (продолжает принимать) и сохраняем информацию о всех семействах характеристик
  {
   if (i_descriptor!=FNNAME_DAT)
   {
    m_operation_state = 0;
    break;
   }

   fn_data = (FnNameDat*)i_packet_data;

   TCHAR name_string[256];
   OemToChar(fn_data->name,name_string);
   fn_names[fn_data->index] = name_string;

   fn_indexes.push_back(fn_data->index);

   bool all_names_received = true;
   for(int i = 0; i < fn_data->tables_num; i++)
   {
    if (std::find(fn_indexes.begin(),fn_indexes.end(),i)==fn_indexes.end())
     all_names_received = false;
   }

   if (all_names_received) //вся информация о семейтвах характеристик принята
    m_operation_state = 2; //следующая операция
  }
  break;

  case 2: //чтение параметров компиляции прошивки
   {
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_FWC_OPTIONS));
    BYTE ci_descriptor = FWINFO_DAT;
    if (i_descriptor != ci_descriptor)
    {
     mp_comm->m_pControlApp->ChangeContext(ci_descriptor);
    }
    else
    {//тот что надо!
     const FWInfoDat* fw_info = (const FWInfoDat*)i_packet_data;
     fw_options = fw_info->options;

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
