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
#include "resource.h"
#include "common/MathHelpers.h"
#include "DLLLinkedFunctions.h"
#include "io-core/secu3io.h"
#include "MapIds.h"
#include "TablesSetPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
void __cdecl CTablesSetPanel::OnChangeCoilRegulTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_COILREGUL);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseCoilRegulTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_coilregul_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_coilregul_map_wnd_handle, TYPE_MAP_COILREGUL);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnGetYAxisLabel(LPTSTR io_label_string, void* i_param)
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

const UINT CTablesSetPanel::IDD = IDD_TD_ALLTABLES_PANEL;

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel dialog

CTablesSetPanel::CTablesSetPanel(CWnd* pParent /*= NULL*/)
: Super(CTablesSetPanel::IDD, pParent)
, m_coilreg_enabled(false)
{
 m_attenuator_map_chart_state = 0;
 m_coilregul_map_chart_state = 0;

 m_attenuator_map_wnd_handle = NULL;
 m_coilregul_map_wnd_handle = NULL;

 int rpm = 200;
 for(size_t i = 0; i < 128; i++)
 {
  m_attenuator_table_slots[i] = (float)rpm;
  rpm+=60;
 }
}

void CTablesSetPanel::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_FUNSET_LIST, m_funset_listbox);
 DDX_Control(pDX, IDC_TD_VIEW_ATTENUATOR_MAP, m_view_attenuator_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_COIL_REGULATION, m_view_coilregul_map_btn);
}

BEGIN_MESSAGE_MAP(CTablesSetPanel, Super)
 ON_BN_CLICKED(IDC_TD_VIEW_ATTENUATOR_MAP, OnViewAttenuatorMap)
 ON_BN_CLICKED(IDC_TD_VIEW_COIL_REGULATION, OnViewCoilRegulMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATTENUATOR_MAP, OnUpdateViewAttenuatorMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_COIL_REGULATION, OnUpdateViewCoilRegulMap)
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

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CTablesSetPanel::OnUpdateViewAttenuatorMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_attenuator_map_chart_state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewCoilRegulMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_coilreg_enabled);
 pCmdUI->SetCheck( (m_coilregul_map_chart_state) ? TRUE : FALSE );
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
  DLL::Chart2DUpdate(m_attenuator_map_wnd_handle,GetAttenuatorMap(true),GetAttenuatorMap(false));
 if (m_coilregul_map_chart_state)
  DLL::Chart2DUpdate(m_coilregul_map_wnd_handle,GetCoilRegulMap(true), GetCoilRegulMap(false));
}

void CTablesSetPanel::EnableCoilRegulation(bool enable)
{
 m_coilreg_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_coilregul_map_chart_state && ::IsWindow(m_coilregul_map_wnd_handle))
  DLL::Chart2DEnable(m_coilregul_map_wnd_handle, enable && Super::IsAllowed());
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
    MLL::GetString(IDS_ATTENUATOR_MAP).c_str());
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

void CTablesSetPanel::OnViewCoilRegulMap()
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_coilregul_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_coilregul_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_coilregul_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_coilregul_map_chart_state = 1;
  m_coilregul_map_wnd_handle = DLL::Chart2DCreate(GetCoilRegulMap(true), GetCoilRegulMap(false), 0.25f, 16.0, SECU3IO::coilregul_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COILREGUL_UNIT).c_str(),
    MLL::GetString(IDS_COILREGUL_MAP).c_str());
  DLL::Chart2DSetAxisValuesFormat(m_coilregul_map_wnd_handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(m_coilregul_map_wnd_handle, OnChangeCoilRegulTable, this);
  DLL::Chart2DSetOnClose(m_coilregul_map_wnd_handle, OnCloseCoilRegulTable, this);
  DLL::Chart2DUpdate(m_coilregul_map_wnd_handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_coilregul_map_wnd_handle, TYPE_MAP_COILREGUL);

  DLL::Chart2DShow(m_coilregul_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_coilregul_map_wnd_handle);
 }
}

float* CTablesSetPanel::GetAttenuatorMap(bool i_original)
{
 if (i_original)
  return m_attenuator_map_original;
 else
  return m_attenuator_map_active;
}

float* CTablesSetPanel::GetCoilRegulMap(bool i_original)
{
 if (i_original)
  return m_coilregul_map_original;
 else
  return m_coilregul_map_active;
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
 case TYPE_MAP_COILREGUL:
  return m_coilregul_map_wnd_handle ? m_coilregul_map_wnd_handle : NULL;
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
