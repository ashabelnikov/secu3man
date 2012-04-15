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
#include "ui-core/fnt_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CDVDeskDlg, Super)
 ON_WM_DESTROY()
 ON_COMMAND_RANGE(IDC_DV_BASE1_CHECK, IDC_DV_BASE4_CHECK, OnBaseCheck)
END_MESSAGE_MAP()

const UINT CDVDeskDlg::IDD = IDD_DBGVAR_DESK;

/////////////////////////////////////////////////////////////////////////////
// CDVDeskDlg dialog

CDVDeskDlg::CDVDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CDVDeskDlg::IDD, pParent)
, m_update_period(100)
, m_was_initialized(false)
, m_enabled(-1)
{
 for(size_t i = 0; i < VU_SIZE; ++i)
 {
  m_vu[i].var_value = 0;
  m_vu[i].base_fmt = _T("0x%04X");
 }
}

void CDVDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 for(size_t i = 0; i < VU_SIZE; ++i)
 {
  DDX_Control(pDX, IDC_DV_VAR1_VALUE + i, m_vu[i].var_field);
  DDX_Control(pDX, IDC_DV_VAR1_CAPTION + i, m_vu[i].var_caption);
  DDX_Control(pDX, IDC_DV_BASE1_CHECK + i, m_vu[i].base_check);
  DDX_Text_Fmt(pDX,IDC_DV_VAR1_VALUE + i, m_vu[i].var_value, m_vu[i].base_fmt.c_str());
 }
}

/////////////////////////////////////////////////////////////////////////////
// CDVDeskDlg message handlers

BOOL CDVDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //Set font of fields
 CloneWndFont(&m_vu[0].var_field, &m_fieldFont, 11, true);
 for(size_t i = 0; i < VU_SIZE; ++i)
  m_vu[i].var_field.SetFont(&m_fieldFont);

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

void CDVDeskDlg::OnBaseCheck(UINT nID)
{
 size_t index = nID - IDC_DV_BASE1_CHECK;
 bool checked = m_vu[index].base_check.GetCheck() == BST_CHECKED;
 m_vu[index].base_fmt = checked ? _T("%05d") : _T("0x%04X");
 UpdateData(FALSE);
}

//разрешение/запрещение приборов
void CDVDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;

 for(size_t i = 0; i < VU_SIZE; ++i)
 {
  m_vu[i].var_field.EnableWindow(enable);
  m_vu[i].var_caption.EnableWindow(enable);
  m_vu[i].base_check.EnableWindow(enable);
 }
}

bool CDVDeskDlg::IsEnabled(void)
{
 return m_enabled;
}

void CDVDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);

 for(size_t i = 0; i < VU_SIZE; ++i)
 {
  m_vu[i].var_field.ShowWindow(sw);
  m_vu[i].var_caption.ShowWindow(sw);
  m_vu[i].base_check.ShowWindow(sw);
 }
 ShowWindow(sw);
}

using namespace SECU3IO;

void CDVDeskDlg::SetValues(const DbgvarDat* i_values)
{
 m_vu[0].var_value = i_values->var1;
 m_vu[1].var_value = i_values->var2;
 m_vu[2].var_value = i_values->var3;
 m_vu[3].var_value = i_values->var4;
}

void CDVDeskDlg::GetValues(DbgvarDat* o_values)
{
 UpdateData();
 o_values->var1 = m_vu[0].var_value;
 o_values->var2 = m_vu[1].var_value;
 o_values->var3 = m_vu[2].var_value;
 o_values->var4 = m_vu[3].var_value;
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
