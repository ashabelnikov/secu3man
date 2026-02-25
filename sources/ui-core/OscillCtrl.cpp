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

/** \file OscillCtrl.cpp
 */

#include "stdafx.h"
#include "math.h"
#include "OscillCtrl.h"
#include "common/MathHelpers.h"
#include "common/GDIHelpers.h"
#include "common/DPIAware.h"

#undef min
#undef max

#define R(rgb)      ((BYTE)((rgb)>>16))
#define G(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define B(rgb)      ((BYTE)(rgb))

void AlphaBlend(UINT32* src, UINT32* dst, int w, int h, UINT a)
{
 UINT n = (256 - a); //added 1 to compensate division by 256 instead of 255
 a++;  //added 1 to compensate division by 256 instead of 255
 DWORD sz = w * h;
 for (DWORD i = 0; i < sz; ++i)
 {
  if (src[i] & 0xFFFFFF) //skip pixels with black color - this is because we write our own AlphaBlend version
  {
   dst[i] = (((a * B(src[i])) >> 8) | ((a * G(src[i])) & 0xff00) | (((a * R(src[i])) << 8) & 0xff0000)) +
            (((n * B(dst[i])) >> 8) | ((n * G(dst[i])) & 0xff00) | (((n * R(dst[i])) << 8) & 0xff0000));
  }
 }
}

BEGIN_MESSAGE_MAP(COscillCtrl, Super)
 ON_WM_ENABLE()
 ON_WM_PAINT()
 ON_WM_NCHITTEST()
 ON_WM_SIZE()
 ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// COscillCtrl
COscillCtrl::COscillCtrl()
: m_point_position(0)                   //position of cursor inside plot area
, m_gridSizeX(8)                        //horizontal space for grid
, m_gridNumY(5)                         //vertical space for grid
, m_decimalPlaces(3)                    //Num. of decimal places for the y axis
, m_shtPixels(2)                        //number of pixel bitmap will be shifted for
, m_lowLimit(-10.0)                     //bottom limit for Y value
, m_uppLimit(10.0)                      //top limit for Y value
, m_range(m_uppLimit - m_lowLimit)      //Reange of the Y axis
, m_normalBackColor(RGB(0, 0, 0))       // see also SetBackgroundColor
, m_crBackColor(RGB(0, 0, 0))           // see also SetBackgroundColor
, m_normalGridColor(RGB(0, 255, 255))   // see also SetGridColor
, m_crGridColor(RGB(0, 255, 255))       // see also SetGridColor
, m_normalPlotColor(RGB(255, 255, 255)) // see also SetPlotColor
, m_crPlotColor(RGB(255, 255, 255))     // see also SetPlotColor
, m_penPlot(PS_SOLID, 0, m_crPlotColor^m_crBackColor) //pen used for drawing
, m_brushBack(GDIHelpers::InvColor(m_crBackColor))//background brush
, m_BlackBrush(RGB(0, 0, 0))            //black color brush
, m_strUnitX(_T("Samples"))             //See also SetUnitX
, m_strUnitY(_T("Y units"))             //See also SetUnitY
, m_num_y_chars(0)                      //Number of reserved chars for width from the left
, m_pBmpOldGrid(NULL) 
, m_pBmpOldPlot(NULL)
, m_pBmpOldValue(NULL)
, m_cursBrush(RGB(255,0,0))
, m_show_cursor(false)                  //don't show cursor
, m_show_value(false)                   //don't show value
, m_value_height(100)                   //100%
, mp_valBits(NULL)
, m_plotWidth(0)                        //graph's line width
{
 m_COLOR_3DFACE = GetSysColor(COLOR_3DFACE);
}

COscillCtrl::~COscillCtrl()
{/*
 if (m_pBmpOldPlot != NULL)
  m_dcPlot.SelectObject(m_pBmpOldPlot);
 if (m_pBmpOldGrid != NULL)
  m_dcGrid.SelectObject(m_pBmpOldGrid);*/
}

