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
#include "common/dpiaware.h"
#include "common/MathHelpers.h"
#include "DLLLinkedFunctions.h"
#include "GridModeEditorIgnDlg.h"
#include "io-core/secu3io.h"
#include "MapIds.h"
#include "ui-core/WndScroller.h"
#include "AutoTuneController.h"
#include "ui-core/MapEditorCtrl.h"
#include "ui-core/ToolTipCtrlEx.h"

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
 if (_this->mp_gridModeEditorIgnDlg.get())
  _this->mp_gridModeEditorIgnDlg->UpdateView();
}

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
  _this->GetGpscMap(false)[9 + i_type] = i_value;

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
 if (m_md[TYPE_MAP_INJ_AETPS].state && mapType == TYPE_MAP_INJ_AETPS)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AETPS].handle, GetAETPSMap(true), GetAETPSMap(false));
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

//const UINT CButtonsPanel::IDD = IDD_TD_BUTTONS_PANEL; //WTF?

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel dialog

CButtonsPanel::CButtonsPanel(UINT dialog_id, CWnd* pParent /*=NULL*/, bool enableAutoTune /*=false*/)
: Super(dialog_id, pParent)
, m_grid_map_state_ign(0)
, m_grid_map_state_inj(0)
, m_charts_enabled(-1)
, IDD(IDD_TD_BUTTONS_PANEL)
, m_en_aa_indication(false)
, mp_scr(new CWndScroller)
, m_scrl_view(855)
, m_fuel_injection(false)
, m_gasdose(false)
, m_carb_afr(false)
, m_en_gas_corr(false)
, m_choke_op_enabled(false)
, mp_autoTuneCntr(enableAutoTune ? new CAutoTuneController() : NULL)
, m_children_charts(false)
, m_openedChart(NULL)
, m_toggleMapWnd(false)
, m_it_mode(0)
{
 for(int i = TYPE_MAP_SET_START; i <= TYPE_MAP_SET_END; ++i)
 {
  MapData md;
  m_md.insert(std::make_pair(i, md));
 }
 memset(m_rpm_grid_values, 0, 16 * sizeof(float));
 memset(m_clt_grid_values, 0, 16 * sizeof(float));
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
 DDX_Control(pDX, IDC_TD_GME_IGN_CHECK, m_grid_mode_editing_ign_check);
 DDX_Control(pDX, IDC_TD_GME_INJ_CHECK, m_grid_mode_editing_inj_check);
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
 DDX_Control(pDX, IDC_TD_VIEW_IATCLT_MAP,  m_view_iatclt_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_TPSSWT_MAP,  m_view_tpsswt_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_GTSC_MAP,  m_view_gtsc_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_GPSC_MAP,  m_view_gpsc_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_ATSC_MAP,  m_view_atsc_map_btn);
}

BEGIN_MESSAGE_MAP(CButtonsPanel, Super)
 ON_BN_CLICKED(IDC_TD_VIEW_START_MAP,OnViewStartMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IDLE_MAP, OnViewIdleMap)
 ON_BN_CLICKED(IDC_TD_VIEW_WORK_MAP, OnViewWorkMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TEMP_MAP, OnViewTempMap)
 ON_BN_CLICKED(IDC_TD_GME_IGN_CHECK, OnGridModeEditingIgn)
 ON_BN_CLICKED(IDC_TD_GME_INJ_CHECK, OnGridModeEditingInj)
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
 ON_BN_CLICKED(IDC_TD_VIEW_IATCLT_MAP, OnViewIATCLTMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TPSSWT_MAP, OnViewTpsswtMap)
 ON_BN_CLICKED(IDC_TD_VIEW_GTSC_MAP, OnViewGtscMap)
 ON_BN_CLICKED(IDC_TD_VIEW_GPSC_MAP, OnViewGpscMap)
 ON_BN_CLICKED(IDC_TD_VIEW_ATSC_MAP, OnViewAtscMap)

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
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IATCLT_MAP, OnUpdateViewIATCLTMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TPSSWT_MAP, OnUpdateViewTpsswtMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_GTSC_MAP, OnUpdateViewGtscMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_GPSC_MAP, OnUpdateViewGpscMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_GME_IGN_CHECK, OnUpdateGridModeEditingIgn)
 ON_UPDATE_COMMAND_UI(IDC_TD_GME_INJ_CHECK, OnUpdateGridModeEditingInj)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_ATSC_MAP, OnUpdateViewAtscMap)
 ON_WM_TIMER()
 ON_WM_DESTROY()
 ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel message handlers

BOOL CButtonsPanel::OnInitDialog()
{
 Super::OnInitDialog();

 SetTimer(TIMER_ID, 250, NULL);

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_grid_mode_editing_ign_check, MLL::GetString(IDS_TD_GME_IGN_CHECK_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_grid_mode_editing_inj_check, MLL::GetString(IDS_TD_GME_INJ_CHECK_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_start_map_btn, MLL::GetString(IDS_TD_VIEW_START_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_idle_map_btn, MLL::GetString(IDS_TD_VIEW_IDLE_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_work_map_btn, MLL::GetString(IDS_TD_VIEW_WORK_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_temp_map_btn, MLL::GetString(IDS_TD_VIEW_TEMP_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_crnk_map_btn, MLL::GetString(IDS_TD_VIEW_CRNK_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_ve_map_btn, MLL::GetString(IDS_TD_VIEW_VE_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_afr_map_btn, MLL::GetString(IDS_TD_VIEW_AFR_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_wrmp_map_btn, MLL::GetString(IDS_TD_VIEW_WRMP_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_atsc_map_btn, MLL::GetString(IDS_TD_VIEW_ATSC_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_aetps_map_btn, MLL::GetString(IDS_TD_VIEW_AETPS_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_aerpm_map_btn, MLL::GetString(IDS_TD_VIEW_AERPM_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_aftstr_map_btn, MLL::GetString(IDS_TD_VIEW_AFTSTR_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_gtsc_map_btn, MLL::GetString(IDS_TD_VIEW_GTSC_MAP_TT))); 
 VERIFY(mp_ttc->AddWindow(&m_view_gpsc_map_btn, MLL::GetString(IDS_TD_VIEW_GPSC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_dead_map_btn, MLL::GetString(IDS_TD_VIEW_DEAD_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_idlr_map_btn, MLL::GetString(IDS_TD_VIEW_IDLR_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_idlc_map_btn, MLL::GetString(IDS_TD_VIEW_IDLC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_it_map_btn, MLL::GetString(IDS_TD_VIEW_IT_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_itrpm_map_btn, MLL::GetString(IDS_TD_VIEW_ITRPM_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_rigid_map_btn, MLL::GetString(IDS_TD_VIEW_RIGID_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_egocrv_map_btn, MLL::GetString(IDS_TD_VIEW_EGOCRV_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_iacc_map_btn, MLL::GetString(IDS_TD_VIEW_IACC_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_iaccw_map_btn, MLL::GetString(IDS_TD_VIEW_IACCW_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_iatclt_map_btn, MLL::GetString(IDS_TD_VIEW_IATCLT_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_view_tpsswt_map_btn, MLL::GetString(IDS_TD_VIEW_TPSSWT_MAP_TT))); 
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewStartMap()
{
 //If button was released, then close editor's window
 if (m_view_start_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_DA_START].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_DA_START].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_DA_START].state = 1;
  m_md[TYPE_MAP_DA_START].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetStartMap(true),GetStartMap(false),-15.0,55.0,SECU3IO::start_map_rpm_slots,16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_START_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_DA_START].handle, OnWndActivationStartMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_DA_START].handle,OnChangeStartMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_DA_START].handle,OnChangeSettingsCME,this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_DA_START].handle,OnCloseStartMap,this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_DA_START].handle, m_md[TYPE_MAP_DA_START].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_START].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_DA_START].handle, TYPE_MAP_DA_START);

  DLL::Chart2DShow(m_md[TYPE_MAP_DA_START].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_DA_START].handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewIdleMap()
{
 //If button was released, then close editor's window
 if (m_view_idle_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_DA_IDLE].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_DA_IDLE].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_DA_IDLE].state = 1;
  m_md[TYPE_MAP_DA_IDLE].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetIdleMap(true),GetIdleMap(false),-15.0,55.0,GetRPMGrid(),16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_IDLE_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_DA_IDLE].handle,OnWndActivationIdleMap,this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_DA_IDLE].handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_DA_IDLE].handle,OnChangeIdleMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_DA_IDLE].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_DA_IDLE].handle,OnCloseIdleMap,this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_DA_IDLE].handle, m_md[TYPE_MAP_DA_IDLE].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_IDLE].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_DA_IDLE].handle, TYPE_MAP_DA_IDLE);

  DLL::Chart2DShow(m_md[TYPE_MAP_DA_IDLE].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_DA_IDLE].handle);
 }
}

void CButtonsPanel::OnViewWorkMap()
{
 //If button was released, then close editor's window
 if (m_view_work_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_DA_WORK].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_DA_WORK].state)&&(DLL::Chart3DCreate))
 {
  m_md[TYPE_MAP_DA_WORK].state = 1;
  m_md[TYPE_MAP_DA_WORK].handle = DLL::Chart3DCreate(_ChartParentHwnd(), GetWorkMap(true),GetWorkMap(false),GetRPMGrid(),16,16,-15.0,55.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_WORK_MAP).c_str());
  DLL::Chart3DSetOnWndActivation(m_md[TYPE_MAP_DA_WORK].handle, OnWndActivationWorkMap, this);
  DLL::Chart3DSetOnGetAxisLabel(m_md[TYPE_MAP_DA_WORK].handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_md[TYPE_MAP_DA_WORK].handle,OnChangeWorkMap,this);
  DLL::Chart3DSetOnChangeSettings(m_md[TYPE_MAP_DA_WORK].handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(m_md[TYPE_MAP_DA_WORK].handle,OnCloseWorkMap,this);
  DLL::Chart3DSetPtMovingStep(m_md[TYPE_MAP_DA_WORK].handle, m_md[TYPE_MAP_DA_WORK].ptMovStep);

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_DA_WORK].handle, TYPE_MAP_DA_WORK);

  DLL::Chart3DShow(m_md[TYPE_MAP_DA_WORK].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_DA_WORK].handle);
 }
}

