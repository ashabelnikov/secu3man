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

/** \file MIDeskDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include <algorithm>
#include <numeric>
#include <limits>
#include "MIDeskDlg.h"

#include "common/Dll.h"
#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
#include "common/DPIAware.h"
#include "common/SettingsTypes.h"
#include "ui-core/AnalogMeterCtrl.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/ColorDialogEx.h"

#include "MIAirFlow.h"
#include "MIAPPS.h"
#include "MIDwellAngle.h"
#include "MIEGOCorr.h"
#include "MIIAT.h"
#include "MIInjPW.h"
#include "MIPressure.h"
#include "MITachometer.h"
#include "MITemperature.h"
#include "MIThrottleGate.h"
#include "MIVoltage.h"
#include "MIVoltmeter.h"
#include "MIVehicleSpeed.h"
#include "MITPSDot.h"
#include "MIMAPDot.h"
#include "MIMap2.h"
#include "MIMapD.h"
#include "MIMaf.h"
#include "MITemp2.h"
#include "MIGrts.h"
#include "MIFuelConsum.h"
#include "MIFuelConsumF.h"
#include "MIKnockRetard.h"
#include "MIKnockGraph.h"
#include "MISensAFR.h"
#include "MIChokePos.h"
#include "MISynLoad.h"
#include "MIInjTim.h"
#include "MIFtls.h"
#include "MIEgts.h"
#include "MIOps.h"
#include "MIInjDuty.h"
#include "MIVentDuty.h"
#include "MIFts.h"
#include "ContextMenuManager.h"
#include "MITargAFR.h"
#include "MIDiffAFR.h"

#include "ScaleEditorDlg.h"

#undef max

BEGIN_MESSAGE_MAP(CMIDeskDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_WM_PAINT()
 ON_WM_LBUTTONDOWN()
 ON_WM_MOUSEMOVE()
 ON_WM_LBUTTONUP()
 ON_WM_SETCURSOR()
 ON_WM_CONTEXTMENU()
 ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
 ON_COMMAND(IDM_MI_MET_DEL_GAUGE, OnMetDeleteGauge)
 ON_UPDATE_COMMAND_UI(IDM_MI_MET_DEL_GAUGE, OnUpdateMetDelete)
 ON_COMMAND_RANGE(IDM_MI_MET_RPM, IDM_MI_MET_APPS, OnMetAddGauge)
 ON_COMMAND_RANGE(IDM_MI_GRH_RPM, IDM_MI_GRH_APPS, OnMetAddGauge)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_RPM, IDM_MI_MET_APPS, OnUpdateMetAddGauge)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_GRH_RPM, IDM_MI_GRH_APPS, OnUpdateMetAddGauge)
 ON_COMMAND_RANGE(IDM_MI_MET_ROWS1, IDM_MI_MET_ROWS4, OnMetNumOfRows)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_ROWS1, IDM_MI_MET_ROWS4, OnUpdateMetNumOfRows)
 ON_COMMAND_RANGE(IDM_MI_MET_TITLE_FONT050, IDM_MI_MET_TITLE_FONT150, OnMetTitleFont)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_TITLE_FONT050, IDM_MI_MET_TITLE_FONT150, OnUpdateMetTitleFont)
 ON_COMMAND_RANGE(IDM_MI_MET_VALUE_FONT050, IDM_MI_MET_VALUE_FONT150, OnMetValueFont)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_VALUE_FONT050, IDM_MI_MET_VALUE_FONT150, OnUpdateMetValueFont)
 ON_COMMAND_RANGE(IDM_MI_MET_PANE_FONT050, IDM_MI_MET_PANE_FONT150, OnMetPaneFont)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_PANE_FONT050, IDM_MI_MET_PANE_FONT150, OnUpdateMetPaneFont)
 ON_COMMAND_RANGE(IDM_MI_MET_LABEL_FONT050, IDM_MI_MET_LABEL_FONT150, OnMetLabelFont)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_LABEL_FONT050, IDM_MI_MET_LABEL_FONT150, OnUpdateMetLabelFont)
 ON_COMMAND(IDM_MI_MET_ENDRAGNDROP, OnMetEnDragNDrop)
 ON_UPDATE_COMMAND_UI(IDM_MI_MET_ENDRAGNDROP, OnUpdateMetEnDragNDrop)
 ON_COMMAND_RANGE(IDM_MI_IND_ROWS1, IDM_MI_IND_ROWS4, OnIndNumOfRows)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_IND_ROWS1, IDM_MI_IND_ROWS4, OnUpdateIndNumOfRows)
 ON_COMMAND_RANGE(IDM_MI_IND_PERCHEIGHT03, IDM_MI_IND_PERCHEIGHT15, OnIndPercHeight)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_IND_PERCHEIGHT03, IDM_MI_IND_PERCHEIGHT15, OnUpdateIndPercHeight)
 ON_COMMAND(IDM_MI_IND_ENDRAGNDROP, OnIndEnDragNDrop)
 ON_UPDATE_COMMAND_UI(IDM_MI_IND_ENDRAGNDROP, OnUpdateIndEnDragNDrop)
 ON_COMMAND(IDM_MI_IND_DEL_INDICATOR, OnIndDelete)
 ON_UPDATE_COMMAND_UI(IDM_MI_IND_DEL_INDICATOR, OnUpdateIndDelete)
 ON_COMMAND_RANGE(IDM_MI_IND_GAS_V, IDM_MI_IND_ALTRN_I, OnIndAddIndicator)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_IND_GAS_V, IDM_MI_IND_ALTRN_I, OnUpdateIndAddIndicator)
 ON_COMMAND(IDM_MI_IND_SET_COLOR, OnIndSetColor)
 ON_UPDATE_COMMAND_UI(IDM_MI_IND_SET_COLOR, OnUpdateIndSetColor)
 ON_COMMAND(IDM_MI_MET_EDITSCALE, OnMetEditScale)
END_MESSAGE_MAP()

const UINT CMIDeskDlg::IDD = IDD_MEAS_INSTRUMENT_DESK;

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg dialog

CMIDeskDlg::CMIDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CMIDeskDlg::IDD, pParent)
, m_update_period(100)
, m_was_initialized(false)
, m_enabled(-1)
, m_metRows(0)
, m_indRows(0)
, m_showSpeedAndDistance(false)
, m_showChokePos(false)
, m_showGDPos(false)
, mp_miTemperat(NULL)
, m_leds(DLL::GetModuleHandle())
, m_hMoveCursor(NULL)
, m_defCursor(NULL)
, m_draggingMet(false)
, m_draggingInd(false)
, mp_ctxMenuMgrMet(new CMetContextMenuManager())
, mp_ctxMenuMgrInd(new CIndContextMenuManager())
, m_metDragNDrop(false)
, m_indDragNDrop(false)
, m_it_mode(0)
, m_show_graph_cursor(false)
, m_graphShowValue(false)
, m_graphValueHeight(100) //100%
, m_graphShtPixels(2)
{
 mp_ctxMenuMgrMet->CreateContent();
 mp_ctxMenuMgrInd->CreateContent();

 memset(&m_values, 0, sizeof(SECU3IO::SensorDat));
 m_hMoveCursor = (HCURSOR)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND_MOVE), IMAGE_CURSOR, 0, 0, 0);
 VERIFY(m_hMoveCursor);
}

CMIDeskDlg::~CMIDeskDlg()
{
 _MetCleanUp();
 VERIFY(DestroyCursor(m_hMoveCursor));
}

void CMIDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_MI_INDICATORS, m_leds);
}

BOOL CMIDeskDlg::Create(UINT nIDTemplate, CWnd* pParentWnd, CRect& rect)
{
 BOOL result = Super::Create(nIDTemplate, pParentWnd);
 if (!result)
  return FALSE;

 SetWindowPos(NULL, rect.TopLeft().x,rect.TopLeft().y, rect.Width(), rect.Height(), SWP_NOZORDER);

 m_was_initialized = true;

 return result;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg message handlers

BOOL CMIDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();

 mp_ctxMenuMgrMet->Attach(this);
 mp_ctxMenuMgrInd->Attach(this);

 if (!m_font.GetSafeHandle())
 {
/*LOGFONT LogFont;
  memset(&LogFont, 0x00, sizeof(LogFont));
  _tcsncpy(LogFont.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE);
  DPIAware dpi;
  LogFont.lfHeight = dpi.FontHeight(8);
  m_font.CreateFontIndirect(&LogFont);*/
  CloneWndFont(this, &m_font, -1, false);  
 }

 m_leds.SetFont(&m_font);
 
 Enable(false);

 m_update_timer.SetTimer(this, &CMIDeskDlg::OnUpdateTimer, m_update_period);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CMIDeskDlg::OnDestroy()
{
 m_was_initialized = false;
 _MetCleanUp();
 Super::OnDestroy();
 m_update_timer.KillTimer();
 m_enabled = -1;
}

//разрешение/запрещение приборов
void CMIDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;
 //meters
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Enable(enable);
 //indicators
 m_leds.EnableWindow(enable);
}

void CMIDeskDlg::Show(bool show)
{
 //meters
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Show(show);
 //indicators
 m_leds.ShowWindow(show ? SW_SHOW : SW_HIDE);
}

using namespace SECU3IO;

