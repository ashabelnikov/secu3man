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

/** \file FunSetPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/dpiaware.h"
#include "FunSetPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"
#include "ui-core/DynFieldsDialog.h"
#include "ui-core/MsgBox.h"
#include "../MAPCalc/MAPCalcController.h"
#include "common/Calculations.h"
#include <math.h>

BEGIN_MESSAGE_MAP(CFunSetPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_BENZIN_MAPS_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_GAS_MAPS_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_LOAD_SRC_COMBO, OnChangeDataLoadSrc)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_GAS_UNI_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_BENZIN_UNI_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_BAROCORRTYPE_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_VE2MF_COMBO, OnChangeVE2MF)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_GAS_V_UNI_COMBO, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_MAP_GRAD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_PRESS_SWING_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_OFFSET_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_OFFSET2_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_GRADIENT2_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_OFFSET3_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_GRADIENT3_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_OFFSET4_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_GRADIENT4_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_OFFSET5_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_GRADIENT5_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_MAP_CALC_BUTTON, OnMapCalcButton)
 ON_BN_CLICKED(IDC_PD_MAP_CALC2_BUTTON, OnMap2CalcButton)
 ON_BN_CLICKED(IDC_PD_MAP_CALC3_BUTTON, OnGpsCalcButton)
 ON_BN_CLICKED(IDC_PD_MAP_CALC4_BUTTON, OnFpsCalcButton)
 ON_BN_CLICKED(IDC_PD_APPS1_CALC_BUTTON, OnApps1CalcButton)
 ON_BN_CLICKED(IDC_PD_FUNSET_USE_LDAX_GRID, OnChangeDataLdaxGrid)
 ON_BN_CLICKED(IDC_PD_TPS_CALC_BUTTON, OnTpsCalcButton)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_EDIT,OnUpdateControlsLower)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_SPIN,OnUpdateControlsLower)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_CAPTION,OnUpdateControlsLower)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_UNIT,OnUpdateControlsLower)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_EDIT,OnUpdateControlsUpper)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_SPIN,OnUpdateControlsUpper)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_CAPTION,OnUpdateControlsUpper)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_UNIT,OnUpdateControlsUpper)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_BENZIN_MAPS_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_BENZIN_MAPS_COMBO_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_GAS_MAPS_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_GAS_MAPS_COMBO_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_LOAD_SRC_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_LOAD_SRC_COMBO_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_GAS_UNI_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_GAS_UNI_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_BENZIN_UNI_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_BENZIN_UNI_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_BAROCORRTYPE_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_BAROCORRTYPE_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET2_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET2_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET2_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET2_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET3_EDIT,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET3_SPIN,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET3_CAPTION,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET3_UNIT,OnUpdateControlsSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET4_EDIT,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET4_SPIN,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET4_CAPTION,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET4_UNIT,OnUpdateControlsSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_OFFSET_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_OFFSET_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_OFFSET_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT2_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT2_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT2_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT2_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT3_EDIT,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT3_SPIN,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT3_CAPTION,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT3_UNIT,OnUpdateControlsSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT4_EDIT,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT4_SPIN,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT4_CAPTION,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT4_UNIT,OnUpdateControlsSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_SENSOR2_GROUP,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_SENSOR3_GROUP,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_SENSOR4_GROUP,OnUpdateControlsSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_AXIS_PAR_GROUP,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_SENSOR_GROUP,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_SENSOR_GROUP,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_MAP_CALC_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MAP_CALC2_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MAP_CALC3_BUTTON, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_MAP_CALC4_BUTTON, OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_TPS_CALC_BUTTON, OnUpdateControlsTPSLearning)
 ON_UPDATE_COMMAND_UI(IDC_PD_APPS1_CALC_BUTTON, OnUpdateControlsAPPS1Learning)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_VE2MF_CAPTION, OnUpdateControlsFuelInject)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_VE2MF_COMBO, OnUpdateControlsFuelInject)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_GAS_V_UNI_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_GAS_V_UNI_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_USE_LDAX_GRID, OnUpdateControls)

 //APPS1
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET5_EDIT,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET5_SPIN,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET5_CAPTION,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET5_UNIT,OnUpdateControlsSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT5_EDIT,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT5_SPIN,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT5_CAPTION,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT5_UNIT,OnUpdateControlsSECU3i)

END_MESSAGE_MAP()

CFunSetPageDlg::CFunSetPageDlg(bool tps_learning /*=true*/, bool apps1_learning /*=true*/)
: m_enabled(false)
, m_enable_secu3t_features(false)
, m_map_grad_edit(CEditEx::MODE_FLOAT, true)
, m_press_swing_edit(CEditEx::MODE_FLOAT, true)
, m_map_curve_offset_edit(CEditEx::MODE_FLOAT, true)
, m_map_curve_gradient_edit(CEditEx::MODE_FLOAT, true)
, m_map2_curve_offset_edit(CEditEx::MODE_FLOAT, true)
, m_map2_curve_gradient_edit(CEditEx::MODE_FLOAT, true)
, m_tps_curve_offset_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_tps_curve_gradient_edit(CEditEx::MODE_FLOAT, true)
, mp_scr(new CWndScroller)
, m_fuel_injection(false)
, m_tps_learning(tps_learning)
, m_apps1_learning(apps1_learning)
, m_tpsl_push_value(.0f)
, m_tpsl_release_value(.0f)
, m_apps1l_push_value(.0f)
, m_apps1l_release_value(.0f)
, m_gps_curve_offset_edit(CEditEx::MODE_FLOAT, true)
, m_gps_curve_gradient_edit(CEditEx::MODE_FLOAT, true)
, m_fps_curve_offset_edit(CEditEx::MODE_FLOAT, true)
, m_fps_curve_gradient_edit(CEditEx::MODE_FLOAT, true)
, m_apps1_curve_offset_edit(CEditEx::MODE_FLOAT, true)
, m_apps1_curve_gradient_edit(CEditEx::MODE_FLOAT, true)
{
 m_params.map_lower_pressure = 4.5f;
 m_params.map_upper_pressure = 10.0f;
 m_params.fn_benzin = 0;
 m_params.fn_gas = 1;
 m_params.map_curve_offset = 0.547f;
 m_params.map_curve_gradient = 20.9f;
 m_params.map2_curve_offset = 0.547f;
 m_params.map2_curve_gradient = 20.9f;
 m_params.tps_curve_offset = 0.4f; //(V)
 m_params.tps_curve_gradient = 25.64f; //(%/V)
 m_params.load_src_cfg = 0;
 m_params.uni_benzin = SECU3IO::UNI_OUTPUT_NUM; //disabled
 m_params.uni_gas = SECU3IO::UNI_OUTPUT_NUM; //disabled
 m_params.barocorr_type = 0; //disabled
 m_params.use_load_grid = false;
 m_params.ve2_map_func = 0; //use 1st VE map
 m_params.uni_gas_v = SECU3IO::UNI_OUTPUT_NUM; //disabled
 m_params.tps_raw = .0f;
 m_params.gps_curve_offset = 0.547f;
 m_params.gps_curve_gradient = 20.9f;
 m_params.fps_curve_offset = 0.5f;
 m_params.fps_curve_gradient = 172.25f;
 m_params.apps1_curve_offset = 0.720f;
 m_params.apps1_curve_gradient = 26.45f;
}

