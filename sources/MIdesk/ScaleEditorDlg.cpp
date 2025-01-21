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

/** \file ScaleEditorDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "ScaleEditorDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/ColorDialogEx.h"
#include "common/DPIAware.h"
#include "common/GDIHelpers.h"
#include "common/Dll.h"

const UINT ScaleEditorDlg::IDD = IDD_MI_SCALE_EDITOR;

#define WM_KICKIDLE 0x036A //hack

/////////////////////////////////////////////////////////////////////////////
// ScaleEditorDlg dialog

BEGIN_MESSAGE_MAP(ScaleEditorDlg, Super)
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
 ON_WM_LBUTTONDOWN()
 ON_NOTIFY(NM_CUSTOMDRAW, IDC_MI_SED_AZ_LIST, OnCustomdrawList)
 ON_NOTIFY(LVN_ITEMCHANGED, IDC_MI_SED_AZ_LIST, OnChangeAZList)
 ON_BN_CLICKED(IDC_MI_SED_AZ_DEL, OnAZDelete)
 ON_BN_CLICKED(IDC_MI_SED_AZ_ADD, OnAZAdd)
 ON_EN_CHANGE(IDC_MI_SED_AZ_BEGIN_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_MI_SED_AZ_END_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_MI_SED_SCALE_BEGIN_EDIT, OnChangeDataSc)
 ON_EN_CHANGE(IDC_MI_SED_SCALE_END_EDIT, OnChangeDataSc)
 ON_EN_CHANGE(IDC_MI_SED_SCALE_TICS_EDIT, OnChangeDataSc)
 ON_UPDATE_COMMAND_UI(IDC_MI_SED_AZ_LIST, OnUpdateMetCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MI_SED_AZ_ADD, OnUpdateMetCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MI_SED_AZ_DEL, OnUpdateAZDel)
 ON_UPDATE_COMMAND_UI(IDC_MI_SED_AZ_BEGIN_EDIT, OnUpdateAZEdit)
 ON_UPDATE_COMMAND_UI(IDC_MI_SED_AZ_BEGIN_SPIN, OnUpdateAZEdit)
 ON_UPDATE_COMMAND_UI(IDC_MI_SED_AZ_END_EDIT, OnUpdateAZEdit)
END_MESSAGE_MAP()

ScaleEditorDlg::ScaleEditorDlg(CWnd* pParent /*=NULL*/)
: Super(ScaleEditorDlg::IDD, pParent)
, m_meter(false)
, m_scale_begin_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_scale_end_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_scale_tics_edit(CEditEx::MODE_INT, true)
, m_az_begin_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_az_end_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_az_color(DLL::GetModuleHandle())
{
 //empty
}

ScaleEditorDlg::~ScaleEditorDlg()
{
 //empty
}

void ScaleEditorDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_MI_SED_AZ_ADD, m_az_add);
 DDX_Control(pDX, IDC_MI_SED_AZ_DEL, m_az_del);
 DDX_Control(pDX, IDC_MI_SED_SCALE_BEGIN_EDIT, m_scale_begin_edit);
 DDX_Control(pDX, IDC_MI_SED_SCALE_BEGIN_SPIN, m_scale_begin_spin);
 DDX_Control(pDX, IDC_MI_SED_SCALE_END_EDIT, m_scale_end_edit);
 DDX_Control(pDX, IDC_MI_SED_SCALE_END_SPIN, m_scale_end_spin);
 DDX_Control(pDX, IDC_MI_SED_SCALE_TICS_EDIT, m_scale_tics_edit);
 DDX_Control(pDX, IDC_MI_SED_SCALE_TICS_SPIN, m_scale_tics_spin);
 DDX_Control(pDX, IDC_MI_SED_AZ_LIST, m_az_list);
 DDX_Control(pDX, IDC_MI_SED_AZ_COLOR, m_az_color);
 DDX_Control(pDX, IDC_MI_SED_AZ_BEGIN_EDIT, m_az_begin_edit);
 DDX_Control(pDX, IDC_MI_SED_AZ_BEGIN_SPIN, m_az_begin_spin);
 DDX_Control(pDX, IDC_MI_SED_AZ_END_EDIT, m_az_end_edit);
 DDX_Control(pDX, IDC_MI_SED_AZ_END_SPIN, m_az_end_spin);

 m_scale_begin_edit.DDX_Value(pDX, IDC_MI_SED_SCALE_BEGIN_EDIT, m_cfg.scaleMin);
 m_scale_end_edit.DDX_Value(pDX, IDC_MI_SED_SCALE_END_EDIT, m_cfg.scaleMax);
 m_scale_tics_edit.DDX_Value(pDX, IDC_MI_SED_SCALE_TICS_EDIT, m_cfg.ticksNum);

 if (m_az_list.GetSelectedCount()==1)
 {
  POSITION pos = m_az_list.GetFirstSelectedItemPosition();
  if (pos)
  { 
   int sel = m_az_list.GetNextSelectedItem(pos);
   m_az_begin_edit.DDX_Value(pDX, IDC_MI_SED_AZ_BEGIN_EDIT, m_cfg.alezn[sel].start);
   m_az_end_edit.DDX_Value(pDX, IDC_MI_SED_AZ_END_EDIT, m_cfg.alezn[sel].end);
  }
 }
}