// COscillCtrl message handlers
BOOL COscillCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
 BOOL result;
 static CString className = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW);

 result = Super::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, className, NULL, dwStyle,
                         rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, pParentWnd->GetSafeHwnd(), (HMENU)nID);
 if (result != 0)
 {
  _SetStateColors(IsWindowEnabled());
  InvalidateCtrl();
 }

 return result;
}

void COscillCtrl::OnPaint()
{ 
 if (m_COLOR_3DFACE != GetSysColor(COLOR_3DFACE))
 {
  m_COLOR_3DFACE = GetSysColor(COLOR_3DFACE);
  _SetStateColors(IsWindowEnabled());
  InvalidateCtrl(false, false, false);
 }

 CPaintDC dc(this);
 CDC memDC;
 CBitmap memBmp;

 //Draw all data onto memory DC
 memDC.CreateCompatibleDC(&dc);

// memBmp.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height());
 BITMAPINFO bmi; 
 bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
 bmi.bmiHeader.biWidth = m_rcClient.Width();
 bmi.bmiHeader.biHeight = m_rcClient.Height();
 bmi.bmiHeader.biPlanes = 1;
 bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
 bmi.bmiHeader.biCompression = BI_RGB;
 bmi.bmiHeader.biSizeImage = m_rcClient.Width() * m_rcClient.Height() * 4;
 VOID* p_memdcBits;
 HBITMAP hbitmap = CreateDIBSection(m_dcValue.GetSafeHdc(), &bmi, DIB_RGB_COLORS, &p_memdcBits, NULL, 0x0);
 memBmp.Attach(hbitmap);

 CBitmap* oldBmp = (CBitmap*)memDC.SelectObject(&memBmp);

 if (memDC.GetSafeHdc() != NULL)
 {
  memDC.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), &m_dcGrid, 0, 0, NOTSRCCOPY);
  memDC.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), &m_dcPlot, 0, 0, SRCINVERT);
  if (m_show_value)
  { //blend value's bitmap to resulting one
// BLENDFUNCTION bf = {AC_SRC_OVER, 0, 128, AC_SRC_ALPHA};
// memDC.AlphaBlend(0, 0, m_rcClient.Width(), m_rcClient.Height(), &m_dcValue, 0, 0, m_rcClient.Width(), m_rcClient.Height(), bf);
   AlphaBlend(((UINT32 *)mp_valBits), ((UINT32 *)p_memdcBits), m_rcClient.Width(), m_rcClient.Height(), 128);
  }
  dc.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), &memDC, 0, 0, SRCCOPY);
 }

 memDC.SelectObject(oldBmp);
}

void COscillCtrl::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 GetClientRect(m_rcClient);

 // this rect will be modified in the InvalidateCtrl
 m_rcPlot.left   = 20;
 m_rcPlot.top    = 10;
 m_rcPlot.right  = m_rcClient.right - 10;
 m_rcPlot.bottom = m_rcClient.bottom - 25;

 //update vertical factor, see also SetRange function
 m_vertFactor = (double)m_rcPlot.Height() / m_range;
 InvalidateCtrl(true, true);
 _DrawCursor(false);
}

#if _MSC_VER >= 1400
LRESULT COscillCtrl::OnNcHitTest(CPoint point)
#else
UINT COscillCtrl::OnNcHitTest(CPoint point)
#endif
{
 return HTTRANSPARENT;
}

void COscillCtrl::OnEnable(BOOL bEnable)
{
 _SetStateColors(bEnable);
 InvalidateCtrl();
}

