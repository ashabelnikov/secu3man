/****************************************************************
 *               SECU-3  - engine control unit
 *    Designed by Aleksey Shabelnikov. Ukraine, Gorlovka 2007.
 *    STc - Microprocessors systems: design & programming
 *    contacts:
 *              shabelnikov-stc@mail.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

bool RemoveInstanceByHWND(HWND hWnd);

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
        , aai_min(0.0f), aai_max(0.0f)
        , m_pOnChange(NULL)
        , m_pOnClose(NULL)
        , m_param_on_change(NULL)
        , m_param_on_close(NULL)
{
  setval  = 0;
  val_n   = 0;
  count_of_function_points = 0;
  chart_title_text = "";
  original_function  = NULL;
  modified_function  = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Chart1ClickSeries(TCustomChart *Sender,
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

void __fastcall TForm1::Chart1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
  if ((m_pOnChange) && (setval))
    m_pOnChange(m_param_on_change);
  setval = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Chart1MouseMove(TObject *Sender, TShiftState Shift,
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

void TForm1::DataPrepare()
{
  setval = 0;
  val_n  = 0;
  AnsiString as;

  //диапазон значений шкалы графика должен быть немного шире...
  float range = aai_max - aai_min;
  Chart1->LeftAxis->Maximum = aai_max + range / 15.0;
  Chart1->LeftAxis->Minimum = aai_min - range / 20;

  Chart1->Title->Text->Clear();
  Chart1->Title->Text->Add(chart_title_text);
  //Chart1->LeftA ->Title->Caption = y_axis_title;
  Chart1->BottomAxis->Title->Caption = x_axis_title;

  for(int i = 0; i < count_of_function_points; i++)
  {
    as.sprintf("%d",horizontal_axis_grid_values[i]);
    Series1->Add(original_function[i],as,clAqua);
    Series2->Add(modified_function[i],as,clRed);
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
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

void TForm1::SetOnChange(EventHandler i_pOnChange,void* i_param)
{
  m_pOnChange = i_pOnChange;
  m_param_on_change = i_param;
}

void TForm1::SetOnClose(EventHandler i_pOnClose,void* i_param)
{
  m_pOnClose = i_pOnClose;
  m_param_on_close = i_param;
}

//---------------------------------------------------------------------------
void TForm1::RestrictAndSetValue(int index, double v)
{
  if (v > aai_max)  v = aai_max;
  if (v < aai_min) v = aai_min;
  modified_function[index] = v;
  Series2->YValue[index] = v;
}

void __fastcall TForm1::ButtonAngleUpClick(TObject *Sender)
{
 for (int i = 0; i < 16; i++ )
   {
   RestrictAndSetValue(i, Series2->YValue[i] + 0.5);
   }
  if (m_pOnChange)
    m_pOnChange(m_param_on_change);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonAngleDownClick(TObject *Sender)
{
  for (int i = 0; i < 16; i++ )
    {
    RestrictAndSetValue(i, Series2->YValue[i] - 0.5);
    }
  if (m_pOnChange)
    m_pOnChange(m_param_on_change);
}
//---------------------------------------------------------------------------

