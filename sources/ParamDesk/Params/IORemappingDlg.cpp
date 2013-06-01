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
#include "IORemappingDlg.h"
#include "ui-core/ToolTipCtrlEx.h"

namespace {
CComboBox* _GetCBbyIOSID(const std::map<UINT, std::pair<int, CComboBox*> >& map, int iosId)
{
 std::map<UINT, std::pair<int, CComboBox*> >::const_iterator it;
 for(it = map.begin(); it != map.end(); ++it)
  if (it->second.first == iosId)
   return it->second.second;
 return NULL;
}
}

const UINT CIORemappingDlg::IDD = IDD_IO_REMAPPING;

//See also FirmwareDataMediator.h
const UINT IOCaptionStart = IDC_IO_REMAPPING_IGN_OUT1_CAPTION;
const UINT IOCaptionEnd = IDC_IO_REMAPPING_DE_CAPTION;
const UINT IOComboboxStart = IDC_IO_REMAPPING_IGN_OUT1_COMBOBOX;
const UINT IOComboboxEnd = IDC_IO_REMAPPING_DE_COMBOBOX;
const UINT IOCheckboxStart = IDC_IO_REMAPPING_IGN_OUT1_CHECKBOX;
const UINT IOCheckboxEnd = IDC_IO_REMAPPING_DE_CHECKBOX;
const UINT IOTTStrStart = IDS_IO_REMAPPING_IGN_OUT1_TT;
const UINT IOTTStrEnd = IDS_IO_REMAPPING_DE_TT;

BEGIN_MESSAGE_MAP(CIORemappingDlg, CDialog)
 ON_CONTROL_RANGE(CBN_SELCHANGE, IOComboboxStart, IOComboboxEnd, OnChangeSelection)
 ON_CONTROL_RANGE(BN_CLICKED, IOCheckboxStart, IOCheckboxEnd, OnChangeInversion)
 ON_UPDATE_COMMAND_UI_RANGE(IOCaptionStart,  IOCaptionEnd, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOComboboxStart,  IOComboboxEnd, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOCheckboxStart,  IOCheckboxEnd, OnUpdateControlsChecks)
 ON_UPDATE_COMMAND_UI(IDC_IO_REMAPPING_CAPTION, OnUpdateControlsCommon)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIORemappingDlg dialog

CIORemappingDlg::CIORemappingDlg(CWnd* pParent /*=NULL*/)
: Super(CIORemappingDlg::IDD, pParent)
, m_enabled(false)
, m_enable_secu3t_features(false)
{
 UINT ctrlId = 0;
 for(size_t i = FWDM::IOS_START; i < FWDM::IOS_COUNT; ++i)
 {
  //Exclude ADD_I1 and ADD_I2 because they have not to appear in the UI
  if ((i != FWDM::IOS_ADD_I1) && (i != FWDM::IOS_ADD_I2))
  {
   std::pair<int, CComboBox*> cb_info;
   cb_info.first = FWDM::IOS_START + i; //slot ID
   cb_info.second = new CComboBox();  //MFC object
   std::pair<int, CButton*> cm_info;
   cm_info.first = FWDM::IOS_START + i; //slot ID
   cm_info.second = new CButton();    //MFC object
   m_iorcb.insert(std::make_pair(IOComboboxStart + ctrlId, cb_info));
   m_iorcm.insert(std::make_pair(IOCheckboxStart + ctrlId, cm_info));
   m_enflg.insert(std::make_pair(IOComboboxStart + ctrlId++, std::make_pair(false, false)));
  }
 }
}

CIORemappingDlg::~CIORemappingDlg()
{
 std::map<UINT, std::pair<int, CComboBox*> >::iterator cb;
 for(cb = m_iorcb.begin(); cb != m_iorcb.end(); ++cb)
  delete (cb->second.second);
 std::map<UINT, std::pair<int, CButton*> >::iterator cm;
 for(cm = m_iorcm.begin(); cm != m_iorcm.end(); ++cm)
  delete (cm->second.second);
}

void CIORemappingDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 std::map<UINT, std::pair<int, CComboBox*> >::iterator cb;
 for(cb = m_iorcb.begin(); cb != m_iorcb.end(); ++cb)
  DDX_Control(pDX, cb->first, *(cb->second.second));
 std::map<UINT, std::pair<int, CButton*> >::iterator cm;
 for(cm = m_iorcm.begin(); cm != m_iorcm.end(); ++cm)
  DDX_Control(pDX, cm->first, *(cm->second.second));
}

/////////////////////////////////////////////////////////////////////////////
// CIORemappingDlg message handlers