void COscillCtrl::InvalidateCtrl(bool recreateBmpGrid /*=false*/, bool recreateBmpPlot /*=false*/, bool invalidate /*= true*/)
{
 int i, nCharacters;
 CPen *oldPen;
 CPen solidPen(PS_SOLID, 0, GDIHelpers::InvColor(m_crGridColor));
 CFont axisFont, yUnitFont, *oldFont;
 CString strTemp;

 CClientDC dc(this);

 if (m_dcGrid.GetSafeHdc() == NULL)
  m_dcGrid.CreateCompatibleDC(&dc);

 if (recreateBmpGrid && m_pBmpOldGrid && m_dcGrid.GetSafeHdc())
 {
  m_dcGrid.SelectObject(m_pBmpOldGrid);
  m_bmpGrid.DeleteObject();
 }

 if (m_bmpGrid.GetSafeHandle() == NULL)
 {
  m_bmpGrid.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height());
  m_pBmpOldGrid = m_dcGrid.SelectObject(&m_bmpGrid);
 }

 m_dcGrid.SetBkColor(GDIHelpers::InvColor(m_crBackColor));

 //fill the grid with background
 m_dcGrid.FillRect(m_rcClient, &m_brushBack);

 //correct rectangle depending on the number of characters,
 //avoid overflow range error (crash), when m_uppLimit or m_lowLimit is zero. 
 double log10ul = (m_uppLimit!=.0) ? log10(fabs(m_uppLimit)) : .0;
 double log10ll = (m_lowLimit!=.0) ? log10(fabs(m_lowLimit)) : .0;
 nCharacters = std::max(abs((int)log10ul), abs((int)log10ll));
 nCharacters = nCharacters + 4 + m_decimalPlaces;
 if (m_num_y_chars > 0)
  nCharacters = m_num_y_chars;
 m_rcPlot.left = m_rcClient.left + (6 * nCharacters);

 //correct plot width according to a number of points
 m_rcPlot.right = m_rcPlot.left + (GetMaxPtCount() * m_shtPixels);

 //draw plot rectangle
 oldPen = m_dcGrid.SelectObject(&solidPen);
 GDIHelpers::DrawEmptyRectangle(m_dcGrid, m_rcPlot);
 m_dcGrid.SelectObject(oldPen);

 if (0!=m_gridNumY)
 {
  //draw grid
  for (i = m_rcPlot.left; i < m_rcPlot.right; i+=m_gridSizeX)
  {
   for (int j = 0; j < m_gridNumY; ++j)
    m_dcGrid.SetPixel (i, m_rcPlot.top + MathHelpers::Round(((float)(m_rcPlot.Height() * j)) / m_gridNumY), GDIHelpers::InvColor(m_crGridColor));
  }
 }

 axisFont.CreateFont (14, 0, 0, 0, 300, FALSE, FALSE, 0, RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, _T("Arial"));
 yUnitFont.CreateFont (14, 0, 900, 0, 300,FALSE, FALSE, 0, RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS, _T("Arial"));

 oldFont = m_dcGrid.SelectObject(&axisFont);

 //y max
 m_dcGrid.SetTextColor(GDIHelpers::InvColor(m_crGridColor));
 m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP);
 strTemp.Format (_T("%.*lf"), m_decimalPlaces, m_uppLimit);
 m_dcGrid.TextOut (m_rcPlot.left - 4, m_rcPlot.top, strTemp);

 //y min
 m_dcGrid.SetTextAlign (TA_RIGHT|TA_BASELINE);
 strTemp.Format (_T("%.*lf"), m_decimalPlaces, m_lowLimit);
 m_dcGrid.TextOut (m_rcPlot.left - 4, m_rcPlot.bottom, strTemp);

 //x min
 m_dcGrid.SetTextAlign (TA_LEFT|TA_TOP);
 m_dcGrid.TextOut (m_rcPlot.left, m_rcPlot.bottom + 4, "0");

 //x max
 m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP);
 strTemp.Format (_T("%d"), GetMaxPtCount());
 m_dcGrid.TextOut (m_rcPlot.right, m_rcPlot.bottom + 4, strTemp);

 //x units
 m_dcGrid.SetTextAlign (TA_CENTER|TA_TOP);
 m_dcGrid.TextOut ((m_rcPlot.left + m_rcPlot.right) / 2, m_rcPlot.bottom + 4, m_strUnitX);

 m_dcGrid.SelectObject(oldFont);

 //y units
 oldFont = m_dcGrid.SelectObject(&yUnitFont);
 m_dcGrid.SetTextAlign(TA_CENTER | TA_BASELINE);
 m_dcGrid.SetBkMode(TRANSPARENT);
 m_dcGrid.TextOut((m_rcClient.left + m_rcPlot.left) / 2,(m_rcPlot.bottom + m_rcPlot.top) / 2, m_strUnitY);
 m_dcGrid.SelectObject(oldFont);

 if (m_dcPlot.GetSafeHdc() == NULL)
  m_dcPlot.CreateCompatibleDC(&dc);

 if (recreateBmpPlot && m_pBmpOldPlot && m_dcPlot.GetSafeHdc())
 {
  m_dcPlot.SelectObject(m_pBmpOldPlot);
  m_bmpPlot.DeleteObject();
 }

 if (m_bmpPlot.GetSafeHandle() == NULL)
 {
  m_bmpPlot.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height());
  m_pBmpOldPlot = m_dcPlot.SelectObject(&m_bmpPlot);
 }

 // clear the plot bitmap
 m_dcPlot.SetBkColor(RGB(0, 0, 0));
 m_dcPlot.FillRect(m_rcClient, &m_BlackBrush);

 //delete points from left or right sides depending on the position of cursor
 size_t ptCount = GetMaxPtCount();
 if (m_points.size() > ptCount)
 {
  size_t i = 0, n = m_points.size() - ptCount;
  for (; i < n; ++i)
  {
   if (m_point_position == (m_points.size()-1))
    break;
   m_points.pop_front();
  }
  for (; i < n; ++i)
  {
   if (m_point_position==0)
    break;
   m_points.pop_back();
   m_point_position--;
  }
 }

 //redraw all points
 for (std::deque<double>::iterator it = m_points.begin(); it != m_points.end(); ++it)
 {
  size_t index = std::distance(m_points.begin(), it);
  _DrawPoint(false, index);
 }

 //Create DC, bitmap and font for displaying value
 if (m_show_value)
 {
  if (m_dcValue.GetSafeHdc() == NULL)
   m_dcValue.CreateCompatibleDC(&dc);

  if (m_dcValue.GetSafeHdc())
  {
   m_dcValue.SelectObject(m_pBmpOldValue);
   m_bmpValue.DeleteObject();
  }

  if (m_bmpValue.GetSafeHandle() == NULL)
  {
   //m_bmpValue.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height());
   BITMAPINFO bmi; 
   bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bmi.bmiHeader.biWidth = m_rcClient.Width();
   bmi.bmiHeader.biHeight = m_rcClient.Height();
   bmi.bmiHeader.biPlanes = 1;
   bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
   bmi.bmiHeader.biCompression = BI_RGB;
   bmi.bmiHeader.biSizeImage = m_rcClient.Width() * m_rcClient.Height() * 4;
   HBITMAP hbitmap = CreateDIBSection(m_dcValue.GetSafeHdc(), &bmi, DIB_RGB_COLORS, &mp_valBits, NULL, 0x0);
   m_bmpValue.Attach(hbitmap);
   m_pBmpOldValue = m_dcValue.SelectObject(&m_bmpValue);
  }

  m_dcValue.SetBkColor(RGB(0, 0, 0));
  m_dcValue.FillRect(m_rcClient, &m_BlackBrush);

  DPIAware dpi;
  if (m_valueFont.GetSafeHandle())
   m_valueFont.DeleteObject();
  m_valueFont.CreateFont(dpi.FontHeight((m_rcPlot.Height()*m_value_height)/(3*100)), 0, 0, 0, 300, FALSE, FALSE, 0, RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, _T("Arial"));
 }

 // finally, force the plot area to redraw
 if (invalidate)
  InvalidateRect(m_rcClient);
}

