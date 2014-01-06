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
#include "resource.h"
#include "LMDeskDlg.h"

#include "MIHelpers.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

BEGIN_MESSAGE_MAP(CLMDeskDlg, CDialog)
END_MESSAGE_MAP()

const UINT CLMDeskDlg::IDD = IDD_LOG_MARKS_DESK;

/////////////////////////////////////////////////////////////////////////////
// CLMDeskDlg dialog

CLMDeskDlg::CLMDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CLMDeskDlg::IDD, pParent)
, m_enabled(-1)
{
 //empty
}

void CLMDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_LOG_MARKS_K1, m_key_text[0]);
 DDX_Control(pDX, IDC_LOG_MARKS_K2, m_key_text[1]);
 DDX_Control(pDX, IDC_LOG_MARKS_K3, m_key_text[2]);
}

/////////////////////////////////////////////////////////////////////////////
// CLMDeskDlg message handlers

BOOL CLMDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();
 m_enabled = -1; //reset cache flag

 //create a tooltip control and assign tooltips
// mp_ttc.reset(new CToolTipCtrlEx());
// VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
// mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
// mp_ttc->ActivateToolTips(true);

 Enable(false);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

//разрешение/запрещение приборов
void CLMDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;
 m_key_text[0].EnableWindow(enable);
 m_key_text[1].EnableWindow(enable);
 m_key_text[2].EnableWindow(enable);
}

void CLMDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 ShowWindow(sw);
}

void CLMDeskDlg::SetValues(bool k1, bool k2, bool k3)
{
 TCHAR buff[10] = {0};
 _stprintf(buff, _T("%s"), k1 ? _T("Ctrl+1") : _T(""));
 m_key_text[0].SetWindowText(buff);
 _stprintf(buff, _T("%s"), k2 ? _T("Ctrl+2") : _T(""));
 m_key_text[1].SetWindowText(buff);
 _stprintf(buff, _T("%s"), k3 ? _T("Ctrl+3") : _T(""));
 m_key_text[2].SetWindowText(buff);
}

void CLMDeskDlg::Resize(const CRect& i_rect)
{
 //todo
}
