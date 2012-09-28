/*
 *
 *  ChartSurfaceSerie.h
 *
 *  Written by Cédric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *  This code may be used for any non-commercial and commercial purposes in a compiled form.
 *  The code may be redistributed as long as it remains unmodified and providing that the
 *  author name and this disclaimer remain intact. The sources can be modified WITH the author
 *  consent only.
 *
 *  This code is provided without any garanties. I cannot be held responsible for the damage or
 *  the loss of time it causes. Use it at your own risks
 *
 *  An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */

#pragma once

#include "ChartSerie.h"

class AFX_EXT_CLASS CChartSurfaceSerie : public CChartSerie
{
 public:
  CChartSurfaceSerie(CChartCtrl* pParent);
  virtual ~CChartSurfaceSerie();

  enum FillStyle
  {
   fsSolid = 0,
   fsHatchDownDiag,
   fsHatchUpDiag,
   fsHatchCross,
   fsHatchDiagCross,
   fsHatchHorizontal,
   fsHatchVertical
  };

  void SetFillStyle(FillStyle NewStyle);
  FillStyle GetFillStyle() const { return m_FillStyle; }

  void SetHorizontal(bool bHoriz);
  bool GetHorizontal() const { return m_bHorizontal; }

  void SetSeriesOrdering(SeriesOrdering newOrdering);

 private:
  void DrawLegend(CDC* pDC, const CRect& rectBitmap) const;

  void DrawAll(CDC *pDC);
  void Draw(CDC* pDC);

  FillStyle m_FillStyle;
  // The surface serie can be either horizontal (the fill will be done between the curve
  // and the corresponding horizontal axis) or vertical (the fill will be done between
  // the curve and the corresponding vertical axis)
  bool m_bHorizontal;
};
