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
#include "common/GDIHelpers.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/WndScroller.h"


void CRSDeskDlg::Input::StoreRects(void)
{
 for (int i = 0; i < 3; ++i)
  rect[i] = GDIHelpers::GetChildWndRect(&ctrl[i]);
}

void CRSDeskDlg::Input::Scale(float Xf, float Yf)
{
 for (int i = 0; i < 3; ++i)
 {
  CRect rc = rect[i];
  GDIHelpers::ScaleRect(rc, Xf, Yf);
  ctrl[i].MoveWindow(rc);
  ctrl[i].Invalidate();
 }
}

BEGIN_MESSAGE_MAP(CRSDeskDlg, CDialog)
 ON_WM_DESTROY()
 ON_WM_SIZE()
END_MESSAGE_MAP()

const UINT CRSDeskDlg::IDD = IDD_RAW_SENSORS_DESK;

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg dialog

CRSDeskDlg::CRSDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CRSDeskDlg::IDD, pParent)
, m_enabled(-1)
, m_enable_secu3t_features(false)
, m_enable_extraio(false)
, m_enable_spiadc(false)
, m_was_initialized(false)
, mp_scr(new CWndScroller)
{
 //empty
}

CRSDeskDlg::~CRSDeskDlg()
{
 //empty
}

void CRSDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 //field
 DDX_Control(pDX, IDC_RS_MAP_VALUE, m_map_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_UBAT_VALUE, m_ubat_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_TEMP_VALUE, m_temp_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_KNOCK_VALUE, m_knock_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_TPS_VALUE, m_tps_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_ADD_I1_VALUE, m_add_i1_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_ADD_I2_VALUE, m_add_i2_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_ADD_I3_VALUE, m_add_i3_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_ADD_I4_VALUE, m_add_i4_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_ADD_I5_VALUE, m_add_i5_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_ADD_I6_VALUE, m_add_i6_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_ADD_I7_VALUE, m_add_i7_inp.ctrl[0]);
 DDX_Control(pDX, IDC_RS_ADD_I8_VALUE, m_add_i8_inp.ctrl[0]);

 //caption
 DDX_Control(pDX, IDC_RS_MAP_CAPTION, m_map_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_UBAT_CAPTION, m_ubat_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_TEMP_CAPTION, m_temp_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_KNOCK_CAPTION, m_knock_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_TPS_CAPTION, m_tps_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_ADD_I1_CAPTION, m_add_i1_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_ADD_I2_CAPTION, m_add_i2_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_ADD_I3_CAPTION, m_add_i3_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_ADD_I4_CAPTION, m_add_i4_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_ADD_I5_CAPTION, m_add_i5_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_ADD_I6_CAPTION, m_add_i6_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_ADD_I7_CAPTION, m_add_i7_inp.ctrl[1]);
 DDX_Control(pDX, IDC_RS_ADD_I8_CAPTION, m_add_i8_inp.ctrl[1]);

 //unit
 DDX_Control(pDX, IDC_RS_MAP_UNIT, m_map_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_UBAT_UNIT, m_ubat_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_TEMP_UNIT, m_temp_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_KNOCK_UNIT, m_knock_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_TPS_UNIT, m_tps_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_ADD_I1_UNIT, m_add_i1_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_ADD_I2_UNIT, m_add_i2_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_ADD_I3_UNIT, m_add_i3_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_ADD_I4_UNIT, m_add_i4_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_ADD_I5_UNIT, m_add_i5_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_ADD_I6_UNIT, m_add_i6_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_ADD_I7_UNIT, m_add_i7_inp.ctrl[2]);
 DDX_Control(pDX, IDC_RS_ADD_I8_UNIT, m_add_i8_inp.ctrl[2]);

 DDX_Text_Fmt(pDX,IDC_RS_MAP_VALUE, m_map_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_UBAT_VALUE, m_ubat_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_TEMP_VALUE, m_temp_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_KNOCK_VALUE, m_knock_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_TPS_VALUE, m_tps_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I1_VALUE, m_add_i1_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I2_VALUE, m_add_i2_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I3_VALUE, m_add_i3_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I4_VALUE, m_add_i4_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I5_VALUE, m_add_i5_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I6_VALUE, m_add_i6_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I7_VALUE, m_add_i7_inp.value, _T("%.3f"));
 DDX_Text_Fmt(pDX,IDC_RS_ADD_I8_VALUE, m_add_i8_inp.value, _T("%.3f"));
}

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg message handlers

