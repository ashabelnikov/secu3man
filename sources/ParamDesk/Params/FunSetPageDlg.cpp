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
#include "FunSetPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CFunSetPageDlg::IDD = IDD_PD_FUNSET_PAGE;

BEGIN_MESSAGE_MAP(CFunSetPageDlg, Super)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_BENZIN_MAPS_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_GAS_MAPS_COMBO, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_MAP_GRAD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_PRESS_SWING_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_OFFSET_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT, OnChangeData)

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
END_MESSAGE_MAP()

CFunSetPageDlg::CFunSetPageDlg(CWnd* pParent /*=NULL*/)
: Super(CFunSetPageDlg::IDD, pParent)
, m_enabled(false)
, m_map_grad_edit(CEditEx::MODE_FLOAT)
, m_press_swing_edit(CEditEx::MODE_FLOAT)
, m_map_curve_offset_edit(CEditEx::MODE_FLOAT)
, m_map_curve_gradient_edit(CEditEx::MODE_FLOAT)
, m_tps_curve_offset_edit(CEditEx::MODE_FLOAT)
, m_tps_curve_gradient_edit(CEditEx::MODE_FLOAT)
{
 m_params.map_lower_pressure = 4.5f;
 m_params.map_upper_pressure = 10.0f;
 m_params.fn_benzin = 0;
 m_params.fn_gas = 1;
 m_params.map_curve_offset = 0.547f;
 m_params.map_curve_gradient = 20.9f;
 m_params.tps_curve_offset = 0.4f; //(V)
 m_params.tps_curve_gradient = 25.64f; //(%/V)
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

 m_map_grad_edit.DDX_Value(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_params.map_lower_pressure);
 m_press_swing_edit.DDX_Value(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_params.map_upper_pressure);
 m_map_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_OFFSET_EDIT, m_params.map_curve_offset);
 m_map_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, m_params.map_curve_gradient);
 m_tps_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_TPS_CURVE_OFFSET_EDIT, m_params.tps_curve_offset);
 m_tps_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_TPS_CURVE_GRADIENT_EDIT, m_params.tps_curve_gradient);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_BENZIN_MAPS_COMBO, m_params.fn_benzin);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_GAS_MAPS_COMBO, m_params.fn_gas);
}

/////////////////////////////////////////////////////////////////////////////
// CFunSetPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CFunSetPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CFunSetPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_map_grad_spin.SetBuddy(&m_map_grad_edit);
 m_map_grad_edit.SetLimitText(4);
 m_map_grad_edit.SetDecimalPlaces(2);
 m_map_grad_spin.SetRangeAndDelta(0.25f,105.0f,0.25f);

 m_press_swing_spin.SetBuddy(&m_press_swing_edit);
 m_press_swing_edit.SetLimitText(4);
 m_press_swing_edit.SetDecimalPlaces(2);
 m_press_swing_spin.SetRangeAndDelta(0.25,500.0f,0.25f);

 m_map_curve_offset_spin.SetBuddy(&m_map_curve_offset_edit);
 m_map_curve_offset_edit.SetLimitText(5);
 m_map_curve_offset_edit.SetDecimalPlaces(3);
 m_map_curve_offset_spin.SetRangeAndDelta(-5.0f,5.0f,0.0025f);

 m_map_curve_gradient_spin.SetBuddy(&m_map_curve_gradient_edit);
 m_map_curve_gradient_edit.SetLimitText(5);
 m_map_curve_gradient_edit.SetDecimalPlaces(3);
 m_map_curve_gradient_spin.SetRangeAndDelta(-150.0f,150.0f,0.01f);

 m_tps_curve_offset_spin.SetBuddy(&m_tps_curve_offset_edit);
 m_tps_curve_offset_edit.SetLimitText(5);
 m_tps_curve_offset_edit.SetDecimalPlaces(3);
 m_tps_curve_offset_spin.SetRangeAndDelta(-5.0f, 5.0f, 0.0025f);

 m_tps_curve_gradient_spin.SetBuddy(&m_tps_curve_gradient_edit);
 m_tps_curve_gradient_edit.SetLimitText(5);
 m_tps_curve_gradient_edit.SetDecimalPlaces(3);
 m_tps_curve_gradient_spin.SetRangeAndDelta(.0f, 100.0f, 0.01f);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_map_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_curve_offset_edit, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_OFFSET_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tps_curve_offset_spin, MLL::GetString(IDS_PD_FUNSET_TPS_CURVE_OFFSET_EDIT_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 FillCBByFunNames(); //инициализируем контент ComboBox-ов семейств характеристик
 UpdateData(FALSE);  //инициализируем контроллы диалога данными
 UpdateDialogControls(this, TRUE);

 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CFunSetPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//разрешение/запрещение контроллов (всех поголовно)
void CFunSetPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CFunSetPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//TODO: Если будет необходимость использовать режим сортировки, то для корректного
//ассоциирования элементов можно использовать CComboBox::SetItemData. В любом случае
//должна измениться только внутренняя архитектура этого диалога

//заполняет ComboBox-ы имен семейств характеристик
void CFunSetPageDlg::FillCBByFunNames(void)
{
 if (!::IsWindow(m_hWnd))
  return;

 //удаляем предыдущий контент
 m_benzin_maps_combo.ResetContent();
 m_gas_maps_combo.ResetContent();

 //добавляем новый контент
 for(size_t i = 0; i < m_fun_names.size(); i++)
 {
  m_benzin_maps_combo.AddString(m_fun_names[i].c_str());
  m_gas_maps_combo.AddString(m_fun_names[i].c_str());
 }

 //после удаления всех элементов текущее выделение в ComboBox-ах
 //теряется и мы должны вернуть старое выделение.

 //для газа
 if (m_params.fn_gas < m_fun_names.size())
  m_gas_maps_combo.SetCurSel(m_params.fn_gas);
 else
  m_gas_maps_combo.SetCurSel(0);

 //для бензиа
 if (m_params.fn_benzin < m_fun_names.size())
  m_benzin_maps_combo.SetCurSel(m_params.fn_benzin);
 else
  m_benzin_maps_combo.SetCurSel(0);
}

std::vector<_TSTRING>& CFunSetPageDlg::AccessFunNames(void)
{
 return m_fun_names;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CFunSetPageDlg::GetValues(SECU3IO::FunSetPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::FunSetPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CFunSetPageDlg::SetValues(const SECU3IO::FunSetPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::FunSetPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
