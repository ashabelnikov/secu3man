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
#include <vector>
#include <algorithm>

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
 inline FPN RoundP1(FPN f)
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
 bool Smooth1D(T* ip_in, T* op_out, size_t i_data_size, size_t i_kernel_size, bool* ip_mask = NULL)
 {
  //Check input parameters
  if (!(i_kernel_size & 1) || i_kernel_size <= 0)
   return false;
  if(!ip_in || !op_out || i_data_size <=0)
   return false;

  int j, k, k2 = i_kernel_size / 2;
  for(size_t i = 0; i < i_data_size; ++i)
  {
   if (ip_mask && !ip_mask[i])
   {
    op_out[i] = ip_in[i];
    continue;    
   }  
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

 //Smoothing for 2D function using mooving avarage method with symmetrical kernel
 //in - input buffer
 //out - output buffer (result)
 //ySize - number of rows
 //xSize - number of columns
 //i_kernel_size - must be odd number! e.g. 1,3,5,7...
 //
 // | . . . .
 // y . . . .
 // | . . . .
 //  ---X---
 template <class T>
 bool Smooth2D(T *in, T* out, int ySize, int xSize, size_t i_kernel_size, bool* ip_mask = NULL)
 {
  //Check input parameters
  if (!(i_kernel_size & 1) || i_kernel_size == 0)
   return false;
  if(ySize <=0 || xSize <=0)
   return false;

  int ii, jj, ki, kj, k2 = i_kernel_size / 2;

  for(int i = 0; i < ySize; ++i)
  {
   for(int j = 0; j < xSize; ++j)
   {
    int oi = i*xSize + j;
    if (ip_mask && !ip_mask[oi])
    {
     out[oi] = in[oi];
     continue;    
    }  
    out[oi] = 0;
    T divisor = 0;
    for(ki = -k2; ki <= k2; ++ki)
    {
     for(kj = -k2; kj <= k2; ++kj)
     {
      ii = (i + ki);
      jj = (j + kj);
      if (ii >= 0 && ii < xSize && jj >= 0 && jj < ySize)
      {
       out[oi]+= in[ii*xSize + jj];
       divisor+= 1;
      }
     }
    }
    out[oi]/= divisor;
   }
  }
  return true;
 }

 //Sigma-filter for 2D function using mooving avarage method with symmetrical kernel
 //in - input buffer
 //out - output buffer (result)
 //ySize - number of rows
 //xSize - number of columns
 //i_kernel_size - must be odd number! e.g. 1,3,5,7...
 //
 // | . . . .
 // y . . . .
 // | . . . .
 //  ---X---
 template <class T>
 bool SigmaFilter2D(T *in, T* out, int ySize, int xSize, size_t i_kernel_size, bool median = false)
 {
  //Check input parameters
  if (!(i_kernel_size & 1) || i_kernel_size == 0)
   return false;
  if(ySize <=0 || xSize <=0)
   return false;

  int ii, jj, ki, kj, k2 = i_kernel_size / 2, n;

  for(int i = 0; i < ySize; ++i)
  {
   for(int j = 0; j < xSize; ++j)
   {
    //calculate arithmetic mean
    T m = 0; n = 0;
    for(ki = -k2; ki <= k2; ++ki)
    {
     for(kj = -k2; kj <= k2; ++kj)
     {
      ii = (i + ki);
      jj = (j + kj);
      if (ii >= 0 && ii < xSize && jj >= 0 && jj < ySize)
      {
       m+= in[ii*xSize + jj];
       n++;
      }
     }
    }
    if (0==n)
     m = 0;
    else
     m/=((T)n);
    //calculate standard deviation
    T s = 0;
    for(ki = -k2; ki <= k2; ++ki)
    {
     for(kj = -k2; kj <= k2; ++kj)
     {
      ii = (i + ki);
      jj = (j + kj);
      if (ii >= 0 && ii < xSize && jj >= 0 && jj < ySize)
       s+= pow(in[ii*xSize + jj] - m, 2.0f);
     }
    }
    if (0==n)
     s = 0;
    else
     s = sqrt(s / ((T)n));

    if (median)
    {
     std::vector<T> v;
     //filter values using median filter
     for(ki = -k2; ki <= k2; ++ki)
     {
      for(kj = -k2; kj <= k2; ++kj)
      {
       ii = (i + ki);
       jj = (j + kj);
       if (ii >= 0 && ii < xSize && jj >= 0 && jj < ySize)
        v.push_back(in[ii*xSize + jj]);      
      }
     }
     std::sort(v.begin(), v.end());
     out[i*xSize + j] = v[v.size()/2];
    }
    else
    {
     //filter values using "3 sigma rule"
     m = 0, n= 0;
     for(ki = -k2; ki <= k2; ++ki)
     {
      for(kj = -k2; kj <= k2; ++kj)
      {
       ii = (i + ki);
       jj = (j + kj);
       if (ii >= 0 && ii < xSize && jj >= 0 && jj < ySize)
       {
        if (fabs(in[ii*xSize + jj] - in[i*xSize + j]) < (3.0f * s))
         m+=in[ii*xSize + jj], ++n;
       }
      }
     }
     if (n > 0)
      out[i*xSize + j] = m / n;
     else
      out[i*xSize + j] = in[i*xSize + j];
    }  
   }
  }
  return true;
 }

  //Restrict specified value to min and max
 template <class T>
 inline T RestrictValue(T i_value, T i_min, T i_max)
 {
  if (i_value < i_min)
   i_value = i_min;
  if (i_value > i_max)
   i_value = i_max;
  return i_value;
 }

 template <class T>
 std::vector<T> BuildGridFromRange(T begin, T end, size_t num, bool rev = false)
 {
  ASSERT(num);
  std::vector<T> grid;
  grid.reserve(num);
  if (rev)
  {
   for (int i = num - 1; i >= 0; --i)
    grid.push_back(begin + ((end - begin) * i) / (num - 1));
  }
  else
  {
   for (size_t i = 0; i < num; ++i)
    grid.push_back(begin + ((end - begin) * i) / (num - 1));
  }
  return grid; 
 }

 static void AxisCellLookup(float& arg, const float *Bins, int size, int& p1, int& p2)
 {
  int i;

  if (arg > Bins[size-1])
   arg = Bins[size-1];
  if (arg < Bins[0])
   arg = Bins[0];

  for(i = size - 2; i >= 0; --i)
   if (arg >= Bins[i]) break; 

  p1 = i, p2 = i + 1; //save results
 }

 template<int ySize, int xSize>
 float static BilinearInterpolation(float x, float y, float (&function)[ySize][xSize], const float* xBins, const float* yBins)
 {
  int xi, yi, xi1, yi1;

  AxisCellLookup(x, xBins, xSize, xi, xi1);
  AxisCellLookup(y, yBins, ySize, yi, yi1);

  float f2 = function[yi1][xi] + ((function[yi1][xi1] - function[yi1][xi]) * (x - xBins[xi])) / (xBins[xi1] - xBins[xi]);
  float f1 = function[yi][xi] + ((function[yi][xi1] - function[yi][xi]) * (x - xBins[xi])) / (xBins[xi1] - xBins[xi]);
  return f1 + ((f2 - f1) * (y - yBins[yi])) / (yBins[yi1] - yBins[yi]);
 }

 template<int xSize>
 float static LinearInterpolation(float x, float (&function)[xSize], const float* xBins)
 {
  int xi, xi1;
  AxisCellLookup(x, xBins, xSize, xi, xi1);
  return (function[xi] + ((function[xi1] - function[xi]) * (x - xBins[xi])) / (xBins[xi1] - xBins[xi]));
 }

 template <class T>
 static T Distance(T x, T y, T x0, T y0)
 {
  return sqrt(pow((x - x0), (T)2) + pow((y - y0), (T)2));
 }

 static float InjTimValueTransform(int it_mode, float source, int direction)
 {
  float value = 0;
  if (direction)
  { //from chart
   switch(it_mode)
   {
    case 0: //BTDC
     value = source;
     break;
    case 1: //ATDC
     value = 720.0f - source;
     break;
    case 2: //-360...360 BTDC
     value = 720.0f + source;    
     break;
    case 3: //-360...360 ATDC    
     value = 720.0f + (-source);
     break;
   }
  }
  else
  { //to chart
   switch(it_mode)
   {
    case 0: //BTDC
     value = (source > 720.0f) ? source - 720.0f : source;
     break;
    case 1: //ATDC
     value = 720.0f - ((source > 720.0f) ? source - 720.0f : source);
     break;
    case 2: //-360...360 BTDC
     if (source > 720.0f)
      value = source - 720.0f;
     else
      value = (source < 360.0f) ? source : -(720.0f - source);
     break;
    case 3: //-360...360 ATDC
     if (source > 720.0f)
      value = source - 720.0f;
     else
      value = (source < 360.0f) ? source : -(720.0f - source);
     value = -value;
     break;
   }
  }
  return value;
 }


 //Compare two floating point numbers
 //Note: this function may work incorrectly with small values of e due to features of floating point numbers' representation.
 template <class T>
 inline bool IsEqualFlt(T a, T b, T e)
 {
  return fabs(a - b) < e;
 }

 //get item from array as if its order of rows is reversed
 //nrow - number of array's rows, ncol - number of array's columns
 template <class T, size_t nrow, size_t ncol>
 T RevLineGet(T* p_arr, size_t i)
 {
  return p_arr[(((nrow - 1) - (i / ncol)) * ncol) + (i % ncol)];
 }

 //get items from array in reverse order
 //nitems - size of array
 template <class T, size_t nitems>
 T RevItemGet(T* p_arr, size_t i)
 {
  return p_arr[(nitems - 1) - i];
 }

}