LPCTSTR CFunSetPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PD_FUNSET_PAGE;
}

void CFunSetPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_FUNSET_GAS_MAPS_COMBO, m_gas_maps_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_BENZIN_MAPS_COMBO, m_benzin_maps_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_LOAD_SRC_COMBO, m_load_src_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_GAS_UNI_COMBO, m_gas_uni_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_BENZIN_UNI_COMBO, m_benzin_uni_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_BAROCORRTYPE_COMBO, m_barocorr_type_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_VE2MF_COMBO, m_ve2mf_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_SPIN, m_press_swing_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_press_swing_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_SPIN, m_map_grad_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_map_grad_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET_EDIT, m_map_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET_SPIN, m_map_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET2_EDIT, m_map2_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET2_SPIN, m_map2_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET3_EDIT, m_gps_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET3_SPIN, m_gps_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET4_EDIT, m_fps_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET4_SPIN, m_fps_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, m_map_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_SPIN, m_map_curve_gradient_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT2_EDIT, m_map2_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT2_SPIN, m_map2_curve_gradient_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT3_EDIT, m_gps_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT3_SPIN, m_gps_curve_gradient_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT4_EDIT, m_fps_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT4_SPIN, m_fps_curve_gradient_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT, m_tps_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_TPS_CURVE_OFFSET_SPIN, m_tps_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT, m_tps_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_TPS_CURVE_GRADIENT_SPIN, m_tps_curve_gradient_spin);
 DDX_Control(pDX, IDC_PD_MAP_CALC_BUTTON, m_calc_map_btn);
 DDX_Control(pDX, IDC_PD_MAP_CALC2_BUTTON, m_calc_map2_btn);
 DDX_Control(pDX, IDC_PD_MAP_CALC3_BUTTON, m_calc_gps_btn);
 DDX_Control(pDX, IDC_PD_MAP_CALC4_BUTTON, m_calc_fps_btn);
 DDX_Control(pDX, IDC_PD_TPS_CALC_BUTTON, m_calc_tps_btn);
 DDX_Control(pDX, IDC_PD_APPS1_CALC_BUTTON, m_calc_apps1_btn);
 DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_UNIT, m_lolo_unit);
 DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_UNIT, m_hilo_unit);
 DDX_Control(pDX, IDC_PD_FUNSET_USE_LDAX_GRID, m_use_ldax_grid_check);
 DDX_Control(pDX, IDC_PD_FUNSET_GAS_V_UNI_COMBO, m_gas_v_uni_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET5_EDIT, m_apps1_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET5_SPIN, m_apps1_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT5_EDIT, m_apps1_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT5_SPIN, m_apps1_curve_gradient_spin);

 m_map_grad_edit.DDX_Value(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_params.map_lower_pressure);
 m_press_swing_edit.DDX_Value(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_params.map_upper_pressure);
 m_map_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_OFFSET_EDIT, m_params.map_curve_offset);
 m_map_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, m_params.map_curve_gradient);
 m_map2_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_OFFSET2_EDIT, m_params.map2_curve_offset);
 m_map2_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_GRADIENT2_EDIT, m_params.map2_curve_gradient);
 m_gps_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_OFFSET3_EDIT, m_params.gps_curve_offset);
 m_gps_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_GRADIENT3_EDIT, m_params.gps_curve_gradient);
 m_fps_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_OFFSET4_EDIT, m_params.fps_curve_offset);
 m_fps_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_GRADIENT4_EDIT, m_params.fps_curve_gradient);
 m_tps_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT, m_params.tps_curve_offset);
 m_tps_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT, m_params.tps_curve_gradient);
 m_apps1_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_OFFSET5_EDIT, m_params.apps1_curve_offset);
 m_apps1_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_GRADIENT5_EDIT, m_params.apps1_curve_gradient);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_BENZIN_MAPS_COMBO, m_params.fn_benzin);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_GAS_MAPS_COMBO, m_params.fn_gas);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_LOAD_SRC_COMBO, m_params.load_src_cfg);
 DDX_CBIndex_int(pDX, IDC_PD_FUNSET_GAS_UNI_COMBO, m_params.uni_gas);
 DDX_CBIndex_int(pDX, IDC_PD_FUNSET_BENZIN_UNI_COMBO, m_params.uni_benzin);
 DDX_CBIndex_int(pDX, IDC_PD_FUNSET_BAROCORRTYPE_COMBO, m_params.barocorr_type);
 DDX_Check_bool(pDX, IDC_PD_FUNSET_USE_LDAX_GRID, m_params.use_load_grid);
 DDX_CBIndex_int(pDX, IDC_PD_FUNSET_VE2MF_COMBO, m_params.ve2_map_func);
 DDX_CBIndex_int(pDX, IDC_PD_FUNSET_GAS_V_UNI_COMBO, m_params.uni_gas_v);
}

