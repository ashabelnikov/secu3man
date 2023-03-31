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

/** \file FWRPMGridEditController.cpp
 * \author Alexey A. Shabelnikov
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

static const size_t itemNumberRPM = 16;
static const size_t itemNumberCLT = 16;
static const size_t itemNumberLod = 16;
static const float gridMinStep[3] = {100, 5, 1};
static const float gridMinValue[3] = {300, -40, 0};
static const float gridMaxValue[3] = {20000, 250, 500};

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
 float valuesRPM[itemNumberRPM] = {0};
 mp_fwdm->GetRPMGridMap(valuesRPM);
 mp_view->SetValues(0, valuesRPM);
 float valuesCLT[itemNumberCLT] = {0};
 mp_fwdm->GetCLTGridMap(valuesCLT);
 mp_view->SetValues(1, valuesCLT);
 float valuesLod[itemNumberLod] = {0};
 mp_fwdm->GetLoadGridMap(valuesLod);
 mp_view->SetValues(2, valuesLod, false);

 int result = mp_view->DoModal();
 if (result==IDOK)
 {
  mp_view->GetValues(0, valuesRPM);
  mp_fwdm->SetRPMGridMap(valuesRPM);
  mp_view->GetValues(1, valuesCLT);
  mp_fwdm->SetCLTGridMap(valuesCLT);
  mp_view->GetValues(2, valuesLod, false);
  mp_fwdm->SetLoadGridMap(valuesLod);
 }
 mp_view.reset();
 return result;
}

bool CFWRPMGridEditController::_CheckItemForErrors(int mode, size_t itemIndex, float value, bool i_check_only /*= false*/)
{
 bool error = true;
 size_t itemNumber = -1;
 if (0==mode)
  itemNumber = itemNumberRPM;
 else if (1==mode)
  itemNumber = itemNumberCLT;
 else if (2==mode)
  itemNumber = itemNumberLod;
 else
 {
  ASSERT(0); //unexpected value
  return false;
 }

 if (value < gridMinValue[mode])
 {
  if (!i_check_only)
  {
   _TSSTREAM s;
   s << MLL::GetString(IDS_RGE_ERROR_RPM_LESS_MIN);
   s << gridMinValue[mode];
   m_errors[mode].push_back(std::make_pair(itemIndex, s.str().c_str()));
  }
 }
 else if (value > gridMaxValue[mode])
 {
  if (!i_check_only)
  {
   _TSSTREAM s;
   s << MLL::GetString(IDS_RGE_ERROR_RPM_ABOVE_MAX);
   s << gridMaxValue[mode];
   m_errors[mode].push_back(std::make_pair(itemIndex, s.str().c_str()));
  }
 }
 else if (((itemIndex < itemNumber-1) ? (gridMinStep[mode] > fabs(value - mp_view->GetValue(mode, itemIndex+1))) : 0) ||
          ((itemIndex >  0) ? (gridMinStep[mode] > fabs(value - mp_view->GetValue(mode, itemIndex-1))) : 0))
 {
  if (!i_check_only)
  {
   _TSSTREAM s;
   s << MLL::GetString(IDS_RGE_ERROR_RPM_STEP_LESS_MIN);
   s << gridMinStep[mode];
   m_errors[mode].push_back(std::make_pair(itemIndex, s.str().c_str()));
  }
 }
 else if (((itemIndex < itemNumber-1) ? (value > mp_view->GetValue(mode, itemIndex+1)) : 0) ||
          ((itemIndex >  0) ? (value < mp_view->GetValue(mode, itemIndex-1)) : 0))
 {
  if (!i_check_only)
   m_errors[mode].push_back(std::make_pair(itemIndex, MLL::GetString(IDS_RGE_ERROR_RPM_VAL_ORDER).c_str()));
 }
 else
  error = false; //Ok

 return error;
}

void CFWRPMGridEditController::OnItemChange(int mode, size_t itemIndex, float value)
{
 if (!mp_view.get())
  return;

 //Check changed item for errors
 _CheckItemForErrors(mode, itemIndex, value);

 //Recheck all present errors and remove items which are OK
 std::list<std::pair<size_t, _TSTRING> >::iterator it = m_errors[mode].begin();
 while(it!=m_errors[mode].end())
 {
  if (!_CheckItemForErrors(mode, it->first, mp_view->GetValue(mode, it->first), true)) //only check
  {//OK
   mp_view->SetItemError(mode, it->first, false);  
   it = m_errors[mode].erase(it);
  }
  else //next error
   ++it;
 }
 
 //Update view
 for(it = m_errors[mode].begin(); it != m_errors[mode].end(); ++it)
  mp_view->SetItemError(mode, it->first, true);

 mp_view->SetErrMessage(mode, _T("")); //erase message
 if (m_errors[mode].size())
  mp_view->SetErrMessage(mode, m_errors[mode].rbegin()->second);
}

void CFWRPMGridEditController::OnLoadDefVal(int mode)
{
 size_t itemNumber = -1;
 if (0==mode)
  itemNumber = itemNumberRPM;
 else if (1==mode)
  itemNumber = itemNumberCLT;
 else if (2==mode)
  itemNumber = itemNumberLod;
 else
 {
  ASSERT(0); //unexpected value
  return;
 }

 if (!mp_view.get())
  return;

 if (0==mode)
  mp_view->SetValues(mode, SECU3IO::work_map_rpm_slots);
 else if (1==mode)
  mp_view->SetValues(mode, SECU3IO::temp_map_tmp_slots);
 else if (2==mode)
  mp_view->SetValues(mode, SECU3IO::work_map_lod_slots, false);

 mp_view->SetErrMessage(mode, _T(""));
 for(size_t i = 0; i < itemNumber; i++)
  mp_view->SetItemError(mode, i, false);
 m_errors[mode].clear(); //reset errors
}

bool CFWRPMGridEditController::IsOkEnabled(void)
{
 for(size_t j = 0; j < 2; ++j)
 {
  if (m_errors[j].size())
   return false; //Ok button will be enabled if errors' stack is empty
 }
 return true; //no errors
}
