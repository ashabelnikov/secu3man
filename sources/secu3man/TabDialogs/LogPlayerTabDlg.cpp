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
 ON_WM_MOUSEMOVE()
 ON_WM_LBUTTONDOWN()
 ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CLogPlayerTabDlg::CLogPlayerTabDlg(CWnd* pParent /*=NULL*/)
: Super(CLogPlayerTabDlg::IDD, pParent)
, mp_CEDeskDlg(new CCEDeskDlg())
, mp_LMDeskDlg(new CLMDeskDlg())
, mp_MIDeskDlg(new CMIDeskDlg())
, mp_LPPanelDlg(new CLPControlPanelDlg)
, m_initialized(false)
, m_all_enabled(false)
, m_splitterPos(243)
, m_moveSplitter(false)
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

 ///////////////////////////////////////////////
 CRect rcMI, rc;
 mp_MIDeskDlg->GetWindowRect(&rcMI);
 mp_CEDeskDlg->GetWindowRect(&rc);
 m_miMargin = rc.top - rcMI.bottom;
 ///////////////////////////////////////////////

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

void CLogPlayerTabDlg::OnMouseMove(UINT nFlags, CPoint point)
{
 CRect pd_rc = GDIHelpers::GetChildWndRect(mp_LPPanelDlg.get());
 CRect ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
 CRect captRect(pd_rc.right, pd_rc.top, ce_rc.left, ce_rc.bottom);

 if (captRect.PtInRect(point))
  ::SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZEWE, IMAGE_CURSOR, 0, 0, LR_SHARED));

 if (m_moveSplitter)
 {
  _MoveSplitter(point.x, m_moveStart.x);
  
  //Update saved position 
  pd_rc = GDIHelpers::GetChildWndRect(mp_LPPanelDlg.get());
  ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
  m_splitterPos = (pd_rc.right + ce_rc.left) / 2;
 }

 Super::OnMouseMove(nFlags, point);
}

void CLogPlayerTabDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
 CRect pd_rc = GDIHelpers::GetChildWndRect(mp_LPPanelDlg.get());
 CRect ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
 CRect captRect(pd_rc.right, pd_rc.top, ce_rc.left, ce_rc.bottom);

 if (captRect.PtInRect(point))
 {
  ::SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZEWE, IMAGE_CURSOR, 0, 0, LR_SHARED));
  m_moveSplitter = true;
  m_moveStart = point;
  m_moveStrtWidthPD = pd_rc.Width();
  m_moveStrtRectMI = GDIHelpers::GetChildWndRect(mp_MIDeskDlg.get());
  SetCapture();
 }

 return Super::OnLButtonDown(nFlags, point);
}

void CLogPlayerTabDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
 Super::OnLButtonUp(nFlags, point);
 m_moveSplitter = false;
 ReleaseCapture();
}

void CLogPlayerTabDlg::SetSplitterPos(int pos)
{
 DPIAware da;
 m_splitterPos = da.ScaleX(pos);

 CRect pd_rc = GDIHelpers::GetChildWndRect(mp_LPPanelDlg.get());
 CRect ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
 m_moveStrtRectMI = GDIHelpers::GetChildWndRect(mp_MIDeskDlg.get());
 m_moveStrtWidthPD = pd_rc.Width();

 _MoveSplitter(m_splitterPos, (pd_rc.right + ce_rc.left) / 2);
}

int CLogPlayerTabDlg::GetSplitterPos(void) const
{
 DPIAware da;
 return da.UnScaleX(m_splitterPos);
}

void CLogPlayerTabDlg::_MoveSplitter(int x, int start_x)
{
 CRect pd_rc = GDIHelpers::GetChildWndRect(mp_LPPanelDlg.get());
 CRect ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());

 DPIAware da;
 CRect rc;
 GetClientRect(&rc);

 //restrict splitter position
 if (x < da.ScaleX(5))
  x = da.ScaleX(5);
 if (x > rc.right - m_miMargin - da.ScaleX(5))
  x = rc.right - m_miMargin - da.ScaleX(5);

 int dx = x - start_x;

 mp_LPPanelDlg->SetWindowPos(NULL, 0, 0, m_moveStrtWidthPD + dx, pd_rc.Height(), SWP_NOMOVE | SWP_NOZORDER);

 CRect lm_rc = GDIHelpers::GetChildWndRect(mp_LMDeskDlg.get());
 mp_LMDeskDlg->SetWindowPos(NULL, 0, 0, m_moveStrtWidthPD + dx, lm_rc.Height(), SWP_NOMOVE | SWP_NOZORDER);

 rc = GDIHelpers::GetChildWndRect(mp_MIDeskDlg.get());
 mp_MIDeskDlg->MoveWindow(m_moveStrtRectMI.left + dx, rc.top, m_moveStrtRectMI.Width() - dx, rc.Height());

 mp_CEDeskDlg->SetWindowPos(NULL, m_moveStrtRectMI.left + dx, ce_rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
