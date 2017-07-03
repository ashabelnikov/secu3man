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

/** \file IORemappingDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "IORemappingDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

CComboBox* CIORemappingDlg::_GetCBbyIOSID(const std::map<UINT, int>& map, int iosId) const
{
 std::map<UINT, int>::const_iterator it;
 for(it = map.begin(); it != map.end(); ++it)
  if (it->second == iosId)
   return (CComboBox*)GetDlgItem(it->first);
 return NULL;
}

const UINT CIORemappingDlg::IDD = IDD_IO_REMAPPING;

//See also FirmwareDataMediator.h
const UINT IOCaptionStart = IDC_IO_REMAPPING_IGN_OUT1_CAPTION;
const UINT IOCaptionEnd = IDC_IO_REMAPPING_CKPS_CAPTION;
const UINT IOCaptionStart3I = IDC_IO_REMAPPING3I_IGN_O1_CAPTION;
const UINT IOCaptionEnd3I = IDC_IO_REMAPPING3I_EPAS_I_CAPTION;

const UINT IOComboboxStart = IDC_IO_REMAPPING_IGN_OUT1_COMBOBOX;
const UINT IOComboboxEnd = IDC_IO_REMAPPING_CKPS_COMBOBOX;
const UINT IOComboboxStart3I = IDC_IO_REMAPPING3I_IGN_O1_COMBOBOX;
const UINT IOComboboxEnd3I = IDC_IO_REMAPPING3I_EPAS_I_COMBOBOX;

const UINT IOCheckboxStart = IDC_IO_REMAPPING_IGN_OUT1_CHECKBOX;
const UINT IOCheckboxEnd = IDC_IO_REMAPPING_CKPS_CHECKBOX;
const UINT IOCheckboxStart3I = IDC_IO_REMAPPING3I_IGN_O1_CHECKBOX;
const UINT IOCheckboxEnd3I = IDC_IO_REMAPPING3I_EPAS_I_CHECKBOX;

const UINT IOTTStrStart = IDS_IO_REMAPPING_IGN_OUT1_TT;
const UINT IOTTStrEnd = IDS_IO_REMAPPING_CKPS_TT;
const UINT IOTTStrStart3I = IDS_IO_REMAPPING3I_IGN_O1_TT;
const UINT IOTTStrEnd3I = IDS_IO_REMAPPING3I_EPAS_I_TT;

BEGIN_MESSAGE_MAP(CIORemappingDlg, CModelessDialog)
 ON_CONTROL_RANGE(CBN_SELCHANGE, IOComboboxStart, IOComboboxEnd, OnChangeSelection)
 ON_CONTROL_RANGE(BN_CLICKED, IOCheckboxStart, IOCheckboxEnd, OnChangeInversion)

 ON_CONTROL_RANGE(CBN_SELCHANGE, IOComboboxStart3I, IOComboboxEnd3I, OnChangeSelection)
 ON_CONTROL_RANGE(BN_CLICKED, IOCheckboxStart3I, IOCheckboxEnd3I, OnChangeInversion)

 ON_UPDATE_COMMAND_UI_RANGE(IOCaptionStart,  IOCaptionEnd, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOComboboxStart,  IOComboboxEnd, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOCheckboxStart,  IOCheckboxEnd, OnUpdateControlsChecks)

 ON_UPDATE_COMMAND_UI_RANGE(IOCaptionStart3I,  IOCaptionEnd3I, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOComboboxStart3I,  IOComboboxEnd3I, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOCheckboxStart3I,  IOCheckboxEnd3I, OnUpdateControlsChecks)

 ON_UPDATE_COMMAND_UI(IDC_IO_REMAPPING_CAPTION, OnUpdateControlsCommon)
 ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIORemappingDlg dialog

CIORemappingDlg::CIORemappingDlg(CWnd* pParent /*=NULL*/)
: Super(CIORemappingDlg::IDD, pParent)
, m_enabled(false)
, m_enable_secu3t_features(false)
, mp_scr(new CWndScroller)
{
 _FillControls();
}

CIORemappingDlg::~CIORemappingDlg()
{
 //empty
}

/////////////////////////////////////////////////////////////////////////////
// CIORemappingDlg message handlers

BOOL CIORemappingDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //create a tooltip control and assign tooltips
 _SetTooltips(); 

 //initialize window scroller
 mp_scr->Init(this);
 mp_scr->SetViewSizeF(.0f, m_enable_secu3t_features ? 1.32f : 2.15f);

 _ShowControls();

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CIORemappingDlg::OnDestroy()
{
 mp_scr->Close();
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CIORemappingDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 UINT CaptionStart = m_enable_secu3t_features ? IOCaptionStart : IOCaptionStart3I;
 UINT CaptionEnd = m_enable_secu3t_features ? IOCaptionEnd : IOCaptionEnd3I;
 UINT ComboboxStart = m_enable_secu3t_features ? IOComboboxStart : IOComboboxStart3I;
 UINT id = pCmdUI->m_nID;
 if (pCmdUI->m_nID >= CaptionStart && pCmdUI->m_nID <= CaptionEnd)
  id = ComboboxStart + (pCmdUI->m_nID - CaptionStart);

 pCmdUI->Enable(m_enabled && m_enflg[id].first);
}

