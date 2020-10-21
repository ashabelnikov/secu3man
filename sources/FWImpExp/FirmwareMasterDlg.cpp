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

/** \file FirmwareMasterDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "FirmwareMasterDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/Label.h"

const UINT CFirmwareMasterDlg::IDD = IDD_FIRMWARE_MASTER;

#define WM_KICKIDLE 0x036A //hack

/////////////////////////////////////////////////////////////////////////////
// CFirmwareMasterDlg dialog

BEGIN_MESSAGE_MAP(CFirmwareMasterDlg, Super)
 ON_WM_DESTROY()
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
 ON_COMMAND_RANGE(IDC_FWM_DWELL_CHECK, IDC_FWM_SPLIT_CHECK, OnFwmCheck)
 ON_CBN_SELCHANGE(IDC_FWM_SECU3_UNIT_COMBO, OnChangeUnitCombo)
 ON_CBN_SELCHANGE(IDC_FWM_SYNCHRO_COMBO, OnChangeSyncCombo)
END_MESSAGE_MAP()

CFirmwareMasterDlg::CFirmwareMasterDlg(CWnd* pParent /*=NULL*/)
: Super(CFirmwareMasterDlg::IDD, pParent)
, m_unit_idx(0)
, m_sync_idx(0)
, mp_FuseLink(new CLabel)
{
 for (int i = 0; i < FWM_NR_OF_FLAGS; ++i)
  m_fwm_flags[i] = false;
}

void CFirmwareMasterDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_FWM_SECU3_UNIT_COMBO, m_unit_combo);
 DDX_Control(pDX, IDC_FWM_SYNCHRO_COMBO, m_sync_combo);

 DDX_Control(pDX, IDC_FWM_DWELL_CHECK, m_fwm_checks[FWM_DWELL]);
 DDX_Control(pDX, IDC_FWM_2CHIG_CHECK, m_fwm_checks[FWM_2CHIG]);
 DDX_Control(pDX, IDC_FWM_PHIGN_CHECK, m_fwm_checks[FWM_PHIGN]);
 DDX_Control(pDX, IDC_FWM_INJ_CHECK, m_fwm_checks[FWM_INJ]);
 DDX_Control(pDX, IDC_FWM_CAFR_CHECK, m_fwm_checks[FWM_CAFR]);
 DDX_Control(pDX, IDC_FWM_OBD_CHECK, m_fwm_checks[FWM_OBD]);
 DDX_Control(pDX, IDC_FWM_TPIC_CHECK, m_fwm_checks[FWM_TPIC]);
 DDX_Control(pDX, IDC_FWM_FPWM_CHECK, m_fwm_checks[FWM_FPWM]);
 DDX_Control(pDX, IDC_FWM_SPLIT_CHECK, m_fwm_checks[FWM_SPLIT]);

 DDX_CBIndex_int(pDX, IDC_FWM_SECU3_UNIT_COMBO, m_unit_idx);
 DDX_CBIndex_int(pDX, IDC_FWM_SYNCHRO_COMBO, m_sync_idx);

 DDX_Check_bool(pDX, IDC_FWM_DWELL_CHECK, m_fwm_flags[FWM_DWELL]);
 DDX_Check_bool(pDX, IDC_FWM_2CHIG_CHECK, m_fwm_flags[FWM_2CHIG]);
 DDX_Check_bool(pDX, IDC_FWM_PHIGN_CHECK, m_fwm_flags[FWM_PHIGN]);
 DDX_Check_bool(pDX, IDC_FWM_INJ_CHECK, m_fwm_flags[FWM_INJ]);
 DDX_Check_bool(pDX, IDC_FWM_CAFR_CHECK, m_fwm_flags[FWM_CAFR]);
 DDX_Check_bool(pDX, IDC_FWM_OBD_CHECK, m_fwm_flags[FWM_OBD]);
 DDX_Check_bool(pDX, IDC_FWM_TPIC_CHECK, m_fwm_flags[FWM_TPIC]);
 DDX_Check_bool(pDX, IDC_FWM_FPWM_CHECK, m_fwm_flags[FWM_FPWM]);
 DDX_Check_bool(pDX, IDC_FWM_SPLIT_CHECK, m_fwm_flags[FWM_SPLIT]);

 DDX_Control(pDX, IDC_FWM_FUSE_LINK, *mp_FuseLink);

}

/////////////////////////////////////////////////////////////////////////////
// CFirmwareMasterDlg message handlers

