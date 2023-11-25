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
#include "io-core/MapIds.h"
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

 //Назначаем обработчики событий от представления
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
  return IDCANCEL; //отказ пользователя или ошибка
 
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
 if (mp_view->GetFWDFlag(FLAG_CESETT_DAT))
  m_fwdm->GetCESettingsData(mp_fwd->cesd);

 //import separate tables
 if (mp_view->GetFWDFlag(FLAG_ATTEN_MAP))
  m_fwdm->GetSepMap(ETMT_ATTENUATOR, mp_fwd->attenuator_table);

 if (mp_view->GetFWDFlag(FLAG_DWLCNTR_MAP))
  m_fwdm->GetSepMap(ETMT_DWELLCNTRL, mp_fwd->dwellcntrl_table);

 if (mp_view->GetFWDFlag(FLAG_CTS_MAP))
  m_fwdm->GetSepMap(ETMT_CTS_CURVE, mp_fwd->ctscurve_table);

 if (mp_view->GetFWDFlag(FLAG_ATS_MAP))
  m_fwdm->GetSepMap(ETMT_ATS_CURVE, mp_fwd->atscurve_table);

 if (mp_view->GetFWDFlag(FLAG_ATSAAC_MAP))
  m_fwdm->GetSepMap(ETMT_ATS_CORR, mp_fwd->ats_corr_table);

 if (mp_view->GetFWDFlag(FLAG_GASDOSE_MAP))
  m_fwdm->GetSepMap(ETMT_GASDOSE, mp_fwd->gasdose_pos_table);

 if (mp_view->GetFWDFlag(FLAG_BAROCORR_MAP))
  m_fwdm->GetSepMap(ETMT_BAROCORR, mp_fwd->barocorr_table);

 if (mp_view->GetFWDFlag(FLAG_MANIT_MAP))
  m_fwdm->GetSepMap(ETMT_MANIGNTIM, mp_fwd->pa4_igntim_corr);

 if (mp_view->GetFWDFlag(FLAG_TMP2CURVE_MAP))
  m_fwdm->GetSepMap(ETMT_TMP2_CURVE, mp_fwd->tmp2_curve);

 if (mp_view->GetFWDFlag(FLAG_CRKTEMP_MAP))
  m_fwdm->GetSepMap(ETMT_CRKCLT_CORR, mp_fwd->ctscrk_corr);

 if (mp_view->GetFWDFlag(FLAG_EHPAUSE_MAP))
  m_fwdm->GetSepMap(ETMT_EH_PAUSE, mp_fwd->eh_pause_table);

 if (mp_view->GetFWDFlag(FLAG_CRNKTHRD_MAP))
  m_fwdm->GetSepMap(ETMT_CRANKING_THRD, mp_fwd->cranking_thrd);

 if (mp_view->GetFWDFlag(FLAG_CRNKTIME_MAP))
  m_fwdm->GetSepMap(ETMT_CRANKING_TIME, mp_fwd->cranking_time);

 if (mp_view->GetFWDFlag(FLAG_ABANTHRD_MAP))
  m_fwdm->GetSepMap(ETMT_SMAPABAN_THRD, mp_fwd->smapaban_thrd);

 if (mp_view->GetFWDFlag(FLAG_KNOCKZONE_MAP))
  m_fwdm->GetSepMap(ETMT_KNOCK_ZONE, mp_fwd->knock_zone);

 if (mp_view->GetFWDFlag(FLAG_LAMBDAZONE_MAP))
  m_fwdm->GetSepMap(ETMT_LAMBDA_ZONE, mp_fwd->lambda_zone);

 if (mp_view->GetFWDFlag(FLAG_GRTSCURVE_MAP))
  m_fwdm->GetSepMap(ETMT_GRTS_CURVE, mp_fwd->grts_curve);

 if (mp_view->GetFWDFlag(FLAG_GRHEAT_MAP))
  m_fwdm->GetSepMap(ETMT_GRHEAT_DUTY, mp_fwd->grheat_duty);

 if (mp_view->GetFWDFlag(FLAG_IACUCOEF_MAP))
  m_fwdm->GetSepMap(ETMT_PWMIAC_UCOEF, mp_fwd->pwmiac_ucoef);

 if (mp_view->GetFWDFlag(FLAG_AFTSTRK0_MAP))
  m_fwdm->GetSepMap(ETMT_AFTSTR_STRK0, mp_fwd->aftstr_strk0);

 if (mp_view->GetFWDFlag(FLAG_AFTSTRK1_MAP))
  m_fwdm->GetSepMap(ETMT_AFTSTR_STRK1, mp_fwd->aftstr_strk1);

 if (mp_view->GetFWDFlag(FLAG_GRVDELAY_MAP))
  m_fwdm->GetSepMap(ETMT_GRVDELAY, mp_fwd->grv_delay);

 if (mp_view->GetFWDFlag(FLAG_FTLSCURVE_MAP))
  m_fwdm->GetSepMap(ETMT_FTLS_CURVE, mp_fwd->ftls_curve);

 if (mp_view->GetFWDFlag(FLAG_EGTSCURVE_MAP))
  m_fwdm->GetSepMap(ETMT_EGTS_CURVE, mp_fwd->egts_curve);

 if (mp_view->GetFWDFlag(FLAG_OPSCURVE_MAP))
  m_fwdm->GetSepMap(ETMT_OPS_CURVE, mp_fwd->ops_curve);

 if (mp_view->GetFWDFlag(FLAG_MANINJPWC_MAP))
  m_fwdm->GetSepMap(ETMT_MANINJPWC, mp_fwd->injpw_coef);

 if (mp_view->GetFWDFlag(FLAG_MAFCURVE_MAP))
  m_fwdm->GetSepMap(ETMT_MAF_CURVE, mp_fwd->maf_curve);

 if (mp_view->GetFWDFlag(FLAG_FTLSCOR_MAP))
  m_fwdm->GetSepMap(ETMT_FTLSCOR, mp_fwd->ftls_corr);

 if (mp_view->GetFWDFlag(FLAG_FTSCURVE_MAP))
  m_fwdm->GetSepMap(ETMT_FTS_CURVE, mp_fwd->fts_curve);

 if (mp_view->GetFWDFlag(FLAG_FUELDENSC_MAP))
  m_fwdm->GetSepMap(ETMT_FUELDENS_CORR, mp_fwd->fueldens_corr);

 if (mp_view->GetFWDFlag(FLAG_INJNONLINP_MAP))
  m_fwdm->GetSepMap(ETMT_INJNONLINP, mp_fwd->inj_nonlinp_corr);

 if (mp_view->GetFWDFlag(FLAG_INJNONLING_MAP))
  m_fwdm->GetSepMap(ETMT_INJNONLING, mp_fwd->inj_nonling_corr);

 if (mp_view->GetFWDFlag(FLAG_XTAU_XFACC_MAP))
  m_fwdm->GetSepMap(ETMT_XTAU_XFACC, mp_fwd->xtau_xfacc);

 if (mp_view->GetFWDFlag(FLAG_XTAU_XFDEC_MAP))
  m_fwdm->GetSepMap(ETMT_XTAU_XFDEC, mp_fwd->xtau_xfdec);

 if (mp_view->GetFWDFlag(FLAG_XTAU_TFACC_MAP))
  m_fwdm->GetSepMap(ETMT_XTAU_TFACC, mp_fwd->xtau_tfacc);

 if (mp_view->GetFWDFlag(FLAG_XTAU_TFDEC_MAP))
  m_fwdm->GetSepMap(ETMT_XTAU_TFDEC, mp_fwd->xtau_tfdec);

 //копируем таблицу сетки оборотов
 m_fwdm->GetRPMGridMap(mp_fwd->rpm_slots);

 //copy CLT grid table
 m_fwdm->GetCLTGridMap(mp_fwd->clt_slots);

 //копируем таблицу сетки нагрузки
 m_fwdm->GetLoadGridMap(mp_fwd->load_slots);
}

