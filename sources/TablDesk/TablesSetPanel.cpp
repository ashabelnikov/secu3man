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

const UINT CTablesSetPanel::IDD = IDD_TD_ALLTABLES_PANEL;

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel dialog

CTablesSetPanel::CTablesSetPanel(CWnd* pParent /*= NULL*/)
: Super(CTablesSetPanel::IDD, pParent)
, m_dwellcntrl_enabled(false)
, m_cts_curve_enabled(false)
, m_tmp2_curve_enabled(false)
{
 m_scrl_view = 815;

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
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
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

//Updates controls which state depends on whether or not data is
void CTablesSetPanel::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool enabled = m_IsAllowed ? m_IsAllowed() : false;
 pCmdUI->Enable(enabled ? TRUE : FALSE);
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

void CTablesSetPanel::SetFunSetListBox(std::vector<_TSTRING> i_list_of_names)
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
 //If button was released, then close editor's window
 if (m_view_attenuator_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_ATTENUATOR].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_ATTENUATOR].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_ATTENUATOR].state = 1;
  m_md[TYPE_MAP_ATTENUATOR].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetAttenuatorMap(true),GetAttenuatorMap(false),0.0f,63,m_attenuator_table_slots,128,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ATTENUATOR_GAIN_UNIT).c_str(),
    MLL::GetString(IDS_ATTENUATOR_MAP).c_str(), false);
  DLL::Chart2DSetMarksVisible(m_md[TYPE_MAP_ATTENUATOR].handle,1, false); //пр€чем надписи над узловыми точками функции
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_ATTENUATOR].handle,OnChangeAttenuatorTable, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_ATTENUATOR].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_ATTENUATOR].handle,OnCloseAttenuatorTable, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_ATTENUATOR].handle, OnWndActivationAttenuatorTable, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_ATTENUATOR].handle, 0, OnGetYAxisLabel, this);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_ATTENUATOR].handle, 0, _T("#0.00"));
  DLL::Chart2DInverseAxis(m_md[TYPE_MAP_ATTENUATOR].handle, 0, true);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_ATTENUATOR].handle, m_md[TYPE_MAP_ATTENUATOR].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_ATTENUATOR].handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_ATTENUATOR].handle, TYPE_MAP_ATTENUATOR);

  DLL::Chart2DShow(m_md[TYPE_MAP_ATTENUATOR].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_ATTENUATOR].handle);
 }
}

void CTablesSetPanel::OnViewDwellCntrlMap()
{
 //If button was released, then close editor's window
 if (m_view_dwellcntrl_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_DWELLCNTRL].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_DWELLCNTRL].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_DWELLCNTRL].state = 1;
  m_md[TYPE_MAP_DWELLCNTRL].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetDwellCntrlMap(true), GetDwellCntrlMap(false), 0.25f, 16.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DWELLCNTRL_UNIT).c_str(),
    MLL::GetString(IDS_DWELLCNTRL_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_DWELLCNTRL].handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_DWELLCNTRL].handle, OnChangeDwellCntrlTable, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_DWELLCNTRL].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_DWELLCNTRL].handle, OnCloseDwellCntrlTable, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_DWELLCNTRL].handle, OnWndActivationDwellCntrlTable, this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_DWELLCNTRL].handle, m_md[TYPE_MAP_DWELLCNTRL].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DWELLCNTRL].handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_DWELLCNTRL].handle, TYPE_MAP_DWELLCNTRL);

  DLL::Chart2DShow(m_md[TYPE_MAP_DWELLCNTRL].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_DWELLCNTRL].handle);
 }
}

void CTablesSetPanel::OnViewCTSCurveMap()
{
 //If button was released, then close editor's window
 if (m_view_cts_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_CTS_CURVE].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_CTS_CURVE].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_CTS_CURVE].state = 1;
  m_md[TYPE_MAP_CTS_CURVE].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCTSCurveMap(true), GetCTSCurveMap(false), -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_CTS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_CTS_CURVE].handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_CTS_CURVE].handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, -1, -1, OnChangeCTSXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_CTS_CURVE].handle, 1, OnGetXAxisLabel, this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_CTS_CURVE].handle, OnChangeCTSCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_CTS_CURVE].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_CTS_CURVE].handle, OnCloseCTSCurveTable, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_CTS_CURVE].handle, OnWndActivationCTSCurveTable, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_CTS_CURVE].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_CTS_CURVE].handle, m_md[TYPE_MAP_CTS_CURVE].ptMovStep);
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_CTS_CURVE].handle, 1, m_cts_curve_x_axis_limits[0], m_cts_curve_x_axis_limits[1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_CTS_CURVE].handle, TYPE_MAP_CTS_CURVE);

  DLL::Chart2DShow(m_md[TYPE_MAP_CTS_CURVE].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_CTS_CURVE].handle);
 }
}