void COscillCtrl::_DrawPoint(bool i_reverse, int ptidx /*= -1*/)
{
 int prevX, currX, prevY, currY;
 double prevPos, currPos;

 if (m_dcPlot.GetSafeHdc() != NULL)
 {
  int shtPixels = i_reverse ? -m_shtPixels : m_shtPixels;
  int shtPixelsH = shtPixels / 2, shtPixels15 = shtPixels + shtPixelsH;

  if ((m_points.size() < 2 && ptidx == -1) || (ptidx != -1 && ptidx < 1))
  {
   //draw a single pixel if there is only one point
   currPos = (ptidx < 0) ? ((!i_reverse) ? m_points.back() : m_points.front()) : m_points.at(ptidx);
   prevX = currX = (!i_reverse) ? m_rcPlot.right - shtPixelsH : m_rcPlot.left - shtPixelsH;
   prevY = currY = _MapYValue(currPos);
   m_dcPlot.SetPixel(currX, currY, m_crPlotColor^m_crBackColor);
  }
  else if ((m_points.size() >= 2 && ptidx == -1) || (ptidx != -1 && ptidx >= 1))
  {
   //shift bimmap
   if (!i_reverse)
    m_dcPlot.BitBlt(m_rcPlot.left, m_rcPlot.top, m_rcPlot.Width() - m_shtPixels, m_rcPlot.Height(),
                   &m_dcPlot, m_rcPlot.left + m_shtPixels, m_rcPlot.top, SRCCOPY);
   else
    m_dcPlot.BitBlt(m_rcPlot.left + m_shtPixels, m_rcPlot.top, m_rcPlot.Width()-m_shtPixels, m_rcPlot.Height(),
                   &m_dcPlot, m_rcPlot.left, m_rcPlot.top, SRCCOPY);

   //fill the cleanup area with the background color
   CRect rcCls = m_rcPlot;
   if (!i_reverse)                                           
   {                                           
    rcCls.left  = rcCls.right - m_shtPixels;
    rcCls.right+= m_plotWidth/2; //take into account width of line
   }
   else
   {
    rcCls.right  = rcCls.left + m_shtPixels;
    rcCls.left-= m_plotWidth/2; //take into account width of line
   }
   m_dcPlot.FillRect(rcCls, &m_BlackBrush);

   // Draw a line
   CPen *oldPen = m_dcPlot.SelectObject(&m_penPlot);

   prevPos = (ptidx < 0) ? ((!i_reverse) ? m_points.at(m_points.size() - 2)  : m_points.at(1)) : m_points.at(ptidx-1);
   currPos = (ptidx < 0) ? ((!i_reverse) ? m_points.back() : m_points.front()) : m_points.at(ptidx);

   prevX = (!i_reverse) ? m_rcPlot.right - shtPixels15 : m_rcPlot.left - shtPixels15;
   currX = (!i_reverse) ? m_rcPlot.right - shtPixelsH : m_rcPlot.left - shtPixelsH;
   prevY = _MapYValue(prevPos), currY = _MapYValue(currPos);

   m_dcPlot.MoveTo (prevX, prevY);
   m_dcPlot.LineTo (currX, currY);

   // restore the pen
   m_dcPlot.SelectObject(oldPen);
  }

  //vertical clipping
  if ((prevY <= m_rcPlot.top) || (currY <= m_rcPlot.top))
  {//top
   int addSht = std::max((m_plotWidth/2) + 1, shtPixelsH); //take into accont width of line
   int addWdt = m_plotWidth / 2;
   if (i_reverse)
    m_dcPlot.FillRect(CRect(prevX + addSht, m_rcClient.top, currX - addWdt, m_rcPlot.top), &m_BlackBrush);
   else
    m_dcPlot.FillRect(CRect(prevX - addWdt, m_rcClient.top, currX + addSht, m_rcPlot.top), &m_BlackBrush);
  }
  if ((prevY >= m_rcPlot.bottom) || (currY >= m_rcPlot.bottom))
  {//bottom
   int addSht = std::max((m_plotWidth/2) + 1, shtPixelsH); //take into accont width of line
   int addWdt = m_plotWidth / 2;
   if (i_reverse)
    m_dcPlot.FillRect(CRect(prevX + addSht, m_rcPlot.bottom, currX - addWdt, m_rcClient.bottom), &m_BlackBrush);
   else
    m_dcPlot.FillRect(CRect(prevX - addWdt, m_rcPlot.bottom, currX + addSht, m_rcClient.bottom), &m_BlackBrush);
  }
 }
}

