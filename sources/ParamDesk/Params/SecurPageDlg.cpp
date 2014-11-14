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

#include "stdafx.h"
#include "Resources/resource.h"
#include "SecurPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CSecurPageDlg::IDD = IDD_PD_SECUR_PAGE;

BEGIN_MESSAGE_MAP(CSecurPageDlg, Super)
 ON_BN_CLICKED(IDC_PD_SECUR_BT_APPLY_BUTTON, OnChangeDataApply)
 ON_BN_CLICKED(IDC_PD_SECUR_BT_USE_CHECK, OnChangeDataUseBtCheck)
 ON_BN_CLICKED(IDC_PD_SECUR_IMM_USE_CHECK, OnChangeData)
 ON_EN_CHANGE(IDC_PD_SECUR_BT_NAME_EDIT, OnChangeDataNamePass)
 ON_EN_CHANGE(IDC_PD_SECUR_BT_PASS_EDIT, OnChangeDataNamePass)
 ON_EN_CHANGE(IDC_PD_SECUR_IBTN_KEY1_EDIT, OnChangeDataIbtnKeys)
 ON_EN_CHANGE(IDC_PD_SECUR_IBTN_KEY2_EDIT, OnChangeDataIbtnKeys)

 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_NAME_EDIT, OnUpdateNameAndPass)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_PASS_EDIT, OnUpdateNameAndPass)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_NAME_CAPTION, OnUpdateNameAndPass)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_PASS_CAPTION, OnUpdateNameAndPass)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_APPLY_BUTTON, OnUpdateApplyButton)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_GROUP, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_BT_USE_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_IMM_USE_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_IMM_GROUP, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_IBTN_KEY1_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_IBTN_KEY2_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SECUR_IBTN_KEYS_CAPTION, OnUpdateControls)
END_MESSAGE_MAP()

CSecurPageDlg::CSecurPageDlg(CWnd* pParent /*=NULL*/)
: Super(CSecurPageDlg::IDD, pParent)
, m_enabled(false)
, m_namepass_enabled(false)
, m_bt_name_edit(CEditEx::MODE_STRING)
, m_bt_pass_edit(CEditEx::MODE_INT)
, m_ibtn_key1_edit(CEditEx::MODE_HEXSTR)
, m_ibtn_key2_edit(CEditEx::MODE_HEXSTR)
{
 _tcscpy(m_params.bt_name, _T(""));
 _tcscpy(m_params.bt_pass, _T(""));
 m_params.use_bt = 0;
 m_params.use_imm = 0;
 for(int j = 0; j < SECU3IO::IBTN_KEYS_NUM; ++j)
  memset(m_params.ibtn_keys[j], 0, SECU3IO::IBTN_KEY_SIZE);
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
 DDX_Control(pDX, IDC_PD_SECUR_BT_USE_CHECK, m_bt_use_check);
 DDX_Control(pDX, IDC_PD_SECUR_IMM_USE_CHECK, m_imm_use_check);
 DDX_Control(pDX, IDC_PD_SECUR_IBTN_KEY1_EDIT, m_ibtn_key1_edit);
 DDX_Control(pDX, IDC_PD_SECUR_IBTN_KEY2_EDIT, m_ibtn_key2_edit);

 CString name = m_bt_name.c_str();
 DDX_Text(pDX, IDC_PD_SECUR_BT_NAME_EDIT, name);
 m_bt_name = name.GetBuffer(0);
 CString pass = m_bt_pass.c_str();
 DDX_Text(pDX, IDC_PD_SECUR_BT_PASS_EDIT, pass);
 m_bt_pass = pass.GetBuffer(0);

 DDX_Check_UCHAR(pDX, IDC_PD_SECUR_BT_USE_CHECK, m_params.use_bt);
 DDX_Check_UCHAR(pDX, IDC_PD_SECUR_IMM_USE_CHECK, m_params.use_imm);
}

void CSecurPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CSecurPageDlg::OnUpdateNameAndPass(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_namepass_enabled && m_params.use_bt);
}

void CSecurPageDlg::OnUpdateApplyButton(CCmdUI* pCmdUI)
{
 bool namepass_not_empty = (m_bt_name_edit.LineLength() > 0) && (m_bt_pass_edit.LineLength() == 4);
 pCmdUI->Enable(m_enabled && m_namepass_enabled && m_params.use_bt && namepass_not_empty);
}

/////////////////////////////////////////////////////////////////////////////
// CSecurPageDlg message handlers

BOOL CSecurPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_bt_name_edit.SetLimitText(8);
 m_bt_pass_edit.SetLimitText(4);

 m_ibtn_key1_edit.SetLimitText(12);
 m_ibtn_key2_edit.SetLimitText(12);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_bt_apply_button, MLL::GetString(IDS_PD_SECUR_BT_APPLY_BUTTON_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CSecurPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CSecurPageDlg::OnChangeDataApply()
{
 UpdateData();
 _tcscpy(m_params.bt_name, m_bt_name.c_str());
 _tcscpy(m_params.bt_pass, m_bt_pass.c_str());
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CSecurPageDlg::OnChangeDataIbtnKeys()
{
 CString str;
 int result1, result2;
 unsigned long long value1, value2;
 m_ibtn_key1_edit.GetWindowText(str);
 result1 = _stscanf(str.GetBuffer(0), _T("%I64X"), &value1);
 m_ibtn_key2_edit.GetWindowText(str);
 result2 = _stscanf(str.GetBuffer(0), _T("%I64X"), &value2);

 if (1==result1 && 1==result2)
 {
  int i;
  for(i = 0; i < SECU3IO::IBTN_KEY_SIZE; ++i)
   m_params.ibtn_keys[0][i] = (BYTE)((value1 >> i*8) & 0xFF);
  for(i = 0; i < SECU3IO::IBTN_KEY_SIZE; ++i)
   m_params.ibtn_keys[1][i] = (BYTE)((value2 >> i*8) & 0xFF);
  OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
 }
}

void CSecurPageDlg::OnChangeDataUseBtCheck()
{
 UpdateData();
 UpdateDialogControls(this, TRUE);
 m_params.set_btbr = 1;
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CSecurPageDlg::OnChangeDataNamePass()
{
 UpdateDialogControls(this, TRUE);
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

void CSecurPageDlg::EnableBTNameAndPass(bool enable)
{
 if (m_namepass_enabled == enable)
  return; //already has needed state
 m_namepass_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
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
 bool use_bt_prev = m_params.use_bt;
 memcpy(&m_params, i_values, sizeof(SECU3IO::SecurPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
 if (use_bt_prev != (m_params.use_bt > 0))
  UpdateDialogControls(this, TRUE); //to apply state of "Use bluetooth" checkbox

 CString str, value; int i;
 for(i = 0; i < SECU3IO::IBTN_KEY_SIZE; ++i)
 {
  value.Format(_T("%02X"), (int)m_params.ibtn_keys[0][SECU3IO::IBTN_KEY_SIZE - i - 1]);
  str.Append(value);
 }
 m_ibtn_key1_edit.SetWindowText(str);

 str = _T("");
 for(i = 0; i < SECU3IO::IBTN_KEY_SIZE; ++i)
 {
  value.Format(_T("%02X"), (int)m_params.ibtn_keys[1][SECU3IO::IBTN_KEY_SIZE - i - 1]);
  str.Append(value);
 }
 m_ibtn_key2_edit.SetWindowText(str);
}
