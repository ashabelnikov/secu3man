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

/** \file ButtonsPanel.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "ButtonsPanel.h"
#include "common/MathHelpers.h"
#include "DLLLinkedFunctions.h"
#include "GridModeEditorDlg.h"
#include "io-core/secu3io.h"
#include "MapIds.h"
#include "ui-core/WndScroller.h"

#define TIMER_ID 0

void __cdecl CButtonsPanel::OnChangeStartMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_DA_START);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

void __cdecl CButtonsPanel::OnCloseStartMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_start_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_start_map_wnd_handle, TYPE_MAP_DA_START);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIdleMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_DA_IDLE);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

void __cdecl CButtonsPanel::OnCloseIdleMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_idle_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_idle_map_wnd_handle, TYPE_MAP_DA_IDLE);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeWorkMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_DA_WORK);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseWorkMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_work_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_work_map_wnd_handle, TYPE_MAP_DA_WORK);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeTempMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_DA_TEMP_CORR);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseTempMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_temp_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_temp_map_wnd_handle, TYPE_MAP_DA_TEMP_CORR);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnGetXAxisLabelRPM(LPTSTR io_label_string, int index, void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }
 _stprintf(io_label_string, _T("%d"), MathHelpers::Round(_this->GetRPMGrid()[index]));
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationStartMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_start_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationIdleMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_idle_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationWorkMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_work_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationTempMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_temp_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeVEMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_VE);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseVEMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_ve_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_ve_map_wnd_handle, TYPE_MAP_INJ_VE);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeAFRMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_AFR);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseAFRMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_afr_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_afr_map_wnd_handle, TYPE_MAP_INJ_AFR);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeCrnkMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_CRNK);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseCrnkMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_crnk_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_crnk_map_wnd_handle, TYPE_MAP_INJ_CRNK);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeWrmpMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_WRMP);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseWrmpMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_wrmp_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_wrmp_map_wnd_handle, TYPE_MAP_INJ_WRMP);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeDeadMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_DEAD);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseDeadMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_dead_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_dead_map_wnd_handle, TYPE_MAP_INJ_DEAD);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIdlrMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IDLR);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseIdlrMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_idlr_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_idlr_map_wnd_handle, TYPE_MAP_INJ_IDLR);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIdlcMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IDLC);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseIdlcMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_idlc_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_idlc_map_wnd_handle, TYPE_MAP_INJ_IDLC);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeAETPSMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_AETPS);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseAETPSMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_aetps_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_aetps_map_wnd_handle, TYPE_MAP_INJ_AETPS);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeAERPMMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_AERPM);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseAERPMMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_aerpm_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_aerpm_map_wnd_handle, TYPE_MAP_INJ_AERPM);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeAftstrMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_AFTSTR);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseAftstrMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_aftstr_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_aftstr_map_wnd_handle, TYPE_MAP_INJ_AFTSTR);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeITMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IT);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseITMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_it_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_it_map_wnd_handle, TYPE_MAP_INJ_IT);
}



//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeITRPMMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_ITRPM);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseITRPMMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_itrpm_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_itrpm_map_wnd_handle, TYPE_MAP_INJ_ITRPM);
}


//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeRigidMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_RIGID);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseRigidMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_rigid_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_rigid_map_wnd_handle, TYPE_MAP_INJ_RIGID);
}


//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeEGOCrvMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_EGOCRV);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseEGOCrvMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_egocrv_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_egocrv_map_wnd_handle, TYPE_MAP_INJ_EGOCRV);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeEGOCrvXAxisEdit(void* i_param, int i_type, float i_value)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
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
  _this->GetEGOCurveMap(false)[16 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_EGOCRV);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIACCMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IACC);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseIACCMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_iacc_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_iacc_map_wnd_handle, TYPE_MAP_INJ_IACC);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIACCXAxisEdit(void* i_param, int i_type, float i_value)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
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
  _this->GetIACCMap(false)[8 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IACC);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIACCWMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IACCW);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseIACCWMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_iaccw_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_iaccw_map_wnd_handle, TYPE_MAP_INJ_IACCW);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIACCWXAxisEdit(void* i_param, int i_type, float i_value)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
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
  _this->GetIACCWMap(false)[16 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IACCW);
 if (_this->mp_gridModeEditorDlg.get())
  _this->mp_gridModeEditorDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationITMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_it_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationVEMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_ve_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationAFRMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_afr_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationCrnkMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_crnk_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationWrmpMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_wrmp_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationDeadMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_dead_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationIdlrMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_idlr_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationIdlcMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_idlc_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationAETPSMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_aetps_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationAERPMMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_aerpm_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationAftstrMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_aftstr_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationITRPMMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_itrpm_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationRigidMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_rigid_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationEGOCrvMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_egocrv_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationIACCMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_iacc_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationIACCWMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 if (_this->m_OnWndActivation)
  _this->m_OnWndActivation(_this->m_iaccw_map_wnd_handle, cmd);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapChanged(int mapType)
{
 if (m_start_map_chart_state && mapType == TYPE_MAP_DA_START)
  DLL::Chart2DUpdate(m_start_map_wnd_handle, GetStartMap(true), GetStartMap(false));
 if (m_idle_map_chart_state && mapType == TYPE_MAP_DA_IDLE)
  DLL::Chart2DUpdate(m_idle_map_wnd_handle, GetIdleMap(true), GetIdleMap(false));
 if (m_work_map_chart_state && mapType == TYPE_MAP_DA_WORK)
  DLL::Chart3DUpdate(m_work_map_wnd_handle, GetWorkMap(true), GetWorkMap(false));
 if (m_temp_map_chart_state && mapType == TYPE_MAP_DA_TEMP_CORR)
  DLL::Chart2DUpdate(m_temp_map_wnd_handle, GetTempMap(true), GetTempMap(false));
 if (m_ve_map_chart_state && mapType == TYPE_MAP_INJ_VE)
  DLL::Chart3DUpdate(m_ve_map_wnd_handle, GetVEMap(true), GetVEMap(false));
 if (m_afr_map_chart_state && mapType == TYPE_MAP_INJ_AFR)
  DLL::Chart3DUpdate(m_afr_map_wnd_handle, GetAFRMap(true), GetAFRMap(false));
 if (m_crnk_map_chart_state && mapType == TYPE_MAP_INJ_CRNK)
  DLL::Chart2DUpdate(m_crnk_map_wnd_handle, GetCrnkMap(true), GetCrnkMap(false));
 if (m_wrmp_map_chart_state && mapType == TYPE_MAP_INJ_WRMP)
  DLL::Chart2DUpdate(m_wrmp_map_wnd_handle, GetWrmpMap(true), GetWrmpMap(false));
 if (m_dead_map_chart_state && mapType == TYPE_MAP_INJ_DEAD)
  DLL::Chart2DUpdate(m_dead_map_wnd_handle, GetDeadMap(true), GetDeadMap(false));
 if (m_idlr_map_chart_state && mapType == TYPE_MAP_INJ_IDLR)
  DLL::Chart2DUpdate(m_idlr_map_wnd_handle, GetIdlrMap(true), GetIdlrMap(false));
 if (m_idlc_map_chart_state && mapType == TYPE_MAP_INJ_IDLC)
  DLL::Chart2DUpdate(m_idlc_map_wnd_handle, GetIdlcMap(true), GetIdlcMap(false));
 if (m_aetps_map_chart_state && mapType == TYPE_MAP_INJ_AETPS)
  DLL::Chart2DUpdate(m_aetps_map_wnd_handle, GetAETPSMap(true), GetAETPSMap(false));
 if (m_aerpm_map_chart_state && mapType == TYPE_MAP_INJ_AERPM)
  DLL::Chart2DUpdate(m_aerpm_map_wnd_handle, GetAERPMMap(true), GetAERPMMap(false));
 if (m_aftstr_map_chart_state && mapType == TYPE_MAP_INJ_AFTSTR)
  DLL::Chart2DUpdate(m_aftstr_map_wnd_handle, GetAftstrMap(true), GetAftstrMap(false));
 if (m_it_map_chart_state && mapType == TYPE_MAP_INJ_IT)
  DLL::Chart3DUpdate(m_it_map_wnd_handle, GetITMap(true), GetITMap(false));
 if (m_itrpm_map_chart_state && mapType == TYPE_MAP_INJ_ITRPM)
  DLL::Chart2DUpdate(m_itrpm_map_wnd_handle, GetITRPMMap(true), GetITRPMMap(false));
 if (m_rigid_map_chart_state && mapType == TYPE_MAP_INJ_RIGID)
  DLL::Chart2DUpdate(m_rigid_map_wnd_handle, GetRigidMap(true), GetRigidMap(false));
 if (m_egocrv_map_chart_state && mapType == TYPE_MAP_INJ_EGOCRV)
  DLL::Chart2DUpdate(m_rigid_map_wnd_handle, GetEGOCurveMap(true), GetEGOCurveMap(false));
 if (m_iacc_map_chart_state && mapType == TYPE_MAP_INJ_IACC)
  DLL::Chart2DUpdate(m_iacc_map_wnd_handle, GetIACCMap(true), GetIACCMap(false));
 if (m_iaccw_map_chart_state && mapType == TYPE_MAP_INJ_IACCW)
  DLL::Chart2DUpdate(m_iaccw_map_wnd_handle, GetIACCWMap(true), GetIACCWMap(false));

 if (m_OnMapChanged)
  m_OnMapChanged(mapType);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapClosed(HWND hwnd, int mapType)
{
 m_grid_map_state = 0;
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(mp_gridModeEditorDlg->m_hWnd, TYPE_MAP_GME_WND);
}

//------------------------------------------------------------------------

//const UINT CButtonsPanel::IDD = IDD_TD_BUTTONS_PANEL; //WTF?

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel dialog

CButtonsPanel::CButtonsPanel(UINT dialog_id, CWnd* pParent /*=NULL*/)
: Super(dialog_id, pParent)
, m_work_map_chart_state(0)
, m_temp_map_chart_state(0)
, m_start_map_chart_state(0)
, m_idle_map_chart_state(0)
, m_ve_map_chart_state(0)
, m_afr_map_chart_state(0)
, m_crnk_map_chart_state(0)
, m_wrmp_map_chart_state(0)
, m_dead_map_chart_state(0)
, m_idlr_map_chart_state(0)
, m_idlc_map_chart_state(0)
, m_aetps_map_chart_state(0)
, m_aerpm_map_chart_state(0)
, m_aftstr_map_chart_state(0)
, m_it_map_chart_state(0)
, m_itrpm_map_chart_state(0)
, m_rigid_map_chart_state(0)
, m_egocrv_map_chart_state(0)
, m_iacc_map_chart_state(0)
, m_iaccw_map_chart_state(0)
, m_grid_map_state(0)
, m_start_map_wnd_handle(NULL)
, m_idle_map_wnd_handle(NULL)
, m_work_map_wnd_handle(NULL)
, m_temp_map_wnd_handle(NULL)
, m_ve_map_wnd_handle(NULL)
, m_afr_map_wnd_handle(NULL)
, m_crnk_map_wnd_handle(NULL)
, m_wrmp_map_wnd_handle(NULL)
, m_dead_map_wnd_handle(NULL)
, m_idlr_map_wnd_handle(NULL)
, m_idlc_map_wnd_handle(NULL)
, m_aetps_map_wnd_handle(NULL)
, m_aerpm_map_wnd_handle(NULL)
, m_aftstr_map_wnd_handle(NULL)
, m_it_map_wnd_handle(NULL)
, m_itrpm_map_wnd_handle(NULL)
, m_rigid_map_wnd_handle(NULL)
, m_egocrv_map_wnd_handle(NULL)
, m_iacc_map_wnd_handle(NULL)
, m_iaccw_map_wnd_handle(NULL)
, m_charts_enabled(-1)
, IDD(IDD_TD_BUTTONS_PANEL)
, m_en_aa_indication(false)
, mp_scr(new CWndScroller)
, m_scrl_factor(3.2f)
, m_fuel_injection(false)
, m_gasdose(false)
{
 memset(m_start_map_active, 0, 16 * sizeof(float));
 memset(m_start_map_original, 0, 16 * sizeof(float));
 memset(m_idle_map_active, 0, 16 * sizeof(float));
 memset(m_idle_map_original, 0, 16 * sizeof(float));
 memset(m_work_map_active, 0, 16 * 16 * sizeof(float));
 memset(m_work_map_original, 0, 16 * 16 * sizeof(float));
 memset(m_temp_map_active, 0, 16 * sizeof(float));
 memset(m_temp_map_original, 0, 16 * sizeof(float));
 memset(m_rpm_grid_values, 0, 16 * sizeof(float));
 memset(m_ve_map_active, 0, 16 * 16 * sizeof(float));
 memset(m_ve_map_original, 0, 16 * 16 * sizeof(float));
 memset(m_afr_map_active, 0, 16 * 16 * sizeof(float));
 memset(m_afr_map_original, 0, 16 * 16 * sizeof(float));
 memset(m_crnk_map_active, 0, 16 * sizeof(float));
 memset(m_crnk_map_original, 0, 16 * sizeof(float));
 memset(m_wrmp_map_active, 0, 16 * sizeof(float));
 memset(m_wrmp_map_original, 0, 16 * sizeof(float));
 memset(m_dead_map_active, 0, 32 * sizeof(float));
 memset(m_dead_map_original, 0, 32 * sizeof(float));
 memset(m_idlr_map_active, 0, 16 * sizeof(float));
 memset(m_idlr_map_original, 0, 16 * sizeof(float));
 memset(m_idlc_map_active, 0, 16 * sizeof(float));
 memset(m_idlc_map_original, 0, 16 * sizeof(float));
 memset(m_aetps_map_active, 0, 8 * sizeof(float));
 memset(m_aetps_map_original, 0, 8 * sizeof(float));
 memset(m_aerpm_map_active, 0, 8 * sizeof(float));
 memset(m_aerpm_map_original, 0, 8 * sizeof(float));
 memset(m_aftstr_map_active, 0, 16 * sizeof(float));
 memset(m_aftstr_map_original, 0, 16 * sizeof(float));
 memset(m_it_map_active, 0, 16 * 16 * sizeof(float));
 memset(m_it_map_original, 0, 16 * 16 * sizeof(float));
 memset(m_itrpm_map_active, 0, 16 * sizeof(float));
 memset(m_itrpm_map_original, 0, 16 * sizeof(float));
 memset(m_rigid_map_active, 0, 8 * sizeof(float));
 memset(m_rigid_map_original, 0, 8 * sizeof(float));
 memset(m_egocrv_map_active, 0, (16+2) * sizeof(float));
 memset(m_egocrv_map_original, 0, (16+2) * sizeof(float));
 memset(m_iacc_map_active, 0, (8+2) * sizeof(float));
 memset(m_iacc_map_original, 0, (8+2) * sizeof(float));
 memset(m_iaccw_map_active, 0, (16+2) * sizeof(float));
 memset(m_iaccw_map_original, 0, (16+2) * sizeof(float));
}

