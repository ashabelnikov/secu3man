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

/** \file CKPSPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "CKPSPageDlg.h"

#include <algorithm>
#include <map>
#include <math.h>
#include "ui-core/ToolTipCtrlEx.h"
#include "common/MathHelpers.h"
#include "ui-core/ddx_helpers.h"

const UINT CCKPSPageDlg::IDD = IDD_PD_CKPS_PAGE;

BEGIN_MESSAGE_MAP(CCKPSPageDlg, Super)
 ON_CBN_SELCHANGE(IDC_PD_CKPS_COGS_BEFORE_TDC_COMBOBOX, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_CKPS_ENGINE_CYL_COMBOBOX, OnChangeDataCylNum)
 ON_EN_CHANGE(IDC_PD_CKPS_IGNITION_COGS_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CKPS_HALL_WND_WIDTH_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CKPS_COGS_NUM_EDIT, OnChangeDataCogsNum)
 ON_EN_CHANGE(IDC_PD_CKPS_MISS_NUM_EDIT, OnChangeDataCogsNum)

 ON_BN_CLICKED(IDC_PD_CKPS_MERGE_IGN_OUTPUTS, OnChangeMergeOutputs)
 ON_BN_CLICKED(IDC_PD_CKPS_POSFRONT_RADIOBOX, OnClickedPdPosFrontRadio)
 ON_BN_CLICKED(IDC_PD_CKPS_NEGFRONT_RADIOBOX, OnClickedPdNegFrontRadio)
 ON_BN_CLICKED(IDC_PD_REF_S_POSFRONT_RADIOBOX, OnClickedPdPosFrontRadio2)
 ON_BN_CLICKED(IDC_PD_REF_S_NEGFRONT_RADIOBOX, OnClickedPdNegFrontRadio2)

 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_FRONT_GROUPBOX, OnUpdateCKPSFront)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_POSFRONT_RADIOBOX, OnUpdateCKPSFront)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_NEGFRONT_RADIOBOX, OnUpdateCKPSFront)
 ON_UPDATE_COMMAND_UI(IDC_PD_REF_S_FRONT_GROUPBOX, OnUpdateControls_REF_S_Front)
 ON_UPDATE_COMMAND_UI(IDC_PD_REF_S_POSFRONT_RADIOBOX, OnUpdateControls_REF_S_Front)
 ON_UPDATE_COMMAND_UI(IDC_PD_REF_S_NEGFRONT_RADIOBOX, OnUpdateControls_REF_S_Front)

 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_COGS_BEFORE_TDC_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_COGS_BEFORE_TDC_COMBOBOX, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_ENGINE_CYL_CAPTION, OnUpdateCylNumber)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_ENGINE_CYL_COMBOBOX, OnUpdateCylNumber)

 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_IGNITION_COGS_SPIN, OnUpdateIgnitionCogs)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_IGNITION_COGS_EDIT, OnUpdateIgnitionCogs)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_IGNITION_COGS_UNIT, OnUpdateIgnitionCogs)

 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_HALL_WND_WIDTH_SPIN, OnUpdateHallWndWidth)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_HALL_WND_WIDTH_EDIT, OnUpdateHallWndWidth)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_HALL_WND_WIDTH_UNIT, OnUpdateHallWndWidth)

 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_COGS_NUM_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_COGS_NUM_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_COGS_NUM_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_MISS_NUM_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_MISS_NUM_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_MISS_NUM_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_MERGE_IGN_OUTPUTS, OnUpdateMergeOutputs)
END_MESSAGE_MAP()

CCKPSPageDlg::CCKPSPageDlg(CWnd* pParent /*=NULL*/)
: Super(CCKPSPageDlg::IDD, pParent)
, m_ignition_cogs_edit(CEditEx::MODE_INT, true)
, m_hall_wnd_width_edit(CEditEx::MODE_FLOAT, true)
, m_wheel_cogs_num_edit(CEditEx::MODE_INT, true)
, m_wheel_miss_num_edit(CEditEx::MODE_INT, true)
, m_enabled(false)
, m_igncogs_enabled(false)
, m_odd_cylnum_enabled(false)
, m_ckps_enabled(false)
, m_inpmrg_enabled(false)
, m_max_cylinders(8)
{
 m_params.ckps_cogs_btdc = 20;
 m_params.ckps_edge_type = 0;
 m_params.ref_s_edge_type = 0;
 m_params.ckps_ignit_cogs = 20;
 m_params.ckps_merge_ign_outs = 0;
 m_params.ckps_cogs_num = 60;
 m_params.ckps_miss_num = 2;
 m_params.ckps_engine_cyl = 4;
 m_params.hall_wnd_width = 60.0f;
}

