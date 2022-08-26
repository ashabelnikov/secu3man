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

/** \file TablesSetPanelEvents.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "common/MathHelpers.h"
#include "DLLLinkedFunctions.h"
#include "io-core/secu3io.h"
#include "MapIds.h"
#include "TablesSetPanel.h"
#include "ui-core/ToolTipCtrlEx.h"

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeAttenuatorTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_ATTENUATOR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseAttenuatorTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeDwellCntrlTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_DWELLCNTRL);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseDwellCntrlTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeCTSCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_CTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseCTSCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeATSCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_ATS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseATSCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeATSAACTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_ATS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseATSAACTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnGetYAxisLabel(LPTSTR io_label_string, int index, void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnGetXAxisLabel(LPTSTR io_label_string, int index, void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }
 _stprintf(io_label_string, _T("")); //empty string
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeCTSXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeATSXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeGasdoseTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_GASDOSE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseGasdoseTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeBarocorrTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_BAROCORR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseBarocorrTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeBarocorrXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeManIgntimTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_MANIGNTIM);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseManIgntimTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeTmp2CurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_TMP2_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseTmp2CurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeTmp2CurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeCrkTempTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_CRKCLT_CORR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseCrkTempTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeEHPauseTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_EH_PAUSE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseEHPauseTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationAttenuatorTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_ATTENUATOR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationDwellCntrlTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_DWELLCNTRL].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationCTSCurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_CTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationATSCurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_ATS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationATSAACTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_ATS_CORR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationGasdoseTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_GASDOSE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationBarocorrTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_BAROCORR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationManIgntimTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_MANIGNTIM].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationTmp2CurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_TMP2_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationCrkTempTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_CRKCLT_CORR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnWndActivationEHPauseTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_EH_PAUSE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeCrankingThrdMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_CRANKING_THRD);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseCrankingThrdMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationCrankingThrdMap(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_CRANKING_THRD].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeCrankingTimeMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_CRANKING_TIME);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseCrankingTimeMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationCrankingTimeMap(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_CRANKING_TIME].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeSmapabanThrdMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_SMAPABAN_THRD);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseSmapabanThrdMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationSmapabanThrdMap(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_SMAPABAN_THRD].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeKnockZoneMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_KNOCK_ZONE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseKnockZoneMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationKnockZoneMap(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_KNOCK_ZONE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeGrtsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_GRTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseGrtsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeGrtsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationGrtsCurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_GRTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeGrHeatDutyTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_GRHEAT_DUTY);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseGrHeatDutyTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationGrHeatDutyTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_GRHEAT_DUTY].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangePwmIacUCoefMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_PWMIAC_UCOEF);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnClosePwmIacUCoefMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationPwmIacUCoefMap(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_PWMIAC_UCOEF].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeAftstrStrk0Map(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_AFTSTR_STRK0);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseAftstrStrk0Map(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationAftstrStrk0Map(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_AFTSTR_STRK0].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeAftstrStrk1Map(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_AFTSTR_STRK1);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseAftstrStrk1Map(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationAftstrStrk1Map(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_AFTSTR_STRK1].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeGrValDelMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_GRVDELAY);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseGrValDelMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationGrValDelMap(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_GRVDELAY].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeFtlsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_FTLS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseFtlsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeFtlsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationFtlsCurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_FTLS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeEgtsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_EGTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseEgtsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeEgtsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationEgtsCurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_EGTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeOpsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_OPS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseOpsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeOpsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationOpsCurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_OPS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeManInjPwcTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_MANINJPWC);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseManInjPwcTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationManInjPwcTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_MANINJPWC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeMAFCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_MAF_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseMAFCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeMAFCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationMAFCurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_MAF_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeFtlsCorTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_FTLSCOR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseFtlsCorTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationFtlsCorTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_FTLSCOR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeLambdaZoneMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_LAMBDA_ZONE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseLambdaZoneMap(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationLambdaZoneMap(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_LAMBDA_ZONE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeFtsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_FTS_CURVE);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseFtsCurveTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnChangeFtsCurveXAxisEdit(void* i_param, int i_type, float i_value)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationFtsCurveTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_FTS_CURVE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnChangeFuelDensCorrTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_FUELDENS_CORR);
}

//------------------------------------------------------------------------
void __cdecl CTablesSetPanel::OnCloseFuelDensCorrTable(void* i_param)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
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
void __cdecl CTablesSetPanel::OnWndActivationFuelDensCorrTable(void* i_param, long cmd)
{
 CTablesSetPanel* _this = static_cast<CTablesSetPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_FUELDENS_CORR].handle, cmd);
}

//------------------------------------------------------------------------
