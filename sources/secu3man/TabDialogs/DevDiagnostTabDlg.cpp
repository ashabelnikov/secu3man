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
#include "DevDiagnostTabDlg.h"

#include "common/FastDelegate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_ID 0

using namespace fastdelegate;

const UINT CDevDiagnostTabDlg::IDD = IDD_DEV_DIAGNOSTICS;

const UINT OutputsCheckStart = IDC_DEV_DIAG_IGN_OUT1_CHECK;
const UINT OutputsCheckEnd = IDC_DEV_DIAG_ADD_O2_CHECK;
const UINT InputsTextStart = IDC_DEV_DIAG_VOLTAGE;
const UINT InputsTextEnd = IDC_DEV_DIAG_PS;
const UINT InputsCaptionStart = IDC_DEV_DIAG_VOLTAGE_CAPTION;
const UINT InputsCaptionEnd = IDC_DEV_DIAG_KS_2_CAPTION;

BEGIN_MESSAGE_MAP(CDevDiagnostTabDlg, Super)
 ON_WM_DESTROY()
 ON_WM_TIMER()
 ON_COMMAND_RANGE(OutputsCheckStart, OutputsCheckEnd, OnOutputCheckToggle)
 ON_BN_CLICKED(IDC_DEV_DIAG_ENTER_CHECK, OnEnterButton)
 ON_UPDATE_COMMAND_UI_RANGE(OutputsCheckStart, OutputsCheckEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI_RANGE(InputsTextStart, InputsTextEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI_RANGE(InputsCaptionStart, InputsCaptionEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_OUTPUTS_GROUP, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_INPUTS_GROUP, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_ENTER_CHECK, OnUpdateEnterButton)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_WARNING_TEXT, OnUpdateEnterButton)
END_MESSAGE_MAP()

CDevDiagnostTabDlg::CDevDiagnostTabDlg(CWnd* pParent /*=NULL*/)
: Super(CDevDiagnostTabDlg::IDD, pParent)
, m_enable_diag_controls(false)
, m_enable_enter_button(false)
{
 //todo
}

void CDevDiagnostTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_DEV_DIAG_ENTER_CHECK, m_enter_button);
}

LPCTSTR CDevDiagnostTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CDevDiagnostTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 SetTimer(TIMER_ID, 250, NULL);

 UpdateDialogControls(this,TRUE);
 return TRUE;
}

void CDevDiagnostTabDlg::OnUpdateDiagControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable_diag_controls);
}

void CDevDiagnostTabDlg::OnUpdateEnterButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable_enter_button);
}

void CDevDiagnostTabDlg::OnTimer(UINT nIDEvent)
{
 //dirty hack
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);
}

void CDevDiagnostTabDlg::OnDestroy()
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);
}

void CDevDiagnostTabDlg::OnOutputCheckToggle(UINT nID)
{
 CButton* p_check = static_cast<CButton*>(GetDlgItem(nID));
 if (!p_check)
  return;

 bool state = p_check->GetCheck() == BST_CHECKED;
 if (m_on_output_check)
  m_on_output_check(nID-OutputsCheckStart, state);
}

void CDevDiagnostTabDlg::OnEnterButton()
{
 if (m_on_enter_button)
 {
  m_on_enter_button(BST_CHECKED == m_enter_button.GetCheck());
 }
}

void CDevDiagnostTabDlg::EnableDiagControls(bool i_enable)
{
 m_enable_diag_controls = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CDevDiagnostTabDlg::EnableEnterButton(bool i_enable)
{
 m_enable_enter_button = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CDevDiagnostTabDlg::setOnOutputToggle(EventOutputToggle OnFunction)
{
 m_on_output_check = OnFunction;
}

void CDevDiagnostTabDlg::setOnEnterButton(EventToggle OnFunction)
{
 m_on_enter_button = OnFunction;
}
