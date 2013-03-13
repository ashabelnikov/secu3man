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

#include <map>
#include "common/fastdelegate.h"
#include "common/UnicodeSupport.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"

class CMAPCalcDlg : public CDialog
{
 public:
  typedef int UnitId;
  typedef std::map<UnitId, _TSTRING> UnitMap;
 private:
  typedef CDialog Super;
  typedef std::pair<float, float> Limit;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<UnitId> EventHandlerId;
  typedef fastdelegate::FastDelegate2<UnitId, UnitId> EventHandler2Id;
  typedef fastdelegate::FastDelegate0<bool> EventResult;

 public:
  CMAPCalcDlg(CWnd* pParent = NULL);   // standard constructor
 ~CMAPCalcDlg();
  static const UINT IDD;

  void Fill1stptPressUnit(const UnitMap& i_units);
  void Fill1stptVoltUnit(const UnitMap& i_units);
  void Fill2ndptPressUnit(const UnitMap& i_units);
  void Fill2ndptVoltUnit(const UnitMap& i_units);
  void FillGradientUnit(const UnitMap& i_n_units, const UnitMap& i_d_units);

  void SetPressLimits(float i_min, float i_max, int i_dp);
  void SetVoltLimits(float i_min, float i_max, int i_dp);
  void SetGradLimits(float i_min, float i_max, int i_dp);

  void Set1stptPress(float i_value, UnitId i_unit);
  void Set1stptVolt(float i_value, UnitId i_unit);
  void Set2ndptPress(float i_value, UnitId i_unit);
  void Set2ndptVolt(float i_value, UnitId i_unit);
  void SetGradient(float i_value, UnitId i_n_unit, UnitId i_d_unit);
  void SetGradientUnit(UnitId i_n_unit, UnitId i_d_unit);

  float Get1stptPress(void) const;
  float Get1stptVolt(void) const;
  float Get2ndptPress(void) const;
  float Get2ndptVolt(void) const;
  float GetGradient(void) const;

  void SetKnownGradientCheck(bool i_value);
  bool GetKnownGradientCheck(void) const;
  void EnableGradientCtrls(bool i_enable);
  void Enable2ndptCtrls(bool i_enable);

  void setOnKnowGradientCheck(const EventHandler& OnFunction);
  void setOnActivate(const EventHandler& OnFunction);
  void setOn1stptPressUnit(const EventHandlerId& OnFunction);
  void setOn1stptVoltUnit(const EventHandlerId& OnFunction);
  void setOn2ndptPressUnit(const EventHandlerId& OnFunction);
  void setOn2ndptVoltUnit(const EventHandlerId& OnFunction);
  void setOnGradientUnit(const EventHandler2Id& OnFunctionN, const EventHandler2Id& OnFunctionD);
  void setOnOK(const EventResult& OnFunction);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
  afx_msg void OnUpdateGradientCtrls(CCmdUI* pCmdUI);
  afx_msg void OnUpdate2ndptCtrls(CCmdUI* pCmdUI);
  afx_msg void OnKnownGradientCheck();
  afx_msg void On1stptPressCombo();
  afx_msg void On1stptVoltCombo();
  afx_msg void On2ndptPressCombo();
  afx_msg void On2ndptVoltCombo();
  afx_msg void OnGradientNCombo();
  afx_msg void OnGradientDCombo();
  DECLARE_MESSAGE_MAP()

 private:
  EventHandler m_OnKnowGradientCheck;
  EventHandler m_OnActivate;
  EventResult  m_OnOK;
  EventHandlerId m_On1stptPressUnit;
  EventHandlerId m_On1stptVoltUnit;
  EventHandlerId m_On2ndptPressUnit;
  EventHandlerId m_On2ndptVoltUnit;
  EventHandler2Id m_OnGradientUnitN;
  EventHandler2Id m_OnGradientUnitD;

  CEditEx m_1stpt_press_edit;
  CSpinButtonCtrlEx m_1stpt_press_spin;
  CComboBox m_1stpt_press_unit;
  CEditEx m_1stpt_volt_edit;
  CSpinButtonCtrlEx m_1stpt_volt_spin;
  CComboBox m_1stpt_volt_unit;

  CEditEx m_2ndpt_press_edit;
  CSpinButtonCtrlEx m_2ndpt_press_spin;
  CComboBox m_2ndpt_press_unit;
  CEditEx m_2ndpt_volt_edit;
  CSpinButtonCtrlEx m_2ndpt_volt_spin;
  CComboBox m_2ndpt_volt_unit;

  CEditEx m_gradient_edit;
  CSpinButtonCtrlEx m_gradient_spin;
  CComboBox m_gradient_n_unit;
  CComboBox m_gradient_d_unit;

  CButton m_known_grad_check;

  float m_1stpt_press;
  float m_1stpt_volt;
  float m_2ndpt_press;
  float m_2ndpt_volt;
  float m_gradient;

  bool m_enable_gradient_ctrls;
  bool m_enable_2ndpt_ctrls;

  Limit m_pressLimits;
  Limit m_voltLimits;
  Limit m_gradLimits;
};
