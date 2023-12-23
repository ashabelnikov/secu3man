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

/** \file AutoTuneController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include <algorithm>
#include <limits>
#include "AutoTuneController.h"
#include "common/MathHelpers.h"
#include "GMEInjVEDlg.h"
#include "GridModeEditorInjDlg.h"
#include "io-core/MapIds.h"
#include "io-core/SECU3IO.h"
#include "ui-core/MsgBox.h"

#define LDQUEUE_SIZE 500 //approx. at least 5 seconds of records
#define LDQUEUE_SIZE_MIN 20
#define MAX_AFR_HITS 255
#define VE_UPDATE_PERIOD 500

#undef max
#undef min

#pragma warning(disable : 4355)  

float blendAfr(const SECU3IO::SensorDat& dv)
{
 if (dv.afr!=.0f && dv.afr2!=.0f)
  return (dv.afr + dv.afr2) / 2.0f;
 else if (dv.afr2!=.0f)
  return dv.afr2;
 else
  return dv.afr;
}

CAutoTuneController::CAutoTuneController()
: mp_ve(NULL)
, mp_ve2(NULL)
, mp_afr(NULL)
, mp_rpmGrid(NULL)
, mp_lodGrid1(NULL)
, mp_lodGrid2(NULL)
, mp_lodGridx(NULL)
, m_active(false)
, m_afrerr(0.1f)
, mp_view(NULL)
, m_fifoReady(false)
, m_maxLamDel(5000.0f) //5 sec.
, m_timer(this, &CAutoTuneController::OnTimer)
, m_statSize(PTS_PER_NODE)
, m_autoBlockThrd(0)
, m_growingMode(false)
, m_minAFR(8.0)
, m_maxAFR(22.0)
, m_MinDistThrd(10)
, m_minTPS(0.0)
, m_maxTPS(100.0)
, m_cltThrd(70.0)
, m_ve2mf(0) //use 1st VE map (neither mul or add)
, m_active_ve(0) //1st VE map is active by default
, m_tunSoft(1.0f)
{
 mp_lodGridx = &m_work_map_load_slots[0]; //select default load grid for VE1

 for (int l = 0; l < VEMAP_LOAD_SIZE; ++l)
 {
  for (int r = 0; r < VEMAP_RPM_SIZE; ++r)
  {
   m_avdists[l][r] = std::numeric_limits<float>::max();
   m_afrhits[l][r] = 0;
   m_lastchg[l][r] = 0;
   m_scatter[l][r].reserve(32);
  }
 }
}

CAutoTuneController::~CAutoTuneController()
{
 //empty
}

size_t CAutoTuneController::_CalcFIFOSize(void)
{
 if (m_logdata.size() < LDQUEUE_SIZE_MIN)
  return LDQUEUE_SIZE;
 int dtm = (m_logdata[0].time - m_logdata[LDQUEUE_SIZE_MIN-1].time);
 if (dtm==0)
 {
  ASSERT(0); //internal error
  return 0;
 }
 return MathHelpers::Round((m_maxLamDel * LDQUEUE_SIZE_MIN) / dtm);
}

void CAutoTuneController::SetDynamicValues(const SECU3IO::SensorDat& dv)
{
 if (!m_active)
  return; //not in active state or FIFO is not initialized

 //update load axis grid if necessry
 if (m_ldaxNeedsUpdate || ldaxWatchBaroPress(dv.baro_press))
 {
  if (ldaxIsTable())
  {
   mp_lodGridx = mp_lodGrid1;   //use for VE1 load grid map set by BindLoadGrid()
  }
  else
  {
   m_work_map_load_slots = MathHelpers::BuildGridFromRange(ldaxGetMin(), ldaxGetBaroMax(), VEMAP_LOAD_SIZE);
   mp_lodGridx = &m_work_map_load_slots[0];//use for VE1 dynamically generated load grid map
  }
  m_ldaxNeedsUpdate = false;
 }

 //Add new item into the FIFO
 LogDataEntry lde, e;
 lde.rpm = (float)dv.rpm;
 lde.load = dv.load;
 lde.afr = blendAfr(dv); //because we have single VE map, but two oxygen sensors
 lde.ae = dv.acceleration; //acceleration/deceleration
 lde.ie = dv.ephh_valve;  //fuel cut
 lde.tps = dv.tps;
 lde.clt = dv.temperat; //coolant temperature
 lde.time = GetTickCount(); //time when data entry arrived

 m_logdata.push_front(lde);
 if (m_logdata.size() < LDQUEUE_SIZE_MIN)
  return;

 size_t fifoSize = _CalcFIFOSize();
 if (0==fifoSize)
  return; //unexpected situation
 size_t fifoSize50 = (size_t)(fifoSize*1.5f); //+50%
 while (m_logdata.size() > fifoSize50)
  m_logdata.pop_back();

 bool update = false;
 if (m_logdata.size() >= fifoSize)
 {
  update = !m_fifoReady;
  m_fifoReady = true;
 }
 else
 {
  update = m_fifoReady;
  m_fifoReady = false;
  mp_view->SetStatusText(MLL::GetString(IDS_GME_INJ_STATUS_INIFIFO));
  return; //FIFO is not ready
 }

 if (update && mp_view->GetSafeHwnd())
  mp_view->UpdateDialogControls(mp_view, true);

 //find actual work point (rpm, load). We look into the past because lambda sensor has some delay, which depends on rpm and load.
 //Result will be in the e variable
 size_t i, s, ss;
 for(i = 0, s = 0; i < m_logdata.size(); ++i)
 {
  e = m_logdata[i];
  int delay = (int)MathHelpers::BilinearInterpolation<LAMDEL_RPM_SIZE, LAMDEL_LOAD_SIZE>(e.rpm, e.load, m_lamDelay, m_lamDelayRPMBins, m_lamDelayLoadBins);
  int timeinpast = lde.time - e.time; //how deep we currently in the past
  s+= delay;
  ss = s / (i + 1);
  if (timeinpast >= (int)ss)
  {
   s = ss;
   break;
  }
 }

 //calculate correction factor using actual and target AFRs
 float target_afr = MathHelpers::BilinearInterpolation<VEMAP_RPM_SIZE, VEMAP_LOAD_SIZE>(e.rpm, e.load, *(F3DM_t*)mp_afr, mp_rpmGrid, _GetLoadGrid(0));
 if (target_afr < 0.0001f)
 {
  SECUMessageBox(_T("Internal program error: division by zero. CAutoTuneController::SetDynamicValues"));
  return; //error: division by zero
 }
 float corrFactor = lde.afr / target_afr;

 if (lde.afr < 0.0001f)
 {
  mp_view->SetStatusText(MLL::GetString(IDS_GME_INJ_STATUS_EGOCFG));
  return; //system has no information about AFR (e.g. sensor not enabled etc)
 }
 else if (lde.clt < m_cltThrd)
 {
  mp_view->SetStatusText(MLL::GetString(IDS_GME_INJ_STATUS_CLTTH));
  return; //engine is not warmed up
 }
 else
  mp_view->SetStatusText(_T(""));

 float e_load = (0==m_active_ve) ? e.load : e.tps;

 //Add point to our map of scattered points
 int r_idx = _FindNearestGridPoint(e.rpm, mp_rpmGrid, VEMAP_RPM_SIZE);
 int l_idx = _FindNearestGridPoint(e_load, _GetLoadGrid(), VEMAP_LOAD_SIZE);

 //if growing mode enabled, state depends on relationship of current and previous RPMs
 bool growing = (!m_growingMode || (((i+1) < m_logdata.size()) && e.rpm > m_logdata[i+1].rpm));

 //skip if AFR is outside set range
 bool afr_in_range = (lde.afr >= m_minAFR) && (lde.afr <= m_maxAFR);

 bool tps_in_range = (lde.tps >= m_minTPS) && (lde.tps <= m_maxTPS);

 if (m_lastchg[l_idx][r_idx] != 0xFFFFFFFF && (GetTickCount() - m_lastchg[l_idx][r_idx]) > ss && growing && !e.ae && e.ie && afr_in_range && tps_in_range && !m_blocked[l_idx][r_idx])
 {
  ScatterItem_t& node = m_scatter[l_idx][r_idx];
  if (node.size() < (size_t)m_statSize)
   node.push_back(NodePoint(corrFactor, e.rpm, e_load));
  else
  {
   for(size_t i = 0; i < node.size(); ++i)
   {
    float d1 = MathHelpers::Distance(e.rpm, e_load, mp_rpmGrid[r_idx], _GetLoadGrid()[l_idx]); //distance to new point
    float d2 = MathHelpers::Distance(node[i].rpm, node[i].load, mp_rpmGrid[r_idx], _GetLoadGrid()[l_idx]); //distance to the current point
    if (d1 < d2)
    { //replace existing point if distance to new point is less
     node[i] = NodePoint(corrFactor, e.rpm, e_load);
     break;
    }
   }
  }
 }

 //update node's statistics which displayed by view
 if (fabs(lde.afr - target_afr) <= m_afrerr)
 {
  m_afrhits[l_idx][r_idx]++;
  if (m_afrhits[l_idx][r_idx] > MAX_AFR_HITS)
   m_afrhits[l_idx][r_idx] = MAX_AFR_HITS;
  std::vector<std::pair<int, int> > sel;
  sel.push_back(std::make_pair(l_idx, r_idx)); //update single cell
  mp_view->UpdateCelWgtMapCell(&sel); //Possible bug: will gradient be updated?
 }
}

bool CAutoTuneController::_ApplyCorrection(void)
{
 bool any_change = false;
 for(size_t l = 0; l < VEMAP_LOAD_SIZE; ++l)
 {
  for(size_t r = 0; r < VEMAP_RPM_SIZE; ++r)
  {
   ScatterItem_t& node = m_scatter[l][r];
   if (node.size() == m_statSize && false==m_blocked[l][r])
   { //apply correction to corresponding cell in the VE map and reset points accumulated for node
    float avdist = .0f;
    float corr = _ShepardInterpolation(mp_rpmGrid[r], _GetLoadGrid()[l], node, 0.3, 0.01, avdist);
    if (avdist < m_avdists[l][r] || (m_afrhits[l][r] < m_MinDistThrd))
    {
     m_avdists[l][r] = avdist;

     //apply softness factor to correction
     corr = 1.0f + ((corr - 1.0f) / m_tunSoft);

     //Apply correction depending on the selected function
     if (0==m_ve2mf || 1==m_ve2mf) //VE1 or VE1*VE2
     {
      float& ve = _GetVEItem(l, r);
      ve = ve * corr;  //change VE cell
      //do not let value to reach zero, because after that result of any further correction will always be zero...
      ve = MathHelpers::RestrictValue<float>(ve, 0.01f, 1.99f);
     }
     else if (2==m_ve2mf)  //VE1+VE2
     {
      float& ve1 = _GetVEItem(l, r, 0);
      float& ve2 = _GetVEItem(l, r, 1);
      float ve = ve1 + ve2;      
      if (0==m_active_ve)
      { //tunung 1st map
       ve1 = (ve * corr) - ve2;  //change VE cell of 1st map
       ve1 = MathHelpers::RestrictValue<float>(ve1, 0.01f, 1.99f);
      }
      else
      { //tuning 2nd map
       ve2 = (ve * corr) - ve1;  //change VE cell of 2nd map
       ve2 = MathHelpers::RestrictValue<float>(ve2, 0.00f, 1.99f);
      }
     }
     m_lastchg[l][r] = 0xFFFFFFFF;
     node.clear();
     any_change = true;
    }
   }
   if (true==m_blocked[l][r])
    m_scatter[l][r].clear();
  }
 }
 return any_change;
}

void CAutoTuneController::OnTimer(void)
{
 bool update_view = false;

 //Apply accumulated statistics to the current VE cell
 if (_ApplyCorrection())
 {
  //Update views and send data to SECU. This call plocks execution thread for relatively long time
  if (m_OnMapChanged)
   m_OnMapChanged(_GetActiveVEMapId());

  //assign time to associated changed VE cells
  for(size_t l = 0; l < VEMAP_LOAD_SIZE; ++l)
   for(size_t r = 0; r < VEMAP_RPM_SIZE; ++r)
    if (m_lastchg[l][r] == 0xFFFFFFFF)
     m_lastchg[l][r] = GetTickCount();
  
  update_view = true;
 }

 //automaticvally block cells
 if (0 != m_autoBlockThrd)
 {
  for(size_t l = 0; l < VEMAP_LOAD_SIZE; ++l)
  {
   for(size_t r = 0; r < VEMAP_RPM_SIZE; ++r)
   {
    if (m_afrhits[l][r] > m_autoBlockThrd)
    {
     if (!m_blocked[l][r])
      update_view = true;
     m_blocked[l][r] = true;
     m_scatter[l][r].clear(); //reset statistics
    }
   }
  }
 }

 if (update_view && mp_view->GetSafeHwnd())
  mp_view->UpdateView(); //Update our view
}

bool CAutoTuneController::isFIFOReady(void)
{
 return m_fifoReady;
}

void CAutoTuneController::Activate(void)
{
 /*m_active = true;*/
 m_fifoReady = false;
 OnChangeLamDel(); //init m_maxLamDel variable
}

