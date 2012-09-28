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
#include "Resources/resource.h"
#include "LogPlayerTabDlg.h"

#include "common/FastDelegate.h"
#include "common/MathHelpers.h"
#include "LPControlPanelDlg.h"
#include "MIDesk/MIDeskDlg.h"
#include "ui-core/OScopeCtrl.h"

using namespace std;

using namespace fastdelegate;

const UINT CLogPlayerTabDlg::IDD = IDD_LOG_PLAYER;

BEGIN_MESSAGE_MAP(CLogPlayerTabDlg, Super)
END_MESSAGE_MAP()

CLogPlayerTabDlg::CLogPlayerTabDlg(CWnd* pParent /*=NULL*/)
: Super(CLogPlayerTabDlg::IDD, pParent)
, mp_MIDeskDlg(new CMIDeskDlg())
, mp_LPPanelDlg(new CLPControlPanelDlg)
, mp_OScopeCtrl(new COScopeCtrl())
{
 //empty
}

void CLogPlayerTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
}

LPCTSTR CLogPlayerTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CLogPlayerTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CRect rect;
 GetDlgItem(IDC_LP_MIDESK_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);

 mp_MIDeskDlg->Create(CMIDeskDlg::IDD, this);
 mp_MIDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_MIDeskDlg->Show(true);

 GetDlgItem(IDC_LP_LOGCONTROL_PANEL_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_LPPanelDlg->Create(CLPControlPanelDlg::IDD, this);
 mp_LPPanelDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

 //инициализируем осциллограф
 _InitializeOscilloscopeControl();

 UpdateDialogControls(this,TRUE);
 return TRUE;
}

void CLogPlayerTabDlg::EnlargeMonitor(bool i_enlarge)
{
 CRect rect;
 GetClientRect(rect);

 if (i_enlarge)
 {
  mp_MIDeskDlg->GetWindowRect(m_original_mi_rect);
  ScreenToClient(m_original_mi_rect);
  CRect cp_rect;
  mp_LPPanelDlg->GetWindowRect(cp_rect);
  rect.left+=cp_rect.Width();
  CRect mi_rect = m_original_mi_rect;
  _ResizeRect(rect, mi_rect);
  mp_MIDeskDlg->Resize(mi_rect);
 }
 else
  mp_MIDeskDlg->Resize(m_original_mi_rect);
}

void CLogPlayerTabDlg::_ResizeRect(const CRect& i_external, CRect& io_victim)
{
 float Xf = ((float)i_external.Width()) / io_victim.Width();
 float Yf = ((float)i_external.Height()) / io_victim.Height();
 float factor = min(Xf, Yf);
 //масштабируем
 io_victim.right = MathHelpers::Round((io_victim.Width() * factor));
 io_victim.bottom = MathHelpers::Round((io_victim.Height() * factor));
 io_victim.left = 0;
 io_victim.top = 0;
 //центрируем
 CPoint center_external = i_external.CenterPoint();
 CPoint center_victim = io_victim.CenterPoint();
 io_victim.OffsetRect(center_external - center_victim);
}

//инициализация осциллографа для сигнала ДД
void CLogPlayerTabDlg::_InitializeOscilloscopeControl(void)
{
 CRect rect;
 GetDlgItem(IDC_LP_SIGNAL_OSCILLOSCOPE_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 // create the control
 mp_OScopeCtrl->Create(WS_VISIBLE | WS_CHILD | WS_DISABLED, rect, this);

 // customize the control
 mp_OScopeCtrl->SetRange(0.0, 5.0, 1);
 mp_OScopeCtrl->SetYUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_V_UNIT));
 mp_OScopeCtrl->SetXUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_H_UNIT));
 mp_OScopeCtrl->SetBackgroundColor(RGB(0, 0, 64));
 mp_OScopeCtrl->SetGridColor(RGB(192, 192, 255));
 mp_OScopeCtrl->SetPlotColor(RGB(255, 255, 255));
}

void CLogPlayerTabDlg::AppendKnockValue(double i_value, bool i_reverse)
{
 mp_OScopeCtrl->AppendPoint(i_value, i_reverse);
}

void CLogPlayerTabDlg::ResetKnockOscilloscope(void)
{
 mp_OScopeCtrl->Reset();
}

