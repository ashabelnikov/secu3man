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

/** \file EEPROMTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "EEPROMTabDlg.h"
#include "about/secu-3about.h"
#include "common/DPIAware.h"
#include "common/unicodesupport.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "tabldesk/ButtonsPanel.h"
#include "EEPROMContextMenuManager.h"

#define TIMER_ID 0

const UINT CEEPROMTabDlg::IDD = IDD_EEPROM_DATA;

CEEPROMTabDlg::CEEPROMTabDlg(CWnd* pParent /*=NULL*/)
: Super(CEEPROMTabDlg::IDD, pParent)
, mp_ParamDeskDlg(new CParamDeskDlg(NULL, true)) //<-- используем вкладку параметров детонации
, mp_ContextMenuManager(new CEEPROMContextMenuManager())
, mp_TablesPanel(new CButtonsPanel(0, NULL))
, m_is_bl_items_available(false)
{
 mp_ContextMenuManager->CreateContent();

 //========================================================
 if (!CheckBitmaps() || !CheckAppMenu() || !CheckAppLogo() || !CheckAbout())
  delete this;
 //========================================================
}

void CEEPROMTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_EE_EE_NAME,m_ee_name);
 DDX_Control(pDX, IDC_EE_MODIFICATION_FLAG, m_modification_flag);
 DDX_Control(pDX, IDC_EE_MAPSET_NAME, m_ee_mapset_name);
}

LPCTSTR CEEPROMTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BEGIN_MESSAGE_MAP(CEEPROMTabDlg, Super)
 ON_WM_CONTEXTMENU()
 ON_WM_INITMENUPOPUP()
 ON_WM_DESTROY()
 ON_WM_TIMER()
 ON_WM_DROPFILES()
 ON_BN_CLICKED(IDC_EE_POPUPMENU_BUTTON, OnPopupMenuButton)
 ON_BN_CLICKED(IDC_EE_CE_ERRORS_BTN, OnCEErrorsButton)
 ON_COMMAND(IDM_EE_OPEN_EEPROM, OnOpenEEPROMFromFile)
 ON_COMMAND(IDM_EE_SAVE_EEPROM, OnSaveEEPROMToFile)
 ON_COMMAND(IDM_EE_READ_EEPROM, OnReadEEPROMFromSECU)
 ON_COMMAND(IDM_EE_WRITE_EEPROM, OnWriteEEPROMToSECU)
 ON_EN_CHANGE(IDC_EE_MAPSET_NAME, OnChangeMapsetName)

 ON_UPDATE_COMMAND_UI(IDM_EE_READ_EEPROM, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_EE_WRITE_EEPROM, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_EE_OPEN_EEPROM, OnUpdatePopupMenu_file)
 ON_UPDATE_COMMAND_UI(IDM_EE_SAVE_EEPROM, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDC_EE_MAPSET_NAME, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_EE_CE_ERRORS_BTN, OnUpdateControls)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEEPROMTabDlg message handlers

BOOL CEEPROMTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //create parameters desk
 CRect rect;
 GetDlgItem(IDC_EE_PD_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_ParamDeskDlg->Create(CParamDeskDlg::IDD,this);
 mp_ParamDeskDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_ParamDeskDlg->SetTitle(MLL::LoadString(IDS_EE_PARAMETERS));
 mp_ParamDeskDlg->ShowSaveButton(false);
 mp_ParamDeskDlg->Resize(rect.Width(), rect.Height());
 mp_ParamDeskDlg->ShowWindow(SW_SHOW);

 //create tables desk
 GetDlgItem(IDC_EE_TD_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect); 
 mp_TablesPanel->Create(mp_TablesPanel->IDD, this);
 mp_TablesPanel->SetPosition(rect.TopLeft().x,rect.TopLeft().y, GetDlgItem(IDC_EE_MAPSET_NAME));
 mp_TablesPanel->ShowWindow(SW_SHOWNORMAL);

 mp_ContextMenuManager->Attach(this);

 SetTimer(TIMER_ID,250,NULL);

 UpdateDialogControls(this,TRUE);

 m_ee_mapset_name.SetLimitText(16);

 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()) || !CheckAppLogo())
 { GetParent()->DestroyWindow(); }
 //=================================================================

 //Enable drap & drop functionality
 DragAcceptFiles(true);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CEEPROMTabDlg::OnDestroy()
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);
}

void CEEPROMTabDlg::OnTimer(UINT nIDEvent)
{
 //dirty hack
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);

 //update state (if necessary)
 bool enable = IsEEPROMOpened(); 
 if (mp_ParamDeskDlg->IsEnabled()!=enable)
  mp_ParamDeskDlg->Enable(enable);
}

void CEEPROMTabDlg::OnUpdatePopupMenu_bl(CCmdUI* pCmdUI)
{
 //этот элемент должен быть разрешен только если бутлоадер доступен и EEPROM открытo
 if ((pCmdUI->m_nID==IDM_EE_WRITE_EEPROM))
 {
  BOOL enable = (m_is_bl_items_available && IsEEPROMOpened()) ? TRUE : FALSE;
  pCmdUI->Enable(enable);
  return;
 }

 BOOL enable = m_is_bl_items_available ? TRUE : FALSE;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck(FALSE);
}

void CEEPROMTabDlg::OnUpdatePopupMenu_file(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(TRUE);
 pCmdUI->SetCheck(FALSE);
}

