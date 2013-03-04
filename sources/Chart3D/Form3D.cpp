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

#include "../common/MathHelpers.h"
#include "Form3D.h"
#include "resource.h"
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
, m_values_format_x("%.00f")  //integer 
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
void TForm3D::Enable(bool enable)
{
 if (false==enable)
 {
  for (int i = 0; i < 32; i++ )
   Chart1->Series[i]->Active = false;
 }
 else
  DataPrepare();

 Chart1->Enabled = enable;
 Label1->Enabled = enable;
 Label2->Enabled = enable;
 TrackBar1->Enabled = enable;
 CheckBox1->Enabled = enable;
 CheckBox2->Enabled = enable && CheckBox1->Checked;
 ButtonAngleUp->Enabled = enable;
 ButtonAngleDown->Enabled = enable;
 if (CheckBox1->Checked)
  Chart1->Visible = enable;
}

//---------------------------------------------------------------------------
void TForm3D::InitPopupMenu(HINSTANCE hInstance)
{
 char string[1024 + 1];
 ::LoadString(hInstance, IDS_PM_ZERO_ALL_POINTS, string, 1024);
 PM_ZeroAllPoints->Caption = string;
 ::LoadString(hInstance, IDS_PM_SET_ALLTO_1ST_PT, string, 1024);
 PM_Dup1stPoint->Caption = string;
 ::LoadString(hInstance, IDS_PM_BLD_CURVE_1ST_LAST_PT, string, 1024);
 PM_BldCurveUsing1stAndLastPoints->Caption = string;
 ::LoadString(hInstance, IDS_PM_ZERO_ALL_CURVES, string, 1024);
 PM_ZeroAllCurves->Caption = string;
 ::LoadString(hInstance, IDS_PM_SET_ALLTO_THIS_CR, string, 1024);
 PM_DupThisCurve->Caption = string;
 ::LoadString(hInstance, IDS_PM_BLD_SHAPE_1ST_LAST_CR, string, 1024);
 PM_BldShapeUsing1stAndLastCurves->Caption = string;
}

//---------------------------------------------------------------------------
void TForm3D::InitHints(HINSTANCE hInstance)
{
 char string[1024 + 1];
 ::LoadString(hInstance, IDS_TT_SMOOTHING_3_POINTS, string, 1024);
 Smoothing3x->Hint = string;
 ::LoadString(hInstance, IDS_TT_SMOOTHING_5_POINTS, string, 1024);
 Smoothing5x->Hint = string;
 ::LoadString(hInstance, IDS_TT_MOVE_FUNC_UP, string, 1024);
 ButtonAngleUp->Hint = string;
 ::LoadString(hInstance, IDS_TT_MOVE_FUNC_DOWN, string, 1024);
 ButtonAngleDown->Hint = string;
 ::LoadString(hInstance, IDS_TT_FUNC_VIEW_3D, string, 1024);
 CheckBox1->Hint = string;
 ::LoadString(hInstance, IDS_TT_FUNC_VIEW_BACK, string, 1024);
 CheckBox2->Hint = string;
 ::LoadString(hInstance, IDS_TT_SELECT_CURVE, string, 1024);
 TrackBar1->Hint = string;
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
  RestrictAndSetChartValue(val_n, v);
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
  Label1->Enabled = false;
  CheckBox2->Enabled = true;
  CheckBox2Click(NULL);
  ButtonAngleUp->Enabled = false;
  ButtonAngleDown->Enabled = false;
  ButtonAngleUp->Visible = false;
  ButtonAngleDown->Visible = false;
  Smoothing3x->Enabled = false;
  Smoothing5x->Enabled = false;
  Smoothing3x->Visible = false;
  Smoothing5x->Visible = false;
  for (int i = 0; i < PopupMenu->Items->Count; i++)
   PopupMenu->Items->Items[i]->Enabled = false;
 }
 else
 {
  ShowPoints(true);
  Chart1->View3D = false;
  MakeOneVisible(airflow);
  TrackBar1->Enabled = true;
  Label1->Enabled = true;
  CheckBox2->Enabled = false;
  FillChart(0,0);
  ButtonAngleUp->Enabled = true;
  ButtonAngleDown->Enabled = true;
  ButtonAngleUp->Visible = true;
  ButtonAngleDown->Visible = true;
  Smoothing3x->Enabled = true;
  Smoothing5x->Enabled = true;
  Smoothing3x->Visible = true;
  Smoothing5x->Visible = true;
  for (int i = 0; i < PopupMenu->Items->Count; i++)
   PopupMenu->Items->Items[i]->Enabled = true;
 }
}

