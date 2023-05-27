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

/** \file ButtonsPanelEvents.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "AutoTuneController.h"
#include "ButtonsPanel.h"
#include "common/MathHelpers.h"
#include "DLLLinkedFunctions.h"
#include "GridModeEditorIgnDlg.h"
#include "MapIds.h"
#include "ui-core/MapEditorCtrl.h"


//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeSettingsCME(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 for(int i = TYPE_MAP_ALL_START; i <= TYPE_MAP_ALL_END - 2; ++i)
 {
  if (_this->m_md[i].state)
  {
   if (i == TYPE_MAP_DA_WORK || i == TYPE_MAP_INJ_VE || i == TYPE_MAP_INJ_VE2 || i == TYPE_MAP_INJ_AFR || i == TYPE_MAP_INJ_IT || i == TYPE_MAP_PWM1 || i == TYPE_MAP_PWM2 || i == TYPE_MAP_GASDOSE)  
    _this->m_md[i].ptMovStep = DLL::Chart3DGetPtMovingStep(_this->m_md[i].handle);
   else
    _this->m_md[i].ptMovStep = DLL::Chart2DGetPtMovingStep(_this->m_md[i].handle);
  }
 }

 if (_this->m_OnChangeSettings)
  _this->m_OnChangeSettings();
}

//------------------------------------------------------------------------
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
 if (_this->mp_gridModeEditorIgnDlg.get())
  _this->mp_gridModeEditorIgnDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseStartMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_DA_START].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_DA_START].handle, TYPE_MAP_DA_START);
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
 if (_this->mp_gridModeEditorIgnDlg.get())
  _this->mp_gridModeEditorIgnDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseIdleMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_DA_IDLE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_DA_IDLE].handle, TYPE_MAP_DA_IDLE);
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
 if (_this->mp_gridModeEditorIgnDlg.get())
  _this->mp_gridModeEditorIgnDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_DA_WORK].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_DA_WORK].handle, TYPE_MAP_DA_WORK);
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
 if (_this->mp_gridModeEditorIgnDlg.get())
  _this->mp_gridModeEditorIgnDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_DA_TEMP_CORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_DA_TEMP_CORR].handle, TYPE_MAP_DA_TEMP_CORR);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_DA_START].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_DA_IDLE].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_DA_WORK].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_DA_TEMP_CORR].handle, cmd);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_VE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_VE].handle, TYPE_MAP_INJ_VE);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeVE2Map(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_VE2);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseVE2Map(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_VE2].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_VE2].handle, TYPE_MAP_INJ_VE2);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_AFR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_AFR].handle, TYPE_MAP_INJ_AFR);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_CRNK].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_CRNK].handle, TYPE_MAP_INJ_CRNK);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_WRMP].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_WRMP].handle, TYPE_MAP_INJ_WRMP);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_DEAD].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_DEAD].handle, TYPE_MAP_INJ_DEAD);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_IDLR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_IDLR].handle, TYPE_MAP_INJ_IDLR);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_IDLC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_IDLC].handle, TYPE_MAP_INJ_IDLC);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeThrassMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_THRASS);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseThrassMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_THRASS].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_THRASS].handle, TYPE_MAP_INJ_THRASS);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_AETPS].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_AETPS].handle, TYPE_MAP_INJ_AETPS);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeAEMAPMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_AEMAP);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseAEMAPMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_AEMAP].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_AEMAP].handle, TYPE_MAP_INJ_AEMAP);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_AERPM].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_AERPM].handle, TYPE_MAP_INJ_AERPM);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_AFTSTR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_AFTSTR].handle, TYPE_MAP_INJ_AFTSTR);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_IT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_IT].handle, TYPE_MAP_INJ_IT);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_ITRPM].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_ITRPM].handle, TYPE_MAP_INJ_ITRPM);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_RIGID].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_RIGID].handle, TYPE_MAP_INJ_RIGID);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_EGOCRV].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_EGOCRV].handle, TYPE_MAP_INJ_EGOCRV);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_IACC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_IACC].handle, TYPE_MAP_INJ_IACC);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
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
 _this->m_md[TYPE_MAP_INJ_IACCW].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_IACCW].handle, TYPE_MAP_INJ_IACCW);
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
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIATCLTMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IATCLT);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseIATCLTMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_IATCLT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_IATCLT].handle, TYPE_MAP_INJ_IATCLT);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIATCLTXAxisEdit(void* i_param, int i_type, float i_value)
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
  _this->GetIATCLTMap(false)[8 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IATCLT);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeTpsswtMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_TPSSWT);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseTpsswtMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_TPSSWT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_TPSSWT].handle, TYPE_MAP_INJ_TPSSWT);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeGtscMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_GTSC);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseGtscMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_GTSC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_GTSC].handle, TYPE_MAP_INJ_GTSC);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeGpscMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_GPSC);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseGpscMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_GPSC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_GPSC].handle, TYPE_MAP_INJ_GPSC);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeGpscXAxisEdit(void* i_param, int i_type, float i_value)
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
  _this->GetGpscMap(false)[17 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_GPSC);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeAtscMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_ATSC);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseAtscMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_ATSC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_ATSC].handle, TYPE_MAP_INJ_ATSC);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangePwm1Map(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_PWM1);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnClosePwm1Map(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_PWM1].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_PWM1].handle, TYPE_MAP_PWM1);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangePwm2Map(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_PWM2);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnClosePwm2Map(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_PWM2].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_PWM2].handle, TYPE_MAP_PWM2);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_IT].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_VE].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationVE2Map(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_VE2].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_AFR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_CRNK].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_WRMP].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_DEAD].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_IDLR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_IDLC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationThrassMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_THRASS].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_AETPS].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationAEMAPMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_AEMAP].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_AERPM].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_AFTSTR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_ITRPM].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_RIGID].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_EGOCRV].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_IACC].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_IACCW].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationIATCLTMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_IATCLT].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationTpsswtMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_TPSSWT].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationGtscMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_GTSC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationGpscMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_GPSC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationAtscMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_ATSC].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationPwm1Map(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_PWM1].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationPwm2Map(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_PWM2].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationIACMATMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_IACMAT].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeIACMATMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_IACMAT);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseIACMATMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_IACMAT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_IACMAT].handle, TYPE_MAP_INJ_IACMAT);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapChangedIgn(int mapType)
{
 if (m_md[TYPE_MAP_DA_START].state && mapType == TYPE_MAP_DA_START)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_START].handle, GetStartMap(true), GetStartMap(false));
 if (m_md[TYPE_MAP_DA_IDLE].state && mapType == TYPE_MAP_DA_IDLE)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_IDLE].handle, GetIdleMap(true), GetIdleMap(false));
 if (m_md[TYPE_MAP_DA_WORK].state && mapType == TYPE_MAP_DA_WORK)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_DA_WORK].handle, GetWorkMap(true), GetWorkMap(false));
 if (m_md[TYPE_MAP_DA_TEMP_CORR].state && mapType == TYPE_MAP_DA_TEMP_CORR)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_TEMP_CORR].handle, GetTempMap(true), GetTempMap(false));
 if (m_md[TYPE_MAP_DA_TEMPI_CORR].state && mapType == TYPE_MAP_DA_TEMPI_CORR)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_TEMPI_CORR].handle, GetTempIdlMap(true), GetTempIdlMap(false));

 if (m_OnMapChanged)
  m_OnMapChanged(mapType);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapClosedIgn(HWND hwnd, int mapType)
{
 m_grid_map_state_ign = 0;
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(mp_gridModeEditorIgnDlg->m_hWnd, TYPE_MAP_GME_IGN_WND);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapChangedInj(int mapType)
{
 if (m_md[TYPE_MAP_INJ_VE].state && mapType == TYPE_MAP_INJ_VE)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_VE].handle, GetVEMap(true), GetVEMap(false));
 if (m_md[TYPE_MAP_INJ_VE2].state && mapType == TYPE_MAP_INJ_VE2)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_VE2].handle, GetVE2Map(true), GetVE2Map(false));
 if (m_md[TYPE_MAP_INJ_AFR].state && mapType == TYPE_MAP_INJ_AFR)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_AFR].handle, GetAFRMap(true), GetAFRMap(false));
 if (m_md[TYPE_MAP_INJ_CRNK].state && mapType == TYPE_MAP_INJ_CRNK)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_CRNK].handle, GetCrnkMap(true), GetCrnkMap(false));
 if (m_md[TYPE_MAP_INJ_WRMP].state && mapType == TYPE_MAP_INJ_WRMP)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_WRMP].handle, GetWrmpMap(true), GetWrmpMap(false));
 if (m_md[TYPE_MAP_INJ_DEAD].state && mapType == TYPE_MAP_INJ_DEAD)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_DEAD].handle, GetDeadMap(true), GetDeadMap(false));
 if (m_md[TYPE_MAP_INJ_IDLR].state && mapType == TYPE_MAP_INJ_IDLR)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IDLR].handle, GetIdlrMap(true), GetIdlrMap(false));
 if (m_md[TYPE_MAP_INJ_IDLC].state && mapType == TYPE_MAP_INJ_IDLC)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IDLC].handle, GetIdlcMap(true), GetIdlcMap(false));
 if (m_md[TYPE_MAP_INJ_THRASS].state && mapType == TYPE_MAP_INJ_THRASS)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_THRASS].handle, GetThrassMap(true), GetThrassMap(false));
 if (m_md[TYPE_MAP_INJ_AETPS].state && mapType == TYPE_MAP_INJ_AETPS)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AETPS].handle, GetAETPSMap(true), GetAETPSMap(false));
 if (m_md[TYPE_MAP_INJ_AEMAP].state && mapType == TYPE_MAP_INJ_AEMAP)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AEMAP].handle, GetAEMAPMap(true), GetAEMAPMap(false));
 if (m_md[TYPE_MAP_INJ_AERPM].state && mapType == TYPE_MAP_INJ_AERPM)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AERPM].handle, GetAERPMMap(true), GetAERPMMap(false));
 if (m_md[TYPE_MAP_INJ_AFTSTR].state && mapType == TYPE_MAP_INJ_AFTSTR)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AFTSTR].handle, GetAftstrMap(true), GetAftstrMap(false));
 if (m_md[TYPE_MAP_INJ_IT].state && mapType == TYPE_MAP_INJ_IT)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_IT].handle, GetITMap(true), GetITMap(false));
 if (m_md[TYPE_MAP_INJ_ITRPM].state && mapType == TYPE_MAP_INJ_ITRPM)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_ITRPM].handle, GetITRPMMap(true), GetITRPMMap(false));
 if (m_md[TYPE_MAP_INJ_RIGID].state && mapType == TYPE_MAP_INJ_RIGID)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_RIGID].handle, GetRigidMap(true), GetRigidMap(false));
 if (m_md[TYPE_MAP_INJ_EGOCRV].state && mapType == TYPE_MAP_INJ_EGOCRV)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_EGOCRV].handle, GetEGOCurveMap(true), GetEGOCurveMap(false));
 if (m_md[TYPE_MAP_INJ_IACC].state && mapType == TYPE_MAP_INJ_IACC)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IACC].handle, GetIACCMap(true), GetIACCMap(false));
 if (m_md[TYPE_MAP_INJ_IACCW].state && mapType == TYPE_MAP_INJ_IACCW)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IACCW].handle, GetIACCWMap(true), GetIACCWMap(false));
 if (m_md[TYPE_MAP_INJ_IATCLT].state && mapType == TYPE_MAP_INJ_IATCLT)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IATCLT].handle, GetIATCLTMap(true), GetIATCLTMap(false));
 if (m_md[TYPE_MAP_INJ_TPSSWT].state && mapType == TYPE_MAP_INJ_TPSSWT)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_TPSSWT].handle, GetTpsswtMap(true), GetTpsswtMap(false));
 if (m_md[TYPE_MAP_INJ_GTSC].state && mapType == TYPE_MAP_INJ_GTSC)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_GTSC].handle, GetGtscMap(true), GetGtscMap(false));
 if (m_md[TYPE_MAP_INJ_GPSC].state && mapType == TYPE_MAP_INJ_GPSC)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_GPSC].handle, GetGpscMap(true), GetGpscMap(false));
 if (m_md[TYPE_MAP_INJ_ATSC].state && mapType == TYPE_MAP_INJ_ATSC)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_ATSC].handle, GetAtscMap(true), GetAtscMap(false));
 if (m_md[TYPE_MAP_PWM1].state && mapType == TYPE_MAP_PWM1)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_PWM1].handle, GetPwm1Map(true), GetPwm1Map(false));
 if (m_md[TYPE_MAP_PWM2].state && mapType == TYPE_MAP_PWM2)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_PWM2].handle, GetPwm2Map(true), GetPwm2Map(false));
 if (m_md[TYPE_MAP_INJ_IACMAT].state && mapType == TYPE_MAP_INJ_IACMAT)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IACMAT].handle, GetIACMATMap(true), GetIACMATMap(false));
 if (m_md[TYPE_MAP_INJ_TPSZON].state && mapType == TYPE_MAP_INJ_TPSZON)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_TPSZON].handle, GetTpszonMap(true), GetTpszonMap(false));

 if (m_OnMapChanged)
  m_OnMapChanged(mapType);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapClosedInj(HWND hwnd, int mapType)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->Deactivate();

 m_grid_map_state_inj = 0;
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(mp_gridModeEditorInjDlg->m_hWnd, TYPE_MAP_GME_INJ_WND);
}

//------------------------------------------------------------------------
float __cdecl CButtonsPanel::OnValueTransformITMap(void* i_param, float source, int direction)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return source;
 }

 return MathHelpers::InjTimValueTransform(_this->m_it_mode, source, direction);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnGetXAxisLabelCLT(LPTSTR io_label_string, int index, void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }
 _stprintf(io_label_string, _T("%d"), MathHelpers::Round(_this->GetCLTGrid()[index]));
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeTempIdlMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_DA_TEMPI_CORR);
 if (_this->mp_gridModeEditorIgnDlg.get())
  _this->mp_gridModeEditorIgnDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseTempIdlMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_DA_TEMPI_CORR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_DA_TEMPI_CORR].handle, TYPE_MAP_DA_TEMPI_CORR);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationTempIdlMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_DA_TEMPI_CORR].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationTpszonMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_TPSZON].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeTpszonMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_TPSZON);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseTpszonMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_TPSZON].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_TPSZON].handle, TYPE_MAP_INJ_TPSZON);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationCylMultMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_CYLMULT].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeCylMultMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_CYLMULT);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseCylMultMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_CYLMULT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_CYLMULT].handle, TYPE_MAP_INJ_CYLMULT);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnWndActivationCylAddMap(void* i_param, long cmd)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 //allow controller to process event
 _this->OnWndActivation(_this->m_md[TYPE_MAP_INJ_CYLADD].handle, cmd);
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeCylAddMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(TYPE_MAP_INJ_CYLADD);
 if (_this->mp_gridModeEditorInjDlg.get())
  _this->mp_gridModeEditorInjDlg->UpdateView();
}

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnCloseCylAddMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }
 _this->m_md[TYPE_MAP_INJ_CYLADD].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[TYPE_MAP_INJ_CYLADD].handle, TYPE_MAP_INJ_CYLADD);
}

//------------------------------------------------------------------------