void CButtonsPanel::OnViewTempMap()
{
 //If button was released, then close editor's window
 if (m_view_temp_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_DA_TEMP_CORR].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_DA_TEMP_CORR].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_DA_TEMP_CORR].state = 1;
  m_md[TYPE_MAP_DA_TEMP_CORR].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetTempMap(true),GetTempMap(false),-15.0,25.0,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_TEMPCORR_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_DA_TEMP_CORR].handle,OnWndActivationTempMap,this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_DA_TEMP_CORR].handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_DA_TEMP_CORR].handle,OnChangeTempMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_DA_TEMP_CORR].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_DA_TEMP_CORR].handle,OnCloseTempMap,this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_DA_TEMP_CORR].handle, m_md[TYPE_MAP_DA_TEMP_CORR].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_TEMP_CORR].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_DA_TEMP_CORR].handle, TYPE_MAP_DA_TEMP_CORR);

  DLL::Chart2DShow(m_md[TYPE_MAP_DA_TEMP_CORR].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_DA_TEMP_CORR].handle);
 }
}

void CButtonsPanel::OnViewVEMap()
{
 //If button was released, then close editor's window
 if (m_view_ve_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_VE].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_VE].state)&&(DLL::Chart3DCreate))
 {
  m_md[TYPE_MAP_INJ_VE].state = 1;
  m_md[TYPE_MAP_INJ_VE].handle = DLL::Chart3DCreate(_ChartParentHwnd(), GetVEMap(true),GetVEMap(false),GetRPMGrid(),16,16,0.0f,1.99f,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_VE_UNIT).c_str(),
    MLL::GetString(IDS_VE_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(m_md[TYPE_MAP_INJ_VE].handle, _T("#0.00"));
  DLL::Chart3DSetPtMovingStep(m_md[TYPE_MAP_INJ_VE].handle, m_md[TYPE_MAP_INJ_VE].ptMovStep);
  DLL::Chart3DSetOnWndActivation(m_md[TYPE_MAP_INJ_VE].handle, OnWndActivationVEMap, this);
  DLL::Chart3DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_VE].handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_md[TYPE_MAP_INJ_VE].handle,OnChangeVEMap,this);
  DLL::Chart3DSetOnChangeSettings(m_md[TYPE_MAP_INJ_VE].handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(m_md[TYPE_MAP_INJ_VE].handle,OnCloseVEMap,this);

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_VE].handle, TYPE_MAP_INJ_VE);

  DLL::Chart3DShow(m_md[TYPE_MAP_INJ_VE].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_VE].handle);
 }
}

void CButtonsPanel::OnViewAFRMap()
{
 //If button was released, then close editor's window
 if (m_view_afr_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_AFR].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_AFR].state)&&(DLL::Chart3DCreate))
 {
  m_md[TYPE_MAP_INJ_AFR].state = 1;
  m_md[TYPE_MAP_INJ_AFR].handle = DLL::Chart3DCreate(_ChartParentHwnd(), GetAFRMap(true),GetAFRMap(false),GetRPMGrid(),16,16,8.0f,22.0f,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFR_UNIT).c_str(),
    MLL::GetString(IDS_AFR_MAP).c_str());
  DLL::Chart3DSetPtValuesFormat(m_md[TYPE_MAP_INJ_AFR].handle, _T("#00.0"));
  DLL::Chart3DSetPtMovingStep(m_md[TYPE_MAP_INJ_AFR].handle, m_md[TYPE_MAP_INJ_AFR].ptMovStep);
  DLL::Chart3DSetOnWndActivation(m_md[TYPE_MAP_INJ_AFR].handle, OnWndActivationAFRMap, this);
  DLL::Chart3DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_AFR].handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_md[TYPE_MAP_INJ_AFR].handle,OnChangeAFRMap,this);
  DLL::Chart3DSetOnChangeSettings(m_md[TYPE_MAP_INJ_AFR].handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(m_md[TYPE_MAP_INJ_AFR].handle,OnCloseAFRMap,this);

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_AFR].handle, TYPE_MAP_INJ_AFR);

  DLL::Chart3DShow(m_md[TYPE_MAP_INJ_AFR].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_AFR].handle);
 }
}

void CButtonsPanel::OnViewCrnkMap()
{
 //If button was released, then close editor's window
 if (m_view_crnk_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_CRNK].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_CRNK].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_CRNK].state = 1;
  m_md[TYPE_MAP_INJ_CRNK].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetCrnkMap(true), GetCrnkMap(false), 0.25f, 100.0, GetCLTGrid(), 16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_INJPW_UNIT).c_str(),
    MLL::GetString(IDS_CRNK_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_CRNK].handle,OnWndActivationCrnkMap,this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_CRNK].handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_INJ_CRNK].handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_CRNK].handle, OnChangeCrnkMap, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_CRNK].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_CRNK].handle, OnCloseCrnkMap, this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_CRNK].handle, m_md[TYPE_MAP_INJ_CRNK].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_CRNK].handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_CRNK].handle, TYPE_MAP_INJ_CRNK);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_CRNK].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_CRNK].handle);
 }
}

void CButtonsPanel::OnViewWrmpMap()
{
 //if button was released, then close editor's window
 if (m_view_wrmp_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_WRMP].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_WRMP].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_WRMP].state = 1;
  m_md[TYPE_MAP_INJ_WRMP].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetWrmpMap(true),GetWrmpMap(false),0.0f,199.0f, GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_WRMP_UNIT).c_str(),
    MLL::GetString(IDS_WRMP_MAP).c_str(), false);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_WRMP].handle, m_md[TYPE_MAP_INJ_WRMP].ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_WRMP].handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_WRMP].handle,OnWndActivationWrmpMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_WRMP].handle,OnChangeWrmpMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_WRMP].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_WRMP].handle,OnCloseWrmpMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_WRMP].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_WRMP].handle, TYPE_MAP_INJ_WRMP);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_WRMP].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_WRMP].handle);
 }
}

void CButtonsPanel::OnViewDeadMap()
{
 //if button was released, then close editor's window
 if (m_view_dead_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_DEAD].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_DEAD].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_DEAD].state = 1;
  m_md[TYPE_MAP_INJ_DEAD].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetDeadMap(true), GetDeadMap(false), -1.0f, 12.0, SECU3IO::dwellcntrl_map_slots, 32,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_DEAD_UNIT).c_str(),
    MLL::GetString(IDS_DEAD_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_DEAD].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_DEAD].handle, m_md[TYPE_MAP_INJ_DEAD].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_DEAD].handle,OnWndActivationDeadMap,this);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_INJ_DEAD].handle, 1, _T("%.01f"));
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_DEAD].handle, OnChangeDeadMap, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_DEAD].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_DEAD].handle, OnCloseDeadMap, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_DEAD].handle, NULL, NULL); //<--actuate changes

   //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_DEAD].handle, TYPE_MAP_INJ_DEAD);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_DEAD].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_DEAD].handle);
 }
}

void CButtonsPanel::OnViewIdlrMap()
{
 //if button was released, then close editor's window
 if (m_view_idlr_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_IDLR].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_IDLR].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_IDLR].state = 1;
  m_md[TYPE_MAP_INJ_IDLR].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetIdlrMap(true),GetIdlrMap(false),0.0f,100.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_IAC_UNIT).c_str(),
    MLL::GetString(IDS_IDLR_MAP).c_str(), false);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_IDLR].handle, m_md[TYPE_MAP_INJ_IDLR].ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_IDLR].handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_IDLR].handle,OnWndActivationIdlrMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_IDLR].handle,OnChangeIdlrMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_IDLR].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_IDLR].handle,OnCloseIdlrMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IDLR].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_IDLR].handle, TYPE_MAP_INJ_IDLR);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_IDLR].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_IDLR].handle);
 }
}

void CButtonsPanel::OnViewIdlcMap()
{
 //if button was released, then close editor's window
 if (m_view_idlc_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_IDLC].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_IDLC].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_IDLC].state = 1;
  m_md[TYPE_MAP_INJ_IDLC].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetIdlcMap(true),GetIdlcMap(false),0.0f,100.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_IAC_UNIT).c_str(),
    MLL::GetString(IDS_IDLC_MAP).c_str(), false);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_IDLC].handle, m_md[TYPE_MAP_INJ_IDLC].ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_IDLC].handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_IDLC].handle,OnWndActivationIdlcMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_IDLC].handle,OnChangeIdlcMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_IDLC].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_IDLC].handle,OnCloseIdlcMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IDLC].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_IDLC].handle, TYPE_MAP_INJ_IDLC);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_IDLC].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_IDLC].handle);
 }
}

