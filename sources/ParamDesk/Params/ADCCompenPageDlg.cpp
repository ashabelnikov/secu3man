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
#include "Resources/resource.h"
#include "ADCCompenPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CADCCompenPageDlg::IDD = IDD_PD_ADCCOMPEN_PAGE;

BEGIN_MESSAGE_MAP(CADCCompenPageDlg, Super)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_MAP_KORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_UBAT_KORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TEMP_KORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TPS_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TPS_KORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I1_KORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I2_KORRECTION_EDIT, OnChangeEdit)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_CAPTION, OnUpdateControlsSECU3T)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_CAPTION, OnUpdateControlsSECU3T)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_KORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_KORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_KORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_KORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_EDIT, OnUpdateControlsSECU3T)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_KORRECTION_EDIT, OnUpdateControlsSECU3T)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_EDIT, OnUpdateControlsSECU3T)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_KORRECTION_EDIT, OnUpdateControlsSECU3T)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_KORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_KORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_KORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_KORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_UNIT, OnUpdateControlsSECU3T)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_KORRECTION_UNIT, OnUpdateControlsSECU3T)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_UNIT, OnUpdateControlsSECU3T)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_KORRECTION_UNIT, OnUpdateControlsSECU3T)
END_MESSAGE_MAP()

CADCCompenPageDlg::CADCCompenPageDlg(CWnd* pParent /*=NULL*/)
: Super(CADCCompenPageDlg::IDD, pParent)
, m_enabled(false)
, m_enable_secu3t_features(false)
, m_map_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_map_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_ubat_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_ubat_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_temp_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_temp_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_tps_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_tps_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_ai1_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_ai1_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_ai2_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_ai2_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
{
 m_params.map_adc_factor = 1.0f;
 m_params.map_adc_correction = 0.0f;
 m_params.ubat_adc_factor = 1.0f;
 m_params.ubat_adc_correction = 0.0f;
 m_params.temp_adc_factor = 1.0f;
 m_params.temp_adc_correction = 0.0f;
 m_params.tps_adc_factor = 1.0f;
 m_params.tps_adc_correction = 0.0f;
 m_params.ai1_adc_factor = 1.0f;
 m_params.ai1_adc_correction = 0.0f;
 m_params.ai2_adc_factor = 1.0f;
 m_params.ai2_adc_correction = 0.0f;
}

LPCTSTR CADCCompenPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CADCCompenPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_MAP_FACTOR_SPIN, m_map_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_MAP_KORRECTION_SPIN, m_map_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, m_map_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_MAP_KORRECTION_EDIT, m_map_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_UBAT_FACTOR_SPIN, m_ubat_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_UBAT_KORRECTION_SPIN, m_ubat_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, m_ubat_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_UBAT_KORRECTION_EDIT, m_ubat_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TEMP_FACTOR_SPIN, m_temp_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TEMP_KORRECTION_SPIN, m_temp_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, m_temp_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TEMP_KORRECTION_EDIT, m_temp_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TPS_FACTOR_SPIN, m_tps_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TPS_KORRECTION_SPIN, m_tps_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TPS_FACTOR_EDIT, m_tps_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TPS_KORRECTION_EDIT, m_tps_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_SPIN, m_ai1_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I1_KORRECTION_SPIN, m_ai1_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_EDIT, m_ai1_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I1_KORRECTION_EDIT, m_ai1_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_SPIN, m_ai2_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I2_KORRECTION_SPIN, m_ai2_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_EDIT, m_ai2_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I2_KORRECTION_EDIT, m_ai2_correction_edit);

 m_map_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, m_params.map_adc_factor);
 m_map_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_MAP_KORRECTION_EDIT, m_params.map_adc_correction);

 m_ubat_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, m_params.ubat_adc_factor);
 m_ubat_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_UBAT_KORRECTION_EDIT, m_params.ubat_adc_correction);

 m_temp_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, m_params.temp_adc_factor);
 m_temp_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TEMP_KORRECTION_EDIT, m_params.temp_adc_correction);

 m_tps_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TPS_FACTOR_EDIT, m_params.tps_adc_factor);
 m_tps_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TPS_KORRECTION_EDIT, m_params.tps_adc_correction);

 m_ai1_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_EDIT, m_params.ai1_adc_factor);
 m_ai1_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I1_KORRECTION_EDIT, m_params.ai1_adc_correction);

 m_ai2_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_EDIT, m_params.ai2_adc_factor);
 m_ai2_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I2_KORRECTION_EDIT, m_params.ai2_adc_correction);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CADCCompenPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CADCCompenPageDlg::OnUpdateControlsSECU3T(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_enable_secu3t_features);
}

/////////////////////////////////////////////////////////////////////////////
// CADCCompenPageDlg message handlers

