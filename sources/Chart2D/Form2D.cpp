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
#include <stdio.h>
#include <tchar.h>
#include <algorithm>
#include "../common/MathHelpers.h"
#include "resource.h"
#pragma hdrstop

#include "Form2D.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

bool RemoveInstanceByHWND(HWND hWnd);
//---------------------------------------------------------------------------
__fastcall TForm2D::TForm2D(TComponent* Owner)
: TForm(Owner)
, m_count_of_function_points(0)
, m_aai_min(0.0f), m_aai_max(0.0f)
, m_original_function(NULL)
, m_modified_function(NULL)
, m_horizontal_axis_values_format("%.00f")  //integer
, m_chart_title_text("")
, m_x_axis_title("")
, m_y_axis_title("")
, m_pOnChange(NULL)
, m_pOnClose(NULL)
, m_pOnGetYAxisLabel(NULL)
, m_pOnGetXAxisLabel(NULL)
, m_pOnWndActivation(NULL)
, m_pOnChangeXEditValue(NULL)
, m_param_on_change(NULL)
, m_param_on_close(NULL)
, m_param_on_get_y_axis_label(NULL)
, m_param_on_get_x_axis_label(NULL)
, m_param_on_wnd_activation(NULL)
, m_param_on_change_xedit_value(NULL)
, m_setval(0)
, m_val_n(0)
, m_horizontal_axis_grid_mode(0)
{
 memset(&m_horizontal_axis_grid_values[0][0], 0, sizeof(float) * 1024);
 memset(&m_horizontal_axis_grid_values[1][0], 0, sizeof(float) * 1024);
 m_selpts.push_back(0);
}

//---------------------------------------------------------------------------
void TForm2D::DataPrepare()
{
 m_setval = 0;
 m_val_n  = 0;
 AnsiString as;

 //диапазон значений шкалы графика должен быть немного шире...
 Chart1->LeftAxis->Maximum = m_aai_max;
 Chart1->LeftAxis->Minimum = m_aai_min;

 Chart1->Title->Text->Clear();
 Chart1->Title->Text->Add(m_chart_title_text);
 Chart1->LeftAxis->Title->Caption = m_y_axis_title;
 Chart1->BottomAxis->Title->Caption = m_x_axis_title;

 for(int i = 0; i < m_count_of_function_points; i++)
 {
  as.sprintf(m_horizontal_axis_values_format.c_str(), m_horizontal_axis_grid_values[m_horizontal_axis_grid_mode][i]);
  Series1->Add(m_original_function[i], as, clAqua);
  Series2->Add(m_modified_function[i], as, clRed);
 }
}