CButtonsPanel::~CButtonsPanel()
{
 //empty
}

void CButtonsPanel::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_VIEW_WORK_MAP,  m_view_work_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_TEMP_MAP,  m_view_temp_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_START_MAP, m_view_start_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_IDLE_MAP,  m_view_idle_map_btn);
 DDX_Control(pDX, IDC_TD_GME_CHECK, m_grid_mode_editing_check);
 DDX_Control(pDX, IDC_TD_VIEW_VE_MAP,  m_view_ve_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_AFR_MAP,  m_view_afr_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_CRNK_MAP,  m_view_crnk_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_WRMP_MAP,  m_view_wrmp_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_DEAD_MAP,  m_view_dead_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_IDLR_MAP,  m_view_idlr_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_IDLC_MAP,  m_view_idlc_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_AETPS_MAP,  m_view_aetps_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_AERPM_MAP,  m_view_aerpm_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_AFTSTR_MAP,  m_view_aftstr_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_IT_MAP,  m_view_it_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_ITRPM_MAP,  m_view_itrpm_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_RIGID_MAP,  m_view_rigid_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_EGOCRV_MAP,  m_view_egocrv_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_IACC_MAP,  m_view_iacc_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_IACCW_MAP,  m_view_iaccw_map_btn);
}

BEGIN_MESSAGE_MAP(CButtonsPanel, Super)
 ON_BN_CLICKED(IDC_TD_VIEW_START_MAP,OnViewStartMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IDLE_MAP, OnViewIdleMap)
 ON_BN_CLICKED(IDC_TD_VIEW_WORK_MAP, OnViewWorkMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TEMP_MAP, OnViewTempMap)
 ON_BN_CLICKED(IDC_TD_GME_CHECK, OnGridModeEditing)
 ON_BN_CLICKED(IDC_TD_VIEW_VE_MAP, OnViewVEMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AFR_MAP, OnViewAFRMap)
 ON_BN_CLICKED(IDC_TD_VIEW_CRNK_MAP, OnViewCrnkMap)
 ON_BN_CLICKED(IDC_TD_VIEW_WRMP_MAP, OnViewWrmpMap)
 ON_BN_CLICKED(IDC_TD_VIEW_DEAD_MAP, OnViewDeadMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IDLR_MAP, OnViewIdlrMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IDLC_MAP, OnViewIdlcMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AETPS_MAP, OnViewAETPSMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AERPM_MAP, OnViewAERPMMap)
 ON_BN_CLICKED(IDC_TD_VIEW_AFTSTR_MAP, OnViewAftstrMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IT_MAP, OnViewITMap)
 ON_BN_CLICKED(IDC_TD_VIEW_ITRPM_MAP, OnViewITRPMMap)
 ON_BN_CLICKED(IDC_TD_VIEW_RIGID_MAP, OnViewRigidMap)
 ON_BN_CLICKED(IDC_TD_VIEW_EGOCRV_MAP, OnViewEGOCrvMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IACC_MAP, OnViewIACCMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IACCW_MAP, OnViewIACCWMap)

 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_START_MAP,OnUpdateViewStartMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IDLE_MAP, OnUpdateViewIdleMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_WORK_MAP, OnUpdateViewWorkMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TEMP_MAP, OnUpdateViewTempMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_VE_MAP, OnUpdateViewVEMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AFR_MAP, OnUpdateViewAFRMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_CRNK_MAP, OnUpdateViewCrnkMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_WRMP_MAP, OnUpdateViewWrmpMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_DEAD_MAP, OnUpdateViewDeadMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IDLR_MAP, OnUpdateViewIdlrMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IDLC_MAP, OnUpdateViewIdlcMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AETPS_MAP, OnUpdateViewAETPSMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AERPM_MAP, OnUpdateViewAERPMMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_AFTSTR_MAP, OnUpdateViewAftstrMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IT_MAP, OnUpdateViewITMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ITRPM_MAP, OnUpdateViewITRPMMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_RIGID_MAP, OnUpdateViewRigidMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_EGOCRV_MAP, OnUpdateViewEGOCrvMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IACC_MAP, OnUpdateViewIACCMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IACCW_MAP, OnUpdateViewIACCWMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_GME_CHECK, OnUpdateGridModeEditing)
 ON_WM_TIMER()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel message handlers

