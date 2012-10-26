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

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Enum Types
enum LEDMemberEnum
{
 led_rect,  //рамка
 led_on,    //включенное состояние
 led_off    //выключенное сщстояние
};

/////////////////////////////////////////////////////////////////////////////
// CLEDIndicator window

class AFX_EXT_CLASS CLEDIndicator : public CStatic
{
 public:
  CLEDIndicator();
  virtual ~CLEDIndicator();

  void SetColor(enum LEDMemberEnum led_member, COLORREF Color);
  void GetColor(enum LEDMemberEnum led_member, COLORREF* pColor);

  void SetState(bool state);
  bool GetState(void);

 protected:
  void ActuateColors();

  // Generated message map functions
  afx_msg void OnPaint();
  DECLARE_MESSAGE_MAP()

 private:
  CBrush m_onBrush;
  CBrush m_offBrush;
  CPen   m_rectPen;

  COLORREF m_onColor;
  COLORREF m_offColor;
  COLORREF m_rectColor;

  int m_rectWidth;
  bool m_state;
};

/////////////////////////////////////////////////////////////////////////////
