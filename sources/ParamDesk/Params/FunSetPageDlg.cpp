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
#include "FunSetPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"
#include "../MAPCalc/MAPCalcController.h"

const UINT CFunSetPageDlg::IDD = IDD_PD_FUNSET_PAGE;

BEGIN_MESSAGE_MAP(CFunSetPageDlg, Super)
 ON_WM_DESTROY()
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_BENZIN_MAPS_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_GAS_MAPS_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_LOAD_SRC_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_GAS_UNI_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_BENZIN_UNI_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_BAROCORRTYPE_COMBO, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_MAP_GRAD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_PRESS_SWING_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_OFFSET_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_MAP_CALC_BUTTON, OnMapCalcButton)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_UNIT,OnUpdateControls)

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
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_OFFSET_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_OFFSET_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_OFFSET_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_AXIS_PAR_GROUP,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_SENSOR_GROUP,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_TPS_SENSOR_GROUP,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_MAP_CALC_BUTTON, OnUpdateControls)
END_MESSAGE_MAP()

CFunSetPageDlg::CFunSetPageDlg(CWnd* pParent /*=NULL*/)
: Super(CFunSetPageDlg::IDD, pParent)
, m_enabled(false)
, m_map_grad_edit(CEditEx::MODE_FLOAT, true)
, m_press_swing_edit(CEditEx::MODE_FLOAT, true)
, m_map_curve_offset_edit(CEditEx::MODE_FLOAT, true)
, m_map_curve_gradient_edit(CEditEx::MODE_FLOAT, true)
, m_tps_curve_offset_edit(CEditEx::MODE_FLOAT, true)
, m_tps_curve_gradient_edit(CEditEx::MODE_FLOAT, true)
, mp_scr(new CWndScroller)
{
 m_params.map_lower_pressure = 4.5f;
 m_params.map_upper_pressure = 10.0f;
 m_params.fn_benzin = 0;
 m_params.fn_gas = 1;
 m_params.map_curve_offset = 0.547f;
 m_params.map_curve_gradient = 20.9f;
 m_params.tps_curve_offset = 0.4f; //(V)
 m_params.tps_curve_gradient = 25.64f; //(%/V)
 m_params.load_src_cfg = 0;
 m_params.uni_benzin = SECU3IO::UNI_OUTPUT_NUM; //disabled
 m_params.uni_gas = SECU3IO::UNI_OUTPUT_NUM; //disabled
 m_params.barocorr_type = 0; //disabled
}

LPCTSTR CFunSetPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
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
 DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_SPIN, m_press_swing_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_press_swing_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_SPIN, m_map_grad_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_map_grad_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET_EDIT, m_map_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET_SPIN, m_map_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, m_map_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_SPIN, m_map_curve_gradient_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT, m_tps_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_TPS_CURVE_OFFSET_SPIN, m_tps_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT, m_tps_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_TPS_CURVE_GRADIENT_SPIN, m_tps_curve_gradient_spin);
 DDX_Control(pDX, IDC_PD_MAP_CALC_BUTTON, m_calc_map_btn);

 m_map_grad_edit.DDX_Value(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_params.map_lower_pressure);
 m_press_swing_edit.DDX_Value(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_params.map_upper_pressure);
 m_map_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_OFFSET_EDIT, m_params.map_curve_offset);
 m_map_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, m_params.map_curve_gradient);
 m_tps_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT, m_params.tps_curve_offset);
 m_tps_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT, m_params.tps_curve_gradient);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_BENZIN_MAPS_COMBO, m_params.fn_benzin);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_GAS_MAPS_COMBO, m_params.fn_gas);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_LOAD_SRC_COMBO, m_params.load_src_cfg);
 DDX_CBIndex_int(pDX, IDC_PD_FUNSET_GAS_UNI_COMBO, m_params.uni_gas);
 DDX_CBIndex_int(pDX, IDC_PD_FUNSET_BENZIN_UNI_COMBO, m_params.uni_benzin);
 DDX_CBIndex_int(pDX, IDC_PD_FUNSET_BAROCORRTYPE_COMBO, m_params.barocorr_type);
}