void CAutoTuneController::Deactivate(void)
{
 m_active = false;
 m_logdata.clear();
 m_fifoReady = false;
 m_timer.KillTimer();
}

void CAutoTuneController::ResetStat(void)
{
 if (mp_view->GetRstAllCheck())
 { //all
  for (int l = 0; l < VEMAP_LOAD_SIZE; ++l)
  {
   for (int r = 0; r < VEMAP_RPM_SIZE; ++r)
   {
    m_scatter[l][r].clear();
    m_avdists[l][r] = std::numeric_limits<float>::max();
    m_afrhits[l][r] = 0;
    m_lastchg[l][r] = 0; //any cell allowed for changes
   }
  }
  mp_view->UpdateCelWgtMapCell(); //update all cells
 }
 else
 {
  const std::vector<std::pair<int, int> >& sel = mp_view->GetVESelection();
  for(size_t i = 0; i < sel.size(); ++i)
  {
   int l = sel[i].first, r = sel[i].second;
   m_scatter[l][r].clear();
   m_avdists[l][r] = std::numeric_limits<float>::max();
   m_afrhits[l][r] = 0;
   m_lastchg[l][r] = 0; //any cell allowed for changes
  }
  mp_view->UpdateCelWgtMapCell(&sel); //update selected cells. Possible bug: will gradient be updated?
 }
}