void CEEPROMTabDlg::OnUpdatePopupMenu_file1(CCmdUI* pCmdUI)
{
 bool opened = IsEEPROMOpened();
 pCmdUI->Enable(opened ? TRUE : FALSE);
 pCmdUI->SetCheck(FALSE);
}

void CEEPROMTabDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool enabled = IsEEPROMOpened();
 pCmdUI->Enable(enabled ? TRUE : FALSE);
}

void CEEPROMTabDlg::OnPopupMenuButton()
{
 CRect rc;
 GetDlgItem(IDC_EE_POPUPMENU_BUTTON)->GetWindowRect(rc);
 mp_ContextMenuManager->TrackPopupMenu(rc.left, rc.top, false); //bottom align
}

void CEEPROMTabDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 mp_ContextMenuManager->OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}

void CEEPROMTabDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 mp_ContextMenuManager->TrackPopupMenu(point.x, point.y);
}

void CEEPROMTabDlg::OnDropFiles(HDROP hDropInfo)
{
 TCHAR fileName[MAX_PATH+1];
 if (DragQueryFile(hDropInfo, 0, fileName, MAX_PATH))
 {
  if (m_OnDropFile)
   m_OnDropFile(fileName);
 }
 else
  MessageBeep(MB_ICONEXCLAMATION);
}

void CEEPROMTabDlg::OnChangeMapsetName()
{
 if (m_OnMapsetNameChanged)
  m_OnMapsetNameChanged();
}

void CEEPROMTabDlg::SetEEFileName(_TSTRING i_name)
{
 m_ee_name.SetWindowText(i_name.c_str());
}

void CEEPROMTabDlg::SetModified(bool i_modified)
{
 if (i_modified)
  m_modification_flag.SetWindowText(_T("*"));
 else
  m_modification_flag.SetWindowText(_T(" "));
}

//разрешает/запрещает элементы меню св€занные с бутлоадером
void CEEPROMTabDlg::EnableBLItems(bool enable)
{
 m_is_bl_items_available = enable;

 if (!::IsWindow(this->m_hWnd))
  return;

 UpdateDialogControls(this,TRUE);

 //если меню отображаетс€ в текущий момент, то не смотр€ на то что элементы запрещены,
 //прорисовка останетс€ старой (недоделка Microsoft?).  ороче говор€ делаем это сами
 mp_ContextMenuManager->EnableBLMenuItems(enable);
}

bool CEEPROMTabDlg::IsBLItemsEnabled(void)
{
 return m_is_bl_items_available;
}

bool CEEPROMTabDlg::IsEEPROMOpened(void)
{
 if (m_IsEEPROMOpened)
  return m_IsEEPROMOpened();
 return false;
}

void CEEPROMTabDlg::OnOpenEEPROMFromFile()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()) || !CheckAppLogo())
  delete this;
 //=================================================================

 if (m_OnOpenEEPROMFromFile)
  m_OnOpenEEPROMFromFile();
}

void CEEPROMTabDlg::OnSaveEEPROMToFile()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
 { DestroyWindow(); return; }
 if (!CheckAppLogo())
  delete this;
 //=================================================================

 if (m_OnSaveEEPROMToFile)
  m_OnSaveEEPROMToFile();
}

void CEEPROMTabDlg::OnReadEEPROMFromSECU()
{
 if (m_OnReadEEPROMFromSECU)
  m_OnReadEEPROMFromSECU();

 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()) || !CheckAppLogo())
 { GetParent()->DestroyWindow(); }
 //=================================================================
}

void CEEPROMTabDlg::OnWriteEEPROMToSECU()
{
 if (m_OnWriteEEPROMToSECU)
  m_OnWriteEEPROMToSECU();
}

void CEEPROMTabDlg::SetMapsetName(CString i_text)
{
 m_ee_mapset_name.SetWindowText(i_text);
}

CString CEEPROMTabDlg::GetMapsetName(void)
{
 CString o_text;
 m_ee_mapset_name.GetWindowText(o_text);
 return o_text;
}

void CEEPROMTabDlg::OnCEErrorsButton()
{
 if (m_OnShowCEErrors)
  m_OnShowCEErrors();
}

void CEEPROMTabDlg::setIsEEPROMOpened(EventResult IsFunction)
{m_IsEEPROMOpened = IsFunction;}

void CEEPROMTabDlg::setOnOpenEEPROMFromFile(EventHandler OnFunction)
{m_OnOpenEEPROMFromFile = OnFunction;}

void CEEPROMTabDlg::setOnSaveEEPROMToFile(EventHandler OnFunction)
{m_OnSaveEEPROMToFile = OnFunction;}

void CEEPROMTabDlg::setOnReadEEPROMFromSECU(EventHandler OnFunction)
{m_OnReadEEPROMFromSECU = OnFunction;}

void CEEPROMTabDlg::setOnWriteEEPROMToSECU(EventHandler OnFunction)
{m_OnWriteEEPROMToSECU = OnFunction;}

void CEEPROMTabDlg::setOnDropFile(EventString OnFunction)
{m_OnDropFile = OnFunction;}

void CEEPROMTabDlg::setOnMapsetNameChanged(EventHandler OnFunction)
{m_OnMapsetNameChanged = OnFunction;}

void CEEPROMTabDlg::setOnShowCEErrors(EventHandler OnFunction)
{m_OnShowCEErrors = OnFunction;}
