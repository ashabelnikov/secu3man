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

/** \file Calculations.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

namespace calc {

/** Calculates constant used in the firmware for calculation of MAF load
 * \param cyl_disp Cylinder displacement in liters
 * \param cyl_num Number if engine cylinders
 * \return constant
 */

static float calcMAFLoadConst(float cyl_disp, int cyl_num)
{
 //calculate constant used for calculation of MAF load (synthetic load). NOTE: engine size (in cc) = cyl_disp(L) * 1000 * cyl_num
 return (float)((((101.35 * 64.0 * 128.0) * 120.0) / (64.0 * 0.0012041)) / (double(cyl_disp) * 1000.0 * double(cyl_num))); 
}

}