void CMIDeskDlg::SetValues(const SensorDat* i_values, bool i_revdir /* = false*/)
{
 m_values = *i_values; //save for further use in the _IndFactory() for indicstors

 //Transform inj. tim. values:
 m_values.inj_tim_begin = MathHelpers::InjTimValueTransform(m_it_mode, m_values.inj_tim_begin, 0);
 m_values.inj_tim_end = MathHelpers::InjTimValueTransform(m_it_mode, m_values.inj_tim_end, 0);

 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Append(&m_values, i_revdir); //add values to buffers and queues
}

void CMIDeskDlg::OnUpdateTimer(void)
{
 if (!m_was_initialized)
  return;

 //meters
 MetFields_t::iterator mm;
 for(mm = m_metFields.begin(); mm != m_metFields.end(); ++mm)
  mm->second->SetValues();

 //indicators
 IndFields_t::iterator it;
 int idx = 0;
 for(it = m_indFields.begin(); it != m_indFields.end(); ++it, ++idx)
  m_leds.SetItemState(idx, *it->second.p_value, true);
}

void CMIDeskDlg::SetUpdatePeriod(unsigned int i_period)
{
 m_update_period = i_period;
 if (m_was_initialized)
 {
  m_update_timer.KillTimer();
  m_update_timer.SetTimer(this,&CMIDeskDlg::OnUpdateTimer, m_update_period);
 }
}

void CMIDeskDlg::SetSpeedUnit(ESpeedUnit i_unit)
{
 m_speedUnit = (i_unit == SU_KMH) ? MLL::GetString(IDS_MI_KM_H) : MLL::GetString(IDS_MI_MP_H);
}

void CMIDeskDlg::SetDistanceUnit(ESpeedUnit i_unit)
{
 m_distanceUnit = (i_unit == SU_KMH) ? MLL::GetString(IDS_MI_KM) : MLL::GetString(IDS_MI_MI);
}

void CMIDeskDlg::_Resize(void)
{
 bool redraw = true;
 CRect rect;
 GetClientRect(&rect);
 if (!rect.IsRectNull())
 {
  //meters
  MetFields_t::iterator it;
  for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
   it->second->Resize(_GetMetItemRect(it->first), redraw);

  //Indicators
  m_leds.MoveWindow(_GetGaugesRect(false), redraw);
  m_leds.Invalidate();

//RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
 }
}

void CMIDeskDlg::ShowChokePos(bool i_show)
{
 m_showChokePos = i_show;
 if (mp_miTemperat!=NULL)
  mp_miTemperat->ShowTRP(i_show);
}

void CMIDeskDlg::ShowGDPos(bool i_show)
{
 m_showGDPos = i_show;
 if (mp_miTemperat!=NULL)
  mp_miTemperat->ShowTLP(i_show);
}

void CMIDeskDlg::ShowSpeedAndDistance(bool i_show)
{
 m_showSpeedAndDistance = i_show;
}

void CMIDeskDlg::SetMetAverage(MetAverage& ma)
{
 m_avrNum = ma;
}

void CMIDeskDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_was_initialized)
  _Resize();
}

void CMIDeskDlg::SetIndicatorsCfg(float IndHeingtPercent, int IndRows, IndCfg_t IndGas_v, IndCfg_t IndCarb, IndCfg_t IndIdleValve, IndCfg_t IndPowerValve, IndCfg_t IndStBlock, IndCfg_t IndAE, IndCfg_t IndCoolingFan, IndCfg_t IndCE, IndCfg_t IndFCRevLim, IndCfg_t IndFloodClear, IndCfg_t IndSysLocked, IndCfg_t IndIgn_i, IndCfg_t IndCond_i, IndCfg_t IndEpas_i, IndCfg_t IndAftStrEnr, IndCfg_t IndIacClLoop, IndCfg_t UniOut1, IndCfg_t UniOut2, IndCfg_t UniOut3, IndCfg_t UniOut4, IndCfg_t UniOut5, IndCfg_t UniOut6, IndCfg_t IndGpa4_i, IndCfg_t IndInput1, IndCfg_t IndInput2, IndCfg_t IndAuto_i, IndCfg_t IndMapsel0, IndCfg_t IndRefprs_i, IndCfg_t IndAltrn_i)
{
 m_indCfg.clear();
 m_indCfg.insert(std::make_pair(IDM_MI_IND_GAS_V, IndGas_v));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_CARB, IndCarb));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_IDLEVALVE, IndIdleValve));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_POWERVALVE, IndPowerValve));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_STBLOCK, IndStBlock));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_AE, IndAE));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_COOLINGFAN, IndCoolingFan));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_CE, IndCE));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_FCREVLIM, IndFCRevLim));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_FLOODCLEAR, IndFloodClear));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_SYSLOCKED, IndSysLocked));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_IGN_I, IndIgn_i));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_COND_I, IndCond_i));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_EPAS_I, IndEpas_i));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_AFTSTRENR, IndAftStrEnr));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_IACCLLOOP, IndIacClLoop));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_UNIOUT1, UniOut1));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_UNIOUT2, UniOut2));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_UNIOUT3, UniOut3));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_UNIOUT4, UniOut4));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_UNIOUT5, UniOut5));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_UNIOUT6, UniOut6));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_GPA4_I, IndGpa4_i));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_INPUT1, IndInput1));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_INPUT2, IndInput2));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_AUTO_I, IndAuto_i));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_MAPSEL0, IndMapsel0));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_REFPRS_I, IndRefprs_i));
 m_indCfg.insert(std::make_pair(IDM_MI_IND_ALTRN_I, IndAltrn_i));

 m_indRows = IndRows;
 m_IndHeightCoeff = IndHeingtPercent / 100.0f;

 m_leds.SetNumRows(IndRows);
 m_indFields.clear();

 for (std::map<UINT, IndCfg_t>::iterator fi = m_indCfg.begin(); fi != m_indCfg.end(); ++fi)
  _IndFactory(fi->first);

 _CreateLEDs();
}