BOOL CADCCompenPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_map_factor_spin.SetBuddy(&m_map_factor_edit);
 m_map_factor_edit.SetLimitText(6);
 m_map_factor_edit.SetDecimalPlaces(3);
 m_map_factor_spin.SetRangeAndDelta(-2.0f,2.0f,0.001f);

 m_map_correction_spin.SetBuddy(&m_map_correction_edit);
 m_map_correction_edit.SetLimitText(6);
 m_map_correction_edit.SetDecimalPlaces(4);
 m_map_correction_spin.SetRangeAndDelta(-2.0f,2.0f,0.0025f);

 m_ubat_factor_spin.SetBuddy(&m_ubat_factor_edit);
 m_ubat_factor_edit.SetLimitText(6);
 m_ubat_factor_edit.SetDecimalPlaces(3);
 m_ubat_factor_spin.SetRangeAndDelta(-2.0f,2.0f,0.001f);

 m_ubat_correction_spin.SetBuddy(&m_ubat_correction_edit);
 m_ubat_correction_edit.SetLimitText(6);
 m_ubat_correction_edit.SetDecimalPlaces(4);
 m_ubat_correction_spin.SetRangeAndDelta(-2.0f,2.0f,0.0025f);

 m_temp_factor_spin.SetBuddy(&m_temp_factor_edit);
 m_temp_factor_edit.SetLimitText(6);
 m_temp_factor_edit.SetDecimalPlaces(3);
 m_temp_factor_spin.SetRangeAndDelta(-2.0f,2.0f,0.001f);

 m_temp_correction_spin.SetBuddy(&m_temp_correction_edit);
 m_temp_correction_edit.SetLimitText(6);
 m_temp_correction_edit.SetDecimalPlaces(4);
 m_temp_correction_spin.SetRangeAndDelta(-2.0f,2.0f,0.0025f);

 m_tps_factor_spin.SetBuddy(&m_tps_factor_edit);
 m_tps_factor_edit.SetLimitText(6);
 m_tps_factor_edit.SetDecimalPlaces(3);
 m_tps_factor_spin.SetRangeAndDelta(-2.0f,2.0f,0.001f);

 m_tps_correction_spin.SetBuddy(&m_tps_correction_edit);
 m_tps_correction_edit.SetLimitText(6);
 m_tps_correction_edit.SetDecimalPlaces(4);
 m_tps_correction_spin.SetRangeAndDelta(-2.0f,2.0f,0.0025f);

 m_ai1_factor_spin.SetBuddy(&m_ai1_factor_edit);
 m_ai1_factor_edit.SetLimitText(6);
 m_ai1_factor_edit.SetDecimalPlaces(3);
 m_ai1_factor_spin.SetRangeAndDelta(-2.0f,2.0f,0.001f);

 m_ai1_correction_spin.SetBuddy(&m_ai1_correction_edit);
 m_ai1_correction_edit.SetLimitText(6);
 m_ai1_correction_edit.SetDecimalPlaces(4);
 m_ai1_correction_spin.SetRangeAndDelta(-2.0f,2.0f,0.0025f);

 m_ai2_factor_spin.SetBuddy(&m_ai2_factor_edit);
 m_ai2_factor_edit.SetLimitText(6);
 m_ai2_factor_edit.SetDecimalPlaces(3);
 m_ai2_factor_spin.SetRangeAndDelta(-2.0f,2.0f,0.001f);

 m_ai2_correction_spin.SetBuddy(&m_ai2_correction_edit);
 m_ai2_correction_edit.SetLimitText(6);
 m_ai2_correction_edit.SetDecimalPlaces(4);
 m_ai2_correction_spin.SetRangeAndDelta(-2.0f,2.0f,0.0025f);

 UpdateData(FALSE);
 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_map_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));

  VERIFY(mp_ttc->AddWindow(&m_ubat_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ubat_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ubat_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ubat_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));

  VERIFY(mp_ttc->AddWindow(&m_temp_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_temp_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_temp_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_temp_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));

  VERIFY(mp_ttc->AddWindow(&m_tps_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));

  VERIFY(mp_ttc->AddWindow(&m_ai1_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai1_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai1_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai1_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));

  VERIFY(mp_ttc->AddWindow(&m_ai2_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai2_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai2_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai2_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_KORRECTION_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CADCCompenPageDlg::OnChangeEdit()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//разрешение/запрещение контроллов (всех поголовно)
void CADCCompenPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
 {
  UpdateDialogControls(this, TRUE);
  RedrawWindow(); //strange, without this function call spin buttons don't update correctly...
 }
}

//что с контроллами?
bool CADCCompenPageDlg::IsEnabled(void)
{
 return m_enabled;
}

void CADCCompenPageDlg::EnableSECU3TItems(bool i_enable)
{
 if (m_enable_secu3t_features == i_enable)
  return; //already has needed state
 m_enable_secu3t_features = i_enable;
 if (::IsWindow(m_hWnd))
 {
  UpdateDialogControls(this, TRUE);
  RedrawWindow(); //strange, without this function call spin buttons don't update correctly...
 }
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CADCCompenPageDlg::GetValues(SECU3IO::ADCCompenPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::ADCCompenPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CADCCompenPageDlg::SetValues(const SECU3IO::ADCCompenPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::ADCCompenPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
