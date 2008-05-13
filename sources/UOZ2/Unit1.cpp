/****************************************************************
 *               SECU-3  - engine control unit
 *    Designed by Aleksey Shabelnikov. Ukraine, Gorlovka 2007.
 *    STc - Microprocessors systems: design & programming
 *    contacts:
 *              shabelnikov-stc@mail.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TForm1::DataPrepare()
{
  HideAllSeries();
  CheckBox2->Enabled = false;
  TrackBar1->Max = count_z - 1;
  SetAirFlow(0);
  MakeOneVisible(airflow);
  ShowPoints(true);
  setval = 0;
  val_n  = 0;

  Chart1->Title->Text->Clear();
  Chart1->Title->Text->Add(u_title);
  Chart1->BottomAxis->Title->Caption = x_title;

  //диапазон значений на графике будет немного шире чем требуемый...
  int range = aai_max - aai_min;
  Chart1->LeftAxis->Maximum = aai_max + range / 15.0f;
  Chart1->LeftAxis->Minimum = aai_min - range / 20.0f;

  Chart1->Chart3DPercent = 29;
  FillChart(0,0);
}
//---------------------------------------------------------------------------

//get item from oroginal function
float TForm1::GetItem_o(int z, int x)
{
  if (x >= count_z) return 0.0f;
  if (z >= count_x) return 0.0f;
  int i  = (count_z - 1) - z;
  return *(original_function + ((i * count_x) + x));
}
//---------------------------------------------------------------------------

//get item from modified function
float TForm1::GetItem_m(int z, int x)
{
  if (z >= count_z) return 0.0f;
  if (x >= count_x) return 0.0f;
  int i  = (count_z - 1) - z;
  return *(modified_function + ((i * count_z) + x));
}
//---------------------------------------------------------------------------

//set item in modified function
int TForm1::SetItem(int z, int x, float value)
{
  if (z >= count_z) return 0;
  if (x >= count_x) return 0;
  int i  = (count_z - 1) - z;
  *(modified_function + ((i * count_x) + x)) = value;
 return 1;
}
//---------------------------------------------------------------------------

void TForm1::SetAirFlow(int flow)
{
  AnsiString as;
  TrackBar1->Position = flow;
  as.sprintf("%d",flow+1);
  Label1->Caption = as;
  airflow = flow;
  MakeOneVisible(flow);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{
  SetAirFlow(TrackBar1->Position);
  setval = 0;
  val_n  = 0;
}
//---------------------------------------------------------------------------

void TForm1::MakeOneVisible(int flow)
{
  for(int i = 0; i < count_z; i++)
  {
    if (i==flow)
    {
      Chart1->Series[i]->Active = true;
      Chart1->Series[i+count_z]->Active = true;
    }
    else
    {
      Chart1->Series[i]->Active = false;
      Chart1->Series[i+count_z]->Active = false;
    }
  }
}
//---------------------------------------------------------------------------

void TForm1::ShowPoints(bool show)
{
  for (int i = 0; i < count_z; i++)
  {
    Chart1->Series[i]->Marks->Visible = show;
    Chart1->Series[i + count_z]->Marks->Visible = show;
    ((TPointSeries*)Chart1->Series[i])->Pointer->Visible = show;
    ((TPointSeries*)Chart1->Series[i + count_z])->Pointer->Visible = show;
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Chart1ClickSeries(TCustomChart *Sender,
      TChartSeries *Series, int ValueIndex, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
  if (Series==Chart1->Series[airflow + count_z])
  {
    if (Button==mbLeft)
    {
      setval  = 1;
      val_n   = ValueIndex;
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
    v = Chart1->Series[airflow + count_z]->YScreenToValue(Y);
    if (v > aai_max) v = aai_max;
    if (v < aai_min) v = aai_min;
    SetItem(airflow,val_n,v);
    Chart1->Series[airflow + count_z]->YValue[val_n] = v;
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
  *close_flag = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
  if (CheckBox1->Checked)
  {
    ShowPoints(false);
    Chart1->View3D  = true;
    MakeAllVisible();
    TrackBar1->Enabled = false;
    CheckBox2->Enabled = true;
    CheckBox2Click(NULL);
  }
  else
  {
    ShowPoints(true);
    Chart1->View3D = false;
    MakeOneVisible(airflow);
    TrackBar1->Enabled = true;
    CheckBox2->Enabled = false;
    FillChart(0,0);
  }
}
//---------------------------------------------------------------------------

void TForm1::MakeAllVisible(void)
{
  for(int i = 0; i < count_z; i++)
  {
    Chart1->Series[i]->Active = false;
    Chart1->Series[i + count_z]->Active = true;
  }
}
//---------------------------------------------------------------------------

//цвета для 3D графика
long col[16] ={0xA88CD5,
               0xD26EDC,
               0xC38CBE,
               0xCB9491,
               0xC8AA85,
               0xCDC38F,
               0xD3D48F,
               0xB2D573,
               0x87DCA3,
               0x87e4A3,
               0x99E9A3,
               0x5DF3DF,
               0x3ACDE9,
               0x78AFE9,
               0x5D94EB,
               0x555AFD};
//---------------------------------------------------------------------------

void TForm1::FillChart(bool dir,int cm)
{
  int d,k;  AnsiString as;
  if (!dir)
  {
    d = 1;
    k = 0;
  }
  else
  {
    d = -1;
    k = count_z - 1;
  }

  for(int j = 0; j < count_z; j++)
  {
    Chart1->Series[j]->Clear();
    Chart1->Series[j+count_z]->Clear();
  }
  
  for(int j = 0; j < count_z; j++)
  {
    for(int i = 0; i < count_x; i++)
    {
      as.sprintf("%d",u_slots[i]);
      if (cm)
      {
//      Chart1->Series[k]->Add(GetItem(j,i),as,clBlue);
        Chart1->Series[k + count_z]->Add(GetItem_m(j,i),as,col[j]);
      }
      else
      {
        Chart1->Series[k]->Add(GetItem_o(j,i),as,clAqua);
        Chart1->Series[k + count_z]->Add(GetItem_m(j,i),as,clRed);
      }
    }
    k+=d;
  }
}
//---------------------------------------------------------------------------

//вид 3D графика сзади или спереди
void __fastcall TForm1::CheckBox2Click(TObject *Sender)
{
  if (CheckBox2->Checked)
  {
    FillChart(0,1);
  }
  else
  {
    FillChart(1,1);
  }
}
//---------------------------------------------------------------------------

//Скрывает все серии значений
void TForm1::HideAllSeries(void)
{
  for(int i = 0; i < 32; i++)
    Chart1->Series[i]->Active = false;
}

//---------------------------------------------------------------------------