void CButtonsPanel::OnViewAETPSMap()
{
 //if button was released, then close editor's window
 if (m_view_aetps_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_AETPS].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_AETPS].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_AETPS].state = 1;
  const float bins_lims[5] = {-1000.0f, 1000.0f, 1.0f, 0.0f, 10.0f}; //min -1000%, max 1000%, inc 1%, 0 dec places, min diff 10%
  m_md[TYPE_MAP_INJ_AETPS].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetAETPSMap(true),GetAETPSMap(false),-55.0f,199.0f,bins_lims,8,
    MLL::GetString(IDS_MAPS_DPDT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AETPS_UNIT).c_str(),
    MLL::GetString(IDS_AETPS_MAP).c_str(), true); //<-- use horizontal axis bins
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_AETPS].handle, m_md[TYPE_MAP_INJ_AETPS].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_AETPS].handle,OnWndActivationAETPSMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_AETPS].handle,OnChangeAETPSMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_AETPS].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_AETPS].handle,OnCloseAETPSMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AETPS].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_AETPS].handle, TYPE_MAP_INJ_AETPS);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_AETPS].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_AETPS].handle);
 }
}

void CButtonsPanel::OnViewAERPMMap()
{
 //if button was released, then close editor's window
 if (m_view_aerpm_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_AERPM].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_AERPM].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_AERPM].state = 1;
  const float bins_lims[5] = {0.0f, 25000.0f, 100.0f, 0.0f, 100.0f}; //min 0min-1, max 25000min-1, inc 100min-1, 0 dec places, min diff 100min-1
  m_md[TYPE_MAP_INJ_AERPM].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetAERPMMap(true),GetAERPMMap(false),0.0f,199.0f,bins_lims,4,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AERPM_UNIT).c_str(),
    MLL::GetString(IDS_AERPM_MAP).c_str(), true); //<--use horizontal axis bins
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_AERPM].handle, m_md[TYPE_MAP_INJ_AERPM].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_AERPM].handle,OnWndActivationAERPMMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_AERPM].handle,OnChangeAERPMMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_AERPM].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_AERPM].handle,OnCloseAERPMMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AERPM].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_AERPM].handle, TYPE_MAP_INJ_AERPM);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_AERPM].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_AERPM].handle);
 }
}

void CButtonsPanel::OnViewAftstrMap()
{
 //if button was released, then close editor's window
 if (m_view_aftstr_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_AFTSTR].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_AFTSTR].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_AFTSTR].state = 1;
  m_md[TYPE_MAP_INJ_AFTSTR].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetAftstrMap(true),GetAftstrMap(false),0.0f,199.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFTSTR_UNIT).c_str(),
    MLL::GetString(IDS_AFTSTR_MAP).c_str(), false);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_AFTSTR].handle, m_md[TYPE_MAP_INJ_AFTSTR].ptMovStep);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_AFTSTR].handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_AFTSTR].handle,OnWndActivationAftstrMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_AFTSTR].handle,OnChangeAftstrMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_AFTSTR].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_AFTSTR].handle,OnCloseAftstrMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AFTSTR].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_AFTSTR].handle, TYPE_MAP_INJ_AFTSTR);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_AFTSTR].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_AFTSTR].handle);
 }
}

void CButtonsPanel::OnViewITMap()
{
 //if button was released, then close editor's window
 if (m_view_it_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_IT].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_IT].state)&&(DLL::Chart3DCreate))
 {
  m_md[TYPE_MAP_INJ_IT].state = 1;
  float y1, y2;
  _GetITModeRange(y1, y2);
  m_md[TYPE_MAP_INJ_IT].handle = DLL::Chart3DCreate(_ChartParentHwnd(), GetITMap(true),GetITMap(false),GetRPMGrid(),16,16,y1,y2,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString((m_it_mode == 1 || m_it_mode == 3) ? IDS_MAPS_INJTIM_UNIT1 : IDS_MAPS_INJTIM_UNIT).c_str(),
    MLL::GetString(IDS_IT_MAP).c_str());
  DLL::Chart3DSetPtMovingStep(m_md[TYPE_MAP_INJ_IT].handle, m_md[TYPE_MAP_INJ_IT].ptMovStep);
  DLL::Chart3DSetOnWndActivation(m_md[TYPE_MAP_INJ_IT].handle, OnWndActivationITMap, this);
  DLL::Chart3DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_IT].handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart3DSetOnChange(m_md[TYPE_MAP_INJ_IT].handle,OnChangeITMap,this);
  DLL::Chart3DSetOnChangeSettings(m_md[TYPE_MAP_INJ_IT].handle, OnChangeSettingsCME, this);
  DLL::Chart3DSetOnClose(m_md[TYPE_MAP_INJ_IT].handle,OnCloseITMap,this);
  DLL::Chart3DSetOnValueTransform(m_md[TYPE_MAP_INJ_IT].handle, OnValueTransformITMap, this);
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_IT].handle, NULL, NULL); //<--actuate changes
  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_IT].handle, TYPE_MAP_INJ_IT);

  DLL::Chart3DShow(m_md[TYPE_MAP_INJ_IT].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_IT].handle);
 }
}

void CButtonsPanel::OnViewITRPMMap()
{
 //if button was released, then close editor's window
 if (m_view_itrpm_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_ITRPM].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_ITRPM].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_ITRPM].state = 1;
  m_md[TYPE_MAP_INJ_ITRPM].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetITRPMMap(true),GetITRPMMap(false),0.0f,2500.0f,GetCLTGrid(),16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_ITRPM_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_ITRPM].handle, _T("#0"));
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_ITRPM].handle, 1, OnGetXAxisLabelCLT, this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_ITRPM].handle, m_md[TYPE_MAP_INJ_ITRPM].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_ITRPM].handle,OnWndActivationITRPMMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_ITRPM].handle,OnChangeITRPMMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_ITRPM].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_ITRPM].handle,OnCloseITRPMMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_ITRPM].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_ITRPM].handle, TYPE_MAP_INJ_ITRPM);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_ITRPM].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_ITRPM].handle);
 }
}

void CButtonsPanel::OnViewRigidMap()
{
 //if button was released, then close editor's window
 if (m_view_rigid_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_RIGID].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_RIGID].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_RIGID].state = 1;
  m_md[TYPE_MAP_INJ_RIGID].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetRigidMap(true),GetRigidMap(false),0.0f,5.0f,SECU3IO::rigid_map_slots,8,
    MLL::GetString(IDS_MAPS_RIGID_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_RIGID_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_RIGID].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_RIGID].handle, m_md[TYPE_MAP_INJ_RIGID].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_RIGID].handle,OnWndActivationRigidMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_RIGID].handle,OnChangeRigidMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_RIGID].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_RIGID].handle,OnCloseRigidMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_RIGID].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_RIGID].handle, TYPE_MAP_INJ_RIGID);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_RIGID].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_RIGID].handle);
 }
}

void CButtonsPanel::OnViewEGOCrvMap()
{
 //If button was released, then close editor's window
 if (m_view_egocrv_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_EGOCRV].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_EGOCRV].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_EGOCRV].state = 1;
  m_md[TYPE_MAP_INJ_EGOCRV].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetEGOCurveMap(true), GetEGOCurveMap(false), 6.00, 24.00, NULL, 16,
    MLL::GetString(IDS_MAPS_VOLT_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_AFR_UNIT).c_str(),
    MLL::GetString(IDS_EGO_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_EGOCRV].handle,OnWndActivationEGOCrvMap, this);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_EGOCRV].handle, _T("#0.00"));
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_INJ_EGOCRV].handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_INJ_EGOCRV].handle, 1, true, 0, 5.5f, 0, 5.5f, 0.01f, -1, -1, OnChangeEGOCrvXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_EGOCRV].handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_EGOCRV].handle, OnChangeEGOCrvMap, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_EGOCRV].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_EGOCRV].handle, OnCloseEGOCrvMap, this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_EGOCRV].handle, m_md[TYPE_MAP_INJ_EGOCRV].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_EGOCRV].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_EGOCRV].handle, 1, GetEGOCurveMap(false)[16], GetEGOCurveMap(false)[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_EGOCRV].handle, TYPE_MAP_INJ_EGOCRV);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_EGOCRV].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_EGOCRV].handle);
 }
}

void CButtonsPanel::OnViewIACCMap()
{
 //If button was released, then close editor's window
 if (m_view_iacc_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_IACC].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_IACC].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_IACC].state = 1;
  m_md[TYPE_MAP_INJ_IACC].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetIACCMap(true), GetIACCMap(false), 0.00f, 1.99f, NULL, 8,
    MLL::GetString(IDS_MAPS_IACPOS_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_IACC_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_IACC].handle,OnWndActivationIACCMap, this);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_IACC].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_IACC].handle, m_md[TYPE_MAP_INJ_IACC].ptMovStep);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_INJ_IACC].handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_INJ_IACC].handle, 1, true, 0, 100.0f, 0, 100.0f, 0.5f, -1, -1, OnChangeIACCXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_IACC].handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_IACC].handle, OnChangeIACCMap, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_IACC].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_IACC].handle, OnCloseIACCMap, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IACC].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_IACC].handle, 1, GetIACCMap(false)[8], GetIACCMap(false)[8+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_IACC].handle, TYPE_MAP_INJ_IACC);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_IACC].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_IACC].handle);
 }
}

