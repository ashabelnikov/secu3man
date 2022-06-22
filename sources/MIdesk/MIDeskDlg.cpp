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
#include "ui-core/AnalogMeterCtrl.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/ColorDialogEx.h"

#include "MIAirFlow.h"
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
#include "ContextMenuManager.h"

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
 ON_COMMAND_RANGE(IDM_MI_MET_RPM, IDM_MI_MET_MAPDOT, OnMetAddGauge)
 ON_COMMAND_RANGE(IDM_MI_GRH_RPM, IDM_MI_GRH_MAPDOT, OnMetAddGauge)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_MET_RPM, IDM_MI_MET_MAPDOT, OnUpdateMetAddGauge)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_GRH_RPM, IDM_MI_GRH_MAPDOT, OnUpdateMetAddGauge)
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
 ON_COMMAND_RANGE(IDM_MI_IND_GAS_V, IDM_MI_IND_UNIOUT6, OnIndAddIndicator)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_MI_IND_GAS_V, IDM_MI_IND_UNIOUT6, OnUpdateIndAddIndicator)
 ON_COMMAND(IDM_MI_IND_SET_COLOR, OnIndSetColor)
 ON_UPDATE_COMMAND_UI(IDM_MI_IND_SET_COLOR, OnUpdateIndSetColor)
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
, m_tachoMax(8000)
, m_pressMax(110)
, m_tempMax(120)
, m_injpwMax(24)
{
 mp_ctxMenuMgrMet->CreateContent();
 mp_ctxMenuMgrInd->CreateContent();

 memset(&m_values, 0, sizeof(SECU3IO::SensorDat));
 m_hMoveCursor = (HCURSOR)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND_MOVE), IMAGE_CURSOR, 0, 0, 0);
 VERIFY(m_hMoveCursor);

 int r = 64;
 m_rpmQVal[0].reserve(r);
 m_rpmQVal[1].reserve(r);
 m_knockQVal[0].reserve(r);
 m_knockQVal[1].reserve(r);
 m_vbatQVal[0].reserve(r);
 m_vbatQVal[1].reserve(r);
 m_mapQVal[0].reserve(r);
 m_mapQVal[1].reserve(r);
 m_ai1QVal[0].reserve(r);
 m_ai1QVal[1].reserve(r);
 m_tpsQVal[0].reserve(r);
 m_tpsQVal[1].reserve(r);
 m_igntimQVal[0].reserve(r);
 m_igntimQVal[1].reserve(r);
 m_cltQVal[0].reserve(r);
 m_cltQVal[1].reserve(r);
 m_ai2QVal[0].reserve(r);
 m_ai2QVal[1].reserve(r);
 m_injpwQVal[0].reserve(r);
 m_injpwQVal[1].reserve(r);
 m_iatQVal[0].reserve(r);
 m_iatQVal[1].reserve(r);
 m_egocQVal[0].reserve(r);
 m_egocQVal[1].reserve(r);
 m_airflQVal[0].reserve(r);
 m_airflQVal[1].reserve(r);
 m_vssQVal[0].reserve(r);
 m_vssQVal[1].reserve(r);
 m_tpsdotQVal[0].reserve(r);
 m_tpsdotQVal[1].reserve(r);
 m_mapdotQVal[0].reserve(r);
 m_mapdotQVal[1].reserve(r);
 m_map2QVal[0].reserve(r);
 m_map2QVal[1].reserve(r);
 m_mapdQVal[0].reserve(r);
 m_mapdQVal[1].reserve(r);
 m_tmp2QVal[0].reserve(r);
 m_tmp2QVal[1].reserve(r);
 m_fuelcQVal[0].reserve(r);
 m_fuelcQVal[1].reserve(r);
 m_fuelchQVal[0].reserve(r);
