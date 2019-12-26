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

#define RINGBUFF_SIZE 32

struct RingBuffItem
{
 RingBuffItem()
 : m_idx(0)
 , m_result(0)
 , m_avnum(0)
 {
  std::fill(m_buff, m_buff + RINGBUFF_SIZE, 0.0f);
 }

 void Append(float value) //update ring buffer
 {
  if (m_avnum > 0)
  {
   m_buff[m_idx++] = value;
   if (m_idx >= m_avnum)
    m_idx = 0;
  }
  else
   m_result = value;
 }

 void Calculate(void)
 {
  if (m_avnum > 0)
  {
   float sum = std::accumulate(m_buff, m_buff + m_avnum, 0.0f);
   m_result = sum / m_avnum;
  }
 }

 float m_buff[RINGBUFF_SIZE];
 int m_idx;
 int m_avnum;
 float m_result;
};
