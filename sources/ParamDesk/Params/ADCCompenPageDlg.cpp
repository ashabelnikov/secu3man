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

/** \file ADCCompenPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "ADCCompenPageDlg.h"
#include "common/dpiaware.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/WndScroller.h"

const UINT CADCCompenPageDlg::IDD = IDD_PD_ADCCOMPEN_PAGE;

BEGIN_MESSAGE_MAP(CADCCompenPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_MAP_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_UBAT_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TEMP_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TPS_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_TPS_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I1_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I2_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I3_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I3_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I4_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I4_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I5_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I5_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I6_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I6_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I7_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I7_CORRECTION_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I8_FACTOR_EDIT, OnChangeEdit)
 ON_EN_CHANGE(IDC_PD_ADCCOMPEN_ADD_I8_CORRECTION_EDIT, OnChangeEdit)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I3_CAPTION, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I4_CAPTION, OnUpdateControlsSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I5_CAPTION, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I6_CAPTION, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I7_CAPTION, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I8_CAPTION, OnUpdateControlsSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_CORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_CORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_CORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_CORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_CORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_CORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I3_FACTOR_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I3_CORRECTION_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I4_FACTOR_EDIT, OnUpdateControlsSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I4_CORRECTION_EDIT, OnUpdateControlsSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I5_FACTOR_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I5_CORRECTION_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I6_FACTOR_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I6_CORRECTION_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I7_FACTOR_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I7_CORRECTION_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I8_FACTOR_EDIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I8_CORRECTION_EDIT, OnUpdateControlsSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_MAP_CORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_UBAT_CORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TEMP_CORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_TPS_CORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I1_CORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I2_CORRECTION_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I3_FACTOR_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I3_CORRECTION_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I4_FACTOR_UNIT, OnUpdateControlsSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I4_CORRECTION_UNIT, OnUpdateControlsSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I5_FACTOR_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I5_CORRECTION_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I6_FACTOR_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I6_CORRECTION_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I7_FACTOR_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I7_CORRECTION_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I8_FACTOR_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I8_CORRECTION_UNIT, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I5_FACTOR_SPIN, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I5_CORRECTION_SPIN, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I6_FACTOR_SPIN, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I6_CORRECTION_SPIN, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I7_FACTOR_SPIN, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I7_CORRECTION_SPIN, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I8_FACTOR_SPIN, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_ADCCOMPEN_ADD_I8_CORRECTION_SPIN, OnUpdateControlsSECU3i)
END_MESSAGE_MAP()

CADCCompenPageDlg::CADCCompenPageDlg(CWnd* pParent /*=NULL*/)
: Super(CADCCompenPageDlg::IDD, pParent)
, m_enabled(false)
, m_enable_secu3t_features(false)
, m_map_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_map_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ubat_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ubat_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_temp_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_temp_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_tps_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_tps_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai1_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai1_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai2_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai2_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai3_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai3_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai4_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai4_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai5_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai5_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai6_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai6_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai7_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai7_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai8_factor_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ai8_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, mp_scr(new CWndScroller)
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
 m_params.ai3_adc_factor = 1.0f;
 m_params.ai3_adc_correction = 0.0f;
 m_params.ai4_adc_factor = 1.0f;
 m_params.ai4_adc_correction = 0.0f;
 m_params.ai5_adc_factor = 1.0f;
 m_params.ai5_adc_correction = 0.0f;
 m_params.ai6_adc_factor = 1.0f;
 m_params.ai6_adc_correction = 0.0f;
 m_params.ai7_adc_factor = 1.0f;
 m_params.ai7_adc_correction = 0.0f;
 m_params.ai8_adc_factor = 1.0f;
 m_params.ai8_adc_correction = 0.0f;
}

CADCCompenPageDlg::~CADCCompenPageDlg()
{
 //empty
}

