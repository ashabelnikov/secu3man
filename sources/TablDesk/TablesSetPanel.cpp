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

/** \file TablesSetPanel.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "common/MathHelpers.h"
#include "DLLLinkedFunctions.h"
#include "DwellCalcDlg.h"
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


const UINT CTablesSetPanel::IDD = IDD_TD_ALLTABLES_PANEL;

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel dialog

CTablesSetPanel::CTablesSetPanel(CWnd* pParent /*= NULL*/)
: Super(CTablesSetPanel::IDD, pParent)
, m_dwellcntrl_enabled(false)
, m_cts_curve_enabled(false)
, m_tmp2_curve_enabled(false)
, m_grts_curve_enabled(false)
, m_grheat_duty_enabled(false)
, m_pwmiac_ucoef_enabled(false)
, m_aftstr_strk_enabled(false)
, m_ftls_curve_enabled(false)
, m_egts_curve_enabled(false)
, m_ops_curve_enabled(false)
, m_maninjpwc_enabled(false)
, m_lambdazone_enabled(false)
{
 m_scrl_view = 1120;

 for(int i = TYPE_MAP_SEP_START; i <= TYPE_MAP_SEP_END; ++i)
 {
  MapData md;
  m_md.insert(std::make_pair(i, md));
 }

 int rpm = 200;
 for(size_t i = 0; i < 128; i++)
 {
  m_attenuator_table_slots[i] = (float)rpm;
  rpm+=60;
 }

/*
 for(size_t i = 0; i < 64; ++i)
  m_mafcurve_slots[i] = (float)((5.0*i) / 63.0);
*/
 memset(m_cts_curve_x_axis_limits, 0, sizeof(float) * 2);
 memset(m_ats_curve_x_axis_limits, 0, sizeof(float) * 2);
}

void CTablesSetPanel::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_FUNSET_LIST, m_funset_listbox);
 DDX_Control(pDX, IDC_TD_VIEW_ATTENUATOR_MAP, m_view_attenuator_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_DWELL_CONTROL, m_view_dwellcntrl_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_CTS_CURVE, m_view_cts_curve_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_ATS_CURVE, m_view_ats_curve_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_ATS_MAP, m_view_ats_aac_map_btn);
 DDX_Control(pDX, IDC_TD_DWELL_CALC_BUTTON, m_calc_dwell_btn);
 DDX_Control(pDX, IDC_TD_RPM_GRID_BUTTON, m_rpm_grid_btn);
 DDX_Control(pDX, IDC_TD_FW_CONSTS_BUTTON, m_fw_consts_btn);
 DDX_Control(pDX, IDC_TD_VIEW_GDP_MAP, m_view_gasdose_map_btn);
 DDX_Control(pDX, IDC_TD_EDIT_CEPAR, m_edit_cesettings_btn);
 DDX_Control(pDX, IDC_TD_VIEW_BAROCORR_MAP, m_view_barocorr_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_MANIGNTIM_MAP, m_view_manigntim_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_TMP2_CURVE, m_view_tmp2_curve_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_CRKTEMP_MAP, m_view_crktemp_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_EH_PAUSE_MAP, m_view_eh_pause_map_btn);
 DDX_Control(pDX, IDC_TD_CRANKING_THRD_MAP, m_view_cranking_thrd_map_btn);
 DDX_Control(pDX, IDC_TD_CRANKING_TIME_MAP, m_view_cranking_time_map_btn);
 DDX_Control(pDX, IDC_TD_SMAPABAN_THRD_MAP, m_view_smapaban_thrd_map_btn);
 DDX_Control(pDX, IDC_TD_KNOCK_ZONES_MAP, m_view_knock_zone_map_btn);
 DDX_Control(pDX, IDC_TD_LAMBDA_ZONES_MAP, m_view_lambda_zone_map_btn);
 DDX_Control(pDX, IDC_TD_GRTS_CURVE, m_view_grts_curve_map_btn);
 DDX_Control(pDX, IDC_TD_GRHEAT_DUTY_MAP, m_view_grheat_duty_map_btn);
 DDX_Control(pDX, IDC_TD_PWMIAC_UCOEF_MAP, m_view_pwmiac_ucoef_map_btn);
 DDX_Control(pDX, IDC_TD_AFTSTR_STRK0_MAP, m_view_aftstr_strk0_map_btn);
 DDX_Control(pDX, IDC_TD_AFTSTR_STRK1_MAP, m_view_aftstr_strk1_map_btn);
 DDX_Control(pDX, IDC_TD_GRVDELAY_MAP, m_view_grvaldel_map_btn);
 DDX_Control(pDX, IDC_TD_FTLS_CURVE, m_view_ftls_curve_map_btn);
 DDX_Control(pDX, IDC_TD_EGTS_CURVE, m_view_egts_curve_map_btn);
 DDX_Control(pDX, IDC_TD_OPS_CURVE, m_view_ops_curve_map_btn);
 DDX_Control(pDX, IDC_TD_MANINJPWC_MAP, m_view_maninjpwc_map_btn);
 DDX_Control(pDX, IDC_TD_MAF_CURVE, m_view_mafcurve_map_btn);
 DDX_Control(pDX, IDC_TD_FTLSCOR_MAP, m_view_ftlscor_map_btn);
}

