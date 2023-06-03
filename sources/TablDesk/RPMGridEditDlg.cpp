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
#include "ui-core/ToolTipCtrlEx.h"

const UINT CRPMGridEditDlg::IDD = IDD_RPM_GRID_EDITOR;

static const int editStart[3] = {IDC_RGE_EDIT_0,  IDC_CGE_EDIT_0,  IDC_LGE_EDIT_0};
static const int editEnd[3] =   {IDC_RGE_EDIT_15, IDC_CGE_EDIT_15, IDC_LGE_EDIT_15};

static const COLORREF itemErrColor = RGB(255,120,120);
static const COLORREF errorMsgColor = RGB(255, 0, 0);

static const int numGrids = 3;

/////////////////////////////////////////////////////////////////////////////
// CRPMGridEditDlg dialog

BEGIN_MESSAGE_MAP(CRPMGridEditDlg, Super)
 ON_CONTROL_RANGE(EN_CHANGE, editStart[0], editEnd[0], OnChangeEdit)
 ON_CONTROL_RANGE(EN_CHANGE, editStart[1], editEnd[1], OnChangeEdit1)
 ON_CONTROL_RANGE(EN_CHANGE, editStart[2], editEnd[2], OnChangeEdit2)
 ON_BN_CLICKED(IDC_RGE_LOAD_DEF_VAL, OnLoadDefValBtn)
 ON_BN_CLICKED(IDC_CGE_LOAD_DEF_VAL, OnLoadDefValBtn1)
 ON_BN_CLICKED(IDC_LGE_LOAD_DEF_VAL, OnLoadDefValBtn2)
 ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOkButton)
 ON_WM_CTLCOLOR()
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()

CRPMGridEditDlg::CRPMGridEditDlg(CWnd* pParent /*=NULL*/)
: Super(CRPMGridEditDlg::IDD, pParent)
, m_redBrush(itemErrColor)
{//RPM:
 m_values[0].reserve(32);
 m_edits[0].reserve(32);
 m_errflags[0].reserve(32);
 for(size_t i = 0; i < 16; ++i)
 {
  m_edits[0].push_back(new CEditEx(CEditEx::MODE_INT));
  m_values[0].push_back(0);
  m_errflags[0].push_back(false);
 }
 //CLT:
 m_values[1].reserve(32);
 m_edits[1].reserve(32);
 m_errflags[1].reserve(32);
 for(size_t i = 0; i < 16; ++i)
 {
  m_edits[1].push_back(new CEditEx(CEditEx::MODE_INT | CEditEx::MODE_SIGNED));
  m_edits[1].back()->SetDecimalPlaces(3);
  m_values[1].push_back(0);
  m_errflags[1].push_back(false);
 }
 //load:
 m_values[2].reserve(32);
 m_edits[2].reserve(32);
 m_errflags[2].reserve(32);
 for(size_t i = 0; i < 16; ++i)
 {
  m_edits[2].push_back(new CEditEx(CEditEx::MODE_FLOAT));
  m_edits[2].back()->SetDecimalPlaces(2);
  m_values[2].push_back(0);
  m_errflags[2].push_back(false);
 }
}

CRPMGridEditDlg::~CRPMGridEditDlg()
{
 for(size_t j = 0; j < numGrids; ++j)
  for(size_t i = 0; i < m_edits[j].size(); ++i)
   delete m_edits[j][i];
}

void CRPMGridEditDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_RGE_ERROR_MSG_TEXT, m_errMsg[0]);
 DDX_Control(pDX, IDC_CGE_ERROR_MSG_TEXT, m_errMsg[1]);
 DDX_Control(pDX, IDC_LGE_ERROR_MSG_TEXT, m_errMsg[2]);

 for (size_t j = 0; j < numGrids; ++j)
 {
  for(size_t i = 0; i < m_edits[j].size(); ++i)
  {
   DDX_Control(pDX, editStart[j] + i, *(m_edits[j][i]));
   m_edits[j][i]->DDX_Value(pDX, editStart[j] + i, m_values[j][i]);
  }
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

 for(size_t i = 0; i < m_edits[0].size(); ++i)
  m_edits[0][i]->SetLimitText(5);

 for(size_t i = 0; i < m_edits[1].size(); ++i)
  m_edits[1][i]->SetLimitText(5);

 for(size_t i = 0; i < m_edits[2].size(); ++i)
  m_edits[2][i]->SetLimitText(6);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_RGE_LOAD_DEF_VAL), MLL::GetString(IDS_RGE_LOAD_DEF_VAL_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_CGE_LOAD_DEF_VAL), MLL::GetString(IDS_CGE_LOAD_DEF_VAL_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_LGE_LOAD_DEF_VAL), MLL::GetString(IDS_LGE_LOAD_DEF_VAL_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

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
 HBRUSH hbr = Super::OnCtlColor(pDC, pWnd, nCtlColor);
 if ((nCtlColor == CTLCOLOR_STATIC && pWnd->m_hWnd == m_errMsg[0].m_hWnd) || (nCtlColor == CTLCOLOR_STATIC && pWnd->m_hWnd == m_errMsg[1].m_hWnd) || (nCtlColor == CTLCOLOR_STATIC && pWnd->m_hWnd == m_errMsg[2].m_hWnd))
 {
  pDC->SetTextColor(errorMsgColor);
  pDC->SetBkMode(TRANSPARENT);
  hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
 }
 else if (nCtlColor == CTLCOLOR_EDIT)
 {
  for(size_t j = 0; j < numGrids; ++j)
  {
   for(size_t i = 0; i < m_edits[j].size(); ++i)
   {
    if (pWnd->m_hWnd == m_edits[j][i]->m_hWnd && true==m_errflags[j][i])
    { 
     pDC->SetBkColor(itemErrColor);
     hbr = m_redBrush;
    }
   }
  }
 }
 return hbr;
}

