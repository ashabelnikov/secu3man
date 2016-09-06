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

/** \file AnalogMeter.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <algorithm>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// Enum Types
enum MeterMemberEnum
{
 meter_title,   //заголовок (название прибора)
 meter_tlpane,  //top-left pane
 meter_trpane,  //top-right pane
 meter_needle,  //стрелка
 meter_grid,    //деления шкалы
 meter_value,   //числовое значение отображаемое внизу
 meter_labels,  //подписи напротив каждого деления
 meter_unit,    //единицы измерения
 meter_bground, //фон
};

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeter command target
class AFX_EXT_CLASS CAnalogMeter
{
 public:
  CAnalogMeter() ;
  virtual ~CAnalogMeter();

 // Operations
 public:
  void ShowMeter(CDC *pDC, CRect rectBorder);
  virtual void Update(CDC *pDC);

  void SetColor(enum MeterMemberEnum meter_member, COLORREF Color);
  void SetState(enum MeterMemberEnum meter_member, bool State);
  void SetRange(double dMin, double dMax);
  void SetNeedleValue(double value);
  void SetTLPane(CString strPane);
  void SetTRPane(CString strPane);
  void SetFontScale(int nFontScale);
  void SetLabelsDecimals(int nRangeDecimals);
  void SetValueDecimals(int nValueDecimals);
  void SetTitle(CString strTitle);
  void SetUnit(CString strUnit);
  void SetGridLineWidth(int width)
  {
   m_nGridLineWidth = width;  // set pen/brush colors
   ActuateColors();
  }

  void SetMeterSize(double half_angle) {m_dLimitAngleDeg = half_angle;}
  void SetTickNumber(int number) { m_nTickNumber = number;};

  void AddAlertZone(double start,double end, COLORREF color);
  void ResetAlertZones(void);

  void    GetColor(enum MeterMemberEnum meter_member, COLORREF* pColor) const;
  void    GetState(enum MeterMemberEnum meter_member, bool* pState) const;
  double  GetMinRange() const {return m_dMinScale; };
  double  GetMaxRange() const {return m_dMaxScale; };
  int     GetFontScale() const {return m_nFontScale; };
  int     GetRangeDecimals() const {return m_nLabelsDecimals; };
  int     GetValueDecimals() const {return m_nValueDecimals; };
  CString GetTitle() const {return m_strTitle; };
  CString GetTLPane() const {return m_strTLPane_n; };
  CString GetTRPane() const {return m_strTRPane_n; };
  CString GetUnit() const {return m_strUnit; };
  double  GetNeedlePos(void) const {return m_dNeedlePos_n;};

 protected:
  bool m_boolUseBitmaps;
  bool m_boolForceRedraw;

 // Implementation
 private:
  double DegToRad(double deg);
  void DrawPie(const CRect& Bounds,double start_value, double end_value, COLORREF color);
  void DrawGrid(const CRect& Bounds);
  void DrawChord(const CRect& Bounds);

  void DrawScale();
  void DrawNeedle();
  void ShowMeterImage(CDC *pDC);
  void ActuateColors();
  void DrawAlertZones(const CRect& Bounds);

  struct AlertZone
  {
   double start;
   double end;
   COLORREF color;
  };

  std::vector<AlertZone*> m_AlertZones;

  bool m_swTitle;
  bool m_swTLPane;
  bool m_swTRPane;
  bool m_swGrid;
  bool m_swValue;
  bool m_swLabels;
  bool m_swUnit;
  bool m_swNeedle;

  COLORREF m_colorTitle;
  COLORREF m_colorTLPane;
  COLORREF m_colorTRPane;
  COLORREF m_colorNeedle;
  COLORREF m_colorGrid;
  COLORREF m_colorValue;
  COLORREF m_colorLabels;
  COLORREF m_colorBGround;

  int m_nFontScale;
  int m_nLabelsDecimals;
  int m_nValueDecimals;
  int m_nRectWidth;
  int m_nRectHeight;
  int m_nCXPix;
  int m_nCYPix;
  int m_nRadiusPix;
  int m_nLeftLimitXPix;
  int m_nLeftLimitYPix;
  int m_nRightLimitXPix;
  int m_nRightLimitYPix;
  int m_nHalfBaseWidth;
  int m_nTextBaseSpacing;
  int m_nFontHeight;
  int m_nGridLineWidth;
  int m_nTickNumber;

  double m_dPI;
  double m_dLimitAngleDeg;
  double m_dLimitAngleRad;
  double m_dRadiansPerValue;
  double m_dNeedlePos;
  double m_dNeedlePos_n;
  double m_dMinScale;
  double m_dMaxScale;

  CString m_strTitle;
  CString m_strUnit;
  CString m_strTLPane;
  CString m_strTLPane_n;
  CString m_strTRPane;
  CString m_strTRPane_n;

  CDC     m_dcGrid;
  CBitmap m_bitmapGrid;
  CBitmap *m_pbitmapOldGrid;

  CDC     m_dcNeedle ;
  CBitmap m_bitmapNeedle;
  CBitmap *m_pbitmapOldNeedle;

  CRect m_rectDraw;
  CRect m_rectGfx;
  CRect m_rectOwner;
  CRect m_rectMinValue;
  CRect m_rectMaxValue;
  CRect m_rectValue;

  CFont m_fontValue;
  CString m_fontType;

  CPen    m_PenG_Grid;
  CBrush  m_BrushG_Grid;
  CPen    m_PenG_BGround;
  CBrush  m_BrushG_BGround;
  CPen    m_PenN_Needle;
  CBrush  m_BrushN_Needle;
  CPen    m_PenN_BGround;
  CBrush  m_BrushN_BGround;
};

/////////////////////////////////////////////////////////////////////////////