BEGIN_MESSAGE_MAP(CTablesSetPanel, Super)
 ON_BN_CLICKED(IDC_TD_VIEW_ATTENUATOR_MAP, OnViewAttenuatorMap)
 ON_BN_CLICKED(IDC_TD_VIEW_DWELL_CONTROL, OnViewDwellCntrlMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CTS_CURVE, OnViewCTSCurveMap)
 ON_BN_CLICKED(IDC_TD_DWELL_CALC_BUTTON, OnDwellCalcButton)
 ON_BN_CLICKED(IDC_TD_RPM_GRID_BUTTON, OnRPMGridButton)
 ON_BN_CLICKED(IDC_TD_FW_CONSTS_BUTTON, OnFwConstsButton)
 ON_BN_CLICKED(IDC_TD_VIEW_ATS_CURVE, OnViewATSCurveMap)
 ON_BN_CLICKED(IDC_TD_VIEW_ATS_MAP, OnViewATSAACMap)
 ON_BN_CLICKED(IDC_TD_VIEW_GDP_MAP, OnViewGasdosePosMap)
 ON_BN_CLICKED(IDC_TD_EDIT_CEPAR, OnCESettingsButton)
 ON_BN_CLICKED(IDC_TD_VIEW_BAROCORR_MAP, OnViewBarocorrMap)
 ON_BN_CLICKED(IDC_TD_VIEW_MANIGNTIM_MAP, OnViewManIgntimMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TMP2_CURVE, OnViewTmp2CurveMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CRKTEMP_MAP, OnViewCrkTempMap)
 ON_BN_CLICKED(IDC_TD_VIEW_EH_PAUSE_MAP, OnViewEHPauseMap)
 ON_BN_CLICKED(IDC_TD_CRANKING_THRD_MAP, OnViewCrankingThrdMap)
 ON_BN_CLICKED(IDC_TD_CRANKING_TIME_MAP, OnViewCrankingTimeMap)
 ON_BN_CLICKED(IDC_TD_SMAPABAN_THRD_MAP, OnViewSmapabanThrdMap)
 ON_BN_CLICKED(IDC_TD_KNOCK_ZONES_MAP, OnViewKnockZoneMap)
 ON_BN_CLICKED(IDC_TD_LAMBDA_ZONES_MAP, OnViewLambdaZoneMap)
 ON_BN_CLICKED(IDC_TD_GRTS_CURVE, OnViewGrtsCurveMap)
 ON_BN_CLICKED(IDC_TD_GRHEAT_DUTY_MAP, OnViewGrHeatDutyMap)
 ON_BN_CLICKED(IDC_TD_PWMIAC_UCOEF_MAP, OnViewPwmIacUCoefMap)
 ON_BN_CLICKED(IDC_TD_AFTSTR_STRK0_MAP, OnViewAftstrStrk0Map)
 ON_BN_CLICKED(IDC_TD_AFTSTR_STRK1_MAP, OnViewAftstrStrk1Map)
 ON_BN_CLICKED(IDC_TD_GRVDELAY_MAP, OnViewGrValDelMap)
 ON_BN_CLICKED(IDC_TD_FTLS_CURVE, OnViewFtlsCurveMap)
 ON_BN_CLICKED(IDC_TD_EGTS_CURVE, OnViewEgtsCurveMap)
 ON_BN_CLICKED(IDC_TD_OPS_CURVE, OnViewOpsCurveMap)
 ON_BN_CLICKED(IDC_TD_MANINJPWC_MAP, OnViewManInjPwcMap)
 ON_BN_CLICKED(IDC_TD_MAF_CURVE, OnViewMAFCurveMap)
 ON_BN_CLICKED(IDC_TD_FTLSCOR_MAP, OnViewFtlsCorMap)

 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATTENUATOR_MAP, OnUpdateViewAttenuatorMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_DWELL_CONTROL, OnUpdateViewDwellCntrlMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CTS_CURVE, OnUpdateViewCTSCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATS_CURVE, OnUpdateViewATSCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATS_MAP, OnUpdateViewATSAACMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_DWELL_CALC_BUTTON, OnUpdateViewDwellCntrlMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_RPM_GRID_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_FW_CONSTS_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_FUNSET_LIST, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_MAP_GROUPBOX, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_GDP_MAP, OnUpdateViewGasdosePosMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_EDIT_CEPAR, OnUpdateCESettingsButton)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_BAROCORR_MAP, OnUpdateViewBarocorrMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_MANIGNTIM_MAP, OnUpdateViewManIgntimMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TMP2_CURVE, OnUpdateViewTmp2CurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CRKTEMP_MAP, OnUpdateViewCrkTempMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_EH_PAUSE_MAP, OnUpdateViewEHPauseMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_CRANKING_THRD_MAP, OnUpdateViewCrankingThrdMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_CRANKING_TIME_MAP, OnUpdateViewCrankingTimeMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_SMAPABAN_THRD_MAP, OnUpdateViewSmapabanThrdMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_KNOCK_ZONES_MAP, OnUpdateViewKnockZoneMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_LAMBDA_ZONES_MAP, OnUpdateViewLambdaZoneMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_GRTS_CURVE, OnUpdateViewGrtsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_GRHEAT_DUTY_MAP, OnUpdateViewGrHeatDutyMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_PWMIAC_UCOEF_MAP, OnUpdateViewPwmIacUCoefMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_AFTSTR_STRK0_MAP, OnUpdateViewAftstrStrk0Map)
 ON_UPDATE_COMMAND_UI(IDC_TD_AFTSTR_STRK1_MAP, OnUpdateViewAftstrStrk1Map)
 ON_UPDATE_COMMAND_UI(IDC_TD_GRVDELAY_MAP, OnUpdateViewGrValDelMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_FTLS_CURVE, OnUpdateViewFtlsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_EGTS_CURVE, OnUpdateViewEgtsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_OPS_CURVE, OnUpdateViewOpsCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_MANINJPWC_MAP, OnUpdateViewManInjPwcMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_MAF_CURVE, OnUpdateViewMAFCurveMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_FTLSCOR_MAP, OnUpdateViewFtlsCorMap)
 ON_NOTIFY(LVN_ITEMCHANGED, IDC_TD_FUNSET_LIST, OnChangeFunsetList)
 ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TD_FUNSET_LIST, OnEndLabelEditFunsetList)
 ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel message handlers