void COscillCtrl::AppendPoint(double dNewPoint, bool i_reverse/* = false*/)
{
 if (!IsWindowEnabled())
  return; //do not try to add new points when control is disabled
 if (GetMaxPtCount()==0)
  return;
 size_t MaxPointIndex = GetMaxPtCount() - 1;

 //select default direction
 if (m_points.empty())
  m_point_position = i_reverse ? MaxPointIndex : 0;

 if (!i_reverse)
 {
  if (m_point_position > 0)
  {
   m_point_position--;
   _DrawValue();
   _DrawCursor();
   return;
  }
  m_points.push_back(dNewPoint);
  //restrict size of queue
  if (m_points.size() > GetMaxPtCount())
   m_points.pop_front();
 }
 else
 {
  if (m_point_position < MaxPointIndex)
  {
   if (m_point_position >= (m_points.size()-1))
   { //append points without shifting plot bitmap to fill empty area from the left
    m_points.push_front(dNewPoint);
    InvalidateCtrl(false, true);  
   }
   m_point_position++;
   _DrawValue();
   _DrawCursor();
   return;
  }

  m_points.push_front(dNewPoint);
  //restrict size of queue
  if (m_points.size() > GetMaxPtCount())
   m_points.pop_back();
 }

 _DrawValue();
 _DrawPoint(i_reverse);
 _DrawCursor(false);
 Invalidate();
}

