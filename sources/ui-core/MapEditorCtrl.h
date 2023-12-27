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
  typedef fastdelegate::FastDelegate2<float, int, float> EventHandler3; //param1, param2, retval

  CMapEditorCtrl(int rows, int cols, bool invDataRowsOrder = false, bool invDataVGridOrder = false, HMODULE hMod = NULL, int minLabelWidthInChars = 0, bool readOnly = false, bool absGrad = false);
  virtual ~CMapEditorCtrl();

  void SetRange(float i_min, float i_max);
  void AttachMap(float* p_map, float* p_mapOrig = NULL);

  void AttachLabels(const float* horizLabels, const float* vertLabels);
  void ShowLabels(bool horizShow, bool vertShow);
  void SetDecimalPlaces(int value, int horiz, int vert);
  void SetArguments(float i_arg, float j_arg);
  void SetSelection(int i, int j); // i - row, j - column
  const std::vector<std::pair<int, int> >& GetSelection(void); // first - row, second - column
  void EnableAbroadMove(bool up, bool down);
  void UpdateDisplay(const std::vector<std::pair<int, int> >* p_updList = NULL); //call if data changed outside control and thus control should be updated
  void ShowMarkers(bool show, bool redraw = true);
  void SetValueIncrement(float inc);
  void SetGradientList(const std::vector<COLORREF>& colors);
  void SetItemColor(int i, int j, COLORREF color); // i - row, j - column
  void Redraw(void); //does full redraw of control

  //Creates control dynamically
  BOOL Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id);
  void setOnAbroadMove(EventHandler2 OnCB);
  void setOnChange(EventHandler OnCB);
  void setOnSelChange(EventHandler OnCB);
  void setOnValueTransform(EventHandler3 OnCB);

  static void SetSettings(int gradSat, int gradBrt, bool boldFont, bool spotMarkers, float spotMarkersSize);
  static bool GetSpotMarkers(void);
  static void SetCSVSepSymbol(char sepsymb);

 protected:
  afx_msg void OnPaint();
  afx_msg void OnDestroy();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnSetFocus(CWnd*);
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnEnable(BOOL bEnable);
  afx_msg LRESULT OnWMSetFont(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnWMGetFont(WPARAM wParam, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg UINT OnGetDlgCode();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnInc();
  afx_msg void OnDec();
  afx_msg void OnSetTo();
  afx_msg void OnSub();
  afx_msg void OnAdd();
  afx_msg void OnMul();
  afx_msg void OnExportCsv();
  afx_msg void OnImportCsv();
  afx_msg void OnRevert();
  afx_msg void OnClipboardCopy();
  afx_msg void OnClipboardPaste();
  afx_msg void OnUpdateSetTo(CCmdUI* pCmdUI);
  afx_msg void OnUpdateImportCsv(CCmdUI* pCmdUI);
  afx_msg void OnUpdateClipboardPaste(CCmdUI* pCmdUI);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  BOOL PreTranslateMessage(MSG *pMsg);
  DECLARE_MESSAGE_MAP()

 private:
  void OnEditChar(UINT nChar, CEditExCustomKeys*);
  void OnEditKill(CEditExCustomKeys*);

  void _DrawGrid(const std::vector<std::pair<int, int> >* p_updList = NULL);
  void _DrawLabels(void);
  void _DrawMarkers(void);
  void _ShowImage(CDC* pDC, CRect* p_rect = NULL);

  void _UpdateMinMaxElems(void);
  void _ActivateEdit();
  void _DeactivateEdit(void);
  bool _ValidateItem(CEditExCustomKeys* pItem, float* p_value = NULL);
  bool _RegisterWindowClass(HMODULE hMod = NULL);
  void _DrawItem(CDC& dc, int i, int j);
  int _GetGradIndex(float value);
  float _GetItem(int i, int j); // i - row, j - column
  void _SetItem(int i, int j, float value); // i - row, j - column
  CRect _GetItemRect(CDC* pDC, int i , int j); // i - row, j - column
  CRect _GetMarkerRect(int i , int j, int inflate = 2);
  int _GetLabelWidth(CDC* pDC);
  int _GetLabelHeight(CDC* pDC);
  void _DrawMarker(CDC* pDC, int i, int j);
  void _2DLookupH(float x, std::vector<int>& pt);
  void _2DLookupV(float x, std::vector<int>& pt);
  bool _isCellInSelRect(int i, int j);
  bool _SelChange(int cur_i, int cur_j, int end_i, int end_j, bool notify = true);
  bool _CellByPoint(CPoint point, int& oi, int& oj);
  void _GetSelRect(CRect& o_rc);
  void _ActivateTooltip(int i, int j);
  void _ClipboardCopy(void);
  void _ClipboardPaste(void);

  inline  CString _FloatToStr(float value, int decPlaces) //used for drawing of items
  {
   TCHAR buff[16];
   _stprintf(buff, "%.*f", decPlaces, value);
   return buff;
  }

  void _SetItemTr(int i, int j, float value);
  float _GetItemTr(int i, int j);
  float _GetItemTrO(int i, int j);

  template <class T>
  inline T _GetItem(T* p_array, int i, int j)
  {
   ASSERT(p_array);
   int ii = m_invDataRowsOrder ? (m_rows - 1) - i : i;
   return p_array[(ii*m_cols)+j];
  }

  template <class T>
  inline void _SetItem(T* p_array, int i, int j, T value)
  {
   ASSERT(p_array);
   int ii = m_invDataRowsOrder ? (m_rows - 1) - i : i;
   p_array[(ii*m_cols)+j] = value;
  }

  inline float _GetVGrid(int i)
  {
   int ii = m_invDataVGridOrder ? (m_rows - 1) - i : i;   
   return mp_vertLabels[ii];
  }

  int m_cur_i; //current row index
  int m_cur_j; //current column index
  int m_end_i; //end of selection row index
  int m_end_j; //end of selection column index
  int m_rows;  //number of rows
  int m_cols;  //number of columns
  float m_minVal; //minimum allowed value
  float m_maxVal; //maximum allowed value
  float m_vMinVal;
  float m_vMaxVal;
  float* mp_map; //array of data for editing
  float* mp_mapTr;
  float* mp_mapOrig;
  int m_decPlaces; // number for decimal places
  int m_decPlacesH;
  int m_decPlacesV;
  std::auto_ptr<CEditExCustomKeys> mp_edit;
  int m_tp_i;
  int m_tp_j;
  std::vector<std::pair<int, int> > m_sel;
  bool m_sel_changed;

  EventHandler2 m_OnAbroadMove;
  EventHandler m_OnChange;
  EventHandler m_OnSelChange;
  EventHandler3 m_OnValueTransform;
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
  bool m_invDataRowsOrder; //true - if rows data is in inverted order
  bool m_invDataVGridOrder; //true - if vertical labels data is in inverted order
  bool m_readOnly;
  bool m_absGrad;
  int m_minLabelWidthInChars;
  bool m_lbuttondown;

  friend class CEditExCustomKeys;

  std::vector<COLORREF> m_gradColor;
  DWORD* mp_itemColors;

  static int m_gradSaturation;
  static int m_gradBrightness;
  static bool m_boldFont;
  static bool m_spotMarkers;
  static float m_spotMarkersSize;
  static CMapEditorCtrl* mp_other;
  static char m_csvsep_symb;

  CDC     m_dcGrid;
  CBitmap m_bmGrid;
  CBitmap *mp_bmOldGrid;
  CDC     m_dcMark;
  CBitmap m_bmMark;
  CBitmap *mp_bmOldMark;
  CToolTipCtrl *mp_tooltip;

  CBrush m_blackBrush;
  COLORREF m_bgrdBrushColor;
  CBrush m_bgrdBrush;
  CRgn m_clipRgn;
  bool m_forceRedraw;
  CPoint m_ballCoord;
  CBrush m_ballBrush;
};
