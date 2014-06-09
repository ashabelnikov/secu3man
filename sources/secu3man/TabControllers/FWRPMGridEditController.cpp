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
#include <algorithm>
#include <math.h>
#include <sstream>
#include "Resources/resource.h"
#include "FWRPMGridEditController.h"

#include "common/fastdelegate.h"
#include "common/LangLayer.h"
#include "io-core/FirmwareDataMediator.h"
#include "io-core/SECU3IO.h"
#include "TablDesk/RPMGridEditDlg.h"

using namespace fastdelegate;

static const size_t itemNumber = 16;
static const float gridMinStep = 100;
static const float gridMinValue = 600;
static const float gridMaxValue = 12000;

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
 mp_view->setIsOkEnabled(MakeDelegate(this, &CFWRPMGridEditController::IsOkEnabled));
 float values[itemNumber] = {0};
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

bool CFWRPMGridEditController::_CheckItemForErrors(size_t itemIndex, float value, bool i_check_only /*= false*/)
{
 bool error = true;
 if (value < gridMinValue)
 {
  if (!i_check_only)
  {
   _TSSTREAM s;
   s << MLL::GetString(IDS_RGE_ERROR_RPM_LESS_MIN);
   s << gridMinValue;
   m_errors.push_back(std::make_pair(itemIndex, s.str().c_str()));
  }
 }
 else if (value > gridMaxValue)
 {
  if (!i_check_only)
  {
   _TSSTREAM s;
   s << MLL::GetString(IDS_RGE_ERROR_RPM_ABOVE_MAX);
   s << gridMaxValue;
   m_errors.push_back(std::make_pair(itemIndex, s.str().c_str()));
  }
 }
 else if (((itemIndex < itemNumber-1) ? (gridMinStep > fabs(value - mp_view->GetValue(itemIndex+1))) : 0) ||
          ((itemIndex >  0) ? (gridMinStep > fabs(value - mp_view->GetValue(itemIndex-1))) : 0))
 {
  if (!i_check_only)
  {
   _TSSTREAM s;
   s << MLL::GetString(IDS_RGE_ERROR_RPM_STEP_LESS_MIN);
   s << gridMinStep;
   m_errors.push_back(std::make_pair(itemIndex, s.str().c_str()));
  }
 }
 else if (((itemIndex < itemNumber-1) ? (value > mp_view->GetValue(itemIndex+1)) : 0) ||
          ((itemIndex >  0) ? (value < mp_view->GetValue(itemIndex-1)) : 0))
 {
  if (!i_check_only)
   m_errors.push_back(std::make_pair(itemIndex, MLL::GetString(IDS_RGE_ERROR_RPM_VAL_ORDER).c_str()));
 }
 else
  error = false; //Ok

 return error;
}

void CFWRPMGridEditController::OnItemChange(size_t itemIndex, float value)
{
 if (!mp_view.get())
  return;

 //Check changed item for errors
 _CheckItemForErrors(itemIndex, value);

 //Recheck all present errors and remove items which are OK
 std::list<std::pair<size_t, _TSTRING> >::iterator it = m_errors.begin();
 while(it!=m_errors.end())
 {
  if (!_CheckItemForErrors(it->first, mp_view->GetValue(it->first), true)) //only check
  {//OK
   mp_view->SetItemError(it->first, false);  
   it = m_errors.erase(it);
  }
  else //next error
   ++it;
 }

 //Update view
 for(it = m_errors.begin(); it != m_errors.end(); ++it)
  mp_view->SetItemError(it->first, true);
 mp_view->SetErrMessage(_T("")); //erase message
 if (m_errors.size())
  mp_view->SetErrMessage(m_errors.rbegin()->second);
}

void CFWRPMGridEditController::OnLoadDefVal(void)
{
 if (!mp_view.get())
  return;
 mp_view->SetValues(SECU3IO::work_map_rpm_slots);
 mp_view->SetErrMessage(_T(""));
 for(size_t i = 0; i < itemNumber; i++)
  mp_view->SetItemError(i, false);
 m_errors.clear(); //reset errors
}

bool CFWRPMGridEditController::IsOkEnabled(void)
{
 return 0==m_errors.size(); //Ok button will be enabled if errors' stack is empty
}