void COscillCtrl::Reset()
{
 m_point_position = 0;
 m_points.clear();
 InvalidateCtrl(); //clear all data
}

size_t COscillCtrl::GetMaxPtCount(void)
{
 return (m_rcPlot.Width() / m_shtPixels);
}

void COscillCtrl::_SetStateColors(bool state)
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
 m_brushBack.CreateSolidBrush(GDIHelpers::InvColor(m_crBackColor));
 m_penPlot.DeleteObject();
 m_penPlot.CreatePen(PS_SOLID, m_plotWidth, m_crPlotColor^m_crBackColor);
}

int COscillCtrl::_MapYValue(double value)
{
 return (m_rcPlot.bottom - MathHelpers::Round((value - m_lowLimit) * m_vertFactor));
}

void COscillCtrl::ReserveCharsY(int num)
{
 m_num_y_chars = num;
}

void COscillCtrl::SetRange(double low, double upp)
{
 ASSERT(upp > low);
 m_lowLimit = low;
 m_uppLimit = upp;

 //prevent diision by zero
 m_range = m_uppLimit - m_lowLimit;
 if (abs(m_range) < 0.00001)
 {
  if (m_uppLimit >= m_lowLimit)
   m_uppLimit = m_lowLimit + 0.00001;
  else
   m_uppLimit = m_lowLimit - 0.00001;
 }

 m_range = m_uppLimit - m_lowLimit;
 m_vertFactor = (double)m_rcPlot.Height() / m_range;
 InvalidateCtrl();
}

void COscillCtrl::SetDecPlaces(int decimalPlaces /* = 1*/, int decimalPlacesV /* = 1*/)
{
 m_decimalPlaces = decimalPlaces;
 m_decimalPlacesV = decimalPlacesV;
 InvalidateCtrl();
}

void COscillCtrl::SetUnitX(const _TSTRING& str)
{
 m_strUnitX = str.c_str();
 InvalidateCtrl();
}

