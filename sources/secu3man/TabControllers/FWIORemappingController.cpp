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
#include "FWIORemappingController.h"

#include "common/fastdelegate.h"
#include "ParamDesk/Params/IORemappingDlg.h"

using namespace fastdelegate;
typedef CFirmwareDataMediator FWDM;

//#define IOREMCNTR_DEBUG

namespace {
//Following functions for compatibility
std::map<FWDM::IOSid, FWDM::IOSid> idm;
void _fill_idm(void) {
 typedef CFirmwareDataMediator FWDM;
 if (idm.size())
  return;
 idm.insert(std::make_pair(FWDM::IOS_ECFv0, FWDM::IOS_ECF));
 idm.insert(std::make_pair(FWDM::IOS_ST_BLOCKv0, FWDM::IOS_ST_BLOCK));
 idm.insert(std::make_pair(FWDM::IOS_IGN_OUT3v0, FWDM::IOS_IGN_OUT3));
 idm.insert(std::make_pair(FWDM::IOS_IGN_OUT4v0, FWDM::IOS_IGN_OUT4));
 idm.insert(std::make_pair(FWDM::IOS_ADD_IO1v0, FWDM::IOS_ADD_IO1));
 idm.insert(std::make_pair(FWDM::IOS_ADD_IO2v0, FWDM::IOS_ADD_IO2));
 idm.insert(std::make_pair(FWDM::IOS_IEv0, FWDM::IOS_IE));
 idm.insert(std::make_pair(FWDM::IOS_FEv0, FWDM::IOS_FE));
}

//Convert slot ID from V0.0 to V1.0+
CFirmwareDataMediator::IOSid _IOSfV0(CFirmwareDataMediator::IOSid idv0) {
 typedef CFirmwareDataMediator FWDM;
 _fill_idm();
 std::map<FWDM::IOSid, FWDM::IOSid>::const_iterator it = idm.find(idv0);
 if (it != idm.end())
  return it->second;
 else
  return FWDM::IOS_NA;
}

//Convert slot ID from V1.0+ to V0.0
CFirmwareDataMediator::IOSid _IOStV0(CFirmwareDataMediator::IOSid id) {
 typedef CFirmwareDataMediator FWDM;
 _fill_idm();
 std::map<FWDM::IOSid, FWDM::IOSid>::const_iterator it = idm.begin();
 for(; it != idm.end(); ++it)
  if (it->second == id)
   return it->first;
 return FWDM::IOS_NA;
}
}

//Get ID of first plug
CFWIORemappingController::FWDM::IOPid CFWIORemappingController::_IOPStart(void) {
 return (FWDM::IOV_V00 == mp_fwdm->GetIORemVersion()) ? FWDM::IOP_STARTv0 : FWDM::IOP_START;
}
//Get count of plugs
CFWIORemappingController::FWDM::IOPid CFWIORemappingController::_IOPCount(void) {
 return (FWDM::IOV_V00 == mp_fwdm->GetIORemVersion()) ? FWDM::IOP_COUNTv0 : FWDM::IOP_COUNT;
}
//Get ID of first slot
CFWIORemappingController::FWDM::IOSid CFWIORemappingController::_IOSStart(void) {
 return (FWDM::IOV_V00 == mp_fwdm->GetIORemVersion()) ? FWDM::IOS_STARTv0 : FWDM::IOS_START;
}
//Get count of slots
CFWIORemappingController::FWDM::IOSid CFWIORemappingController::_IOSCount(void) {
 return (FWDM::IOV_V00 == mp_fwdm->GetIORemVersion()) ? FWDM::IOS_COUNTv0 : FWDM::IOS_COUNT;
}

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

 //Enable inversion check boxes (applicable only for V1.0+)
 _EnableInversionItems();

 //Update UI
 _UpdateView();
}

