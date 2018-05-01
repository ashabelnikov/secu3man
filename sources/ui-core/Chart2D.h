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

#include "vector"

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

  friend class CChart2D;
};


//2D chart control
class AFX_EXT_CLASS CChart2D : public CWnd
{
  typedef CWnd Super;

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

  //Set visibility of specified serie
  void SetSerieVisibility(int serieIdx, bool visible);

 protected:
  afx_msg void OnPaint();
  afx_msg void OnEnable(BOOL bEnable);
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
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
  void _SetStateColors(bool state);

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
  CString m_strLabelX;
  CString m_strLabelY;
  CString m_chartTitle;
  CFont *mp_titleFont;
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
};