/////////////////////////////////////////////////////////////////////////////
// ScaleEditorDlg message handlers

void ScaleEditorDlg::OnOK()
{ 
 UpdateData(TRUE);
 Super::OnOK();
}

void ScaleEditorDlg::OnCancel()
{
 Super::OnCancel();
}

BOOL ScaleEditorDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CloneWndFont(this, &m_font, -1, false);  

 m_az_color.SetNumRows(1);
 m_az_color.AddItem(_T(""), RGB(127,127,127));
 m_az_color.SetItemState(0, false);
 m_az_color.SetFont(&m_font);

 DPIAware dpia;
 m_az_list.InsertColumn(0, MLL::LoadString(IDS_MI_SED_AZLH_START), LVCFMT_LEFT, dpia.ScaleX(80));
 m_az_list.InsertColumn(1, MLL::LoadString(IDS_MI_SED_AZLH_END), LVCFMT_LEFT, 80);
 m_az_list.InsertColumn(2, MLL::LoadString(IDS_MI_SED_AZLH_COLOR), LVCFMT_LEFT, 60);
 m_az_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

 //fill the list with data
 for(size_t i = 0; i < m_cfg.alezn.size(); ++i)
  AppendAZList(i, FormatValue(m_cfg.alezn[i].start), FormatValue(m_cfg.alezn[i].end), m_cfg.alezn[i].color);

 m_scale_begin_edit.SetLimitText(10);
 m_scale_begin_spin.SetBuddy(&m_scale_begin_edit);
 m_scale_begin_edit.SetDecimalPlaces(3);
 m_scale_begin_spin.SetRangeAndDelta(-3000, 30000, 10);
 m_scale_begin_edit.SetRange(-3000, 30000);

 m_scale_end_edit.SetLimitText(10);
 m_scale_end_spin.SetBuddy(&m_scale_end_edit);
 m_scale_end_edit.SetDecimalPlaces(3);
 m_scale_end_spin.SetRangeAndDelta(-3000, 30000, 10);
 m_scale_end_edit.SetRange(-3000, 30000);

 m_az_begin_edit.SetLimitText(10);
 m_az_begin_spin.SetBuddy(&m_az_begin_edit);
 m_az_begin_edit.SetDecimalPlaces(3);
 m_az_begin_spin.SetRangeAndDelta(-3000, 30000, 10);
 m_az_begin_edit.SetRange(-3000, 30000);

 m_az_end_edit.SetLimitText(10);
 m_az_end_spin.SetBuddy(&m_az_end_edit);
 m_az_end_edit.SetDecimalPlaces(3);
 m_az_end_spin.SetRangeAndDelta(-3000, 30000, 10);
 m_az_end_edit.SetRange(-3000, 30000);

 m_scale_tics_edit.SetLimitText(4);
 m_scale_tics_spin.SetBuddy(&m_scale_tics_edit);
 m_scale_tics_edit.SetDecimalPlaces(0);
 m_scale_tics_spin.SetRangeAndDelta(0, 100, 1);
 m_scale_tics_edit.SetRange(0, 100);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_scale_begin_edit, MLL::GetString(IDS_MI_SED_SCALE_BEGIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_scale_begin_spin, MLL::GetString(IDS_MI_SED_SCALE_BEGIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_scale_end_edit, MLL::GetString(IDS_MI_SED_SCALE_END_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_scale_end_spin, MLL::GetString(IDS_MI_SED_SCALE_END_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_az_add, MLL::GetString(IDS_MI_SED_AZ_ADD_TT)));
 VERIFY(mp_ttc->AddWindow(&m_az_del, MLL::GetString(IDS_MI_SED_AZ_DEL_TT)));
 VERIFY(mp_ttc->AddWindow(&m_scale_tics_edit, MLL::GetString(IDS_MI_SED_SCALE_TICS_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_scale_tics_spin, MLL::GetString(IDS_MI_SED_SCALE_TICS_EDIT_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT ScaleEditorDlg::OnKickIdle(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
 UpdateDialogControls(this, true);
 return FALSE;
}

INT_PTR ScaleEditorDlg::DoModal()
{
 INT_PTR result = Super::DoModal();
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}

void ScaleEditorDlg::OnUpdateMetCtrls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_meter);
}

void ScaleEditorDlg::OnUpdateAZDel(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_meter && m_az_list.GetSelectedCount());
}

void ScaleEditorDlg::OnUpdateAZEdit(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_meter && m_az_list.GetSelectedCount()==1);
}

void ScaleEditorDlg::OnChangeData()
{
 UpdateData();
 if (m_az_list.GetSelectedCount()==1)
 {
  POSITION pos = m_az_list.GetFirstSelectedItemPosition();
  if (pos)
  { 
   int sel = m_az_list.GetNextSelectedItem(pos);
   m_az_list.SetItemText(sel, 0, FormatValue(m_cfg.alezn[sel].start).c_str());
   m_az_list.SetItemText(sel, 1, FormatValue(m_cfg.alezn[sel].end).c_str());
  }
 }
}

void ScaleEditorDlg::OnChangeDataSc()
{
 UpdateData();
}

void ScaleEditorDlg::SetData(const MetCfg& cfg, bool meter)
{
 m_cfg = cfg;
 m_meter = meter;
}

void ScaleEditorDlg::GetData(MetCfg& cfg) const
{
 cfg = m_cfg;
}

void ScaleEditorDlg::AppendAZList(int index, const _TSTRING& i_start, const _TSTRING& i_end, COLORREF color)
{
 m_az_list.InsertItem(index, i_start.c_str());
 m_az_list.SetItemText(index, 1, i_end.c_str());
 m_az_list.SetItemText(index, 2, FormatColor(color).c_str());
 m_az_list.SetItemData(index, color);
}

void ScaleEditorDlg::OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

 // Take the default processing unless 
 // we set this to something else below.
 *pResult = CDRF_DODEFAULT;

 // First thing - check the draw stage. If it's the control's prepaint
 // stage, then tell Windows we want messages for every item.
 if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
 {
  *pResult = CDRF_NOTIFYITEMDRAW;
 }
 else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
 {
  // This is the notification message for an item.  We'll request
  // notifications before each subitem's prepaint stage.
  *pResult = CDRF_NOTIFYSUBITEMDRAW;
 }
 else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
 {            
  // This is the prepaint stage for a subitem. Here's where we set the
  // item's text and background colors. Our return value will tell 
  // Windows to draw the subitem itself, but it will use the new colors
  // we set here.
  if (2 == pLVCD->iSubItem)
  {
   pLVCD->clrTextBk = pLVCD->nmcd.lItemlParam;
  }

  // Tell Windows to paint the control itself.
  *pResult = CDRF_DODEFAULT;
 }
}

void ScaleEditorDlg::OnChangeAZList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR; 
 if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED || pnmv->uOldState & LVIS_SELECTED))
 {
  int sel_idx = m_az_list.GetNextItem(-1, LVNI_SELECTED);
  if (sel_idx != -1)
  {
   UpdateData(FALSE);
   m_az_color.SetItemState(0, true);
   m_az_color.SetItemColor(0, m_cfg.alezn[sel_idx].color);
  }
  else
  {
   m_az_color.SetItemState(0, false);
  }
 }
 else
 {
  if (m_az_list.GetSelectedCount()!=1)
   m_az_color.SetItemState(0, false);
 }

 *pResult = 0;
}

void ScaleEditorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
 //indicators
 CPoint ptscr = point;
 ClientToScreen(&ptscr);
 CRect rc = m_az_color.GetItemRect(0);
 m_az_color.ClientToScreen(&rc);
 if (rc.PtInRect(ptscr))
 {
  if (m_az_list.GetSelectedCount()==1)
  {
   CColorDialogEx dlg; 
   if (dlg.DoModal() == IDOK) 
   { 
    COLORREF color = dlg.GetColor(); 
    m_az_color.SetItemColor(0, color);

    POSITION pos = m_az_list.GetFirstSelectedItemPosition();
    if (pos)
    { 
     int sel = m_az_list.GetNextSelectedItem(pos);
     m_cfg.alezn[sel].color = color;   
     m_az_list.SetItemData(sel, color);
     m_az_list.SetItemText(sel, 2, FormatColor(color).c_str());
    }
   }
  }
 }

 Super::OnLButtonDown(nFlags, point);
}

void ScaleEditorDlg::OnAZAdd()
{
 POSITION pos = m_az_list.GetFirstSelectedItemPosition();
 AlertZone az = {0.0f, 0.0f, RGB(127, 127, 127)};
 if (pos == NULL)
 {//No items selected!
  int count = m_az_list.GetItemCount();
  AppendAZList(count,  FormatValue(az.start), FormatValue(az.end), az.color);
  //add to the end of the vector...
  m_cfg.alezn.push_back(az);
 }
 else
 {
  int index = 0;
  while(pos)
  { //find last selected item
   index = m_az_list.GetNextSelectedItem(pos);
  }

  std::vector<AlertZone> azv;
  for(size_t i = 0; i <= (size_t)index; ++i)
   azv.push_back(m_cfg.alezn[i]);
  azv.push_back(az); //new item
  for(size_t i = index+1; i < m_cfg.alezn.size(); ++i)
   azv.push_back(m_cfg.alezn[i]);
  m_cfg.alezn = azv;  
      
  AppendAZList(index+1, FormatValue(az.start), FormatValue(az.end), az.color);
 }
}

