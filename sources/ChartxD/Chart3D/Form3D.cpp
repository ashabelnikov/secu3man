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

/** \file Form3D.cpp
 * \author Alexey A. Shabelnikov
 */

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include "resource.h"
#include "Form3D.h"
#include "../common/MathHelpers.h"
#include "../common/DPIAware.h"
#include "../common/StrUtils.h"
#include "../PtmovStep/PtMovStepDlg.h"
#include "../ManageFrm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "Form3D.dfm"

char TForm3D::m_csvsep_symb = ',';

//---------------------------------------------------------------------------
__fastcall TForm3D::TForm3D(HWND parent)
: TForm(parent)
, m_hInst(NULL)
, m_count_x(0)
, m_count_z(0)
, m_fnc_min(0)
, m_fnc_max(0)
, mp_modified_function(NULL)
, mp_original_function(NULL)
, m_u_title("")
, m_x_title("")
, m_y_title("")
, m_pOnChange(NULL)
, m_pOnChangeSettings(NULL)
, m_pOnClose(NULL)
, m_pOnWndActivation(NULL)
, m_pOnGetXAxisLabel(NULL)
, m_param_on_change(NULL)
, m_param_on_change_sett(NULL)
, m_param_on_close(NULL)
, m_param_on_wnd_activation(NULL)
, m_param_on_get_x_axis_label(NULL)
, m_pOnValueTransform(NULL)
, m_param_on_value_transform(NULL)
, m_setval(0)
, m_val_x(0)
, m_val_z(0)
, m_air_flow_position(0)
, m_values_format_x("%.00f")  //integer 
, m_chart_active(false)
, m_pt_moving_step(0.5f)
, m_visibleMarkIdx(-1)
, m_3d_transparency(10.0)
, m_mc_xpos(-1)
, m_mc_ypos(-1)
, m_mc_rotation(0)
, m_mc_elevation(0)
, m_mc_allow(true)
{
 //empty
}

//---------------------------------------------------------------------------
__fastcall TForm3D::~TForm3D()
{
 //empty
}

//---------------------------------------------------------------------------
void TForm3D::DataPrepare(bool create)
{
 if (create)
 { //create
  //At the moment of creation we are in the 2D mode by default
  UpdateSystemColors();
  FillChart();
  SetAirFlow(m_air_flow_position); //set trackbar position  //cr

  if (m_chart_active)
  {
   MarkPoints(true);
   Chart1->Title->Font->Style = TFontStyles() << fsBold;
  }
  else
  {
   UnmarkPoints();
   Chart1->Title->Font->Style = TFontStyles();
  }
 }
 else
 { //update
  //When updating (after creation), we may be either in the 2D or 3D mode
  if (CheckBox3d->Checked)
   UpdateSurfaceValues();
  else
   UpdateChartValues();
 }
}

//---------------------------------------------------------------------------
void TForm3D::SetOnChange(EventHandler i_pOnChange,void* i_param)
{
 m_pOnChange = i_pOnChange;
 m_param_on_change = i_param;
}

//---------------------------------------------------------------------------
void TForm3D::SetOnChangeSettings(EventHandler i_pOnChange, void* i_param)
{
 m_pOnChangeSettings = i_pOnChange;
 m_param_on_change_sett = i_param;
}

//---------------------------------------------------------------------------
void TForm3D::SetOnClose(EventHandler i_pOnClose,void* i_param)
{
 m_pOnClose = i_pOnClose;
 m_param_on_close = i_param;
}

//---------------------------------------------------------------------------
void TForm3D::SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param)
{
 m_pOnWndActivation = i_pOnWndActivation;
 m_param_on_wnd_activation = i_param;
}

//---------------------------------------------------------------------------
void TForm3D::SetOnGetXAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param)
{
 m_pOnGetXAxisLabel = i_pOnGetAxisLabel;
 m_param_on_get_x_axis_label = i_param;
}

//---------------------------------------------------------------------------
void TForm3D::SetOnValueTransform(OnValueTransform i_pOnValueTransform, void* i_param)
{
 m_pOnValueTransform = i_pOnValueTransform;
 m_param_on_value_transform = i_param;
}

//---------------------------------------------------------------------------
void TForm3D::Enable(bool enable)
{
 if (enable)
 {
  if (CheckBox3d->Checked)
   MakeAllVisible(); //3D
  else
   MakeOneVisible(m_air_flow_position); //2D
 }
 else
 { //disable all series
  HideAllSeries();
  Series3D->Active = false;
  Chart1->Title->Font->Style = TFontStyles(); //set chart's title font to normal (not bold)
 }

 Chart1->Enabled = enable;
 LabelAfv->Enabled = enable;
 LabelAfc->Enabled = enable;
 TrackBarAf->Enabled = enable;
 CheckBox3d->Enabled = enable;
 CheckBoxBv->Enabled = enable && CheckBox3d->Checked;
 ButtonAngleUp->Enabled = enable;
 ButtonAngleDown->Enabled = enable;
 Smoothing3x->Enabled = enable;
 Smoothing5x->Enabled = enable;
 if (CheckBox3d->Checked)
  Chart1->Visible = enable;
 TrackLeft->Enabled = enable;
 TrackRight->Enabled = enable;
}

//---------------------------------------------------------------------------
void TForm3D::InitPopupMenu(HINSTANCE hInstance)
{
 char string[1024 + 1];
 ::LoadString(hInstance, IDS_PM_ZERO_ALL_POINTS, string, 1024);
 PM_ZeroAllPoints->Caption = string;
 ::LoadString(hInstance, IDS_PM_SET_ALLTO_1ST_PT, string, 1024);
 PM_Dup1stPoint->Caption = string;
 ::LoadString(hInstance, IDS_PM_BLD_CURVE_1ST_LAST_PT, string, 1024);
 PM_BldCurveUsing1stAndLastPoints->Caption = string;
 ::LoadString(hInstance, IDS_PM_ZERO_ALL_CURVES, string, 1024);
 PM_ZeroAllCurves->Caption = string;
 ::LoadString(hInstance, IDS_PM_SET_ALLTO_THIS_CR, string, 1024);
 PM_DupThisCurve->Caption = string;
 ::LoadString(hInstance, IDS_PM_BLD_SHAPE_1ST_LAST_CR, string, 1024);
 PM_BldShapeUsing1stAndLastCurves->Caption = string;
 ::LoadString(hInstance, IDS_PM_COPY_FROM_CURVE, string, 1024);
 PM_CopyFromCurve->Caption = string;
 ::LoadString(hInstance, IDS_PM_COPY_TO_CURVE, string, 1024);
 PM_CopyToCurve->Caption = string;
 ::LoadString(hInstance, IDS_PM_SET_PTMOV_STEP, string, 1024);
 PM_SetPtMovStep->Caption = string;
 ::LoadString(hInstance, IDS_PM_HIDE_MARKS, string, 1024);
 PM_HideMarks->Caption = string;
 ::LoadString(hInstance, IDS_PM_HIDE_OLDCURVE, string, 1024);
 PM_HideOldCurve->Caption = string;
 ::LoadString(hInstance, IDS_PM_EXPORT, string, 1024);
 PM_Export->Caption = string;
 ::LoadString(hInstance, IDS_PM_EXPORT_CSV, string, 1024);
 PM_ExportCSV->Caption = string;
 ::LoadString(hInstance, IDS_PM_IMPORT, string, 1024);
 PM_Import->Caption = string;
 ::LoadString(hInstance, IDS_PM_IMPORT_CSV, string, 1024);
 PM_ImportCSV->Caption = string;
 ::LoadString(hInstance, IDS_PM_INC, string, 1024);
 PM_Inc->Caption = string;
 ::LoadString(hInstance, IDS_PM_DEC, string, 1024);
 PM_Dec->Caption = string;
 ::LoadString(hInstance, IDS_PM_SETTO, string, 1024);
 PM_SetTo->Caption = string;
 ::LoadString(hInstance, IDS_PM_SUB, string, 1024);
 PM_Sub->Caption = string;
 ::LoadString(hInstance, IDS_PM_ADD, string, 1024);
 PM_Add->Caption = string;
 ::LoadString(hInstance, IDS_PM_MUL, string, 1024);
 PM_Mul->Caption = string;
 ::LoadString(hInstance, IDS_PM_COPY, string, 1024);
 PM_Copy->Caption = string;
 ::LoadString(hInstance, IDS_PM_PASTE, string, 1024);
 PM_Paste->Caption = string;
 ::LoadString(hInstance, IDS_PM_ALLOW_MOUSE_ZOOM, string, 1024);
 PM_AllowMouseZoom->Caption = string;
 ::LoadString(hInstance, IDS_PM_ALLOW_MOUSE_CAMERA, string, 1024);
 PM_AllowMouseCamera->Caption = string;
 ::LoadString(hInstance, IDS_PM_INTERPOLATE, string, 1024);
 PM_Interpolate->Caption = string;
} 