/////////////////////////////////////////////////////////////////////////////
// CFunSetPageDlg message handlers

//Update controls
void CFunSetPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CFunSetPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_calc_map_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
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
 m_map_curve_gradient_edit.SetLimitText(5);
 m_map_curve_gradient_edit.SetDecimalPlaces(3);
 m_map_curve_gradient_spin.SetRangeAndDelta(-150.0f,150.0f,0.01f);
 m_map_curve_gradient_edit.SetRange(-150.0f,150.0f);

 m_tps_curve_offset_spin.SetBuddy(&m_tps_curve_offset_edit);
 m_tps_curve_offset_edit.SetLimitText(5);
 m_tps_curve_offset_edit.SetDecimalPlaces(3);
 m_tps_curve_offset_spin.SetRangeAndDelta(-5.0f, 5.0f, 0.0025f);
 m_tps_curve_offset_edit.SetRange(-5.0f, 5.0f);

 m_tps_curve_gradient_spin.SetBuddy(&m_tps_curve_gradient_edit);
 m_tps_curve_gradient_edit.SetLimitText(5);
 m_tps_curve_gradient_edit.SetDecimalPlaces(3);
 m_tps_curve_gradient_spin.SetRangeAndDelta(-100.0f, 100.0f, 0.01f);
 m_tps_curve_gradient_edit.SetRange(-100.0f, 100.0f);

 m_gas_uni_combo.AddString(_T("1"));
 m_gas_uni_combo.AddString(_T("2"));
 m_gas_uni_combo.AddString(_T("3"));
 m_gas_uni_combo.AddString(_T("--no--"));

 m_benzin_uni_combo.AddString(_T("1"));
 m_benzin_uni_combo.AddString(_T("2"));
 m_benzin_uni_combo.AddString(_T("3"));
 m_benzin_uni_combo.AddString(_T("--no--"));

 m_barocorr_type_combo.AddString(MLL::LoadString(IDS_PD_FUNSET_BAROCORR_TYPE0));
 m_barocorr_type_combo.AddString(MLL::LoadString(IDS_PD_FUNSET_BAROCORR_TYPE1));
 m_barocorr_type_combo.AddString(MLL::LoadString(IDS_PD_FUNSET_BAROCORR_TYPE2));
 m_barocorr_type_combo.AddString(MLL::LoadString(IDS_PD_FUNSET_BAROCORR_TYPE3));

 //initialize window scroller
 mp_scr->Init(this);
 mp_scr->SetViewSizeF(.0f, 1.5f);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_gradient_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_gradient_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_GRADIENT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_press_swing_edit, MLL::GetString(IDS_PD_FUNSET_PRESS_SWING_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_press_swing_spin, MLL::GetString(IDS_PD_FUNSET_PRESS_SWING_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_grad_edit, MLL::GetString(IDS_PD_FUNSET_MAP_GRAD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_grad_spin, MLL::GetString(IDS_PD_FUNSET_MAP_GRAD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_tps_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_curve_gradient_edit, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_curve_gradient_spin, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_calc_map_btn, MLL::GetString(IDS_PD_MAP_CALC_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gas_maps_combo, MLL::GetString(IDS_PD_FUNSET_GAS_MAPS_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_benzin_maps_combo, MLL::GetString(IDS_PD_FUNSET_BENZIN_MAPS_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_load_src_combo, MLL::GetString(IDS_PD_FUNSET_LOAD_SRC_COMBO_TT)));

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
}

void CFunSetPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
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

 m_load_src_combo.AddString(MLL::LoadString(IDS_PD_LOAD_OPT_MAP));
 m_load_src_combo.AddString(MLL::LoadString(IDS_PD_LOAD_OPT_TPS));

 //for gas
 if (m_params.load_src_cfg < 2)
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
}
