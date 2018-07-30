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

/** \file AutoTuneController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include <deque>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"
#include "DynamicValues.h"
#include "common/ObjectTimer.h"

class CGridModeEditorInjDlg;
class CGMEInjVEDlg;

struct LogDataEntry
{
 float rpm;
 float load;
 float afr;
 DWORD time; //ms
};

struct NodePoint
{
 NodePoint(float i_f, float i_rpm, float i_load) : f(i_f), rpm(i_rpm), load(i_load) {}
 float f;
 float rpm;
 float load;
};

#define LAMDEL_LOAD_SIZE 3
#define LAMDEL_RPM_SIZE 3

#define VEMAP_LOAD_SIZE 16
#define VEMAP_RPM_SIZE 16

#define PTS_PER_NODE 16

class CAutoTuneController
{
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef std::vector<NodePoint> ScatterItem_t;
  typedef float F3DM_t[VEMAP_RPM_SIZE][VEMAP_LOAD_SIZE];

 public:
  CAutoTuneController();
 ~CAutoTuneController();

 void Init(void);

 void SetDynamicValues(const TablDesk::DynVal& dv);

 void BindMaps(float* pVE, float* pAFR);
 void BindRPMGrid(float* pGrid);
 void SetLoadAxisCfg(float minVal, float maxVal);
 void SetAFRError(float afrError);
 void SetStatSize(int statSize);
 void SetAutoBlockThrd(int thrd);
 void SetGrowingMode(bool growing);

 float* GetLamDelMap(int id);
 bool* GetBlockedCells(void);

 bool isFIFOReady(void);

 void Activate(void);
 void Deactivate(void);

 void ResetStat(void);
 void StartStop(void);

 void SetView(CGridModeEditorInjDlg* pView);

 void setOnMapChanged(EventWithCode OnFunction);

 private:
  int _FindNearestGridPoint(float arg, float *grid, int gSize);
  float _ShepardInterpolation(float rpm, float load, const ScatterItem_t& points, double power, double eps, float& o_avdist);
  bool _ApplyCorrection(void);
  float& _GetVEItem(int i, int j);
  void OnChangeLamDel(void);
  void OnViewActivate(void);
  size_t _CalcFIFOSize(void);
  void OnTimer(void);

 private:
  CObjectTimer<CAutoTuneController> m_timer;
  EventWithCode m_OnMapChanged;
  std::deque<LogDataEntry> m_logdata;
  float m_lamDelay[LAMDEL_LOAD_SIZE][LAMDEL_RPM_SIZE];
  float m_lamDelayLoadBins[LAMDEL_LOAD_SIZE];
  float m_lamDelayRPMBins[LAMDEL_RPM_SIZE];

  float* mp_ve;
  float* mp_afr;

  //contains statistics (scattered points)
  ScatterItem_t m_scatter[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE];
  float m_avdists[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE];
  float m_afrhits[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE];
  DWORD m_lastchg[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE]; //time of last change of related VE cell
  bool m_blocked[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE];

  bool m_active;
  float* mp_rpmGrid;

  float m_ldaxMin;
  float m_ldaxMax;
  float m_baro_press;
  bool m_ldaxNeedsUpdate;
  std::vector<float> mp_loadGrid;
  float m_afrerr;

  CGMEInjVEDlg* mp_view;
  bool m_fifoReady;
  float m_maxLamDel;
  int m_statSize;
  int m_autoBlockThrd;
  bool m_growingMode;
};
