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

#include <vector>
#include "PMControllerBase.h"

class CCommunicationManager;
class CStatusBarManager;

class CPMInitDataCollector : public CPMStateMachineState
{
  typedef CPMStateMachineState Super;
 public:
  CPMInitDataCollector(CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar);
  virtual ~CPMInitDataCollector();

  virtual void StartDataCollection(void);

  //„тение всех необходимых данных после коннекта (конечный автомат)
  //возвращает true когда все данные прочитаны  
  virtual bool CollectData(const BYTE i_descriptor, const void* i_packet_data);

  //obtain results
  const std::vector<_TSTRING>& GetFNNames(void) const;
  const DWORD& GetFWOptions(void) const;

 private:
  CCommunicationManager* mp_comm;
  CStatusBarManager* mp_sbar;

  std::vector<_TSTRING> fn_names;
  std::vector<int> fn_indexes;
  DWORD fw_options; //биты опций прошивки
};
