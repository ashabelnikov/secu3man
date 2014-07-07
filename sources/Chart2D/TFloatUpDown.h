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

#ifndef _FLOATUPDOWN_H_
#define _FLOATUPDOWN_H_

#include <vcl\Classes.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\SysUtils.hpp>

class TFloatUpDown : public TCustomUpDown
{
 __published:
  __property AlignButton;
  __property TWinControl *FloatAssociate = { read=mp_floatAssociate, write=SetFloatAssociate };
  __property TUDChangingEvent OnFloatChanging = { read=m_OnFloatChanging, write=m_OnFloatChanging };
  __property TUDClickEvent OnFloatClick = { read=m_OnFloatClick, write=m_OnFloatClick };
  __property double FloatMin = { read=m_floatMin, write=SetFloatMin };
  __property double FloatMax = { read=m_floatMax, write=SetFloatMax };
  __property double FloatIncrement = { read=m_floatIncrement, write=SetFloatIncrement };
  __property double FloatPosition = { read=m_floatPosition, write=SetFloatPosition };
  __property int DecimalPlaces = {read=m_decimalPlaces, write=m_decimalPlaces };
  __property ArrowKeys;
  __property Enabled;
  __property Hint;
  __property Orientation;
  __property ParentShowHint;
  __property PopupMenu;
  __property ShowHint;
  __property TabOrder;
  __property TabStop;
  __property Visible;
  __property Wrap;
  __property OnEnter;
  __property OnExit;
  __property OnMouseDown;
  __property OnMouseMove;
  __property OnMouseUp;

 public:
  __fastcall TFloatUpDown(TComponent* Owner);

 private:
  void __fastcall CustomUpDownClick(TObject *Sender, TUDBtnType Button);
  void __fastcall CustomUpDownOnKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall SetFloatMin(double floatMin);
  void __fastcall SetFloatMax(double floatMax);
  void __fastcall SetFloatIncrement(double increment);
  void __fastcall SetFloatPosition(double position);
  void __fastcall SetFloatAssociate(TWinControl *mp_floatAssociate);

private:
  int m_decimalPlaces;
  double m_floatPosition;
  double m_floatIncrement;
  double m_floatMin;
  double m_floatMax;
  TUDChangingEvent m_OnFloatChanging;
  TUDClickEvent m_OnFloatClick;
  TWinControl *mp_floatAssociate;
  TKeyEvent m_oldOnKeyDown; 
};

#endif //_FLOATUPDOWN_H_