/////////////////////////////////////////////////////////////////////////////
// CFunSetPageDlg message handlers

//Update controls
void CFunSetPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CFunSetPageDlg::OnUpdateControlsLower(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_use_ldax_grid_check.GetCheck()==BST_CHECKED);
}

void CFunSetPageDlg::OnUpdateControlsUpper(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (m_params.load_src_cfg != 1) && !m_use_ldax_grid_check.GetCheck()==BST_CHECKED); //disabled if MAP(baro) selected as load type
}

void CFunSetPageDlg::OnUpdateControlsSECU3i(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_enable_secu3t_features);
}

void CFunSetPageDlg::OnUpdateControlsFuelInject(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_fuel_injection);
}

void CFunSetPageDlg::OnUpdateControlsTPSLearning(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_tps_learning);
}

void CFunSetPageDlg::OnUpdateControlsAPPS1Learning(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_apps1_learning);
}

BOOL CFunSetPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_calc_map_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                            MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));

 m_calc_map2_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                            MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));

 m_calc_gps_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                            MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));

 m_calc_fps_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                            MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));

 m_calc_tps_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                            MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));

 m_calc_apps1_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                            MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));

 m_map_grad_spin.SetBuddy(&m_map_grad_edit);
 m_map_grad_edit.SetLimitText(4);
 m_map_grad_edit.SetDecimalPlaces(2);
 m_map_grad_spin.SetRangeAndDelta(0.25f,105.0f,0.25f);
 m_map_grad_edit.SetRange(0.25f,105.0f);

 m_press_swing_spin.SetBuddy(&m_press_swing_edit);
 m_press_swing_edit.SetLimitText(4);
 m_press_swing_edit.SetDecimalPlaces(2);
 m_press_swing_spin.SetRangeAndDelta(0.25,500.0f,0.25f);
 m_press_swing_edit.SetRange(0.25,500.0f);

 m_map_curve_offset_spin.SetBuddy(&m_map_curve_offset_edit);
 m_map_curve_offset_edit.SetLimitText(5);
 m_map_curve_offset_edit.SetDecimalPlaces(3);
 m_map_curve_offset_spin.SetRangeAndDelta(-5.0f,5.0f,0.0025f);
 m_map_curve_offset_edit.SetRange(-5.0f,5.0f);

 m_map_curve_gradient_spin.SetBuddy(&m_map_curve_gradient_edit);
 m_map_curve_gradient_edit.SetLimitText(6);
 m_map_curve_gradient_edit.SetDecimalPlaces(3);
 m_map_curve_gradient_spin.SetRangeAndDelta(-150.0f,150.0f,0.01f);
 m_map_curve_gradient_edit.SetRange(-150.0f,150.0f);

 m_map2_curve_offset_spin.SetBuddy(&m_map2_curve_offset_edit);
 m_map2_curve_offset_edit.SetLimitText(5);
 m_map2_curve_offset_edit.SetDecimalPlaces(3);
 m_map2_curve_offset_spin.SetRangeAndDelta(-5.0f,5.0f,0.0025f);
 m_map2_curve_offset_edit.SetRange(-5.0f,5.0f);

 m_map2_curve_gradient_spin.SetBuddy(&m_map2_curve_gradient_edit);
 m_map2_curve_gradient_edit.SetLimitText(6);
 m_map2_curve_gradient_edit.SetDecimalPlaces(3);
 m_map2_curve_gradient_spin.SetRangeAndDelta(-150.0f,150.0f,0.01f);
 m_map2_curve_gradient_edit.SetRange(-150.0f,150.0f);

 m_gps_curve_offset_spin.SetBuddy(&m_gps_curve_offset_edit);
 m_gps_curve_offset_edit.SetLimitText(5);
 m_gps_curve_offset_edit.SetDecimalPlaces(3);
 m_gps_curve_offset_spin.SetRangeAndDelta(-5.0f,5.0f,0.0025f);
 m_gps_curve_offset_edit.SetRange(-5.0f,5.0f);

 m_gps_curve_gradient_spin.SetBuddy(&m_gps_curve_gradient_edit);
 m_gps_curve_gradient_edit.SetLimitText(6);
 m_gps_curve_gradient_edit.SetDecimalPlaces(3);
 m_gps_curve_gradient_spin.SetRangeAndDelta(-150.0f,150.0f,0.01f);
 m_gps_curve_gradient_edit.SetRange(-150.0f,150.0f);

 m_fps_curve_offset_spin.SetBuddy(&m_fps_curve_offset_edit);
 m_fps_curve_offset_edit.SetLimitText(5);
 m_fps_curve_offset_edit.SetDecimalPlaces(3);
 m_fps_curve_offset_spin.SetRangeAndDelta(-5.0f,5.0f,0.0025f);
 m_fps_curve_offset_edit.SetRange(-5.0f,5.0f);

 m_fps_curve_gradient_spin.SetBuddy(&m_fps_curve_gradient_edit);
 m_fps_curve_gradient_edit.SetLimitText(6);
 m_fps_curve_gradient_edit.SetDecimalPlaces(3);
 m_fps_curve_gradient_spin.SetRangeAndDelta(-500.0f,500.0f,0.01f);
 m_fps_curve_gradient_edit.SetRange(-500.0f,500.0f);

 m_tps_curve_offset_spin.SetBuddy(&m_tps_curve_offset_edit);
 m_tps_curve_offset_edit.SetLimitText(6);
 m_tps_curve_offset_edit.SetDecimalPlaces(3);
 m_tps_curve_offset_spin.SetRangeAndDelta(-5.0f, 5.0f, 0.0025f);
 m_tps_curve_offset_edit.SetRange(-5.0f, 5.0f);

 m_tps_curve_gradient_spin.SetBuddy(&m_tps_curve_gradient_edit);
 m_tps_curve_gradient_edit.SetLimitText(6);
 m_tps_curve_gradient_edit.SetDecimalPlaces(3);
 m_tps_curve_gradient_spin.SetRangeAndDelta(-100.0f, 100.0f, 0.01f);
 m_tps_curve_gradient_edit.SetRange(-100.0f, 100.0f);

 m_apps1_curve_offset_spin.SetBuddy(&m_apps1_curve_offset_edit);
 m_apps1_curve_offset_edit.SetLimitText(6);
 m_apps1_curve_offset_edit.SetDecimalPlaces(3);
 m_apps1_curve_offset_spin.SetRangeAndDelta(-5.0f,5.0f,0.0025f);
 m_apps1_curve_offset_edit.SetRange(-5.0f,5.0f);

 m_apps1_curve_gradient_spin.SetBuddy(&m_apps1_curve_gradient_edit);
 m_apps1_curve_gradient_edit.SetLimitText(7);
 m_apps1_curve_gradient_edit.SetDecimalPlaces(3);
 m_apps1_curve_gradient_spin.SetRangeAndDelta(-500.0f,500.0f,0.01f);
 m_apps1_curve_gradient_edit.SetRange(-500.0f,500.0f);

 m_gas_uni_combo.AddString(_T("1"));
 m_gas_uni_combo.AddString(_T("2"));
 m_gas_uni_combo.AddString(_T("3"));
 m_gas_uni_combo.AddString(_T("4"));
 m_gas_uni_combo.AddString(_T("5"));
 m_gas_uni_combo.AddString(_T("6"));
 m_gas_uni_combo.AddString(_T("--no--"));

 m_benzin_uni_combo.AddString(_T("1"));
 m_benzin_uni_combo.AddString(_T("2"));
 m_benzin_uni_combo.AddString(_T("3"));
 m_benzin_uni_combo.AddString(_T("4"));
 m_benzin_uni_combo.AddString(_T("5"));
 m_benzin_uni_combo.AddString(_T("6"));
 m_benzin_uni_combo.AddString(_T("--no--"));

 m_barocorr_type_combo.AddString(MLL::LoadString(IDS_PD_FUNSET_BAROCORR_TYPE0));
 m_barocorr_type_combo.AddString(MLL::LoadString(IDS_PD_FUNSET_BAROCORR_TYPE1));
 m_barocorr_type_combo.AddString(MLL::LoadString(IDS_PD_FUNSET_BAROCORR_TYPE2));
 m_barocorr_type_combo.AddString(MLL::LoadString(IDS_PD_FUNSET_BAROCORR_TYPE3));

 m_ve2mf_combo.AddString(_T("VE1"));     //0 - use 1st VE map
 m_ve2mf_combo.AddString(_T("VE1*VE2")); //1 - use multiplication
 m_ve2mf_combo.AddString(_T("VE1+VE2")); //2 - use addition

 m_gas_v_uni_combo.AddString(_T("1"));
 m_gas_v_uni_combo.AddString(_T("2"));
 m_gas_v_uni_combo.AddString(_T("3"));
 m_gas_v_uni_combo.AddString(_T("4"));
 m_gas_v_uni_combo.AddString(_T("5"));
 m_gas_v_uni_combo.AddString(_T("6"));
 m_gas_v_uni_combo.AddString(_T("--no--"));

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_gradient_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_gradient_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_map2_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map2_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map2_curve_gradient_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map2_curve_gradient_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_gps_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gps_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gps_curve_gradient_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gps_curve_gradient_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_fps_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fps_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fps_curve_gradient_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fps_curve_gradient_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_press_swing_edit, MLL::GetString(IDS_PD_FUNSET_PRESS_SWING_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_press_swing_spin, MLL::GetString(IDS_PD_FUNSET_PRESS_SWING_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_grad_edit, MLL::GetString(IDS_PD_FUNSET_MAP_GRAD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_grad_spin, MLL::GetString(IDS_PD_FUNSET_MAP_GRAD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_tps_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_curve_gradient_edit, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_curve_gradient_spin, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_calc_map_btn, MLL::GetString(IDS_PD_MAP_CALC_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_calc_map2_btn, MLL::GetString(IDS_PD_MAP_CALC2_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_calc_gps_btn, MLL::GetString(IDS_PD_MAP_CALC2_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_calc_fps_btn, MLL::GetString(IDS_PD_MAP_CALC2_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_calc_tps_btn, MLL::GetString(IDS_PD_TPS_CALC_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_calc_apps1_btn, MLL::GetString(IDS_PD_APPS_CALC_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gas_maps_combo, MLL::GetString(IDS_PD_FUNSET_GAS_MAPS_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_benzin_maps_combo, MLL::GetString(IDS_PD_FUNSET_BENZIN_MAPS_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_load_src_combo, MLL::GetString(IDS_PD_FUNSET_LOAD_SRC_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_benzin_uni_combo, MLL::GetString(IDS_PD_FUNSET_BENZIN_UNI_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gas_uni_combo, MLL::GetString(IDS_PD_FUNSET_GAS_UNI_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_barocorr_type_combo, MLL::GetString(IDS_PD_FUNSET_BAROCORRTYPE_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_use_ldax_grid_check, MLL::GetString(IDC_PD_FUNSET_USE_LDAX_GRID_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ve2mf_combo, MLL::GetString(IDC_PD_FUNSET_VE2MF_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gas_v_uni_combo, MLL::GetString(IDS_PD_FUNSET_GAS_V_UNI_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_apps1_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_APPS_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_apps1_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_APPS_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_apps1_curve_gradient_edit, MLL::GetString(IDS_PD_FUNSET_APPS_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_apps1_curve_gradient_spin, MLL::GetString(IDS_PD_FUNSET_APPS_CURVE_GRADIENT_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 FillCBByFunNames(); //Initialize content of tables' set ComboBox
 FillCBByLoadOpts();
 UpdateData(false);  //Initialize dialog controls with data
 UpdateDialogControls(this, TRUE);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CFunSetPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
 m_tpsl_tmr.KillTimer();
 m_apps1l_tmr.KillTimer();
}

void CFunSetPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CFunSetPageDlg::OnChangeDataLdaxGrid()
{
 UpdateDialogControls(this, TRUE);
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CFunSetPageDlg::OnChangeVE2MF()
{
 UpdateData();
 OnChangeNotify();
}

void CFunSetPageDlg::UpdateLoadAxisUnits(void)
{
 int ids = (m_params.load_src_cfg < 2) ? IDS_KPA_UNIT : IDS_PERCENT_UNIT;
 m_lolo_unit.SetWindowText(MLL::LoadString(ids));
 m_hilo_unit.SetWindowText(MLL::LoadString(ids));
}

void CFunSetPageDlg::OnChangeDataLoadSrc()
{
 UpdateData();
 UpdateLoadAxisUnits();

 //edit engine size here in the modal dialog window if firmware has no fuel injection support (Injection tab is disabled in ignition-only firmwares)
 if (m_params.load_src_cfg == 4 && !m_fuel_injection) //MAF
 {
  CDynFieldsContainer dfd(this, MLL::GetString(IDS_EDIT_VALUE), 120, true);
  float engineSize = m_params.inj_cyl_disp * m_params.cyl_num;
  dfd.AppendItem(MLL::GetString(IDS_ENGINE_SIZE), _T("L"), 0.01f, 8.0f, 0.0001f, 4, &engineSize, MLL::GetString(IDS_ENGINE_SIZE_TT));
  if (dfd.DoModal()==IDOK)
  {
   //calculate new value of cylinder's displacement
   m_params.inj_cyl_disp = engineSize / m_params.cyl_num;
   //calculate constant used for calculation of MAF load (synthetic load)
   m_params.mafload_const = calc::calcMAFLoadConst(m_params.inj_cyl_disp, m_params.cyl_num);
  }
 }

 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
 UpdateDialogControls(this, TRUE);
}

void CFunSetPageDlg::OnMapCalcButton()
{
 float offset = m_params.map_curve_offset, gradient = m_params.map_curve_gradient; 
 if (CMAPCalcController::Calculate(offset, gradient))
 {
  m_params.map_curve_offset = offset;
  m_params.map_curve_gradient = gradient;
  UpdateData(false);
  OnChangeNotify(); //notify event receiver about change
 }
}

void CFunSetPageDlg::OnMap2CalcButton()
{
 float offset = m_params.map2_curve_offset, gradient = m_params.map2_curve_gradient; 
 if (CMAPCalcController::Calculate(offset, gradient))
 {
  m_params.map2_curve_offset = offset;
  m_params.map2_curve_gradient = gradient;
  UpdateData(false);
  OnChangeNotify(); //notify event receiver about change
 }
}

void CFunSetPageDlg::OnGpsCalcButton()
{
 float offset = m_params.gps_curve_offset, gradient = m_params.gps_curve_gradient; 
 if (CMAPCalcController::Calculate(offset, gradient))
 {
  m_params.gps_curve_offset = offset;
  m_params.gps_curve_gradient = gradient;
  UpdateData(false);
  OnChangeNotify(); //notify event receiver about change
 }
}

void CFunSetPageDlg::OnFpsCalcButton()
{
 float offset = m_params.fps_curve_offset, gradient = m_params.fps_curve_gradient; 
 float pressMax = 1000000.0f; //Pa
 if (CMAPCalcController::Calculate(offset, gradient, &pressMax))
 {
  m_params.fps_curve_offset = offset;
  m_params.fps_curve_gradient = gradient;
  UpdateData(false);
  OnChangeNotify(); //notify event receiver about change
 }
}

void CFunSetPageDlg::OnTpsCalcButton()
{
 SECUMessageBox(MLL::GetString(IDS_PD_FUNSET_TPSLEARN_PUSH).c_str(), MB_OK | MB_ICONASTERISK);
 if (m_OnTPSLearning)
  m_OnTPSLearning(1); //start
 m_tpsl_tmr.SetTimer(this, &CFunSetPageDlg::OnTPsLearningPushTimer, 1000);
}

void CFunSetPageDlg::OnApps1CalcButton()
{
 SECUMessageBox(MLL::GetString(IDS_PD_FUNSET_APPSLEARN_PUSH).c_str(), MB_OK | MB_ICONASTERISK);
 if (m_OnAPPS1Learning)
  m_OnAPPS1Learning(1); //start
 m_apps1l_tmr.SetTimer(this, &CFunSetPageDlg::OnAPPS1LearningPushTimer, 1000);
}

void CFunSetPageDlg::OnTPsLearningPushTimer(void)
{
 m_tpsl_push_value = m_params.tps_raw;
 m_tpsl_tmr.KillTimer();
 SECUMessageBox(MLL::GetString(IDS_PD_FUNSET_TPSLEARN_RELEASE).c_str(), MB_OK | MB_ICONASTERISK);
 m_tpsl_tmr.SetTimer(this, &CFunSetPageDlg::OnTPsLearningReleaseTimer, 1000);
}

void CFunSetPageDlg::OnTPsLearningReleaseTimer(void)
{
 m_tpsl_release_value = m_params.tps_raw;
 m_tpsl_tmr.KillTimer();
 if (m_OnTPSLearning)
  m_OnTPSLearning(0); //finish

 float d = m_tpsl_push_value - m_tpsl_release_value;
 if (fabs(d) > 0.1f) //prevent division by zero
 { 
  SECUMessageBox(MLL::GetString(IDS_PD_FUNSET_TPSLEARN_FINISH).c_str(), MB_OK | MB_ICONASTERISK);
  m_params.tps_curve_gradient = (100.0f / d); //gradient in %/V
  m_params.tps_curve_offset = .0f - m_tpsl_release_value;  //offset if V
  UpdateData(false); //copy data from variables to dialog
  OnChangeNotify();
 }
 else
 {
  SECUMessageBox(MLL::GetString(IDS_PD_FUNSET_TPSLEARN_ERROR).c_str(), MB_OK | MB_ICONERROR);
 }
}

void CFunSetPageDlg::OnAPPS1LearningPushTimer(void)
{
 m_apps1l_push_value = m_params.apps1_raw;
 m_apps1l_tmr.KillTimer();
 SECUMessageBox(MLL::GetString(IDS_PD_FUNSET_APPSLEARN_RELEASE).c_str(), MB_OK | MB_ICONASTERISK);
 m_apps1l_tmr.SetTimer(this, &CFunSetPageDlg::OnAPPS1LearningReleaseTimer, 1000);
}

void CFunSetPageDlg::OnAPPS1LearningReleaseTimer(void)
{
 m_apps1l_release_value = m_params.apps1_raw;
 m_apps1l_tmr.KillTimer();
 if (m_OnAPPS1Learning)
  m_OnAPPS1Learning(0); //finish

 float d = m_apps1l_push_value - m_apps1l_release_value;
 if (fabs(d) > 0.1f) //prevent division by zero
 { 
  SECUMessageBox(MLL::GetString(IDS_PD_FUNSET_APPSLEARN_FINISH).c_str(), MB_OK | MB_ICONASTERISK);
  m_params.apps1_curve_gradient = (100.0f / d); //gradient in %/V
  m_params.apps1_curve_offset = .0f - m_apps1l_release_value;  //offset if V
  UpdateData(false); //copy data from variables to dialog
  OnChangeNotify();
 }
 else
 {
  SECUMessageBox(MLL::GetString(IDS_PD_FUNSET_APPSLEARN_ERROR).c_str(), MB_OK | MB_ICONERROR);
 }
}

//enable/disable all controls
void CFunSetPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//get state of controls
bool CFunSetPageDlg::IsEnabled(void)
{
 return m_enabled;
}

void CFunSetPageDlg::EnableSECU3TItems(bool i_enable)
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

void CFunSetPageDlg::EnableFuelInjection(bool i_enable)
{
 if (m_fuel_injection == i_enable)
  return; //already has needed state
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//Fills comboboxes of sets of tables
void CFunSetPageDlg::FillCBByFunNames(void)
{
 if (!::IsWindow(m_hWnd))
  return;

 //delete previous conetent
 m_benzin_maps_combo.ResetContent();
 m_gas_maps_combo.ResetContent();

 //add new content
 for(size_t i = 0; i < m_fun_names.size(); i++)
 {
  m_benzin_maps_combo.AddString(m_fun_names[i].c_str());
  m_gas_maps_combo.AddString(m_fun_names[i].c_str());
 }

 //после удаления всех элементов текущее выделение в ComboBox-ах
 //теряется и мы должны вернуть старое выделение.

 //form gas
 if (m_params.fn_gas < m_fun_names.size())
  m_gas_maps_combo.SetCurSel(m_params.fn_gas);
 else
  m_gas_maps_combo.SetCurSel(0);

 //for petrol
 if (m_params.fn_benzin < m_fun_names.size())
  m_benzin_maps_combo.SetCurSel(m_params.fn_benzin);
 else
  m_benzin_maps_combo.SetCurSel(0);
}

void CFunSetPageDlg::FillCBByLoadOpts(void)
{
 if (!::IsWindow(m_hWnd))
  return;

 m_load_src_combo.ResetContent();

 m_load_src_combo.AddString(MLL::LoadString(IDS_PD_LOAD_OPT_MAP));       //MAP
 m_load_src_combo.AddString(MLL::LoadString(IDS_PD_LOAD_OPT_MAPBARO));   //MAP(baro)
 m_load_src_combo.AddString(MLL::LoadString(IDS_PD_LOAD_OPT_TPS));       //TPS
 m_load_src_combo.AddString(MLL::LoadString(IDS_PD_LOAD_OPT_MAPTPS));    //MAP+TPS
 m_load_src_combo.AddString(MLL::LoadString(IDS_PD_LOAD_OPT_MAF));       //MAF

 //for gas
 if (m_params.load_src_cfg < 5)
  m_load_src_combo.SetCurSel(m_params.load_src_cfg);
 else
  m_load_src_combo.SetCurSel(0);
}

std::vector<_TSTRING>& CFunSetPageDlg::AccessFunNames(void)
{
 return m_fun_names;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CFunSetPageDlg::GetValues(SECU3IO::FunSetPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::FunSetPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CFunSetPageDlg::SetValues(const SECU3IO::FunSetPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::FunSetPar));
 UpdateData(false); //copy data from variables to dialog
 UpdateLoadAxisUnits();
 UpdateDialogControls(this, TRUE);
}

void CFunSetPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(780));
}
