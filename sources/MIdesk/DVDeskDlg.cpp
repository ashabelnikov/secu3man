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
#include "DVDeskDlg.h"

#include "io-core/SECU3IO.h"
#include "MIHelpers.h"
#include "ui-core/ddx_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CDVDeskDlg, Super)
 ON_WM_DESTROY()
END_MESSAGE_MAP()

const UINT CDVDeskDlg::IDD = IDD_DBGVAR_DESK;

/////////////////////////////////////////////////////////////////////////////
// CDVDeskDlg dialog

CDVDeskDlg::CDVDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CDVDeskDlg::IDD, pParent)
, m_var1_value(0)
, m_var2_value(0)
, m_var3_value(0)
, m_var4_value(0)
, m_update_period(100)
, m_was_initialized(false)
, m_enabled(-1)
{
 //empty
}

void CDVDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_DV_VAR1_VALUE, m_var1_field);
 DDX_Control(pDX, IDC_DV_VAR2_VALUE, m_var2_field);
 DDX_Control(pDX, IDC_DV_VAR3_VALUE, m_var3_field);
 DDX_Control(pDX, IDC_DV_VAR4_VALUE, m_var4_field);

 DDX_Control(pDX, IDC_DV_VAR1_CAPTION, m_var1_caption);
 DDX_Control(pDX, IDC_DV_VAR2_CAPTION, m_var2_caption);
 DDX_Control(pDX, IDC_DV_VAR3_CAPTION, m_var3_caption);
 DDX_Control(pDX, IDC_DV_VAR4_CAPTION, m_var4_caption);

 DDX_Text_Fmt(pDX,IDC_DV_VAR1_VALUE, m_var1_value, _T("0x%04X"));
 DDX_Text_Fmt(pDX,IDC_DV_VAR2_VALUE, m_var2_value, _T("0x%04X"));
 DDX_Text_Fmt(pDX,IDC_DV_VAR3_VALUE, m_var3_value, _T("0x%04X"));
 DDX_Text_Fmt(pDX,IDC_DV_VAR4_VALUE, m_var4_value, _T("0x%04X"));
}

/////////////////////////////////////////////////////////////////////////////
// CDVDeskDlg message handlers

BOOL CDVDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CFont font;
 VERIFY(font.CreateFont(
   16,                        // nHeight
   0,                         // nWidth
   0,                         // nEscapement
   0,                         // nOrientation
   FW_BOLD,                   // nWeight
   FALSE,                     // bItalic
   FALSE,                     // bUnderline
   0,                         // cStrikeOut
   ANSI_CHARSET,              // nCharSet
   OUT_DEFAULT_PRECIS,        // nOutPrecision
   CLIP_DEFAULT_PRECIS,       // nClipPrecision
   DEFAULT_QUALITY,           // nQuality
   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
   _T("Arial")));             // lpszFacename

 m_var1_field.SetFont(&font);
 m_var2_field.SetFont(&font);
 m_var3_field.SetFont(&font);
 m_var4_field.SetFont(&font);

 Enable(false);
 UpdateData(FALSE);

 m_update_timer.SetTimer(this, &CDVDeskDlg::OnUpdateTimer, m_update_period);
 m_was_initialized = true;

 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CDVDeskDlg::OnDestroy()
{
 m_was_initialized = false;
 Super::OnDestroy();
 m_update_timer.KillTimer();
}

//разрешение/запрещение приборов
void CDVDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;
 m_var1_field.EnableWindow(enable);
 m_var2_field.EnableWindow(enable);
 m_var3_field.EnableWindow(enable);
 m_var4_field.EnableWindow(enable);

 m_var1_caption.EnableWindow(enable);
 m_var2_caption.EnableWindow(enable);
 m_var3_caption.EnableWindow(enable);
 m_var4_caption.EnableWindow(enable);
}

bool CDVDeskDlg::IsEnabled(void)
{
 return m_enabled;
}

void CDVDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 m_var1_field.ShowWindow(sw);
 m_var2_field.ShowWindow(sw);
 m_var3_field.ShowWindow(sw);
 m_var4_field.ShowWindow(sw);

 m_var1_caption.ShowWindow(sw);
 m_var2_caption.ShowWindow(sw);
 m_var3_caption.ShowWindow(sw);
 m_var4_caption.ShowWindow(sw);
 ShowWindow(sw);
}

using namespace SECU3IO;

void CDVDeskDlg::SetValues(const DbgvarDat* i_values)
{
 m_var1_value = i_values->var1;
 m_var2_value = i_values->var2;
 m_var3_value = i_values->var3;
 m_var4_value = i_values->var4;
}

void CDVDeskDlg::GetValues(DbgvarDat* o_values)
{
 UpdateData();
 o_values->var1 = m_var1_value;
 o_values->var2 = m_var2_value;
 o_values->var3 = m_var3_value;
 o_values->var4 = m_var4_value;
}

void CDVDeskDlg::OnUpdateTimer(void)
{
 if (!m_was_initialized)
  return;
 UpdateData(FALSE);  //<--slow process
}

void CDVDeskDlg::SetUpdatePeriod(unsigned int i_period)
{
 m_update_period = i_period;
 if (m_was_initialized)
 {
  m_update_timer.KillTimer();
  m_update_timer.SetTimer(this, &CDVDeskDlg::OnUpdateTimer, m_update_period);
 }
}