BOOL CTablesSetPanel::OnInitDialog()
{
 Super::OnInitDialog();

 m_calc_dwell_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                              MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));
 m_rpm_grid_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_GRID_UP), MAKEINTRESOURCE(IDB_GRID_DOWN), 
                              MAKEINTRESOURCE(IDB_GRID_FOCUSED), MAKEINTRESOURCE(IDB_GRID_DISABLED));
 m_fw_consts_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CONST_UP), MAKEINTRESOURCE(IDB_CONST_DOWN), 
                              MAKEINTRESOURCE(IDB_CONST_FOCUSED), MAKEINTRESOURCE(IDB_CONST_DISABLED));

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_calc_dwell_btn, MLL::GetString(IDS_TD_DWELL_CALC_BUTTON_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_rpm_grid_btn, MLL::GetString(IDS_TD_RPM_GRID_BUTTON_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_dwellcntrl_map_btn, MLL::GetString(IDS_TD_VIEW_DWELL_CONTROL_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_fw_consts_btn, MLL::GetString(IDS_TD_FW_CONSTS_BUTTON_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_funset_listbox, MLL::GetString(IDS_TD_FUNSET_LIST_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_crktemp_map_btn, MLL::GetString(IDS_TD_VIEW_CRKTEMP_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_attenuator_map_btn, MLL::GetString(IDS_TD_VIEW_ATTENUATOR_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_gasdose_map_btn, MLL::GetString(IDS_TD_VIEW_GDP_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_barocorr_map_btn, MLL::GetString(IDS_TD_VIEW_BAROCORR_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_cts_curve_map_btn, MLL::GetString(IDS_TD_VIEW_CTS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_ats_curve_map_btn, MLL::GetString(IDS_TD_VIEW_ATS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_ats_aac_map_btn, MLL::GetString(IDS_TD_VIEW_ATS_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_edit_cesettings_btn, MLL::GetString(IDS_TD_EDIT_CEPAR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_manigntim_map_btn, MLL::GetString(IDS_TD_VIEW_MANIGNTIM_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_tmp2_curve_map_btn, MLL::GetString(IDS_TD_VIEW_TMP2_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_eh_pause_map_btn, MLL::GetString(IDS_TD_VIEW_EH_PAUSE_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_cranking_thrd_map_btn, MLL::GetString(IDS_TD_CRANKING_THRD_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_cranking_time_map_btn, MLL::GetString(IDS_TD_CRANKING_TIME_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_smapaban_thrd_map_btn, MLL::GetString(IDS_TD_SMAPABAN_THRD_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_knock_zone_map_btn, MLL::GetString(IDS_TD_VIEW_KNOCK_ZONE_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_lambda_zone_map_btn, MLL::GetString(IDS_TD_LAMBDA_ZONES_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_grts_curve_map_btn, MLL::GetString(IDS_TD_GRTS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_grheat_duty_map_btn, MLL::GetString(IDS_TD_GRHEAT_DUTY_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_pwmiac_ucoef_map_btn, MLL::GetString(IDS_TD_PWMIAC_UCOEF_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_aftstr_strk0_map_btn, MLL::GetString(IDS_TD_AFTSTR_STRK0_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_aftstr_strk1_map_btn, MLL::GetString(IDS_TD_AFTSTR_STRK1_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_grvaldel_map_btn, MLL::GetString(IDS_TD_GRVDELAY_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_ftls_curve_map_btn, MLL::GetString(IDS_TD_FTLS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_egts_curve_map_btn, MLL::GetString(IDS_TD_EGTS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_ops_curve_map_btn, MLL::GetString(IDS_TD_OPS_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_maninjpwc_map_btn, MLL::GetString(IDS_TD_MANINJPWC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_mafcurve_map_btn, MLL::GetString(IDS_TD_MAF_CURVE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_ftlscor_map_btn, MLL::GetString(IDS_TD_FTLSCOR_MAP_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CTablesSetPanel::OnDestroy()
{
 Super::OnDestroy();
}

void CTablesSetPanel::OnUpdateViewAttenuatorMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_ATTENUATOR].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewDwellCntrlMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_dwellcntrl_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_DWELLCNTRL].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewCTSCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_cts_curve_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_CTS_CURVE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewATSCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_ATS_CURVE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewATSAACMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_ATS_CORR].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewGasdosePosMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart3DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_gasdose);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_GASDOSE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateCESettingsButton(CCmdUI* pCmdUI)
{
 bool enable = m_IsAllowed ? m_IsAllowed() : false;
 pCmdUI->Enable(enable);
}

void CTablesSetPanel::OnUpdateViewBarocorrMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && (m_gasdose || m_fuel_injection));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_BAROCORR].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewManIgntimMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_MANIGNTIM].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewTmp2CurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_tmp2_curve_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_TMP2_CURVE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewGrtsCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_grts_curve_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_GRTS_CURVE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewCrkTempMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_CRKCLT_CORR].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewEHPauseMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_EH_PAUSE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewCrankingThrdMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_CRANKING_THRD].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewCrankingTimeMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_CRANKING_TIME].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewSmapabanThrdMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_SMAPABAN_THRD].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewKnockZoneMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart3DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_KNOCK_ZONE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewLambdaZoneMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart3DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_LAMBDA_ZONE].state) ? TRUE : FALSE );
}

//Updates controls which state depends on whether or not data is
void CTablesSetPanel::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool enabled = m_IsAllowed ? m_IsAllowed() : false;
 pCmdUI->Enable(enabled ? TRUE : FALSE);
}

void CTablesSetPanel::OnUpdateViewGrHeatDutyMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_grheat_duty_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_GRHEAT_DUTY].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewPwmIacUCoefMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_pwmiac_ucoef_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_PWMIAC_UCOEF].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewAftstrStrk0Map(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_aftstr_strk_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_AFTSTR_STRK0].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewAftstrStrk1Map(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_aftstr_strk_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_AFTSTR_STRK1].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewGrValDelMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_grheat_duty_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_GRVDELAY].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewFtlsCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_ftls_curve_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_FTLS_CURVE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewFtlsCorMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_ftls_curve_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_FTLSCOR].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewEgtsCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_egts_curve_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_EGTS_CURVE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewOpsCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_ops_curve_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_OPS_CURVE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewManInjPwcMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable && m_maninjpwc_enabled);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_MANINJPWC].state) ? TRUE : FALSE );
}

void CTablesSetPanel::OnUpdateViewMAFCurveMap(CCmdUI* pCmdUI)
{
 bool opened = m_IsAllowed ? m_IsAllowed() : false;
 BOOL enable = (DLL::Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_MAF_CURVE].state) ? TRUE : FALSE );
}

void CTablesSetPanel::UpdateOpenedCharts(void)
{
 Super::UpdateOpenedCharts();
 if (m_md[TYPE_MAP_ATTENUATOR].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_ATTENUATOR].handle, GetAttenuatorMap(true), GetAttenuatorMap(false));
 if (m_md[TYPE_MAP_DWELLCNTRL].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DWELLCNTRL].handle, GetDwellCntrlMap(true), GetDwellCntrlMap(false));
 if (m_md[TYPE_MAP_CTS_CURVE].state)
 {
  DLL::Chart2DUpdateYRange(m_md[TYPE_MAP_CTS_CURVE].handle, GetCLTGrid()[0], GetCLTGrid()[15]);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_CTS_CURVE].handle, GetCTSCurveMap(true), GetCTSCurveMap(false));
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_CTS_CURVE].handle, 1, m_cts_curve_x_axis_limits[0], m_cts_curve_x_axis_limits[1]);
 }
 if (m_md[TYPE_MAP_ATS_CURVE].state)
 {
  DLL::Chart2DUpdate(m_md[TYPE_MAP_ATS_CURVE].handle, GetATSCurveMap(true), GetATSCurveMap(false));
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_ATS_CURVE].handle, 1, m_ats_curve_x_axis_limits[0], m_ats_curve_x_axis_limits[1]);
 }
 if (m_md[TYPE_MAP_ATS_CORR].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_ATS_CORR].handle, GetATSAACMap(true), GetATSAACMap(false));

 if (m_md[TYPE_MAP_GASDOSE].state)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_GASDOSE].handle, GetGasdosePosMap(true), GetGasdosePosMap(false));

 if (m_md[TYPE_MAP_BAROCORR].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_BAROCORR].handle, GetBarocorrMap(true), GetBarocorrMap(false));

 if (m_md[TYPE_MAP_TMP2_CURVE].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_TMP2_CURVE].handle, GetTmp2CurveMap(true), GetTmp2CurveMap(false));

 if (m_md[TYPE_MAP_CRKCLT_CORR].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_CRKCLT_CORR].handle, GetCrkTempMap(true), GetCrkTempMap(false));

 if (m_md[TYPE_MAP_EH_PAUSE].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_EH_PAUSE].handle, GetEHPauseMap(true), GetEHPauseMap(false));

 if (m_md[TYPE_MAP_CRANKING_THRD].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_CRANKING_THRD].handle, GetCrankingThrdMap(true), GetCrankingThrdMap(false));

 if (m_md[TYPE_MAP_CRANKING_TIME].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_CRANKING_TIME].handle, GetCrankingTimeMap(true), GetCrankingTimeMap(false));

 if (m_md[TYPE_MAP_SMAPABAN_THRD].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_SMAPABAN_THRD].handle, GetSmapabanThrdMap(true), GetSmapabanThrdMap(false));

 if (m_md[TYPE_MAP_KNOCK_ZONE].state)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_KNOCK_ZONE].handle, GetKnockZoneMap(true), GetKnockZoneMap(false));

 if (m_md[TYPE_MAP_LAMBDA_ZONE].state)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_LAMBDA_ZONE].handle, GetLambdaZoneMap(true), GetLambdaZoneMap(false));

 if (m_md[TYPE_MAP_GRTS_CURVE].state)
 {
  DLL::Chart2DUpdateYRange(m_md[TYPE_MAP_GRTS_CURVE].handle, GetCLTGrid()[0], GetCLTGrid()[15]);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_GRTS_CURVE].handle, GetGrtsCurveMap(true), GetGrtsCurveMap(false));
 }

 if (m_md[TYPE_MAP_GRHEAT_DUTY].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_GRHEAT_DUTY].handle, GetGrHeatDutyMap(true), GetGrHeatDutyMap(false));

 if (m_md[TYPE_MAP_PWMIAC_UCOEF].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_PWMIAC_UCOEF].handle, GetPwmIacUCoefMap(true), GetPwmIacUCoefMap(false));

 if (m_md[TYPE_MAP_AFTSTR_STRK0].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_AFTSTR_STRK0].handle, GetAftstrStrk0Map(true), GetAftstrStrk0Map(false));

 if (m_md[TYPE_MAP_AFTSTR_STRK1].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_AFTSTR_STRK1].handle, GetAftstrStrk1Map(true), GetAftstrStrk1Map(false));

 if (m_md[TYPE_MAP_GRVDELAY].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_GRVDELAY].handle, GetGrValDelMap(true), GetGrValDelMap(false));

 if (m_md[TYPE_MAP_FTLS_CURVE].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_FTLS_CURVE].handle, GetFtlsCurveMap(true), GetFtlsCurveMap(false));

 if (m_md[TYPE_MAP_EGTS_CURVE].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_EGTS_CURVE].handle, GetEgtsCurveMap(true), GetEgtsCurveMap(false));

 if (m_md[TYPE_MAP_OPS_CURVE].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_OPS_CURVE].handle, GetOpsCurveMap(true), GetOpsCurveMap(false));

 if (m_md[TYPE_MAP_FTLSCOR].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_FTLSCOR].handle, GetFtlsCorMap(true), GetFtlsCorMap(false));
}