void CMIDeskDlg::SetMetersCfg(const MetersCfg* cfg, int TitleFontSize, int ValueFontSize, int PaneFontSize, int LabelFontSize)
{
 m_metCfg.clear();
 m_metCfg.insert(std::make_pair(IDM_MI_MET_RPM, cfg->m_optMetRPM[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAP, cfg->m_optMetMAP[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_VBAT, cfg->m_optMetVBat[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_IGNTIM, cfg->m_optMetIgnTim[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_CLT, cfg->m_optMetCLT[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_ADDI1, cfg->m_optMetAddI1[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_ADDI2, cfg->m_optMetAddI2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJPW, cfg->m_optMetInjPW[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_IAT, cfg->m_optMetIAT[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_EGOCORR, cfg->m_optMetEGOCorr[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TPS, cfg->m_optMetTPS[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_AIRFLOW, cfg->m_optMetAirFlow[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_VEHSPD, cfg->m_optMetVehicleSpeed[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TPSDOT, cfg->m_optMetTPSDot[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAPDOT, cfg->m_optMetMAPDot[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAP2, cfg->m_optMetMAP2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_GPS, cfg->m_optMetGPS[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAPD, cfg->m_optMetMAPD[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TMP2, cfg->m_optMetTmp2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FUELCONSUM, cfg->m_optMetFuelConsum[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_KNOCKRETARD, cfg->m_optMetKnockRetard[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_KNOCKGRAPH, cfg->m_optMetKnockGraph[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_SENSAFR, cfg->m_optMetSensAFR[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_CHOKEPOS, cfg->m_optMetChokePos[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_GDPOS, cfg->m_optMetGDPos[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_SYNLOAD, cfg->m_optMetSynLoad[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJTIMB, cfg->m_optMetInjTimB[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJTIME, cfg->m_optMetInjTimE[0]));
 m_optMetInjTimB1[0] = cfg->m_optMetInjTimB1[0];
 m_optMetInjTimE1[0] = cfg->m_optMetInjTimE1[0];
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FUELCONSUMF, cfg->m_optMetFuelConsumF[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_GRTS, cfg->m_optMetGrts[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FTLS, cfg->m_optMetFtls[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_EGTS, cfg->m_optMetEgts[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_OPS, cfg->m_optMetOps[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJDUTY, cfg->m_optMetInjDuty[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAF, cfg->m_optMetMAF[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_VENTDUTY, cfg->m_optMetVentDuty[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FTS, cfg->m_optMetFts[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_EGOCORR2, cfg->m_optMetEGOCorr2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_SENSAFR2, cfg->m_optMetSensAFR2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TARGAFR, cfg->m_optMetTargAFR[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_DIFFAFR, cfg->m_optMetDiffAFR[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_DIFFAFR2, cfg->m_optMetDiffAFR2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FPS, cfg->m_optMetFPS[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_APPS, cfg->m_optMetAPPS[0]));

 //graphs
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_RPM, cfg->m_optMetRPM[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAP, cfg->m_optMetMAP[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_VBAT, cfg->m_optMetVBat[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_IGNTIM, cfg->m_optMetIgnTim[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_CLT, cfg->m_optMetCLT[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_ADDI1, cfg->m_optMetAddI1[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_ADDI2, cfg->m_optMetAddI2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_INJPW, cfg->m_optMetInjPW[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_IAT, cfg->m_optMetIAT[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_EGOCORR, cfg->m_optMetEGOCorr[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_TPS, cfg->m_optMetTPS[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_AIRFLOW, cfg->m_optMetAirFlow[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_VEHSPD, cfg->m_optMetVehicleSpeed[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_TPSDOT, cfg->m_optMetTPSDot[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAPDOT, cfg->m_optMetMAPDot[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAP2, cfg->m_optMetMAP2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_GPS, cfg->m_optMetGPS[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAPD, cfg->m_optMetMAPD[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_TMP2, cfg->m_optMetTmp2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_FUELCONSUM, cfg->m_optMetFuelConsum[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_KNOCKRETARD, cfg->m_optMetKnockRetard[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_KNOCKGRAPH, cfg->m_optMetKnockGraph[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_SENSAFR, cfg->m_optMetSensAFR[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_CHOKEPOS, cfg->m_optMetChokePos[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_GDPOS, cfg->m_optMetGDPos[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_SYNLOAD, cfg->m_optMetSynLoad[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_INJTIMB, cfg->m_optMetInjTimB[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_INJTIME, cfg->m_optMetInjTimE[1]));
 m_optMetInjTimB1[1] = cfg->m_optMetInjTimB1[1];
 m_optMetInjTimE1[1] = cfg->m_optMetInjTimE1[1];
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_FUELCONSUMF, cfg->m_optMetFuelConsumF[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_GRTS, cfg->m_optMetGrts[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_FTLS, cfg->m_optMetFtls[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_EGTS, cfg->m_optMetEgts[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_OPS, cfg->m_optMetOps[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_INJDUTY, cfg->m_optMetInjDuty[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAF, cfg->m_optMetMAF[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_VENTDUTY, cfg->m_optMetVentDuty[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_FTS, cfg->m_optMetFts[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_EGOCORR2, cfg->m_optMetEGOCorr2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_SENSAFR2, cfg->m_optMetSensAFR2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_TARGAFR, cfg->m_optMetTargAFR[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_DIFFAFR, cfg->m_optMetDiffAFR[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_DIFFAFR2, cfg->m_optMetDiffAFR2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_FPS, cfg->m_optMetFPS[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_APPS, cfg->m_optMetAPPS[1]));

 m_metRows = cfg->m_optMetRows;
 m_TitleFontSize = TitleFontSize;
 m_ValueFontSize = ValueFontSize;
 m_PaneFontSize = PaneFontSize;
 m_LabelFontSize = LabelFontSize;

 //remember color to have ability to detect changing of system colors (see OnPaint)
 m_COLOR_BTNFACE = GetSysColor(COLOR_BTNFACE);

 //destroy current widgets and clear list
 _MetCleanUp();

 for (std::map<UINT, MetCfg>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);

 _MetRearrangeKeys();

 //enable/disable
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Enable(m_enabled, false);

 _Resize();
}

void CMIDeskDlg::_MetCleanUp(void)
{
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  delete it->second;
 m_metFields.clear();
 mp_miTemperat = NULL;
}

CRect CMIDeskDlg::_GetMetItemRect(int idx)
{
 //calculate number of columns from number of items and rows
 int cols = ((int)m_metFields.size() / m_metRows) + ((int)m_metFields.size() % m_metRows ? 1 : 0);
 int i = idx / cols;
 int j = idx % cols;
 CRect rc = _GetGaugesRect(true); 
 int space  = 0;
 float width  = ((float)(rc.right - rc.left) - ((float)cols)*space) / ((float)cols);
 float height = ((float)(rc.bottom - rc.top) - ((float)m_metRows)*space) / ((float)m_metRows);
 return CRect(MathHelpers::Round(j*(width+space)), MathHelpers::Round(i*(height+space)), MathHelpers::Round(j*(width+space) + width), MathHelpers::Round(i*(height+space) + height));
}

void CMIDeskDlg::_MetRearrangeKeys(void)
{
 int i = 0;
 MetFields_t new_map;
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
 {
  new_map.insert(std::make_pair(i, it->second));
  m_metCfg[it->second->m_uiID].position = i++;
 }
 m_metFields = new_map;
}

void CMIDeskDlg::_IndRearrangeKeys(void)
{
 int i = 0;
 IndFields_t new_map;
 IndFields_t::iterator it;
 for(it = m_indFields.begin(); it != m_indFields.end(); ++it)
 {
  new_map.insert(std::make_pair(i, it->second));
  m_indCfg[it->second.m_uiID].first = i++;
 }
 m_indFields = new_map;
}

void CMIDeskDlg::OnPaint() 
{
 CPaintDC dc(this); // device context for painting
 COLORREF newcolor = GetSysColor(COLOR_BTNFACE);
 if (newcolor != m_COLOR_BTNFACE)
 {
  m_COLOR_BTNFACE = newcolor;
  for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
   it->second->UpdateColors();
 }
}

void CMIDeskDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
 //gauges
 for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
 {
  CRect rc = it->second->GetWindowRect();
  if (rc.PtInRect(point) && m_metDragNDrop)
  {
   m_defCursor = SetCursor(m_hMoveCursor);
   m_draggingMet = true;
   m_dragItemMet = it;
  }
 }

 //indicators
 CPoint ptscr = point;
 ClientToScreen(&ptscr);
 for(IndFields_t::iterator it = m_indFields.begin(); it != m_indFields.end(); ++it)  
 {
  CRect rc = m_leds.GetItemRect(std::distance(m_indFields.begin(), it));
  m_leds.ClientToScreen(&rc);
  if (rc.PtInRect(ptscr) && m_indDragNDrop)
  {
   m_defCursor = SetCursor(m_hMoveCursor);
   m_draggingInd = true;
   m_dragItemInd = it;
  }
 }

 Super::OnLButtonDown(nFlags, point);
}

void CMIDeskDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
 //meters
 if (m_draggingMet && m_metDragNDrop)
 {
  for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
  {
   CRect rc = it->second->GetWindowRect();
   if (rc.PtInRect(point))
   {
    if (m_dragItemMet != it)
    {
     //swap gauges
     MeasInstrBase* widget = m_dragItemMet->second;
     m_dragItemMet->second = it->second;
     it->second = widget;

     //update order in settings
     m_metCfg[m_dragItemMet->second->m_uiID].position = m_dragItemMet->first;
     m_metCfg[it->second->m_uiID].position = it->first; 

     //update positions of widgets
     _Resize();

     if (m_OnMISettingsChanged)
      m_OnMISettingsChanged();

     break;
    }
   }
  }
 }

 //indicators
 if (m_draggingInd && m_indDragNDrop)
 {
  CPoint ptscr = point;
  ClientToScreen(&ptscr);
  for(IndFields_t::iterator it = m_indFields.begin(); it != m_indFields.end(); ++it)  
  {
   CRect rc = m_leds.GetItemRect(std::distance(m_indFields.begin(), it));
   m_leds.ClientToScreen(&rc);
   if (rc.PtInRect(ptscr))
   {
    if (m_dragItemInd != it)
    {
     //swap indicators
     IndFieldData ind = m_dragItemInd->second;
     m_dragItemInd->second = it->second;
     it->second = ind;

     //update order in settings
     m_indCfg[m_dragItemInd->second.m_uiID].first = m_dragItemInd->first;
     m_indCfg[it->second.m_uiID].first = it->first; 

     _CreateLEDs();

     m_leds.Invalidate();

     if (m_OnMISettingsChanged)
      m_OnMISettingsChanged();

     break;
    }
   }
  }
 }
 if (m_defCursor)
  SetCursor(m_defCursor);
 m_draggingMet = false;
 m_draggingInd = false;
 Super::OnLButtonUp(nFlags, point);
}

void CMIDeskDlg::OnMouseMove(UINT nFlags, CPoint point)
{
 if (!_GetGaugesRect(true).PtInRect(point))
  m_draggingMet = false;

 if (!_GetGaugesRect(false).PtInRect(point))
  m_draggingInd = false;

 if (m_draggingMet || m_draggingInd)
  SetCursor(m_hMoveCursor);

 //this code tell system to call OnMouseLeave when mouse leaves window
 TRACKMOUSEEVENT tme;
 tme.cbSize = sizeof(TRACKMOUSEEVENT);
 tme.dwFlags = TME_LEAVE;
 tme.hwndTrack = GetSafeHwnd();
 TrackMouseEvent(&tme);

 Super::OnMouseMove(nFlags, point);
}

BOOL CMIDeskDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
 if (m_draggingMet || m_draggingInd)
  return FALSE;

 return Super::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CMIDeskDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
 m_draggingMet = false;
 m_draggingInd = false;
 return TRUE;
}

void CMIDeskDlg::GetMetersCfg(MetersCfg* cfg, int &TitleFontSize, int &ValueFontSize, int &PaneFontSize, int &LabelFontSize)
{
 cfg->m_optMetRows = m_metRows;
 TitleFontSize = m_TitleFontSize, ValueFontSize = m_ValueFontSize, PaneFontSize = m_PaneFontSize, LabelFontSize = m_LabelFontSize;

 cfg->m_optMetRPM[0] = m_metCfg[IDM_MI_MET_RPM];
 cfg->m_optMetMAP[0] = m_metCfg[IDM_MI_MET_MAP];
 cfg->m_optMetVBat[0] = m_metCfg[IDM_MI_MET_VBAT];
 cfg->m_optMetIgnTim[0] = m_metCfg[IDM_MI_MET_IGNTIM];
 cfg->m_optMetCLT[0] = m_metCfg[IDM_MI_MET_CLT];
 cfg->m_optMetAddI1[0] = m_metCfg[IDM_MI_MET_ADDI1];
 cfg->m_optMetAddI2[0] = m_metCfg[IDM_MI_MET_ADDI2];
 cfg->m_optMetInjPW[0] = m_metCfg[IDM_MI_MET_INJPW];
 cfg->m_optMetIAT[0] = m_metCfg[IDM_MI_MET_IAT];
 cfg->m_optMetEGOCorr[0] = m_metCfg[IDM_MI_MET_EGOCORR];
 cfg->m_optMetTPS[0] = m_metCfg[IDM_MI_MET_TPS];
 cfg->m_optMetAirFlow[0] = m_metCfg[IDM_MI_MET_AIRFLOW];
 cfg->m_optMetVehicleSpeed[0] = m_metCfg[IDM_MI_MET_VEHSPD];
 cfg->m_optMetTPSDot[0] = m_metCfg[IDM_MI_MET_TPSDOT];
 cfg->m_optMetMAPDot[0] = m_metCfg[IDM_MI_MET_MAPDOT];
 cfg->m_optMetMAP2[0] = m_metCfg[IDM_MI_MET_MAP2];
 cfg->m_optMetGPS[0] = m_metCfg[IDM_MI_MET_GPS];
 cfg->m_optMetMAPD[0] = m_metCfg[IDM_MI_MET_MAPD];
 cfg->m_optMetTmp2[0] = m_metCfg[IDM_MI_MET_TMP2];
 cfg->m_optMetFuelConsum[0] = m_metCfg[IDM_MI_MET_FUELCONSUM];
 cfg->m_optMetKnockRetard[0] = m_metCfg[IDM_MI_MET_KNOCKRETARD];
 cfg->m_optMetKnockGraph[0] = m_metCfg[IDM_MI_MET_KNOCKGRAPH];
 cfg->m_optMetSensAFR[0] = m_metCfg[IDM_MI_MET_SENSAFR];
 cfg->m_optMetChokePos[0] = m_metCfg[IDM_MI_MET_CHOKEPOS];
 cfg->m_optMetGDPos[0] = m_metCfg[IDM_MI_MET_GDPOS];
 cfg->m_optMetSynLoad[0] = m_metCfg[IDM_MI_MET_SYNLOAD];
 cfg->m_optMetInjTimB[0] = m_metCfg[IDM_MI_MET_INJTIMB];
 cfg->m_optMetInjTimE[0] = m_metCfg[IDM_MI_MET_INJTIME];
 cfg->m_optMetInjTimB1[0] = m_optMetInjTimB1[0];
 cfg->m_optMetInjTimE1[0] = m_optMetInjTimE1[0];
 cfg->m_optMetFuelConsumF[0] = m_metCfg[IDM_MI_MET_FUELCONSUMF];
 cfg->m_optMetGrts[0] = m_metCfg[IDM_MI_MET_GRTS];
 cfg->m_optMetFtls[0] = m_metCfg[IDM_MI_MET_FTLS];
 cfg->m_optMetEgts[0] = m_metCfg[IDM_MI_MET_EGTS];
 cfg->m_optMetOps[0] = m_metCfg[IDM_MI_MET_OPS];
 cfg->m_optMetInjDuty[0] = m_metCfg[IDM_MI_MET_INJDUTY];
 cfg->m_optMetMAF[0] = m_metCfg[IDM_MI_MET_MAF];
 cfg->m_optMetVentDuty[0] = m_metCfg[IDM_MI_MET_VENTDUTY];
 cfg->m_optMetFts[0] = m_metCfg[IDM_MI_MET_FTS];
 cfg->m_optMetEGOCorr2[0] = m_metCfg[IDM_MI_MET_EGOCORR2];
 cfg->m_optMetSensAFR2[0] = m_metCfg[IDM_MI_MET_SENSAFR2];
 cfg->m_optMetTargAFR[0] = m_metCfg[IDM_MI_MET_TARGAFR];
 cfg->m_optMetDiffAFR[0] = m_metCfg[IDM_MI_MET_DIFFAFR];
 cfg->m_optMetDiffAFR2[0] = m_metCfg[IDM_MI_MET_DIFFAFR2];
 cfg->m_optMetFPS[0] = m_metCfg[IDM_MI_MET_FPS];
 cfg->m_optMetAPPS[0] = m_metCfg[IDM_MI_MET_APPS];

 cfg->m_optMetRPM[1] = m_metCfg[IDM_MI_GRH_RPM];
 cfg->m_optMetMAP[1] = m_metCfg[IDM_MI_GRH_MAP];
 cfg->m_optMetVBat[1] = m_metCfg[IDM_MI_GRH_VBAT];
 cfg->m_optMetIgnTim[1] = m_metCfg[IDM_MI_GRH_IGNTIM];
 cfg->m_optMetCLT[1] = m_metCfg[IDM_MI_GRH_CLT];
 cfg->m_optMetAddI1[1] = m_metCfg[IDM_MI_GRH_ADDI1];
 cfg->m_optMetAddI2[1] = m_metCfg[IDM_MI_GRH_ADDI2];
 cfg->m_optMetInjPW[1] = m_metCfg[IDM_MI_GRH_INJPW];
 cfg->m_optMetIAT[1] = m_metCfg[IDM_MI_GRH_IAT];
 cfg->m_optMetEGOCorr[1] = m_metCfg[IDM_MI_GRH_EGOCORR];
 cfg->m_optMetTPS[1] = m_metCfg[IDM_MI_GRH_TPS];
 cfg->m_optMetAirFlow[1] = m_metCfg[IDM_MI_GRH_AIRFLOW];
 cfg->m_optMetVehicleSpeed[1] = m_metCfg[IDM_MI_GRH_VEHSPD];
 cfg->m_optMetTPSDot[1] = m_metCfg[IDM_MI_GRH_TPSDOT];
 cfg->m_optMetMAPDot[1] = m_metCfg[IDM_MI_GRH_MAPDOT];
 cfg->m_optMetMAP2[1] = m_metCfg[IDM_MI_GRH_MAP2];
 cfg->m_optMetGPS[1] = m_metCfg[IDM_MI_GRH_GPS];
 cfg->m_optMetMAPD[1] = m_metCfg[IDM_MI_GRH_MAPD];
 cfg->m_optMetTmp2[1] = m_metCfg[IDM_MI_GRH_TMP2];
 cfg->m_optMetFuelConsum[1] = m_metCfg[IDM_MI_GRH_FUELCONSUM];
 cfg->m_optMetKnockRetard[1] = m_metCfg[IDM_MI_GRH_KNOCKRETARD];
 cfg->m_optMetKnockGraph[1] = m_metCfg[IDM_MI_GRH_KNOCKGRAPH];
 cfg->m_optMetSensAFR[1] = m_metCfg[IDM_MI_GRH_SENSAFR];
 cfg->m_optMetChokePos[1] = m_metCfg[IDM_MI_GRH_CHOKEPOS];
 cfg->m_optMetGDPos[1] = m_metCfg[IDM_MI_GRH_GDPOS];
 cfg->m_optMetSynLoad[1] = m_metCfg[IDM_MI_GRH_SYNLOAD];
 cfg->m_optMetInjTimB[1] = m_metCfg[IDM_MI_GRH_INJTIMB];
 cfg->m_optMetInjTimE[1] = m_metCfg[IDM_MI_GRH_INJTIME];
 cfg->m_optMetInjTimB1[1] = m_optMetInjTimB1[1];
 cfg->m_optMetInjTimE1[1] = m_optMetInjTimE1[1];
 cfg->m_optMetFuelConsumF[1] = m_metCfg[IDM_MI_GRH_FUELCONSUMF];
 cfg->m_optMetGrts[1] = m_metCfg[IDM_MI_GRH_GRTS];
 cfg->m_optMetFtls[1] = m_metCfg[IDM_MI_GRH_FTLS];
 cfg->m_optMetEgts[1] = m_metCfg[IDM_MI_GRH_EGTS];
 cfg->m_optMetOps[1] = m_metCfg[IDM_MI_GRH_OPS];
 cfg->m_optMetInjDuty[1] = m_metCfg[IDM_MI_GRH_INJDUTY];
 cfg->m_optMetMAF[1] = m_metCfg[IDM_MI_GRH_MAF];
 cfg->m_optMetVentDuty[1] = m_metCfg[IDM_MI_GRH_VENTDUTY];
 cfg->m_optMetFts[1] = m_metCfg[IDM_MI_GRH_FTS];
 cfg->m_optMetEGOCorr2[1] = m_metCfg[IDM_MI_GRH_EGOCORR2];
 cfg->m_optMetSensAFR2[1] = m_metCfg[IDM_MI_GRH_SENSAFR2];
 cfg->m_optMetTargAFR[1] = m_metCfg[IDM_MI_GRH_TARGAFR];
 cfg->m_optMetDiffAFR[1] = m_metCfg[IDM_MI_GRH_DIFFAFR];
 cfg->m_optMetDiffAFR2[1] = m_metCfg[IDM_MI_GRH_DIFFAFR2];
 cfg->m_optMetFPS[1] = m_metCfg[IDM_MI_GRH_FPS];
 cfg->m_optMetAPPS[1] = m_metCfg[IDM_MI_GRH_APPS];
}

void CMIDeskDlg::GetIndicatorsCfg(float &IndHeingtPercent, int &IndRows, IndCfg_t &IndGas_v, IndCfg_t &IndCarb, IndCfg_t &IndIdleValve, IndCfg_t &IndPowerValve, IndCfg_t &IndStBlock, IndCfg_t &IndAE,
                      IndCfg_t &IndCoolingFan, IndCfg_t &IndCE, IndCfg_t &IndFCRevLim, IndCfg_t &IndFloodClear, IndCfg_t &IndSysLocked, IndCfg_t &IndIgn_i, IndCfg_t &IndCond_i, IndCfg_t &IndEpas_i, IndCfg_t &IndAftStrEnr, IndCfg_t &IndIacClLoop, IndCfg_t &UniOut1, IndCfg_t &UniOut2, IndCfg_t &UniOut3, IndCfg_t &UniOut4, IndCfg_t &UniOut5, IndCfg_t &UniOut6, IndCfg_t &IndGpa4_i, IndCfg_t &IndInput1, IndCfg_t &IndInput2, IndCfg_t &IndAuto_i, IndCfg_t &IndMapsel0, IndCfg_t &IndRefprs_i, IndCfg_t &IndAltrn_i)
{
 IndHeingtPercent = m_IndHeightCoeff * 100.0f;
 IndRows = m_indRows;
 
 IndGas_v = m_indCfg[IDM_MI_IND_GAS_V];
 IndCarb = m_indCfg[IDM_MI_IND_CARB];
 IndIdleValve = m_indCfg[IDM_MI_IND_IDLEVALVE];
 IndPowerValve = m_indCfg[IDM_MI_IND_POWERVALVE];
 IndStBlock = m_indCfg[IDM_MI_IND_STBLOCK];
 IndAE = m_indCfg[IDM_MI_IND_AE];
 IndCoolingFan = m_indCfg[IDM_MI_IND_COOLINGFAN];
 IndCE = m_indCfg[IDM_MI_IND_CE];
 IndFCRevLim = m_indCfg[IDM_MI_IND_FCREVLIM];
 IndFloodClear = m_indCfg[IDM_MI_IND_FLOODCLEAR];
 IndSysLocked = m_indCfg[IDM_MI_IND_SYSLOCKED];
 IndIgn_i = m_indCfg[IDM_MI_IND_IGN_I];
 IndCond_i = m_indCfg[IDM_MI_IND_COND_I];
 IndEpas_i = m_indCfg[IDM_MI_IND_EPAS_I];
 IndAftStrEnr = m_indCfg[IDM_MI_IND_AFTSTRENR];
 IndIacClLoop = m_indCfg[IDM_MI_IND_IACCLLOOP];
 UniOut1 = m_indCfg[IDM_MI_IND_UNIOUT1];
 UniOut2 = m_indCfg[IDM_MI_IND_UNIOUT2];
 UniOut3 = m_indCfg[IDM_MI_IND_UNIOUT3];
 UniOut4 = m_indCfg[IDM_MI_IND_UNIOUT4];
 UniOut5 = m_indCfg[IDM_MI_IND_UNIOUT5];
 UniOut6 = m_indCfg[IDM_MI_IND_UNIOUT6];
 IndGpa4_i = m_indCfg[IDM_MI_IND_GPA4_I];
 IndInput1 = m_indCfg[IDM_MI_IND_INPUT1];
 IndInput2 = m_indCfg[IDM_MI_IND_INPUT2];
 IndAuto_i = m_indCfg[IDM_MI_IND_AUTO_I];
 IndMapsel0 = m_indCfg[IDM_MI_IND_MAPSEL0];
 IndRefprs_i = m_indCfg[IDM_MI_IND_REFPRS_I];
 IndAltrn_i = m_indCfg[IDM_MI_IND_ALTRN_I];
}

void CMIDeskDlg::setOnMISettingsChanged(EventHandler i_Function)
{
 m_OnMISettingsChanged = i_Function;
}

void CMIDeskDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 //display menu for gauges
 CRect rc = _GetGaugesRect(true, true);
 if (rc.PtInRect(point))
 {
  if (m_metFields.size())
  {
   m_dragItemMet = m_metFields.end();
   for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
   {
    CRect rc = it->second->GetWindowRect(true);
    if (rc.PtInRect(point))
    {
     m_dragItemMet = it;
     break;
    }
   }
  }
  else
  {
   m_dragItemMet = m_metFields.end();
  }
  mp_ctxMenuMgrMet->TrackPopupMenu(point.x, point.y);
 }
 //display menu for indicators
 rc = _GetGaugesRect(false, true);
 if (rc.PtInRect(point))
 {
  if (m_indFields.size())
  {
   m_dragItemInd = m_indFields.end();
   for(IndFields_t::iterator it = m_indFields.begin(); it != m_indFields.end(); ++it)  
   {
    CRect rc = m_leds.GetItemRect(std::distance(m_indFields.begin(), it));
    m_leds.ClientToScreen(&rc);
    if (rc.PtInRect(point))
    {
     m_dragItemInd = it;
     break;
    }
   }
  }
  else
  {
   m_dragItemInd = m_indFields.end();
  }
  mp_ctxMenuMgrInd->TrackPopupMenu(point.x, point.y);
 }
}

void CMIDeskDlg::OnMetDeleteGauge()
{
 m_metCfg[m_dragItemMet->second->m_uiID].position = std::numeric_limits<int>::max(); 
 delete m_dragItemMet->second;
 m_metFields.erase(m_dragItemMet);

 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetDelete(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_metFields.size() && m_dragItemMet != m_metFields.end());
}

void CMIDeskDlg::OnMetEditScale()
{
 ScaleEditorDlg dlg;

 MetCfg* p_mcfg = NULL;
 UINT uiId = m_dragItemMet->second->m_uiID;

 CString gaugeName;
 mp_ctxMenuMgrMet->GetAddMenuItemString(uiId, gaugeName, m_dragItemMet->second->isGraph());

 if (m_it_mode < 2)
 { //0...720
  p_mcfg = &m_metCfg[uiId];
 }
 else
 { //-360...360
  if (uiId == IDM_MI_MET_INJTIMB)
   p_mcfg = &m_optMetInjTimB1[0];
  else if (uiId == IDM_MI_MET_INJTIME)
   p_mcfg = &m_optMetInjTimE1[0];
  else if (uiId == IDM_MI_GRH_INJTIMB)
   p_mcfg = &m_optMetInjTimB1[1];
  else if (uiId == IDM_MI_GRH_INJTIME)
   p_mcfg = &m_optMetInjTimE1[1];
  else
   p_mcfg = &m_metCfg[uiId]; //other gauges
 }

 dlg.SetName(gaugeName);
 dlg.SetData(*p_mcfg, !m_dragItemMet->second->isGraph());
 if (dlg.DoModal()==IDOK)
 {
  dlg.GetData(*p_mcfg); //update settings
  if (uiId == IDM_MI_MET_INJTIMB)
  {
   CMIInjTimB* widget = static_cast<CMIInjTimB*>(m_dragItemMet->second);
   widget->SetLimits(m_metCfg[uiId].scaleMin, m_metCfg[uiId].scaleMax, m_optMetInjTimB1[0].scaleMin, m_optMetInjTimB1[0].scaleMax);
   widget->SetAlertZones(m_metCfg[uiId].alezn, m_optMetInjTimB1[0].alezn);
   widget->SetTickNumber(m_metCfg[uiId].ticksNum, m_optMetInjTimB1[0].ticksNum);
   widget->SetLineWidth(p_mcfg->scaleWidth);
   widget->SetLineColor(p_mcfg->scaleColor);
   widget->SetITMode(m_it_mode); //<---sets limits, alert zones and number of ticks
  }
  else if (uiId == IDM_MI_MET_INJTIME)
  {
   CMIInjTimE* widget = static_cast<CMIInjTimE*>(m_dragItemMet->second);
   widget->SetLimits(m_metCfg[uiId].scaleMin, m_metCfg[uiId].scaleMax, m_optMetInjTimE1[0].scaleMin, m_optMetInjTimE1[0].scaleMax);
   widget->SetAlertZones(m_metCfg[uiId].alezn, m_optMetInjTimE1[0].alezn);
   widget->SetTickNumber(m_metCfg[uiId].ticksNum, m_optMetInjTimE1[0].ticksNum);
   widget->SetLineWidth(p_mcfg->scaleWidth);
   widget->SetLineColor(p_mcfg->scaleColor);
   widget->SetITMode(m_it_mode); //<---sets limits, alert zones and number of ticks
  }
  else if (uiId == IDM_MI_GRH_INJTIMB)
  {
   CMIInjTimBGraph* widget = static_cast<CMIInjTimBGraph*>(m_dragItemMet->second);
   widget->SetLimits(m_metCfg[uiId].scaleMin, m_metCfg[uiId].scaleMax, m_optMetInjTimB1[1].scaleMin, m_optMetInjTimB1[1].scaleMax);
   widget->SetTickNumber(m_metCfg[uiId].ticksNum, m_optMetInjTimB1[1].ticksNum);
   widget->SetLineWidth(p_mcfg->scaleWidth);
   widget->SetLineColor(p_mcfg->scaleColor);
   widget->SetITMode(m_it_mode); //<---sets limits and number of ticks
  }
  else if (uiId == IDM_MI_GRH_INJTIME)
  {
   CMIInjTimEGraph* widget = static_cast<CMIInjTimEGraph*>(m_dragItemMet->second);
   widget->SetLimits(m_metCfg[uiId].scaleMin, m_metCfg[uiId].scaleMax, m_optMetInjTimE1[1].scaleMin, m_optMetInjTimE1[1].scaleMax);
   widget->SetTickNumber(m_metCfg[uiId].ticksNum, m_optMetInjTimE1[1].ticksNum);
   widget->SetLineWidth(p_mcfg->scaleWidth);
   widget->SetLineColor(p_mcfg->scaleColor);
   widget->SetITMode(m_it_mode); //<---sets limits and number of ticks
  }
  else
  {
   //update widget
   MeasInstrBase* widget = m_dragItemMet->second;
   widget->SetLimits(p_mcfg->scaleMin, p_mcfg->scaleMax);
   if (!widget->isGraph())
   {
    widget->SetAlertZones(p_mcfg->alezn);
   }
   widget->SetTickNumber(p_mcfg->ticksNum);
   widget->SetLineWidth(p_mcfg->scaleWidth);
   widget->SetLineColor(p_mcfg->scaleColor);
  }
  m_dragItemMet->second->Redraw();

  if (m_OnMISettingsChanged)
   m_OnMISettingsChanged();
 }
}

void CMIDeskDlg::OnMetAddGauge(UINT nID)
{
 if (m_dragItemMet == m_metFields.end())
  m_metCfg[nID].position = m_metFields.size() ? m_metFields.rbegin()->first : 0;
 else
  m_metCfg[nID].position = m_dragItemMet->first;

 MeasInstrBase *widget = _MetFactory(nID);
 if (widget)
  widget->Enable(m_enabled);

 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetAddGauge(CCmdUI* pCmdUI)
{
 bool already_add = m_metCfg[pCmdUI->m_nID].position != std::numeric_limits<int>::max();
 pCmdUI->Enable(!already_add);
 pCmdUI->SetCheck(already_add);
}

MeasInstrBase* CMIDeskDlg::_MetFactory(UINT uiID)
{
 DPIAware da;
 int TitleFontSize = da.UnScaleY(m_TitleFontSize);
 int ValueFontSize = da.UnScaleY(m_ValueFontSize);
 int PaneFontSize = da.UnScaleY(m_PaneFontSize);
 int LabelFontSize = da.UnScaleY(m_LabelFontSize);

 MeasInstrBase* new_widget = NULL;
 switch(uiID)
 {
  case IDM_MI_MET_RPM:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITachometer* widget = new CMITachometer();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrRPM, &m_avrNum.avrVehicleSpeed, NULL); //<--don't average distance
   widget->Create(this);
   widget->ShowTLP(m_showSpeedAndDistance);
   widget->ShowTRP(m_showSpeedAndDistance);
   widget->SetPaneUnit(m_speedUnit, m_distanceUnit);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_RPM:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITachometerGraph* widget = new CMITachometerGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAP:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIPressure* widget = new CMIPressure();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrMAP, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAP:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIPressureGraph* widget = new CMIPressureGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_VBAT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIVoltmeter* widget = new CMIVoltmeter();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrVolt, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_VBAT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIVoltmeterGraph* widget = new CMIVoltmeterGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_IGNTIM:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIDwellAngle* widget = new CMIDwellAngle();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrIgnTim, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_IGNTIM:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIDwellAngleGraph* widget = new CMIDwellAngleGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_CLT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
   {
    mp_miTemperat = NULL;
    break;
   }
   CMITemperature* widget = new CMITemperature();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrCLT, &m_avrNum.avrGDPos, &m_avrNum.avrChokePos);
   widget->Create(this);
   widget->ShowTLP(m_showGDPos);
   widget->ShowTRP(m_showChokePos);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   mp_miTemperat = widget;
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_CLT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITemperatureGraph* widget = new CMITemperatureGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_ADDI1:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIAddI1* widget = new CMIAddI1();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrAddI1, 0, 0);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_ADDI1:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIAddI1Graph* widget = new CMIAddI1Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_ADDI2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIAddI2* widget = new CMIAddI2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrAddI2, 0, 0);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_ADDI2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIAddI2Graph* widget = new CMIAddI2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_INJPW:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIInjPW* widget = new CMIInjPW();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrInjPW, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_INJPW:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIInjPWGraph* widget = new CMIInjPWGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_IAT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIIAT* widget = new CMIIAT();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrIAT, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_IAT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIIATGraph* widget = new CMIIATGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_EGOCORR:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIEGOCorr* widget = new CMIEGOCorr();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrEGOCorr, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_EGOCORR:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIEGOCorrGraph* widget = new CMIEGOCorrGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_TPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIThrottleGate* widget = new CMIThrottleGate();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrTPS, &m_avrNum.avrAirFlow, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
// widget->ShowTLP(true);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_TPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIThrottleGateGraph* widget = new CMIThrottleGateGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_AIRFLOW:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIAirFlow* widget = new CMIAirFlow();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrAirFlow, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_AIRFLOW:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIAirFlowGraph* widget = new CMIAirFlowGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_VEHSPD:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIVehicleSpeed* widget = new CMIVehicleSpeed();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrVehicleSpeed, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetMeterUnit(m_speedUnit);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_VEHSPD:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIVehicleSpeedGraph* widget = new CMIVehicleSpeedGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetMeterUnit(m_speedUnit);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_TPSDOT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITPSDot* widget = new CMITPSDot();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrTPSDot, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_TPSDOT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITPSDotGraph* widget = new CMITPSDotGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAPDOT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIMAPDot* widget = new CMIMAPDot();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrMAPDot, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAPDOT:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIMAPDotGraph* widget = new CMIMAPDotGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAP2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIMap2* widget = new CMIMap2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrMAP2, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAP2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIMap2Graph* widget = new CMIMap2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_GPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIGps* widget = new CMIGps();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrGPS, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_GPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIGpsGraph* widget = new CMIGpsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAPD:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIMapD* widget = new CMIMapD();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrMAPD, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAPD:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIMapDGraph* widget = new CMIMapDGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_TMP2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITemp2* widget = new CMITemp2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrTmp2, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_TMP2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITemp2Graph* widget = new CMITemp2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_GRTS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIGrts* widget = new CMIGrts();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrGrts, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_GRTS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIGrtsGraph* widget = new CMIGrtsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_FUELCONSUM:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFuelConsum* widget = new CMIFuelConsum();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrFuelConsum, &m_avrNum.avrFuelConsumF, NULL); //<--don't average consumed fuel
   widget->Create(this);
// widget->ShowTLP(true);
// widget->ShowTRP(true);
   widget->SetPaneUnit(MLL::GetString(IDS_MI_FUELCONSUMH_UNIT), MLL::GetString(IDS_MI_CONSFUEL_UNIT));
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_FUELCONSUM:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFuelConsumGraph* widget = new CMIFuelConsumGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_KNOCKRETARD:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIKnockRetard* widget = new CMIKnockRetard();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrKnockRetard, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_KNOCKRETARD:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIKnockRetardGraph* widget = new CMIKnockRetardGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_KNOCKGRAPH:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIKnock* widget = new CMIKnock();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrKnock, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_KNOCKGRAPH:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIKnockGraph* widget = new CMIKnockGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_SENSAFR:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMISensAFR* widget = new CMISensAFR();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrSensAFR, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_SENSAFR:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMISensAFRGraph* widget = new CMISensAFRGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_CHOKEPOS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIChokePos* widget = new CMIChokePos();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrChokePos, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_CHOKEPOS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIChokePosGraph* widget = new CMIChokePosGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_GDPOS:
  {                                  	
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIGDPos* widget = new CMIGDPos();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrGDPos, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_GDPOS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIGDPosGraph* widget = new CMIGDPosGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_SYNLOAD:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMISynLoad* widget = new CMISynLoad();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrSynLoad, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_SYNLOAD:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMISynLoadGraph* widget = new CMISynLoadGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_INJTIMB:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIInjTimB* widget = new CMIInjTimB();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrInjTimB, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax, m_optMetInjTimB1[0].scaleMin, m_optMetInjTimB1[0].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn, m_optMetInjTimB1[0].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum, m_optMetInjTimB1[0].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetITMode(m_it_mode); //<---sets limits, alert zones and number of ticks
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_INJTIMB:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIInjTimBGraph* widget = new CMIInjTimBGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax, m_optMetInjTimB1[1].scaleMin, m_optMetInjTimB1[1].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum, m_optMetInjTimB1[1].ticksNum); 
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetITMode(m_it_mode); //<---sets limits and number of ticks
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_INJTIME:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIInjTimE* widget = new CMIInjTimE();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrInjTimE, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax, m_optMetInjTimE1[0].scaleMin, m_optMetInjTimE1[0].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn, m_optMetInjTimE1[0].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum, m_optMetInjTimE1[0].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetITMode(m_it_mode); //<---sets limits, alert zones and number of ticks
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_INJTIME:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIInjTimEGraph* widget = new CMIInjTimEGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax, m_optMetInjTimE1[1].scaleMin, m_optMetInjTimE1[1].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum, m_optMetInjTimE1[1].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetITMode(m_it_mode); //<---sets limits and number of ticks
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_FUELCONSUMF:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFuelConsumF* widget = new CMIFuelConsumF();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrFuelConsumF, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_FUELCONSUMF:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFuelConsumFGraph* widget = new CMIFuelConsumFGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_FTLS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFtls* widget = new CMIFtls();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrFtls, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_FTLS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFtlsGraph* widget = new CMIFtlsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_EGTS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIEgts* widget = new CMIEgts();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrEgts, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_EGTS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIEgtsGraph* widget = new CMIEgtsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_OPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIOps* widget = new CMIOps();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrOps, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_OPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIOpsGraph* widget = new CMIOpsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_INJDUTY:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIInjDuty* widget = new CMIInjDuty();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(NULL, NULL, NULL); //<--don't avarage inj. duty
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_INJDUTY:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIInjDutyGraph* widget = new CMIInjDutyGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAF:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIMaf* widget = new CMIMaf();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrMAF, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAF:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIMafGraph* widget = new CMIMafGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_VENTDUTY:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIVentDuty* widget = new CMIVentDuty();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(NULL, NULL, NULL);  //<-- don't avarage vent. duty
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_VENTDUTY:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIVentDutyGraph* widget = new CMIVentDutyGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_FTS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFts* widget = new CMIFts();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrFts, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_FTS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFtsGraph* widget = new CMIFtsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_EGOCORR2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIEGOCorr2* widget = new CMIEGOCorr2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrEGOCorr, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_EGOCORR2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIEGOCorr2Graph* widget = new CMIEGOCorr2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_SENSAFR2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMISensAFR2* widget = new CMISensAFR2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrSensAFR, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_SENSAFR2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMISensAFR2Graph* widget = new CMISensAFR2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_TARGAFR:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITargAFR* widget = new CMITargAFR();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrTargAFR, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_TARGAFR:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMITargAFRGraph* widget = new CMITargAFRGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_DIFFAFR:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIDiffAFR* widget = new CMIDiffAFR();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrSensAFR, NULL, NULL);  //<-- don't use averaging
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_DIFFAFR:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIDiffAFRGraph* widget = new CMIDiffAFRGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_DIFFAFR2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIDiffAFR2* widget = new CMIDiffAFR2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrSensAFR, NULL, NULL);  //<-- don't use averaging
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_DIFFAFR2:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIDiffAFR2Graph* widget = new CMIDiffAFR2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_FPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFps* widget = new CMIFps();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrFPS, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_FPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIFpsGraph* widget = new CMIFpsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_APPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIAPPS* widget = new CMIAPPS();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->BindAverageNum(&m_avrNum.avrAPPS, NULL, NULL);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetAlertZones(m_metCfg[uiID].alezn);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_APPS:
  {
   if (m_metCfg[uiID].position == std::numeric_limits<int>::max())
    break;
   CMIAPPSGraph* widget = new CMIAPPSGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->SetLimits(m_metCfg[uiID].scaleMin, m_metCfg[uiID].scaleMax);
   widget->SetTickNumber(m_metCfg[uiID].ticksNum);
   widget->SetLineWidth(m_metCfg[uiID].scaleWidth);
   widget->SetLineColor(m_metCfg[uiID].scaleColor);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID].position, widget));
   new_widget = widget;
   break;
  }
 }
 return new_widget;
}

void CMIDeskDlg::OnMetNumOfRows(UINT nID)
{
 m_metRows = (nID - IDM_MI_MET_ROWS1) + 1;

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetNumOfRows(CCmdUI* pCmdUI)
{
 pCmdUI->SetCheck(m_metRows == (pCmdUI->m_nID - IDM_MI_MET_ROWS1) + 1);
}

void CMIDeskDlg::OnMetTitleFont(UINT nID)
{
 m_TitleFontSize = ((nID - IDM_MI_MET_TITLE_FONT050) * 10) + 50;

 _MetCleanUp();
 for (std::map<UINT, MetCfg>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);
 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetTitleFont(CCmdUI* pCmdUI)
{
 int fontSize = ((pCmdUI->m_nID - IDM_MI_MET_TITLE_FONT050) * 10) + 50;
 pCmdUI->SetCheck(m_TitleFontSize == fontSize);
}

void CMIDeskDlg::OnMetValueFont(UINT nID)
{
 m_ValueFontSize = ((nID - IDM_MI_MET_VALUE_FONT050) * 10) + 50;

 _MetCleanUp();
 for (std::map<UINT, MetCfg>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);
 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetValueFont(CCmdUI* pCmdUI)
{
 int fontSize = ((pCmdUI->m_nID - IDM_MI_MET_VALUE_FONT050) * 10) + 50;
 pCmdUI->SetCheck(m_ValueFontSize == fontSize);
}

void CMIDeskDlg::OnMetPaneFont(UINT nID)
{
 m_PaneFontSize = ((nID - IDM_MI_MET_PANE_FONT050) * 10) + 50;

 _MetCleanUp();
 for (std::map<UINT, MetCfg>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);
 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetPaneFont(CCmdUI* pCmdUI)
{
 int fontSize = ((pCmdUI->m_nID - IDM_MI_MET_PANE_FONT050) * 10) + 50;
 pCmdUI->SetCheck(m_PaneFontSize == fontSize);
}

void CMIDeskDlg::OnMetLabelFont(UINT nID)
{
 m_LabelFontSize = ((nID - IDM_MI_MET_LABEL_FONT050) * 10) + 50;

 _MetCleanUp();
 for (std::map<UINT, MetCfg>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
  _MetFactory(fi->first);
 _MetRearrangeKeys();

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetLabelFont(CCmdUI* pCmdUI)
{
 int fontSize = ((pCmdUI->m_nID - IDM_MI_MET_LABEL_FONT050) * 10) + 50;
 pCmdUI->SetCheck(m_LabelFontSize == fontSize);
}

void CMIDeskDlg::SetMetersDragNDrop(bool enable)
{
 m_metDragNDrop = enable;
}

bool CMIDeskDlg::GetMetersDragNDrop(void) const
{
 return m_metDragNDrop;
}

void CMIDeskDlg::SetIndicatorsDragNDrop(bool enable)
{
 m_indDragNDrop = enable;
}

bool CMIDeskDlg::GetIndicatorsDragNDrop(void) const
{
 return m_indDragNDrop;
}

void CMIDeskDlg::OnMetEnDragNDrop()
{
 m_metDragNDrop = !m_metDragNDrop;

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateMetEnDragNDrop(CCmdUI* pCmdUI)
{
 pCmdUI->SetCheck(m_metDragNDrop);
}

void CMIDeskDlg::OnIndNumOfRows(UINT nID)
{
 m_indRows = (nID - IDM_MI_IND_ROWS1) + 1;
 
 m_leds.SetNumRows(m_indRows);
 m_leds.Invalidate();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndNumOfRows(CCmdUI* pCmdUI)
{
 pCmdUI->SetCheck(m_indRows == (pCmdUI->m_nID - IDM_MI_IND_ROWS1) + 1);
}

float CMIDeskDlg::_HeightCoeffFromID(UINT nID)
{
 switch(nID)
 {
  case IDM_MI_IND_PERCHEIGHT03: return 0.03f;
  case IDM_MI_IND_PERCHEIGHT05: return 0.05f;
  case IDM_MI_IND_PERCHEIGHT07: return 0.07f;
  case IDM_MI_IND_PERCHEIGHT10: return 0.10f;
  case IDM_MI_IND_PERCHEIGHT15: return 0.15f;
 }
 return 0.10f;
}

void CMIDeskDlg::OnIndPercHeight(UINT nID)
{ 
 m_IndHeightCoeff = _HeightCoeffFromID(nID);

 //update positions of widgets
 _Resize();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndPercHeight(CCmdUI* pCmdUI)
{
 float coeff = _HeightCoeffFromID(pCmdUI->m_nID);
 pCmdUI->SetCheck(fabs(m_IndHeightCoeff - coeff) < 0.001f);
}

void CMIDeskDlg::OnIndEnDragNDrop()
{
 m_indDragNDrop = !m_indDragNDrop;

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndEnDragNDrop(CCmdUI* pCmdUI)
{
 pCmdUI->SetCheck(m_indDragNDrop);
}

void CMIDeskDlg::OnIndDelete()
{
 m_indCfg[m_dragItemInd->second.m_uiID].first = std::numeric_limits<int>::max(); 
 m_indFields.erase(m_dragItemInd);

 _CreateLEDs();

 m_leds.Invalidate();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndDelete(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_indFields.size() && m_dragItemInd != m_indFields.end());
}

void CMIDeskDlg::OnIndAddIndicator(UINT nID)
{
 if (m_dragItemInd == m_indFields.end())
  m_indCfg[nID].first = m_indFields.size() ? m_indFields.rbegin()->first : 0;
 else
  m_indCfg[nID].first = m_dragItemInd->first;

 _IndFactory(nID);

 _IndRearrangeKeys();

 _CreateLEDs();

 m_leds.Invalidate();

 if (m_OnMISettingsChanged)
  m_OnMISettingsChanged();
}

void CMIDeskDlg::OnUpdateIndAddIndicator(CCmdUI* pCmdUI)
{
 bool already_add = m_indCfg[pCmdUI->m_nID].first != std::numeric_limits<int>::max();
 pCmdUI->Enable(!already_add);
 pCmdUI->SetCheck(already_add);
}

void CMIDeskDlg::_IndFactory(UINT uiID)
{
 switch(uiID)
 {
  case IDM_MI_IND_GAS_V:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_GAS_V), m_indCfg[uiID].second, &m_values.gas, IDM_MI_IND_GAS_V)));
   break;
  case IDM_MI_IND_CARB:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_CARB), m_indCfg[uiID].second, &m_values.carb, IDM_MI_IND_CARB)));
   break;
  case IDM_MI_IND_IDLEVALVE: 
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_IDLEVALVE), m_indCfg[uiID].second, &m_values.ephh_valve, IDM_MI_IND_IDLEVALVE)));
   break;
  case IDM_MI_IND_POWERVALVE:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_POWERVALVE), m_indCfg[uiID].second, &m_values.epm_valve, IDM_MI_IND_POWERVALVE)));
   break;
  case IDM_MI_IND_STBLOCK:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_STBLOCK), m_indCfg[uiID].second, &m_values.st_block, IDM_MI_IND_STBLOCK)));
   break;
  case IDM_MI_IND_AE:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_AE), m_indCfg[uiID].second, &m_values.acceleration, IDM_MI_IND_AE)));
   break;
  case IDM_MI_IND_COOLINGFAN:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_COOLINGFAN), m_indCfg[uiID].second, &m_values.cool_fan, IDM_MI_IND_COOLINGFAN)));
   break;
  case IDM_MI_IND_CE:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_CE), m_indCfg[uiID].second, &m_values.ce_state, IDM_MI_IND_CE)));
   break;
  case IDM_MI_IND_FCREVLIM:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_FCREVLIM), m_indCfg[uiID].second, &m_values.fc_revlim, IDM_MI_IND_FCREVLIM)));
   break;
  case IDM_MI_IND_FLOODCLEAR:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_FLOODCLEAR), m_indCfg[uiID].second, &m_values.floodclear, IDM_MI_IND_FLOODCLEAR)));
   break;
  case IDM_MI_IND_SYSLOCKED:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_SYSLOCKED), m_indCfg[uiID].second, &m_values.sys_locked, IDM_MI_IND_SYSLOCKED)));
   break;
  case IDM_MI_IND_IGN_I:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_IGN_I), m_indCfg[uiID].second, &m_values.ign_i, IDM_MI_IND_IGN_I)));
   break;
  case IDM_MI_IND_COND_I:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_COND_I), m_indCfg[uiID].second, &m_values.cond_i, IDM_MI_IND_COND_I)));
   break;
  case IDM_MI_IND_EPAS_I:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_EPAS_I), m_indCfg[uiID].second, &m_values.epas_i, IDM_MI_IND_EPAS_I)));
   break;
  case IDM_MI_IND_AFTSTRENR:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_AFTSTRENR), m_indCfg[uiID].second, &m_values.aftstr_enr, IDM_MI_IND_AFTSTRENR)));
   break;
  case IDM_MI_IND_IACCLLOOP:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_IACCLLOOP), m_indCfg[uiID].second, &m_values.iac_cl_loop, IDM_MI_IND_IACCLLOOP)));
   break;
  case IDM_MI_IND_UNIOUT1:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_UNIOUT1), m_indCfg[uiID].second, &m_values.uniout[0], IDM_MI_IND_UNIOUT1)));
   break;
  case IDM_MI_IND_UNIOUT2:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_UNIOUT2), m_indCfg[uiID].second, &m_values.uniout[1], IDM_MI_IND_UNIOUT2)));
   break;
  case IDM_MI_IND_UNIOUT3:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_UNIOUT3), m_indCfg[uiID].second, &m_values.uniout[2], IDM_MI_IND_UNIOUT3)));
   break;
  case IDM_MI_IND_UNIOUT4:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_UNIOUT4), m_indCfg[uiID].second, &m_values.uniout[3], IDM_MI_IND_UNIOUT4)));
   break;
  case IDM_MI_IND_UNIOUT5:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_UNIOUT5), m_indCfg[uiID].second, &m_values.uniout[4], IDM_MI_IND_UNIOUT5)));
   break;
  case IDM_MI_IND_UNIOUT6:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_UNIOUT6), m_indCfg[uiID].second, &m_values.uniout[5], IDM_MI_IND_UNIOUT6)));
   break;
  case IDM_MI_IND_GPA4_I:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_GPA4_I), m_indCfg[uiID].second, &m_values.gpa4_i, IDM_MI_IND_GPA4_I)));
   break;
  case IDM_MI_IND_INPUT1:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_INPUT1), m_indCfg[uiID].second, &m_values.input1, IDM_MI_IND_INPUT1)));
   break;
  case IDM_MI_IND_INPUT2:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_INPUT2), m_indCfg[uiID].second, &m_values.input2, IDM_MI_IND_INPUT2)));
   break;
  case IDM_MI_IND_AUTO_I:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_AUTO_I), m_indCfg[uiID].second, &m_values.auto_i, IDM_MI_IND_AUTO_I)));
   break;
  case IDM_MI_IND_MAPSEL0:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_MAPSEL0), m_indCfg[uiID].second, &m_values.mapsel0, IDM_MI_IND_MAPSEL0)));
   break;
  case IDM_MI_IND_REFPRS_I:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_REFPRS_I), m_indCfg[uiID].second, &m_values.refprs_i, IDM_MI_IND_REFPRS_I)));
   break;
  case IDM_MI_IND_ALTRN_I:
   if (m_indCfg[uiID].first != std::numeric_limits<int>::max())
    m_indFields.insert(std::make_pair(m_indCfg[uiID].first, IndFieldData(MLL::GetString(IDS_MI_IND_ALTRN_I), m_indCfg[uiID].second, &m_values.altrn_i, IDM_MI_IND_ALTRN_I)));
   break;
 }
}

