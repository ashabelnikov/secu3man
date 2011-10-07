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

#ifndef _FORM2D_H_
#define _FORM2D_H_
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>

typedef void (__cdecl *EventHandler)(void* i_param);
typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, void* i_param);
typedef void (__cdecl *OnWndActivation)(void* i_param, long cmd);

//---------------------------------------------------------------------------
class TForm2D : public TForm
{
 __published:  // IDE-managed Components
  TChart *Chart1;
  TLineSeries *Series1;
  TLineSeries *Series2;
  TBitBtn *ButtonAngleUp;
  TBitBtn *ButtonAngleDown;
  TButton *Smoothing3x;
  TButton *Smoothing5x;
  TPopupMenu *PopupMenu;
  TMenuItem  *PM_ZeroAllPoints;
  TMenuItem  *PM_Dup1stPoint;
  TMenuItem  *PM_BldCurveUsing1stAndLastPoints;
  void __fastcall Chart1ClickSeries(TCustomChart *Sender,
  TChartSeries *Series, int ValueIndex, TMouseButton Button,
  TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseUp(TObject *Sender, TMouseButton Button,
  TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseMove(TObject *Sender, TShiftState Shift,
    int X, int Y);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ButtonAngleUpClick(TObject *Sender);
  void __fastcall ButtonAngleDownClick(TObject *Sender);
  void __fastcall Smoothing3xClick(TObject *Sender);
  void __fastcall Smoothing5xClick(TObject *Sender);
  void __fastcall Chart1GetAxisLabel(TChartAxis *Sender,
    TChartSeries *Series, int ValueIndex, AnsiString &LabelText);
  void __fastcall OnZeroAllPoints(TObject *Sender);
  void __fastcall OnDuplicate1stPoint(TObject *Sender);
  void __fastcall OnBldCurveUsing1stAndLastPoints(TObject *Sender);

 public:  // User declarations
  __fastcall TForm2D(TComponent* Owner);
  void DataPrepare();

  void SetOnChange(EventHandler i_pOnChange,void* i_param);
  void SetOnClose(EventHandler i_pOnClose,void* i_param);
  void SetOnGetYAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
  void SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param);

  void Enable(bool enable);
  void InitPopupMenu(HINSTANCE hInstance);

 public: //properties
  int   count_of_function_points;
  float aai_min;
  float aai_max;
  float *original_function;
  float *modified_function;
  float horizontal_axis_grid_values[1024];
  AnsiString  horizontal_axis_values_format;
  AnsiString  chart_title_text;
  AnsiString  x_axis_title;
  AnsiString  y_axis_title;

 private:
  void RestrictAndSetValue(int index, double v);
  void ShiftFunction(float i_value);
  virtual void __fastcall WndProc(Messages::TMessage &Message);

 private:  // User declarations
  //адрес функции которая будет вызываться после изменения данных
  EventHandler m_pOnChange;
  void* m_param_on_change;

  //адрес функции которая будет вызываться во время закрытия окна
  EventHandler m_pOnClose;
  void* m_param_on_close;

  //адпес функции которая будет вызываться при рисовании надписей на оси Y
  OnGetAxisLabel m_pOnGetYAxisLabel;
  void* m_param_on_get_axis_label;

  //адрес функции которая будет вызываться при свертывании/развертывании окна
  OnWndActivation m_pOnWndActivation;
  void* m_param_on_wnd_activation;

  bool setval;
  int  val_n;
};
#endif //_FORM2D_H_

