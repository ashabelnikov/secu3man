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

/** \file InjDrvStockDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "InjDrvStockDlg.h"
#include "ui-core/ToolTipCtrlEx.h"

#define WM_KICKIDLE 0x036A //hack

const UINT CInjDrvStockDlg::IDD = IDD_INJDRV_STOCK;

/////////////////////////////////////////////////////////////////////////////
// CInjDrvStockDlg dialog

CInjDrvStockDlg::CInjDrvStockDlg(CWnd* pParent /*= NULL*/)
: Super(CInjDrvStockDlg::IDD, pParent)
{
 //empty
}

CInjDrvStockDlg::~CInjDrvStockDlg()
{
 //empty
}

void CInjDrvStockDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_INJDRV_PRESETS_LIST, m_presetsList);
}

void CInjDrvStockDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_presetsList.GetSelectedCount() > 0);
}

BEGIN_MESSAGE_MAP(CInjDrvStockDlg, Super)
 ON_WM_DESTROY()
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
 ON_UPDATE_COMMAND_UI(IDOK, OnUpdateControls)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInjDrvStockDlg message handlers

LRESULT CInjDrvStockDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
 UpdateDialogControls(this, FALSE);
 return 0L;
}

BOOL CInjDrvStockDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (m_OnActivate)
  m_OnActivate();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_presetsList, MLL::GetString(IDS_INJDRV_PRESETS_LIST_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CInjDrvStockDlg::OnDestroy()
{
 Super::OnDestroy();
}

void CInjDrvStockDlg::OnOK()
{
 //Before closing dialog, notify controller
 if (m_OnOkButton)
  m_OnOkButton();

 Super::OnOK();
}

void CInjDrvStockDlg::OnCancel()
{
 //Before closing dialog, notify controller
 if (m_OnCancelButton)
  m_OnCancelButton();

 Super::OnCancel();
}

void CInjDrvStockDlg::InsertItem(const _TSTRING& value)
{
 m_presetsList.InsertItem(m_presetsList.GetItemCount(), value.c_str());
}

int CInjDrvStockDlg::GetSelectedIndex(void)
{
 if (m_presetsList.GetSelectedCount() == 0)
  return -1;

 return m_presetsList.GetNextItem(-1, LVNI_SELECTED);
}
