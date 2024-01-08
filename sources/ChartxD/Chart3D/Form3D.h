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
#include "TChartEx.h"
#include "TeeSurfa.hpp"
#include "Selection.h"
#include <utility>

typedef void (__cdecl *EventHandler)(void* i_param);
typedef void (__cdecl *OnWndActivation)(void* i_param, long cmd);
typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, int index, void* i_param);
typedef float (__cdecl *OnValueTransform)(void* i_param, float sourceVal, int direction);

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
  TIsoSurfaceSeries *Series3D;
  TLabel *LabelAfv;
  TLabel *LabelAfc;
  TTrackBar *TrackBarAf;
  TCheckBox *CheckBox3d;
  TCheckBox *CheckBoxBv;
  TBitBtn *ButtonAngleUp;
  TBitBtn *ButtonAngleDown;
  TButton *Smoothing3x;
  TButton *Smoothing5x;
  TButton *TrackLeft;
  TButton *TrackRight;
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
  TMenuItem *PM_HideOldCurve;

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
  TMenuItem *PM_Export;
  TMenuItem *PM_ExportCSV;
  TMenuItem *PM_Import;
  TMenuItem *PM_ImportCSV;
  TMenuItem *Separator2;
  TMenuItem *PM_Copy;
  TMenuItem *PM_Paste;
  TMenuItem *PM_Inc;
  TMenuItem *PM_Dec;
  TMenuItem *PM_SetTo;
  TMenuItem *PM_Sub;
  TMenuItem *PM_Add;
  TMenuItem *PM_Mul;
  TOpenDialog* openDialog;
  TSaveDialog* saveDialog;

  void __fastcall TrackBarAfChange(TObject *Sender);
  void __fastcall Chart1ClickSeries(TCustomChart *Sender, TChartSeries *Series, int ValueIndex, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall OnChart1DblClick(TObject *Sender);
  void __fastcall Chart1GetAxisLabel(TChartAxis *Sender, TChartSeries *Series, int ValueIndex, AnsiString &LabelText);
  void __fastcall CheckBox3dClick(TObject *Sender);
  void __fastcall CheckBoxBvClick(TObject *Sender);
  void __fastcall OnCloseForm(TObject *Sender, TCloseAction &Action);
  void __fastcall ButtonAngleUpClick(TObject *Sender);
  void __fastcall ButtonAngleDownClick(TObject *Sender);
  void __fastcall Smoothing3xClick(TObject *Sender);
  void __fastcall Smoothing5xClick(TObject *Sender);
  void __fastcall TrackLeftClick(TObject *Sender);
  void __fastcall TrackRightClick(TObject *Sender);
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
  void __fastcall OnHideOldCurve(TObject *Sender);
  void __fastcall FormDeactivate(TObject *Sender);
  void __fastcall OnCreateForm(TObject *Sender);
  void __fastcall OnExportCSV(TObject *Sender);
  void __fastcall OnImportCSV(TObject *Sender);
  void __fastcall OnCopy(TObject *Sender);
  void __fastcall OnPaste(TObject *Sender);
  void __fastcall OnInc(TObject *Sender);
  void __fastcall OnDec(TObject *Sender);
  void __fastcall OnSetTo(TObject *Sender);
  void __fastcall OnSub(TObject *Sender);
  void __fastcall OnAdd(TObject *Sender);
  void __fastcall OnMul(TObject *Sender);
  double __fastcall OnGetYValue(TChartSeries *Sender, int X, int Z); //from TIsoSurfaceSerie
  void __fastcall OnAfterDrawChart(TObject *Sender);

 public:
  __fastcall TForm3D(HWND parent);
  __fastcall ~TForm3D();
  void DataPrepare(bool create);

  void SetOnChange(EventHandler i_pOnChange,void* i_param);
  void SetOnChangeSettings(EventHandler i_pOnChange,void* i_param);
  void SetOnClose(EventHandler i_pOnClose,void* i_param);
  void SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param);
  void SetOnGetXAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
  void SetOnValueTransform(OnValueTransform i_pOnValueTransform, void* i_param);

  void Enable(bool enable);
  void InitPopupMenu(HINSTANCE hInstance);
  void InitHints(HINSTANCE hInstance);
  void SetPtValuesFormat(LPCTSTR ptValFormat);

  void SetDimentions(int z, int x);
  void SetRange(float ymin, float ymax);
  void SetChartTitle(const AnsiString& title);
  void SetXAxisTitle(const AnsiString& title);
  void SetYAxisTitle(const AnsiString& title);

 public: //Properties
  float m_u_slots[64];
  float *mp_modified_function;
  float *mp_original_function;
  AnsiString m_values_format_x;
  float m_pt_moving_step;
  static char m_csvsep_symb;

 private:
  float GetItem_m(int z, int x) const;
  float* GetItem_mp(int z, int x);
  float GetItem_o(int z, int x) const;
  int SetItem(int z, int x, float value);
  void SetAirFlow(int flow);
  void MakeOneVisible(int flow);
  void MakeAllVisible(void);
  void FillChart(void);
  void HideAllSeries(void);
  int __fastcall GetZPos(int z) const;
  void __fastcall ShiftPoints(float i_value, bool all = false);
  void __fastcall MarkPoints(bool i_mark);
  void __fastcall UnmarkPoints(void);
  void __fastcall SelLeftArrow(bool i_shift);
  void __fastcall SelRightArrow(bool i_shift);
  void __fastcall SelDownArrow(bool i_shift); //only in 3D
  void __fastcall SelUpArrow(bool i_shift);   //only in 3D

  void RestrictAndSetChartValue(int index, double v);
  void RestrictAndSetValue(int index_z, int index_x, double v);
  void __fastcall CopyCurve(int fromIndex, int toIndex);
  void SetChartValue(int z, int index, double value);
  void UpdateChartValues(void);
  void UpdateSurfaceValues(void);
  virtual void __fastcall WndProc(Messages::TMessage &Message);
  void UpdateSystemColors(void);
  void ClipboardCopy(void);
  void ClipboardPaste(void);

 private: // User declarations
  //адрес функции которая будет вызываться после изменения данных
  EventHandler m_pOnChange;
  void* m_param_on_change;

  EventHandler m_pOnChangeSettings;
  void* m_param_on_change_sett;

  EventHandler m_pOnClose;
  void* m_param_on_close;

  OnWndActivation m_pOnWndActivation;
  void* m_param_on_wnd_activation;

  //address of function which will be called at drawing labels on the X-axis
  OnGetAxisLabel m_pOnGetXAxisLabel;
  void* m_param_on_get_x_axis_label;

  //optional callback function which will be called for transforming each value
  OnValueTransform m_pOnValueTransform;
  void* m_param_on_value_transform;

  int m_setval;
  int m_val_x; //x-position of the selection point
  int m_val_z; //z-position of the selection point
  int m_air_flow_position;
  bool m_chart_active;

  std::pair<int, int> m_prev_pt;
  int m_visibleMarkIdx;
 
  HINSTANCE m_hInst;
  double m_3d_transparency;

  Selection m_sel;

  int m_count_z;
  int m_count_x;
  float m_fnc_min;
  float m_fnc_max;
  AnsiString m_x_title;
  AnsiString m_y_title;
  AnsiString m_u_title; //chart title

  //for changing position of camera using mouse:
  int m_mc_xpos;
  int m_mc_ypos;
  int m_mc_rotation;
  int m_mc_elevation;
};
//---------------------------------------------------------------------------
#endif //_FORM3D_H_

