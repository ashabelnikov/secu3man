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

/** \file MPSZImpExpController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MPSZImpExpController.h"

#include "common/FastDelegate.h"
#include "MapImpExpDlg.h"
#include "MPSZFileDataIO.h"

using namespace fastdelegate;

MPSZImportController::MPSZImportController(FWMapsDataHolder* ip_fwd)
: mp_fwd(ip_fwd)
, mp_view(new CMapImpExpDlg())
, mp_mpsz_io(new MPSZFileDataIO())
, m_mpsz_file_name(_T(""))
{
 ASSERT(mp_fwd);
 ASSERT(mp_view);
 ASSERT(mp_mpsz_io);

 //Назначаем обработчики событий от представления
 mp_view->setOnOkButton(MakeDelegate(this,&MPSZImportController::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&MPSZImportController::OnCancelPressed));
 mp_view->setOnExchangeButton(MakeDelegate(this,&MPSZImportController::OnExchangePressed));
 mp_view->setOnActivate(MakeDelegate(this,&MPSZImportController::OnViewActivate));
 mp_view->setIsExchengeButtonAllowed(MakeDelegate(this,&MPSZImportController::IsExchangeButtonAllowed));
 mp_view->setOnFWDCurrentListNamechanged(MakeDelegate(this,&MPSZImportController::OnCurrentListNameChanged));
 mp_view->setOnFWDOtherListNamechanged(MakeDelegate(this,&MPSZImportController::OnOtherListNameChanged));
}

int MPSZImportController::DoImport(void)
{
 static TCHAR BASED_CODE szFilter[] = _T("MPSZ Files (*.mpx)|*.mpx|MPSZ-II Files (*.mpx)|*.mpx|MPSZ Files (*.mpz)|*.mpz|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);

 if (open.DoModal()==IDOK)
 {
  MPSZFileDataIO::EFileTypes type;
  if (open.GetOFN().nFilterIndex == 1 && open.GetFileExt()==_T("mpx"))
   type = MPSZFileDataIO::FILE_TYPE_MPX;
  else if (open.GetOFN().nFilterIndex == 2 && open.GetFileExt()==_T("mpx"))
   type = MPSZFileDataIO::FILE_TYPE_MPXv2;
  else if (open.GetOFN().nFilterIndex == 3 && open.GetFileExt()==_T("mpz"))
   type = MPSZFileDataIO::FILE_TYPE_MPZ;
  else
  {
   if (open.GetFileExt() == _T("mpx"))
    type = MPSZFileDataIO::FILE_TYPE_MPX;
   else if (open.GetFileExt() == _T("mpz"))
    type = MPSZFileDataIO::FILE_TYPE_MPZ;
   else
    type = MPSZFileDataIO::FILE_TYPE_MPX; //если у файла нет расширения или оно другое то по умолчанию mpx
  }

  m_mpsz_file_name = _TSTRING(open.GetFileName());
  bool result = mp_mpsz_io->Load(_TSTRING(open.GetPathName()),type);
  if (!result)
  {
   AfxMessageBox(MLL::LoadString(IDS_CANT_LOAD_THIS_FILE),MB_OK|MB_ICONWARNING);
   return IDCANCEL;
  }
 }
 else
  return IDCANCEL; //отказ пользователя

 return mp_view->DoModal();
}

bool MPSZImportController::IsExchangeButtonAllowed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();
 return (current_sel != -1) && (other_sel != -1);
}

MPSZImportController::~MPSZImportController()
{
 delete mp_view; //avoid memory leak
 delete mp_mpsz_io;
}

void MPSZImportController::OnOkPressed(void)
{
 //копируем таблицу сетки оборотов
 memcpy(mp_fwd->rpm_slots, mp_mpsz_io->GetData().rpm_slots, sizeof(float) * F_RPM_SLOTS);
}

void MPSZImportController::OnCancelPressed(void)
{
 //empty
}

//нажали кнопку перемещения данных
void MPSZImportController::OnExchangePressed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();

 if (mp_mpsz_io->GetData().maps[other_sel].name!=_T(""))
  mp_fwd->maps[current_sel].name = mp_mpsz_io->GetData().maps[other_sel].name;
 else
 { //если строка пустая, то генерируем "искусcтвенное" имя
  TCHAR name[32];
  _stprintf(name,MLL::GetString(IDS_MAP_NO_NAME).c_str(),other_sel+1);
  mp_fwd->maps[current_sel].name = name;
 }

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 mp_view->SetFWDCurrentListSelection(current_sel);

 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  memcpy(mp_fwd->maps[current_sel].f_str, mp_mpsz_io->GetData().maps[other_sel].f_str,sizeof(float) * F_STR_POINTS);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  memcpy(mp_fwd->maps[current_sel].f_idl, mp_mpsz_io->GetData().maps[other_sel].f_idl,sizeof(float) * F_IDL_POINTS);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  memcpy(mp_fwd->maps[current_sel].f_wrk, mp_mpsz_io->GetData().maps[other_sel].f_wrk,sizeof(float) * F_WRK_POINTS_L * F_WRK_POINTS_F);
}

//модальное окно активировалось - проводим его инициализацию
void MPSZImportController::OnViewActivate(void)
{
 mp_view->SetFWDCurrentListTitle(MLL::GetString(IDS_SECU3_CURRENT_FW));
 CString title;
 title.Format(MLL::GetString(IDS_MPSZ_FW_FILE).c_str(),m_mpsz_file_name.c_str());
 mp_view->SetFWDOtherListTitle(_TSTRING(title));

 //ignition
 mp_view->SetFWDFlag(FLAG_TEMP_MAP, false);
 mp_view->EnableFWDFlag(FLAG_TEMP_MAP, false);
 //fuel injection
 mp_view->SetFWDFlag(FLAG_VE_MAP, false);
 mp_view->EnableFWDFlag(FLAG_VE_MAP, false);
 mp_view->SetFWDFlag(FLAG_AFR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_AFR_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRNK_MAP, false);
 mp_view->EnableFWDFlag(FLAG_CRNK_MAP, false);
 mp_view->SetFWDFlag(FLAG_WRMP_MAP, false);
 mp_view->EnableFWDFlag(FLAG_WRMP_MAP, false);
 mp_view->SetFWDFlag(FLAG_DEAD_MAP, false);
 mp_view->EnableFWDFlag(FLAG_DEAD_MAP, false);
 mp_view->SetFWDFlag(FLAG_IDLR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IDLR_MAP, false);
 mp_view->SetFWDFlag(FLAG_IDLC_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IDLC_MAP, false);
 mp_view->SetFWDFlag(FLAG_AETPS_MAP, false);
 mp_view->EnableFWDFlag(FLAG_AETPS_MAP, false);
 mp_view->SetFWDFlag(FLAG_AERPM_MAP, false);
 mp_view->EnableFWDFlag(FLAG_AERPM_MAP, false);
 mp_view->SetFWDFlag(FLAG_AFTSTR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_AFTSTR_MAP, false);
 mp_view->SetFWDFlag(FLAG_IT_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IT_MAP, false);
 mp_view->SetFWDFlag(FLAG_ITRPM_MAP, false);
 mp_view->EnableFWDFlag(FLAG_ITRPM_MAP, false);
 mp_view->SetFWDFlag(FLAG_RIGID_MAP, false);
 mp_view->EnableFWDFlag(FLAG_RIGID_MAP, false);
 mp_view->SetFWDFlag(FLAG_EGOCRV_MAP, false);
 mp_view->EnableFWDFlag(FLAG_EGOCRV_MAP, false);
 mp_view->SetFWDFlag(FLAG_IACCORRW_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IACCORRW_MAP, false);
 mp_view->SetFWDFlag(FLAG_IACCORR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IACCORR_MAP, false);
 mp_view->SetFWDFlag(FLAG_IATCLT_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IATCLT_MAP, false);
 mp_view->SetFWDFlag(FLAG_TPSSWT_MAP, false);
 mp_view->EnableFWDFlag(FLAG_TPSSWT_MAP, false);

 //separate
 mp_view->SetFWDFlag(FLAG_DWLCNTR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_DWLCNTR_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP, false);
 mp_view->EnableFWDFlag(FLAG_ATTEN_MAP, false);
 mp_view->SetFWDFlag(FLAG_CTS_MAP, false);
 mp_view->EnableFWDFlag(FLAG_CTS_MAP, false);
 mp_view->SetFWDFlag(FLAG_CHOKE_MAP, false);
 mp_view->EnableFWDFlag(FLAG_CHOKE_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATS_MAP, false);
 mp_view->EnableFWDFlag(FLAG_ATS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATSAAC_MAP, false);
 mp_view->EnableFWDFlag(FLAG_ATSAAC_MAP, false);
 mp_view->SetFWDFlag(FLAG_GASDOSE_MAP, false);     //GD
 mp_view->EnableFWDFlag(FLAG_GASDOSE_MAP, false);  //GD
 mp_view->EnableFWDFlag(FLAG_BAROCORR_MAP, false);

 mp_view->SetExchangeButtonCaption(_T("<"));
 mp_view->SetWindowText(MLL::LoadString(IDS_IMPORT_MPSZ_TABLES));

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 std::vector<_TSTRING> strings = mp_mpsz_io->GetData().GetListOfNames();
 mp_view->FillFWDOtherList(strings);

 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
}

void MPSZImportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void MPSZImportController::OnOtherListNameChanged(int item, CString text)
{
 mp_mpsz_io->GetDataLeft().maps[item].name = text;
}

//////////////////////////////////////////////////////////////////////////////////////////
MPSZExportController::MPSZExportController(FWMapsDataHolder* ip_fwd)
: mp_fwd(ip_fwd)
, mp_view(new CMapImpExpDlg())
, mp_mpsz_io(new MPSZFileDataIO())
, m_mpsz_file_name(_T(""))
{
 ASSERT(mp_fwd);
 ASSERT(mp_view);
 ASSERT(mp_mpsz_io);

 //Назначаем обработчики событий от представления
 mp_view->setOnOkButton(MakeDelegate(this,&MPSZExportController::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&MPSZExportController::OnCancelPressed));
 mp_view->setOnExchangeButton(MakeDelegate(this,&MPSZExportController::OnExchangePressed));
 mp_view->setOnActivate(MakeDelegate(this,&MPSZExportController::OnViewActivate));
 mp_view->setIsExchengeButtonAllowed(MakeDelegate(this,&MPSZExportController::IsExchangeButtonAllowed));
 mp_view->setOnFWDCurrentListNamechanged(MakeDelegate(this,&MPSZExportController::OnCurrentListNameChanged));
 mp_view->setOnFWDOtherListNamechanged(MakeDelegate(this,&MPSZExportController::OnOtherListNameChanged));
}

int MPSZExportController::DoExport(void)
{
 //этот класс необходим чтобы изменять дефаултное расширение в зависимости от выбранного фильтра
 class CFileDialogEx : public CFileDialog
 {
  public:
   CFileDialogEx() : CFileDialog(FALSE,_T("mpx"), NULL, NULL,
     _T("MPSZ Files (*.mpx)|*.mpx|MPSZ-II Files (*.mpx)|*.mpx|MPSZ Files (*.mpz)|*.mpz|All Files (*.*)|*.*||"), NULL) {};
   virtual ~CFileDialogEx() {};

  protected:
   virtual void OnTypeChange( )
   {
    if (m_ofn.nFilterIndex==1)
     m_ofn.lpstrDefExt = _T("mpx");
    if (m_ofn.nFilterIndex==2)
     m_ofn.lpstrDefExt = _T("mpx");
    if (m_ofn.nFilterIndex==3)
     m_ofn.lpstrDefExt = _T("mpz");
   }
 };

 CFileDialogEx save;
 if (save.DoModal()==IDOK)
 {
  MPSZFileDataIO::EFileTypes type;
  if (save.GetOFN().nFilterIndex == 1) //mpx
  {
   type = MPSZFileDataIO::FILE_TYPE_MPX;
   mp_mpsz_io->SetActualSetsNumber(MPSZ_NUMBER_OF_MAPS);
  }
  if (save.GetOFN().nFilterIndex == 2)  //mpx MPSZ II
  {
   type = MPSZFileDataIO::FILE_TYPE_MPXv2;
   mp_mpsz_io->SetActualSetsNumber(MPSZ_NUMBER_OF_MAPS);
  }
  else if (save.GetOFN().nFilterIndex == 3) //mpz
  {
   type = MPSZFileDataIO::FILE_TYPE_MPZ;
   mp_mpsz_io->SetActualSetsNumber(MPSZ_NUMBER_OF_MAPS_IN_MPZ_FILE);
  }
  else
  {
   type = MPSZFileDataIO::FILE_TYPE_MPX; //если у файла нет расширения или оно другое то по умолчанию mpx
   mp_mpsz_io->SetActualSetsNumber(MPSZ_NUMBER_OF_MAPS);
  }

  //сохраняет имя файла выбранного пользователем для сохранения
  m_mpsz_file_name = _TSTRING(save.GetFileName());

  int id = mp_view->DoModal();
  if (id == IDOK)
  {
   bool result = mp_mpsz_io->Save(_TSTRING(save.GetPathName()),type);
   if (!result)
   {
    AfxMessageBox(MLL::LoadString(IDS_CANT_SAVE_THIS_FILE),MB_OK|MB_ICONWARNING);
    return IDCANCEL;
   }
  }
  return id;
 }
 else
  return IDCANCEL; //отказ пользователя
}

bool MPSZExportController::IsExchangeButtonAllowed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();
 return (current_sel != -1) && (other_sel != -1);
}

MPSZExportController::~MPSZExportController()
{
 delete mp_view; //avoid memory leak
 delete mp_mpsz_io;
}

void MPSZExportController::OnOkPressed(void)
{
 //копируем таблицу сетки оборотов
 memcpy(mp_mpsz_io->GetDataLeft().rpm_slots, mp_fwd->rpm_slots, sizeof(float) * F_RPM_SLOTS);
}

void MPSZExportController::OnCancelPressed(void)
{
 //empty
}

//нажали кнопку перемещения данных
void MPSZExportController::OnExchangePressed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();

 mp_mpsz_io->GetDataLeft().maps[other_sel].name = mp_fwd->maps[current_sel].name;
 mp_view->FillFWDOtherList(mp_mpsz_io->GetData().GetListOfNames());
 mp_view->SetFWDOtherListSelection(other_sel);

 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  memcpy(mp_mpsz_io->GetDataLeft().maps[other_sel].f_str, mp_fwd->maps[current_sel].f_str, sizeof(float) * F_STR_POINTS);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  memcpy(mp_mpsz_io->GetDataLeft().maps[other_sel].f_idl, mp_fwd->maps[current_sel].f_idl, sizeof(float) * F_IDL_POINTS);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  memcpy(mp_mpsz_io->GetDataLeft().maps[other_sel].f_wrk, mp_fwd->maps[current_sel].f_wrk, sizeof(float) * F_WRK_POINTS_L * F_WRK_POINTS_F);
}

//modal window has activated - perform its initialization
void MPSZExportController::OnViewActivate(void)
{
 mp_view->SetFWDCurrentListTitle(MLL::GetString(IDS_SECU3_CURRENT_FW));
 CString title;
 title.Format(MLL::GetString(IDS_MPSZ_FW_FILE).c_str(),m_mpsz_file_name.c_str());
 mp_view->SetFWDOtherListTitle(_TSTRING(title));

 //ignition
 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP, false);     //absent in MPSZ
 mp_view->EnableFWDFlag(FLAG_TEMP_MAP, false);
 //fuel injection
 mp_view->SetFWDFlag(FLAG_VE_MAP, false);
 mp_view->EnableFWDFlag(FLAG_VE_MAP, false);
 mp_view->SetFWDFlag(FLAG_AFR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_AFR_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRNK_MAP, false);
 mp_view->EnableFWDFlag(FLAG_CRNK_MAP, false);
 mp_view->SetFWDFlag(FLAG_WRMP_MAP, false);
 mp_view->EnableFWDFlag(FLAG_WRMP_MAP, false);
 mp_view->SetFWDFlag(FLAG_DEAD_MAP, false);
 mp_view->EnableFWDFlag(FLAG_DEAD_MAP, false);
 mp_view->SetFWDFlag(FLAG_IDLR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IDLR_MAP, false);
 mp_view->SetFWDFlag(FLAG_IDLC_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IDLC_MAP, false);
 mp_view->SetFWDFlag(FLAG_AETPS_MAP, false);
 mp_view->EnableFWDFlag(FLAG_AETPS_MAP, false);
 mp_view->SetFWDFlag(FLAG_AERPM_MAP, false);
 mp_view->EnableFWDFlag(FLAG_AERPM_MAP, false);
 mp_view->SetFWDFlag(FLAG_AFTSTR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_AFTSTR_MAP, false);
 mp_view->SetFWDFlag(FLAG_IT_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IT_MAP, false);
 mp_view->SetFWDFlag(FLAG_ITRPM_MAP, false);
 mp_view->EnableFWDFlag(FLAG_ITRPM_MAP, false);
 mp_view->SetFWDFlag(FLAG_RIGID_MAP, false);
 mp_view->EnableFWDFlag(FLAG_RIGID_MAP, false);
 mp_view->SetFWDFlag(FLAG_EGOCRV_MAP, false);
 mp_view->EnableFWDFlag(FLAG_EGOCRV_MAP, false);
 mp_view->SetFWDFlag(FLAG_IACCORRW_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IACCORRW_MAP, false);
 mp_view->SetFWDFlag(FLAG_IACCORR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IACCORR_MAP, false);
 mp_view->SetFWDFlag(FLAG_IATCLT_MAP, false);
 mp_view->EnableFWDFlag(FLAG_IATCLT_MAP, false);
 mp_view->SetFWDFlag(FLAG_TPSSWT_MAP, false);
 mp_view->EnableFWDFlag(FLAG_TPSSWT_MAP, false);

 //separate
 mp_view->SetFWDFlag(FLAG_DWLCNTR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_DWLCNTR_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP, false);
 mp_view->EnableFWDFlag(FLAG_ATTEN_MAP, false);
 mp_view->SetFWDFlag(FLAG_CTS_MAP, false);
 mp_view->EnableFWDFlag(FLAG_CTS_MAP, false);
 mp_view->SetFWDFlag(FLAG_CHOKE_MAP, false);
 mp_view->EnableFWDFlag(FLAG_CHOKE_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATS_MAP, false);
 mp_view->EnableFWDFlag(FLAG_ATS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATSAAC_MAP, false);
 mp_view->EnableFWDFlag(FLAG_ATSAAC_MAP, false);
 mp_view->SetFWDFlag(FLAG_GASDOSE_MAP, false);    //GD
 mp_view->EnableFWDFlag(FLAG_GASDOSE_MAP, false); //GD
 mp_view->SetFWDFlag(FLAG_BAROCORR_MAP, false);
 mp_view->EnableFWDFlag(FLAG_BAROCORR_MAP, false);
 mp_view->SetFWDFlag(FLAG_MANIT_MAP, false);
 mp_view->EnableFWDFlag(FLAG_MANIT_MAP, false);
 
 mp_view->SetExchangeButtonCaption(_T(">"));
 mp_view->SetWindowText(MLL::LoadString(IDS_EXPORT_MPSZ_TABLES));

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 std::vector<_TSTRING> strings = mp_mpsz_io->GetData().GetListOfNames();
 mp_view->FillFWDOtherList(strings);
}

void MPSZExportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void MPSZExportController::OnOtherListNameChanged(int item, CString text)
{
 mp_mpsz_io->GetDataLeft().maps[item].name = text;
}