void CTablesSetPanel::OnViewATSCurveMap()
{
 //If button was released, then close editor's window
 if (m_view_ats_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_ATS_CURVE].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_ATS_CURVE].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_ATS_CURVE].state = 1;
  m_md[TYPE_MAP_ATS_CURVE].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetATSCurveMap(true), GetATSCurveMap(false), -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_ATS_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_ATS_CURVE].handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_ATS_CURVE].handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, -1, -1, OnChangeATSXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_ATS_CURVE].handle, 1, OnGetXAxisLabel, this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_ATS_CURVE].handle, OnChangeATSCurveTable, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_ATS_CURVE].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_ATS_CURVE].handle, OnCloseATSCurveTable, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_ATS_CURVE].handle, OnWndActivationATSCurveTable, this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_ATS_CURVE].handle, m_md[TYPE_MAP_ATS_CURVE].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_ATS_CURVE].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_ATS_CURVE].handle, 1, m_ats_curve_x_axis_limits[0], m_ats_curve_x_axis_limits[1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_ATS_CURVE].handle, TYPE_MAP_ATS_CURVE);

  DLL::Chart2DShow(m_md[TYPE_MAP_ATS_CURVE].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_ATS_CURVE].handle);
 }
}

void CTablesSetPanel::OnViewATSAACMap()
{
 //If button was released, then close editor's window
 if (m_view_ats_aac_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_ATS_CORR].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_ATS_CORR].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_ATS_CORR].state = 1;
  m_md[TYPE_MAP_ATS_CORR].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetATSAACMap(true),GetATSAACMap(false),-15.0,25.0,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_ATSCORR_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_ATS_CORR].handle,OnChangeATSAACTable,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_ATS_CORR].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_ATS_CORR].handle,OnCloseATSAACTable,this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_ATS_CORR].handle, OnWndActivationATSAACTable, this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_ATS_CORR].handle, m_md[TYPE_MAP_ATS_CORR].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_ATS_CORR].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_ATS_CORR].handle, TYPE_MAP_ATS_CORR);

  DLL::Chart2DShow(m_md[TYPE_MAP_ATS_CORR].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_ATS_CORR].handle);
 }
}

void CTablesSetPanel::OnViewGasdosePosMap()
{
 //if button has been turned off, then close editor's window
 if (m_view_gasdose_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_GASDOSE].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_GASDOSE].state)&&(DLL::Chart3DCreate))
 {
  m_md[TYPE_MAP_GASDOSE].state = 1;
  m_md[TYPE_MAP_GASDOSE].handle = DLL::Chart3DCreate(_ChartParentHwnd(), GetGasdosePosMap(true),GetGasdosePosMap(false),GetRPMGrid(),16,16,0,100.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_GDP_UNIT).c_str(),
    MLL::GetString(IDS_GDP_MAP).c_str());
  DLL::Chart3DSetOnGetAxisLabel(m_md[TYPE_MAP_GASDOSE].handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_md[TYPE_MAP_GASDOSE].handle,OnChangeGasdoseTable,this);
  DLL::Chart3DSetOnChangeSettings(m_md[TYPE_MAP_GASDOSE].handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(m_md[TYPE_MAP_GASDOSE].handle,OnCloseGasdoseTable,this);
  DLL::Chart3DSetOnWndActivation(m_md[TYPE_MAP_GASDOSE].handle, OnWndActivationGasdoseTable, this);
  DLL::Chart3DSetPtMovingStep(m_md[TYPE_MAP_GASDOSE].handle, m_md[TYPE_MAP_GASDOSE].ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_GASDOSE].handle, TYPE_MAP_GASDOSE);

  DLL::Chart3DShow(m_md[TYPE_MAP_GASDOSE].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_GASDOSE].handle);
 }
}

