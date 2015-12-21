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

/** \file MiscPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/unicodesupport.h"
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"


class CToolTipCtrlEx;

class CMiscPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  typedef std::vector<int> BRCType;

  CMiscPageDlg(CWnd* pParent = NULL);
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);
  void EnableHallOutputParams(bool enable);

  void GetValues(SECU3IO::MiscelPar* o_values);
  void SetValues(const SECU3IO::MiscelPar* i_values);

  void FillUARTSpeedComboBox(const BRCType& i_baudrates);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsIgncutoff(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsHOP(CCmdUI* pCmdUI);
  afx_msg void OnChangeData();
  afx_msg void OnIgncutoffCheck();
  DECLARE_MESSAGE_MAP()

  int _GetBRFromComboBoxByIndex(int i_cb_index);
  int _GetIndexFromComboBoxByBR(int i_baudrate);

 private:
  CStatic m_uart_speed_label;
  CComboBox m_uart_speed_combo;

  CSpinButtonCtrlEx m_packet_period_spin;
  CEditEx m_packet_period_edit;
  CStatic m_packet_period_label;
  CStatic m_packet_period_unit;

  CButton m_igncutoff_check;
  CEditEx m_igncutoff_rpm_edit;
  CSpinButtonCtrlEx m_igncutoff_rpm_spin;
  CStatic m_igncutoff_rpm_unit;

  CEditEx m_hop_start_edit;
  CSpinButtonCtrlEx m_hop_start_spin;
  CStatic m_hop_start_unit;
  
  CEditEx m_hop_durat_edit;
  CSpinButtonCtrlEx m_hop_durat_spin;
  CStatic m_hop_durat_unit;

  SECU3IO::MiscelPar m_params;
  bool m_enabled;
  bool m_hall_output_enabled;
  BRCType m_baudrates;
  int m_uart_speed_cb_index;
  
  CButton m_flpmp_offongas_check;
    
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
