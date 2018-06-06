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
 m_pModel->m_optMIDeskUpdatePeriod.value = m_pView->GetMIDeskUpdatePeriod();
 m_pModel->m_optUseDVFeatures.value = m_pView->GetUseDVFeatures();
 m_pModel->m_optDVDeskUpdatePeriod.value = m_pView->GetDVDeskUpdatePeriod();
 m_pModel->m_optShowToolTips.value = m_pView->GetShowToolTips();
 m_pModel->m_optShowExFixtures.value = m_pView->GetShowExFixtures();
 m_pModel->m_optHexDataMode.value = m_pView->GetHexDataMode();

 size_t index = m_pView->GetCSVSepSymbol();
 m_pModel->m_optCSVSepSymbol.value = m_pModel->m_AllowaleCSVSepSymbols[index].second;

 m_pModel->m_optInterfaceLang.value = m_pView->GetInterfaceLanguage();
 m_pModel->m_optECUPlatformType.value = m_pView->GetECUPlatformType();

 m_pModel->m_optTachometerMax.value = m_pView->GetTachometerMax();
 m_pModel->m_optPressureMax.value = m_pView->GetPressureMax();

 m_pModel->m_optPulsesPer1Km.value = m_pView->GetNumPulsesPer1Km();
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
 m_pView->SetMIDeskUpdatePeriod(m_pModel->m_optMIDeskUpdatePeriod.value);
 m_pView->SetUseDVFeatures(m_pModel->m_optUseDVFeatures.value);
 m_pView->SetDVDeskUpdatePeriod(m_pModel->m_optDVDeskUpdatePeriod.value);
 m_pView->SetShowToolTips(m_pModel->m_optShowToolTips.value);
 m_pView->SetShowExFixtures(m_pModel->m_optShowExFixtures.value);
 m_pView->SetHexDataMode(m_pModel->m_optHexDataMode.value);

 size_t count = m_pModel->m_AllowaleCSVSepSymbols.size();
 for(size_t i = 0; i < count; i++)
  if (m_pModel->m_optCSVSepSymbol.value == m_pModel->m_AllowaleCSVSepSymbols[i].second)
   m_pView->SetCSVSepSymbol(i);

 m_pView->SetInterfaceLanguage(m_pModel->m_optInterfaceLang.value);
 m_pView->SetECUPlatformType(m_pModel->m_optECUPlatformType.value);

 m_pView->SetTachometerMax(m_pModel->m_optTachometerMax.value);
 m_pView->SetPressureMax(m_pModel->m_optPressureMax.value);

 m_pView->SetNumPulsesPer1Km(m_pModel->m_optPulsesPer1Km.value);
}

int CAppSettingsController::ShowDialog(void)
{
 ASSERT(m_pView);
 return m_pView->ShowDialog();
}
