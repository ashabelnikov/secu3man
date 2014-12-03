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

#include "stdafx.h"
#include "resource.h"
#include "common/MathHelpers.h"
#include "DLLLinkedFunctions.h"
#include "DwellCalcDlg.h"
#include "io-core/secu3io.h"
#include "MapIds.h"
#include "TablesSetPanel.h"
#include "ui-core/ToolTipCtrlEx.h"

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeAttenuatorTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_ATTENUATOR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseAttenuatorTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_attenuator_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_attenuator_map_wnd_handle, TYPE_MAP_ATTENUATOR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeDwellCntrlTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_DWELLCNTRL);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseDwellCntrlTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_dwellcntrl_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_dwellcntrl_map_wnd_handle, TYPE_MAP_DWELLCNTRL);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeCTSCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_CTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseCTSCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_cts_curve_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_cts_curve_map_wnd_handle, TYPE_MAP_CTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeChokeOpTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_CHOKE_OP);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseChokeOpTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_choke_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_choke_map_wnd_handle, TYPE_MAP_CHOKE_OP);
}


//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeATSCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_ATS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseATSCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_ats_curve_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_ats_curve_map_wnd_handle, TYPE_MAP_ATS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeATSAACTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_ATS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseATSAACTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_ats_aac_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_ats_aac_map_wnd_handle, TYPE_MAP_ATS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnGetYAxisLabel(LPTSTR io_label_string, int index, void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }

 float value = (float)_tcstod(io_label_string, NULL);
 int i = MathHelpers::Round(value);
 if (i >= 0 && i < SECU3IO::ATTENUATOR_LEVELS_SIZE)
  _stprintf(io_label_string, _T("%0.3f"), SECU3IO::hip9011_attenuator_gains[i]);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnGetXAxisLabel(LPTSTR io_label_string, int index, void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }
 _stprintf(io_label_string, _T("")); //empty string
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeCTSXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 
 _this->m_cts_curve_x_axis_limits[i_type] = i_value;

 //allow controller to detect changes
 if (_this->m_OnCTSXAxisEditChanged)
  _this->m_OnCTSXAxisEditChanged(i_type, i_value);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeATSXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 
 _this->m_ats_curve_x_axis_limits[i_type] = i_value;

 //allow controller to detect changes
 if (_this->m_OnATSXAxisEditChanged)
  _this->m_OnATSXAxisEditChanged(i_type, i_value);
}

//------------------------------------------------------------------------

const UINT CTablesSetPanel::IDD = IDD_TD_ALLTABLES_PANEL;

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel dialog

CTablesSetPanel::CTablesSetPanel(CWnd* pParent /*= NULL*/)
: Super(CTablesSetPanel::IDD, pParent)
, m_dwellcntrl_enabled(false)
, m_cts_curve_enabled(false)
, m_choke_op_enabled(false)
{
 m_scrl_factor = 1.90f;

 m_attenuator_map_chart_state = 0;
 m_dwellcntrl_map_chart_state = 0;
 m_cts_curve_map_chart_state = 0;
 m_ats_curve_map_chart_state = 0;
 m_ats_aac_map_chart_state = 0;
 m_choke_map_chart_state = 0;

 m_attenuator_map_wnd_handle = NULL;
 m_dwellcntrl_map_wnd_handle = NULL;
 m_cts_curve_map_wnd_handle = NULL;
 m_ats_curve_map_wnd_handle = NULL;
 m_ats_aac_map_wnd_handle = NULL;
 m_choke_map_wnd_handle = NULL;

 int rpm = 200;
 for(size_t i = 0; i < 128; i++)
 {
  m_attenuator_table_slots[i] = (float)rpm;
  rpm+=60;
 }

 memset(m_cts_curve_x_axis_limits, 0, sizeof(float) * 2);
 memset(m_ats_curve_x_axis_limits, 0, sizeof(float) * 2);
}

void CTablesSetPanel::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_FUNSET_LIST, m_funset_listbox);
 DDX_Control(pDX, IDC_TD_VIEW_ATTENUATOR_MAP, m_view_attenuator_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_DWELL_CONTROL, m_view_dwellcntrl_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_CTS_CURVE, m_view_cts_curve_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_ATS_CURVE, m_view_ats_curve_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_ATS_MAP, m_view_ats_aac_map_btn);
 DDX_Control(pDX, IDC_TD_DWELL_CALC_BUTTON, m_calc_dwell_btn);
 DDX_Control(pDX, IDC_TD_VIEW_CHOKE_MAP, m_view_choke_op_map_btn);
 DDX_Control(pDX, IDC_TD_RPM_GRID_BUTTON, m_rpm_grid_btn);
}