void CTablesSetPanel::EnableDwellControl(bool enable)
{
 m_dwellcntrl_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_DWELLCNTRL].state && ::IsWindow(m_md[TYPE_MAP_DWELLCNTRL].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_DWELLCNTRL].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableCTSCurve(bool enable)
{
 m_cts_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_CTS_CURVE].state && ::IsWindow(m_md[TYPE_MAP_CTS_CURVE].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_CTS_CURVE].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableChokeOp(bool enable)
{
 Super::EnableChokeOp(enable);
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CTablesSetPanel::EnableFuelInjection(bool i_enable)
{
 Super::EnableFuelInjection(i_enable);
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_BAROCORR].state && ::IsWindow(m_md[TYPE_MAP_BAROCORR].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_BAROCORR].handle, (i_enable || m_gasdose) && IsAllowed());
}

void CTablesSetPanel::EnableGasdose(bool enable)
{
 Super::EnableGasdose(enable);
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_GASDOSE].state && ::IsWindow(m_md[TYPE_MAP_GASDOSE].handle))
  DLL::Chart3DEnable(m_md[TYPE_MAP_GASDOSE].handle, enable && Super::IsAllowed());
 if (m_md[TYPE_MAP_BAROCORR].state && ::IsWindow(m_md[TYPE_MAP_BAROCORR].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_BAROCORR].handle, (enable || m_fuel_injection) && IsAllowed());
}

void CTablesSetPanel::EnableTmp2Curve(bool enable)
{
 m_tmp2_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_TMP2_CURVE].state && ::IsWindow(m_md[TYPE_MAP_TMP2_CURVE].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_TMP2_CURVE].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableGrtsCurve(bool enable)
{
 m_grts_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_GRTS_CURVE].state && ::IsWindow(m_md[TYPE_MAP_GRTS_CURVE].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_GRTS_CURVE].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableGrHeatDutyMap(bool enable)
{
 m_grheat_duty_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_GRHEAT_DUTY].state && ::IsWindow(m_md[TYPE_MAP_GRHEAT_DUTY].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_GRHEAT_DUTY].handle, enable && Super::IsAllowed());
 if (m_md[TYPE_MAP_GRVDELAY].state && ::IsWindow(m_md[TYPE_MAP_GRVDELAY].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_GRVDELAY].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnablePwmIacUCoefMap(bool enable)
{
 m_pwmiac_ucoef_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_PWMIAC_UCOEF].state && ::IsWindow(m_md[TYPE_MAP_PWMIAC_UCOEF].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_PWMIAC_UCOEF].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableAftstrStrkMap(bool enable)
{
 m_aftstr_strk_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_AFTSTR_STRK0].state && ::IsWindow(m_md[TYPE_MAP_AFTSTR_STRK0].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_AFTSTR_STRK0].handle, enable && Super::IsAllowed());
 if (m_md[TYPE_MAP_AFTSTR_STRK1].state && ::IsWindow(m_md[TYPE_MAP_AFTSTR_STRK1].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_AFTSTR_STRK1].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableFtlsCurve(bool enable)
{
 m_ftls_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_FTLS_CURVE].state && ::IsWindow(m_md[TYPE_MAP_FTLS_CURVE].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_FTLS_CURVE].handle, enable && Super::IsAllowed());
 if (m_md[TYPE_MAP_FTLSCOR].state && ::IsWindow(m_md[TYPE_MAP_FTLSCOR].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_FTLSCOR].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableEgtsCurve(bool enable)
{
 m_egts_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_EGTS_CURVE].state && ::IsWindow(m_md[TYPE_MAP_EGTS_CURVE].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_EGTS_CURVE].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableOpsCurve(bool enable)
{
 m_ops_curve_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_OPS_CURVE].state && ::IsWindow(m_md[TYPE_MAP_OPS_CURVE].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_OPS_CURVE].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableManInjPwc(bool enable)
{
 m_maninjpwc_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_MANINJPWC].state && ::IsWindow(m_md[TYPE_MAP_MANINJPWC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_MANINJPWC].handle, enable && Super::IsAllowed());
}

