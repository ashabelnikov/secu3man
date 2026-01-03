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

/** \file CESettingsDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <limits>
#include <afxpriv.h>
#include "resource.h"
#include "common/dpiaware.h"
#include "common/GDIHelpers.h"
#include "CESettingsDlg.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "ui-core/EditEx.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/tooltipctrlex.h"

#undef max   //avoid conflicts with C++

/////////////////////////////////////////////////////////////////////////////
// CCESettingsDlg dialog

const UINT CCESettingsDlg::IDD = IDD_CESETTINGS_EDITOR;

BEGIN_MESSAGE_MAP(CCESettingsDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()

 ON_BN_CLICKED(IDC_CESETT_MAP_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_VBAT_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_CTS_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_KS_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_TPS_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_ADD_I1_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_ADD_I2_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_ADD_I3_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_ADD_I4_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_ADD_I5_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_ADD_I6_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_ADD_I7_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_ADD_I8_V_USEEM_CHECK, OnEMCheck)
 ON_BN_CLICKED(IDC_CESETT_STEPPERIC_FLG_CHECK, OnStepperICFlgCheck)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_MAP_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_MAP_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_MAP_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_VBAT_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_VBAT_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_VBAT_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_CTS_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_CTS_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_CTS_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_KS_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_KS_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_KS_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_TPS_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_TPS_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_TPS_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I1_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I1_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I1_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I2_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I2_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I2_V_EM_CAPTION, OnUpdateEmValue)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_MIN_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_MIN_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_MAX_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_MAX_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_GROUP, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_MIN_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_MAX_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I3_V_USEEM_CHECK, OnUpdateSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_MIN_EDIT, OnUpdateSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_MIN_SPIN, OnUpdateSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_MAX_EDIT, OnUpdateSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_MAX_SPIN, OnUpdateSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_GROUP, OnUpdateSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_MIN_CAPTION, OnUpdateSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_MAX_CAPTION, OnUpdateSECU3iEx)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I4_V_USEEM_CHECK, OnUpdateSECU3iEx)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_MIN_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_MIN_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_MAX_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_MAX_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_GROUP, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_MIN_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_MAX_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I5_V_USEEM_CHECK, OnUpdateSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_MIN_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_MIN_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_MAX_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_MAX_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_GROUP, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_MIN_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_MAX_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I6_V_USEEM_CHECK, OnUpdateSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_MIN_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_MIN_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_MAX_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_MAX_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_GROUP, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_MIN_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_MAX_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I7_V_USEEM_CHECK, OnUpdateSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_MIN_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_MIN_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_MAX_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_MAX_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_EM_EDIT, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_EM_SPIN, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_GROUP, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_MIN_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_MAX_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_EM_CAPTION, OnUpdateEmValue)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_ADD_I8_V_USEEM_CHECK, OnUpdateSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_OILPRESS_GROUP, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_OILPRESS_THRD_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_OILPRESS_THRD_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_OILPRESS_THRD_CAPTION, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_OILPRESS_TIMER_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_OILPRESS_TIMER_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_OILPRESS_TIMER_CAPTION, OnUpdateSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_STEPPERIC_FLG_CHECK, OnUpdateSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_DBW_GROUP, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_TPSDIFF_THRD_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_TPSDIFF_THRD_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_TPSDIFF_THRD_CAPTION, OnUpdateSECU3i)

 ON_UPDATE_COMMAND_UI(IDC_CESETT_APPSDIFF_THRD_EDIT, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_APPSDIFF_THRD_SPIN, OnUpdateSECU3i)
 ON_UPDATE_COMMAND_UI(IDC_CESETT_APPSDIFF_THRD_CAPTION, OnUpdateSECU3i)
END_MESSAGE_MAP()

CCESettingsDlg::CCESettingsDlg(CWnd* pParent /*=NULL*/)
: Super(CCESettingsDlg::IDD, pParent)
, mp_scr(new CWndScroller)
, mp_data(new CESettingsData())
, m_enable_secu3t_features(false)
, m_enable_extraio(false)
, m_map_v_min_edit(CEditEx::MODE_FLOAT)
, m_map_v_max_edit(CEditEx::MODE_FLOAT)
, m_map_v_em_edit(CEditEx::MODE_FLOAT)
, m_vbat_v_min_edit(CEditEx::MODE_FLOAT)
, m_vbat_v_max_edit(CEditEx::MODE_FLOAT)
, m_vbat_v_em_edit(CEditEx::MODE_FLOAT)
, m_cts_v_min_edit(CEditEx::MODE_FLOAT)
, m_cts_v_max_edit(CEditEx::MODE_FLOAT)
, m_cts_v_em_edit(CEditEx::MODE_FLOAT)
, m_ks_v_min_edit(CEditEx::MODE_FLOAT)
, m_ks_v_max_edit(CEditEx::MODE_FLOAT)
, m_ks_v_em_edit(CEditEx::MODE_FLOAT)
, m_tps_v_min_edit(CEditEx::MODE_FLOAT)
, m_tps_v_max_edit(CEditEx::MODE_FLOAT)
, m_tps_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i1_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i1_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i1_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i2_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i2_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i2_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i3_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i3_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i3_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i4_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i4_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i4_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i5_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i5_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i5_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i6_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i6_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i6_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i7_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i7_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i7_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i8_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i8_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i8_v_em_edit(CEditEx::MODE_FLOAT)
, m_oilpress_thrd_edit(CEditEx::MODE_FLOAT)
, m_oilpress_timer_edit(CEditEx::MODE_INT)
, m_tpsdiff_thrd_edit(CEditEx::MODE_FLOAT)
, m_appsdiff_thrd_edit(CEditEx::MODE_FLOAT)
{
 //empty
}

