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

/** \file RSDeskDlg.h
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "RSDeskDlg.h"

#include "io-core/SECU3IO.h"
#include "MIHelpers.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/WndScroller.h"


BEGIN_MESSAGE_MAP(CRSDeskDlg, CDialog)
 ON_WM_DESTROY()
END_MESSAGE_MAP()

const UINT CRSDeskDlg::IDD = IDD_RAW_SENSORS_DESK;

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg dialog

CRSDeskDlg::CRSDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CRSDeskDlg::IDD, pParent)
, m_map_value(0.0f)
, m_ubat_value(0.0f)
, m_temp_value(0.0f)
, m_knock_value(0.0f)
, m_tps_value(0.0f)
, m_add_i1_value(0.0f)
, m_add_i2_value(0.0f)
, m_add_i3_value(0.0f)
, m_add_i4_value(0.0f)
, m_enabled(-1)
, m_enable_secu3t_features(false)
, m_enable_extraio(false)
, mp_scr(new CWndScroller)
{
 //empty
}

void CRSDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_RS_MAP_VALUE, m_map_field);
 DDX_Control(pDX, IDC_RS_UBAT_VALUE, m_ubat_field);
 DDX_Control(pDX, IDC_RS_TEMP_VALUE, m_temp_field);
 DDX_Control(pDX, IDC_RS_KNOCK_VALUE, m_knock_field);
 DDX_Control(pDX, IDC_RS_TPS_VALUE, m_tps_field);
 DDX_Control(pDX, IDC_RS_ADD_I1_VALUE, m_add_i1_field);
 DDX_Control(pDX, IDC_RS_ADD_I2_VALUE, m_add_i2_field);
 DDX_Control(pDX, IDC_RS_ADD_I3_VALUE, m_add_i3_field);
 DDX_Control(pDX, IDC_RS_ADD_I4_VALUE, m_add_i4_field);

 DDX_Control(pDX, IDC_RS_MAP_CAPTION, m_map_caption);
 DDX_Control(pDX, IDC_RS_UBAT_CAPTION, m_ubat_caption);
 DDX_Control(pDX, IDC_RS_TEMP_CAPTION, m_temp_caption);
 DDX_Control(pDX, IDC_RS_KNOCK_CAPTION, m_knock_caption);
 DDX_Control(pDX, IDC_RS_TPS_CAPTION, m_tps_caption);
 DDX_Control(pDX, IDC_RS_ADD_I1_CAPTION, m_add_i1_caption);
 DDX_Control(pDX, IDC_RS_ADD_I2_CAPTION, m_add_i2_caption);
 DDX_Control(pDX, IDC_RS_ADD_I3_CAPTION, m_add_i3_caption);
 DDX_Control(pDX, IDC_RS_ADD_I4_CAPTION, m_add_i4_caption);

 DDX_Control(pDX, IDC_RS_MAP_UNIT, m_map_unit);
 DDX_Control(pDX, IDC_RS_UBAT_UNIT, m_ubat_unit);
 DDX_Control(pDX, IDC_RS_TEMP_UNIT, m_temp_unit);
 DDX_Control(pDX, IDC_RS_KNOCK_UNIT, m_knock_unit);
 DDX_Control(pDX, IDC_RS_TPS_UNIT, m_tps_unit);
 DDX_Control(pDX, IDC_RS_ADD_I1_UNIT, m_add_i1_unit);
 DDX_Control(pDX, IDC_RS_ADD_I2_UNIT, m_add_i2_unit);
 DDX_Control(pDX, IDC_RS_ADD_I3_UNIT, m_add_i3_unit);
 DDX_Control(pDX, IDC_RS_ADD_I4_UNIT, m_add_i4_unit);

 DDX_Text_Fmt(pDX,IDC_RS_MAP_VALUE, m_map_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_UBAT_VALUE, m_ubat_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_TEMP_VALUE, m_temp_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_KNOCK_VALUE, m_knock_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_TPS_VALUE, m_tps_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I1_VALUE, m_add_i1_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I2_VALUE, m_add_i2_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I3_VALUE, m_add_i3_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I4_VALUE, m_add_i4_value, _T("%.3f"));
}

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg message handlers

BOOL CRSDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();
 m_enabled = -1; //reset cache flag

 CloneWndFont(&m_map_field, &m_fieldFont, 16, true);

 m_map_field.SetFont(&m_fieldFont);
 m_ubat_field.SetFont(&m_fieldFont);
 m_temp_field.SetFont(&m_fieldFont);
 m_knock_field.SetFont(&m_fieldFont);
 m_tps_field.SetFont(&m_fieldFont);
 m_add_i1_field.SetFont(&m_fieldFont);
 m_add_i2_field.SetFont(&m_fieldFont);
 m_add_i3_field.SetFont(&m_fieldFont);
 m_add_i4_field.SetFont(&m_fieldFont);

 //initialize window scroller
 mp_scr->Init(this);
 updateScrollerSize();

 Enable(false);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CRSDeskDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

//разрешение/запрещение приборов
void CRSDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;
 m_map_field.EnableWindow(enable);
 m_ubat_field.EnableWindow(enable);
 m_temp_field.EnableWindow(enable);
 m_knock_field.EnableWindow(enable);
 m_tps_field.EnableWindow(enable);
 m_add_i1_field.EnableWindow(enable);
 m_add_i2_field.EnableWindow(enable);
 m_add_i3_field.EnableWindow(enable && !m_enable_secu3t_features);
 m_add_i4_field.EnableWindow(enable && !m_enable_secu3t_features && m_enable_extraio);

 m_map_caption.EnableWindow(enable);
 m_ubat_caption.EnableWindow(enable);
 m_temp_caption.EnableWindow(enable);
 m_knock_caption.EnableWindow(enable);
 m_tps_caption.EnableWindow(enable);
 m_add_i1_caption.EnableWindow(enable);
 m_add_i2_caption.EnableWindow(enable);
 m_add_i3_caption.EnableWindow(enable && !m_enable_secu3t_features);
 m_add_i4_caption.EnableWindow(enable && !m_enable_secu3t_features && m_enable_extraio);

 m_map_unit.EnableWindow(enable);
 m_ubat_unit.EnableWindow(enable);
 m_temp_unit.EnableWindow(enable);
 m_knock_unit.EnableWindow(enable);
 m_tps_unit.EnableWindow(enable);
 m_add_i1_unit.EnableWindow(enable);
 m_add_i2_unit.EnableWindow(enable);
 m_add_i3_unit.EnableWindow(enable && !m_enable_secu3t_features);
 m_add_i4_unit.EnableWindow(enable && !m_enable_secu3t_features && m_enable_extraio);
}

void CRSDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 int sw3i = ((show && !m_enable_secu3t_features) ? SW_SHOW : SW_HIDE);

 m_map_field.ShowWindow(sw);
 m_ubat_field.ShowWindow(sw);
 m_temp_field.ShowWindow(sw);
 m_knock_field.ShowWindow(sw);
 m_tps_field.ShowWindow(sw);
 m_add_i1_field.ShowWindow(sw);
 m_add_i2_field.ShowWindow(sw);
 m_add_i3_field.ShowWindow(sw3i);
 m_add_i4_field.ShowWindow(sw3i);

 m_map_caption.ShowWindow(sw);
 m_ubat_caption.ShowWindow(sw);
 m_temp_caption.ShowWindow(sw);
 m_knock_caption.ShowWindow(sw);
 m_tps_caption.ShowWindow(sw);
 m_add_i1_caption.ShowWindow(sw);
 m_add_i2_caption.ShowWindow(sw);
 m_add_i3_caption.ShowWindow(sw3i);
 m_add_i4_caption.ShowWindow(sw3i);

 m_map_unit.ShowWindow(sw);
 m_ubat_unit.ShowWindow(sw);
 m_temp_unit.ShowWindow(sw);
 m_knock_unit.ShowWindow(sw);
 m_tps_unit.ShowWindow(sw);
 m_add_i1_unit.ShowWindow(sw);
 m_add_i2_unit.ShowWindow(sw);
 m_add_i3_unit.ShowWindow(sw3i);
 m_add_i4_unit.ShowWindow(sw3i);
}

using namespace SECU3IO;

void CRSDeskDlg::SetValues(const RawSensDat* i_values)
{
 m_map_value = i_values->map_value;
 m_ubat_value = i_values->ubat_value;
 m_temp_value = i_values->temp_value;
 m_knock_value = i_values->knock_value;
 m_tps_value = i_values->tps_value;
 m_add_i1_value = i_values->add_i1_value;
 m_add_i2_value = i_values->add_i2_value;
 m_add_i3_value = i_values->add_i3_value;
 m_add_i4_value = i_values->add_i4_value;
 UpdateData(FALSE);
}

void CRSDeskDlg::GetValues(RawSensDat* o_values)
{
 UpdateData();
 o_values->map_value = m_map_value;
 o_values->ubat_value = m_ubat_value;
 o_values->temp_value = m_temp_value;
 o_values->knock_value = m_knock_value;
 o_values->tps_value = m_tps_value;
 o_values->add_i1_value = m_add_i1_value;
 o_values->add_i2_value = m_add_i2_value;
 o_values->add_i3_value = m_add_i3_value;
 o_values->add_i4_value = m_add_i4_value;
}

void CRSDeskDlg::EnableSECU3TItems(bool i_enable)
{
 if (i_enable == m_enable_secu3t_features)
  return; //already has needed state
 m_enable_secu3t_features = i_enable;

 updateScrollerSize();

 //in the SECU-3i only
 m_add_i3_caption.EnableWindow(m_enabled && !i_enable);
 m_add_i3_field.EnableWindow(m_enabled && !i_enable);
 m_add_i3_unit.EnableWindow(m_enabled && !i_enable);
 m_add_i4_caption.EnableWindow(m_enabled && !i_enable && m_enable_extraio);
 m_add_i4_field.EnableWindow(m_enabled && !i_enable && m_enable_extraio);
 m_add_i4_unit.EnableWindow(m_enabled && !i_enable && m_enable_extraio);

 int sw3i = ((i_enable && !m_enable_secu3t_features) ? SW_SHOW : SW_HIDE);
 m_add_i3_caption.ShowWindow(sw3i);
 m_add_i3_field.ShowWindow(sw3i);
 m_add_i3_unit.ShowWindow(sw3i);
 m_add_i4_caption.ShowWindow(sw3i);
 m_add_i4_field.ShowWindow(sw3i);
 m_add_i4_unit.ShowWindow(sw3i);
}

void CRSDeskDlg::EnableExtraIO(bool i_enable)
{
 if (i_enable == m_enable_extraio)
  return; //already has needed state
 m_enable_extraio = i_enable;
 //in the SECU-3i only
 m_add_i4_caption.EnableWindow(m_enabled && !i_enable && !m_enable_secu3t_features);
 m_add_i4_field.EnableWindow(m_enabled && !i_enable && !m_enable_secu3t_features);
 m_add_i4_unit.EnableWindow(m_enabled && !i_enable && !m_enable_secu3t_features);
}

void CRSDeskDlg::Resize(const CRect& i_rect)
{
 mp_scr->ResetScrollPos();

 //Calculate scale factors basing on the previous size of window
 CRect old_rect;
 float Xf, Yf;
 GetWindowRect(old_rect);
 MIHelpers::CalcRectToRectRatio(i_rect, old_rect, Xf, Yf);

 MoveWindow(i_rect.left, i_rect.top, i_rect.Width(), i_rect.Height());

 //Resize controls
 CRect rect;
#define _RESIZE(wnd)\
 rect = MIHelpers::GetChildWndRect(&wnd);\
 MIHelpers::ScaleRect(rect, Xf, Yf);\
 wnd.MoveWindow(rect);

 _RESIZE(m_map_field);
 _RESIZE(m_ubat_field);
 _RESIZE(m_temp_field);
 _RESIZE(m_knock_field);
 _RESIZE(m_tps_field);
 _RESIZE(m_add_i1_field);
 _RESIZE(m_add_i2_field);
 _RESIZE(m_add_i3_field);
 _RESIZE(m_add_i4_field);

 _RESIZE(m_map_caption);
 _RESIZE(m_ubat_caption);
 _RESIZE(m_temp_caption);
 _RESIZE(m_knock_caption);
 _RESIZE(m_tps_caption);
 _RESIZE(m_add_i1_caption);
 _RESIZE(m_add_i2_caption);
 _RESIZE(m_add_i3_caption);
 _RESIZE(m_add_i4_caption);

 _RESIZE(m_map_unit);
 _RESIZE(m_ubat_unit);
 _RESIZE(m_temp_unit);
 _RESIZE(m_knock_unit);
 _RESIZE(m_tps_unit);
 _RESIZE(m_add_i1_unit);
 _RESIZE(m_add_i2_unit);
 _RESIZE(m_add_i3_unit);
 _RESIZE(m_add_i4_unit);

#undef _RESIZE

 RedrawWindow();

 updateScrollerSize();
}

void CRSDeskDlg::updateScrollerSize(void)
{
 mp_scr->SetViewSizeF(.0f, m_enable_secu3t_features ? 1.0f : 1.25f);
}