BOOL CIORemappingDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 UINT ttStrId = IOTTStrStart;
 UINT cbObjId = IOComboboxStart;
 for(; cbObjId <= IOComboboxEnd; ++cbObjId, ++ttStrId)
  VERIFY(mp_ttc->AddWindow(m_iorcb[cbObjId].second, MLL::GetString(ttStrId)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CIORemappingDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool enable_secu3t_features = true;
 switch(pCmdUI->m_nID)
 {
  case IDC_IO_REMAPPING_ADD_IO1_COMBOBOX:
  case IDC_IO_REMAPPING_ADD_IO2_COMBOBOX:
  case IDC_IO_REMAPPING_ADD_IO1_CAPTION:
  case IDC_IO_REMAPPING_ADD_IO2_CAPTION:
   enable_secu3t_features = m_enable_secu3t_features;
   break;
 };
 
 UINT id = pCmdUI->m_nID;
 if (pCmdUI->m_nID >= IOCaptionStart && pCmdUI->m_nID <= IOCaptionEnd)
  id = IOComboboxStart + (pCmdUI->m_nID - IOCaptionStart);
 pCmdUI->Enable(m_enabled && enable_secu3t_features && m_enflg[id].first);
}

void CIORemappingDlg::OnUpdateControlsChecks(CCmdUI* pCmdUI)
{
 bool enable_secu3t_features = true;
 switch(pCmdUI->m_nID)
 {
  case IDC_IO_REMAPPING_ADD_IO1_CHECKBOX:
  case IDC_IO_REMAPPING_ADD_IO2_CHECKBOX:
   enable_secu3t_features = m_enable_secu3t_features;
   break;
 };
 
 UINT id = pCmdUI->m_nID;
 if (pCmdUI->m_nID >= IOCheckboxStart && pCmdUI->m_nID <= IOCheckboxEnd)
  id = IOComboboxStart + (pCmdUI->m_nID - IOCheckboxStart);
 pCmdUI->Enable(m_enabled && enable_secu3t_features && m_enflg[id].first && m_enflg[id].second);
}

void CIORemappingDlg::OnUpdateControlsCommon(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CIORemappingDlg::OnChangeSelection(UINT nID)
{ 
 if (m_iorcb.find(nID) != m_iorcb.end())
 {
  int selection = m_iorcb[nID].second->GetCurSel();
  if (CB_ERR != selection)
  {
   FWDM::IOPid data = (FWDM::IOPid)m_iorcb[nID].second->GetItemData(selection);
   if (m_OnItemSel)
    m_OnItemSel((FWDM::IOSid)m_iorcb[nID].first, data);
  }
 }
}

void CIORemappingDlg::OnChangeInversion(UINT nID)
{ 
 std::map<UINT, std::pair<int, CButton*> >::const_iterator it = m_iorcm.find(nID);
 if (it != m_iorcm.end())
 {
  if (m_OnItemInv)
   m_OnItemInv((FWDM::IOSid)it->second.first, it->second.second->GetCheck() == BST_CHECKED);
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
 m_enable_secu3t_features = i_enable;
 UpdateDialogControls(this, TRUE);
}

void CIORemappingDlg::EnableItem(FWDM::IOSid iosId, bool i_enable)
{
 std::map<UINT, std::pair<int, CComboBox*> >::iterator it;
 for(it = m_iorcb.begin(); it != m_iorcb.end(); ++it)
 {
  if (it->second.first == iosId)
  {
   m_enflg[it->first].first = i_enable;
   UpdateDialogControls(this, TRUE);
   return;
  }
 }
}

void CIORemappingDlg::EnableInversion(FWDM::IOSid iosId, bool i_invert)
{
 std::map<UINT, std::pair<int, CComboBox*> >::iterator it;
 for(it = m_iorcb.begin(); it != m_iorcb.end(); ++it)
 {
  if (it->second.first == iosId)
  {
   m_enflg[it->first].second = i_invert;
   UpdateDialogControls(this, TRUE);
   return;
  }
 }
}

void CIORemappingDlg::ResetContent(void)
{
 std::map<UINT, std::pair<int, CComboBox*> >::iterator cb;
 for(cb = m_iorcb.begin(); cb != m_iorcb.end(); ++cb)
  cb->second.second->ResetContent();
 std::map<UINT, std::pair<int, CButton*> >::iterator cm;
 for(cm = m_iorcm.begin(); cm != m_iorcm.end(); ++cm)
  cm->second.second->SetCheck(BST_UNCHECKED);
 std::map<UINT, std::pair<bool, bool> >::iterator fg;
 for(fg = m_enflg.begin(); fg != m_enflg.end(); ++fg)
  fg->second.first = fg->second.second = false;
}

bool CIORemappingDlg::InvertItem(FWDM::IOSid iosId, bool i_invert)
{
 std::map<UINT, std::pair<int, CButton*> >::const_iterator it = m_iorcm.begin();
 for(; it != m_iorcm.end(); ++it)
  if (it->second.first == iosId)
  {
   it->second.second->SetCheck(i_invert ? BST_CHECKED : BST_UNCHECKED);
   return true;
  }
 return false;
}

void CIORemappingDlg::setOnItemInverted(EventItemInv OnFunction)
{
 m_OnItemInv = OnFunction;
}
