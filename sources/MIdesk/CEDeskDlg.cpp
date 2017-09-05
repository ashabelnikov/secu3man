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

/** \file CEDeskDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "CEDeskDlg.h"

#include "io-core/bitmask.h"
#include "io-core/ce_errors.h"
#include "io-core/SECU3IO.h"
#include "MIHelpers.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

BEGIN_MESSAGE_MAP(CCEDeskDlg, CDialog)
END_MESSAGE_MAP()

const UINT CCEDeskDlg::IDD = IDD_CE_ERRORS_DESK;

/////////////////////////////////////////////////////////////////////////////
// CCEDeskDlg dialog

CCEDeskDlg::CCEDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CCEDeskDlg::IDD, pParent)
, m_enabled(-1)
{
 //empty
}

void CCEDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 for(size_t i = 0; i < SECU3_CE_ERRCODES_COUNT; ++i)
  DDX_Control(pDX, IDC_CE_DESK_0 + i, m_CEErrors[i]);
 DDX_Control(pDX, IDC_CE_DESK_CE, m_ce_text);
}

/////////////////////////////////////////////////////////////////////////////
// CCEDeskDlg message handlers

BOOL CCEDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();
 m_enabled = -1; //reset cache flag

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(this, MLL::GetString(IDS_CE_ERRORS_DESK_TT)));
 for(int i = IDS_CED_CKPS_MALFUNCTION_TT; i <= IDS_CED_ADD_I4_SENSOR_TT; ++i)
 {
  VERIFY(mp_ttc->AddWindow(&m_CEErrors[i - IDS_CED_CKPS_MALFUNCTION_TT], MLL::GetString(i)));
 }
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 Enable(false);
 return TRUE;  // return TRUE unless you set the focus to a control
}

//enable/disable fixtures
void CCEDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;
 for(size_t i = 0; i < SECU3_CE_ERRCODES_COUNT; ++i)
  m_CEErrors[i].EnableWindow(enable);
 m_ce_text.EnableWindow(enable);
}

void CCEDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 ShowWindow(sw);
}

void CCEDeskDlg::SetValues(WORD i_errors)
{
 for(size_t i = 0; i < SECU3_CE_ERRCODES_COUNT; ++i)
 {
  if (CHECKBIT16(i_errors, secu3_ce_error_codes[i].first))
  {
   TCHAR buff[10] = {0};
   _stprintf(buff, _T("%d"), secu3_ce_error_codes[i].second);
   m_CEErrors[i].SetWindowText(buff);
  }
  else
   m_CEErrors[i].SetWindowText(_T(""));
 }
}

void CCEDeskDlg::Resize(const CRect& i_rect)
{
 //Calculate scale factors basing on previous size
 CRect old_rect;
 float Xf, Yf;
 GetWindowRect(old_rect);
 MIHelpers::CalcRectToRectRatio(i_rect, old_rect, Xf, Yf);

 MoveWindow(i_rect.left, i_rect.top, i_rect.Width(), i_rect.Height());

 //resize controls
 CRect rect;
#define _RESIZE(wnd)\
 rect = MIHelpers::GetChildWndRect(&wnd);\
 MIHelpers::ScaleRect(rect, Xf, Yf);\
 wnd.MoveWindow(rect);

 for(size_t i = 0; i < SECU3_CE_ERRCODES_COUNT; ++i) {
  _RESIZE(m_CEErrors[i]);
 }

#undef _RESIZE

 RedrawWindow();
}
