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
#include "common/ObjectTimer.h"
#include "LdaxCfg.h"

namespace SECU3IO { struct SensorDat; };

class CGridModeEditorInjDlg;
class CGMEInjVEDlg;

struct LogDataEntry
{
 float rpm;
 float load;
 float afr;
 DWORD time; //ms
 bool ae;
 bool ie;
 float tps;
 float clt;
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

class CAutoTuneController : public LdaxCfg
{
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef std::vector<NodePoint> ScatterItem_t;
  typedef float F3DM_t[VEMAP_RPM_SIZE][VEMAP_LOAD_SIZE];

 public:
  CAutoTuneController();
 ~CAutoTuneController();

 void Init(void);

 void SetDynamicValues(const SECU3IO::SensorDat& dv);

 void BindMaps(float* pVE, float* pAFR, float* pVE2);
 void BindRPMGrid(float* pGrid);
 void BindLoadGrid(const float* pGrid1, const float* pGrid2);
 virtual void SetLoadAxisCfg(float minVal, float maxVal, int ldaxCfg, bool useTable, bool forceUpdate = false);
 void SetVE2MapFunc(int func); // 0 - 1st map, 1 - mult, 2 - add
 void SetAFRError(float afrError);
 void SetStatSize(int statSize);
 void SetAutoBlockThrd(int thrd);
 void SetGrowingMode(bool growing);
 void SetMinAFR(float afr);
 void SetMaxAFR(float afr);
 void SetMinDistThrd(int thrd);
 void SetMinTPS(float tps);
 void SetMaxTPS(float tps);
 void SetCLTThrd(float clt);
 void SetTunSoftness(float soft);

 float* GetLamDelMap(int id);
 bool* GetBlockedCells(void);

 bool isFIFOReady(void);

 void Activate(void);
 void Deactivate(void);

 void ResetStat(void);
 void StartStop(void);
 void Smoothing(void);

 void SetView(CGridModeEditorInjDlg* pView);

 void setOnMapChanged(EventWithCode OnFunction);

 private:
  int _FindNearestGridPoint(float arg, const float *grid, int gSize);
  float _ShepardInterpolation(float rpm, float load, const ScatterItem_t& points, double power, double eps, float& o_avdist);
  bool _ApplyCorrection(void);
  float& _GetVEItem(int i, int j, int vemap = -1);
  float* _GetVEMap(int vemap = -1);
  void OnChangeLamDel(void);
  void OnViewActivate(void);
  size_t _CalcFIFOSize(void);
  void OnTimer(void);
  void OnSelectVEMap(int vemap); //0 - 1st, 1 - 2nd
  int _GetActiveVEMapId(void);
  const float* _GetLoadGrid(int grid = -1);

 private:
  CObjectTimer<CAutoTuneController> m_timer;
  EventWithCode m_OnMapChanged;
  std::deque<LogDataEntry> m_logdata;
  float m_lamDelay[LAMDEL_LOAD_SIZE][LAMDEL_RPM_SIZE];
  float m_lamDelayLoadBins[LAMDEL_LOAD_SIZE];
  float m_lamDelayRPMBins[LAMDEL_RPM_SIZE];

  float* mp_ve;
  float* mp_ve2;
  float* mp_afr;

  //contains statistics (scattered points)
  ScatterItem_t m_scatter[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE];
  float m_avdists[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE];
  float m_afrhits[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE];
  DWORD m_lastchg[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE]; //time of last change of related VE cell
  bool m_blocked[VEMAP_LOAD_SIZE][VEMAP_RPM_SIZE];

  bool m_active;
  float* mp_rpmGrid;
  const float* mp_lodGrid1; //load grid set for VE1
  const float* mp_lodGrid2; //load grid set for VE2

  const float* mp_lodGridx; //current load grid selected for VE1 (mp_lodGrid1 or m_work_map_load_slots)
  float m_afrerr;

  CGMEInjVEDlg* mp_view;
  bool m_fifoReady;
  float m_maxLamDel;
  int m_statSize;
  int m_autoBlockThrd;
  bool m_growingMode;

  float m_minAFR;
  float m_maxAFR;
  int m_MinDistThrd;

  float m_minTPS;
  float m_maxTPS;
 
  float m_cltThrd;
  int m_ve2mf;
  int m_active_ve;

  float m_tunSoft;
};