LPCTSTR CCKPSPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CCKPSPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX,IDC_PD_CKPS_FRONT_GROUPBOX, m_ckps_front_groupbox);
 DDX_Control(pDX,IDC_PD_CKPS_POSFRONT_RADIOBOX, m_ckps_posfront_radio);
 DDX_Control(pDX,IDC_PD_CKPS_NEGFRONT_RADIOBOX, m_ckps_negfront_radio);

 DDX_Control(pDX,IDC_PD_REF_S_FRONT_GROUPBOX, m_ref_s_front_groupbox);
 DDX_Control(pDX,IDC_PD_REF_S_POSFRONT_RADIOBOX, m_ref_s_posfront_radio);
 DDX_Control(pDX,IDC_PD_REF_S_NEGFRONT_RADIOBOX, m_ref_s_negfront_radio);

 DDX_Control(pDX,IDC_PD_CKPS_COGS_BEFORE_TDC_CAPTION, m_teeth_before_tdc_label);
 DDX_Control(pDX,IDC_PD_CKPS_COGS_BEFORE_TDC_COMBOBOX, m_teeth_before_tdc_combo);
 DDX_Control(pDX,IDC_PD_CKPS_ENGINE_CYL_COMBOBOX, m_engine_cyl_combo);

 DDX_Control(pDX,IDC_PD_CKPS_IGNITION_COGS_SPIN, m_ignition_cogs_spin);
 DDX_Control(pDX,IDC_PD_CKPS_IGNITION_COGS_EDIT, m_ignition_cogs_edit);
 DDX_Control(pDX,IDC_PD_CKPS_IGNITION_COGS_UNIT, m_ignition_cogs_label);

 DDX_Control(pDX,IDC_PD_CKPS_HALL_WND_WIDTH_SPIN, m_hall_wnd_width_spin);
 DDX_Control(pDX,IDC_PD_CKPS_HALL_WND_WIDTH_EDIT, m_hall_wnd_width_edit);
 DDX_Control(pDX,IDC_PD_CKPS_HALL_WND_WIDTH_UNIT, m_hall_wnd_width_label);

 DDX_Control(pDX,IDC_PD_CKPS_COGS_NUM_SPIN, m_wheel_cogs_num_spin);
 DDX_Control(pDX,IDC_PD_CKPS_COGS_NUM_EDIT, m_wheel_cogs_num_edit);
 DDX_Control(pDX,IDC_PD_CKPS_COGS_NUM_UNIT, m_wheel_cogs_num_label);

 DDX_Control(pDX,IDC_PD_CKPS_MISS_NUM_SPIN, m_wheel_miss_num_spin);
 DDX_Control(pDX,IDC_PD_CKPS_MISS_NUM_EDIT, m_wheel_miss_num_edit);
 DDX_Control(pDX,IDC_PD_CKPS_MISS_NUM_UNIT, m_wheel_miss_num_label);

 DDX_Control(pDX, IDC_PD_CKPS_MERGE_IGN_OUTPUTS, m_merge_ign_outputs_check);

 DDX_Text(pDX, IDC_PD_CKPS_IGNITION_COGS_EDIT, m_params.ckps_ignit_cogs);
 m_hall_wnd_width_edit.DDX_Value(pDX, IDC_PD_CKPS_HALL_WND_WIDTH_EDIT, m_params.hall_wnd_width);
 DDX_Text(pDX, IDC_PD_CKPS_COGS_NUM_EDIT, m_params.ckps_cogs_num);
 DDX_Text(pDX, IDC_PD_CKPS_MISS_NUM_EDIT, m_params.ckps_miss_num);
 DDX_Radio_UCHAR(pDX, IDC_PD_CKPS_NEGFRONT_RADIOBOX, m_params.ckps_edge_type);
 DDX_Radio_UCHAR(pDX, IDC_PD_REF_S_NEGFRONT_RADIOBOX, m_params.ref_s_edge_type);
 DDX_Check_UCHAR(pDX, IDC_PD_CKPS_MERGE_IGN_OUTPUTS, m_params.ckps_merge_ign_outs);
}

/////////////////////////////////////////////////////////////////////////////
// CCKPSPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CCKPSPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_ckps_enabled);
}

void CCKPSPageDlg::OnUpdateCKPSFront(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled/* && (m_ckps_enabled)*/);
}

void CCKPSPageDlg::OnUpdateControls_REF_S_Front(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled/* && m_params.ckps_miss_num == 0 && m_ckps_enabled*/);
}