void CButtonsPanel::OnViewIACCWMap()
{
 //If button was released, then close editor's window
 if (m_view_iaccw_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_IACCW].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_IACCW].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_IACCW].state = 1;
  m_md[TYPE_MAP_INJ_IACCW].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetIACCWMap(true), GetIACCWMap(false), 0.00, 1.00, NULL, 16,
    MLL::GetString(IDS_MAPS_TPSPOS_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_IACCW_CURVE_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_IACCW].handle,OnWndActivationIACCWMap, this);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_IACCW].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_IACCW].handle, m_md[TYPE_MAP_INJ_IACCW].ptMovStep);
  DLL::Chart2DSetAxisValuesFormat(m_md[TYPE_MAP_INJ_IACCW].handle, 1, _T("%.02f"));
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_INJ_IACCW].handle, 1, true, 0, 100.0f, 0, 100.0f, 0.5f, -1, -1, OnChangeIACCWXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_IACCW].handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_IACCW].handle, OnChangeIACCWMap, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_IACCW].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_IACCW].handle, OnCloseIACCWMap, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IACCW].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_IACCW].handle, 1, GetIACCWMap(false)[16], GetIACCWMap(false)[16+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_IACCW].handle, TYPE_MAP_INJ_IACCW);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_IACCW].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_IACCW].handle);
 }
}

void CButtonsPanel::OnViewIATCLTMap()
{
 //If button was released, then close editor's window
 if (m_view_iatclt_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_IATCLT].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_IATCLT].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_IATCLT].state = 1;
  m_md[TYPE_MAP_INJ_IATCLT].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetIATCLTMap(true), GetIATCLTMap(false), 0.00, 1.00, NULL, 8,
    MLL::GetString(IDS_MAPS_AIRFLOW_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_IATCLT_CORR_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_IATCLT].handle,OnWndActivationIATCLTMap, this);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_IATCLT].handle, _T("#0.000"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_IATCLT].handle, m_md[TYPE_MAP_INJ_IATCLT].ptMovStep);
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_INJ_IATCLT].handle, 1, true, 0, 2000000.0f, 0, 2000000.0f, 100.0f, 7, 0, OnChangeIATCLTXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_IATCLT].handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_IATCLT].handle, OnChangeIATCLTMap, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_IATCLT].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_IATCLT].handle, OnCloseIATCLTMap, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IATCLT].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_IATCLT].handle, 1, GetIATCLTMap(false)[8], GetIATCLTMap(false)[8+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_IATCLT].handle, TYPE_MAP_INJ_IATCLT);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_IATCLT].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_IATCLT].handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewTpsswtMap()
{
 //If button was released, then close editor's window
 if (m_view_tpsswt_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_TPSSWT].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_TPSSWT].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_TPSSWT].state = 1;
  m_md[TYPE_MAP_INJ_TPSSWT].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetTpsswtMap(true),GetTpsswtMap(false),0.0,100.0,GetRPMGrid(),16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_TPSSWT_UNIT).c_str(),
    MLL::GetString(IDS_TPSSWT_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_TPSSWT].handle,OnWndActivationTpsswtMap,this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_TPSSWT].handle, 1, OnGetXAxisLabelRPM, this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_TPSSWT].handle,OnChangeTpsswtMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_TPSSWT].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_TPSSWT].handle,OnCloseTpsswtMap,this);
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_TPSSWT].handle, m_md[TYPE_MAP_INJ_TPSSWT].ptMovStep);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_TPSSWT].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_TPSSWT].handle, TYPE_MAP_INJ_TPSSWT);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_TPSSWT].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_TPSSWT].handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewGtscMap()
{
 //if button was released, then close editor's window
 if (m_view_gtsc_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_GTSC].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_GTSC].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_GTSC].state = 1;
  m_md[TYPE_MAP_INJ_GTSC].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetGtscMap(true),GetGtscMap(false),0.01f,1.99f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_GTSC_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_GTSC].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_GTSC].handle, m_md[TYPE_MAP_INJ_GTSC].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_GTSC].handle,OnWndActivationGtscMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_GTSC].handle,OnChangeGtscMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_GTSC].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_GTSC].handle,OnCloseGtscMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_GTSC].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_GTSC].handle, TYPE_MAP_INJ_GTSC);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_GTSC].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_GTSC].handle);
 }
}

void CButtonsPanel::OnViewGpscMap()
{
 //If button was released, then close editor's window
 if (m_view_gpsc_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_GPSC].handle, WM_CLOSE, 0, 0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_GPSC].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_GPSC].state = 1;
  m_md[TYPE_MAP_INJ_GPSC].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetGpscMap(true), GetGpscMap(false), 0.00f, 1.99f, NULL, 9,
    MLL::GetString(IDS_MAPS_PRESS_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_GPSC_MAP).c_str(), false);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_GPSC].handle,OnWndActivationGpscMap, this);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_GPSC].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_GPSC].handle, m_md[TYPE_MAP_INJ_GPSC].ptMovStep);
  DLL::Chart2DSetAxisEdits(m_md[TYPE_MAP_INJ_GPSC].handle, 1, true, 50.0f, 500.0f, 100.0f, 500.0f, 2.0f, 3, 0, OnChangeGpscXAxisEdit, this);
  DLL::Chart2DSetOnGetAxisLabel(m_md[TYPE_MAP_INJ_GPSC].handle, 1, NULL, NULL);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_GPSC].handle, OnChangeGpscMap, this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_GPSC].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_GPSC].handle, OnCloseGpscMap, this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_GPSC].handle, NULL, NULL); //<--actuate changes
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_GPSC].handle, 1, GetGpscMap(false)[9], GetGpscMap(false)[9+1]);

  //allow controller to detect closing of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_GPSC].handle, TYPE_MAP_INJ_GPSC);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_GPSC].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_GPSC].handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewAtscMap()
{
 //if button was released, then close editor's window
 if (m_view_atsc_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_md[TYPE_MAP_INJ_ATSC].handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_md[TYPE_MAP_INJ_ATSC].state)&&(DLL::Chart2DCreate))
 {
  m_md[TYPE_MAP_INJ_ATSC].state = 1;
  m_md[TYPE_MAP_INJ_ATSC].handle = DLL::Chart2DCreate(_ChartParentHwnd(), GetAtscMap(true),GetAtscMap(false),0.5f,1.5f,SECU3IO::temp_map_tmp_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_COEFF_UNIT).c_str(),
    MLL::GetString(IDS_ATSC_MAP).c_str(), false);
  DLL::Chart2DSetPtValuesFormat(m_md[TYPE_MAP_INJ_ATSC].handle, _T("#0.00"));
  DLL::Chart2DSetPtMovingStep(m_md[TYPE_MAP_INJ_ATSC].handle, m_md[TYPE_MAP_INJ_ATSC].ptMovStep);
  DLL::Chart2DSetOnWndActivation(m_md[TYPE_MAP_INJ_ATSC].handle,OnWndActivationAtscMap,this);
  DLL::Chart2DSetOnChange(m_md[TYPE_MAP_INJ_ATSC].handle,OnChangeAtscMap,this);
  DLL::Chart2DSetOnChangeSettings(m_md[TYPE_MAP_INJ_ATSC].handle, OnChangeSettingsCME, this);
  DLL::Chart2DSetOnClose(m_md[TYPE_MAP_INJ_ATSC].handle,OnCloseAtscMap,this);
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_ATSC].handle, NULL, NULL); //<--actuate changes

  //let controller to know about opening of this window
  OnOpenMapWnd(m_md[TYPE_MAP_INJ_ATSC].handle, TYPE_MAP_INJ_ATSC);

  DLL::Chart2DShow(m_md[TYPE_MAP_INJ_ATSC].handle, true);
 }
 else
 {
  ::SetFocus(m_md[TYPE_MAP_INJ_ATSC].handle);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnGridModeEditingIgn()
{
 if (m_grid_mode_editing_ign_check.GetCheck()==BST_CHECKED)
 {
  mp_gridModeEditorIgnDlg.reset(new CGridModeEditorIgnDlg());
  mp_gridModeEditorIgnDlg->BindMaps(m_md[TYPE_MAP_DA_START].active, m_md[TYPE_MAP_DA_IDLE].active, m_md[TYPE_MAP_DA_WORK].active, m_md[TYPE_MAP_DA_TEMP_CORR].active);
  mp_gridModeEditorIgnDlg->BindRPMGrid(GetRPMGrid());
  mp_gridModeEditorIgnDlg->BindCLTGrid(GetCLTGrid());
  mp_gridModeEditorIgnDlg->setIsAllowed(fastdelegate::MakeDelegate(this, &CButtonsPanel::IsAllowed));
  mp_gridModeEditorIgnDlg->setOnMapChanged(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapChangedIgn));
  mp_gridModeEditorIgnDlg->setOnCloseMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapClosedIgn));
  mp_gridModeEditorIgnDlg->setOnOpenMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnOpenMapWnd));
  mp_gridModeEditorIgnDlg->EnableAdvanceAngleIndication(m_en_aa_indication);
  VERIFY(mp_gridModeEditorIgnDlg->Create(CGridModeEditorIgnDlg::IDD, NULL));
  mp_gridModeEditorIgnDlg->SetLoadAxisCfg(m_ldaxMinVal, (m_ldaxCfg == 1) ? std::numeric_limits<float>::max() : m_ldaxMaxVal);
  mp_gridModeEditorIgnDlg->ShowWindow(SW_SHOW);
  m_grid_map_state_ign = 1;
 }
 else
 {
  OnGridMapClosedIgn(mp_gridModeEditorIgnDlg->m_hWnd, TYPE_MAP_GME_IGN_WND);
  mp_gridModeEditorIgnDlg->DestroyWindow();
  mp_gridModeEditorIgnDlg.reset(NULL);
 }
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnGridModeEditingInj()
{
 if (m_grid_mode_editing_inj_check.GetCheck()==BST_CHECKED)
 {
  mp_gridModeEditorInjDlg.reset(new CGridModeEditorInjDlg());
  if (mp_autoTuneCntr.get())
  {
   mp_autoTuneCntr->SetView(mp_gridModeEditorInjDlg.get());
   mp_autoTuneCntr->BindRPMGrid(GetRPMGrid());
   mp_autoTuneCntr->BindMaps(GetVEMap(false), GetAFRMap(false));
   mp_autoTuneCntr->SetLoadAxisCfg(m_ldaxMinVal, (m_ldaxCfg == 1) ? std::numeric_limits<float>::max() : m_ldaxMaxVal);
   mp_autoTuneCntr->setOnMapChanged(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapChangedInj));
   mp_autoTuneCntr->Init();
  }
  mp_gridModeEditorInjDlg->BindMaps(GetVEMap(false), GetAFRMap(false), GetITMap(false), GetIdlcMap(false), GetIdlrMap(false), GetITRPMMap(false), GetRigidMap(false),
                                    GetIACCMap(false), GetIACCWMap(false), GetAftstrMap(false), GetWrmpMap(false), GetAETPSMap(false), GetAERPMMap(false),
                                    GetCrnkMap(false), GetDeadMap(false), GetEGOCurveMap(false), GetIATCLTMap(false), GetTpsswtMap(false), GetAtscMap(false), GetGtscMap(false), GetGpscMap(false));
  mp_gridModeEditorInjDlg->BindRPMGrid(GetRPMGrid());
  mp_gridModeEditorInjDlg->BindCLTGrid(GetCLTGrid());
  mp_gridModeEditorInjDlg->setIsAllowed(fastdelegate::MakeDelegate(this, &CButtonsPanel::IsAllowed));
  mp_gridModeEditorInjDlg->setOnMapChanged(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapChangedInj));
  mp_gridModeEditorInjDlg->setOnCloseMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnGridMapClosedInj));
  mp_gridModeEditorInjDlg->setOnOpenMapWnd(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnOpenMapWnd));
  mp_gridModeEditorInjDlg->setOnChangeSettings(fastdelegate::MakeDelegate(this, &CButtonsPanel::OnChangeSettingsGME));
  VERIFY(mp_gridModeEditorInjDlg->Create(CGridModeEditorInjDlg::IDD, NULL));
  mp_gridModeEditorInjDlg->SetLoadAxisCfg(m_ldaxMinVal, (m_ldaxCfg == 1) ? std::numeric_limits<float>::max() : m_ldaxMaxVal);
  mp_gridModeEditorInjDlg->SetITMode(m_it_mode);
  mp_gridModeEditorInjDlg->ShowWindow(SW_SHOW);

  if (mp_autoTuneCntr.get())
   mp_autoTuneCntr->Activate();
  m_grid_map_state_inj = 1;
 }
 else
 {
  OnGridMapClosedInj(mp_gridModeEditorInjDlg->m_hWnd, TYPE_MAP_GME_INJ_WND);
  mp_gridModeEditorInjDlg->DestroyWindow();
  mp_gridModeEditorInjDlg.reset(NULL);
 }
}

