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

/** \file TablesSetPanel.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "common/FastDelegate.h"
#include "ButtonsPanel.h"
#include "common/dpiaware.h"
#include "common/gdihelpers.h"
#include "SeptabsPanel.h"
#include "io-core/secu3io.h"
#include "TablesSetPanel.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel dialog

CTablesSetPanel::CTablesSetPanel()
: mp_ButtonsPanel(new CButtonsPanel(false, false, true))
, mp_SeptabsPanel(new CSeptabsPanel(true, true, true, true))
, mp_scr(new CWndScroller)
{
 mp_ButtonsPanel->setIsAllowed(MakeDelegate(this, &CTablesSetPanel::IsAllowed));
}

CTablesSetPanel::~CTablesSetPanel()
{
}

BOOL CTablesSetPanel::Create(CWnd* pParentWnd /*= NULL*/)
{
 mp_SeptabsPanel->setIsAllowed(m_IsAllowed);
 return Super::Create(IDD_TD_ALLTABLES_PANEL, pParentWnd);
}

void CTablesSetPanel::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_FUNSET_LIST, m_funset_listbox);
}

BEGIN_MESSAGE_MAP(CTablesSetPanel, Super)
 ON_NOTIFY(LVN_ITEMCHANGED, IDC_TD_FUNSET_LIST, OnChangeFunsetList)
 ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TD_FUNSET_LIST, OnEndLabelEditFunsetList)
 ON_UPDATE_COMMAND_UI(IDC_TD_FUNSET_LIST, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_MAP_GROUPBOX, OnUpdateControls)
 ON_WM_DESTROY()
 ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel message handlers

BOOL CTablesSetPanel::OnInitDialog()
{
 Super::OnInitDialog();

 //initialize window scroller
 mp_scr->Init(this);

 //create dialog with set of maps
 CRect rect = GDIHelpers::GetChildWndRect(this, IDC_TD_SETTABS_FRAME);
 mp_ButtonsPanel->Create(this);
 mp_ButtonsPanel->SetPosition(rect, &m_funset_listbox);
 mp_ButtonsPanel->ShowWindow(SW_SHOWNORMAL);
 mp_ButtonsPanel->SetMapSetIndex(-1);

 //create dialog with separate maps
 rect = GDIHelpers::GetChildWndRect(this, IDC_TD_SEPTABS_FRAME);
 mp_SeptabsPanel->Create(this);
 mp_SeptabsPanel->SetPosition(rect, mp_ButtonsPanel.get());
 mp_SeptabsPanel->ShowWindow(SW_SHOWNORMAL);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_funset_listbox, MLL::GetString(IDS_TD_FUNSET_LIST_TT))); 
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CTablesSetPanel::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CTablesSetPanel::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(1430));
}

void CTablesSetPanel::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after, x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

void CTablesSetPanel::UpdateOpenedCharts(void)
{
 mp_ButtonsPanel->UpdateOpenedCharts();
 mp_SeptabsPanel->UpdateOpenedCharts();
 UpdateDialogControls(this,TRUE);
}

//Updates controls which state depends on whether or not data is
void CTablesSetPanel::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool enabled = m_IsAllowed ? m_IsAllowed() : false;
 pCmdUI->Enable(enabled ? TRUE : FALSE);
}

//изменилось выделение в спимке семейств характеристик
void CTablesSetPanel::OnChangeFunsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR;

 if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED))
 {
  //состояние выделения изменилось
  int selected_index = 0;

  //will return -1 if there are no selected items, at all
  selected_index = m_funset_listbox.GetNextItem(-1, LVNI_SELECTED);

  if (m_OnFunSetSelectionChanged)
   m_OnFunSetSelectionChanged(selected_index);

  mp_ButtonsPanel->SetMapSetIndex(selected_index+1);
 }
 else
 {
  mp_ButtonsPanel->SetMapSetIndex(-1);
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

void CTablesSetPanel::SetFunSetListBox(const std::vector<_TSTRING>& i_list_of_names)
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

HWND CTablesSetPanel::GetMapWindow(int wndType)
{
 HWND hwnd = mp_ButtonsPanel->GetMapWindow(wndType);
 if (hwnd != NULL)
  return hwnd;

 hwnd = mp_SeptabsPanel->GetMapWindow(wndType);
 if (hwnd != NULL)
  return hwnd;

 return NULL;
}

bool CTablesSetPanel::IsAllowed(void)
{ //change behavior of CButtonsPanel::IsAllowed()
 return (m_IsAllowed ? m_IsAllowed() : false) && m_funset_listbox.GetSelectedCount();
}

void CTablesSetPanel::setOnFunSetSelectionChanged(EventWithCode OnFunction)
{m_OnFunSetSelectionChanged = OnFunction;}

void CTablesSetPanel::setOnFunSetNamechanged(EventWithCodeAndString OnFunction)
{m_OnFunSetNamechanged = OnFunction;}

void CTablesSetPanel::setIsAllowed(EventResult IsFunction)
{m_IsAllowed = IsFunction;}

void CTablesSetPanel::CloseCharts(void)
{
 mp_ButtonsPanel->CloseCharts();;
 mp_SeptabsPanel->CloseCharts();;
}

void CTablesSetPanel::ShowOpenedCharts(bool i_show)
{
 mp_ButtonsPanel->ShowOpenedCharts(i_show);
 mp_SeptabsPanel->ShowOpenedCharts(i_show);
}

void CTablesSetPanel::MakeChartsChildren(bool children)
{
 mp_ButtonsPanel->MakeChartsChildren(children);
 mp_SeptabsPanel->MakeChartsChildren(children);
}

void CTablesSetPanel::EnableToggleMapWnd(bool toggle)
{
 mp_ButtonsPanel->EnableToggleMapWnd(toggle);
 mp_SeptabsPanel->EnableToggleMapWnd(toggle);
}

void CTablesSetPanel::EnableEmbedMapWnd(bool embed, const CRect& rc)
{
 mp_ButtonsPanel->EnableEmbedMapWnd(embed, rc);
 mp_SeptabsPanel->EnableEmbedMapWnd(embed, rc);
}

void CTablesSetPanel::OnShow(bool show)
{
 mp_ButtonsPanel->OnShow(show);
 mp_SeptabsPanel->OnShow(show);
}

void CTablesSetPanel::SetLoadAxisCfg(float minVal, float maxVal, int loadSrc, bool useTable, bool forceUpdate /*= false*/)
{
 mp_ButtonsPanel->SetLoadAxisCfg(minVal, maxVal, loadSrc, useTable, forceUpdate);
 mp_SeptabsPanel->SetLoadAxisCfg(minVal, maxVal, loadSrc, useTable, forceUpdate);
}
