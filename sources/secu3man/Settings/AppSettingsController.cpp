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
 //сохраняем данные из представления в модель
 ASSERT((m_pModel!=NULL)&&(m_pView!=NULL));
 m_pModel->m_optPortName = m_pView->GetPortName();
 m_pModel->m_optBaudRateApplication = m_pView->GetBaudRateApplication();
 m_pModel->m_optBaudRateBootloader = m_pView->GetBaudRateBootloader();
 m_pModel->m_optLogFilesFolder = m_pView->GetLogFilesFolder();
 m_pModel->m_optUseAppFolder = m_pView->GetUseAppFolder();
 m_pModel->m_optMIDeskUpdatePeriod = m_pView->GetMIDeskUpdatePeriod();
 m_pModel->m_optUseDVFeatures = m_pView->GetUseDVFeatures();
 m_pModel->m_optDVDeskUpdatePeriod = m_pView->GetDVDeskUpdatePeriod();

 size_t index = m_pView->GetCSVSepSymbol();
 m_pModel->m_optCSVSepSymbol = m_pModel->m_AllowaleCSVSepSymbols[index].second;

 m_pModel->m_optInterLang = (EInterLang)m_pView->GetInterfaceLanguage();
 m_pModel->m_optECUPlatformType = (EECUPlatform)m_pView->GetECUPlatformType();

 m_pModel->m_optTachometerMax = m_pView->GetTachometerMax();
 m_pModel->m_optPressureMax = m_pView->GetPressureMax();
}

void CAppSettingsController::OnCancel(void)
{
 //При нажатии на Cancel ничего не делаем (не трогаем модель!)
}

void CAppSettingsController::OnActivate(void)
{
 //перекачиваем данные из модели в представление
 ASSERT((m_pModel!=NULL)&&(m_pView!=NULL));
 m_pView->FillCtrlsWithAllowableBaudRates(m_pModel->m_AllowableBaudRates);
 m_pView->FillCtrlsWithAllowablePorts(m_pModel->m_AllowablePorts);
 m_pView->FillCtrlsWithAllowableCSVSepSymbols(m_pModel->m_AllowaleCSVSepSymbols);
 m_pView->FillCtrlsWithAllowableInterfaceLanguages(m_pModel->m_AllowableLanguages);
 m_pView->FillCtrlsWithAllowableECUPlatformTypes(m_pModel->m_AllowablePlatforms);

 m_pView->SetPortName(m_pModel->m_optPortName);
 m_pView->SetBaudRateApplication(m_pModel->m_optBaudRateApplication);
 m_pView->SetBaudRateBootloader(m_pModel->m_optBaudRateBootloader);
 m_pView->SetLogFilesFolder(m_pModel->m_optLogFilesFolder);
 m_pView->SetUseAppFolder(m_pModel->m_optUseAppFolder);
 m_pView->SetMIDeskUpdatePeriod(m_pModel->m_optMIDeskUpdatePeriod);
 m_pView->SetUseDVFeatures(m_pModel->m_optUseDVFeatures);
 m_pView->SetDVDeskUpdatePeriod(m_pModel->m_optDVDeskUpdatePeriod);

 size_t count = m_pModel->m_AllowaleCSVSepSymbols.size();
 for(size_t i = 0; i < count; i++)
  if (m_pModel->m_optCSVSepSymbol == m_pModel->m_AllowaleCSVSepSymbols[i].second)
   m_pView->SetCSVSepSymbol(i);

 m_pView->SetInterfaceLanguage(m_pModel->m_optInterLang);
 m_pView->SetECUPlatformType(m_pModel->m_optECUPlatformType);

 m_pView->SetTachometerMax(m_pModel->m_optTachometerMax);
 m_pView->SetPressureMax(m_pModel->m_optPressureMax);
}

int CAppSettingsController::ShowDialog(void)
{
 ASSERT(m_pView);
 return m_pView->ShowDialog();
}