void CButtonsPanel::OnUpdateViewStartMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_DA_START].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdleMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_DA_IDLE].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewWorkMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_DA_WORK].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewTempMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_DA_TEMP_CORR].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateGridModeEditingIgn(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 pCmdUI->Enable(allowed);
 pCmdUI->SetCheck((mp_gridModeEditorIgnDlg.get() && m_grid_map_state_ign) ? TRUE : FALSE);
}

void CButtonsPanel::OnUpdateGridModeEditingInj(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 pCmdUI->Enable(allowed);
 pCmdUI->SetCheck((mp_gridModeEditorInjDlg.get() && m_grid_map_state_inj) ? TRUE : FALSE);
}

void CButtonsPanel::OnUpdateViewVEMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_VE].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAFRMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_AFR].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewCrnkMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_CRNK].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewWrmpMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_WRMP].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewDeadMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_DEAD].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdlrMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_choke_op_enabled));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_IDLR].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIdlcMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_choke_op_enabled));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_IDLC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAETPSMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_AETPS].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAERPMMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_AERPM].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAftstrMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_AFTSTR].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewITMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart3DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_IT].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewITRPMMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_choke_op_enabled));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_ITRPM].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewRigidMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_RIGID].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewEGOCrvMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose || m_carb_afr));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_EGOCRV].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIACCMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_IACC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIACCWMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && m_fuel_injection);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_IACCW].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewIATCLTMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_IATCLT].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewTpsswtMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable /*&& m_fuel_injection*/); //required by ignition and fuel injection
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_TPSSWT].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewGtscMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose) && m_en_gas_corr);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_GTSC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewGpscMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose) && m_en_gas_corr);
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_GPSC].state) ? TRUE : FALSE );
}

void CButtonsPanel::OnUpdateViewAtscMap(CCmdUI* pCmdUI)
{
 bool allowed = IsAllowed();
 BOOL enable = (DLL::Chart2DCreate!=NULL) && allowed;
 pCmdUI->Enable(enable && (m_fuel_injection || m_gasdose || m_choke_op_enabled));
 pCmdUI->SetCheck( (m_md[TYPE_MAP_INJ_ATSC].state) ? TRUE : FALSE );
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
 if (m_md[TYPE_MAP_DA_START].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_START].handle, GetStartMap(true), GetStartMap(false));
 if (m_md[TYPE_MAP_DA_IDLE].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_IDLE].handle, GetIdleMap(true), GetIdleMap(false));
 if (m_md[TYPE_MAP_DA_WORK].state)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_DA_WORK].handle, GetWorkMap(true), GetWorkMap(false));
 if (m_md[TYPE_MAP_DA_TEMP_CORR].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_DA_TEMP_CORR].handle, GetTempMap(true), GetTempMap(false));
 if (m_md[TYPE_MAP_INJ_VE].state)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_VE].handle, GetVEMap(true), GetVEMap(false));
 if (m_md[TYPE_MAP_INJ_AFR].state)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_AFR].handle, GetAFRMap(true), GetAFRMap(false));
 if (m_md[TYPE_MAP_INJ_CRNK].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_CRNK].handle, GetCrnkMap(true), GetCrnkMap(false));
 if (m_md[TYPE_MAP_INJ_WRMP].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_WRMP].handle, GetWrmpMap(true), GetWrmpMap(false));
 if (m_md[TYPE_MAP_INJ_DEAD].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_DEAD].handle, GetDeadMap(true), GetDeadMap(false));
 if (m_md[TYPE_MAP_INJ_IDLR].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IDLR].handle, GetIdlrMap(true), GetIdlrMap(false));
 if (m_md[TYPE_MAP_INJ_IDLC].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IDLC].handle, GetIdlcMap(true), GetIdlcMap(false));
 if (m_md[TYPE_MAP_INJ_AETPS].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AETPS].handle, GetAETPSMap(true), GetAETPSMap(false));
 if (m_md[TYPE_MAP_INJ_AERPM].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AERPM].handle, GetAERPMMap(true), GetAERPMMap(false));
 if (m_md[TYPE_MAP_INJ_AFTSTR].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_AFTSTR].handle, GetAftstrMap(true), GetAftstrMap(false));
 if (m_md[TYPE_MAP_INJ_IT].state)
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_IT].handle, GetITMap(true), GetITMap(false));
 if (m_md[TYPE_MAP_INJ_ITRPM].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_ITRPM].handle, GetITRPMMap(true), GetITRPMMap(false));
 if (m_md[TYPE_MAP_INJ_RIGID].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_RIGID].handle, GetRigidMap(true), GetRigidMap(false));
 if (m_md[TYPE_MAP_INJ_EGOCRV].state)
 {
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_EGOCRV].handle, GetEGOCurveMap(true), GetEGOCurveMap(false));
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_EGOCRV].handle, 1, GetEGOCurveMap(false)[16], GetEGOCurveMap(false)[16+1]);
 }
 if (m_md[TYPE_MAP_INJ_IACC].state)
 {
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IACC].handle, GetIACCMap(true), GetIACCMap(false));
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_IACC].handle, 1, GetIACCMap(false)[8], GetIACCMap(false)[8+1]);
 }
 if (m_md[TYPE_MAP_INJ_IACCW].state)
 {
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IACCW].handle, GetIACCWMap(true), GetIACCWMap(false));
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_IACCW].handle, 1, GetIACCWMap(false)[16], GetIACCWMap(false)[16+1]);
 }
 if (m_md[TYPE_MAP_INJ_IATCLT].state)
 {
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_IATCLT].handle, GetIATCLTMap(true), GetIATCLTMap(false));
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_IATCLT].handle, 1, GetIATCLTMap(false)[8], GetIATCLTMap(false)[8+1]);
 }
 if (m_md[TYPE_MAP_INJ_TPSSWT].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_TPSSWT].handle, GetTpsswtMap(true), GetTpsswtMap(false));
 if (m_md[TYPE_MAP_INJ_GTSC].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_GTSC].handle, GetGtscMap(true), GetGtscMap(false));
 if (m_md[TYPE_MAP_INJ_GPSC].state)
 {
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_GPSC].handle, GetGpscMap(true), GetGpscMap(false));
  DLL::Chart2DUpdateAxisEdits(m_md[TYPE_MAP_INJ_GPSC].handle, 1, GetGpscMap(false)[9], GetGpscMap(false)[9+1]);
 }
 if (m_md[TYPE_MAP_INJ_ATSC].state)
  DLL::Chart2DUpdate(m_md[TYPE_MAP_INJ_ATSC].handle, GetAtscMap(true), GetAtscMap(false));
 if (mp_gridModeEditorIgnDlg.get() && m_grid_map_state_ign)
  mp_gridModeEditorIgnDlg->UpdateView(true);
 if (mp_gridModeEditorInjDlg.get() && m_grid_map_state_inj)
  mp_gridModeEditorInjDlg->UpdateView(true);
}