// m_fuelchQVal[1].reserve(r);
 m_knkretQVal[0].reserve(r);
 m_knkretQVal[1].reserve(r);
 m_senafrQVal[0].reserve(r);
 m_senafrQVal[1].reserve(r);
 m_chposQVal[0].reserve(r);
 m_chposQVal[1].reserve(r);
 m_gdposQVal[0].reserve(r);
 m_gdposQVal[1].reserve(r);
 m_synldQVal[0].reserve(r);
 m_synldQVal[1].reserve(r);
 m_itbQVal[0].reserve(r);
 m_itbQVal[1].reserve(r);
 m_iteQVal[0].reserve(r);
 m_iteQVal[1].reserve(r);
 m_fuelcfQVal[0].reserve(r);
 m_fuelcfQVal[1].reserve(r);
 m_grtsQVal[0].reserve(r);
 m_grtsQVal[1].reserve(r);
 m_distQVal.push_back(std::make_pair(0.0f, false));
 m_ftlsQVal[0].reserve(r);
 m_ftlsQVal[1].reserve(r);
 m_egtsQVal[0].reserve(r);
 m_egtsQVal[1].reserve(r);
 m_opsQVal[0].reserve(r);
 m_opsQVal[1].reserve(r);
 m_injdutyQVal.reserve(r);
 m_mafQVal[0].reserve(r);
 m_mafQVal[1].reserve(r);
 m_ventdutyQVal.reserve(r);
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
 m_values = *i_values;
 //Transform inj. tim. values:
 m_values.inj_tim_begin = MathHelpers::InjTimValueTransform(m_it_mode, m_values.inj_tim_begin, 0);
 m_values.inj_tim_end = MathHelpers::InjTimValueTransform(m_it_mode, m_values.inj_tim_end, 0);

 m_ringRPM.Append((float)i_values->frequen);
 m_ringKnock.Append((float)i_values->knock_k);
 m_ringVBat.Append(i_values->voltage);
 m_ringMAP.Append(i_values->pressure);
 m_ringAddI1.Append(i_values->add_i1);
 m_ringTPS.Append(i_values->tps);
 m_ringIgnTim.Append(i_values->adv_angle);
 m_ringCLT.Append(i_values->temperat);
 m_ringAddI2.Append(i_values->add_i2);
 m_ringInjPW.Append(i_values->inj_pw);
 m_ringIAT.Append(i_values->air_temp);
 m_ringEGOCorr.Append(i_values->lambda_corr);
 m_ringAirFlow.Append(i_values->air_flow);
 m_ringVehicleSpeed.Append(i_values->speed);
 m_ringTPSDot.Append((float)i_values->tpsdot);
 m_ringMAPDot.Append((float)i_values->mapdot);
 m_ringMAP2.Append(i_values->map2);
 m_ringMAPD.Append(i_values->mapd);
 m_ringTmp2.Append(i_values->tmp2);
 m_ringFuelConsum.Append(i_values->inj_ffd);
 m_ringFuelConsumH.Append(i_values->inj_ffh);
 m_ringKnockRetard.Append(i_values->knock_retard);
 m_ringSensAFR.Append(i_values->afr);
 m_ringChokePos.Append(i_values->choke_pos);
 m_ringGDPos.Append(i_values->gasdose_pos);
 m_ringSynLoad.Append(i_values->load);
 m_ringInjTimB.Append(m_values.inj_tim_begin); //use transformed value
 m_ringInjTimE.Append(m_values.inj_tim_end);   //use transformed value
 m_ringFuelConsumF.Append(i_values->inj_fff);
 m_ringGrts.Append(i_values->grts);
 m_ringFtls.Append(i_values->ftls);
 m_ringEgts.Append(i_values->egts);
 m_ringOps.Append(i_values->ops);
 m_ringMAF.Append(i_values->maf);

 m_ringRPM.Calculate();
 m_ringKnock.Calculate();
 m_ringVBat.Calculate();
 m_ringMAP.Calculate();
 m_ringAddI1.Calculate();
 m_ringTPS.Calculate();
 m_ringIgnTim.Calculate();
 m_ringCLT.Calculate();
 m_ringAddI2.Calculate();
 m_ringInjPW.Calculate();
 m_ringIAT.Calculate();
 m_ringEGOCorr.Calculate();
 m_ringAirFlow.Calculate();
 m_ringVehicleSpeed.Calculate();
 m_ringTPSDot.Calculate();
 m_ringMAPDot.Calculate();
 m_ringMAP2.Calculate();
 m_ringMAPD.Calculate();
 m_ringTmp2.Calculate();
 m_ringFuelConsum.Calculate();
 m_ringFuelConsumH.Calculate();
 m_ringKnockRetard.Calculate();
 m_ringSensAFR.Calculate();
 m_ringChokePos.Calculate();
 m_ringGDPos.Calculate();
 m_ringSynLoad.Calculate();
 m_ringInjTimB.Calculate();
 m_ringInjTimE.Calculate();
 m_ringFuelConsumF.Calculate();
 m_ringGrts.Calculate();
 m_ringFtls.Calculate();
 m_ringEgts.Calculate();
 m_ringOps.Calculate();
 m_ringMAF.Calculate();

 m_rpmQVal[0].push_back(std::make_pair(m_ringRPM.m_result, i_revdir));
 m_rpmQVal[1].push_back(std::make_pair((float)i_values->frequen, i_revdir));

 m_knockQVal[0].push_back(std::make_pair(m_ringKnock.m_result, i_revdir));
 m_knockQVal[1].push_back(std::make_pair(i_values->knock_k, i_revdir));

 m_vbatQVal[0].push_back(std::make_pair(m_ringVBat.m_result, i_revdir));
 m_vbatQVal[1].push_back(std::make_pair(i_values->voltage, i_revdir));

 m_mapQVal[0].push_back(std::make_pair(m_ringMAP.m_result, i_revdir));
 m_mapQVal[1].push_back(std::make_pair(i_values->pressure, i_revdir));

 m_ai1QVal[0].push_back(std::make_pair(m_ringAddI1.m_result, i_revdir));
 m_ai1QVal[1].push_back(std::make_pair(i_values->add_i1, i_revdir));

 m_tpsQVal[0].push_back(std::make_pair(m_ringTPS.m_result, i_revdir));
 m_tpsQVal[1].push_back(std::make_pair(i_values->tps, i_revdir));

 m_igntimQVal[0].push_back(std::make_pair(m_ringIgnTim.m_result, i_revdir));
 m_igntimQVal[1].push_back(std::make_pair(i_values->adv_angle, i_revdir));

 m_cltQVal[0].push_back(std::make_pair(m_ringCLT.m_result, i_revdir));
 m_cltQVal[1].push_back(std::make_pair(i_values->temperat, i_revdir));

 m_ai2QVal[0].push_back(std::make_pair(m_ringAddI2.m_result, i_revdir));
 m_ai2QVal[1].push_back(std::make_pair(i_values->add_i2, i_revdir));

 m_injpwQVal[0].push_back(std::make_pair(m_ringInjPW.m_result, i_revdir));
 m_injpwQVal[1].push_back(std::make_pair(i_values->inj_pw, i_revdir));

 m_iatQVal[0].push_back(std::make_pair(m_ringIAT.m_result, i_revdir));
 m_iatQVal[1].push_back(std::make_pair(i_values->air_temp, i_revdir));

 m_egocQVal[0].push_back(std::make_pair(m_ringEGOCorr.m_result, i_revdir));
 m_egocQVal[1].push_back(std::make_pair(i_values->lambda_corr, i_revdir));

 m_airflQVal[0].push_back(std::make_pair(m_ringAirFlow.m_result, i_revdir));
 m_airflQVal[1].push_back(std::make_pair(i_values->air_flow, i_revdir));

 m_vssQVal[0].push_back(std::make_pair(m_ringVehicleSpeed.m_result, i_revdir));
 m_vssQVal[1].push_back(std::make_pair(i_values->speed, i_revdir));

 m_tpsdotQVal[0].push_back(std::make_pair(m_ringTPSDot.m_result, i_revdir));
 m_tpsdotQVal[1].push_back(std::make_pair((float)i_values->tpsdot, i_revdir));

 m_mapdotQVal[0].push_back(std::make_pair(m_ringMAPDot.m_result, i_revdir));
 m_mapdotQVal[1].push_back(std::make_pair((float)i_values->mapdot, i_revdir));

 m_map2QVal[0].push_back(std::make_pair(m_ringMAP2.m_result, i_revdir));
 m_map2QVal[1].push_back(std::make_pair(i_values->map2, i_revdir));

 m_mapdQVal[0].push_back(std::make_pair(m_ringMAPD.m_result, i_revdir));
 m_mapdQVal[1].push_back(std::make_pair(i_values->mapd, i_revdir));

 m_tmp2QVal[0].push_back(std::make_pair(m_ringTmp2.m_result, i_revdir));
 m_tmp2QVal[1].push_back(std::make_pair(i_values->tmp2, i_revdir));

 m_grtsQVal[0].push_back(std::make_pair(m_ringGrts.m_result, i_revdir));
 m_grtsQVal[1].push_back(std::make_pair(i_values->grts, i_revdir));

 m_fuelcQVal[0].push_back(std::make_pair(m_ringFuelConsum.m_result, i_revdir));
 m_fuelcQVal[1].push_back(std::make_pair(i_values->inj_ffd, i_revdir));

 m_fuelchQVal[0].push_back(std::make_pair(m_ringFuelConsumH.m_result, i_revdir));
