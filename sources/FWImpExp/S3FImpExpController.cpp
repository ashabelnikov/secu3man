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
#include "S3FImpExpController.h"

#include "common/FastDelegate.h"
#include "MapImpExpDlg.h"
#include "S3FFileDataIO.h"

using namespace fastdelegate;

namespace {
void GenArtificialName(_TSTRING& str, size_t index)
{
 if (str == _T(""))
 { //искусственное им€
  TCHAR name[32];
  _stprintf(name, MLL::GetString(IDS_MAP_NO_NAME).c_str(), index);
  str = _TSTRING(name);
 }
}
std::vector<_TSTRING> GenArtificialNames(const std::vector<_TSTRING>& inpNames)
{
 std::vector<_TSTRING> strings = inpNames;
 for (size_t i = 0; i < strings.size(); ++i)
  GenArtificialName(strings[i], i+1); 
 return strings;
}
}

S3FImportController::S3FImportController(FWMapsDataHolder* ip_fwd)
: mp_fwd(ip_fwd)
, mp_view(new CMapImpExpDlg())
, mp_s3f_io(new S3FFileDataIO())
, m_s3f_file_name(_T(""))
{
 ASSERT(mp_fwd);
 ASSERT(mp_view);
 ASSERT(mp_s3f_io);

 //Ќазначаем обработчики событий от представлени€
 mp_view->setOnOkButton(MakeDelegate(this,&S3FImportController::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&S3FImportController::OnCancelPressed));
 mp_view->setOnExchangeButton(MakeDelegate(this,&S3FImportController::OnExchangePressed));
 mp_view->setOnActivate(MakeDelegate(this,&S3FImportController::OnViewActivate));
 mp_view->setIsExchengeButtonAllowed(MakeDelegate(this,&S3FImportController::IsExchangeButtonAllowed));
 mp_view->setOnFWDCurrentListNamechanged(MakeDelegate(this,&S3FImportController::OnCurrentListNameChanged));
 mp_view->setOnFWDOtherListNamechanged(MakeDelegate(this,&S3FImportController::OnOtherListNameChanged));
}

int S3FImportController::DoImport(void)
{
 static TCHAR BASED_CODE szFilter[] = _T("SECU-3 Format Files (*.s3f)|*.s3f|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);

 if (open.DoModal()==IDOK)
 {
  m_s3f_file_name = _TSTRING(open.GetFileName());
  bool result = mp_s3f_io->Load(m_s3f_file_name);
  if (!result)
  {
   AfxMessageBox(MLL::LoadString(IDS_CANT_LOAD_THIS_FILE),MB_OK|MB_ICONWARNING);
   return IDCANCEL;
  }
  if (!mp_s3f_io->IsFileIntegrityOk())
  { //file CRC is bad, warn user
   if (IDCANCEL == AfxMessageBox(MLL::LoadString(IDS_FILE_WRONG_CRC),MB_OKCANCEL|MB_ICONWARNING))
    return IDCANCEL;
  }
 }
 else
  return IDCANCEL; //отказ пользовател€

 return mp_view->DoModal();
}

bool S3FImportController::IsExchangeButtonAllowed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();
 return (current_sel != -1) && (other_sel != -1);
}

S3FImportController::~S3FImportController()
{
 delete mp_view; //avoid memory leak
 delete mp_s3f_io;
}

void S3FImportController::OnOkPressed(void)
{
 //import separate tables
 if (mp_view->GetFWDFlag(FLAG_ATTEN_MAP))
  memcpy(mp_fwd->attenuator_table, mp_s3f_io->GetData().attenuator_table, sizeof(float) * KC_ATTENUATOR_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_DWLCNTR_MAP))
  memcpy(mp_fwd->dwellcntrl_table, mp_s3f_io->GetData().dwellcntrl_table, sizeof(float) * COIL_ON_TIME_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_CTS_MAP))
 {
  memcpy(mp_fwd->ctscurve_table, mp_s3f_io->GetData().ctscurve_table, sizeof(float) * THERMISTOR_LOOKUP_TABLE_SIZE);
  mp_fwd->ctscurve_vlimits[0] = mp_s3f_io->GetData().ctscurve_vlimits[0];
  mp_fwd->ctscurve_vlimits[1] = mp_s3f_io->GetData().ctscurve_vlimits[1];
 }
}

void S3FImportController::OnCancelPressed(void)
{
 //empty
}