//This method is suitable for V1.0+ (not for V0.0, because inversion checkboxes are disabled)
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

 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {
  m_defValMap.insert(std::make_pair(FWDM::IOS_ECFv0, FWDM::IOP_ECFv0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ST_BLOCKv0, FWDM::IOP_ST_BLOCKv0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT3v0, FWDM::IOP_IGN_OUT3v0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT4v0, FWDM::IOP_IGN_OUT4v0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_IO1v0, FWDM::IOP_ADD_IO1v0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_IO2v0, FWDM::IOP_ADD_IO2v0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IEv0, FWDM::IOP_IEv0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_FEv0, FWDM::IOP_FEv0));

  //fill view with values
  mp_view->ResetContent();
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_ECFv0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ECF, true);

  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_ST_BLOCKv0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ST_BLOCK, true);

  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3v0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT3, true);

  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4v0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT4, true);

  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1v0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ADD_IO1, true);

  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_ADD_IO2v0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ADD_IO2, true);

  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IEv0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IE, true);

  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FEv0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_FE, true);
 }
 else
 { //v1.0+
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
  if (iov >= FWDM::IOV_V12)
  {
   mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_SM_DIR, _T("SM_DIR"));    //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_SM_STP, _T("SM_STP"));    //appeared in v1.2
  }
  mp_view->AddItem(FWDM::IOS_IGN_OUT2, FWDM::IOP_IGN_OUT2, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT2, true);
  mp_view->EnableInversion(FWDM::IOS_IGN_OUT2, true);

  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  if (iov >= FWDM::IOV_V12)
  {
   mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_SM_DIR, _T("SM_DIR"));    //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_SM_STP, _T("SM_STP"));    //appeared in v1.2
  }
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT3, true);
  mp_view->EnableInversion(FWDM::IOS_IGN_OUT3, true);

  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  if (iov >= FWDM::IOV_V12)
  {
   mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_SM_DIR, _T("SM_DIR"));    //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_SM_STP, _T("SM_STP"));    //appeared in v1.2
  }
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT4, true);
  mp_view->EnableInversion(FWDM::IOS_IGN_OUT4, true);

  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_IGN, _T("IGN"));
  if (iov >= FWDM::IOV_V11)
   mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_BC_INPUT, _T("BC_INPUT")); //appeared in v1.1
  if (iov >= FWDM::IOV_V13)
   mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_MAPSEL0, _T("MAPSEL0"));   //appeared in v1.3
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
  if (iov >= FWDM::IOV_V12)
  {
   mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_SM_DIR, _T("SM_DIR"));     //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_SM_STP, _T("SM_STP"));     //appeared in v1.2
  }
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ADD_IO1, true);
  mp_view->EnableInversion(FWDM::IOS_ADD_IO1, true);

  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_IGN, _T("IGN"));
  if (iov >= FWDM::IOV_V11)
   mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_BC_INPUT, _T("BC_INPUT")); //appeared in v1.1
  if (iov >= FWDM::IOV_V13)
   mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_MAPSEL0, _T("MAPSEL0"));   //appeared in v1.3
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_IGN_OUT3, _T("IGN_OUT3"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_IGN_OUT4, _T("IGN_OUT4"));
  if (iov >= FWDM::IOV_V12)
  {  
   mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_SM_DIR, _T("SM_DIR"));     //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_SM_STP, _T("SM_STP"));     //appeared in v1.2
  }
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
  if (iov >= FWDM::IOV_V12)
  {
   mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_SM_DIR, _T("SM_DIR"));         //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_SM_STP, _T("SM_STP"));         //appeared in v1.2
  }
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
  if (iov >= FWDM::IOV_V12)
  {
   mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_SM_DIR, _T("SM_DIR"));    //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_SM_STP, _T("SM_STP"));    //appeared in v1.2
  }
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
  if (iov >= FWDM::IOV_V12)
  {
   mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_SM_DIR, _T("SM_DIR"));          //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_SM_STP, _T("SM_STP"));          //appeared in v1.2
  }
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
  if (iov >= FWDM::IOV_V12)
  {
   mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_SM_DIR, _T("SM_DIR"));          //appeared in v1.2
   mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_SM_STP, _T("SM_STP"));          //appeared in v1.2
  }
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FE, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_FE, true); 
  mp_view->EnableInversion(FWDM::IOS_FE, true);
  //PS input:
  mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_IGN, _T("IGN"));
  if (iov >= FWDM::IOV_V11)
   mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_BC_INPUT, _T("BC_INPUT"));      //appeared in v1.1
  if (iov >= FWDM::IOV_V13)
   mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_MAPSEL0, _T("MAPSEL0"));        //appeared in v1.3
  mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_PS, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_PS, true); 
  mp_view->EnableInversion(FWDM::IOS_PS, true);
 }
}