// m_fuelchQVal[1].push_back(std::make_pair(i_values->inj_ffh, i_revdir));

 m_knkretQVal[0].push_back(std::make_pair(m_ringKnockRetard.m_result, i_revdir));
 m_knkretQVal[1].push_back(std::make_pair(i_values->knock_retard, i_revdir));

 m_senafrQVal[0].push_back(std::make_pair(m_ringSensAFR.m_result, i_revdir));
 m_senafrQVal[1].push_back(std::make_pair(i_values->afr, i_revdir));

 m_chposQVal[0].push_back(std::make_pair(m_ringChokePos.m_result, i_revdir));
 m_chposQVal[1].push_back(std::make_pair(i_values->choke_pos, i_revdir));

 m_gdposQVal[0].push_back(std::make_pair(m_ringGDPos.m_result, i_revdir));
 m_gdposQVal[1].push_back(std::make_pair(i_values->gasdose_pos, i_revdir));

 m_synldQVal[0].push_back(std::make_pair(m_ringSynLoad.m_result, i_revdir));
 m_synldQVal[1].push_back(std::make_pair(i_values->load, i_revdir));

 m_itbQVal[0].push_back(std::make_pair(m_ringInjTimB.m_result, i_revdir));
 m_itbQVal[1].push_back(std::make_pair(m_values.inj_tim_begin, i_revdir)); //use transformed value

 m_iteQVal[0].push_back(std::make_pair(m_ringInjTimE.m_result, i_revdir));
 m_iteQVal[1].push_back(std::make_pair(m_values.inj_tim_end, i_revdir));   //use transformed value

 m_fuelcfQVal[0].push_back(std::make_pair(m_ringFuelConsumF.m_result, i_revdir));
 m_fuelcfQVal[1].push_back(std::make_pair(i_values->inj_fff, i_revdir));

 m_distQVal[0].first = i_values->distance; //passed distance

 m_ftlsQVal[0].push_back(std::make_pair(m_ringFtls.m_result, i_revdir));
 m_ftlsQVal[1].push_back(std::make_pair(i_values->ftls, i_revdir));

 m_egtsQVal[0].push_back(std::make_pair(m_ringEgts.m_result, i_revdir));
 m_egtsQVal[1].push_back(std::make_pair(i_values->egts, i_revdir));

 m_opsQVal[0].push_back(std::make_pair(m_ringOps.m_result, i_revdir));
 m_opsQVal[1].push_back(std::make_pair(i_values->ops, i_revdir));

 m_injdutyQVal.push_back(std::make_pair(i_values->inj_duty, i_revdir)); //injector's duty

 m_mafQVal[0].push_back(std::make_pair(m_ringMAF.m_result, i_revdir));
 m_mafQVal[1].push_back(std::make_pair(i_values->maf, i_revdir));

 m_ventdutyQVal.push_back(std::make_pair(i_values->vent_duty, i_revdir)); //cooling fan's duty
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

 m_rpmQVal[0].clear(); 
 m_rpmQVal[1].clear(); 
 m_knockQVal[0].clear();
 m_knockQVal[1].clear();
 m_vbatQVal[0].clear(); 
 m_vbatQVal[1].clear(); 
 m_mapQVal[0].clear(); 
 m_mapQVal[1].clear(); 
 m_ai1QVal[0].clear(); 
 m_ai1QVal[1].clear(); 
 m_tpsQVal[0].clear(); 
 m_tpsQVal[1].clear(); 
 m_igntimQVal[0].clear(); 
 m_igntimQVal[1].clear(); 
 m_cltQVal[0].clear();
 m_cltQVal[1].clear();
 m_ai2QVal[0].clear();
 m_ai2QVal[1].clear();
 m_injpwQVal[0].clear();
 m_injpwQVal[1].clear();
 m_iatQVal[0].clear();
 m_iatQVal[1].clear();
 m_egocQVal[0].clear();
 m_egocQVal[1].clear();
 m_airflQVal[0].clear();
 m_airflQVal[1].clear();
 m_vssQVal[0].clear();
 m_vssQVal[1].clear();
 m_tpsdotQVal[0].clear();
 m_tpsdotQVal[1].clear();
 m_mapdotQVal[0].clear();
 m_mapdotQVal[1].clear();
 m_map2QVal[0].clear();
 m_map2QVal[1].clear();
 m_mapdQVal[0].clear();
 m_mapdQVal[1].clear();
 m_tmp2QVal[0].clear();
 m_tmp2QVal[1].clear();
 m_fuelcQVal[0].clear();
 m_fuelcQVal[1].clear();
 m_fuelchQVal[0].clear();
 //m_fuelchQVal[1].clear();
 m_knkretQVal[0].clear();
 m_knkretQVal[1].clear();
 m_senafrQVal[0].clear();
 m_senafrQVal[1].clear();
 m_chposQVal[0].clear();
 m_chposQVal[1].clear();
 m_gdposQVal[0].clear();
 m_gdposQVal[1].clear();
 m_synldQVal[0].clear();
 m_synldQVal[1].clear();
 m_itbQVal[0].clear();
 m_itbQVal[1].clear();
 m_iteQVal[0].clear();
 m_iteQVal[1].clear();
 m_fuelcfQVal[0].clear();
 m_fuelcfQVal[1].clear();
 m_grtsQVal[0].clear();
 m_grtsQVal[1].clear();
 m_ftlsQVal[0].clear();
 m_ftlsQVal[1].clear();
 m_egtsQVal[0].clear();
 m_egtsQVal[1].clear();
 m_opsQVal[0].clear();
 m_opsQVal[1].clear();
 m_injdutyQVal.clear(); //injector's duty
 m_mafQVal[0].clear();
 m_mafQVal[1].clear();
 m_ventdutyQVal.clear(); //cooling fan's duty
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