void CButtonsPanel::EnableAdvanceAngleIndication(bool i_enable)
{
 m_en_aa_indication = i_enable;
 if (mp_gridModeEditorIgnDlg.get())
  mp_gridModeEditorIgnDlg->EnableAdvanceAngleIndication(m_en_aa_indication);
}

void CButtonsPanel::SetDynamicValues(const TablDesk::DynVal& dv)
{
 if (mp_gridModeEditorIgnDlg.get())
  mp_gridModeEditorIgnDlg->SetDynamicValues(dv);
 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->SetDynamicValues(dv);
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetDynamicValues(dv);
}

void CButtonsPanel::EnableFuelInjection(bool i_enable)
{
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_INJ_VE].state && ::IsWindow(m_md[TYPE_MAP_INJ_VE].handle))
  DLL::Chart3DEnable(m_md[TYPE_MAP_INJ_VE].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_AFR].state && ::IsWindow(m_md[TYPE_MAP_INJ_AFR].handle))
  DLL::Chart3DEnable(m_md[TYPE_MAP_INJ_AFR].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_CRNK].state && ::IsWindow(m_md[TYPE_MAP_INJ_CRNK].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_CRNK].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_WRMP].state && ::IsWindow(m_md[TYPE_MAP_INJ_WRMP].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_WRMP].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_DEAD].state && ::IsWindow(m_md[TYPE_MAP_INJ_DEAD].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_DEAD].handle, i_enable && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IDLR].state && ::IsWindow(m_md[TYPE_MAP_INJ_IDLR].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IDLR].handle, (i_enable || m_choke_op_enabled) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IDLC].state && ::IsWindow(m_md[TYPE_MAP_INJ_IDLC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IDLC].handle, (i_enable || m_choke_op_enabled) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_AETPS].state && ::IsWindow(m_md[TYPE_MAP_INJ_AETPS].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AETPS].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_AERPM].state && ::IsWindow(m_md[TYPE_MAP_INJ_AERPM].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AERPM].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_AFTSTR].state && ::IsWindow(m_md[TYPE_MAP_INJ_AFTSTR].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AFTSTR].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IT].state && ::IsWindow(m_md[TYPE_MAP_INJ_IT].handle))
  DLL::Chart3DEnable(m_md[TYPE_MAP_INJ_IT].handle, i_enable && IsAllowed());
 if (m_md[TYPE_MAP_INJ_ITRPM].state && ::IsWindow(m_md[TYPE_MAP_INJ_ITRPM].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_ITRPM].handle, (i_enable || m_choke_op_enabled) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_RIGID].state && ::IsWindow(m_md[TYPE_MAP_INJ_RIGID].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_RIGID].handle, i_enable && IsAllowed());
 if (m_md[TYPE_MAP_INJ_EGOCRV].state && ::IsWindow(m_md[TYPE_MAP_INJ_EGOCRV].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_EGOCRV].handle, (i_enable || m_gasdose || m_carb_afr) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IACC].state && ::IsWindow(m_md[TYPE_MAP_INJ_IACC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IACC].handle, i_enable && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IACCW].state && ::IsWindow(m_md[TYPE_MAP_INJ_IACCW].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IACCW].handle, i_enable && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IATCLT].state && ::IsWindow(m_md[TYPE_MAP_INJ_IATCLT].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IATCLT].handle, (i_enable || m_gasdose) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_TPSSWT].state && ::IsWindow(m_md[TYPE_MAP_INJ_TPSSWT].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_TPSSWT].handle, i_enable && IsAllowed());
 if (m_md[TYPE_MAP_INJ_GTSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_GTSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_GTSC].handle, ((i_enable || m_gasdose) && m_en_gas_corr) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_GPSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_GPSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_GPSC].handle, ((i_enable || m_gasdose) && m_en_gas_corr) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_ATSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_ATSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_ATSC].handle, (i_enable || m_gasdose || m_choke_op_enabled) && IsAllowed());
}

void CButtonsPanel::EnableGasdose(bool i_enable)
{
 m_gasdose = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_INJ_AETPS].state && ::IsWindow(m_md[TYPE_MAP_INJ_AETPS].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AETPS].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_AERPM].state && ::IsWindow(m_md[TYPE_MAP_INJ_AERPM].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AERPM].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_VE].state && ::IsWindow(m_md[TYPE_MAP_INJ_VE].handle))
  DLL::Chart3DEnable(m_md[TYPE_MAP_INJ_VE].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_AFR].state && ::IsWindow(m_md[TYPE_MAP_INJ_AFR].handle))
  DLL::Chart3DEnable(m_md[TYPE_MAP_INJ_AFR].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_WRMP].state && ::IsWindow(m_md[TYPE_MAP_INJ_WRMP].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_WRMP].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_CRNK].state && ::IsWindow(m_md[TYPE_MAP_INJ_CRNK].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_CRNK].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_AFTSTR].state && ::IsWindow(m_md[TYPE_MAP_INJ_AFTSTR].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AFTSTR].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_ATSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_ATSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_ATSC].handle, (i_enable || m_fuel_injection || m_choke_op_enabled) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IATCLT].state && ::IsWindow(m_md[TYPE_MAP_INJ_IATCLT].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IATCLT].handle, (i_enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_EGOCRV].state && ::IsWindow(m_md[TYPE_MAP_INJ_EGOCRV].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_EGOCRV].handle, (i_enable || m_fuel_injection || m_carb_afr) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_GTSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_GTSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_GTSC].handle, (m_en_gas_corr && (i_enable || m_fuel_injection)) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_GPSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_GPSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_GPSC].handle, (m_en_gas_corr && (i_enable || m_fuel_injection)) && IsAllowed());
}

void CButtonsPanel::EnableCarbAfr(bool i_enable)
{
 m_carb_afr = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);

 if (m_md[TYPE_MAP_INJ_EGOCRV].state && ::IsWindow(m_md[TYPE_MAP_INJ_EGOCRV].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_EGOCRV].handle, (i_enable || m_fuel_injection || m_gasdose) && IsAllowed());
}

void CButtonsPanel::EnableGasCorr(bool i_enable)
{
 m_en_gas_corr = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);

 if (m_md[TYPE_MAP_INJ_GTSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_GTSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_GTSC].handle, (i_enable && (m_gasdose || m_fuel_injection)) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_GPSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_GPSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_GPSC].handle, (i_enable && (m_gasdose || m_fuel_injection)) && IsAllowed());
}

void CButtonsPanel::EnableChokeOp(bool enable)
{
 m_choke_op_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
 if (m_md[TYPE_MAP_INJ_ATSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_ATSC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_ATSC].handle, (enable || m_gasdose || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IDLR].state && ::IsWindow(m_md[TYPE_MAP_INJ_IDLR].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IDLR].handle, (enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_IDLC].state && ::IsWindow(m_md[TYPE_MAP_INJ_IDLC].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IDLC].handle, (enable || m_fuel_injection) && IsAllowed());
 if (m_md[TYPE_MAP_INJ_ITRPM].state && ::IsWindow(m_md[TYPE_MAP_INJ_ITRPM].handle))
  DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_ITRPM].handle, (enable || m_fuel_injection) && IsAllowed());
}


float* CButtonsPanel::GetStartMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_DA_START].original;
 else
  return m_md[TYPE_MAP_DA_START].active;
}

float* CButtonsPanel::GetIdleMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_DA_IDLE].original;
 else
  return m_md[TYPE_MAP_DA_IDLE].active;
}

float* CButtonsPanel::GetWorkMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_DA_WORK].original;
 else
  return m_md[TYPE_MAP_DA_WORK].active;
}

float* CButtonsPanel::GetTempMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_DA_TEMP_CORR].original;
 else
  return m_md[TYPE_MAP_DA_TEMP_CORR].active;
}

