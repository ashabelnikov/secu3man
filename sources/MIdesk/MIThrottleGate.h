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

/** \file MIThrottleGate.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "IMeasInstrument.h"
#include "ui-core/LEDIndicator.h"

class AFX_EXT_CLASS CMIThrottleGate : public IMeasInstrument
{
 public:
  CMIThrottleGate();
  virtual ~CMIThrottleGate();

  void DDX_Controls(CDataExchange* pDX, int nIDC_led, int nIDC_caption);

  //-------interface-----------------------
  virtual void Create(void);
  virtual void Scale(float i_x_factor, float i_y_factor, bool repaint = true);
  virtual void SetValue(float value);
  virtual float GetValue(void);
  virtual void Show(bool show);
  virtual void Enable(bool enable);
  virtual bool IsVisible(void);
  virtual bool IsEnabled(void);
  virtual void SetLimits(float loLimit, float upLimit);
  virtual void SetTicks(int number);
  //---------------------------------------

  void SetPosition(float value); //specific function
  float GetPosition(void) const; //specific function

 private:
  bool m_prev_enable;
  CLEDIndicator m_led;
  CStatic m_caption;
  float m_loLimit;
  float m_upLimit;
  float m_value;
  CRect m_rect[2];
};
