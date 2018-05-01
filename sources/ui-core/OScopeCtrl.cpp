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

/** \file OScopeCtrl.cpp
 */

#include "stdafx.h"
#include "math.h"
#include "OScopeCtrl.h"
#include "common/MathHelpers.h"

#undef min //fucking stuff
#undef max

BEGIN_MESSAGE_MAP(COScopeCtrl, CWnd)
 ON_WM_PAINT()
 ON_WM_SIZE()
 ON_WM_ENABLE()
END_MESSAGE_MAP()

// COScopeCtrl
COScopeCtrl::COScopeCtrl()
: m_point_position(0)
, m_nShiftPixels(2)
, m_gridSizeX(8)
, m_gridNumY(5)
, m_dPreviousPosition(0.0) //Previous position value for LineTo
, m_nYDecimals(3)          //Number of decimal places on the y axis
// Range
, m_dLowerLimit(-10.0)
, m_dUpperLimit(10.0)
, m_dRange(m_dUpperLimit - m_dLowerLimit)
// bitmaps for restoring memory DC's
, m_pbitmapOldGrid(NULL)
, m_pbitmapOldPlot(NULL)
// background, grid and data colors
, m_normalBackColor(RGB(0, 0, 0))       // see also SetBackgroundColor
, m_crBackColor(RGB(0, 0, 0))           // see also SetBackgroundColor
, m_normalGridColor(RGB(0, 255, 255))   // see also SetGridColor
, m_crGridColor(RGB(0, 255, 255))       // see also SetGridColor
, m_normalPlotColor(RGB(255, 255, 255)) // see also SetPlotColor
, m_crPlotColor(RGB(255, 255, 255))     // see also SetPlotColor
// initialize pen/brush
, m_penPlot(PS_SOLID, 0, m_crPlotColor)
, m_brushBack(m_crBackColor)
, m_BlackBrush(RGB(0, 0, 0))
// Units
, m_strXUnitsString(_T("Samples"))  // See also SetXUnits
, m_strYUnitsString(_T("Y units"))  // See also SetYUnits
{
 //empty
}

COScopeCtrl::~COScopeCtrl()
{
 if (m_pbitmapOldGrid != NULL)
  m_dcGrid.SelectObject(m_pbitmapOldGrid);
 if (m_pbitmapOldPlot != NULL)
  m_dcPlot.SelectObject(m_pbitmapOldPlot);
}

// COScopeCtrl message handlers
BOOL COScopeCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
 BOOL result;
 static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);

 result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, className, NULL, dwStyle,
                          rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
                          pParentWnd->GetSafeHwnd(), (HMENU)nID);
 if (result != 0)
 {
  _SetStateColors(IsWindowEnabled());
  InvalidateCtrl();
 }

 return result;
}

void COScopeCtrl::SetRange(double dLower, double dUpper, int nDecimalPlaces)
{
 ASSERT(dUpper > dLower);
 m_dLowerLimit = dLower;
 m_dUpperLimit = dUpper;
 m_nYDecimals = nDecimalPlaces;
 m_dRange = m_dUpperLimit - m_dLowerLimit;
 m_dVerticalFactor = (double)m_rectPlot.Height() / m_dRange;
 InvalidateCtrl();
}

void COScopeCtrl::SetXUnits(CString string)
{
 m_strXUnitsString = string;
 InvalidateCtrl();
}

void COScopeCtrl::SetYUnits(CString string)
{
 m_strYUnitsString = string;
 InvalidateCtrl();
}

void COScopeCtrl::SetYGridNumber(int ny)
{
 m_gridNumY = ny;
}

void COScopeCtrl::SetXGridSize(int sx)
{
 m_gridSizeX = sx;
}

void COScopeCtrl::SetGridColor(COLORREF color)
{
 m_normalGridColor = color;
 _SetStateColors(IsWindowEnabled());
 InvalidateCtrl();
}

void COScopeCtrl::SetPlotColor(COLORREF color)
{
 m_normalPlotColor = color;
 _SetStateColors(IsWindowEnabled());
 InvalidateCtrl();
}

void COScopeCtrl::SetBackgroundColor(COLORREF color)
{
 m_normalBackColor = color;
 _SetStateColors(IsWindowEnabled());
 InvalidateCtrl();
}

