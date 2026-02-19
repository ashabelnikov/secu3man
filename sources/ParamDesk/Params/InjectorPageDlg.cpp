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

/** \file InjectorPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/dpiaware.h"
#include "common/mathhelpers.h"
#include "InjectorPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"
#include "ui-core/MsgBox.h"
#include "common/Calculations.h"

BEGIN_MESSAGE_MAP(CInjectorPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_CBN_SELCHANGE(IDC_PD_INJECTOR_INJCONFIG_COMBO, OnChangeDataInjCfg)
 ON_CBN_SELCHANGE(IDC_PD_INJECTOR_INJCONFIG_G_COMBO, OnChangeDataInjCfg_g)
 ON_CBN_SELCHANGE(IDC_PD_INJECTOR_SQUIRTNUM_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_INJECTOR_SQUIRTNUM_G_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_INJECTOR_ANGLESPEC_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_INJECTOR_ANGLESPEC_G_COMBO, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_CYLDISP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_FLOWRATE_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_FLOWRATE_G_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_TIMING_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_TIMING_G_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_TIMING_CRK_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_TIMING_CRK_G_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_FFFCONST_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_MINPW_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_MINPW_G_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_MAXPW_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_MAXPW_G_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_IPWLROC_D_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_IPWLROC_I_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_INJECTOR_USETIMINGMAP_CHECK, OnInjUseTimingMap)
 ON_BN_CLICKED(IDC_PD_INJECTOR_USETIMINGMAP_G_CHECK, OnInjUseTimingMap)
 ON_BN_CLICKED(IDC_PD_INJECTOR_USEADDCORRS_CHECK,OnChangeData) 
 ON_BN_CLICKED(IDC_PD_INJECTOR_USEAIRDEN_CHECK,OnChangeData) 
 ON_BN_CLICKED(IDC_PD_INJECTOR_USEDIFFPRESS_CHECK,OnChangeData) 
 ON_BN_CLICKED(IDC_PD_INJECTOR_SECINJROWSWT_CHECK,OnChangeData) 
 ON_BN_CLICKED(IDC_PD_INJECTOR_FSAFTERSTART_CHECK,OnChangeData) 

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_CYLDISP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_CYLDISP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_CYLDISP_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_CYLDISP_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_G_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_G_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_G_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_G_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MINPW_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MINPW_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MINPW_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MINPW_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MINPW_G_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MINPW_G_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MINPW_G_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MINPW_G_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MAXPW_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MAXPW_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MAXPW_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MAXPW_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MAXPW_G_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MAXPW_G_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MAXPW_G_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_MAXPW_G_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_EDIT,OnUpdateInjTiming)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_SPIN,OnUpdateInjTiming)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CAPTION,OnUpdateInjTiming)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_UNIT,OnUpdateInjTiming)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_G_EDIT,OnUpdateInjTiming_g)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_G_SPIN,OnUpdateInjTiming_g)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_G_CAPTION,OnUpdateInjTiming_g)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_G_UNIT,OnUpdateInjTiming_g)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CRK_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CRK_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CRK_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CRK_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CRK_G_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CRK_G_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CRK_G_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_TIMING_CRK_G_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FFFCONST_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FFFCONST_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FFFCONST_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_INJCONFIG_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_INJCONFIG_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_INJCONFIG_G_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_INJCONFIG_G_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_SQUIRTNUM_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_SQUIRTNUM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_SQUIRTNUM_G_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_SQUIRTNUM_G_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_USETIMINGMAP_CHECK,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_USETIMINGMAP_G_CHECK,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_USEADDCORRS_CHECK,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_USEAIRDEN_CHECK,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_USEDIFFPRESS_CHECK,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_SECINJROWSWT_CHECK,OnUpdateControlsSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FSAFTERSTART_CHECK,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_ANGLESPEC_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_ANGLESPEC_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_ANGLESPEC_G_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_ANGLESPEC_G_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_SECONDFUEL_GROUP,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_GENERAL_GROUP,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_IPWLROC_D_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_IPWLROC_D_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_IPWLROC_D_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_IPWLROC_D_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_IPWLROC_I_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_IPWLROC_I_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_IPWLROC_I_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_IPWLROC_I_UNIT,OnUpdateControls)
END_MESSAGE_MAP()

CInjectorPageDlg::CInjectorPageDlg()
: m_enabled(false)
, m_enable_secu3t_features(false)
, m_cyldisp_edit(CEditEx::MODE_FLOAT, true)
, m_fff_const_edit(CEditEx::MODE_FLOAT, true)
, m_ipwlroc_d_edit(CEditEx::MODE_FLOAT, true)
, m_ipwlroc_i_edit(CEditEx::MODE_FLOAT, true)
, m_ovf_msgbox(false)
, m_maf_ovf_msgbox(false)
, mp_scr(new CWndScroller)
, m_itmode(0)
{
 m_fuel_density[0] = 0.710f; //petrol density (0.710 g/cc)
 m_fuel_density[1] = 0.536f; //LPG density (0.536 g/cc)

 for(int i = 0; i < 2; ++i)
 {
  //controls
  m_flowrate_edit[i].SetMode(CEditEx::MODE_FLOAT);
  m_flowrate_edit[i].SetOwnDDV(true);
  m_inj_timing_edit[i].SetMode(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED);
  m_inj_timing_edit[i].SetOwnDDV(true);
  m_inj_timing_crk_edit[i].SetMode(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED);
  m_inj_timing_crk_edit[i].SetOwnDDV(true);
  m_min_pw_edit[i].SetMode(CEditEx::MODE_FLOAT);
  m_min_pw_edit[i].SetOwnDDV(true);
  m_max_pw_edit[i].SetMode(CEditEx::MODE_FLOAT);
  m_max_pw_edit[i].SetOwnDDV(true);
  //params
  m_params.inj_usetimingmap[i] = 0;
  m_params.inj_config[i] = SECU3IO::INJCFG_SIMULTANEOUS;
  m_params.inj_squirt_num[i] = 4;
  m_params.inj_flow_rate[i] = 200.0f;
  m_params.inj_sd_igl_const[i] = 0;
  m_params.inj_timing[i] = 0; 
  m_params.inj_timing_crk[i] = 0; 
  m_params.inj_anglespec[i] = 0;
  m_params.inj_min_pw[i] = 1.0f; 
  m_params.inj_maf_const[i] = 0;
  m_params.inj_max_pw[i] = 100.0f;
 }

 m_params.inj_cyl_disp = 0.375f;
 m_params.cyl_num = 4;
 m_params.fff_const = 16000.0f;

 m_params.inj_useairden = false;
 m_params.inj_useaddcorrs = false;
 m_params.inj_usediffpress = false;
 m_params.inj_secinjrowswt = false;
 m_params.inj_fsafterstart = false;
 
 m_params.mafload_const = 0;

 m_params.injpw_dec_speed = 0.4;
 m_params.injpw_inc_speed = 0.4;
}

CInjectorPageDlg::~CInjectorPageDlg()
{
 //empty
}

LPCTSTR CInjectorPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PD_INJECTOR_PAGE;
}

void CInjectorPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX,IDC_PD_INJECTOR_CYLDISP_EDIT, m_cyldisp_edit);
 DDX_Control(pDX,IDC_PD_INJECTOR_CYLDISP_SPIN, m_cyldisp_spin);

 DDX_Control(pDX,IDC_PD_INJECTOR_FLOWRATE_EDIT, m_flowrate_edit[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_FLOWRATE_SPIN, m_flowrate_spin[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_FLOWRATE_G_EDIT, m_flowrate_edit[1]);
 DDX_Control(pDX,IDC_PD_INJECTOR_FLOWRATE_G_SPIN, m_flowrate_spin[1]);

 DDX_Control(pDX,IDC_PD_INJECTOR_INJCONFIG_COMBO, m_injcfg_combo[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_INJCONFIG_G_COMBO, m_injcfg_combo[1]);

 DDX_Control(pDX,IDC_PD_INJECTOR_SQUIRTNUM_COMBO, m_sqrnum_combo[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_SQUIRTNUM_G_COMBO, m_sqrnum_combo[1]);

 DDX_Control(pDX,IDC_PD_INJECTOR_ANGLESPEC_COMBO, m_injas_combo[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_ANGLESPEC_G_COMBO, m_injas_combo[1]);

 DDX_Control(pDX,IDC_PD_INJECTOR_TIMING_EDIT, m_inj_timing_edit[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_TIMING_SPIN, m_inj_timing_spin[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_TIMING_G_EDIT, m_inj_timing_edit[1]);
 DDX_Control(pDX,IDC_PD_INJECTOR_TIMING_G_SPIN, m_inj_timing_spin[1]);

 DDX_Control(pDX,IDC_PD_INJECTOR_TIMING_CRK_EDIT, m_inj_timing_crk_edit[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_TIMING_CRK_SPIN, m_inj_timing_crk_spin[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_TIMING_CRK_G_EDIT, m_inj_timing_crk_edit[1]);
 DDX_Control(pDX,IDC_PD_INJECTOR_TIMING_CRK_G_SPIN, m_inj_timing_crk_spin[1]);

 DDX_Control(pDX,IDC_PD_INJECTOR_USETIMINGMAP_CHECK, m_inj_usetimingmap_check[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_USETIMINGMAP_G_CHECK, m_inj_usetimingmap_check[1]);
 DDX_Control(pDX,IDC_PD_INJECTOR_USEADDCORRS_CHECK, m_inj_useaddcorrs_check);
 DDX_Control(pDX,IDC_PD_INJECTOR_USEAIRDEN_CHECK, m_inj_useairden_check);
 DDX_Control(pDX,IDC_PD_INJECTOR_USEDIFFPRESS_CHECK, m_inj_usediffpress_check);
 DDX_Control(pDX,IDC_PD_INJECTOR_SECINJROWSWT_CHECK, m_inj_secinjrowswt_check);
 DDX_Control(pDX,IDC_PD_INJECTOR_FSAFTERSTART_CHECK, m_inj_fsafterstart_check);

 DDX_Control(pDX,IDC_PD_INJECTOR_FFFCONST_EDIT, m_fff_const_edit);
 DDX_Control(pDX,IDC_PD_INJECTOR_FFFCONST_SPIN, m_fff_const_spin);

 DDX_Control(pDX,IDC_PD_INJECTOR_MINPW_EDIT, m_min_pw_edit[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_MINPW_SPIN, m_min_pw_spin[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_MINPW_G_EDIT, m_min_pw_edit[1]);
 DDX_Control(pDX,IDC_PD_INJECTOR_MINPW_G_SPIN, m_min_pw_spin[1]);

 DDX_Control(pDX,IDC_PD_INJECTOR_MAXPW_EDIT, m_max_pw_edit[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_MAXPW_SPIN, m_max_pw_spin[0]);
 DDX_Control(pDX,IDC_PD_INJECTOR_MAXPW_G_EDIT, m_max_pw_edit[1]);
 DDX_Control(pDX,IDC_PD_INJECTOR_MAXPW_G_SPIN, m_max_pw_spin[1]);

 DDX_Control(pDX,IDC_PD_INJECTOR_IPWLROC_D_EDIT, m_ipwlroc_d_edit);
 DDX_Control(pDX,IDC_PD_INJECTOR_IPWLROC_D_SPIN, m_ipwlroc_d_spin);
 DDX_Control(pDX,IDC_PD_INJECTOR_IPWLROC_I_EDIT, m_ipwlroc_i_edit);
 DDX_Control(pDX,IDC_PD_INJECTOR_IPWLROC_I_SPIN, m_ipwlroc_i_spin);

 float engdisp = m_params.inj_cyl_disp * m_params.cyl_num; //convert cyl.disp. to eng.disp
 m_cyldisp_edit.DDX_Value(pDX, IDC_PD_INJECTOR_CYLDISP_EDIT, engdisp);
 m_params.inj_cyl_disp = engdisp / m_params.cyl_num; //convert eng.disp to cyl.disp

 m_flowrate_edit[0].DDX_Value(pDX, IDC_PD_INJECTOR_FLOWRATE_EDIT, m_params.inj_flow_rate[0]);
 m_flowrate_edit[1].DDX_Value(pDX, IDC_PD_INJECTOR_FLOWRATE_G_EDIT, m_params.inj_flow_rate[1]);

 float inj_timing = MathHelpers::InjTimValueTransform(m_itmode, m_params.inj_timing[0], 0);
 m_inj_timing_edit[0].DDX_Value(pDX, IDC_PD_INJECTOR_TIMING_EDIT, inj_timing);
 m_params.inj_timing[0] = MathHelpers::InjTimValueTransform(m_itmode, inj_timing, 1);

 inj_timing = MathHelpers::InjTimValueTransform(m_itmode, m_params.inj_timing[1], 0);
 m_inj_timing_edit[1].DDX_Value(pDX, IDC_PD_INJECTOR_TIMING_G_EDIT, inj_timing);
 m_params.inj_timing[1] = MathHelpers::InjTimValueTransform(m_itmode, inj_timing, 1);

 inj_timing = MathHelpers::InjTimValueTransform(m_itmode, m_params.inj_timing_crk[0], 0);
 m_inj_timing_crk_edit[0].DDX_Value(pDX, IDC_PD_INJECTOR_TIMING_CRK_EDIT, inj_timing);
 m_params.inj_timing_crk[0] = MathHelpers::InjTimValueTransform(m_itmode, inj_timing, 1);

 inj_timing = MathHelpers::InjTimValueTransform(m_itmode, m_params.inj_timing_crk[1], 0);
 m_inj_timing_crk_edit[1].DDX_Value(pDX, IDC_PD_INJECTOR_TIMING_CRK_G_EDIT, inj_timing);
 m_params.inj_timing_crk[1] = MathHelpers::InjTimValueTransform(m_itmode, inj_timing, 1);

 DDX_Check_bool(pDX, IDC_PD_INJECTOR_USETIMINGMAP_CHECK, m_params.inj_usetimingmap[0]);
 DDX_Check_bool(pDX, IDC_PD_INJECTOR_USETIMINGMAP_G_CHECK, m_params.inj_usetimingmap[1]);
 DDX_Check_bool(pDX, IDC_PD_INJECTOR_USEADDCORRS_CHECK, m_params.inj_useaddcorrs);
 DDX_Check_bool(pDX, IDC_PD_INJECTOR_USEAIRDEN_CHECK, m_params.inj_useairden);
 DDX_Check_bool(pDX, IDC_PD_INJECTOR_USEDIFFPRESS_CHECK, m_params.inj_usediffpress);
 DDX_Check_bool(pDX, IDC_PD_INJECTOR_SECINJROWSWT_CHECK, m_params.inj_secinjrowswt);
 DDX_Check_bool(pDX, IDC_PD_INJECTOR_FSAFTERSTART_CHECK, m_params.inj_fsafterstart);

 m_fff_const_edit.DDX_Value(pDX, IDC_PD_INJECTOR_FFFCONST_EDIT, m_params.fff_const);

 m_min_pw_edit[0].DDX_Value(pDX, IDC_PD_INJECTOR_MINPW_EDIT, m_params.inj_min_pw[0]);
 m_min_pw_edit[1].DDX_Value(pDX, IDC_PD_INJECTOR_MINPW_G_EDIT, m_params.inj_min_pw[1]);

 m_max_pw_edit[0].DDX_Value(pDX, IDC_PD_INJECTOR_MAXPW_EDIT, m_params.inj_max_pw[0]);
 m_max_pw_edit[1].DDX_Value(pDX, IDC_PD_INJECTOR_MAXPW_G_EDIT, m_params.inj_max_pw[1]);

 m_ipwlroc_d_edit.DDX_Value(pDX, IDC_PD_INJECTOR_IPWLROC_D_EDIT, m_params.injpw_dec_speed);
 m_ipwlroc_i_edit.DDX_Value(pDX, IDC_PD_INJECTOR_IPWLROC_I_EDIT, m_params.injpw_inc_speed);
}

void CInjectorPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CInjectorPageDlg::OnUpdateInjTiming(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (m_inj_usetimingmap_check[0].GetCheck()!=BST_CHECKED));
}

void CInjectorPageDlg::OnUpdateInjTiming_g(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (m_inj_usetimingmap_check[1].GetCheck()!=BST_CHECKED));
}

void CInjectorPageDlg::OnUpdateControlsSECU3i(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_enable_secu3t_features);
}

/////////////////////////////////////////////////////////////////////////////
// CInjectorPageDlg message handlers

BOOL CInjectorPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_cyldisp_spin.SetBuddy(&m_cyldisp_edit);
 m_cyldisp_edit.SetLimitText(6);
 m_cyldisp_edit.SetDecimalPlaces(4);
 m_cyldisp_spin.SetRangeAndDelta(0.01f, 8.0000f, 0.0001);
 m_cyldisp_edit.SetRange(0.01f, 8.0000f);

 float mi, ma;
 _GetITModeRange(mi, ma);

 for (int i = 0; i < 2; ++i)
 {
  m_flowrate_spin[i].SetBuddy(&m_flowrate_edit[i]);
  m_flowrate_edit[i].SetLimitText(6);
  m_flowrate_edit[i].SetDecimalPlaces(2);
  m_flowrate_spin[i].SetRangeAndDelta(50.00f, 1023.00f, 0.02f);
  m_flowrate_edit[i].SetRange(50.00f, 1023.00f);

  m_inj_timing_spin[i].SetBuddy(&m_inj_timing_edit[i]);
  m_inj_timing_edit[i].SetLimitText(4);
  m_inj_timing_edit[i].SetDecimalPlaces(0);
  m_inj_timing_spin[i].SetRangeAndDelta(mi, ma, 1.0);
  m_inj_timing_edit[i].SetRange(mi, ma);

  m_inj_timing_crk_spin[i].SetBuddy(&m_inj_timing_crk_edit[i]);
  m_inj_timing_crk_edit[i].SetLimitText(4);
  m_inj_timing_crk_edit[i].SetDecimalPlaces(0);
  m_inj_timing_crk_spin[i].SetRangeAndDelta(mi, ma, 1.0);
  m_inj_timing_crk_edit[i].SetRange(mi, ma);

  m_min_pw_spin[i].SetBuddy(&m_min_pw_edit[i]);
  m_min_pw_edit[i].SetLimitText(5);
  m_min_pw_edit[i].SetDecimalPlaces(2);
  m_min_pw_spin[i].SetRangeAndDelta(0.10f, 6.50f, 0.025f);
  m_min_pw_edit[i].SetRange(0.10f, 6.50f);

  m_max_pw_spin[i].SetBuddy(&m_max_pw_edit[i]);
  m_max_pw_edit[i].SetLimitText(6);
  m_max_pw_edit[i].SetDecimalPlaces(2);
  m_max_pw_spin[i].SetRangeAndDelta(0.0f, 100.0f, 0.1f);
  m_max_pw_edit[i].SetRange(0.0f, 100.0f);
 }

 m_fff_const_spin.SetBuddy(&m_fff_const_edit);
 m_fff_const_edit.SetLimitText(5);
 m_fff_const_edit.SetDecimalPlaces(0);
 m_fff_const_spin.SetRangeAndDelta(8000.0f, 32000.0f, 1.0);
 m_fff_const_edit.SetRange(8000.0f, 32000.0f);

 m_ipwlroc_d_spin.SetBuddy(&m_ipwlroc_d_edit);
 m_ipwlroc_d_edit.SetLimitText(6);
 m_ipwlroc_d_edit.SetDecimalPlaces(2);
 m_ipwlroc_d_spin.SetRangeAndDelta(0.01f, 10.0f, 0.01f);
 m_ipwlroc_d_edit.SetRange(0.01f, 10.0f);

 m_ipwlroc_i_spin.SetBuddy(&m_ipwlroc_i_edit);
 m_ipwlroc_i_edit.SetLimitText(6);
 m_ipwlroc_i_edit.SetDecimalPlaces(2);
 m_ipwlroc_i_spin.SetRangeAndDelta(0.01f, 10.0f, 0.01f);
 m_ipwlroc_i_edit.SetRange(0.01f, 10.0f);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_cyldisp_edit, MLL::GetString(IDS_PD_INJECTOR_CYLDISP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cyldisp_spin, MLL::GetString(IDS_PD_INJECTOR_CYLDISP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_inj_secinjrowswt_check, MLL::GetString(IDS_PD_INJECTOR_SECINJROWSWT_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_inj_useaddcorrs_check, MLL::GetString(IDS_PD_INJECTOR_USEADDCORRS_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_inj_useairden_check, MLL::GetString(IDS_PD_INJECTOR_USEAIRDEN_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_inj_usediffpress_check, MLL::GetString(IDS_PD_INJECTOR_USEDIFFPRESS_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fff_const_edit, MLL::GetString(IDS_PD_INJECTOR_FFFCONST_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fff_const_spin, MLL::GetString(IDS_PD_INJECTOR_FFFCONST_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_inj_fsafterstart_check, MLL::GetString(IDS_PD_INJECTOR_FSAFTERSTART_CHECK_TT)));

 for(int i = 0; i < 2; ++i)
 {
  VERIFY(mp_ttc->AddWindow(&m_sqrnum_combo[i], MLL::GetString(IDS_PD_INJECTOR_SQUIRTNUM_COMBO_TT)));
  VERIFY(mp_ttc->AddWindow(&m_injcfg_combo[i], MLL::GetString(IDS_PD_INJECTOR_INJCONFIG_COMBO_TT)));
  VERIFY(mp_ttc->AddWindow(&m_injas_combo[i], MLL::GetString(IDS_PD_INJECTOR_ANGLESPEC_COMBO_TT)));
  VERIFY(mp_ttc->AddWindow(&m_inj_timing_edit[i], MLL::GetString(IDS_PD_INJECTOR_TIMING_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_inj_timing_spin[i], MLL::GetString(IDS_PD_INJECTOR_TIMING_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_inj_timing_crk_edit[i], MLL::GetString(IDS_PD_INJECTOR_TIMING_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_inj_timing_crk_spin[i], MLL::GetString(IDS_PD_INJECTOR_TIMING_EDIT_TT)));
  //injector's flow rate
  VERIFY(mp_ttc->AddWindow(&m_flowrate_edit[i], MLL::GetString(IDS_PD_INJECTOR_FLOWRATE_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_flowrate_spin[i], MLL::GetString(IDS_PD_INJECTOR_FLOWRATE_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_min_pw_edit[i], MLL::GetString(IDS_PD_INJECTOR_MINPW_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_min_pw_spin[i], MLL::GetString(IDS_PD_INJECTOR_MINPW_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_max_pw_edit[i], MLL::GetString(IDS_PD_INJECTOR_MAXPW_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_max_pw_spin[i], MLL::GetString(IDS_PD_INJECTOR_MAXPW_EDIT_TT)));
  VERIFY(mp_ttc->AddWindow(&m_inj_usetimingmap_check[i], MLL::GetString(IDS_PD_INJECTOR_USETIMINGMAP_CHECK_TT)));
 }

 VERIFY(mp_ttc->AddWindow(&m_ipwlroc_d_edit, MLL::GetString(IDS_PD_INJECTOR_IPWLROC_D_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ipwlroc_d_spin, MLL::GetString(IDS_PD_INJECTOR_IPWLROC_D_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ipwlroc_i_edit, MLL::GetString(IDS_PD_INJECTOR_IPWLROC_I_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ipwlroc_i_spin, MLL::GetString(IDS_PD_INJECTOR_IPWLROC_I_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 for(int i = 0; i < 2; ++i)
 {
  _FillInjCfgComboBox(i);
  _FillSqrNumComboBox(i);
  _FillAngleSpecsComboBox(i);
 }

 //initialize window scroller
 mp_scr->Init(this);

 //Set bold font
 CloneWndFont(this, &m_boldDlgFont, -1, true);
 GetDlgItem(IDC_PD_INJECTOR_SECONDFUEL_GROUP)->SetFont(&m_boldDlgFont);
 GetDlgItem(IDC_PD_INJECTOR_GENERAL_GROUP)->SetFont(&m_boldDlgFont);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CInjectorPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CInjectorPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CInjectorPageDlg::OnChangeDataInjCfg()
{
 UpdateData();
 m_params.inj_config[0] = _GetInjCfgComboBoxSelection(0); //update inj_config, because it will be used in _FillSqrNumComboBox()

 //SqrNum combobox depends on injection config and cylinder number,
 //Also we have to preserve selection
 int sel = _GetSqrNumComboBoxSelection(0);
 _FillSqrNumComboBox(0);
 _SetSqrNumComboBoxSelection(sel, 0);

 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CInjectorPageDlg::OnChangeDataInjCfg_g()
{
 UpdateData();
 m_params.inj_config[1] = _GetInjCfgComboBoxSelection(1); //update inj_config, because it will be used in _FillSqrNumComboBox()

 //SqrNum combobox depends on injection config and cylinder number,
 //Also we have to preserve selection
 int sel = _GetSqrNumComboBoxSelection(1);
 _FillSqrNumComboBox(1);
 _SetSqrNumComboBoxSelection(sel, 1);

 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CInjectorPageDlg::OnInjUseTimingMap()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
 UpdateDialogControls(this,TRUE);
}

//разрешение/запрещение контроллов (всех поголовно)
void CInjectorPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CInjectorPageDlg::IsEnabled(void)
{
 return m_enabled;
}

void CInjectorPageDlg::EnableSECU3TItems(bool i_enable)
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
void CInjectorPageDlg::GetValues(SECU3IO::InjctrPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables

 for(int i = 0; i < 2; ++i)
 {
  m_params.inj_config[i] = _GetInjCfgComboBoxSelection(i);
  m_params.inj_squirt_num[i] = _GetSqrNumComboBoxSelection(i);
  m_params.inj_anglespec[i] = _GetAngleSpecsComboBoxSelection(i);

  //----------------------------------------------------------------------------
  //convert inj.flow rate from cc/min to g/min
  float mifr = m_params.inj_flow_rate[i] * m_fuel_density[i];

  int inj_num = m_params.cyl_num;
  int bnk_num = m_params.cyl_num;
  if (m_params.inj_config[i] == SECU3IO::INJCFG_THROTTLEBODY)
  {
   inj_num = 1; //single injector
   bnk_num = 1; //single bank
  }
  else if (m_params.inj_config[i] == SECU3IO::INJCFG_SIMULTANEOUS)
  {
   inj_num = m_params.cyl_num; //= N cylinders
   bnk_num = 1; //single bank
  }
  else if (m_params.inj_config[i] == SECU3IO::INJCFG_2BANK_ALTERN) //available only for even cylinder number engines
  {
   inj_num = m_params.cyl_num; //= N cylinders
   bnk_num = 2;                // 2 banks
  }
  else if (m_params.inj_config[i] == SECU3IO::INJCFG_SEMISEQUENTIAL) //available only for even cylinder number engines
  {
   inj_num = m_params.cyl_num; //= N cylinders
   bnk_num = m_params.cyl_num / 2;
  }
  else if (m_params.inj_config[i] == SECU3IO::INJCFG_SEMISEQSEPAR) //available only for even cylinder number engines
  {
   inj_num = m_params.cyl_num; //= N cylinders
   bnk_num = m_params.cyl_num / 2;
  }
  else if (m_params.inj_config[i] == SECU3IO::INGCFG_FULLSEQUENTIAL)
  {
   inj_num = m_params.cyl_num; //= N cylinders
   bnk_num = m_params.cyl_num; //= N cylinders
  }
  else
   ASSERT(0);

  //calculate constant (calculation of this value related to the ideal gas law, see firmware code for more information)
  m_params.inj_sd_igl_const[i] = ((m_params.inj_cyl_disp * 3.482f * 18750000.0f) / mifr) * ((float(bnk_num) * float(m_params.cyl_num)) / (float(inj_num) * float(m_params.inj_squirt_num[i])));
  //----------------------------------------------------------------------------

  if (m_params.inj_sd_igl_const[i] > 524288)
  {
   m_params.inj_sd_igl_const[i] = 524288;
   if (!m_ovf_msgbox)
   {
    m_ovf_msgbox = true;
    SECUMessageBox(MLL::LoadString(IDS_SD_IGL_OVERFLOW));
    m_ovf_msgbox = false;
   }
  }

  //calculate constant used for calculation of inj. PW using MAF.
  m_params.inj_maf_const[i] = (float)(((120.0 * 18750000.0) / mifr * (double(bnk_num) / (double(inj_num) * double(m_params.inj_squirt_num[i])))) / 64.0);
  if (m_params.inj_maf_const[i] > (103200*4))
  {
   m_params.inj_maf_const[i] = (103200*4);
   if (!m_maf_ovf_msgbox)
   {
    m_maf_ovf_msgbox = true;
    SECUMessageBox(MLL::LoadString(IDS_MAF_IGL_OVERFLOW));
    m_maf_ovf_msgbox = false;
   }
  }

 } //for

 //calculate constant used for calculation of MAF load (synthetic load)
 m_params.mafload_const = calc::calcMAFLoadConst(m_params.inj_cyl_disp, m_params.cyl_num);

 memcpy(o_values,&m_params, sizeof(SECU3IO::InjctrPar));
}

//Use this function to set data in dialog
void CInjectorPageDlg::SetValues(const SECU3IO::InjctrPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::InjctrPar));

 for (int i = 0; i < 2; ++i)
 {
  //Because its contents depends on number of cylinders
  _FillInjCfgComboBox(i);

  //Fill it here because its contents depend on selected injection configuration & cylinder number
  //Note, selection will be set by following calls
  _FillSqrNumComboBox(i);

  _SetInjCfgComboBoxSelection(m_params.inj_config[i], i);
  _SetSqrNumComboBoxSelection(m_params.inj_squirt_num[i], i);
  _SetAngleSpecsComboBoxSelection(m_params.inj_anglespec[i], i);
 }

 UpdateData(FALSE); //copy data from variables to dialog controls
}

void CInjectorPageDlg::_FillAngleSpecsComboBox(int fi)
{
 m_anglespecs.clear();
 m_anglespecs.push_back(std::make_pair(SECU3IO::INJANGLESPEC_BEGIN, MLL::GetString(IDS_INJ_ANGLESPEC_BEGIN)));
 m_anglespecs.push_back(std::make_pair(SECU3IO::INJANGLESPEC_MIDDLE, MLL::GetString(IDS_INJ_ANGLESPEC_MIDDLE)));
 m_anglespecs.push_back(std::make_pair(SECU3IO::INJANGLESPEC_END, MLL::GetString(IDS_INJ_ANGLESPEC_END)));

 m_injas_combo[fi].ResetContent();
 for(size_t i = 0; i < m_anglespecs.size(); i++)
 {
  int index = m_injas_combo[fi].AddString(m_anglespecs[i].second.c_str());
  if (index==CB_ERR)
  {
   ASSERT(0);
   continue;
  }
  m_injas_combo[fi].SetItemData(index, i);
 }
}

int CInjectorPageDlg::_GetAngleSpecsComboBoxSelection(int fi)
{
 int index = m_injas_combo[fi].GetCurSel();
 if (index==CB_ERR)
 {
  ASSERT(0);
  return 0;
 }
 int as_index = m_injas_combo[fi].GetItemData(index);
 return m_anglespecs[as_index].first;
}

void CInjectorPageDlg::_SetAngleSpecsComboBoxSelection(int i_sel, int fi)
{
 for(size_t i = 0; i < m_anglespecs.size(); i++)
 {
  if (m_anglespecs[i].first != i_sel) //find index in container for configuration
   continue;
  //find related index and select corresponding item
  int count = m_injas_combo[fi].GetCount();
  for(int ii = 0; ii < count; ii++)
  if (m_injas_combo[fi].GetItemData(ii) == i)
  {
   m_injas_combo[fi].SetCurSel(ii);
   return;
  }
 }
 ASSERT(0);
}

void CInjectorPageDlg::_FillInjCfgComboBox(int fi)
{
 m_injcfgs.clear();
 m_injcfgs.push_back(std::make_pair(SECU3IO::INJCFG_THROTTLEBODY, MLL::GetString(IDS_INJ_CFG_THROTTLEBODY)));
 m_injcfgs.push_back(std::make_pair(SECU3IO::INJCFG_SIMULTANEOUS, MLL::GetString(IDS_INJ_CFG_SIMULTANEOUSLY)));
 if (m_params.cyl_num != 2 && m_params.cyl_num != 1 && m_params.cyl_num != 3 && m_params.cyl_num != 5)
  m_injcfgs.push_back(std::make_pair(SECU3IO::INJCFG_2BANK_ALTERN, MLL::GetString(IDS_INJ_CFG_2BANK_ALTERN)));
 if (m_params.cyl_num != 1 && m_params.cyl_num != 3 && m_params.cyl_num != 5)
 {
  m_injcfgs.push_back(std::make_pair(SECU3IO::INJCFG_SEMISEQUENTIAL, MLL::GetString(IDS_INJ_CFG_SEMISEQUENTIAL)));
  m_injcfgs.push_back(std::make_pair(SECU3IO::INJCFG_SEMISEQSEPAR, MLL::GetString(IDS_INJ_CFG_SEMISEQSEPAR)));
 }
 m_injcfgs.push_back(std::make_pair(SECU3IO::INGCFG_FULLSEQUENTIAL, MLL::GetString(IDS_INJ_CFG_FULLSEQUENTIAL)));

 m_injcfg_combo[fi].ResetContent();
 for(size_t i = 0; i < m_injcfgs.size(); i++)
 {
  int index = m_injcfg_combo[fi].AddString(m_injcfgs[i].second.c_str());
  if (index==CB_ERR)
  {
   ASSERT(0);
   continue;
  }
  m_injcfg_combo[fi].SetItemData(index, i);
 }

 //prevent setting wrong configuration
 if (((m_params.cyl_num == 1 || m_params.cyl_num == 2 || m_params.cyl_num == 3 || m_params.cyl_num == 5) && (m_params.inj_config[fi] == SECU3IO::INJCFG_2BANK_ALTERN)) ||
     ((m_params.cyl_num == 1 || m_params.cyl_num == 3 || m_params.cyl_num == 5) && (m_params.inj_config[fi] == SECU3IO::INJCFG_SEMISEQUENTIAL || m_params.inj_config[fi] == SECU3IO::INJCFG_SEMISEQSEPAR)))
 {
  m_params.inj_config[fi] = SECU3IO::INJCFG_THROTTLEBODY;
  _SetInjCfgComboBoxSelection(SECU3IO::INJCFG_THROTTLEBODY, fi);
  OnChangeNotify();
 }
}

int  CInjectorPageDlg::_GetInjCfgComboBoxSelection(int fi)
{
 int index = m_injcfg_combo[fi].GetCurSel();
 if (index==CB_ERR)
 {
  ASSERT(0);
  return 0;
 }
 int cfg_index = m_injcfg_combo[fi].GetItemData(index);
 return m_injcfgs[cfg_index].first;
}

void CInjectorPageDlg::_SetInjCfgComboBoxSelection(int i_sel, int fi)
{
 for(size_t i = 0; i < m_injcfgs.size(); i++)
 {
  if (m_injcfgs[i].first != i_sel) //find index in conntainer for configuration
   continue;
  //find related index and select corresponding item
  int count = m_injcfg_combo[fi].GetCount();
  for(int ii = 0; ii < count; ii++)
  if (m_injcfg_combo[fi].GetItemData(ii) == i)
  {
   m_injcfg_combo[fi].SetCurSel(ii);
   return;
  }
 }
 ASSERT(0);
}

void CInjectorPageDlg::_FillSqrNumComboBox(int fi)
{
 m_sqrnum[fi].clear();

 //Fill squirts number list depending of selected configuration and number of engine cylinders
 if (m_params.inj_config[fi] == SECU3IO::INJCFG_THROTTLEBODY || m_params.inj_config[fi] == SECU3IO::INJCFG_SIMULTANEOUS)
 {
  switch(m_params.cyl_num)
  {
   case 1:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    break;
   case 2:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    break;
   case 3:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    m_sqrnum[fi].push_back(std::make_pair(3, _T("3")));
    break;
   case 4:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    m_sqrnum[fi].push_back(std::make_pair(4, _T("4")));
    break;
   case 5:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    m_sqrnum[fi].push_back(std::make_pair(5, _T("5")));
    break;
   case 6:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    m_sqrnum[fi].push_back(std::make_pair(3, _T("3")));
    m_sqrnum[fi].push_back(std::make_pair(6, _T("6")));
    break;
   case 8:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    m_sqrnum[fi].push_back(std::make_pair(4, _T("4")));
    m_sqrnum[fi].push_back(std::make_pair(8, _T("8")));
    break;
  }
 }
 else if (m_params.inj_config[fi] == SECU3IO::INJCFG_2BANK_ALTERN)
 {
  switch(m_params.cyl_num)
  {
   case 1:
    ASSERT(0); //not available on odd cyl. number engine
    break;
   case 2:
    ASSERT(0); //not applicable with 2 cylinder engine
    break;
   case 3:
    ASSERT(0); //not available on odd cyl. number engine
    break;
   case 4:     //for 4 cyl engine this mode is the same as semi-sequential
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    m_sqrnum[fi].push_back(std::make_pair(4, _T("4")));
    break;
   case 5:
    ASSERT(0); //not available on odd cyl. number engine
    break;
   case 6:
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    m_sqrnum[fi].push_back(std::make_pair(6, _T("6")));
    break;
   case 8:
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    m_sqrnum[fi].push_back(std::make_pair(4, _T("4")));
    m_sqrnum[fi].push_back(std::make_pair(8, _T("8")));
    break;
  }
 }
 else if (m_params.inj_config[fi] == SECU3IO::INJCFG_SEMISEQUENTIAL || m_params.inj_config[fi] == SECU3IO::INJCFG_SEMISEQSEPAR)
 {
  switch(m_params.cyl_num)
  {
   case 1:
    ASSERT(0); //not available on odd cyl. number engine
    break;
   case 2:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    break;
   case 3:
    ASSERT(0); //not available on odd cyl. number engine
    break;
   case 4:
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    m_sqrnum[fi].push_back(std::make_pair(4, _T("4")));
    break;
   case 5:
    ASSERT(0); //not available on odd cyl. number engine
    break;
   case 6:
    m_sqrnum[fi].push_back(std::make_pair(3, _T("3")));
    m_sqrnum[fi].push_back(std::make_pair(6, _T("6")));
    break;
   case 8:
    m_sqrnum[fi].push_back(std::make_pair(4, _T("4")));
    m_sqrnum[fi].push_back(std::make_pair(8, _T("8")));
    break;
  }
 }
 else if (m_params.inj_config[fi] == SECU3IO::INGCFG_FULLSEQUENTIAL)
 {
  switch(m_params.cyl_num)
  {
   case 1:
    m_sqrnum[fi].push_back(std::make_pair(1, _T("1")));
    break;
   case 2:
    m_sqrnum[fi].push_back(std::make_pair(2, _T("2")));
    break;
   case 3:
    m_sqrnum[fi].push_back(std::make_pair(3, _T("3")));
    break;
   case 4:
    m_sqrnum[fi].push_back(std::make_pair(4, _T("4")));
    break;
   case 5:
    m_sqrnum[fi].push_back(std::make_pair(5, _T("5")));
    break;
   case 6:
    m_sqrnum[fi].push_back(std::make_pair(6, _T("6")));
    break;
   case 8:
    m_sqrnum[fi].push_back(std::make_pair(8, _T("8")));
    break;
  }
 }
 else
 {
  ASSERT(0); //unknown configuration
 }

 m_sqrnum_combo[fi].ResetContent();
 for(size_t i = 0; i < m_sqrnum[fi].size(); i++)
 {
  int index = m_sqrnum_combo[fi].AddString(m_sqrnum[fi][i].second.c_str());
  if (index==CB_ERR)
  {
   ASSERT(0);
   continue;
  }
  m_sqrnum_combo[fi].SetItemData(index, i);
 }
}

int CInjectorPageDlg::_GetSqrNumComboBoxSelection(int fi)
{
 int index = m_sqrnum_combo[fi].GetCurSel();
 if (index==CB_ERR)
 {
  ASSERT(0);
  return 0;
 }
 int num_index = m_sqrnum_combo[fi].GetItemData(index);
 return m_sqrnum[fi][num_index].first;
}

void CInjectorPageDlg::_SetSqrNumComboBoxSelection(int i_sel, int fi)
{
 for(size_t i = 0; i < m_sqrnum[fi].size(); i++)
 {
  if (m_sqrnum[fi][i].first != i_sel) //find index in container
   continue;
  //find related index and select corresponding item
  int count = m_sqrnum_combo[fi].GetCount();
  for(int ii = 0; ii < count; ii++)
  if (m_sqrnum_combo[fi].GetItemData(ii) == i)
  {
   m_sqrnum_combo[fi].SetCurSel(ii);
   return;
  }
 }

 //if we are not able to find corresponding number then select middle number
 int count = m_sqrnum_combo[fi].GetCount();
 m_sqrnum_combo[fi].SetCurSel(count/2);
}

void CInjectorPageDlg::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(930));
}

void CInjectorPageDlg::SetITEdMode(int mode)
{
 m_itmode = mode;

 float mi, ma;
 _GetITModeRange(mi, ma);

 if (GetSafeHwnd())
 { 
  m_inj_timing_edit[0].SetLimitText(4);
  m_inj_timing_spin[0].SetRangeAndDelta(mi, ma, 1.0);
  m_inj_timing_edit[0].SetRange(mi, ma);

  m_inj_timing_edit[1].SetLimitText(4);
  m_inj_timing_spin[1].SetRangeAndDelta(mi, ma, 1.0);
  m_inj_timing_edit[1].SetRange(mi, ma);

  m_inj_timing_crk_edit[0].SetLimitText(4);
  m_inj_timing_crk_spin[0].SetRangeAndDelta(mi, ma, 1.0);
  m_inj_timing_crk_edit[0].SetRange(mi, ma);

  m_inj_timing_crk_edit[1].SetLimitText(4);
  m_inj_timing_crk_spin[1].SetRangeAndDelta(mi, ma, 1.0);
  m_inj_timing_crk_edit[1].SetRange(mi, ma);
  UpdateData(FALSE);
 }
}

void CInjectorPageDlg::_GetITModeRange(float& y1, float& y2)
{
 y1 = (m_itmode < 2) ? .0f : -360.0f;
 y2 = (m_itmode < 2) ? 720.0f : 360.0f;
}

void CInjectorPageDlg::SetFuelDensity(float fd1, float fd2)
{
 m_fuel_density[0] = fd1;
 m_fuel_density[1] = fd2;
}