CCESettingsDlg::~CCESettingsDlg()
{
 if (mp_data)
  delete mp_data;
}

void CCESettingsDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_CESETT_MAP_V_MIN_EDIT, m_map_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_MAP_V_MIN_SPIN, m_map_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_MAP_V_MAX_EDIT, m_map_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_MAP_V_MAX_SPIN, m_map_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_MAP_V_EM_EDIT, m_map_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_MAP_V_EM_SPIN, m_map_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_MAP_V_USEEM_CHECK, m_map_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_VBAT_V_MIN_EDIT, m_vbat_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_MIN_SPIN, m_vbat_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_MAX_EDIT, m_vbat_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_MAX_SPIN, m_vbat_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_EM_EDIT, m_vbat_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_EM_SPIN, m_vbat_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_USEEM_CHECK, m_vbat_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_CTS_V_MIN_EDIT, m_cts_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_CTS_V_MIN_SPIN, m_cts_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_CTS_V_MAX_EDIT, m_cts_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_CTS_V_MAX_SPIN, m_cts_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_CTS_V_EM_EDIT, m_cts_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_CTS_V_EM_SPIN, m_cts_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_CTS_V_USEEM_CHECK, m_cts_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_KS_V_MIN_EDIT, m_ks_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_KS_V_MIN_SPIN, m_ks_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_KS_V_MAX_EDIT, m_ks_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_KS_V_MAX_SPIN, m_ks_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_KS_V_EM_EDIT, m_ks_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_KS_V_EM_SPIN, m_ks_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_KS_V_USEEM_CHECK, m_ks_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_TPS_V_MIN_EDIT, m_tps_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_TPS_V_MIN_SPIN, m_tps_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_TPS_V_MAX_EDIT, m_tps_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_TPS_V_MAX_SPIN, m_tps_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_TPS_V_EM_EDIT, m_tps_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_TPS_V_EM_SPIN, m_tps_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_TPS_V_USEEM_CHECK, m_tps_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_MIN_EDIT, m_add_i1_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_MIN_SPIN, m_add_i1_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_MAX_EDIT, m_add_i1_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_MAX_SPIN, m_add_i1_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_EM_EDIT, m_add_i1_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_EM_SPIN, m_add_i1_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_USEEM_CHECK, m_add_i1_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_MIN_EDIT, m_add_i2_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_MIN_SPIN, m_add_i2_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_MAX_EDIT, m_add_i2_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_MAX_SPIN, m_add_i2_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_EM_EDIT, m_add_i2_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_EM_SPIN, m_add_i2_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_USEEM_CHECK, m_add_i2_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_ADD_I3_V_MIN_EDIT, m_add_i3_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I3_V_MIN_SPIN, m_add_i3_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I3_V_MAX_EDIT, m_add_i3_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I3_V_MAX_SPIN, m_add_i3_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I3_V_EM_EDIT, m_add_i3_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I3_V_EM_SPIN, m_add_i3_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I3_V_USEEM_CHECK, m_add_i3_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_ADD_I4_V_MIN_EDIT, m_add_i4_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I4_V_MIN_SPIN, m_add_i4_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I4_V_MAX_EDIT, m_add_i4_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I4_V_MAX_SPIN, m_add_i4_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I4_V_EM_EDIT, m_add_i4_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I4_V_EM_SPIN, m_add_i4_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I4_V_USEEM_CHECK, m_add_i4_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_ADD_I5_V_MIN_EDIT, m_add_i5_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I5_V_MIN_SPIN, m_add_i5_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I5_V_MAX_EDIT, m_add_i5_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I5_V_MAX_SPIN, m_add_i5_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I5_V_EM_EDIT, m_add_i5_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I5_V_EM_SPIN, m_add_i5_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I5_V_USEEM_CHECK, m_add_i5_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_ADD_I6_V_MIN_EDIT, m_add_i6_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I6_V_MIN_SPIN, m_add_i6_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I6_V_MAX_EDIT, m_add_i6_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I6_V_MAX_SPIN, m_add_i6_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I6_V_EM_EDIT, m_add_i6_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I6_V_EM_SPIN, m_add_i6_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I6_V_USEEM_CHECK, m_add_i6_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_ADD_I7_V_MIN_EDIT, m_add_i7_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I7_V_MIN_SPIN, m_add_i7_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I7_V_MAX_EDIT, m_add_i7_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I7_V_MAX_SPIN, m_add_i7_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I7_V_EM_EDIT, m_add_i7_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I7_V_EM_SPIN, m_add_i7_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I7_V_USEEM_CHECK, m_add_i7_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_ADD_I8_V_MIN_EDIT, m_add_i8_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I8_V_MIN_SPIN, m_add_i8_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I8_V_MAX_EDIT, m_add_i8_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I8_V_MAX_SPIN, m_add_i8_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I8_V_EM_EDIT, m_add_i8_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I8_V_EM_SPIN, m_add_i8_v_em_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I8_V_USEEM_CHECK, m_add_i8_v_useem_check);

 DDX_Control(pDX, IDC_CESETT_OILPRESS_THRD_EDIT, m_oilpress_thrd_edit);
 DDX_Control(pDX, IDC_CESETT_OILPRESS_THRD_SPIN, m_oilpress_thrd_spin);
 DDX_Control(pDX, IDC_CESETT_OILPRESS_TIMER_EDIT, m_oilpress_timer_edit);
 DDX_Control(pDX, IDC_CESETT_OILPRESS_TIMER_SPIN, m_oilpress_timer_spin);

 DDX_Control(pDX, IDC_CESETT_STEPPERIC_FLG_CHECK, m_stepperic_flg_check);

 DDX_Control(pDX, IDC_CESETT_TPSDIFF_THRD_EDIT, m_tpsdiff_thrd_edit);
 DDX_Control(pDX, IDC_CESETT_TPSDIFF_THRD_SPIN, m_tpsdiff_thrd_spin);

 DDX_Control(pDX, IDC_CESETT_APPSDIFF_THRD_EDIT, m_appsdiff_thrd_edit);
 DDX_Control(pDX, IDC_CESETT_APPSDIFF_THRD_SPIN, m_appsdiff_thrd_spin);

 DDX_Control(pDX, IDC_CESETT_ADDI5678_FLG_CHECK, m_addi5678_flg_check);

 //Do data exchange and validation
 m_map_v_min_edit.DDX_Value(pDX, IDC_CESETT_MAP_V_MIN_EDIT, mp_data->map_v_min);
 DDV_MinMaxFloat(pDX, mp_data->map_v_min, 0.0f, 5.5f);
 m_map_v_max_edit.DDX_Value(pDX, IDC_CESETT_MAP_V_MAX_EDIT, mp_data->map_v_max);
 DDV_MinMaxFloat(pDX, mp_data->map_v_max, 0.0f, 5.5f);
 m_map_v_em_edit.DDX_Value(pDX, IDC_CESETT_MAP_V_EM_EDIT, mp_data->map_v_em);
 DDV_MinMaxFloat(pDX, mp_data->map_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_MAP_V_USEEM_CHECK, mp_data->map_v_useem);

 m_vbat_v_min_edit.DDX_Value(pDX, IDC_CESETT_VBAT_V_MIN_EDIT, mp_data->vbat_v_min);
 DDV_MinMaxFloat(pDX, mp_data->vbat_v_min, 8.0f, 18.0f);
 m_vbat_v_max_edit.DDX_Value(pDX, IDC_CESETT_VBAT_V_MAX_EDIT, mp_data->vbat_v_max);
 DDV_MinMaxFloat(pDX, mp_data->vbat_v_max, 8.0f, 18.0f);
 m_vbat_v_em_edit.DDX_Value(pDX, IDC_CESETT_VBAT_V_EM_EDIT, mp_data->vbat_v_em);
 DDV_MinMaxFloat(pDX, mp_data->vbat_v_em, 8.0f, 18.0f);
 DDX_Check_bool(pDX, IDC_CESETT_VBAT_V_USEEM_CHECK, mp_data->vbat_v_useem);

 m_cts_v_min_edit.DDX_Value(pDX, IDC_CESETT_CTS_V_MIN_EDIT, mp_data->cts_v_min);
 DDV_MinMaxFloat(pDX, mp_data->cts_v_min, 0.0f, 5.5f);
 m_cts_v_max_edit.DDX_Value(pDX, IDC_CESETT_CTS_V_MAX_EDIT, mp_data->cts_v_max);
 DDV_MinMaxFloat(pDX, mp_data->cts_v_max, 0.0f, 5.5f);
 m_cts_v_em_edit.DDX_Value(pDX, IDC_CESETT_CTS_V_EM_EDIT, mp_data->cts_v_em);
 DDV_MinMaxFloat(pDX, mp_data->cts_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_CTS_V_USEEM_CHECK, mp_data->cts_v_useem);

 m_ks_v_min_edit.DDX_Value(pDX, IDC_CESETT_KS_V_MIN_EDIT, mp_data->ks_v_min);
 DDV_MinMaxFloat(pDX, mp_data->ks_v_min, 0.0f, 5.5f);
 m_ks_v_max_edit.DDX_Value(pDX, IDC_CESETT_KS_V_MAX_EDIT, mp_data->ks_v_max);
 DDV_MinMaxFloat(pDX, mp_data->ks_v_max, 0.0f, 5.5f);
 m_ks_v_em_edit.DDX_Value(pDX, IDC_CESETT_KS_V_EM_EDIT, mp_data->ks_v_em);
 DDV_MinMaxFloat(pDX, mp_data->ks_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_KS_V_USEEM_CHECK, mp_data->ks_v_useem);

 m_tps_v_min_edit.DDX_Value(pDX, IDC_CESETT_TPS_V_MIN_EDIT, mp_data->tps_v_min);
 DDV_MinMaxFloat(pDX, mp_data->tps_v_min, 0.0f, 5.5f);
 m_tps_v_max_edit.DDX_Value(pDX, IDC_CESETT_TPS_V_MAX_EDIT, mp_data->tps_v_max);
 DDV_MinMaxFloat(pDX, mp_data->tps_v_max, 0.0f, 5.5f);
 m_tps_v_em_edit.DDX_Value(pDX, IDC_CESETT_TPS_V_EM_EDIT, mp_data->tps_v_em);
 DDV_MinMaxFloat(pDX, mp_data->tps_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_TPS_V_USEEM_CHECK, mp_data->tps_v_useem);

 m_add_i1_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I1_V_MIN_EDIT, mp_data->add_i1_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_min, 0.0f, 5.5f);
 m_add_i1_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I1_V_MAX_EDIT, mp_data->add_i1_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_max, 0.0f, 5.5f);
 m_add_i1_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I1_V_EM_EDIT, mp_data->add_i1_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_ADD_I1_V_USEEM_CHECK, mp_data->add_i1_v_useem);

 m_add_i2_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I2_V_MIN_EDIT, mp_data->add_i2_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i2_v_min, 0.0f, 5.5f);
 m_add_i2_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I2_V_MAX_EDIT, mp_data->add_i2_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i2_v_max, 0.0f, 5.5f);
 m_add_i2_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I2_V_EM_EDIT, mp_data->add_i2_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i2_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_ADD_I2_V_USEEM_CHECK, mp_data->add_i2_v_useem);

 m_add_i3_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I3_V_MIN_EDIT, mp_data->add_i3_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i3_v_min, 0.0f, 5.5f);
 m_add_i3_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I3_V_MAX_EDIT, mp_data->add_i3_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i3_v_max, 0.0f, 5.5f);
 m_add_i3_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I3_V_EM_EDIT, mp_data->add_i3_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i3_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_ADD_I3_V_USEEM_CHECK, mp_data->add_i3_v_useem);

 m_add_i4_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I4_V_MIN_EDIT, mp_data->add_i4_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i4_v_min, 0.0f, 5.5f);
 m_add_i4_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I4_V_MAX_EDIT, mp_data->add_i4_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i4_v_max, 0.0f, 5.5f);
 m_add_i4_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I4_V_EM_EDIT, mp_data->add_i4_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i4_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_ADD_I4_V_USEEM_CHECK, mp_data->add_i4_v_useem);

 m_add_i5_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I5_V_MIN_EDIT, mp_data->add_i5_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i5_v_min, 0.0f, 5.5f);
 m_add_i5_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I5_V_MAX_EDIT, mp_data->add_i5_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i5_v_max, 0.0f, 5.5f);
 m_add_i5_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I5_V_EM_EDIT, mp_data->add_i5_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i5_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_ADD_I5_V_USEEM_CHECK, mp_data->add_i5_v_useem);

 m_add_i6_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I6_V_MIN_EDIT, mp_data->add_i6_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i6_v_min, 0.0f, 5.5f);
 m_add_i6_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I6_V_MAX_EDIT, mp_data->add_i6_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i6_v_max, 0.0f, 5.5f);
 m_add_i6_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I6_V_EM_EDIT, mp_data->add_i6_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i6_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_ADD_I6_V_USEEM_CHECK, mp_data->add_i6_v_useem);

 m_add_i7_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I7_V_MIN_EDIT, mp_data->add_i7_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i7_v_min, 0.0f, 5.5f);
 m_add_i7_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I7_V_MAX_EDIT, mp_data->add_i7_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i7_v_max, 0.0f, 5.5f);
 m_add_i7_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I7_V_EM_EDIT, mp_data->add_i7_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i7_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_ADD_I7_V_USEEM_CHECK, mp_data->add_i7_v_useem);

 m_add_i8_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I8_V_MIN_EDIT, mp_data->add_i8_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i8_v_min, 0.0f, 5.5f);
 m_add_i8_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I8_V_MAX_EDIT, mp_data->add_i8_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i8_v_max, 0.0f, 5.5f);
 m_add_i8_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I8_V_EM_EDIT, mp_data->add_i8_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i8_v_em, 0.0f, 5.5f);
 DDX_Check_bool(pDX, IDC_CESETT_ADD_I8_V_USEEM_CHECK, mp_data->add_i8_v_useem);

 m_oilpress_thrd_edit.DDX_Value(pDX, IDC_CESETT_OILPRESS_THRD_EDIT, mp_data->oilpress_thrd);
 DDV_MinMaxFloat(pDX, mp_data->oilpress_thrd, 0.0f, 8.0f);
 m_oilpress_timer_edit.DDX_Value(pDX, IDC_CESETT_OILPRESS_TIMER_EDIT, mp_data->oilpress_timer);
 DDV_MinMaxInt(pDX, mp_data->oilpress_timer, 0, 4000);

 DDX_Check_bool(pDX, IDC_CESETT_STEPPERIC_FLG_CHECK, mp_data->stepperic_flg);

 m_tpsdiff_thrd_edit.DDX_Value(pDX, IDC_CESETT_TPSDIFF_THRD_EDIT, mp_data->tpsdiff_thrd);
 DDV_MinMaxFloat(pDX, mp_data->tpsdiff_thrd, 0.0025f, 1.0f);

 m_appsdiff_thrd_edit.DDX_Value(pDX, IDC_CESETT_APPSDIFF_THRD_EDIT, mp_data->appsdiff_thrd);
 DDV_MinMaxFloat(pDX, mp_data->appsdiff_thrd, 0.0025f, 1.0f);

 DDX_Check_bool(pDX, IDC_CESETT_ADDI5678_FLG_CHECK, mp_data->addi5678_flg);
}