void COScopeCtrl::InvalidateCtrl(bool recreateBmpGrid /*=false*/, bool recreateBmpPlot /*=false*/)
{
 // This is all being drawn (only once) to a bitmap.  The result is then BitBlt'd to the control whenever needed.
 int i, nCharacters;

 CPen *oldPen;
 CPen solidPen(PS_SOLID, 0, m_crGridColor);
 CFont axisFont, yUnitFont, *oldFont;
 CString strTemp;

 // in case we haven't established the memory dc's
 CClientDC dc(this);

 if (m_dcGrid.GetSafeHdc() == NULL)
  m_dcGrid.CreateCompatibleDC(&dc);

 if (recreateBmpGrid && m_pbitmapOldGrid && m_dcGrid.GetSafeHdc())
 {
  m_dcGrid.SelectObject(m_pbitmapOldGrid);
  m_bitmapGrid.DeleteObject();
 }

 if (m_bitmapGrid.GetSafeHandle() == NULL)
 {
  m_bitmapGrid.CreateCompatibleBitmap(&dc, m_rectClient.Width(), m_rectClient.Height());
  m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
 }

 m_dcGrid.SetBkColor (m_crBackColor);

 // fill the grid background
 m_dcGrid.FillRect(m_rectClient, &m_brushBack);

 // draw the plot rectangle:
 // determine how wide the y axis scaling values are
 nCharacters = abs((int)log10(fabs(m_dUpperLimit))) ;

 //avoid overflow range error (crash), when m_dLowerLimit is zero.
 double log10ll = (m_dLowerLimit!=.0) ? log10(fabs(m_dLowerLimit)) : log10ll = .0;
 nCharacters = std::max(nCharacters, abs((int)log10ll));

 // add the units digit, decimal point and a minus sign, and an extra space
 // as well as the number of decimal places to display
 nCharacters = nCharacters + 4 + m_nYDecimals;

 // adjust the plot rectangle dimensions
 // assume 6 pixels per character (this may need to be adjusted)
 m_rectPlot.left = m_rectClient.left + 6*(nCharacters);

 // draw the plot rectangle
 oldPen = m_dcGrid.SelectObject (&solidPen);
 m_dcGrid.MoveTo (m_rectPlot.left, m_rectPlot.top);
 m_dcGrid.LineTo (m_rectPlot.right+1, m_rectPlot.top);
 m_dcGrid.LineTo (m_rectPlot.right+1, m_rectPlot.bottom+1);
 m_dcGrid.LineTo (m_rectPlot.left, m_rectPlot.bottom+1);
 m_dcGrid.LineTo (m_rectPlot.left, m_rectPlot.top);
 m_dcGrid.SelectObject (oldPen);

 // draw grid
 for (i = m_rectPlot.left; i < m_rectPlot.right; i+=m_gridSizeX)
 {
  for (int j = 0; j < m_gridNumY; ++j)
   m_dcGrid.SetPixel (i, m_rectPlot.top + MathHelpers::Round(((float)(m_rectPlot.Height() * j)) / m_gridNumY), m_crGridColor);
 }

 // create some fonts (horizontal and vertical)
 // use a height of 14 pixels and 300 weight
 // (these may need to be adjusted depending on the display)
 axisFont.CreateFont (14, 0, 0, 0, 300,
                       FALSE, FALSE, 0, RUSSIAN_CHARSET,
                       OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY,
                       DEFAULT_PITCH|FF_SWISS, _T("Arial"));
 yUnitFont.CreateFont (14, 0, 900, 0, 300,
                       FALSE, FALSE, 0, RUSSIAN_CHARSET,
                       OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY,
                       DEFAULT_PITCH|FF_SWISS, _T("Arial"));

 // grab the horizontal font
 oldFont = m_dcGrid.SelectObject(&axisFont);

 // y max
 m_dcGrid.SetTextColor (m_crGridColor);
 m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP);
 strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dUpperLimit);
 m_dcGrid.TextOut (m_rectPlot.left - 4, m_rectPlot.top, strTemp);

 // y min
 m_dcGrid.SetTextAlign (TA_RIGHT|TA_BASELINE);
 strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dLowerLimit);
 m_dcGrid.TextOut (m_rectPlot.left - 4, m_rectPlot.bottom, strTemp);

 // x min
 m_dcGrid.SetTextAlign (TA_LEFT|TA_TOP);
 m_dcGrid.TextOut (m_rectPlot.left, m_rectPlot.bottom + 4, "0");

 // x max
 m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP);
 strTemp.Format (_T("%d"), _GetPtCount());
 m_dcGrid.TextOut (m_rectPlot.right, m_rectPlot.bottom + 4, strTemp);

 // x units
 m_dcGrid.SetTextAlign (TA_CENTER|TA_TOP);
 m_dcGrid.TextOut ((m_rectPlot.left + m_rectPlot.right) / 2,
                    m_rectPlot.bottom + 4, m_strXUnitsString);

 // restore the font
 m_dcGrid.SelectObject(oldFont);

 // y units
 oldFont = m_dcGrid.SelectObject(&yUnitFont);
 m_dcGrid.SetTextAlign (TA_CENTER|TA_BASELINE);
 m_dcGrid.TextOut ((m_rectClient.left + m_rectPlot.left) / 2,
                    (m_rectPlot.bottom + m_rectPlot.top) / 2, m_strYUnitsString);
 m_dcGrid.SelectObject(oldFont);

 // at this point we are done filling the the grid bitmap,
 // no more drawing to this bitmap is needed until the setting are changed

