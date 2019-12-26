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

/** \file LogPlayerTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "LogPlayerTabDlg.h"

#include "about/secu-3about.h"
#include "common/DPIAware.h"
#include "common/FastDelegate.h"
#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
#include "LPControlPanelDlg.h"
#include "MIDesk/LMDeskDlg.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/CEDeskDlg.h"
#include "ui-core/OscillCtrl.h"
#include "ui-core/ToolTipCtrlEx.h"

using namespace std;
using namespace fastdelegate;

const UINT CLogPlayerTabDlg::IDD = IDD_LOG_PLAYER;

BEGIN_MESSAGE_MAP(CLogPlayerTabDlg, Super)
 ON_WM_DROPFILES()
 ON_WM_SIZE()
 ON_WM_DESTROY()
 ON_UPDATE_COMMAND_UI(IDC_LP_LOG_MARKS_DESK_CAPTION, OnUpdateControls)
END_MESSAGE_MAP()

CLogPlayerTabDlg::CLogPlayerTabDlg(CWnd* pParent /*=NULL*/)
: Super(CLogPlayerTabDlg::IDD, pParent)
, mp_CEDeskDlg(new CCEDeskDlg())
, mp_LMDeskDlg(new CLMDeskDlg())
, mp_MIDeskDlg(new CMIDeskDlg())
, mp_LPPanelDlg(new CLPControlPanelDlg)
, m_initialized(false)
, m_all_enabled(false)
{
 //=================================================================
 if (!CheckBitmaps() || !CheckAppMenu())
  delete this;
 //=================================================================
}

void CLogPlayerTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
}

void CLogPlayerTabDlg::OnDropFiles(HDROP hDropInfo)
{
 TCHAR fileName[MAX_PATH+1];
 if (DragQueryFile(hDropInfo, 0, fileName, MAX_PATH))
 {
  if (m_OnDropFile)
   m_OnDropFile(fileName);
 }
 else
  MessageBeep(MB_ICONEXCLAMATION);
}

LPCTSTR CLogPlayerTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CLogPlayerTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CRect rect;

 rect = GDIHelpers::GetChildWndRect(this, IDC_LP_MIDESK_FRAME);
 mp_MIDeskDlg->Create(CMIDeskDlg::IDD, this, rect);
 mp_MIDeskDlg->Show(true);

 rect = GDIHelpers::GetChildWndRect(this, IDC_LP_CEDESK_FRAME);
 mp_CEDeskDlg->Create(CCEDeskDlg::IDD, this);
 mp_CEDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_CEDeskDlg->Show(true);

 rect = GDIHelpers::GetChildWndRect(this, IDC_LP_LOGCONTROL_PANEL_FRAME);
 mp_LPPanelDlg->Create(CLPControlPanelDlg::IDD, this);
 mp_LPPanelDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 
 rect = GDIHelpers::GetChildWndRect(this, IDC_LP_LOG_MARKS_DESK_HOLDER);
 mp_LMDeskDlg->Create(CLMDeskDlg::IDD, this);
 mp_LMDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_LMDeskDlg->Show(true);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //Enable drap & drop functionality
 DragAcceptFiles(true);

 UpdateDialogControls(this,TRUE);
 m_initialized = true;
 return TRUE;
}

void CLogPlayerTabDlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}

void CLogPlayerTabDlg::setOnDropFile(EventString OnFunction)
{m_OnDropFile = OnFunction;}

void CLogPlayerTabDlg::OnSize( UINT nType, int cx, int cy )
{
 if (m_initialized)
 {
  CRect rc1, rc2, etc_rc;

  DPIAware da;
  rc1 = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
  GetClientRect(&rc2);
  mp_CEDeskDlg->MoveWindow(rc1.left, rc2.bottom  - rc1.Height() - da.ScaleY(3), rc1.Width(), rc1.Height());
  
  rc1 = GDIHelpers::GetChildWndRect(mp_MIDeskDlg.get());
  rc2 = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
  mp_MIDeskDlg->SetWindowPos(NULL, 0, 0, cx - rc1.left - da.ScaleX(3), rc2.top - rc1.top - da.ScaleY(3), SWP_NOMOVE | SWP_NOZORDER);
 }

 Super::OnSize(nType, cx, cy);
}

void CLogPlayerTabDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_all_enabled);
}

void CLogPlayerTabDlg::EnableAll(bool i_enable)
{
 m_all_enabled = i_enable;
 UpdateDialogControls(this,TRUE);
}
