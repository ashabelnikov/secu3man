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
#include "ui-core/AnalogMeterCtrl.h"

static const int InpFldNum = 4;

CRSDeskDlg::Input::Input(UINT id0, UINT id1, UINT id2, UINT id3, float valueRange /*= 5.0f*/, int ticsNum /*= 5*/)
: value(.0f)
{
 ctrl[0] = new CStatic();
 ctrl[1] = new CStatic();
 ctrl[2] = new CStatic();

 CAnalogMeterCtrl *meter = new CAnalogMeterCtrl();
 meter->SetRange(.0, valueRange);
 meter->SetTickNumber(ticsNum);
 meter->SetMeterSize(120);
 meter->SetState(meter_3drect, false);
 meter->SetState(meter_labels, false);
 meter->SetState(meter_value, false);
 meter->SetNeedleWidth(1.5);
 meter->SetTickLength(1.35);
 ctrl[3] = meter;

 resid[0] = id0;
 resid[1] = id1;
 resid[2] = id2;
 resid[3] = id3;
}

CRSDeskDlg::Input::~Input()
{
 for (int i = 0; i < InpFldNum; ++i)
  delete ctrl[i];
}

void CRSDeskDlg::Input::StoreRects(void)
{
 for (int i = 0; i < InpFldNum; ++i)
  rect[i] = GDIHelpers::GetChildWndRect(ctrl[i]);

 CAnalogMeterCtrl *meter = static_cast<CAnalogMeterCtrl*>(ctrl[3]);
 meter->SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 meter->SetColor(meter_needle, RGB(60,100,255));
 meter->SetColor(meter_grid, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 meter->Update();
}

void CRSDeskDlg::Input::Scale(float Xf, float Yf, Input* p_other /*=NULL*/)
{
 for(int i = 0; i < InpFldNum; ++i)
 {
  CRect rc = p_other ? p_other->rect[i] : rect[i];
  GDIHelpers::ScaleRect(rc, Xf, Yf);
  ctrl[i]->MoveWindow(rc);
  ctrl[i]->Invalidate();
 }
}

void CRSDeskDlg::Input::UpdateMeter(void)
{
 CAnalogMeterCtrl *meter = static_cast<CAnalogMeterCtrl*>(ctrl[3]);
 meter->SetNeedleValue(value);    
 meter->Update();
}

void CRSDeskDlg::Input::UpdateColors(void)
{
 CAnalogMeterCtrl *meter = static_cast<CAnalogMeterCtrl*>(ctrl[3]);
 meter->SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 meter->SetColor(meter_grid, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 meter->Redraw();
}

void CRSDeskDlg::Input::DDX_Control(CDataExchange *pDX)
{
 for(int i = 0; i < InpFldNum; ++i)
  ::DDX_Control(pDX, resid[i], *ctrl[i]);
}

void CRSDeskDlg::Input::DDX_Text_Fmt(CDataExchange *pDX, LPCTSTR templ)
{ 
 ::DDX_Text_Fmt(pDX, resid[0], value, templ);
}

void CRSDeskDlg::Input::EnableWindow(bool enable)
{
 for(int i = 0; i < InpFldNum; ++i)
  ctrl[i]->EnableWindow(enable);

 CAnalogMeterCtrl *meter = static_cast<CAnalogMeterCtrl*>(ctrl[3]);
 meter->SetState(meter_needle, enable);
 meter->SetState(meter_grid, enable);
 COLORREF bk_color;
 meter->GetColor(meter_bground, &bk_color);
 meter->SetColor(meter_bground, enable ? bk_color : ::GetSysColor(COLOR_BTNFACE));
 meter->Redraw();
}

void CRSDeskDlg::Input::ShowWindow(int show)
{
 for(int i = 0; i < InpFldNum; ++i)
  ctrl[i]->ShowWindow(show);
}

BEGIN_MESSAGE_MAP(CRSDeskDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_WM_PAINT()
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
, m_map_inp(IDC_RS_MAP_VALUE, IDC_RS_MAP_CAPTION, IDC_RS_MAP_UNIT, IDC_RS_MAP_METER)
, m_ubat_inp(IDC_RS_UBAT_VALUE, IDC_RS_UBAT_CAPTION, IDC_RS_UBAT_UNIT, IDC_RS_UBAT_METER, 20.0f, 10)
, m_temp_inp(IDC_RS_TEMP_VALUE, IDC_RS_TEMP_CAPTION, IDC_RS_TEMP_UNIT, IDC_RS_TEMP_METER)
, m_knock_inp(IDC_RS_KNOCK_VALUE, IDC_RS_KNOCK_CAPTION, IDC_RS_KNOCK_UNIT, IDC_RS_KNOCK_METER)
, m_tps_inp(IDC_RS_TPS_VALUE, IDC_RS_TPS_CAPTION, IDC_RS_TPS_UNIT, IDC_RS_TPS_METER)
, m_add_i1_inp(IDC_RS_ADD_I1_VALUE, IDC_RS_ADD_I1_CAPTION, IDC_RS_ADD_I1_UNIT, IDC_RS_ADD_I1_METER)
, m_add_i2_inp(IDC_RS_ADD_I2_VALUE, IDC_RS_ADD_I2_CAPTION, IDC_RS_ADD_I2_UNIT, IDC_RS_ADD_I2_METER)
, m_add_i3_inp(IDC_RS_ADD_I3_VALUE, IDC_RS_ADD_I3_CAPTION, IDC_RS_ADD_I3_UNIT, IDC_RS_ADD_I3_METER)
, m_add_i4_inp(IDC_RS_ADD_I4_VALUE, IDC_RS_ADD_I4_CAPTION, IDC_RS_ADD_I4_UNIT, IDC_RS_ADD_I4_METER)
, m_add_i5_inp(IDC_RS_ADD_I5_VALUE, IDC_RS_ADD_I5_CAPTION, IDC_RS_ADD_I5_UNIT, IDC_RS_ADD_I5_METER)
, m_add_i6_inp(IDC_RS_ADD_I6_VALUE, IDC_RS_ADD_I6_CAPTION, IDC_RS_ADD_I6_UNIT, IDC_RS_ADD_I6_METER)
, m_add_i7_inp(IDC_RS_ADD_I7_VALUE, IDC_RS_ADD_I7_CAPTION, IDC_RS_ADD_I7_UNIT, IDC_RS_ADD_I7_METER)
, m_add_i8_inp(IDC_RS_ADD_I8_VALUE, IDC_RS_ADD_I8_CAPTION, IDC_RS_ADD_I8_UNIT, IDC_RS_ADD_I8_METER)
{
 m_fields.push_back(&m_map_inp);
 m_fields.push_back(&m_ubat_inp);
 m_fields.push_back(&m_temp_inp);
 m_fields.push_back(&m_knock_inp);
 m_fields.push_back(&m_tps_inp);
 m_fields.push_back(&m_add_i1_inp);
 m_fields.push_back(&m_add_i2_inp);
 m_fields.push_back(&m_add_i3_inp);
 m_fields.push_back(&m_add_i4_inp);
 m_fields.push_back(&m_add_i5_inp);
 m_fields.push_back(&m_add_i6_inp);
 m_fields.push_back(&m_add_i7_inp);
 m_fields.push_back(&m_add_i8_inp);
}

CRSDeskDlg::~CRSDeskDlg()
{
 //empty
}

void CRSDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 for(size_t i = 0; i < m_fields.size(); ++i)
  m_fields[i]->DDX_Control(pDX);

 for(size_t i = 0; i < m_fields.size(); ++i)
  m_fields[i]->DDX_Text_Fmt(pDX, _T("%.3f"));
}

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg message handlers

BOOL CRSDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();
 m_enabled = -1; //reset cache flag

 //remember color to have ability to detect changing of system colors (see OnPaint)
 m_COLOR_BTNFACE = GetSysColor(COLOR_BTNFACE);

 CloneWndFont(m_map_inp.ctrl[0], &m_fieldFont, 16, true);

 for(size_t i = 0; i < m_fields.size(); ++i)
  m_fields[i]->ctrl[0]->SetFont(&m_fieldFont);

 //initialize window scroller
 mp_scr->Init(this);

 Enable(false);
 UpdateData(FALSE);

 //Store original rects
 for(size_t i = 0; i < m_fields.size(); ++i)
  m_fields[i]->StoreRects();

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

 m_map_inp.EnableWindow(enable);
 m_ubat_inp.EnableWindow(enable);
 m_temp_inp.EnableWindow(enable);
 m_knock_inp.EnableWindow(enable);
 m_tps_inp.EnableWindow(enable);
 m_add_i1_inp.EnableWindow(enable);
 m_add_i2_inp.EnableWindow(enable);
 m_add_i3_inp.EnableWindow(enable && !m_enable_secu3t_features);
 m_add_i4_inp.EnableWindow(enable && m_enable_extraio);
 m_add_i5_inp.EnableWindow(enable && !m_enable_secu3t_features && m_enable_spiadc);
 m_add_i6_inp.EnableWindow(enable && !m_enable_secu3t_features && m_enable_spiadc);
 m_add_i7_inp.EnableWindow(enable && !m_enable_secu3t_features && m_enable_spiadc);
 m_add_i8_inp.EnableWindow(enable && !m_enable_secu3t_features && m_enable_spiadc);
}

void CRSDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 int sw3i = ((show && !m_enable_secu3t_features) ? SW_SHOW : SW_HIDE);

 m_map_inp.ShowWindow(sw);
 m_ubat_inp.ShowWindow(sw);
 m_temp_inp.ShowWindow(sw);
 m_knock_inp.ShowWindow(sw);
 m_tps_inp.ShowWindow(sw);
 m_add_i1_inp.ShowWindow(sw);
 m_add_i2_inp.ShowWindow(sw);
 m_add_i3_inp.ShowWindow(sw3i);
 m_add_i4_inp.ShowWindow(sw);
 m_add_i5_inp.ShowWindow(sw3i);
 m_add_i6_inp.ShowWindow(sw3i);
 m_add_i7_inp.ShowWindow(sw3i);
 m_add_i8_inp.ShowWindow(sw3i);
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

 for(size_t i = 0; i < m_fields.size(); ++i)
  m_fields[i]->UpdateMeter();
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

 //Move Add_i4
 CRect rect;
 GetClientRect(&rect);
 if (!rect.IsRectNull())
  _Resize(rect, true);

 //in the SECU-3i only
 m_add_i3_inp.EnableWindow(m_enabled && !i_enable);
 m_add_i5_inp.EnableWindow(m_enabled && !i_enable && m_enable_spiadc);
 m_add_i6_inp.EnableWindow(m_enabled && !i_enable && m_enable_spiadc);
 m_add_i7_inp.EnableWindow(m_enabled && !i_enable && m_enable_spiadc);
 m_add_i8_inp.EnableWindow(m_enabled && !i_enable && m_enable_spiadc);
 int sw3i = ((!i_enable) ? SW_SHOW : SW_HIDE);
 m_add_i3_inp.ShowWindow(sw3i);
 m_add_i5_inp.ShowWindow(sw3i);
 m_add_i6_inp.ShowWindow(sw3i);
 m_add_i7_inp.ShowWindow(sw3i);
 m_add_i8_inp.ShowWindow(sw3i);
}