void CCKPSPageDlg::OnUpdateIgnitionCogs(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_igncogs_enabled && m_ckps_enabled);
 pCmdUI->m_pOther->ShowWindow(m_ckps_enabled ? SW_SHOW : SW_HIDE);
}

void CCKPSPageDlg::OnUpdateCylNumber(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CCKPSPageDlg::OnUpdateMergeOutputs(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_inpmrg_enabled && m_ckps_enabled);
}

void CCKPSPageDlg::OnUpdateHallWndWidth(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_ckps_enabled); //enabled only for Hall sensor
 pCmdUI->m_pOther->ShowWindow(m_ckps_enabled ? SW_HIDE : SW_SHOW);
}

BOOL CCKPSPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_ignition_cogs_edit.SetLimitText(2);
 m_ignition_cogs_edit.SetDecimalPlaces(2);
 m_ignition_cogs_spin.SetBuddy(&m_ignition_cogs_edit);
 m_ignition_cogs_spin.SetRangeAndDelta(1, 200, 1);
 m_ignition_cogs_edit.SetRange(1, 200);

 m_hall_wnd_width_spin.SetBuddy(&m_hall_wnd_width_edit);
 m_hall_wnd_width_edit.SetLimitText(5);
 m_hall_wnd_width_edit.SetDecimalPlaces(1);
 m_hall_wnd_width_spin.SetRangeAndDelta(30.0f, 110.0f, 0.1f);
 m_hall_wnd_width_edit.SetRange(30.0f, 110.0f);

 m_wheel_cogs_num_edit.SetLimitText(3);
 m_wheel_cogs_num_edit.SetDecimalPlaces(3);
 m_wheel_cogs_num_spin.SetBuddy(&m_wheel_cogs_num_edit);
 m_wheel_cogs_num_spin.SetRangeAndDelta(16, 200, 1);
 m_wheel_cogs_num_edit.SetRange(16, 200);

 m_wheel_miss_num_edit.SetLimitText(1);
 m_wheel_miss_num_edit.SetDecimalPlaces(1);
 m_wheel_miss_num_spin.SetBuddy(&m_wheel_miss_num_edit);
 m_wheel_miss_num_spin.SetRangeAndDelta(0, 2, 1);
 m_wheel_miss_num_edit.SetRange(0, 2);

 _FillCKPSTeethBTDCComboBox(); //инициализируем комбо бокс числа зубьев до в.м.т.
 _FillCKPSEngineCylComboBox(); //инициализируем комбо бокс цисла цилиндров двигателя.
 UpdateData(FALSE);  //initialize dialog controls with data

 UpdateDialogControls(this, TRUE);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CCKPSPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CCKPSPageDlg::OnChangeDataCogsNum()
{
 UpdateData();

 //BTDC combobox depends on cogs number and cylinder number,
 //Also we have to preserve selection
 int sel = _GetCKPSTeethBTDCComboBoxSelection();
 _FillCKPSTeethBTDCComboBox();
 _SetCKPSTeethBTDCComboBoxSelection(sel);

 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CCKPSPageDlg::OnChangeDataCylNum()
{
 UpdateData();
 m_params.ckps_engine_cyl = _GetCKPSEngineCylComboBoxSelection();

 //BTDC combobox depends on cogs number and cylinder number,
 //Also we have to preserve selection
 int sel = _GetCKPSTeethBTDCComboBoxSelection();
 _FillCKPSTeethBTDCComboBox();
 _SetCKPSTeethBTDCComboBoxSelection(sel);

 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CCKPSPageDlg::OnClickedPdPosFrontRadio()
{
 m_ckps_negfront_radio.SetCheck(0);
 UpdateData();
 OnChangeNotify();
}

void CCKPSPageDlg::OnClickedPdNegFrontRadio()
{
 m_ckps_posfront_radio.SetCheck(0);
 UpdateData();
 OnChangeNotify();
}

void CCKPSPageDlg::OnClickedPdPosFrontRadio2()
{//REF_S
 m_ref_s_negfront_radio.SetCheck(0);
 UpdateData();
 OnChangeNotify();
}

void CCKPSPageDlg::OnClickedPdNegFrontRadio2()
{//REF_S
 m_ref_s_posfront_radio.SetCheck(0);
 UpdateData();
 OnChangeNotify();
}

void CCKPSPageDlg::OnChangeMergeOutputs()
{
 UpdateData();

 //Cylinder number combobox depends on merge outputs check box,
 //Also we have to preserve selection 
 int sel = _GetCKPSEngineCylComboBoxSelection();
 _FillCKPSEngineCylComboBox();
 _SetCKPSEngineCylComboBoxSelection(sel);

 OnChangeNotify();
}

//Enable/disable all controls
void CCKPSPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//что с контроллами?
bool CCKPSPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CCKPSPageDlg::GetValues(SECU3IO::CKPSPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 m_params.ckps_cogs_btdc = _GetCKPSTeethBTDCComboBoxSelection();
 m_params.ckps_engine_cyl = _GetCKPSEngineCylComboBoxSelection();
 memcpy(o_values,&m_params, sizeof(SECU3IO::CKPSPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CCKPSPageDlg::SetValues(const SECU3IO::CKPSPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params, i_values, sizeof(SECU3IO::CKPSPar));

 //Fill it here because its contents depend on cogs & cylinder number. Note, selection will be set by
 //following calls
 _FillCKPSTeethBTDCComboBox();

 //комбо боксы
 _SetCKPSTeethBTDCComboBoxSelection(m_params.ckps_cogs_btdc);
 _SetCKPSEngineCylComboBoxSelection(m_params.ckps_engine_cyl);

 //устанавливаем состояние контроллов фронта ДПКВ
 m_ckps_posfront_radio.SetCheck(m_params.ckps_edge_type);
 m_ckps_negfront_radio.SetCheck(~m_params.ckps_edge_type);

 //устанавливаем состояние контроллов фронта ДНО (входы REF_S)
 m_ref_s_posfront_radio.SetCheck(m_params.ref_s_edge_type);
 m_ref_s_negfront_radio.SetCheck(~m_params.ref_s_edge_type);

 UpdateData(FALSE); //копируем данные из переменных в диалог
}

void CCKPSPageDlg::EnableIgnitionCogs(bool enable)
{
 if (m_igncogs_enabled == enable)
  return; //already has needed state
 m_igncogs_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CCKPSPageDlg::SetMaxCylinders(int number)
{
 m_max_cylinders = number;
 if (::IsWindow(this->m_hWnd))
  _FillCKPSEngineCylComboBox();
}

void CCKPSPageDlg::EnableOddCylinders(bool enable)
{
 m_odd_cylnum_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  _FillCKPSEngineCylComboBox();
}

void CCKPSPageDlg::EnableCKPSItems(bool enable)
{
 if (m_ckps_enabled == enable)
  return; //already has needed state
 m_ckps_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CCKPSPageDlg::EnableInputsMerging(bool enable)
{
 if (m_inpmrg_enabled == enable)
  return; //already has needed state
 m_inpmrg_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CCKPSPageDlg::_FillCKPSTeethBTDCComboBox(void)
{
 m_cogs_numbers.clear();

 std::map<int, float> degBTDC;
/*if (m_params.ckps_miss_num > 0)
 {
  //Note: Some users require less limited values, so we set 120 for all.
  //Old values are commented out
  degBTDC.insert(std::make_pair(1, 180.0f)); //120
  degBTDC.insert(std::make_pair(2, 180.0f)); //120
  degBTDC.insert(std::make_pair(3, 180.0f)); //120
  degBTDC.insert(std::make_pair(4, 180.0f)); //120
  degBTDC.insert(std::make_pair(5, 180.0f));  //60
  degBTDC.insert(std::make_pair(6, 160.0f));  //50
  degBTDC.insert(std::make_pair(7, 140.0f));  //45
  degBTDC.insert(std::make_pair(8, 120.0f));  //40
 }
 else*/
 { //non-missing teeth wheel
  degBTDC.insert(std::make_pair(1, 330.0f));
  degBTDC.insert(std::make_pair(2, 330.0f));
  degBTDC.insert(std::make_pair(3, 330.0f));
  degBTDC.insert(std::make_pair(4, 330.0f));
  degBTDC.insert(std::make_pair(5, 330.0f));
  degBTDC.insert(std::make_pair(6, 270.0f));
  degBTDC.insert(std::make_pair(7, 270.0f));
  degBTDC.insert(std::make_pair(8, 270.0f));
 }

 //calculate range for number of cogs BTDC
 int cyl = m_params.ckps_engine_cyl;
 if (cyl > m_max_cylinders)
  cyl = m_max_cylinders;
 int cogs = m_params.ckps_cogs_num;
 float deg = degBTDC[cyl];
 int cogsBTDC = MathHelpers::Round(ceil(deg / (360.0f / cogs)));
 int lo_limit = /*(m_params.ckps_miss_num > 0) ? MathHelpers::Round(cogsBTDC - (cogsBTDC / 2.0f)) :*/ MathHelpers::Round(ceil(66.0f / (360.0f / cogs)));
 int hi_limit = /*(m_params.ckps_miss_num > 0) ? MathHelpers::Round(cogsBTDC + (cogsBTDC / 1.5f)) :*/ cogsBTDC;
 for(int tn = lo_limit; tn <= hi_limit; ++tn)
 {
  CString str;
  str.Format(_T("%d"), tn);
  m_cogs_numbers.push_back(std::make_pair(tn, str.GetBuffer(0)));
 }

 m_teeth_before_tdc_combo.ResetContent();
 for(size_t i = 0; i < m_cogs_numbers.size(); i++)
 {
  int index = m_teeth_before_tdc_combo.AddString(m_cogs_numbers[i].second.c_str());
  if (index==CB_ERR)
  {
   ASSERT(0);
   continue;
  }
  m_teeth_before_tdc_combo.SetItemData(index, i);
 }
}

int CCKPSPageDlg::_GetCKPSTeethBTDCComboBoxSelection(void)
{
 int index = m_teeth_before_tdc_combo.GetCurSel();
 if (index==CB_ERR)
 {
  ASSERT(0);
  return 0;
 }
 int cog_index = m_teeth_before_tdc_combo.GetItemData(index);
 return m_cogs_numbers[cog_index].first;
}

void CCKPSPageDlg::_SetCKPSTeethBTDCComboBoxSelection(int i_sel)
{
 for(size_t i = 0; i < m_cogs_numbers.size(); i++)
 {
  if (m_cogs_numbers[i].first != i_sel) //find index in container for cog number
   continue;
  //find related index and select corresponding item
  int count = m_teeth_before_tdc_combo.GetCount();
  for(int ii = 0; ii < count; ii++)
  if (m_teeth_before_tdc_combo.GetItemData(ii) == i)
  {
   m_teeth_before_tdc_combo.SetCurSel(ii);
   return;
  }
 }
 //if we are not able to find corresponding number then select middle number
 int middle = m_cogs_numbers.size() / 2;
 int count = m_teeth_before_tdc_combo.GetCount();
 for(int ii = 0; ii < count; ii++)
 if (m_teeth_before_tdc_combo.GetItemData(ii) == middle)
 {
  m_teeth_before_tdc_combo.SetCurSel(ii);
  return;
 }
}

void CCKPSPageDlg::_FillCKPSEngineCylComboBox(void)
{
 bool odd_cylnum_enabled = m_params.ckps_merge_ign_outs || m_odd_cylnum_enabled;

 m_engine_cyls.clear();
 if (odd_cylnum_enabled) //odd
  m_engine_cyls.push_back(std::make_pair(1,_TSTRING(_T("1"))));
 m_engine_cyls.push_back(std::make_pair(2,_TSTRING(_T("2"))));
 if (odd_cylnum_enabled) //odd
  m_engine_cyls.push_back(std::make_pair(3,_TSTRING(_T("3"))));
 m_engine_cyls.push_back(std::make_pair(4,_TSTRING(_T("4"))));
 if (m_max_cylinders > 4)
 {
  if (odd_cylnum_enabled) //odd
   m_engine_cyls.push_back(std::make_pair(5,_TSTRING(_T("5"))));
  m_engine_cyls.push_back(std::make_pair(6,_TSTRING(_T("6"))));
 }
 if (m_max_cylinders > 6)
  m_engine_cyls.push_back(std::make_pair(8,_TSTRING(_T("8"))));

 m_engine_cyl_combo.ResetContent();
 for(size_t i = 0; i < m_engine_cyls.size(); i++)
 {
  int index = m_engine_cyl_combo.AddString(m_engine_cyls[i].second.c_str());
  if (index==CB_ERR)
  {
   ASSERT(0);
   continue;
  }
  m_engine_cyl_combo.SetItemData(index, i);
 }
}

int CCKPSPageDlg::_GetCKPSEngineCylComboBoxSelection(void)
{
 int index = m_engine_cyl_combo.GetCurSel();
 if (index==CB_ERR)
 {
  ASSERT(0);
  return 0;
 }
 int cyl_index = m_engine_cyl_combo.GetItemData(index);
 return m_engine_cyls[cyl_index].first;
}

void CCKPSPageDlg::_SetCKPSEngineCylComboBoxSelection(int i_sel)
{
 for(size_t i = 0; i < m_engine_cyls.size(); i++)
 {
  if (m_engine_cyls[i].first != i_sel) //find index in conntainer for cog number
   continue;
  //find related index and select corresponding item
  int count = m_engine_cyl_combo.GetCount();
  for(int ii = 0; ii < count; ii++)
  if (m_engine_cyl_combo.GetItemData(ii) == i)
  {
   m_engine_cyl_combo.SetCurSel(ii);
   return;
  }
 }
 ASSERT(0);
}