BEGIN_MESSAGE_MAP(CTablesSetPanel, Super)
 ON_BN_CLICKED(IDC_TD_VIEW_ATTENUATOR_MAP, OnViewAttenuatorMap)
 ON_BN_CLICKED(IDC_TD_VIEW_DWELL_CONTROL, OnViewDwellCntrlMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CTS_CURVE, OnViewCTSCurveMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CHOKE_MAP, OnViewChokeOpMap)
 ON_BN_CLICKED(IDC_TD_DWELL_CALC_BUTTON, OnDwellCalcButton)
 ON_BN_CLICKED(IDC_TD_RPM_GRID_BUTTON, OnRPMGridButton)
 ON_BN_CLICKED(IDC_TD_VIEW_ATS_CURVE, OnViewATSCurveMap)
 ON_BN_CLICKED(IDC_TD_VIEW_ATS_MAP, OnViewATSAACMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATTENUATOR_MAP, OnUpdateViewAttenuatorMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_DWELL_CONTROL, OnUpdateViewDwellCntrlMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CTS_CURVE, OnUpdateViewCTSCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATS_CURVE, OnUpdateViewATSCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATS_MAP, OnUpdateViewATSAACMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CHOKE_MAP, OnUpdateViewChokeOpMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_DWELL_CALC_BUTTON, OnUpdateViewDwellCntrlMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_RPM_GRID_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_FUNSET_LIST, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_MAP_GROUPBOX, OnUpdateControls)
 ON_NOTIFY(LVN_ITEMCHANGED, IDC_TD_FUNSET_LIST, OnChangeFunsetList)
 ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TD_FUNSET_LIST, OnEndLabelEditFunsetList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel message handlers

BOOL CTablesSetPanel::OnInitDialog()
{
 Super::OnInitDialog();

 m_calc_dwell_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                              MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));
 m_rpm_grid_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_GRID_UP), MAKEINTRESOURCE(IDB_GRID_DOWN), 
                              MAKEINTRESOURCE(IDB_GRID_FOCUSED), MAKEINTRESOURCE(IDB_GRID_DISABLED));

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_calc_dwell_btn, MLL::GetString(IDS_TD_DWELL_CALC_BUTTON_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_rpm_grid_btn, MLL::GetString(IDS_TD_RPM_GRID_BUTTON_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_dwellcntrl_map_btn, MLL::GetString(IDS_TD_VIEW_DWELL_CONTROL_TT))); 
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CTablesSetPanel::OnUpdateViewAttenuatorMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_attenuator_map_chart_state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewDwellCntrlMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_dwellcntrl_enabled);
 pCmdUI->SetCheck( (m_dwellcntrl_map_chart_state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewCTSCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_cts_curve_enabled);
 pCmdUI->SetCheck( (m_cts_curve_map_chart_state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewATSCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_ats_curve_map_chart_state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewATSAACMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_ats_aac_map_chart_state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewChokeOpMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_choke_op_enabled);
 pCmdUI->SetCheck( (m_choke_map_chart_state) ? TRUE : FALSE );
}

//обновл€ет контроллы состо€ние которых зависит от того - есть данные или нет
void CTablesSetPanel::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool enabled = m_IsAllowed ? m_IsAllowed() : false;
 pCmdUI->Enable(enabled ? TRUE : FALSE);
}

void CTablesSetPanel::UpdateOpenedCharts(void)
{
 Super::UpdateOpenedCharts();
 if (m_attenuator_map_chart_state)
  DLL::Chart2DUpdate(m_attenuator_map_wnd_handle, GetAttenuatorMap(true), GetAttenuatorMap(false));
 if (m_dwellcntrl_map_chart_state)
  DLL::Chart2DUpdate(m_dwellcntrl_map_wnd_handle, GetDwellCntrlMap(true), GetDwellCntrlMap(false));
 if (m_cts_curve_map_chart_state)
 {
  DLL::Chart2DUpdate(m_cts_curve_map_wnd_handle, GetCTSCurveMap(true), GetCTSCurveMap(false));
  DLL::Chart2DUpdateAxisEdits(m_cts_curve_map_wnd_handle, 1, m_cts_curve_x_axis_limits[0], m_cts_curve_x_axis_limits[1]);
 }
 if (m_choke_map_chart_state)
  DLL::Chart2DUpdate(m_choke_map_wnd_handle, GetChokeOpMap(true), GetChokeOpMap(false));
}

