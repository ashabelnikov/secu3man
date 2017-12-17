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

/** \file ddx_helpers.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "ddx_helpers.h"

void DDX_HELPERS_API DDX_Check_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value )
{
 BOOL b_value;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_Check(pDX, nIDC, b_value);
  value = (b_value) ? 1 : 0;
 }
 else
 {
  b_value = (value) ? TRUE : FALSE;
  DDX_Check(pDX, nIDC, b_value);
 }
}

void DDX_HELPERS_API DDX_Check_bool(CDataExchange* pDX, int nIDC, bool& value )
{
 BOOL b_value;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_Check(pDX, nIDC, b_value);
  value = (b_value) ? true : false;
 }
 else
 {
  b_value = (value) ? TRUE : FALSE;
  DDX_Check(pDX, nIDC, b_value);
 }
}

void DDX_HELPERS_API DDX_CBIndex_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& index)
{
 int i_index;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_CBIndex(pDX, nIDC, i_index);
  index = (UCHAR)i_index;
 }
 else
 {
  i_index = (int)index;
  DDX_CBIndex(pDX, nIDC, i_index);
 }
}

void DDX_HELPERS_API DDX_CBIndex_int(CDataExchange* pDX, int nIDC, int& index)
{
 int i_index;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_CBIndex(pDX, nIDC, i_index);
  index = i_index;
 }
 else
 {
  i_index = index;
  DDX_CBIndex(pDX, nIDC, i_index);
 }
}

void DDX_HELPERS_API DDX_Radio_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value )
{
 int b_value;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_Radio(pDX, nIDC, b_value);
  value = (b_value) ? 1 : 0;
 }
 else
 {
  b_value = (value) ? TRUE : FALSE;
  DDX_Radio(pDX, nIDC, b_value);
 }
}

void DDX_HELPERS_API DDX_Radio_bool(CDataExchange* pDX, int nIDC, bool& value )
{
 int b_value;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_Radio(pDX, nIDC, b_value);
  value = (b_value) ? true : false;
 }
 else
 {
  b_value = (value) ? TRUE : FALSE;
  DDX_Radio(pDX, nIDC, b_value);
 }
}

void DDX_HELPERS_API DDX_Text_Fmt(CDataExchange* pDX, int nIDC, float& value, LPCTSTR OutFmt, LPCTSTR InFmt)
{
 CString temp;
 if (!pDX->m_bSaveAndValidate)
  temp.Format(OutFmt, value);

 DDX_Text(pDX, nIDC, temp);

 if (pDX->m_bSaveAndValidate)
  _stscanf(temp, InFmt, &value);
}

void DDX_HELPERS_API DDX_Text_Fmt(CDataExchange* pDX, int nIDC, int& value, LPCTSTR OutFmt, LPCTSTR InFmt)
{
 CString temp;
 if (!pDX->m_bSaveAndValidate)
  temp.Format(OutFmt, value);

 DDX_Text(pDX, nIDC, temp);

 if (pDX->m_bSaveAndValidate)
  _stscanf(temp, InFmt, &value);
}
