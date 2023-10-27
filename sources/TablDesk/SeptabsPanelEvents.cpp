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
#include "MapIds.h"
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

 for(int i = TYPE_MAP_SEP_START; i <= TYPE_MAP_SEP_END; ++i)
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
   _this->m_OnMapChanged(TYPE_MAP_ATTENUATOR);
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
 _this->m_md[TYPE_MAP_ATTENUATOR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_ATTENUATOR].handle, TYPE_MAP_ATTENUATOR);
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
   _this->m_OnMapChanged(TYPE_MAP_DWELLCNTRL);
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
 _this->m_md[TYPE_MAP_DWELLCNTRL].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_DWELLCNTRL].handle, TYPE_MAP_DWELLCNTRL);
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
   _this->m_OnMapChanged(TYPE_MAP_CTS_CURVE);
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
 _this->m_md[TYPE_MAP_CTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_CTS_CURVE].handle, TYPE_MAP_CTS_CURVE);
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
   _this->m_OnMapChanged(TYPE_MAP_ATS_CURVE);
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
 _this->m_md[TYPE_MAP_ATS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_ATS_CURVE].handle, TYPE_MAP_ATS_CURVE);
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
   _this->m_OnMapChanged(TYPE_MAP_ATS_CORR);
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
 _this->m_md[TYPE_MAP_ATS_CORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_ATS_CORR].handle, TYPE_MAP_ATS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnGetYAxisLabel(LPTSTR io_label_string, int index, void* i_param)
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
void __cdecl CSeptabsPanel::OnGetXAxisLabel(LPTSTR io_label_string, int index, void* i_param)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }
 _stprintf(io_label_string, _T("")); //empty string
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeCTSXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 _this->m_cts_curve_x_axis_limits[i_type] = i_value;

 //allow controller to detect changes
 if (_this->m_OnCTSXAxisEditChanged)
  _this->m_OnCTSXAxisEditChanged(i_type, i_value);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeATSXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 
 _this->m_ats_curve_x_axis_limits[i_type] = i_value;

 //allow controller to detect changes
 if (_this->m_OnATSXAxisEditChanged)
  _this->m_OnATSXAxisEditChanged(i_type, i_value);
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
  _this->m_OnMapChanged(TYPE_MAP_GASDOSE);
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
 _this->m_md[TYPE_MAP_GASDOSE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_GASDOSE].handle, TYPE_MAP_GASDOSE);
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
  _this->m_OnMapChanged(TYPE_MAP_BAROCORR);
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
 _this->m_md[TYPE_MAP_BAROCORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_BAROCORR].handle, TYPE_MAP_BAROCORR);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeBarocorrXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (i_type > 1)
 {
  ASSERT(0);
 }
 else
  _this->GetBarocorrMap(false)[9 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_BAROCORR);
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
  _this->m_OnMapChanged(TYPE_MAP_MANIGNTIM);
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
 _this->m_md[TYPE_MAP_MANIGNTIM].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_MANIGNTIM].handle, TYPE_MAP_MANIGNTIM);
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
  _this->m_OnMapChanged(TYPE_MAP_TMP2_CURVE);
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
 _this->m_md[TYPE_MAP_TMP2_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_TMP2_CURVE].handle, TYPE_MAP_TMP2_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeTmp2CurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (i_type > 1)
 {
  ASSERT(0);
 }
 else
  _this->GetTmp2CurveMap(false)[16 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_TMP2_CURVE);
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
  _this->m_OnMapChanged(TYPE_MAP_CRKCLT_CORR);
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
 _this->m_md[TYPE_MAP_CRKCLT_CORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_CRKCLT_CORR].handle, TYPE_MAP_CRKCLT_CORR);
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
   _this->m_OnMapChanged(TYPE_MAP_EH_PAUSE);
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
 _this->m_md[TYPE_MAP_EH_PAUSE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_EH_PAUSE].handle, TYPE_MAP_EH_PAUSE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_ATTENUATOR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_DWELLCNTRL].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_CTS_CURVE].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_ATS_CURVE].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_ATS_CORR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_GASDOSE].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_BAROCORR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_MANIGNTIM].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_TMP2_CURVE].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_CRKCLT_CORR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_EH_PAUSE].handle, cmd);
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
   _this->m_OnMapChanged(TYPE_MAP_CRANKING_THRD);
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
 _this->m_md[TYPE_MAP_CRANKING_THRD].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_CRANKING_THRD].handle, TYPE_MAP_CRANKING_THRD);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_CRANKING_THRD].handle, cmd);
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
   _this->m_OnMapChanged(TYPE_MAP_CRANKING_TIME);
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
 _this->m_md[TYPE_MAP_CRANKING_TIME].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_CRANKING_TIME].handle, TYPE_MAP_CRANKING_TIME);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_CRANKING_TIME].handle, cmd);
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
   _this->m_OnMapChanged(TYPE_MAP_SMAPABAN_THRD);
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
 _this->m_md[TYPE_MAP_SMAPABAN_THRD].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_SMAPABAN_THRD].handle, TYPE_MAP_SMAPABAN_THRD);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_SMAPABAN_THRD].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_KNOCK_ZONE);
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
 _this->m_md[TYPE_MAP_KNOCK_ZONE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_KNOCK_ZONE].handle, TYPE_MAP_KNOCK_ZONE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_KNOCK_ZONE].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_GRTS_CURVE);
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
 _this->m_md[TYPE_MAP_GRTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_GRTS_CURVE].handle, TYPE_MAP_GRTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeGrtsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (i_type > 1)
 {
  ASSERT(0);
 }
 else
  _this->GetGrtsCurveMap(false)[16 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_GRTS_CURVE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_GRTS_CURVE].handle, cmd);
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
   _this->m_OnMapChanged(TYPE_MAP_GRHEAT_DUTY);
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
 _this->m_md[TYPE_MAP_GRHEAT_DUTY].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_GRHEAT_DUTY].handle, TYPE_MAP_GRHEAT_DUTY);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_GRHEAT_DUTY].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_PWMIAC_UCOEF);
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
 _this->m_md[TYPE_MAP_PWMIAC_UCOEF].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_PWMIAC_UCOEF].handle, TYPE_MAP_PWMIAC_UCOEF);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_PWMIAC_UCOEF].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_AFTSTR_STRK0);
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
 _this->m_md[TYPE_MAP_AFTSTR_STRK0].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_AFTSTR_STRK0].handle, TYPE_MAP_AFTSTR_STRK0);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_AFTSTR_STRK0].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_AFTSTR_STRK1);
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
 _this->m_md[TYPE_MAP_AFTSTR_STRK1].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_AFTSTR_STRK1].handle, TYPE_MAP_AFTSTR_STRK1);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_AFTSTR_STRK1].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_GRVDELAY);
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
 _this->m_md[TYPE_MAP_GRVDELAY].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_GRVDELAY].handle, TYPE_MAP_GRVDELAY);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_GRVDELAY].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_FTLS_CURVE);
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
 _this->m_md[TYPE_MAP_FTLS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_FTLS_CURVE].handle, TYPE_MAP_FTLS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeFtlsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (i_type > 1)
 {
  ASSERT(0);
 }
 else
  _this->GetFtlsCurveMap(false)[17 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_FTLS_CURVE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_FTLS_CURVE].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_EGTS_CURVE);
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
 _this->m_md[TYPE_MAP_EGTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_EGTS_CURVE].handle, TYPE_MAP_EGTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeEgtsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (i_type > 1)
 {
  ASSERT(0);
 }
 else
  _this->GetEgtsCurveMap(false)[17 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_EGTS_CURVE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_EGTS_CURVE].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_OPS_CURVE);
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
 _this->m_md[TYPE_MAP_OPS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_OPS_CURVE].handle, TYPE_MAP_OPS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeOpsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (i_type > 1)
 {
  ASSERT(0);
 }
 else
  _this->GetOpsCurveMap(false)[17 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_OPS_CURVE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_OPS_CURVE].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_MANINJPWC);
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
 _this->m_md[TYPE_MAP_MANINJPWC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_MANINJPWC].handle, TYPE_MAP_MANINJPWC);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_MANINJPWC].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_MAF_CURVE);
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
 _this->m_md[TYPE_MAP_MAF_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_MAF_CURVE].handle, TYPE_MAP_MAF_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeMAFCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (i_type > 1)
 {
  ASSERT(0);
 }
 else
  _this->GetMAFCurveMap(false)[64 + 1 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_MAF_CURVE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_MAF_CURVE].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_FTLSCOR);
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
 _this->m_md[TYPE_MAP_FTLSCOR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_FTLSCOR].handle, TYPE_MAP_FTLSCOR);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_FTLSCOR].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_LAMBDA_ZONE);
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
 _this->m_md[TYPE_MAP_LAMBDA_ZONE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_LAMBDA_ZONE].handle, TYPE_MAP_LAMBDA_ZONE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_LAMBDA_ZONE].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_FTS_CURVE);
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
 _this->m_md[TYPE_MAP_FTS_CURVE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_FTS_CURVE].handle, TYPE_MAP_FTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CSeptabsPanel::OnChangeFtsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CSeptabsPanel* _this = static_cast<CSeptabsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (i_type > 1)
 {
  ASSERT(0);
 }
 else
  _this->GetFtsCurveMap(false)[17 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_FTS_CURVE);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_FTS_CURVE].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_FUELDENS_CORR);
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
 _this->m_md[TYPE_MAP_FUELDENS_CORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_FUELDENS_CORR].handle, TYPE_MAP_FUELDENS_CORR);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_FUELDENS_CORR].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_XTAU_XFACC);
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
 _this->m_md[TYPE_MAP_XTAU_XFACC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_XTAU_XFACC].handle, TYPE_MAP_XTAU_XFACC);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_XTAU_XFACC].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_XTAU_XFDEC);
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
 _this->m_md[TYPE_MAP_XTAU_XFDEC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_XTAU_XFDEC].handle, TYPE_MAP_XTAU_XFDEC);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_XTAU_XFDEC].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_XTAU_TFACC);
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
 _this->m_md[TYPE_MAP_XTAU_TFACC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_XTAU_TFACC].handle, TYPE_MAP_XTAU_TFACC);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_XTAU_TFACC].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_XTAU_TFDEC);
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
 _this->m_md[TYPE_MAP_XTAU_TFDEC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_XTAU_TFDEC].handle, TYPE_MAP_XTAU_TFDEC);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_XTAU_TFDEC].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_INJNONLINP);
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
 _this->m_md[TYPE_MAP_INJNONLINP].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJNONLINP].handle, TYPE_MAP_INJNONLINP);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJNONLINP].handle, cmd);
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
  _this->m_OnMapChanged(TYPE_MAP_INJNONLING);
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
 _this->m_md[TYPE_MAP_INJNONLING].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJNONLING].handle, TYPE_MAP_INJNONLING);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJNONLING].handle, cmd);
}

//------------------------------------------------------------------------