//Note that this function uses default inversion values from m_invFlags map.
void CFWIORemappingController::_AttachFreeSlotsToDefaultPlugs(void)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  for(int s = FWDM::IOS_STARTv0; s < FWDM::IOS_COUNTv0; ++s)
  {
   if (!_IsSlotFree((FWDM::IOSid)s))
    continue;
   std::map<FWDM::IOSid, FWDM::IOPid>::const_iterator it = m_defValMap.find((FWDM::IOSid)s);
   if (it != m_defValMap.end())
    _AttachPlug(it->second, (FWDM::IOSid)s); //default
  }
 }
 else
 {//V1.0+  
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
}

bool CFWIORemappingController::_IsSlotFree(FWDM::IOSid iosId)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId);
  for(int p = FWDM::IOP_STARTv0; p < FWDM::IOP_COUNTv0; ++p)
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (plug == slot)
    return false; //already connected to one of plugs
  }
 }
 else
 {//V1.0+
  for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
  {  
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (plug == 0)
    continue; //skip reserved plugs
   if (_CmpPnS((FWDM::IOPid)p, iosId))
    return false; //already connected to one of plugs
   //Если входы ADD_Ix не свободны, то выходы ADD_IOx тоже не считаем свободными (и наоборот), в противном случае
   //будет назначен выход по умолчанию и он будет мешать работе входа
   if ((iosId == FWDM::IOS_ADD_IO1 && _CmpPnS((FWDM::IOPid)p, FWDM::IOS_ADD_I1)) ||
      (iosId == FWDM::IOS_ADD_I1   && _CmpPnS((FWDM::IOPid)p, FWDM::IOS_ADD_IO1)))    
    return false; //already connected to one of plugs
   if ((iosId == FWDM::IOS_ADD_IO2 && _CmpPnS((FWDM::IOPid)p, FWDM::IOS_ADD_I2)) ||
      (iosId == FWDM::IOS_ADD_I2   && _CmpPnS((FWDM::IOPid)p, FWDM::IOS_ADD_IO2)))
    return false; //already connected to one of plugs
  } 
 }
 return true; //free!
}

void CFWIORemappingController::_DetachPlugsFromSpecifiedSlot(FWDM::IOSid iosId)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, _IOStV0(iosId));
  for(int p = FWDM::IOP_STARTv0; p < FWDM::IOP_COUNTv0; ++p)
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if(plug == slot)
    _AttachPlug((FWDM::IOPid)p); //stub
  }
 }
 else
 {//V1.0+
  for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (plug == 0)
    continue; //skip reserved plugs
   //Если происходит отключение ADD_IO1, то нужно также отключить ADD_I1;
   //Если происходит отключение ADD_IO2, то нужно также отключить ADD_I2;
   if( _CmpPnS((FWDM::IOPid)p, iosId) ||
      (iosId == FWDM::IOS_ADD_IO1 && _CmpPnS((FWDM::IOPid)p, FWDM::IOS_ADD_I1)) ||
      (iosId == FWDM::IOS_ADD_IO2 && _CmpPnS((FWDM::IOPid)p, FWDM::IOS_ADD_I2)))
    _AttachPlug((FWDM::IOPid)p); //stub
  }
 }
}

