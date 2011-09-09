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
#include "resource.h"
#include "SECUImpExpController.h"
#include <algorithm>

#include "common/FastDelegate.h"
#include "io-core/FirmwareDataMediator.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "io-core/PlatformParamHolder.h"
#include "MapImpExpDlg.h"

using namespace fastdelegate;

SECU3ImportController::SECU3ImportController(FWMapsDataHolder* ip_fwd)
: mp_fwd(ip_fwd)
, m_fwdm(NULL)
, mp_view(new CMapImpExpDlg())
, m_secu3_file_name(_T(""))
{
 ASSERT(mp_fwd);
 ASSERT(mp_view);

 //Ќазначаем обработчики событий от представлени€
 mp_view->setOnOkButton(MakeDelegate(this,&SECU3ImportController::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&SECU3ImportController::OnCancelPressed));
 mp_view->setOnExchangeButton(MakeDelegate(this,&SECU3ImportController::OnExchangePressed));
 mp_view->setOnActivate(MakeDelegate(this,&SECU3ImportController::OnViewActivate));
 mp_view->setIsExchengeButtonAllowed(MakeDelegate(this,&SECU3ImportController::IsExchangeButtonAllowed));
 mp_view->setOnFWDCurrentListNamechanged(MakeDelegate(this,&SECU3ImportController::OnCurrentListNameChanged));
 mp_view->setOnFWDOtherListNamechanged(MakeDelegate(this,&SECU3ImportController::OnOtherListNameChanged));
}

void SECU3ImportController::setFileReader(Delegate1 delegate)
{
 m_FileReader = delegate;
}

int SECU3ImportController::DoImport(void)
{
 if (!m_FileReader)
  return IDCANCEL;

 std::vector<int> sizes = PlatformParamHolder::GetFirmwareSizes();
 std::vector<int>::const_iterator it = std::max_element(sizes.begin(), sizes.end());
 std::vector<BYTE> buffer((*it));
 int selected_size = 0;
 _TSTRING file_name;
 bool result = m_FileReader(&buffer[0], sizes, NULL, &selected_size, &file_name, NULL);
 if (true==result)
 {
  m_secu3_file_name = file_name;
  EECUPlatform platform_id;
  if (!PlatformParamHolder::GetPlatformIdByFirmwareSize(selected_size, platform_id))
   return IDCANCEL; //error
  PlatformParamHolder params(platform_id);
  m_fwdm = new CFirmwareDataMediator(params.GetFlashParameters());
  m_fwdm->LoadBytes(&buffer[0]);
 }
 else
  return IDCANCEL; //отказ пользовател€ или ошибка
 
 return mp_view->DoModal();
}

bool SECU3ImportController::IsExchangeButtonAllowed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();
 return (current_sel != -1) && (other_sel != -1);
}

SECU3ImportController::~SECU3ImportController()
{
 delete mp_view; //avoid memory leak
 delete m_fwdm;
}

void SECU3ImportController::OnOkPressed(void)
{
 //import separate tables
 if (mp_view->GetFWDFlag(FLAG_ATTEN_MAP))
  m_fwdm->GetAttenuatorMap(mp_fwd->attenuator_table);

 if (mp_view->GetFWDFlag(FLAG_COILREG_MAP))
  m_fwdm->GetCoilRegulMap(mp_fwd->coilregul_table);
}

void SECU3ImportController::OnCancelPressed(void)
{
 //empty
}

