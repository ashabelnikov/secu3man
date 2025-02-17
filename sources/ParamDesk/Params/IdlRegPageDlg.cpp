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

/** \file IdlRegPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/dpiaware.h"
#include "IdlRegPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ToolTipCtrlEx.h"

BEGIN_MESSAGE_MAP(CIdlRegPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_EN_CHANGE(IDC_PD_IDLREG_DEAD_BAND_RPM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_FACTOR_NEG_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_FACTOR_POS_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_GOAL_RPM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_RESTRICTION_MIN_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_RESTRICTION_MAX_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_TURN_ON_TEMP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_IDLTORUNADD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_RPMONRUNADD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_PROPORTIONAL_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_INTEGRAL_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_PROPORTIONALM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_INTEGRALM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_DIFFERENTIAL_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_COEFFTHRD1_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_COEFFTHRD2_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_INTRPMLIM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_MAPVALUE_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_IACMINPOS_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_IACMAXPOS_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_IAC_DEADBAND_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_IRRKLOAD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_IRRKRPM_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_IDLREG_USE_REGULATOR, OnChangeData)
 ON_BN_CLICKED(IDC_PD_IDLREG_USE_ONGAS, OnChangeData)
 ON_BN_CLICKED(IDC_PD_IDLREG_USECLOSEDLOOP_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_IDLREG_PMODE_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_IDLREG_USECLIACONGAS_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_IDLREG_USETHRASSMAP_CHECK, OnChangeDataThrass)
 ON_BN_CLICKED(IDC_PD_IDLREG_WITH_IACREG_CHECK, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTORS_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_POS_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_POS_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_POS_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_NEG_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_NEG_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_NEG_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_GOAL_RPM_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_GOAL_RPM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_GOAL_RPM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_GOAL_RPM_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DEAD_BAND_RPM_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DEAD_BAND_RPM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DEAD_BAND_RPM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DEAD_BAND_RPM_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_USE_REGULATOR,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_USE_ONGAS,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PMODE_CHECK,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MIN_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MIN_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MIN_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MAX_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MAX_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MAX_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTIONS_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_TURN_ON_TEMP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_TURN_ON_TEMP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_TURN_ON_TEMP_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_WITH_IACREG_CHECK,OnUpdateFuelInjectionControls)

 //closed loop related:
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_USECLOSEDLOOP_CHECK,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IDLTORUNADD_EDIT,OnUpdateFuelInjectionControlsAdd)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IDLTORUNADD_SPIN,OnUpdateFuelInjectionControlsAdd)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IDLTORUNADD_CAPTION,OnUpdateFuelInjectionControlsAdd)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IDLTORUNADD_UNIT,OnUpdateFuelInjectionControlsAdd)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RPMONRUNADD_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RPMONRUNADD_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RPMONRUNADD_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RPMONRUNADD_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PROPORTIONAL_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PROPORTIONAL_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PROPORTIONAL_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PROPORTIONAL_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTEGRAL_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTEGRAL_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTEGRAL_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTEGRAL_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PROPORTIONALM_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PROPORTIONALM_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PROPORTIONALM_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_PROPORTIONALM_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTEGRALM_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTEGRALM_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTEGRALM_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTEGRALM_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DIFFERENTIAL_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DIFFERENTIAL_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DIFFERENTIAL_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DIFFERENTIAL_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_COEFFTHRD1_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_COEFFTHRD1_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_COEFFTHRD1_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_COEFFTHRD1_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_COEFFTHRD2_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_COEFFTHRD2_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_COEFFTHRD2_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_COEFFTHRD2_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTRPMLIM_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTRPMLIM_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTRPMLIM_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_INTRPMLIM_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_MAPVALUE_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_MAPVALUE_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_MAPVALUE_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_MAPVALUE_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IACMINPOS_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IACMINPOS_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IACMINPOS_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IACMINPOS_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IACMAXPOS_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IACMAXPOS_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IACMAXPOS_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IACMAXPOS_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_USECLIACONGAS_CHECK,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IAC_DEADBAND_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IAC_DEADBAND_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IAC_DEADBAND_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IAC_DEADBAND_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_USETHRASSMAP_CHECK,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IRRKLOAD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IRRKLOAD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IRRKLOAD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IRRKLOAD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IRRKRPM_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IRRKRPM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IRRKRPM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_IRRKRPM_UNIT,OnUpdateControls)
END_MESSAGE_MAP()

CIdlRegPageDlg::CIdlRegPageDlg()
: m_enabled(false)
, m_fuel_injection(false)
, m_factor_pos_edit(CEditEx::MODE_FLOAT, true)
, m_factor_neg_edit(CEditEx::MODE_FLOAT, true)
, m_restriction_min_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_restriction_max_edit(CEditEx::MODE_FLOAT, true)
, m_goal_rpm_edit(CEditEx::MODE_INT, true)
, m_dead_band_rpm_edit(CEditEx::MODE_INT, true)
, m_turn_on_temp_edit(CEditEx::MODE_FLOAT, true)
, m_idltorunadd_edit(CEditEx::MODE_FLOAT, true)
, m_rpmonrunadd_edit(CEditEx::MODE_INT, true)
, m_idlregp_edit(CEditEx::MODE_FLOAT, true)
, m_idlregi_edit(CEditEx::MODE_FLOAT, true)
, m_idlregpm_edit(CEditEx::MODE_FLOAT, true)
, m_idlregim_edit(CEditEx::MODE_FLOAT, true)
, m_idlregd_edit(CEditEx::MODE_FLOAT, true)
, m_coeffthrd1_edit(CEditEx::MODE_FLOAT, true)
, m_coeffthrd2_edit(CEditEx::MODE_FLOAT, true)
, m_intrpmlim_edit(CEditEx::MODE_INT, true)
, m_mapvalue_edit(CEditEx::MODE_FLOAT, true)
, m_iacminpos_edit(CEditEx::MODE_FLOAT, true)
, m_iacmaxpos_edit(CEditEx::MODE_FLOAT, true)
, m_iac_deadband_edit(CEditEx::MODE_INT, true)
, m_irrkload_edit(CEditEx::MODE_FLOAT, true)
, m_irrkrpm_edit(CEditEx::MODE_FLOAT, true)
, mp_scr(new CWndScroller)
{
 m_params.ifac1 = 1.0f;
 m_params.ifac2 = 1.0f;
 m_params.MINEFR = 10;
 m_params.idling_rpm = 850;
 m_params.idl_regul = false;
 m_params.use_regongas = true;
 m_params.preg_mode = false;
 m_params.min_angle = -15.0f;
 m_params.max_angle = 30.0f;
 m_params.turn_on_temp = 50.0f;
 //closed loop related:
 m_params.closed_loop = false;
 m_params.idl_to_run_add = 30.0f;
 m_params.rpm_on_run_add = 200;
 m_params.idl_reg_p[0] = 0.1f;   //-
 m_params.idl_reg_i[0] = 0.03f;  //-
 m_params.idl_reg_p[1] = 0.1f;   //+
 m_params.idl_reg_i[1] = 0.03f;  //+
 m_params.idl_coef_thrd1 = 1.50f;
 m_params.idl_coef_thrd2 = 1.86f;
 m_params.idl_intrpm_lim = 200;
 m_params.idl_map_value = 25.0f;
 m_params.idl_iacminpos = 10.0f;
 m_params.idl_iacmaxpos = 90.0f;
 m_params.idl_useiacclongas = true;
 m_params.iac_reg_db = 10;
 m_params.use_thrassmap = false;
 m_params.with_iacreg = false;
 m_params.idl_reg_d = 0.1f;
 m_params.irr_k_load = 2.0f;
 m_params.irr_k_rpm = 2.0f;
}

LPCTSTR CIdlRegPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PD_IDLREG_PAGE;
}

void CIdlRegPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_IDLREG_USE_REGULATOR, m_use_regulator);
 DDX_Control(pDX, IDC_PD_IDLREG_GOAL_RPM_SPIN, m_goal_rpm_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_GOAL_RPM_EDIT, m_goal_rpm_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_FACTOR_POS_SPIN, m_factor_pos_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_FACTOR_POS_EDIT, m_factor_pos_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_FACTOR_NEG_SPIN, m_factor_neg_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_FACTOR_NEG_EDIT, m_factor_neg_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_DEAD_BAND_RPM_SPIN, m_dead_band_rpm_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_DEAD_BAND_RPM_EDIT, m_dead_band_rpm_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_RESTRICTION_MIN_EDIT, m_restriction_min_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_RESTRICTION_MIN_SPIN, m_restriction_min_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_RESTRICTION_MAX_EDIT, m_restriction_max_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_RESTRICTION_MAX_SPIN, m_restriction_max_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_TURN_ON_TEMP_EDIT, m_turn_on_temp_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_TURN_ON_TEMP_SPIN, m_turn_on_temp_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_USE_ONGAS, m_use_regongas);
 DDX_Control(pDX, IDC_PD_IDLREG_PMODE_CHECK, m_preg_mode_check);
 DDX_Control(pDX, IDC_PD_IDLREG_USETHRASSMAP_CHECK, m_use_thrassmap_check);
 DDX_Control(pDX, IDC_PD_IDLREG_WITH_IACREG_CHECK, m_with_iacreg_check);
 //closed loop related:
 DDX_Control(pDX, IDC_PD_IDLREG_USECLOSEDLOOP_CHECK, m_use_closedloop);
 DDX_Control(pDX, IDC_PD_IDLREG_IDLTORUNADD_SPIN, m_idltorunadd_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_IDLTORUNADD_EDIT, m_idltorunadd_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_RPMONRUNADD_SPIN, m_rpmonrunadd_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_RPMONRUNADD_EDIT, m_rpmonrunadd_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_PROPORTIONAL_SPIN, m_idlregp_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_PROPORTIONAL_EDIT, m_idlregp_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_INTEGRAL_SPIN, m_idlregi_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_INTEGRAL_EDIT, m_idlregi_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_PROPORTIONALM_SPIN, m_idlregpm_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_PROPORTIONALM_EDIT, m_idlregpm_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_INTEGRALM_SPIN, m_idlregim_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_INTEGRALM_EDIT, m_idlregim_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_COEFFTHRD1_SPIN, m_coeffthrd1_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_COEFFTHRD1_EDIT, m_coeffthrd1_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_COEFFTHRD2_SPIN, m_coeffthrd2_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_COEFFTHRD2_EDIT, m_coeffthrd2_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_INTRPMLIM_SPIN, m_intrpmlim_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_INTRPMLIM_EDIT, m_intrpmlim_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_MAPVALUE_SPIN, m_mapvalue_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_MAPVALUE_EDIT, m_mapvalue_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_IACMINPOS_SPIN, m_iacminpos_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_IACMINPOS_EDIT, m_iacminpos_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_IACMAXPOS_SPIN, m_iacmaxpos_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_IACMAXPOS_EDIT, m_iacmaxpos_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_USECLIACONGAS_CHECK, m_use_claicongas_check);
 DDX_Control(pDX, IDC_PD_IDLREG_IAC_DEADBAND_SPIN, m_iac_deadband_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_IAC_DEADBAND_EDIT, m_iac_deadband_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_DIFFERENTIAL_SPIN, m_idlregd_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_DIFFERENTIAL_EDIT, m_idlregd_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_IRRKLOAD_SPIN, m_irrkload_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_IRRKLOAD_EDIT, m_irrkload_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_IRRKRPM_SPIN, m_irrkrpm_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_IRRKRPM_EDIT, m_irrkrpm_edit);

 m_factor_pos_edit.DDX_Value(pDX, IDC_PD_IDLREG_FACTOR_POS_EDIT, m_params.ifac1); //+
 m_factor_neg_edit.DDX_Value(pDX, IDC_PD_IDLREG_FACTOR_NEG_EDIT, m_params.ifac2); //-
 m_restriction_min_edit.DDX_Value(pDX, IDC_PD_IDLREG_RESTRICTION_MIN_EDIT, m_params.min_angle);
 m_restriction_max_edit.DDX_Value(pDX, IDC_PD_IDLREG_RESTRICTION_MAX_EDIT, m_params.max_angle);
 m_goal_rpm_edit.DDX_Value(pDX, IDC_PD_IDLREG_GOAL_RPM_EDIT, m_params.idling_rpm);
 m_dead_band_rpm_edit.DDX_Value(pDX, IDC_PD_IDLREG_DEAD_BAND_RPM_EDIT, m_params.MINEFR);
 m_turn_on_temp_edit.DDX_Value(pDX, IDC_PD_IDLREG_TURN_ON_TEMP_EDIT, m_params.turn_on_temp);
 DDX_Check_bool(pDX, IDC_PD_IDLREG_USE_REGULATOR, m_params.idl_regul);
 DDX_Check_bool(pDX, IDC_PD_IDLREG_USE_ONGAS, m_params.use_regongas);
 DDX_Check_bool(pDX, IDC_PD_IDLREG_PMODE_CHECK, m_params.preg_mode);
 DDX_Check_bool(pDX, IDC_PD_IDLREG_WITH_IACREG_CHECK, m_params.with_iacreg);

 //closed loop related:
 DDX_Check_bool(pDX, IDC_PD_IDLREG_USECLOSEDLOOP_CHECK, m_params.closed_loop);
 m_idltorunadd_edit.DDX_Value(pDX, IDC_PD_IDLREG_IDLTORUNADD_EDIT, m_params.idl_to_run_add);
 m_rpmonrunadd_edit.DDX_Value(pDX, IDC_PD_IDLREG_RPMONRUNADD_EDIT, m_params.rpm_on_run_add);
 m_idlregp_edit.DDX_Value(pDX, IDC_PD_IDLREG_PROPORTIONAL_EDIT, m_params.idl_reg_p[1]);   //+
 m_idlregi_edit.DDX_Value(pDX, IDC_PD_IDLREG_INTEGRAL_EDIT, m_params.idl_reg_i[1]);       //+
 m_idlregpm_edit.DDX_Value(pDX, IDC_PD_IDLREG_PROPORTIONALM_EDIT, m_params.idl_reg_p[0]); //-
 m_idlregim_edit.DDX_Value(pDX, IDC_PD_IDLREG_INTEGRALM_EDIT, m_params.idl_reg_i[0]);     //-
 m_idlregd_edit.DDX_Value(pDX, IDC_PD_IDLREG_DIFFERENTIAL_EDIT, m_params.idl_reg_d);
 m_coeffthrd1_edit.DDX_Value(pDX, IDC_PD_IDLREG_COEFFTHRD1_EDIT, m_params.idl_coef_thrd1);
 m_coeffthrd2_edit.DDX_Value(pDX, IDC_PD_IDLREG_COEFFTHRD2_EDIT, m_params.idl_coef_thrd2);
 m_intrpmlim_edit.DDX_Value(pDX, IDC_PD_IDLREG_INTRPMLIM_EDIT, m_params.idl_intrpm_lim);
 m_mapvalue_edit.DDX_Value(pDX, IDC_PD_IDLREG_MAPVALUE_EDIT, m_params.idl_map_value);
 m_iacminpos_edit.DDX_Value(pDX, IDC_PD_IDLREG_IACMINPOS_EDIT, m_params.idl_iacminpos);
 m_iacmaxpos_edit.DDX_Value(pDX, IDC_PD_IDLREG_IACMAXPOS_EDIT, m_params.idl_iacmaxpos);
 DDX_Check_bool(pDX, IDC_PD_IDLREG_USECLIACONGAS_CHECK, m_params.idl_useiacclongas);
 m_iac_deadband_edit.DDX_Value(pDX, IDC_PD_IDLREG_IAC_DEADBAND_EDIT, m_params.iac_reg_db);
 DDX_Check_bool(pDX, IDC_PD_IDLREG_USETHRASSMAP_CHECK, m_params.use_thrassmap);
 m_irrkload_edit.DDX_Value(pDX, IDC_PD_IDLREG_IRRKLOAD_EDIT, m_params.irr_k_load);
 m_irrkrpm_edit.DDX_Value(pDX, IDC_PD_IDLREG_IRRKRPM_EDIT, m_params.irr_k_rpm);
}

/////////////////////////////////////////////////////////////////////////////
// CIdlRegPageDlg message handlers

void CIdlRegPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CIdlRegPageDlg::OnUpdateFuelInjectionControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_fuel_injection);
}

void CIdlRegPageDlg::OnUpdateFuelInjectionControlsAdd(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_fuel_injection && false==m_params.use_thrassmap);
}

BOOL CIdlRegPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_factor_pos_spin.SetBuddy(&m_factor_pos_edit);
 m_factor_pos_edit.SetLimitText(5);
 m_factor_pos_edit.SetDecimalPlaces(2);
 m_factor_pos_spin.SetRangeAndDelta(0.0f,10.0f,0.01f);
 m_factor_pos_edit.SetRange(0.0f,10.0f);

 m_factor_neg_spin.SetBuddy(&m_factor_neg_edit);
 m_factor_neg_edit.SetLimitText(5);
 m_factor_neg_edit.SetDecimalPlaces(2);
 m_factor_neg_spin.SetRangeAndDelta(0.0f,10.0f,0.01f);
 m_factor_neg_edit.SetRange(0.0f,10.0f);

 m_dead_band_rpm_edit.SetLimitText(3);
 m_dead_band_rpm_edit.SetDecimalPlaces(3);
 m_dead_band_rpm_spin.SetBuddy(&m_dead_band_rpm_edit);
 m_dead_band_rpm_spin.SetRangeAndDelta(0,500,1);
 m_dead_band_rpm_edit.SetRange(0,500);

 m_goal_rpm_edit.SetLimitText(4);
 m_goal_rpm_spin.SetBuddy(&m_goal_rpm_edit);
 m_goal_rpm_spin.SetRangeAndDelta(0,1800,5);
 m_goal_rpm_edit.SetRange(0,1800);

 m_restriction_min_spin.SetBuddy(&m_restriction_min_edit);
 m_restriction_min_edit.SetLimitText(5);
 m_restriction_min_edit.SetDecimalPlaces(2);
 m_restriction_min_spin.SetRangeAndDelta(-15.0f,30.0f,0.025f);
 m_restriction_min_edit.SetRange(-15.0f,30.0f);

 m_restriction_max_spin.SetBuddy(&m_restriction_max_edit);
 m_restriction_max_edit.SetLimitText(5);
 m_restriction_max_edit.SetDecimalPlaces(2);
 m_restriction_max_spin.SetRangeAndDelta(-15.0f,30.0f,0.025f);
 m_restriction_max_edit.SetRange(-15.0f,30.0f);

 m_turn_on_temp_spin.SetBuddy(&m_turn_on_temp_edit);
 m_turn_on_temp_edit.SetLimitText(7);
 m_turn_on_temp_edit.SetDecimalPlaces(2);
 m_turn_on_temp_spin.SetRangeAndDelta(-40.0f,300.0f,0.2f);
 m_turn_on_temp_edit.SetRange(-40.0f,300.0f);

 //closed loop related
 m_idltorunadd_spin.SetBuddy(&m_idltorunadd_edit);
 m_idltorunadd_edit.SetLimitText(5);
 m_idltorunadd_edit.SetDecimalPlaces(1);
 m_idltorunadd_spin.SetRangeAndDelta(0.0f,100.0f,0.5f);
 m_idltorunadd_edit.SetRange(0.0f,100.0f);

 m_rpmonrunadd_spin.SetBuddy(&m_rpmonrunadd_edit);
 m_rpmonrunadd_edit.SetLimitText(4);
 m_rpmonrunadd_spin.SetRangeAndDelta(0,2000,10);
 m_rpmonrunadd_edit.SetRange(0,2000);

 m_idlregp_spin.SetBuddy(&m_idlregp_edit);
 m_idlregp_edit.SetLimitText(5);
 m_idlregp_edit.SetDecimalPlaces(3);
 m_idlregp_spin.SetRangeAndDelta(0.0f,5.0f,0.005f);
 m_idlregp_edit.SetRange(0.0f,5.0f);

 m_idlregi_spin.SetBuddy(&m_idlregi_edit);
 m_idlregi_edit.SetLimitText(5);
 m_idlregi_edit.SetDecimalPlaces(3);
 m_idlregi_spin.SetRangeAndDelta(0.0f,5.0f,0.005f);
 m_idlregi_edit.SetRange(0.0f,5.0f);

 m_idlregpm_spin.SetBuddy(&m_idlregpm_edit);
 m_idlregpm_edit.SetLimitText(5);
 m_idlregpm_edit.SetDecimalPlaces(3);
 m_idlregpm_spin.SetRangeAndDelta(0.0f,5.0f,0.005f);
 m_idlregpm_edit.SetRange(0.0f,5.0f);

 m_idlregim_spin.SetBuddy(&m_idlregim_edit);
 m_idlregim_edit.SetLimitText(5);
 m_idlregim_edit.SetDecimalPlaces(3);
 m_idlregim_spin.SetRangeAndDelta(0.0f,5.0f,0.005f);
 m_idlregim_edit.SetRange(0.0f,5.0f);

 m_coeffthrd1_spin.SetBuddy(&m_coeffthrd1_edit);
 m_coeffthrd1_edit.SetLimitText(4);
 m_coeffthrd1_edit.SetDecimalPlaces(2);
 m_coeffthrd1_spin.SetRangeAndDelta(1.0f,2.99f,0.01f);
 m_coeffthrd1_edit.SetRange(1.0f,2.99f);

 m_coeffthrd2_spin.SetBuddy(&m_coeffthrd2_edit);
 m_coeffthrd2_edit.SetLimitText(4);
 m_coeffthrd2_edit.SetDecimalPlaces(2);
 m_coeffthrd2_spin.SetRangeAndDelta(1.0f,2.99f,0.01f);
 m_coeffthrd2_edit.SetRange(1.0f,2.99f);

 m_intrpmlim_spin.SetBuddy(&m_intrpmlim_edit);
 m_intrpmlim_edit.SetLimitText(4);
 m_intrpmlim_spin.SetRangeAndDelta(10,1200,10);
 m_intrpmlim_edit.SetRange(0,1200);

 m_mapvalue_spin.SetBuddy(&m_mapvalue_edit);
 m_mapvalue_edit.SetLimitText(5);
 m_mapvalue_edit.SetDecimalPlaces(1);
 m_mapvalue_spin.SetRangeAndDelta(0.0f,100.0f,0.1f);
 m_mapvalue_edit.SetRange(0.0f,100.0f);

 m_iacminpos_spin.SetBuddy(&m_iacminpos_edit);
 m_iacminpos_edit.SetLimitText(5);
 m_iacminpos_edit.SetDecimalPlaces(1);
 m_iacminpos_spin.SetRangeAndDelta(0.0f,100.0f,0.5f);
 m_iacminpos_edit.SetRange(0.0f,100.0f);

 m_iacmaxpos_spin.SetBuddy(&m_iacmaxpos_edit);
 m_iacmaxpos_edit.SetLimitText(5);
 m_iacmaxpos_edit.SetDecimalPlaces(1);
 m_iacmaxpos_spin.SetRangeAndDelta(0.0f,100.0f,0.5f);
 m_iacmaxpos_edit.SetRange(0.0f,100.0f);

 m_iac_deadband_edit.SetLimitText(3);
 m_iac_deadband_edit.SetDecimalPlaces(3);
 m_iac_deadband_spin.SetBuddy(&m_iac_deadband_edit);
 m_iac_deadband_spin.SetRangeAndDelta(0,500,1);
 m_iac_deadband_edit.SetRange(0,500);

 m_idlregd_spin.SetBuddy(&m_idlregd_edit);
 m_idlregd_edit.SetLimitText(5);
 m_idlregd_edit.SetDecimalPlaces(3);
 m_idlregd_spin.SetRangeAndDelta(0.0f,5.0f,0.005f);
 m_idlregd_edit.SetRange(0.0f,5.0f);

 m_irrkload_spin.SetBuddy(&m_irrkload_edit);
 m_irrkload_edit.SetLimitText(6);
 m_irrkload_edit.SetDecimalPlaces(2);
 m_irrkload_spin.SetRangeAndDelta(0.0f,48.0f,0.05f);
 m_irrkload_edit.SetRange(0.0f,48.0f);

 m_irrkrpm_spin.SetBuddy(&m_irrkrpm_edit);
 m_irrkrpm_edit.SetLimitText(6);
 m_irrkrpm_edit.SetDecimalPlaces(2);
 m_irrkrpm_spin.SetRangeAndDelta(0.0f,48.0f,0.05f);
 m_irrkrpm_edit.SetRange(0.0f,48.0f);

 UpdateData(FALSE);

 //initialize window scroller
 mp_scr->Init(this);

 //Create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 VERIFY(mp_ttc->AddWindow(&m_goal_rpm_edit, MLL::GetString(IDS_PD_IDLREG_GOAL_RPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_goal_rpm_spin, MLL::GetString(IDS_PD_IDLREG_GOAL_RPM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_factor_neg_edit, MLL::GetString(IDS_PD_IDLREG_FACTORM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_factor_neg_spin, MLL::GetString(IDS_PD_IDLREG_FACTORM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_factor_pos_edit, MLL::GetString(IDS_PD_IDLREG_FACTOR_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_factor_pos_spin, MLL::GetString(IDS_PD_IDLREG_FACTOR_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_dead_band_rpm_edit, MLL::GetString(IDS_PD_IDLREG_DEAD_BAND_RPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_dead_band_rpm_spin, MLL::GetString(IDS_PD_IDLREG_DEAD_BAND_RPM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_restriction_min_edit, MLL::GetString(IDS_PD_IDLREG_RESTRICTION_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_restriction_min_spin, MLL::GetString(IDS_PD_IDLREG_RESTRICTION_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_restriction_max_edit, MLL::GetString(IDS_PD_IDLREG_RESTRICTION_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_restriction_max_spin, MLL::GetString(IDS_PD_IDLREG_RESTRICTION_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_turn_on_temp_edit, MLL::GetString(IDS_PD_IDLREG_TURN_ON_TEMP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_turn_on_temp_spin, MLL::GetString(IDS_PD_IDLREG_TURN_ON_TEMP_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_use_regulator, MLL::GetString(IDS_PD_IDLREG_USE_REGULATOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_use_regongas, MLL::GetString(IDS_PD_IDLREG_USE_ONGAS_TT)));

 VERIFY(mp_ttc->AddWindow(&m_use_claicongas_check, MLL::GetString(IDS_PD_IDLREG_USECLIACONGAS_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_mapvalue_edit, MLL::GetString(IDC_PD_IDLREG_MAPVALUE_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_mapvalue_spin, MLL::GetString(IDC_PD_IDLREG_MAPVALUE_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_intrpmlim_edit, MLL::GetString(IDS_PD_IDLREG_INTRPMLIM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_intrpmlim_spin, MLL::GetString(IDS_PD_IDLREG_INTRPMLIM_EDIT_TT)));
 
 VERIFY(mp_ttc->AddWindow(&m_preg_mode_check, MLL::GetString(IDS_PD_IDLREG_PMODE_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_use_closedloop, MLL::GetString(IDS_PD_IDLREG_USECLOSEDLOOP_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_idltorunadd_edit, MLL::GetString(IDS_PD_IDLREG_IDLTORUNADD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_idltorunadd_spin, MLL::GetString(IDS_PD_IDLREG_IDLTORUNADD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_rpmonrunadd_edit, MLL::GetString(IDS_PD_IDLREG_RPMONRUNADD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rpmonrunadd_spin, MLL::GetString(IDS_PD_IDLREG_RPMONRUNADD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_idlregp_edit, MLL::GetString(IDS_PD_IDLREG_PROPORTIONAL_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_idlregp_spin, MLL::GetString(IDS_PD_IDLREG_PROPORTIONAL_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_idlregi_edit, MLL::GetString(IDS_PD_IDLREG_INTEGRAL_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_idlregi_spin, MLL::GetString(IDS_PD_IDLREG_INTEGRAL_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_idlregpm_edit, MLL::GetString(IDS_PD_IDLREG_PROPORTIONALM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_idlregpm_spin, MLL::GetString(IDS_PD_IDLREG_PROPORTIONALM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_idlregim_edit, MLL::GetString(IDS_PD_IDLREG_INTEGRALM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_idlregim_spin, MLL::GetString(IDS_PD_IDLREG_INTEGRALM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_coeffthrd1_edit, MLL::GetString(IDS_PD_IDLREG_COEFFTHRD1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_coeffthrd1_spin, MLL::GetString(IDS_PD_IDLREG_COEFFTHRD1_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_coeffthrd2_edit, MLL::GetString(IDS_PD_IDLREG_COEFFTHRD2_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_coeffthrd2_spin, MLL::GetString(IDS_PD_IDLREG_COEFFTHRD2_EDIT_TT)));
 
 VERIFY(mp_ttc->AddWindow(&m_iacminpos_edit, MLL::GetString(IDS_PD_IDLREG_IACMINPOS_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_iacminpos_spin, MLL::GetString(IDS_PD_IDLREG_IACMINPOS_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_iacmaxpos_edit, MLL::GetString(IDS_PD_IDLREG_IACMAXPOS_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_iacmaxpos_spin, MLL::GetString(IDS_PD_IDLREG_IACMAXPOS_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_iac_deadband_edit, MLL::GetString(IDS_PD_IDLREG_IAC_DEADBAND_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_iac_deadband_spin, MLL::GetString(IDS_PD_IDLREG_IAC_DEADBAND_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_use_thrassmap_check, MLL::GetString(IDS_PD_IDLREG_USETHRASSMAP_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_with_iacreg_check, MLL::GetString(IDS_PD_IDLREG_WITH_IACREG_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_idlregd_edit, MLL::GetString(IDS_PD_IDLREG_DIFFERENTIAL_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_idlregd_spin, MLL::GetString(IDS_PD_IDLREG_DIFFERENTIAL_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_irrkload_edit, MLL::GetString(IDS_PD_IDLREG_IRRKLOAD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_irrkload_spin, MLL::GetString(IDS_PD_IDLREG_IRRKLOAD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_irrkrpm_edit, MLL::GetString(IDS_PD_IDLREG_IRRKRPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_irrkrpm_spin, MLL::GetString(IDS_PD_IDLREG_IRRKRPM_EDIT_TT)));
      
 mp_ttc->SetMaxTipWidth(250); //Set text wrapping width
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CIdlRegPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CIdlRegPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CIdlRegPageDlg::OnChangeDataThrass()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
 UpdateDialogControls(this, TRUE);
}

//Enable/disable all controls
void CIdlRegPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//get state of all controls (enabled or disabled)
bool CIdlRegPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CIdlRegPageDlg::GetValues(SECU3IO::IdlRegPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::IdlRegPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CIdlRegPageDlg::SetValues(const SECU3IO::IdlRegPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::IdlRegPar));
 UpdateData(FALSE); //copy data from variables to dialog
}

void CIdlRegPageDlg::EnableFuelInjection(bool i_enable)
{
 if (m_fuel_injection == i_enable)
  return; //already has needed state
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CIdlRegPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(990));
}
