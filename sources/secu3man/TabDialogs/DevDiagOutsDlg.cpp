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

/** \file DevDiagOutsDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "DevDiagOutsDlg.h"

#include "common/DPIAware.h"
#include "common/FastDelegate.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ToolTipCtrlEx.h"

#define TIMER_ID 0

using namespace fastdelegate;

const UINT CDevDiagOutsDlg::IDD = IDD_DEVDIAG_OUTPUTS;

const UINT OutputsCheckStart = IDC_DEV_DIAG_IGN_OUT1_CHECK;
const UINT OutputsCheckEnd = IDC_DEV_DIAG_DE_CHECK;
const UINT OutputsCheckStart1 = IDC_DEV_DIAG_IGN_O1_CHECK;     //secu-3i
const UINT OutputsCheckEnd1 = IDC_DEV_DIAG_TACH_O_CHECK;      //secu-3i

BEGIN_MESSAGE_MAP(CDevDiagOutsDlg, Super)
 ON_WM_DESTROY()
 ON_WM_TIMER()
 ON_WM_SIZE()
 ON_COMMAND_RANGE(OutputsCheckStart, OutputsCheckEnd, OnOutputCheckToggle)
 ON_COMMAND_RANGE(OutputsCheckStart1, OutputsCheckEnd1, OnOutputCheckToggle) //secu-3i
 ON_UPDATE_COMMAND_UI_RANGE(OutputsCheckStart, OutputsCheckEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI_RANGE(OutputsCheckStart1, OutputsCheckEnd1, OnUpdateDiagControls) //secu-3i
END_MESSAGE_MAP()

CDevDiagOutsDlg::CDevDiagOutsDlg(CWnd* pParent /*=NULL*/)
: Super(CDevDiagOutsDlg::IDD, pParent)
, m_enable_diag_controls(false)
, m_enable_secu3t_features(false)
, m_enable_blde_testing(false)
, m_enable_tacho_testing(false)
, mp_scr(new CWndScroller)
{
 //empty
}

void CDevDiagOutsDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
}

LPCTSTR CDevDiagOutsDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CDevDiagOutsDlg::OnInitDialog()
{
 Super::OnInitDialog();
  
 SetTimer(TIMER_ID, 250, NULL);

 UpdateDialogControls(this,TRUE);

 //initialize window scroller
 mp_scr->Init(this);
 _UpdateScrlViewSize();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_IGN_OUT1_CHECK), MLL::GetString(IDS_DEV_DIAG_IGN_OUT1_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_IGN_O1_CHECK), MLL::GetString(IDS_DEV_DIAG_IGN_OUT1_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_IGN_OUT2_CHECK), MLL::GetString(IDS_DEV_DIAG_IGN_OUT2_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_IGN_O2_CHECK), MLL::GetString(IDS_DEV_DIAG_IGN_OUT2_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_IE_CHECK), MLL::GetString(IDS_DEV_DIAG_IE_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_FE_CHECK), MLL::GetString(IDS_DEV_DIAG_FE_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ST_BLOCK_CHECK), MLL::GetString(IDS_DEV_DIAG_ST_BLOCK_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_STBL_O_CHECK), MLL::GetString(IDS_DEV_DIAG_ST_BLOCK_CHECK_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 return TRUE;
}

//secu-3t version
void CDevDiagOutsDlg::OnUpdateDiagControls(CCmdUI* pCmdUI)
{
 switch(pCmdUI->m_nID)
 {
  case IDC_DEV_DIAG_BLO_CHECK: //secu-3i
  case IDC_DEV_DIAG_DEO_CHECK: //secu-3i
  case IDC_DEV_DIAG_BL_CHECK:
  case IDC_DEV_DIAG_DE_CHECK:
   pCmdUI->Enable(m_enable_diag_controls && m_enable_blde_testing);  
   break;

  case IDC_DEV_DIAG_TACH_O_CHECK: //secu-3i
   pCmdUI->Enable(m_enable_diag_controls && m_enable_tacho_testing);  
   break;

  default:
   pCmdUI->Enable(m_enable_diag_controls);
 };
}

void CDevDiagOutsDlg::OnTimer(UINT nIDEvent)
{
 //dirty hack
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);
}

void CDevDiagOutsDlg::OnDestroy()
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);
 mp_scr->Close();
}

void CDevDiagOutsDlg::OnOutputCheckToggle(UINT nID)
{
 CButton* p_check = static_cast<CButton*>(GetDlgItem(nID));
 if (!p_check)
  return;

 bool state = p_check->GetCheck() == BST_CHECKED;
 if (m_on_output_check)
  m_on_output_check(nID-(m_enable_secu3t_features ? OutputsCheckStart : OutputsCheckStart1), state);
}

void CDevDiagOutsDlg::EnableDiagControls(bool i_enable)
{
 m_enable_diag_controls = i_enable;
 UpdateDialogControls(this, TRUE);
}

void CDevDiagOutsDlg::EnableSECU3TFeatures(bool i_enable)
{
 m_enable_secu3t_features = i_enable;

 _UpdateScrlViewSize();

 for(int i = OutputsCheckStart; i <= OutputsCheckEnd; ++i)
  GetDlgItem(i)->ShowWindow(i_enable ? SW_SHOW : SW_HIDE);
 for(int i = OutputsCheckStart1; i <= OutputsCheckEnd1; ++i)
  GetDlgItem(i)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);

 UpdateDialogControls(this,TRUE);
}

void CDevDiagOutsDlg::EnableBLDETesting(bool i_enable)
{
 m_enable_blde_testing = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CDevDiagOutsDlg::EnableTACHOTesting(bool i_enable)
{
 m_enable_tacho_testing = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CDevDiagOutsDlg::setOnOutputToggle(EventOutputToggle OnFunction)
{
 m_on_output_check = OnFunction;
}

void CDevDiagOutsDlg::SetOutputValue(int id, bool state)
{
 CButton *pButton = (CButton*)GetDlgItem((m_enable_secu3t_features ? OutputsCheckStart : OutputsCheckStart1) + id);
 pButton->SetCheck(state ? BST_CHECKED : BST_UNCHECKED);
}

void CDevDiagOutsDlg::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after, x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

void CDevDiagOutsDlg::_UpdateScrlViewSize(void)
{
 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(0, m_enable_secu3t_features ? da.ScaleY(300) : da.ScaleY(580));
}

void CDevDiagOutsDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 _UpdateScrlViewSize();
}
