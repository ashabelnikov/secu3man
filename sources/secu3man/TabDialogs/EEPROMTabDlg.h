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

/** \file EEPROMTabDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include <memory>
#include "common/fastdelegate.h"
#include "common/unicodesupport.h"
#include "ui-core/TabDialog.h"

class CEEPROMContextMenuManager;
class CParamDeskDlg;
class CButtonsPanel;
class CToolTipCtrlEx;
class CLabel;

class CEEPROMTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate1<_TSTRING> EventString;

 public:
  CEEPROMTabDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  void SetEEFileName(_TSTRING i_name);
  void SetModified(bool i_modified);

  void SetMapsetName(CString i_text);
  CString GetMapsetName(void);

  void EnableBLItems(bool enable);
  bool IsBLItemsEnabled(void);

  void setIsEEPROMOpened(EventResult IsFunction);
  void setIsLoadGridsAvailable(EventResult IsFunction);
  void setOnOpenEEPROMFromFile(EventHandler OnFunction);
  void setOnSaveEEPROMToFile(EventHandler OnFunction);
  void setOnReadEEPROMFromSECU(EventHandler OnFunction);
  void setOnWriteEEPROMToSECU(EventHandler OnFunction); 
  void setOnDropFile(EventString OnFunction);   //Drag and Drop
  void setOnMapsetNameChanged(EventHandler OnFunction);
  void setOnShowCEErrors(EventHandler OnFunction);
  void setOnLoadGrids(EventHandler OnFunction);
  void setOnResetEeprom(EventHandler OnFunction);

  void EnableMakingChartsChildren(bool enable);
  void EnableToggleMapWnd(bool toggle);

  std::auto_ptr<CParamDeskDlg> mp_ParamDeskDlg;
  std::auto_ptr<CEEPROMContextMenuManager> mp_ContextMenuManager;
  std::auto_ptr<CButtonsPanel> mp_TablesPanel;

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnClose();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnPopupMenuButton();
  afx_msg void OnUpdatePopupMenu_bl(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_file(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_file1(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_grids(CCmdUI* pCmdUI);
  afx_msg void OnOpenEEPROMFromFile();
  afx_msg void OnSaveEEPROMToFile();
  afx_msg void OnReadEEPROMFromSECU();
  afx_msg void OnWriteEEPROMToSECU();
  afx_msg void OnChangeMapsetName();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnCEErrorsButton();
  afx_msg void OnLoadGrids();
  afx_msg void OnResetEeprom();
  DECLARE_MESSAGE_MAP()

  bool IsEEPROMOpened(void);

 private:
  void OnEeresetLinkClick(void);
  void OnEeContLinkClick(void);
  void OnEeUsingLinkClick(void);

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CLabel> mp_eeresetLink;
  std::auto_ptr<CLabel> mp_eecontLink;
  std::auto_ptr<CLabel> mp_eeusingLink;

  CEdit     m_ee_name;
  CEdit     m_ee_mapset_name;
  CStatic   m_modification_flag;
  CButton   m_ee_popup_menu_button;
  CButton   m_ee_ce_errors_btn;

  bool m_is_bl_items_available;
  bool m_initialized;

  EventResult  m_IsEEPROMOpened;
  EventHandler m_OnOpenEEPROMFromFile;
  EventHandler m_OnSaveEEPROMToFile;
  EventHandler m_OnReadEEPROMFromSECU;
  EventHandler m_OnWriteEEPROMToSECU;
  EventHandler m_OnMapsetNameChanged;
  EventString  m_OnDropFile;
  EventHandler m_OnShowCEErrors;
  EventHandler m_OnLoadGrids;
  EventResult  m_IsLoadGridsAvailable;
  EventHandler m_OnResetEeprom;
};
