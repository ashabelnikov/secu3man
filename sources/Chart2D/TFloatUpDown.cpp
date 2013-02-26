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

#include <vcl\vcl.h>
#pragma hdrstop

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include "TFloatUpDown.h"

//---------------------------------------------------------------------------
namespace
{
 void __fastcall Register()
 {
  TComponentClass classes[1] = {__classid(TFloatUpDown)};
  RegisterComponents("secu-3_vcl", classes, 0);
 }

 template<class T>
 void RestrictVal(T& o_value, T i_minVal, T i_maxVal)
 {
  if (o_value < i_minVal)
   o_value = i_minVal;
  if (o_value > i_maxVal)
   o_value = i_maxVal;  
 }

 void RoundNearestMultiple(double &o_value, double i_multiple)
 {
  double mod = fmod(o_value, i_multiple);
  if (mod)
  {
   if (mod > i_multiple / 2)
    o_value = ceil(o_value / i_multiple) * i_multiple;
   else
    o_value = floor(o_value / i_multiple) * i_multiple;
  }
 }
}

//---------------------------------------------------------------------------
__fastcall TFloatUpDown::TFloatUpDown(TComponent* Owner)
: TCustomUpDown(Owner)
, m_decimalPlaces(2)
, m_floatMin(0)
, m_floatMax(100)
, m_floatPosition(0)
, m_floatIncrement(1)
, m_OnFloatChanging(NULL)
, m_oldOnKeyDown(NULL)
{
 Min = SHRT_MIN;
 Max = SHRT_MAX;
 Position = 0;
 Increment = 1;
 OnClick = CustomUpDownClick; 
}

//---------------------------------------------------------------------------
static inline TFloatUpDown *ValidCtrCheck()
{
 return new TFloatUpDown(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TFloatUpDown::CustomUpDownClick(TObject *Sender,TUDBtnType Button)
{
 if (m_OnFloatClick) 
  m_OnFloatClick(Sender,Button);

 double CurPos = m_floatPosition, NewPos;

 if (mp_floatAssociate)
 {
  TCustomEdit *CustomEdit = NULL;
  try
  {
   CustomEdit = dynamic_cast<TCustomEdit*>(mp_floatAssociate);
  }
  catch (Exception &exception) {};
  if (CustomEdit)
  {
   double value = 0;
   int result = sscanf(CustomEdit->Text.c_str(), "%lf", &value);
   if (result == 1)
    CurPos = value;
   else
    CurPos = 0;
  }
 }

 //Change current position depending on pressed button
 if (Button == btNext)
  NewPos = CurPos + m_floatIncrement;
 else
  NewPos = CurPos - m_floatIncrement;

 // Rounding up to the nearest multiple of an increment value
 RoundNearestMultiple(NewPos, m_floatIncrement);

 //Restrict value using our min. and max.
 RestrictVal(NewPos, m_floatMin, m_floatMax);

 if (m_OnFloatChanging)
 {
  bool AllowChange = true;
  m_OnFloatChanging(this, AllowChange);
  if (!AllowChange)
   return;
 }
 SetFloatPosition(NewPos);
}

//---------------------------------------------------------------------------
void __fastcall TFloatUpDown::CustomUpDownOnKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
 //Simulate buttons pressing
 if (Key == VK_UP)  Click(btNext);
 else if (Key == VK_DOWN) Click(btPrev);

 if (m_oldOnKeyDown)
  m_oldOnKeyDown(Sender, Key, Shift); //let our buddy's event listener to precess OnKeyDown
}

//---------------------------------------------------------------------------
void __fastcall TFloatUpDown::SetFloatMin(double floatMin)
{
 m_floatMin = floatMin;
 if (m_floatMin > m_floatMax)
  m_floatMax = m_floatMin;
 if (m_floatPosition < m_floatMin)
  SetFloatPosition(m_floatMin);
}

//---------------------------------------------------------------------------
void __fastcall TFloatUpDown::SetFloatMax(double floatMax)
{
 m_floatMax = floatMax;
 if (m_floatMin > m_floatMax)
  m_floatMin = m_floatMax;
 if (m_floatPosition > m_floatMax)
  m_floatPosition = m_floatMax;
}

//---------------------------------------------------------------------------
void __fastcall TFloatUpDown::SetFloatIncrement(double increment)
{
 m_floatIncrement = increment;
}

//---------------------------------------------------------------------------
void __fastcall TFloatUpDown::SetFloatPosition(double position)
{
 if (position < m_floatMin)
  position = m_floatMin;
 if (position > m_floatMax)
  position = m_floatMax;
 m_floatPosition = position;

 if (mp_floatAssociate)
 {
  TCustomEdit *CustomEdit = NULL;
  try
  {
   CustomEdit = dynamic_cast<TCustomEdit*>(mp_floatAssociate);
  }
  catch (Exception &exception) {};
  if (CustomEdit)
  {
   AnsiString as;
   as.sprintf("%.*lf", m_decimalPlaces, m_floatPosition);
   CustomEdit->Text = as;
  }
 }
}

//---------------------------------------------------------------------------
void __fastcall TFloatUpDown::SetFloatAssociate(TWinControl *ip_floatAssociate)
{
 Associate = ip_floatAssociate; //move and size control
 if (ArrowKeys && ip_floatAssociate)
 {
  TEdit* buddyControl = static_cast<TEdit*>(Associate);
  m_oldOnKeyDown = buddyControl->OnKeyDown;         //save old 
  buddyControl->OnKeyDown = CustomUpDownOnKeyDown;  //set new
 }
 Associate = this;
 mp_floatAssociate = ip_floatAssociate;
 SetFloatPosition(m_floatPosition);
}

//---------------------------------------------------------------------------
