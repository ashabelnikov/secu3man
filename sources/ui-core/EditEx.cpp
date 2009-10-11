 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEditEx::CEditEx(int i_mode /* = MODE_STRING*/)
: m_mode(i_mode)
, m_DecimalPlaces(4)
{
 //na
}

CEditEx::~CEditEx()
{
 //na
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
 ON_WM_CHAR()
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
