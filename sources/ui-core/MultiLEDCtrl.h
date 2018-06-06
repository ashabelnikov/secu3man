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

/** \file MultiLEDCtrl.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include <vector>

class CMultiLEDCtrl : public CWnd
{
  typedef CWnd Super;

 public:

  CMultiLEDCtrl();
  virtual ~CMultiLEDCtrl();

  //Creates control dynamically
  BOOL Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id);

  void SetNumRows(int rows);
  void AddItem(const CString& text, COLORREF color = RGB(0,255,0));
  void SetItemState(int idx, bool state, bool invalidate = true);

 protected:
  afx_msg void OnPaint();
  afx_msg void OnDestroy();
  afx_msg void OnEnable(BOOL bEnable);
  afx_msg LRESULT OnWMSetFont(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnWMGetFont(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

 private:
  bool _RegisterWindowClass(void);
  CRect _GetItemRect(int idx);

  struct LEDItem
  {
   CString m_text;
   bool m_state;
   COLORREF m_color;
   CBrush* mp_brush;
  };

  int m_rows;
  int m_cols;
  CFont m_cFont;
  std::vector<LEDItem> m_items;
  CBrush m_bkBrush;
};
