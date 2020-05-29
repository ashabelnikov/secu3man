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

/** \file DwellCalcDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/UnicodeSupport.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include <memory>

class CToolTipCtrlEx;

class CDwellCalcDlg : public CDialog
{
  typedef CDialog Super;

 public:
  CDwellCalcDlg(CWnd* pParent = NULL);   // standard constructor
 ~CDwellCalcDlg();
  static const UINT IDD;

  void SetVoltageValues(const float* values, size_t size);
  void SetLimits(float t_lo_limit, float t_hi_limit);
  const float* GetTimeValues(void);

  virtual INT_PTR DoModal();

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()

 private:
  CEditEx m_r_edit;
  CSpinButtonCtrlEx m_r_spin;

  CEditEx m_l_edit;
  CSpinButtonCtrlEx m_l_spin;

  CEditEx m_i_edit;
  CSpinButtonCtrlEx m_i_spin;

  float m_r, m_l, m_i;

  float* mp_t_values;
  float* mp_v_values;
  size_t m_size;
  float m_t_lo_limit, m_t_hi_limit;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