void CAutoTuneController::StartStop(void)
{
 if (m_active)
 {
  m_active = false;
  mp_view->SetStatusText(MLL::GetString(IDS_GME_INJ_STATUS_STOP));
  m_logdata.clear();
  m_fifoReady = false;
  mp_view->UpdateDialogControls(mp_view, true);
  mp_view->SetStrStpCaption(MLL::GetString(IDS_GME_INJ_BUTTON_START));
  m_timer.KillTimer();
 }
 else
 {
  m_active = true;
  mp_view->SetStatusText(MLL::GetString(IDS_GME_INJ_STATUS_INIFIFO));
  mp_view->SetStrStpCaption(MLL::GetString(IDS_GME_INJ_BUTTON_STOP));
  mp_view->UpdateDialogControls(mp_view, true);
  m_timer.SetTimer(VE_UPDATE_PERIOD);
 }
}

void CAutoTuneController::Smoothing(void)
{
 if (mp_view->GetFinishCheck())
 {
  //build list of available points
  std::vector<NodePoint> nd;
  for (int l = 0; l < VEMAP_LOAD_SIZE; ++l)
  {
   for (int r = 0; r < VEMAP_RPM_SIZE; ++r)
   {
    if (m_afrhits[l][r] >= m_MinDistThrd)
     nd.push_back(NodePoint(_GetVEItem(l, r), (float)r, (float)l));
   }
  }
  if (nd.size())
  {
   //IDW interpolation based on existing points
   for (int l = 0; l < VEMAP_LOAD_SIZE; ++l)
   {
    for (int r = 0; r < VEMAP_RPM_SIZE; ++r)
    {
     float avd = 0;
     _GetVEItem(l, r) = _ShepardInterpolation((float)r, (float)l, nd, 2.0, 0.01, avd);
    }
   }
  }
 }

 float orig[VEMAP_LOAD_SIZE*VEMAP_RPM_SIZE] = {0};
 float modif[VEMAP_LOAD_SIZE*VEMAP_RPM_SIZE] = {0};
 std::copy(_GetVEMap(), _GetVEMap() + (VEMAP_LOAD_SIZE * VEMAP_RPM_SIZE), orig);

 bool result = MathHelpers::SigmaFilter2D(orig, modif, VEMAP_LOAD_SIZE, VEMAP_RPM_SIZE, 3, true); //use median filter

 if (!result)
  return;

 std::copy(modif, modif + (VEMAP_LOAD_SIZE * VEMAP_RPM_SIZE), _GetVEMap());

 if (m_OnMapChanged) //Update views and send data to SECU. This call blocks execution thread for relatively long time
  m_OnMapChanged(_GetActiveVEMapId());

 //assign time to associated changed VE cells
 for(size_t l = 0; l < VEMAP_LOAD_SIZE; ++l)
  for(size_t r = 0; r < VEMAP_RPM_SIZE; ++r)
   if (!MathHelpers::IsEqualFlt(_GetVEItem(l, r), orig[(l*VEMAP_RPM_SIZE)+r], 0.0001f)) // !=
    m_lastchg[l][r] = GetTickCount();

 mp_view->UpdateView(); //Update our view
}

