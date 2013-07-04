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
#include "SecurPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CSecurPageDlg::IDD = IDD_PD_SECUR_PAGE;

BEGIN_MESSAGE_MAP(CSecurPageDlg, Super)
 ON_BN_CLICKED(IDC_PD_SECUR_BT_APPLY_BUTTON, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_NAME_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_PASS_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_NAME_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_PASS_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_APPLY_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_GROUP, OnUpdateControls)
END_MESSAGE_MAP()

CSecurPageDlg::CSecurPageDlg(CWnd* pParent /*=NULL*/)
: Super(CSecurPageDlg::IDD, pParent)
, m_enabled(false)
, m_bt_name_edit(CEditEx::MODE_STRING)
, m_bt_pass_edit(CEditEx::MODE_STRING)
{
 _tcscpy(m_params.bt_name, _T(""));
 _tcscpy(m_params.bt_pass, _T(""));
}

LPCTSTR CSecurPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CSecurPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_PD_SECUR_BT_NAME_EDIT, m_bt_name_edit);
 DDX_Control(pDX, IDC_PD_SECUR_BT_NAME_CAPTION, m_bt_name_caption);
 DDX_Control(pDX, IDC_PD_SECUR_BT_PASS_EDIT, m_bt_pass_edit);
 DDX_Control(pDX, IDC_PD_SECUR_BT_PASS_CAPTION, m_bt_pass_caption);
 DDX_Control(pDX, IDC_PD_SECUR_BT_APPLY_BUTTON, m_bt_apply_button);

 CString name = m_params.bt_name;
 DDX_Text(pDX, IDC_PD_SECUR_BT_NAME_EDIT, name);
 _tcscpy(m_params.bt_name, name.GetBuffer(0));
 CString pass = m_params.bt_pass;
 DDX_Text(pDX, IDC_PD_SECUR_BT_PASS_EDIT, pass);
 _tcscpy(m_params.bt_pass, pass.GetBuffer(0));
}

void CSecurPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CSecurPageDlg message handlers

BOOL CSecurPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_bt_name_edit.SetLimitText(6);
 m_bt_pass_edit.SetLimitText(4);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_bt_apply_button, MLL::GetString(IDS_PD_SECUR_BT_APPLY_BUTTON_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CSecurPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

//разрешение/запрещение контроллов (всех поголовно)
void CSecurPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CSecurPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CSecurPageDlg::GetValues(SECU3IO::SecurPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::SecurPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CSecurPageDlg::SetValues(const SECU3IO::SecurPar* i_values)
{
 ASSERT(i_values);
 _TSTRING name = m_params.bt_name, pass = m_params.bt_pass;
 memcpy(&m_params, i_values, sizeof(SECU3IO::SecurPar));
 _tcscpy(m_params.bt_name, name.c_str());
 _tcscpy(m_params.bt_pass, pass.c_str());
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
