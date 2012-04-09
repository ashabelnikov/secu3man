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


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT CIORemappingDlg::IDD = IDD_IO_REMAPPING;

const UINT IOCaptionStart = IDC_IO_REMAPPING_IGN_OUT3_CAPTION;
const UINT IOCaptionEnd = IDC_IO_REMAPPING_ST_BLOCK_CAPTION;
const UINT IOComboboxStart = IDC_IO_REMAPPING_IGN_OUT3_COMBOBOX;
const UINT IOComboboxEnd = IDC_IO_REMAPPING_ST_BLOCK_COMBOBOX;

BEGIN_MESSAGE_MAP(CIORemappingDlg, CDialog)
 ON_UPDATE_COMMAND_UI(IDC_IO_REMAPPING_ECF_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOCaptionStart,  IOCaptionEnd, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IOComboboxStart,  IOComboboxEnd, OnUpdateControls)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIORemappingDlg dialog

CIORemappingDlg::CIORemappingDlg(CWnd* pParent /*=NULL*/)
: Super(CIORemappingDlg::IDD, pParent)
, m_enabled(false)
{
 //empty
}

void CIORemappingDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

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
 pCmdUI->Enable(m_enabled);
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
