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

#pragma once

class IMeasInstrument
{
 public:
  IMeasInstrument() {};
  virtual ~IMeasInstrument() {};

  //interface for measurement instrument
  virtual void Create(void) = 0;
  virtual void Scale(float i_x_factor, float i_y_factor) = 0;

  virtual void SetValue(float value) = 0;
  virtual float GetValue(void) = 0;

  //скрытие/отображение прибора
  virtual void Show(bool show) = 0;

  //разрешение/запрещение прибора
  virtual void Enable(bool enable) = 0;

  //прибор видим или скрыт ?
  virtual bool IsVisible(void) = 0;

  //прибор разрешен или запрещен ?
  virtual bool IsEnabled(void) = 0;

  //установка пределов измерения
  virtual void SetLimits(float loLimit, float upLimit) = 0;

  //установка количества делений
  virtual void SetTicks(int number) = 0;
};