//---------------------------------------------------------------------------
void TForm3D::InitHints(HINSTANCE hInstance)
{
 m_hInst = hInstance; //save HINSTANCE for further use
 char string[1024 + 1];
 ::LoadString(hInstance, IDS_TT_SMOOTHING_3_POINTS, string, 1024);
 Smoothing3x->Hint = string;
 ::LoadString(hInstance, IDS_TT_SMOOTHING_5_POINTS, string, 1024);
 Smoothing5x->Hint = string;
 ::LoadString(hInstance, IDS_TT_MOVE_FUNC_UP, string, 1024);
 ButtonAngleUp->Hint = string;
 ::LoadString(hInstance, IDS_TT_MOVE_FUNC_DOWN, string, 1024);
 ButtonAngleDown->Hint = string;
 ::LoadString(hInstance, IDS_TT_FUNC_VIEW_3D, string, 1024);
 CheckBox3d->Hint = string;
 ::LoadString(hInstance, IDS_TT_FUNC_VIEW_BACK, string, 1024);
 CheckBoxBv->Hint = string;
 ::LoadString(hInstance, IDS_TT_SELECT_CURVE, string, 1024);
 TrackBarAf->Hint = string;
}

//---------------------------------------------------------------------------
void TForm3D::SetPtValuesFormat(LPCTSTR ptValFormat)
{
 for (int i = 0; i < m_count_z * 2; i++ )
  Chart1->Series[i]->ValueFormat = ptValFormat; //format for point values
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::TrackBarAfChange(TObject *Sender)
{
 if (CheckBox3d->Checked)
  return; //in 3D mode user doesn't move track bar, it is disabled
 //2D only:
 SetAirFlow(TrackBarAf->Position);
 m_setval = 0;
 UnmarkPoints();
 MarkPoints(true);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1ClickSeries(TCustomChart *Sender,
      TChartSeries *Series, int ValueIndex, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 if (CheckBox3d->Checked)
  return; //not used in 3D mode

 if (Series==Chart1->Series[m_air_flow_position + m_count_z])
 {
  //if Ctrl key is not pressed then, select single point
  if (!Shift.Contains(ssCtrl))
  {
   MarkPoints(false);
   m_sel.Clear();
   m_sel.Set(m_air_flow_position, ValueIndex, true);
   MarkPoints(true);
  }

  if (Button==mbLeft)
  {
   m_setval = 1;
   m_val_x  = ValueIndex;
   m_val_z  = CheckBoxBv->Checked ? m_count_z-1-m_air_flow_position : m_air_flow_position;
  }
 }
 m_prev_pt = std::make_pair(X, Y);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 if (CheckBox3d->Checked)
 {
  m_mc_xpos = m_mc_ypos = -1;
  return; //not used in 3D mode
 }

 //implementation of multiple points selection
 if (Shift.Contains(ssCtrl) && (m_prev_pt.first == X && m_prev_pt.second == Y))
 {
  MarkPoints(false);
  if (false==m_sel.Get(m_air_flow_position, m_val_x))
   m_sel.Set(m_air_flow_position, m_val_x, true);
  else
   m_sel.Set(m_air_flow_position, m_val_x, false);
  MarkPoints(true);
 }

 if ((m_pOnChange) && (m_setval))
  m_pOnChange(m_param_on_change);
 m_setval = 0;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnChartMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if (CheckBox3d->Checked && m_mc_allow)
 { //remember position of mouse cursor and current position of camera
  m_mc_xpos = X;
  m_mc_ypos = Y;
  m_mc_rotation = Chart1->View3DOptions->Rotation; 
  m_mc_elevation = Chart1->View3DOptions->Elevation; 
 }

 if (ActiveControl != Chart1)
 {
  ActiveControl = Chart1;
  m_chart_active = true;
 }

 OnEnterChart(NULL);

 if (!Active)
  SetFocus();
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 if (CheckBox3d->Checked && m_mc_xpos != -1 && m_mc_ypos != -1)
 { //change position of camera using current and stored coordinates of the mouse cursor
  int r = (X - m_mc_xpos) / 4;
  int e = (m_mc_ypos - Y) / 4;
  Chart1->View3DOptions->Rotation = m_mc_rotation + r;
  Chart1->View3DOptions->Elevation = m_mc_elevation + e;
 }

 //===========================================
 if (m_visibleMarkIdx != -1)
 {
  if (m_setval)
  {
   m_visibleMarkIdx = m_val_x;
   Chart1->Invalidate();
  }
  else
  {
   int idx = Chart1->Series[m_air_flow_position + m_count_z]->GetCursorValueIndex();
   if (idx >= 0)
   {
    m_visibleMarkIdx = idx;
    Chart1->Invalidate();
   }
  } 
 }
 //===========================================

 double v;
 if (m_setval)
 {//select on the fly point being moved
  if (false==m_sel.Get(m_air_flow_position, m_val_x))
  {
   m_sel.Set(m_air_flow_position, m_val_x, true);
   MarkPoints(true);
  }
  //Move all selected points
  v = Chart1->Series[m_air_flow_position + m_count_z]->YScreenToValue(Y);
  ShiftPoints(v - GetItem_m(m_air_flow_position, m_val_x));
 } 
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnChart1DblClick(TObject *Sender)
{
 if (!CheckBox3d->Checked)
  return; //not used in 2D mode

 int i = Series3D->GetCursorValueIndex();
 if (i > 0)
 {
  --i; //delphi...
  m_val_z = i / m_count_x;
  m_val_x = i % m_count_x;

  m_sel.Clear();
  m_sel.Set(m_val_z, m_val_x, true);

  Chart1->Invalidate();
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1GetAxisLabel(TChartAxis *Sender,
      TChartSeries *Series, int ValueIndex, AnsiString &LabelText)
{
 if (Sender == Chart1->BottomAxis)
 { //X
  if (m_pOnGetXAxisLabel)
  {
   if (ValueIndex >= 0 && ValueIndex < m_count_x)
   {
    TCHAR string[64];
    _tcscpy(string, LabelText.c_str());
    m_pOnGetXAxisLabel(string, ValueIndex, m_param_on_get_x_axis_label);
    LabelText = string;
   }
  }
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::CheckBox3dClick(TObject *Sender)
{
 if (CheckBoxBv->Checked)
  m_sel.InvertZ();
 if (CheckBox3d->Checked)
 { //3D
  Series3D->CreateValues(m_count_x, m_count_z);
  Chart1->View3D  = true;
  MakeAllVisible();
  TrackBarAf->Enabled = false;
  TrackLeft->Enabled = false;
  TrackRight->Enabled = false;
  LabelAfv->Enabled = false;
  LabelAfc->Enabled = false;
  CheckBoxBv->Enabled = true;
  PM_Export->Enabled = true;
  PM_Import->Enabled = true;
  PM_HideMarks->Enabled = false;
  PM_HideOldCurve->Enabled = false;
  PM_AllowMouseCamera->Enabled = true;
 }
 else
 { //2D
  Series3D->Clear(); //delete values
  UpdateChartValues();
  Chart1->View3D = false;
  MakeOneVisible(m_air_flow_position);
  TrackBarAf->Enabled = true;
  TrackLeft->Enabled = true;
  TrackRight->Enabled = true;
  LabelAfv->Enabled = true;
  LabelAfc->Enabled = true;
  CheckBoxBv->Enabled = false;
  PM_HideMarks->Enabled = true;
  PM_HideOldCurve->Enabled = true;
  PM_AllowMouseCamera->Enabled = false;
 }
}

//---------------------------------------------------------------------------
//Back view in 3D mode
void __fastcall TForm3D::CheckBoxBvClick(TObject *Sender)
{
 m_val_z = m_count_z - 1 - m_val_z;
 m_sel.InvertZ();
 UpdateSurfaceValues();
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnCloseForm(TObject *Sender, TCloseAction &Action)
{
 if (m_pOnClose)
  m_pOnClose(m_param_on_close);
 RemoveInstanceByHWND(Handle);
 Action = caFree; //close form and free resources
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ButtonAngleUpClick(TObject *Sender)
{
 ShiftPoints(Chart1->LeftAxis->Inverted ? -m_pt_moving_step : m_pt_moving_step, true); //all
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ButtonAngleDownClick(TObject *Sender)
{
 ShiftPoints(Chart1->LeftAxis->Inverted ? m_pt_moving_step : -m_pt_moving_step, true); //all
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Smoothing3xClick(TObject *Sender)
{
 if (!CheckBox3d->Checked)
 { //2D
  float* p_source_function = new float[m_count_x];
  for (int x = 0; x < m_count_x; ++x)
   p_source_function[x] = *GetItem_mp(m_air_flow_position, x);
  MathHelpers::Smooth1D(p_source_function, GetItem_mp(m_air_flow_position, 0), m_count_x, 3, m_sel.Get(m_air_flow_position));
  delete[] p_source_function;
  for (int x = 0; x < m_count_x; x++ )
   RestrictAndSetChartValue(x, GetItem_m(m_air_flow_position, x));
 }
 else
 {//3D
  float* p_source_function = new float[m_count_z * m_count_x];
  std::copy(mp_modified_function, mp_modified_function + (m_count_z * m_count_x), p_source_function);
  if (CheckBoxBv->Checked) m_sel.InvertZ();
  MathHelpers::Smooth2D(p_source_function, mp_modified_function, m_count_z, m_count_x, 3, m_sel.Get());
  if (CheckBoxBv->Checked) m_sel.InvertZ();
  delete[] p_source_function;
  for (int z = 0; z < m_count_z; z++ )
   for (int x = 0; x < m_count_x; x++ )
    RestrictAndSetValue(z, x, GetItem_m(z, x));
  UpdateSurfaceValues();
 }

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Smoothing5xClick(TObject *Sender)
{
 if (!CheckBox3d->Checked)
 { //2D
  float* p_source_function = new float[m_count_x];
  for (int x = 0; x < m_count_x; ++x)
   p_source_function[x] = *GetItem_mp(m_air_flow_position, x);
  MathHelpers::Smooth1D(p_source_function, GetItem_mp(m_air_flow_position, 0), m_count_x, 5, m_sel.Get(m_air_flow_position));
  delete[] p_source_function;
  for (int x = 0; x < m_count_x; x++ )
   RestrictAndSetChartValue(x, GetItem_m(m_air_flow_position, x));
 }
 else
 { //3D
  float* p_source_function = new float[m_count_z * m_count_x];
  std::copy(mp_modified_function, mp_modified_function + (m_count_z * m_count_x), p_source_function);
  if (CheckBoxBv->Checked) m_sel.InvertZ();
  MathHelpers::Smooth2D(p_source_function, mp_modified_function, m_count_z, m_count_x, 5, m_sel.Get());
  if (CheckBoxBv->Checked) m_sel.InvertZ();
  delete[] p_source_function;
  for (int z = 0; z < m_count_z; z++ )
   for (int x = 0; x < m_count_x; x++ )
    RestrictAndSetValue(z, x, GetItem_m(z, x)); 
  UpdateSurfaceValues();
 }

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
//get item from original function
float TForm3D::GetItem_o(int z, int x) const
{
 if ((z >= m_count_z) || (x >= m_count_x)) return 0.0f;
 if (m_pOnValueTransform)
  return m_pOnValueTransform(m_param_on_value_transform, *(mp_original_function + ((z * m_count_x) + x)), 0); //get
 else
  return *(mp_original_function + ((z * m_count_x) + x));
}

//---------------------------------------------------------------------------
//get item from modified function
float TForm3D::GetItem_m(int z, int x) const
{
 if ((z >= m_count_z) || (x >= m_count_x)) return 0.0f;
 if (m_pOnValueTransform)
  return m_pOnValueTransform(m_param_on_value_transform, *(mp_modified_function + ((z * m_count_x) + x)), 0); //get
 else
  return *(mp_modified_function + ((z * m_count_x) + x));
}

//---------------------------------------------------------------------------
float* TForm3D::GetItem_mp(int z, int x)
{
 if ((z >= m_count_z) || (x >= m_count_x)) return NULL;
 return (mp_modified_function + ((z * m_count_x) + x));
}

//---------------------------------------------------------------------------
//set item in modified function
int TForm3D::SetItem(int z, int x, float value)
{
 if ((z >= m_count_z) || (x >= m_count_x)) return 0;
 if (m_pOnValueTransform)
  *(mp_modified_function + ((z * m_count_x) + x)) = m_pOnValueTransform(m_param_on_value_transform, value, 1); //set
 else
  *(mp_modified_function + ((z * m_count_x) + x)) = value;
 return 1;
}

//---------------------------------------------------------------------------
void TForm3D::SetAirFlow(int flow)
{
 AnsiString as;
 TrackBarAf->Position = flow;
 as.sprintf("%d", flow + 1);
 LabelAfv->Caption = as;
 m_air_flow_position = flow;
 if (!CheckBox3d->Checked)
  MakeOneVisible(flow);
}

//---------------------------------------------------------------------------
//2D view
void TForm3D::MakeOneVisible(int flow)
{
 Series3D->Active = false;
 for(int z = 0; z < m_count_z; z++)
 {
  Chart1->Series[z]->Active = (z==flow) && !PM_HideOldCurve->Checked;
  Chart1->Series[z + m_count_z]->Active = (z==flow);
 }
}

//---------------------------------------------------------------------------
//3D view
void TForm3D::MakeAllVisible(void)
{
 for(int z = 0; z < m_count_z; z++)
 {
  Chart1->Series[z]->Active = false;
  Chart1->Series[z + m_count_z]->Active = false;
 }
 Series3D->Active = true;
}

//---------------------------------------------------------------------------
void TForm3D::FillChart(void)
{
 AnsiString as; 
 for(int z = 0; z < m_count_z; z++)
 {
  for(int x = 0; x < m_count_x; x++)
  { //Add series
   as.sprintf(m_values_format_x.c_str(), m_u_slots[x]);
   Chart1->Series[z]->Add(GetItem_o(z, x), as, clAqua); //original
   Chart1->Series[z + m_count_z]->Add(GetItem_m(z, x), as, clRed); //modified
  }
 }
}

//---------------------------------------------------------------------------
//2D only
void TForm3D::HideAllSeries(void)
{
 for(int z = 0; z < m_count_z * 2; z++)
  Chart1->Series[z]->Active = false;
}

//---------------------------------------------------------------------------
int __fastcall TForm3D::GetZPos(int z) const
{
 return CheckBoxBv->Checked ? m_count_z - 1 - z : z;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ShiftPoints(float i_value, bool all /*= false*/)
{
 if (!CheckBox3d->Checked)
 { //2D
  for(int x = 0; x < m_count_x; ++x)
  {
   if (all || true==m_sel.Get(m_air_flow_position, x))
    RestrictAndSetChartValue(x, GetItem_m(m_air_flow_position, x) + i_value);
  }
 }
 else
 { //3D
  for(int z = 0; z < m_count_x; ++z)
  {
   for(int x = 0; x < m_count_x; ++x)
   {
    if (all || true==m_sel.Get(z, x))
     RestrictAndSetValue(GetZPos(z), x, GetItem_m(GetZPos(z), x) + i_value);
   }
  }
  UpdateSurfaceValues();
 }
}

//---------------------------------------------------------------------------
//Used in 2D only
void __fastcall TForm3D::MarkPoints(bool i_mark)
{
 for(int x = 0; x < m_count_x; ++x)
 {
  if (true==m_sel.Get(m_air_flow_position, x))
   (Chart1->Series[m_air_flow_position + m_count_z])->ValueColor[x] = (i_mark ? clNavy : clRed);
 }
}

//---------------------------------------------------------------------------
//Used in 2D only
void __fastcall TForm3D::UnmarkPoints(void)
{
 for(int x = 0; x < m_count_x; ++x)
 {
  (Chart1->Series[m_air_flow_position + m_count_z])->ValueColor[x] = clRed;
 }
}

//---------------------------------------------------------------------------
//2D version
// v - value in chart's format
void TForm3D::RestrictAndSetChartValue(int index, double v)
{
 v = MathHelpers::RestrictValue<double>(v, m_fnc_min, m_fnc_max);
 SetItem(m_air_flow_position, index, v);
 SetChartValue(m_air_flow_position, index, v);
}

//---------------------------------------------------------------------------
//3D version
// v - value in chart's format
void TForm3D::RestrictAndSetValue(int index_z, int index_x, double v)
{
 v = MathHelpers::RestrictValue<double>(v, m_fnc_min, m_fnc_max);
 SetItem(index_z, index_x, v);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::CopyCurve(int fromIndex, int toIndex)
{
 if (CheckBox3d->Checked)
 {//3D
  for(int x = 0; x < m_count_x; ++x)
   SetItem(toIndex, x, GetItem_m(fromIndex, x));
  UpdateSurfaceValues();
 }
 else
 {//2D
  for(int x = 0; x < m_count_x; ++x)
  { 
   SetChartValue(toIndex, x, GetItem_m(fromIndex, x));
   SetItem(toIndex, x, GetItem_m(fromIndex, x));
  }
 }
}

//---------------------------------------------------------------------------
void TForm3D::SetChartValue(int z, int index, double value)
{
 Chart1->Series[z + m_count_z]->YValue[index] = value;
}

//---------------------------------------------------------------------------
//2D only
void TForm3D::UpdateChartValues(void)
{
 for(int z = 0; z < m_count_x; ++z)
  for(int x = 0; x < m_count_x; ++x)
   SetChartValue(z, x, GetItem_m(z, x));
}

//---------------------------------------------------------------------------
//3D only
void TForm3D::UpdateSurfaceValues(void)
{
 Series3D->ReCreateValues();
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::WndProc(Messages::TMessage &Message)
{
 TForm::WndProc(Message);

 if (Message.Msg == WM_SYSCOMMAND && m_pOnWndActivation)
  m_pOnWndActivation(m_param_on_wnd_activation, Message.WParam);

 if (Message.Msg == WM_SYSCOLORCHANGE)
 {
  UpdateSystemColors();
  TrackBarAf->SetFocus(); //smal hack, for updating of colors of a track bar
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnZeroAllPoints(TObject *Sender)
{
 if (CheckBox3d->Checked)
 {//3D
  for (int x = 0; x < m_count_x; x++ )
   RestrictAndSetValue(GetZPos(m_val_z), x, 0);
  UpdateSurfaceValues();
 }
 else
 {//2D
  for (int x = 0; x < m_count_x; x++ )
   RestrictAndSetChartValue(x, 0);
 }

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnDuplicate1stPoint(TObject *Sender)
{
 if (CheckBox3d->Checked)
 {//3D
  for (int x = 0; x < m_count_x; x++)
   RestrictAndSetValue(GetZPos(m_val_z), x, GetItem_m(GetZPos(m_val_z), 0));
  UpdateSurfaceValues();
 }
 else
 {//2D
  for (int x = 0; x < m_count_x; x++)
   RestrictAndSetChartValue(x, GetItem_m(m_air_flow_position, 0));
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnBldCurveUsing1stAndLastPoints(TObject *Sender)
{
 if (CheckBox3d->Checked)
 {//3D
  std::vector<int> selpts;
  selpts.push_back(0);
  selpts.push_back(m_count_x-1);
  Interpolate2D(GetZPos(m_val_z), selpts);
  UpdateSurfaceValues();
 }
 else
 {//2D
  std::vector<int> selpts;
  selpts.push_back(0);
  selpts.push_back(m_count_x-1);
  Interpolate2D(m_air_flow_position, selpts);
  UpdateChartValues();  
 }

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnZeroAllCurves(TObject *Sender)
{
 if (CheckBox3d->Checked)
 {//3D
  for (int z = 0; z < m_count_z; z++ )
   for (int x = 0; x < m_count_x; x++ )
    SetItem(z, x, 0);
  UpdateSurfaceValues();
 }
 else
 {//2D
  for (int z = 0; z < m_count_z; z++ )
  {
   for (int x = 0; x < m_count_x; x++ )
   {
    SetChartValue(z, x, 0);
    SetItem(z, x, 0);
   }
  }
 }

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnDuplicateThisCurve(TObject *Sender)
{
 if (CheckBox3d->Checked)
 {//3D
  for (int z = 0; z < m_count_z; z++ )
   for (int x = 0; x < m_count_x; x++ )
    SetItem(z, x, GetItem_m(GetZPos(m_val_z), x));
  UpdateSurfaceValues();
 }
 else
 {//2D
  for (int z = 0; z < m_count_z; z++ )
  {
   for (int x = 0; x < m_count_x; x++ )
   {
    SetChartValue(z, x, GetItem_m(m_air_flow_position, x));
    SetItem(z, x, GetItem_m(m_air_flow_position, x));
   }
  }
 }

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnBuildShapeUsing1stAndLastCurves(TObject *Sender)
{
 if (CheckBox3d->Checked)
 {//3D
  Interpolate3D(0, m_count_x-1, 0, m_count_z-1);
  UpdateSurfaceValues();
 }
 else
 {//2D
  Interpolate3D(0, m_count_x-1, 0, m_count_z-1);
  UpdateChartValues();
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::SelLeftArrow(bool i_shift)
{
 if (i_shift)
 {//Shift key is pressed
  if (m_sel.Size(CheckBox3d->Checked ? m_val_z : m_air_flow_position))
  {
   if (!CheckBox3d->Checked) MarkPoints(false);
   if (m_sel.Left() != m_val_x && m_sel.Left() != m_sel.Right())
   {
    for(int z = m_sel.Down(); z <= m_sel.Up(); ++z)
     m_sel.PopRight(z);
    m_val_x = m_sel.Right();
   }
   else
   {
    m_val_x = m_sel.Left() - 1;
    if (m_val_x >= 0)
    {
     for(int z = m_sel.Down(); z <= m_sel.Up(); ++z)
      m_sel.Set(z, m_val_x, true);
    }
    else
     m_val_x = 0;
   }
   if (!CheckBox3d->Checked) MarkPoints(true);
   else Chart1->Invalidate();
  }
 }
 else
 {//Without shift
  if (!CheckBox3d->Checked) MarkPoints(false);
  int prev_pt = m_sel.Size(CheckBox3d->Checked ? m_val_z : m_air_flow_position) ? m_sel.Right() - 1 : 0;
  if (prev_pt < 0)
   prev_pt = 0;
  m_sel.Clear();
  m_sel.Set(CheckBox3d->Checked ? m_val_z : m_air_flow_position, prev_pt, true);
  if (!CheckBox3d->Checked) MarkPoints(true);
  else Chart1->Invalidate();
  m_val_x = prev_pt;
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::SelRightArrow(bool i_shift)
{
 if (i_shift)
 { //Shift key is pressed
  if (m_sel.Size(CheckBox3d->Checked ? m_val_z : m_air_flow_position))
  {
   if (!CheckBox3d->Checked) MarkPoints(false);
   if (m_sel.Right() != m_val_x &&  m_sel.Left() != m_sel.Right())
   {
    for(int z = m_sel.Down(); z <= m_sel.Up(); ++z)
     m_sel.PopLeft(z);
    m_val_x = m_sel.Left();
   }
   else
   {
    m_val_x = m_sel.Right() + 1;
    if (m_val_x < m_count_x)
    {
     for(int z = m_sel.Down(); z <= m_sel.Up(); ++z)
      m_sel.Set(z, m_val_x, true);
    }
    else
    {
     m_val_x = m_count_x - 1;
    }
   }
   if (!CheckBox3d->Checked) MarkPoints(true);
   else Chart1->Invalidate();
  }
 }
 else
 { //Without shift
  if (!CheckBox3d->Checked) MarkPoints(false);
  int next_pt = m_sel.Size(CheckBox3d->Checked ? m_val_z : m_air_flow_position) ? m_sel.Right() + 1 : 0;
  if (next_pt >= m_count_x)
   next_pt = m_count_x - 1;
  m_sel.Clear();
  m_sel.Set(CheckBox3d->Checked ? m_val_z : m_air_flow_position, next_pt, true);
  if (!CheckBox3d->Checked) MarkPoints(true);
  else Chart1->Invalidate();
  m_val_x = next_pt;
 }
}

void __fastcall TForm3D::SelUpArrow(bool i_shift)
{
 if (!CheckBox3d->Checked)
 { //2D
  if (m_air_flow_position < (m_count_z-1))
   ++m_air_flow_position;
  SetAirFlow(m_air_flow_position);
  return;
 }

 if (i_shift)
 { //Shift key is pressed
  if (m_sel.Up() != m_val_z &&  m_sel.Down() != m_sel.Up())
  {
   m_sel.Clear(m_sel.Down());
   m_val_z = m_sel.Down();
  }
  else
  {
   m_val_z = m_sel.Up() + 1;
   if (m_val_z < m_count_z)
    m_sel.CopyRow(m_val_z - 1, m_val_z);
   else
    m_val_z = m_count_z-1;
  }
  Chart1->Invalidate();
 }
 else
 { //Without shift  
  if (m_val_z < (m_count_z-1))
  {
   m_sel.Clear();
   ++m_val_z;
  }
  m_sel.Set(m_val_z, m_val_x, true);
  Chart1->Invalidate();
 }
}

void __fastcall TForm3D::SelDownArrow(bool i_shift)
{
 if (!CheckBox3d->Checked)
 { //2D
  if (m_air_flow_position > 0)
   --m_air_flow_position;
  SetAirFlow(m_air_flow_position);
  return;
 }

 if (i_shift)
 { //Shift key is pressed
  if (m_sel.Down() != m_val_z && m_sel.Down() != m_sel.Up())
  {
   m_sel.Clear(m_sel.Up());
   m_val_z = m_sel.Up();
  }
  else
  {
   m_val_z = m_sel.Down() - 1;
   if (m_val_z >= 0)
    m_sel.CopyRow(m_val_z + 1, m_val_z);  
   else
    m_val_z = 0;
  }
  Chart1->Invalidate();
 }
 else
 { //Without shift  
  if (m_val_z > 0)
  {
   m_sel.Clear();
   --m_val_z;
  }
  m_sel.Set(m_val_z, m_val_x, true);
  Chart1->Invalidate();
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::CtrlKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
 //Implement keyboard actions related to chart
 if (ActiveControl == Chart1)
 {
  if (Key == VK_OEM_6 || Key == VK_OEM_PERIOD)
  { //move points upward
   ShiftPoints(Chart1->LeftAxis->Inverted ? -m_pt_moving_step : m_pt_moving_step);
   if (m_pOnChange)
    m_pOnChange(m_param_on_change);
  }
  else if (Key == VK_OEM_5 || Key == VK_OEM_COMMA)
  { //move points downward
   ShiftPoints(Chart1->LeftAxis->Inverted ? m_pt_moving_step : -m_pt_moving_step);
   if (m_pOnChange)
    m_pOnChange(m_param_on_change);
  }
  else if (Key == VK_LEFT)
  { //move selection to the left
   SelLeftArrow(Shift.Contains(ssShift));
  }
  else if (Key == VK_RIGHT)
  { //move selection to the right
   SelRightArrow(Shift.Contains(ssShift));
  }
  else if (Key == VK_RETURN)
  {
   if (CheckBox3d->Checked)
   {
    if (m_val_z > 0)
    { 
     --m_val_z;
    }
    else
    {     
     m_val_z = m_count_z-1;
     if (m_val_x < m_count_x-1)
      ++m_val_x;     
     else
      m_val_x = 0;     
    }
    m_sel.Clear();
    m_sel.Set(m_val_z, m_val_x, true);
    Chart1->Invalidate();
   }
  }
  else if (Key == VK_DOWN)
  { //decrement curve index
   SelDownArrow(Shift.Contains(ssShift));
  }
  else if (Key == VK_UP)
  { //increment curve index
   SelUpArrow(Shift.Contains(ssShift));
  }
  else if (Key == 'B')
  {
   if (CheckBox3d->Checked)
   {
    CheckBoxBv->Checked = !CheckBoxBv->Checked;
    UnmarkPoints();
    MarkPoints(true);
   }
  }
  else if (Key == 'Q')
  { //toggle area view
   if (((TPointSeries*)Chart1->Series[0 + m_count_z])->DrawArea)
    for (int z = 0; z < m_count_z; z++)
     ((TPointSeries*)Chart1->Series[z + m_count_z])->DrawArea = False;
   else
    for (int z = 0; z < m_count_z; z++)
     ((TPointSeries*)Chart1->Series[z + m_count_z])->DrawArea = True;
  }
  else if (Key == 'A')
  {
   if (Shift.Contains(ssCtrl))
   { //Ctrl+A
    if (CheckBox3d->Checked)
    {
     for(int z = 0; z < m_count_z; z++)
      for(int x = 0; x < m_count_x; x++)
       m_sel.Set(z, x, true);
     Chart1->Invalidate();
    }
    else
    {
     MarkPoints(false);
     for(int x = 0; x < m_count_x; x++)
      m_sel.Set(m_air_flow_position, x, true);
     MarkPoints(true);
    }
   }
   else
   {
    Chart1->View3DOptions->Rotation+=1;
    if (Chart1->View3DOptions->Rotation > 360)
     Chart1->View3DOptions->Rotation = 360;
   }
  }
  else if (Key == 'D')
  {
   Chart1->View3DOptions->Rotation-=1;
   if (Chart1->View3DOptions->Rotation < 0)
    Chart1->View3DOptions->Rotation = 0;
  }
  else if (Key == 'S')
  {
   Chart1->View3DOptions->Elevation+=1;
   if (Chart1->View3DOptions->Elevation > 360)
    Chart1->View3DOptions->Elevation = 360;
  }
  else if (Key == 'W')
  {
   Chart1->View3DOptions->Elevation-=1;
   if (Chart1->View3DOptions->Elevation < 0)
    Chart1->View3DOptions->Elevation = 0;
  }
  else if (Key == 'R')
  { //zoom+
   Chart1->View3DOptions->Zoom+=1;
   if (Chart1->View3DOptions->Zoom > 100)
    Chart1->View3DOptions->Zoom = 100;
  }
  else if (Key == 'F')
  { //zoom-
   Chart1->View3DOptions->Zoom-=1;
   if (Chart1->View3DOptions->Zoom < 0)
    Chart1->View3DOptions->Zoom = 0;
  }
  else if (Key == 'T')
  { //transparency+
   m_3d_transparency+=1.0;
   if (m_3d_transparency > 100.0)
    m_3d_transparency = 100.0;
   Series3D->Transparency = m_3d_transparency;
  }
  else if (Key == 'G')
  { //transparency-
   m_3d_transparency-=1.0;
   if (m_3d_transparency < .0)
    m_3d_transparency = .0;
   Series3D->Transparency = m_3d_transparency;
  }
  else if (Key == 0x43 && Shift.Contains(ssCtrl))
  { //Ctrl+C
   ClipboardCopy();
  }
  else if (Key == 0x56 && Shift.Contains(ssCtrl))
  { //Ctrl+V
   ClipboardPaste();
  }
  else if (Key == 'E')
  { //3D walls on/off
   if (Chart1->View3DWalls)
    Chart1->View3DWalls = false;
   else
    Chart1->View3DWalls = true;
  }
  else if (Key == VK_INSERT)
  {
   if (Shift.Contains(ssCtrl))
    ClipboardCopy();
   else if (Shift.Contains(ssShift))
    ClipboardPaste();
  }
  else if (Key == VK_HOME)
  { //move selection point to the left end
   if (Shift.Contains(ssCtrl))
   {
    if (CheckBox3d->Checked)
    {
     m_sel.Clear();
     m_val_z = m_count_z-1;
     m_sel.Set(m_val_z, m_val_x, true);
     Chart1->Invalidate();
    }
   }
   else
   {
    if (CheckBox3d->Checked)
    { //3D
     m_sel.Clear();
     m_val_x = 0;
     m_sel.Set(m_val_z, m_val_x, true);
     Chart1->Invalidate();
    }
    else
    {
     MarkPoints(false);
     m_sel.Clear();
     m_val_x = 0;
     m_sel.Set(m_air_flow_position, m_val_x, true);
     MarkPoints(true);
    }
   }
  }
  else if (Key == VK_END)
  { //move selection point to the right end
   if (Shift.Contains(ssCtrl))
   {
    if (CheckBox3d->Checked)
    {
     m_sel.Clear();
     m_val_z = 0;
     m_sel.Set(m_val_z, m_val_x, true);
     Chart1->Invalidate();
    }
   }
   else
   {
    if (CheckBox3d->Checked)
    { //3D
     m_sel.Clear();
     m_val_x = m_count_x-1;
     m_sel.Set(m_val_z, m_val_x, true);
     Chart1->Invalidate();
    }
    else
    {
     MarkPoints(false);
     m_sel.Clear();
     m_val_x = m_count_x-1;
     m_sel.Set(m_air_flow_position, m_val_x, true);
     MarkPoints(true);
    }
   }
  }
 }

 //Following code will simulate normal dialog messages
 TWinControl* ctrl = (TWinControl*)Sender;
 if (0 != ctrl->Perform(CM_CHILDKEY, Key, (LPARAM)ctrl))
  return;

 int mask = 0;
 switch(Key)
 {
  case VK_TAB:
   mask = DLGC_WANTTAB;
   break;
//  case VK_LEFT:
//  case VK_RIGHT:
//  case VK_UP:
//  case VK_DOWN:
//   mask = DLGC_WANTARROWS;
//   break;
  case VK_ESCAPE:
   Close();      //close form on Esc
  case VK_RETURN: 
  case VK_EXECUTE:
  case VK_CANCEL:
   mask = DLGC_WANTALLKEYS;
   break;
 }
 if (mask != 0)
 { 
  if ((0 == ctrl->Perform(CM_WANTSPECIALKEY, Key, 0)) &
     (ctrl->Perform(WM_GETDLGCODE, 0, 0) & (0==mask)) &
     (this->Perform(CM_DIALOGKEY, Key, 0)))
  return;
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnEnterChart(TObject* Sender)
{
 m_chart_active = true;
 if (!CheckBox3d->Checked) MarkPoints(true);
 Chart1->Title->Font->Style = TFontStyles() << fsBold;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnExitChart(TObject* Sender)
{
 m_chart_active = false;
 if (!CheckBox3d->Checked) MarkPoints(false);
 Chart1->Title->Font->Style = TFontStyles();
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::FormDeactivate(TObject *Sender)
{
 OnExitChart(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnCopyToCurve(TObject *Sender)
{
 int z = GetZPos(CheckBox3d->Checked ? m_val_z : m_air_flow_position);
 if (Sender == PM_CopyToCurve0)       CopyCurve(z, 0);
 else if (Sender == PM_CopyToCurve1)  CopyCurve(z, 1);
 else if (Sender == PM_CopyToCurve2)  CopyCurve(z, 2);
 else if (Sender == PM_CopyToCurve3)  CopyCurve(z, 3);
 else if (Sender == PM_CopyToCurve4)  CopyCurve(z, 4);
 else if (Sender == PM_CopyToCurve5)  CopyCurve(z, 5);
 else if (Sender == PM_CopyToCurve6)  CopyCurve(z, 6);
 else if (Sender == PM_CopyToCurve7)  CopyCurve(z, 7);
 else if (Sender == PM_CopyToCurve8)  CopyCurve(z, 8);
 else if (Sender == PM_CopyToCurve9)  CopyCurve(z, 9);
 else if (Sender == PM_CopyToCurve10) CopyCurve(z, 10);
 else if (Sender == PM_CopyToCurve11) CopyCurve(z, 11);
 else if (Sender == PM_CopyToCurve12) CopyCurve(z, 12);
 else if (Sender == PM_CopyToCurve13) CopyCurve(z, 13);
 else if (Sender == PM_CopyToCurve14) CopyCurve(z, 14);
 else if (Sender == PM_CopyToCurve15) CopyCurve(z, 15);
 else return;
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnCopyFromCurve(TObject *Sender)
{
 int z = GetZPos(CheckBox3d->Checked ? m_val_z : m_air_flow_position);
 if (Sender == PM_CopyFromCurve0)       CopyCurve(0, z);
 else if (Sender == PM_CopyFromCurve1)  CopyCurve(1, z);
 else if (Sender == PM_CopyFromCurve2)  CopyCurve(2, z);
 else if (Sender == PM_CopyFromCurve3)  CopyCurve(3, z);
 else if (Sender == PM_CopyFromCurve4)  CopyCurve(4, z);
 else if (Sender == PM_CopyFromCurve5)  CopyCurve(5, z);
 else if (Sender == PM_CopyFromCurve6)  CopyCurve(6, z);
 else if (Sender == PM_CopyFromCurve7)  CopyCurve(7, z);
 else if (Sender == PM_CopyFromCurve8)  CopyCurve(8, z);
 else if (Sender == PM_CopyFromCurve9)  CopyCurve(9, z);
 else if (Sender == PM_CopyFromCurve10) CopyCurve(10, z);
 else if (Sender == PM_CopyFromCurve11) CopyCurve(11, z);
 else if (Sender == PM_CopyFromCurve12) CopyCurve(12, z);
 else if (Sender == PM_CopyFromCurve13) CopyCurve(13, z);
 else if (Sender == PM_CopyFromCurve14) CopyCurve(14, z);
 else if (Sender == PM_CopyFromCurve15) CopyCurve(15, z);
 else return;
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnSetPtMovStep(TObject *Sender)
{
 Application->CreateForm(__classid(TPtMovStepDlg), &PtMovStepDlg);
 PtMovStepDlg->SetValue(m_pt_moving_step);
 if (PtMovStepDlg->ShowModal()==mrOk)
 {
  m_pt_moving_step = PtMovStepDlg->GetValue();
  if (m_pOnChangeSettings)
   m_pOnChangeSettings(m_param_on_change_sett);
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::LineSeriesGetMarkText(TChartSeries *Sender, int ValueIndex, AnsiString &LabelText)
{
 if (m_visibleMarkIdx==-1)
  return; //show all marks
 else if (m_visibleMarkIdx==-2)
  LabelText = ""; //hide all marks
 else
 { //show only one mark which is under mouse cursor
  if (ValueIndex==m_visibleMarkIdx)
   return;
  else
   LabelText = "";
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnHideMarks(TObject *Sender)
{
 PM_HideMarks->Checked = PM_HideMarks->Checked ? false : true; //toggle check mark
 if (!PM_HideMarks->Checked)
  m_visibleMarkIdx=-1; //show all marks
 else
  m_visibleMarkIdx=-2; //show only one mark which is under mouse cursor
 Chart1->Invalidate();
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnHideOldCurve(TObject *Sender)
{
 PM_HideOldCurve->Checked = PM_HideOldCurve->Checked ? false : true; //toggle check mark
 Chart1->Series[m_air_flow_position]->Active = !PM_HideOldCurve->Checked; //apply changes to current curve
 Chart1->Invalidate();
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnCreateForm(TObject *Sender)
{
 DPIAware da;
 ChangeScale(96, da.GetDPIX());
}

//---------------------------------------------------------------------------
void TForm3D::UpdateSystemColors(void)
{
 TrackBarAf->Brush->Color = TColor(GetSysColor(COLOR_BTNFACE));
 Chart1->LeftAxis->LabelsFont->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 Chart1->LeftAxis->Title->Font->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 Chart1->BottomAxis->LabelsFont->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 Chart1->BottomAxis->Title->Font->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 for(int z = 0; z < m_count_z; z++)
 {
  Chart1->Series[z]->Marks->Font->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
  Chart1->Series[z+m_count_z]->Marks->Font->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 }
 Chart1->Frame->Color = TColor(~GetSysColor(COLOR_BTNFACE)&0xFFFFFF);
 Chart1->LeftAxis->Axis->Color = TColor(~GetSysColor(COLOR_BTNFACE)&0xFFFFFF);
 Chart1->BottomAxis->Axis->Color = TColor(~GetSysColor(COLOR_BTNFACE)&0xFFFFFF);
 Chart1->DepthAxis->Axis->Color = TColor(~GetSysColor(COLOR_BTNFACE)&0xFFFFFF);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnExportCSV(TObject *Sender)
{
 if (saveDialog->Execute())
 {
  FILE* fh = _tfopen(saveDialog->FileName.c_str(),_T("wb+"));
  if (NULL == fh)
  {
   ::MessageBox(NULL, "Can't open file for writing!", "Error", MB_OK | MB_ICONERROR);
   return;
  }

  char temp[16];
  sprintf(temp, "%%s%c", m_csvsep_symb);

  //write function's values
  AnsiString valFmt = Chart1->Series[0]->ValueFormat;
  if (CheckBoxBv->Checked)
  {
   for(int z = 0; z < m_count_z; z++)
   {
    for(int x = 0; x < m_count_x; x++)
    {
     AnsiString as = FormatFloat(valFmt, GetItem_m(z, x));  
     if (x == m_count_x-1)
      fprintf(fh, "%s\r\n", as.c_str());    
     else
      fprintf(fh, temp, as.c_str());
    }
   }
  }
  else
  {
   for(int z = m_count_z-1; z >= 0; z--)
   {
    for(int x = 0; x < m_count_x; x++)
    {
     AnsiString as = FormatFloat(valFmt, GetItem_m(z, x));  
     if (x == m_count_x-1)
      fprintf(fh, "%s\r\n", as.c_str());    
     else
      fprintf(fh, temp, as.c_str());
    }
   }
  }

  //write horizontal axis's grid
  for(int x = 0; x < m_count_x; x++)
  {
   AnsiString as;
   if (m_pOnGetXAxisLabel && m_param_on_get_x_axis_label)
   {
    TCHAR string[64];  
    _stprintf(string, m_values_format_x.c_str(), m_u_slots[x]);
    m_pOnGetXAxisLabel(string, x, m_param_on_get_x_axis_label);
    as = string;
   }
   else
    as.sprintf(m_values_format_x.c_str(), m_u_slots[x]);

   if (x == m_count_x-1)
    fprintf(fh, "%s\r\n", as.c_str());    
   else
    fprintf(fh, temp, as.c_str());
  }

  fclose(fh);
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnImportCSV(TObject *Sender)
{
 if (openDialog->Execute())
 {
  FILE* file = fopen(openDialog->FileName.c_str(), "r");
  if (NULL == file)
  {
   ::MessageBox(NULL, "Can't open file for reading!", "Error", MB_OK | MB_ICONERROR);
   return;
  }
  //parse file
  std::vector<std::vector<std::string> > csv;
  char line[8192+1];
  while (fgets(line, 8192, file))
  {
   csv.push_back(StrUtils::TokenizeStr(line, m_csvsep_symb));
  }

  fclose(file);
    
  if (csv.size() != (size_t)(m_count_z+1))
  {
   ::MessageBox(NULL, "Error reading csv file! Incorrect number of rows.", "Error", MB_OK | MB_ICONERROR);
   return;
  }
  
  for(size_t i = 0; i < csv.size(); ++i)
  {
   if (csv[i].size() != (size_t)m_count_x)
   {
    ::MessageBox(NULL, "Error reading csv file! Incorrect number of values in row.", "Error", MB_OK | MB_ICONERROR);
    return;
   }
  }

  if (CheckBoxBv->Checked)
  {
   //set read values
   for(int z = 0; z < m_count_z; z++)
    for(int x = 0; x < m_count_x; x++)     
     RestrictAndSetValue(z, x, atof(csv[z][x].c_str()));
  }
  else
  {
   //set read values
   for(int z = 0; z < m_count_z; z++)
    for(int x = 0; x < m_count_x; x++)     
     RestrictAndSetValue(z, x, atof(csv[(m_count_z-1)-z][x].c_str()));
  }

  //Update
  if (CheckBox3d->Checked)
   UpdateSurfaceValues();
  else
   UpdateChartValues();

  if (m_pOnChange)
   m_pOnChange(m_param_on_change);
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::TrackLeftClick(TObject *Sender)
{
 TrackBarAf->Position--;
 TrackBarAfChange(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::TrackRightClick(TObject *Sender)
{
 TrackBarAf->Position++;
 TrackBarAfChange(NULL);
}

//---------------------------------------------------------------------------
void TForm3D::ClipboardCopy(void)
{
 if (CheckBox3d->Checked)
 {//3D
  if (m_sel.Size())
  {
   AnsiString valFmt = Chart1->Series[0]->ValueFormat;
   AnsiString str;
   for(int z = m_sel.Up(); z >= m_sel.Down(); --z)
   {
    for(int x = m_sel.Left(); x <= m_sel.Right(); ++x)
    {
     AnsiString as = FormatFloat(valFmt, GetItem_m(z, x));  
     if (x==m_sel.Right())
      as+="\r\n";
     else
      as+="\t";
     str+=as;
    }
   }
   Clipboard()->AsText = str;
  }
 }
 else
 {//2D
  if (m_sel.Size(m_air_flow_position))
  {
   AnsiString valFmt = Chart1->Series[0]->ValueFormat;
   AnsiString str;
   for(int x = m_sel.Left(); x <= m_sel.Right(); ++x)
   {
    AnsiString as = FormatFloat(valFmt, GetItem_m(m_air_flow_position, x));  
    if (x==m_sel.Right())
     as+="\r\n";
    else
     as+="\t";
    str+=as;
   }
   Clipboard()->AsText = str;
  } 
 }
}

//---------------------------------------------------------------------------
void TForm3D::ClipboardPaste(void)
{
 if (Clipboard()->HasFormat(CF_TEXT))
 {
  if (CheckBox3d->Checked)
  {//3D
   if (m_sel.Size())
   {
    //parse string
    AnsiString str = Clipboard()->AsText; 
    std::vector<_TSTRING> lines = StrUtils::TokenizeStr(str.c_str(), '\n');
    std::vector<std::vector<_TSTRING> > csv;
    for(size_t i = 0; i < lines.size(); ++i)
    {
     _TSTRING line = StrUtils::rtrim(lines[i], "\t\n\r");
     csv.push_back(StrUtils::TokenizeStr(line.c_str(), '\t'));
    }

    TCHAR decPt = _TDECIMAL_POINT(localeconv())[0]; //symbol of the decimal point used by current locale
 
    for(size_t z = 0; z < csv.size(); ++z)
    {
     int index_z = (CheckBoxBv->Checked ? m_count_z-1-m_sel.Up() : m_sel.Up()) - z;
     if (index_z >= 0)
     {
      for(size_t x = 0; x < csv[z].size(); ++x)
      {
       int index_x = m_sel.Left() + x;
       if (index_x < m_count_x)
       {
        //change decimal point, thus it will be compatible with our current locale
        _TSTRING& s = csv[z][x];
        std::size_t pos = s.find_first_of(_T(".,"));
        if (pos != _TSTRING::npos)
         s[pos] = decPt;

        RestrictAndSetValue(index_z, index_x, atof(s.c_str()));
       }
      }
     }
    }
    UpdateSurfaceValues();
    if (m_pOnChange)
     m_pOnChange(m_param_on_change);
   }
  }
  else
  {//2D
   if (m_sel.Size(m_air_flow_position))
   {
    //parse string
    AnsiString str = Clipboard()->AsText; 
    std::vector<_TSTRING> lines = StrUtils::TokenizeStr(str.c_str(), '\n');
    std::vector<std::vector<_TSTRING> > csv;
    for(size_t i = 0; i < lines.size(); ++i)
    {
     _TSTRING line = StrUtils::rtrim(lines[i], "\t\n\r");
     csv.push_back(StrUtils::TokenizeStr(line.c_str(), '\t'));
    }

    TCHAR decPt = _TDECIMAL_POINT(localeconv())[0]; //symbol of the decimal point used by current locale
 
    for(size_t x = 0; x < csv[0].size(); ++x)
    {
     int index = m_sel.Left() + x;
     if (index < m_count_x)
     {
      //change decimal point, thus it will be compatible with our current locale
      _TSTRING& s = csv[0][x];
      std::size_t pos = s.find_first_of(_T(".,"));
      if (pos != _TSTRING::npos)
       s[pos] = decPt;

      RestrictAndSetChartValue(index, atof(s.c_str()));
     }
    }
    if (m_pOnChange)
     m_pOnChange(m_param_on_change);    
   }
  }
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnCopy(TObject *Sender)
{
 ClipboardCopy();
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnPaste(TObject *Sender)
{
 ClipboardPaste();
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnInc(TObject *Sender)
{
 ShiftPoints(Chart1->LeftAxis->Inverted ? -m_pt_moving_step : m_pt_moving_step);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnDec(TObject *Sender)
{
 ShiftPoints(Chart1->LeftAxis->Inverted ? m_pt_moving_step : -m_pt_moving_step);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnSetTo(TObject *Sender)
{
 Application->CreateForm(__classid(TPtMovStepDlg), &PtMovStepDlg);
 PtMovStepDlg->SetValue(m_pt_moving_step);
 char title[1024 + 1];
 ::LoadString(m_hInst, IDS_DLG_SETTO, title, 1024);
 PtMovStepDlg->SetTitle(title);
 if (PtMovStepDlg->ShowModal()==mrOk)
 {
  if (!CheckBox3d->Checked)
  { //2D
   for(int x = 0; x < m_count_x; ++x)
   {
    if (false==m_sel.Get(m_air_flow_position, x))
     continue;  
    RestrictAndSetChartValue(x, PtMovStepDlg->GetValue());   
   }
  }
  else
  { //3D
   for(int z = 0; z < m_count_z; ++z)
   {
    for(int x = 0; x < m_count_x; ++x)
    {
     if (false==m_sel.Get(z, x))
      continue;
     RestrictAndSetValue(GetZPos(z), x, PtMovStepDlg->GetValue());
    }
   }
   UpdateSurfaceValues();
  }
 }
 if (m_pOnChange) 
  m_pOnChange(m_param_on_change);    
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnSub(TObject *Sender)
{
 Application->CreateForm(__classid(TPtMovStepDlg), &PtMovStepDlg);
 PtMovStepDlg->SetValue(m_pt_moving_step);
 char title[1024 + 1];
 ::LoadString(m_hInst, IDS_DLG_SUB, title, 1024);
 PtMovStepDlg->SetTitle(title);
 if (PtMovStepDlg->ShowModal()==mrOk)
 {
  if (!CheckBox3d->Checked)
  { //2D
   for(int x = 0; x < m_count_x; ++x)
   {
    if (false==m_sel.Get(m_air_flow_position, x))
     continue;  
    float value = GetItem_m(m_air_flow_position, x);
    value-= Chart1->LeftAxis->Inverted ? -PtMovStepDlg->GetValue() : PtMovStepDlg->GetValue();
    RestrictAndSetChartValue(x, value);   
   }
  }
  else
  { //3D
   for(int z = 0; z < m_count_z; ++z)
   {
    for(int x = 0; x < m_count_x; ++x)
    {
     if (false==m_sel.Get(z, x))
      continue;
     float value = GetItem_m(GetZPos(z), x);
     value-= Chart1->LeftAxis->Inverted ? -PtMovStepDlg->GetValue() : PtMovStepDlg->GetValue();
     RestrictAndSetValue(GetZPos(z), x, value);
    }
   }
   UpdateSurfaceValues();
  }
 }
 if (m_pOnChange) 
  m_pOnChange(m_param_on_change);    
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnAdd(TObject *Sender)
{
 Application->CreateForm(__classid(TPtMovStepDlg), &PtMovStepDlg);
 PtMovStepDlg->SetValue(m_pt_moving_step);
 char title[1024 + 1];
 ::LoadString(m_hInst, IDS_DLG_ADD, title, 1024);
 PtMovStepDlg->SetTitle(title);
 if (PtMovStepDlg->ShowModal()==mrOk)
 {
  if (!CheckBox3d->Checked)
  { //2D
   for(int x = 0; x < m_count_x; ++x)
   {
    if (false==m_sel.Get(m_air_flow_position, x))
     continue;  
    float value = GetItem_m(m_air_flow_position, x);
    value+= Chart1->LeftAxis->Inverted ? -PtMovStepDlg->GetValue() : PtMovStepDlg->GetValue();
    RestrictAndSetChartValue(x, value);   
   }
  }
  else
  { //3D
   for(int z = 0; z < m_count_z; ++z)
   {
    for(int x = 0; x < m_count_x; ++x)
    {
     if (false==m_sel.Get(z, x))
      continue;
     float value = GetItem_m(GetZPos(z), x);
     value+= Chart1->LeftAxis->Inverted ? -PtMovStepDlg->GetValue() : PtMovStepDlg->GetValue();
     RestrictAndSetValue(GetZPos(z), x, value);
    }
   }
   UpdateSurfaceValues();
  }
 }
 if (m_pOnChange) 
  m_pOnChange(m_param_on_change);    
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnMul(TObject *Sender)
{
 Application->CreateForm(__classid(TPtMovStepDlg), &PtMovStepDlg);
 PtMovStepDlg->SetValue(m_pt_moving_step);
 char title[1024 + 1];
 ::LoadString(m_hInst, IDS_DLG_MUL, title, 1024);
 PtMovStepDlg->SetTitle(title);
 if (PtMovStepDlg->ShowModal()==mrOk)
 {
  if (!CheckBox3d->Checked)
  { //2D
   for(int x = 0; x < m_count_x; ++x)
   {
    if (false==m_sel.Get(m_air_flow_position, x))
     continue;  
    float value = GetItem_m(m_air_flow_position, x);
    float mult = PtMovStepDlg->GetValue()==0 ? 1e-6 : PtMovStepDlg->GetValue();
    value*= Chart1->LeftAxis->Inverted ? 1.0f/mult : PtMovStepDlg->GetValue();
    RestrictAndSetChartValue(x, value);   
   }
  }
  else
  { //3D
   for(int z = 0; z < m_count_z; ++z)
   {
    for(int x = 0; x < m_count_x; ++x)
    {
     if (false==m_sel.Get(z, x))
      continue;
     float value = GetItem_m(GetZPos(z), x);
     float mult = PtMovStepDlg->GetValue()==0 ? 1e-6 : PtMovStepDlg->GetValue();
     value*= Chart1->LeftAxis->Inverted ? 1.0f/mult : PtMovStepDlg->GetValue();
     RestrictAndSetValue(GetZPos(z), x, value);
    }
   }
   UpdateSurfaceValues();
  }
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
double __fastcall TForm3D::OnGetYValue(TChartSeries *Sender, int X, int Z)
{
 X--, Z--; //delphi...

 if (CheckBoxBv->Checked)
  Z = (m_count_z-1) - Z; //invert order along Z-axis
  
 //Avoid issue, appeared when values of functions are equal.
 //So, we add very small values to make function's points different
 double delta = (X + Z) * 1e-12;
 return GetItem_m(Z, X) + delta;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnAfterDrawChart(TObject *Sender)
{
 if (!CheckBox3d->Checked)
  return;

 for(int Z = 0; Z < m_count_z; ++Z)
 {
  for(int X = 0; X < m_count_x; ++X)
  {
   if (false==m_sel.Get(Z, X))
    continue;

   int ptIdx = (Z * m_count_x) + X;

   int x = Series3D->CalcXPos(ptIdx);
   int y = Series3D->CalcYPos(ptIdx);
   int z = Series3D->CalcZPos(ptIdx);

   TCanvas3D *pCanvas = Chart1->Canvas;
   pCanvas->Pen->Color = clNavy;

   if ((m_val_z==Z && m_val_x==X) && m_chart_active)
    pCanvas->Pen->Width = 2;
   else
    pCanvas->Pen->Width = 1;

   pCanvas->Brush->Color = clNavy;

   int d = 7;
   pCanvas->MoveTo3D(x - d, y, z - d);
   pCanvas->LineTo3D(x - d, y, z + d);
   pCanvas->LineTo3D(x + d, y, z + d);
   pCanvas->LineTo3D(x + d, y, z - d);
   pCanvas->LineTo3D(x - d, y, z - d);
  }
 }
}

//---------------------------------------------------------------------------
void TForm3D::SetDimentions(int z, int x)
{
 TrackBarAf->Max = m_count_z - 1;
 m_sel.SetDimentions(z, x);
 m_count_z = z;
 m_count_x = x;
}

//---------------------------------------------------------------------------
void TForm3D::SetRange(float ymin, float ymax)
{
 m_fnc_min = ymin;
 m_fnc_max = ymax;
 //value range on the graph will be slightly wider than required...
 int range = m_fnc_max - m_fnc_min;
 Chart1->LeftAxis->Maximum = m_fnc_max + range / 15.0f;
 Chart1->LeftAxis->Minimum = m_fnc_min - range / 20.0f;
}

//---------------------------------------------------------------------------
void TForm3D::SetChartTitle(const AnsiString& title)
{
 m_u_title = title;
 Chart1->Title->Text->SetText(title.c_str());
}

//---------------------------------------------------------------------------
void TForm3D::SetXAxisTitle(const AnsiString& title)
{
 m_x_title = title;
 Chart1->BottomAxis->Title->Caption = title;
}

//---------------------------------------------------------------------------
void TForm3D::SetYAxisTitle(const AnsiString& title)
{
 m_y_title = title;
 Chart1->LeftAxis->Title->Caption = title;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnAllowMouseZoom(TObject *Sender)
{
 if (PM_AllowMouseZoom->Checked)
 {
  PM_AllowMouseZoom->Checked = false;
  Chart1->Zoom->Allow = false;
 }
 else
 {
  m_mc_allow = false;
  Chart1->Zoom->Allow = true;
  PM_AllowMouseZoom->Checked = true;
  PM_AllowMouseCamera->Checked = false;
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnAllowMouseCamera(TObject *Sender)
{
 if (PM_AllowMouseCamera->Checked)
 {
  PM_AllowMouseCamera->Checked = false;
  m_mc_allow = false;
 }
 else
 {
  m_mc_allow = true;
  Chart1->Zoom->Allow = false;
  PM_AllowMouseCamera->Checked = true;
  PM_AllowMouseZoom->Checked = false;
 } 
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnInterpolate(TObject *Sender)
{
 if (CheckBox3d->Checked)
 { //3D
  if (CheckBoxBv->Checked) m_sel.InvertZ();
  std::vector<int> selpts;
  selpts.push_back(m_sel.Left());
  selpts.push_back(m_sel.Right());
  Interpolate2D(m_sel.Down(), selpts);
  Interpolate2D(m_sel.Up(), selpts);
  Interpolate3D(m_sel.Left(), m_sel.Right(), m_sel.Down(), m_sel.Up());
  if (CheckBoxBv->Checked) m_sel.InvertZ();
  UpdateSurfaceValues();
 }
 else
 { //2D
  std::vector<int> selpts = m_sel.GetSelIdxs(m_air_flow_position);
  Interpolate2D(m_air_flow_position, selpts);
  UpdateChartValues();
 }

 if (m_pOnChange) 
  m_pOnChange(m_param_on_change);    
}

//---------------------------------------------------------------------------
void TForm3D::Interpolate2D(int z, const std::vector<int>& i_selpts)
{
 if (i_selpts.size())
 {
  bool gap = false;
  for(size_t i = 0; i < i_selpts.size()-1; ++i)
  {
   if (abs(i_selpts[i+1]-i_selpts[i]) > 1)
   {
    gap = true;
    break;
   }
  }

  std::vector<int> selpts;
  if (!gap)
  {
   selpts.push_back(i_selpts.front());
   selpts.push_back(i_selpts.back());
  }
  else
  {
   selpts = i_selpts;
  }
  
  for(size_t i = 0; i < selpts.size() - 1; ++i)
  {
   int size = selpts[i + 1] - selpts[i];
   if (size < 2)
    continue; //skip two neighbour selected points  
   double firstPtVal = GetItem_m(z, selpts[i]);
   double lastPtVal = GetItem_m(z, selpts[i + 1]);
   for (int x = selpts[i] + 1; x < selpts[i + 1]; x++)
    RestrictAndSetValue(z, x, firstPtVal + (((lastPtVal-firstPtVal) / ((double)size)) * (x-(selpts[i]))));
  }
 }
}

//---------------------------------------------------------------------------
//interpolate betwwen two z-curves (first and last)
void TForm3D::Interpolate3D(int left, int right, int down, int up)
{
 double intrmPtCount = up - down;
 for (int x = left; x <= right; ++x)
 {
  for (int z = down+1; z <= up-1; z++)
  {
   double firstPtVal = GetItem_m(down, x);
   double lastPtVal = GetItem_m(up, x);
   double value = firstPtVal + (((lastPtVal-firstPtVal) / intrmPtCount) * (z-down));
   RestrictAndSetValue(z, x, value);
  }
 }
}

//---------------------------------------------------------------------------
