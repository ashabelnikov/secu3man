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

/** \file MIDeskDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/ObjectTimer.h"
#include "IMIView.h"
#include "io-core/SECU3IO.h"
#include "MeasInstrBase.h"
#include "RingBuffer.h"

#include "ui-core/DialogWithAccelerators.h"
#include "ui-core/MultiLEDCtrl.h"
#include "common/fastdelegate.h"
#include <map>

class CMetContextMenuManager;
class CIndContextMenuManager;

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg dialog

class AFX_EXT_CLASS CMIDeskDlg : public CModelessDialog, public IMIView
{
  typedef CModelessDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CMIDeskDlg(CWnd* pParent = NULL);   // standard constructor
 ~CMIDeskDlg();
  static const UINT IDD;

  BOOL Create(UINT nIDTemplate, CWnd* pParentWnd, CRect& rect);

  //--------interface implementation---------------
  virtual void Show(bool show);
  virtual void Enable(bool enable);
  virtual void SetValues(const SECU3IO::SensorDat* i_values);
  //-----------------------------------------------

  //установка периода обновления в миллисекундах
  void SetUpdatePeriod(unsigned int i_period);

  //Set maximum value of tachometer displayed on the fixture
  void SetTachometerMax(int i_max);

  //Set maximum value of pressure meter displayed on the fixture
  void SetPressureMax(int i_max);

  void SetSpeedUnit(int i_unit); //0 - km/h, 1 - mi/h

  void SetDistanceUnit(int i_unit); //0 - km, 1 - mi

  //Show/hide choke position indicator
  void ShowChokePos(bool i_show);

  //Show/hide GD position indicator
  void ShowGDPos(bool i_show);

  //Show/hide speed and distance indicators
  void ShowSpeedAndDistance(bool i_show);

  void SetRPMAverageNum(int avnum);
  void SetVoltAverageNum(int avnum);
  void SetMAPAverageNum(int avnum);
  void SetAI1AverageNum(int avnum);
  void SetTPSAverageNum(int avnum);
  
  typedef std::pair<int, COLORREF> IndCfg_t;

  void SetIndicatorsCfg(float IndHeingtPercent, int IndRows, IndCfg_t IndGas_v, IndCfg_t IndCarb, IndCfg_t IndIdleValve, IndCfg_t IndPowerValve, IndCfg_t IndStBlock, IndCfg_t IndAE,
                        IndCfg_t IndCoolingFan, IndCfg_t IndCE, IndCfg_t IndFCRevLim, IndCfg_t IndFloodClear, IndCfg_t IndSysLocked);

  void GetIndicatorsCfg(float &IndHeingtPercent, int &IndRows, IndCfg_t &IndGas_v, IndCfg_t &IndCarb, IndCfg_t &IndIdleValve, IndCfg_t &IndPowerValve, IndCfg_t &IndStBlock, IndCfg_t &IndAE,
                        IndCfg_t &IndCoolingFan, IndCfg_t &IndCE, IndCfg_t &IndFCRevLim, IndCfg_t &IndFloodClear, IndCfg_t &IndSysLocked);

  void SetMetersCfg(int MetRows, int MetRPM, int MetMAP, int MetVBat, int MetIgnTim, int MetCLT, int MetAddI1, int MetAddI2,
                    int InjPW, int MetIAT, int MetEGOCorr, int MetTPS, int MetAirFlow, int MetVehicleSpeed, int MetTPSDot, int MetMAP2,
                    int MetMapD, int MetTmp2, int MetFuelConsum, int MetKnockRetard, int MetKnockGraph, int MetSensAFR, int MetChokePos,
                    int MetGDPos, int MetSynLoad, int TitleFontSize, int ValueFontSize, int PaneFontSize, int LabelFontSize);

  void GetMetersCfg(int &MetRows, int &MetRPM, int &MetMAP, int &MetVBat, int &MetIgnTim, int &MetCLT, int &MetAddI1, int &MetAddI2,
                    int &InjPW, int &MetIAT, int &MetEGOCorr, int &MetTPS, int &MetAirFlow, int &MetVehicleSpeed, int &MetTPSDot, int &MetMAP2,
                    int &MetMapD, int &MetTmp2, int &MetFuelConsum, int &MetKnockRetard, int &MetKnockGraph, int &MetSensAFR, int &MetChokePos,
                    int &MetGDPos, int &MetSynLoad, int &TitleFontSize, int &ValueFontSize, int &PaneFontSize, int &LabelFontSize);

  void SetMetersDragNDrop(bool enable);
  bool GetMetersDragNDrop(void) const;

  void SetIndicatorsDragNDrop(bool enable);
  bool GetIndicatorsDragNDrop(void) const;

  void setOnMISettingsChanged(EventHandler i_Function);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog(); //activate
  afx_msg void OnDestroy();    //deactivate
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT, CPoint);
  afx_msg void OnMouseMove(UINT, CPoint);
  afx_msg void OnLButtonUp(UINT, CPoint);
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnMetAddGauge(UINT nID);
  afx_msg void OnMetDeleteGauge();
  afx_msg void OnUpdateMetDelete(CCmdUI* pCmdUI);
  afx_msg void OnUpdateMetAddGauge(CCmdUI* pCmdUI);
  afx_msg void OnMetNumOfRows(UINT nID);
  afx_msg void OnUpdateMetNumOfRows(CCmdUI* pCmdUI);
  afx_msg void OnMetTitleFont(UINT nID);
  afx_msg void OnUpdateMetTitleFont(CCmdUI* pCmdUI);
  afx_msg void OnMetValueFont(UINT nID);
  afx_msg void OnUpdateMetValueFont(CCmdUI* pCmdUI);
  afx_msg void OnMetPaneFont(UINT nID);
  afx_msg void OnUpdateMetPaneFont(CCmdUI* pCmdUI);
  afx_msg void OnMetLabelFont(UINT nID);
  afx_msg void OnUpdateMetLabelFont(CCmdUI* pCmdUI);
  afx_msg void OnMetEnDragNDrop();
  afx_msg void OnUpdateMetEnDragNDrop(CCmdUI* pCmdUI);
  afx_msg void OnIndNumOfRows(UINT nID);
  afx_msg void OnUpdateIndNumOfRows(CCmdUI* pCmdUI);
  afx_msg void OnIndPercHeight(UINT nID);
  afx_msg void OnUpdateIndPercHeight(CCmdUI* pCmdUI);
  afx_msg void OnIndEnDragNDrop();
  afx_msg void OnUpdateIndEnDragNDrop(CCmdUI* pCmdUI);
  afx_msg void OnIndDelete();
  afx_msg void OnUpdateIndDelete(CCmdUI* pCmdUI);
  afx_msg void OnIndAddIndicator(UINT nID);
  afx_msg void OnUpdateIndAddIndicator(CCmdUI* pCmdUI);
  afx_msg void OnIndSetColor();
  afx_msg void OnUpdateIndSetColor(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

  void OnUpdateTimer(void);

  // Implementation
 private:
  void _Resize(void);
  CRect _GetMetItemRect(int idx);
  void _MetCleanUp(void);
  void _MetRearrangeKeys(void);
  void _IndRearrangeKeys(void);
  void _MetFactory(UINT uiID);
  void _IndFactory(UINT uiID);
  float _HeightCoeffFromID(UINT nID);
  void _CreateLEDs(void);
  CRect _GetGaugesRect(bool meters, bool screenCoord = false);

  struct IndFieldData
  {
   IndFieldData(const _TSTRING& cap, COLORREF col, unsigned char* val, UINT uiID) : caption(cap), p_value(val), color(col), m_uiID(uiID) {};
   _TSTRING caption;
   unsigned char *p_value;
   COLORREF color;
   UINT m_uiID;
  };

  typedef std::multimap<int, MeasInstrBase*> MetFields_t;
  typedef std::multimap<int, IndFieldData> IndFields_t;

  MetFields_t m_metFields;
  int m_metRows;
  bool m_draggingMet;
  MetFields_t::iterator m_dragItemMet;
  std::map<UINT, int> m_metCfg;
  bool m_metDragNDrop;
  std::auto_ptr<CMetContextMenuManager> mp_ctxMenuMgrMet;
  int m_TitleFontSize, m_ValueFontSize, m_PaneFontSize, m_LabelFontSize;
  
  IndFields_t m_indFields;
  CMultiLEDCtrl m_leds;
  CFont m_font;
  int m_indRows;
  float m_IndHeightCoeff;
  bool m_draggingInd;
  IndFields_t::iterator m_dragItemInd;
  std::map<UINT, IndCfg_t> m_indCfg;
  bool m_indDragNDrop;
  std::auto_ptr<CIndContextMenuManager> mp_ctxMenuMgrInd;

  SECU3IO::SensorDat m_values;
  float m_air_flow; //todo: replace by direct value from SensorDat
  float m_tps_dot;  //todo: replace by direct value from SensorDat
  CObjectTimer<CMIDeskDlg> m_update_timer;
  unsigned int m_update_period;
  bool m_was_initialized;
  int m_enabled;

  RingBuffItem m_ringRPM;
  RingBuffItem m_ringVBat;
  RingBuffItem m_ringMAP;
  RingBuffItem m_ringAddI1;
  RingBuffItem m_ringTPS;

  bool m_showSpeedAndDistance;
  bool m_showChokePos;
  bool m_showGDPos;
  _TSTRING m_speedUnit;
  _TSTRING m_distanceUnit;
  int m_tachoMax;
  int m_pressMax;
  MeasInstrBase* mp_miTemperat;

  COLORREF m_COLOR_BTNFACE; //for detecting of system colors changing

  HCURSOR m_hMoveCursor;
  HCURSOR m_defCursor;
  EventHandler m_OnMISettingsChanged;
};

/////////////////////////////////////////////////////////////////////////////