void CTablesSetPanel::EnableLambdaZones(bool enable)
{
 m_lambdazone_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_LAMBDA_ZONE].state && ::IsWindow(m_md[TYPE_MAP_LAMBDA_ZONE].handle))
  DLL::Chart3DEnable(m_md[TYPE_MAP_LAMBDA_ZONE].handle, enable && Super::IsAllowed());
}

//изменилось выделение в спимке семейств характеристик
void CTablesSetPanel::OnChangeFunsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR;

 if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED))
 {
  //состо€ние выделени€ изменилось
  int selected_index = 0;

  //will return -1 if there are no selected items, at all
  selected_index = m_funset_listbox.GetNextItem(-1, LVNI_SELECTED);

  if (m_OnFunSetSelectionChanged)
   m_OnFunSetSelectionChanged(selected_index);
 }

 *pResult = 0;
}

void CTablesSetPanel::OnEndLabelEditFunsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLVDISPINFO *pdi = (LV_DISPINFO FAR *) pNMHDR;

 if (pdi->item.mask == LVIF_TEXT)
 {
  m_funset_listbox.SetItem(&pdi->item);
  if (m_OnFunSetNamechanged)
   m_OnFunSetNamechanged(pdi->item.iItem,pdi->item.pszText);
 }
}

void CTablesSetPanel::SetFunSetListBox(const std::vector<_TSTRING>& i_list_of_names)
{
 m_funset_listbox.DeleteAllItems();
 for (size_t i = 0; i < i_list_of_names.size(); i++)
 {
  m_funset_listbox.InsertItem(i,i_list_of_names[i].c_str());
 }
}

void CTablesSetPanel::SetFunSetListBoxSelection(int i_selected_index)
{
 if (i_selected_index!=-1)
  m_funset_listbox.SetItemState(i_selected_index, LVIS_SELECTED, LVIS_SELECTED);

 int i = m_funset_listbox.GetNextItem(-1, LVNI_SELECTED);
}

