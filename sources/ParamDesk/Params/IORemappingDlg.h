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

/** \file IORemappingDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include <memory>
#include "IRemapDeskView.h"
#include "ui-core/DialogWithAccelerators.h"

class CToolTipCtrlEx;
class CWndScroller;

/////////////////////////////////////////////////////////////////////////////
// CIORemapping dialog

class AFX_EXT_CLASS CIORemappingDlg : public CModelessDialog, public IRemapDeskView
{
  typedef CModelessDialog Super;

 public:
  CIORemappingDlg(CWnd* pParent = NULL);   // standard constructor
 ~CIORemappingDlg();                       // destructor
  static const UINT IDD;

  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);

  //----------interface implementation---------------------------
  virtual bool IsEnabled(void);
  virtual void Enable(bool enable);
  virtual void Show(bool show);

  virtual bool AddItem(FWDM::IOSid iosId, FWDM::IOPid iopId, const _TSTRING& i_text);
  virtual bool SelectItem(FWDM::IOSid iosId, FWDM::IOPid iopId);
  virtual FWDM::IOPid GetSelection(FWDM::IOSid iosId) const;
  virtual void setOnItemSelected(EventItemSel OnFunction);
  virtual void EnableSECU3TItems(bool i_enable); //true - SECU-3T, false - SECU-3i
  virtual void EnableItem(FWDM::IOSid iosId, bool i_enable);
  virtual void EnableInversion(FWDM::IOSid iosId, bool i_invert);
  virtual void ResetContent(void);
  virtual bool InvertItem(FWDM::IOSid iosId, bool i_invert);
  virtual void setOnItemInverted(EventItemInv OnFunction);
  //-------------------------------------------------------------

  // Implementation
 protected:
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsChecks(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsCommon(CCmdUI* pCmdUI);
  afx_msg void OnChangeSelection(UINT nID);
  afx_msg void OnChangeInversion(UINT nID);
  DECLARE_MESSAGE_MAP()

 private:
  void _FillControls(void);
  void _SetTooltips(void);
  void _ShowControls(void);
  void _UpdateScrlViewSize(void);
  CComboBox* _GetCBbyIOSID(const std::map<UINT, int>& map, int iosId) const;

  bool m_enabled;
  bool m_enable_secu3t_features;

  EventItemSel m_OnItemSel;
  EventItemInv m_OnItemInv;
  std::map<UINT, int> m_iorcb;
  std::map<UINT, int> m_iorcm;
  std::map<UINT, std::pair<bool, bool> > m_enflg;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
};

/////////////////////////////////////////////////////////////////////////////
