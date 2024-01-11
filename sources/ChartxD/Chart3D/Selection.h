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

/** \file Selection.h
 * \author Alexey A. Shabelnikov
 */

#ifndef _SELECTION_H_
#define _SELECTION_H_
#include  <vector>

//---------------------------------------------------------------------------
class Selection
{
 public:
  Selection();
 ~Selection();

  void SetDimentions(int z, int x);

  int Left(void); //get first from the left
  int Right(void); //get first from the right
  int Up(void);
  int Down(void);
  void PopLeft(int z); //pop first from the left
  void PopRight(int z);//pop first from the right
  void Clear(void); //clear selection
  void Clear(int z); //clear selection of a single row
  int Size(void);
  int Size(int z);
  void Set(int z, int x, bool value) { m_selpts[z][x] = value; }
  bool Get(int z, int x) { return m_selpts[z][x]; }
  bool* Get(int z) { return &m_selpts[z][0]; }  
  bool* Get() { return &m_selpts[0][0]; }
  std::vector<int> GetSelIdxs(int z);
  void CopyRow(int zSrc, int zDst);
  void InvertZ(void);

 private:
  bool m_selpts[16][16]; //[z][x]
  int m_count_z;
  int m_count_x;
};
//---------------------------------------------------------------------------

#endif //_SELECTION_H_
