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

#include <vcl.h>
#pragma hdrstop

#include "../common/MathHelpers.h"
#include "Form3D.h"
#include "resource.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

bool RemoveInstanceByHWND(HWND hWnd);

//---------------------------------------------------------------------------
//цвета для 3D графика
long col[16] ={0xA88CD5, 0xD26EDC, 0xC38CBE, 0xCB9491,
               0xC8AA85, 0xCDC38F, 0xD3D48F, 0xB2D573,
               0x87DCA3, 0x87e4A3, 0x99E9A3, 0x5DF3DF,
               0x3ACDE9, 0x78AFE9, 0x5D94EB, 0x555AFD};

//---------------------------------------------------------------------------
__fastcall TForm3D::TForm3D(TComponent* Owner)
: TForm(Owner)
, m_count_x(0)
, m_count_z(0)
, m_fnc_min(0)
, m_fnc_max(0)
, mp_modified_function(NULL)
, mp_original_function(NULL)
, m_u_title("")
, m_x_title("")
, m_pOnChange(NULL)
, m_pOnClose(NULL)
, m_pOnWndActivation(NULL)
, m_pOnGetXAxisLabel(NULL)
, m_param_on_change(NULL)
, m_param_on_close(NULL)
, m_param_on_wnd_activation(NULL)
, m_param_on_get_x_axis_label(NULL)
, m_setval(0)
, m_val_n(0)
, m_air_flow_position(0)
, m_values_format_x("%.00f")  //integer 
, m_chart_active(true)
{
 m_selpts.push_back(0);
}

//---------------------------------------------------------------------------
void TForm3D::DataPrepare()
{
 HideAllSeries();
 CheckBoxBv->Enabled = false;
 TrackBarAf->Max = m_count_z - 1;
 ShowPoints(true);
 m_setval = 0;
 m_val_n  = 0;

 Chart1->Title->Text->Clear();
 Chart1->Title->Text->Add(m_u_title);
 Chart1->BottomAxis->Title->Caption = m_x_title;

 //диапазон значений на графике будет немного шире чем требуемый...
 int range = m_fnc_max - m_fnc_min;
 Chart1->LeftAxis->Maximum = m_fnc_max + range / 15.0f;
 Chart1->LeftAxis->Minimum = m_fnc_min - range / 20.0f;

 Chart1->Chart3DPercent = 29;
 FillChart(0,0);

 SetAirFlow(m_air_flow_position); //set trackbar position
 CheckBox3dClick(NULL);

 if (m_chart_active)
 {
  UnmarkPoints();
  MarkPoints(true);
  Chart1->Title->Font->Style = TFontStyles() << fsBold;
 }
 else
 {
  UnmarkPoints();
  Chart1->Title->Font->Style = TFontStyles();
 }
}