void CMIDeskDlg::SetTachometerMax(int i_max)
{
 m_tachoMax = i_max;
}

void CMIDeskDlg::SetPressureMax(int i_max)
{
 m_pressMax = i_max;
}

void CMIDeskDlg::SetTemperatureMax(int i_max)
{
 m_tempMax = i_max;
}

void CMIDeskDlg::SetInjPWMax(int i_max)
{
 m_injpwMax = i_max;
}

void CMIDeskDlg::SetSpeedUnit(int i_unit)
{
 m_speedUnit = (i_unit == 0) ? MLL::GetString(IDS_MI_KM_H) : MLL::GetString(IDS_MI_MP_H);
}

void CMIDeskDlg::SetDistanceUnit(int i_unit)
{
 m_distanceUnit = (i_unit == 0) ? MLL::GetString(IDS_MI_KM) : MLL::GetString(IDS_MI_MI);
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

void CMIDeskDlg::SetRPMAverageNum(int avnum)
{
 m_ringRPM.m_avnum = avnum;
}

void CMIDeskDlg::SetVoltAverageNum(int avnum)
{
 m_ringVBat.m_avnum = avnum;
}

void CMIDeskDlg::SetMAPAverageNum(int avnum)
{
 m_ringMAP.m_avnum = avnum;
}

void CMIDeskDlg::SetAI1AverageNum(int avnum)
{
 m_ringAddI1.m_avnum = avnum;
}

void CMIDeskDlg::SetTPSAverageNum(int avnum)
{
 m_ringTPS.m_avnum = avnum;
}

void CMIDeskDlg::SetKnockAverageNum(int avnum)
{
 m_ringKnock.m_avnum = avnum;
}

void CMIDeskDlg::SetIgnTimAverageNum(int avnum)
{
 m_ringIgnTim.m_avnum = avnum;
}

void CMIDeskDlg::SetCLTAverageNum(int avnum)
{
 m_ringCLT.m_avnum = avnum;
}

void CMIDeskDlg::SetAddI2AverageNum(int avnum)
{
 m_ringAddI2.m_avnum = avnum;
}

void CMIDeskDlg::SetInjPWAverageNum(int avnum)
{
 m_ringInjPW.m_avnum = avnum;
}

void CMIDeskDlg::SetIATAverageNum(int avnum)
{
 m_ringIAT.m_avnum = avnum;
}

void CMIDeskDlg::SetEGOCorrAverageNum(int avnum)
{
 m_ringEGOCorr.m_avnum = avnum;
}

void CMIDeskDlg::SetAirFlowAverageNum(int avnum)
{
 m_ringAirFlow.m_avnum = avnum;
}

void CMIDeskDlg::SetVehicleSpeedAverageNum(int avnum)
{
 m_ringVehicleSpeed.m_avnum = avnum;
}

void CMIDeskDlg::SetTPSDotAverageNum(int avnum)
{
 m_ringTPSDot.m_avnum = avnum;
}

void CMIDeskDlg::SetMAPDotAverageNum(int avnum)
{
 m_ringMAPDot.m_avnum = avnum;
}

void CMIDeskDlg::SetMAP2AverageNum(int avnum)
{
 m_ringMAP2.m_avnum = avnum;
}

void CMIDeskDlg::SetMAPDAverageNum(int avnum)
{
 m_ringMAPD.m_avnum = avnum;
}

void CMIDeskDlg::SetTmp2AverageNum(int avnum)
{
 m_ringTmp2.m_avnum = avnum;
}

void CMIDeskDlg::SetGrtsAverageNum(int avnum)
{
 m_ringGrts.m_avnum = avnum;
}

void CMIDeskDlg::SetFuelConsumAverageNum(int avnum)
{
 m_ringFuelConsum.m_avnum = avnum;
 m_ringFuelConsumH.m_avnum = avnum;
}

void CMIDeskDlg::SetFuelConsumFAverageNum(int avnum)
{
 m_ringFuelConsumF.m_avnum = avnum;
}

void CMIDeskDlg::SetKnockRetardAverageNum(int avnum)
{
 m_ringKnockRetard.m_avnum = avnum;
}

void CMIDeskDlg::SetSensAFRAverageNum(int avnum)
{
 m_ringSensAFR.m_avnum = avnum;
}

void CMIDeskDlg::SetChokePosAverageNum(int avnum)
{
 m_ringChokePos.m_avnum = avnum;
}

void CMIDeskDlg::SetGDPosAverageNum(int avnum)
{
 m_ringGDPos.m_avnum = avnum;
}

void CMIDeskDlg::SetSynLoadAverageNum(int avnum)
{
 m_ringSynLoad.m_avnum = avnum;
}

void CMIDeskDlg::SetInjTimBAverageNum(int avnum)
{
 m_ringInjTimB.m_avnum = avnum;
}

void CMIDeskDlg::SetInjTimEAverageNum(int avnum)
{
 m_ringInjTimE.m_avnum = avnum;
}

void CMIDeskDlg::SetFtlsAverageNum(int avnum)
{
 m_ringFtls.m_avnum = avnum;
}

void CMIDeskDlg::SetEgtsAverageNum(int avnum)
{
 m_ringEgts.m_avnum = avnum;
}

void CMIDeskDlg::SetOpsAverageNum(int avnum)
{
 m_ringOps.m_avnum = avnum;
}

void CMIDeskDlg::SetMAFAverageNum(int avnum)
{
 m_ringMAF.m_avnum = avnum;
}

void CMIDeskDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_was_initialized)
  _Resize();
}

