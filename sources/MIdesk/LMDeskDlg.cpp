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

/** \file LMDeskDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "LMDeskDlg.h"

#include "common/GDIHelpers.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

BEGIN_MESSAGE_MAP(CLMDeskDlg, CDialog)
 ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

const UINT CLMDeskDlg::IDD = IDD_LOG_MARKS_DESK;

/////////////////////////////////////////////////////////////////////////////
// CLMDeskDlg dialog

CLMDeskDlg::CLMDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CLMDeskDlg::IDD, pParent)
, m_enabled(-1)
{
 for(int i = 0; i < KEY_COUNT; ++i)
  m_key_flags[i] = false;
 m_key_str[0] = _T("   Ctrl+1   "); m_key_colors[0] = RGB(255, 0, 0);
 m_key_str[1] = _T("   Ctrl+2   "); m_key_colors[1] = RGB(0, 255, 0);
 m_key_str[2] = _T("   Ctrl+3   "); m_key_colors[2] = RGB(0, 0, 255);
}

void CLMDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 for(int i = 0; i < KEY_COUNT; ++i)
  DDX_Control(pDX, IDC_LOG_MARKS_K1+i, m_key_text[i]);
}

/////////////////////////////////////////////////////////////////////////////
// CLMDeskDlg message handlers

BOOL CLMDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();
 m_enabled = -1; //reset cache flag

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(this, MLL::GetString(IDS_LOG_MARKS_DESK_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 Enable(false);
 return TRUE;  // return TRUE unless you set the focus to a control
}

HBRUSH CLMDeskDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
 if (nCtlColor == CTLCOLOR_STATIC)
 {
  for(int i = 0; i < KEY_COUNT; ++i)
  {
   if (m_key_flags[i] && pWnd->GetDlgCtrlID() == IDC_LOG_MARKS_K1+i)
   {
    pDC->SetTextColor(RGB(255, 255, 0));
    pDC->SetBkColor(m_key_colors[i]);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
   }
  }
 }
 return CDialog::OnCtlColor(pDC, pWnd, nCtlColor); 
}

//enable/disable fixtures
void CLMDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;
 for(int i = 0; i < KEY_COUNT; ++i)
  m_key_text[i].EnableWindow(enable);
 GetDlgItem(IDC_LP_LOG_MARKS_DESK_CAPTION)->EnableWindow(enable);
}

void CLMDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 ShowWindow(sw);
}

void CLMDeskDlg::SetValues(bool k1, bool k2, bool k3)
{
 bool new_flags[KEY_COUNT] = {k1, k2, k3};
 for(int i = 0; i < KEY_COUNT; ++i)
 {
  if (m_key_flags[i] != new_flags[i])
  {
   m_key_flags[i] = new_flags[i];
   if (m_key_flags[i])
    m_key_text[i].SetWindowText(m_key_str[i].c_str());
   else
    m_key_text[i].SetWindowText(_T(""));
  }
 }
}

void CLMDeskDlg::Resize(const CRect& i_rect)
{
 //todo
}