if (m_dcPlot.GetSafeHdc() == NULL)
  m_dcPlot.CreateCompatibleDC(&dc);

 if (recreateBmpPlot && m_pbitmapOldPlot && m_dcPlot.GetSafeHdc())
 {
  m_dcPlot.SelectObject(m_pbitmapOldPlot);
  m_bitmapPlot.DeleteObject();
 }

 if (m_bitmapPlot.GetSafeHandle() == NULL)
 {
  m_bitmapPlot.CreateCompatibleBitmap(&dc, m_rectClient.Width(), m_rectClient.Height());
  m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot);
 }

 // make sure the plot bitmap is cleared
 m_dcPlot.SetBkColor(RGB(0, 0, 0));
 m_dcPlot.FillRect(m_rectClient, &m_BlackBrush);

 // finally, force the plot area to redraw
 InvalidateRect(m_rectClient);
}

void COScopeCtrl::AppendPoint(double dNewPoint, bool i_reverse/* = false*/)
{
 if (!IsWindowEnabled())
  return; //do not try to add new points when control is disabled
 ASSERT(_GetPtCount() != 0);
 size_t MaxPointIndex = _GetPtCount() - 1;

 //select default direction
 if (m_points.empty())
  m_point_position = i_reverse ? MaxPointIndex : 0;

 if (!i_reverse)
 {
  if (m_points.size())
   m_dPreviousPosition = m_points.back();

  m_points.push_back(dNewPoint);
  //restrict size of queue
  if (m_points.size() > MaxPointIndex)
   m_points.pop_front();

  if (m_point_position > 0)
  {
   m_point_position--;
   return;
  }
 }
 else
 {
  if (m_points.size())
   m_dPreviousPosition = m_points.front();

  m_points.push_front(dNewPoint);
  //restrict size of queue
  if (m_points.size() > MaxPointIndex)
   m_points.pop_back();

  if (m_point_position < MaxPointIndex)
  {
   m_point_position++;
   return;
  }
 }

 m_dCurrentPosition = dNewPoint;
 DrawPoint(i_reverse);
 Invalidate();
}

void COScopeCtrl::OnPaint()
{
 CPaintDC dc(this);
 CDC memDC;
 CBitmap memBmp;
 CBitmap* oldBmp;

 //Draw all data onto memory DC
 memDC.CreateCompatibleDC(&dc);
 memBmp.CreateCompatibleBitmap(&dc, m_rectClient.Width(), m_rectClient.Height());
 oldBmp = (CBitmap *)memDC.SelectObject(&memBmp);

 if (memDC.GetSafeHdc() != NULL)
 {
  // draw the grid on the memory DC
  memDC.BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), &m_dcGrid, 0, 0, SRCCOPY);
  // draw the plot on the memory DC
  memDC.BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(),&m_dcPlot, 0, 0, SRCPAINT);
  // finally, output memory DC to display
  dc.BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(),&memDC, 0, 0, SRCCOPY);
 }

 memDC.SelectObject(oldBmp);
}

