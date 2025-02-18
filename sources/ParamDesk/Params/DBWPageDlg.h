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

/** \file DBWPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "common/objecttimer.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;
class CWndScroller;

class CDBWPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CDBWPageDlg();
 ~CDBWPageDlg();
  virtual LPCTSTR GetDialogID(void) const;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::DBWPar* o_values);
  void SetValues(const SECU3IO::DBWPar* i_values);
  void EnableHomePos(bool enable);

  void setOnRequestDataCollection(EventWithCode OnCB) {m_OnRequestDataCollection = OnCB;}

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangeData();
  afx_msg void OnUpdHomePos();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsHomepos(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
 void OnRequestDataCollectionTimer(void);

  SECU3IO::DBWPar m_params;
  bool m_enabled;
  bool m_homepos_enabled;

  CSpinButtonCtrlEx m_etc_p_spin;
  CEditEx m_etc_p_edit;
  CSpinButtonCtrlEx m_etc_i_spin;
  CEditEx m_etc_i_edit;
  CSpinButtonCtrlEx m_etc_d_spin;
  CEditEx m_etc_d_edit;
  CSpinButtonCtrlEx m_etc_nmax_duty_spin;
  CEditEx m_etc_nmax_duty_edit;
  CSpinButtonCtrlEx m_etc_pmax_duty_spin;
  CEditEx m_etc_pmax_duty_edit;
  CSpinButtonCtrlEx m_etc_pidperiod_spin;
  CEditEx m_etc_pidperiod_edit;
  CSpinButtonCtrlEx m_etc_frictorqop_spin;
  CEditEx m_etc_frictorqop_edit;
  CSpinButtonCtrlEx m_etc_frictorqcl_spin;
  CEditEx m_etc_frictorqcl_edit;
  CSpinButtonCtrlEx m_etc_frictorqth_spin;
  CEditEx m_etc_frictorqth_edit;
  CSpinButtonCtrlEx m_etc_maxiacadd_spin;
  CEditEx m_etc_maxiacadd_edit;
  CEditEx m_etc_homepos_edit;
  CButton m_updhomepos_button;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;

  EventWithCode m_OnRequestDataCollection;
  CObjectTimer<CDBWPageDlg> m_upd_tmr;  
};
