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

/** \file ErrorMsg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "ErrorMsg.h"
#include "Application/CommunicationManager.h"
#include "ui-core/MsgBox.h"

namespace ErrorMsg
{

CString GenerateErrorStr(CCommunicationManager* p_comm) //redundant to same function in CFirmwareTabController
{
 ASSERT(p_comm);
 switch(p_comm->m_pBootLoader->GetLastError())
 {
  case CBootLoader::BL_ERROR_NOANSWER:
   return MLL::LoadString(IDS_BL_ERROR_NOANSWER);
  case CBootLoader::BL_ERROR_CHKSUM:
   return MLL::LoadString(IDS_BL_ERROR_CRC);
  case CBootLoader::BL_ERROR_WRONG_DATA:
   return MLL::LoadString(IDS_BL_ERROR_WRONG_DATA);
 }
 ASSERT(0); //что за ошибка?
 return CString(_T(""));
}

bool AskUserAboutTabLeaving(void)
{
 return (IDYES==SECUMessageBox(MLL::LoadString(IDS_FW_LEAVE_TAB_WARNING), MB_YESNO|MB_DEFBUTTON2));
}

};