void CTablesSetPanel::EnableDwellControl(bool enable)
{
 m_dwellcntrl_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_dwellcntrl_map_chart_state && ::IsWindow(m_dwellcntrl_map_wnd_handle))
  DLL::Chart2DEnable(m_dwellcntrl_map_wnd_handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableCTSCurve(bool enable)
{
 m_cts_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_cts_curve_map_chart_state && ::IsWindow(m_cts_curve_map_wnd_handle))
  DLL::Chart2DEnable(m_cts_curve_map_wnd_handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableChokeOp(bool enable)
{
 m_choke_op_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_choke_map_chart_state && ::IsWindow(m_choke_map_wnd_handle))
  DLL::Chart2DEnable(m_choke_map_wnd_handle, enable && Super::IsAllowed());
}

//изменилось выделение в спимке семейств характеристик
void CTablesSetPanel::OnChangeFunsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR;

 if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED))
 {
  //состо€ние выделени€ изменилось
  int selected_index = 0;

  //will return -1 if there are no selected items, at all
  selected_index = m_funset_listbox.GetNextItem(-1, LVNI_SELECTED);

  if (m_OnFunSetSelectionChanged)
   m_OnFunSetSelectionChanged(selected_index);
 }

 *pResult = 0;
}

void CTablesSetPanel::OnEndLabelEditFunsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLVDISPINFO *pdi = (LV_DISPINFO FAR *) pNMHDR;

 if (pdi->item.mask == LVIF_TEXT)
 {
  m_funset_listbox.SetItem(&pdi->item);
  if (m_OnFunSetNamechanged)
   m_OnFunSetNamechanged(pdi->item.iItem,pdi->item.pszText);
 }
}

void CTablesSetPanel::SetFunSetListBox(std::vector<_TSTRING> i_list_of_names)
{
 m_funset_listbox.DeleteAllItems();
 for (size_t i = 0; i < i_list_of_names.size(); i++)
 {
  m_funset_listbox.InsertItem(i,i_list_of_names[i].c_str());
 }
}

void CTablesSetPanel::SetFunSetListBoxSelection(int i_selected_index)
{
 if (i_selected_index!=-1)
  m_funset_listbox.SetItemState(i_selected_index, LVIS_SELECTED, LVIS_SELECTED);

 int i = m_funset_listbox.GetNextItem(-1, LVNI_SELECTED);
}

void CTablesSetPanel::OnViewAttenuatorMap()
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_attenuator_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_attenuator_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_attenuator_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_attenuator_map_chart_state = 1;
  m_attenuator_map_wnd_handle = DLL::Chart2DCreate(GetAttenuatorMap(true),GetAttenuatorMap(false),0.0f,63,m_attenuator_table_slots,128,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ATTENUATOR_GAIN_UNIT).c_str(),
    MLL::GetString(IDS_ATTENUATOR_MAP).c_str(), false);
  DLL::Chart2DSetMarksVisible(m_attenuator_map_wnd_handle,1, false); //пр€чем надписи над узловыми точками функции
  DLL::Chart2DSetOnChange(m_attenuator_map_wnd_handle,OnChangeAttenuatorTable, this);
  DLL::Chart2DSetOnClose(m_attenuator_map_wnd_handle,OnCloseAttenuatorTable, this);
  DLL::Chart2DSetOnGetAxisLabel(m_attenuator_map_wnd_handle, 0, OnGetYAxisLabel, this);
  DLL::Chart2DSetAxisValuesFormat(m_attenuator_map_wnd_handle, 0, _T("#0.00"));
  DLL::Chart2DInverseAxis(m_attenuator_map_wnd_handle, 0, true);
  DLL::Chart2DUpdate(m_attenuator_map_wnd_handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_attenuator_map_wnd_handle, TYPE_MAP_ATTENUATOR);

  DLL::Chart2DShow(m_attenuator_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_attenuator_map_wnd_handle);
 }
}

void CTablesSetPanel::OnViewDwellCntrlMap()
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_dwellcntrl_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_dwellcntrl_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_dwellcntrl_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_dwellcntrl_map_chart_state = 1;
  m_dwellcntrl_map_wnd_handle = DLL::Chart2DCreate(GetDwellCntrlMap(true), GetDwellCntrlMap(false), 0.25f, 16.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DWELLCNTRL_UNIT).c_str(),
    MLL::GetString(IDS_DWELLCNTRL_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_dwellcntrl_map_wnd_handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(m_dwellcntrl_map_wnd_handle, OnChangeDwellCntrlTable, this);
  DLL::Chart2DSetOnClose(m_dwellcntrl_map_wnd_handle, OnCloseDwellCntrlTable, this);
  DLL::Chart2DUpdate(m_dwellcntrl_map_wnd_handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_dwellcntrl_map_wnd_handle, TYPE_MAP_DWELLCNTRL);

  DLL::Chart2DShow(m_dwellcntrl_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_dwellcntrl_map_wnd_handle);
 }
}

