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

/** \file RPMGridEditDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <afxpriv.h>
#include <float.h>
#include <math.h>
#include "resource.h"
#include "RPMGridEditDlg.h"
#include "ui-core/EditEx.h"

const UINT CRPMGridEditDlg::IDD = IDD_RPM_GRID_EDITOR;

static const int editStart = IDC_RGE_EDIT_0;
static const int editEnd = IDC_RGE_EDIT_15;
static const COLORREF itemErrColor = RGB(255,120,120);
static const COLORREF errorMsgColor = RGB(255, 0, 0);

/////////////////////////////////////////////////////////////////////////////
// CRPMGridEditDlg dialog

BEGIN_MESSAGE_MAP(CRPMGridEditDlg, Super)
 ON_CONTROL_RANGE(EN_CHANGE, editStart, editEnd, OnChangeEdit)
 ON_BN_CLICKED(IDC_RGE_LOAD_DEF_VAL, OnLoadDefValBtn)
 ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOkButton)
 ON_WM_CTLCOLOR()
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()

CRPMGridEditDlg::CRPMGridEditDlg(CWnd* pParent /*=NULL*/)
: Super(CRPMGridEditDlg::IDD, pParent)
, m_redBrush(itemErrColor)
{
 for(size_t i = 0; i < 16; ++i)
 {
  m_edits.push_back(new CEditEx(CEditEx::MODE_INT));
  m_values.push_back(0);
  m_errflags.push_back(false);
 }
}

CRPMGridEditDlg::~CRPMGridEditDlg()
{
 for(size_t i = 0; i < m_edits.size(); ++i)
  delete m_edits[i];
}

void CRPMGridEditDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_RGE_ERROR_MSG_TEXT, m_errMsg);
 for(size_t i = 0; i < m_edits.size(); ++i)
 {
  DDX_Control(pDX, editStart + i, *(m_edits[i]));
  m_edits[i]->DDX_Value(pDX, editStart + i, m_values[i]);
 }
}

/////////////////////////////////////////////////////////////////////////////
// CRPMGridEditDlg message handlers

void CRPMGridEditDlg::OnOK()
{
 Super::OnOK();
}

void CRPMGridEditDlg::OnCancel()
{
 Super::OnCancel();
}

void CRPMGridEditDlg::OnUpdateOkButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_isOkEnabled ? m_isOkEnabled() : false);
}

BOOL CRPMGridEditDlg::OnInitDialog()
{
 Super::OnInitDialog();

 for(size_t i = 0; i < m_edits.size(); ++i)
  m_edits[i]->SetLimitText(5);

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT CRPMGridEditDlg::OnKickIdle(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
 UpdateDialogControls(this, true);
 return FALSE;
}

HBRUSH CRPMGridEditDlg::OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor)
{
 HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
 if (nCtlColor == CTLCOLOR_STATIC && pWnd->m_hWnd == m_errMsg.m_hWnd)
 {
  pDC->SetTextColor(errorMsgColor);
  pDC->SetBkMode(TRANSPARENT);
  hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
 }
 else if (nCtlColor == CTLCOLOR_EDIT)
 {
  for(size_t i = 0; i < m_edits.size(); ++i)
   if (pWnd->m_hWnd == m_edits[i]->m_hWnd && true==m_errflags[i])
   { 
    pDC->SetBkColor(itemErrColor);
    hbr = m_redBrush;
   }
 }
 return hbr;
}

void CRPMGridEditDlg::OnChangeEdit(UINT nID)
{
 UpdateData();
 //notify event receiver about change of view content
 size_t index = (nID - editStart);
 if (m_onChange && index < m_edits.size())
  m_onChange(index, m_values[index]);
}

void CRPMGridEditDlg::OnLoadDefValBtn()
{
 if (m_onLoadDefVal)
  m_onLoadDefVal();
}

void CRPMGridEditDlg::SetValues(const float* ip_values)
{
 if (!ip_values)
  return;
 m_values = std::vector<float>(ip_values, ip_values + m_values.size());
 if (::IsWindow(m_hWnd))
  UpdateData(FALSE);
}

void CRPMGridEditDlg::GetValues(float* op_values)
{
 if (!op_values)
  return;
 for(size_t i = 0; i < m_values.size(); ++i)
  op_values[i] = m_values[i];
}

float CRPMGridEditDlg::GetValue(size_t index)
{
 if (index >= m_edits.size())
  return 0;
 UpdateData();
 return m_values[index];
}

void CRPMGridEditDlg::setOnChange(EventOnChange onFunction)
{
 m_onChange = onFunction;
}

void CRPMGridEditDlg::setOnLoadDefVal(EventHandler OnFunction)
{
 m_onLoadDefVal = OnFunction;
}

void CRPMGridEditDlg::setIsOkEnabled(EventResult OnFunction)
{
 m_isOkEnabled = OnFunction;
}

void CRPMGridEditDlg::SetErrMessage(const _TSTRING& str)
{
 m_errMsg.SetWindowText(str.c_str());
 m_errMsg.ShowWindow(str.size() ? SW_SHOW : SW_HIDE);
}

void CRPMGridEditDlg::SetItemError(size_t index, bool i_error)
{
 if (index >= m_errflags.size())
  return;
 m_errflags[index] = i_error;
 //Update cprresponding control
 m_edits[index]->Invalidate();
}
