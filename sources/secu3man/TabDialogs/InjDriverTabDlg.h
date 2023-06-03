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

/** \file InjDriverTabDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <vector>
#include <map>
#include "common/fastdelegate.h"
#include "ui-core/TabDialog.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "io-core/SECU3IO.h"

class CChart2D;
class CToolTipCtrlEx;
class CClickableBmp;
class CLabel;
class CWndScroller;

class CInjDriverTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CInjDriverTabDlg();
  virtual LPCTSTR GetDialogID(void) const;

  void SetVoltLineColor(DWORD color);
  void SetPtMovStep(float peakOn, float peakDuty, float holdDuty, float peakFull, float pthPause, float pwMul, float pwAdd);
  void SetValues(SECU3IO::InjDrvPar* ip_data, bool voltage_only = false);
  void GetValues(SECU3IO::InjDrvPar* op_data);
  void EnableAll(bool enable);
  void EnableSaveBtn(bool enable);
  void EnableVoltage(bool enable);
  int GetCurrSetIdx(void);
  void SetOfflineCheck(void);
  void EnableBLItems(bool enable);
  void SetShowGraphLabels(bool show);
  int GetInjDrvSel(void);
  bool GetEditSimultanCheck(void);

  void setOnChange(EventHandler onCB);
  void setOnSaveButton(EventHandler onCB);
  void setOnExportToAFile(EventHandler onCB);
  void setOnImportFromAFile(EventHandler onCB);
  void setOnSaveToFirmware(EventHandler onCB);
  void setOnLoadFromFirmware(EventHandler onCB);
  void setOnShowFirmwareInfo(EventHandler onCB);
  void setOnExitOfflineMode(EventHandler onCB);
  void setOnWriteFirmwareFromFile(EventHandler onCB);  
  void setOnReadFirmwareToFile(EventHandler onCB);  
  void setOnReadLzblInfo(EventHandler onCB);
  void setOnWriteEEPROMFromFile(EventHandler onCB);  
  void setOnReadEEPROMToFile(EventHandler onCB);  
  void setOnResetEEPROM(EventHandler onCB);  
  void setOnSelInjDrv(EventHandler onCB);
  void setOnFirmwareMaster(EventHandler onCB);  
  void setOnImportFromStock(EventHandler onCB);

 protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnEepromSaveButton();
  afx_msg void OnPeakOnTabSelCheck();
  afx_msg void OnPeakDutyTabSelCheck();
  afx_msg void OnHoldDutyTabSelCheck();
  afx_msg void OnPeakFullTabSelCheck();
  afx_msg void OnPthPauseTabSelCheck();
  afx_msg void OnPWMulTabSelCheck();
  afx_msg void OnPWAddTabSelCheck();
  afx_msg void OnSelendokSetOfSett();
  afx_msg void OnSelendokTestCh();
  afx_msg void OnSelendokInjDrv();
  afx_msg void OnUpArrowButton();
  afx_msg void OnDownArrowButton();
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataChecks();
  afx_msg void OnChangeDataTstPeakPwm();
  afx_msg void OnChangeDataTstHoldPwm();
  afx_msg void OnExportToAFile();
  afx_msg void OnImportFromAFile();
  afx_msg void OnImportFromStock();
  afx_msg void OnSaveToFirmware();
  afx_msg void OnLoadFromFirmware();
  afx_msg void OnShowFirmwareInfo();
  afx_msg void OnOfflineModeExit(); 
  afx_msg void OnWriteFirmwareFromFile(void);  
  afx_msg void OnReadFirmwareToFile(void);  
  afx_msg void OnReadLzblInfo(void);
  afx_msg void OnWriteEEPROMFromFile(void);  
  afx_msg void OnReadEEPROMToFile(void);  
  afx_msg void OnResetEEPROM();
  afx_msg void OnFirmwareMaster();
  afx_msg void OnUpdateControlsPO(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsPD(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsHD(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsPF(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsPP(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsArrowBtns(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsEESave(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsVoltage(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOfflineModeExit(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBLItems(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

  void OnChangeChart(int serieIdx);
  void UpdateChartValues(void);
  void SetChartVoltageValue(float voltage);
  void OnLogoClick(void);
  void OnTipLinkClick(void);
  void OnBLRecoverLinkClick(void);

  CEditEx m_pwm_period_edit;
  CSpinButtonCtrlEx m_pwm_period_spin;

  CEditEx m_peak_duty_edit;
  CSpinButtonCtrlEx m_peak_duty_spin;

  CEditEx m_hold_duty_edit;
  CSpinButtonCtrlEx m_hold_duty_spin;

  CEditEx m_peak_on_time_edit;
  CSpinButtonCtrlEx m_peak_on_time_spin;

  CEditEx m_peak_pwm_time_edit;
  CSpinButtonCtrlEx m_peak_pwm_time_spin;

  CEditEx m_pth_pause_edit;
  CSpinButtonCtrlEx m_pth_pause_spin;

  CEditEx m_testch_frq_edit;
  CSpinButtonCtrlEx m_testch_frq_spin;

  CEditEx m_testch_duty_edit;
  CSpinButtonCtrlEx m_testch_duty_spin;

  CComboBox m_set_of_sett_combo;
  CComboBox m_testch_combo;
  CComboBox m_injdrv_combo;

  CStatic m_voltage_pane;
  CStatic m_gas_v_pane;
  CStatic m_crc0_pane;
  CStatic m_crc1_pane;

  CButton m_eeprom_save_btn;
  CButton m_peak_on_tabsel_check;
  CButton m_peak_duty_tabsel_check;
  CButton m_hold_duty_tabsel_check;
  CButton m_peak_full_tabsel_check;
  CButton m_pth_pause_tabsel_check;
  CButton m_pwmul_tabsel_check;
  CButton m_pwadd_tabsel_check;

  CButton m_peak_on_check;
  CButton m_peak_duty_check;
  CButton m_hold_duty_check;
  CButton m_peak_full_check;
  CButton m_pth_pause_check;

  CButton m_up_arrow;
  CButton m_down_arrow;

  CButton m_tst_peak_pwm_check;
  CButton m_tst_hold_pwm_check;

  CButton m_load_flash_check;

  CButton m_edit_simultan_check;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CChart2D> mp_chart;
  std::auto_ptr<CClickableBmp> mp_secu3logo;
  std::auto_ptr<CLabel> mp_secu3orgLink;
  std::auto_ptr<CLabel> mp_TipLink;
  std::auto_ptr<CWndScroller> mp_scr;
  std::auto_ptr<CLabel> mp_BLRecLink;

 private:
  EventHandler m_onChange;
  EventHandler m_onSaveButton;
  EventHandler m_onExportToAFile;
  EventHandler m_onImportFromAFile;
  EventHandler m_onSaveToFirmware;
  EventHandler m_onLoadFromFirmware;
  EventHandler m_onShowFirmwareInfo;
  EventHandler m_onExitOfflineMode;
  EventHandler m_onWriteFirmwareFromFile;
  EventHandler m_onReadFirmwareToFile;
  EventHandler m_onReadLzblInfo;
  EventHandler m_onWriteEEPROMFromFile;
  EventHandler m_onReadEEPROMToFile;
  EventHandler m_onResetEEPROM;
  EventHandler m_onSelInjDrv;
  EventHandler m_onFirmwareMaster;
  EventHandler m_onImportFromStock;

  bool m_initialized;
  bool m_enable;
  bool m_enable_ee_save;
  bool m_enable_voltage;
  bool m_offline;
  int m_set_of_sett_idx;
  bool m_is_bl_items_available;
  SECU3IO::InjDrvPar m_params[2];
  float *mp_curr_curve;
  float m_x_axis_min;
  float m_x_axis_max;
  float m_x_axis_step;
  float m_y_axis_min;
  float m_y_axis_max;
  int m_curve_idx;

  bool m_crc0_pane_f;
  bool m_crc1_pane_f;

  std::map<int, float> m_ptMovStep;
};
