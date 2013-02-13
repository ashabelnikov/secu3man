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
#include <math.h>
#include "Resources/resource.h"
#include "FWRPMGridEditController.h"

#include "common/fastdelegate.h"
#include "io-core/FirmwareDataMediator.h"
#include "io-core/SECU3IO.h"
#include "TablDesk/RPMGridEditDlg.h"

using namespace fastdelegate;

CFWRPMGridEditController::CFWRPMGridEditController()
{
 //empty
}

CFWRPMGridEditController::~CFWRPMGridEditController()
{
 //empty
}

void CFWRPMGridEditController::AttachFWDM(FWDM* ip_fwdm)
{
 mp_fwdm = ip_fwdm;
 ASSERT(mp_fwdm);
}

int CFWRPMGridEditController::Edit(void)
{
 mp_view.reset(new CRPMGridEditDlg());
 mp_view->setOnChange(MakeDelegate(this, &CFWRPMGridEditController::OnItemChange));
 mp_view->setOnLoadDefVal(MakeDelegate(this, &CFWRPMGridEditController::OnLoadDefVal));
 float values[16] = {0};
 mp_fwdm->GetRPMGridMap(values);
 mp_view->SetValues(values);
 int result = mp_view->DoModal();
 if (result==IDOK)
 {
  mp_view->GetValues(values);
  mp_fwdm->SetRPMGridMap(values);
 }
 mp_view.reset();
 return result;
}

void CFWRPMGridEditController::OnItemChange(size_t itemIndex, float value)
{
 if (!mp_view.get())
  return;

 if (value < 600)
 {
  mp_view->SetErrMessage(_T(""));
  mp_view->SetErrMessage("Значения не могут быть меньше 600");
  mp_view->SetItemError(itemIndex, true);
 }
 else if (value > 7500)
 {
  mp_view->SetErrMessage(_T(""));
  mp_view->SetErrMessage("Значения не могут быть больше 7500");
  mp_view->SetItemError(itemIndex, true); 
 }
 else if ((itemIndex < 15) ? (value > mp_view->GetValue(itemIndex+1)) : 0)
 {
  mp_view->SetErrMessage(_T(""));
  mp_view->SetErrMessage("Предыдущее знач. не может быть больше следующего");
  mp_view->SetItemError(itemIndex, true);  
 }
 else if (((itemIndex < 15) ? (100 > fabs(value - mp_view->GetValue(itemIndex+1))) : 0) ||
          ((itemIndex >  0) ? (100 > fabs(value - mp_view->GetValue(itemIndex-1))) : 0))
 {
  mp_view->SetErrMessage(_T(""));
  mp_view->SetErrMessage("Шаг сетки не может быть меньше 100");
  mp_view->SetItemError(itemIndex, true);   
 }
 else
 {
  mp_view->SetErrMessage(_T(""));
  mp_view->SetItemError(itemIndex, false); 
 }
}

void CFWRPMGridEditController::OnLoadDefVal(void)
{
 if (!mp_view.get())
  return;
 mp_view->SetValues(SECU3IO::idle_map_rpm_slots);
 mp_view->SetErrMessage(_T(""));
 for(size_t i = 0; i < 16; i++)
  mp_view->SetItemError(i, false);
}
