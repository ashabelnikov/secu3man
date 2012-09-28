/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include "stdafx.h"
#include "resource.h"
#include "ButtonsPanel.h"
#include "DLLLinkedFunctions.h"
#include "io-core/secu3io.h"
#include "MapIds.h"

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

//const UINT CButtonsPanel::IDD = IDD_TD_BUTTONS_PANEL; //WTF?

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel dialog

CButtonsPanel::CButtonsPanel(UINT dialog_id, CWnd* pParent /*=NULL*/)
: Super(dialog_id, pParent)
, m_work_map_chart_state(0)
, m_temp_map_chart_state(0)
, m_start_map_chart_state(0)
, m_idle_map_chart_state(0)
, m_start_map_wnd_handle(NULL)
, m_idle_map_wnd_handle(NULL)
, m_work_map_wnd_handle(NULL)
, m_temp_map_wnd_handle(NULL)
, m_charts_enabled(-1)
, IDD(IDD_TD_BUTTONS_PANEL)
{
 memset(m_start_map_active, 0, 16 * sizeof(float));
 memset(m_start_map_original, 0, 16 * sizeof(float));
 memset(m_idle_map_active, 0, 16 * sizeof(float));
 memset(m_idle_map_original, 0, 16 * sizeof(float));
 memset(m_work_map_active, 0, 16 * 16 * sizeof(float));
 memset(m_work_map_original, 0, 16 * 16 * sizeof(float));
 memset(m_temp_map_active, 0, 16 * sizeof(float));
 memset(m_temp_map_original, 0, 16 * sizeof(float));
}

void CButtonsPanel::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_VIEW_WORK_MAP,  m_view_work_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_TEMP_MAP,  m_view_temp_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_START_MAP, m_view_start_map_btn);
 DDX_Control(pDX, IDC_TD_VIEW_IDLE_MAP,  m_view_idle_map_btn);
}

BEGIN_MESSAGE_MAP(CButtonsPanel, Super)
 ON_BN_CLICKED(IDC_TD_VIEW_START_MAP,OnViewStartMap)
 ON_BN_CLICKED(IDC_TD_VIEW_IDLE_MAP, OnViewIdleMap)
 ON_BN_CLICKED(IDC_TD_VIEW_WORK_MAP, OnViewWorkMap)
 ON_BN_CLICKED(IDC_TD_VIEW_TEMP_MAP, OnViewTempMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_START_MAP,OnUpdateViewStartMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_IDLE_MAP, OnUpdateViewIdleMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_WORK_MAP, OnUpdateViewWorkMap)
 ON_UPDATE_COMMAND_UI(IDC_TD_VIEW_TEMP_MAP, OnUpdateViewTempMap)
 ON_WM_TIMER()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel message handlers

BOOL CButtonsPanel::OnInitDialog()
{
 Super::OnInitDialog();

 SetTimer(TIMER_ID, 250, NULL);

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

//-----------------------------------------------------------------------------------------------
void CButtonsPanel::OnViewStartMap()
{
 //если кнопку "выключили" то закрываем окно редактора
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
    MLL::GetString(IDS_START_MAP).c_str());
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
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_idle_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_idle_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_idle_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_idle_map_chart_state = 1;
  m_idle_map_wnd_handle = DLL::Chart2DCreate(GetIdleMap(true),GetIdleMap(false),-15.0,55.0,SECU3IO::idle_map_rpm_slots,16,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_IDLE_MAP).c_str());
  DLL::Chart2DSetOnWndActivation(m_idle_map_wnd_handle,OnWndActivationIdleMap,this);
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
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_work_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_work_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_work_map_chart_state)&&(DLL::Chart3DCreate))
 {
  m_work_map_chart_state = 1;
  m_work_map_wnd_handle = DLL::Chart3DCreate(GetWorkMap(true),GetWorkMap(false),SECU3IO::work_map_rpm_slots,16,16,-15.0,55.0,
    MLL::GetString(IDS_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_WORK_MAP).c_str());
  DLL::Chart3DSetOnWndActivation(m_work_map_wnd_handle, OnWndActivationWorkMap, this);
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
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_temp_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_temp_map_wnd_handle,WM_CLOSE,0,0);
  return;
 }

 if ((!m_temp_map_chart_state)&&(DLL::Chart2DCreate))
 {
  m_temp_map_chart_state = 1;
  m_temp_map_wnd_handle = DLL::Chart2DCreate(GetTempMap(true),GetTempMap(false),-15.0,25.0,SECU3IO::temp_map_rpm_slots,16,
    MLL::GetString(IDS_MAPS_TEMPERATURE_UNIT).c_str(),
    MLL::GetString(IDS_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_TEMPCORR_MAP).c_str());
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
 default:
  return NULL;
 }
}

void CButtonsPanel::_EnableCharts(bool enable)
{
 if (m_charts_enabled != (int)enable)
 {
  if (m_start_map_chart_state && ::IsWindow(m_start_map_wnd_handle))
   DLL::Chart2DEnable(m_start_map_wnd_handle, enable && IsAllowed());

  if (m_idle_map_chart_state && ::IsWindow(m_idle_map_wnd_handle))
   DLL::Chart2DEnable(m_idle_map_wnd_handle, enable && IsAllowed());

  if (m_work_map_chart_state && ::IsWindow(m_work_map_wnd_handle))
   DLL::Chart3DEnable(m_work_map_wnd_handle, enable && IsAllowed());

  if (m_temp_map_chart_state && ::IsWindow(m_temp_map_wnd_handle))
   DLL::Chart2DEnable(m_temp_map_wnd_handle, enable && IsAllowed());
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