BOOL CRSDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();
 m_enabled = -1; //reset cache flag

 CloneWndFont(&m_map_inp.ctrl[0], &m_fieldFont, 16, true);

 m_map_inp.ctrl[0].SetFont(&m_fieldFont);
 m_ubat_inp.ctrl[0].SetFont(&m_fieldFont);
 m_temp_inp.ctrl[0].SetFont(&m_fieldFont);
 m_knock_inp.ctrl[0].SetFont(&m_fieldFont);
 m_tps_inp.ctrl[0].SetFont(&m_fieldFont);
 m_add_i1_inp.ctrl[0].SetFont(&m_fieldFont);
 m_add_i2_inp.ctrl[0].SetFont(&m_fieldFont);
 m_add_i3_inp.ctrl[0].SetFont(&m_fieldFont);
 m_add_i4_inp.ctrl[0].SetFont(&m_fieldFont);
 m_add_i5_inp.ctrl[0].SetFont(&m_fieldFont);
 m_add_i6_inp.ctrl[0].SetFont(&m_fieldFont);
 m_add_i7_inp.ctrl[0].SetFont(&m_fieldFont);
 m_add_i8_inp.ctrl[0].SetFont(&m_fieldFont);

 //initialize window scroller
 mp_scr->Init(this);

 Enable(false);
 UpdateData(FALSE);

 //Store iriginal rects
 m_map_inp.StoreRects();
 m_ubat_inp.StoreRects();
 m_temp_inp.StoreRects(); 
 m_knock_inp.StoreRects();
 m_tps_inp.StoreRects();
 m_add_i1_inp.StoreRects();
 m_add_i2_inp.StoreRects();
 m_add_i3_inp.StoreRects();
 m_add_i4_inp.StoreRects();
 m_add_i5_inp.StoreRects();
 m_add_i6_inp.StoreRects();
 m_add_i7_inp.StoreRects();
 m_add_i8_inp.StoreRects();

 GetClientRect(m_origRect);

 m_was_initialized = true;

 updateScrollerSize();

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CRSDeskDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
 m_was_initialized = false;
}

//разрешение/запрещение приборов
void CRSDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;

 for (int i = 0; i < 3; ++i)
 {
  m_map_inp.ctrl[i].EnableWindow(enable);
  m_ubat_inp.ctrl[i].EnableWindow(enable);
  m_temp_inp.ctrl[i].EnableWindow(enable);
  m_knock_inp.ctrl[i].EnableWindow(enable);
  m_tps_inp.ctrl[i].EnableWindow(enable);
  m_add_i1_inp.ctrl[i].EnableWindow(enable);
  m_add_i2_inp.ctrl[i].EnableWindow(enable);
  m_add_i3_inp.ctrl[i].EnableWindow(enable && !m_enable_secu3t_features);
  m_add_i4_inp.ctrl[i].EnableWindow(enable && !m_enable_secu3t_features && m_enable_extraio);
  m_add_i5_inp.ctrl[i].EnableWindow(enable && !m_enable_secu3t_features && m_enable_spiadc);
  m_add_i6_inp.ctrl[i].EnableWindow(enable && !m_enable_secu3t_features && m_enable_spiadc);
  m_add_i7_inp.ctrl[i].EnableWindow(enable && !m_enable_secu3t_features && m_enable_spiadc);
  m_add_i8_inp.ctrl[i].EnableWindow(enable && !m_enable_secu3t_features && m_enable_spiadc);
 }
}

void CRSDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 int sw3i = ((show && !m_enable_secu3t_features) ? SW_SHOW : SW_HIDE);

 for (int i = 0; i < 3; ++i)
 {
  m_map_inp.ctrl[i].ShowWindow(sw);
  m_ubat_inp.ctrl[i].ShowWindow(sw);
  m_temp_inp.ctrl[i].ShowWindow(sw);
  m_knock_inp.ctrl[i].ShowWindow(sw);
  m_tps_inp.ctrl[i].ShowWindow(sw);
  m_add_i1_inp.ctrl[i].ShowWindow(sw);
  m_add_i2_inp.ctrl[i].ShowWindow(sw);
  m_add_i3_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i4_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i5_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i6_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i7_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i8_inp.ctrl[i].ShowWindow(sw3i);
 }
}

using namespace SECU3IO;

void CRSDeskDlg::SetValues(const RawSensDat* i_values)
{
 m_map_inp.value = i_values->map_value;
 m_ubat_inp.value = i_values->ubat_value;
 m_temp_inp.value = i_values->temp_value;
 m_knock_inp.value = i_values->knock_value;
 m_tps_inp.value = i_values->tps_value;
 m_add_i1_inp.value = i_values->add_i1_value;
 m_add_i2_inp.value = i_values->add_i2_value;
 m_add_i3_inp.value = i_values->add_i3_value;
 m_add_i4_inp.value = i_values->add_i4_value;
 m_add_i5_inp.value = i_values->add_i5_value;
 m_add_i6_inp.value = i_values->add_i6_value;
 m_add_i7_inp.value = i_values->add_i7_value;
 m_add_i8_inp.value = i_values->add_i8_value;
 UpdateData(FALSE);
}