//---------------------------------------------------------------------------
void TForm2D::SetOnChange(EventHandler i_pOnChange, void* i_param)
{
 m_pOnChange = i_pOnChange;
 m_param_on_change = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::SetOnClose(EventHandler i_pOnClose, void* i_param)
{
 m_pOnClose = i_pOnClose;
 m_param_on_close = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::SetOnGetYAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param)
{
 m_pOnGetYAxisLabel = i_pOnGetAxisLabel;
 m_param_on_get_y_axis_label = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::SetOnGetXAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param)
{
 m_pOnGetXAxisLabel = i_pOnGetAxisLabel;
 m_param_on_get_x_axis_label = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param)
{
 m_pOnWndActivation = i_pOnWndActivation;
 m_param_on_wnd_activation = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::ShowXEdits(bool i_show)
{
 m_horizontal_axis_grid_mode = i_show;
 EditXBegin->Visible = i_show;
 EditXEnd->Visible = i_show;
 SpinXBegin->Visible = i_show;
 SpinXEnd->Visible = i_show;
}

//---------------------------------------------------------------------------
void TForm2D::CfgXEdits(int i_type, float i_min, float i_max, float i_step)
{
 switch(i_type)
 {
  case 0:  //begin
   SpinXBegin->FloatMin = i_min;
   SpinXBegin->FloatMax = i_max;
   SpinXBegin->FloatIncrement = i_step;
   break;
  case 1:  //end
   SpinXEnd->FloatMin = i_min;
   SpinXEnd->FloatMax = i_max;
   SpinXEnd->FloatIncrement = i_step;
   break;
 }
}

//---------------------------------------------------------------------------
void TForm2D::SetXEditsCB(OnChangeValue i_pOnChangeValue, void* i_param)
{
 m_pOnChangeXEditValue = i_pOnChangeValue;
 m_param_on_change_xedit_value = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::SetXEditVal(int i_type, float i_value)
{
 AnsiString as;
 as.sprintf(m_horizontal_axis_values_format.c_str(), i_value);
 switch(i_type)
 {
  case 0:  //begin
   EditXBegin->Text = as;
   break;
  case 1:  //end
   EditXEnd->Text = as;
   break;
 }
}

//---------------------------------------------------------------------------
void TForm2D::Enable(bool i_enable)
{
 Series1->Active = i_enable;
 Series2->Active = i_enable;
 Smoothing3x->Enabled = i_enable;
 Smoothing5x->Enabled = i_enable;
 ButtonAngleUp->Enabled = i_enable;
 ButtonAngleDown->Enabled = i_enable;
 Chart1->Enabled = i_enable;
}

//---------------------------------------------------------------------------
void TForm2D::InitPopupMenu(HINSTANCE hInstance)
{
 char string[1024 + 1];
 ::LoadString(hInstance, IDS_PM_ZERO_ALL_POINTS, string, 1024);
 PM_ZeroAllPoints->Caption = string;
 ::LoadString(hInstance, IDS_PM_SET_ALLTO_1ST_PT, string, 1024);
 PM_Dup1stPoint->Caption = string;
 ::LoadString(hInstance, IDS_PM_BLD_CURVE_1ST_LAST_PT, string, 1024);
 PM_BldCurveUsing1stAndLastPoints->Caption = string;
}

//---------------------------------------------------------------------------
void TForm2D::InitHints(HINSTANCE hInstance)
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
 ::LoadString(hInstance, IDS_TT_H_AXIS_BEGIN_VAL, string, 1024);
 EditXBegin->Hint = string;
 SpinXBegin->Hint = string;
 ::LoadString(hInstance, IDS_TT_H_AXIS_END_VAL, string, 1024);
 EditXEnd->Hint = string;
 SpinXEnd->Hint = string; 
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Chart1ClickSeries(TCustomChart *Sender,
      TChartSeries *Series, int ValueIndex, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 if (Series==Series2)
 {
  //if Ctrl key is not pressed than select single point
  if (!Shift.Contains(ssCtrl))
  {
   MarkPoints(false);
   m_selpts.clear();
   m_selpts.push_back(ValueIndex);
   MarkPoints(true);
  }

  if (Button==mbLeft) //левая кнопка мышки?
  {
   m_setval  = 1;
   m_val_n = ValueIndex;
  }
 }
 m_prev_pt = std::make_pair(X, Y);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Chart1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 //implementation of multiple points selection by using Ctrl key
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
void __fastcall TForm2D::Chart1MouseMove(TObject *Sender, TShiftState Shift,
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
  v = Series2->YScreenToValue(Y);
  ShiftPoints(v - Series2->YValue[m_val_n]);
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::FormClose(TObject *Sender, TCloseAction &Action)
{
 if (m_pOnClose)
  m_pOnClose(m_param_on_close);
 m_setval  = 0;
 m_val_n   = 0;
 m_count_of_function_points = 0;
 m_chart_title_text = "";
 m_original_function = NULL;
 m_modified_function = NULL;
 RemoveInstanceByHWND(Handle);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::ButtonAngleUpClick(TObject *Sender)
{
 ShiftFunction(Chart1->LeftAxis->Inverted ? -0.5 : 0.5);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::ButtonAngleDownClick(TObject *Sender)
{
 ShiftFunction(Chart1->LeftAxis->Inverted ? 0.5 : -0.5);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Smoothing3xClick(TObject *Sender)
{
 float* p_source_function = new float[m_count_of_function_points];
 std::copy(m_modified_function, m_modified_function + m_count_of_function_points, p_source_function);
 MathHelpers::Smooth1D(p_source_function, m_modified_function, m_count_of_function_points, 3);
 delete[] p_source_function;

 for (int i = 0; i < m_count_of_function_points; i++ )
  Series2->YValue[i] = m_modified_function[i];
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Smoothing5xClick(TObject *Sender)
{
 float* p_source_function = new float[m_count_of_function_points];
 std::copy(m_modified_function, m_modified_function + m_count_of_function_points, p_source_function);
 MathHelpers::Smooth1D(p_source_function, m_modified_function, m_count_of_function_points, 5);
 delete[] p_source_function;

 for (int i = 0; i < m_count_of_function_points; i++ )
  Series2->YValue[i] = m_modified_function[i];
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Chart1GetAxisLabel(TChartAxis *Sender,
      TChartSeries *Series, int ValueIndex, AnsiString &LabelText)
{
 if (Sender == Chart1->LeftAxis)
 { //Y
  if (m_pOnGetYAxisLabel)
  {
   TCHAR string[64];
   _tcscpy(string, LabelText.c_str());
   m_pOnGetYAxisLabel(string, m_param_on_get_y_axis_label);
   LabelText = string;
  }
 }
 else if (Sender == Chart1->BottomAxis)
 { //X
  if (0==m_horizontal_axis_grid_mode)
  {
   if (m_pOnGetXAxisLabel)
   {
    TCHAR string[64];
    _tcscpy(string, LabelText.c_str());
    m_pOnGetXAxisLabel(string, m_param_on_get_x_axis_label);
    LabelText = string;
   }
  }
  else if (1==m_horizontal_axis_grid_mode)
  {
   AnsiString as;
   as.sprintf(m_horizontal_axis_values_format.c_str(), m_horizontal_axis_grid_values[m_horizontal_axis_grid_mode][ValueIndex]);
   LabelText = as;
  }
 }
}

//---------------------------------------------------------------------------
void TForm2D::RestrictAndSetValue(int index, double v)
{
 if (v > m_aai_max)
  v = m_aai_max;
 if (v < m_aai_min)
  v = m_aai_min;
 m_modified_function[index] = v;
 Series2->YValue[index] = v;
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::ShiftFunction(float i_value)
{
 for (int i = 0; i < m_count_of_function_points; ++i )
 {
  RestrictAndSetValue(i, Series2->YValue[i] + i_value);
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::ShiftPoints(float i_value)
{
 for(size_t i = 0; i < m_selpts.size(); ++i)
  RestrictAndSetValue(m_selpts[i], Series2->YValue[m_selpts[i]] + i_value);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::MarkPoints(bool i_mark)
{
 for(size_t i = 0; i < m_selpts.size(); ++i)
  Series2->ValueColor[m_selpts[i]] = i_mark ? clNavy : clRed; 
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::WndProc(Messages::TMessage &Message)
{
 TForm::WndProc(Message);

 if (Message.Msg == WM_SYSCOMMAND && m_pOnWndActivation)
  m_pOnWndActivation(m_param_on_wnd_activation, Message.WParam);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::OnZeroAllPoints(TObject *Sender)
{
 for (int i = 0; i < m_count_of_function_points; ++i )
  RestrictAndSetValue(i, 0);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::OnDuplicate1stPoint(TObject *Sender)
{
 for (int i = 0; i < m_count_of_function_points; i++ )
  RestrictAndSetValue(i, Series2->YValue[0]);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::OnBldCurveUsing1stAndLastPoints(TObject *Sender)
{
 double firstPtVal = Series2->YValue[0];
 double lastPtVal = Series2->YValue[m_count_of_function_points - 1];
 double intrmPtCount = m_count_of_function_points - 1;
 for (int i = 1; i < m_count_of_function_points - 1; i++ )
  RestrictAndSetValue(i, firstPtVal + (((lastPtVal-firstPtVal) / intrmPtCount) * i));
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::EditXBeginOnChange(TObject *Sender)
{
 double bValue = 0, eValue = 0;
 if (1!=sscanf(EditXBegin->Text.c_str(), "%lf", &bValue))
  return;
 if (1!=sscanf(EditXEnd->Text.c_str(), "%lf", &eValue))
  return;
   
 double step = (eValue - bValue) / ((double)m_count_of_function_points - 1);
 for(int i = 0; i < m_count_of_function_points; ++i)
  m_horizontal_axis_grid_values[m_horizontal_axis_grid_mode][i] = bValue + (step * i);
 Chart1->Invalidate();

 if (m_pOnChangeXEditValue)
  m_pOnChangeXEditValue(m_param_on_change_xedit_value, 0, bValue);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::EditXEndOnChange(TObject *Sender)
{
 double bValue = 0, eValue = 0;
 if (1!=sscanf(EditXBegin->Text.c_str(), "%lf", &bValue))
  return;
 if (1!=sscanf(EditXEnd->Text.c_str(), "%lf", &eValue))
  return;

 double step = (eValue - bValue) / ((double)m_count_of_function_points - 1);
 for(int i = 0; i < m_count_of_function_points; ++i)
  m_horizontal_axis_grid_values[m_horizontal_axis_grid_mode][i] = bValue + (step * i);
 Chart1->Invalidate();

 if (m_pOnChangeXEditValue)
  m_pOnChangeXEditValue(m_param_on_change_xedit_value, 1, eValue);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::SelLeftArrow(bool i_shift)
{
 if (i_shift)
 {//Shift key is pressed
  MarkPoints(false);
  if (m_selpts.front() != m_val_n)
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
void __fastcall TForm2D::SelRightArrow(bool i_shift)
{
 if (i_shift)
 { //Shift key is pressed
  MarkPoints(false);
  if (m_selpts.back() != m_val_n)
  {
   m_selpts.pop_front();
   m_val_n = m_selpts.front();
  }
  else
  {
   m_val_n = m_selpts.back() + 1;
   if (m_val_n < m_count_of_function_points)
    m_selpts.push_back(m_val_n);
   else
    m_val_n = m_count_of_function_points-1;
  }
  MarkPoints(true);
 }
 else
 { //Without shift
  MarkPoints(false);
  int next_pt = m_selpts.size() ? m_selpts.back() + 1 : 0;
  if (next_pt >= m_count_of_function_points)
   next_pt = m_count_of_function_points-1;
  m_selpts.clear();
  m_selpts.push_back(next_pt);
  MarkPoints(true);
  m_val_n = next_pt;
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::CtrlKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
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
 }

 //Following code simulates normal dialog messages
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
void __fastcall TForm2D::OnEnterChart(TObject* Sender)
{
 MarkPoints(true);
 Chart1->Title->Font->Style = TFontStyles() << fsBold;
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::OnExitChart(TObject* Sender)
{
 MarkPoints(false);
 Chart1->Title->Font->Style = TFontStyles();
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::OnChartMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if (ActiveControl != Chart1)
  ActiveControl = Chart1;
}

//---------------------------------------------------------------------------