LPCTSTR CADCCompenPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CADCCompenPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_MAP_FACTOR_SPIN, m_map_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_MAP_CORRECTION_SPIN, m_map_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, m_map_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_MAP_CORRECTION_EDIT, m_map_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_UBAT_FACTOR_SPIN, m_ubat_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_UBAT_CORRECTION_SPIN, m_ubat_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, m_ubat_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_UBAT_CORRECTION_EDIT, m_ubat_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TEMP_FACTOR_SPIN, m_temp_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TEMP_CORRECTION_SPIN, m_temp_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, m_temp_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TEMP_CORRECTION_EDIT, m_temp_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TPS_FACTOR_SPIN, m_tps_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TPS_CORRECTION_SPIN, m_tps_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TPS_FACTOR_EDIT, m_tps_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_TPS_CORRECTION_EDIT, m_tps_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_SPIN, m_ai1_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I1_CORRECTION_SPIN, m_ai1_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_EDIT, m_ai1_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I1_CORRECTION_EDIT, m_ai1_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_SPIN, m_ai2_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I2_CORRECTION_SPIN, m_ai2_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_EDIT, m_ai2_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I2_CORRECTION_EDIT, m_ai2_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I3_FACTOR_SPIN, m_ai3_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I3_CORRECTION_SPIN, m_ai3_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I3_FACTOR_EDIT, m_ai3_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I3_CORRECTION_EDIT, m_ai3_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I4_FACTOR_SPIN, m_ai4_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I4_CORRECTION_SPIN, m_ai4_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I4_FACTOR_EDIT, m_ai4_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I4_CORRECTION_EDIT, m_ai4_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I5_FACTOR_SPIN, m_ai5_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I5_CORRECTION_SPIN, m_ai5_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I5_FACTOR_EDIT, m_ai5_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I5_CORRECTION_EDIT, m_ai5_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I6_FACTOR_SPIN, m_ai6_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I6_CORRECTION_SPIN, m_ai6_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I6_FACTOR_EDIT, m_ai6_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I6_CORRECTION_EDIT, m_ai6_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I7_FACTOR_SPIN, m_ai7_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I7_CORRECTION_SPIN, m_ai7_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I7_FACTOR_EDIT, m_ai7_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I7_CORRECTION_EDIT, m_ai7_correction_edit);

 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I8_FACTOR_SPIN, m_ai8_factor_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I8_CORRECTION_SPIN, m_ai8_correction_spin);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I8_FACTOR_EDIT, m_ai8_factor_edit);
 DDX_Control(pDX, IDC_PD_ADCCOMPEN_ADD_I8_CORRECTION_EDIT, m_ai8_correction_edit);

 m_map_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_MAP_FACTOR_EDIT, m_params.map_adc_factor);
 m_map_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_MAP_CORRECTION_EDIT, m_params.map_adc_correction);

 m_ubat_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_UBAT_FACTOR_EDIT, m_params.ubat_adc_factor);
 m_ubat_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_UBAT_CORRECTION_EDIT, m_params.ubat_adc_correction);

 m_temp_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TEMP_FACTOR_EDIT, m_params.temp_adc_factor);
 m_temp_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TEMP_CORRECTION_EDIT, m_params.temp_adc_correction);

 m_tps_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TPS_FACTOR_EDIT, m_params.tps_adc_factor);
 m_tps_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_TPS_CORRECTION_EDIT, m_params.tps_adc_correction);

 m_ai1_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I1_FACTOR_EDIT, m_params.ai1_adc_factor);
 m_ai1_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I1_CORRECTION_EDIT, m_params.ai1_adc_correction);

 m_ai2_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I2_FACTOR_EDIT, m_params.ai2_adc_factor);
 m_ai2_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I2_CORRECTION_EDIT, m_params.ai2_adc_correction);

 m_ai3_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I3_FACTOR_EDIT, m_params.ai3_adc_factor);
 m_ai3_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I3_CORRECTION_EDIT, m_params.ai3_adc_correction);

 m_ai4_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I4_FACTOR_EDIT, m_params.ai4_adc_factor);
 m_ai4_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I4_CORRECTION_EDIT, m_params.ai4_adc_correction);

 m_ai5_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I5_FACTOR_EDIT, m_params.ai5_adc_factor);
 m_ai5_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I5_CORRECTION_EDIT, m_params.ai5_adc_correction);

 m_ai6_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I6_FACTOR_EDIT, m_params.ai6_adc_factor);
 m_ai6_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I6_CORRECTION_EDIT, m_params.ai6_adc_correction);

 m_ai7_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I7_FACTOR_EDIT, m_params.ai7_adc_factor);
 m_ai7_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I7_CORRECTION_EDIT, m_params.ai7_adc_correction);

 m_ai8_factor_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I8_FACTOR_EDIT, m_params.ai8_adc_factor);
 m_ai8_correction_edit.DDX_Value(pDX, IDC_PD_ADCCOMPEN_ADD_I8_CORRECTION_EDIT, m_params.ai8_adc_correction);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CADCCompenPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CADCCompenPageDlg::OnUpdateControlsSECU3i(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_enable_secu3t_features);
}