/////////////////////////////////////////////////////////////////////////////
// CCESettingsDlg message handlers

void CCESettingsDlg::OnUpdateSECU3i(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(!m_enable_secu3t_features);
}

void CCESettingsDlg::OnUpdateSECU3iEx(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable_extraio);
}

void CCESettingsDlg::OnUpdateEmValue(CCmdUI* pCmdUI)
{
 switch(pCmdUI->m_nID)
 {
  case IDC_CESETT_MAP_V_EM_EDIT:
  case IDC_CESETT_MAP_V_EM_SPIN:
  case IDC_CESETT_MAP_V_EM_CAPTION:
   pCmdUI->Enable(mp_data->map_v_useem);
   break;
  case IDC_CESETT_VBAT_V_EM_EDIT:
  case IDC_CESETT_VBAT_V_EM_SPIN:
  case IDC_CESETT_VBAT_V_EM_CAPTION:
   pCmdUI->Enable(mp_data->vbat_v_useem);
   break;
  case IDC_CESETT_CTS_V_EM_EDIT:
  case IDC_CESETT_CTS_V_EM_SPIN:
  case IDC_CESETT_CTS_V_EM_CAPTION:
   pCmdUI->Enable(mp_data->cts_v_useem);
   break;
  case IDC_CESETT_KS_V_EM_EDIT:
  case IDC_CESETT_KS_V_EM_SPIN:
  case IDC_CESETT_KS_V_EM_CAPTION:
   pCmdUI->Enable(mp_data->ks_v_useem);
   break;
  case IDC_CESETT_TPS_V_EM_EDIT:
  case IDC_CESETT_TPS_V_EM_SPIN:
  case IDC_CESETT_TPS_V_EM_CAPTION:
   pCmdUI->Enable(mp_data->tps_v_useem);
   break;
  case IDC_CESETT_ADD_I1_V_EM_EDIT:
  case IDC_CESETT_ADD_I1_V_EM_SPIN:
  case IDC_CESETT_ADD_I1_V_EM_CAPTION:
   pCmdUI->Enable(mp_data->add_i1_v_useem);
   break;
  case IDC_CESETT_ADD_I2_V_EM_EDIT:
  case IDC_CESETT_ADD_I2_V_EM_SPIN:
  case IDC_CESETT_ADD_I2_V_EM_CAPTION:
   pCmdUI->Enable(mp_data->add_i2_v_useem);
   break;
  case IDC_CESETT_ADD_I3_V_EM_EDIT:
  case IDC_CESETT_ADD_I3_V_EM_SPIN:
  case IDC_CESETT_ADD_I3_V_EM_CAPTION:
   pCmdUI->Enable(!m_enable_secu3t_features && mp_data->add_i3_v_useem);
   break;
  case IDC_CESETT_ADD_I4_V_EM_EDIT:
  case IDC_CESETT_ADD_I4_V_EM_SPIN:
  case IDC_CESETT_ADD_I4_V_EM_CAPTION:
   pCmdUI->Enable(m_enable_extraio && mp_data->add_i4_v_useem);
   break;
  case IDC_CESETT_ADD_I5_V_EM_EDIT:
  case IDC_CESETT_ADD_I5_V_EM_SPIN:
  case IDC_CESETT_ADD_I5_V_EM_CAPTION:
   pCmdUI->Enable(!m_enable_secu3t_features && mp_data->add_i5_v_useem);
   break;
  case IDC_CESETT_ADD_I6_V_EM_EDIT:
  case IDC_CESETT_ADD_I6_V_EM_SPIN:
  case IDC_CESETT_ADD_I6_V_EM_CAPTION:
   pCmdUI->Enable(!m_enable_secu3t_features && mp_data->add_i6_v_useem);
   break;
  case IDC_CESETT_ADD_I7_V_EM_EDIT:
  case IDC_CESETT_ADD_I7_V_EM_SPIN:
  case IDC_CESETT_ADD_I7_V_EM_CAPTION:
   pCmdUI->Enable(!m_enable_secu3t_features && mp_data->add_i7_v_useem);
   break;
  case IDC_CESETT_ADD_I8_V_EM_EDIT:
  case IDC_CESETT_ADD_I8_V_EM_SPIN:
  case IDC_CESETT_ADD_I8_V_EM_CAPTION:
   pCmdUI->Enable(!m_enable_secu3t_features && mp_data->add_i8_v_useem);
   break;
 }
}

