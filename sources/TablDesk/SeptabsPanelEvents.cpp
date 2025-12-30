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

/** \file SeptabsPanelEvents.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "common/MathHelpers.h"
#include "DLLLinkedFunctions.h"
#include "io-core/secu3io.h"
#include "io-core/MapIds.h"
#include "SeptabsPanel.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ToolTipCtrlEx.h"

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeSettingsCME(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
 {
  if (_this->m_md[i].state)
  {
   if (_this->Is3DMap(i))  
    _this->m_md[i].ptMovStep = DLL::Chart3DGetPtMovingStep(_this->m_md[i].handle);
   else
    _this->m_md[i].ptMovStep = DLL::Chart2DGetPtMovingStep(_this->m_md[i].handle);
  }
 }

 if (_this->m_OnChangeSettings)
  _this->m_OnChangeSettings();
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeAttenuatorTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_ATTENUATOR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseAttenuatorTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_ATTENUATOR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_ATTENUATOR].handle, ETMT_ATTENUATOR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeDwellCntrlTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_DWELLCNTRL);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseDwellCntrlTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_DWELLCNTRL].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_DWELLCNTRL].handle, ETMT_DWELLCNTRL);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeCTSCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_CTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseCTSCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_CTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_CTS_CURVE].handle, ETMT_CTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeATSCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_ATS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseATSCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_ATS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_ATS_CURVE].handle, ETMT_ATS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeATSAACTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_ATS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseATSAACTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_ATS_CORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_ATS_CORR].handle, ETMT_ATS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnGetAttenuatorYAxisLabel(LPTSTR io_label_string, int index, void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }

 float value = (float)_tcstod(io_label_string, NULL);
 int i = MathHelpers::Round(value);
 if (i >= 0 && i < SECU3IO::ATTENUATOR_LEVELS_SIZE)
  _stprintf(io_label_string, _T("%0.3f"), SECU3IO::hip9011_attenuator_gains[i]);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeGasdoseTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_GASDOSE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseGasdoseTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_GASDOSE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_GASDOSE].handle, ETMT_GASDOSE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeBarocorrTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_BAROCORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseBarocorrTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_BAROCORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_BAROCORR].handle, ETMT_BAROCORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeManIgntimTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_MANIGNTIM);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseManIgntimTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_MANIGNTIM].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_MANIGNTIM].handle, ETMT_MANIGNTIM);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeTmp2CurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_TMP2_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseTmp2CurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_TMP2_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_TMP2_CURVE].handle, ETMT_TMP2_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeCrkTempTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_CRKCLT_CORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseCrkTempTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_CRKCLT_CORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_CRKCLT_CORR].handle, ETMT_CRKCLT_CORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeEHPauseTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_EH_PAUSE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseEHPauseTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_EH_PAUSE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_EH_PAUSE].handle, ETMT_EH_PAUSE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationAttenuatorTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_ATTENUATOR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationDwellCntrlTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_DWELLCNTRL].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationCTSCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_CTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationATSCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_ATS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationATSAACTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_ATS_CORR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationGasdoseTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_GASDOSE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationBarocorrTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_BAROCORR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationManIgntimTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_MANIGNTIM].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationTmp2CurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_TMP2_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationCrkTempTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_CRKCLT_CORR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationEHPauseTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_EH_PAUSE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeCrankingThrdMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_CRANKING_THRD);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseCrankingThrdMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_CRANKING_THRD].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_CRANKING_THRD].handle, ETMT_CRANKING_THRD);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationCrankingThrdMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_CRANKING_THRD].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeCrankingTimeMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_CRANKING_TIME);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseCrankingTimeMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_CRANKING_TIME].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_CRANKING_TIME].handle, ETMT_CRANKING_TIME);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationCrankingTimeMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_CRANKING_TIME].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeSmapabanThrdMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_SMAPABAN_THRD);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseSmapabanThrdMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_SMAPABAN_THRD].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_SMAPABAN_THRD].handle, ETMT_SMAPABAN_THRD);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationSmapabanThrdMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_SMAPABAN_THRD].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeKnockZoneMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_KNOCK_ZONE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseKnockZoneMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_KNOCK_ZONE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_KNOCK_ZONE].handle, ETMT_KNOCK_ZONE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationKnockZoneMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_KNOCK_ZONE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeGrtsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_GRTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseGrtsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_GRTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_GRTS_CURVE].handle, ETMT_GRTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationGrtsCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_GRTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeGrHeatDutyTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(ETMT_GRHEAT_DUTY);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseGrHeatDutyTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_GRHEAT_DUTY].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_GRHEAT_DUTY].handle, ETMT_GRHEAT_DUTY);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationGrHeatDutyTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_GRHEAT_DUTY].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangePwmIacUCoefMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_PWMIAC_UCOEF);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnClosePwmIacUCoefMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_PWMIAC_UCOEF].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_PWMIAC_UCOEF].handle, ETMT_PWMIAC_UCOEF);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationPwmIacUCoefMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_PWMIAC_UCOEF].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeAftstrStrk0Map(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_AFTSTR_STRK0);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseAftstrStrk0Map(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_AFTSTR_STRK0].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_AFTSTR_STRK0].handle, ETMT_AFTSTR_STRK0);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationAftstrStrk0Map(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_AFTSTR_STRK0].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeAftstrStrk1Map(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_AFTSTR_STRK1);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseAftstrStrk1Map(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_AFTSTR_STRK1].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_AFTSTR_STRK1].handle, ETMT_AFTSTR_STRK1);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationAftstrStrk1Map(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_AFTSTR_STRK1].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeGrValDelMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_GRVDELAY);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseGrValDelMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_GRVDELAY].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_GRVDELAY].handle, ETMT_GRVDELAY);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationGrValDelMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_GRVDELAY].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeFtlsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_FTLS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseFtlsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_FTLS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_FTLS_CURVE].handle, ETMT_FTLS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationFtlsCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_FTLS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeEgtsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_EGTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseEgtsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_EGTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_EGTS_CURVE].handle, ETMT_EGTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationEgtsCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_EGTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeOpsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_OPS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseOpsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_OPS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_OPS_CURVE].handle, ETMT_OPS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationOpsCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_OPS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeManInjPwcTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_MANINJPWC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseManInjPwcTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_MANINJPWC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_MANINJPWC].handle, ETMT_MANINJPWC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationManInjPwcTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_MANINJPWC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeMAFCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_MAF_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseMAFCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_MAF_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_MAF_CURVE].handle, ETMT_MAF_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationMAFCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_MAF_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeFtlsCorTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_FTLSCOR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseFtlsCorTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_FTLSCOR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_FTLSCOR].handle, ETMT_FTLSCOR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationFtlsCorTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_FTLSCOR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeLambdaZoneMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_LAMBDA_ZONE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseLambdaZoneMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_LAMBDA_ZONE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_LAMBDA_ZONE].handle, ETMT_LAMBDA_ZONE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationLambdaZoneMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_LAMBDA_ZONE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeFtsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_FTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseFtsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_FTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_FTS_CURVE].handle, ETMT_FTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationFtsCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_FTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeFuelDensCorrTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_FUELDENS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseFuelDensCorrTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_FUELDENS_CORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_FUELDENS_CORR].handle, ETMT_FUELDENS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationFuelDensCorrTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_FUELDENS_CORR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeXtauXfAccMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_XTAU_XFACC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseXtauXfAccMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_XTAU_XFACC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_XTAU_XFACC].handle, ETMT_XTAU_XFACC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationXtauXfAccMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_XTAU_XFACC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeXtauXfDecMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_XTAU_XFDEC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseXtauXfDecMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_XTAU_XFDEC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_XTAU_XFDEC].handle, ETMT_XTAU_XFDEC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationXtauXfDecMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_XTAU_XFDEC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeXtauTfAccMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_XTAU_TFACC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseXtauTfAccMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_XTAU_TFACC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_XTAU_TFACC].handle, ETMT_XTAU_TFACC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationXtauTfAccMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_XTAU_TFACC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeXtauTfDecMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_XTAU_TFDEC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseXtauTfDecMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_XTAU_TFDEC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_XTAU_TFDEC].handle, ETMT_XTAU_TFDEC);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationXtauTfDecMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_XTAU_TFDEC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeInjNonLinPTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_INJNONLINP);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseInjNonLinPTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_INJNONLINP].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJNONLINP].handle, ETMT_INJNONLINP);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationInjNonLinPTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_INJNONLINP].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeInjNonLinGTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_INJNONLING);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseInjNonLinGTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_INJNONLING].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJNONLING].handle, ETMT_INJNONLING);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationInjNonLinGTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_INJNONLING].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeEGODelayMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_EGO_DELAY);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseEGODelayMap(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_EGO_DELAY].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_EGO_DELAY].handle, ETMT_EGO_DELAY);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationEGODelayMap(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_EGO_DELAY].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeWUAFR0Map(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_WU_AFR0);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseWUAFR0Map(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_WU_AFR0].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_WU_AFR0].handle, ETMT_WU_AFR0);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationWUAFR0Map(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_WU_AFR0].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeWUAFR1Map(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_WU_AFR1);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseWUAFR1Map(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_WU_AFR1].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_WU_AFR1].handle, ETMT_WU_AFR1);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationWUAFR1Map(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_WU_AFR1].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeETCSprPrelTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_ETC_SPRPREL);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseETCSprPrelTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_ETC_SPRPREL].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_ETC_SPRPREL].handle, ETMT_ETC_SPRPREL);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationETCSprPrelTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_ETC_SPRPREL].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeETCAcceptErrTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_ETC_ACCEERR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseETCAcceptErrTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_ETC_ACCEERR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_ETC_ACCEERR].handle, ETMT_ETC_ACCEERR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationETCAcceptErrTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_ETC_ACCEERR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeETCThrottlePosTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_ETC_THROPOS);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseETCThrottlePosTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_ETC_THROPOS].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_ETC_THROPOS].handle, ETMT_ETC_THROPOS);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationETCThrottlePosTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_ETC_THROPOS].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeOtsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_OTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnCloseOtsCurveTable(void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[ETMT_OTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_OTS_CURVE].handle, ETMT_OTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnWndActivationOtsCurveTable(void* i_param, long cmd)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[ETMT_OTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
