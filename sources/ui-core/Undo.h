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

/** \file Undo.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include <vector>
#include <algorithm>

class UndoCntr
{
 public:
  UndoCntr()
  : mp_map(NULL)
  , m_size(0)
  {
   //empty
  }

  ~UndoCntr()
  {
   for(size_t i = 0; i < m_undo.size(); ++i)
    delete[] m_undo[i];
   for(size_t i = 0; i < m_redo.size(); ++i)
    delete[] m_redo[i];
  }
  
  void Attach(float* p_map, int size)
  {
   mp_map = p_map;
   m_size = size;
  }

  void Add()
  {
   //discard items in Redo list
   for(size_t i = 0; i < m_redo.size(); ++i)
    delete[] m_redo[i];
   m_redo.clear();

   float* p_data = new float[m_size];
   std::copy(mp_map, mp_map + m_size, p_data);
   m_undo.push_back(p_data);
  }
  
  void DoUndo()
  {
   if (!CanUndo()) return;
   //save current state to Redo list
   float* p_data = new float[m_size];
   std::copy(mp_map, mp_map + m_size, p_data);
   m_redo.push_back(p_data);
   //remove data from the Undo list and place it to current state
   p_data = m_undo.back();
   std::copy(p_data, p_data + m_size, mp_map);
   delete[] p_data;
   m_undo.pop_back();
  }

  void DoRedo()
  {
   if (!CanRedo()) return;
   //save current state to Undo list
   float* p_data = new float[m_size];
   std::copy(mp_map, mp_map + m_size, p_data);
   m_undo.push_back(p_data);
   //remove data from the Redo list and place it to current state
   p_data = m_redo.back();
   std::copy(p_data, p_data + m_size, mp_map);
   delete[] p_data;
   m_redo.pop_back();
  }

  bool CanUndo(void) {return m_undo.size();}
  bool CanRedo(void) {return m_redo.size();}

 private:
  std::vector<float*> m_undo;
  std::vector<float*> m_redo;
  float* mp_map;
  size_t m_size;
};
