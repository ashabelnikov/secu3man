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

const UINT CIORemappingDlg::IDD = IDD_IO_REMAPPING;

//See also FirmwareDataMediator.h
const UINT IOCaptionStart = IDC_IO_REMAPPING_ECF_CAPTION;
const UINT IOCaptionEnd = IDC_IO_REMAPPING_FE_CAPTION;
const UINT IOComboboxStart = IDC_IO_REMAPPING_ECF_COMBOBOX;
const UINT IOComboboxEnd = IDC_IO_REMAPPING_FE_COMBOBOX;

BEGIN_MESSAGE_MAP(CIORemappingDlg, CDialog)
 ON_CONTROL_RANGE(CBN_SELCHANGE, IOComboboxStart, IOComboboxEnd, OnChangeSelection)
 ON_UPDATE_COMMAND_UI(IDC_IO_REMAPPING_ECF_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOCaptionStart,  IOCaptionEnd, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOComboboxStart,  IOComboboxEnd, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_IO_REMAPPING_CAPTION, OnUpdateControls)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIORemappingDlg dialog

CIORemappingDlg::CIORemappingDlg(CWnd* pParent /*=NULL*/)
: Super(CIORemappingDlg::IDD, pParent)
, m_enabled(false)
, m_enable_secu3t_features(false)
{
 //empty
}

void CIORemappingDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 for(size_t i = 0; i < FWDM::IOS_COUNT; ++i)
  DDX_Control(pDX, IOComboboxStart + i, m_iorcb[i]);
}

/////////////////////////////////////////////////////////////////////////////
// CIORemappingDlg message handlers

BOOL CIORemappingDlg::OnInitDialog()
{
 Super::OnInitDialog();

 UpdateDialogControls(this, TRUE);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CIORemappingDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 switch(pCmdUI->m_nID)
 {
  case IDC_IO_REMAPPING_ADD_IO1_COMBOBOX:
  case IDC_IO_REMAPPING_ADD_IO2_COMBOBOX:
  case IDC_IO_REMAPPING_ADD_IO1_CAPTION:
  case IDC_IO_REMAPPING_ADD_IO2_CAPTION:
   pCmdUI->Enable(m_enabled && m_enable_secu3t_features);
   break;
  default:
   pCmdUI->Enable(m_enabled);
 };
}

void CIORemappingDlg::OnChangeSelection(UINT nID)
{
 FWDM::IOSid index = (FWDM::IOSid)(nID - IOComboboxStart);
 if (index < FWDM::IOS_COUNT)
 {
  int selection = m_iorcb[index].GetCurSel();
  if (CB_ERR != selection)
  {
   FWDM::IOPid data = (FWDM::IOPid)m_iorcb[index].GetItemData(selection);
   if (m_OnItemSel)
    m_OnItemSel(index, data);
  }
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
 if (iosId >= FWDM::IOS_COUNT)
  return false;
 int iid = m_iorcb[iosId].AddString(i_text.c_str());
 if (CB_ERR == iid)
  return false;
 iid = m_iorcb[iosId].SetItemData(iid, iopId);
 if (CB_ERR == iid)
  return false;
 return true; //Ok 
}

bool CIORemappingDlg::SelectItem(FWDM::IOSid iosId, FWDM::IOPid iopId)
{
 if (iosId >= FWDM::IOS_COUNT)
  return false;

 int count = m_iorcb[iosId].GetCount();
 for(int i = 0; i < count; ++i)
 {
  DWORD data = m_iorcb[iosId].GetItemData(i);
  if (CB_ERR == data || data != iopId)
   continue;
  m_iorcb[iosId].SetCurSel(i);
  return true; //Ok
 }
 return false; //Not Ok
}

CIORemappingDlg::FWDM::IOPid CIORemappingDlg::GetSelection(FWDM::IOSid iosId) const
{
 int selection = m_iorcb[iosId].GetCurSel();
 return (FWDM::IOPid)m_iorcb[iosId].GetItemData(selection);
}

void CIORemappingDlg::setOnItemSelected(EventItemSel OnFunction)
{
 m_OnItemSel = OnFunction;
}

void CIORemappingDlg::EnableSECU3TItems(bool i_enable)
{
 m_enable_secu3t_features = i_enable;
 UpdateDialogControls(this,TRUE);
}