float* CButtonsPanel::GetVEMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_VE].original;
 else
  return m_md[TYPE_MAP_INJ_VE].active;
}

float* CButtonsPanel::GetAFRMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_AFR].original;
 else
  return m_md[TYPE_MAP_INJ_AFR].active;
}

float* CButtonsPanel::GetCrnkMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_CRNK].original;
 else
  return m_md[TYPE_MAP_INJ_CRNK].active;
}

float* CButtonsPanel::GetWrmpMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_WRMP].original;
 else
  return m_md[TYPE_MAP_INJ_WRMP].active;
}

float* CButtonsPanel::GetDeadMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_DEAD].original;
 else
  return m_md[TYPE_MAP_INJ_DEAD].active;
}

float* CButtonsPanel::GetIdlrMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_IDLR].original;
 else
  return m_md[TYPE_MAP_INJ_IDLR].active;
}

float* CButtonsPanel::GetIdlcMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_IDLC].original;
 else
  return m_md[TYPE_MAP_INJ_IDLC].active;
}

float* CButtonsPanel::GetAETPSMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_AETPS].original;
 else
  return m_md[TYPE_MAP_INJ_AETPS].active;
}

float* CButtonsPanel::GetAERPMMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_AERPM].original;
 else
  return m_md[TYPE_MAP_INJ_AERPM].active;
}

float* CButtonsPanel::GetAftstrMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_AFTSTR].original;
 else
  return m_md[TYPE_MAP_INJ_AFTSTR].active;
}

float* CButtonsPanel::GetITMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_IT].original;
 else
  return m_md[TYPE_MAP_INJ_IT].active;
}

float* CButtonsPanel::GetITRPMMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_ITRPM].original;
 else
  return m_md[TYPE_MAP_INJ_ITRPM].active;
}

float* CButtonsPanel::GetRigidMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_RIGID].original;
 else
  return m_md[TYPE_MAP_INJ_RIGID].active;
}

float* CButtonsPanel::GetEGOCurveMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_EGOCRV].original;
 else
  return m_md[TYPE_MAP_INJ_EGOCRV].active;
}

float* CButtonsPanel::GetIACCMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_IACC].original;
 else
  return m_md[TYPE_MAP_INJ_IACC].active;
}

float* CButtonsPanel::GetIACCWMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_IACCW].original;
 else
  return m_md[TYPE_MAP_INJ_IACCW].active;
}

float* CButtonsPanel::GetIATCLTMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_IATCLT].original;
 else
  return m_md[TYPE_MAP_INJ_IATCLT].active;
}

float* CButtonsPanel::GetTpsswtMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_TPSSWT].original;
 else
  return m_md[TYPE_MAP_INJ_TPSSWT].active;
}

float* CButtonsPanel::GetGtscMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_GTSC].original;
 else
  return m_md[TYPE_MAP_INJ_GTSC].active;
}

float* CButtonsPanel::GetAtscMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_ATSC].original;
 else
  return m_md[TYPE_MAP_INJ_ATSC].active;
}

float* CButtonsPanel::GetGpscMap(bool i_original)
{
 if (i_original)
  return m_md[TYPE_MAP_INJ_GPSC].original;
 else
  return m_md[TYPE_MAP_INJ_GPSC].active;
}

float* CButtonsPanel::GetRPMGrid(void)
{
 return m_rpm_grid_values;
}

float* CButtonsPanel::GetCLTGrid(void)
{
 return m_clt_grid_values;
}

HWND CButtonsPanel::GetMapWindow(int wndType)
{
if (wndType >= TYPE_MAP_SET_START && wndType <= TYPE_MAP_SET_END)
  return m_md[wndType].state ? m_md[wndType].handle : NULL;
else if (wndType == TYPE_MAP_GME_IGN_WND) //pseudo map
  return (mp_gridModeEditorIgnDlg.get() && m_grid_map_state_ign) ? mp_gridModeEditorIgnDlg->m_hWnd : NULL; 
else if (wndType == TYPE_MAP_GME_INJ_WND) //pseudo map
  return (mp_gridModeEditorInjDlg.get() && m_grid_map_state_inj) ? mp_gridModeEditorInjDlg->m_hWnd : NULL; 
 else
  return NULL;
}

void CButtonsPanel::_EnableCharts(bool enable)
{
 if (m_charts_enabled != (int)enable)
 {//ignition
  if (m_md[TYPE_MAP_DA_START].state && ::IsWindow(m_md[TYPE_MAP_DA_START].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_DA_START].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_DA_IDLE].state && ::IsWindow(m_md[TYPE_MAP_DA_IDLE].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_DA_IDLE].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_DA_WORK].state && ::IsWindow(m_md[TYPE_MAP_DA_WORK].handle))
   DLL::Chart3DEnable(m_md[TYPE_MAP_DA_WORK].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_DA_TEMP_CORR].state && ::IsWindow(m_md[TYPE_MAP_DA_TEMP_CORR].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_DA_TEMP_CORR].handle, enable && IsAllowed());

  //fuel injection
  if (m_md[TYPE_MAP_INJ_VE].state && ::IsWindow(m_md[TYPE_MAP_INJ_VE].handle))
   DLL::Chart3DEnable(m_md[TYPE_MAP_INJ_VE].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_AFR].state && ::IsWindow(m_md[TYPE_MAP_INJ_AFR].handle))
   DLL::Chart3DEnable(m_md[TYPE_MAP_INJ_AFR].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_CRNK].state && ::IsWindow(m_md[TYPE_MAP_INJ_CRNK].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_CRNK].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_WRMP].state && ::IsWindow(m_md[TYPE_MAP_INJ_WRMP].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_WRMP].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_DEAD].state && ::IsWindow(m_md[TYPE_MAP_INJ_DEAD].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_DEAD].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_IDLR].state && ::IsWindow(m_md[TYPE_MAP_INJ_IDLR].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IDLR].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_IDLC].state && ::IsWindow(m_md[TYPE_MAP_INJ_IDLC].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IDLC].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_AETPS].state && ::IsWindow(m_md[TYPE_MAP_INJ_AETPS].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AETPS].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_AERPM].state && ::IsWindow(m_md[TYPE_MAP_INJ_AERPM].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AERPM].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_AFTSTR].state && ::IsWindow(m_md[TYPE_MAP_INJ_AFTSTR].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_AFTSTR].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_IT].state && ::IsWindow(m_md[TYPE_MAP_INJ_IT].handle))
   DLL::Chart3DEnable(m_md[TYPE_MAP_INJ_IT].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_ITRPM].state && ::IsWindow(m_md[TYPE_MAP_INJ_ITRPM].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_ITRPM].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_RIGID].state && ::IsWindow(m_md[TYPE_MAP_INJ_RIGID].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_RIGID].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_EGOCRV].state && ::IsWindow(m_md[TYPE_MAP_INJ_EGOCRV].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_EGOCRV].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_IACC].state && ::IsWindow(m_md[TYPE_MAP_INJ_IACC].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IACC].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_IACCW].state && ::IsWindow(m_md[TYPE_MAP_INJ_IACCW].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IACCW].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_IATCLT].state && ::IsWindow(m_md[TYPE_MAP_INJ_IATCLT].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_IATCLT].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_TPSSWT].state && ::IsWindow(m_md[TYPE_MAP_INJ_TPSSWT].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_TPSSWT].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_GTSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_GTSC].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_GTSC].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_GPSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_GPSC].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_GPSC].handle, enable && IsAllowed());
  if (m_md[TYPE_MAP_INJ_ATSC].state && ::IsWindow(m_md[TYPE_MAP_INJ_ATSC].handle))
   DLL::Chart2DEnable(m_md[TYPE_MAP_INJ_ATSC].handle, enable && IsAllowed());
  if (mp_gridModeEditorIgnDlg.get() && m_grid_map_state_ign && ::IsWindow(mp_gridModeEditorIgnDlg->m_hWnd))
   mp_gridModeEditorIgnDlg->UpdateDialogControls(mp_gridModeEditorIgnDlg.get(), TRUE);
  if (mp_gridModeEditorInjDlg.get() && m_grid_map_state_inj && ::IsWindow(mp_gridModeEditorInjDlg->m_hWnd))
   mp_gridModeEditorInjDlg->UpdateDialogControls();
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

void CButtonsPanel::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(m_scrl_view));
}

void CButtonsPanel::SetLoadAxisCfg(float minVal, float maxVal, int loadSrc)
{
 m_ldaxMinVal = minVal;
 m_ldaxMaxVal = maxVal;
 m_ldaxCfg = loadSrc;

 if (mp_gridModeEditorIgnDlg.get())
  mp_gridModeEditorIgnDlg->SetLoadAxisCfg(m_ldaxMinVal, (m_ldaxCfg == 1) ? std::numeric_limits<float>::max() : m_ldaxMaxVal);

 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->SetLoadAxisCfg(m_ldaxMinVal, (m_ldaxCfg == 1) ? std::numeric_limits<float>::max() : m_ldaxMaxVal);

 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetLoadAxisCfg(m_ldaxMinVal, (m_ldaxCfg == 1) ? std::numeric_limits<float>::max() : m_ldaxMaxVal);
}

float* CButtonsPanel::GetLamDelMap(int id)
{
 ASSERT(mp_autoTuneCntr.get());
 return mp_autoTuneCntr->GetLamDelMap(id);
}

