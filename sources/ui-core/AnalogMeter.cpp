 /****************************************************************
 *
 *  Original code written by Mark C. Malburg
 *  Improved by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *   http://secu-3.org
 *   e-mail: shabelnikov-stc@mail.ru
 *  Microprocessor systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include <math.h>
#include "AnalogMeter.h"

//IF You use _UNICODE:
//In the Output category of the Link tab in the Project Settings dialog box,
//set the Entry Point Symbol to wWinMainCRTStartup.

#define ROUND(x) (int)((x) + 0.5 - (double)((x) < 0))

//переводот из градусов в радианы
double CAnalogMeter::DegToRad(double deg)
{
 return ((deg)*m_dPI/180.0);
}

CAnalogMeter::CAnalogMeter()
: m_dLimitAngleDeg(145)
, m_nGridLineWidth(1)
, m_fontType(_T("Times New Roman"))
, m_pbitmapOldGrid(NULL)
, m_pbitmapOldNeedle(NULL)
{
 m_dPI = 4.0 * atan(1.0);  // for trig calculations

 // initialized rectangle locations, will be modified on first drawing
 m_rectDraw    = CRect(0, 0, 0, 0);
 m_nRectWidth  = 0;
 m_nRectHeight = 0;

 // draw the whole thing the first time
 m_boolForceRedraw  = true;
 m_dRadiansPerValue = 0.0;  // will be modified on first drawing

 // false if we are printing
 m_boolUseBitmaps = true;

 // default unit, scaling and needle position
 m_dMinScale   = -10.0;
 m_dMaxScale   =  10.0;
 m_dNeedlePos  =  0.0;
 m_dNeedlePos_n=  0.0;
 m_strTitle    =  "";
 m_strTRPane   =  "";
 m_strTRPane_n =  "";
 m_strUnit     =  "";

 // for numerical values
 m_nFontScale     = 100;
 m_nLabelsDecimals = 1;
 m_nValueDecimals = 1;

 m_nTickNumber = 20;

 // switches
 m_swTitle   = true;
 m_swTRPane  = false;
 m_swGrid    = true;
 m_swLabels  = true;
 m_swValue   = true;
 m_swUnit    = true;
 m_swNeedle  = true;

 // title color
 m_colorTitle     = RGB(128, 128, 128);
 //top-right pane color
 m_colorTRPane    = RGB(0, 0, 255);
 // normal grid color
 m_colorGrid      = RGB(0, 0, 50);
 // current numerical value color
 m_colorValue     = RGB(0, 0, 0);
 // background color
 m_colorBGround   = RGB(202, 202, 202);
 // needle color
 m_colorNeedle    = RGB(255, 0, 0) ;
 // range color
 m_colorLabels    = RGB(0, 0, 0);
 // value color
 m_colorValue     = RGB(0, 0, 0);

 // set pen/brush colors
 ActuateColors();
}

CAnalogMeter::~CAnalogMeter()
{
 if (m_pbitmapOldGrid)
  m_dcGrid.SelectObject(m_pbitmapOldGrid);
 m_dcGrid.DeleteDC();
 if (m_pbitmapOldNeedle)
  m_dcNeedle.SelectObject(m_pbitmapOldNeedle);
 m_dcNeedle.DeleteDC();
 m_bitmapGrid.DeleteObject();
 m_bitmapNeedle.DeleteObject();
 ResetAlertZones();
}

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeter message handlers

void CAnalogMeter::ShowMeter(CDC * pDC, CRect rectBorder)
{
 // check for a new meter or a resize of the old one.
 // (if the rectangles have changed, then redraw from scratch).
 // If we are printing, always draw from scratch without bitmaps.
 if (m_rectOwner != rectBorder)
  m_boolForceRedraw = true;

 if (m_boolForceRedraw || (pDC->IsPrinting()))
 {
  m_boolForceRedraw = false;
  // first store the rectangle for the owner
  // and determine the rectangle to draw to
  m_rectOwner = rectBorder;
  if (pDC->IsPrinting())
  {
   m_boolUseBitmaps = false;
   m_rectDraw       = m_rectOwner;  // draw directly to the owner
  }
  else
  {
   m_boolUseBitmaps  = true;
   m_rectDraw.left   = 0;           // draw to a bitmap rectangle
   m_rectDraw.top    = 0;
   m_rectDraw.right  = rectBorder.Width();
   m_rectDraw.bottom = rectBorder.Height();
  }
  m_nRectWidth  = m_rectDraw.Width();
  m_nRectHeight = m_rectDraw.Height();

  // if we already have a memory dc, destroy it
  // (this occurs for a re-size of the meter)
  if (m_dcGrid.GetSafeHdc())
  {
   m_dcGrid.SelectObject(m_pbitmapOldGrid);
   m_dcGrid.DeleteDC();
   m_dcNeedle.SelectObject(m_pbitmapOldNeedle);
   m_dcNeedle.DeleteDC();
   m_bitmapGrid.DeleteObject();
   m_bitmapNeedle.DeleteObject();
  }
  if (m_boolUseBitmaps)
  {
   // create a memory based dc for drawing the grid
   m_dcGrid.CreateCompatibleDC(pDC);
   m_bitmapGrid.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight);
   m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
   // create a memory based dc for drawing the needle
   m_dcNeedle.CreateCompatibleDC(pDC);
   m_bitmapNeedle.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight);
   m_pbitmapOldNeedle = m_dcNeedle.SelectObject(&m_bitmapNeedle);
  }
  else  // no bitmaps, draw to the destination
  {
   // use the destination dc for the grid
   m_dcGrid.m_hDC = pDC->m_hDC;
   m_dcGrid.m_hAttribDC = pDC->m_hAttribDC;
   // use the destination dc for the grid
   m_dcNeedle.m_hDC = pDC->m_hDC;
   m_dcNeedle.m_hAttribDC = pDC->m_hAttribDC;
  }

  // draw the grid in to the "grid dc"
  DrawScale();
  // draw the needle in the "needle dc"
  DrawNeedle();
 }
 // display the new image, combining the needle with the grid
 if (m_boolUseBitmaps)
  ShowMeterImage(pDC);
}

//////////////////////////////////////////////////////
void CAnalogMeter::Update(CDC *pDC)
{
 // if the needle hasn't changed, don't bother updating
 if ((m_dNeedlePos == m_dNeedlePos_n) && (m_strTRPane == m_strTRPane_n))
  return;

 // store the values in the member variables
 // for availability elsewhere
 if (m_dNeedlePos != m_dNeedlePos_n)
  m_dNeedlePos = m_dNeedlePos_n;
 if (m_strTRPane != m_strTRPane_n)
  m_strTRPane = m_strTRPane_n;

 // do not support updates if we are not working with
 // bitmaps images
 if (!m_boolUseBitmaps)
  return;

 // must have created the grid if we are going to
 // update the needle (the needle locations are
 // calculateed based on the grid)
 if (!m_dcGrid.GetSafeHdc())
  return;

 // draw the new needle image
 DrawNeedle();

 // combine the needle with the grid and display the result
 if (m_boolUseBitmaps)
  ShowMeterImage (pDC);
}

//////////////////////////////////////////
void CAnalogMeter::DrawScale()
{
 double   dTemp;
 CPen*    pPenOld;
 CBrush*  pBrushOld;
 CFont*   pFontOld;
 CString  tempString;

 bool disable_title;
 bool disable_range;
 bool disable_unit;

 m_rectGfx = m_rectDraw;

 ///////////////////////
 //  clear background //
 ///////////////////////

 // new pen / brush
 pPenOld   = NULL;
 pBrushOld = NULL;
 if(m_PenG_BGround.m_hObject)
  pPenOld = m_dcGrid.SelectObject(&m_PenG_BGround);
 if(m_BrushG_BGround.m_hObject)
  pBrushOld = m_dcGrid.SelectObject(&m_BrushG_BGround);

 m_dcGrid.Rectangle(m_rectGfx);
 m_rectGfx.DeflateRect(3,3);
 m_dcGrid.Draw3dRect(m_rectGfx, (~RGB(255, 255, 255))&0xFFFFFF, (~RGB(0, 0, 0))&0xFFFFFF);
 m_rectGfx.DeflateRect(1,1);
 m_dcGrid.Draw3dRect(m_rectGfx, (~RGB(220, 220, 220))&0xFFFFFF, (~RGB(127, 127, 127))&0xFFFFFF);
 m_rectGfx.DeflateRect(4,4);

 // old pen / brush
 if (pPenOld)
  m_dcGrid.SelectObject(pPenOld);

 if (pBrushOld)
  m_dcGrid.SelectObject(pBrushOld);

 ///////////////////////
 //  check size       //
 ///////////////////////
 disable_title = false;
 disable_range = false;
 disable_unit  = false;
 if((m_rectGfx.Height() < 50) || (m_rectGfx.Width() < 50))
 {
  disable_title = true;
  disable_range = true;
  disable_unit  = true;
 }
 if((m_rectGfx.Height() < 20) || (m_rectGfx.Width() < 20))
  return;

 // make a square
 if(m_rectGfx.Height() > m_rectGfx.Width())
  m_rectGfx.DeflateRect(0, (m_rectGfx.Height() - m_rectGfx.Width())/2);
 if(m_rectGfx.Height() < m_rectGfx.Width())
  m_rectGfx.DeflateRect((m_rectGfx.Width() - m_rectGfx.Height())/2, 0);

 ///////////////////////
 //  create font      //
 ///////////////////////
 m_nFontHeight = m_rectGfx.Height()/4;

 if(((m_rectGfx.Width()) > 0) && ((m_rectGfx.Height()) > 0))
 {
  int    height = 0;
  int    width  = 0;
  double scale = 1.0;
  do
  {
   m_nFontHeight = (int)((double)m_nFontHeight * scale);
   m_fontValue.DeleteObject() ;
   if(!m_fontValue.CreateFont (m_nFontHeight, 0, 0, 0, 400,
     false, false, 0, RUSSIAN_CHARSET,
     OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
     DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, m_fontType))
    return;

   tempString.Format(_T("%.*f"), m_nLabelsDecimals, m_dMinScale);
   int len1 = tempString.GetLength();
   tempString.Format(_T("%.*f"), m_nLabelsDecimals, m_dMaxScale);
   int len2 = tempString.GetLength();

   if (len1 > len2)
    width = len1;
   else
    width = len2;

   TEXTMETRIC TM;
   pFontOld = m_dcGrid.SelectObject(&m_fontValue);
   m_dcGrid.GetTextMetrics(&TM);
   height = TM.tmHeight;
   width  = TM.tmAveCharWidth * width;
   m_dcGrid.SelectObject(pFontOld) ;

   scale -= 0.01;
  } while(((height > (m_rectGfx.Height()/8)*m_nFontScale/100) || (width > (m_rectGfx.Width()/3))) && (scale > 0.0));
 }

 // place for title
 if(!disable_title && m_swTitle && m_strTitle != "")
 {
  TEXTMETRIC TM;
  pFontOld = m_dcGrid.SelectObject(&m_fontValue);
  m_dcGrid.GetTextMetrics(&TM);
  m_rectGfx.top += TM.tmHeight + 3;
  m_rectGfx.DeflateRect(TM.tmHeight, 0);
  m_dcGrid.SelectObject(pFontOld);
 }

 // determine the angular scaling
 m_dLimitAngleRad = DegToRad(m_dLimitAngleDeg);
 m_dRadiansPerValue = (2.0*m_dLimitAngleRad)/(m_dMaxScale-m_dMinScale);

 // determine the center point
 m_nCXPix = m_rectGfx.left + m_rectGfx.Width()/2;
 m_nCYPix = m_rectGfx.top  + m_rectGfx.Height()/2;

 // determine the size and location of the meter "pie"
 m_nRadiusPix     = m_rectGfx.Height()*50/100;
 m_nHalfBaseWidth = m_nRadiusPix/50;
 dTemp = m_nCXPix - m_nRadiusPix*sin(m_dLimitAngleRad);
 m_nLeftLimitXPix = ROUND(dTemp);
 dTemp = m_nCYPix - m_nRadiusPix*cos(m_dLimitAngleRad);
 m_nLeftLimitYPix = ROUND(dTemp);

 dTemp = m_nCXPix + m_nRadiusPix*sin(m_dLimitAngleRad);
 m_nRightLimitXPix = ROUND(dTemp);
 m_nRightLimitYPix = m_nLeftLimitYPix;

 // determine the placement of the current value text
 m_rectValue.left   = m_rectGfx.left  + 10;
 m_rectValue.top    = m_rectGfx.top   + 10;
 m_rectValue.right  = m_rectGfx.right - 10;
 m_rectValue.bottom = ROUND(m_nCYPix + m_nRadiusPix * 0.9);

 // determine the placement of the minimum value
 m_rectMinValue.left   = m_rectGfx.left + 1;
 m_rectMinValue.top    = m_rectGfx.bottom - m_rectGfx.Height()/5;
 m_rectMinValue.right  = m_rectGfx.left + m_rectGfx.Width()/2 + 1;
 m_rectMinValue.bottom = m_rectGfx.bottom - m_rectGfx.Height()/50;

 // determine the placement of the maximum value
 m_rectMaxValue.right  = m_rectGfx.right - 1;
 m_rectMaxValue.top    = m_rectGfx.bottom - m_rectGfx.Height()/5;
 m_rectMaxValue.left   = m_rectGfx.left + m_rectGfx.Width()/2 + 1;
 m_rectMaxValue.bottom = m_rectGfx.bottom - m_rectGfx.Height()/50;

 int nLeftBoundX, nRightBoundX,nLeftBoundY, nRightBoundY;
 nLeftBoundX  = m_nCXPix - m_nRadiusPix;
 nRightBoundX = m_nCXPix + m_nRadiusPix;
 nLeftBoundY  = m_nCYPix - m_nRadiusPix;
 nRightBoundY = m_nCYPix + m_nRadiusPix;

 if(m_strUnit == "")
  m_rectValue.bottom += m_nFontHeight;
 else
  m_rectValue.bottom += m_nFontHeight/2;

 ///////////////////////
 //  draw grid        //
 ///////////////////////
 CRect Bounds(nLeftBoundX,nLeftBoundY,nRightBoundX,nRightBoundY);

 DrawAlertZones(Bounds);
 DrawChord(Bounds);
 DrawGrid(Bounds);

 // grab the font and set the text color
 pFontOld = m_dcGrid.SelectObject(&m_fontValue);
 m_dcGrid.SetTextColor((~m_colorLabels)&0xFFFFFF);
 m_dcGrid.SetBkColor((~m_colorBGround)&0xFFFFFF);
 m_nTextBaseSpacing = m_rectMinValue.Height()/4;

 if(!disable_title && m_swTitle && m_strTitle != "")
 {
  // show the title
  m_dcGrid.SetTextAlign(TA_CENTER|TA_BOTTOM);
  m_dcGrid.SetTextColor((~m_colorTitle)&0xFFFFFF);
  m_dcGrid.SetBkColor((~m_colorBGround)&0xFFFFFF);
  m_dcGrid.TextOut ((m_rectGfx.left+m_rectGfx.right)/2,
  m_rectGfx.top - 3, m_strTitle);
 }

 if(!disable_unit && m_swUnit && m_strUnit != "")
 {
  // show the unit
  m_dcGrid.SetTextAlign(TA_CENTER|TA_BOTTOM);
  m_dcGrid.SetTextColor((~m_colorValue)&0xFFFFFF);
  m_dcGrid.SetBkColor((~m_colorBGround)&0xFFFFFF);
  m_dcGrid.TextOut ((m_rectGfx.left+m_rectGfx.right)/2,
  m_nCYPix - m_nRadiusPix/2 + m_nFontHeight + m_nFontHeight/2, m_strUnit);
 }

 // restore the font
 m_dcGrid.SelectObject(pFontOld);
}

//рисует центр прибора использу€ цвет фона
void CAnalogMeter::DrawChord(const CRect& Bounds)
{
 if(!m_swGrid)
  return;

 CRect Circle = Bounds;
 CBrush*  pBrushOld = NULL;
 CPen* pPenOld = NULL;
 CBrush chord_brush;
 CPen   chord_pen;

 chord_brush.CreateSolidBrush((~m_colorBGround)&0xFFFFFF);
 chord_pen.CreatePen(PS_NULL,0,(~m_colorBGround)&0xFFFFFF);

 int r = m_nRadiusPix / 5;

 Circle.DeflateRect(r,r,r,r);

 CPoint ptStart(ROUND(m_nCXPix + m_nRadiusPix*sin(m_dLimitAngleRad)),
 ROUND(m_nCYPix - m_nRadiusPix*cos(m_dLimitAngleRad)));
 CPoint ptEnd(m_nLeftLimitXPix, m_nLeftLimitYPix);

 pBrushOld = m_dcGrid.SelectObject(&chord_brush);
 pPenOld = m_dcGrid.SelectObject(&chord_pen);
 m_dcGrid.Chord(Circle,ptStart,ptEnd);
 m_dcGrid.SelectObject(pPenOld);
 m_dcGrid.SelectObject(pBrushOld);
}

void  CAnalogMeter::DrawAlertZones(const CRect& Bounds)
{
 if(m_swGrid)
 {
  //рисуем зоны
  std::vector<AlertZone*>::iterator it;
  for(it = m_AlertZones.begin(); it!=m_AlertZones.end(); ++it)
  {
   DrawPie(Bounds,(*it)->start,(*it)->end,(*it)->color);
  }
 }
}

//–исует шкалу со штрихами и надпис€ми
void CAnalogMeter::DrawGrid(const CRect& Bounds)
{
 double dX,dY,len;
 int tick;
 CPen*    pPenOld;
 CBrush*  pBrushOld;
 CFont*   pFontOld ;

 // new pen / brush
 pPenOld   = NULL;
 pBrushOld = NULL;
 if(m_PenG_Grid.m_hObject)
  pPenOld = m_dcGrid.SelectObject(&m_PenG_Grid);

 if(m_BrushG_Grid.m_hObject)
  pBrushOld = m_dcGrid.SelectObject(&m_BrushG_Grid);

 if(m_swGrid)
 {
  // arc
  m_dcGrid.Arc(Bounds.left,Bounds.top,Bounds.right+1,Bounds.bottom+1,
     ROUND(m_nCXPix + m_nRadiusPix*sin(m_dLimitAngleRad)),
     ROUND(m_nCYPix - m_nRadiusPix*cos(m_dLimitAngleRad)),
     m_nLeftLimitXPix, m_nLeftLimitYPix) ;
 }

 if (m_nTickNumber%2)
  m_nTickNumber--;

 double tick_value = m_dMinScale;
 double rad_per_tick = ((double)m_nTickNumber * 2.0) / (m_nTickNumber*m_nTickNumber);

 int step_min = -(m_nTickNumber/2);
 int step_max = m_nTickNumber - (m_nTickNumber/2);

 for (tick = step_min; tick <= step_max; tick++)
 {
  //-------draw ticks ---------------
  dX = m_nCXPix + m_nRadiusPix*sin(m_dLimitAngleRad*tick*rad_per_tick);
  dY = m_nCYPix - m_nRadiusPix*cos(m_dLimitAngleRad*tick*rad_per_tick);
  if(m_swGrid) m_dcGrid.MoveTo(ROUND(dX), ROUND(dY)) ;
  if (tick % 2)
   len = 1.0;    //short tick
  else
   len = 0.95;   //long tick
  dX = m_nCXPix + 0.92*len*m_nRadiusPix*sin(m_dLimitAngleRad*tick*rad_per_tick);
  dY = m_nCYPix - 0.92*len*m_nRadiusPix*cos(m_dLimitAngleRad*tick*rad_per_tick);
  if(m_swGrid) m_dcGrid.LineTo(ROUND(dX), ROUND(dY)) ;

  if (!m_swLabels) //нужно ли рисовать подписи?
   continue;

  //-------draw text labels for long ticks --------
  if(!(tick % 2))
  {
   CString cs;
   cs.Format(_T("%.*f"),m_nLabelsDecimals,tick_value);
   TEXTMETRIC TM;

   pFontOld = m_dcGrid.SelectObject(&m_fontValue);
   m_dcGrid.GetTextMetrics(&TM);

   double tw = (double)(TM.tmAveCharWidth * cs.GetLength());
   double th = (double)TM.tmHeight;

   double r = 0.92*len*m_nRadiusPix*0.8;

   double fX = m_nCXPix + r*sin(m_dLimitAngleRad*tick*rad_per_tick);
   double fY = m_nCYPix - r*cos(m_dLimitAngleRad*tick*rad_per_tick);

   double tx = fX - tw/2;
   double ty = fY - th/2;

   m_dcGrid.SetTextColor((~m_colorLabels)&0xFFFFFF);
   m_dcGrid.SetBkColor((~m_colorBGround)&0xFFFFFF);
   m_dcGrid.SetBkMode(TRANSPARENT);

   if (tick==0)
    ty=m_nCYPix - 0.92*len*m_nRadiusPix*cos(m_dLimitAngleRad*tick*rad_per_tick);

   m_dcGrid.TextOut((int)tx,(int)ty,cs);
   m_dcGrid.SelectObject(pFontOld);
  }
  //------------------------------------------------

  double step = ((double)(m_dMaxScale - m_dMinScale)) / ((double)m_nTickNumber);
  tick_value+= step;
 }

 // old pen / brush
 if (pPenOld)
  m_dcGrid.SelectObject(pPenOld);
 if (pBrushOld)
  m_dcGrid.SelectObject(pBrushOld);
}

//рисует сегмент окружности с заданным цветом
void CAnalogMeter::DrawPie(const CRect& Bounds,double start_value, double end_value, COLORREF color)
{
 CBrush pie_brush;
 CPen   pie_pen;
 CBrush* pBrushOld;
 CPen* pPenOld;

 double DergeesPerValue = (2.0*m_dLimitAngleDeg)/(m_dMaxScale-m_dMinScale);

 double s_ang = DergeesPerValue*(start_value - (m_dMinScale));
 double e_ang = DergeesPerValue*(end_value - (m_dMinScale));

 double start_radian = DegToRad(m_dLimitAngleDeg - s_ang);
 double end_radian = DegToRad(e_ang-m_dLimitAngleDeg);

 pie_brush.CreateSolidBrush(~(color)&0xFFFFFF);
 pie_pen.CreatePen(PS_NULL,0,~(color)&0xFFFFFF);

 pBrushOld = m_dcGrid.SelectObject(&pie_brush);
 pPenOld = m_dcGrid.SelectObject(&pie_pen);

 m_dcGrid.Pie(Bounds.left,Bounds.top,Bounds.right+1,Bounds.bottom+1,
     ROUND(m_nCXPix + m_nRadiusPix*sin(end_radian)),
     ROUND(m_nCYPix - m_nRadiusPix*cos(end_radian)),
     ROUND(m_nCXPix - m_nRadiusPix*sin(start_radian)),
     ROUND(m_nCYPix - m_nRadiusPix*cos(start_radian)));

 m_dcGrid.SelectObject(pPenOld);
 m_dcGrid.SelectObject(pBrushOld);
}

//рисует стрелку прибора
void CAnalogMeter::DrawNeedle()
{
 CPoint  pPoints[6];
 CString tempString;
 CPen*   pPenOld;
 CBrush* pBrushOld;
 CFont*  pFontOld;
 double  dAngleRad, dX, dY;
 double  dCosAngle, dSinAngle;
 bool    disable_value;
 bool    disable_trpane;

 if (!m_dcNeedle.GetSafeHdc())
  return;

 if(m_boolUseBitmaps)
 {
  // new pen / brush
  pPenOld   = NULL;
  pBrushOld = NULL;
  if(m_PenN_BGround.m_hObject)
   pPenOld = m_dcNeedle.SelectObject(&m_PenN_BGround);

  if(m_BrushN_BGround.m_hObject)
   pBrushOld = m_dcNeedle.SelectObject(&m_BrushN_BGround);

  m_dcNeedle.Rectangle(m_rectDraw);

  // old pen / brush
  if(pPenOld)
   m_dcGrid.SelectObject(pPenOld);

  if(pBrushOld)
   m_dcGrid.SelectObject(pBrushOld);
 }

 ///////////////////////
 // check sizes       //
 ///////////////////////
 disable_value = false;
 disable_trpane = false;
 if((m_rectGfx.Height() < 50) || (m_rectGfx.Width() < 50))
 {
  disable_value = true;
  disable_trpane = true;
 }
 if((m_rectGfx.Height() < 20) || (m_rectGfx.Width() < 20))
  return;

 if(!disable_value && m_swValue)
 {
  pFontOld = m_dcNeedle.SelectObject(&m_fontValue);
  m_dcNeedle.SetTextAlign(TA_CENTER|TA_BASELINE);
  m_dcNeedle.SetTextColor(m_colorValue^m_colorBGround);
  m_dcNeedle.SetBkColor(RGB(0,0,0));
  tempString.Format(_T("%.*f"), m_nValueDecimals, m_dNeedlePos);

  m_dcNeedle.TextOut((m_rectValue.right+m_rectValue.left)/2,
      m_rectValue.bottom-m_nTextBaseSpacing, tempString);

  m_dcNeedle.SelectObject(pFontOld);
 }

 if (!disable_trpane && m_swTRPane) //top-right pane
 {
  pFontOld = m_dcNeedle.SelectObject(&m_fontValue);
  m_dcNeedle.SetTextAlign(TA_RIGHT | TA_TOP);
  m_dcNeedle.SetTextColor(m_colorTRPane^m_colorBGround);
  m_dcNeedle.SetBkColor(RGB(0,0,0)); 
  CRect rect = m_rectDraw;
  rect.DeflateRect(8,3);
  m_dcNeedle.TextOut(rect.right, rect.top, m_strTRPane);
  m_dcNeedle.SelectObject(pFontOld); 
 }

 dAngleRad = (m_dNeedlePos - m_dMinScale)*m_dRadiansPerValue - m_dLimitAngleRad;
 dAngleRad = max(dAngleRad, -m_dLimitAngleRad);
 dAngleRad = min(dAngleRad, m_dLimitAngleRad);
 dCosAngle = cos(dAngleRad);
 dSinAngle = sin(dAngleRad);

 // tip
 dX = m_nCXPix + m_nRadiusPix*dSinAngle;
 dY = m_nCYPix - m_nRadiusPix*dCosAngle;
 pPoints[0].x = ROUND(dX);
 pPoints[0].y = ROUND(dY);

 // left base
 dX = m_nCXPix - m_nHalfBaseWidth*dCosAngle;
 dY = m_nCYPix - m_nHalfBaseWidth*dSinAngle;
 pPoints[1].x = ROUND(dX);
 pPoints[1].y = ROUND(dY);

 // right base
 pPoints[2].x = m_nCXPix + (m_nCXPix-pPoints[1].x);
 pPoints[2].y = m_nCYPix + (m_nCYPix-pPoints[1].y);

 // tip
 pPoints[3].x = pPoints[0].x;
 pPoints[3].y = pPoints[0].y;

 // new pen / brush
 pPenOld   = NULL;
 pBrushOld = NULL;
 if(m_PenN_Needle.m_hObject)
  pPenOld = m_dcNeedle.SelectObject(&m_PenN_Needle);

 if(m_BrushN_Needle.m_hObject)
  pBrushOld = m_dcNeedle.SelectObject(&m_BrushN_Needle);

 if (m_swNeedle)
 {
  // draw the needle pointer
  m_dcNeedle.Polygon(pPoints, 4);

  // draw circle at the bottom
  int r = (m_nRadiusPix/20);
  m_dcNeedle.Ellipse (m_nCXPix-m_nHalfBaseWidth-r, m_nCYPix-m_nHalfBaseWidth-r,
  m_nCXPix+m_nHalfBaseWidth+1+r, m_nCYPix+m_nHalfBaseWidth+1+r);
 }

 // old pen / brush
 if(pPenOld)
  m_dcGrid.SelectObject(pPenOld);

 if(pBrushOld)
  m_dcGrid.SelectObject(pBrushOld);

} // end DrawNeedle

///////////////////////////////////
void CAnalogMeter::ShowMeterImage(CDC *pDC)
{
 CDC memDC;
 CBitmap memBitmap;
 CBitmap* oldBitmap; // bitmap originally found in CMemDC

 // this function is only used when the needle and grid
 // have been drawn to bitmaps and they need to be combined
 // and sent to the destination
 if (!m_boolUseBitmaps)
  return;

 // to avoid flicker, establish a memory dc, draw to it
 // and then BitBlt it to the destination "pDC"
 memDC.CreateCompatibleDC(pDC);
 memBitmap.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight);
 oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap);

 // make sure we have the bitmaps
 if (!m_dcGrid.GetSafeHdc())
  return;

 if (!m_dcNeedle.GetSafeHdc())
  return;

 if (memDC.GetSafeHdc() != NULL)
 {
  // draw the inverted grid
  memDC.BitBlt(0, 0, m_nRectWidth, m_nRectHeight, &m_dcGrid, 0, 0, NOTSRCCOPY);
  // merge the needle image with the grid
  memDC.BitBlt(0, 0, m_nRectWidth, m_nRectHeight, &m_dcNeedle, 0, 0, SRCINVERT);
  // copy the resulting bitmap to the destination
  pDC->BitBlt(m_rectOwner.left, m_rectOwner.top, m_nRectWidth, m_nRectHeight,
      &memDC, 0, 0, SRCCOPY);
 }

 memDC.SelectObject(oldBitmap);
} // end ShowMeterImage

//////////////////////////////////////////////////////
void CAnalogMeter::ActuateColors()
{
 if(m_PenG_Grid.m_hObject)
  m_PenG_Grid.DeleteObject();

 if(m_PenG_Grid.m_hObject == NULL)
  m_PenG_Grid.CreatePen(PS_SOLID, m_nGridLineWidth, (~m_colorGrid)&0xFFFFFF);

 if(m_BrushG_Grid.m_hObject)
  m_BrushG_Grid.DeleteObject();

 if(m_BrushG_Grid.m_hObject == NULL)
  m_BrushG_Grid.CreateSolidBrush((~m_colorGrid)&0xFFFFFF);

 if(m_PenG_BGround.m_hObject)
  m_PenG_BGround.DeleteObject();

 if(m_PenG_BGround.m_hObject == NULL)
  m_PenG_BGround.CreatePen(PS_SOLID, 1, (~m_colorBGround)&0xFFFFFF);

 if(m_BrushG_BGround.m_hObject)
  m_BrushG_BGround.DeleteObject();

 if(m_BrushG_BGround.m_hObject == NULL)
  m_BrushG_BGround.CreateSolidBrush((~m_colorBGround)&0xFFFFFF);

 if(m_PenN_Needle.m_hObject)
  m_PenN_Needle.DeleteObject();

 if(m_PenN_Needle.m_hObject == NULL)
  m_PenN_Needle.CreatePen(PS_SOLID, 0, (m_colorNeedle^m_colorBGround)&0xFFFFFF);

 if(m_BrushN_Needle.m_hObject)
  m_BrushN_Needle.DeleteObject();

 if(m_BrushN_Needle.m_hObject == NULL)
  m_BrushN_Needle.CreateSolidBrush((m_colorNeedle^m_colorBGround)&0xFFFFFF);

 if(m_PenN_BGround.m_hObject)
  m_PenN_BGround.DeleteObject();

 if(m_PenN_BGround.m_hObject == NULL)
  m_PenN_BGround.CreatePen(PS_SOLID, 0, RGB(0,0,0));

 if(m_BrushN_BGround.m_hObject)
  m_BrushN_BGround.DeleteObject();

 if(m_BrushN_BGround.m_hObject == NULL)
  m_BrushN_BGround.CreateSolidBrush(RGB(0,0,0));
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetState(enum MeterMemberEnum meter_member, bool State)
{
 switch(meter_member)
 {
  case meter_title:
   m_swTitle = State;
   break;

  case meter_trpane:
   m_swTRPane = State;
   break;

  case meter_grid:
   m_swGrid = State;
   break;

  case meter_value:
   m_swValue = State;
   break;

  case meter_labels:
   m_swLabels = State;
   break;

  case meter_unit:
   m_swUnit = State;
   break;

  case meter_needle:
   m_swNeedle = State;
   break;
 }
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetColor(enum MeterMemberEnum meter_member, COLORREF Color)
{
 switch(meter_member)
 {
  case meter_title:
   m_colorTitle = Color;
   break;

  case meter_trpane:
   m_colorTRPane = Color;
   break;

  case meter_needle:
   m_colorNeedle = Color;
   break;

  case meter_grid:
   m_colorGrid = Color;
   break;

  case meter_value:
   m_colorValue = Color;
   break;

  case meter_labels:
   m_colorLabels = Color;
   break;

  case meter_bground:
   m_colorBGround = Color;
   break;
 }

 // set pen/brush colors
 ActuateColors();
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetRange(double dMin, double dMax)
{
 // The function does NOT force the re-drawing of the meter.
 // The owner must explicitly call the ShowMeter function
 m_dMinScale   = dMin;
 m_dMaxScale   = dMax;
 m_boolForceRedraw = true;
}

void CAnalogMeter::SetNeedleValue(double value)
{
 m_dNeedlePos_n = value;
}

void CAnalogMeter::SetTRPane(CString strPane)
{
 m_strTRPane_n = strPane;
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetFontScale(int nFontScale)
{
 // The function does NOT force the re-drawing of the meter.
 // The owner must explicitly call the ShowMeter function
 if(m_nFontScale < 1)   m_nFontScale = 1;
 if(m_nFontScale > 100) m_nFontScale = 100;
 m_nFontScale = nFontScale;
 m_boolForceRedraw = true;
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetLabelsDecimals(int nLabelsDecimals)
{
 // The function does NOT force the re-drawing of the meter.
 // The owner must explicitly call the ShowMeter function
 if(m_nLabelsDecimals < 0)   m_nLabelsDecimals = 0;
 if(m_nLabelsDecimals > 100) m_nLabelsDecimals = 100;
 m_nLabelsDecimals = nLabelsDecimals;
 m_boolForceRedraw = true;
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetValueDecimals(int nValueDecimals)
{
 // The function does NOT force the re-drawing of the meter.
 // The owner must explicitly call the ShowMeter function
 if(m_nValueDecimals < 0)   m_nValueDecimals = 0;
 if(m_nValueDecimals > 100) m_nValueDecimals = 100;
 m_nValueDecimals = nValueDecimals;
 m_boolForceRedraw = true;
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetTitle(CString strTitle)
{
 // The function does NOT force the re-drawing of the meter.
 // The owner must explicitly call the ShowMeter function
 m_strTitle = strTitle;
 m_boolForceRedraw = true;
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetUnit(CString strUnit)
{
 // The function does NOT force the re-drawing of the meter.
 // The owner must explicitly call the ShowMeter function
 m_strUnit = strUnit;
 m_boolForceRedraw = true;
}

//////////////////////////////////////////////////////
void CAnalogMeter::GetColor(enum MeterMemberEnum meter_member, COLORREF* pColor) const
{
 switch(meter_member)
 {
  case meter_title:
   *pColor = m_colorTitle;
   break;

  case meter_trpane:
   *pColor = m_colorTRPane;
   break;

  case meter_needle:
   *pColor = m_colorNeedle;
   break;

  case meter_grid:
   *pColor = m_colorGrid;
   break;

  case meter_value:
   *pColor = m_colorValue;
   break;

  case meter_labels:
   *pColor = m_colorLabels;
   break;

  case meter_bground:
   *pColor = m_colorBGround;
   break;
 }
}

//////////////////////////////////////////////////////
void CAnalogMeter::GetState(enum MeterMemberEnum meter_member, bool* pState) const
{
 switch(meter_member)
 {
  case meter_title:
   *pState = m_swTitle;
   break;

  case meter_trpane:
   *pState = m_swTRPane;
   break;

  case meter_grid:
   *pState = m_swGrid;
   break;

  case meter_value:
   *pState = m_swValue;
   break;

  case meter_labels:
   *pState = m_swLabels;
   break;

  case meter_unit:
   *pState = m_swUnit;
   break;
 }
}

//добавл€ет зону с заданными параметрами (диапазон и цвет)
void CAnalogMeter::AddAlertZone(double start,double end, COLORREF color)
{
 AlertZone* p_az  = new AlertZone;
 p_az->start = start;
 p_az->end   = end;
 p_az->color = color;

 m_AlertZones.push_back(p_az);
}

//удал€ет все зоны
void CAnalogMeter::ResetAlertZones(void)
{
 std::vector<AlertZone*>::iterator it;

 for(it = m_AlertZones.begin(); it!=m_AlertZones.end(); ++it)
 {
  delete (*it);
 }

 m_AlertZones.clear();
}
