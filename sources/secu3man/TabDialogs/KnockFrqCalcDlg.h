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

#include <memory>
#include "common/FastDelegate.h"
#include "ui-core/DialogWithAccelerators.h"

class CEditEx;

class CKnockFrqCalcDlg : public CModelessDialog
{
  typedef CModelessDialog Super;
  typedef fastdelegate::FastDelegate1<float> EventHandler;

 public:
  CKnockFrqCalcDlg(CWnd* pParent = NULL);
  static const UINT IDD;

  void setOnCalculate(EventHandler OnFunction);

 protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  afx_msg void OnFrqCalcButton();
  DECLARE_MESSAGE_MAP()

private:
  EventHandler  m_OnCalculate;
  CBitmapButton m_calc_frq_btn;
  std::auto_ptr<CEditEx> mp_cyl_d_edit;
  float m_cyl_d;
};
