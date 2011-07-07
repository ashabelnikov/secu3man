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
#pragma hdrstop

#include "Form2D.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

bool RemoveInstanceByHWND(HWND hWnd);
//---------------------------------------------------------------------------
__fastcall TForm2D::TForm2D(TComponent* Owner)
: TForm(Owner)
, count_of_function_points(0)
, aai_min(0.0f), aai_max(0.0f)
, original_function(NULL)
, modified_function(NULL)
, horizontal_axis_values_format("%.00f")  //integer
, chart_title_text("")
, x_axis_title("")
, y_axis_title("")
, m_pOnChange(NULL)
, m_pOnClose(NULL)
, m_pOnGetYAxisLabel(NULL)
, m_pOnWndActivation(NULL)
, m_param_on_change(NULL)
, m_param_on_close(NULL)
, m_param_on_get_axis_label(NULL)
, m_param_on_wnd_activation(NULL)
, setval(0)
, val_n(0)
{
 //empty
}

//---------------------------------------------------------------------------
void TForm2D::DataPrepare()
{
 setval = 0;
 val_n  = 0;
 AnsiString as;

 //диапазон значений шкалы графика должен быть немного шире...
 Chart1->LeftAxis->Maximum = aai_max;
 Chart1->LeftAxis->Minimum = aai_min;

 Chart1->Title->Text->Clear();
 Chart1->Title->Text->Add(chart_title_text);
 Chart1->LeftAxis->Title->Caption = y_axis_title;
 Chart1->BottomAxis->Title->Caption = x_axis_title;

 for(int i = 0; i < count_of_function_points; i++)
 {
  as.sprintf(horizontal_axis_values_format.c_str(), horizontal_axis_grid_values[i]);
  Series1->Add(original_function[i], as, clAqua);
  Series2->Add(modified_function[i], as, clRed);
 }
}

//---------------------------------------------------------------------------
void TForm2D::SetOnChange(EventHandler i_pOnChange,void* i_param)
{
 m_pOnChange = i_pOnChange;
 m_param_on_change = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::SetOnClose(EventHandler i_pOnClose,void* i_param)
{
 m_pOnClose = i_pOnClose;
 m_param_on_close = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::SetOnGetYAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param)
{
 m_pOnGetYAxisLabel = i_pOnGetAxisLabel;
 m_param_on_get_axis_label = i_param;
}

void TForm2D::SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param)
{
 m_pOnWndActivation = i_pOnWndActivation;
 m_param_on_wnd_activation = i_param;
}

//---------------------------------------------------------------------------
void TForm2D::Enable(bool enable)
{
 Series1->Active = enable;
 Series2->Active = enable;
 Smoothing3x->Enabled = enable;
 Smoothing5x->Enabled = enable;
 ButtonAngleUp->Enabled = enable;
 ButtonAngleDown->Enabled = enable;
 Chart1->Enabled = enable;
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Chart1ClickSeries(TCustomChart *Sender,
      TChartSeries *Series, int ValueIndex, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 if (Series==Series2)
 {
  if (Button==mbLeft) //левая кнопка мышки?
  {
   setval  = 1;
   val_n = ValueIndex;
  }
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Chart1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 if ((m_pOnChange) && (setval))
  m_pOnChange(m_param_on_change);
 setval = 0;
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Chart1MouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
 double v;
 if (setval)
 {
  v = Series2->YScreenToValue(Y);
  RestrictAndSetValue(val_n, v);
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::FormClose(TObject *Sender, TCloseAction &Action)
{
 if (m_pOnClose)
  m_pOnClose(m_param_on_close);
 setval  = 0;
 val_n   = 0;
 count_of_function_points = 0;
 chart_title_text = "";
 original_function = NULL;
 modified_function = NULL;
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
 float* p_source_function = new float[count_of_function_points];
 std::copy(modified_function, modified_function + count_of_function_points, p_source_function);
 MathHelpers::Smooth1D(p_source_function, modified_function, count_of_function_points, 3);
 delete[] p_source_function;

 for (int i = 0; i < count_of_function_points; i++ )
  Series2->YValue[i] = modified_function[i];
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Smoothing5xClick(TObject *Sender)
{
 float* p_source_function = new float[count_of_function_points];
 std::copy(modified_function, modified_function + count_of_function_points, p_source_function);
 MathHelpers::Smooth1D(p_source_function, modified_function, count_of_function_points, 5);
 delete[] p_source_function;

 for (int i = 0; i < count_of_function_points; i++ )
  Series2->YValue[i] = modified_function[i];
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::Chart1GetAxisLabel(TChartAxis *Sender,
      TChartSeries *Series, int ValueIndex, AnsiString &LabelText)
{
 if (Sender == Chart1->LeftAxis)
 {
  if (m_pOnGetYAxisLabel)
  {
   TCHAR string[64];
   _tcscpy(string, LabelText.c_str());
   m_pOnGetYAxisLabel(string, m_param_on_get_axis_label);
   LabelText = string;
  }
 }
}

//---------------------------------------------------------------------------
void TForm2D::RestrictAndSetValue(int index, double v)
{
 if (v > aai_max)
  v = aai_max;
 if (v < aai_min)
  v = aai_min;
 modified_function[index] = v;
 Series2->YValue[index] = v;
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::ShiftFunction(float i_value)
{
 for (int i = 0; i < count_of_function_points; i++ )
 {
  RestrictAndSetValue(i, Series2->YValue[i] + i_value);
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm2D::WndProc(Messages::TMessage &Message)
{
 TForm::WndProc(Message);

 if (Message.Msg == WM_SYSCOMMAND && m_pOnWndActivation)
  m_pOnWndActivation(m_param_on_wnd_activation, Message.WParam);
}

//---------------------------------------------------------------------------

