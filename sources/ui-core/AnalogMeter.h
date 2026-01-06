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

/** \file AnalogMeter.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <algorithm>
#include <vector>

// Enum types for altering of properties
enum MeterMemberEnum
{
 meter_title,   //title (name of gauge)
 meter_tlpane,  //top-left pane
 meter_trpane,  //top-right pane
 meter_needle,  //arrow
 meter_grid,    //ticks of grid
 meter_value,   //digital value displayed at the bottom
 meter_labels,  //tick labels
 meter_unit,    //units
 meter_bground, //background
 meter_3drect,  //3D rect around a meter
};

// CAnalogMeter class
class AFX_EXT_CLASS CAnalogMeter
{
 public:
  CAnalogMeter();
  virtual ~CAnalogMeter();

 // Operations
 public:
  void ShowMeter(CDC *pDC, CRect rectBorder);
  virtual void Update(CDC *pDC);

  void SetColor(enum MeterMemberEnum meter_member, COLORREF Color);
  void SetState(enum MeterMemberEnum meter_member, bool State);
  void SetRange(double dMin, double dMax, bool updateAlertZones = false);
  void SetNeedleValue(double value);
  void SetTLPane(CString strPane);
  void SetTRPane(CString strPane);
  void SetFontScale(enum MeterMemberEnum meter_member, int nFontScale);
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

  void GetColor(enum MeterMemberEnum meter_member, COLORREF* pColor) const;
  void GetState(enum MeterMemberEnum meter_member, bool* pState) const;

  void SetTickLength(double length) { m_tickLength = 1.0 / length;}
  void SetNeedleWidth(double width) { m_needleWidth = width; }

  void SetPieRadius(double rad) { m_pieRadius = rad; };
 
 protected:
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
  bool m_sw3DRect;

  COLORREF m_colorTitle;
  COLORREF m_colorTLPane;
  COLORREF m_colorTRPane;
  COLORREF m_colorNeedle;
  COLORREF m_colorGrid;
  COLORREF m_colorValue;
  COLORREF m_colorLabels;
  COLORREF m_colorBGround;

  int m_nFontScale;
  int m_nFontScaleVal;
  int m_nFontScalePane;
  int m_nFontScaleLabel;
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
  int m_nFontHeightVal;
  int m_nFontHeightPane;
  int m_nFontHeightLabel;
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
  double m_tickLength;
  double m_needleWidth;
  double m_pieRadius;

  CString m_strTitle;
  CString m_strUnit;
  CString m_strTLPane;
  CString m_strTLPane_n;
  CString m_strTRPane;
  CString m_strTRPane_n;

  CDC     m_dcGrid;
  CBitmap m_bitmapGrid;
  CBitmap *m_pbitmapOldGrid;

  CDC     m_dcNeedle;
  CBitmap m_bitmapNeedle;
  CBitmap *m_pbitmapOldNeedle;

  CRect m_rectDraw;
  CRect m_rectGfx;
  CRect m_rectOwner;
  CRect m_rectMinValue;
  CRect m_rectMaxValue;
  CRect m_rectValue;

  CFont m_fontValue;
  CFont m_fontValueVal;
  CFont m_fontValuePane;
  CFont m_fontValueLabel;

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
