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

/** \file Selection.cpp
 * \author Alexey A. Shabelnikov
 */

#include <algorithm>
#include "Selection.h"

//---------------------------------------------------------------------------
Selection::Selection()
{
 std::fill(&m_selpts[0], &m_selpts[0] + SELBUF_MAX, false);
 _selpts(0,0) = true;
}

//---------------------------------------------------------------------------
Selection::~Selection()
{
 //empty
}

//---------------------------------------------------------------------------
void Selection::SetDimentions(int z, int x)
{
 m_count_z = z;
 m_count_x = x;
}

//---------------------------------------------------------------------------
int Selection::Left(void) //get first from the left
{
 for(int z = 0; z < m_count_z; ++z)
 {
  for(int x = 0; x < m_count_x; ++x)
  {
   if (true==_selpts(z, x))
    return x;
  }
 }
 return 0;
}

//---------------------------------------------------------------------------
int Selection::Right(void) //get first from the right
{
 for(int z = 0; z < m_count_z; ++z)
 {
  for(int x = m_count_x - 1; x >= 0; --x)
  {
   if (true==_selpts(z, x))
    return x;
  }
 }
 return 0;
}

//---------------------------------------------------------------------------
int Selection::Up(void)
{
 for(int z = m_count_z - 1; z >= 0; --z)
 {
  if (std::find(&_selpts(z, 0), &_selpts(z, 0) + m_count_x, true) != &_selpts(z, 0) + m_count_x)
   return z;
 }
 return 0;
}

//---------------------------------------------------------------------------
int Selection::Down(void)
{
 for(int z = 0; z < m_count_z; ++z)
 {
  if (std::find(&_selpts(z, 0), &_selpts(z, 0) + m_count_x, true) != &_selpts(z, 0) + m_count_x)
   return z;
 }
 return 0;
}

//---------------------------------------------------------------------------
int Selection::Size(int z)
{
 int s = 0;
 for(int x = 0; x < m_count_x; ++x)
 {
  s+=_selpts(z, x);   
 }
 return s;
}

//---------------------------------------------------------------------------
int Selection::Size(void)
{
 int s = 0;
 for(int z = 0; z < m_count_z; ++z)
  for(int x = 0; x < m_count_x; ++x)
   s+=_selpts(z, x);
 return s;
}

//---------------------------------------------------------------------------
void Selection::PopRight(int z) //pop first from the right
{
 for(int x = m_count_x - 1; x >= 0; --x)
 {
  if (true==_selpts(z, x))
  {
   _selpts(z, x) = false;
   return;
  }
 }
 return;
}

//---------------------------------------------------------------------------
void Selection::PopLeft(int z) //pop first from the left
{
 for(int x = 0; x < m_count_x; ++x)
 {
  if (true==_selpts(z, x))
  {
   _selpts(z, x) = false;
   return;
  }
 }
 return;
}

//---------------------------------------------------------------------------
void Selection::Clear(void)
{
 for(int z = 0; z < m_count_z; ++z)
 {
  std::fill(&_selpts(z, 0), &_selpts(z, 0) + m_count_x, false);
 }
}

//---------------------------------------------------------------------------
void Selection::Clear(int z) //clear specified row
{
 std::fill(&_selpts(z, 0), &_selpts(z, 0) + m_count_x, false);
}

//---------------------------------------------------------------------------
void Selection::CopyRow(int zSrc, int zDst)
{
 std::copy(&_selpts(zSrc, 0), &_selpts(zSrc, 0) + m_count_x, &_selpts(zDst, 0));  
}

//---------------------------------------------------------------------------
void Selection::InvertZ(void)
{
 bool sel[SELBUF_MAX];
 for(int z = 0; z < m_count_z; ++z)
  std::copy(&_selpts(z, 0), &_selpts(z, 0) + m_count_x, &sel[z * m_count_x]);
 for(int z = 0; z < m_count_z; ++z)
  std::copy(&sel[z * m_count_x], &sel[z * m_count_x] + m_count_x, &_selpts(m_count_z-1-z, 0));
}

//---------------------------------------------------------------------------
std::vector<int> Selection::GetSelIdxs(int z)
{
 std::vector<int> idxs;
 for (int i = 0; i < m_count_x; ++i)
 {
  if (_selpts(z, i))
   idxs.push_back(i);
 }
 return idxs;
}

//---------------------------------------------------------------------------
