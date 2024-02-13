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

/** \file Form2D.h
 * \author Alexey A. Shabelnikov
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
#include <vector>
#include "TFloatUpDown.h"
#include "TChartEx.h"

typedef void (__cdecl *EventHandler)(void* i_param);
typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, int index, void* i_param);
typedef void (__cdecl *OnWndActivation)(void* i_param, long cmd);
typedef void (__cdecl *OnChangeValue)(void* i_param, int type, float value);

class UndoCntr;

//---------------------------------------------------------------------------
class TForm2D : public TForm
{
 __published:  // IDE-managed Components
  TChartEx *Chart1;
  TLineSeries *Series1;
  TLineSeries *Series2;
  TBitBtn *ButtonAngleUp;
  TBitBtn *ButtonAngleDown;
  TBitBtn *ButtonShowBins;
  TButton *Smoothing3x;
  TButton *Smoothing5x;
  TPopupMenu *PopupMenu;
  TMenuItem *PM_ZeroAllPoints;
  TMenuItem *PM_Dup1stPoint;
  TMenuItem *PM_BldCurveUsing1stAndLastPoints;
  TMenuItem *Separator1;
  TMenuItem *PM_SetPtMovStep;
  TMenuItem *PM_HideMarks;
  TMenuItem *PM_HideOldCurve;
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
  TMenuItem *PM_Interpolate;
  TMenuItem *PM_Undo;
  TMenuItem *PM_Redo;
  TEdit *EditXBegin;
  TEdit *EditXEnd;
  TFloatUpDown *SpinXBegin;
  TFloatUpDown *SpinXEnd;
  TPanel *PanelBins;
  TEdit *Edit1;
  TEdit *Edit2;
  TEdit *Edit3;
  TEdit *Edit4;
  TEdit *Edit5;
  TEdit *Edit6;
  TEdit *Edit7;
  TEdit *Edit8;
  TFloatUpDown *UpDown1;
  TFloatUpDown *UpDown2;
  TFloatUpDown *UpDown3;
  TFloatUpDown *UpDown4;
  TFloatUpDown *UpDown5;
  TFloatUpDown *UpDown6;
  TFloatUpDown *UpDown7;
  TFloatUpDown *UpDown8;
  TOpenDialog* openDialog;
  TSaveDialog* saveDialog;

  void __fastcall Chart1ClickSeries(TCustomChart *Sender, TChartSeries *Series, int ValueIndex, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Chart1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ButtonAngleUpClick(TObject *Sender);
  void __fastcall ButtonAngleDownClick(TObject *Sender);
  void __fastcall Smoothing3xClick(TObject *Sender);
  void __fastcall Smoothing5xClick(TObject *Sender);
  void __fastcall ButtonShowBinsClick(TObject *Sender);
  void __fastcall Chart1GetAxisLabel(TChartAxis *Sender, TChartSeries *Series, int ValueIndex, AnsiString &LabelText);
  void __fastcall OnZeroAllPoints(TObject *Sender);
  void __fastcall OnDuplicate1stPoint(TObject *Sender);
  void __fastcall OnBldCurveUsing1stAndLastPoints(TObject *Sender);
  void __fastcall EditXBeginOnChange(TObject *Sender);
  void __fastcall EditXEndOnChange(TObject *Sender);
  void __fastcall BinsEditOnChange(TObject *Sender);
  void __fastcall CtrlKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall OnEnterChart(TObject* Sender);
  void __fastcall OnExitChart(TObject* Sender);
  void __fastcall OnChartMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnSetPtMovStep(TObject *Sender);
  void __fastcall LineSeries2GetMarkText(TChartSeries *Sender, int ValueIndex, AnsiString &LabelText);
  void __fastcall OnHideMarks(TObject *Sender);
  void __fastcall OnHideOldCurve(TObject *Sender);
  void __fastcall FormDeactivate(TObject *Sender);
  void __fastcall FormResize(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
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
  void __fastcall OnInterpolate(TObject *Sender);
  void __fastcall OnUndo(TObject *Sender);
  void __fastcall OnRedo(TObject *Sender);

 public:  // User declarations
  __fastcall TForm2D(HWND parent);
  __fastcall ~TForm2D();
  void DataPrepare();

  void SetOnChange(EventHandler i_pOnChange,void* i_param);
  void SetOnChangeSettings(EventHandler i_pOnChange,void* i_param);
  void SetOnClose(EventHandler i_pOnClose,void* i_param);
  void SetOnGetYAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
  void SetOnGetXAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
  void SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param);
  void ShowXEdits(void);
  void CfgXEdits(void);
  void SetXEditVal(void);

  void Enable(bool i_enable);
  void InitPopupMenu(HINSTANCE hInstance);
  void InitHints(HINSTANCE hInstance);
  void SetPtValuesFormat(LPCTSTR ptValFormat);
  void InitBins(void);
  void UpdateSystemColors(void);

  void __fastcall AttachData(const float* p_orig, float* p_modi);

 public: //properties
  int m_count_x;
  float m_fnc_min;
  float m_fnc_max;
  float m_horizontal_axis_grid_values[256];
  float m_horizontal_axis_values[256]; //dynamically generated values in the "begin & end" mode
  AnsiString m_horizontal_axis_values_format;  //for axis, not for edit bins
  AnsiString m_chart_title_text;
  AnsiString m_x_axis_title;
  AnsiString m_y_axis_title;
  float m_pt_moving_step;
  int m_horizontal_axis_grid_mode;             //3 modes are possible
  static char m_csvsep_symb;
  static bool m_classic2DKeys;

 private:
  void UndoAdd(void);

  const float *mp_original_function;
  float *mp_modified_function;

  TEdit* m_binsEdit[8];
  TFloatUpDown* m_binsUpDown[8];

  void __fastcall UpdateBinsPosition(void);
  void RestrictAndSetValue(int index, double v);
  void __fastcall ShiftFunction(float i_value);
  void __fastcall ShiftPoints(float i_value);
  void __fastcall MarkPoints(bool i_mark);
  void __fastcall SelLeftArrow(bool i_shift);
  void __fastcall SelRightArrow(bool i_shift);
  bool __fastcall CheckBinForErrors(int itemIndex, float value);
  void __fastcall UpdateBins(void);
  void ClipboardCopy(void);
  void ClipboardPaste(void);
  virtual void __fastcall WndProc(Messages::TMessage &Message);

 private:  // User declarations
  //адрес функции которая будет вызываться после изменения данных
  EventHandler m_pOnChange;
  void* m_param_on_change;

  EventHandler m_pOnChangeSettings;
  void* m_param_on_change_sett;

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

  bool m_setval;
  int  m_val_n;
  std::deque<int> m_selpts;
  std::pair<int, int> m_prev_pt;
  std::vector<int> m_errors;    //for bins errors' checking
  int m_visibleMarkIdx;

  HINSTANCE m_hInst;

  UndoCntr* mp_undo;
};
#endif //_FORM2D_H_