void CRSDeskDlg::GetValues(RawSensDat* o_values)
{
 UpdateData();
 o_values->map_value = m_map_inp.value;
 o_values->ubat_value = m_ubat_inp.value;
 o_values->temp_value = m_temp_inp.value;
 o_values->knock_value = m_knock_inp.value;
 o_values->tps_value = m_tps_inp.value;
 o_values->add_i1_value = m_add_i1_inp.value;
 o_values->add_i2_value = m_add_i2_inp.value;
 o_values->add_i3_value = m_add_i3_inp.value;
 o_values->add_i4_value = m_add_i4_inp.value;
 o_values->add_i5_value = m_add_i5_inp.value;
 o_values->add_i6_value = m_add_i6_inp.value;
 o_values->add_i7_value = m_add_i7_inp.value;
 o_values->add_i8_value = m_add_i8_inp.value;
}

void CRSDeskDlg::EnableSECU3TItems(bool i_enable)
{
 if (i_enable == m_enable_secu3t_features)
  return; //already has needed state
 m_enable_secu3t_features = i_enable;

 updateScrollerSize();

 //in the SECU-3i only
 for (int i = 0; i < 3; ++i)
 {
  m_add_i3_inp.ctrl[i].EnableWindow(m_enabled && !i_enable);
  m_add_i4_inp.ctrl[i].EnableWindow(m_enabled && !i_enable && m_enable_extraio);
  m_add_i5_inp.ctrl[i].EnableWindow(m_enabled && !i_enable && m_enable_spiadc);
  m_add_i6_inp.ctrl[i].EnableWindow(m_enabled && !i_enable && m_enable_spiadc);
  m_add_i7_inp.ctrl[i].EnableWindow(m_enabled && !i_enable && m_enable_spiadc);
  m_add_i8_inp.ctrl[i].EnableWindow(m_enabled && !i_enable && m_enable_spiadc);
  int sw3i = ((!i_enable) ? SW_SHOW : SW_HIDE);
  m_add_i3_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i4_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i5_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i6_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i7_inp.ctrl[i].ShowWindow(sw3i);
  m_add_i8_inp.ctrl[i].ShowWindow(sw3i);
 }
}

void CRSDeskDlg::EnableExtraIO(bool i_enable)
{
 if (i_enable == m_enable_extraio)
  return; //already has needed state
 m_enable_extraio = i_enable;
 //in the SECU-3i only
 for (int i = 0; i < 3; ++i)
  m_add_i4_inp.ctrl[i].EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
}

void CRSDeskDlg::EnableSpiAdc(bool i_enable)
{
 if (i_enable == m_enable_spiadc)
  return; //already has needed state
 m_enable_spiadc = i_enable;
 //in the SECU-3i units with m1284 processor only
 for (int i = 0; i < 3; ++i)
 {
  m_add_i5_inp.ctrl[i].EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
  m_add_i6_inp.ctrl[i].EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
  m_add_i7_inp.ctrl[i].EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
  m_add_i8_inp.ctrl[i].EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
 }
}

void CRSDeskDlg::Resize(const CRect& i_rect)
{
 //Calculate scale factors basing on the previous size of window
 float Xf, Yf;
 GDIHelpers::CalcRectToRectRatio(i_rect, m_origRect, Xf, Yf);

 //Resize controls
 m_map_inp.Scale(Xf, Yf);
 m_ubat_inp.Scale(Xf, Yf);
 m_temp_inp.Scale(Xf, Yf);
 m_knock_inp.Scale(Xf, Yf);
 m_tps_inp.Scale(Xf, Yf);
 m_add_i1_inp.Scale(Xf, Yf);
 m_add_i2_inp.Scale(Xf, Yf);
 m_add_i3_inp.Scale(Xf, Yf);
 m_add_i4_inp.Scale(Xf, Yf);
 m_add_i5_inp.Scale(Xf, Yf);
 m_add_i6_inp.Scale(Xf, Yf);
 m_add_i7_inp.Scale(Xf, Yf);
 m_add_i8_inp.Scale(Xf, Yf);

 UpdateWindow();
}

void CRSDeskDlg::updateScrollerSize(void)
{
 mp_scr->SetViewSizeF(.0f, m_enable_secu3t_features ? 1.0f : 1.75f);
}

void CRSDeskDlg::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);
 if (m_was_initialized)
 {
  mp_scr->ResetScrollPos(false);
  updateScrollerSize();

  CRect rect;
  GetClientRect(&rect);
  if (!rect.IsRectNull())
   Resize(rect);
 }
}
