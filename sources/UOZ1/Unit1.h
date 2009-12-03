//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
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

typedef void (__cdecl *EventHandler)(void* i_param);
typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, void* i_param);

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
 __published:	// IDE-managed Components
  TChart *Chart1;
  TLineSeries *Series1;
  TLineSeries *Series2;
  TBitBtn *ButtonAngleUp;
  TBitBtn *ButtonAngleDown;
  TButton *Smoothing3x;
  TButton *Smoothing5x;
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
 public:
  void SetOnChange(EventHandler i_pOnChange,void* i_param);
  void SetOnClose(EventHandler i_pOnClose,void* i_param);
  void SetOnGetYAxisLabel(OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
 public:		// User declarations
  float aai_min;
  float aai_max;
  int   count_of_function_points;
  int   horizontal_axis_grid_values[1024];
  AnsiString  chart_title_text;
  AnsiString  x_axis_title;
  AnsiString  y_axis_title;
  float *original_function;
  float *modified_function;

  __fastcall TForm1(TComponent* Owner);
  void DataPrepare();

 private:	// User declarations
  bool setval;
  int  val_n;

  //адрес функции которая будет вызываться после изменения данных
  EventHandler m_pOnChange;
  void* m_param_on_change;

  //адрес функции которая будет вызываться во время закрытия окна
  EventHandler m_pOnClose;
  void* m_param_on_close;

  //адпес функции которая будет вызываться при рисовании надписей на оси Y
  OnGetAxisLabel m_pOnGetYAxisLabel;
  void* m_param_on_get_axis_label;

  void RestrictAndSetValue(int index, double v);
  float Smoothing(int size, int index, int lower_bound, int upper_bound, float data[]);
};
#endif
