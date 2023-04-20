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

/** \file RingBuffer.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <numeric>

#define RINGBUFF_SIZE 64

struct RingBuffItem
{
 RingBuffItem()
 : m_idx(0)
 , m_result(0)
 , m_avnum(0)
 , m_size(0)
 , m_sum(.0f)
 {
  std::fill(m_buff, m_buff + RINGBUFF_SIZE, 0.0f);
 }

 void Append(float value)    //update ring buffer
 {
  if (m_avnum > 0)
  {
   m_sum-=m_buff[m_idx];     //"remove" old value
   m_sum+= value;            //add new value

   m_buff[m_idx++] = value;  //remember new value 
   if (m_idx >= m_avnum)
    m_idx = 0;   
                             //limit size to the specified value
   if (m_size < m_avnum)
    ++m_size;  
  }
  else
  {
   m_result = value;
  }
 }

 void Calculate(void)
 {
  if (m_size > 0)
  {
   m_result = m_sum / m_size;
  }
 }

 float m_buff[RINGBUFF_SIZE]; //allocate memory statically to decrease loss of speed
 int m_idx;      //current index in the ring buffer
 int m_avnum;    //set maximum size of the ring buffer
 int m_size;     //actual size of data in the ring buffer
 float m_sum;    //sum of values in the ring buffer
 float m_result; //arithmetic mean result
};
