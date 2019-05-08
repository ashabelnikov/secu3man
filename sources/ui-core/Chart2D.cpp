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

/** \file Chart2D.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Chart2D.h"
#include <math.h>

#define ROUND(x) (int)((x) + 0.5 - (double)((x) < 0))

CSerieData::CSerieData(UINT nPoint) 
: mp_valueX(NULL)
, mp_valueY(NULL)
, m_plotColor(RGB(0,0,0))
, m_normalPlotColor(RGB(0,0,0))
, m_ptIndex(0)
, m_ptCount(0)
, m_visible(true)
, m_handle(false)
, m_curSel(0)
{
 mp_valueX = new (std::nothrow) double[nPoint];
 mp_valueY = new (std::nothrow) double[nPoint];
 if ((mp_valueX == NULL) || (mp_valueY == NULL)) 
  return;
 m_ptCount = nPoint;
}

CSerieData::CSerieData(const CSerieData& rhs)
{
 mp_valueX = new (std::nothrow) double[rhs.m_ptCount];
 mp_valueY = new (std::nothrow) double[rhs.m_ptCount];
 if ((mp_valueX == NULL) || (mp_valueY == NULL))
  return;
 *this = rhs; //copy data fields
}

CSerieData::~CSerieData()
{
 delete[] mp_valueX;
 delete[] mp_valueY;
}

CSerieData& CSerieData::operator=(const CSerieData& rhs)
{
 m_plotColor = rhs.m_plotColor;
 m_normalPlotColor = rhs.m_normalPlotColor;
 m_ptIndex = rhs.m_ptIndex;
 m_ptCount = rhs.m_ptCount;
 std::copy(rhs.mp_valueX, rhs.mp_valueX + m_ptCount, mp_valueX);
 std::copy(rhs.mp_valueY, rhs.mp_valueY + m_ptCount, mp_valueY);
 m_visible = rhs.m_visible;
 m_handle = rhs.m_handle;
 m_curSel = rhs.m_curSel;
 return *this;
}

CChart2D::CChart2D()
: m_chartTitle(_T(""))
, m_strLabelX(_T(""))
, m_strLabelY(_T(""))
, m_normalBackColor(RGB(255,255,255))
, m_normalAxisColor(RGB(0,0,0))
, m_normalGridColor(RGB(120,120,120))
, m_normalTitleColor(RGB(0,0,0))
, m_normalLabelColor(RGB(0,0,0))
, m_grid_x(10)
, m_grid_y(10)
, mp_legendFontY(NULL)
, mp_legendFontX(NULL)
, mp_titleFont(NULL)
, mp_titleFontB(NULL)
, m_updateBackground(true)
, m_updateBkBitmap(true)
, m_axisFmtX("%5.1f")
, m_axisFmtY("%5.1f")
, m_ptHandleRadius(5)
, m_setval(false)
, m_val_idx(0)
, m_ser_idx(0)
, mp_tooltip(NULL)
, m_ptMovStep(1.0)
, mp_oldBkBitmap(NULL)
{
 SetRange(-10, 10, -10, 10);
}

CChart2D::~CChart2D()
{
 if(memBkDC.GetSafeHdc() != NULL)
  memBkDC.SelectObject(mp_oldBkBitmap);
 delete mp_titleFont;
 delete mp_titleFontB;
 delete mp_legendFontX;
 delete mp_legendFontY;
 delete mp_tooltip;
}

BEGIN_MESSAGE_MAP(CChart2D, Super)
 ON_WM_PAINT()
 ON_WM_ENABLE()
 ON_WM_DESTROY()
 ON_WM_LBUTTONDOWN()
 ON_WM_LBUTTONUP()
 ON_WM_MOUSEMOVE()
 ON_WM_SETFOCUS()
 ON_WM_KILLFOCUS()
 ON_WM_KEYDOWN()
 ON_WM_GETDLGCODE()
 ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CChart2D::Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id)
{
 BOOL result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, NULL, NULL, dwStyle,
               rect.left, rect.top, rect.Width(), rect.Height(), pParent->GetSafeHwnd(), (HMENU)id);
 if (!result)
  AfxMessageBox(_T("Error creating Chart2D control's window!"));

 GetClientRect(&m_ctlRect);
 _CreateFont();
 _CalcRect();
 _CalcMapFactors();

 if (result)
 {
  _SetStateColors(IsWindowEnabled());
  Invalidate(TRUE);
 }

 return result;
}

BOOL CChart2D::PreTranslateMessage (MSG * pMsg)
{
 if (mp_tooltip)
  mp_tooltip->RelayEvent(pMsg);

 return Super::PreTranslateMessage(pMsg);
} 

void CChart2D::OnDestroy()
{
 delete mp_titleFont;
 delete mp_titleFontB;
 delete mp_legendFontX;
 delete mp_legendFontY;
 mp_titleFont = NULL;
 mp_titleFontB = NULL;
 mp_legendFontX = NULL;
 mp_legendFontY = NULL;
}

void CChart2D::_CalcRect(void)
{
 CFont *old;
 CString str;
 CPaintDC dc(this);

 //offset from left
 old = dc.SelectObject(mp_legendFontY);
 str.Format(m_axisFmtY, m_rangeY.second);
 CSize offset_left1 = dc.GetTextExtent(str);
 dc.SelectObject(old);
 CSize offset_left(0, 0);
 if (!m_chartTitle.IsEmpty())
 {
  old = dc.SelectObject(mp_legendFontX);
  offset_left = dc.GetTextExtent(m_strLabelY);
  dc.SelectObject(old);
 }
 m_axisRect.left = m_ctlRect.left + (offset_left1.cx + 7 + offset_left.cy);

 //offset from right
 old = dc.SelectObject(mp_legendFontY);
 str.Format(m_axisFmtX, m_rangeX.second);
 CSize offset_right = dc.GetTextExtent(str);
 offset_right.cx/=2; //half of text width
 dc.SelectObject(old);
 m_axisRect.right = m_ctlRect.right - (offset_right.cx + 2);

 //offset from top
 CSize offset_top(0, 0);
 old = dc.SelectObject(mp_legendFontY);
 offset_top = dc.GetTextExtent("1234");
 offset_top.cy/= 2; //half of text height
 dc.SelectObject(old);
 if (!m_chartTitle.IsEmpty()) {  
  old = dc.SelectObject(::GetFocus() == m_hWnd ? mp_titleFontB : mp_titleFont);
  offset_top = dc.GetTextExtent(m_chartTitle, m_chartTitle.GetLength());
  dc.SelectObject(old);
 }
 m_axisRect.top = m_ctlRect.top + offset_top.cy;

 //offset from bottom
 old = dc.SelectObject(mp_legendFontY);
 CSize offset_bottom1 = dc.GetTextExtent("1234");
 dc.SelectObject(old);
 CSize offset_bottom(0,0);
 if (!m_strLabelX.IsEmpty()) {
  old = dc.SelectObject(mp_legendFontY);
  offset_bottom = dc.GetTextExtent(m_strLabelX, m_strLabelX.GetLength());
  dc.SelectObject(old);
 }
 m_axisRect.bottom = m_ctlRect.bottom - (offset_bottom1.cy + offset_bottom.cy);
}

// Draw filled box
void CChart2D::_DrawBorder(CDC *pDC)
{
 CBrush brushCtl(m_BackColor);
 pDC->Rectangle(m_ctlRect);
 pDC->FillRect(m_ctlRect, &brushCtl);
}

// Draw horizontal and vertical axis
void CChart2D::_DrawAxis(CDC *pDC)
{
 CPen pen(PS_SOLID, 2, m_AxisColor);

 CPen *old = pDC->SelectObject(&pen);
 pDC->MoveTo(_MapCoord(m_rangeX.first, m_rangeY.first));
 pDC->LineTo(_MapCoord(m_rangeX.second, m_rangeY.first));
 pDC->MoveTo(_MapCoord(m_rangeX.first, m_rangeY.first));
 pDC->LineTo(_MapCoord(m_rangeX.first, m_rangeY.second));
 pDC->SelectObject(old);
}

CPoint CChart2D::_MapCoord(double x, double y)
{
 //Convert logical coordinates to screen coordinates
 int xPixel = ROUND((x - m_rangeX.first) / m_mapfX);
 int yPixel = ROUND((y - m_rangeY.first) / m_mapfY);

 //Calulate point to be drawn
 CPoint retPt;
 retPt.x = xPixel + m_axisRect.left;
 retPt.y = m_axisRect.bottom - yPixel;
 return retPt;
}

LPoint CChart2D::_UnMapCoord(CPoint pt)
{
 pt.x = pt.x - m_axisRect.left;
 pt.y = (pt.y - m_axisRect.bottom);
 //convert screen coordinates to logical coordinates
 LPoint lp;
 lp.x = m_rangeX.first + (pt.x * m_mapfX);
 lp.y = m_rangeY.first - (pt.y * m_mapfY);
 return lp;
}

void CChart2D::_PlotSeries(CDC *pDC)
{
 for(size_t i = 0 ; i < m_serie.size() ; i++)
 {
  if (!m_serie[i].m_visible)
   continue;
  CPen pen(PS_SOLID, 0, m_serie[i].m_plotColor);
  CPen* old = pDC->SelectObject(&pen);
  CPoint pt = _MapCoord(m_serie[i].mp_valueX[0],m_serie[i].mp_valueY[0]);
  pDC->MoveTo(pt);
  CBrush brushSel(RGB(50,50,255));
  if (m_serie[i].m_handle)
  {
   pDC->Ellipse(pt.x-m_ptHandleRadius, pt.y-m_ptHandleRadius, pt.x+m_ptHandleRadius, pt.y+m_ptHandleRadius);
   if (0 == m_serie[i].m_curSel && IsWindowEnabled())
   {
    CBrush* oldbr = pDC->SelectObject(&brushSel);
    pDC->FloodFill(pt.x, pt.y, m_serie[i].m_plotColor);
    pDC->SelectObject(oldbr);
   }
  }
  //Plot serie data
  for(int index = 1; index <= m_serie[i].m_ptIndex ; index++)
  {
   pt = _MapCoord(m_serie[i].mp_valueX[index], m_serie[i].mp_valueY[index]);
   pDC->LineTo(pt);
   if (m_serie[i].m_handle)
   {    
    pDC->Ellipse(pt.x-m_ptHandleRadius, pt.y-m_ptHandleRadius, pt.x+m_ptHandleRadius, pt.y+m_ptHandleRadius);
    if (index == m_serie[i].m_curSel && IsWindowEnabled())
    {
     CBrush* oldbr = pDC->SelectObject(&brushSel);
     pDC->FloodFill(pt.x, pt.y, m_serie[i].m_plotColor);
     pDC->SelectObject(oldbr);
    }
   }
  }
  pDC->SelectObject(old);
 }
}

void CChart2D::_DrawChartTitle(CDC *pDC)
{
 int x , y, oldbkmode;
 CFont *old;

 old = pDC->SelectObject(::GetFocus() == m_hWnd ? mp_titleFontB : mp_titleFont);
 oldbkmode = pDC->SetBkMode(TRANSPARENT);
 COLORREF oldColor = pDC->SetTextColor(m_TextTitleColor);

 CSize textSize = pDC->GetTextExtent(m_chartTitle, m_chartTitle.GetLength());

 //Calculate centre window MapCoord of text
 pDC->SetTextAlign(TA_LEFT);
 y = m_ctlRect.top;
 x = m_ctlRect.left + (m_ctlRect.Width() / 2) - (textSize.cx / 2);

 pDC->TextOut(x, y, m_chartTitle);
 pDC->SetTextColor(oldColor);
 pDC->SetBkMode(oldbkmode);
 pDC->SelectObject(old);
}

void CChart2D::_DrawGrid(CDC *pDC)
{
 CPen *old , pen(PS_SOLID, 0, m_GridColor), pen_dash;
 LOGBRUSH logBrush;
 logBrush.lbStyle = BS_SOLID;
 logBrush.lbColor = m_GridColor;
 pen_dash.CreatePen(PS_GEOMETRIC|PS_DOT, 1, &logBrush); 

 CPoint m_start, m_stop;
 int i, oldBk;
 double x, y, step;

 old = pDC->SelectObject(&pen);
 oldBk = pDC->SetBkMode(TRANSPARENT);

 // Draw vertical grid 
 step = (m_rangeX.second - m_rangeX.first) / (double)m_grid_x;
 for( i = 0 ; i <= m_grid_x ; i++ )
 {
  if (i == 0 || i == m_grid_x)
   pDC->SelectObject(pen);
  else
   pDC->SelectObject(pen_dash);

  x = m_rangeX.first + (step * (double)i);
  m_start = _MapCoord(x, m_rangeY.first);
  m_stop  = _MapCoord(x, m_rangeY.second);
  pDC->MoveTo(m_start);
  pDC->LineTo(m_stop);
 }

 // Draw horizontal grid
 step = (m_rangeY.second - m_rangeY.first) / (double)m_grid_y;
 for( i = 0; i <= m_grid_y; i++ )
 {
  if (i == 0 || i == m_grid_y)
   pDC->SelectObject(pen);
  else
   pDC->SelectObject(pen_dash);

  y = m_rangeY.first + (step * (double)i);
  m_start = _MapCoord(m_rangeX.first, y);
  m_stop  = _MapCoord(m_rangeX.second, y);
  pDC->MoveTo(m_start);
  pDC->LineTo(m_stop);
 }

 pDC->SetBkMode(oldBk);
 pDC->SelectObject(old);
}

bool CChart2D::SetRange(double min_x, double max_x, double min_y, double max_y)
{
 m_rangeY.first = min_y;
 m_rangeY.second = max_y;
 m_rangeX.first = min_x;
 m_rangeX.second = max_x;
 m_updateBackground = true;
 _CalcMapFactors();
 return true;
}

// Calculate mapping factors
void CChart2D::_CalcMapFactors(void)
{
 if (m_axisRect.Width())
  m_mapfX = (m_rangeX.second - m_rangeX.first) / ((double)m_axisRect.Width());
 if (m_axisRect.Height())
  m_mapfY = (m_rangeY.second - m_rangeY.first) / ((double)m_axisRect.Height());
}

int CChart2D::AddSerie(int nPoints)
{
 m_serie.push_back(CSerieData(nPoints));
 return (int)m_serie.size()-1;
}

bool CChart2D::SetXYValue(int serieIdx, double x, double y, int index)
{
 if (serieIdx > (int)(m_serie.size()-1)) return false;
 if (index >= m_serie[serieIdx].m_ptCount) return false;

 if (x > m_rangeX.second) x = m_rangeX.second;
 if (x < m_rangeX.first) x = m_rangeX.first;
 if (y > m_rangeY.second) y = m_rangeY.second;
 if (y < m_rangeY.first) y = m_rangeY.first;

 m_serie[serieIdx].mp_valueX[index] = x;
 m_serie[serieIdx].mp_valueY[index] = y;

 m_serie[serieIdx].m_ptIndex = index;
 return true;
}

const double* CChart2D::GetYValues(int serieIdx)
{
 ASSERT(serieIdx < (int)(m_serie.size()));
 return m_serie[serieIdx].mp_valueY;
}

// Set chart title
void CChart2D::SetChartTitle(CString str)
{
 m_chartTitle = str;
 m_updateBackground = true;
}

// Set number of grids for horizontal and vertical axis
void CChart2D::SetGridXYNumber(int nx, int ny)
{
 m_grid_x = nx;
 m_grid_y = ny;
 m_updateBackground = true;
}

// Create desired fonts for draw legend in x axis and y axis.
void CChart2D::_CreateFont()
{
 //Create system font
 LOGFONT d_lf;

 // Init desired font
 memset(&d_lf, 0, sizeof(LOGFONT));
 lstrcpy(d_lf.lfFaceName, _T("MS Sans Serif"));

 // Initial font size
 CWindowDC wdc(NULL);
 const int cyPixels = wdc.GetDeviceCaps(LOGPIXELSY);
 d_lf.lfHeight = -1 * MulDiv(7, cyPixels, 72);

 // Create a new font 7 pts
 mp_legendFontY = new CFont();
 mp_legendFontY->CreateFontIndirect(&d_lf);

 d_lf.lfHeight = -1 * MulDiv(12, cyPixels, 72);
 d_lf.lfWeight = FW_REGULAR;
 mp_titleFont = new CFont();
 mp_titleFont->CreateFontIndirect(&d_lf);

 d_lf.lfWeight = FW_BOLD;
 mp_titleFontB = new CFont();
 mp_titleFontB->CreateFontIndirect(&d_lf);

 d_lf.lfWeight = 0 ;
 d_lf.lfOrientation = 900; // Rotate 90 degree for x axis
 d_lf.lfEscapement = 900;
 d_lf.lfHeight = -1 * MulDiv(7, cyPixels, 72);
 mp_legendFontX = new CFont();
 mp_legendFontX->CreateFontIndirect(&d_lf);
}

void CChart2D::_DrawGridLabel(CDC *pDC)
{
 int i;
 CPoint cal_pt;
 CSize txtSize;

 CFont *oldFont = pDC->SelectObject(mp_legendFontY);

 pDC->SetTextAlign(TA_RIGHT);
 pDC->SetBkMode(TRANSPARENT);
 COLORREF oldColor = pDC->SetTextColor(m_TextLabelColor);

 //draw y-axis labels
 double res = (m_rangeY.second - m_rangeY.first) / m_grid_y;
 for (i = 0; i <= m_grid_y; ++i)
 {
  double y = m_rangeY.first + (res * (double)i);
  cal_pt = _MapCoord(m_rangeX.first, y);
  CString str;
  str.Format(m_axisFmtY, y);

  txtSize = pDC->GetTextExtent(str);
  cal_pt.x -= 2;
  cal_pt.y -= txtSize.cy / 2;

  pDC->TextOut(cal_pt.x, cal_pt.y, str);
 }

 //draw x-axis name
 txtSize = pDC->GetTextExtent(m_strLabelX);
 cal_pt.x = m_ctlRect.CenterPoint().x + (txtSize.cx / 2);
 cal_pt.y = m_ctlRect.bottom - txtSize.cy;
 pDC->TextOut(cal_pt.x, cal_pt.y, m_strLabelX);

 //draw x-axis labels
 res = (m_rangeX.second - m_rangeX.first) / m_grid_x;
 for (i = 0; i <= m_grid_x; ++i)
 {
  double y = m_rangeX.first + (res * (double)i);
  cal_pt = _MapCoord(y, m_rangeY.first);
  CString str;
  str.Format(m_axisFmtX,  y);

  txtSize = pDC->GetTextExtent(str);
  cal_pt.x += txtSize.cx / 2;
  cal_pt.y += 0;
  pDC->TextOut(cal_pt.x, cal_pt.y, str);
 }

 pDC->SelectObject(mp_legendFontX);

 //draw y-axis name
 txtSize = pDC->GetTextExtent(m_strLabelY);
 cal_pt.x = m_ctlRect.left + 0;
 cal_pt.y = m_ctlRect.CenterPoint().y  - (txtSize.cx / 2);
 pDC->TextOut(cal_pt.x, cal_pt.y, m_strLabelY);
 pDC->SelectObject(oldFont);
 pDC->SetTextColor(oldColor);
}

void CChart2D::ClearChart(void)
{
 m_setval = false;
 delete mp_tooltip;
 mp_tooltip = NULL;
 for (size_t i = 0; i < m_serie.size(); ++i)
  m_serie[i].m_ptIndex = 0;
 InvalidateRect(m_ctlRect);
}

void CChart2D::SetChartLabel(CString str_x, CString str_y)
{
 m_strLabelX = str_x;
 m_strLabelY = str_y;
}

void CChart2D::OnPaint()
{
 CPaintDC dc(this); // device context for painting

 CDC memPlotDC;
 CBitmap *oldBitmap;
 CBitmap m_plotBitmap;

 // Check if background need to be redrawn
 if (m_updateBackground)
 {
  _DrawBackground(&dc);
  m_updateBackground = false;
 }
 memPlotDC.CreateCompatibleDC(&dc);
 m_plotBitmap.CreateCompatibleBitmap(&dc, m_ctlRect.Width(), m_ctlRect.Height());
 oldBitmap = (CBitmap*)memPlotDC.SelectObject(&m_plotBitmap);

 // BitBlt background
 memPlotDC.BitBlt(0, 0, m_ctlRect.Width(), m_ctlRect.Height(), &memBkDC,0,0,SRCCOPY);
 _PlotSeries(&memPlotDC);

 dc.BitBlt(0,0, m_ctlRect.Width(), m_ctlRect.Height(), &memPlotDC,0,0,SRCCOPY);
 memPlotDC.SelectObject(oldBitmap);
}

void CChart2D::OnEnable(BOOL bEnable)
{
 _SetStateColors(bEnable);
 Invalidate(TRUE);
}

void CChart2D::_SetStateColors(bool state)
{
 if (state)
 { //enabled
  m_BackColor = m_normalBackColor;
  m_AxisColor = m_normalAxisColor;
  m_GridColor = m_normalGridColor;
  m_TextTitleColor = m_normalTitleColor;
  m_TextLabelColor = m_normalLabelColor;
  for (size_t i = 0; i < m_serie.size(); ++i)
   m_serie[i].m_plotColor = m_serie[i].m_normalPlotColor;
 }
 else
 { //disabled
  m_BackColor = GetSysColor(COLOR_3DFACE);
  m_AxisColor = GetSysColor(COLOR_GRAYTEXT);
  m_GridColor = GetSysColor(COLOR_GRAYTEXT);
  m_TextTitleColor = GetSysColor(COLOR_GRAYTEXT);
  m_TextLabelColor = GetSysColor(COLOR_GRAYTEXT);
  for (size_t i = 0; i < m_serie.size(); ++i)
   m_serie[i].m_plotColor = GetSysColor(COLOR_GRAYTEXT);
 }
}

void CChart2D::_DrawBackground(CDC *pDC)
{
 if(memBkDC.GetSafeHdc() == NULL)
  memBkDC.CreateCompatibleDC(pDC);

 if (m_updateBkBitmap && mp_oldBkBitmap && memBkDC.GetSafeHdc())
 {
  memBkDC.SelectObject(mp_oldBkBitmap);
  m_bkBitmap.DeleteObject();
  m_updateBkBitmap = false;
 }

 if (m_bkBitmap.GetSafeHandle() == NULL)
  m_bkBitmap.CreateCompatibleBitmap(pDC, m_ctlRect.Width(), m_ctlRect.Height());

 mp_oldBkBitmap = (CBitmap*)memBkDC.SelectObject(&m_bkBitmap);
 _DrawBorder(&memBkDC);
 _DrawAxis(&memBkDC);
 _DrawGrid(&memBkDC);
 _DrawGridLabel(&memBkDC);
 _DrawChartTitle(&memBkDC);
}

void CChart2D::Invalidate(BOOL bErase)
{
 if (bErase)
  m_updateBackground = true;

 CWnd::Invalidate(bErase);
}

void CChart2D::SetSerieColor(int serieIdx, COLORREF color)
{
 if (serieIdx > (int)m_serie.size()-1) return;
 m_serie[serieIdx].m_normalPlotColor = color;

 if (::IsWindow(m_hWnd))
 {
  _SetStateColors(IsWindowEnabled());
  Invalidate();
 }
}

void CChart2D::SetBackColor(COLORREF color)
{
 m_normalBackColor = color;
 if (::IsWindow(m_hWnd))
 {
  _SetStateColors(IsWindowEnabled());
  Invalidate();
 }
}

void CChart2D::SetAxisColor(COLORREF color)
{
 m_normalAxisColor = color;
 if (::IsWindow(m_hWnd))
 {
  _SetStateColors(IsWindowEnabled());
  Invalidate();
 }
}

void CChart2D::SetGridColor(COLORREF color)
{
 m_normalGridColor = color;
 if (::IsWindow(m_hWnd)) {
  _SetStateColors(IsWindowEnabled());
  Invalidate();
 }
}

void CChart2D::SetTitleColor(COLORREF color)
{
 m_normalTitleColor = color;
 if (::IsWindow(m_hWnd))
 {
  _SetStateColors(IsWindowEnabled());
  Invalidate();
 }
}

void CChart2D::SetLabelColor(COLORREF color)
{
 m_normalLabelColor = color;
 if (::IsWindow(m_hWnd))
 {
  _SetStateColors(IsWindowEnabled());
  Invalidate();
 }
}

void CChart2D::SetXAxisValueFormat(CString fmt)
{
 m_axisFmtX = fmt;
}

void CChart2D::SetYAxisValueFormat(CString fmt)
{
 m_axisFmtY = fmt;
}

void CChart2D::SetSerieVisibility(int serieIdx, bool visible)
{
 if (serieIdx > (int)m_serie.size() - 1) return;
 m_serie[serieIdx].m_visible = visible;

 if (::IsWindow(m_hWnd))
 {
  _SetStateColors(IsWindowEnabled());
  Invalidate();
 } 
}

void CChart2D::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);
 GetClientRect(&m_ctlRect);
 _CalcRect();
 _CalcMapFactors();
 m_updateBackground = true;
 m_updateBkBitmap = true;
 RedrawWindow();
}

void CChart2D::SetSerieHandle(int serieIdx, bool showHandle)
{
 if (serieIdx > (int)m_serie.size() - 1) return;
 m_serie[serieIdx].m_handle = showHandle;

if (::IsWindow(m_hWnd))
 {
  _SetStateColors(IsWindowEnabled());
  Invalidate();
 } 
}

static int EuclDist(CPoint pt1, CPoint pt2)
{
 int dx = pt1.x - pt2.x;
 int dy = pt1.y - pt2.y;
 return ROUND(sqrt((float)(dx*dx + dy*dy)));
}

void CChart2D::OnLButtonDown(UINT nFlags, CPoint point)
{
 SetFocus();
 //find nearest points handle 
 for(size_t i = 0 ; i < m_serie.size() ; i++)
 {
  if (!m_serie[i].m_visible || !m_serie[i].m_handle)
   continue;
  for(int index = 0; index <= m_serie[i].m_ptIndex ; index++)
  {
   CPoint pt = _MapCoord(m_serie[i].mp_valueX[index], m_serie[i].mp_valueY[index]);
   if (EuclDist(pt, point) <= m_ptHandleRadius)
   {
    m_setval  = 1;
    m_val_idx = index;
    m_ser_idx = i;
    m_serie[i].m_curSel = index;
   }
  }
 }

 Super::OnLButtonDown(nFlags, point);
}

void CChart2D::OnMouseMove(UINT nFlags, CPoint point)
{
 if (m_setval)
 {
  LPoint lp = _UnMapCoord(point);
  _RestrictAndSetValue(m_ser_idx, m_val_idx, lp.y);
  Invalidate();
 }

//find nearest points handle 
 size_t val_idx = 0;
 size_t ser_idx = 0;
 bool found = false;
 for(size_t i = 0 ; i < m_serie.size() ; i++)
 {
  if (!m_serie[i].m_visible || !m_serie[i].m_handle)
   continue;
  for(int index = 0; index <= m_serie[i].m_ptIndex ; index++)
  {
   CPoint pt = _MapCoord(m_serie[i].mp_valueX[index], m_serie[i].mp_valueY[index]);
   if (EuclDist(pt, point) <= m_ptHandleRadius)
   {
    if (!mp_tooltip)
    { //activate toolttip
     mp_tooltip = new CToolTipCtrl ();
     mp_tooltip->Create(this);
     mp_tooltip->AddTool(this);
     mp_tooltip->Activate(true);
     mp_tooltip->SetDelayTime(10, TTDT_INITIAL); // length of time the pointer must remain stationary within a tool's bounding rectangle before the tool tip window appears.
     mp_tooltip->SetDelayTime(10, TTDT_AUTOPOP); // amount of time the tooltip window remains visible if the pointer is stationary within a tool's bounding rectangle
     mp_tooltip->SetDelayTime(10, TTDT_RESHOW);  // amount of time it takes for subsequent tooltip windows to appear as the pointer moves from one tool to another.
     mp_tooltip->UpdateTipText(_T(""), this);
    }
    val_idx = index;
    ser_idx = i;
    found = true;
    break;
   }
  }
 }

 if (!found && mp_tooltip && !m_setval)
 { //deactivate tooltip
  mp_tooltip ->Activate(false);
  delete mp_tooltip;
  mp_tooltip = NULL;
 }

 if (mp_tooltip)
 {//get value and restrict it
  if (!found)
  {
   val_idx = m_val_idx;
   ser_idx = m_ser_idx;
  }  
  _UpdateTipText(m_serie[ser_idx].mp_valueY[val_idx]); //Set text value
 } 

 Super::OnMouseMove(nFlags, point);
}

double CChart2D::_RestrictAndSetValue(size_t seridx, size_t index, double v)
{
 if (v > m_rangeY.second)
  v = m_rangeY.second;
 if (v < m_rangeY.first)
  v = m_rangeY.first;
 return m_serie[seridx].mp_valueY[index] = v;
}

void CChart2D::OnLButtonUp(UINT nFlags, CPoint point)
{ 
 if ((m_onChangeCB) && (m_setval))
  m_onChangeCB((int)m_ser_idx);
 m_setval = 0;

 Super::OnLButtonUp(nFlags, point);
}

void CChart2D::SetOnChange(const EventInt& OnChangeCB)
{
 m_onChangeCB = OnChangeCB;
}

void CChart2D::OnSetFocus(CWnd* wnd)
{
 m_updateBackground = true;
 Invalidate();
}

void CChart2D::OnKillFocus(CWnd* wnd)
{
 m_updateBackground = true;
 Invalidate();
}

UINT CChart2D::OnGetDlgCode()
{
 return Super::OnGetDlgCode() | DLGC_WANTARROWS;
}

void CChart2D::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 switch(nChar)
 {
 case VK_UP:
  {
   int index = m_serie[m_ser_idx].m_curSel;
   double new_val = _RestrictAndSetValue(m_ser_idx, index, m_serie[m_ser_idx].mp_valueY[index]+m_ptMovStep);
   Invalidate();   
   if (m_onChangeCB)
    m_onChangeCB((int)m_ser_idx);
  _UpdateTipText(new_val);
   break;
  }
 case VK_DOWN:
  {
   int index = m_serie[m_ser_idx].m_curSel;
   double new_val = _RestrictAndSetValue(m_ser_idx, index, m_serie[m_ser_idx].mp_valueY[index]-m_ptMovStep);
   Invalidate();
   if (m_onChangeCB)
    m_onChangeCB((int)m_ser_idx);
   _UpdateTipText(new_val);
   break;
  }
  case VK_LEFT:
   if (m_serie[m_ser_idx].m_curSel > 0)
    m_serie[m_ser_idx].m_curSel--;
   Invalidate();
   break;
  case VK_RIGHT:
   if (m_serie[m_ser_idx].m_curSel < m_serie[m_ser_idx].m_ptIndex)
    m_serie[m_ser_idx].m_curSel++;
   Invalidate();
   break;
 }
}

void CChart2D::SetPtMovStep(double value)
{
 m_ptMovStep = value;
}

void CChart2D::_UpdateTipText(double val)
{
 if (mp_tooltip)
 {
  CString toolText;
  toolText.Format(m_axisFmtY, val);
  CString str;
  mp_tooltip->GetText(str,this);
  if (str != toolText)
   mp_tooltip->UpdateTipText (toolText, this);
 }
}
