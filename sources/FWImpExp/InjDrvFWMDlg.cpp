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

/** \file InjDrvFWMDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "InjDrvFWMDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/Label.h"

const UINT CInjDrvFWMDlg::IDD = IDD_INJDRV_FWM;

#define WM_KICKIDLE 0x036A //hack

/////////////////////////////////////////////////////////////////////////////
// CFirmwareMasterDlg dialog

BEGIN_MESSAGE_MAP(CInjDrvFWMDlg, Super)
 ON_WM_DESTROY()
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
 ON_COMMAND_RANGE(IDC_FWM_INPINV_CHECK, IDC_FWM_PWMINV_CHECK, OnFwmCheck)
 ON_BN_CLICKED(IDC_FWM_SAVE_BTN, OnSave)
END_MESSAGE_MAP()

CInjDrvFWMDlg::CInjDrvFWMDlg(CWnd* pParent /*=NULL*/)
: Super(CInjDrvFWMDlg::IDD, pParent)
, m_proc_idx(0)
, mp_FuseLink(new CLabel)
, mp_ChecksLink(new CLabel)
{
 for (int i = 0; i < FWM_NR_OF_FLAGS; ++i)
  m_fwm_flags[i] = false;
}

void CInjDrvFWMDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_FWM_PROCESSOR_COMBO, m_proc_combo);

 DDX_CBIndex_int(pDX, IDC_FWM_PROCESSOR_COMBO, m_proc_idx);

 DDX_Control(pDX, IDC_FWM_INPINV_CHECK, m_fwm_checks[FWM_INPINV]);
 DDX_Control(pDX, IDC_FWM_FLBINV_CHECK, m_fwm_checks[FWM_FLBINV]);
 DDX_Control(pDX, IDC_FWM_PWMINV_CHECK, m_fwm_checks[FWM_PWMINV]);
 DDX_Control(pDX, IDC_FWM_PWCORR_CHECK, m_fwm_checks[FWM_PWCORR]);

 DDX_Check_bool(pDX, IDC_FWM_INPINV_CHECK, m_fwm_flags[FWM_INPINV]);
 DDX_Check_bool(pDX, IDC_FWM_FLBINV_CHECK, m_fwm_flags[FWM_FLBINV]);
 DDX_Check_bool(pDX, IDC_FWM_PWMINV_CHECK, m_fwm_flags[FWM_PWMINV]);
 DDX_Check_bool(pDX, IDC_FWM_PWCORR_CHECK, m_fwm_flags[FWM_PWCORR]);

 DDX_Control(pDX, IDC_FWM_FUSE_LINK, *mp_FuseLink);
 DDX_Control(pDX, IDC_FWM_CHECKS_LINK, *mp_ChecksLink);
}

/////////////////////////////////////////////////////////////////////////////
// CInjDrvFWMDlg message handlers

LRESULT CInjDrvFWMDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
 UpdateDialogControls(this, FALSE);
 return 0L;
}

void CInjDrvFWMDlg::OnOK()
{
 //Before closing dialog, notify controller
 if (m_OnOkButton)
  m_OnOkButton();

 Super::OnOK();
}

void CInjDrvFWMDlg::OnCancel()
{
 //Before closing dialog, notify controller
 if (m_OnCancelButton)
  m_OnCancelButton();

 Super::OnCancel();
}

BOOL CInjDrvFWMDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (m_OnActivate)
  m_OnActivate();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 int idc = IDC_FWM_INPINV_CHECK;
 for (UINT ids = IDS_FWM_INPINV_CHECK_TT; ids <= IDS_FWM_PWCORR_CHECK_TT; ids++, idc++)
  VERIFY(mp_ttc->AddWindow(GetDlgItem(idc), MLL::GetString(ids)));

 VERIFY(mp_ttc->AddWindow(&m_proc_combo, MLL::GetString(IDS_FWM_PROCESSOR_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_FWM_SAVE_BTN), MLL::GetString(IDS_FWM_SAVE_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDOK), MLL::GetString(IDS_FWM_LOAD_BTN_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //init tip links
 mp_FuseLink->SetLink(true);
 mp_FuseLink->SetTextColor(RGB(0, 0, 255));
 mp_FuseLink->SetFontUnderline(true);
 mp_FuseLink->SetLinkCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HAND));
 mp_FuseLink->SetOnClick(fastdelegate::MakeDelegate(this, &CInjDrvFWMDlg::OnFuseLinkClick));

 mp_ChecksLink->SetLink(true);
 mp_ChecksLink->SetTextColor(RGB(0, 0, 255));
 mp_ChecksLink->SetFontUnderline(true);
 mp_ChecksLink->SetLinkCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HAND));
 mp_ChecksLink->SetOnClick(fastdelegate::MakeDelegate(this, &CInjDrvFWMDlg::OnChecksLinkClick));

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CInjDrvFWMDlg::OnDestroy()
{
 Super::OnDestroy();
}

void CInjDrvFWMDlg::OnFwmCheck(UINT nID)
{
 if (m_OnChangeFwmCheck)
  m_OnChangeFwmCheck(nID - IDC_FWM_INPINV_CHECK);
}

void CInjDrvFWMDlg::OnSave()
{
 if (m_OnSaveButton)
  m_OnSaveButton();
}

void CInjDrvFWMDlg::SetFwmFlag(FwmFlag i_flag_type, bool i_state)
{
 m_fwm_flags[i_flag_type] = i_state ? true : false;
 UpdateData(FALSE);
}

bool CInjDrvFWMDlg::GetFwmFlag(FwmFlag i_flag_type)
{
 UpdateData();
 bool state = m_fwm_flags[i_flag_type] ? true : false;
 return state;
}

void CInjDrvFWMDlg::EnableFwmFlag(FwmFlag i_flag_type, bool i_enable)
{
 BOOL enable = i_enable ? true : false;
 m_fwm_checks[i_flag_type].EnableWindow(enable);
}

INT_PTR CInjDrvFWMDlg::DoModal()
{
 INT_PTR result = Super::DoModal();
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}

void CInjDrvFWMDlg::FillProcCombo(void)
{
 m_proc_combo.AddString(_T("ATmega328PB"));
 m_proc_combo.AddString(_T("ATmega328(P)")); //*
}

void CInjDrvFWMDlg::SetProcCombo(int id)
{
 m_proc_idx = id;
 UpdateData(FALSE);
}

int CInjDrvFWMDlg::GetProcCombo(void)
{
 UpdateData();
 return m_proc_idx;
}

void CInjDrvFWMDlg::OnFuseLinkClick(void)
{
 if (m_OnFuseLink)
  m_OnFuseLink();
}

void CInjDrvFWMDlg::OnChecksLinkClick(void)
{
 if (m_OnChecksLink)
  m_OnChecksLink();
}