void CMIDeskDlg::SetIndicatorsCfg(float IndHeingtPercent, int IndRows, IndCfg_t IndGas_v, IndCfg_t IndCarb, IndCfg_t IndIdleValve, IndCfg_t IndPowerValve, IndCfg_t IndStBlock, IndCfg_t IndAE, IndCfg_t IndCoolingFan, IndCfg_t IndCE, IndCfg_t IndFCRevLim, IndCfg_t IndFloodClear, IndCfg_t IndSysLocked, IndCfg_t IndIgn_i, IndCfg_t IndCond_i, IndCfg_t IndEpas_i, IndCfg_t IndAftStrEnr, IndCfg_t IndIacClLoop, IndCfg_t UniOut1, IndCfg_t UniOut2, IndCfg_t UniOut3, IndCfg_t UniOut4, IndCfg_t UniOut5, IndCfg_t UniOut6)
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

 m_indRows = IndRows;
 m_IndHeightCoeff = IndHeingtPercent / 100.0f;

 m_leds.SetNumRows(IndRows);
 m_indFields.clear();

 for (std::map<UINT, IndCfg_t>::iterator fi = m_indCfg.begin(); fi != m_indCfg.end(); ++fi)
  _IndFactory(fi->first);

 _CreateLEDs();
}

void CMIDeskDlg::SetMetersCfg(int MetRows, int *MetRPM, int *MetMAP, int *MetVBat, int *MetIgnTim, int *MetCLT, int *MetAddI1, int *MetAddI2,
                    int *MetInjPW, int *MetIAT, int *MetEGOCorr, int *MetTPS, int *MetAirFlow, int *MetVehicleSpeed, int *MetTPSDot, int *MetMAP2,
                    int *MetMAPD, int *MetTmp2, int *MetFuelConsum, int *MetKnockRetard, int *MetKnockGraph, int *MetSensAFR, int *MetChokePos,
                    int *MetGDPos, int *MetSynLoad, int *MetInjTimB, int *MetInjTimE, int *MetFuelConsumF, int *MetGrts, int *MetFtls, int *MetEgts,
                    int *MetOps, int *MetInjDuty, int *MetMAF, int *MetVentDuty, int *MetMAPDot, int TitleFontSize, int ValueFontSize, int PaneFontSize, int LabelFontSize)
{
 m_metCfg.clear();
 m_metCfg.insert(std::make_pair(IDM_MI_MET_RPM, MetRPM[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAP, MetMAP[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_VBAT, MetVBat[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_IGNTIM, MetIgnTim[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_CLT, MetCLT[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_ADDI1, MetAddI1[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_ADDI2, MetAddI2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJPW, MetInjPW[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_IAT, MetIAT[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_EGOCORR, MetEGOCorr[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TPS, MetTPS[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_AIRFLOW, MetAirFlow[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_VEHSPD, MetVehicleSpeed[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TPSDOT, MetTPSDot[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAPDOT, MetMAPDot[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAP2, MetMAP2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAPD, MetMAPD[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_TMP2, MetTmp2[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FUELCONSUM, MetFuelConsum[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_KNOCKRETARD, MetKnockRetard[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_KNOCKGRAPH, MetKnockGraph[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_SENSAFR, MetSensAFR[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_CHOKEPOS, MetChokePos[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_GDPOS, MetGDPos[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_SYNLOAD, MetSynLoad[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJTIMB, MetInjTimB[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJTIME, MetInjTimE[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FUELCONSUMF, MetFuelConsumF[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_GRTS, MetGrts[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_FTLS, MetFtls[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_EGTS, MetEgts[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_OPS, MetOps[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_INJDUTY, MetInjDuty[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_MAF, MetMAF[0]));
 m_metCfg.insert(std::make_pair(IDM_MI_MET_VENTDUTY, MetVentDuty[0]));

 //graphs
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_RPM, MetRPM[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAP, MetMAP[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_VBAT, MetVBat[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_IGNTIM, MetIgnTim[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_CLT, MetCLT[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_ADDI1, MetAddI1[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_ADDI2, MetAddI2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_INJPW, MetInjPW[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_IAT, MetIAT[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_EGOCORR, MetEGOCorr[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_TPS, MetTPS[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_AIRFLOW, MetAirFlow[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_VEHSPD, MetVehicleSpeed[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_TPSDOT, MetTPSDot[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAPDOT, MetMAPDot[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAP2, MetMAP2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAPD, MetMAPD[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_TMP2, MetTmp2[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_FUELCONSUM, MetFuelConsum[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_KNOCKRETARD, MetKnockRetard[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_KNOCKGRAPH, MetKnockGraph[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_SENSAFR, MetSensAFR[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_CHOKEPOS, MetChokePos[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_GDPOS, MetGDPos[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_SYNLOAD, MetSynLoad[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_INJTIMB, MetInjTimB[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_INJTIME, MetInjTimE[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_FUELCONSUMF, MetFuelConsumF[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_GRTS, MetGrts[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_FTLS, MetFtls[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_EGTS, MetEgts[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_OPS, MetOps[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_INJDUTY, MetInjDuty[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_MAF, MetMAF[1]));
 m_metCfg.insert(std::make_pair(IDM_MI_GRH_VENTDUTY, MetVentDuty[1]));

 m_metRows = MetRows;
 m_TitleFontSize = TitleFontSize;
 m_ValueFontSize = ValueFontSize;
 m_PaneFontSize = PaneFontSize;
 m_LabelFontSize = LabelFontSize;

 //remember color to have ability to detect changing of system colors (see OnPaint)
 m_COLOR_BTNFACE = GetSysColor(COLOR_BTNFACE);

 //destroy current widgets and clear list
 _MetCleanUp();

 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
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
  m_metCfg[it->second->m_uiID] = i++;
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
     m_metCfg[m_dragItemMet->second->m_uiID] = m_dragItemMet->first;
     m_metCfg[it->second->m_uiID] = it->first; 

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

void CMIDeskDlg::GetMetersCfg(int &MetRows, int *MetRPM, int *MetMAP, int *MetVBat, int *MetIgnTim, int *MetCLT, int *MetAddI1, int *MetAddI2,
                    int *MetInjPW, int *MetIAT, int *MetEGOCorr, int *MetTPS, int *MetAirFlow, int *MetVehicleSpeed, int *MetTPSDot, int *MetMAP2,
                    int *MetMapD, int *MetTmp2, int *MetFuelConsum, int *MetKnockRetard, int *MetKnockGraph, int *MetSensAFR, int *MetChokePos,
                    int *MetGDPos, int *MetSynLoad, int *MetInjTimB, int *MetInjTimE, int *MetFuelConsumF, int *MetGrts, int *MetFtls, int *MetEgts,
                    int *MetOps, int *MetInjDuty, int *MetMAF, int *MetVentDuty, int *MetMAPDot, int &TitleFontSize, int &ValueFontSize, int &PaneFontSize, int &LabelFontSize)
{

 MetRows = m_metRows;
 TitleFontSize = m_TitleFontSize, ValueFontSize = m_ValueFontSize, PaneFontSize = m_PaneFontSize, LabelFontSize = m_LabelFontSize;

 MetRPM[0] = m_metCfg[IDM_MI_MET_RPM];
 MetMAP[0] = m_metCfg[IDM_MI_MET_MAP];
 MetVBat[0] = m_metCfg[IDM_MI_MET_VBAT];
 MetIgnTim[0] = m_metCfg[IDM_MI_MET_IGNTIM];
 MetCLT[0] = m_metCfg[IDM_MI_MET_CLT];
 MetAddI1[0] = m_metCfg[IDM_MI_MET_ADDI1];
 MetAddI2[0] = m_metCfg[IDM_MI_MET_ADDI2];
 MetInjPW[0] = m_metCfg[IDM_MI_MET_INJPW];
 MetIAT[0] = m_metCfg[IDM_MI_MET_IAT];
 MetEGOCorr[0] = m_metCfg[IDM_MI_MET_EGOCORR];
 MetTPS[0] = m_metCfg[IDM_MI_MET_TPS];
 MetAirFlow[0] = m_metCfg[IDM_MI_MET_AIRFLOW];
 MetVehicleSpeed[0] = m_metCfg[IDM_MI_MET_VEHSPD];
 MetTPSDot[0] = m_metCfg[IDM_MI_MET_TPSDOT];
 MetMAPDot[0] = m_metCfg[IDM_MI_MET_MAPDOT];
 MetMAP2[0] = m_metCfg[IDM_MI_MET_MAP2];
 MetMapD[0] = m_metCfg[IDM_MI_MET_MAPD];
 MetTmp2[0] = m_metCfg[IDM_MI_MET_TMP2];
 MetFuelConsum[0] = m_metCfg[IDM_MI_MET_FUELCONSUM];
 MetKnockRetard[0] = m_metCfg[IDM_MI_MET_KNOCKRETARD];
 MetKnockGraph[0] = m_metCfg[IDM_MI_MET_KNOCKGRAPH];
 MetSensAFR[0] = m_metCfg[IDM_MI_MET_SENSAFR];
 MetChokePos[0] = m_metCfg[IDM_MI_MET_CHOKEPOS];
 MetGDPos[0] = m_metCfg[IDM_MI_MET_GDPOS];
 MetSynLoad[0] = m_metCfg[IDM_MI_MET_SYNLOAD];
 MetInjTimB[0] = m_metCfg[IDM_MI_MET_INJTIMB];
 MetInjTimE[0] = m_metCfg[IDM_MI_MET_INJTIME];
 MetFuelConsumF[0] = m_metCfg[IDM_MI_MET_FUELCONSUMF];
 MetGrts[0] = m_metCfg[IDM_MI_MET_GRTS];
 MetFtls[0] = m_metCfg[IDM_MI_MET_FTLS];
 MetEgts[0] = m_metCfg[IDM_MI_MET_EGTS];
 MetOps[0] = m_metCfg[IDM_MI_MET_OPS];
 MetInjDuty[0] = m_metCfg[IDM_MI_MET_INJDUTY];
 MetMAF[0] = m_metCfg[IDM_MI_MET_MAF];
 MetVentDuty[0] = m_metCfg[IDM_MI_MET_VENTDUTY];

 MetRPM[1] = m_metCfg[IDM_MI_GRH_RPM];
 MetMAP[1] = m_metCfg[IDM_MI_GRH_MAP];
 MetVBat[1] = m_metCfg[IDM_MI_GRH_VBAT];
 MetIgnTim[1] = m_metCfg[IDM_MI_GRH_IGNTIM];
 MetCLT[1] = m_metCfg[IDM_MI_GRH_CLT];
 MetAddI1[1] = m_metCfg[IDM_MI_GRH_ADDI1];
 MetAddI2[1] = m_metCfg[IDM_MI_GRH_ADDI2];
 MetInjPW[1] = m_metCfg[IDM_MI_GRH_INJPW];
 MetIAT[1] = m_metCfg[IDM_MI_GRH_IAT];
 MetEGOCorr[1] = m_metCfg[IDM_MI_GRH_EGOCORR];
 MetTPS[1] = m_metCfg[IDM_MI_GRH_TPS];
 MetAirFlow[1] = m_metCfg[IDM_MI_GRH_AIRFLOW];
 MetVehicleSpeed[1] = m_metCfg[IDM_MI_GRH_VEHSPD];
 MetTPSDot[1] = m_metCfg[IDM_MI_GRH_TPSDOT];
 MetMAPDot[1] = m_metCfg[IDM_MI_GRH_MAPDOT];
 MetMAP2[1] = m_metCfg[IDM_MI_GRH_MAP2];
 MetMapD[1] = m_metCfg[IDM_MI_GRH_MAPD];
 MetTmp2[1] = m_metCfg[IDM_MI_GRH_TMP2];
 MetFuelConsum[1] = m_metCfg[IDM_MI_GRH_FUELCONSUM];
 MetKnockRetard[1] = m_metCfg[IDM_MI_GRH_KNOCKRETARD];
 MetKnockGraph[1] = m_metCfg[IDM_MI_GRH_KNOCKGRAPH];
 MetSensAFR[1] = m_metCfg[IDM_MI_GRH_SENSAFR];
 MetChokePos[1] = m_metCfg[IDM_MI_GRH_CHOKEPOS];
 MetGDPos[1] = m_metCfg[IDM_MI_GRH_GDPOS];
 MetSynLoad[1] = m_metCfg[IDM_MI_GRH_SYNLOAD];
 MetInjTimB[1] = m_metCfg[IDM_MI_GRH_INJTIMB];
 MetInjTimE[1] = m_metCfg[IDM_MI_GRH_INJTIME];
 MetFuelConsumF[1] = m_metCfg[IDM_MI_GRH_FUELCONSUMF];
 MetGrts[1] = m_metCfg[IDM_MI_GRH_GRTS];
 MetFtls[1] = m_metCfg[IDM_MI_GRH_FTLS];
 MetEgts[1] = m_metCfg[IDM_MI_GRH_EGTS];
 MetOps[1] = m_metCfg[IDM_MI_GRH_OPS];
 MetInjDuty[1] = m_metCfg[IDM_MI_GRH_INJDUTY];
 MetMAF[1] = m_metCfg[IDM_MI_GRH_MAF];
 MetVentDuty[1] = m_metCfg[IDM_MI_GRH_VENTDUTY];
}

void CMIDeskDlg::GetIndicatorsCfg(float &IndHeingtPercent, int &IndRows, IndCfg_t &IndGas_v, IndCfg_t &IndCarb, IndCfg_t &IndIdleValve, IndCfg_t &IndPowerValve, IndCfg_t &IndStBlock, IndCfg_t &IndAE,
                      IndCfg_t &IndCoolingFan, IndCfg_t &IndCE, IndCfg_t &IndFCRevLim, IndCfg_t &IndFloodClear, IndCfg_t &IndSysLocked, IndCfg_t &IndIgn_i, IndCfg_t &IndCond_i, IndCfg_t &IndEpas_i, IndCfg_t &IndAftStrEnr, IndCfg_t &IndIacClLoop, IndCfg_t &UniOut1, IndCfg_t &UniOut2, IndCfg_t &UniOut3, IndCfg_t &UniOut4, IndCfg_t &UniOut5, IndCfg_t &UniOut6)
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
 m_metCfg[m_dragItemMet->second->m_uiID] = std::numeric_limits<int>::max(); 
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

void CMIDeskDlg::OnMetAddGauge(UINT nID)
{
 if (m_dragItemMet == m_metFields.end())
  m_metCfg[nID] = m_metFields.size() ? m_metFields.rbegin()->first : 0;
 else
  m_metCfg[nID] = m_dragItemMet->first;

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
 bool already_add = m_metCfg[pCmdUI->m_nID] != std::numeric_limits<int>::max();
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
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITachometer* widget = new CMITachometer();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_rpmQVal[0], &m_vssQVal[0], &m_distQVal);
   widget->ShowTLP(m_showSpeedAndDistance);
   widget->ShowTRP(m_showSpeedAndDistance);
   widget->SetPaneUnit(m_speedUnit, m_distanceUnit);
   widget->SetLimits(0, (float)m_tachoMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_RPM:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITachometerGraph* widget = new CMITachometerGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_rpmQVal[1], NULL, NULL);
   widget->SetLimits(0, (float)m_tachoMax);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAP:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIPressure* widget = new CMIPressure();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_mapQVal[0], NULL, NULL);
   widget->SetLimits(10, (float)m_pressMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAP:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIPressureGraph* widget = new CMIPressureGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_mapQVal[1], NULL, NULL);
   widget->SetLimits(0, (float)m_pressMax);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_VBAT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIVoltmeter* widget = new CMIVoltmeter();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_vbatQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_VBAT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIVoltmeterGraph* widget = new CMIVoltmeterGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_vbatQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_IGNTIM:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIDwellAngle* widget = new CMIDwellAngle();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_igntimQVal[0], NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_IGNTIM:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIDwellAngleGraph* widget = new CMIDwellAngleGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_igntimQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_CLT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
   {
    mp_miTemperat = NULL;
    break;
   }
   CMITemperature* widget = new CMITemperature();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_cltQVal[0], &m_gdposQVal[0], &m_chposQVal[0]);  
   widget->ShowTLP(m_showGDPos);
   widget->ShowTRP(m_showChokePos);
   widget->SetLimits(-40.0, (float)m_tempMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   mp_miTemperat = widget;
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_CLT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITemperatureGraph* widget = new CMITemperatureGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_cltQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(-40.0, (float)m_tempMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_ADDI1:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAddI1* widget = new CMIAddI1();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ai1QVal[0], NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_ADDI1:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAddI1Graph* widget = new CMIAddI1Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_ai1QVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_ADDI2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAddI2* widget = new CMIAddI2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ai2QVal[0], NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_ADDI2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAddI2Graph* widget = new CMIAddI2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_ai2QVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_INJPW:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjPW* widget = new CMIInjPW();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_injpwQVal[0], NULL, NULL);  
   widget->SetLimits(0, (float)m_injpwMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_INJPW:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjPWGraph* widget = new CMIInjPWGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_injpwQVal[1], NULL, NULL);  
   widget->SetLimits(0, (float)m_injpwMax);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_IAT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIIAT* widget = new CMIIAT();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_iatQVal[0], NULL, NULL);  
   widget->SetLimits(-40.0, (float)m_tempMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_IAT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIIATGraph* widget = new CMIIATGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_iatQVal[1], NULL, NULL);  
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(-40.0, (float)m_tempMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_EGOCORR:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIEGOCorr* widget = new CMIEGOCorr();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_egocQVal[0], NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_EGOCORR:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIEGOCorrGraph* widget = new CMIEGOCorrGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_egocQVal[1], NULL, NULL);  
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_TPS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIThrottleGate* widget = new CMIThrottleGate();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_tpsQVal[0], &m_airflQVal[0], NULL);
   widget->ShowTLP(true);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_TPS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIThrottleGateGraph* widget = new CMIThrottleGateGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_tpsQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_AIRFLOW:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAirFlow* widget = new CMIAirFlow();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_airflQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_AIRFLOW:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIAirFlowGraph* widget = new CMIAirFlowGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_airflQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_VEHSPD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIVehicleSpeed* widget = new CMIVehicleSpeed();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_vssQVal[0], NULL, NULL);
   widget->SetMeterUnit(m_speedUnit);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_VEHSPD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIVehicleSpeedGraph* widget = new CMIVehicleSpeedGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_vssQVal[1], NULL, NULL);
   widget->SetMeterUnit(m_speedUnit);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_TPSDOT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITPSDot* widget = new CMITPSDot();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_tpsdotQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_TPSDOT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITPSDotGraph* widget = new CMITPSDotGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_tpsdotQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAPDOT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMAPDot* widget = new CMIMAPDot();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_mapdotQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAPDOT:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMAPDotGraph* widget = new CMIMAPDotGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_mapdotQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAP2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMap2* widget = new CMIMap2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_map2QVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAP2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMap2Graph* widget = new CMIMap2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_map2QVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAPD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMapD* widget = new CMIMapD();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_mapdQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAPD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMapDGraph* widget = new CMIMapDGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_mapdQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_TMP2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITemp2* widget = new CMITemp2();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_tmp2QVal[0], NULL, NULL);
   widget->SetLimits(-40.0, (float)m_tempMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_TMP2:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMITemp2Graph* widget = new CMITemp2Graph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_tmp2QVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(-40.0, (float)m_tempMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_GRTS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIGrts* widget = new CMIGrts();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_grtsQVal[0], NULL, NULL);
   widget->SetLimits(-40.0, (float)m_tempMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_GRTS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIGrtsGraph* widget = new CMIGrtsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_grtsQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(-40.0, (float)m_tempMax);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_FUELCONSUM:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIFuelConsum* widget = new CMIFuelConsum();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_fuelcQVal[0], &m_fuelchQVal[0], NULL);
   widget->ShowTLP(true);
   widget->SetPaneUnit(MLL::GetString(IDS_MI_FUELCONSUMH_UNIT), _T(""));
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_FUELCONSUM:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIFuelConsumGraph* widget = new CMIFuelConsumGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_fuelcQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_KNOCKRETARD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIKnockRetard* widget = new CMIKnockRetard();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_knkretQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_KNOCKRETARD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIKnockRetardGraph* widget = new CMIKnockRetardGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_knkretQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_KNOCKGRAPH:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIKnock* widget = new CMIKnock();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_knockQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_KNOCKGRAPH:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIKnockGraph* widget = new CMIKnockGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_knockQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_SENSAFR:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMISensAFR* widget = new CMISensAFR();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_senafrQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_SENSAFR:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMISensAFRGraph* widget = new CMISensAFRGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_senafrQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_CHOKEPOS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIChokePos* widget = new CMIChokePos();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_chposQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_CHOKEPOS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIChokePosGraph* widget = new CMIChokePosGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_chposQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_GDPOS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIGDPos* widget = new CMIGDPos();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_gdposQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_GDPOS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIGDPosGraph* widget = new CMIGDPosGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_gdposQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_SYNLOAD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMISynLoad* widget = new CMISynLoad();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_synldQVal[0], NULL, NULL);  
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_SYNLOAD:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMISynLoadGraph* widget = new CMISynLoadGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_synldQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_INJTIMB:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjTimB* widget = new CMIInjTimB();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_itbQVal[0], NULL, NULL);  
   widget->SetITMode(m_it_mode);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_INJTIMB:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjTimBGraph* widget = new CMIInjTimBGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_itbQVal[1], NULL, NULL);
   widget->SetITMode(m_it_mode);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_INJTIME:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjTimE* widget = new CMIInjTimE();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_iteQVal[0], NULL, NULL);  
   widget->SetITMode(m_it_mode);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_INJTIME:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjTimEGraph* widget = new CMIInjTimEGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_iteQVal[1], NULL, NULL);
   widget->SetITMode(m_it_mode);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_FUELCONSUMF:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIFuelConsumF* widget = new CMIFuelConsumF();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_fuelcfQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_FUELCONSUMF:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIFuelConsumFGraph* widget = new CMIFuelConsumFGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_fuelcfQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_FTLS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIFtls* widget = new CMIFtls();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ftlsQVal[0], NULL, NULL);
   widget->SetLimits(0.0, 100.0);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_FTLS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIFtlsGraph* widget = new CMIFtlsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_ftlsQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(0.0, 100.0);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_EGTS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIEgts* widget = new CMIEgts();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_egtsQVal[0], NULL, NULL);
   widget->SetLimits(0.0, 1100.0);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_EGTS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIEgtsGraph* widget = new CMIEgtsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_egtsQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(0.0, 1100.0);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_OPS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIOps* widget = new CMIOps();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_opsQVal[0], NULL, NULL);
   widget->SetLimits(0.0, 6.0);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_OPS:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIOpsGraph* widget = new CMIOpsGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_opsQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   widget->SetLimits(0.0, 6.0);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_INJDUTY:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjDuty* widget = new CMIInjDuty();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_injdutyQVal, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_INJDUTY:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIInjDutyGraph* widget = new CMIInjDutyGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_injdutyQVal, NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_MAF:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMaf* widget = new CMIMaf();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_mafQVal[0], NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_MAF:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIMafGraph* widget = new CMIMafGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_mafQVal[1], NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_MET_VENTDUTY:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIVentDuty* widget = new CMIVentDuty();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->Create(this);
   widget->BindVars(&m_ventdutyQVal, NULL, NULL);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
   new_widget = widget;
   break;
  }
  case IDM_MI_GRH_VENTDUTY:
  {
   if (m_metCfg[uiID] == std::numeric_limits<int>::max())
    break;
   CMIVentDutyGraph* widget = new CMIVentDutyGraph();
   widget->m_uiID = uiID;
   widget->SetFontSize(TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);
   widget->ShowCursor(m_show_graph_cursor);
   widget->ShowValue(m_graphShowValue);
   widget->SetValueHeight(m_graphValueHeight);
   widget->Create(this);
   widget->BindVars(&m_ventdutyQVal, NULL, NULL);
   widget->SetShtPixels(m_graphShtPixels);
   m_metFields.insert(std::make_pair(m_metCfg[uiID], widget));
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
 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
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
 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
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
 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
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
 for (std::map<UINT, int>::iterator fi = m_metCfg.begin(); fi != m_metCfg.end(); ++fi)
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
 }
}

void CMIDeskDlg::OnIndSetColor()
{
 CColorDialogEx dlg; 
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