void CTablesSetPanel::OnViewCTSCurveMap()
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_cts_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_cts_curve_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_cts_curve_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_cts_curve_map_chart_state = 1;
  m_cts_curve_map_wnd_handle = DLL::Chart2DCreate(GetCTSCurveMap(true), GetCTSCurveMap(false), -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_CTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_cts_curve_map_wnd_handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_cts_curve_map_wnd_handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, OnChangeCTSXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_cts_curve_map_wnd_handle, 1, OnGetXAxisLabel, this);
  DLL::Chart2DSetOnChange(m_cts_curve_map_wnd_handle, OnChangeCTSCurveTable, this);
  DLL::Chart2DSetOnClose(m_cts_curve_map_wnd_handle, OnCloseCTSCurveTable, this);
  DLL::Chart2DUpdate(m_cts_curve_map_wnd_handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_cts_curve_map_wnd_handle, 1, m_cts_curve_x_axis_limits[0], m_cts_curve_x_axis_limits[1]);

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_cts_curve_map_wnd_handle, TYPE_MAP_CTS_CURVE);

  DLL::Chart2DShow(m_cts_curve_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_cts_curve_map_wnd_handle);
 }
}

void CTablesSetPanel::OnViewChokeOpMap()
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_choke_op_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_choke_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_choke_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_choke_map_chart_state = 1;
  m_choke_map_wnd_handle = DLL::Chart2DCreate(GetChokeOpMap(true), GetChokeOpMap(false), 0, 100.0, SECU3IO::choke_op_map_slots, 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_CHOKE_OP_UNIT).c_str(),
    MLL::GetString(IDS_CHOKE_OP_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(m_choke_map_wnd_handle, OnChangeChokeOpTable, this);
  DLL::Chart2DSetOnClose(m_choke_map_wnd_handle, OnCloseChokeOpTable, this);
  DLL::Chart2DUpdate(m_choke_map_wnd_handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_choke_map_wnd_handle, TYPE_MAP_CHOKE_OP);

  DLL::Chart2DShow(m_choke_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_choke_map_wnd_handle);
 }
}


void CTablesSetPanel::OnViewATSCurveMap()
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_ats_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_ats_curve_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_ats_curve_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_ats_curve_map_chart_state = 1;
  m_ats_curve_map_wnd_handle = DLL::Chart2DCreate(GetATSCurveMap(true), GetATSCurveMap(false), -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_ATS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_ats_curve_map_wnd_handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_ats_curve_map_wnd_handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, OnChangeCTSXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_ats_curve_map_wnd_handle, 1, OnGetXAxisLabel, this);
  DLL::Chart2DSetOnChange(m_ats_curve_map_wnd_handle, OnChangeATSCurveTable, this);
  DLL::Chart2DSetOnClose(m_ats_curve_map_wnd_handle, OnCloseATSCurveTable, this);
  DLL::Chart2DUpdate(m_ats_curve_map_wnd_handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_ats_curve_map_wnd_handle, 1, m_ats_curve_x_axis_limits[0], m_ats_curve_x_axis_limits[1]);

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_ats_curve_map_wnd_handle, TYPE_MAP_ATS_CURVE);

  DLL::Chart2DShow(m_ats_curve_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_ats_curve_map_wnd_handle);
 }
}

void CTablesSetPanel::OnViewATSAACMap()
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_ats_aac_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_ats_aac_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_ats_aac_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_ats_aac_map_chart_state = 1;
  m_ats_aac_map_wnd_handle = DLL::Chart2DCreate(GetATSAACMap(true),GetATSAACMap(false),-15.0,25.0,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_ATSCORR_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(m_ats_aac_map_wnd_handle,OnChangeATSAACTable,this);
  DLL::Chart2DSetOnClose(m_ats_aac_map_wnd_handle,OnCloseATSAACTable,this);
  DLL::Chart2DUpdate(m_ats_aac_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_ats_aac_map_wnd_handle, TYPE_MAP_ATS_CORR);

  DLL::Chart2DShow(m_ats_aac_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_ats_aac_map_wnd_handle);
 }
}

