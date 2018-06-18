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

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg dialog

class AFX_EXT_CLASS CMIDeskDlg : public CModelessDialog, public IMIView
{
  typedef CModelessDialog Super;

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

  //изменение размеров окна
  void Resize(const CRect& i_rect);

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

  void SetIndicatorsCfg(int IndRows, int IndGas_v, int IndCarb, int IndIdleValve, int IndPowerValve, int IndStBlock, int IndAE, int IndCoolingFan, int IndCE);

  void SetMetersCfg(int MetRows, int MetRPM, int MetMAP, int MetVBat, int MetIgnTim, int MetCLT, int MetAddI1, int MetAddI2,
                    int InjPW, int MetIAT, int MetEGOCorr, int MetTPS, int MetAirFlow, int MetVehicleSpeed, int MetTPSDot, int MetMAP2,
                    int MetMapD, int MetTmp2, int MetFuelConsum, int MetKnockRetard, int MetKnockGraph, int MetSensAFR);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog(); //activate
  afx_msg void OnDestroy();    //deactivate
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

  void OnUpdateTimer(void);

  // Implementation
 private:
  CRect _GetMetItemRect(int idx);
  void _MetCleanUp(void);
  typedef std::map<int, MeasInstrBase*> MetFields_t;
  MetFields_t m_metFields;
  int m_metRows;
  
  typedef std::map<int, std::pair<_TSTRING, unsigned char*> > IndFields_t;
  IndFields_t m_indFields;
  CMultiLEDCtrl m_leds;
  CFont m_font;

  SECU3IO::SensorDat m_values;
  float m_air_flow; //todo: replace by direct value fron SensorDat
  float m_tps_dot;  //todo: replace by direct value fron SensorDat
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
};

/////////////////////////////////////////////////////////////////////////////
