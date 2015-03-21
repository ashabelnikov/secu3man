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

/** \file EditEx.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "EditEx.h"

static const COLORREF ErrColor = RGB(255,120,120);

CEditEx::CEditEx(int i_mode /* = MODE_STRING*/, bool i_own_ddv /*=false*/)
: m_mode(i_mode)
, m_DecimalPlaces(4)
, m_own_ddv(i_own_ddv)
, m_ddv_status(true)
, m_redBrush(ErrColor)
, m_min(0), m_max(100)
{
 //empty
}

CEditEx::~CEditEx()
{
 //empty
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
 ON_WM_CHAR()
 ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//-------------------------------------------------------------
//фильтрует символы для целых десятичных чисел
bool CEditEx::OnChar_int(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 CString str;

 //если неразрешено вводить отрицательные числа, то пропускаем "-"
 if ((nChar == '-') && !(m_mode & MODE_SIGNED))
 {
  return false;
 }

 if(!(nChar >= '0' && nChar <= '9' || nChar == '-' || nChar == '\b'))
  return false;

 GetWindowText(str);

 if(nChar == '-' && !str.IsEmpty() && str[0] == '-')
  return false;

 int nStartChar, nEndChar;
 GetSel(nStartChar, nEndChar);

 if(nChar == '\b' && nStartChar <= 0)
  return false;

 if(nChar == '-' && (nStartChar != 0 || nEndChar != 0))
  return false;

 return true;
}

//-------------------------------------------------------------
//фильтрует символы для чисел с плавающей запятой
bool CEditEx::OnChar_float(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 CString str;

 //если неразрешено вводить отрицательные числа, то пропускаем "-"
 if ((nChar == '-') && !(m_mode & MODE_SIGNED))
 {
  return false;
 }

 if(!(nChar >= '0' && nChar <= '9' || nChar == '.' || nChar == '-' || nChar == '\b'))
  return false;

 GetWindowText(str);

 if(nChar == '-' && !str.IsEmpty() && str[0] == '-')
  return false;

 if(nChar == '.' && (str.Find('.') >= 0 || str.IsEmpty()))
  return false;

 int nStartChar, nEndChar;
 GetSel(nStartChar, nEndChar);

 if(nChar == '\b' && nStartChar <= 0)
  return false;

 if(nChar == '-' && (nStartChar != 0 || nEndChar != 0))
  return false;

 return true;
}

//-------------------------------------------------------------
//фильтрует символы для шестнадцатиричных чисел
bool CEditEx::OnChar_hex(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 CString str;
 if(!(nChar >= 'A' && nChar <= 'F' || nChar >= 'a' && nChar <= 'f'
   || nChar >= '0' && nChar <= '9' || nChar == '\b'))
  return false;

 GetWindowText(str);

 int nStartChar, nEndChar;
 GetSel(nStartChar, nEndChar);

 if(nChar == '\b' && nStartChar <= 0)
  return false;

 if(nChar != '\b' && nEndChar - nStartChar == 0 && str.GetLength() >= 8)
  return false;

 return true;
}

//-------------------------------------------------------------
//фильтрует символы для шестнадцатиричной строки
bool CEditEx::OnChar_hexstr(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 CString str;
 if(!(nChar >= 'A' && nChar <= 'F' || nChar >= 'a' && nChar <= 'f'
   || nChar >= '0' && nChar <= '9' || nChar == '\b'))
  return false;

 GetWindowText(str);

 int nStartChar, nEndChar;
 GetSel(nStartChar, nEndChar);

 if(nChar == '\b' && nStartChar <= 0)
  return false;

 return true;
}

//-------------------------------------------------------------
//фильтруем вводимые символы в зависимомти от режима
void CEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 CString str;
 bool result = false;

 switch(m_mode & MODE_MASK)
 {
  case MODE_INT:
   result = OnChar_int(nChar, nRepCnt, nFlags);
   break;

  case MODE_FLOAT:
   result = OnChar_float(nChar, nRepCnt, nFlags);
   break;

  case MODE_HEX:
   result = OnChar_hex(nChar, nRepCnt, nFlags);
   break;

  case MODE_STRING:
   result = true; //there are no filtering for string
   break;
 
  case MODE_HEXSTR:
   result = OnChar_hexstr(nChar, nRepCnt, nFlags);
   break;
 }

 if (result)
  CEdit::OnChar(nChar, nRepCnt, nFlags);
}

//-------------------------------------------------------------
bool CEditEx::GetValue(float& o_value)
{
 CString s;
 GetWindowText(s);
 int result = 0;
 float f_value = 0.0f;
 int   i_value = 0;

 switch(m_mode & MODE_MASK)
 {
  case MODE_INT:
   result = _stscanf(s, _T("%d"), &i_value);
   o_value = (float)i_value;
   break;

  case MODE_FLOAT:
   result = _stscanf(s, _T("%f"), &f_value);
   o_value = (float)f_value;
   break;

  case MODE_HEX:
   result = _stscanf(s, _T("%x"), &i_value);
   o_value = (float)i_value;
   break;

  default:
   ASSERT(0);
 }

 return (result == 1);
}

//-------------------------------------------------------------
bool CEditEx::SetValue(float i_value)
{
 CString s;

 switch(m_mode & MODE_MASK)
 {
  case MODE_INT:
   s.Format(_T("%0*d"),m_DecimalPlaces,(int)i_value);
   break;

  case MODE_FLOAT:
   s.Format(_T("%.*f"),m_DecimalPlaces,(float)i_value);
   break;

  case MODE_HEX:
   s.Format(_T("%0*x"),m_DecimalPlaces,(int)i_value);
   s.MakeUpper();
   break;

  default:
   ASSERT(0);
 }

 SetWindowText(s);
 return true;
}

//-------------------------------------------------------------
float CEditEx::operator = (float i_value)
{
 SetValue(i_value);
 return(i_value);
}

//-------------------------------------------------------------
void CEditEx::SetRange(float i_min, float i_max)
{
 m_min = i_min;
 m_max = i_max;
}

//-------------------------------------------------------------
HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor)
{
 HBRUSH hbr = NULL;
 if (nCtlColor == CTLCOLOR_EDIT && m_own_ddv)
 {
  if (false==m_ddv_status)
  { 
   pDC->SetBkColor(ErrColor);
   hbr = m_redBrush;
  }
 }
 return hbr;
}

//-------------------------------------------------------------
