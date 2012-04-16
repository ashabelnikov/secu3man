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

#include <map>
#include "io-core/FirmwareDataMediator.h"

class CIORemappingDlg;

class CFWIORemappingController
{
  typedef CIORemappingDlg IORVIEW;
  typedef CFirmwareDataMediator FWDM;

 public:
  CFWIORemappingController(IORVIEW* ip_view);
  virtual ~CFWIORemappingController();

  void AttachFWDM(FWDM* ip_fwdm);

  //начало работы контроллера
  void OnActivate(void);

  //конец работы контроллера
  void OnDeactivate(void);

  void Enable(bool state);

  void EnableSECU3TFeatures(bool i_enable);

 private:
  bool _CheckErrors(void);
  void _UpdateView(void);
  void _AttachPlug(FWDM::IOPid iopId);
  void _AttachPlug(FWDM::IOPid iopId, FWDM::IOSid iosId);
  void _DetachPlugsFromSlot(FWDM::IOSid iosId);
  void OnItemSelected(FWDM::IOSid iosId, FWDM::IOPid iopId);

  CIORemappingDlg* mp_view;
  FWDM* mp_fwdm;
  std::map<FWDM::IOPid, FWDM::IOSid> m_defValMap;
};
