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
  virtual void SetValues(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  virtual void Reset(void);
  //-----------------------------------------------

  //установка периода обновления в миллисекундах
  void SetUpdatePeriod(unsigned int i_period);

  //Set maximum value of tachometer displayed on the fixture
  void SetTachometerMax(int i_max);

  //Set maximum value of pressure meter displayed on the fixture
  void SetPressureMax(int i_max);

  //Set maximum value of temperature displayed on the fixture
  void SetTemperatureMax(int i_max);

  //Set maximum value of inj. PW displayed on the fixture
  void SetInjPWMax(int i_max);

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
  void SetKnockAverageNum(int avnum);
  void SetIgnTimAverageNum(int avnum);

  void SetCLTAverageNum(int avnum);
  void SetAddI2AverageNum(int avnum);  
  void SetInjPWAverageNum(int avnum);
  void SetIATAverageNum(int avnum);
  void SetEGOCorrAverageNum(int avnum);
  void SetAirFlowAverageNum(int avnum);
  void SetVehicleSpeedAverageNum(int avnum);
  void SetTPSDotAverageNum(int avnum);
  void SetMAP2AverageNum(int avnum);
  void SetMAPDAverageNum(int avnum);
  void SetTmp2AverageNum(int avnum);
  void SetFuelConsumAverageNum(int avnum);
  void SetFuelConsumFAverageNum(int avnum);
  void SetKnockRetardAverageNum(int avnum);
  void SetSensAFRAverageNum(int avnum);
  void SetChokePosAverageNum(int avnum);
  void SetGDPosAverageNum(int avnum);
  void SetSynLoadAverageNum(int avnum);
  void SetInjTimBAverageNum(int avnum);
  void SetInjTimEAverageNum(int avnum);
  void SetGrtsAverageNum(int avnum);
  void SetFtlsAverageNum(int avnum);
  void SetEgtsAverageNum(int avnum);
  void SetOpsAverageNum(int avnum);
  void SetMAFAverageNum(int avnum);

  typedef std::pair<int, COLORREF> IndCfg_t;

  void SetIndicatorsCfg(float IndHeingtPercent, int IndRows, IndCfg_t IndGas_v, IndCfg_t IndCarb, IndCfg_t IndIdleValve, IndCfg_t IndPowerValve, IndCfg_t IndStBlock, IndCfg_t IndAE,
                        IndCfg_t IndCoolingFan, IndCfg_t IndCE, IndCfg_t IndFCRevLim, IndCfg_t IndFloodClear, IndCfg_t IndSysLocked, IndCfg_t IndIgn_i, IndCfg_t IndCond_i, IndCfg_t IndEpas_i, IndCfg_t IndAftStrEnr, IndCfg_t IndIacClLoop);

  void GetIndicatorsCfg(float &IndHeingtPercent, int &IndRows, IndCfg_t &IndGas_v, IndCfg_t &IndCarb, IndCfg_t &IndIdleValve, IndCfg_t &IndPowerValve, IndCfg_t &IndStBlock, IndCfg_t &IndAE,
                        IndCfg_t &IndCoolingFan, IndCfg_t &IndCE, IndCfg_t &IndFCRevLim, IndCfg_t &IndFloodClear, IndCfg_t &IndSysLocked, IndCfg_t &IndIgn_i, IndCfg_t &IndCond_i, IndCfg_t &IndEpas_i, IndCfg_t &IndAftStrEnr, IndCfg_t &IndIacClLoop);

  void SetMetersCfg(int MetRows, int *MetRPM, int *MetMAP, int *MetVBat, int *MetIgnTim, int *MetCLT, int *MetAddI1, int *MetAddI2,
                    int *InjPW, int *MetIAT, int *MetEGOCorr, int *MetTPS, int *MetAirFlow, int *MetVehicleSpeed, int *MetTPSDot, int *MetMAP2,
                    int *MetMapD, int *MetTmp2, int *MetFuelConsum, int *MetKnockRetard, int *MetKnockGraph, int *MetSensAFR, int *MetChokePos,
                    int *MetGDPos, int *MetSynLoad, int *MetInjTimB, int *MetInjTimE, int *MetFuelConsumF, int *MetGrts, int *MetFtls, int *MetEgts,
                    int *MetOps, int *MetInjDuty, int *MetMAF, int TitleFontSize, int ValueFontSize, int PaneFontSize, int LabelFontSize);

  void GetMetersCfg(int &MetRows, int *MetRPM, int *MetMAP, int *MetVBat, int *MetIgnTim, int *MetCLT, int *MetAddI1, int *MetAddI2,
                    int *InjPW, int *MetIAT, int *MetEGOCorr, int *MetTPS, int *MetAirFlow, int *MetVehicleSpeed, int *MetTPSDot, int *MetMAP2,
                    int *MetMapD, int *MetTmp2, int *MetFuelConsum, int *MetKnockRetard, int *MetKnockGraph, int *MetSensAFR, int *MetChokePos,
                    int *MetGDPos, int *MetSynLoad, int *MetInjTimB, int *MetInjTimE, int *MetFuelConsumF, int *MetGrts, int *MetFtls, int *MetEgts,
                    int *MetOps, int *MetInjDuty, int *MetMAF, int &TitleFontSize, int &ValueFontSize, int &PaneFontSize, int &LabelFontSize);

  void SetMetersDragNDrop(bool enable);
  bool GetMetersDragNDrop(void) const;

  void SetIndicatorsDragNDrop(bool enable);
  bool GetIndicatorsDragNDrop(void) const;

  void setOnMISettingsChanged(EventHandler i_Function);

  void SetITMode(int mode);

  void ShowGraphCursor(bool show);

  int GetGraphSamplesNum(void);

  void SetGraphShtPixels(int n);

  void SetGraphShowValue(bool show);

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
  MeasInstrBase* _MetFactory(UINT uiID);
  void _IndFactory(UINT uiID);
  float _HeightCoeffFromID(UINT nID);
  void _CreateLEDs(void);
  CRect _GetGaugesRect(bool meters, bool screenCoord = false);

  struct IndFieldData
  {
   IndFieldData(const _TSTRING& cap, COLORREF col, bool* val, UINT uiID) : caption(cap), p_value(val), color(col), m_uiID(uiID) {};
   _TSTRING caption;
   bool *p_value;
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
  CObjectTimer<CMIDeskDlg> m_update_timer;
  unsigned int m_update_period;
  bool m_was_initialized;
  int m_enabled;

  bool m_showSpeedAndDistance;
  bool m_showChokePos;
  bool m_showGDPos;
  _TSTRING m_speedUnit;
  _TSTRING m_distanceUnit;
  int m_tachoMax;
  int m_pressMax;
  int m_tempMax;
  int m_injpwMax;
  MeasInstrBase* mp_miTemperat;

  COLORREF m_COLOR_BTNFACE; //for detecting of system colors changing

  HCURSOR m_hMoveCursor;
  HCURSOR m_defCursor;
  EventHandler m_OnMISettingsChanged;

  int m_it_mode;
  bool m_show_graph_cursor;
  int m_graphShtPixels;
  bool m_graphShowValue;

  RingBuffItem m_ringRPM;
  RingBuffItem m_ringKnock;
  RingBuffItem m_ringVBat;
  RingBuffItem m_ringMAP;
  RingBuffItem m_ringAddI1;
  RingBuffItem m_ringTPS;
  RingBuffItem m_ringIgnTim;
  RingBuffItem m_ringCLT;
  RingBuffItem m_ringAddI2;
  RingBuffItem m_ringInjPW;
  RingBuffItem m_ringIAT;
  RingBuffItem m_ringEGOCorr;
  RingBuffItem m_ringAirFlow;
  RingBuffItem m_ringVehicleSpeed;
  RingBuffItem m_ringTPSDot;
  RingBuffItem m_ringMAP2;
  RingBuffItem m_ringMAPD;
  RingBuffItem m_ringTmp2;
  RingBuffItem m_ringFuelConsum;
  RingBuffItem m_ringFuelConsumH;
  RingBuffItem m_ringKnockRetard;
  RingBuffItem m_ringSensAFR;
  RingBuffItem m_ringChokePos;
  RingBuffItem m_ringGDPos;
  RingBuffItem m_ringSynLoad;
  RingBuffItem m_ringInjTimB;
  RingBuffItem m_ringInjTimE;
  RingBuffItem m_ringFuelConsumF;
  RingBuffItem m_ringGrts;
  RingBuffItem m_ringFtls;
  RingBuffItem m_ringEgts;
  RingBuffItem m_ringOps;
  RingBuffItem m_ringMAF;

  GraphVal_t m_rpmQVal[2];
  GraphVal_t m_knockQVal[2];
  GraphVal_t m_vbatQVal[2];
  GraphVal_t m_mapQVal[2];
  GraphVal_t m_ai1QVal[2];
  GraphVal_t m_tpsQVal[2];
  GraphVal_t m_igntimQVal[2];
  GraphVal_t m_cltQVal[2];
  GraphVal_t m_ai2QVal[2];
  GraphVal_t m_injpwQVal[2];
  GraphVal_t m_iatQVal[2];
  GraphVal_t m_egocQVal[2];
  GraphVal_t m_airflQVal[2];
  GraphVal_t m_vssQVal[2];
  GraphVal_t m_tpsdotQVal[2];
  GraphVal_t m_map2QVal[2];
  GraphVal_t m_mapdQVal[2];
  GraphVal_t m_tmp2QVal[2];
  GraphVal_t m_fuelcQVal[2];
  GraphVal_t m_fuelchQVal[2];
  GraphVal_t m_knkretQVal[2];
  GraphVal_t m_senafrQVal[2];
  GraphVal_t m_chposQVal[2];
  GraphVal_t m_gdposQVal[2];
  GraphVal_t m_synldQVal[2];
  GraphVal_t m_itbQVal[2];
  GraphVal_t m_iteQVal[2];
  GraphVal_t m_distQVal;
  GraphVal_t m_fuelcfQVal[2];
  GraphVal_t m_grtsQVal[2];
  GraphVal_t m_ftlsQVal[2];
  GraphVal_t m_egtsQVal[2];
  GraphVal_t m_opsQVal[2];
  GraphVal_t m_injdutyQVal;
  GraphVal_t m_mafQVal[2];
};

/////////////////////////////////////////////////////////////////////////////