void CTablesSetPanel::OnViewAttenuatorMap()
{
 MapData &md = m_md[TYPE_MAP_ATTENUATOR];
 //If button was released, then close editor's window
 if (m_view_attenuator_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetAttenuatorMap(true),GetAttenuatorMap(false),0.0f,63,m_attenuator_table_slots,128,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ATTENUATOR_GAIN_UNIT).c_str(),
    MLL::GetString(IDS_ATTENUATOR_MAP).c_str(), false);
  DLL::Chart2DSetMarksVisible(md.handle,1, false); //пр€чем надписи над узловыми точками функции
  DLL::Chart2DSetOnChange(md.handle,OnChangeAttenuatorTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAttenuatorTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationAttenuatorTable, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 0, OnGetYAxisLabel, this);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 0, _T("#0.00"));
  DLL::Chart2DInverseAxis(md.handle, 0, true);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_ATTENUATOR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewDwellCntrlMap()
{
 MapData &md = m_md[TYPE_MAP_DWELLCNTRL];
 //If button was released, then close editor's window
 if (m_view_dwellcntrl_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetDwellCntrlMap(true), GetDwellCntrlMap(false), 0.25f, 16.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DWELLCNTRL_UNIT).c_str(),
    MLL::GetString(IDS_DWELLCNTRL_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(md.handle, OnChangeDwellCntrlTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseDwellCntrlTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationDwellCntrlTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_DWELLCNTRL);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewCTSCurveMap()
{
 MapData &md = m_md[TYPE_MAP_CTS_CURVE];
 //If button was released, then close editor's window
 if (m_view_cts_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCTSCurveMap(true), GetCTSCurveMap(false), GetCLTGrid()[0], GetCLTGrid()[15], NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_CTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, -1, -1, OnChangeCTSXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabel, this);
  DLL::Chart2DSetOnChange(md.handle, OnChangeCTSCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseCTSCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationCTSCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, m_cts_curve_x_axis_limits[0], m_cts_curve_x_axis_limits[1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_CTS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewATSCurveMap()
{
 MapData &md = m_md[TYPE_MAP_ATS_CURVE];
 //If button was released, then close editor's window
 if (m_view_ats_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetATSCurveMap(true), GetATSCurveMap(false), -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_ATS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, -1, -1, OnChangeATSXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabel, this);
  DLL::Chart2DSetOnChange(md.handle, OnChangeATSCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseATSCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationATSCurveTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, m_ats_curve_x_axis_limits[0], m_ats_curve_x_axis_limits[1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_ATS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewATSAACMap()
{
 MapData &md = m_md[TYPE_MAP_ATS_CORR];
 //If button was released, then close editor's window
 if (m_view_ats_aac_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetATSAACMap(true),GetATSAACMap(false),-15.0,25.0,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_ATSCORR_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(md.handle,OnChangeATSAACTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseATSAACTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationATSAACTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_ATS_CORR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewGasdosePosMap()
{
 MapData &md = m_md[TYPE_MAP_GASDOSE];
 //if button has been turned off, then close editor's window
 if (m_view_gasdose_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), GetGasdosePosMap(true),GetGasdosePosMap(false),GetRPMGrid(),16,16,0,100.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_GDP_UNIT).c_str(),
    MLL::GetString(IDS_GDP_MAP).c_str());
  DLL::Chart3DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(md.handle,OnChangeGasdoseTable,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseGasdoseTable,this);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationGasdoseTable, this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_GASDOSE);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewBarocorrMap()
{
 MapData &md = m_md[TYPE_MAP_BAROCORR];
 //If button was released, then close editor's window
 if (m_view_barocorr_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetBarocorrMap(true), GetBarocorrMap(false), 80.0, 120.0, NULL, 9,
    MLL::GetString(IDS_MAPS_ATMOPRESS_UNIT).c_str(),  //horizontal axis
    MLL::GetString(IDS_MAPS_COEFFP_UNIT).c_str(),     //vertical axis
    MLL::GetString(IDS_BAROCORR_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 60.0f, 110.0f, 60.0f, 110.0f, 0.1f, 5, 1, OnChangeBarocorrXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeBarocorrTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseBarocorrTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationBarocorrTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, GetBarocorrMap(false)[9], GetBarocorrMap(false)[9+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_BAROCORR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewTmp2CurveMap()
{
 MapData &md = m_md[TYPE_MAP_TMP2_CURVE];
 //If button was released, then close editor's window
 if (m_view_tmp2_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetTmp2CurveMap(true), GetTmp2CurveMap(false), -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_TMP2_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeTmp2CurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeTmp2CurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseTmp2CurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationTmp2CurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, GetTmp2CurveMap(false)[16], GetTmp2CurveMap(false)[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_TMP2_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewGrtsCurveMap()
{
 MapData &md = m_md[TYPE_MAP_GRTS_CURVE];
 //If button was released, then close editor's window
 if (m_view_grts_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetGrtsCurveMap(true), GetGrtsCurveMap(false), -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_GRTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeGrtsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeGrtsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseGrtsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationGrtsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, GetGrtsCurveMap(false)[16], GetGrtsCurveMap(false)[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_GRTS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewManIgntimMap()
{
 MapData &md = m_md[TYPE_MAP_MANIGNTIM];
 //If button was released, then close editor's window
 if (m_view_manigntim_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetManIgntimMap(true),GetManIgntimMap(false),-15.0,15.0,SECU3IO::manigntim_map_slots,16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_MANIGNTIM_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.0"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnChange(md.handle,OnChangeManIgntimTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseManIgntimTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationManIgntimTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_MANIGNTIM);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewCrkTempMap()
{
 MapData &md = m_md[TYPE_MAP_CRKCLT_CORR];
 //If button was released, then close editor's window
 if (m_view_crktemp_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCrkTempMap(true),GetCrkTempMap(false),-15.0,25.0, GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_CRKTEMP_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(md.handle,OnChangeCrkTempTable,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseCrkTempTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationCrkTempTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_CRKCLT_CORR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewEHPauseMap()
{
 MapData &md = m_md[TYPE_MAP_EH_PAUSE];
 //If button was released, then close editor's window
 if (m_view_eh_pause_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetEHPauseMap(true), GetEHPauseMap(false), 0.01f, 2.55f, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_EH_PAUSE_UNIT).c_str(),
    MLL::GetString(IDS_EH_PAUSE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(md.handle, OnChangeEHPauseTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseEHPauseTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationEHPauseTable, this);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_EH_PAUSE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewCrankingThrdMap()
{
 MapData &md = m_md[TYPE_MAP_CRANKING_THRD];
 //if button was released, then close editor's window
 if (m_view_cranking_thrd_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCrankingThrdMap(true),GetCrankingThrdMap(false),0.0f,2500.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_CRANKING_THRD_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationCrankingThrdMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeCrankingThrdMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseCrankingThrdMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_CRANKING_THRD);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewCrankingTimeMap()
{
 MapData &md = m_md[TYPE_MAP_CRANKING_TIME];
 //if button was released, then close editor's window
 if (m_view_cranking_time_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCrankingTimeMap(true),GetCrankingTimeMap(false),0.0f,255.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_STROKE_UNIT).c_str(),
    MLL::GetString(IDS_CRANKING_TIME_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationCrankingTimeMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeCrankingTimeMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseCrankingTimeMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_CRANKING_TIME);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewSmapabanThrdMap()
{
 MapData &md = m_md[TYPE_MAP_SMAPABAN_THRD];
 //if button was released, then close editor's window
 if (m_view_smapaban_thrd_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetSmapabanThrdMap(true),GetSmapabanThrdMap(false),0.0f,2500.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_SMAPABAN_THRD_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnWndActivation(md.handle,OnWndActivationSmapabanThrdMap,this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeSmapabanThrdMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseSmapabanThrdMap,this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_SMAPABAN_THRD);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewKnockZoneMap()
{
 MapData &md = m_md[TYPE_MAP_KNOCK_ZONE];
 //if button has been turned off, then close editor's window
 if (m_view_knock_zone_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), GetKnockZoneMap(true),GetKnockZoneMap(false),GetRPMGrid(),16,16,0,1.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_KNOCK_ZONE_UNIT).c_str(),
    MLL::GetString(IDS_KNOCK_ZONE_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart3DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(md.handle,OnChangeKnockZoneMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseKnockZoneMap,this);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationKnockZoneMap, this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_KNOCK_ZONE);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewLambdaZoneMap()
{
 MapData &md = m_md[TYPE_MAP_LAMBDA_ZONE];
 //if button has been turned off, then close editor's window
 if (m_view_lambda_zone_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart3DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart3DCreate(_ChartParentHwnd(), GetLambdaZoneMap(true),GetLambdaZoneMap(false),GetRPMGrid(),16,16,0,1.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_LAMBDA_ZONE_UNIT).c_str(),
    MLL::GetString(IDS_LAMBDA_ZONE_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart3DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(md.handle,OnChangeLambdaZoneMap,this);
  DLL::Chart3DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(md.handle,OnCloseLambdaZoneMap,this);
  DLL::Chart3DSetOnWndActivation(md.handle, OnWndActivationLambdaZoneMap, this);
  DLL::Chart3DSetPtMovingStep(md.handle, md.ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_LAMBDA_ZONE);

  DLL::Chart3DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewGrHeatDutyMap()
{
 MapData &md = m_md[TYPE_MAP_GRHEAT_DUTY];
 //If button was released, then close editor's window
 if (m_view_grheat_duty_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetGrHeatDutyMap(true),GetGrHeatDutyMap(false),0.0,100.0, GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DUTY_UNIT).c_str(),
    MLL::GetString(IDS_GRHEAT_DUTY_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(md.handle,OnChangeGrHeatDutyTable,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseGrHeatDutyTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationGrHeatDutyTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_GRHEAT_DUTY);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewPwmIacUCoefMap()
{
 MapData &md = m_md[TYPE_MAP_PWMIAC_UCOEF];
 //If button was released, then close editor's window
 if (m_view_pwmiac_ucoef_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetPwmIacUCoefMap(true),GetPwmIacUCoefMap(false), 0.0, 3.0, SECU3IO::voltage_map_slots, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_PWMIAC_UCOEF_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.01f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetOnChange(md.handle,OnChangePwmIacUCoefMap,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnClosePwmIacUCoefMap,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationPwmIacUCoefMap, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_PWMIAC_UCOEF);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewAftstrStrk0Map()
{
 MapData &md = m_md[TYPE_MAP_AFTSTR_STRK0];
 //If button was released, then close editor's window
 if (m_view_aftstr_strk0_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetAftstrStrk0Map(true), GetAftstrStrk0Map(false), 1.0, 2000.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(), //x unit
    MLL::GetString(IDS_MAPS_STROKE_UNIT).c_str(),      //y unit
    MLL::GetString(IDS_AFTSTR_STRK0_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.0f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnChange(md.handle,OnChangeAftstrStrk0Map,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAftstrStrk0Map,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationAftstrStrk0Map, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_AFTSTR_STRK0);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_AFTSTR_STRK0].handle);
 }
}

void CTablesSetPanel::OnViewAftstrStrk1Map()
{
 MapData &md = m_md[TYPE_MAP_AFTSTR_STRK1];
 //If button was released, then close editor's window
 if (m_view_aftstr_strk1_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetAftstrStrk1Map(true), GetAftstrStrk1Map(false), 1.0, 2000.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(), //x unit
    MLL::GetString(IDS_MAPS_STROKE_UNIT).c_str(),      //y unit
    MLL::GetString(IDS_AFTSTR_STRK1_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.0f"));
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0"));
  DLL::Chart2DSetOnChange(md.handle,OnChangeAftstrStrk1Map,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseAftstrStrk1Map,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationAftstrStrk1Map, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_AFTSTR_STRK1);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewGrValDelMap()
{
 MapData &md = m_md[TYPE_MAP_GRVDELAY];
 //If button was released, then close editor's window
 if (m_view_grvaldel_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetGrValDelMap(true), GetGrValDelMap(false),0.0, 120.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_GRVDELAY_UNIT).c_str(),
    MLL::GetString(IDS_GRVDELAY_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(md.handle,OnChangeGrValDelMap,this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseGrValDelMap,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationGrValDelMap, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_GRVDELAY);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewFtlsCurveMap()
{
 MapData &md = m_md[TYPE_MAP_FTLS_CURVE];
 //If button was released, then close editor's window
 if (m_view_ftls_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetFtlsCurveMap(true), GetFtlsCurveMap(false), 0.0, 100.0, NULL, 17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_FUELTANK_UNIT).c_str(),
    MLL::GetString(IDS_FTLS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeFtlsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeFtlsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseFtlsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationFtlsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, GetFtlsCurveMap(false)[17], GetFtlsCurveMap(false)[17+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_FTLS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewFtlsCorMap()
{
 MapData &md = m_md[TYPE_MAP_FTLSCOR];
 //If button was released, then close editor's window
 if (m_view_ftlscor_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetFtlsCorMap(true),GetFtlsCorMap(false), 0.0, 3.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_FTLSCOR_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.01f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetOnChange(md.handle,OnChangeFtlsCorTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseFtlsCorTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationFtlsCorTable, this);
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_FTLSCOR);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewEgtsCurveMap()
{
 MapData &md = m_md[TYPE_MAP_EGTS_CURVE];
 //If button was released, then close editor's window
 if (m_view_egts_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetEgtsCurveMap(true), GetEgtsCurveMap(false), 0.0, 1100.0, NULL, 17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_EGTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.0"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeEgtsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeEgtsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseEgtsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationEgtsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, GetEgtsCurveMap(false)[17], GetEgtsCurveMap(false)[17+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_EGTS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewOpsCurveMap()
{
 MapData &md = m_md[TYPE_MAP_OPS_CURVE];
 //If button was released, then close editor's window
 if (m_view_ops_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetOpsCurveMap(true), GetOpsCurveMap(false), 0.0, 6.0, NULL, 17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_PRESSUREKG_UNIT).c_str(),
    MLL::GetString(IDS_OPS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeOpsCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(md.handle, OnChangeOpsCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle, OnCloseOpsCurveTable, this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationOpsCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, GetOpsCurveMap(false)[17], GetOpsCurveMap(false)[17+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_OPS_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewManInjPwcMap()
{
 MapData &md = m_md[TYPE_MAP_MANINJPWC];
 //If button was released, then close editor's window
 if (m_view_maninjpwc_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetManInjPwcMap(true),GetManInjPwcMap(false),0.5,1.5,SECU3IO::maninjpwc_map_slots,17,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_MANINJPWC_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.03f"));
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.000"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnChange(md.handle,OnChangeManInjPwcTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseManInjPwcTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationManInjPwcTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_MANINJPWC);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnViewMAFCurveMap()
{
 MapData &md = m_md[TYPE_MAP_MAF_CURVE];
 //If button was released, then close editor's window
 if (m_view_mafcurve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(md.handle,WM_CLOSE,0,0);
  return;
 }

 if ((!md.state)&&(DLL::Chart2DCreate))
 {
  md.state = 1;
  md.handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetMAFCurveMap(true),GetMAFCurveMap(false), 0.0, 650.0, /*m_mafcurve_slots*/NULL, 64,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAF_UNIT).c_str(),
    MLL::GetString(IDS_MAF_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(md.handle, 1, _T("%.03f"));
  DLL::Chart2DSetAxisEdits(md.handle, 1, true, 0, 5.00f, 0, 5.00f, 0.01f, -1, -1, OnChangeMAFCurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(md.handle, 1, NULL, NULL);
  DLL::Chart2DSetPtValuesFormat(md.handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(md.handle, md.ptMovStep);
  DLL::Chart2DSetOnChange(md.handle,OnChangeMAFCurveTable,this);
  DLL::Chart2DSetOnChangeSettings(md.handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(md.handle,OnCloseMAFCurveTable,this);
  DLL::Chart2DSetOnWndActivation(md.handle, OnWndActivationMAFCurveTable, this);
  DLL::Chart2DUpdate(md.handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(md.handle, 1, GetMAFCurveMap(false)[64+1], GetMAFCurveMap(false)[64+2]);

  //let controller to know about opening of this window
  OnOpenMapWnd(md.handle, TYPE_MAP_MAF_CURVE);

  DLL::Chart2DShow(md.handle, true);
 }
 else
 {
  ::SetFocus(md.handle);
 }
}

void CTablesSetPanel::OnDwellCalcButton()
{
 CDwellCalcDlg dialog;
 dialog.SetVoltageValues(SECU3IO::dwellcntrl_map_slots, 32);
 dialog.SetLimits(0.25f, 16.0f);
 if (dialog.DoModal()==IDOK)
 {
  //copy results
  std::copy(dialog.GetTimeValues(), dialog.GetTimeValues() + 32, GetDwellCntrlMap(false));
  if (DLL::Chart2DCreate)
  {//Update charts and notify about changes
   UpdateOpenedCharts();
   if (this->m_OnMapChanged)
    this->m_OnMapChanged(TYPE_MAP_DWELLCNTRL);
  }
 }
}

void CTablesSetPanel::OnRPMGridButton()
{
 if (m_OnRPMGridEditButton)
  m_OnRPMGridEditButton();
}

void CTablesSetPanel::OnFwConstsButton()
{
 if (m_OnFwConstsButton)
  m_OnFwConstsButton();
}

void CTablesSetPanel::OnCESettingsButton()
{
 if (m_OnCESettingsButton)
  m_OnCESettingsButton();
 m_edit_cesettings_btn.SetCheck(BST_UNCHECKED);
}

float* CTablesSetPanel::GetAttenuatorMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_ATTENUATOR].original;
 else
  return m_md[TYPE_MAP_ATTENUATOR].active;
}

float* CTablesSetPanel::GetDwellCntrlMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_DWELLCNTRL].original;
 else
  return m_md[TYPE_MAP_DWELLCNTRL].active;
}

float* CTablesSetPanel::GetCTSCurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_CTS_CURVE].original;
 else
  return m_md[TYPE_MAP_CTS_CURVE].active;
}

float* CTablesSetPanel::GetATSCurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_ATS_CURVE].original;
 else
  return m_md[TYPE_MAP_ATS_CURVE].active;
}

float* CTablesSetPanel::GetATSAACMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_ATS_CORR].original;
 else
  return m_md[TYPE_MAP_ATS_CORR].active;
}

void CTablesSetPanel::SetCTSXAxisEdits(float i_begin, float i_end)
{
 m_cts_curve_x_axis_limits[0] = i_begin;
 m_cts_curve_x_axis_limits[1] = i_end;	
}

void CTablesSetPanel::SetATSXAxisEdits(float i_begin, float i_end)
{
 m_ats_curve_x_axis_limits[0] = i_begin;
 m_ats_curve_x_axis_limits[1] = i_end;	
}

float* CTablesSetPanel::GetGasdosePosMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_GASDOSE].original;
 else
  return m_md[TYPE_MAP_GASDOSE].active;
}

float* CTablesSetPanel::GetBarocorrMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_BAROCORR].original;
 else
  return m_md[TYPE_MAP_BAROCORR].active;
}

float* CTablesSetPanel::GetManIgntimMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_MANIGNTIM].original;
 else
  return m_md[TYPE_MAP_MANIGNTIM].active;
}

float* CTablesSetPanel::GetTmp2CurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_TMP2_CURVE].original;
 else
  return m_md[TYPE_MAP_TMP2_CURVE].active;
}

float* CTablesSetPanel::GetGrtsCurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_GRTS_CURVE].original;
 else
  return m_md[TYPE_MAP_GRTS_CURVE].active;
}

float* CTablesSetPanel::GetCrkTempMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_CRKCLT_CORR].original;
 else
  return m_md[TYPE_MAP_CRKCLT_CORR].active;
}

float* CTablesSetPanel::GetEHPauseMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_EH_PAUSE].original;
 else
  return m_md[TYPE_MAP_EH_PAUSE].active;
}

float* CTablesSetPanel::GetGrHeatDutyMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_GRHEAT_DUTY].original;
 else
  return m_md[TYPE_MAP_GRHEAT_DUTY].active;
}

float* CTablesSetPanel::GetCrankingThrdMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_CRANKING_THRD].original;
 else
  return m_md[TYPE_MAP_CRANKING_THRD].active;
}

float* CTablesSetPanel::GetCrankingTimeMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_CRANKING_TIME].original;
 else
  return m_md[TYPE_MAP_CRANKING_TIME].active;
}

float* CTablesSetPanel::GetSmapabanThrdMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_SMAPABAN_THRD].original;
 else
  return m_md[TYPE_MAP_SMAPABAN_THRD].active;
}

float* CTablesSetPanel::GetKnockZoneMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_KNOCK_ZONE].original;
 else
  return m_md[TYPE_MAP_KNOCK_ZONE].active;
}

float* CTablesSetPanel::GetLambdaZoneMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_LAMBDA_ZONE].original;
 else
  return m_md[TYPE_MAP_LAMBDA_ZONE].active;
}

float* CTablesSetPanel::GetPwmIacUCoefMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_PWMIAC_UCOEF].original;
 else
  return m_md[TYPE_MAP_PWMIAC_UCOEF].active;
}

float* CTablesSetPanel::GetAftstrStrk0Map(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_AFTSTR_STRK0].original;
 else
  return m_md[TYPE_MAP_AFTSTR_STRK0].active;
}

float* CTablesSetPanel::GetAftstrStrk1Map(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_AFTSTR_STRK1].original;
 else
  return m_md[TYPE_MAP_AFTSTR_STRK1].active;
}

float* CTablesSetPanel::GetGrValDelMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_GRVDELAY].original;
 else
  return m_md[TYPE_MAP_GRVDELAY].active;
}

float* CTablesSetPanel::GetFtlsCurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_FTLS_CURVE].original;
 else
  return m_md[TYPE_MAP_FTLS_CURVE].active;
}

float* CTablesSetPanel::GetFtlsCorMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_FTLSCOR].original;
 else
  return m_md[TYPE_MAP_FTLSCOR].active;
}

float* CTablesSetPanel::GetEgtsCurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_EGTS_CURVE].original;
 else
  return m_md[TYPE_MAP_EGTS_CURVE].active;
}

float* CTablesSetPanel::GetOpsCurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_OPS_CURVE].original;
 else
  return m_md[TYPE_MAP_OPS_CURVE].active;
}

float* CTablesSetPanel::GetManInjPwcMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_MANINJPWC].original;
 else
  return m_md[TYPE_MAP_MANINJPWC].active;
}

float* CTablesSetPanel::GetMAFCurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_MAF_CURVE].original;
 else
  return m_md[TYPE_MAP_MAF_CURVE].active;
}

HWND CTablesSetPanel::GetMapWindow(int wndType)
{
 HWND hwnd = Super::GetMapWindow(wndType);
 if (hwnd != NULL)
  return hwnd;

 if (wndType >= TYPE_MAP_SEP_START && wndType <= TYPE_MAP_SEP_END)
  return m_md[wndType].state ? m_md[wndType].handle : NULL;
 else
  return NULL;
}

bool CTablesSetPanel::IsAllowed(void)
{ //change behavior of base class
 return Super::IsAllowed() && m_funset_listbox.GetSelectedCount();
}

void CTablesSetPanel::setOnFunSetSelectionChanged(EventWithCode OnFunction)
{m_OnFunSetSelectionChanged = OnFunction;}

void CTablesSetPanel::setOnFunSetNamechanged(EventWithCodeAndString OnFunction)
{m_OnFunSetNamechanged = OnFunction;}

void CTablesSetPanel::setOnCTSXAxisEditChanged(EventWithCodeAndFloat OnFunction)
{m_OnCTSXAxisEditChanged = OnFunction;}

void CTablesSetPanel::setOnATSXAxisEditChanged(EventWithCodeAndFloat OnFunction)
{m_OnATSXAxisEditChanged = OnFunction;}

void CTablesSetPanel::setOnRPMGridEditButton(EventHandler OnFunction)
{m_OnRPMGridEditButton = OnFunction;}

void CTablesSetPanel::setOnFwConstsButton(EventHandler OnFunction)
{m_OnFwConstsButton = OnFunction;}

void CTablesSetPanel::setOnCESettingsButton(EventHandler OnFunction)
{m_OnCESettingsButton = OnFunction;}

void CTablesSetPanel::CloseCharts(void)
{
 for(int i = TYPE_MAP_ALL_START; i <= TYPE_MAP_ALL_END; ++i)
 {
  HWND hwnd = GetMapWindow(i);
  if (hwnd)
   ::SendMessage(hwnd, WM_CLOSE, 0, 0);
 }
}

void CTablesSetPanel::ShowOpenedCharts(bool i_show)
{
 for(int i = TYPE_MAP_ALL_START; i <= TYPE_MAP_ALL_END; ++i)
 {
  HWND hwnd = GetMapWindow(i);
  if (hwnd)
   ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 }
}