void CTablesSetPanel::OnViewBarocorrMap()
{
 //If button was released, then close editor's window
 if (m_view_barocorr_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_BAROCORR].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_BAROCORR].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_BAROCORR].state = 1;
  m_md[TYPE_MAP_BAROCORR].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetBarocorrMap(true), GetBarocorrMap(false), 80.0, 120.0, NULL, 9,
    MLL::GetString(IDS_MAPS_ATMOPRESS_UNIT).c_str(),  //horizontal axis
    MLL::GetString(IDS_MAPS_COEFFP_UNIT).c_str(),     //vertical axis
    MLL::GetString(IDS_BAROCORR_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_BAROCORR].handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_BAROCORR].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_BAROCORR].handle, m_md[TYPE_MAP_BAROCORR].ptMovStep);
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_BAROCORR].handle, 1, true, 60.0f, 110.0f, 60.0f, 110.0f, 0.1f, 5, 1, OnChangeBarocorrXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_BAROCORR].handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_BAROCORR].handle, OnChangeBarocorrTable, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_BAROCORR].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_BAROCORR].handle, OnCloseBarocorrTable, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_BAROCORR].handle, OnWndActivationBarocorrTable, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_BAROCORR].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_BAROCORR].handle, 1, GetBarocorrMap(false)[9], GetBarocorrMap(false)[9+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_BAROCORR].handle, TYPE_MAP_BAROCORR);

  DLL::Chart2DShow(m_md[TYPE_MAP_BAROCORR].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_BAROCORR].handle);
 }
}

void CTablesSetPanel::OnViewTmp2CurveMap()
{
 //If button was released, then close editor's window
 if (m_view_tmp2_curve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_TMP2_CURVE].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_TMP2_CURVE].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_TMP2_CURVE].state = 1;
  m_md[TYPE_MAP_TMP2_CURVE].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetTmp2CurveMap(true), GetTmp2CurveMap(false), -40.0, 120.0, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_TMP2_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_TMP2_CURVE].handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_TMP2_CURVE].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_TMP2_CURVE].handle, m_md[TYPE_MAP_TMP2_CURVE].ptMovStep);
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_TMP2_CURVE].handle, 1, true, 0, 9.1f, 0, 9.1f, 0.01f, 5, 2, OnChangeTmp2CurveXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_TMP2_CURVE].handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_TMP2_CURVE].handle, OnChangeTmp2CurveTable, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_TMP2_CURVE].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_TMP2_CURVE].handle, OnCloseTmp2CurveTable, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_TMP2_CURVE].handle, OnWndActivationTmp2CurveTable, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_TMP2_CURVE].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_TMP2_CURVE].handle, 1, GetTmp2CurveMap(false)[16], GetTmp2CurveMap(false)[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_TMP2_CURVE].handle, TYPE_MAP_TMP2_CURVE);

  DLL::Chart2DShow(m_md[TYPE_MAP_TMP2_CURVE].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_TMP2_CURVE].handle);
 }
}

void CTablesSetPanel::OnViewManIgntimMap()
{
 //If button was released, then close editor's window
 if (m_view_manigntim_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_MANIGNTIM].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_MANIGNTIM].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_MANIGNTIM].state = 1;
  m_md[TYPE_MAP_MANIGNTIM].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetManIgntimMap(true),GetManIgntimMap(false),-15.0,15.0,SECU3IO::manigntim_map_slots,16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_MANIGNTIM_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_MANIGNTIM].handle, 1, _T("%.02f"));
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_MANIGNTIM].handle, _T("#0.0"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_MANIGNTIM].handle, m_md[TYPE_MAP_MANIGNTIM].ptMovStep);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_MANIGNTIM].handle,OnChangeManIgntimTable,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_MANIGNTIM].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_MANIGNTIM].handle,OnCloseManIgntimTable,this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_MANIGNTIM].handle, OnWndActivationManIgntimTable, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_MANIGNTIM].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_MANIGNTIM].handle, TYPE_MAP_MANIGNTIM);

  DLL::Chart2DShow(m_md[TYPE_MAP_MANIGNTIM].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_MANIGNTIM].handle);
 }
}

void CTablesSetPanel::OnViewCrkTempMap()
{
 //If button was released, then close editor's window
 if (m_view_crktemp_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_CRKCLT_CORR].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_CRKCLT_CORR].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_CRKCLT_CORR].state = 1;
  m_md[TYPE_MAP_CRKCLT_CORR].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCrkTempMap(true),GetCrkTempMap(false),-15.0,25.0,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_CRKTEMP_MAP).c_str(), false);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_CRKCLT_CORR].handle,OnChangeCrkTempTable,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_CRKCLT_CORR].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_CRKCLT_CORR].handle,OnCloseCrkTempTable,this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_CRKCLT_CORR].handle, OnWndActivationCrkTempTable, this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_CRKCLT_CORR].handle, m_md[TYPE_MAP_CRKCLT_CORR].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_CRKCLT_CORR].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_CRKCLT_CORR].handle, TYPE_MAP_CRKCLT_CORR);

  DLL::Chart2DShow(m_md[TYPE_MAP_CRKCLT_CORR].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_CRKCLT_CORR].handle);
 }
}

