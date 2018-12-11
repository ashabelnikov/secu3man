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

/** \file AppSettingsManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "AppSettingsDlg.h"
#include "AppSettingsController.h"
#include "AppSettingsManager.h"
#include "AppSettingsModel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppSettingsManager::CAppSettingsManager()
: m_pModel(NULL)
, m_pController(NULL)
, m_pDialog(NULL)
{
 m_pModel = new CAppSettingsModel();
 m_pDialog = new CAppSettingsDlg(); //View
 m_pController = new CAppSettingsController(m_pDialog,m_pModel);
}

CAppSettingsManager::~CAppSettingsManager()
{
 delete m_pModel;
 delete m_pController;
 delete m_pDialog;
}

bool CAppSettingsManager::ReadSettings(void) const
{
  ASSERT(m_pModel);
  return m_pModel->ReadSettings();
}

bool CAppSettingsManager::WriteSettings(void) const
{
 ASSERT(m_pModel);
 return m_pModel->WriteSettings();
}

int CAppSettingsManager::ShowDialog(void) const
{ //delegate call to controller...
 ASSERT(m_pController);
 return m_pController->ShowDialog();
}

ISettingsData* CAppSettingsManager::GetSettings(void) const
{
 return m_pModel;
}

CAppSettingsModel* CAppSettingsManager::GetModel(void) const
{
 return m_pModel;
}

bool CAppSettingsManager::IsRestartRequired(void)
{
 return m_pDialog->IsRestartRequired();
}
