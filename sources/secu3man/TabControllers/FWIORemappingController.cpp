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

#include "stdafx.h"
#include "Resources/resource.h"
#include "FWIORemappingController.h"

#include "common/fastdelegate.h"
#include "ParamDesk/Params/IORemappingDlg.h"

using namespace fastdelegate;
typedef CFirmwareDataMediator FWDM;

//#define IOREMCNTR_DEBUG

CFWIORemappingController::CFWIORemappingController(IORVIEW* ip_view)
: mp_view(ip_view)
, mp_fwdm(NULL)
{
 mp_view->setOnItemSelected(MakeDelegate(this, &CFWIORemappingController::OnItemSelected));
 mp_view->setOnItemInverted(MakeDelegate(this, &CFWIORemappingController::OnItemInverted));
}

CFWIORemappingController::~CFWIORemappingController()
{
 //empty
}

void CFWIORemappingController::AttachFWDM(FWDM* ip_fwdm)
{
 mp_fwdm = ip_fwdm;
 ASSERT(mp_fwdm);

 _PrepareLogic();

 _CheckForErrors();

 _EnableInversionItems();

 _UpdateView();
}

void CFWIORemappingController::OnActivate(void)
{
 _PrepareLogic();
}

void CFWIORemappingController::OnDeactivate(void)
{
 mp_view->ResetContent();
}

void CFWIORemappingController::Enable(bool state)
{
 mp_view->Enable(state);
}

void CFWIORemappingController::EnableSECU3TFeatures(bool i_enable)
{
 mp_view->EnableSECU3TItems(i_enable);
}

void CFWIORemappingController::OnItemSelected(FWDM::IOSid iosId, FWDM::IOPid iopId)
{
 //Detach plugs from specified slot
 _DetachPlugsFromSpecifiedSlot(iosId);

 //Attach a selected plug to slot
 _AttachPlugToSpecifiedSlot(iopId, iosId);

 //Not connected plugs must be connected to default free slots
 _AttachFreeSlotsToDefaultPlugs();

 //For debug purposes
 _DisplayPlugs();

 //Enable inversion check boxes
 _EnableInversionItems();

 //Update UI
 _UpdateView();
}

void CFWIORemappingController::OnItemInverted(FWDM::IOSid iosId, bool i_invert)
{
 _SetInvFlag(iosId, i_invert); //update inversion flags

 //Get ID of plug currently connected to slot with identified by iosId
 FWDM::IOPid iopId = mp_view->GetSelection(iosId);

 //reattach corresponding plug 
 _AttachPlugToSpecifiedSlot(iopId, iosId);
}

