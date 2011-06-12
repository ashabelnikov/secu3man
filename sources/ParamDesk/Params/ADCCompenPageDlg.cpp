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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT CADCCompenPageDlg::IDD = IDD_PD_ADCCOMPEN_PAGE;

BEGIN_MESSAGE_MAP(CADCCompenPageDlg, Super)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, OnChangePdMAPFactorEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_MAP_KORRECTION_EDIT, OnChangePdMAPCorrectionEdit)

 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, OnChangePdUBATFactorEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_UBAT_KORRECTION_EDIT, OnChangePdUBATCorrectionEdit)

 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, OnChangePdTEMPFactorEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TEMP_KORRECTION_EDIT, OnChangePdTEMPCorrectionEdit)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_KORRECTION_EDIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_KORRECTION_EDIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_KORRECTION_EDIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_CAPTION, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_KORRECTION_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_KORRECTION_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_KORRECTION_UNIT, OnUpdateControls)
END_MESSAGE_MAP()

CADCCompenPageDlg::CADCCompenPageDlg(CWnd* pParent /*=NULL*/)
: Super(CADCCompenPageDlg::IDD, pParent)
, m_enabled(FALSE)
, m_map_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_map_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_ubat_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_ubat_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_temp_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_temp_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
{
 m_params.map_adc_factor = 1.0f;
 m_params.map_adc_correction = 0.0f;
 m_params.ubat_adc_factor = 1.0f;
 m_params.ubat_adc_correction = 0.0f;
 m_params.temp_adc_factor = 1.0f;
 m_params.temp_adc_correction = 0.0f;
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

 m_map_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, m_params.map_adc_factor);
 m_map_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_MAP_KORRECTION_EDIT, m_params.map_adc_correction);

 m_ubat_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, m_params.ubat_adc_factor);
 m_ubat_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_UBAT_KORRECTION_EDIT, m_params.ubat_adc_correction);

 m_temp_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, m_params.temp_adc_factor);
 m_temp_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TEMP_KORRECTION_EDIT, m_params.temp_adc_correction);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CADCCompenPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
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

 UpdateData(FALSE);
 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CADCCompenPageDlg::OnChangePdMAPFactorEdit()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CADCCompenPageDlg::OnChangePdMAPCorrectionEdit()
{
 UpdateData();
 OnChangeNotify();
}

void CADCCompenPageDlg::OnChangePdUBATFactorEdit()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CADCCompenPageDlg::OnChangePdUBATCorrectionEdit()
{
 UpdateData();
 OnChangeNotify();
}

void CADCCompenPageDlg::OnChangePdTEMPFactorEdit()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CADCCompenPageDlg::OnChangePdTEMPCorrectionEdit()
{
 UpdateData();
 OnChangeNotify();
}

//разрешение/запрещение контроллов (всех поголовно)
void CADCCompenPageDlg::Enable(bool enable)
{
 m_enabled = (enable) ? TRUE : FALSE;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CADCCompenPageDlg::IsEnabled(void)
{
 return (m_enabled) ? true : false;
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
