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

/** \file EditSettingsDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <vector>
#include "Resources/resource.h"
#include "EditSettingsDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/MsgBox.h"

const UINT CEditSettingsDlg::IDD = IDD_APP_SETTINGS_EDITOR;

CEditSettingsDlg::CEditSettingsDlg(const _TSTRING& filename, bool syntaxHighlight /*=true*/, CWnd* pParent /*=NULL*/)
: CDialog(CEditSettingsDlg::IDD, pParent)
, m_filename(filename)
, m_syntaxHighlight(syntaxHighlight)
{
 //empty
}

void CEditSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_INI_RICHEDIT, m_edit);
}


BEGIN_MESSAGE_MAP(CEditSettingsDlg, CDialog)
END_MESSAGE_MAP()

BOOL CEditSettingsDlg::OnInitDialog()
{
 Super::OnInitDialog();
    
 //read contents of file:
 CFile f;
 CFileException ex;
 TCHAR szError[1024];
 if(!f.Open(m_filename.c_str(), CFile::modeRead, &ex))
 {
  ex.GetErrorMessage(szError, 1024);
  SECUMessageBox(szError);
 }
 else
 { //read contents
  unsigned int size = (unsigned int)f.GetLength();
  std::vector<TCHAR> buff(size+2, 0);
  f.Read(&buff[0], size);
  f.Close();
  m_edit.SetWindowText(&buff[0]);
  //highlight syntax:
  if (m_syntaxHighlight)
   _HighlightSyntax();
 }
 
 GetDlgItem(IDOK)->SetFocus();

 UpdateData(FALSE);
 return FALSE;  // return TRUE unless you set the focus to a control
}

void CEditSettingsDlg::OnCancel()
{
 Super::OnCancel();
}

void CEditSettingsDlg::OnOK()
{
 CFile f;
 CFileException ex;
 TCHAR szError[1024];
 if(!f.Open(m_filename.c_str(), CFile::modeWrite | CFile::modeCreate, &ex))
 {
  ex.GetErrorMessage(szError, 1024);
  SECUMessageBox(szError);
 }
 else
 { //write contents
  CString str;
  m_edit.GetWindowText(str);
  f.Write((LPCTSTR)str, str.GetLength());
  f.Close();
 }
 Super::OnOK(); 
}

void CEditSettingsDlg::_HighlightSyntax(void)
{
 CString buff;
 m_edit.GetWindowText(buff);
 m_edit.SetRedraw(FALSE);
 m_edit.HideSelection(TRUE, TRUE);

 int sectS = -1, varS = -1, commS = -1;
 int idx = 0;
 bool newline = true;

 CHARFORMAT cf_comm;
 cf_comm.cbSize = sizeof(cf_comm);
 cf_comm.dwMask = CFM_STRIKEOUT | CFM_COLOR | CFM_CHARSET;
 cf_comm.dwEffects = 0;
 cf_comm.bCharSet = RUSSIAN_CHARSET;
 cf_comm.crTextColor = RGB(200,200,150);

 CHARFORMAT cf_sect;
 cf_sect.cbSize = sizeof(cf_sect);
 cf_sect.dwMask = CFM_STRIKEOUT | CFM_BOLD | CFM_CHARSET;
 cf_comm.bCharSet = RUSSIAN_CHARSET;
 cf_sect.dwEffects = CFE_BOLD;

 CHARFORMAT cf_var;
 cf_var.cbSize = sizeof(cf_var);
 cf_var.dwMask = CFM_STRIKEOUT | CFM_COLOR | CFM_CHARSET;
 cf_comm.bCharSet = RUSSIAN_CHARSET;
 cf_var.dwEffects = 0;
 cf_var.crTextColor = RGB(0,0,255);


 for(int i = 0; i < buff.GetLength(); ++i)
 {
  if (buff[i] == _T('\n'))
  {
   if (commS != -1)
   { //highlight comments
    m_edit.SetSel(commS, idx);
    m_edit.SetSelectionCharFormat(cf_comm);     
    commS = -1;
   }
   newline = true;
  }

  else if (buff[i]== _T('[') && newline)
  {
   sectS = idx;
   newline = false;
  }
  else if (buff[i] == _T(']') && sectS != -1)
  {//highlight sections
   m_edit.SetSel(sectS, idx+1);
   m_edit.SetSelectionCharFormat(cf_sect);   
   sectS = -1;
  }
  else if (buff[i] != _T(';') && buff[i] != _T('=') && newline)
  {
   varS = idx;
   newline = false;
  }
  else if (buff[i] == _T('=') && varS != -1)
  { //highlight variables
   m_edit.SetSel(varS, idx);
   m_edit.SetSelectionCharFormat(cf_var);   
   varS = -1;
  }
  else if (buff[i] == _T(';'))
  {
   commS = idx;
   newline = false;
  }
  else
   newline = false;
   
  if (buff[i] != _T('\r'))
   idx++;  
 }
 m_edit.SetRedraw(TRUE);
 m_edit.HideSelection(TRUE, FALSE);
}