void CFWIORemappingController::_UpdateView(void)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  for(int s = FWDM::IOS_STARTv0; s < FWDM::IOS_COUNTv0; ++s)
  {  
   DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, (FWDM::IOSid)s);
   for(int p = FWDM::IOP_STARTv0; p < FWDM::IOP_COUNTv0; ++p)
   {
    DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
    //If slot == stub, then it is not allowed in this firmware
    if (slot == plug && slot != mp_fwdm->GetSStub())
     mp_view->SelectItem(_IOSfV0((FWDM::IOSid)s), (FWDM::IOPid)p);
   }
  }
 }
 else
 {//V1.0+
  for(int s = FWDM::IOS_START; s < FWDM::IOS_COUNT; ++s)
  {  
   if (((FWDM::IOSid)s == FWDM::IOS_ADD_I1) || ((FWDM::IOSid)s == FWDM::IOS_ADD_I2))
    continue; //skip ADD_I1 and ADD_I2 because they have not to appear in the UI
   DWORD slot[2] = {mp_fwdm->GetIOSlot(FWDM::IOX_INIT, (FWDM::IOSid)s, false), mp_fwdm->GetIOSlot(FWDM::IOX_INIT, (FWDM::IOSid)s, true)};
   DWORD slot_add_i1[2] = {mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I1, false), mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I1, true)};
   DWORD slot_add_i2[2] = {mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I2, false), mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I2, true)};
   bool itemSet = false;
   for(int p = FWDM::IOP_START; p < FWDM::IOP_COUNT; ++p)
   {
    DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
    //If slot == stub, then it is not allowed in this firmware
    if ((slot[0] != mp_fwdm->GetSStub()) && ((slot[0] == plug) || (s == FWDM::IOS_ADD_IO1 && plug == slot_add_i1[0]) ||
        (s == FWDM::IOS_ADD_IO2 && plug == slot_add_i2[0])))
    {
     mp_view->SelectItem((FWDM::IOSid)s, (FWDM::IOPid)p);
     mp_view->InvertItem((FWDM::IOSid)s, false);
     _SetInvFlag((FWDM::IOSid)s, false); //update inversion flags
     itemSet = true;
    }
    else if ((slot[1] != mp_fwdm->GetSStub()) && ((slot[1] == plug) || (s == FWDM::IOS_ADD_IO1 && plug == slot_add_i1[1]) ||
        (s == FWDM::IOS_ADD_IO2 && plug == slot_add_i2[1])))
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
}

void CFWIORemappingController::_AttachPlug(FWDM::IOPid iopId)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetSStub());
  mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetSStub());
 }
 else
 {//V1.0+
  mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetSStub());
  if (_IsIOPInput(iopId))
   mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetGStub()); //для входов своя заглушка, 
  else
   mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetSStub()); //для выходов своя
 }
}

void CFWIORemappingController::_AttachPlugToSpecifiedSlot(FWDM::IOPid iopId, FWDM::IOSid iosId)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)//V0.0
  _AttachPlug(iopId, _IOStV0(iosId));
 else
 {//V1.0+
  bool inv = m_invFlags[iosId];
  if (_IsIOPInput(iopId) && iosId == FWDM::IOS_ADD_IO1)
   _AttachPlug(iopId, FWDM::IOS_ADD_I1, inv); //substitute IOS_ADD_IO1 to IOS_ADD_I1
  else if (_IsIOPInput(iopId) && iosId == FWDM::IOS_ADD_IO2)
   _AttachPlug(iopId, FWDM::IOS_ADD_I2, inv); //substitute IOS_ADD_IO2 to IOS_ADD_I2
  else if (iopId == FWDM::IOP_ADD_IO1)   //when connecting ADD_IO1 plug back to ADD_IO1 slot we also have to connect ADD_I1
   _AttachPlug(FWDM::IOP_ADD_IO1, iosId, inv), _AttachPlug(FWDM::IOP_ADD_I1, FWDM::IOS_ADD_I1, inv);
  else if (iopId == FWDM::IOP_ADD_IO2)   //when connecting ADD_IO1 plug back to ADD_IO1 slot we also have to connect ADD_I1
   _AttachPlug(FWDM::IOP_ADD_IO2, iosId, inv), _AttachPlug(FWDM::IOP_ADD_I2, FWDM::IOS_ADD_I2, inv);
  else
   _AttachPlug(iopId, iosId, inv);
 }
}

