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

/** \file Form3D.h
 * \author Alexey A. Shabelnikov
 */

#ifndef _FORM3D_H_
#define _FORM3D_H_
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include <deque>
#include "TChartEx.h"

typedef void (__cdecl *EventHandler)(void* i_param);
typedef void (__cdecl *OnWndActivation)(void* i_param, long cmd);
typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, int index, void* i_param);

//---------------------------------------------------------------------------
class TForm3D : public TForm
{
 __published: // IDE-managed Components
  TChartEx *Chart1;
  TLineSeries *Series1;
  TLineSeries *Series2;
  TLineSeries *Series3;
  TLineSeries *Series4;
  TLineSeries *Series5;
  TLineSeries *Series6;
  TLineSeries *Series7;
  TLineSeries *Series8;
  TLineSeries *Series9;
  TLineSeries *Series10;
  TLineSeries *Series11;
  TLineSeries *Series12;
  TLineSeries *Series13;
  TLineSeries *Series14;
  TLineSeries *Series15;
  TLineSeries *Series16;
  TLineSeries *Series17;
  TLineSeries *Series18;
  TLineSeries *Series19;
  TLineSeries *Series20;
  TLineSeries *Series21;
  TLineSeries *Series22;
  TLineSeries *Series23;
  TLineSeries *Series24;
  TLineSeries *Series25;
  TLineSeries *Series26;
  TLineSeries *Series27;
  TLineSeries *Series28;
  TLineSeries *Series29;
  TLineSeries *Series30;
  TLineSeries *Series31;
  TLineSeries *Series32;
  TLabel *LabelAfv;
  TLabel *LabelAfc;
  TTrackBar *TrackBarAf;
  TCheckBox *CheckBox3d;
  TCheckBox *CheckBoxBv;
  TBitBtn *ButtonAngleUp;
  TBitBtn *ButtonAngleDown;
  TButton *Smoothing3x;
  TButton *Smoothing5x;
  TPopupMenu *PopupMenu;
  TMenuItem *PM_ZeroAllPoints;
  TMenuItem *PM_Dup1stPoint;
  TMenuItem *PM_BldCurveUsing1stAndLastPoints;
  TMenuItem *PM_ZeroAllCurves;
  TMenuItem *PM_DupThisCurve;
  TMenuItem *PM_BldShapeUsing1stAndLastCurves;
  TMenuItem *Separator1;
  TMenuItem *PM_SetPtMovStep;
  TMenuItem *PM_HideMarks;

  TMenuItem *PM_CopyToCurve;
  TMenuItem *PM_CopyToCurve0;
  TMenuItem *PM_CopyToCurve1;
  TMenuItem *PM_CopyToCurve2;
  TMenuItem *PM_CopyToCurve3;
  TMenuItem *PM_CopyToCurve4;
  TMenuItem *PM_CopyToCurve5;
  TMenuItem *PM_CopyToCurve6;
  TMenuItem *PM_CopyToCurve7;
  TMenuItem *PM_CopyToCurve8;
  TMenuItem *PM_CopyToCurve9;
  TMenuItem *PM_CopyToCurve10;
  TMenuItem *PM_CopyToCurve11;
  TMenuItem *PM_CopyToCurve12;
  TMenuItem *PM_CopyToCurve13;
  TMenuItem *PM_CopyToCurve14;
  TMenuItem *PM_CopyToCurve15;

  TMenuItem *PM_CopyFromCurve;
  TMenuItem *PM_CopyFromCurve0;
  TMenuItem *PM_CopyFromCurve1;
  TMenuItem *PM_CopyFromCurve2;
  TMenuItem *PM_CopyFromCurve3;
  TMenuItem *PM_CopyFromCurve4;
  TMenuItem *PM_CopyFromCurve5;
  TMenuItem *PM_CopyFromCurve6;
  TMenuItem *PM_CopyFromCurve7;
  TMenuItem *PM_CopyFromCurve8;
  TMenuItem *PM_CopyFromCurve9;
  TMenuItem *PM_CopyFromCurve10;
  TMenuItem *PM_CopyFromCurve11;
  TMenuItem *PM_CopyFromCurve12;
  TMenuItem *PM_CopyFromCurve13;
  TMenuItem *PM_CopyFromCurve14;
  TMenuItem *PM_CopyFromCurve15;

