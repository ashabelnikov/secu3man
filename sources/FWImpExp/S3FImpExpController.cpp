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

/** \file S3FImpExpController.cpp
 * \author Alexey A. Shabelnikov
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
  bool result = mp_s3f_io->Load(_TSTRING(open.GetPathName()));
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
  return IDCANCEL; //canceled by user

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

 if (mp_view->GetFWDFlag(FLAG_ATS_MAP))
 {
  memcpy(mp_fwd->atscurve_table, mp_s3f_io->GetData().atscurve_table, sizeof(float) * THERMISTOR_LOOKUP_TABLE_SIZE);
  mp_fwd->atscurve_vlimits[0] = mp_s3f_io->GetData().atscurve_vlimits[0];
  mp_fwd->atscurve_vlimits[1] = mp_s3f_io->GetData().atscurve_vlimits[1];
 }

 if (mp_view->GetFWDFlag(FLAG_ATSAAC_MAP))
  memcpy(mp_fwd->ats_corr_table, mp_s3f_io->GetData().ats_corr_table, sizeof(float) * ATS_CORR_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_GASDOSE_MAP))
  memcpy(mp_fwd->gasdose_pos_table, mp_s3f_io->GetData().gasdose_pos_table, sizeof(float) * GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE);

 if (mp_view->GetFWDFlag(FLAG_TMP2CURVE_MAP))
  memcpy(mp_fwd->tmp2_curve, mp_s3f_io->GetData().tmp2_curve, sizeof(float) * (THERMISTOR_LOOKUP_TABLE_SIZE+2));

 //copy RPM grid
 memcpy(mp_fwd->rpm_slots, mp_s3f_io->GetData().rpm_slots, sizeof(float) * F_RPM_SLOTS);
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

 //ignition
 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  memcpy(mp_fwd->maps[current_sel].f_str, mp_s3f_io->GetData().maps[other_sel].f_str,sizeof(float) * F_STR_POINTS);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  memcpy(mp_fwd->maps[current_sel].f_idl, mp_s3f_io->GetData().maps[other_sel].f_idl,sizeof(float) * F_IDL_POINTS);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  memcpy(mp_fwd->maps[current_sel].f_wrk, mp_s3f_io->GetData().maps[other_sel].f_wrk,sizeof(float) * F_WRK_POINTS_L * F_WRK_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_TEMP_MAP))
  memcpy(mp_fwd->maps[current_sel].f_tmp, mp_s3f_io->GetData().maps[other_sel].f_tmp,sizeof(float) * F_TMP_POINTS);

 //fuel injection
 if (mp_view->GetFWDFlag(FLAG_VE_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_ve, mp_s3f_io->GetData().maps[other_sel].inj_ve,sizeof(float) * INJ_VE_POINTS_L * INJ_VE_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_AFR_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_afr, mp_s3f_io->GetData().maps[other_sel].inj_afr,sizeof(float) * INJ_VE_POINTS_L * INJ_VE_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_CRNK_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_cranking, mp_s3f_io->GetData().maps[other_sel].inj_cranking,sizeof(float) * INJ_CRANKING_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_WRMP_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_warmup, mp_s3f_io->GetData().maps[other_sel].inj_warmup,sizeof(float) * INJ_WARMUP_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_DEAD_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_dead_time, mp_s3f_io->GetData().maps[other_sel].inj_dead_time,sizeof(float) * INJ_DT_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_IDLR_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_iac_run_pos, mp_s3f_io->GetData().maps[other_sel].inj_iac_run_pos,sizeof(float) * INJ_IAC_POS_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_IDLC_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_iac_crank_pos, mp_s3f_io->GetData().maps[other_sel].inj_iac_crank_pos,sizeof(float) * INJ_IAC_POS_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_AETPS_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_ae_tps, mp_s3f_io->GetData().maps[other_sel].inj_ae_tps,sizeof(float) * INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2);

 if (mp_view->GetFWDFlag(FLAG_AERPM_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_ae_rpm, mp_s3f_io->GetData().maps[other_sel].inj_ae_rpm,sizeof(float) * INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2);

 if (mp_view->GetFWDFlag(FLAG_AFTSTR_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_aftstr, mp_s3f_io->GetData().maps[other_sel].inj_aftstr, sizeof(float) * INJ_AFTSTR_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_IT_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_timing, mp_s3f_io->GetData().maps[other_sel].inj_timing,sizeof(float) * INJ_VE_POINTS_L * INJ_VE_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_ITRPM_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_target_rpm, mp_s3f_io->GetData().maps[other_sel].inj_target_rpm,sizeof(float) * INJ_TARGET_RPM_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_RIGID_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_idl_rigidity, mp_s3f_io->GetData().maps[other_sel].inj_idl_rigidity,sizeof(float) * INJ_IDL_RIGIDITY_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_EGOCRV_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_ego_curve, mp_s3f_io->GetData().maps[other_sel].inj_ego_curve, sizeof(float) * (INJ_EGO_CURVE_SIZE+2));

 if (mp_view->GetFWDFlag(FLAG_IACCORRW_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_iac_corr_w, mp_s3f_io->GetData().maps[other_sel].inj_iac_corr_w,sizeof(float) * (INJ_IAC_CORR_W_SIZE+2));

 if (mp_view->GetFWDFlag(FLAG_IACCORR_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_iac_corr, mp_s3f_io->GetData().maps[other_sel].inj_iac_corr,sizeof(float) * (INJ_IAC_CORR_SIZE+2));

 if (mp_view->GetFWDFlag(FLAG_IATCLT_MAP))
  memcpy(mp_fwd->maps[current_sel].inj_iatclt_corr, mp_s3f_io->GetData().maps[other_sel].inj_iatclt_corr,sizeof(float) * (INJ_IATCLT_CORR_SIZE+2));
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

 std::vector<_TSTRING> curr_strings = mp_fwd->GetListOfNames();
 mp_view->FillFWDCurrentList(curr_strings);
 if (1==curr_strings.size()) //select item if it is single
  mp_view->SetFWDCurrentListSelection(0);

 std::vector<_TSTRING> strings = mp_s3f_io->GetData().GetListOfNames();
 mp_view->FillFWDOtherList(GenArtificialNames(strings));
 if (1==strings.size()) //select item if it is single
  mp_view->SetFWDOtherListSelection(0);

 //ignition
 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP, true);

 bool injen = (mp_s3f_io->GetVersion() > 0x0102);
 bool sv0104 = (mp_s3f_io->GetVersion() > 0x0103);
 bool sv0105 = (mp_s3f_io->GetVersion() > 0x0104);
 bool sv0106 = (mp_s3f_io->GetVersion() > 0x0105);
 bool sv0107 = (mp_s3f_io->GetVersion() > 0x0106);

 bool sepmap = mp_s3f_io->HasSeparateMaps();

 //injection
 mp_view->SetFWDFlag(FLAG_VE_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_VE_MAP, injen);
 mp_view->SetFWDFlag(FLAG_AFR_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_AFR_MAP, injen);
 mp_view->SetFWDFlag(FLAG_CRNK_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_CRNK_MAP, injen);
 mp_view->SetFWDFlag(FLAG_WRMP_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_WRMP_MAP, injen);
 mp_view->SetFWDFlag(FLAG_DEAD_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_DEAD_MAP, injen);
 mp_view->SetFWDFlag(FLAG_IDLR_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_IDLR_MAP, injen);
 mp_view->SetFWDFlag(FLAG_IDLC_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_IDLC_MAP, injen);
 mp_view->SetFWDFlag(FLAG_AETPS_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_AETPS_MAP, injen);
 mp_view->SetFWDFlag(FLAG_AERPM_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_AERPM_MAP, injen); 
 mp_view->SetFWDFlag(FLAG_AFTSTR_MAP, injen);
 mp_view->EnableFWDFlag(FLAG_AFTSTR_MAP, injen); 
 mp_view->SetFWDFlag(FLAG_IT_MAP, sv0105);           //since v01.05
 mp_view->EnableFWDFlag(FLAG_IT_MAP, sv0105);        //since v01.05
 mp_view->SetFWDFlag(FLAG_ITRPM_MAP, sv0106);        //since v01.06
 mp_view->EnableFWDFlag(FLAG_ITRPM_MAP, sv0106);     //since v01.06
 mp_view->SetFWDFlag(FLAG_RIGID_MAP, sv0106);        //since v01.06
 mp_view->EnableFWDFlag(FLAG_RIGID_MAP, sv0106);     //since v01.06
 mp_view->SetFWDFlag(FLAG_EGOCRV_MAP, sv0105);       //since v01.05
 mp_view->EnableFWDFlag(FLAG_EGOCRV_MAP, sv0105);    //since v01.05
 mp_view->SetFWDFlag(FLAG_IACCORRW_MAP, sv0106);     //since v01.06
 mp_view->EnableFWDFlag(FLAG_IACCORRW_MAP, sv0106);  //since v01.06
 mp_view->SetFWDFlag(FLAG_IACCORR_MAP, sv0106);      //since v01.06
 mp_view->EnableFWDFlag(FLAG_IACCORR_MAP, sv0106);   //since v01.06
 mp_view->SetFWDFlag(FLAG_IATCLT_MAP, sv0106);       //since v01.06
 mp_view->EnableFWDFlag(FLAG_IATCLT_MAP, sv0106);    //since v01.06
 mp_view->SetFWDFlag(FLAG_TPSSWT_MAP, false);        //since todo
 mp_view->EnableFWDFlag(FLAG_TPSSWT_MAP, false);     //since todo
 mp_view->SetFWDFlag(FLAG_GTSC_MAP, false);          //since todo
 mp_view->EnableFWDFlag(FLAG_GTSC_MAP, false);       //since todo
 mp_view->SetFWDFlag(FLAG_GPSC_MAP, false);          //since todo
 mp_view->EnableFWDFlag(FLAG_GPSC_MAP, false);       //since todo
 mp_view->SetFWDFlag(FLAG_ATSC_MAP, false);          //since todo
 mp_view->EnableFWDFlag(FLAG_ATSC_MAP, false);       //since todo
 //separate
 mp_view->SetFWDFlag(FLAG_DWLCNTR_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP, false);
 mp_view->SetFWDFlag(FLAG_CTS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATSAAC_MAP, false);
 mp_view->SetFWDFlag(FLAG_GASDOSE_MAP, false);
 mp_view->SetFWDFlag(FLAG_BAROCORR_MAP, false);
 mp_view->SetFWDFlag(FLAG_MANIT_MAP, false);
 mp_view->SetFWDFlag(FLAG_TMP2CURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRKTEMP_MAP, false);
 mp_view->SetFWDFlag(FLAG_EHPAUSE_MAP, false);
 mp_view->EnableFWDFlag(FLAG_DWLCNTR_MAP, sepmap);
 mp_view->EnableFWDFlag(FLAG_ATTEN_MAP, sepmap);
 mp_view->EnableFWDFlag(FLAG_CTS_MAP, sepmap);
 mp_view->EnableFWDFlag(FLAG_ATS_MAP, injen && sepmap);       //since v01.03
 mp_view->EnableFWDFlag(FLAG_ATSAAC_MAP, injen && sepmap);    //since v01.03
 mp_view->EnableFWDFlag(FLAG_GASDOSE_MAP, sv0104 && sepmap);  //since v01.04

 mp_view->EnableFWDFlag(FLAG_BAROCORR_MAP, false && sepmap);  //since todo
 mp_view->EnableFWDFlag(FLAG_MANIT_MAP, false && sepmap);     //since todo

 mp_view->EnableFWDFlag(FLAG_TMP2CURVE_MAP, sv0107 && sepmap);  //since v01.07

 mp_view->EnableFWDFlag(FLAG_CRKTEMP_MAP, false && sepmap);   //since todo

 mp_view->EnableFWDFlag(FLAG_EHPAUSE_MAP, false && sepmap);   //since todo
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
   bool result = mp_s3f_io->Save(_TSTRING(save.GetPathName()));
   if (!result)
   {
    AfxMessageBox(MLL::LoadString(IDS_CANT_SAVE_THIS_FILE),MB_OK|MB_ICONWARNING);
    return IDCANCEL;
   }
  }
  return id;
 }
 else
  return IDCANCEL; //canceled by user
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
 //export separate tables
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

 if (mp_view->GetFWDFlag(FLAG_ATS_MAP))
 {
  memcpy(mp_s3f_io->GetDataLeft().atscurve_table, mp_fwd->atscurve_table, sizeof(float) * THERMISTOR_LOOKUP_TABLE_SIZE);
  mp_s3f_io->GetDataLeft().atscurve_vlimits[0] = mp_fwd->atscurve_vlimits[0];
  mp_s3f_io->GetDataLeft().atscurve_vlimits[1] = mp_fwd->atscurve_vlimits[1];
 }

 if (mp_view->GetFWDFlag(FLAG_ATSAAC_MAP))
  memcpy(mp_s3f_io->GetDataLeft().ats_corr_table, mp_fwd->ats_corr_table, sizeof(float) * ATS_CORR_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_GASDOSE_MAP))
  memcpy(mp_s3f_io->GetDataLeft().gasdose_pos_table, mp_fwd->gasdose_pos_table, sizeof(float) * GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE);

 if (mp_view->GetFWDFlag(FLAG_TMP2CURVE_MAP))
  memcpy(mp_s3f_io->GetDataLeft().tmp2_curve, mp_fwd->tmp2_curve, sizeof(float) * (THERMISTOR_LOOKUP_TABLE_SIZE+2));

 //empty strings must be replaced with some default names
 for(size_t i = 0; i < mp_s3f_io->GetData().maps.size(); ++i)
  GenArtificialName(mp_s3f_io->GetDataLeft().maps[i].name, i+1);

 //copy RPM grid
 memcpy(mp_s3f_io->GetDataLeft().rpm_slots, mp_fwd->rpm_slots, sizeof(float) * F_RPM_SLOTS);
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

 //ignition
 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].f_str, mp_fwd->maps[current_sel].f_str, sizeof(float) * F_STR_POINTS);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].f_idl, mp_fwd->maps[current_sel].f_idl, sizeof(float) * F_IDL_POINTS);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].f_wrk, mp_fwd->maps[current_sel].f_wrk, sizeof(float) * F_WRK_POINTS_L * F_WRK_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_TEMP_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].f_tmp, mp_fwd->maps[current_sel].f_tmp, sizeof(float) * F_TMP_POINTS);

 //fuel injection
 if (mp_view->GetFWDFlag(FLAG_VE_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_ve, mp_fwd->maps[current_sel].inj_ve, sizeof(float) * INJ_VE_POINTS_L * INJ_VE_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_AFR_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_afr, mp_fwd->maps[current_sel].inj_afr, sizeof(float) * INJ_VE_POINTS_L * INJ_VE_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_CRNK_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_cranking, mp_fwd->maps[current_sel].inj_cranking, sizeof(float) * INJ_CRANKING_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_WRMP_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_warmup, mp_fwd->maps[current_sel].inj_warmup, sizeof(float) * INJ_WARMUP_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_DEAD_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_dead_time, mp_fwd->maps[current_sel].inj_dead_time, sizeof(float) * INJ_DT_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_IDLR_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_iac_run_pos, mp_fwd->maps[current_sel].inj_iac_run_pos, sizeof(float) * INJ_IAC_POS_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_IDLC_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_iac_crank_pos, mp_fwd->maps[current_sel].inj_iac_crank_pos, sizeof(float) * INJ_IAC_POS_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_AETPS_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_ae_tps, mp_fwd->maps[current_sel].inj_ae_tps, sizeof(float) * INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2);

 if (mp_view->GetFWDFlag(FLAG_AERPM_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_ae_rpm, mp_fwd->maps[current_sel].inj_ae_rpm, sizeof(float) * INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2);

 if (mp_view->GetFWDFlag(FLAG_AFTSTR_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_aftstr, mp_fwd->maps[current_sel].inj_aftstr, sizeof(float) * INJ_AFTSTR_LOOKUP_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_IT_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_timing, mp_fwd->maps[current_sel].inj_timing, sizeof(float) * INJ_VE_POINTS_L * INJ_VE_POINTS_F);

 if (mp_view->GetFWDFlag(FLAG_ITRPM_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_target_rpm, mp_fwd->maps[current_sel].inj_target_rpm, sizeof(float) * INJ_TARGET_RPM_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_RIGID_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_idl_rigidity, mp_fwd->maps[current_sel].inj_idl_rigidity, sizeof(float) * INJ_IDL_RIGIDITY_TABLE_SIZE);

 if (mp_view->GetFWDFlag(FLAG_EGOCRV_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_ego_curve, mp_fwd->maps[current_sel].inj_ego_curve, sizeof(float) * (INJ_EGO_CURVE_SIZE+2));

 if (mp_view->GetFWDFlag(FLAG_IACCORRW_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_iac_corr_w, mp_fwd->maps[current_sel].inj_iac_corr_w, sizeof(float) * (INJ_IAC_CORR_W_SIZE+2));

 if (mp_view->GetFWDFlag(FLAG_IACCORR_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_iac_corr, mp_fwd->maps[current_sel].inj_iac_corr, sizeof(float) * (INJ_IAC_CORR_SIZE+2));

 if (mp_view->GetFWDFlag(FLAG_IATCLT_MAP))
  memcpy(mp_s3f_io->GetDataLeft().maps[other_sel].inj_iatclt_corr, mp_fwd->maps[current_sel].inj_iatclt_corr, sizeof(float) * (INJ_IATCLT_CORR_SIZE+2));
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

 //ignition
 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP, true);
 //injection
 mp_view->SetFWDFlag(FLAG_VE_MAP, true);
 mp_view->SetFWDFlag(FLAG_AFR_MAP, true);
 mp_view->SetFWDFlag(FLAG_CRNK_MAP, true);
 mp_view->SetFWDFlag(FLAG_WRMP_MAP, true);
 mp_view->SetFWDFlag(FLAG_DEAD_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLR_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLC_MAP, true);
 mp_view->SetFWDFlag(FLAG_AETPS_MAP, true);
 mp_view->SetFWDFlag(FLAG_AERPM_MAP, true);
 mp_view->SetFWDFlag(FLAG_AFTSTR_MAP, true);
 mp_view->SetFWDFlag(FLAG_IT_MAP, true);
 mp_view->SetFWDFlag(FLAG_ITRPM_MAP, true);
 mp_view->SetFWDFlag(FLAG_RIGID_MAP, true);
 mp_view->SetFWDFlag(FLAG_EGOCRV_MAP, true);
 mp_view->SetFWDFlag(FLAG_IACCORRW_MAP, true);
 mp_view->SetFWDFlag(FLAG_IACCORR_MAP, true);
 mp_view->SetFWDFlag(FLAG_IATCLT_MAP, true);
 mp_view->SetFWDFlag(FLAG_TPSSWT_MAP, false);    //todo
 mp_view->EnableFWDFlag(FLAG_TPSSWT_MAP, false); //todo
 mp_view->SetFWDFlag(FLAG_GTSC_MAP, false);      //todo
 mp_view->EnableFWDFlag(FLAG_GTSC_MAP, false);   //todo
 mp_view->SetFWDFlag(FLAG_GPSC_MAP, false);      //todo
 mp_view->EnableFWDFlag(FLAG_GPSC_MAP, false);   //todo
 mp_view->SetFWDFlag(FLAG_ATSC_MAP, false);      //todo
 mp_view->EnableFWDFlag(FLAG_ATSC_MAP, false);   //todo
 //separate
 mp_view->SetFWDFlag(FLAG_DWLCNTR_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP, false);
 mp_view->SetFWDFlag(FLAG_CTS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATSAAC_MAP, false);
 mp_view->SetFWDFlag(FLAG_GASDOSE_MAP, false);
 mp_view->SetFWDFlag(FLAG_BAROCORR_MAP, false);
 mp_view->SetFWDFlag(FLAG_MANIT_MAP, false);
 mp_view->SetFWDFlag(FLAG_TMP2CURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRKTEMP_MAP, false);
 mp_view->SetFWDFlag(FLAG_EHPAUSE_MAP, false);

 mp_view->EnableFWDFlag(FLAG_MANIT_MAP, false);    //todo
 mp_view->EnableFWDFlag(FLAG_BAROCORR_MAP, false); //todo
 mp_view->EnableFWDFlag(FLAG_CRKTEMP_MAP, false);  //todo
 mp_view->EnableFWDFlag(FLAG_EHPAUSE_MAP, false);  //todo
}

void S3FExportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void S3FExportController::OnOtherListNameChanged(int item, CString text)
{
 mp_s3f_io->GetDataLeft().maps[item].name = text;
}