//---------------------------------------------------------------------------
//вид 3D графика сзади или спереди
void __fastcall TForm3D::CheckBox2Click(TObject *Sender)
{
 FillChart(!CheckBox2->Checked, 1);
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
  RestrictAndSetChartValue(i, Chart1->Series[airflow + count_z]->YValue[i] + 0.5);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::ButtonAngleDownClick(TObject *Sender)
{
 for (int i = 0; i < 16; i++ )
  RestrictAndSetChartValue(i, Chart1->Series[airflow + count_z]->YValue[i] - 0.5);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Smoothing3xClick(TObject *Sender)
{
 float* p_source_function = new float[count_x];
 for (int i = 0; i < count_x; ++i)
  p_source_function[i] = GetItem_m(airflow, i);
 MathHelpers::Smooth1D(p_source_function, GetItem_mp(airflow, 0), count_x, 3);
 delete[] p_source_function;
 for (int i = 0; i < count_x; i++ )
  RestrictAndSetChartValue(i, *GetItem_mp(airflow, i));

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::Smoothing5xClick(TObject *Sender)
{
 float* p_source_function = new float[count_x];
 for (int i = 0; i < count_x; ++i)
  p_source_function[i] = GetItem_m(airflow, i);
 MathHelpers::Smooth1D(p_source_function, GetItem_mp(airflow, 0), count_x, 5);
 delete[] p_source_function;
 for (int i = 0; i < count_x; i++ )
  RestrictAndSetChartValue(i, *GetItem_mp(airflow, i));

 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
//get item from original function
float TForm3D::GetItem_o(int z, int x)
{
 if ((z >= count_z) || (x >= count_x)) return 0.0f;
 int i  = (count_z - 1) - z;
 return *(original_function + ((i * count_x) + x));
}

//---------------------------------------------------------------------------
//get item from modified function
float TForm3D::GetItem_m(int z, int x)
{
 if ((z >= count_z) || (x >= count_x)) return 0.0f;
 int i  = (count_z - 1) - z;
 return *(modified_function + ((i * count_x) + x));
}

//---------------------------------------------------------------------------
float* TForm3D::GetItem_mp(int z, int x)
{
 if ((z >= count_z) || (x >= count_x)) return NULL;
 int i  = (count_z - 1) - z;
 return (modified_function + ((i * count_x) + x));
}

//---------------------------------------------------------------------------
//set item in modified function
int TForm3D::SetItem(int z, int x, float value)
{
 if ((z >= count_z) || (x >= count_x)) return 0;
 int i  = (count_z - 1) - z;
 *(modified_function + ((i * count_x) + x)) = value;
 return 1;
}

//---------------------------------------------------------------------------
void TForm3D::SetAirFlow(int flow)
{
 AnsiString as;
 TrackBar1->Position = flow;
 as.sprintf("%d", flow + 1);
 Label1->Caption = as;
 airflow = flow;
 MakeOneVisible(flow);
}

//---------------------------------------------------------------------------
void TForm3D::MakeOneVisible(int flow)
{
 for(int i = 0; i < count_z; i++)
 {
  Chart1->Series[i]->Active = (i==flow);
  Chart1->Series[i+count_z]->Active = (i==flow);
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
 int d, k; AnsiString as;
 if (!dir)
  d = 1, k = 0;
 else
  d = -1, k = count_z - 1;

 for(int j = 0; j < count_z; j++)
 {
  Chart1->Series[j]->Clear();
  Chart1->Series[j+count_z]->Clear();
 }

 for(int j = 0; j < count_z; j++)
 {
  for(int i = 0; i < count_x; i++)
  {
   as.sprintf(m_values_format_x.c_str(),u_slots[i]);
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
void TForm3D::RestrictAndSetChartValue(int index, double v)
{
 if (v > aai_max) v = aai_max;
 if (v < aai_min) v = aai_min;
 SetItem(airflow,index, v);
 Chart1->Series[airflow + count_z]->YValue[index] = v;
}

//---------------------------------------------------------------------------
double TForm3D::GetChartValue(int z, int index)
{
 return Chart1->Series[z + count_z]->YValue[index];
}

//---------------------------------------------------------------------------
void TForm3D::SetChartValue(int z, int index, double value)
{
 Chart1->Series[z + count_z]->YValue[index] = value;
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::WndProc(Messages::TMessage &Message)
{
 TForm::WndProc(Message);

 if (Message.Msg == WM_SYSCOMMAND && m_pOnWndActivation)
  m_pOnWndActivation(m_param_on_wnd_activation, Message.WParam);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnZeroAllPoints(TObject *Sender)
{
 for (int i = 0; i < count_x; i++ )
  RestrictAndSetChartValue(i, 0);
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnDuplicate1stPoint(TObject *Sender)
{
 for (int i = 0; i < count_x; i++ )
  RestrictAndSetChartValue(i, GetChartValue(airflow, 0));
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnBldCurveUsing1stAndLastPoints(TObject *Sender)
{
 double firstPtVal = GetChartValue(airflow, 0);
 double lastPtVal = GetChartValue(airflow, count_x - 1);
 double intrmPtCount = count_x - 1;
 for (int i = 1; i < count_x - 1; i++ )
  RestrictAndSetChartValue(i, firstPtVal + (((lastPtVal-firstPtVal) / intrmPtCount) * i));
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnZeroAllCurves(TObject *Sender)
{
 for (int z = 0; z < count_z; z++ )
 {
  for (int x = 0; x < count_x; x++ )
  {
   SetChartValue(z, x, 0);
   SetItem(z, x, 0);
  }
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnDuplicateThisCurve(TObject *Sender)
{
 for (int z = 0; z < count_z; z++ )
 {
  for (int x = 0; x < count_x; x++ )
  {
   SetChartValue(z, x, GetChartValue(airflow, x));
   SetItem(z, x, GetChartValue(airflow, x));
  }
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::OnBuildShapeUsing1stAndLastCurves(TObject *Sender)
{
 double intrmPtCount = count_z - 1;
 for (int x = 0; x < count_x; x++ )
 {
  for (int z = 1; z < count_z - 1; z++ )
  {
   double firstPtVal = GetChartValue(0, x);
   double lastPtVal = GetChartValue(count_z - 1, x);
   double value = firstPtVal + (((lastPtVal-firstPtVal) / intrmPtCount) * z);
   SetChartValue(z, x, value);
   SetItem(z, x, value);
  }
 }
 if (m_pOnChange)
  m_pOnChange(m_param_on_change);
}

//---------------------------------------------------------------------------
void __fastcall TForm3D::CtrlKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
 TWinControl* ctrl = (TWinControl*)Sender;

 if (0 != ctrl->Perform(CM_CHILDKEY, Key, (LPARAM)ctrl))
  return;

 int mask = 0;
 switch(Key)
 {
  case VK_TAB:
   mask = DLGC_WANTTAB;
   break;
//  case VK_LEFT:
//  case VK_RIGHT:
//  case VK_UP:
//  case VK_DOWN:
//   mask = DLGC_WANTARROWS;
//   break;
  case VK_RETURN: 
  case VK_EXECUTE:
  case VK_ESCAPE:
  case VK_CANCEL:
   mask = DLGC_WANTALLKEYS;
   break;
 }
 if (mask != 0)
 { 
  if ((0 == ctrl->Perform(CM_WANTSPECIALKEY, Key, 0)) &
     (ctrl->Perform(WM_GETDLGCODE, 0, 0) & (0==mask)) & 
     (this->Perform(CM_DIALOGKEY, Key, 0)))
  return;
 }
}

//---------------------------------------------------------------------------