//нажали кнопку перемещени€ данных
void SECU3ImportController::OnExchangePressed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();

 std::vector<_TSTRING> names = m_fwdm->GetFunctionsSetNames();

 if (names[other_sel] != _T(""))
  mp_fwd->maps[current_sel].name = names[other_sel];
 else
 { //если строка пуста€, то генерируем "искусcтвенное" им€
  TCHAR name[32];
  _stprintf(name,MLL::GetString(IDS_MAP_NO_NAME).c_str(), other_sel+1);
  mp_fwd->maps[current_sel].name = name;
 }

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 mp_view->SetFWDCurrentListSelection(current_sel);

 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  m_fwdm->GetStartMap(other_sel, mp_fwd->maps[current_sel].f_str);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  m_fwdm->GetIdleMap(other_sel, mp_fwd->maps[current_sel].f_idl);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  m_fwdm->GetWorkMap(other_sel, mp_fwd->maps[current_sel].f_wrk);

 if (mp_view->GetFWDFlag(FLAG_TEMP_MAP))
  m_fwdm->GetTempMap(other_sel, mp_fwd->maps[current_sel].f_tmp);
}

//модальное окно активировалось - проводим его инициализацию
void SECU3ImportController::OnViewActivate(void)
{
 mp_view->SetFWDCurrentListTitle(MLL::GetString(IDS_SECU3_CURRENT_FW));
 CString title;
 title.Format(MLL::GetString(IDS_SECU3_FW_FILE).c_str(),m_secu3_file_name.c_str());
 mp_view->SetFWDOtherListTitle(_TSTRING(title));

 mp_view->SetExchangeButtonCaption(_T("<"));
 mp_view->SetWindowText(MLL::LoadString(IDS_IMPORT_SECU3_TABLES));

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 std::vector<_TSTRING> strings = m_fwdm->GetFunctionsSetNames();
 mp_view->FillFWDOtherList(strings);

 mp_view->SetFWDFlag(FLAG_START_MAP,true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP,true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP,true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP,true);
 mp_view->SetFWDFlag(FLAG_COILREG_MAP,true);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP,true);
}

void SECU3ImportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void SECU3ImportController::OnOtherListNameChanged(int item, CString text)
{
 m_fwdm->SetFunctionsSetName(item, text.GetBuffer(1000));
}

//////////////////////////////////////////////////////////////////////////////////////////
SECU3ExportController::SECU3ExportController(FWMapsDataHolder* ip_fwd)
: mp_fwd(ip_fwd)
, m_fwdm(NULL)
, mp_view(new CMapImpExpDlg())
, m_secu3_file_name(_T(""))
, m_secu3_file_path(_T(""))
{
 ASSERT(mp_fwd);
 ASSERT(mp_view);

 //Ќазначаем обработчики событий от представлени€
 mp_view->setOnOkButton(MakeDelegate(this,&SECU3ExportController::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&SECU3ExportController::OnCancelPressed));
 mp_view->setOnExchangeButton(MakeDelegate(this,&SECU3ExportController::OnExchangePressed));
 mp_view->setOnActivate(MakeDelegate(this,&SECU3ExportController::OnViewActivate));
 mp_view->setIsExchengeButtonAllowed(MakeDelegate(this,&SECU3ExportController::IsExchangeButtonAllowed));
 mp_view->setOnFWDCurrentListNamechanged(MakeDelegate(this,&SECU3ExportController::OnCurrentListNameChanged));
 mp_view->setOnFWDOtherListNamechanged(MakeDelegate(this,&SECU3ExportController::OnOtherListNameChanged));
}

void SECU3ExportController::setFileReader(Delegate1 delegate)
{
 m_FileReader = delegate;
}

int SECU3ExportController::DoExport(void)
{
 if (!m_FileReader)
  return IDCANCEL;

 std::vector<int> sizes = PlatformParamHolder::GetFirmwareSizes();
 std::vector<int>::const_iterator it = std::max_element(sizes.begin(), sizes.end());
 std::vector<BYTE> buffer((*it));
 int selected_size = 0;
 _TSTRING file_name;
 _TSTRING title = MLL::LoadString(IDS_OPEN_FILE_FOR_EXPORT);
 _TSTRING file_path;
 bool result = m_FileReader(&buffer[0], sizes, &title, &selected_size, &file_name, &file_path);
 if (true==result)
 {
  m_secu3_file_name = file_name;  //save file name
  m_secu3_file_path = file_path;  //save file path
  EECUPlatform platform_id;
  if (!PlatformParamHolder::GetPlatformIdByFirmwareSize(selected_size, platform_id))
   return IDCANCEL; //error
  PlatformParamHolder params(platform_id);
  m_fwdm = new CFirmwareDataMediator(params.GetFlashParameters());
  m_fwdm->LoadBytes(&buffer[0]);
 }
 else
  return IDCANCEL; //отказ пользовател€ или ошибка
 
 return mp_view->DoModal();
}

