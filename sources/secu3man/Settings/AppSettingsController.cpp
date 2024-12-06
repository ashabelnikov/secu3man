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

/** \file AppSettingsController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "AppSettingsController.h"
#include "AppSettingsModel.h"

using namespace fastdelegate;

//Presenter implementation

CAppSettingsController::CAppSettingsController(IAppSettingsDlg* i_pView, CAppSettingsModel* i_pModel)
: m_pView(i_pView)
, m_pModel(i_pModel)
{
 ASSERT(m_pView);
 m_pView->setFunctionOnOk(MakeDelegate(this,&CAppSettingsController::OnOk));
 m_pView->setFunctionOnCancel(MakeDelegate(this,&CAppSettingsController::OnCancel));
 m_pView->setFunctionOnActivate(MakeDelegate(this,&CAppSettingsController::OnActivate));
}

CAppSettingsController::~CAppSettingsController()
{
 //empty
}

void CAppSettingsController::OnOk(void)
{
 //save data from view into the model
 ASSERT((m_pModel!=NULL)&&(m_pView!=NULL));
 m_pModel->m_optPortName.value = m_pView->GetPortName();
 m_pModel->m_optBaudRateApplication.value = m_pView->GetBaudRateApplication();
 m_pModel->m_optBaudRateBootloader.value = m_pView->GetBaudRateBootloader();
 m_pModel->m_optLogFilesFolder.value = m_pView->GetLogFilesFolder();
 m_pModel->m_optUseAppFolder.value = m_pView->GetUseAppFolder();
 m_pModel->m_optAlwaysWriteLog.value = m_pView->GetAlwaysWriteLog();
 m_pModel->m_optWriteLogFields.value = m_pView->GetWriteLogFields();
 m_pModel->m_optMIDeskUpdatePeriod.value = m_pView->GetMIDeskUpdatePeriod();
 m_pModel->m_optUseDVFeatures.value = m_pView->GetUseDVFeatures();
 m_pModel->m_optDVDeskUpdatePeriod.value = m_pView->GetDVDeskUpdatePeriod();
 m_pModel->m_optShowToolTips.value = m_pView->GetShowToolTips();
 m_pModel->m_optShowExFixtures.value = m_pView->GetShowExFixtures();
 m_pModel->m_optShowSpeedAndDist.value = m_pView->GetShowSpeedAndDist();
 m_pModel->m_optExistingPorts.value = m_pView->GetExistingPorts();
 m_pModel->m_optInjDrvTabActive.value = m_pView->GetInjDrvTabActive();
 m_pModel->m_optLogBinaryFmt.value = m_pView->GetLogBinaryFmt();

 size_t index = m_pView->GetCSVSepSymbol();
 m_pModel->m_optCSVSepSymbol.value = m_pModel->m_AllowaleCSVSepSymbols[index].second;

 m_pModel->m_optInterfaceLang.value = m_pView->GetInterfaceLanguage();
 m_pModel->m_optECUPlatformType.value = m_pView->GetECUPlatformType();

 m_pModel->m_optMetScRPM[0].value[1] = (float)m_pView->GetTachometerMax();
 m_pModel->m_optMetScMAP[0].value[1] = (float)m_pView->GetPressureMax();
}

void CAppSettingsController::OnCancel(void)
{
 //Do nothing! (do not touch model)
}

void CAppSettingsController::OnActivate(void)
{
 //pump data from the model into view
 ASSERT((m_pModel!=NULL)&&(m_pView!=NULL));
 m_pView->FillCtrlsWithAllowableBaudRates(m_pModel->m_AllowableBaudRates);
 m_pView->FillCtrlsWithAllowablePorts(m_pModel->m_AllowablePorts);
 m_pView->FillCtrlsWithAllowableCSVSepSymbols(m_pModel->m_AllowaleCSVSepSymbols);
 m_pView->FillCtrlsWithAllowableInterfaceLanguages(m_pModel->m_AllowableLanguages);
 m_pView->FillCtrlsWithAllowableECUPlatformTypes(m_pModel->m_AllowablePlatforms);

 m_pView->SetPortName(m_pModel->m_optPortName.value);
 m_pView->SetBaudRateApplication(m_pModel->m_optBaudRateApplication.value);
 m_pView->SetBaudRateBootloader(m_pModel->m_optBaudRateBootloader.value);
 m_pView->SetLogFilesFolder(m_pModel->m_optLogFilesFolder.value);
 m_pView->SetUseAppFolder(m_pModel->m_optUseAppFolder.value);
 m_pView->SetAlwaysWriteLog(m_pModel->m_optAlwaysWriteLog.value);
 m_pView->SetWriteLogFields(m_pModel->m_optWriteLogFields.value);
 m_pView->SetMIDeskUpdatePeriod(m_pModel->m_optMIDeskUpdatePeriod.value);
 m_pView->SetUseDVFeatures(m_pModel->m_optUseDVFeatures.value);
 m_pView->SetDVDeskUpdatePeriod(m_pModel->m_optDVDeskUpdatePeriod.value);
 m_pView->SetShowToolTips(m_pModel->m_optShowToolTips.value);
 m_pView->SetShowExFixtures(m_pModel->m_optShowExFixtures.value);
 m_pView->SetShowSpeedAndDist(m_pModel->m_optShowSpeedAndDist.value);
 m_pView->SetExistingPorts(m_pModel->m_optExistingPorts.value);
 m_pView->SetInjDrvTabActive(m_pModel->m_optInjDrvTabActive.value);
 m_pView->SetLogBinaryFmt(m_pModel->m_optLogBinaryFmt.value);

 size_t count = m_pModel->m_AllowaleCSVSepSymbols.size();
 for(size_t i = 0; i < count; i++)
  if (m_pModel->m_optCSVSepSymbol.value == m_pModel->m_AllowaleCSVSepSymbols[i].second)
   m_pView->SetCSVSepSymbol(i);

 m_pView->SetInterfaceLanguage(m_pModel->m_optInterfaceLang.value);
 m_pView->SetECUPlatformType(m_pModel->m_optECUPlatformType.value);

 m_pView->SetTachometerMax((int)m_pModel->m_optMetScRPM[0].value[1]);
 m_pView->SetPressureMax((int)m_pModel->m_optMetScMAP[0].value[1]);
}

int CAppSettingsController::ShowDialog(void)
{
 ASSERT(m_pView);
 return m_pView->ShowDialog();
}