void SECU3ImportController::OnCancelPressed(void)
{
 //empty
}

//нажали кнопку перемещения данных
void SECU3ImportController::OnExchangePressed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();

 std::vector<_TSTRING> names = m_fwdm->GetFunctionsSetNames();

 if (names[other_sel] != _T(""))
  mp_fwd->maps[current_sel].name = names[other_sel];
 else
 { //если строка пустая, то генерируем "искусcтвенное" имя
  TCHAR name[32];
  _stprintf(name,MLL::GetString(IDS_MAP_NO_NAME).c_str(), other_sel+1);
  mp_fwd->maps[current_sel].name = name;
 }

 mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
 mp_view->SetFWDCurrentListSelection(current_sel);

 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_IGN_START, mp_fwd->maps[current_sel].f_str);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_IGN_IDLE, mp_fwd->maps[current_sel].f_idl);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_IGN_WORK, mp_fwd->maps[current_sel].f_wrk);

 if (mp_view->GetFWDFlag(FLAG_TEMP_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_IGN_TEMP, mp_fwd->maps[current_sel].f_tmp);

 if (mp_view->GetFWDFlag(FLAG_TEMPI_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_IGN_TEMPI, mp_fwd->maps[current_sel].f_tmp_idl);

 if (mp_view->GetFWDFlag(FLAG_VE_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_VE, mp_fwd->maps[current_sel].inj_ve);

 if (mp_view->GetFWDFlag(FLAG_VE2_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_VE2, mp_fwd->maps[current_sel].inj_ve2);

 if (mp_view->GetFWDFlag(FLAG_AFR_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_AFR, mp_fwd->maps[current_sel].inj_afr);

 if (mp_view->GetFWDFlag(FLAG_CRNK_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_CRNK, mp_fwd->maps[current_sel].inj_cranking);

 if (mp_view->GetFWDFlag(FLAG_WRMP_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_WRMP, mp_fwd->maps[current_sel].inj_warmup);

 if (mp_view->GetFWDFlag(FLAG_DEAD_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_DEAD, mp_fwd->maps[current_sel].inj_dead_time);

 if (mp_view->GetFWDFlag(FLAG_IDLR_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_IDLR, mp_fwd->maps[current_sel].inj_iac_run_pos);

 if (mp_view->GetFWDFlag(FLAG_IDLC_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_IDLC, mp_fwd->maps[current_sel].inj_iac_crank_pos);

 if (mp_view->GetFWDFlag(FLAG_THRASS_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_THRASS, mp_fwd->maps[current_sel].inj_thrass);

 if (mp_view->GetFWDFlag(FLAG_AETPS_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_AETPS, mp_fwd->maps[current_sel].inj_ae_tps);

 if (mp_view->GetFWDFlag(FLAG_AEMAP_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_AEMAP, mp_fwd->maps[current_sel].inj_ae_map);

 if (mp_view->GetFWDFlag(FLAG_AERPM_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_AERPM, mp_fwd->maps[current_sel].inj_ae_rpm);

 if (mp_view->GetFWDFlag(FLAG_AFTSTR_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_AFTSTR, mp_fwd->maps[current_sel].inj_aftstr);

 if (mp_view->GetFWDFlag(FLAG_IT_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_IT, mp_fwd->maps[current_sel].inj_timing);

 if (mp_view->GetFWDFlag(FLAG_ITRPM_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_ITRPM, mp_fwd->maps[current_sel].inj_target_rpm);

 if (mp_view->GetFWDFlag(FLAG_RIGID_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_RIGID, mp_fwd->maps[current_sel].inj_idl_rigidity);

 if (mp_view->GetFWDFlag(FLAG_EGOCRV_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_EGOCRV, mp_fwd->maps[current_sel].inj_ego_curve);

 if (mp_view->GetFWDFlag(FLAG_IACCORRW_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_IACCW, mp_fwd->maps[current_sel].inj_iac_corr_w);

 if (mp_view->GetFWDFlag(FLAG_IACCORR_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_IACC, mp_fwd->maps[current_sel].inj_iac_corr);

 if (mp_view->GetFWDFlag(FLAG_IATCLT_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_IATCLT, mp_fwd->maps[current_sel].inj_iatclt_corr);

 if (mp_view->GetFWDFlag(FLAG_TPSSWT_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_TPSSWT, mp_fwd->maps[current_sel].inj_tpsswt);

 if (mp_view->GetFWDFlag(FLAG_TPSZON_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_TPSZON, mp_fwd->maps[current_sel].inj_tpszon);

 if (mp_view->GetFWDFlag(FLAG_GTSC_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_GTSC, mp_fwd->maps[current_sel].inj_gts_corr);

 if (mp_view->GetFWDFlag(FLAG_GPSC_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_GPSC, mp_fwd->maps[current_sel].inj_gps_corr);

 if (mp_view->GetFWDFlag(FLAG_ATSC_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_ATSC, mp_fwd->maps[current_sel].inj_ats_corr);

 if (mp_view->GetFWDFlag(FLAG_PWM1_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_PWM1, mp_fwd->maps[current_sel].pwm_duty1);

 if (mp_view->GetFWDFlag(FLAG_PWM2_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_PWM2, mp_fwd->maps[current_sel].pwm_duty2);

 if (mp_view->GetFWDFlag(FLAG_IACMAT_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_IACMAT, mp_fwd->maps[current_sel].iac_mat_corr);

 if (mp_view->GetFWDFlag(FLAG_TPSZON_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_TPSZON, mp_fwd->maps[current_sel].inj_tpszon);

 if (mp_view->GetFWDFlag(FLAG_CYLMULT_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_CYLMULT, mp_fwd->maps[current_sel].inj_cylmult);

 if (mp_view->GetFWDFlag(FLAG_CYLADD_MAP))
  m_fwdm->GetSetMap(other_sel, ETMT_INJ_CYLADD, mp_fwd->maps[current_sel].inj_cyladd);
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
 mp_view->SetFWDFlag(FLAG_TEMPI_MAP, true);
 //fuel injection
 mp_view->SetFWDFlag(FLAG_VE_MAP, true);
 mp_view->SetFWDFlag(FLAG_VE2_MAP, true);
 mp_view->SetFWDFlag(FLAG_AFR_MAP, true);
 mp_view->SetFWDFlag(FLAG_CRNK_MAP, true);
 mp_view->SetFWDFlag(FLAG_WRMP_MAP, true);
 mp_view->SetFWDFlag(FLAG_DEAD_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLR_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLC_MAP, true);
 mp_view->SetFWDFlag(FLAG_THRASS_MAP, true);
 mp_view->SetFWDFlag(FLAG_AETPS_MAP, true);
 mp_view->SetFWDFlag(FLAG_AEMAP_MAP, true);
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
 mp_view->SetFWDFlag(FLAG_PWM1_MAP, true);
 mp_view->SetFWDFlag(FLAG_PWM2_MAP, true);
 mp_view->SetFWDFlag(FLAG_IACMAT_MAP, true);
 mp_view->SetFWDFlag(FLAG_TPSZON_MAP, true);
 mp_view->SetFWDFlag(FLAG_CYLMULT_MAP, true);
 mp_view->SetFWDFlag(FLAG_CYLADD_MAP, true);
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
 mp_view->SetFWDFlag(FLAG_KNOCKZONE_MAP, false);
 mp_view->SetFWDFlag(FLAG_LAMBDAZONE_MAP, false);
 mp_view->SetFWDFlag(FLAG_GRTSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_GRHEAT_MAP, false);
 mp_view->SetFWDFlag(FLAG_IACUCOEF_MAP, false);
 mp_view->SetFWDFlag(FLAG_AFTSTRK0_MAP, false);
 mp_view->SetFWDFlag(FLAG_AFTSTRK1_MAP, false);
 mp_view->SetFWDFlag(FLAG_GRVDELAY_MAP, false);
 mp_view->SetFWDFlag(FLAG_FTLSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_EGTSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_OPSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_MANINJPWC_MAP, false);
 mp_view->SetFWDFlag(FLAG_MAFCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_FTLSCOR_MAP, false);
 mp_view->SetFWDFlag(FLAG_FTSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_FUELDENSC_MAP, false);
 mp_view->SetFWDFlag(FLAG_INJNONLINP_MAP, false);
 mp_view->SetFWDFlag(FLAG_INJNONLING_MAP, false);
 mp_view->SetFWDFlag(FLAG_XTAU_XFACC_MAP, false);
 mp_view->SetFWDFlag(FLAG_XTAU_XFDEC_MAP, false);
 mp_view->SetFWDFlag(FLAG_XTAU_TFACC_MAP, false);
 mp_view->SetFWDFlag(FLAG_XTAU_TFDEC_MAP, false);
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

 //Назначаем обработчики событий от представления
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
  return IDCANCEL; //отказ пользователя или ошибка
 
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
  m_fwdm->SetSepMap(ETMT_ATTENUATOR, mp_fwd->attenuator_table);

 if (mp_view->GetFWDFlag(FLAG_DWLCNTR_MAP))
  m_fwdm->SetSepMap(ETMT_DWELLCNTRL, mp_fwd->dwellcntrl_table);

 if (mp_view->GetFWDFlag(FLAG_CTS_MAP))
  m_fwdm->SetSepMap(ETMT_CTS_CURVE, mp_fwd->ctscurve_table);
 
 if (mp_view->GetFWDFlag(FLAG_ATS_MAP))
  m_fwdm->SetSepMap(ETMT_ATS_CURVE, mp_fwd->atscurve_table);

 if (mp_view->GetFWDFlag(FLAG_ATSAAC_MAP))
  m_fwdm->SetSepMap(ETMT_ATS_CORR, mp_fwd->ats_corr_table);

 if (mp_view->GetFWDFlag(FLAG_GASDOSE_MAP))
  m_fwdm->SetSepMap(ETMT_GASDOSE, mp_fwd->gasdose_pos_table);

 if (mp_view->GetFWDFlag(FLAG_BAROCORR_MAP))
  m_fwdm->SetSepMap(ETMT_BAROCORR, mp_fwd->barocorr_table);

 if (mp_view->GetFWDFlag(FLAG_MANIT_MAP))
  m_fwdm->SetSepMap(ETMT_MANIGNTIM, mp_fwd->pa4_igntim_corr);

 if (mp_view->GetFWDFlag(FLAG_TMP2CURVE_MAP))
  m_fwdm->SetSepMap(ETMT_TMP2_CURVE, mp_fwd->tmp2_curve);

 if (mp_view->GetFWDFlag(FLAG_CRKTEMP_MAP))
  m_fwdm->SetSepMap(ETMT_CRKCLT_CORR, mp_fwd->ctscrk_corr);

 if (mp_view->GetFWDFlag(FLAG_EHPAUSE_MAP))
  m_fwdm->SetSepMap(ETMT_EH_PAUSE, mp_fwd->eh_pause_table);

 if (mp_view->GetFWDFlag(FLAG_CRNKTHRD_MAP))
  m_fwdm->SetSepMap(ETMT_CRANKING_THRD, mp_fwd->cranking_thrd);

 if (mp_view->GetFWDFlag(FLAG_CRNKTIME_MAP))
  m_fwdm->SetSepMap(ETMT_CRANKING_TIME, mp_fwd->cranking_time);

 if (mp_view->GetFWDFlag(FLAG_ABANTHRD_MAP))
  m_fwdm->SetSepMap(ETMT_SMAPABAN_THRD, mp_fwd->smapaban_thrd);

 if (mp_view->GetFWDFlag(FLAG_CESETT_DAT))
  m_fwdm->SetCESettingsData(mp_fwd->cesd);

 if (mp_view->GetFWDFlag(FLAG_KNOCKZONE_MAP))
  m_fwdm->SetSepMap(ETMT_KNOCK_ZONE, mp_fwd->knock_zone);

 if (mp_view->GetFWDFlag(FLAG_LAMBDAZONE_MAP))
  m_fwdm->SetSepMap(ETMT_LAMBDA_ZONE, mp_fwd->lambda_zone);

 if (mp_view->GetFWDFlag(FLAG_GRTSCURVE_MAP))
  m_fwdm->SetSepMap(ETMT_GRTS_CURVE, mp_fwd->grts_curve);

 if (mp_view->GetFWDFlag(FLAG_GRHEAT_MAP))
  m_fwdm->SetSepMap(ETMT_GRHEAT_DUTY, mp_fwd->grheat_duty);

 if (mp_view->GetFWDFlag(FLAG_IACUCOEF_MAP))
  m_fwdm->SetSepMap(ETMT_PWMIAC_UCOEF, mp_fwd->pwmiac_ucoef);

 if (mp_view->GetFWDFlag(FLAG_AFTSTRK0_MAP))
  m_fwdm->SetSepMap(ETMT_AFTSTR_STRK0, mp_fwd->aftstr_strk0);

 if (mp_view->GetFWDFlag(FLAG_AFTSTRK1_MAP))
  m_fwdm->SetSepMap(ETMT_AFTSTR_STRK1, mp_fwd->aftstr_strk1);

 if (mp_view->GetFWDFlag(FLAG_GRVDELAY_MAP))
  m_fwdm->SetSepMap(ETMT_GRVDELAY, mp_fwd->grv_delay);

 if (mp_view->GetFWDFlag(FLAG_FTLSCURVE_MAP))
  m_fwdm->SetSepMap(ETMT_FTLS_CURVE, mp_fwd->ftls_curve);

 if (mp_view->GetFWDFlag(FLAG_EGTSCURVE_MAP))
  m_fwdm->SetSepMap(ETMT_EGTS_CURVE, mp_fwd->egts_curve);

 if (mp_view->GetFWDFlag(FLAG_OPSCURVE_MAP))
  m_fwdm->SetSepMap(ETMT_OPS_CURVE, mp_fwd->ops_curve);

 if (mp_view->GetFWDFlag(FLAG_MANINJPWC_MAP))
  m_fwdm->SetSepMap(ETMT_MANINJPWC, mp_fwd->injpw_coef);

 if (mp_view->GetFWDFlag(FLAG_MAFCURVE_MAP))
  m_fwdm->SetSepMap(ETMT_MAF_CURVE, mp_fwd->maf_curve);

 if (mp_view->GetFWDFlag(FLAG_FTLSCOR_MAP))
  m_fwdm->SetSepMap(ETMT_FTLSCOR, mp_fwd->ftls_corr);

 if (mp_view->GetFWDFlag(FLAG_FTSCURVE_MAP))
  m_fwdm->SetSepMap(ETMT_FTS_CURVE, mp_fwd->fts_curve);

 if (mp_view->GetFWDFlag(FLAG_FUELDENSC_MAP))
  m_fwdm->SetSepMap(ETMT_FUELDENS_CORR, mp_fwd->fueldens_corr);

 if (mp_view->GetFWDFlag(FLAG_INJNONLINP_MAP))
  m_fwdm->SetSepMap(ETMT_INJNONLINP, mp_fwd->inj_nonlinp_corr);

 if (mp_view->GetFWDFlag(FLAG_INJNONLING_MAP))
  m_fwdm->SetSepMap(ETMT_INJNONLING, mp_fwd->inj_nonling_corr);

 if (mp_view->GetFWDFlag(FLAG_XTAU_XFACC_MAP))
  m_fwdm->SetSepMap(ETMT_XTAU_XFACC, mp_fwd->xtau_xfacc);

 if (mp_view->GetFWDFlag(FLAG_XTAU_XFDEC_MAP))
  m_fwdm->SetSepMap(ETMT_XTAU_XFDEC, mp_fwd->xtau_xfdec);

 if (mp_view->GetFWDFlag(FLAG_XTAU_TFACC_MAP))
  m_fwdm->SetSepMap(ETMT_XTAU_TFACC, mp_fwd->xtau_tfacc);

 if (mp_view->GetFWDFlag(FLAG_XTAU_TFDEC_MAP))
  m_fwdm->SetSepMap(ETMT_XTAU_TFDEC, mp_fwd->xtau_tfdec);

 //проверяем совместимость и копируем таблицу сетки оборотов
 if (m_fwdm->CheckRPMGridsCompatibility(mp_fwd->rpm_slots))
  m_fwdm->SetRPMGridMap(mp_fwd->rpm_slots);

 //проверяем совместимость и копируем таблицу сетки температуры
 if (m_fwdm->CheckCLTGridsCompatibility(mp_fwd->clt_slots))
  m_fwdm->SetCLTGridMap(mp_fwd->clt_slots);

 //проверяем совместимость и копируем таблицу сетки нагрузки
 if (m_fwdm->CheckLoadGridsCompatibility(mp_fwd->load_slots))
  m_fwdm->SetLoadGridMap(mp_fwd->load_slots);

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

//нажали кнопку перемещения данных
void SECU3ExportController::OnExchangePressed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();

 m_fwdm->SetFunctionsSetName(other_sel, mp_fwd->maps[current_sel].name);
 std::vector<_TSTRING> strings = m_fwdm->GetFunctionsSetNames();
 mp_view->FillFWDOtherList(strings);
 mp_view->SetFWDOtherListSelection(other_sel);

 if (mp_view->GetFWDFlag(FLAG_START_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_IGN_START, mp_fwd->maps[current_sel].f_str);

 if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_IGN_IDLE, mp_fwd->maps[current_sel].f_idl);

 if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_IGN_WORK, mp_fwd->maps[current_sel].f_wrk);

 if (mp_view->GetFWDFlag(FLAG_TEMP_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_IGN_TEMP, mp_fwd->maps[current_sel].f_tmp);

 if (mp_view->GetFWDFlag(FLAG_VE_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_VE, mp_fwd->maps[current_sel].inj_ve);

 if (mp_view->GetFWDFlag(FLAG_VE2_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_VE2, mp_fwd->maps[current_sel].inj_ve2);

 if (mp_view->GetFWDFlag(FLAG_AFR_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_AFR, mp_fwd->maps[current_sel].inj_afr);

 if (mp_view->GetFWDFlag(FLAG_CRNK_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_CRNK, mp_fwd->maps[current_sel].inj_cranking);

 if (mp_view->GetFWDFlag(FLAG_WRMP_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_WRMP, mp_fwd->maps[current_sel].inj_warmup);

 if (mp_view->GetFWDFlag(FLAG_DEAD_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_DEAD, mp_fwd->maps[current_sel].inj_dead_time);

 if (mp_view->GetFWDFlag(FLAG_IDLR_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_IDLR, mp_fwd->maps[current_sel].inj_iac_run_pos);

 if (mp_view->GetFWDFlag(FLAG_IDLC_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_IDLC, mp_fwd->maps[current_sel].inj_iac_crank_pos);

 if (mp_view->GetFWDFlag(FLAG_THRASS_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_THRASS, mp_fwd->maps[current_sel].inj_thrass);

 if (mp_view->GetFWDFlag(FLAG_AETPS_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_AETPS, mp_fwd->maps[current_sel].inj_ae_tps);

 if (mp_view->GetFWDFlag(FLAG_AEMAP_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_AEMAP, mp_fwd->maps[current_sel].inj_ae_map);

 if (mp_view->GetFWDFlag(FLAG_AERPM_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_AERPM, mp_fwd->maps[current_sel].inj_ae_rpm);

 if (mp_view->GetFWDFlag(FLAG_AFTSTR_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_AFTSTR, mp_fwd->maps[current_sel].inj_aftstr);

 if (mp_view->GetFWDFlag(FLAG_IT_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_IT, mp_fwd->maps[current_sel].inj_timing);

 if (mp_view->GetFWDFlag(FLAG_ITRPM_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_ITRPM, mp_fwd->maps[current_sel].inj_target_rpm);

 if (mp_view->GetFWDFlag(FLAG_RIGID_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_RIGID, mp_fwd->maps[current_sel].inj_idl_rigidity);

 if (mp_view->GetFWDFlag(FLAG_EGOCRV_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_EGOCRV, mp_fwd->maps[current_sel].inj_ego_curve);

 if (mp_view->GetFWDFlag(FLAG_IACCORRW_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_IACCW, mp_fwd->maps[current_sel].inj_iac_corr_w);

 if (mp_view->GetFWDFlag(FLAG_IACCORR_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_IACC, mp_fwd->maps[current_sel].inj_iac_corr);

 if (mp_view->GetFWDFlag(FLAG_IATCLT_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_IATCLT, mp_fwd->maps[current_sel].inj_iatclt_corr);

 if (mp_view->GetFWDFlag(FLAG_TPSSWT_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_TPSSWT, mp_fwd->maps[current_sel].inj_tpsswt);

 if (mp_view->GetFWDFlag(FLAG_TPSZON_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_TPSZON, mp_fwd->maps[current_sel].inj_tpszon);

 if (mp_view->GetFWDFlag(FLAG_GTSC_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_GTSC, mp_fwd->maps[current_sel].inj_gts_corr);

 if (mp_view->GetFWDFlag(FLAG_GPSC_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_GPSC, mp_fwd->maps[current_sel].inj_gps_corr);

 if (mp_view->GetFWDFlag(FLAG_ATSC_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_ATSC, mp_fwd->maps[current_sel].inj_ats_corr);

 if (mp_view->GetFWDFlag(FLAG_PWM1_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_PWM1, mp_fwd->maps[current_sel].pwm_duty1);

 if (mp_view->GetFWDFlag(FLAG_PWM2_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_PWM2, mp_fwd->maps[current_sel].pwm_duty2);

 if (mp_view->GetFWDFlag(FLAG_IACMAT_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_IACMAT, mp_fwd->maps[current_sel].iac_mat_corr);

 if (mp_view->GetFWDFlag(FLAG_CYLMULT_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_CYLMULT, mp_fwd->maps[current_sel].inj_cylmult);

 if (mp_view->GetFWDFlag(FLAG_CYLADD_MAP))
  m_fwdm->SetSetMap(other_sel, ETMT_INJ_CYLADD, mp_fwd->maps[current_sel].inj_cyladd);
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
 mp_view->SetFWDFlag(FLAG_TEMPI_MAP, true);
 //fuel injection
 mp_view->SetFWDFlag(FLAG_VE_MAP, true);
 mp_view->SetFWDFlag(FLAG_VE2_MAP, true);
 mp_view->SetFWDFlag(FLAG_AFR_MAP, true);
 mp_view->SetFWDFlag(FLAG_CRNK_MAP, true);
 mp_view->SetFWDFlag(FLAG_WRMP_MAP, true);
 mp_view->SetFWDFlag(FLAG_DEAD_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLR_MAP, true);
 mp_view->SetFWDFlag(FLAG_IDLC_MAP, true);
 mp_view->SetFWDFlag(FLAG_THRASS_MAP, true);
 mp_view->SetFWDFlag(FLAG_AETPS_MAP, true);
 mp_view->SetFWDFlag(FLAG_AEMAP_MAP, true);
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
 mp_view->SetFWDFlag(FLAG_PWM1_MAP, true);
 mp_view->SetFWDFlag(FLAG_PWM2_MAP, true);
 mp_view->SetFWDFlag(FLAG_IACMAT_MAP, true);
 mp_view->SetFWDFlag(FLAG_TPSZON_MAP, true);
 mp_view->SetFWDFlag(FLAG_CYLMULT_MAP, true);
 mp_view->SetFWDFlag(FLAG_CYLADD_MAP, true);
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
 mp_view->SetFWDFlag(FLAG_KNOCKZONE_MAP, false);
 mp_view->SetFWDFlag(FLAG_LAMBDAZONE_MAP, false);
 mp_view->SetFWDFlag(FLAG_GRTSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_GRHEAT_MAP, false);
 mp_view->SetFWDFlag(FLAG_IACUCOEF_MAP, false);
 mp_view->SetFWDFlag(FLAG_AFTSTRK0_MAP, false);
 mp_view->SetFWDFlag(FLAG_AFTSTRK1_MAP, false);
 mp_view->SetFWDFlag(FLAG_GRVDELAY_MAP, false);
 mp_view->SetFWDFlag(FLAG_FTLSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_EGTSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_OPSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_MANINJPWC_MAP, false);
 mp_view->SetFWDFlag(FLAG_MAFCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_FTLSCOR_MAP, false);
 mp_view->SetFWDFlag(FLAG_FTSCURVE_MAP, false);
 mp_view->SetFWDFlag(FLAG_FUELDENSC_MAP, false);
 mp_view->SetFWDFlag(FLAG_INJNONLINP_MAP, false);
 mp_view->SetFWDFlag(FLAG_INJNONLING_MAP, false);
 mp_view->SetFWDFlag(FLAG_XTAU_XFACC_MAP, false);
 mp_view->SetFWDFlag(FLAG_XTAU_XFDEC_MAP, false);
 mp_view->SetFWDFlag(FLAG_XTAU_TFACC_MAP, false);
 mp_view->SetFWDFlag(FLAG_XTAU_TFDEC_MAP, false);
}

void SECU3ExportController::OnCurrentListNameChanged(int item, CString text)
{
 mp_fwd->maps[item].name = text;
}

void SECU3ExportController::OnOtherListNameChanged(int item, CString text)
{
 m_fwdm->SetFunctionsSetName(item, (LPCTSTR)text);
}