void CRSDeskDlg::EnableExtraIO(bool i_enable)
{
 if (i_enable == m_enable_extraio)
  return; //already has needed state
 m_enable_extraio = i_enable;
 //in the SECU-3i only
 m_add_i4_inp.EnableWindow(m_enabled && i_enable);
}

void CRSDeskDlg::EnableSpiAdc(bool i_enable)
{
 if (i_enable == m_enable_spiadc)
  return; //already has needed state
 m_enable_spiadc = i_enable;
 //in the SECU-3i units with m1284 processor only
 m_add_i5_inp.EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
 m_add_i6_inp.EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
 m_add_i7_inp.EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
 m_add_i8_inp.EnableWindow(m_enabled && i_enable && !m_enable_secu3t_features);
}

void CRSDeskDlg::_Resize(const CRect& i_rect, bool add_i4_only /*= false*/)
{
 //Calculate scale factors basing on the previous size of window
 float Xf, Yf;
 GDIHelpers::CalcRectToRectRatio(i_rect, m_origRect, Xf, Yf);

 Input* p_inp = m_enable_secu3t_features ? &m_add_i3_inp : NULL;

 //Resize controls
 if (add_i4_only)
 {
  m_add_i4_inp.Scale(Xf, Yf, p_inp);
 }
 else
 {
  for(size_t i = 0; i < m_fields.size(); ++i)
  {
   if (m_fields[i]==&m_add_i4_inp)
    m_fields[i]->Scale(Xf, Yf, p_inp);
   else
    m_fields[i]->Scale(Xf, Yf);
  }
 }

 UpdateWindow();
}

void CRSDeskDlg::updateScrollerSize(void)
{
 mp_scr->SetViewSizeF(.0f, m_enable_secu3t_features ? 1.1f : 1.75f);
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
   _Resize(rect);
 }
}

void CRSDeskDlg::OnPaint() 
{
 CPaintDC dc(this); // device context for painting

 COLORREF newcolor = GetSysColor(COLOR_BTNFACE);
 if (newcolor != m_COLOR_BTNFACE)
 {
  m_COLOR_BTNFACE = newcolor;
  for(size_t i = 0; i < m_fields.size(); ++i)
   m_fields[i]->UpdateColors();
 }
}