bool SECU3ExportController::IsExchangeButtonAllowed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();
 return (current_sel != -1) && (other_sel != -1);
}

SECU3ExportController::~SECU3ExportController()
{
 delete mp_view; //avoid memory leak
 delete m_fwdm;
}

void SECU3ExportController::OnOkPressed(void)
{
 //import separate tables
 if (mp_view->GetFWDFlag(FLAG_ATTEN_MAP))
  m_fwdm->SetAttenuatorMap(mp_fwd->attenuator_table);

 if (mp_view->GetFWDFlag(FLAG_COILREG_MAP))
  m_fwdm->SetCoilRegulMap(mp_fwd->coilregul_table);

 //allocate memory
 std::vector<BYTE> buffer(m_fwdm->GetPlatformParams().m_total_size);
 m_fwdm->StoreBytes(&buffer[0]);

 //save data into a file
 CFile f;
 CFileException ex;
 TCHAR szError[1024];
 if(!f.Open(m_secu3_file_path.c_str(),CFile::modeWrite|CFile::modeCreate,&ex))
 {
  ex.GetErrorMessage(szError, 1024);
  AfxMessageBox(szError);
  return; //ошибка - данные не сохранены
 }

 f.Write(&buffer[0], m_fwdm->GetPlatformParams().m_total_size);
 f.Close();
}

void SECU3ExportController::OnCancelPressed(void)
{
 //empty
}

//нажали кнопку перемещени€ данных
void SECU3ExportController::OnExchangePressed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();

 m_fwdm->SetFunctionsSetName(other_sel, mp_fwd->maps[current_sel].name);
 std::vector<_TSTRING> strings = m_fwdm->GetFunctionsSetNames();
 mp_view->FillFWDOtherList(strings);
 mp_view->SetFWDOtherListSelection(other_sel);

 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  m_fwdm->SetStartMap(other_sel, mp_fwd->maps[current_sel].f_str);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  m_fwdm->SetIdleMap(other_sel, mp_fwd->maps[current_sel].f_idl);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  m_fwdm->SetWorkMap(other_sel, mp_fwd->maps[current_sel].f_wrk);

 if (mp_view->GetFWDFlag(FLAG_TEMP_MAP))
  m_fwdm->SetTempMap(other_sel, mp_fwd->maps[current_sel].f_tmp);
}

//модальное окно активировалось - проводим его инициализацию
void SECU3ExportController::OnViewActivate(void)
{
 mp_view->SetFWDCurrentListTitle(MLL::GetString(IDS_SECU3_CURRENT_FW));
 CString title;
 title.Format(MLL::GetString(IDS_SECU3_FW_FILE).c_str(),m_secu3_file_name.c_str());
 mp_view->SetFWDOtherListTitle(_TSTRING(title));
 
 mp_view->SetExchangeButtonCaption(_T(">"));
 mp_view->SetWindowText(MLL::LoadString(IDS_EXPORT_SECU3_TABLES));

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 std::vector<_TSTRING> strings = m_fwdm->GetFunctionsSetNames();
 mp_view->FillFWDOtherList(strings);

 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP,true);
 mp_view->SetFWDFlag(FLAG_COILREG_MAP,true);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP,true);
}

void SECU3ExportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void SECU3ExportController::OnOtherListNameChanged(int item, CString text)
{
 m_fwdm->SetFunctionsSetName(item, text.GetBuffer(1000));
}
