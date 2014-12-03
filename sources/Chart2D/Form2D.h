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

#ifndef _FORM2D_H_
#define _FORM2D_H_
//---------------------------------------------------------------------------
#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Series.hpp>
#include <StdCtrls.hpp>
#include <deque>
#include "TFloatUpDown.h"
#include "TChartEx.h"

typedef void (__cdecl *EventHandler)(void* i_param);
typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, int index, void* i_param);
typedef void (__cdecl *OnWndActivation)(void* i_param, long cmd);
typedef void (__cdecl *OnChangeValue)(void* i_param, int type, float value);

//---------------------------------------------------------------------------
class TForm2D : public TForm
{
 __published:  // IDE-managed Components
  TChartEx *Chart1;
  TLineSeries *Series1;
  TLineSeries *Series2;
  TBitBtn *ButtonAngleUp;
  TBitBtn *ButtonAngleDown;
  TButton *Smoothing3x;
  TButton *Smoothing5x;
  TPopupMenu *PopupMenu;
  TMenuItem *PM_ZeroAllPoints;
  TMenuItem *PM_Dup1stPoint;
  TMenuItem *PM_BldCurveUsing1stAndLastPoints;
  TEdit *EditXBegin;
  TEdit *EditX1;
  TEdit *EditX2;
  TEdit *EditXEnd;
  TFloatUpDown *SpinXBegin;
  TFloatUpDown *SpinX1;
  TFloatUpDown *SpinX2;
  TFloatUpDown *SpinXEnd;
  void __fastcall Chart1ClickSeries(TCustomChart *Sender, TChartSeries *Series, int ValueIndex, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ButtonAngleUpClick(TObject *Sender);
  void __fastcall ButtonAngleDownClick(TObject *Sender);
  void __fastcall Smoothing3xClick(TObject *Sender);
  void __fastcall Smoothing5xClick(TObject *Sender);
  void __fastcall Chart1GetAxisLabel(TChartAxis *Sender, TChartSeries *Series, int ValueIndex, AnsiString &LabelText);
  void __fastcall OnZeroAllPoints(TObject *Sender);
  void __fastcall OnDuplicate1stPoint(TObject *Sender);
  void __fastcall OnBldCurveUsing1stAndLastPoints(TObject *Sender);
  void __fastcall EditXBeginOnChange(TObject *Sender);
  void __fastcall EditXEndOnChange(TObject *Sender);
  void __fastcall CtrlKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall OnEnterChart(TObject* Sender);
  void __fastcall OnExitChart(TObject* Sender);
  void __fastcall OnChartMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

 public:  // User declarations
  __fastcall TForm2D(TComponent* Owner);
  void DataPrepare();

  void SetOnChange(EventHandler i_pOnChange,void* i_param);
  void SetOnClose(EventHandler i_pOnClose,void* i_param);
  void SetOnGetYAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
  void SetOnGetXAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
  void SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param);
  void ShowXEdits(bool i_show);
  void SetXEditsCB(OnChangeValue i_pOnChangeValue, void* i_param);
  void CfgXEdits(int i_type, float i_min, float i_max, float i_step);
  void SetXEditVal(int i_type, float i_value);

  void Enable(bool i_enable);
  void InitPopupMenu(HINSTANCE hInstance);
  void InitHints(HINSTANCE hInstance);
  void SetPtValuesFormat(LPCTSTR ptValFormat);

 public: //properties
  int m_count_of_function_points;
  float m_fnc_min;
  float m_fnc_max;
  const float *mp_original_function;
  float *mp_modified_function;
  float m_horizontal_axis_grid_values[2][1024];
  AnsiString m_horizontal_axis_values_format;
  AnsiString m_chart_title_text;
  AnsiString m_x_axis_title;
  AnsiString m_y_axis_title;
  float m_pt_moving_step;
  bool m_bins_mode;

 private:
  void RestrictAndSetValue(int index, double v);
  void __fastcall ShiftFunction(float i_value);
  void __fastcall ShiftPoints(float i_value);
  void __fastcall MarkPoints(bool i_mark);
  void __fastcall SelLeftArrow(bool i_shift);
  void __fastcall SelRightArrow(bool i_shift);
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
  void* m_param_on_get_y_axis_label;

  //адпес функции которая будет вызываться при рисовании надписей на оси X
  OnGetAxisLabel m_pOnGetXAxisLabel;
  void* m_param_on_get_x_axis_label;

  //адрес функции которая будет вызываться при свертывании/развертывании окна
  OnWndActivation m_pOnWndActivation;
  void* m_param_on_wnd_activation;

  //адрес функции которая будет вызываться при изменении соответствующего лимита по оси X (edit controls)
  OnChangeValue m_pOnChangeXEditValue;
  void* m_param_on_change_xedit_value;

  int m_horizontal_axis_grid_mode;

  bool m_setval;
  int  m_val_n;
  std::deque<int> m_selpts;
  std::pair<int, int> m_prev_pt;
};
#endif //_FORM2D_H_