void ScaleEditorDlg::OnAZDelete()
{
 POSITION pos = m_az_list.GetFirstSelectedItemPosition();
 if (pos == NULL)
  return;  //No items selected!

 //build a list of items to be deleted
 std::vector<int> todel;
 while(pos)
 {
  int index = m_az_list.GetNextSelectedItem(pos);
  todel.push_back(index);
 }
 //delete items from vector: build a new vector, excluding items marked as "to be deleted".
 std::vector<AlertZone> azv;
 for(size_t i = 0; i < m_cfg.alezn.size(); ++i)
 {
  if (std::find(todel.begin(), todel.end(), i)==todel.end())
   azv.push_back(m_cfg.alezn[i]);
 }
 m_cfg.alezn = azv;
 
 //delete items from the list control
 pos = m_az_list.GetFirstSelectedItemPosition();
 while(pos)
 {
  int index = m_az_list.GetNextSelectedItem(pos);
  m_az_list.DeleteItem(index);
  pos = m_az_list.GetFirstSelectedItemPosition();
  if (pos == NULL)
   return;  //No items selected!
 }
}

_TSTRING ScaleEditorDlg::FormatValue(float val)
{
 CString str;
 str.Format(_T("%03f"), val);
 return _TSTRING(str);
}

_TSTRING ScaleEditorDlg::FormatColor(COLORREF val)
{
 CString str;
 str.Format(_T("%06X"), GDIHelpers::swapRB(val));
 return _TSTRING(str);
}