void CCESettingsDlg::OnEMCheck()
{
 UpdateData(TRUE);
}

void CCESettingsDlg::OnStepperICFlgCheck()
{
 UpdateData(TRUE);
}

BOOL CCESettingsDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_map_v_min_spin.SetBuddy(&m_map_v_min_edit);
 m_map_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_map_v_min_edit.SetLimitText(6);
 m_map_v_min_edit.SetDecimalPlaces(3);
 m_map_v_max_spin.SetBuddy(&m_map_v_max_edit);
 m_map_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_map_v_max_edit.SetLimitText(6);
 m_map_v_max_edit.SetDecimalPlaces(3);
 m_map_v_em_spin.SetBuddy(&m_map_v_em_edit);
 m_map_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_map_v_em_edit.SetLimitText(6);
 m_map_v_em_edit.SetDecimalPlaces(3);

 m_vbat_v_min_spin.SetBuddy(&m_vbat_v_min_edit);
 m_vbat_v_min_spin.SetRangeAndDelta(8.0, 18.0, 0.01);
 m_vbat_v_min_edit.SetLimitText(6);
 m_vbat_v_min_edit.SetDecimalPlaces(3);
 m_vbat_v_max_spin.SetBuddy(&m_vbat_v_max_edit);
 m_vbat_v_max_spin.SetRangeAndDelta(8.0, 18.0, 0.01);
 m_vbat_v_max_edit.SetLimitText(6);
 m_vbat_v_max_edit.SetDecimalPlaces(3);
 m_vbat_v_em_spin.SetBuddy(&m_vbat_v_em_edit);
 m_vbat_v_em_spin.SetRangeAndDelta(8.0, 18.0, 0.01);
 m_vbat_v_em_edit.SetLimitText(6);
 m_vbat_v_em_edit.SetDecimalPlaces(3);

 m_cts_v_min_spin.SetBuddy(&m_cts_v_min_edit);
 m_cts_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_cts_v_min_edit.SetLimitText(6);
 m_cts_v_min_edit.SetDecimalPlaces(3);
 m_cts_v_max_spin.SetBuddy(&m_cts_v_max_edit);
 m_cts_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_cts_v_max_edit.SetLimitText(6);
 m_cts_v_max_edit.SetDecimalPlaces(3);
 m_cts_v_em_spin.SetBuddy(&m_cts_v_em_edit);
 m_cts_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_cts_v_em_edit.SetLimitText(6);
 m_cts_v_em_edit.SetDecimalPlaces(3);

 m_ks_v_min_spin.SetBuddy(&m_ks_v_min_edit);
 m_ks_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_ks_v_min_edit.SetLimitText(6);
 m_ks_v_min_edit.SetDecimalPlaces(3);
 m_ks_v_max_spin.SetBuddy(&m_ks_v_max_edit);
 m_ks_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_ks_v_max_edit.SetLimitText(6);
 m_ks_v_max_edit.SetDecimalPlaces(3);
 m_ks_v_em_spin.SetBuddy(&m_ks_v_em_edit);
 m_ks_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_ks_v_em_edit.SetLimitText(6);
 m_ks_v_em_edit.SetDecimalPlaces(3);

 m_tps_v_min_spin.SetBuddy(&m_tps_v_min_edit);
 m_tps_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_tps_v_min_edit.SetLimitText(6);
 m_tps_v_min_edit.SetDecimalPlaces(3);
 m_tps_v_max_spin.SetBuddy(&m_tps_v_max_edit);
 m_tps_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_tps_v_max_edit.SetLimitText(6);
 m_tps_v_max_edit.SetDecimalPlaces(3);
 m_tps_v_em_spin.SetBuddy(&m_tps_v_em_edit);
 m_tps_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_tps_v_em_edit.SetLimitText(6);
 m_tps_v_em_edit.SetDecimalPlaces(3);

 m_add_i1_v_min_spin.SetBuddy(&m_add_i1_v_min_edit);
 m_add_i1_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i1_v_min_edit.SetLimitText(6);
 m_add_i1_v_min_edit.SetDecimalPlaces(3);
 m_add_i1_v_max_spin.SetBuddy(&m_add_i1_v_max_edit);
 m_add_i1_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i1_v_max_edit.SetLimitText(6);
 m_add_i1_v_max_edit.SetDecimalPlaces(3);
 m_add_i1_v_em_spin.SetBuddy(&m_add_i1_v_em_edit);
 m_add_i1_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i1_v_em_edit.SetLimitText(6);
 m_add_i1_v_em_edit.SetDecimalPlaces(3);

 m_add_i2_v_min_spin.SetBuddy(&m_add_i2_v_min_edit);
 m_add_i2_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i2_v_min_edit.SetLimitText(6);
 m_add_i2_v_min_edit.SetDecimalPlaces(3);
 m_add_i2_v_max_spin.SetBuddy(&m_add_i2_v_max_edit);
 m_add_i2_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i2_v_max_edit.SetLimitText(6);
 m_add_i2_v_max_edit.SetDecimalPlaces(3);
 m_add_i2_v_em_spin.SetBuddy(&m_add_i2_v_em_edit);
 m_add_i2_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i2_v_em_edit.SetLimitText(6);
 m_add_i2_v_em_edit.SetDecimalPlaces(3);

 m_add_i3_v_min_spin.SetBuddy(&m_add_i3_v_min_edit);
 m_add_i3_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i3_v_min_edit.SetLimitText(6);
 m_add_i3_v_min_edit.SetDecimalPlaces(3);
 m_add_i3_v_max_spin.SetBuddy(&m_add_i3_v_max_edit);
 m_add_i3_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i3_v_max_edit.SetLimitText(6);
 m_add_i3_v_max_edit.SetDecimalPlaces(3);
 m_add_i3_v_em_spin.SetBuddy(&m_add_i3_v_em_edit);
 m_add_i3_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i3_v_em_edit.SetLimitText(6);
 m_add_i3_v_em_edit.SetDecimalPlaces(3);

 m_add_i4_v_min_spin.SetBuddy(&m_add_i4_v_min_edit);
 m_add_i4_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i4_v_min_edit.SetLimitText(6);
 m_add_i4_v_min_edit.SetDecimalPlaces(3);
 m_add_i4_v_max_spin.SetBuddy(&m_add_i4_v_max_edit);
 m_add_i4_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i4_v_max_edit.SetLimitText(6);
 m_add_i4_v_max_edit.SetDecimalPlaces(3);
 m_add_i4_v_em_spin.SetBuddy(&m_add_i4_v_em_edit);
 m_add_i4_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i4_v_em_edit.SetLimitText(6);
 m_add_i4_v_em_edit.SetDecimalPlaces(3);

 m_add_i5_v_min_spin.SetBuddy(&m_add_i5_v_min_edit);
 m_add_i5_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i5_v_min_edit.SetLimitText(6);
 m_add_i5_v_min_edit.SetDecimalPlaces(3);
 m_add_i5_v_max_spin.SetBuddy(&m_add_i5_v_max_edit);
 m_add_i5_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i5_v_max_edit.SetLimitText(6);
 m_add_i5_v_max_edit.SetDecimalPlaces(3);
 m_add_i5_v_em_spin.SetBuddy(&m_add_i5_v_em_edit);
 m_add_i5_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i5_v_em_edit.SetLimitText(6);
 m_add_i5_v_em_edit.SetDecimalPlaces(3);

 m_add_i6_v_min_spin.SetBuddy(&m_add_i6_v_min_edit);
 m_add_i6_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i6_v_min_edit.SetLimitText(6);
 m_add_i6_v_min_edit.SetDecimalPlaces(3);
 m_add_i6_v_max_spin.SetBuddy(&m_add_i6_v_max_edit);
 m_add_i6_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i6_v_max_edit.SetLimitText(6);
 m_add_i6_v_max_edit.SetDecimalPlaces(3);
 m_add_i6_v_em_spin.SetBuddy(&m_add_i6_v_em_edit);
 m_add_i6_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i6_v_em_edit.SetLimitText(6);
 m_add_i6_v_em_edit.SetDecimalPlaces(3);

 m_add_i7_v_min_spin.SetBuddy(&m_add_i7_v_min_edit);
 m_add_i7_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i7_v_min_edit.SetLimitText(6);
 m_add_i7_v_min_edit.SetDecimalPlaces(3);
 m_add_i7_v_max_spin.SetBuddy(&m_add_i7_v_max_edit);
 m_add_i7_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i7_v_max_edit.SetLimitText(6);
 m_add_i7_v_max_edit.SetDecimalPlaces(3);
 m_add_i7_v_em_spin.SetBuddy(&m_add_i7_v_em_edit);
 m_add_i7_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i7_v_em_edit.SetLimitText(6);
 m_add_i7_v_em_edit.SetDecimalPlaces(3);

 m_add_i8_v_min_spin.SetBuddy(&m_add_i8_v_min_edit);
 m_add_i8_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i8_v_min_edit.SetLimitText(6);
 m_add_i8_v_min_edit.SetDecimalPlaces(3);
 m_add_i8_v_max_spin.SetBuddy(&m_add_i8_v_max_edit);
 m_add_i8_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i8_v_max_edit.SetLimitText(6);
 m_add_i8_v_max_edit.SetDecimalPlaces(3);
 m_add_i8_v_em_spin.SetBuddy(&m_add_i8_v_em_edit);
 m_add_i8_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i8_v_em_edit.SetLimitText(6);
 m_add_i8_v_em_edit.SetDecimalPlaces(3);

 m_oilpress_thrd_spin.SetBuddy(&m_oilpress_thrd_edit);
 m_oilpress_thrd_spin.SetRangeAndDelta(0, 8.0, 0.01);
 m_oilpress_thrd_edit.SetLimitText(6);
 m_oilpress_thrd_edit.SetDecimalPlaces(2);
 m_oilpress_timer_spin.SetBuddy(&m_oilpress_timer_edit);
 m_oilpress_timer_spin.SetRangeAndDelta(0, 4000.0, 1.0);
 m_oilpress_timer_edit.SetLimitText(6);
 m_oilpress_timer_edit.SetDecimalPlaces(1);

 m_tpsdiff_thrd_spin.SetBuddy(&m_tpsdiff_thrd_edit);
 m_tpsdiff_thrd_spin.SetRangeAndDelta(0.0025, 1.0, 0.0025);
 m_tpsdiff_thrd_edit.SetLimitText(7);
 m_tpsdiff_thrd_edit.SetDecimalPlaces(4);

 m_appsdiff_thrd_spin.SetBuddy(&m_appsdiff_thrd_edit);
 m_appsdiff_thrd_spin.SetRangeAndDelta(0.0025, 1.0, 0.0025);
 m_appsdiff_thrd_edit.SetLimitText(7);
 m_appsdiff_thrd_edit.SetDecimalPlaces(4);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 VERIFY(mp_ttc->AddWindow(&m_map_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_vbat_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vbat_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vbat_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vbat_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vbat_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vbat_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vbat_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_cts_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cts_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cts_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cts_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cts_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cts_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cts_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ks_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ks_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ks_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ks_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ks_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ks_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ks_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_tps_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_add_i1_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i1_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i1_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i1_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i1_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i1_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i1_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_add_i2_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i2_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i2_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i2_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i2_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i2_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i2_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_add_i3_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i3_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i3_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i3_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i3_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i3_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i3_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_add_i4_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i4_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i4_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i4_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i4_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i4_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i4_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_add_i5_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i5_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i5_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i5_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i5_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i5_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i5_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_add_i6_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i6_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i6_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i6_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i6_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i6_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i6_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_add_i7_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i7_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i7_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i7_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i7_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i7_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i7_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_add_i8_v_min_edit, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i8_v_min_spin, MLL::GetString(IDS_CESETT_MAP_V_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i8_v_max_edit, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i8_v_max_spin, MLL::GetString(IDS_CESETT_MAP_V_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i8_v_em_edit, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i8_v_em_spin, MLL::GetString(IDS_CESETT_MAP_V_EM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_add_i8_v_useem_check, MLL::GetString(IDS_CESETT_MAP_V_USEEM_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_oilpress_thrd_edit, MLL::GetString(IDS_CESETT_OILPRESS_THRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_oilpress_thrd_spin, MLL::GetString(IDS_CESETT_OILPRESS_THRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_oilpress_timer_edit, MLL::GetString(IDS_CESETT_OILPRESS_TIMER_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_oilpress_timer_spin, MLL::GetString(IDS_CESETT_OILPRESS_TIMER_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_stepperic_flg_check, MLL::GetString(IDS_CESETT_STEPPERIC_FLG_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_tpsdiff_thrd_edit, MLL::GetString(IDS_CESETT_TPSDIFF_THRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tpsdiff_thrd_spin, MLL::GetString(IDS_CESETT_TPSDIFF_THRD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_appsdiff_thrd_edit, MLL::GetString(IDS_CESETT_APPSDIFF_THRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_appsdiff_thrd_spin, MLL::GetString(IDS_CESETT_APPSDIFF_THRD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_addi5678_flg_check, MLL::GetString(IDS_CESETT_ADDI5678_FLG_CHECK_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //initialize window scroller
 mp_scr->Init(this);
 _UpdateScrlViewSize();

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CCESettingsDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CCESettingsDlg::SetValues(const CESettingsData& i_data)
{
 if (mp_data)
  *mp_data = i_data;
}

void CCESettingsDlg::GetValues(CESettingsData& o_data)
{
 if (mp_data)
  o_data = *mp_data;
}

void CCESettingsDlg::EnableSECU3TItems(bool i_enable)
{
 m_enable_secu3t_features = i_enable;
}

void CCESettingsDlg::EnableExtraIO(bool i_enable)
{
 m_enable_extraio = i_enable;
}

void CCESettingsDlg::_UpdateScrlViewSize(void)
{
 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(da.ScaleX(475), da.ScaleY(990));
}

void CCESettingsDlg::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);
 _UpdateScrlViewSize();
}

void CCESettingsDlg::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after,x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

/////////////////////////////////////////////////////////////////////////////
// CCESettingsCntr dialog

const UINT CCESettingsCntr::IDD = IDD_CESETTINGS_CONTAINER;

BEGIN_MESSAGE_MAP(CCESettingsCntr, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_WM_GETMINMAXINFO()
 ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOkButton)
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()

CCESettingsCntr::CCESettingsCntr(CWnd* pParent /*=NULL*/)
: Super(CCESettingsCntr::IDD, pParent)
, m_wndPos(0, 0)
, m_wndSize(std::numeric_limits<int>::max(), std::numeric_limits<int>::max())
, m_initialized(false)
{
 //empty
}

CCESettingsCntr::~CCESettingsCntr()
{
 //empty
}

void CCESettingsCntr::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDOK, m_ok_btn);
 DDX_Control(pDX, IDCANCEL, m_cancel_btn);
}

/////////////////////////////////////////////////////////////////////////////
// CCESettingsCntr message handlers

void CCESettingsCntr::OnOK()
{
 bool valid = m_dlg.UpdateData(true);
 if (!valid)
  return;

 Super::OnOK();
}

void CCESettingsCntr::OnCancel()
{
 Super::OnCancel();
}

void CCESettingsCntr::OnUpdateOkButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(true);
}

BOOL CCESettingsCntr::OnInitDialog()
{
 Super::OnInitDialog();
 //create child dialog
 m_dlg.Create(CCESettingsDlg::IDD, this);
 m_dlg.SetPosition(0,0);
 m_dlg.ShowWindow(SW_SHOW);

 CRect rc;
 GetWindowRect(&rc);
 m_createSize = rc.Size();

 if (m_wndPos.x != std::numeric_limits<int>::max() && m_wndPos.y != std::numeric_limits<int>::max())
  SetWindowPos(NULL, m_wndPos.x, m_wndPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
 if (m_wndSize.cx != std::numeric_limits<int>::max() && m_wndSize.cy != std::numeric_limits<int>::max())
  SetWindowPos(NULL, 0, 0, m_wndSize.cx, m_wndSize.cy, SWP_NOMOVE | SWP_NOZORDER);

 m_initialized = true;

 GetClientRect(&rc);
 _UpdateControlsPosition(rc.Width(), rc.Height());

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CCESettingsCntr::OnDestroy()
{
 RECT rc;
 GetWindowRect(&rc);
 m_wndPos.x = rc.left;
 m_wndPos.y = rc.top;
 m_wndSize.cx = rc.right - rc.left;
 m_wndSize.cy = rc.bottom - rc.top;

 Super::OnDestroy();
}

LRESULT CCESettingsCntr::OnKickIdle(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
 m_dlg.UpdateDialogControls(this, true);
 return FALSE;
}

void CCESettingsCntr::SetValues(const CESettingsData& i_data)
{
 m_dlg.SetValues(i_data);
}

void CCESettingsCntr::GetValues(CESettingsData& o_data)
{
 m_dlg.GetValues(o_data);
}

void CCESettingsCntr::EnableSECU3TItems(bool i_enable)
{
 m_dlg.EnableSECU3TItems(i_enable);
}

void CCESettingsCntr::EnableExtraIO(bool i_enable)
{
 m_dlg.EnableExtraIO(i_enable);
}

void CCESettingsCntr::SetWndPosition(int x, int y)
{
 m_wndPos = CPoint(x, y);
}

const CPoint& CCESettingsCntr::GetWndPosition(void)
{
 return m_wndPos;
}

void CCESettingsCntr::SetWndSize(int cx, int cy)
{
 m_wndSize = CSize(cx, cy);
}

const CSize& CCESettingsCntr::GetWndSize(void)
{
 return m_wndSize;
}

void CCESettingsCntr::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 if (!m_initialized)
  return;
 _UpdateControlsPosition(cx, cy);
}

void CCESettingsCntr::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
 if (m_initialized)
 {
  lpMMI->ptMinTrackSize.x = m_createSize.cx;
  lpMMI->ptMinTrackSize.y = m_createSize.cy;

  lpMMI->ptMaxTrackSize.x = m_createSize.cx;
  lpMMI->ptMaxTrackSize.y = (LONG)(m_createSize.cy * 1.75f);
 }
}

void CCESettingsCntr::_UpdateControlsPosition(int cx, int cy)
{
 DPIAware dpi;
 //move OK button
 CRect rcok = GDIHelpers::GetChildWndRect(&m_ok_btn);
 rcok.MoveToY(cy - dpi.ScaleY(8) - rcok.Height());
 m_ok_btn.MoveWindow(rcok);

 //move CANCEL button
 CRect rccn = GDIHelpers::GetChildWndRect(&m_cancel_btn);
 rccn.MoveToY(cy - dpi.ScaleY(8) - rccn.Height());
 m_cancel_btn.MoveWindow(rccn);

 //resize child dialog
 CRect rcdl = GDIHelpers::GetChildWndRect(&m_dlg);
 rcdl.bottom = rcok.top - dpi.ScaleY(8);
 m_dlg.MoveWindow(&rcdl);
}

INT_PTR CCESettingsCntr::DoModal()
{
 INT_PTR result = Super::DoModal();
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}