void CMIDeskDlg::OnIndSetColor()
{
 CColorDialogEx dlg(m_dragItemInd->second.color); 
 if (dlg.DoModal() == IDOK) 
 { 
  COLORREF color = dlg.GetColor(); 

  m_dragItemInd->second.color = color;
  m_indCfg[m_dragItemInd->second.m_uiID].second = color;

  m_leds.SetItemColor(std::distance(m_indFields.begin(), m_dragItemInd), color, true);

  if (m_OnMISettingsChanged)
   m_OnMISettingsChanged();
 }
}

void CMIDeskDlg::OnUpdateIndSetColor(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_indFields.size() && m_dragItemInd != m_indFields.end());
}

void CMIDeskDlg::_CreateLEDs(void)
{
 m_leds.Clear();
 IndFields_t::iterator it;
 for(it = m_indFields.begin(); it != m_indFields.end(); ++it)
  m_leds.AddItem(it->second.caption.c_str(), it->second.color);
}

CRect CMIDeskDlg::_GetGaugesRect(bool meters, bool screenCoord /*= false*/)
{
 CRect rc;
 GetClientRect(&rc);
 if (meters)
  rc.bottom = rc.bottom - MathHelpers::Round(rc.Height() * m_IndHeightCoeff);
 else
  rc.top = rc.bottom - MathHelpers::Round(rc.Height() * m_IndHeightCoeff);
 if (screenCoord)
  ClientToScreen(&rc);
 return rc;
}