void CIORemappingDlg::OnUpdateControlsChecks(CCmdUI* pCmdUI)
{
 UINT CheckboxStart = m_enable_secu3t_features ? IOCheckboxStart : IOCheckboxStart3I;
 UINT CheckboxEnd = m_enable_secu3t_features ? IOCheckboxEnd : IOCheckboxEnd3I;
 UINT ComboboxStart = m_enable_secu3t_features ? IOComboboxStart : IOComboboxStart3I;
 UINT id = pCmdUI->m_nID;
 if (pCmdUI->m_nID >= CheckboxStart && pCmdUI->m_nID <= CheckboxEnd)
  id = ComboboxStart + (pCmdUI->m_nID - CheckboxStart);
 pCmdUI->Enable(m_enabled && m_enflg[id].first && m_enflg[id].second);
}

void CIORemappingDlg::OnUpdateControlsCommon(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CIORemappingDlg::OnChangeSelection(UINT nID)
{ 
 if (m_iorcb.find(nID) != m_iorcb.end())
 {
  int selection = ((CComboBox*)GetDlgItem(nID))->GetCurSel();
  if (CB_ERR != selection)
  {
   FWDM::IOPid data = (FWDM::IOPid)((CComboBox*)GetDlgItem(nID))->GetItemData(selection);
   if (m_OnItemSel)
    m_OnItemSel((FWDM::IOSid)m_iorcb[nID], data);
  }
 }
}

void CIORemappingDlg::OnChangeInversion(UINT nID)
{ 
 std::map<UINT, int>::const_iterator it = m_iorcm.find(nID);
 if (it != m_iorcm.end())
 {
  if (m_OnItemInv)
   m_OnItemInv((FWDM::IOSid)it->second, ((CButton*)GetDlgItem(it->first))->GetCheck() == BST_CHECKED);
 }
}

void CIORemappingDlg::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after,x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

bool CIORemappingDlg::IsEnabled(void)
{
 return m_enabled;
}

void CIORemappingDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;

 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

void CIORemappingDlg::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 this->ShowWindow(nCmdShow);
}
 
bool CIORemappingDlg::AddItem(FWDM::IOSid iosId, FWDM::IOPid iopId, const _TSTRING& i_text)
{ 
 CComboBox* cb = _GetCBbyIOSID(m_iorcb, iosId);
 if (!cb)
  return false;
 int iid = cb->AddString(i_text.c_str());
 if (CB_ERR == iid)
  return false;
 iid = cb->SetItemData(iid, iopId);
 if (CB_ERR == iid)
  return false;
 return true; //Ok 
}

bool CIORemappingDlg::SelectItem(FWDM::IOSid iosId, FWDM::IOPid iopId)
{
 CComboBox* cb = _GetCBbyIOSID(m_iorcb, iosId);
 if (!cb)
  return false;
 if (iopId != FWDM::IOP_NA)
 {
  int count = cb->GetCount();
  for(int i = 0; i < count; ++i)
  {
   DWORD data = cb->GetItemData(i);
   if (CB_ERR == data || data != iopId)
    continue;
   cb->SetCurSel(i);
   return true; //Ok
  }
 }
 else
  cb->SetCurSel(-1);
 return false; //Not Ok
}

CIORemappingDlg::FWDM::IOPid CIORemappingDlg::GetSelection(FWDM::IOSid iosId) const
{
 CComboBox* cb = _GetCBbyIOSID(m_iorcb, iosId);
 ASSERT(cb);
 if (cb)
 {
  int selection = cb->GetCurSel();
  return (FWDM::IOPid)cb->GetItemData(selection);
 }
 return FWDM::IOP_NA; //error
}

void CIORemappingDlg::setOnItemSelected(EventItemSel OnFunction)
{
 m_OnItemSel = OnFunction;
}

void CIORemappingDlg::EnableSECU3TItems(bool i_enable)
{
 m_enable_secu3t_features = i_enable; //update controls flag

 mp_scr->SetViewSizeF(.0f, i_enable ? 1.32f : 2.15f);

 _FillControls();
 _SetTooltips();
 _ShowControls();

 UpdateDialogControls(this, TRUE);
}

void CIORemappingDlg::EnableItem(FWDM::IOSid iosId, bool i_enable)
{
 std::map<UINT, int>::iterator it;
 for(it = m_iorcb.begin(); it != m_iorcb.end(); ++it)
 {
  if (it->second == iosId)
  {
   m_enflg[it->first].first = i_enable;
   UpdateDialogControls(this, TRUE);
   return;
  }
 }
}

