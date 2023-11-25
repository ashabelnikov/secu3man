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
#include "io-core/MapIds.h"
#include "ui-core/MapEditorCtrl.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ToolTipCtrlEx.h"

//------------------------------------------------------------------------
void __cdecl CButtonsPanel::OnChangeSettingsCME(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 for(int i = ETMT_ALL_START; i <= ETMT_ALL_END - 2; ++i) //all maps except two last pseudo maps
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
void __cdecl CButtonsPanel::OnChangeStartMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_IGN_START);
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
 _this->m_md[ETMT_IGN_START].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_IGN_START].handle, ETMT_IGN_START);
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
  _this->m_OnMapChanged(ETMT_IGN_IDLE);
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
 _this->m_md[ETMT_IGN_IDLE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_IGN_IDLE].handle, ETMT_IGN_IDLE);
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
  _this->m_OnMapChanged(ETMT_IGN_WORK);
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
 _this->m_md[ETMT_IGN_WORK].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_IGN_WORK].handle, ETMT_IGN_WORK);
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
  _this->m_OnMapChanged(ETMT_IGN_TEMP);
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
 _this->m_md[ETMT_IGN_TEMP].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_IGN_TEMP].handle, ETMT_IGN_TEMP);
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
 _this->OnWndActivation(_this->m_md[ETMT_IGN_START].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_IGN_IDLE].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_IGN_WORK].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_IGN_TEMP].handle, cmd);
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
  _this->m_OnMapChanged(ETMT_INJ_VE);
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
 _this->m_md[ETMT_INJ_VE].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_VE].handle, ETMT_INJ_VE);
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
  _this->m_OnMapChanged(ETMT_INJ_VE2);
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
 _this->m_md[ETMT_INJ_VE2].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_VE2].handle, ETMT_INJ_VE2);
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
  _this->m_OnMapChanged(ETMT_INJ_AFR);
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
 _this->m_md[ETMT_INJ_AFR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_AFR].handle, ETMT_INJ_AFR);
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
  _this->m_OnMapChanged(ETMT_INJ_CRNK);
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
 _this->m_md[ETMT_INJ_CRNK].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_CRNK].handle, ETMT_INJ_CRNK);
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
  _this->m_OnMapChanged(ETMT_INJ_WRMP);
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
 _this->m_md[ETMT_INJ_WRMP].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_WRMP].handle, ETMT_INJ_WRMP);
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
  _this->m_OnMapChanged(ETMT_INJ_DEAD);
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
 _this->m_md[ETMT_INJ_DEAD].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_DEAD].handle, ETMT_INJ_DEAD);
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
  _this->m_OnMapChanged(ETMT_INJ_IDLR);
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
 _this->m_md[ETMT_INJ_IDLR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_IDLR].handle, ETMT_INJ_IDLR);
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
  _this->m_OnMapChanged(ETMT_INJ_IDLC);
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
 _this->m_md[ETMT_INJ_IDLC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_IDLC].handle, ETMT_INJ_IDLC);
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
  _this->m_OnMapChanged(ETMT_INJ_THRASS);
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
 _this->m_md[ETMT_INJ_THRASS].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_THRASS].handle, ETMT_INJ_THRASS);
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
  _this->m_OnMapChanged(ETMT_INJ_AETPS);
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
 _this->m_md[ETMT_INJ_AETPS].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_AETPS].handle, ETMT_INJ_AETPS);
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
  _this->m_OnMapChanged(ETMT_INJ_AEMAP);
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
 _this->m_md[ETMT_INJ_AEMAP].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_AEMAP].handle, ETMT_INJ_AEMAP);
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
  _this->m_OnMapChanged(ETMT_INJ_AERPM);
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
 _this->m_md[ETMT_INJ_AERPM].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_AERPM].handle, ETMT_INJ_AERPM);
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
  _this->m_OnMapChanged(ETMT_INJ_AFTSTR);
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
 _this->m_md[ETMT_INJ_AFTSTR].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_AFTSTR].handle, ETMT_INJ_AFTSTR);
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
  _this->m_OnMapChanged(ETMT_INJ_IT);
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
 _this->m_md[ETMT_INJ_IT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_IT].handle, ETMT_INJ_IT);
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
  _this->m_OnMapChanged(ETMT_INJ_ITRPM);
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
 _this->m_md[ETMT_INJ_ITRPM].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_ITRPM].handle, ETMT_INJ_ITRPM);
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
  _this->m_OnMapChanged(ETMT_INJ_RIGID);
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
 _this->m_md[ETMT_INJ_RIGID].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_RIGID].handle, ETMT_INJ_RIGID);
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
  _this->m_OnMapChanged(ETMT_INJ_EGOCRV);
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
 _this->m_md[ETMT_INJ_EGOCRV].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_EGOCRV].handle, ETMT_INJ_EGOCRV);
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
  _this->GetMap(ETMT_INJ_EGOCRV, false)[16 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_INJ_EGOCRV);
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
  _this->m_OnMapChanged(ETMT_INJ_IACC);
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
 _this->m_md[ETMT_INJ_IACC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_IACC].handle, ETMT_INJ_IACC);
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
  _this->GetMap(ETMT_INJ_IACC, false)[8 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_INJ_IACC);
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
  _this->m_OnMapChanged(ETMT_INJ_IACCW);
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
 _this->m_md[ETMT_INJ_IACCW].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_IACCW].handle, ETMT_INJ_IACCW);
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
  _this->GetMap(ETMT_INJ_IACCW, false)[16 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_INJ_IACCW);
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
  _this->m_OnMapChanged(ETMT_INJ_IATCLT);
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
 _this->m_md[ETMT_INJ_IATCLT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_IATCLT].handle, ETMT_INJ_IATCLT);
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
  _this->GetMap(ETMT_INJ_IATCLT, false)[8 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_INJ_IATCLT);
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
  _this->m_OnMapChanged(ETMT_INJ_TPSSWT);
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
 _this->m_md[ETMT_INJ_TPSSWT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_TPSSWT].handle, ETMT_INJ_TPSSWT);
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
  _this->m_OnMapChanged(ETMT_INJ_GTSC);
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
 _this->m_md[ETMT_INJ_GTSC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_GTSC].handle, ETMT_INJ_GTSC);
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
  _this->m_OnMapChanged(ETMT_INJ_GPSC);
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
 _this->m_md[ETMT_INJ_GPSC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_GPSC].handle, ETMT_INJ_GPSC);
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
  _this->GetMap(ETMT_INJ_GPSC, false)[17 + i_type] = i_value;

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_INJ_GPSC);
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
  _this->m_OnMapChanged(ETMT_INJ_ATSC);
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
 _this->m_md[ETMT_INJ_ATSC].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_ATSC].handle, ETMT_INJ_ATSC);
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
  _this->m_OnMapChanged(ETMT_PWM1);
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
 _this->m_md[ETMT_PWM1].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_PWM1].handle, ETMT_PWM1);
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
  _this->m_OnMapChanged(ETMT_PWM2);
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
 _this->m_md[ETMT_PWM2].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_PWM2].handle, ETMT_PWM2);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_IT].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_VE].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_VE2].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_AFR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_CRNK].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_WRMP].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_DEAD].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_IDLR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_IDLC].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_THRASS].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_AETPS].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_AEMAP].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_AERPM].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_AFTSTR].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_ITRPM].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_RIGID].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_EGOCRV].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_IACC].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_IACCW].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_IATCLT].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_TPSSWT].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_GTSC].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_GPSC].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_ATSC].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_PWM1].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_PWM2].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_IACMAT].handle, cmd);
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
  _this->m_OnMapChanged(ETMT_INJ_IACMAT);
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
 _this->m_md[ETMT_INJ_IACMAT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_IACMAT].handle, ETMT_INJ_IACMAT);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapChangedIgn(int mapType)
{
 for(int i = ETMT_IG_START; i <= ETMT_IG_END; ++i)
 {
  if (Is3DMap(i))
   DLL::Chart3DUpdate(m_md[i].handle, GetMap(i, true), GetMap(i, false));
  else
   DLL::Chart2DUpdate(m_md[i].handle, GetMap(i, true), GetMap(i, false));
 }

 if (m_OnMapChanged)
  m_OnMapChanged(mapType);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapClosedIgn(HWND hwnd, int mapType)
{
 m_md[ETMT_GME_IGN_WND].state = 0;
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(mp_gridModeEditorIgnDlg->m_hWnd, ETMT_GME_IGN_WND);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapChangedInj(int mapType)
{
 for(int i = ETMT_IN_START; i <= ETMT_IN_END; ++i)
 {
  if (Is3DMap(i))
   DLL::Chart3DUpdate(m_md[i].handle, GetMap(i, true), GetMap(i, false));
  else
   DLL::Chart2DUpdate(m_md[i].handle, GetMap(i, true), GetMap(i, false));
 }

 if (m_OnMapChanged)
  m_OnMapChanged(mapType);
}

//------------------------------------------------------------------------
void CButtonsPanel::OnGridMapClosedInj(HWND hwnd, int mapType)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->Deactivate();

 m_md[ETMT_GME_INJ_WND].state = 0;
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(mp_gridModeEditorInjDlg->m_hWnd, ETMT_GME_INJ_WND);
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
void __cdecl CButtonsPanel::OnChangeTempIdlMap(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
  _this->m_OnMapChanged(ETMT_IGN_TEMPI);
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
 _this->m_md[ETMT_IGN_TEMPI].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_IGN_TEMPI].handle, ETMT_IGN_TEMPI);
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
 _this->OnWndActivation(_this->m_md[ETMT_IGN_TEMPI].handle, cmd);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_TPSZON].handle, cmd);
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
  _this->m_OnMapChanged(ETMT_INJ_TPSZON);
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
 _this->m_md[ETMT_INJ_TPSZON].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_TPSZON].handle, ETMT_INJ_TPSZON);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_CYLMULT].handle, cmd);
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
  _this->m_OnMapChanged(ETMT_INJ_CYLMULT);
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
 _this->m_md[ETMT_INJ_CYLMULT].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_CYLMULT].handle, ETMT_INJ_CYLMULT);
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
 _this->OnWndActivation(_this->m_md[ETMT_INJ_CYLADD].handle, cmd);
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
  _this->m_OnMapChanged(ETMT_INJ_CYLADD);
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
 _this->m_md[ETMT_INJ_CYLADD].state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_md[ETMT_INJ_CYLADD].handle, ETMT_INJ_CYLADD);
}

//------------------------------------------------------------------------
