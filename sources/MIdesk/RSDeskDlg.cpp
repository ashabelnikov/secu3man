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

#include "stdafx.h"
#include "resource.h"
#include "RSDeskDlg.h"

#include "io-core/SECU3IO.h"
#include "MIHelpers.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"

BEGIN_MESSAGE_MAP(CRSDeskDlg, CDialog)
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
, m_enabled(-1)
, m_enable_secu3t_features(false)
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

 DDX_Control(pDX, IDC_RS_MAP_CAPTION, m_map_caption);
 DDX_Control(pDX, IDC_RS_UBAT_CAPTION, m_ubat_caption);
 DDX_Control(pDX, IDC_RS_TEMP_CAPTION, m_temp_caption);
 DDX_Control(pDX, IDC_RS_KNOCK_CAPTION, m_knock_caption);
 DDX_Control(pDX, IDC_RS_TPS_CAPTION, m_tps_caption);
 DDX_Control(pDX, IDC_RS_ADD_I1_CAPTION, m_add_i1_caption);
 DDX_Control(pDX, IDC_RS_ADD_I2_CAPTION, m_add_i2_caption);

 DDX_Control(pDX, IDC_RS_MAP_UNIT, m_map_unit);
 DDX_Control(pDX, IDC_RS_UBAT_UNIT, m_ubat_unit);
 DDX_Control(pDX, IDC_RS_TEMP_UNIT, m_temp_unit);
 DDX_Control(pDX, IDC_RS_KNOCK_UNIT, m_knock_unit);
 DDX_Control(pDX, IDC_RS_TPS_UNIT, m_tps_unit);
 DDX_Control(pDX, IDC_RS_ADD_I1_UNIT, m_add_i1_unit);
 DDX_Control(pDX, IDC_RS_ADD_I2_UNIT, m_add_i2_unit);

 DDX_Text_Fmt(pDX,IDC_RS_MAP_VALUE, m_map_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_UBAT_VALUE, m_ubat_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_TEMP_VALUE, m_temp_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_KNOCK_VALUE, m_knock_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_TPS_VALUE, m_tps_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I1_VALUE, m_add_i1_value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I2_VALUE, m_add_i2_value, _T("%.3f"));
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

 Enable(false);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
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
 m_add_i1_field.EnableWindow(enable && m_enable_secu3t_features);
 m_add_i2_field.EnableWindow(enable && m_enable_secu3t_features);

 m_map_caption.EnableWindow(enable);
 m_ubat_caption.EnableWindow(enable);
 m_temp_caption.EnableWindow(enable);
 m_knock_caption.EnableWindow(enable);
 m_tps_caption.EnableWindow(enable);
 m_add_i1_caption.EnableWindow(enable && m_enable_secu3t_features);
 m_add_i2_caption.EnableWindow(enable && m_enable_secu3t_features);

 m_map_unit.EnableWindow(enable);
 m_ubat_unit.EnableWindow(enable);
 m_temp_unit.EnableWindow(enable);
 m_knock_unit.EnableWindow(enable);
 m_tps_unit.EnableWindow(enable);
 m_add_i1_unit.EnableWindow(enable && m_enable_secu3t_features);
 m_add_i2_unit.EnableWindow(enable && m_enable_secu3t_features);
}

void CRSDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 m_map_field.ShowWindow(sw);
 m_ubat_field.ShowWindow(sw);
 m_temp_field.ShowWindow(sw);
 m_knock_field.ShowWindow(sw);
 m_tps_field.ShowWindow(sw);
 m_add_i1_field.ShowWindow(sw);
 m_add_i2_field.ShowWindow(sw);

 m_map_caption.ShowWindow(sw);
 m_ubat_caption.ShowWindow(sw);
 m_temp_caption.ShowWindow(sw);
 m_knock_caption.ShowWindow(sw);
 m_tps_caption.ShowWindow(sw);
 m_add_i1_caption.ShowWindow(sw);
 m_add_i2_caption.ShowWindow(sw);

 m_map_unit.ShowWindow(sw);
 m_ubat_unit.ShowWindow(sw);
 m_temp_unit.ShowWindow(sw);
 m_knock_unit.ShowWindow(sw);
 m_tps_unit.ShowWindow(sw);
 m_add_i1_unit.ShowWindow(sw);
 m_add_i2_unit.ShowWindow(sw);
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
}

void CRSDeskDlg::EnableSECU3TItems(bool i_enable)
{
 if (i_enable == m_enable_secu3t_features)
  return; //already has needed state
 m_enable_secu3t_features = i_enable;
 m_add_i1_caption.EnableWindow(m_enabled && i_enable);
 m_add_i1_field.EnableWindow(m_enabled && i_enable);
 m_add_i1_unit.EnableWindow(m_enabled && i_enable);
 m_add_i2_caption.EnableWindow(m_enabled && i_enable);
 m_add_i2_field.EnableWindow(m_enabled && i_enable);
 m_add_i2_unit.EnableWindow(m_enabled && i_enable);
}

void CRSDeskDlg::Resize(const CRect& i_rect)
{
 //на основе предыдущего размера окна высчитываем коэффициенты масштабирования
 CRect old_rect;
 float Xf, Yf;
 GetWindowRect(old_rect);
 MIHelpers::CalcRectToRectRatio(i_rect, old_rect, Xf, Yf);

 MoveWindow(i_rect.left, i_rect.top, i_rect.Width(), i_rect.Height());

 //ресайзим контроллы
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

 _RESIZE(m_map_caption);
 _RESIZE(m_ubat_caption);
 _RESIZE(m_temp_caption);
 _RESIZE(m_knock_caption);
 _RESIZE(m_tps_caption);
 _RESIZE(m_add_i1_caption);
 _RESIZE(m_add_i2_caption);

 _RESIZE(m_map_unit);
 _RESIZE(m_ubat_unit);
 _RESIZE(m_temp_unit);
 _RESIZE(m_knock_unit);
 _RESIZE(m_tps_unit);
 _RESIZE(m_add_i1_unit);
 _RESIZE(m_add_i2_unit);

#undef _RESIZE

 RedrawWindow();
}