//нажали кнопку перемещени€ данных
void S3FImportController::OnExchangePressed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();

 if (mp_s3f_io->GetData().maps[other_sel].name!=_T(""))
  mp_fwd->maps[current_sel].name = mp_s3f_io->GetData().maps[other_sel].name;
 else
 { //если строка пуста€, то генерируем "искусcтвенное" им€
  TCHAR name[32];
  _stprintf(name,MLL::GetString(IDS_MAP_NO_NAME).c_str(),other_sel+1);
  mp_fwd->maps[current_sel].name = name;
 }

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 mp_view->SetFWDCurrentListSelection(current_sel);

 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  memcpy(mp_fwd->maps[current_sel].f_str, mp_s3f_io->GetData().maps[other_sel].f_str,sizeof(float) * F_STR_POINTS);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  memcpy(mp_fwd->maps[current_sel].f_idl, mp_s3f_io->GetData().maps[other_sel].f_idl,sizeof(float) * F_IDL_POINTS);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  memcpy(mp_fwd->maps[current_sel].f_wrk, mp_s3f_io->GetData().maps[other_sel].f_wrk,sizeof(float) * F_WRK_POINTS_L * F_WRK_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_TEMP_MAP))
  memcpy(mp_fwd->maps[current_sel].f_tmp, mp_s3f_io->GetData().maps[other_sel].f_tmp,sizeof(float) * F_TMP_POINTS);
}

//модальное окно активировалось - проводим его инициализацию
void S3FImportController::OnViewActivate(void)
{
 mp_view->SetFWDCurrentListTitle(MLL::GetString(IDS_SECU3_CURRENT_FW));
 CString title;
 title.Format(MLL::GetString(IDS_S3F_FW_FILE).c_str(),m_s3f_file_name.c_str());
 mp_view->SetFWDOtherListTitle(_TSTRING(title));

 mp_view->SetExchangeButtonCaption(_T("<"));
 mp_view->SetWindowText(MLL::LoadString(IDS_IMPORT_S3F_TABLES));

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 std::vector<_TSTRING> strings = mp_s3f_io->GetData().GetListOfNames();
 mp_view->FillFWDOtherList(GenArtificialNames(strings));

 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP, true);
 mp_view->SetFWDFlag(FLAG_DWLCNTR_MAP, true);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP, true);
 mp_view->SetFWDFlag(FLAG_CTS_MAP, true);
}

void S3FImportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void S3FImportController::OnOtherListNameChanged(int item, CString text)
{
 mp_s3f_io->GetDataLeft().maps[item].name = text;
}

//////////////////////////////////////////////////////////////////////////////////////////
S3FExportController::S3FExportController(FWMapsDataHolder* ip_fwd)
: mp_fwd(ip_fwd)
, mp_view(new CMapImpExpDlg())
, mp_s3f_io(new S3FFileDataIO())
, m_s3f_file_name(_T(""))
{
 ASSERT(mp_fwd);
 ASSERT(mp_view);
 ASSERT(mp_s3f_io);

 //Ќазначаем обработчики событий от представлени€
 mp_view->setOnOkButton(MakeDelegate(this,&S3FExportController::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&S3FExportController::OnCancelPressed));
 mp_view->setOnExchangeButton(MakeDelegate(this,&S3FExportController::OnExchangePressed));
 mp_view->setOnActivate(MakeDelegate(this,&S3FExportController::OnViewActivate));
 mp_view->setIsExchengeButtonAllowed(MakeDelegate(this,&S3FExportController::IsExchangeButtonAllowed));
 mp_view->setOnFWDCurrentListNamechanged(MakeDelegate(this,&S3FExportController::OnCurrentListNameChanged));
 mp_view->setOnFWDOtherListNamechanged(MakeDelegate(this,&S3FExportController::OnOtherListNameChanged));
}