void CADCCompenPageDlg::OnUpdateControlsSECU3iEx(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_enable_secu3t_features && m_enable_extraio);
}

/////////////////////////////////////////////////////////////////////////////
// CADCCompenPageDlg message handlers

BOOL CADCCompenPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_map_factor_spin.SetBuddy(&m_map_factor_edit);
 m_map_factor_edit.SetLimitText(6);
 m_map_factor_edit.SetDecimalPlaces(3);
 m_map_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_map_factor_edit.SetRange(0.0f,3.999f);

 m_map_correction_spin.SetBuddy(&m_map_correction_edit);
 m_map_correction_edit.SetLimitText(6);
 m_map_correction_edit.SetDecimalPlaces(4);
 m_map_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_map_correction_edit.SetRange(-2.0f,1.999f);

 m_ubat_factor_spin.SetBuddy(&m_ubat_factor_edit);
 m_ubat_factor_edit.SetLimitText(6);
 m_ubat_factor_edit.SetDecimalPlaces(3);
 m_ubat_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ubat_factor_edit.SetRange(0.0f,3.999f);

 m_ubat_correction_spin.SetBuddy(&m_ubat_correction_edit);
 m_ubat_correction_edit.SetLimitText(6);
 m_ubat_correction_edit.SetDecimalPlaces(4);
 m_ubat_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ubat_correction_edit.SetRange(-2.0f,1.999f);

 m_temp_factor_spin.SetBuddy(&m_temp_factor_edit);
 m_temp_factor_edit.SetLimitText(6);
 m_temp_factor_edit.SetDecimalPlaces(3);
 m_temp_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_temp_factor_edit.SetRange(0.0f,3.999f);

 m_temp_correction_spin.SetBuddy(&m_temp_correction_edit);
 m_temp_correction_edit.SetLimitText(6);
 m_temp_correction_edit.SetDecimalPlaces(4);
 m_temp_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_temp_correction_edit.SetRange(-2.0f,1.999f);

 m_tps_factor_spin.SetBuddy(&m_tps_factor_edit);
 m_tps_factor_edit.SetLimitText(6);
 m_tps_factor_edit.SetDecimalPlaces(3);
 m_tps_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_tps_factor_edit.SetRange(0.0f,3.999f);

 m_tps_correction_spin.SetBuddy(&m_tps_correction_edit);
 m_tps_correction_edit.SetLimitText(6);
 m_tps_correction_edit.SetDecimalPlaces(4);
 m_tps_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_tps_correction_edit.SetRange(-2.0f,1.999f);

 m_ai1_factor_spin.SetBuddy(&m_ai1_factor_edit);
 m_ai1_factor_edit.SetLimitText(6);
 m_ai1_factor_edit.SetDecimalPlaces(3);
 m_ai1_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ai1_factor_edit.SetRange(0.0f,3.999f);

 m_ai1_correction_spin.SetBuddy(&m_ai1_correction_edit);
 m_ai1_correction_edit.SetLimitText(6);
 m_ai1_correction_edit.SetDecimalPlaces(4);
 m_ai1_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ai1_correction_edit.SetRange(-2.0f,1.999f);

 m_ai2_factor_spin.SetBuddy(&m_ai2_factor_edit);
 m_ai2_factor_edit.SetLimitText(6);
 m_ai2_factor_edit.SetDecimalPlaces(3);
 m_ai2_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ai2_factor_edit.SetRange(0.0f,3.999f);

 m_ai2_correction_spin.SetBuddy(&m_ai2_correction_edit);
 m_ai2_correction_edit.SetLimitText(6);
 m_ai2_correction_edit.SetDecimalPlaces(4);
 m_ai2_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ai2_correction_edit.SetRange(-2.0f,1.999f);

 m_ai3_factor_spin.SetBuddy(&m_ai3_factor_edit);
 m_ai3_factor_edit.SetLimitText(6);
 m_ai3_factor_edit.SetDecimalPlaces(3);
 m_ai3_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ai3_factor_edit.SetRange(0.0f,3.999f);

 m_ai3_correction_spin.SetBuddy(&m_ai3_correction_edit);
 m_ai3_correction_edit.SetLimitText(6);
 m_ai3_correction_edit.SetDecimalPlaces(4);
 m_ai3_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ai3_correction_edit.SetRange(-2.0f,1.999f);

 m_ai4_factor_spin.SetBuddy(&m_ai4_factor_edit);
 m_ai4_factor_edit.SetLimitText(6);
 m_ai4_factor_edit.SetDecimalPlaces(3);
 m_ai4_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ai4_factor_edit.SetRange(0.0f,3.999f);

 m_ai4_correction_spin.SetBuddy(&m_ai4_correction_edit);
 m_ai4_correction_edit.SetLimitText(6);
 m_ai4_correction_edit.SetDecimalPlaces(4);
 m_ai4_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ai4_correction_edit.SetRange(-2.0f,1.999f);

 m_ai5_factor_spin.SetBuddy(&m_ai5_factor_edit);
 m_ai5_factor_edit.SetLimitText(6);
 m_ai5_factor_edit.SetDecimalPlaces(3);
 m_ai5_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ai5_factor_edit.SetRange(0.0f,3.999f);

 m_ai5_correction_spin.SetBuddy(&m_ai5_correction_edit);
 m_ai5_correction_edit.SetLimitText(6);
 m_ai5_correction_edit.SetDecimalPlaces(4);
 m_ai5_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ai5_correction_edit.SetRange(-2.0f,1.999f);

 m_ai6_factor_spin.SetBuddy(&m_ai6_factor_edit);
 m_ai6_factor_edit.SetLimitText(6);
 m_ai6_factor_edit.SetDecimalPlaces(3);
 m_ai6_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ai6_factor_edit.SetRange(0.0f,3.999f);

 m_ai6_correction_spin.SetBuddy(&m_ai6_correction_edit);
 m_ai6_correction_edit.SetLimitText(6);
 m_ai6_correction_edit.SetDecimalPlaces(4);
 m_ai6_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ai6_correction_edit.SetRange(-2.0f,1.999f);

 m_ai7_factor_spin.SetBuddy(&m_ai7_factor_edit);
 m_ai7_factor_edit.SetLimitText(6);
 m_ai7_factor_edit.SetDecimalPlaces(3);
 m_ai7_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ai7_factor_edit.SetRange(0.0f,3.999f);

 m_ai7_correction_spin.SetBuddy(&m_ai7_correction_edit);
 m_ai7_correction_edit.SetLimitText(6);
 m_ai7_correction_edit.SetDecimalPlaces(4);
 m_ai7_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ai7_correction_edit.SetRange(-2.0f,1.999f);

 m_ai8_factor_spin.SetBuddy(&m_ai8_factor_edit);
 m_ai8_factor_edit.SetLimitText(6);
 m_ai8_factor_edit.SetDecimalPlaces(3);
 m_ai8_factor_spin.SetRangeAndDelta(0.0f,3.999f,0.001f);
 m_ai8_factor_edit.SetRange(0.0f,3.999f);

 m_ai8_correction_spin.SetBuddy(&m_ai8_correction_edit);
 m_ai8_correction_edit.SetLimitText(6);
 m_ai8_correction_edit.SetDecimalPlaces(4);
 m_ai8_correction_spin.SetRangeAndDelta(-2.0f,1.999f,0.0025f);
 m_ai8_correction_edit.SetRange(-2.0f,1.999f);

 UpdateData(FALSE);

 //create tooltip control
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 //set tooltips form different widgets
 VERIFY(mp_ttc->AddWindow(&m_map_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ubat_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ubat_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ubat_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ubat_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_temp_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_temp_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_temp_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_temp_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_tps_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ai1_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai1_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai1_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai1_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ai2_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai2_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai2_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai2_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ai3_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai3_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai3_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai3_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ai4_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai4_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai4_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai4_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ai5_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai5_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai5_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai5_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ai6_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai6_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai6_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai6_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ai7_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai7_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai7_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai7_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ai8_factor_edit, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai8_correction_edit, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai8_factor_spin, MLL::GetString(IDS_PD_ADCCOMPEN_FACTOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ai8_correction_spin, MLL::GetString(IDS_PD_ADCCOMPEN_CORRECTION_TT)));

 //initialize window scroller
 mp_scr->Init(this);

 mp_ttc->SetMaxTipWidth(250); //Set width for text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CADCCompenPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
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

void CADCCompenPageDlg::EnableExtraIO(bool i_enable)
{
 if (m_enable_extraio == i_enable)
  return; //already has needed state
 m_enable_extraio = i_enable;
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

void CADCCompenPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(480));
}
