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

/** \file Form2D.cpp
 * \author Alexey A. Shabelnikov
 */

#include <vcl.h>
#include <stdio.h>
#include <tchar.h>
#include <algorithm>
#pragma hdrstop

#include "resource.h"
#include "Form2D.h"
#include "../common/MathHelpers.h"
#include "../common/DPIAware.h"
#include "../PtMovStep/PtMovStepDlg.h"
#include "../ManageFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "Form2D.dfm"

//---------------------------------------------------------------------------
__fastcall TForm2D::TForm2D(HWND parent)
: TForm(parent)
, m_count_of_function_points(0)
, m_fnc_min(0.0f), m_fnc_max(0.0f)
, mp_original_function(NULL)
, mp_modified_function(NULL)
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
, m_horizontal_axis_grid_mode(0) //0 mode is default
, m_pt_moving_step(0.5f)
, m_visibleMarkIdx(-1)
{
 memset(m_horizontal_axis_grid_values, 0, sizeof(float) * 256);
 m_selpts.push_back(0);
 memset(m_binsEdit, NULL, sizeof(NULL) * 8);
 memset(m_binsUpDown, NULL, sizeof(NULL) * 8);
}

//---------------------------------------------------------------------------
void TForm2D::DataPrepare()
{
 m_setval = 0;
 m_val_n  = 0;
 AnsiString as;

 //range of grid values must be slightly wider...
 Chart1->LeftAxis->Maximum = m_fnc_max;
 Chart1->LeftAxis->Minimum = m_fnc_min;

 Chart1->LeftAxis->LabelsFont->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 Chart1->LeftAxis->Title->Font->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 Chart1->BottomAxis->LabelsFont->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 Chart1->BottomAxis->Title->Font->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));

 Chart1->Frame->Color = TColor(~GetSysColor(COLOR_BTNFACE)&0xFFFFFF);
 Chart1->LeftAxis->Axis->Color = TColor(~GetSysColor(COLOR_BTNFACE)&0xFFFFFF);
 Chart1->BottomAxis->Axis->Color = TColor(~GetSysColor(COLOR_BTNFACE)&0xFFFFFF);

 Chart1->Title->Text->Clear();
 Chart1->Title->Text->Add(m_chart_title_text);
 Chart1->LeftAxis->Title->Caption = m_y_axis_title;
 Chart1->BottomAxis->Title->Caption = m_x_axis_title;

 for(int i = 0; i < m_count_of_function_points; i++)
 {
  if (m_horizontal_axis_grid_mode < 2) //0,1 modes
   as.sprintf(m_horizontal_axis_values_format.c_str(), m_horizontal_axis_grid_values[i]);
  else  //mode 2
   as.sprintf(m_horizontal_axis_values_format.c_str(), mp_modified_function[i+m_count_of_function_points]);
   
  Series1->Add(mp_original_function[i], as, clAqua);
  Series2->Add(mp_modified_function[i], as, clRed);
 }

 Series1->Marks->Font->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));
 Series2->Marks->Font->Color = TColor(GetSysColor(COLOR_WINDOWTEXT));

 if (2==m_horizontal_axis_grid_mode)
  UpdateBins();
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
void TForm2D::CfgXEdits(int i_type, float i_min, float i_max, float i_step, int limitText, int spinDecimalPlaces)
{
 switch(i_type)
 {
  case 0:  //begin
   SpinXBegin->FloatMin = i_min;
   SpinXBegin->FloatMax = i_max;
   SpinXBegin->FloatIncrement = i_step;
   if (-1!=spinDecimalPlaces)
    SpinXBegin->DecimalPlaces = spinDecimalPlaces;
   if (-1!=limitText)
    EditXBegin->MaxLength = limitText;
   break;
  case 1:  //end
   SpinXEnd->FloatMin = i_min;
   SpinXEnd->FloatMax = i_max;
   SpinXEnd->FloatIncrement = i_step;
   if (-1!=spinDecimalPlaces)
    SpinXEnd->DecimalPlaces = spinDecimalPlaces;
   if (-1!=limitText)
    EditXEnd->MaxLength = limitText;
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
 if (m_horizontal_axis_grid_mode == 2) //only in bins mode
 {
  for(int i = 0; i < 8; ++i)
  {
   if (m_binsEdit[i])
    m_binsEdit[i]->Enabled = i_enable;
   if (m_binsUpDown[i])
    m_binsUpDown[i]->Enabled = i_enable;
  }
  ButtonShowBins->Enabled = i_enable;
 }
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
 ::LoadString(hInstance, IDS_PM_SET_PTMOV_STEP, string, 1024);
 PM_SetPtMovStep->Caption = string;
 ::LoadString(hInstance, IDS_PM_HIDE_MARKS, string, 1024);
 PM_HideMarks->Caption = string;
 ::LoadString(hInstance, IDS_PM_HIDE_OLDCURVE, string, 1024);
 PM_HideOldCurve->Caption = string;
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
void TForm2D::SetPtValuesFormat(LPCTSTR ptValFormat)
{
 for (int i = 0; i < 2; i++ )
  Chart1->Series[i]->ValueFormat = ptValFormat; //format for point values
}

//---------------------------------------------------------------------------
void TForm2D::InitBins(void)
{
 ButtonShowBins->Visible = true;
 m_horizontal_axis_grid_mode = 2; //mode 2

 if (m_count_of_function_points > 8)
  ::MessageBox(NULL, "You can not use more than 8 function points in this mode", "Error", MB_OK);

 m_binsEdit[0] = Edit1; m_binsUpDown[0] = UpDown1;
 m_binsEdit[1] = Edit2; m_binsUpDown[1] = UpDown2;
 m_binsEdit[2] = Edit3; m_binsUpDown[2] = UpDown3;
 m_binsEdit[3] = Edit4; m_binsUpDown[3] = UpDown4;
 m_binsEdit[4] = Edit5; m_binsUpDown[4] = UpDown5;
 m_binsEdit[5] = Edit6; m_binsUpDown[5] = UpDown6;
 m_binsEdit[6] = Edit7; m_binsUpDown[6] = UpDown7;
 m_binsEdit[7] = Edit8; m_binsUpDown[7] = UpDown8;
 
 //Set position of edit boxes
 UpdateBinsPosition();

 //hide unnecessary edit boxes
 for(int i = m_count_of_function_points; i < 8; ++i)
 { m_binsEdit[i]->Visible = false; m_binsUpDown[i]->Visible = false; }

 //Set limits and values
 UpdateBins();
}

void __fastcall TForm2D::UpdateBins(void)
{
 //Set limits and values
 for(int i = 0; i < m_count_of_function_points; ++i)
 {
  m_binsUpDown[i]->DecimalPlaces = (int)m_horizontal_axis_grid_values[3];
  m_binsUpDown[i]->FloatMin = m_horizontal_axis_grid_values[0];
  m_binsUpDown[i]->FloatMax = m_horizontal_axis_grid_values[1];
  m_binsUpDown[i]->FloatPosition = mp_modified_function[i+m_count_of_function_points];
  m_binsUpDown[i]->FloatIncrement = m_horizontal_axis_grid_values[2];
 }
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
 //===========================================
 if (m_visibleMarkIdx != -1)
 {
  if (m_setval)
  {
   m_visibleMarkIdx = m_val_n;
   Chart1->Invalidate();
  }
  else
  {
   int idx = Series2->GetCursorValueIndex();
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
 mp_original_function = NULL;
 mp_modified_function = NULL;
 RemoveInstanceByHWND(Handle);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::ButtonAngleUpClick(TObject *Sender)
{
 ShiftFunction(Chart1->LeftAxis->Inverted ? -m_pt_moving_step : m_pt_moving_step);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::ButtonAngleDownClick(TObject *Sender)
{
 ShiftFunction(Chart1->LeftAxis->Inverted ? m_pt_moving_step : -m_pt_moving_step);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Smoothing3xClick(TObject *Sender)
{
 float* p_source_function = new float[m_count_of_function_points];
 std::copy(mp_modified_function, mp_modified_function + m_count_of_function_points, p_source_function);
 MathHelpers::Smooth1D(p_source_function, mp_modified_function, m_count_of_function_points, 3);
 delete[] p_source_function;

 for (int i = 0; i < m_count_of_function_points; i++ )
  Series2->YValue[i] = mp_modified_function[i];
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Smoothing5xClick(TObject *Sender)
{
 float* p_source_function = new float[m_count_of_function_points];
 std::copy(mp_modified_function, mp_modified_function + m_count_of_function_points, p_source_function);
 MathHelpers::Smooth1D(p_source_function, mp_modified_function, m_count_of_function_points, 5);
 delete[] p_source_function;

 for (int i = 0; i < m_count_of_function_points; i++ )
  Series2->YValue[i] = mp_modified_function[i];
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::ButtonShowBinsClick(TObject *Sender)
{
 PanelBins->Visible = PanelBins->Visible ? 0 : 1;
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
   m_pOnGetYAxisLabel(string, ValueIndex, m_param_on_get_y_axis_label);
   LabelText = string;
  }
 }
 else if (Sender == Chart1->BottomAxis)
 { //X
  if (0==m_horizontal_axis_grid_mode) //default slots or custom labels
  {
   if (m_pOnGetXAxisLabel)
   { //custom labels
    TCHAR string[64];
    _tcscpy(string, LabelText.c_str());
    m_pOnGetXAxisLabel(string, ValueIndex, m_param_on_get_x_axis_label);
    LabelText = string;
   }
  }
  else if (1==m_horizontal_axis_grid_mode)  //begin & end bins
  {
   AnsiString as;
   as.sprintf(m_horizontal_axis_values_format.c_str(), m_horizontal_axis_grid_values[ValueIndex]);
   LabelText = as;
  }
  else if (2==m_horizontal_axis_grid_mode)  //separate bins mode
  {
   AnsiString as;
   as.sprintf(m_horizontal_axis_values_format.c_str(), mp_modified_function[ValueIndex + m_count_of_function_points]);
   LabelText = as;
  }
 }
}

//---------------------------------------------------------------------------
void TForm2D::RestrictAndSetValue(int index, double v)
{
 v = MathHelpers::RestrictValue<double>(v, m_fnc_min, m_fnc_max);
 mp_modified_function[index] = v;
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
  m_horizontal_axis_grid_values[i] = bValue + (step * i);
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
  m_horizontal_axis_grid_values[i] = bValue + (step * i);
 Chart1->Invalidate();

 if (m_pOnChangeXEditValue)
  m_pOnChangeXEditValue(m_param_on_change_xedit_value, 1, eValue);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::BinsEditOnChange(TObject *Sender)
{
 double Value = 0;
 if (1!=sscanf(((TEdit*)Sender)->Text.c_str(), "%lf", &Value))
  return;

 for(int i = 0; i < m_count_of_function_points; ++i)
 {
  if (m_binsEdit[i]==Sender)
  {
   //Check changed item for errors
   if (true==CheckBinForErrors(i, Value))
    m_errors.push_back(i);
   mp_modified_function[i+m_count_of_function_points] = Value;
   break;
  }
 }
 
 //Recheck all present errors and remove items which are OK
 std::vector<int>::iterator it = m_errors.begin();  
 while(it!=m_errors.end())
 {
  if (false==CheckBinForErrors(*it, mp_modified_function[(*it)+m_count_of_function_points]))
  {//OK
   m_binsEdit[*it]->Brush->Color = clWindow; //return background color to default
   m_binsEdit[*it]->Invalidate();
   it = m_errors.erase(it);
  }
  else //next error
   ++it;
 } 

 //Update view
 for(it = m_errors.begin(); it != m_errors.end(); ++it)
 {
  m_binsEdit[*it]->Brush->Color = clRed;
  m_binsEdit[*it]->Invalidate();
 }

 Chart1->Invalidate();  //See Chart1GetAxisLabel()

 if (m_pOnChange && m_errors.empty())
  m_pOnChange(m_param_on_change);
}

bool __fastcall TForm2D::CheckBinForErrors(int itemIndex, float value)
{   
 if (value < m_horizontal_axis_grid_values[0]) //compare with min
  return true;
 else if (value > m_horizontal_axis_grid_values[1]) //compare with max
  return true;
 else if (((itemIndex < m_count_of_function_points-1) ? (m_horizontal_axis_grid_values[4] > fabs(value - (mp_modified_function[(itemIndex+1)+m_count_of_function_points]))) : 0) ||
          ((itemIndex >  0) ? (m_horizontal_axis_grid_values[4] > fabs(value - (mp_modified_function[(itemIndex-1)+m_count_of_function_points]))) : 0))
  return true;
 else if (((itemIndex < m_count_of_function_points-1) ? (value > (mp_modified_function[(itemIndex+1)+m_count_of_function_points])) : 0) ||
          ((itemIndex >  0) ? (value < (mp_modified_function[(itemIndex-1)+m_count_of_function_points])) : 0))
  return true;
 else
  return false; //Ok
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
   ShiftPoints(Chart1->LeftAxis->Inverted ? -m_pt_moving_step : m_pt_moving_step);
   if (m_pOnChange)
    m_pOnChange(m_param_on_change);
  }
  else if (Key == VK_DOWN)
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

 OnEnterChart(NULL);

 if (!Active)
  SetFocus();
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::FormDeactivate(TObject *Sender)
{
 OnExitChart(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::OnSetPtMovStep(TObject *Sender)
{
 Application->CreateForm(__classid(TPtMovStepDlg), &PtMovStepDlg);
 PtMovStepDlg->SetValue(m_pt_moving_step);
 if (PtMovStepDlg->ShowModal()==mrOk)
  m_pt_moving_step = PtMovStepDlg->GetValue();
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::LineSeries2GetMarkText(TChartSeries *Sender, int ValueIndex, AnsiString &LabelText)
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
void __fastcall TForm2D::OnHideMarks(TObject *Sender)
{
 PM_HideMarks->Checked = PM_HideMarks->Checked ? false : true; //toggle check mark
 if (!PM_HideMarks->Checked)
  m_visibleMarkIdx=-1; //show all marks
 else
  m_visibleMarkIdx=-2; //show only one mark which is under mouse cursor
 Chart1->Invalidate();
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::OnHideOldCurve(TObject *Sender)
{
 PM_HideOldCurve->Checked = PM_HideOldCurve->Checked ? false : true; //toggle check mark
 Series1->Active = !PM_HideOldCurve->Checked;
 Chart1->Invalidate();
}

//---------------------------------------------------------------------------

void __fastcall TForm2D::FormResize(TObject *Sender)
{
 if ( ButtonShowBins->Visible == true)
  UpdateBinsPosition();
}

void __fastcall TForm2D::UpdateBinsPosition(void)
{
 //Set position of edit boxes
 float lr_space = 5;
 float horz_step = (float(PanelBins->Width)-(lr_space*2)-(Edit1->Width+UpDown1->Width)) /  float(m_count_of_function_points-1);
 for(int i = 0; i < m_count_of_function_points; ++i)
 {
  m_binsUpDown[i]->Visible = True;
  m_binsEdit[i]->Visible = True;
  m_binsEdit[i]->Left = lr_space + (i * horz_step);
  m_binsUpDown[i]->Left = m_binsEdit[i]->Left + m_binsEdit[i]->Width;
 }
}

void __fastcall TForm2D::FormCreate(TObject *Sender)
{
 DPIAware da;
 ChangeScale(96, da.GetDPIX());
}
