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

#pragma once

class AFX_EXT_CLASS CEditEx : public CEdit
{
 public:
  CEditEx(int i_mode = MODE_STRING, bool i_own_ddv = false);
  virtual ~CEditEx();

  //поддерживаемые режимы (фильтрация вводимых символов)
  enum
  {
   MODE_STRING  = 0x00000001,
   MODE_INT     = 0x00000002,
   MODE_FLOAT   = 0x00000004,
   MODE_HEX     = 0x00000008,
   MODE_HEXSTR  = 0x00000010,   //hex string

   MODE_SIGNED  = 0x00000100,   //несовместим с MODE_HEX и MODE_HEXSTR
   //---------------------------------------------------------
   MODE_MASK    = 0x000000FF   //для внутреннего использования
  };

  inline void DDX_Value(CDataExchange *ipDX, int iIDC, float& ioValue)
  {
   _DDX_Value(ipDX, iIDC, ioValue);
  }

  inline void DDX_Value(CDataExchange *ipDX, int iIDC, int& ioValue)
  {
   _DDX_Value(ipDX, iIDC, ioValue);
  }
  
  void SetRange(float i_min, float i_max);

  bool GetValue(float& o_value);
  bool SetValue(float i_value);

  inline bool GetValue(int &o_value)
  {
   float value = 0;
   bool result = GetValue(value);
   o_value = (int)value;
   return result;
  }

  inline bool SetValue(int i_value)
  {
   return SetValue((float)i_value);
  }

  inline void SetMode(DWORD i_mode)
  {
   //нельзя использовать SIGNED для HEX
   ASSERT(!((i_mode & MODE_HEX) && (i_mode & MODE_SIGNED)));
   m_mode = i_mode;
  }

  inline DWORD GetMode(void)
  {
   return m_mode;
  }

  //Устанавливает, для FLOAT: кол-во знаков после запятой, для целых: кол-во десятичных знаков
  inline void SetDecimalPlaces(int i_decimal_places)
  {
   m_DecimalPlaces = i_decimal_places;
  }

  inline int GetDecimalPlaces(void)
  {
   return m_DecimalPlaces;
  }

  float operator = (float i_value);

 protected:
  bool OnChar_int(UINT nChar, UINT nRepCnt, UINT nFlags);
  bool OnChar_float(UINT nChar, UINT nRepCnt, UINT nFlags);
  bool OnChar_hex(UINT nChar, UINT nRepCnt, UINT nFlags);
  bool OnChar_hexstr(UINT nChar, UINT nRepCnt, UINT nFlags);

  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
  DECLARE_MESSAGE_MAP()

 private:
  template <class T>
  void _DDX_Value(CDataExchange *ipDX, int iIDC, T& ioValue);

 private:
  DWORD m_mode;
  int m_DecimalPlaces;
  bool m_own_ddv;
  bool m_ddv_status;
  float m_min, m_max;
  CBrush m_redBrush;
};

//-------------------------------------------------------------
template <class T>
void CEditEx::_DDX_Value(CDataExchange *ipDX, int iIDC, T& ioValue)
{
 // Init CDataExchange members:
 HWND hwnd = ipDX->PrepareEditCtrl (iIDC);
 ASSERT (m_hWnd == hwnd);

 // Now copy to/from control
 if (ipDX->m_bSaveAndValidate)
 {
  if (!m_own_ddv)
  {
   if (!GetValue(ioValue))
    ipDX->Fail(); //message box!
  }
  else
  { //own DDV mode
   T value;
   bool result = GetValue(value);
   m_ddv_status = (result && value >= m_min && value <= m_max);     
   if (m_ddv_status)
    ioValue = value;
   Invalidate();
  }
 }
 else
 {
  SetValue(ioValue);
  m_ddv_status = true;
 }
}

//-------------------------------------------------------------