void COScopeCtrl::DrawPoint(bool i_reverse)
{
 // this does the work of "scrolling" the plot to the left
 // and appending a new data point all of the plotting is
 // directed to the memory based bitmap associated with m_dcPlot
 // the will subsequently be BitBlt'd to the client in OnPaint

 int currX, prevX, currY, prevY;
 int nHalfShiftPixels, nPlotShiftPixels, nShiftPixels;

 CPen *oldPen;
 CRect rectCleanUp;

 if (m_dcPlot.GetSafeHdc() != NULL)
 {
  nShiftPixels = i_reverse ? -m_nShiftPixels : m_nShiftPixels;
  nHalfShiftPixels = nShiftPixels / 2;
  nPlotShiftPixels = nShiftPixels + nHalfShiftPixels;

  // shift the plot by BitBlt'ing it to itself
  // note: the m_dcPlot covers the entire client
  //       but we only shift bitmap that is the size
  //       of the plot rectangle
  // grab the right side of the plot (excluding nShiftPixels on the left/right)
  // move this grabbed bitmap to the left/right by nShiftPixels

  m_dcPlot.BitBlt(m_rectPlot.left, m_rectPlot.top + 1,
                  m_rectPlot.Width(), m_rectPlot.Height(), &m_dcPlot,
                  m_rectPlot.left + nShiftPixels, m_rectPlot.top + 1,
                  SRCCOPY);

  // establish a rectangle over the right side of plot
  // which now needs to be cleaned up prior to adding the new point
  rectCleanUp = m_rectPlot;

  if (!i_reverse)
   rectCleanUp.left  = rectCleanUp.right - nShiftPixels;
  else
   rectCleanUp.right  = rectCleanUp.left - nShiftPixels;

  // fill the cleanup area with the background
  m_dcPlot.FillRect(rectCleanUp, &m_brushBack);

  // draw the next line segment

  // grab the plotting pen
  oldPen = m_dcPlot.SelectObject(&m_penPlot);

  // move to the previous point
  if (!i_reverse)
   prevX = m_rectPlot.right - nPlotShiftPixels;
  else
   prevX = m_rectPlot.left - nPlotShiftPixels;

  prevY = m_rectPlot.bottom - MathHelpers::Round((m_dPreviousPosition - m_dLowerLimit) * m_dVerticalFactor);
  m_dcPlot.MoveTo (prevX, prevY);

  // draw to the current point
  if (!i_reverse)
   currX = m_rectPlot.right - nHalfShiftPixels;
  else
   currX = m_rectPlot.left - nHalfShiftPixels;

  currY = m_rectPlot.bottom - MathHelpers::Round((m_dCurrentPosition - m_dLowerLimit) * m_dVerticalFactor);
  m_dcPlot.LineTo (currX, currY);

  // restore the pen
  m_dcPlot.SelectObject(oldPen);

  // if the data leaks over the upper or lower plot boundaries
  // fill the upper and lower leakage with the background
  // this will facilitate clipping on an as needed basis
  // as opposed to always calling IntersectClipRect
  if ((prevY <= m_rectPlot.top) || (currY <= m_rectPlot.top))
   m_dcPlot.FillRect(CRect(prevX, m_rectClient.top, currX + 1, m_rectPlot.top + 1), &m_brushBack);
  if ((prevY >= m_rectPlot.bottom) || (currY >= m_rectPlot.bottom))
   m_dcPlot.FillRect(CRect(prevX, m_rectPlot.bottom + 1, currX + 1, m_rectClient.bottom + 1), &m_brushBack);
 }
}

void COScopeCtrl::OnSize(UINT nType, int cx, int cy)
{
 CWnd::OnSize(nType, cx, cy);

 GetClientRect(m_rectClient);

 // the "left" coordinate and "width" will be modified in
 // InvalidateCtrl to be based on the width of the y axis scaling
 m_rectPlot.left   = 20;
 m_rectPlot.top    = 10;
 m_rectPlot.right  = m_rectClient.right - 10;
 m_rectPlot.bottom = m_rectClient.bottom - 25;

 // set the scaling factor for now, this can be adjusted
 // in the SetRange functions
 m_dVerticalFactor = (double)m_rectPlot.Height() / m_dRange;
 InvalidateCtrl(true, true);
}

void COScopeCtrl::OnEnable(BOOL bEnable)
{
 _SetStateColors(bEnable);
 InvalidateCtrl();
}

void COScopeCtrl::Reset()
{
 m_point_position = 0;
 m_points.clear();
 m_dPreviousPosition = 0.0;
 InvalidateCtrl(); //clear all data
}

size_t COScopeCtrl::_GetPtCount(void)
{
 return m_rectPlot.Width() / m_nShiftPixels;
}

void COScopeCtrl::_SetStateColors(bool state)
{
 if (state)
 { //enabled
  m_crBackColor = m_normalBackColor;
  m_crGridColor = m_normalGridColor;
  m_crPlotColor = m_normalPlotColor;
 }
 else
 { //disabled
  m_crBackColor = GetSysColor(COLOR_3DFACE);
  m_crGridColor = GetSysColor(COLOR_GRAYTEXT);
  m_crPlotColor = GetSysColor(COLOR_GRAYTEXT);
 }
 m_brushBack.DeleteObject();
 m_brushBack.CreateSolidBrush(m_crBackColor);
 m_penPlot.DeleteObject();
 m_penPlot.CreatePen(PS_SOLID, 0, m_crPlotColor);
}
