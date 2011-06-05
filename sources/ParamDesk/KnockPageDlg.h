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

#pragma once

#include "io-core/SECU3IO.h"
#include "ParamPageEvents.h"
#include "propgrid/propertygrid.h"
#include "ui-core/TabDialog.h"

class AFX_EXT_CLASS CKnockPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CKnockPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::KnockPar* o_values);
  void SetValues(const SECU3IO::KnockPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg LRESULT OnItemChanged(WPARAM, LPARAM);
  afx_msg void OnDestroy();
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::KnockPar m_params;
  BOOL m_enabled;

  CPropertyGrid m_ctrlGrid;

  HITEM m_knock_use_knock_channel_item;
  HITEM m_knock_k_wnd_begin_angle_item;
  HITEM m_knock_k_wnd_end_angle_item;
  HITEM m_knock_bpf_frequency_item;
  HITEM m_knock_integrator_const_item; //us

  HITEM m_knock_retard_step_item;    //deg.
  HITEM m_knock_advance_step_item;   //deg.
  HITEM m_knock_max_retard_item;     //deg.
  HITEM m_knock_threshold_item;      //voltage
  HITEM m_knock_recovery_delay_item; //in work cycles
};