void CMIDeskDlg::SetITMode(int mode)
{
 m_it_mode = mode;
 for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
 {
  if (it->second->m_uiID == IDM_MI_MET_INJTIMB || it->second->m_uiID == IDM_MI_MET_INJTIME)
   (static_cast<CMIInjTim*>(it->second))->SetITMode(m_it_mode);
  if (it->second->m_uiID == IDM_MI_GRH_INJTIMB || it->second->m_uiID == IDM_MI_GRH_INJTIME)
   (static_cast<CMIInjTimGraph*>(it->second))->SetITMode(m_it_mode);
 }
 Invalidate();
}

void CMIDeskDlg::Reset(void)
{
 //Reset graphs
 MetFields_t::iterator it;
 for(it = m_metFields.begin(); it != m_metFields.end(); ++it)
  it->second->Reset();
}

void CMIDeskDlg::ShowGraphCursor(bool show)
{
 m_show_graph_cursor = show;
}

int CMIDeskDlg::GetGraphSamplesNum(void)
{
 size_t samples = 0;
 for(MetFields_t::iterator it = m_metFields.begin(); it != m_metFields.end(); ++it)
 { //find a maximum value
  if (it->second->isGraph())
  {
   size_t ptcnt = it->second->GetMaxPtCount();
   if (ptcnt > samples)
    samples = ptcnt;
  }
 }
 return samples;
}

void CMIDeskDlg::SetGraphShtPixels(int n)
{
 m_graphShtPixels = n;
}

void CMIDeskDlg::SetGraphShowValue(bool show)
{
 m_graphShowValue = show;
}

void CMIDeskDlg::SetGraphValueHeight(int height)
{
 m_graphValueHeight = height;
}
