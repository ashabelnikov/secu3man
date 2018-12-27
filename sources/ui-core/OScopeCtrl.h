/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
   Original code written by Mark C. Malburg, USA, http://digitalmetrology.com

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

/** \file OScopeCtrl.h
 */

#pragma once

#include <algorithm>
#include <deque>

class AFX_EXT_CLASS COScopeCtrl : public CWnd
{
 public:
  COScopeCtrl();
  virtual ~COScopeCtrl();

  virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0);

  //Use for adding of a new point
  //dNewPoint - value of new point
  //i_reverse - if true, then point will be added from the left side (reverse)
  void AppendPoint(double dNewPoint, bool i_reverse = false);

  void SetRange(double dLower, double dUpper, int nDecimalPlaces=1);
  void SetXUnits(CString string);
  void SetYUnits(CString string);
  void SetYGridNumber(int ny); 
  void SetXGridSize(int sx);
  void SetGridColor(COLORREF color);
  void SetPlotColor(COLORREF color);
  void SetBackgroundColor(COLORREF color);
  void InvalidateCtrl(bool recreateBmpGrid = false, bool recreateBmpPlot = false, bool invalidate = true);
  void DrawPoint(bool i_reverse);
  void Reset();

 //Implementation
 protected:
  size_t _GetPtCount(void);
  void _SetStateColors(bool state);
  // Generated message map functions
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnEnable(BOOL bEnable);
#if _MSC_VER >= 1400
  afx_msg LRESULT OnNcHitTest(CPoint point);
#else
  afx_msg UINT OnNcHitTest(CPoint point);
#endif 
  DECLARE_MESSAGE_MAP()

 private:
  const int m_nShiftPixels;    // amount to shift with each new point
  int m_nYDecimals;
  int m_gridNumY;
  int m_gridSizeX;

  CString m_strXUnitsString;
  CString m_strYUnitsString;

  COLORREF m_COLOR_3DFACE;     // for testing of changing of system colors
  COLORREF m_crBackColor;      // background color
  COLORREF m_crGridColor;      // grid color
  COLORREF m_crPlotColor;      // data color

  COLORREF m_normalBackColor;
  COLORREF m_normalGridColor;
  COLORREF m_normalPlotColor;

  double m_dCurrentPosition;   // current position
  double m_dPreviousPosition;  // previous position

  double m_dLowerLimit;        // lower bounds
  double m_dUpperLimit;        // upper bounds
  double m_dRange;
  double m_dVerticalFactor;

  CRect  m_rectClient;
  CRect  m_rectPlot;
  CPen   m_penPlot;
  CBrush m_brushBack;         //background brush
  CBrush m_BlackBrush;        //black brush

  CDC     m_dcGrid;
  CDC     m_dcPlot;
  CBitmap *m_pbitmapOldGrid;
  CBitmap *m_pbitmapOldPlot;
  CBitmap m_bitmapGrid;
  CBitmap m_bitmapPlot;

  //stores values of all points
  std::deque<double> m_points;
  //stores point position, used for correct scrolling
  size_t m_point_position;
};