int CAutoTuneController::_FindNearestGridPoint(float arg, const float *grid, int gSize)
{
 int i, i1;
 MathHelpers::AxisCellLookup(arg, grid, gSize, i, i1); //find two nearest points

 //select one nearest point
 if (arg - grid[i] < (grid[i1] - arg))
  return i;
 else
  return i1;
}

float CAutoTuneController::_ShepardInterpolation(float rpm, float load, const ScatterItem_t& points, double power, double eps /*= 0.01*/, float& o_avdist)
{
 std::vector<double> w(points.size(), 0);
 if (0==points.size())
 {
  o_avdist = 0;
  return 0; //prevent dision by zero
 }
 
 double ws = 0, avd = 0;
 for(size_t i = 0; i < points.size(); ++i)
 {
  double d = MathHelpers::Distance((double)rpm, (double)load, (double)points[i].rpm, (double)points[i].load);
  if (d < eps)
   d = eps;
  avd+=d;
  w[i] = 1.0 / pow(d, power);  //1 / (d) ^ p
  ws+= w[i];
 }

 o_avdist = (float)(avd / points.size()); //average

 double f = 0;
 for(size_t i = 0; i < points.size(); ++i)
  f+= (points[i].f * w[i]);

 if (ws==.0)
  SECUMessageBox(_T("Internal program error: division by zero. CAutoTuneController::_ShepardInterpolation"));
 return (float)(f / ws);
}

