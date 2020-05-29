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

/** \file SECUImpExpController.cpp
 * \author Alexey A. Shabelnikov
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
#include "ui-core/MsgBox.h"

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
  if (!PlatformParamHolder::GetPlatformIdByFirmwareMagic(&buffer[0], selected_size, platform_id))
  {
   if (!PlatformParamHolder::GetPlatformIdByFirmwareSize(selected_size, platform_id))
    return IDCANCEL; //error
  }
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

 if (mp_view->GetFWDFlag(FLAG_DWLCNTR_MAP))
  m_fwdm->GetDwellCntrlMap(mp_fwd->dwellcntrl_table);

 if (mp_view->GetFWDFlag(FLAG_CTS_MAP))
 {
  m_fwdm->GetCTSCurveMap(mp_fwd->ctscurve_table);
  mp_fwd->ctscurve_vlimits[0] = m_fwdm->GetCTSMapVoltageLimit(0);
  mp_fwd->ctscurve_vlimits[1] = m_fwdm->GetCTSMapVoltageLimit(1);
 }

 if (mp_view->GetFWDFlag(FLAG_ATS_MAP))
 {
  m_fwdm->GetATSCurveMap(mp_fwd->atscurve_table);
  mp_fwd->atscurve_vlimits[0] = m_fwdm->GetATSMapVoltageLimit(0);
  mp_fwd->atscurve_vlimits[1] = m_fwdm->GetATSMapVoltageLimit(1);
 }

 if (mp_view->GetFWDFlag(FLAG_ATSAAC_MAP))
  m_fwdm->GetATSAACMap(mp_fwd->ats_corr_table);

 if (mp_view->GetFWDFlag(FLAG_GASDOSE_MAP))
  m_fwdm->GetGasdosePosMap(mp_fwd->gasdose_pos_table);

 if (mp_view->GetFWDFlag(FLAG_BAROCORR_MAP))
  m_fwdm->GetBarocorrMap(mp_fwd->barocorr_table);

 if (mp_view->GetFWDFlag(FLAG_MANIT_MAP))
  m_fwdm->GetManIgntimMap(mp_fwd->pa4_igntim_corr);

 if (mp_view->GetFWDFlag(FLAG_TMP2CURVE_MAP))
  m_fwdm->GetTmp2CurveMap(mp_fwd->tmp2_curve);

 if (mp_view->GetFWDFlag(FLAG_CRKTEMP_MAP))
  m_fwdm->GetCrkTempMap(mp_fwd->ctscrk_corr);

 if (mp_view->GetFWDFlag(FLAG_EHPAUSE_MAP))
  m_fwdm->GetEHPauseMap(mp_fwd->eh_pause_table);

 if (mp_view->GetFWDFlag(FLAG_CRNKTHRD_MAP))
  m_fwdm->GetCrankingThrdMap(mp_fwd->cranking_thrd);

 if (mp_view->GetFWDFlag(FLAG_CRNKTIME_MAP))
  m_fwdm->GetCrankingTimeMap(mp_fwd->cranking_time);

 if (mp_view->GetFWDFlag(FLAG_ABANTHRD_MAP))
  m_fwdm->GetSmapabanThrdMap(mp_fwd->smapaban_thrd);

 if (mp_view->GetFWDFlag(FLAG_CESETT_DAT))
  m_fwdm->GetCESettingsData(mp_fwd->cesd);

 //копируем таблицу сетки оборотов
 m_fwdm->GetRPMGridMap(mp_fwd->rpm_slots);

 //copy CLT grid table
 m_fwdm->GetCLTGridMap(mp_fwd->clt_slots);
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

 if (mp_view->GetFWDFlag(FLAG_VE_MAP))
  m_fwdm->GetVEMap(other_sel, mp_fwd->maps[current_sel].inj_ve);

 if (mp_view->GetFWDFlag(FLAG_AFR_MAP))
  m_fwdm->GetAFRMap(other_sel, mp_fwd->maps[current_sel].inj_afr);

 if (mp_view->GetFWDFlag(FLAG_CRNK_MAP))
  m_fwdm->GetCrnkMap(other_sel, mp_fwd->maps[current_sel].inj_cranking);

 if (mp_view->GetFWDFlag(FLAG_WRMP_MAP))
  m_fwdm->GetWrmpMap(other_sel, mp_fwd->maps[current_sel].inj_warmup);

 if (mp_view->GetFWDFlag(FLAG_DEAD_MAP))
  m_fwdm->GetDeadMap(other_sel, mp_fwd->maps[current_sel].inj_dead_time);

 if (mp_view->GetFWDFlag(FLAG_IDLR_MAP))
  m_fwdm->GetIdlrMap(other_sel, mp_fwd->maps[current_sel].inj_iac_run_pos);

 if (mp_view->GetFWDFlag(FLAG_IDLC_MAP))
  m_fwdm->GetIdlcMap(other_sel, mp_fwd->maps[current_sel].inj_iac_crank_pos);

 if (mp_view->GetFWDFlag(FLAG_AETPS_MAP))
  m_fwdm->GetAETPSMap(other_sel, mp_fwd->maps[current_sel].inj_ae_tps);

 if (mp_view->GetFWDFlag(FLAG_AERPM_MAP))
  m_fwdm->GetAERPMMap(other_sel, mp_fwd->maps[current_sel].inj_ae_rpm);

 if (mp_view->GetFWDFlag(FLAG_AFTSTR_MAP))
  m_fwdm->GetAftstrMap(other_sel, mp_fwd->maps[current_sel].inj_aftstr);

 if (mp_view->GetFWDFlag(FLAG_IT_MAP))
  m_fwdm->GetITMap(other_sel, mp_fwd->maps[current_sel].inj_timing);

 if (mp_view->GetFWDFlag(FLAG_ITRPM_MAP))
  m_fwdm->GetITRPMMap(other_sel, mp_fwd->maps[current_sel].inj_target_rpm);

 if (mp_view->GetFWDFlag(FLAG_RIGID_MAP))
  m_fwdm->GetRigidMap(other_sel, mp_fwd->maps[current_sel].inj_idl_rigidity);

 if (mp_view->GetFWDFlag(FLAG_EGOCRV_MAP))
  m_fwdm->GetEGOCurveMap(other_sel, mp_fwd->maps[current_sel].inj_ego_curve);

 if (mp_view->GetFWDFlag(FLAG_IACCORRW_MAP))
  m_fwdm->GetIACCorrWMap(other_sel, mp_fwd->maps[current_sel].inj_iac_corr_w);

 if (mp_view->GetFWDFlag(FLAG_IACCORR_MAP))
  m_fwdm->GetIACCorrMap(other_sel, mp_fwd->maps[current_sel].inj_iac_corr);

 if (mp_view->GetFWDFlag(FLAG_IATCLT_MAP))
  m_fwdm->GetIATCLTMap(other_sel, mp_fwd->maps[current_sel].inj_iatclt_corr);

 if (mp_view->GetFWDFlag(FLAG_TPSSWT_MAP))
  m_fwdm->GetTpsswtMap(other_sel, mp_fwd->maps[current_sel].inj_tpsswt);

 if (mp_view->GetFWDFlag(FLAG_GTSC_MAP))
  m_fwdm->GetGtscMap(other_sel, mp_fwd->maps[current_sel].inj_gts_corr);

 if (mp_view->GetFWDFlag(FLAG_GPSC_MAP))
  m_fwdm->GetGpscMap(other_sel, mp_fwd->maps[current_sel].inj_gps_corr);

 if (mp_view->GetFWDFlag(FLAG_ATSC_MAP))
  m_fwdm->GetAtscMap(other_sel, mp_fwd->maps[current_sel].inj_ats_corr);
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

 //ignition
 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP, true);
 //fuel injection
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
 mp_view->SetFWDFlag(FLAG_TPSSWT_MAP, true);
 mp_view->SetFWDFlag(FLAG_GTSC_MAP, true);
 mp_view->SetFWDFlag(FLAG_GPSC_MAP, true);
 mp_view->SetFWDFlag(FLAG_ATSC_MAP, true);
 //separate maps
 mp_view->SetFWDFlag(FLAG_DWLCNTR_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP, false);
 mp_view->SetFWDFlag(FLAG_CTS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATSAAC_MAP, false);
 mp_view->SetFWDFlag(FLAG_GASDOSE_MAP, false); //GD
 mp_view->SetFWDFlag(FLAG_BAROCORR_MAP, false);
 mp_view->SetFWDFlag(FLAG_MANIT_MAP, false);
 mp_view->SetFWDFlag(FLAG_TMP2CURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRKTEMP_MAP, false);
 mp_view->SetFWDFlag(FLAG_EHPAUSE_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRNKTHRD_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRNKTIME_MAP, false);
 mp_view->SetFWDFlag(FLAG_ABANTHRD_MAP, false);
 mp_view->SetFWDFlag(FLAG_CESETT_DAT, false);
}

void SECU3ImportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void SECU3ImportController::OnOtherListNameChanged(int item, CString text)
{
 m_fwdm->SetFunctionsSetName(item, (LPCTSTR)text);
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
  if (!PlatformParamHolder::GetPlatformIdByFirmwareMagic(&buffer[0], selected_size, platform_id))
  {
   if (!PlatformParamHolder::GetPlatformIdByFirmwareSize(selected_size, platform_id))
    return IDCANCEL; //error
  }
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
 //export separate tables
 if (mp_view->GetFWDFlag(FLAG_ATTEN_MAP))
  m_fwdm->SetAttenuatorMap(mp_fwd->attenuator_table);

 if (mp_view->GetFWDFlag(FLAG_DWLCNTR_MAP))
  m_fwdm->SetDwellCntrlMap(mp_fwd->dwellcntrl_table);

 if (mp_view->GetFWDFlag(FLAG_CTS_MAP))
 {
  m_fwdm->SetCTSCurveMap(mp_fwd->ctscurve_table);
  m_fwdm->SetCTSMapVoltageLimit(0, mp_fwd->ctscurve_vlimits[0]);
  m_fwdm->SetCTSMapVoltageLimit(1, mp_fwd->ctscurve_vlimits[1]);
 }
 
 if (mp_view->GetFWDFlag(FLAG_ATS_MAP))
 {
  m_fwdm->SetATSCurveMap(mp_fwd->atscurve_table);
  m_fwdm->SetATSMapVoltageLimit(0, mp_fwd->atscurve_vlimits[0]);
  m_fwdm->SetATSMapVoltageLimit(1, mp_fwd->atscurve_vlimits[1]);
 }

 if (mp_view->GetFWDFlag(FLAG_ATSAAC_MAP))
  m_fwdm->SetATSAACMap(mp_fwd->ats_corr_table);

 if (mp_view->GetFWDFlag(FLAG_GASDOSE_MAP))
  m_fwdm->SetGasdosePosMap(mp_fwd->gasdose_pos_table);

 if (mp_view->GetFWDFlag(FLAG_BAROCORR_MAP))
  m_fwdm->SetBarocorrMap(mp_fwd->barocorr_table);

 if (mp_view->GetFWDFlag(FLAG_MANIT_MAP))
  m_fwdm->SetManIgntimMap(mp_fwd->pa4_igntim_corr);

 if (mp_view->GetFWDFlag(FLAG_TMP2CURVE_MAP))
  m_fwdm->SetTmp2CurveMap(mp_fwd->tmp2_curve);

 if (mp_view->GetFWDFlag(FLAG_CRKTEMP_MAP))
  m_fwdm->SetCrkTempMap(mp_fwd->ctscrk_corr);

 if (mp_view->GetFWDFlag(FLAG_EHPAUSE_MAP))
  m_fwdm->SetEHPauseMap(mp_fwd->eh_pause_table);

 if (mp_view->GetFWDFlag(FLAG_CRNKTHRD_MAP))
  m_fwdm->SetCrankingThrdMap(mp_fwd->cranking_thrd);

 if (mp_view->GetFWDFlag(FLAG_CRNKTIME_MAP))
  m_fwdm->SetCrankingTimeMap(mp_fwd->cranking_time);

 if (mp_view->GetFWDFlag(FLAG_ABANTHRD_MAP))
  m_fwdm->SetSmapabanThrdMap(mp_fwd->smapaban_thrd);

 if (mp_view->GetFWDFlag(FLAG_CESETT_DAT))
  m_fwdm->SetCESettingsData(mp_fwd->cesd);

 //провер€ем совместимость и копируем таблицу сетки оборотов
 if (m_fwdm->CheckRPMGridsCompatibility(mp_fwd->rpm_slots))
  m_fwdm->SetRPMGridMap(mp_fwd->rpm_slots);

 //провер€ем совместимость и копируем таблицу сетки температуры
 if (m_fwdm->CheckCLTGridsCompatibility(mp_fwd->clt_slots))
  m_fwdm->SetCLTGridMap(mp_fwd->clt_slots);

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
  SECUMessageBox(szError);
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

 if (mp_view->GetFWDFlag(FLAG_VE_MAP))
  m_fwdm->SetVEMap(other_sel, mp_fwd->maps[current_sel].inj_ve);

 if (mp_view->GetFWDFlag(FLAG_AFR_MAP))
  m_fwdm->SetAFRMap(other_sel, mp_fwd->maps[current_sel].inj_afr);

 if (mp_view->GetFWDFlag(FLAG_CRNK_MAP))
  m_fwdm->SetCrnkMap(other_sel, mp_fwd->maps[current_sel].inj_cranking);

 if (mp_view->GetFWDFlag(FLAG_WRMP_MAP))
  m_fwdm->SetWrmpMap(other_sel, mp_fwd->maps[current_sel].inj_warmup);

 if (mp_view->GetFWDFlag(FLAG_DEAD_MAP))
  m_fwdm->SetDeadMap(other_sel, mp_fwd->maps[current_sel].inj_dead_time);

 if (mp_view->GetFWDFlag(FLAG_IDLR_MAP))
  m_fwdm->SetIdlrMap(other_sel, mp_fwd->maps[current_sel].inj_iac_run_pos);

 if (mp_view->GetFWDFlag(FLAG_IDLC_MAP))
  m_fwdm->SetIdlcMap(other_sel, mp_fwd->maps[current_sel].inj_iac_crank_pos);

 if (mp_view->GetFWDFlag(FLAG_AETPS_MAP))
  m_fwdm->SetAETPSMap(other_sel, mp_fwd->maps[current_sel].inj_ae_tps);

 if (mp_view->GetFWDFlag(FLAG_AERPM_MAP))
  m_fwdm->SetAERPMMap(other_sel, mp_fwd->maps[current_sel].inj_ae_rpm);

 if (mp_view->GetFWDFlag(FLAG_AFTSTR_MAP))
  m_fwdm->SetAftstrMap(other_sel, mp_fwd->maps[current_sel].inj_aftstr);

 if (mp_view->GetFWDFlag(FLAG_IT_MAP))
  m_fwdm->SetITMap(other_sel, mp_fwd->maps[current_sel].inj_timing);

 if (mp_view->GetFWDFlag(FLAG_ITRPM_MAP))
  m_fwdm->SetITRPMMap(other_sel, mp_fwd->maps[current_sel].inj_target_rpm);

 if (mp_view->GetFWDFlag(FLAG_RIGID_MAP))
  m_fwdm->SetRigidMap(other_sel, mp_fwd->maps[current_sel].inj_idl_rigidity);

 if (mp_view->GetFWDFlag(FLAG_EGOCRV_MAP))
  m_fwdm->SetEGOCurveMap(other_sel, mp_fwd->maps[current_sel].inj_ego_curve);

 if (mp_view->GetFWDFlag(FLAG_IACCORRW_MAP))
  m_fwdm->SetIACCorrWMap(other_sel, mp_fwd->maps[current_sel].inj_iac_corr_w);

 if (mp_view->GetFWDFlag(FLAG_IACCORR_MAP))
  m_fwdm->SetIACCorrMap(other_sel, mp_fwd->maps[current_sel].inj_iac_corr);

 if (mp_view->GetFWDFlag(FLAG_IATCLT_MAP))
  m_fwdm->SetIATCLTMap(other_sel, mp_fwd->maps[current_sel].inj_iatclt_corr);

 if (mp_view->GetFWDFlag(FLAG_TPSSWT_MAP))
  m_fwdm->SetTpsswtMap(other_sel, mp_fwd->maps[current_sel].inj_tpsswt);

 if (mp_view->GetFWDFlag(FLAG_GTSC_MAP))
  m_fwdm->SetGtscMap(other_sel, mp_fwd->maps[current_sel].inj_gts_corr);

 if (mp_view->GetFWDFlag(FLAG_GPSC_MAP))
  m_fwdm->SetGpscMap(other_sel, mp_fwd->maps[current_sel].inj_gps_corr);

 if (mp_view->GetFWDFlag(FLAG_ATSC_MAP))
  m_fwdm->SetAtscMap(other_sel, mp_fwd->maps[current_sel].inj_ats_corr);
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

 //ignition
 mp_view->SetFWDFlag(FLAG_START_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLE_MAP, true);
 mp_view->SetFWDFlag(FLAG_WORK_MAP, true);
 mp_view->SetFWDFlag(FLAG_TEMP_MAP, true);
 //fuel injection
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
 mp_view->SetFWDFlag(FLAG_TPSSWT_MAP, true);
 mp_view->SetFWDFlag(FLAG_GTSC_MAP, true);
 mp_view->SetFWDFlag(FLAG_GPSC_MAP, true);
 mp_view->SetFWDFlag(FLAG_ATSC_MAP, true);
 //separate
 mp_view->SetFWDFlag(FLAG_DWLCNTR_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATTEN_MAP, false);
 mp_view->SetFWDFlag(FLAG_CTS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATS_MAP, false);
 mp_view->SetFWDFlag(FLAG_ATSAAC_MAP, false);
 mp_view->SetFWDFlag(FLAG_GASDOSE_MAP, false); //GD
 mp_view->SetFWDFlag(FLAG_BAROCORR_MAP, false);
 mp_view->SetFWDFlag(FLAG_MANIT_MAP, false);
 mp_view->SetFWDFlag(FLAG_TMP2CURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRKTEMP_MAP, false);
 mp_view->SetFWDFlag(FLAG_EHPAUSE_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRNKTHRD_MAP, false);
 mp_view->SetFWDFlag(FLAG_CRNKTIME_MAP, false);
 mp_view->SetFWDFlag(FLAG_ABANTHRD_MAP, false);
 mp_view->SetFWDFlag(FLAG_CESETT_DAT, false);
}

void SECU3ExportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void SECU3ExportController::OnOtherListNameChanged(int item, CString text)
{
 m_fwdm->SetFunctionsSetName(item, (LPCTSTR)text);
}
