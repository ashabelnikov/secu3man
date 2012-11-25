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

#include "common/FastDelegate.h"
#include "common/unicodesupport.h"
#include "IDeskView.h"
#include "io-core/FirmwareDataMediator.h"

//Interface for IO remapping View
class IRemapDeskView : public IDeskView
{
 public:
  typedef CFirmwareDataMediator FWDM;
  typedef fastdelegate::FastDelegate2<FWDM::IOSid, FWDM::IOPid> EventItemSel;
  typedef fastdelegate::FastDelegate2<FWDM::IOSid, bool> EventItemInv;

  virtual bool AddItem(FWDM::IOSid iosId, FWDM::IOPid iopId, const _TSTRING& i_text) = 0;
  virtual bool SelectItem(FWDM::IOSid iosId, FWDM::IOPid iopId) = 0;
  virtual FWDM::IOPid GetSelection(FWDM::IOSid iosId) const = 0;
  virtual void setOnItemSelected(EventItemSel OnFunction) = 0;
  virtual void EnableSECU3TItems(bool i_enable) = 0;
  virtual void EnableItem(FWDM::IOSid iosId, bool i_enable) = 0;
  virtual void EnableInversion(FWDM::IOSid iosId, bool i_invert) = 0;
  virtual void ResetContent(void) = 0;
  virtual bool InvertItem(FWDM::IOSid iosId, bool i_invert) = 0;
  virtual void setOnItemInverted(EventItemInv OnFunction) = 0;
};
