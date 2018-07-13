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

/** \file MapEditorCtrl.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include <memory>
#include <vector>
#include "common/fastdelegate.h"

class CEditExCustomKeys;

class AFX_EXT_CLASS CMapEditorCtrl : public CWnd
{
  typedef CWnd Super;

 public:
  enum AbroadDir
  {
   ABROAD_UP,
   ABROAD_DOWN
  };

  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate2<AbroadDir, int> EventHandler2;

  CMapEditorCtrl(int rows, int cols, bool invDataRowsOrder = false, HMODULE hMod = NULL, int minLabelWidthInChars = 0);
  virtual ~CMapEditorCtrl();

  void SetRange(float i_min, float i_max);
  void AttachMap(float* p_map);
  void AttachLabels(const float* horizLabels, const float* vertLabels);
  void ShowLabels(bool horizShow, bool vertShow);
  void SetDecimalPlaces(int value, int horiz, int vert);
  void SetArguments(float i_arg, float j_arg);
  void SetSelection(int i, int j); // i - row, j - column
  void EnableAbroadMove(bool up, bool down);
  void UpdateDisplay(void); //call if data changed outside control and thus control should be updated
  void ShowMarkers(bool show, bool invalidate = true);
  void SetValueIncrement(float inc);

  //Creates control dynamically
  BOOL Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id);
  void setOnAbroadMove(EventHandler2 OnCB);
  void setOnChange(EventHandler OnCB);

 protected:
  afx_msg void OnPaint();
  afx_msg void OnDestroy();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnSetFocus(CWnd*);
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnEnable(BOOL bEnable);
  afx_msg LRESULT OnWMSetFont(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnWMGetFont(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

 private:
  void OnEditChar(UINT nChar, CEditExCustomKeys*);
  void OnEditKill(CEditExCustomKeys*);

  void _ActivateEdit(void);
  void _DeactivateEdit(void);
  bool _ValidateItem(CEditExCustomKeys* pItem, float* p_value = NULL);
  bool _RegisterWindowClass(HMODULE hMod = NULL);
  void _DrawItem(CDC& dc, const CRect& rect, LPCTSTR text);
  int _GetGradIndex(float value);
  float _GetItem(int i, int j); // i - row, j - column
  void _SetItem(int i, int j, float value); // i - row, j - column
  CRect _GetItemRect(int i , int j); // i - row, j - column
  CRect _GetMarkerRect(int i , int j, int inflate = 2);
  CString _FloatToStr(float value, int decPlaces); //used for drawing of items
  int _GetLabelWidth(void);
  int _GetLabelHeight(void);
  void _DrawMarker(CDC* pDC, int i, int j);

  int m_cur_i; //current row index
  int m_cur_j; //current column index
  int m_rows;  //number of rows
  int m_cols;  //number of columns
  float m_minVal; //minimum allowed value
  float m_maxVal; //maximum allowed value
  float* mp_map; //array of data for editing
  int m_decPlaces; // number for decimal places
  int m_decPlacesH;
  int m_decPlacesV;
  std::auto_ptr<CEditExCustomKeys> mp_edit;

  EventHandler2 m_OnAbroadMove;
  EventHandler m_OnChange;
  CFont m_cFont;
  float* mp_horizLabels;
  float* mp_vertLabels;
  std::vector<CString> m_horizLabels;
  std::vector<CString> m_vertLabels;
  int m_label_width;  //precalculated value of label's width
  int m_label_height; //precalculated value of label's height
  std::vector<CPoint> m_markedItems;

  bool m_horizShow;
  bool m_vertShow;
  bool m_enAbroadUp;
  bool m_enAbroadDown;
  
  bool m_showMarkers;
  float m_increment;
  bool m_invDataRowsOrder;
  int m_minLabelWidthInChars;

  friend class CEditExCustomKeys;
};