void CFWIORemappingController::_PrepareLogic(void)
{
 if (!mp_fwdm)
  return; //Can't prepare when FWDM is not attached

 //Set inversion flags to default values
 m_invFlags.clear();
 for(int s = FWDM::IOS_START; s < FWDM::IOS_COUNT; ++s)
  m_invFlags.insert(std::make_pair((FWDM::IOSid)s, false));

 m_defValMap.clear();
 m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT1, FWDM::IOP_IGN_OUT1));
 m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT2, FWDM::IOP_IGN_OUT2));
 m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3));
 m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4));
 m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1));
 m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_IO2, FWDM::IOP_ADD_IO2));
 m_defValMap.insert(std::make_pair(FWDM::IOS_ECF, FWDM::IOP_ECF));
 m_defValMap.insert(std::make_pair(FWDM::IOS_ST_BLOCK, FWDM::IOP_ST_BLOCK));
 m_defValMap.insert(std::make_pair(FWDM::IOS_IE, FWDM::IOP_IE));
 m_defValMap.insert(std::make_pair(FWDM::IOS_FE, FWDM::IOP_FE));
 m_defValMap.insert(std::make_pair(FWDM::IOS_PS, FWDM::IOP_PS));
 m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_I1, FWDM::IOP_ADD_I1));
 m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_I2, FWDM::IOP_ADD_I2));
 m_defValMap.insert(std::make_pair(FWDM::IOS_CE, FWDM::IOP_CE));
 m_defValMap.insert(std::make_pair(FWDM::IOS_BL, FWDM::IOP_BL));
 m_defValMap.insert(std::make_pair(FWDM::IOS_DE, FWDM::IOP_DE));
 m_defValMap.insert(std::make_pair(FWDM::IOS_REF_S, FWDM::IOP_REF_S));
 m_defValMap.insert(std::make_pair(FWDM::IOS_GAS_V, FWDM::IOP_GAS_V));

 //Fill view with values
 mp_view->ResetContent();
 mp_view->AddItem(FWDM::IOS_IGN_OUT1, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT1, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT1, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT1, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT1, FWDM::IOP_IGN_OUT1, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_IGN_OUT1, true);
 mp_view->EnableInversion(FWDM::IOS_IGN_OUT1, true);

 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_IGN_OUT2, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_IGN_OUT2, true);
 mp_view->EnableInversion(FWDM::IOS_IGN_OUT2, true);

 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_IGN_OUT3, true);
 mp_view->EnableInversion(FWDM::IOS_IGN_OUT3, true);

 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_IGN_OUT4, true);
 mp_view->EnableInversion(FWDM::IOS_IGN_OUT4, true);

 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_ADD_IO1, true);
 mp_view->EnableInversion(FWDM::IOS_ADD_IO1, true);

 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_ADD_IO2, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_ADD_IO2, true);
 mp_view->EnableInversion(FWDM::IOS_ADD_IO2, true);

 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_IGN_OUT7, _T("IGN_OUT7"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_IGN_OUT8, _T("IGN_OUT8"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_IAC_PWM, _T("IAC_PWM"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_ECF, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_ECF, true);
 mp_view->EnableInversion(FWDM::IOS_ECF, true);

 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_IGN_OUT7, _T("IGN_OUT7"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_IGN_OUT8, _T("IGN_OUT8"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_ST_BLOCK, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_ST_BLOCK, true);
 mp_view->EnableInversion(FWDM::IOS_ST_BLOCK, true);

 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IGN_OUT7, _T("IGN_OUT7"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IGN_OUT8, _T("IGN_OUT8"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IE, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_IE, true);
 mp_view->EnableInversion(FWDM::IOS_IE, true);

 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_IGN_OUT7, _T("IGN_OUT7"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_IGN_OUT8, _T("IGN_OUT8"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FE, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_FE, true); 
 mp_view->EnableInversion(FWDM::IOS_FE, true);
 //PS input:
 mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_IGN, _T("IGN"));
 mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_BC_INPUT, _T("BC_INPUT"));
 mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_MAPSEL0, _T("MAPSEL0"));
 mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_SPDSENS, _T("SPD_SENS"));
 mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_PS, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_PS, true); 
 mp_view->EnableInversion(FWDM::IOS_PS, true);

 //CE
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_IGN_OUT7, _T("IGN_OUT7"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_IGN_OUT8, _T("IGN_OUT8"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_INJ_OUT0, _T("INJ_OUT1"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_INJ_OUT1, _T("INJ_OUT2"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_INJ_OUT2, _T("INJ_OUT3"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_INJ_OUT3, _T("INJ_OUT4"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_CE, FWDM::IOP_CE, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_CE, true);
 mp_view->EnableInversion(FWDM::IOS_CE, true);

 //BL
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_IGN_OUT7, _T("IGN_OUT7"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_IGN_OUT8, _T("IGN_OUT8"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_BL, FWDM::IOP_BL, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_BL, true);
 mp_view->EnableInversion(FWDM::IOS_BL, true);

 //DE
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_STROBE, _T("STROBE"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_IGN_OUT7, _T("IGN_OUT7"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_IGN_OUT8, _T("IGN_OUT8"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_SM_DIR, _T("SM_DIR"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_SM_STP, _T("SM_STP"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_INTK_HEAT, _T("INTK_HEAT"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_UNI_OUT0, _T("UNI_OUT1"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_UNI_OUT1, _T("UNI_OUT2"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_UNI_OUT2, _T("UNI_OUT3"));
 mp_view->AddItem(FWDM::IOS_DE, FWDM::IOP_DE, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_DE, true);
 mp_view->EnableInversion(FWDM::IOS_DE, true);

 //ADD_I1
 mp_view->AddItem(FWDM::IOS_ADD_I1, FWDM::IOP_IGN, _T("IGN"));
 mp_view->AddItem(FWDM::IOS_ADD_I1, FWDM::IOP_BC_INPUT, _T("BC_INPUT"));
 mp_view->AddItem(FWDM::IOS_ADD_I1, FWDM::IOP_MAPSEL0, _T("MAPSEL0"));
 mp_view->AddItem(FWDM::IOS_ADD_I1, FWDM::IOP_LAMBDA, _T("LAMBDA"));
//mp_view->AddItem(FWDM::IOS_ADD_I1, FWDM::IOP_AIR_TEMP, _T("AIR_TEMP"));
 mp_view->AddItem(FWDM::IOS_ADD_I1, FWDM::IOP_ADD_I1, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_ADD_I1, true); 
 mp_view->EnableInversion(FWDM::IOS_ADD_I1, true);

 //ADD_I2
 mp_view->AddItem(FWDM::IOS_ADD_I2, FWDM::IOP_IGN, _T("IGN"));
 mp_view->AddItem(FWDM::IOS_ADD_I2, FWDM::IOP_BC_INPUT, _T("BC_INPUT"));
 mp_view->AddItem(FWDM::IOS_ADD_I2, FWDM::IOP_MAPSEL0, _T("MAPSEL0"));
 //mp_view->AddItem(FWDM::IOS_ADD_I2, FWDM::IOP_LAMBDA, _T("LAMBDA"));
 mp_view->AddItem(FWDM::IOS_ADD_I2, FWDM::IOP_AIR_TEMP, _T("AIR_TEMP"));
 mp_view->AddItem(FWDM::IOS_ADD_I2, FWDM::IOP_ADD_I2, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_ADD_I2, true); 
 mp_view->EnableInversion(FWDM::IOS_ADD_I2, true);

 //REF_S
 mp_view->AddItem(FWDM::IOS_REF_S, FWDM::IOP_IGN, _T("IGN"));
 mp_view->AddItem(FWDM::IOS_REF_S, FWDM::IOP_PS, _T("PS"));
 mp_view->AddItem(FWDM::IOS_REF_S, FWDM::IOP_BC_INPUT, _T("BC_INPUT"));
 mp_view->AddItem(FWDM::IOS_REF_S, FWDM::IOP_MAPSEL0, _T("MAPSEL0"));
 mp_view->AddItem(FWDM::IOS_REF_S, FWDM::IOP_SPDSENS, _T("SPD_SENS"));
 mp_view->AddItem(FWDM::IOS_REF_S, FWDM::IOP_REF_S, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_REF_S, true); 
 mp_view->EnableInversion(FWDM::IOS_REF_S, true);

 //GAS_V
 mp_view->AddItem(FWDM::IOS_GAS_V, FWDM::IOP_IGN, _T("IGN"));
 mp_view->AddItem(FWDM::IOS_GAS_V, FWDM::IOP_BC_INPUT, _T("BC_INPUT"));
 mp_view->AddItem(FWDM::IOS_GAS_V, FWDM::IOP_MAPSEL0, _T("MAPSEL0"));
 mp_view->AddItem(FWDM::IOS_GAS_V, FWDM::IOP_GAS_V, _T("NONE"));
 mp_view->EnableItem(FWDM::IOS_GAS_V, true); 
 mp_view->EnableInversion(FWDM::IOS_GAS_V, true);
}

//Note that this function uses default inversion values from m_invFlags map.
void CFWIORemappingController::_AttachFreeSlotsToDefaultPlugs(void)
{
 std::map<FWDM::IOPid, FWDM::IOSid> attachList;
 //build list of free slots which have to be attached to default plugs
 for(int s = FWDM::IOS_START; s < FWDM::IOS_COUNT; ++s)
 {
  if (!_IsSlotFree((FWDM::IOSid)s))
   continue;
  //костыль для возможности переназначения IGN_OUT3 и IGN_OUT4 на другие выходы
  if (((FWDM::IOSid)s == FWDM::IOS_IGN_OUT3) && (mp_fwdm->GetSStub() != mp_fwdm->GetIOPlug(FWDM::IOX_INIT, FWDM::IOP_IGN_OUT3)))
   continue; //3
  if (((FWDM::IOSid)s == FWDM::IOS_IGN_OUT4) && (mp_fwdm->GetSStub() != mp_fwdm->GetIOPlug(FWDM::IOX_INIT, FWDM::IOP_IGN_OUT4)))
   continue; //4
  //hack which allows remap PS input to other input slots
  if (((FWDM::IOSid)s == FWDM::IOS_PS) && (mp_fwdm->GetSStub() != mp_fwdm->GetIOPlug(FWDM::IOX_INIT, FWDM::IOP_PS)))
   continue; //PS
  std::map<FWDM::IOSid, FWDM::IOPid>::const_iterator it = m_defValMap.find((FWDM::IOSid)s);
  if (it != m_defValMap.end())
   attachList.insert(std::make_pair(it->second, (FWDM::IOSid)s));
 }
 //attach
 std::map<FWDM::IOPid, FWDM::IOSid>::const_iterator it = attachList.begin();
 for(; it != attachList.end(); ++it)
 {
  bool inv = m_invFlags[it->second];
  _AttachPlug(it->first, it->second, inv); //default
 }
}

bool CFWIORemappingController::_IsSlotFree(FWDM::IOSid iosId)
{
 for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
 {  
  DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
  if (plug == 0)
   continue; //skip reserved plugs
  if (_CmpPnS((FWDM::IOPid)p, iosId))
   return false; //already connected to one of plugs
 } 
 return true; //free!
}

void CFWIORemappingController::_DetachPlugsFromSpecifiedSlot(FWDM::IOSid iosId)
{
 for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
 {
  DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
  if (plug == 0)
   continue; //skip reserved plugs
  if(_CmpPnS((FWDM::IOPid)p, iosId))
   _AttachPlug((FWDM::IOPid)p); //stub
 }
}

void CFWIORemappingController::_UpdateView(void)
{
 for(int s = FWDM::IOS_START; s < FWDM::IOS_COUNT; ++s)
 {  
  DWORD slot[2] = {mp_fwdm->GetIOSlot(FWDM::IOX_INIT, (FWDM::IOSid)s, false), mp_fwdm->GetIOSlot(FWDM::IOX_INIT, (FWDM::IOSid)s, true)};
  bool itemSet = false;
  for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   //If slot == stub, then it is not allowed in this firmware
   if ((slot[0] != mp_fwdm->GetSStub()) && (slot[0] == plug))
   {
    mp_view->SelectItem((FWDM::IOSid)s, (FWDM::IOPid)p);
    mp_view->InvertItem((FWDM::IOSid)s, false);
    _SetInvFlag((FWDM::IOSid)s, false); //update inversion flags
    itemSet = true;
   }
   else if ((slot[1] != mp_fwdm->GetSStub()) && (slot[1] == plug))
   {
    mp_view->SelectItem((FWDM::IOSid)s, (FWDM::IOPid)p);
    mp_view->InvertItem((FWDM::IOSid)s, true);
    _SetInvFlag((FWDM::IOSid)s, true); //update inversion flags
    itemSet = true;
   }
  }
  if (!itemSet) {
   mp_view->SelectItem((FWDM::IOSid)s, FWDM::IOP_NA);
   mp_view->InvertItem((FWDM::IOSid)s, false);
   _SetInvFlag((FWDM::IOSid)s, false); //update inversion flags
  }
 }
}

void CFWIORemappingController::_AttachPlug(FWDM::IOPid iopId)
{
 mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetSStub());
 if (_IsIOPInput(iopId))
  mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetGStub()); //для входов своя заглушка, 
 else
  mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetSStub()); //для выходов своя
}

void CFWIORemappingController::_AttachPlugToSpecifiedSlot(FWDM::IOPid iopId, FWDM::IOSid iosId)
{
 bool inv = m_invFlags[iosId];
 _AttachPlug(iopId, iosId, inv);
}

void CFWIORemappingController::_AttachPlug(FWDM::IOPid iopId, FWDM::IOSid iosId, bool inv /*=false*/)
{
 mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId, inv));
 mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetIOSlot(FWDM::IOX_DATA, iosId, inv));
}

bool CFWIORemappingController::_IsIOPInput(FWDM::IOPid iopId) const
{
 return (iopId == FWDM::IOP_PS || iopId == FWDM::IOP_ADD_I1 || iopId == FWDM::IOP_ADD_I2 || iopId == FWDM::IOP_IGN || iopId == FWDM::IOP_BC_INPUT || iopId == FWDM::IOP_MAPSEL0 || iopId == FWDM::IOP_SPDSENS || iopId == FWDM::IOP_REF_S || iopId == FWDM::IOP_GAS_V || iopId == FWDM::IOP_LAMBDA || iopId == FWDM::IOP_AIR_TEMP);
}

bool CFWIORemappingController::_IsIOSInput(FWDM::IOSid iosId) const
{
 return (iosId == FWDM::IOS_PS || iosId == FWDM::IOS_ADD_I1 || iosId == FWDM::IOS_ADD_I2 || iosId == FWDM::IOS_REF_S || iosId == FWDM::IOS_GAS_V);
}

void CFWIORemappingController::_SetInvFlag(FWDM::IOSid iosId, bool inv)
{
 m_invFlags[iosId] = inv;
}

bool CFWIORemappingController::_CmpPnS(FWDM::IOPid pid, FWDM::IOSid sid)
{
 DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, pid);
 return (plug == mp_fwdm->GetIOSlot(FWDM::IOX_INIT, sid, false) ||
         plug == mp_fwdm->GetIOSlot(FWDM::IOX_INIT, sid, true));
}

bool CFWIORemappingController::_FixRedundantConnections(void)
{
 bool result = true;
 for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
 {
  for(int i = FWDM::IOX_INIT; i <= /*FWDM::IOX_DATA*/FWDM::IOX_INIT; ++i)
  {
   DWORD plug = mp_fwdm->GetIOPlug((FWDM::IOXtype)i, (FWDM::IOPid)p);
   if (plug == 0)
    continue;
   bool find = false;
   for(int pp = FWDM::IOP_START; pp < FWDM::IOP_COUNT; ++pp)
   {
    DWORD value = mp_fwdm->GetIOPlug((FWDM::IOXtype)i, (FWDM::IOPid)pp);
    if (value == 0 || value == mp_fwdm->GetSStub() || value == mp_fwdm->GetGStub())
     continue; //skip reserved plugs and stubs
    if (plug == value)
    {
     if (find)//redundant?
     {
      _AttachPlug((FWDM::IOPid)pp); //remove redundant connection
      result = false;
     }
     else
      find = true;
    }
   }
  }
 }
 return result;
}

bool CFWIORemappingController::_FixInputsVSOutputs(void)
{
 bool result = true;
 for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
 {
  int plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
  if (plug == 0 || plug == mp_fwdm->GetSStub())
   continue; //skip reserved plugs or plugs which are not utilized in SECU-3

  bool inv = false;  
  FWDM::IOSid sId = _GetConnectedSlot((FWDM::IOPid)p, true, inv); //init
  if ((sId != FWDM::IOS_NA) && ((_IsIOPInput((FWDM::IOPid)p) && !_IsIOSInput((FWDM::IOSid)sId)) ||
      (!_IsIOPInput((FWDM::IOPid)p) &&  _IsIOSInput((FWDM::IOSid)sId))))
  {
    _AttachPlug((FWDM::IOPid)p); //fix using a stub
    result = false;
  }
  inv = false;
  sId = _GetConnectedSlot((FWDM::IOPid)p, false, inv); //data
  if ((sId != FWDM::IOS_NA) && ((_IsIOPInput((FWDM::IOPid)p) && !_IsIOSInput((FWDM::IOSid)sId)) ||
      (!_IsIOPInput((FWDM::IOPid)p) &&  _IsIOSInput((FWDM::IOSid)sId))))
  {
    _AttachPlug((FWDM::IOPid)p); //fix using a stub
    result = false;
  }
 }
 return result;
}

bool CFWIORemappingController::_CheckForErrors(void)
{
 //Check and disconnect plugs if they are redundant
 bool result = _FixRedundantConnections();
 ASSERT(result);

 //Inputs must not be connected to outputs and vice versa
 result = _FixInputsVSOutputs();
 ASSERT(result);

 //Attach free slots to default plugs
 _AttachFreeSlotsToDefaultPlugs();

 _DisplayPlugs();
 return true;
}

FWDM::IOSid CFWIORemappingController::_GetConnectedSlot(FWDM::IOPid iopId, bool init, bool&  inv)
{
 DWORD plug = mp_fwdm->GetIOPlug(init ? FWDM::IOX_INIT : FWDM::IOX_DATA, iopId);
 for(int s = FWDM::IOS_START; s < FWDM::IOS_COUNT; ++s)
 {
  FWDM::IOXtype iotype = init ? FWDM::IOX_INIT : FWDM::IOX_DATA;
  DWORD slot[2] = {mp_fwdm->GetIOSlot(iotype, (FWDM::IOSid)s, false), mp_fwdm->GetIOSlot(iotype, (FWDM::IOSid)s, true)};
  if (slot[0] == mp_fwdm->GetSStub() || slot[0] == mp_fwdm->GetGStub())
   continue; //skip not utilized slots
  if (plug == slot[0]) { inv = false; return (FWDM::IOSid)s; }
  else if (plug == slot[1]) { inv = true; return (FWDM::IOSid)s; }
 }
 return FWDM::IOS_NA; //there is no slot connected to specified plug
}

void CFWIORemappingController::_EnableInversionItems(void)
{
 std::map<FWDM::IOSid, bool> enableFlags;
 enableFlags.insert(std::make_pair(FWDM::IOS_IGN_OUT3, false));
 enableFlags.insert(std::make_pair(FWDM::IOS_IGN_OUT4, false));
 enableFlags.insert(std::make_pair(FWDM::IOS_PS, false));
 for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
 {  
  DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
  if (plug == 0)
   continue; //skip reserved plugs
  std::map<FWDM::IOSid, bool>::iterator it;
  for(it = enableFlags.begin(); it != enableFlags.end(); ++it)
   if (_CmpPnS((FWDM::IOPid)p, it->first))
    it->second = true;
 }
 //Apply to view
 std::map<FWDM::IOSid, bool>::iterator it;
 for(it = enableFlags.begin(); it != enableFlags.end(); ++it)
  mp_view->EnableInversion(it->first, it->second);
}

void CFWIORemappingController::_DisplayPlugs(void)
{
#ifdef IOREMCNTR_DEBUG
 std::map<FWDM::IOSid, _TSTRING> names;
 std::map<FWDM::IOSid, _TSTRING>::iterator it;
 names.insert(std::make_pair(FWDM::IOS_IGN_OUT1, _T("IGN_OUT1")));
 names.insert(std::make_pair(FWDM::IOS_IGN_OUT2, _T("IGN_OUT2")));
 names.insert(std::make_pair(FWDM::IOS_IGN_OUT3, _T("IGN_OUT3")));
 names.insert(std::make_pair(FWDM::IOS_IGN_OUT4, _T("IGN_OUT4")));
 names.insert(std::make_pair(FWDM::IOS_ADD_IO1, _T("ADD_IO1")));
 names.insert(std::make_pair(FWDM::IOS_ADD_IO2, _T("ADD_IO2")));
 names.insert(std::make_pair(FWDM::IOS_ECF, _T("ECF")));
 names.insert(std::make_pair(FWDM::IOS_ST_BLOCK, _T("ST_BLOCK")));
 names.insert(std::make_pair(FWDM::IOS_IE, _T("IE")));
 names.insert(std::make_pair(FWDM::IOS_FE, _T("FE")));
 names.insert(std::make_pair(FWDM::IOS_PS, _T("PS")));
 names.insert(std::make_pair(FWDM::IOS_ADD_I1, _T("ADD_I1")));
 names.insert(std::make_pair(FWDM::IOS_ADD_I2, _T("ADD_I2")));
 names.insert(std::make_pair(FWDM::IOS_CE, _T("CE")));
 names.insert(std::make_pair(FWDM::IOS_BL, _T("BL")));
 names.insert(std::make_pair(FWDM::IOS_DE, _T("DE")));
 names.insert(std::make_pair(FWDM::IOS_REF_S, _T("REF_S")));
 names.insert(std::make_pair(FWDM::IOS_GAS_V, _T("GAS_V")));
 CString out(_T("      [INIT]            [DATA]\n"));
 for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
 {
  FWDM::IOSid sId;
  CString str_i, str_d;
  bool inv = false;
  sId = _GetConnectedSlot((FWDM::IOPid)p, true, inv); //init
  it = names.find(sId);
  if (sId != FWDM::IOS_NA && it != names.end())
   str_i = inv ? (_TSTRING(_T("~"))+it->second).c_str() : it->second.c_str();
  else
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (plug == mp_fwdm->GetSStub())
    str_i = _T("SSTUB");
   else if (plug == mp_fwdm->GetGStub())
    str_i = _T("GSTUB");
   else
    str_i = _T("?");
  }
  inv = false;
  sId = _GetConnectedSlot((FWDM::IOPid)p, false, inv); //data
  it = names.find(sId);
  if (sId != FWDM::IOS_NA && it != names.end())
   str_d = inv ? (_TSTRING(_T("~"))+it->second).c_str() : it->second.c_str();
  else
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_DATA, (FWDM::IOPid)p);
   if (plug == mp_fwdm->GetSStub())
    str_d = _T("SSTUB");
   else if (plug == mp_fwdm->GetGStub())
    str_d = _T("GSTUB");
   else
    str_d = _T("?");
  }
  while(str_i.GetLength() < 20)
   str_i+=_T(" ");

  CString n;
  n.Format("%02d   ", p);
  out+=n+str_i+str_d+_T("\n");
 }
 AfxMessageBox(out);
#endif //IOREMCNTR_DEBUG
}
