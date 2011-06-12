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
#include "TablesPageDlg.h"

#include "TablDesk/ButtonsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT CTablesPageDlg::IDD = IDD_TD_TABLES_PAGE;

BEGIN_MESSAGE_MAP(CTablesPageDlg, Super)
END_MESSAGE_MAP()

CTablesPageDlg::CTablesPageDlg(CWnd* pParent /*=NULL*/)
: Super(CTablesPageDlg::IDD, pParent)
, m_enabled(FALSE)
, mp_ButtonsPanel(new CButtonsPanel(0, NULL))
{
  //empty
}

LPCTSTR CTablesPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CTablesPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CTablesPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CTablesPageDlg message handlers

BOOL CTablesPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CRect rect;
 GetDlgItem(IDC_TD_BUTTONS_PANEL_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 mp_ButtonsPanel->Create(mp_ButtonsPanel->IDD, this);
 mp_ButtonsPanel->SetPosition(rect.left, rect.top, NULL);
 mp_ButtonsPanel->ShowWindow(SW_SHOW);

 UpdateData(FALSE);
 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
	           // EXCEPTION: OCX Property Pages should return FALSE
}

//разрешение/запрещение контроллов (всех поголовно)
void CTablesPageDlg::Enable(bool enable)
{
 m_enabled = (enable) ? TRUE : FALSE;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CTablesPageDlg::IsEnabled(void)
{
 return (m_enabled) ? true : false;
}

/*
//эту функцию необходимо использовать когда надо получить данные из диалога
void CTablesPageDlg::GetValues(SECU3IO::StartrPar* o_values)
{
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CTablesPageDlg::SetValues(const SECU3IO::StartrPar* i_values)
{
}
*/