//---------------------------------------------------------------------------
void TForm3D::SetOnChange(EventHandler i_pOnChange,void* i_param)
{
 m_pOnChange = i_pOnChange;
 m_param_on_change = i_param;
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
void TForm3D::Enable(bool enable)
{
 if (false==enable)
 {
  for (int i = 0; i < 32; i++ )
   Chart1->Series[i]->Active = false;
  Chart1->Title->Font->Style = TFontStyles();
 }
 else
  DataPrepare();

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
} 

//---------------------------------------------------------------------------
void TForm3D::InitHints(HINSTANCE hInstance)
{
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
void __fastcall TForm3D::TrackBarAfChange(TObject *Sender)
{
 SetAirFlow(TrackBarAf->Position);
 m_setval = 0;
 m_val_n  = 0; 
 UnmarkPoints();
 MarkPoints(true);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1ClickSeries(TCustomChart *Sender,
      TChartSeries *Series, int ValueIndex, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 if (Series==Chart1->Series[m_air_flow_position + m_count_z])
 {
  //if Ctrl key is not pressed then, select single point
  if (!Shift.Contains(ssCtrl))
  {
   MarkPoints(false);
   m_selpts.clear();
   m_selpts.push_back(ValueIndex);
   MarkPoints(true);
  }

  if (Button==mbLeft)
  {
   m_setval  = 1;
   m_val_n   = ValueIndex;
  }
 }
 m_prev_pt = std::make_pair(X, Y);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 //implementation of multiple points selection
 if (Shift.Contains(ssCtrl) && (m_prev_pt.first == X && m_prev_pt.second == Y))
 {
  MarkPoints(false);
  std::deque<int>::iterator it = std::find(m_selpts.begin(), m_selpts.end(), m_val_n);
  if (it == m_selpts.end())
   m_selpts.push_back(m_val_n);
  else
   m_selpts.erase(it);
  MarkPoints(true);
  std::sort(m_selpts.begin(), m_selpts.end());
 }

 if ((m_pOnChange) && (m_setval))
  m_pOnChange(m_param_on_change);
 m_setval = 0;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1MouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
 double v;
 if (m_setval)
 {//select on the fly point being moved
  std::deque<int>::iterator it = std::find(m_selpts.begin(), m_selpts.end(), m_val_n);
  if (it == m_selpts.end())
  {
   m_selpts.push_back(m_val_n);
   MarkPoints(true);
   std::sort(m_selpts.begin(), m_selpts.end());
  }
  //Move all selected points
  v = Chart1->Series[m_air_flow_position + m_count_z]->YScreenToValue(Y);
  ShiftPoints(v - Chart1->Series[m_air_flow_position + m_count_z]->YValue[m_val_n]);
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
   TCHAR string[64];
   _tcscpy(string, LabelText.c_str());
   m_pOnGetXAxisLabel(string, ValueIndex, m_param_on_get_x_axis_label);
   LabelText = string;
  }
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::CheckBox3dClick(TObject *Sender)
{
 if (CheckBox3d->Checked)
 {
  ShowPoints(false);
  Chart1->View3D  = true;
  MakeAllVisible();
  TrackBarAf->Enabled = false;
  LabelAfv->Enabled = false;
  CheckBoxBv->Enabled = true;
  CheckBoxBvClick(NULL);
  ButtonAngleUp->Enabled = false;
  ButtonAngleDown->Enabled = false;
  ButtonAngleUp->Visible = false;
  ButtonAngleDown->Visible = false;
  Smoothing3x->Enabled = false;
  Smoothing5x->Enabled = false;
  Smoothing3x->Visible = false;
  Smoothing5x->Visible = false;
  for (int i = 0; i < PopupMenu->Items->Count; i++)
   PopupMenu->Items->Items[i]->Enabled = false;
 }
 else
 {
  ShowPoints(true);
  Chart1->View3D = false;
  MakeOneVisible(m_air_flow_position);
  TrackBarAf->Enabled = true;
  LabelAfv->Enabled = true;
  CheckBoxBv->Enabled = false;
  FillChart(0,0);
  ButtonAngleUp->Enabled = true;
  ButtonAngleDown->Enabled = true;
  ButtonAngleUp->Visible = true;
  ButtonAngleDown->Visible = true;
  Smoothing3x->Enabled = true;
  Smoothing5x->Enabled = true;
  Smoothing3x->Visible = true;
  Smoothing5x->Visible = true;
  for (int i = 0; i < PopupMenu->Items->Count; i++)
   PopupMenu->Items->Items[i]->Enabled = true;
 }
 //Fill bottom area when 3D view is turned on
 for (int i = 0; i < m_count_z; i++)
  ((TPointSeries*)Chart1->Series[i + m_count_z])->DrawArea = CheckBox3d->Checked;  
}

//---------------------------------------------------------------------------
//вид 3D графика сзади или спереди
void __fastcall TForm3D::CheckBoxBvClick(TObject *Sender)
{
 FillChart(!CheckBoxBv->Checked, 1);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnCloseForm(TObject *Sender, TCloseAction &Action)
{
 if (m_pOnClose)
  m_pOnClose(m_param_on_close);
 RemoveInstanceByHWND(Handle);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ButtonAngleUpClick(TObject *Sender)
{
 for (int i = 0; i < 16; i++ )
  RestrictAndSetChartValue(i, Chart1->Series[m_air_flow_position + m_count_z]->YValue[i] + 0.5);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ButtonAngleDownClick(TObject *Sender)
{
 for (int i = 0; i < 16; i++ )
  RestrictAndSetChartValue(i, Chart1->Series[m_air_flow_position + m_count_z]->YValue[i] - 0.5);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Smoothing3xClick(TObject *Sender)
{
 float* p_source_function = new float[m_count_x];
 for (int i = 0; i < m_count_x; ++i)
  p_source_function[i] = GetItem_m(m_air_flow_position, i);
 MathHelpers::Smooth1D(p_source_function, GetItem_mp(m_air_flow_position, 0), m_count_x, 3);
 delete[] p_source_function;
 for (int i = 0; i < m_count_x; i++ )
  RestrictAndSetChartValue(i, *GetItem_mp(m_air_flow_position, i));

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Smoothing5xClick(TObject *Sender)
{
 float* p_source_function = new float[m_count_x];
 for (int i = 0; i < m_count_x; ++i)
  p_source_function[i] = GetItem_m(m_air_flow_position, i);
 MathHelpers::Smooth1D(p_source_function, GetItem_mp(m_air_flow_position, 0), m_count_x, 5);
 delete[] p_source_function;
 for (int i = 0; i < m_count_x; i++ )
  RestrictAndSetChartValue(i, *GetItem_mp(m_air_flow_position, i));

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
//get item from original function
float TForm3D::GetItem_o(int z, int x) const
{
 if ((z >= m_count_z) || (x >= m_count_x)) return 0.0f;
 int i  = (m_count_z - 1) - z;
 return *(mp_original_function + ((i * m_count_x) + x));
}

//---------------------------------------------------------------------------
//get item from modified function
float TForm3D::GetItem_m(int z, int x) const
{
 if ((z >= m_count_z) || (x >= m_count_x)) return 0.0f;
 int i  = (m_count_z - 1) - z;
 return *(mp_modified_function + ((i * m_count_x) + x));
}

//---------------------------------------------------------------------------
float* TForm3D::GetItem_mp(int z, int x)
{
 if ((z >= m_count_z) || (x >= m_count_x)) return NULL;
 int i  = (m_count_z - 1) - z;
 return (mp_modified_function + ((i * m_count_x) + x));
}

//---------------------------------------------------------------------------
//set item in modified function
int TForm3D::SetItem(int z, int x, float value)
{
 if ((z >= m_count_z) || (x >= m_count_x)) return 0;
 int i  = (m_count_z - 1) - z;
 *(mp_modified_function + ((i * m_count_x) + x)) = value;
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
void TForm3D::MakeOneVisible(int flow)
{
 for(int i = 0; i < m_count_z; i++)
 {
  Chart1->Series[i]->Active = (i==flow);
  Chart1->Series[i+m_count_z]->Active = (i==flow);
 }
}

//---------------------------------------------------------------------------
void TForm3D::MakeAllVisible(void)
{
 for(int i = 0; i < m_count_z; i++)
 {
  Chart1->Series[i]->Active = false;
  Chart1->Series[i + m_count_z]->Active = true;
 }
}

//---------------------------------------------------------------------------
void TForm3D::ShowPoints(bool show)
{
 for (int i = 0; i < m_count_z; i++)
 {
  Chart1->Series[i]->Marks->Visible = show;
  Chart1->Series[i + m_count_z]->Marks->Visible = show;
  ((TPointSeries*)Chart1->Series[i])->Pointer->Visible = show;
  ((TPointSeries*)Chart1->Series[i + m_count_z])->Pointer->Visible = show;
 }
}

//---------------------------------------------------------------------------
void TForm3D::FillChart(bool dir,int cm)
{
 int d, k; AnsiString as;
 if (!dir)
  d = 1, k = 0;
 else
  d = -1, k = m_count_z - 1;

 for(int j = 0; j < m_count_z; j++)
 {
  Chart1->Series[j]->Clear();
  Chart1->Series[j+m_count_z]->Clear();
 }

 for(int j = 0; j < m_count_z; j++)
 {
  for(int i = 0; i < m_count_x; i++)
  {
   as.sprintf(m_values_format_x.c_str(),m_u_slots[i]);
   if (cm)
   {
    Chart1->Series[k + m_count_z]->Add(GetItem_m(j,i),as,TColor(col[j]));
   }
   else
   {
    Chart1->Series[k]->Add(GetItem_o(j,i),as,clAqua);
    Chart1->Series[k + m_count_z]->Add(GetItem_m(j,i),as,clRed);
   }
  }
  k+=d;
 }
}

//---------------------------------------------------------------------------
//Скрывает все серии значений
void TForm3D::HideAllSeries(void)
{
 for(int i = 0; i < 32; i++)
  Chart1->Series[i]->Active = false;
}

//---------------------------------------------------------------------------
int __fastcall TForm3D::GetCurveSelIndex(void) const
{
 return ((CheckBox3d->Checked && !CheckBoxBv->Checked) ? m_count_z - 1 - m_air_flow_position : m_air_flow_position);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ShiftPoints(float i_value)
{
 for(size_t i = 0; i < m_selpts.size(); ++i)
  RestrictAndSetChartValue(m_selpts[i], Chart1->Series[GetCurveSelIndex() + m_count_z]->YValue[m_selpts[i]] + i_value);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::MarkPoints(bool i_mark)
{
 TColor color = CheckBox3d->Checked ? TColor(col[m_air_flow_position]) : clRed;
 for(size_t i = 0; i < m_selpts.size(); ++i)
  (Chart1->Series[GetCurveSelIndex() + m_count_z])->ValueColor[m_selpts[i]] = (i_mark ? clNavy : color);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::UnmarkPoints(void)
{
 TColor color = CheckBox3d->Checked ? TColor(col[m_air_flow_position]) : clRed;
 for(int i = 0; i < m_count_x; ++i)
  (Chart1->Series[GetCurveSelIndex() + m_count_z])->ValueColor[i] = color;
}

//---------------------------------------------------------------------------
void TForm3D::RestrictAndSetChartValue(int index, double v)
{
 v = MathHelpers::RestrictValue<double>(v, m_fnc_min, m_fnc_max);
 SetItem(m_air_flow_position, index, v);
 Chart1->Series[GetCurveSelIndex() + m_count_z]->YValue[index] = v;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::CopyCurve(int fromIndex, int toIndex)
{
 for(int i = 0; i < m_count_x; ++i)
 { 
  SetChartValue(toIndex, i, GetChartValue(fromIndex, i));
  SetItem(toIndex, i, Chart1->Series[fromIndex + m_count_z]->YValue[i]);
 }
}

//---------------------------------------------------------------------------
double TForm3D::GetChartValue(int z, int index)
{
 return Chart1->Series[z + m_count_z]->YValue[index];
}

//---------------------------------------------------------------------------
void TForm3D::SetChartValue(int z, int index, double value)
{
 Chart1->Series[z + m_count_z]->YValue[index] = value;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::WndProc(Messages::TMessage &Message)
{
 TForm::WndProc(Message);

 if (Message.Msg == WM_SYSCOMMAND && m_pOnWndActivation)
  m_pOnWndActivation(m_param_on_wnd_activation, Message.WParam);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnZeroAllPoints(TObject *Sender)
{
 for (int i = 0; i < m_count_x; i++ )
  RestrictAndSetChartValue(i, 0);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnDuplicate1stPoint(TObject *Sender)
{
 for (int i = 0; i < m_count_x; i++ )
  RestrictAndSetChartValue(i, GetChartValue(m_air_flow_position, 0));
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnBldCurveUsing1stAndLastPoints(TObject *Sender)
{
 double firstPtVal = GetChartValue(m_air_flow_position, 0);
 double lastPtVal = GetChartValue(m_air_flow_position, m_count_x - 1);
 double intrmPtCount = m_count_x - 1;
 for (int i = 1; i < m_count_x - 1; i++ )
  RestrictAndSetChartValue(i, firstPtVal + (((lastPtVal-firstPtVal) / intrmPtCount) * i));
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnZeroAllCurves(TObject *Sender)
{
 for (int z = 0; z < m_count_z; z++ )
 {
  for (int x = 0; x < m_count_x; x++ )
  {
   SetChartValue(z, x, 0);
   SetItem(z, x, 0);
  }
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnDuplicateThisCurve(TObject *Sender)
{
 for (int z = 0; z < m_count_z; z++ )
 {
  for (int x = 0; x < m_count_x; x++ )
  {
   SetChartValue(z, x, GetChartValue(m_air_flow_position, x));
   SetItem(z, x, GetChartValue(m_air_flow_position, x));
  }
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnBuildShapeUsing1stAndLastCurves(TObject *Sender)
{
 double intrmPtCount = m_count_z - 1;
 for (int x = 0; x < m_count_x; x++ )
 {
  for (int z = 1; z < m_count_z - 1; z++ )
  {
   double firstPtVal = GetChartValue(0, x);
   double lastPtVal = GetChartValue(m_count_z - 1, x);
   double value = firstPtVal + (((lastPtVal-firstPtVal) / intrmPtCount) * z);
   SetChartValue(z, x, value);
   SetItem(z, x, value);
  }
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::SelLeftArrow(bool i_shift)
{
 if (i_shift)
 {//Shift key is pressed
  if (m_selpts.size())
  {
   MarkPoints(false);
   if (m_selpts.front() != m_val_n && m_selpts.back() != m_selpts.front())
   {
    m_selpts.pop_back();
    m_val_n = m_selpts.back();
   }
   else
   {
    m_val_n = m_selpts.front() - 1;
    if (m_val_n >= 0)
     m_selpts.push_front(m_val_n);
    else
     m_val_n = 0;
   }
   MarkPoints(true);
  }
 }
 else
 {//Without shift
  MarkPoints(false);
  int prev_pt = m_selpts.size() ? m_selpts.back() - 1 : 0;
  if (prev_pt < 0)
   prev_pt = 0;
  m_selpts.clear();
  m_selpts.push_back(prev_pt);
  MarkPoints(true);
  m_val_n = prev_pt;
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::SelRightArrow(bool i_shift)
{
 if (i_shift)
 { //Shift key is pressed
  if (m_selpts.size())
  {
   MarkPoints(false);
   if (m_selpts.back() != m_val_n && m_selpts.back() != m_selpts.front())
   {
    m_selpts.pop_front();
    m_val_n = m_selpts.front();
   }
   else
   {
    m_val_n = m_selpts.back() + 1;
    if (m_val_n < m_count_x)
     m_selpts.push_back(m_val_n);
    else
     m_val_n = m_count_x-1;
   }
   MarkPoints(true);
  }
 }
 else
 { //Without shift
  MarkPoints(false);
  int next_pt = m_selpts.size() ? m_selpts.back() + 1 : 0;
  if (next_pt >= m_count_x)
   next_pt = m_count_x-1;
  m_selpts.clear();
  m_selpts.push_back(next_pt);
  MarkPoints(true);
  m_val_n = next_pt;
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::CtrlKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
 //Implement keyboard actions related to chart
 if (ActiveControl == Chart1)
 {
  if (Key == VK_UP)
  { //move points upward
   ShiftPoints(Chart1->LeftAxis->Inverted ? -0.5 : 0.5);
   if (m_pOnChange)
    m_pOnChange(m_param_on_change);
  }
  else if (Key == VK_DOWN)
  { //move points downward
   ShiftPoints(Chart1->LeftAxis->Inverted ? 0.5 : -0.5);
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
  else if (Key == 'Z')
  { //decrement curve index
   if (CheckBox3d->Checked) UnmarkPoints();   //3D view
   if (m_air_flow_position > 0)
    --m_air_flow_position;
   SetAirFlow(m_air_flow_position);
   if (CheckBox3d->Checked) MarkPoints(true); //3D view
  }
  else if (Key == 'X')
  { //increment curve index
   if (CheckBox3d->Checked) UnmarkPoints();   //3D view
   if (m_air_flow_position < (m_count_z-1))
    ++m_air_flow_position;
   SetAirFlow(m_air_flow_position);
   if (CheckBox3d->Checked) MarkPoints(true); //3D view
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
    for (int i = 0; i < m_count_z; i++)
     ((TPointSeries*)Chart1->Series[i + m_count_z])->DrawArea = False;  
   else
    for (int i = 0; i < m_count_z; i++)
     ((TPointSeries*)Chart1->Series[i + m_count_z])->DrawArea = True;  
  }
  else if (Key == 'A')
  {
   Chart1->View3DOptions->Rotation+=1;
   if (Chart1->View3DOptions->Rotation > 360)
    Chart1->View3DOptions->Rotation = 360;
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
  case VK_RETURN: 
  case VK_EXECUTE:
  case VK_ESCAPE:
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
 MarkPoints(true);
 Chart1->Title->Font->Style = TFontStyles() << fsBold;
 m_chart_active = true;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnExitChart(TObject* Sender)
{
 MarkPoints(false);
 Chart1->Title->Font->Style = TFontStyles();
 m_chart_active = false;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnChartMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if (ActiveControl != Chart1)
 {
  ActiveControl = Chart1;
  m_chart_active = true;
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnCopyToCurve(TObject *Sender)
{
 if (Sender == PM_CopyToCurve0)       CopyCurve(m_air_flow_position, 0);
 else if (Sender == PM_CopyToCurve1)  CopyCurve(m_air_flow_position, 1);
 else if (Sender == PM_CopyToCurve2)  CopyCurve(m_air_flow_position, 2);
 else if (Sender == PM_CopyToCurve3)  CopyCurve(m_air_flow_position, 3);
 else if (Sender == PM_CopyToCurve4)  CopyCurve(m_air_flow_position, 4);
 else if (Sender == PM_CopyToCurve5)  CopyCurve(m_air_flow_position, 5);
 else if (Sender == PM_CopyToCurve6)  CopyCurve(m_air_flow_position, 6);
 else if (Sender == PM_CopyToCurve7)  CopyCurve(m_air_flow_position, 7);
 else if (Sender == PM_CopyToCurve8)  CopyCurve(m_air_flow_position, 8);
 else if (Sender == PM_CopyToCurve9)  CopyCurve(m_air_flow_position, 9);
 else if (Sender == PM_CopyToCurve10) CopyCurve(m_air_flow_position, 10);
 else if (Sender == PM_CopyToCurve11) CopyCurve(m_air_flow_position, 11);
 else if (Sender == PM_CopyToCurve12) CopyCurve(m_air_flow_position, 12);
 else if (Sender == PM_CopyToCurve13) CopyCurve(m_air_flow_position, 13);
 else if (Sender == PM_CopyToCurve14) CopyCurve(m_air_flow_position, 14);
 else if (Sender == PM_CopyToCurve15) CopyCurve(m_air_flow_position, 15);
 else return;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnCopyFromCurve(TObject *Sender)
{
 if (Sender == PM_CopyFromCurve0)       CopyCurve(0, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve1)  CopyCurve(1, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve2)  CopyCurve(2, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve3)  CopyCurve(3, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve4)  CopyCurve(4, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve5)  CopyCurve(5, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve6)  CopyCurve(6, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve7)  CopyCurve(7, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve8)  CopyCurve(8, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve9)  CopyCurve(9, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve10) CopyCurve(10, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve11) CopyCurve(11, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve12) CopyCurve(12, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve13) CopyCurve(13, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve14) CopyCurve(14, m_air_flow_position);
 else if (Sender == PM_CopyFromCurve15) CopyCurve(15, m_air_flow_position);
 else return;
}

//---------------------------------------------------------------------------