  void __fastcall TrackBarAfChange(TObject *Sender);
  void __fastcall Chart1ClickSeries(TCustomChart *Sender, TChartSeries *Series, int ValueIndex, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall Chart1GetAxisLabel(TChartAxis *Sender, TChartSeries *Series, int ValueIndex, AnsiString &LabelText);
  void __fastcall CheckBox3dClick(TObject *Sender);
  void __fastcall CheckBoxBvClick(TObject *Sender);
  void __fastcall OnCloseForm(TObject *Sender, TCloseAction &Action);
  void __fastcall ButtonAngleUpClick(TObject *Sender);
  void __fastcall ButtonAngleDownClick(TObject *Sender);
  void __fastcall Smoothing3xClick(TObject *Sender);
  void __fastcall Smoothing5xClick(TObject *Sender);
  void __fastcall OnZeroAllCurves(TObject *Sender);
  void __fastcall OnDuplicateThisCurve(TObject *Sender);
  void __fastcall OnZeroAllPoints(TObject *Sender);
  void __fastcall OnDuplicate1stPoint(TObject *Sender);
  void __fastcall OnBldCurveUsing1stAndLastPoints(TObject *Sender);
  void __fastcall OnBuildShapeUsing1stAndLastCurves(TObject *Sender);
  void __fastcall CtrlKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall OnEnterChart(TObject* Sender);
  void __fastcall OnExitChart(TObject* Sender);
  void __fastcall OnChartMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnCopyToCurve(TObject *Sender);
  void __fastcall OnCopyFromCurve(TObject *Sender);
  void __fastcall OnSetPtMovStep(TObject *Sender);
  void __fastcall LineSeriesGetMarkText(TChartSeries *Sender, int ValueIndex, AnsiString &LabelText);
  void __fastcall OnHideMarks(TObject *Sender);

 public:
  __fastcall TForm3D(TComponent* Owner);
  void DataPrepare();

  void SetOnChange(EventHandler i_pOnChange,void* i_param);
  void SetOnClose(EventHandler i_pOnClose,void* i_param);
  void SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param);
  void SetOnGetXAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);

  void Enable(bool enable);
  void InitPopupMenu(HINSTANCE hInstance);
  void InitHints(HINSTANCE hInstance);
  void SetPtValuesFormat(LPCTSTR ptValFormat);

 public: //Properties
  int m_count_x;
  int m_count_z;
  float m_u_slots[1024];
  float m_fnc_min;
  float m_fnc_max;
  float *mp_modified_function;
  float *mp_original_function;
  AnsiString m_u_title;
  AnsiString m_x_title;
  AnsiString m_y_title;
  AnsiString m_values_format_x;
  float m_pt_moving_step;

 private:
  float GetItem_m(int z, int x) const;
  float* GetItem_mp(int z, int x);
  float GetItem_o(int z, int x) const;
  int SetItem(int z, int x, float value);
  void SetAirFlow(int flow);
  void MakeOneVisible(int flow);
  void MakeAllVisible(void);
  void ShowPoints(bool show);
  void FillChart(bool dir,int cm);
  void HideAllSeries(void);
  int __fastcall GetCurveSelIndex(void) const;
  void __fastcall ShiftPoints(float i_value);
  void __fastcall MarkPoints(bool i_mark);
  void __fastcall UnmarkPoints(void);
  void __fastcall SelLeftArrow(bool i_shift);
  void __fastcall SelRightArrow(bool i_shift);

  void RestrictAndSetChartValue(int index, double v);
  void __fastcall CopyCurve(int fromIndex, int toIndex);
  double GetChartValue(int z, int index);
  void SetChartValue(int z, int index, double value);
  virtual void __fastcall WndProc(Messages::TMessage &Message);

 private: // User declarations
  //адрес функции которая будет вызываться после изменения данных
  EventHandler m_pOnChange;
  void* m_param_on_change;

  EventHandler m_pOnClose;
  void* m_param_on_close;

  OnWndActivation m_pOnWndActivation;
  void* m_param_on_wnd_activation;

  //адпес функции которая будет вызываться при рисовании надписей на оси X
  OnGetAxisLabel m_pOnGetXAxisLabel;
  void* m_param_on_get_x_axis_label;

  int m_setval;
  int m_val_n;
  int m_air_flow_position;
  bool m_chart_active;
  std::deque<int> m_selpts;
  std::pair<int, int> m_prev_pt;
  int m_visibleMarkIdx;
};
//---------------------------------------------------------------------------
#endif //_FORM3D_H_