LRESULT CFirmwareMasterDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
 UpdateDialogControls(this, FALSE);
 return 0L;
}

void CFirmwareMasterDlg::OnOK()
{
 //Before closing dialog, notify controller
 if (m_OnOkButton)
  m_OnOkButton();

 Super::OnOK();
}

void CFirmwareMasterDlg::OnCancel()
{
 //Before closing dialog, notify controller
 if (m_OnCancelButton)
  m_OnCancelButton();

 Super::OnCancel();
}

BOOL CFirmwareMasterDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (m_OnActivate)
  m_OnActivate();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_unit_combo, MLL::GetString(IDS_FWM_SECU3_UNIT_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_sync_combo, MLL::GetString(IDS_FWM_SYNCHRO_COMBO_TT)));
 int idc = IDC_FWM_DWELL_CHECK;
 for (UINT ids = IDS_FWM_DWELL_CHECK_TT; ids <= IDS_FWM_SPLIT_CHECK_TT; ids++, idc++)
 VERIFY(mp_ttc->AddWindow(GetDlgItem(idc), MLL::GetString(ids)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //init tip link
 mp_FuseLink->SetLink(true);
 mp_FuseLink->SetTextColor(RGB(0, 0, 255));
 mp_FuseLink->SetFontUnderline(true);
 mp_FuseLink->SetLinkCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HAND));
 mp_FuseLink->SetOnClick(fastdelegate::MakeDelegate(this, &CFirmwareMasterDlg::OnFuseLinkClick));

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CFirmwareMasterDlg::OnDestroy()
{
 Super::OnDestroy();
}

void CFirmwareMasterDlg::OnFwmCheck(UINT nID)
{
 if (m_OnChangeFwmCheck)
  m_OnChangeFwmCheck(nID - IDC_FWM_DWELL_CHECK);
}

void CFirmwareMasterDlg::OnChangeUnitCombo()
{
 if (m_OnChangeUnit)
  m_OnChangeUnit();
}

void CFirmwareMasterDlg::OnChangeSyncCombo()
{
 if (m_OnChangeSync)
  m_OnChangeSync();
}

void CFirmwareMasterDlg::FillUnitCombo(void)
{
 m_unit_combo.AddString(_T("SECU-3i"));
 m_unit_combo.AddString(_T("SECU-3T")); //*
 m_unit_combo.AddString(_T("SECU-3T (rev6)"));
 m_unit_combo.AddString(_T("SECU-3 Lite"));
 m_unit_combo.AddString(_T("SECU-3 Micro"));
}

void CFirmwareMasterDlg::FillSyncCombo(void)
{
 m_sync_combo.AddString(MLL::LoadString(IDS_FWM_SYNC_CKPS)); //*
 m_sync_combo.AddString(MLL::LoadString(IDS_FWM_SYNC_CAMS));
 m_sync_combo.AddString(MLL::LoadString(IDS_FWM_SYNC_HALL));
 m_sync_combo.AddString(MLL::LoadString(IDS_FWM_SYNC_CKPNP1));
}

void CFirmwareMasterDlg::SetUnitCombo(int id)
{
 m_unit_idx = id;
 UpdateData(FALSE);
}

void CFirmwareMasterDlg::SetSyncCombo(int id)
{
 m_sync_idx = id;
 UpdateData(FALSE);
}

void CFirmwareMasterDlg::SetFwmFlag(FwmFlag i_flag_type, bool i_state)
{
 m_fwm_flags[i_flag_type] = i_state ? true : false;
 UpdateData(FALSE);
}

bool CFirmwareMasterDlg::GetFwmFlag(FwmFlag i_flag_type)
{
 UpdateData();
 bool state = m_fwm_flags[i_flag_type] ? true : false;
 return state;
}

void CFirmwareMasterDlg::EnableFwmFlag(FwmFlag i_flag_type, bool i_enable)
{
 BOOL enable = i_enable ? true : false;
 m_fwm_checks[i_flag_type].EnableWindow(enable);
}

int CFirmwareMasterDlg::GetUnitCombo(void)
{
 UpdateData();
 return m_unit_idx;
}

int CFirmwareMasterDlg::GetSyncCombo(void)
{
 UpdateData();
 return m_sync_idx;
}

INT_PTR CFirmwareMasterDlg::DoModal()
{
 INT_PTR result = Super::DoModal();
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}

void CFirmwareMasterDlg::OnFuseLinkClick(void)
{
 if (m_OnFuseLink)
  m_OnFuseLink();
}