void CRPMGridEditDlg::OnChangeEdit(UINT nID)
{
 UpdateData();
 //notify event receiver about change of view content
 size_t index = (nID - editStart[0]);
 if (m_onChange && index < m_edits[0].size())
  m_onChange(0, index, m_values[0][index]); //RPM
}

void CRPMGridEditDlg::OnChangeEdit1(UINT nID)
{
 UpdateData();
 //notify event receiver about change of view content
 size_t index = (nID - editStart[1]);
 if (m_onChange && index < m_edits[1].size())
  m_onChange(1, index, m_values[1][index]); //CLT
}

void CRPMGridEditDlg::OnChangeEdit2(UINT nID)
{
 UpdateData();
 //notify event receiver about change of view content
 size_t index = (nID - editStart[2]);
 if (m_onChange && index < m_edits[2].size())
  m_onChange(2, index, m_values[2][index]); //Load
}

void CRPMGridEditDlg::OnLoadDefValBtn()
{
 if (m_onLoadDefVal)
  m_onLoadDefVal(0);
}

void CRPMGridEditDlg::OnLoadDefValBtn1()
{
 if (m_onLoadDefVal)
  m_onLoadDefVal(1);
}

void CRPMGridEditDlg::OnLoadDefValBtn2()
{
 if (m_onLoadDefVal)
  m_onLoadDefVal(2);
}

void CRPMGridEditDlg::SetValues(int mode, const float* ip_values, bool rev /*=false*/)
{
 ASSERT(mode >= 0 && mode < numGrids);
 if (!ip_values)
  return;
 
 if (rev)
 { //reverse
  int num = m_values[mode].size();
  for(int i = 0; i < num; ++i)
   m_values[mode][i] = ip_values[(num-1)-i];
 }
 else
 {
  m_values[mode] = std::vector<float>(ip_values, ip_values + m_values[mode].size());
 }
 if (::IsWindow(m_hWnd))
  UpdateData(FALSE);
}

void CRPMGridEditDlg::GetValues(int mode, float* op_values, bool rev /*=false*/)
{
 ASSERT(mode >= 0 && mode < numGrids);
 if (!op_values)
  return;
 if (rev)
 { //reverse
  size_t num = m_values[mode].size();
  for(size_t i = 0; i < num; ++i)
   op_values[i] = m_values[mode][(num-1)-i];
 }
 else
 {
  for(size_t i = 0; i < m_values[mode].size(); ++i)
   op_values[i] = m_values[mode][i];
 }
}

float CRPMGridEditDlg::GetValue(int mode, size_t index)
{
 ASSERT(mode >= 0 && mode < numGrids);
 if (index >= m_edits[mode].size())
  return 0;
 UpdateData();
 return m_values[mode][index];
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

void CRPMGridEditDlg::SetErrMessage(int mode, const _TSTRING& str)
{
 ASSERT(mode >= 0 && mode < numGrids);
 m_errMsg[mode].SetWindowText(str.c_str());
 m_errMsg[mode].ShowWindow(str.size() ? SW_SHOW : SW_HIDE);
}

void CRPMGridEditDlg::SetItemError(int mode, size_t index, bool i_error)
{
 ASSERT(mode >= 0 && mode < numGrids);
 if (index >= m_errflags[mode].size())
  return;
 m_errflags[mode][index] = i_error;
 //Update cprresponding control
 m_edits[mode][index]->Invalidate();
}

INT_PTR CRPMGridEditDlg::DoModal()
{
 INT_PTR result = Super::DoModal();
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}