void CFWIORemappingController::_AttachPlug(FWDM::IOPid iopId, FWDM::IOSid iosId, bool inv /*=false*/)
{
 mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId, inv));
 mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetIOSlot(FWDM::IOX_DATA, iosId, inv));
}

bool CFWIORemappingController::_IsIOPInput(FWDM::IOPid iopId) const
{
 return (iopId == FWDM::IOP_PS || iopId == FWDM::IOP_ADD_I1 || iopId == FWDM::IOP_ADD_I2 || iopId == FWDM::IOP_IGN || iopId == FWDM::IOP_BC_INPUT || iopId == FWDM::IOP_MAPSEL0);
}

bool CFWIORemappingController::_IsIOSInput(FWDM::IOSid iosId) const
{
 return (iosId == FWDM::IOS_PS || iosId == FWDM::IOS_ADD_I1 || iosId == FWDM::IOS_ADD_I2);
}

void CFWIORemappingController::_SetInvFlag(FWDM::IOSid iosId, bool inv)
{
 m_invFlags[iosId] = inv;
 if (iosId == FWDM::IOS_ADD_IO1)
  m_invFlags[FWDM::IOS_ADD_I1] = inv;
 else if (iosId == FWDM::IOS_ADD_IO2)
  m_invFlags[FWDM::IOS_ADD_I2] = inv;
 else if (iosId == FWDM::IOS_ADD_I1)
  m_invFlags[FWDM::IOS_ADD_IO1] = inv;
 else if (iosId == FWDM::IOS_ADD_I2)
  m_invFlags[FWDM::IOS_ADD_IO2] = inv; 
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
 for(int p = _IOPStart(); p < _IOPCount(); ++p)
 {
  for(int i = FWDM::IOX_INIT; i <= /*FWDM::IOX_DATA*/FWDM::IOX_INIT; ++i)
  {
   DWORD plug = mp_fwdm->GetIOPlug((FWDM::IOXtype)i, (FWDM::IOPid)p);
   if (plug == 0)
    continue;
   bool find = false;
   for(int pp = _IOPStart(); pp < _IOPCount(); ++pp)
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
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
  return result; //V0.0 had no input remapping capabilities!

 //V1.0+
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
 for(int s = _IOSStart(); s < _IOSCount(); ++s)
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
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
  return; //V0.0 had no inversion capabilities

 std::map<FWDM::IOSid, bool> enableFlags;
 enableFlags.insert(std::make_pair(FWDM::IOS_IGN_OUT3, false));
 enableFlags.insert(std::make_pair(FWDM::IOS_IGN_OUT4, false));
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
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 std::map<FWDM::IOSid, _TSTRING> names;
 std::map<FWDM::IOSid, _TSTRING>::iterator it;
 if (iov == FWDM::IOV_V00) {//V0.0
  names.insert(std::make_pair(FWDM::IOS_ECFv0, _T("ECF")));
  names.insert(std::make_pair(FWDM::IOS_ST_BLOCKv0, _T("ST_BLOCK")));
  names.insert(std::make_pair(FWDM::IOS_IGN_OUT3v0, _T("IGN_OUT3")));
  names.insert(std::make_pair(FWDM::IOS_IGN_OUT4v0, _T("IGN_OUT4")));
  names.insert(std::make_pair(FWDM::IOS_ADD_IO1v0, _T("ADD_IO1")));
  names.insert(std::make_pair(FWDM::IOS_ADD_IO2v0, _T("ADD_IO2")));
  names.insert(std::make_pair(FWDM::IOS_IEv0, _T("IE")));
  names.insert(std::make_pair(FWDM::IOS_FEv0, _T("FE")));
 }
 else if (iov > FWDM::IOV_V00) {//V1.0+
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
 }
 CString out(_T("      [INIT]            [DATA]\n"));
 for(int p = _IOPStart(); p < _IOPCount(); ++p)
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
