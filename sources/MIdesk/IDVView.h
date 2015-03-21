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

/** \file IDVView.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

namespace SECU3IO
{
 struct DbgvarDat;
}

class IDVView
{
 public:
  virtual void Show(bool show) = 0;                                //показать/спрятать контент представления
  virtual void Enable(bool enable) = 0;                            //разрешение/запрещение представления
  virtual bool IsEnabled(void) = 0;                                //returns true if window enabled
  virtual void SetValues(const SECU3IO::DbgvarDat* i_values) = 0;  //загоняет данные в представление
  virtual void GetValues(SECU3IO::DbgvarDat* o_values) = 0;        //извлекает данные из представления
  virtual void SetUpdatePeriod(unsigned int i_period) = 0;         //update period for internal controls
};