BOOL CButtonsPanel::OnInitDialog()
{
 Super::OnInitDialog();

 SetTimer(TIMER_ID, 250, NULL);

 //initialize window scroller
 mp_scr->Init(this);
 CRect wndRect; GetWindowRect(&wndRect);
 mp_scr->SetViewSize(0, int(wndRect.Height() * m_scrl_factor));

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewStartMap()
{
 //If button was released, then close editor's window
 if (m_view_start_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_start_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_start_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_start_map_chart_state = 1;
  m_start_map_wnd_handle = DLL::Chart2DCreate(GetStartMap(true),GetStartMap(false),-15.0,55.0,SECU3IO::start_map_rpm_slots,16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_START_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_start_map_wnd_handle, OnWndActivationStartMap,this);
  DLL::Chart2DSetOnChange(m_start_map_wnd_handle,OnChangeStartMap,this);
  DLL::Chart2DSetOnClose(m_start_map_wnd_handle,OnCloseStartMap,this);
  DLL::Chart2DUpdate(m_start_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_start_map_wnd_handle, TYPE_MAP_DA_START);

  DLL::Chart2DShow(m_start_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_start_map_wnd_handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewIdleMap()
{
 //If button was released, then close editor's window
 if (m_view_idle_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_idle_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_idle_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_idle_map_chart_state = 1;
  m_idle_map_wnd_handle = DLL::Chart2DCreate(GetIdleMap(true),GetIdleMap(false),-15.0,55.0,GetRPMGrid(),16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_IDLE_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_idle_map_wnd_handle,OnWndActivationIdleMap,this);
  DLL::Chart2DSetOnGetAxisLabel(m_idle_map_wnd_handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart2DSetOnChange(m_idle_map_wnd_handle,OnChangeIdleMap,this);
  DLL::Chart2DSetOnClose(m_idle_map_wnd_handle,OnCloseIdleMap,this);
  DLL::Chart2DUpdate(m_idle_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_idle_map_wnd_handle, TYPE_MAP_DA_IDLE);

  DLL::Chart2DShow(m_idle_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_idle_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewWorkMap()
{
 //If button was released, then close editor's window
 if (m_view_work_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_work_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_work_map_chart_state)&&(DLL::Chart3DCreate))
 {
  m_work_map_chart_state = 1;
  m_work_map_wnd_handle = DLL::Chart3DCreate(GetWorkMap(true),GetWorkMap(false),GetRPMGrid(),16,16,-15.0,55.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_WORK_MAP).c_str());
  DLL::Chart3DSetOnWndActivation(m_work_map_wnd_handle, OnWndActivationWorkMap, this);
  DLL::Chart3DSetOnGetAxisLabel(m_work_map_wnd_handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_work_map_wnd_handle,OnChangeWorkMap,this);
  DLL::Chart3DSetOnClose(m_work_map_wnd_handle,OnCloseWorkMap,this);

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_work_map_wnd_handle, TYPE_MAP_DA_WORK);

  DLL::Chart3DShow(m_work_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_work_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewTempMap()
{
 //If button was released, then close editor's window
 if (m_view_temp_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_temp_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_temp_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_temp_map_chart_state = 1;
  m_temp_map_wnd_handle = DLL::Chart2DCreate(GetTempMap(true),GetTempMap(false),-15.0,25.0,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_TEMPCORR_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_temp_map_wnd_handle,OnWndActivationTempMap,this);
  DLL::Chart2DSetOnChange(m_temp_map_wnd_handle,OnChangeTempMap,this);
  DLL::Chart2DSetOnClose(m_temp_map_wnd_handle,OnCloseTempMap,this);
  DLL::Chart2DUpdate(m_temp_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_temp_map_wnd_handle, TYPE_MAP_DA_TEMP_CORR);

  DLL::Chart2DShow(m_temp_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_temp_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewVEMap()
{
 //If button was released, then close editor's window
 if (m_view_ve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_ve_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_ve_map_chart_state)&&(DLL::Chart3DCreate))
 {
  m_ve_map_chart_state = 1;
  m_ve_map_wnd_handle = DLL::Chart3DCreate(GetVEMap(true),GetVEMap(false),GetRPMGrid(),16,16,0.0f,1.99f,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_VE_UNIT).c_str(),
    MLL::GetString(IDS_VE_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(m_ve_map_wnd_handle, _T("#0.00"));
  DLL::Chart3DSetPtMovingStep(m_ve_map_wnd_handle, 0.05f);
  DLL::Chart3DSetOnWndActivation(m_ve_map_wnd_handle, OnWndActivationVEMap, this);
  DLL::Chart3DSetOnGetAxisLabel(m_ve_map_wnd_handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_ve_map_wnd_handle,OnChangeVEMap,this);
  DLL::Chart3DSetOnClose(m_ve_map_wnd_handle,OnCloseVEMap,this);

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_ve_map_wnd_handle, TYPE_MAP_INJ_VE);

  DLL::Chart3DShow(m_ve_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_ve_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewAFRMap()
{
 //If button was released, then close editor's window
 if (m_view_afr_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_afr_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_afr_map_chart_state)&&(DLL::Chart3DCreate))
 {
  m_afr_map_chart_state = 1;
  m_afr_map_wnd_handle = DLL::Chart3DCreate(GetAFRMap(true),GetAFRMap(false),GetRPMGrid(),16,16,8.0f,22.0f,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFR_UNIT).c_str(),
    MLL::GetString(IDS_AFR_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(m_afr_map_wnd_handle, _T("#00.0"));
  DLL::Chart3DSetPtMovingStep(m_afr_map_wnd_handle, 0.1f);
  DLL::Chart3DSetOnWndActivation(m_afr_map_wnd_handle, OnWndActivationAFRMap, this);
  DLL::Chart3DSetOnGetAxisLabel(m_afr_map_wnd_handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_afr_map_wnd_handle,OnChangeAFRMap,this);
  DLL::Chart3DSetOnClose(m_afr_map_wnd_handle,OnCloseAFRMap,this);

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_afr_map_wnd_handle, TYPE_MAP_INJ_AFR);

  DLL::Chart3DShow(m_afr_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_afr_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewCrnkMap()
{
 //If button was released, then close editor's window
 if (m_view_crnk_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_crnk_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_crnk_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_crnk_map_chart_state = 1;
  m_crnk_map_wnd_handle = DLL::Chart2DCreate(GetCrnkMap(true), GetCrnkMap(false), 0.25f, 16.0, SECU3IO::temp_map_tmp_slots, 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_INJPW_UNIT).c_str(),
    MLL::GetString(IDS_CRNK_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_crnk_map_wnd_handle,OnWndActivationCrnkMap,this);
  DLL::Chart2DSetAxisValuesFormat(m_crnk_map_wnd_handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(m_crnk_map_wnd_handle, OnChangeCrnkMap, this);
  DLL::Chart2DSetOnClose(m_crnk_map_wnd_handle, OnCloseCrnkMap, this);
  DLL::Chart2DUpdate(m_crnk_map_wnd_handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_crnk_map_wnd_handle, TYPE_MAP_INJ_CRNK);

  DLL::Chart2DShow(m_crnk_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_crnk_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewWrmpMap()
{
 //if button was released, then close editor's window
 if (m_view_wrmp_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_wrmp_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_wrmp_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_wrmp_map_chart_state = 1;
  m_wrmp_map_wnd_handle = DLL::Chart2DCreate(GetWrmpMap(true),GetWrmpMap(false),0.0f,199.0f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_WRMP_UNIT).c_str(),
    MLL::GetString(IDS_WRMP_MAP).c_str(), false);
  DLL::Chart2DSetPtMovingStep(m_wrmp_map_wnd_handle, 1.00f);
  DLL::Chart2DSetOnWndActivation(m_wrmp_map_wnd_handle,OnWndActivationWrmpMap,this);
  DLL::Chart2DSetOnChange(m_wrmp_map_wnd_handle,OnChangeWrmpMap,this);
  DLL::Chart2DSetOnClose(m_wrmp_map_wnd_handle,OnCloseWrmpMap,this);
  DLL::Chart2DUpdate(m_wrmp_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_wrmp_map_wnd_handle, TYPE_MAP_INJ_WRMP);

  DLL::Chart2DShow(m_wrmp_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_wrmp_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewDeadMap()
{
 //if button was released, then close editor's window
 if (m_view_dead_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_dead_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_dead_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_dead_map_chart_state = 1;
  m_dead_map_wnd_handle = DLL::Chart2DCreate(GetDeadMap(true), GetDeadMap(false), 0.20f, 16.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DEAD_UNIT).c_str(),
    MLL::GetString(IDS_DEAD_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_dead_map_wnd_handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_dead_map_wnd_handle, 0.1f);
  DLL::Chart2DSetOnWndActivation(m_dead_map_wnd_handle,OnWndActivationDeadMap,this);
  DLL::Chart2DSetAxisValuesFormat(m_dead_map_wnd_handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(m_dead_map_wnd_handle, OnChangeDeadMap, this);
  DLL::Chart2DSetOnClose(m_dead_map_wnd_handle, OnCloseDeadMap, this);
  DLL::Chart2DUpdate(m_dead_map_wnd_handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_dead_map_wnd_handle, TYPE_MAP_INJ_DEAD);

  DLL::Chart2DShow(m_dead_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_dead_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewIdlrMap()
{
 //if button was released, then close editor's window
 if (m_view_idlr_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_idlr_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_idlr_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_idlr_map_chart_state = 1;
  m_idlr_map_wnd_handle = DLL::Chart2DCreate(GetIdlrMap(true),GetIdlrMap(false),0.0f,100.0f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_IAC_UNIT).c_str(),
    MLL::GetString(IDS_IDLR_MAP).c_str(), false);
  DLL::Chart2DSetPtMovingStep(m_idlr_map_wnd_handle, 1.0f);
  DLL::Chart2DSetOnWndActivation(m_idlr_map_wnd_handle,OnWndActivationIdlrMap,this);
  DLL::Chart2DSetOnChange(m_idlr_map_wnd_handle,OnChangeIdlrMap,this);
  DLL::Chart2DSetOnClose(m_idlr_map_wnd_handle,OnCloseIdlrMap,this);
  DLL::Chart2DUpdate(m_idlr_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_idlr_map_wnd_handle, TYPE_MAP_INJ_IDLR);

  DLL::Chart2DShow(m_idlr_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_idlr_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewIdlcMap()
{
 //if button was released, then close editor's window
 if (m_view_idlc_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_idlc_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_idlc_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_idlc_map_chart_state = 1;
  m_idlc_map_wnd_handle = DLL::Chart2DCreate(GetIdlcMap(true),GetIdlcMap(false),0.0f,100.0f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_IAC_UNIT).c_str(),
    MLL::GetString(IDS_IDLC_MAP).c_str(), false);
  DLL::Chart2DSetPtMovingStep(m_idlc_map_wnd_handle, 1.0f);
  DLL::Chart2DSetOnWndActivation(m_idlc_map_wnd_handle,OnWndActivationIdlcMap,this);
  DLL::Chart2DSetOnChange(m_idlc_map_wnd_handle,OnChangeIdlcMap,this);
  DLL::Chart2DSetOnClose(m_idlc_map_wnd_handle,OnCloseIdlcMap,this);
  DLL::Chart2DUpdate(m_idlc_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_idlc_map_wnd_handle, TYPE_MAP_INJ_IDLC);

  DLL::Chart2DShow(m_idlc_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_idlc_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewAETPSMap()
{
 //if button was released, then close editor's window
 if (m_view_aetps_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_aetps_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_aetps_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_aetps_map_chart_state = 1;
  const float bins_lims[5] = {-1000.0f, 1000.0f, 1.0f, 0.0f, 10.0f}; //min -1000%, max 1000%, inc 1%, 0 dec places, min diff 10%
  m_aetps_map_wnd_handle = DLL::Chart2DCreate(GetAETPSMap(true),GetAETPSMap(false),-55.0f,199.0f,bins_lims,8,
    MLL::GetString(IDS_MAPS_DPDT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AETPS_UNIT).c_str(),
    MLL::GetString(IDS_AETPS_MAP).c_str(), true); //<-- use horizontal axis bins
  DLL::Chart2DSetPtMovingStep(m_aetps_map_wnd_handle, 1.0f);
  DLL::Chart2DSetOnWndActivation(m_aetps_map_wnd_handle,OnWndActivationAETPSMap,this);
  DLL::Chart2DSetOnChange(m_aetps_map_wnd_handle,OnChangeAETPSMap,this);
  DLL::Chart2DSetOnClose(m_aetps_map_wnd_handle,OnCloseAETPSMap,this);
  DLL::Chart2DUpdate(m_aetps_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_aetps_map_wnd_handle, TYPE_MAP_INJ_AETPS);

  DLL::Chart2DShow(m_aetps_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_aetps_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewAERPMMap()
{
 //if button was released, then close editor's window
 if (m_view_aerpm_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_aerpm_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_aerpm_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_aerpm_map_chart_state = 1;
  const float bins_lims[5] = {0.0f, 25000.0f, 100.0f, 0.0f, 100.0f}; //min 0min-1, max 25000min-1, inc 100min-1, 0 dec places, min diff 100min-1
  m_aerpm_map_wnd_handle = DLL::Chart2DCreate(GetAERPMMap(true),GetAERPMMap(false),0.0f,199.0f,bins_lims,4,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AERPM_UNIT).c_str(),
    MLL::GetString(IDS_AERPM_MAP).c_str(), true); //<--use horizontal axis bins
  DLL::Chart2DSetPtMovingStep(m_aerpm_map_wnd_handle, 1.0f); //step is 1%
  DLL::Chart2DSetOnWndActivation(m_aerpm_map_wnd_handle,OnWndActivationAERPMMap,this);
  DLL::Chart2DSetOnChange(m_aerpm_map_wnd_handle,OnChangeAERPMMap,this);
  DLL::Chart2DSetOnClose(m_aerpm_map_wnd_handle,OnCloseAERPMMap,this);
  DLL::Chart2DUpdate(m_aerpm_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_aerpm_map_wnd_handle, TYPE_MAP_INJ_AERPM);

  DLL::Chart2DShow(m_aerpm_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_aerpm_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewAftstrMap()
{
 //if button was released, then close editor's window
 if (m_view_aftstr_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_aftstr_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_aftstr_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_aftstr_map_chart_state = 1;
  m_aftstr_map_wnd_handle = DLL::Chart2DCreate(GetAftstrMap(true),GetAftstrMap(false),0.0f,199.0f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFTSTR_UNIT).c_str(),
    MLL::GetString(IDS_AFTSTR_MAP).c_str(), false);
  DLL::Chart2DSetPtMovingStep(m_aftstr_map_wnd_handle, 1.0f);
  DLL::Chart2DSetOnWndActivation(m_aftstr_map_wnd_handle,OnWndActivationAftstrMap,this);
  DLL::Chart2DSetOnChange(m_aftstr_map_wnd_handle,OnChangeAftstrMap,this);
  DLL::Chart2DSetOnClose(m_aftstr_map_wnd_handle,OnCloseAftstrMap,this);
  DLL::Chart2DUpdate(m_aftstr_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_aftstr_map_wnd_handle, TYPE_MAP_INJ_AFTSTR);

  DLL::Chart2DShow(m_aftstr_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_aftstr_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewITMap()
{
 //if button was released, then close editor's window
 if (m_view_it_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_it_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_it_map_chart_state)&&(DLL::Chart3DCreate))
 {
  m_it_map_chart_state = 1;
  m_it_map_wnd_handle = DLL::Chart3DCreate(GetITMap(true),GetITMap(false),GetRPMGrid(),16,16,0.0,720.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_INJTIM_UNIT).c_str(),
    MLL::GetString(IDS_IT_MAP).c_str());
  DLL::Chart3DSetPtMovingStep(m_it_map_wnd_handle, 1.0f);
  DLL::Chart3DSetOnWndActivation(m_it_map_wnd_handle, OnWndActivationITMap, this);
  DLL::Chart3DSetOnGetAxisLabel(m_it_map_wnd_handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_it_map_wnd_handle,OnChangeITMap,this);
  DLL::Chart3DSetOnClose(m_it_map_wnd_handle,OnCloseITMap,this);

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_it_map_wnd_handle, TYPE_MAP_INJ_IT);

  DLL::Chart3DShow(m_it_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_it_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewITRPMMap()
{
 //if button was released, then close editor's window
 if (m_view_itrpm_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_itrpm_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_itrpm_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_itrpm_map_chart_state = 1;
  m_itrpm_map_wnd_handle = DLL::Chart2DCreate(GetITRPMMap(true),GetITRPMMap(false),0.0f,2500.0f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_ITRPM_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_itrpm_map_wnd_handle, _T("#0"));
  DLL::Chart2DSetPtMovingStep(m_itrpm_map_wnd_handle, 10.0f);
  DLL::Chart2DSetOnWndActivation(m_itrpm_map_wnd_handle,OnWndActivationITRPMMap,this);
  DLL::Chart2DSetOnChange(m_itrpm_map_wnd_handle,OnChangeITRPMMap,this);
  DLL::Chart2DSetOnClose(m_itrpm_map_wnd_handle,OnCloseITRPMMap,this);
  DLL::Chart2DUpdate(m_itrpm_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_itrpm_map_wnd_handle, TYPE_MAP_INJ_ITRPM);

  DLL::Chart2DShow(m_itrpm_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_itrpm_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewRigidMap()
{
 //if button was released, then close editor's window
 if (m_view_rigid_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_rigid_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_rigid_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_rigid_map_chart_state = 1;
  m_rigid_map_wnd_handle = DLL::Chart2DCreate(GetRigidMap(true),GetRigidMap(false),0.0f,5.0f,SECU3IO::rigid_map_slots,8,
    MLL::GetString(IDS_MAPS_RIGID_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_RIGID_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_rigid_map_wnd_handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_rigid_map_wnd_handle, 0.01f);
  DLL::Chart2DSetOnWndActivation(m_rigid_map_wnd_handle,OnWndActivationRigidMap,this);
  DLL::Chart2DSetOnChange(m_rigid_map_wnd_handle,OnChangeRigidMap,this);
  DLL::Chart2DSetOnClose(m_rigid_map_wnd_handle,OnCloseRigidMap,this);
  DLL::Chart2DUpdate(m_rigid_map_wnd_handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_rigid_map_wnd_handle, TYPE_MAP_INJ_RIGID);

  DLL::Chart2DShow(m_rigid_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_rigid_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewEGOCrvMap()
{
 //If button was released, then close editor's window
 if (m_view_egocrv_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_egocrv_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_egocrv_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_egocrv_map_chart_state = 1;
  m_egocrv_map_wnd_handle = DLL::Chart2DCreate(GetEGOCurveMap(true), GetEGOCurveMap(false), 6.00, 24.00, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFR_UNIT).c_str(),
    MLL::GetString(IDS_EGO_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_egocrv_map_wnd_handle,OnWndActivationEGOCrvMap, this);
  DLL::Chart2DSetPtValuesFormat(m_egocrv_map_wnd_handle, _T("#0.00"));
  DLL::Chart2DSetAxisValuesFormat(m_egocrv_map_wnd_handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_egocrv_map_wnd_handle, 1, true, 0, 5.5f, 0, 5.5f, 0.01f, OnChangeEGOCrvXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_egocrv_map_wnd_handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_egocrv_map_wnd_handle, OnChangeEGOCrvMap, this);
  DLL::Chart2DSetOnClose(m_egocrv_map_wnd_handle, OnCloseEGOCrvMap, this);
  DLL::Chart2DUpdate(m_egocrv_map_wnd_handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_egocrv_map_wnd_handle, 1, GetEGOCurveMap(false)[16], GetEGOCurveMap(false)[16+1]);

  //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_egocrv_map_wnd_handle, TYPE_MAP_INJ_EGOCRV);

  DLL::Chart2DShow(m_egocrv_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_egocrv_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewIACCMap()
{
 //If button was released, then close editor's window
 if (m_view_iacc_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_iacc_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_iacc_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_iacc_map_chart_state = 1;
  m_iacc_map_wnd_handle = DLL::Chart2DCreate(GetIACCMap(true), GetIACCMap(false), 0.00, 2.00, NULL, 8,
    MLL::GetString(IDS_MAPS_IACPOS_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_IACC_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_iacc_map_wnd_handle,OnWndActivationIACCMap, this);
  DLL::Chart2DSetPtValuesFormat(m_iacc_map_wnd_handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_iacc_map_wnd_handle, 0.05f);
  DLL::Chart2DSetAxisValuesFormat(m_iacc_map_wnd_handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_iacc_map_wnd_handle, 1, true, 0, 100.0f, 0, 100.0f, 0.5f, OnChangeIACCXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_iacc_map_wnd_handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_iacc_map_wnd_handle, OnChangeIACCMap, this);
  DLL::Chart2DSetOnClose(m_iacc_map_wnd_handle, OnCloseIACCMap, this);
  DLL::Chart2DUpdate(m_iacc_map_wnd_handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_iacc_map_wnd_handle, 1, GetIACCMap(false)[8], GetIACCMap(false)[8+1]);

  //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_iacc_map_wnd_handle, TYPE_MAP_INJ_IACC);

  DLL::Chart2DShow(m_iacc_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_iacc_map_wnd_handle);
 }
}

void CButtonsPanel::OnViewIACCWMap()
{
 //If button was released, then close editor's window
 if (m_view_iaccw_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_iaccw_map_wnd_handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_iaccw_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_iaccw_map_chart_state = 1;
  m_iaccw_map_wnd_handle = DLL::Chart2DCreate(GetIACCWMap(true), GetIACCWMap(false), 0.00, 1.00, NULL, 16,
    MLL::GetString(IDS_MAPS_TPSPOS_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_IACCW_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_iaccw_map_wnd_handle,OnWndActivationIACCWMap, this);
  DLL::Chart2DSetPtValuesFormat(m_iaccw_map_wnd_handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_iaccw_map_wnd_handle, 0.05f);
  DLL::Chart2DSetAxisValuesFormat(m_iaccw_map_wnd_handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_iaccw_map_wnd_handle, 1, true, 0, 100.0f, 0, 100.0f, 0.5f, OnChangeIACCWXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_iaccw_map_wnd_handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_iaccw_map_wnd_handle, OnChangeIACCWMap, this);
  DLL::Chart2DSetOnClose(m_iaccw_map_wnd_handle, OnCloseIACCWMap, this);
  DLL::Chart2DUpdate(m_iaccw_map_wnd_handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_iaccw_map_wnd_handle, 1, GetIACCWMap(false)[16], GetIACCWMap(false)[16+1]);

  //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_iaccw_map_wnd_handle, TYPE_MAP_INJ_IACCW);

  DLL::Chart2DShow(m_iaccw_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_iaccw_map_wnd_handle);
 }
}

void CButtonsPanel::OnGridModeEditing()
{
 if (m_grid_mode_editing_check.GetCheck()==BST_CHECKED)
 {
  mp_gridModeEditorDlg.reset(new CGridModeEditorDlg());
  mp_gridModeEditorDlg->BindMaps(m_start_map_active, m_idle_map_active, &m_work_map_active[0][0], m_temp_map_active);
  mp_gridModeEditorDlg->BindRPMGrid(GetRPMGrid());
  mp_gridModeEditorDlg->setIsAllowed(fastdelegate::MakeDelegate(this, &CButtonsPanel::IsAllowed));
  mp_gridModeEditorDlg->setOnMapChanged(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapChanged));
  mp_gridModeEditorDlg->setOnCloseMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapClosed));
  mp_gridModeEditorDlg->setOnOpenMapWnd(m_OnOpenMapWnd);
  mp_gridModeEditorDlg->EnableAdvanceAngleIndication(m_en_aa_indication);
  mp_gridModeEditorDlg->Create(CGridModeEditorDlg::IDD, NULL);
  mp_gridModeEditorDlg->ShowWindow(SW_SHOW);
  m_grid_map_state = 1;
 }
 else
 {
  OnGridMapClosed(mp_gridModeEditorDlg->m_hWnd, TYPE_MAP_GME_WND);
  mp_gridModeEditorDlg->DestroyWindow();
  mp_gridModeEditorDlg.reset(NULL);
 }
}

void CButtonsPanel::OnUpdateViewStartMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_start_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdleMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_idle_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewWorkMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_work_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewTempMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_temp_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateGridModeEditing(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 pCmdUI->Enable(allowed);
 pCmdUI->SetCheck((mp_gridModeEditorDlg.get() && m_grid_map_state) ? TRUE : FALSE);
}

void CButtonsPanel::OnUpdateViewVEMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_ve_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAFRMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_afr_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewCrnkMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_crnk_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewWrmpMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_wrmp_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewDeadMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_dead_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdlrMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_idlr_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdlcMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_idlc_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAETPSMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_aetps_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAERPMMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_aerpm_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAftstrMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_aftstr_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewITMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_it_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewITRPMMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_itrpm_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewRigidMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_rigid_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewEGOCrvMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_egocrv_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIACCMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_iacc_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIACCWMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_iaccw_map_chart_state) ? TRUE : FALSE );
}

void CButtonsPanel::OnTimer(UINT nIDEvent)
{
 //I know it is dirty hack, but... :-)
 UpdateDialogControls(this,TRUE);
 _EnableCharts(IsAllowed());
 Super::OnTimer(nIDEvent);
}

void CButtonsPanel::OnDestroy()
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);
 mp_scr->Close();
}

void CButtonsPanel::UpdateOpenedCharts(void)
{
 if (m_start_map_chart_state)
  DLL::Chart2DUpdate(m_start_map_wnd_handle, GetStartMap(true), GetStartMap(false));
 if (m_idle_map_chart_state)
  DLL::Chart2DUpdate(m_idle_map_wnd_handle, GetIdleMap(true), GetIdleMap(false));
 if (m_work_map_chart_state)
  DLL::Chart3DUpdate(m_work_map_wnd_handle, GetWorkMap(true), GetWorkMap(false));
 if (m_temp_map_chart_state)
  DLL::Chart2DUpdate(m_temp_map_wnd_handle, GetTempMap(true), GetTempMap(false));
 if (m_ve_map_chart_state)
  DLL::Chart3DUpdate(m_ve_map_wnd_handle, GetVEMap(true), GetVEMap(false));
 if (m_afr_map_chart_state)
  DLL::Chart3DUpdate(m_afr_map_wnd_handle, GetAFRMap(true), GetAFRMap(false));
 if (m_crnk_map_chart_state)
  DLL::Chart2DUpdate(m_crnk_map_wnd_handle, GetCrnkMap(true), GetCrnkMap(false));
 if (m_wrmp_map_chart_state)
  DLL::Chart2DUpdate(m_wrmp_map_wnd_handle, GetWrmpMap(true), GetWrmpMap(false));
 if (m_dead_map_chart_state)
  DLL::Chart2DUpdate(m_dead_map_wnd_handle, GetDeadMap(true), GetDeadMap(false));
 if (m_idlr_map_chart_state)
  DLL::Chart2DUpdate(m_idlr_map_wnd_handle, GetIdlrMap(true), GetIdlrMap(false));
 if (m_idlc_map_chart_state)
  DLL::Chart2DUpdate(m_idlc_map_wnd_handle, GetIdlcMap(true), GetIdlcMap(false));
 if (m_aetps_map_chart_state)
  DLL::Chart2DUpdate(m_aetps_map_wnd_handle, GetAETPSMap(true), GetAETPSMap(false));
 if (m_aerpm_map_chart_state)
  DLL::Chart2DUpdate(m_aerpm_map_wnd_handle, GetAERPMMap(true), GetAERPMMap(false));
 if (m_aftstr_map_chart_state)
  DLL::Chart2DUpdate(m_aftstr_map_wnd_handle, GetAftstrMap(true), GetAftstrMap(false));
 if (m_it_map_chart_state)
  DLL::Chart3DUpdate(m_it_map_wnd_handle, GetITMap(true), GetITMap(false));
 if (m_itrpm_map_chart_state)
  DLL::Chart2DUpdate(m_itrpm_map_wnd_handle, GetITRPMMap(true), GetITRPMMap(false));
 if (m_rigid_map_chart_state)
  DLL::Chart2DUpdate(m_rigid_map_wnd_handle, GetRigidMap(true), GetRigidMap(false));
 if (m_egocrv_map_chart_state)
  DLL::Chart2DUpdate(m_egocrv_map_wnd_handle, GetEGOCurveMap(true), GetEGOCurveMap(false));
 if (m_iacc_map_chart_state)
  DLL::Chart2DUpdate(m_iacc_map_wnd_handle, GetIACCMap(true), GetIACCMap(false));
 if (m_iaccw_map_chart_state)
  DLL::Chart2DUpdate(m_iaccw_map_wnd_handle, GetIACCWMap(true), GetIACCWMap(false));
 if (mp_gridModeEditorDlg.get() && m_grid_map_state)
  mp_gridModeEditorDlg->UpdateView();
}

void CButtonsPanel::EnableAdvanceAngleIndication(bool i_enable)
{
 m_en_aa_indication = i_enable;
 if (mp_gridModeEditorDlg.get())
  mp_gridModeEditorDlg->EnableAdvanceAngleIndication(m_en_aa_indication);
}

void CButtonsPanel::SetDynamicValues(const CGridModeEditorDlg::DynVal& dv)
{
 if (mp_gridModeEditorDlg.get())
  mp_gridModeEditorDlg->SetDynamicValues(dv);
}

void CButtonsPanel::EnableFuelInjection(bool i_enable)
{
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_ve_map_chart_state && ::IsWindow(m_ve_map_wnd_handle))
  DLL::Chart3DEnable(m_ve_map_wnd_handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_afr_map_chart_state && ::IsWindow(m_afr_map_wnd_handle))
  DLL::Chart3DEnable(m_afr_map_wnd_handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_crnk_map_chart_state && ::IsWindow(m_crnk_map_wnd_handle))
  DLL::Chart2DEnable(m_crnk_map_wnd_handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_wrmp_map_chart_state && ::IsWindow(m_wrmp_map_wnd_handle))
  DLL::Chart2DEnable(m_wrmp_map_wnd_handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_dead_map_chart_state && ::IsWindow(m_dead_map_wnd_handle))
  DLL::Chart2DEnable(m_dead_map_wnd_handle, i_enable && IsAllowed());
 if (m_idlr_map_chart_state && ::IsWindow(m_idlr_map_wnd_handle))
  DLL::Chart2DEnable(m_idlr_map_wnd_handle, i_enable && IsAllowed());
 if (m_idlc_map_chart_state && ::IsWindow(m_idlc_map_wnd_handle))
  DLL::Chart2DEnable(m_idlc_map_wnd_handle, i_enable && IsAllowed());
 if (m_aetps_map_chart_state && ::IsWindow(m_aetps_map_wnd_handle))
  DLL::Chart2DEnable(m_aetps_map_wnd_handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_aerpm_map_chart_state && ::IsWindow(m_aerpm_map_wnd_handle))
  DLL::Chart2DEnable(m_aerpm_map_wnd_handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_aftstr_map_chart_state && ::IsWindow(m_aftstr_map_wnd_handle))
  DLL::Chart2DEnable(m_aftstr_map_wnd_handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_it_map_chart_state && ::IsWindow(m_it_map_wnd_handle))
  DLL::Chart3DEnable(m_it_map_wnd_handle, i_enable && IsAllowed());
 if (m_itrpm_map_chart_state && ::IsWindow(m_itrpm_map_wnd_handle))
  DLL::Chart2DEnable(m_itrpm_map_wnd_handle, i_enable && IsAllowed());
 if (m_rigid_map_chart_state && ::IsWindow(m_rigid_map_wnd_handle))
  DLL::Chart2DEnable(m_rigid_map_wnd_handle, i_enable && IsAllowed());
 if (m_egocrv_map_chart_state && ::IsWindow(m_egocrv_map_wnd_handle))
  DLL::Chart2DEnable(m_egocrv_map_wnd_handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_iacc_map_chart_state && ::IsWindow(m_iacc_map_wnd_handle))
  DLL::Chart2DEnable(m_iacc_map_wnd_handle, i_enable && IsAllowed());
 if (m_iaccw_map_chart_state && ::IsWindow(m_iaccw_map_wnd_handle))
  DLL::Chart2DEnable(m_iaccw_map_wnd_handle, i_enable && IsAllowed());
}

void CButtonsPanel::EnableGasdose(bool i_enable)
{
 m_gasdose = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_aetps_map_chart_state && ::IsWindow(m_aetps_map_wnd_handle))
  DLL::Chart2DEnable(m_aetps_map_wnd_handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_aerpm_map_chart_state && ::IsWindow(m_aerpm_map_wnd_handle))
  DLL::Chart2DEnable(m_aerpm_map_wnd_handle, (i_enable || m_fuel_injection) && IsAllowed());

 if (m_ve_map_chart_state && ::IsWindow(m_ve_map_wnd_handle))
  DLL::Chart3DEnable(m_ve_map_wnd_handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_afr_map_chart_state && ::IsWindow(m_afr_map_wnd_handle))
  DLL::Chart3DEnable(m_afr_map_wnd_handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_wrmp_map_chart_state && ::IsWindow(m_wrmp_map_wnd_handle))
  DLL::Chart2DEnable(m_wrmp_map_wnd_handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_crnk_map_chart_state && ::IsWindow(m_crnk_map_wnd_handle))
  DLL::Chart2DEnable(m_crnk_map_wnd_handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_aftstr_map_chart_state && ::IsWindow(m_aftstr_map_wnd_handle))
  DLL::Chart2DEnable(m_aftstr_map_wnd_handle, (i_enable || m_fuel_injection) && IsAllowed());

 if (m_egocrv_map_chart_state && ::IsWindow(m_egocrv_map_wnd_handle))
  DLL::Chart2DEnable(m_egocrv_map_wnd_handle, (i_enable || m_fuel_injection) && IsAllowed());
}

float* CButtonsPanel::GetStartMap(bool i_original)
{
 if (i_original)
  return m_start_map_original;
 else
  return m_start_map_active;
}

float* CButtonsPanel::GetIdleMap(bool i_original)
{
 if (i_original)
  return m_idle_map_original;
 else
  return m_idle_map_active;
}

float* CButtonsPanel::GetWorkMap(bool i_original)
{
 if (i_original)
  return &m_work_map_original[0][0];
 else
  return &m_work_map_active[0][0];
}

float* CButtonsPanel::GetTempMap(bool i_original)
{
 if (i_original)
  return m_temp_map_original;
 else
  return m_temp_map_active;
}

float* CButtonsPanel::GetVEMap(bool i_original)
{
 if (i_original)
  return &m_ve_map_original[0][0];
 else
  return &m_ve_map_active[0][0];
}

float* CButtonsPanel::GetAFRMap(bool i_original)
{
 if (i_original)
  return &m_afr_map_original[0][0];
 else
  return &m_afr_map_active[0][0];
}

float* CButtonsPanel::GetCrnkMap(bool i_original)
{
 if (i_original)
  return m_crnk_map_original;
 else
  return m_crnk_map_active;
}

float* CButtonsPanel::GetWrmpMap(bool i_original)
{
 if (i_original)
  return m_wrmp_map_original;
 else
  return m_wrmp_map_active;
}

float* CButtonsPanel::GetDeadMap(bool i_original)
{
 if (i_original)
  return m_dead_map_original;
 else
  return m_dead_map_active;
}

float* CButtonsPanel::GetIdlrMap(bool i_original)
{
 if (i_original)
  return m_idlr_map_original;
 else
  return m_idlr_map_active;
}

float* CButtonsPanel::GetIdlcMap(bool i_original)
{
 if (i_original)
  return m_idlc_map_original;
 else
  return m_idlc_map_active;
}

float* CButtonsPanel::GetAETPSMap(bool i_original)
{
 if (i_original)
  return m_aetps_map_original;
 else
  return m_aetps_map_active;
}

float* CButtonsPanel::GetAERPMMap(bool i_original)
{
 if (i_original)
  return m_aerpm_map_original;
 else
  return m_aerpm_map_active;
}

float* CButtonsPanel::GetAftstrMap(bool i_original)
{
 if (i_original)
  return m_aftstr_map_original;
 else
  return m_aftstr_map_active;
}

float* CButtonsPanel::GetITMap(bool i_original)
{
 if (i_original)
  return &m_it_map_original[0][0];
 else
  return &m_it_map_active[0][0];
}

float* CButtonsPanel::GetITRPMMap(bool i_original)
{
 if (i_original)
  return m_itrpm_map_original;
 else
  return m_itrpm_map_active;
}

float* CButtonsPanel::GetRigidMap(bool i_original)
{
 if (i_original)
  return m_rigid_map_original;
 else
  return m_rigid_map_active;
}

float* CButtonsPanel::GetEGOCurveMap(bool i_original)
{
 if (i_original)
  return m_egocrv_map_original;
 else
  return m_egocrv_map_active;
}

float* CButtonsPanel::GetIACCMap(bool i_original)
{
 if (i_original)
  return m_iacc_map_original;
 else
  return m_iacc_map_active;
}

float* CButtonsPanel::GetIACCWMap(bool i_original)
{
 if (i_original)
  return m_iaccw_map_original;
 else
  return m_iaccw_map_active;
}

float* CButtonsPanel::GetRPMGrid(void)
{
 return m_rpm_grid_values;
}

HWND CButtonsPanel::GetMapWindow(int wndType)
{
 switch(wndType)
 {
 case TYPE_MAP_DA_START:
  return m_start_map_chart_state ? m_start_map_wnd_handle : NULL;
 case TYPE_MAP_DA_IDLE:
  return m_idle_map_chart_state ? m_idle_map_wnd_handle : NULL;
 case TYPE_MAP_DA_WORK:
  return m_work_map_chart_state ? m_work_map_wnd_handle : NULL;
 case TYPE_MAP_DA_TEMP_CORR:
  return m_temp_map_chart_state ? m_temp_map_wnd_handle : NULL;
 //fuel injection maps
 case TYPE_MAP_INJ_VE:
  return m_ve_map_chart_state ? m_ve_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_AFR:
  return m_afr_map_chart_state ? m_afr_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_CRNK:
  return m_crnk_map_chart_state ? m_crnk_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_WRMP:
  return m_wrmp_map_chart_state ? m_wrmp_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_DEAD:
  return m_dead_map_chart_state ? m_dead_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_IDLR:
  return m_idlr_map_chart_state ? m_idlr_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_IDLC:
  return m_idlc_map_chart_state ? m_idlc_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_AETPS:
  return m_aetps_map_chart_state ? m_aetps_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_AERPM:
  return m_aerpm_map_chart_state ? m_aerpm_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_AFTSTR:
  return m_aftstr_map_chart_state ? m_aftstr_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_IT:
  return m_it_map_chart_state ? m_it_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_ITRPM:
  return m_itrpm_map_chart_state ? m_itrpm_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_RIGID:
  return m_rigid_map_chart_state ? m_rigid_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_EGOCRV:
  return m_egocrv_map_chart_state ? m_egocrv_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_IACC:
  return m_iacc_map_chart_state ? m_iacc_map_wnd_handle : NULL;
 case TYPE_MAP_INJ_IACCW:
  return m_iaccw_map_chart_state ? m_iaccw_map_wnd_handle : NULL;

 case TYPE_MAP_GME_WND: //pseudo map
  return (mp_gridModeEditorDlg.get() && m_grid_map_state) ? mp_gridModeEditorDlg->m_hWnd : NULL; 
 default:
  return NULL;
 }
}

void CButtonsPanel::_EnableCharts(bool enable)
{
 if (m_charts_enabled != (int)enable)
 {//ignition
  if (m_start_map_chart_state && ::IsWindow(m_start_map_wnd_handle))
   DLL::Chart2DEnable(m_start_map_wnd_handle, enable && IsAllowed());
  if (m_idle_map_chart_state && ::IsWindow(m_idle_map_wnd_handle))
   DLL::Chart2DEnable(m_idle_map_wnd_handle, enable && IsAllowed());
  if (m_work_map_chart_state && ::IsWindow(m_work_map_wnd_handle))
   DLL::Chart3DEnable(m_work_map_wnd_handle, enable && IsAllowed());
  if (m_temp_map_chart_state && ::IsWindow(m_temp_map_wnd_handle))
   DLL::Chart2DEnable(m_temp_map_wnd_handle, enable && IsAllowed());
  //fuel injection
  if (m_ve_map_chart_state && ::IsWindow(m_ve_map_wnd_handle))
   DLL::Chart3DEnable(m_ve_map_wnd_handle, enable && IsAllowed());
  if (m_afr_map_chart_state && ::IsWindow(m_afr_map_wnd_handle))
   DLL::Chart3DEnable(m_afr_map_wnd_handle, enable && IsAllowed());
  if (m_crnk_map_chart_state && ::IsWindow(m_crnk_map_wnd_handle))
   DLL::Chart2DEnable(m_crnk_map_wnd_handle, enable && IsAllowed());
  if (m_wrmp_map_chart_state && ::IsWindow(m_wrmp_map_wnd_handle))
   DLL::Chart2DEnable(m_wrmp_map_wnd_handle, enable && IsAllowed());
  if (m_dead_map_chart_state && ::IsWindow(m_dead_map_wnd_handle))
   DLL::Chart2DEnable(m_dead_map_wnd_handle, enable && IsAllowed());
  if (m_idlr_map_chart_state && ::IsWindow(m_idlr_map_wnd_handle))
   DLL::Chart2DEnable(m_idlr_map_wnd_handle, enable && IsAllowed());
  if (m_idlc_map_chart_state && ::IsWindow(m_idlc_map_wnd_handle))
   DLL::Chart2DEnable(m_idlc_map_wnd_handle, enable && IsAllowed());
  if (m_aetps_map_chart_state && ::IsWindow(m_aetps_map_wnd_handle))
   DLL::Chart2DEnable(m_aetps_map_wnd_handle, enable && IsAllowed());
  if (m_aerpm_map_chart_state && ::IsWindow(m_aerpm_map_wnd_handle))
   DLL::Chart2DEnable(m_aerpm_map_wnd_handle, enable && IsAllowed());
  if (m_aftstr_map_chart_state && ::IsWindow(m_aftstr_map_wnd_handle))
   DLL::Chart2DEnable(m_aftstr_map_wnd_handle, enable && IsAllowed());
  if (m_it_map_chart_state && ::IsWindow(m_it_map_wnd_handle))
   DLL::Chart3DEnable(m_it_map_wnd_handle, enable && IsAllowed());
  if (m_itrpm_map_chart_state && ::IsWindow(m_itrpm_map_wnd_handle))
   DLL::Chart2DEnable(m_itrpm_map_wnd_handle, enable && IsAllowed());
  if (m_rigid_map_chart_state && ::IsWindow(m_rigid_map_wnd_handle))
   DLL::Chart2DEnable(m_rigid_map_wnd_handle, enable && IsAllowed());
  if (m_egocrv_map_chart_state && ::IsWindow(m_egocrv_map_wnd_handle))
   DLL::Chart2DEnable(m_egocrv_map_wnd_handle, enable && IsAllowed());
  if (m_iacc_map_chart_state && ::IsWindow(m_iacc_map_wnd_handle))
   DLL::Chart2DEnable(m_iacc_map_wnd_handle, enable && IsAllowed());
  if (m_iaccw_map_chart_state && ::IsWindow(m_iaccw_map_wnd_handle))
   DLL::Chart2DEnable(m_iaccw_map_wnd_handle, enable && IsAllowed());

  if (mp_gridModeEditorDlg.get() && m_grid_map_state && ::IsWindow(mp_gridModeEditorDlg->m_hWnd))
   mp_gridModeEditorDlg->UpdateDialogControls(mp_gridModeEditorDlg.get(), TRUE);
 }

 m_charts_enabled = enable;
}

bool CButtonsPanel::IsAllowed(void)
{
 if (m_IsAllowed)
  return m_IsAllowed();
 return false;
}

void CButtonsPanel::setIsAllowed(EventResult IsFunction)
{m_IsAllowed = IsFunction;}

void CButtonsPanel::setOnMapChanged(EventWithCode OnFunction)
{m_OnMapChanged = OnFunction;}

void CButtonsPanel::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CButtonsPanel::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CButtonsPanel::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after, x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

void CButtonsPanel::setOnWndActivation(EventWithHWNDLong OnFunction)
{ m_OnWndActivation = OnFunction; }