int S3FExportController::DoExport(void)
{
 //этот класс необходим чтобы измен€ть дефаултное расширение в зависимости от выбранного фильтра
 class CFileDialogEx : public CFileDialog
 {
  public:
   CFileDialogEx() : CFileDialog(FALSE,_T("s3f"), NULL, NULL,
     _T("SECU-3 Format Files (*.s3f)|*.s3f|All Files (*.*)|*.*||"), NULL) {};
   virtual ~CFileDialogEx() {};

  protected:
   virtual void OnTypeChange( )
   {
    if (m_ofn.nFilterIndex==1)
     m_ofn.lpstrDefExt = _T("s3f");
   }
 };

 CFileDialogEx save;
 if (save.DoModal()==IDOK)
 {
  //сохран€ет им€ файла выбранного пользователем дл€ сохранени€
  m_s3f_file_name = _TSTRING(save.GetFileName());

  int id = mp_view->DoModal();
  if (id == IDOK)
  {
   bool result = mp_s3f_io->Save(m_s3f_file_name);
   if (!result)
   {
    AfxMessageBox(MLL::LoadString(IDS_CANT_SAVE_THIS_FILE),MB_OK|MB_ICONWARNING);
    return IDCANCEL;
   }
  }
  return id;
 }
 else
  return IDCANCEL; //отказ пользовател€
}

bool S3FExportController::IsExchangeButtonAllowed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();
 return (current_sel != -1) && (other_sel != -1);
}

S3FExportController::~S3FExportController()
{
 delete mp_view; //avoid memory leak
 delete mp_s3f_io;
}

void S3FExportController::OnOkPressed(void)
{
 //import separate tables
 if (mp_view->GetFWDFlag(FLAG_ATTEN_MAP))
  memcpy(mp_s3f_io->GetDataLeft().attenuator_table, mp_fwd->attenuator_table, sizeof(float) * KC_ATTENUATOR_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_DWLCNTR_MAP))
  memcpy(mp_s3f_io->GetDataLeft().dwellcntrl_table, mp_fwd->dwellcntrl_table, sizeof(float) * COIL_ON_TIME_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_CTS_MAP))
 {
  memcpy(mp_s3f_io->GetDataLeft().ctscurve_table, mp_fwd->ctscurve_table, sizeof(float) * THERMISTOR_LOOKUP_TABLE_SIZE);
  mp_s3f_io->GetDataLeft().ctscurve_vlimits[0] = mp_fwd->ctscurve_vlimits[0];
  mp_s3f_io->GetDataLeft().ctscurve_vlimits[1] = mp_fwd->ctscurve_vlimits[1];
 }

 //empty strings must be replaced with some default names
 for(size_t i = 0; i < mp_s3f_io->GetMapSetsNumber(); ++i)
  GenArtificialName(mp_s3f_io->GetDataLeft().maps[i].name, i+1);
}

void S3FExportController::OnCancelPressed(void)
{
 //empty
}

//нажали кнопку перемещени€ данных
void S3FExportController::OnExchangePressed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();

 mp_s3f_io->GetDataLeft().maps[other_sel].name = mp_fwd->maps[current_sel].name;
 std::vector<_TSTRING> strings = mp_s3f_io->GetData().GetListOfNames();
 mp_view->FillFWDOtherList(GenArtificialNames(strings));
 mp_view->SetFWDOtherListSelection(other_sel);

 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].f_str, mp_fwd->maps[current_sel].f_str, sizeof(float) * F_STR_POINTS);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].f_idl, mp_fwd->maps[current_sel].f_idl, sizeof(float) * F_IDL_POINTS);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].f_wrk, mp_fwd->maps[current_sel].f_wrk, sizeof(float) * F_WRK_POINTS_L * F_WRK_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_TEMP_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].f_tmp, mp_fwd->maps[current_sel].f_tmp, sizeof(float) * F_TMP_POINTS);
}

//модальное окно активировалось - проводим его инициализацию
void S3FExportController::OnViewActivate(void)
{
 mp_view->SetFWDCurrentListTitle(MLL::GetString(IDS_SECU3_CURRENT_FW));
 CString title;
 title.Format(MLL::GetString(IDS_S3F_FW_FILE).c_str(),m_s3f_file_name.c_str());
 mp_view->SetFWDOtherListTitle(_TSTRING(title));
 
 mp_view->SetExchangeButtonCaption(_T(">"));
 mp_view->SetWindowText(MLL::LoadString(IDS_EXPORT_S3F_TABLES));

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 std::vector<_TSTRING> strings = mp_s3f_io->GetData().GetListOfNames();
 mp_view->FillFWDOtherList(GenArtificialNames(strings));

 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP, true);
 mp_view->SetFWDFlag(FLAG_DWLCNTR_MAP, true);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP, true);
 mp_view->SetFWDFlag(FLAG_CTS_MAP, true);
}

void S3FExportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void S3FExportController::OnOtherListNameChanged(int item, CString text)
{
 mp_s3f_io->GetDataLeft().maps[item].name = text;
}