float& CAutoTuneController::_GetVEItem(int i, int j, int vemap /*=-1*/)
{
 ASSERT(mp_ve);
 if (vemap==-1)
  vemap = m_active_ve;
 if (0==vemap) //1st
  return mp_ve[(i*VEMAP_RPM_SIZE)+j];
 else          //2nd
  return mp_ve2[(i*VEMAP_RPM_SIZE)+j];
}

float* CAutoTuneController::_GetVEMap(int vemap /*=-1*/)
{
 ASSERT(mp_ve);
 ASSERT(mp_ve2);
 if (vemap==-1)
  vemap = m_active_ve;
 if (0==vemap) //1st
  return mp_ve;
 else          //2nd
  return mp_ve2;
}

float* CAutoTuneController::GetLamDelMap(int id)
{
 if (id == 0)
  return &m_lamDelay[0][0];
 if (id == 1)
  return m_lamDelayRPMBins;
 if (id == 2)
  return m_lamDelayLoadBins;
 ASSERT(0);
 return NULL; //big bang!
}

int CAutoTuneController::_GetActiveVEMapId(void)
{
 return (0==m_active_ve) ? ETMT_INJ_VE : ETMT_INJ_VE2;
}

const float* CAutoTuneController::_GetLoadGrid(int grid /* = -1*/)
{
 if (grid == -1)
  grid = m_active_ve;
 return (0==grid) ? mp_lodGridx : mp_lodGrid2; //select between VE1 and VE2 load grids
}