void CIORemappingDlg::EnableInversion(FWDM::IOSid iosId, bool i_invert)
{
 std::map<UINT, int>::iterator it;
 for(it = m_iorcb.begin(); it != m_iorcb.end(); ++it)
 {
  if (it->second == iosId)
  {
   m_enflg[it->first].second = i_invert;
   UpdateDialogControls(this, TRUE);
   return;
  }
 }
}

void CIORemappingDlg::ResetContent(void)
{
 std::map<UINT, int>::iterator cb;
 for(cb = m_iorcb.begin(); cb != m_iorcb.end(); ++cb)
  ((CComboBox*)GetDlgItem(cb->first))->ResetContent();
 std::map<UINT, int>::iterator cm;
 for(cm = m_iorcm.begin(); cm != m_iorcm.end(); ++cm)
  ((CButton*)GetDlgItem(cm->first))->SetCheck(BST_UNCHECKED);
 std::map<UINT, std::pair<bool, bool> >::iterator fg;
 for(fg = m_enflg.begin(); fg != m_enflg.end(); ++fg)
  fg->second.first = fg->second.second = false;
}

bool CIORemappingDlg::InvertItem(FWDM::IOSid iosId, bool i_invert)
{
 std::map<UINT, int>::const_iterator it = m_iorcm.begin();
 for(; it != m_iorcm.end(); ++it)
  if (it->second == iosId)
  {
   ((CButton*)GetDlgItem(it->first))->SetCheck(i_invert ? BST_CHECKED : BST_UNCHECKED);
   return true;
  }
 return false;
}

void CIORemappingDlg::setOnItemInverted(EventItemInv OnFunction)
{
 m_OnItemInv = OnFunction;
}

void CIORemappingDlg::_FillControls(void)
{
 m_iorcb.clear();
 m_iorcm.clear();
 m_enflg.clear();

 size_t iosCount  = m_enable_secu3t_features ? FWDM::IOS_COUNT : FWDM::IOS3I_COUNT;
 UINT ComboboxStart = m_enable_secu3t_features ? IOComboboxStart : IOComboboxStart3I;
 UINT CheckboxStart = m_enable_secu3t_features ? IOCheckboxStart : IOCheckboxStart3I;

 UINT ctrlId = 0;
 for(size_t i = FWDM::IOS_START; i < iosCount; ++i)
 {
  m_iorcb.insert(std::make_pair(ComboboxStart + ctrlId, FWDM::IOS_START + i));
  m_iorcm.insert(std::make_pair(CheckboxStart + ctrlId, FWDM::IOS_START + i));
  m_enflg.insert(std::make_pair(ComboboxStart + ctrlId++, std::make_pair(false, false)));
 }
}

void CIORemappingDlg::_SetTooltips(void)
{
 if (!mp_ttc.get())
 {
  mp_ttc.reset(new CToolTipCtrlEx());
  VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 }

 UINT ttStrId = m_enable_secu3t_features ? IOTTStrStart : IOTTStrStart3I;
 UINT cbObjId = m_enable_secu3t_features ? IOComboboxStart : IOComboboxStart3I;
 UINT cbObjEnd = m_enable_secu3t_features ? IOComboboxEnd : IOComboboxEnd3I;
 
 for(; cbObjId <= cbObjEnd; ++cbObjId, ++ttStrId)
  VERIFY(mp_ttc->AddWindow(GetDlgItem(cbObjId), MLL::GetString(ttStrId)));
 
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);
}

void CIORemappingDlg::_ShowControls(void)
{
 //SECU-3T
 for(int i = IOCheckboxStart; i <= IOCheckboxEnd; ++i)
  GetDlgItem(i)->ShowWindow(m_enable_secu3t_features ? SW_SHOW : SW_HIDE);
 for(int i = IOComboboxStart; i <= IOComboboxEnd; ++i)
  GetDlgItem(i)->ShowWindow(m_enable_secu3t_features ? SW_SHOW : SW_HIDE);
 for(int i = IOCaptionStart; i <= IOCaptionEnd; ++i)
  GetDlgItem(i)->ShowWindow(m_enable_secu3t_features ? SW_SHOW : SW_HIDE);
 //SECU-3i
 for(int i = IOCheckboxStart3I; i <= IOCheckboxEnd3I; ++i)
  GetDlgItem(i)->ShowWindow(m_enable_secu3t_features ? SW_HIDE : SW_SHOW);
 for(int i = IOComboboxStart3I; i <= IOComboboxEnd3I; ++i)
  GetDlgItem(i)->ShowWindow(m_enable_secu3t_features ? SW_HIDE : SW_SHOW);
 for(int i = IOCaptionStart3I; i <= IOCaptionEnd3I; ++i)
  GetDlgItem(i)->ShowWindow(m_enable_secu3t_features ? SW_HIDE : SW_SHOW); 
}
