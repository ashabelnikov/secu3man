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

/** \file Chart2D.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/fastdelegate.h"

struct LPoint
{
 double x;
 double y;
};

//Represents state and data of one serie
class CSerieData
{
 public :
  CSerieData(UINT nPoint);
  CSerieData(const CSerieData& rhs);
 ~CSerieData();
  CSerieData& operator=(const CSerieData& rhs);

 protected:
  COLORREF m_normalPlotColor;
  int m_ptCount;
  int m_ptIndex;
  double *mp_valueX;
  double *mp_valueY;
  COLORREF m_plotColor;
  bool m_visible;  //visibility flag
  bool m_handle;
  int m_curSel;

  friend class CChart2D;
};


//2D chart control
class AFX_EXT_CLASS CChart2D : public CWnd
{
  typedef CWnd Super;
  typedef fastdelegate::FastDelegate1<int> EventInt;

 public:
  CChart2D();
  virtual ~CChart2D();

  //Creates control
  BOOL Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id);

  //Set axis labels
  void SetChartLabel(CString str_x, CString str_y);

  //Clear chart data
  void ClearChart(void);

  //Set number of grids for horizontal and vertical axis
  void SetGridXYNumber(int nx, int ny);

  //Set title
  void SetChartTitle(CString str);

  //Add serie with specified number of points
  //returns index of newly added serie
  int AddSerie(int nPoints);

  //Set point to specified value
  bool SetXYValue(int SerieIdx, double x, double y, int index);

  const double* GetYValues(int serieIdx);

  //Set color of specified serie
  void SetSerieColor(int serieIdx, COLORREF color);

  //Set axis ranges
  bool SetRange(double min_x, double max_x, double min_y, double max_y);

  //Force redraw of control
  virtual void Invalidate(BOOL bErase = FALSE);

  //Set background color
  void SetBackColor(COLORREF color);

  //Set axis color
  void SetAxisColor(COLORREF color);

  //set grid color
  void SetGridColor(COLORREF color);

  //Set title color
  void SetTitleColor(COLORREF color);

  //Set color of axis labels
  void SetLabelColor(COLORREF color);

  //Set values format for horizontal axis
  void SetXAxisValueFormat(CString fmt);

  //Set values format for vertical axis
  void SetYAxisValueFormat(CString fmt);

  //Set visibility of specified serie
  void SetSerieVisibility(int serieIdx, bool visible);

  void SetSerieHandle(int serieIdx, bool showHandle);

  void SetPtMovStep(double value);

  void SetOnChange(const EventInt& OnChangeCB);

 protected:
  afx_msg void OnPaint();
  afx_msg void OnEnable(BOOL bEnable);
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  BOOL PreTranslateMessage (MSG * pMsg);
  afx_msg void OnSetFocus(CWnd* pOldWnd);
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnKeyDown(UINT, UINT, UINT);
  afx_msg UINT OnGetDlgCode();
  DECLARE_MESSAGE_MAP()

 private:
  void _DrawBackground(CDC *pDC);
  void _PlotSeries(CDC *pDC);
  void _DrawBorder(CDC *pDC);
  void _DrawChartTitle(CDC *pDC);
  void _DrawGrid(CDC *pDC);
  void _DrawAxis(CDC *pDC);
  void _DrawGridLabel(CDC *pDC);
  void _CreateFont(void);
  void _CalcRect(void);
  void _CalcMapFactors(void);
  CPoint _MapCoord(double x, double y);
  LPoint _UnMapCoord(CPoint pt);
  void _SetStateColors(bool state);
  void _RestrictAndSetValue(size_t seridx, size_t index, double v);

  std::vector<CSerieData> m_serie;
  CRect m_ctlRect;
  CRect m_axisRect;

  COLORREF m_BackColor;
  COLORREF m_AxisColor;
  COLORREF m_GridColor;
  COLORREF m_TextTitleColor;
  COLORREF m_TextLabelColor;

  COLORREF m_normalBackColor;
  COLORREF m_normalAxisColor;
  COLORREF m_normalGridColor;
  COLORREF m_normalTitleColor;
  COLORREF m_normalLabelColor;

  CBitmap m_bkBitmap;
  CBitmap *mp_oldBkBitmap;
  CDC memBkDC;

  CString m_axisFmtX;
  CString m_axisFmtY;
  CString m_strLabelX;
  CString m_strLabelY;
  CString m_chartTitle;
  CFont *mp_titleFont;
  CFont *mp_titleFontB;
  CFont *mp_legendFontY;
  CFont *mp_legendFontX;
  int m_grid_x;
  int m_grid_y;
  double m_mapfY;
  double m_mapfX;
  std::pair<double, double> m_rangeY; //min, max for vertical axis
  std::pair<double, double> m_rangeX; //min, max for horizontal axis
  bool m_updateBackground;
  bool m_updateBkBitmap;
  int m_ptHandleRadius;
  bool m_setval;
  size_t m_val_idx;
  size_t m_ser_idx;
  EventInt m_onChangeCB;
  CToolTipCtrl *mp_tooltip;
  double m_ptMovStep;
};
