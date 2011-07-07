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

#include <vector>

#include "common/unicodesupport.h"
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CCKPSPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CCKPSPageDlg(CWnd* pParent = NULL);
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::CKPSPar* o_values);
  void SetValues(const SECU3IO::CKPSPar* i_values);

  void EnableIgnitionCogs(bool enable);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSelchangePdCogsBTDCCombo();
  afx_msg void OnSelchangePdEngineCylCombo();
  afx_msg void OnChangePdIgnitionCogsEdit();
  afx_msg void OnClickedPdPosFrontRadio();
  afx_msg void OnClickedPdNegFrontRadio();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateIgnitionCogs(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

  void _FillCKPSTeethBTDCComboBox(void);
  int  _GetCKPSTeethBTDCComboBoxSelection(void);
  void _SetCKPSTeethBTDCComboBoxSelection(int i_sel);

  void _FillCKPSEngineCylComboBox(void);
  int  _GetCKPSEngineCylComboBoxSelection(void);
  void _SetCKPSEngineCylComboBoxSelection(int i_sel);

 private:
  //<cog number, UI name>
  std::vector<std::pair<int, _TSTRING> > m_cogs_numbers;
  //<cylinders, UI name>
  std::vector<std::pair<int, _TSTRING> > m_engine_cyls;

  SECU3IO::CKPSPar m_params;
  bool m_enabled;
  bool m_igncogs_enabled;

  CStatic m_ckps_front_groupbox;
  CButton m_ckps_posfront_radio;
  CButton m_ckps_negfront_radio;

  CStatic m_teeth_before_tdc_label;
  CComboBox m_teeth_before_tdc_combo;
  CComboBox m_engine_cyl_combo;

  CSpinButtonCtrlEx m_ignition_cogs_spin;
  CEditEx m_ignition_cogs_edit;
  CStatic m_ignition_cogs_label;
};