void CButtonsPanel::SetAFRError(float afrError)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetAFRError(afrError);
}

bool* CButtonsPanel::GetBlockedCells(void)
{
 ASSERT(mp_autoTuneCntr.get());
 return mp_autoTuneCntr->GetBlockedCells();
}

void CButtonsPanel::SetStatSize(int statSize)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetStatSize(statSize);
}

void CButtonsPanel::SetAutoBlockThrd(int thrd)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetAutoBlockThrd(thrd);
}

void CButtonsPanel::SetGrowingMode(bool growing)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetGrowingMode(growing);
}

void CButtonsPanel::SetMinAFR(float afr)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMinAFR(afr);
}

void CButtonsPanel::SetMaxAFR(float afr)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMaxAFR(afr);
}

void CButtonsPanel::SetMinDistThrd(int thrd)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMinDistThrd(thrd);
}

void CButtonsPanel::SetMinTPS(float tps)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMinTPS(tps);
}

void CButtonsPanel::SetMaxTPS(float tps)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetMaxTPS(tps);
}

void CButtonsPanel::SetCLTThrd(float clt)
{
 if (mp_autoTuneCntr.get())
  mp_autoTuneCntr->SetCLTThrd(clt);
}

void CButtonsPanel::SetMapEditorSettings(int gradSat, int gradBrt, bool bold)
{
 CMapEditorCtrl::SetSettings(gradSat, gradBrt, bold);
}

void CButtonsPanel::MakeChartsChildren(bool children)
{
 m_children_charts = children;
 HWND hwnd;

 for(int i = TYPE_MAP_SET_START; i <= TYPE_MAP_SET_END; ++i)
 {
  hwnd = GetMapWindow(i);
  _MakeWindowChild(hwnd, children);
 }

 hwnd = GetMapWindow(TYPE_MAP_GME_IGN_WND); //pseudo map
 _MakeWindowChild(hwnd, children);

 hwnd = GetMapWindow(TYPE_MAP_GME_INJ_WND); //pseudo map
 _MakeWindowChild(hwnd, children);
}

void CButtonsPanel::_MakeWindowChild(HWND hwnd, bool child)
{
 if (hwnd && (NULL!=::GetParent(hwnd) != child))
 {
  //For compatibility reasons, SetParent does not modify the WS_CHILD or WS_POPUP window styles of the window whose parent is being changed.
  //Therefore, if hWndNewParent is NULL, you should also clear the WS_CHILD bit and set the WS_POPUP style after calling SetParent.
  //Conversely, if hWndNewParent is not NULL and the window was previously a child of the desktop, you should clear the WS_POPUP style and
  //set the WS_CHILD style before calling SetParent.
  if (child)
   CWnd::FromHandle(hwnd)->ModifyStyle(0, WS_CHILD, 0);

  ::SetParent(hwnd, _ChartParentHwnd());

  if (!child)
   CWnd::FromHandle(hwnd)->ModifyStyle(WS_CHILD, 0, 0);   
 }
}

void CButtonsPanel::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 if (m_toggleMapWnd)
 {
  if (m_openedChart)
   ::SendMessage(m_openedChart,WM_CLOSE,0,0);
  m_openedChart = i_hwnd;
 }

 _MakeWindowChild(i_hwnd, m_children_charts);
 ::SendMessage(i_hwnd, WM_NCACTIVATE, TRUE, -1);

 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(i_hwnd, i_mapType);
}

void CButtonsPanel::OnWndActivation(HWND i_hwnd, long cmd)
{
 if (m_OnWndActivation)
  m_OnWndActivation(i_hwnd, cmd);

 if (m_children_charts && cmd == SC_MINIMIZE && NULL != i_hwnd)
  CWnd::FromHandle(i_hwnd)->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CButtonsPanel::EnableToggleMapWnd(bool toggle)
{
 m_toggleMapWnd = toggle;
}

HWND CButtonsPanel::_ChartParentHwnd(void)
{
 return m_children_charts ? AfxGetMainWnd()->GetSafeHwnd() : NULL;
}

void CButtonsPanel::CloseCharts(void)
{
 HWND hwnd;
 for(int i = TYPE_MAP_SET_START; i <= TYPE_MAP_SET_END; ++i)
 {
  hwnd = GetMapWindow(i);
  if (hwnd)
   ::SendMessage(hwnd, WM_CLOSE, 0, 0);
 }
 hwnd = GetMapWindow(TYPE_MAP_GME_IGN_WND);
 if (hwnd) ::SendMessage(hwnd, WM_CLOSE, 0, 0);
 hwnd = GetMapWindow(TYPE_MAP_GME_INJ_WND);
 if (hwnd) ::SendMessage(hwnd, WM_CLOSE, 0, 0);
}

void CButtonsPanel::ShowOpenedCharts(bool i_show)
{
 HWND hwnd;
 for(int i = TYPE_MAP_SET_START; i <= TYPE_MAP_SET_END; ++i)
 {
  hwnd = GetMapWindow(i);
  if (hwnd)
   ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 }

 hwnd = GetMapWindow(TYPE_MAP_GME_IGN_WND);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);

 hwnd = GetMapWindow(TYPE_MAP_GME_INJ_WND);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
}

void CButtonsPanel::SetITEdMode(int mode)
{
 m_it_mode = mode;
 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->SetITMode(mode); 

 float y1, y2;
 _GetITModeRange(y1, y2);
 if ((m_md[TYPE_MAP_INJ_IT].state)&&(DLL::Chart3DCreate))
 {
  DLL::Chart3DSetFncRange(m_md[TYPE_MAP_INJ_IT].handle, y1, y2);
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_IT].handle, GetITMap(true), GetITMap(false));
  DLL::Chart3DSetAxisTitle(m_md[TYPE_MAP_INJ_IT].handle, 1, MLL::GetString((m_it_mode == 1 || m_it_mode == 3) ? IDS_MAPS_INJTIM_UNIT1 : IDS_MAPS_INJTIM_UNIT).c_str());
 }
}

int CButtonsPanel::GetITEdMode(void) const
{
 return m_it_mode;
}

void CButtonsPanel::setOnChangeSettings(EventHandler OnCB)
{
 m_OnChangeSettings = OnCB;
}

void CButtonsPanel::OnChangeSettingsGME(void)
{
 m_it_mode = mp_gridModeEditorInjDlg->GetITMode();

 float y1, y2;
 _GetITModeRange(y1, y2);
 if ((m_md[TYPE_MAP_INJ_IT].state)&&(DLL::Chart3DCreate))
 {
  DLL::Chart3DSetFncRange(m_md[TYPE_MAP_INJ_IT].handle, y1, y2);
  DLL::Chart3DUpdate(m_md[TYPE_MAP_INJ_IT].handle, GetITMap(true), GetITMap(false));
  DLL::Chart3DSetAxisTitle(m_md[TYPE_MAP_INJ_IT].handle, 1, MLL::GetString((m_it_mode == 1 || m_it_mode == 3) ? IDS_MAPS_INJTIM_UNIT1 : IDS_MAPS_INJTIM_UNIT).c_str());
 }

 if (m_OnChangeSettings)
  m_OnChangeSettings();
}

void __cdecl CButtonsPanel::OnChangeSettingsCME(void* i_param)
{
 CButtonsPanel* _this = static_cast<CButtonsPanel*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what the fuck?
  return;
 }

 for(int i = TYPE_MAP_ALL_START; i < TYPE_MAP_ALL_END - 2; ++i)
 {
  if (_this->m_md[i].state)
  {
   if (i == TYPE_MAP_DA_WORK || i == TYPE_MAP_INJ_VE || i == TYPE_MAP_INJ_AFR || i == TYPE_MAP_INJ_IT)  
    _this->m_md[i].ptMovStep = DLL::Chart3DGetPtMovingStep(_this->m_md[i].handle);
   else
    _this->m_md[i].ptMovStep = DLL::Chart2DGetPtMovingStep(_this->m_md[i].handle);
  }
 }

 if (_this->m_OnChangeSettings)
  _this->m_OnChangeSettings();
}

void CButtonsPanel::_GetITModeRange(float& y1, float& y2)
{
 y1 = (m_it_mode < 2) ? .0f : -360.0f;
 y2 = (m_it_mode < 2) ? 720.0f : 360.0f;
}

float CButtonsPanel::GetPtMovStep(int wndType)
{
 return m_md[wndType].ptMovStep;
}

void CButtonsPanel::SetPtMovStep(int wndType, float value)
{
 m_md[wndType].ptMovStep = value;
 if (m_md[wndType].state)
 {
  if (wndType == TYPE_MAP_DA_WORK || wndType == TYPE_MAP_INJ_VE || wndType == TYPE_MAP_INJ_AFR || wndType == TYPE_MAP_INJ_IT)
   DLL::Chart3DSetPtMovingStep(m_md[wndType].handle, value);
  else
   DLL::Chart2DSetPtMovingStep(m_md[wndType].handle, value); 
 }
}

void CButtonsPanel::TransformValues(void)
{
 float* p_map = m_md[TYPE_MAP_INJ_IT].active;
 for (int i = 0; i < 16*16; ++i)
  p_map[i] = OnValueTransformITMap(this, OnValueTransformITMap(this, p_map[i], 0), 1);
}