void CAutoTuneController::Init(void)
{
 mp_view->BindLamDelMap(&m_lamDelay[0][0], m_lamDelayRPMBins, m_lamDelayLoadBins);
 mp_view->BindCelWgtMap(&m_afrhits[0][0]);
 mp_view->BindCelBlkMap(&m_blocked[0][0]);
 mp_view->setOnRstStt(fastdelegate::MakeDelegate(this, CAutoTuneController::ResetStat));
 mp_view->setOnStrStp(fastdelegate::MakeDelegate(this, CAutoTuneController::StartStop));
 mp_view->setIsReady(fastdelegate::MakeDelegate(this, CAutoTuneController::isFIFOReady));
 mp_view->setOnViewActivate(fastdelegate::MakeDelegate(this, CAutoTuneController::OnViewActivate));
 mp_view->setOnSmooth(fastdelegate::MakeDelegate(this, CAutoTuneController::Smoothing));
 mp_view->setOnChangeLamDel(fastdelegate::MakeDelegate(this, CAutoTuneController::OnChangeLamDel));
 mp_view->setOnSelectVEMap(fastdelegate::MakeDelegate(this, CAutoTuneController::OnSelectVEMap)); 
}

void CAutoTuneController::OnChangeLamDel(void)
{
 m_maxLamDel = *std::max_element(&m_lamDelay[0][0], &m_lamDelay[0][0] + (LAMDEL_LOAD_SIZE*LAMDEL_RPM_SIZE));
}

void CAutoTuneController::OnViewActivate(void)
{
 if (m_active)
 {
  mp_view->SetStatusText(MLL::GetString(IDS_GME_INJ_STATUS_INIFIFO));
  mp_view->SetStrStpCaption(MLL::GetString(IDS_GME_INJ_BUTTON_STOP));
 }
 else
 {
  mp_view->SetStatusText(MLL::GetString(IDS_GME_INJ_STATUS_STOP));
  mp_view->SetStrStpCaption(MLL::GetString(IDS_GME_INJ_BUTTON_START));
 }
}

void CAutoTuneController::SetView(CGridModeEditorInjDlg* pView)
{
 mp_view = pView->m_pVEPageDlg.get();
}

void CAutoTuneController::SetAFRError(float afrError)
{
 m_afrerr = afrError;
}

void CAutoTuneController::setOnMapChanged(EventWithCode OnFunction)
{
 m_OnMapChanged = OnFunction;
}

void CAutoTuneController::SetLoadAxisCfg(float minVal, float maxVal, int ldaxCfg, bool useTable, bool forceUpdate /*= false*/)
{
 LdaxCfg::SetLoadAxisCfg(minVal, maxVal, ldaxCfg, useTable);
}

void CAutoTuneController::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
}

void CAutoTuneController::BindLoadGrid(const float* pGrid1, const float* pGrid2)
{
 mp_lodGrid1 = pGrid1;
 mp_lodGrid2 = pGrid2;
}

void CAutoTuneController::BindMaps(float* pVE, float* pAFR, float* pVE2)
{
 mp_ve = pVE;
 mp_afr = pAFR;
 mp_ve2 = pVE2;
}

bool* CAutoTuneController::GetBlockedCells(void)
{
 return &m_blocked[0][0];
}

void CAutoTuneController::SetStatSize(int statSize)
{
 m_statSize = statSize;
}

void CAutoTuneController::SetAutoBlockThrd(int thrd)
{
 m_autoBlockThrd = thrd;
}

void CAutoTuneController::SetGrowingMode(bool growing)
{
 m_growingMode = growing;
}

void CAutoTuneController::SetMinAFR(float afr)
{
 m_minAFR = afr;
}

void CAutoTuneController::SetMaxAFR(float afr)
{
 m_maxAFR = afr;
}

void CAutoTuneController::SetMinDistThrd(int thrd)
{
 m_MinDistThrd = thrd;
}

void CAutoTuneController::SetMinTPS(float tps)
{
 m_minTPS = tps;
}

void CAutoTuneController::SetMaxTPS(float tps)
{
 m_maxTPS = tps;
}

void CAutoTuneController::SetCLTThrd(float clt)
{
 m_cltThrd = clt;
}

void CAutoTuneController::SetVE2MapFunc(int func)
{
 m_ve2mf = func;
}

void CAutoTuneController::OnSelectVEMap(int vemap) //event from view
{
 m_active_ve = vemap;
}

void CAutoTuneController::SetTunSoftness(float soft)
{
 m_tunSoft = soft;
}
