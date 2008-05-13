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
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
        , aai_min(0.0f), aai_max(0.0f)
{
  setval  = 0;
  val_n   = 0;
  v_count = 0;
  u_title = "";
  original_function  = NULL;
  modified_function  = NULL;
  close_flag = NULL;
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
    if (v > aai_max)  v = aai_max;
    if (v < aai_min) v = aai_min;
    modified_function[val_n] = v;
    Series2->YValue[val_n] = v;
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
  Chart1->Title->Text->Add(u_title);
  for(int i = 0; i < v_count; i++)
  {
    as.sprintf("%d",u_slots[i]);
    Series1->Add(u_values[i],as,clAqua);
    Series2->Add(modified_function[i],as,clRed);
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
  *close_flag = 0;
   setval  = 0;
   val_n   = 0;
   v_count = 0;
   u_title = "";
   original_function = NULL;
   modified_function = NULL;
   close_flag = NULL;
}
//---------------------------------------------------------------------------

