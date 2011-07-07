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
#pragma hdrstop

#include "Form3D.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

bool RemoveInstanceByHWND(HWND hWnd);

//---------------------------------------------------------------------------
//цвета для 3D графика
long col[16] ={0xA88CD5, 0xD26EDC, 0xC38CBE, 0xCB9491,
               0xC8AA85, 0xCDC38F, 0xD3D48F, 0xB2D573,
               0x87DCA3, 0x87e4A3, 0x99E9A3, 0x5DF3DF,
               0x3ACDE9, 0x78AFE9, 0x5D94EB, 0x555AFD};

//---------------------------------------------------------------------------
__fastcall TForm3D::TForm3D(TComponent* Owner)
: TForm(Owner)
, count_x(0)
, count_z(0)
, airflow(0)
, aai_min(0)
, aai_max(0)
, modified_function(NULL)
, original_function(NULL)
, u_title("")
, x_title("")
, m_pOnChange(NULL)
, m_pOnClose(NULL)
, m_pOnWndActivation(NULL)
, m_param_on_change(NULL)
, m_param_on_close(NULL)
, m_param_on_wnd_activation(NULL)
, setval(0)
, val_n(0)
, m_air_flow_position(0)
{
 //empty
}

//---------------------------------------------------------------------------
void TForm3D::DataPrepare()
{
 HideAllSeries();
 CheckBox2->Enabled = false;
 TrackBar1->Max = count_z - 1;
 SetAirFlow(m_air_flow_position);
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

 CheckBox1Click(NULL);
}

//---------------------------------------------------------------------------
void TForm3D::SetOnChange(EventHandler i_pOnChange,void* i_param)
{
 m_pOnChange = i_pOnChange;
 m_param_on_change = i_param;
}

//---------------------------------------------------------------------------
void TForm3D::SetOnClose(EventHandler i_pOnClose,void* i_param)
{
 m_pOnClose = i_pOnClose;
 m_param_on_close = i_param;
}

//---------------------------------------------------------------------------
void TForm3D::SetOnWndActivation(OnWndActivation i_pOnWndActivation, void* i_param)
{
 m_pOnWndActivation = i_pOnWndActivation;
 m_param_on_wnd_activation = i_param;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::TrackBar1Change(TObject *Sender)
{
 m_air_flow_position = TrackBar1->Position;
 SetAirFlow(m_air_flow_position);
 setval = 0;
 val_n  = 0;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1ClickSeries(TCustomChart *Sender,
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
void __fastcall TForm3D::Chart1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 if ((m_pOnChange) && (setval))
  m_pOnChange(m_param_on_change);
 setval = 0;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Chart1MouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
 double v;
 if (setval)
 {
  v = Chart1->Series[airflow + count_z]->YScreenToValue(Y);
  RestrictAndSetValue(val_n, v);
 }
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::CheckBox1Click(TObject *Sender)
{
 if (CheckBox1->Checked)
 {
  ShowPoints(false);
  Chart1->View3D  = true;
  MakeAllVisible();
  TrackBar1->Enabled = false;
  CheckBox2->Enabled = true;
  CheckBox2Click(NULL);
  ButtonAngleUp->Enabled = false;
  ButtonAngleDown->Enabled = false;
  ButtonAngleUp->Visible = false;
  ButtonAngleDown->Visible = false;
 }
 else
 {
  ShowPoints(true);
  Chart1->View3D = false;
  MakeOneVisible(airflow);
  TrackBar1->Enabled = true;
  CheckBox2->Enabled = false;
  FillChart(0,0);
  ButtonAngleUp->Enabled = true;
  ButtonAngleDown->Enabled = true;
  ButtonAngleUp->Visible = true;
  ButtonAngleDown->Visible = true;
 }
}

//---------------------------------------------------------------------------
//вид 3D графика сзади или спереди
void __fastcall TForm3D::CheckBox2Click(TObject *Sender)
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
void __fastcall TForm3D::OnCloseForm(TObject *Sender, TCloseAction &Action)
{
 if (m_pOnClose)
  m_pOnClose(m_param_on_close);
 RemoveInstanceByHWND(Handle);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ButtonAngleUpClick(TObject *Sender)
{
 for (int i = 0; i < 16; i++ )
 {
  RestrictAndSetValue(i, Chart1->Series[airflow + count_z]->YValue[i] + 0.5);
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ButtonAngleDownClick(TObject *Sender)
{
 for (int i = 0; i < 16; i++ )
 {
  RestrictAndSetValue(i, Chart1->Series[airflow + count_z]->YValue[i] - 0.5);
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
//get item from oroginal function
float TForm3D::GetItem_o(int z, int x)
{
 if (z >= count_z) return 0.0f;
 if (x >= count_x) return 0.0f;
 int i  = (count_z - 1) - z;
 return *(original_function + ((i * count_x) + x));
}

//---------------------------------------------------------------------------
//get item from modified function
float TForm3D::GetItem_m(int z, int x)
{
 if (z >= count_z) return 0.0f;
 if (x >= count_x) return 0.0f;
 int i  = (count_z - 1) - z;
 return *(modified_function + ((i * count_x) + x));
}

//---------------------------------------------------------------------------
//set item in modified function
int TForm3D::SetItem(int z, int x, float value)
{
 if (z >= count_z) return 0;
 if (x >= count_x) return 0;
 int i  = (count_z - 1) - z;
 *(modified_function + ((i * count_x) + x)) = value;
 return 1;
}

//---------------------------------------------------------------------------
void TForm3D::SetAirFlow(int flow)
{
 AnsiString as;
 TrackBar1->Position = flow;
 as.sprintf("%d",flow+1);
 Label1->Caption = as;
 airflow = flow;
 MakeOneVisible(flow);
}

//---------------------------------------------------------------------------
void TForm3D::MakeOneVisible(int flow)
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
void TForm3D::MakeAllVisible(void)
{
 for(int i = 0; i < count_z; i++)
 {
  Chart1->Series[i]->Active = false;
  Chart1->Series[i + count_z]->Active = true;
 }
}

//---------------------------------------------------------------------------
void TForm3D::ShowPoints(bool show)
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
void TForm3D::FillChart(bool dir,int cm)
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
    Chart1->Series[k + count_z]->Add(GetItem_m(j,i),as,TColor(col[j]));
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
//Скрывает все серии значений
void TForm3D::HideAllSeries(void)
{
 for(int i = 0; i < 32; i++)
  Chart1->Series[i]->Active = false;
}

//---------------------------------------------------------------------------
void TForm3D::RestrictAndSetValue(int index, double v)
{
 if (v > aai_max) v = aai_max;
 if (v < aai_min) v = aai_min;
 SetItem(airflow,index, v);
 Chart1->Series[airflow + count_z]->YValue[index] = v;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::WndProc(Messages::TMessage &Message)
{
 TForm::WndProc(Message);

 if (Message.Msg == WM_SYSCOMMAND && m_pOnWndActivation)
  m_pOnWndActivation(m_param_on_wnd_activation, Message.WParam);
}

//---------------------------------------------------------------------------