void CTablesSetPanel::OnViewEHPauseMap()
{
 //If button was released, then close editor's window
 if (m_view_eh_pause_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_EH_PAUSE].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_EH_PAUSE].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_EH_PAUSE].state = 1;
  m_md[TYPE_MAP_EH_PAUSE].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetEHPauseMap(true), GetEHPauseMap(false), 0.01f, 2.55f, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_EH_PAUSE_UNIT).c_str(),
    MLL::GetString(IDS_EH_PAUSE_MAP).c_str(), false);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_EH_PAUSE].handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_EH_PAUSE].handle, OnChangeEHPauseTable, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_EH_PAUSE].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_EH_PAUSE].handle, OnCloseEHPauseTable, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_EH_PAUSE].handle, OnWndActivationEHPauseTable, this);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_EH_PAUSE].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_EH_PAUSE].handle, m_md[TYPE_MAP_EH_PAUSE].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_EH_PAUSE].handle, NULL, NULL); //<--actuate changes

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_EH_PAUSE].handle, TYPE_MAP_EH_PAUSE);

  DLL::Chart2DShow(m_md[TYPE_MAP_EH_PAUSE].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_EH_PAUSE].handle);
 }
}

void CTablesSetPanel::OnViewCrankingThrdMap()
{
 //if button was released, then close editor's window
 if (m_view_cranking_thrd_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_CRANKING_THRD].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_CRANKING_THRD].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_CRANKING_THRD].state = 1;
  m_md[TYPE_MAP_CRANKING_THRD].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCrankingThrdMap(true),GetCrankingThrdMap(false),0.0f,2500.0f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_CRANKING_THRD_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_CRANKING_THRD].handle, _T("#0"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_CRANKING_THRD].handle, m_md[TYPE_MAP_CRANKING_THRD].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_CRANKING_THRD].handle,OnWndActivationCrankingThrdMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_CRANKING_THRD].handle,OnChangeCrankingThrdMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_CRANKING_THRD].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_CRANKING_THRD].handle,OnCloseCrankingThrdMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_CRANKING_THRD].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_CRANKING_THRD].handle, TYPE_MAP_CRANKING_THRD);

  DLL::Chart2DShow(m_md[TYPE_MAP_CRANKING_THRD].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_CRANKING_THRD].handle);
 }
}

void CTablesSetPanel::OnViewCrankingTimeMap()
{
 //if button was released, then close editor's window
 if (m_view_cranking_time_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_CRANKING_TIME].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_CRANKING_TIME].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_CRANKING_TIME].state = 1;
  m_md[TYPE_MAP_CRANKING_TIME].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCrankingTimeMap(true),GetCrankingTimeMap(false),0.0f,255.0f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_STROKE_UNIT).c_str(),
    MLL::GetString(IDS_CRANKING_TIME_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_CRANKING_TIME].handle, _T("#0"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_CRANKING_TIME].handle, m_md[TYPE_MAP_CRANKING_TIME].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_CRANKING_TIME].handle,OnWndActivationCrankingTimeMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_CRANKING_TIME].handle,OnChangeCrankingTimeMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_CRANKING_TIME].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_CRANKING_TIME].handle,OnCloseCrankingTimeMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_CRANKING_TIME].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_CRANKING_TIME].handle, TYPE_MAP_CRANKING_TIME);

  DLL::Chart2DShow(m_md[TYPE_MAP_CRANKING_TIME].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_CRANKING_TIME].handle);
 }
}

void CTablesSetPanel::OnViewSmapabanThrdMap()
{
 //if button was released, then close editor's window
 if (m_view_smapaban_thrd_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_SMAPABAN_THRD].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_SMAPABAN_THRD].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_SMAPABAN_THRD].state = 1;
  m_md[TYPE_MAP_SMAPABAN_THRD].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetSmapabanThrdMap(true),GetSmapabanThrdMap(false),0.0f,2500.0f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_SMAPABAN_THRD_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_SMAPABAN_THRD].handle, _T("#0"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_SMAPABAN_THRD].handle, m_md[TYPE_MAP_SMAPABAN_THRD].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_SMAPABAN_THRD].handle,OnWndActivationSmapabanThrdMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_SMAPABAN_THRD].handle,OnChangeSmapabanThrdMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_SMAPABAN_THRD].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_SMAPABAN_THRD].handle,OnCloseSmapabanThrdMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_SMAPABAN_THRD].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_SMAPABAN_THRD].handle, TYPE_MAP_SMAPABAN_THRD);

  DLL::Chart2DShow(m_md[TYPE_MAP_SMAPABAN_THRD].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_SMAPABAN_THRD].handle);
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