void CTablesSetPanel::OnDwellCalcButton()
{
 CDwellCalcDlg dialog;
 dialog.SetVoltageValues(SECU3IO::dwellcntrl_map_slots, 32);
 dialog.SetLimits(0.25f, 16.0f);
 if (dialog.DoModal()==IDOK)
 {
  //copy results
  std::copy(dialog.GetTimeValues(), dialog.GetTimeValues() + 32, GetDwellCntrlMap(false));
  if (DLL::Chart2DCreate)
  {//Update charts and notify about changes
   UpdateOpenedCharts();
   if (this->m_OnMapChanged)
    this->m_OnMapChanged(TYPE_MAP_DWELLCNTRL);
  }
 }
}

void CTablesSetPanel::OnRPMGridButton()
{
 if (m_OnRPMGridEditButton)
  m_OnRPMGridEditButton();
}

float* CTablesSetPanel::GetAttenuatorMap(bool i_original)
{
 if (i_original)
  return m_attenuator_map_original;
 else
  return m_attenuator_map_active;
}

float* CTablesSetPanel::GetDwellCntrlMap(bool i_original)
{
 if (i_original)
  return m_dwellcntrl_map_original;
 else
  return m_dwellcntrl_map_active;
}

float* CTablesSetPanel::GetCTSCurveMap(bool i_original)
{
 if (i_original)
  return m_cts_curve_map_original;
 else
  return m_cts_curve_map_active;
}

float* CTablesSetPanel::GetATSCurveMap(bool i_original)
{
 if (i_original)
  return m_ats_curve_map_original;
 else
  return m_ats_curve_map_active;
}

float* CTablesSetPanel::GetATSAACMap(bool i_original)
{
 if (i_original)
  return m_ats_aac_map_original;
 else
  return m_ats_aac_map_active;
}

void CTablesSetPanel::SetCTSXAxisEdits(float i_begin, float i_end)
{
 m_cts_curve_x_axis_limits[0] = i_begin;
 m_cts_curve_x_axis_limits[1] = i_end;	
}

void CTablesSetPanel::SetATSXAxisEdits(float i_begin, float i_end)
{
 m_ats_curve_x_axis_limits[0] = i_begin;
 m_ats_curve_x_axis_limits[1] = i_end;	
}

float* CTablesSetPanel::GetChokeOpMap(bool i_original)
{
 if (i_original)
  return m_choke_map_original;
 else
  return m_choke_map_active;
}

HWND CTablesSetPanel::GetMapWindow(int wndType)
{
 HWND hwnd = Super::GetMapWindow(wndType);
 if (hwnd != NULL)
  return hwnd;

 switch(wndType)
 {
 case TYPE_MAP_ATTENUATOR:
  return m_attenuator_map_wnd_handle ? m_attenuator_map_wnd_handle : NULL;
 case TYPE_MAP_DWELLCNTRL:
  return m_dwellcntrl_map_wnd_handle ? m_dwellcntrl_map_wnd_handle : NULL;
 case TYPE_MAP_CTS_CURVE:
  return m_cts_curve_map_wnd_handle ? m_cts_curve_map_wnd_handle : NULL;
 case TYPE_MAP_CHOKE_OP:
  return m_choke_map_wnd_handle ? m_choke_map_wnd_handle : NULL;
 case TYPE_MAP_ATS_CURVE:
  return m_ats_curve_map_wnd_handle ? m_ats_curve_map_wnd_handle : NULL;
 case TYPE_MAP_ATS_CORR:
  return m_ats_aac_map_wnd_handle ? m_ats_aac_map_wnd_handle : NULL;

 default:
  return NULL;
 }
}

bool CTablesSetPanel::IsAllowed(void)
{ //change behavior of base class
 return Super::IsAllowed() && m_funset_listbox.GetSelectedCount();
}

void CTablesSetPanel::setOnFunSetSelectionChanged(EventWithCode OnFunction)
{m_OnFunSetSelectionChanged = OnFunction;}

void CTablesSetPanel::setOnFunSetNamechanged(EventWithCodeAndString OnFunction)
{m_OnFunSetNamechanged = OnFunction;}

void CTablesSetPanel::setOnCTSXAxisEditChanged(EventWithCodeAndFloat OnFunction)
{m_OnCTSXAxisEditChanged = OnFunction;}

void CTablesSetPanel::setOnATSXAxisEditChanged(EventWithCodeAndFloat OnFunction)
{m_OnATSXAxisEditChanged = OnFunction;}

void CTablesSetPanel::setOnRPMGridEditButton(EventHandler OnFunction)
{m_OnRPMGridEditButton = OnFunction;}
