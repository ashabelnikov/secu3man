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

/** \file OscillCtrl.h
 */

#pragma once

#include <algorithm>
#include <deque>
#include "common/unicodesupport.h"

class AFX_EXT_CLASS COscillCtrl : public CWnd
{
 typedef CWnd Super;
 public:
  COscillCtrl();
  virtual ~COscillCtrl();

  //create control's window
  virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0);

  //Use for adding of a new point
  //value - value of a new point
  //i_reverse - if true, then point will be added from the left side (reverse)
  void AppendPoint(double value, bool i_reverse = false);

  void ReserveCharsY(int num);
  void SetGridNumberY(int ny); 
  void SetGridSizeX(int sx);
  void SetRange(double low, double upp);
  void SetDecPlaces(int decimalPlaces = 1, int decimalPlacesV = 1);
  void SetPlotColor(COLORREF color);
  void SetPlotWidth(int width);
  void SetGridColor(COLORREF color);
  void SetValueColor(COLORREF color);
  void SetBackgroundColor(COLORREF color);
  void SetUnitX(const _TSTRING& str);
  void SetUnitY(const _TSTRING& str);
  void InvalidateCtrl(bool recreateBmpGrid = false, bool recreateBmpPlot = false, bool invalidate = true);
  void Reset();
  void ShowCursor(bool show);
  size_t GetMaxPtCount(void);
  void SetShtPixels(int n); //n >= 2
  void ShowValue(bool show);
  void SetValueHeight(int height); //value in %
  void Set3DRect(bool disp, bool redraw = false); //display 3D rect around control

 //Implementation
 protected:
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  afx_msg void OnEnable(BOOL bEnable);
  afx_msg void OnSize(UINT nType, int cx, int cy);
#if _MSC_VER >= 1400
  afx_msg LRESULT OnNcHitTest(CPoint point);
#else
  afx_msg UINT OnNcHitTest(CPoint point);
#endif 
  DECLARE_MESSAGE_MAP()

 private:
  void _SetStateColors(bool state);
  int _MapYValue(double);
  void _DrawPoint(bool i_reverse, int ptidx = -1);
  void _DrawCursor(bool i_invalidate = true);
  void _DrawValue(void);

  CDC     m_dcGrid;
  CDC     m_dcPlot;
  CDC     m_dcValue;
  CBitmap *m_pBmpOldGrid;
  CBitmap *m_pBmpOldPlot;
  CBitmap *m_pBmpOldValue;
  CBitmap m_bmpGrid;
  CBitmap m_bmpPlot;
  CBitmap m_bmpValue;
  CPen   m_penPlot;
  CBrush m_brushBack;          //background brush
  CBrush m_BlackBrush;         //black brush
  CBrush m_cursBrush;          //brush used for drawing of cursor
  CFont m_valueFont;

  COLORREF m_COLOR_3DFACE;     // for testing of changing of system colors
  COLORREF m_crBackColor;      // background color
  COLORREF m_crGridColor;      // grid color
  COLORREF m_crPlotColor;      // data color
  COLORREF m_crValueColor;     // value color
  COLORREF m_normalBackColor;
  COLORREF m_normalGridColor;
  COLORREF m_normalPlotColor;
  COLORREF m_normalValueColor;

  CRect  m_rcClient;
  CRect  m_rcPlot;
  CString m_strUnitX;
  CString m_strUnitY;

  int m_shtPixels;             //number of pixels bitmap will be shifted to for each point
  int m_decimalPlaces;         //number of decimal places
  int m_decimalPlacesV;        //number of decimal places for digital value
  int m_gridNumY;
  int m_gridSizeX;
  int m_plotWidth;

  std::deque<double> m_points; //stores values of all points
  size_t m_point_position;     //stores point position, used for correct scrolling
  double m_lowLimit;           //min
  double m_uppLimit;           //max
  double m_range;
  double m_vertFactor;
  int m_num_y_chars;
  bool m_show_cursor;  
  bool m_show_value;
  int m_value_height;         //%
  VOID* mp_valBits;
};