void COscillCtrl::SetUnitY(const _TSTRING& str)
{
 m_strUnitY = str.c_str();
 InvalidateCtrl();
}

void COscillCtrl::SetGridNumberY(int ny)
{
 m_gridNumY = ny - 1;
}

void COscillCtrl::SetGridSizeX(int sx)
{
 m_gridSizeX = sx;
}

void COscillCtrl::SetGridColor(COLORREF color)
{
 m_normalGridColor = color;
 _SetStateColors(IsWindowEnabled());
 InvalidateCtrl();
}

void COscillCtrl::SetPlotColor(COLORREF color)
{
 m_normalPlotColor = color;
 _SetStateColors(IsWindowEnabled());
 InvalidateCtrl();
}

void COscillCtrl::SetPlotWidth(int width)
{
 m_plotWidth = width;
 _SetStateColors(IsWindowEnabled());
 InvalidateCtrl();
}

void COscillCtrl::SetBackgroundColor(COLORREF color)
{
 m_normalBackColor = color;
 _SetStateColors(IsWindowEnabled());
 InvalidateCtrl();
}

void COscillCtrl::_DrawCursor(bool i_invalidate /* = true*/)
{
 if (!m_dcPlot.GetSafeHdc() || !m_show_cursor)
  return;
 int cs = 6; //size of cursor
 int offset = (m_point_position * m_shtPixels) - (cs / 2);

 //clear previous depiction of cursor  
 CRect cc = m_rcClient;
 cc.bottom = cc.top + cs;
 m_dcPlot.FillRect(&cc, &m_BlackBrush);

 //draw new one
 CRect rc(m_rcPlot.right - cs, m_rcClient.top, m_rcPlot.right, m_rcClient.top + cs);
 rc.OffsetRect(-offset, 0);
 m_dcPlot.FillRect(&rc, &m_cursBrush);
 if (i_invalidate)
  Invalidate();
}

void COscillCtrl::ShowCursor(bool show)
{
 m_show_cursor = show;
}

void COscillCtrl::SetShtPixels(int n)
{
 m_shtPixels = n;
}

void COscillCtrl::_DrawValue(void)
{
 if (!m_show_value || !m_dcValue.GetSafeHdc())
  return;

 if (m_points.size() > 0)
 {
  CFont* oldFont = m_dcValue.SelectObject(&m_valueFont);
  m_dcValue.FillRect(m_rcPlot, &m_BlackBrush);
  COLORREF color = m_crGridColor ? m_crGridColor : RGB(1,1,1); //replace black color by nearest value, because black color is reserved by our AlphaBlend()
  m_dcValue.SetTextColor(color);
  m_dcValue.SetTextAlign(TA_RIGHT|TA_TOP);
  CString str;
  str.Format(_T("%.*lf"), m_decimalPlacesV, m_points.at((m_points.size()-1)-m_point_position));
  CSize s = m_dcValue.GetTextExtent(str);
  m_dcValue.TextOut(m_rcPlot.CenterPoint().x + (s.cx/2), m_rcPlot.CenterPoint().y - (s.cy/2), str);
  m_dcValue.SelectObject(&oldFont);
 }
}

void COscillCtrl::ShowValue(bool show)
{
 m_show_value = show;
}

//-------------------------------------------------------------
BOOL COscillCtrl::OnEraseBkgnd(CDC* pDC) 
{
 return TRUE; //prevent flickering
}

//-------------------------------------------------------------
void COscillCtrl::SetValueHeight(int height)
{
 m_value_height = height;
}

//-------------------------------------------------------------
void COscillCtrl::Set3DRect(bool disp, bool redraw /*= false*/)
{ 
 LONG lStyle = ::GetWindowLong(this->m_hWnd, GWL_EXSTYLE);
 if (disp)
  lStyle |= WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;
 else
  lStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
 ::SetWindowLong(this->m_hWnd, GWL_EXSTYLE, lStyle);

 if (redraw)
  SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

//-------------------------------------------------------------
