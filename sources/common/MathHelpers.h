/* Common utilities - mathematical helpers
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
             e-mail: shabelnikov@secu-3.org
             Microprocessor systems - design & programming.
*/

/** \file MathHelpers.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include <math.h>

namespace MathHelpers
{
  //For correct averaging to integer values
  //Averaging to int
  template <class FPN>
  inline int Round(FPN i_value)
  {
   return (int)((i_value) + 0.5f - (FPN)((i_value) < 0));
  }

  //Averaging to specified type
  template <class ITN, class FPN>
  inline ITN Round(FPN i_value)
  {
   return (ITN)((i_value) + 0.5f - (FPN)((i_value) < 0));
  }

  //Round floating point number to the value with 0.1 precision (e.g. 14.68 ==> 14.7)
  template <class FPN>
  FPN RoundP1(FPN f)
  {
   return floor((f * 10 ) + 0.5f) / 10;
  }

  //Convolution for 1D function
  //i_kernel_size - kernel size, any value
  template <class T>
  bool Convolve1D(T* ip_in, T* op_out, size_t i_data_size, const T* ip_kernel, size_t i_kernel_size)
  {
   size_t i, j, k;

   //Check correctness of input arguments
   if(!ip_in || !op_out || !ip_kernel)
    return false;
   if(i_data_size <=0 || i_kernel_size <= 0)
    return false;

   // start convolution from out[i_kernel_size-1] to out[i_data_size-1] (last)
   for(i = i_kernel_size-1; i < i_data_size; ++i)
   {
    op_out[i] = 0;
    for(j = i, k = 0; k < i_kernel_size; --j, ++k)
     op_out[i] += ip_in[j] * ip_kernel[k];
   }

   // continue convolution from out[0] to out[i_kernel_size-2]
   for(i = 0; i < i_kernel_size - 1; ++i)
   {
    op_out[i] = 0;
    for(j = i, k = 0; ((int)j) >= 0; --j, ++k)
     op_out[i] += ip_in[j] * ip_kernel[k];
   }
   return true;
  }

  //Smoothing for 1D function using mooving avarage method with symmetrical kernel
  //i_kernel_size - нечетное число! Например 1,3,5,7...
  template <class T>
  bool Smooth1D(T* ip_in, T* op_out, size_t i_data_size, size_t i_kernel_size)
  {
   //Check input parameters
   if (!(i_kernel_size & 1) || i_kernel_size <= 0)
    return false;
   if(!ip_in || !op_out || i_data_size <=0)
    return false;

   int j, k, k2 = i_kernel_size / 2;
   for(size_t i = 0; i < i_data_size; ++i)
   {
    op_out[i] = 0;
    T divisor = 0;
    for(k = -k2; k <= k2; ++k)
    {
     j = (i + k);
     if (j >= 0 && j < (int)i_data_size)
     {
      op_out[i]+= ip_in[j];
      divisor+= 1;
     }
    }
    op_out[i]/= divisor;
   }
   return true;
  }

  //Restrict specified value to min and max
  template <class T>
  T RestrictValue(T i_value, T i_min, T i_max)
  {
   if (i_value < i_min)
    i_value = i_min;
   if (i_value > i_max)
    i_value = i_max;
   return i_value;
  }
}
