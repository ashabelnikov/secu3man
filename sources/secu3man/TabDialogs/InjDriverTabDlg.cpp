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

/** \file InjDriverTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "InjDriverTabDlg.h"

#include "common/DPIAware.h"
#include "common/FastDelegate.h"
#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
#include "InjDriverTipDlg.h"
#include "io-core/Bitmask.h"
#include "ui-core/Chart2D.h"
#include "ui-core/ClickableBmp.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/Label.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"
#include "ui-core/MsgBox.h"

//Chart series
#define SERIE_PO 0
#define SERIE_PD 1
#define SERIE_HD 2
#define SERIE_VT 3
#define SERIE_PF 4
#define SERIE_PP 5
#define SERIE_PM 6
#define SERIE_PA 7

const int x_axis_points = SECU3IO::LUTABSIZE;
const int x_axis_points1 = SECU3IO::PWCORSIZE;
const float x_axis_min = 5.4f;
const float x_axis_step = 0.8f;
const float x_axis_max = x_axis_min + (x_axis_step * (x_axis_points-1));

CInjDriverTabDlg::CInjDriverTabDlg()
: mp_chart(new CChart2D())
, mp_secu3orgLink(new CLabel)
, mp_TipLink(new CLabel)
, mp_secu3logo(new CClickableBmp())
, mp_scr(new CWndScroller)
, mp_BLRecLink(new CLabel)
, m_initialized(false)
, m_enable(false)
, m_enable_ee_save(false)
, mp_curr_curve(NULL)
, m_set_of_sett_idx(0)
, m_x_axis_min(x_axis_min)
, m_x_axis_max(x_axis_max)
, m_x_axis_step(x_axis_step)
, m_y_axis_min(100.0f)
, m_y_axis_max(10000.0f)
, m_pwm_period_edit(CEditEx::MODE_FLOAT, true)
, m_peak_duty_edit(CEditEx::MODE_FLOAT, true)
, m_hold_duty_edit(CEditEx::MODE_FLOAT, true)
, m_peak_on_time_edit(CEditEx::MODE_FLOAT, true)
, m_peak_pwm_time_edit(CEditEx::MODE_FLOAT, true)
, m_pth_pause_edit(CEditEx::MODE_FLOAT, true)
, m_testch_frq_edit(CEditEx::MODE_FLOAT, true)
, m_testch_duty_edit(CEditEx::MODE_FLOAT, true)
, m_curve_idx(SERIE_PO)
, m_enable_voltage(false)
, m_offline(false)
, m_crc0_pane_f(false) //Ok
, m_crc1_pane_f(false) //Ok
{
 m_ptMovStep.insert(std::make_pair(SERIE_PO, 1.0f));
 m_ptMovStep.insert(std::make_pair(SERIE_PD, 0.1f));
 m_ptMovStep.insert(std::make_pair(SERIE_HD, 0.1f));
 m_ptMovStep.insert(std::make_pair(SERIE_PF, 1.0f));
 m_ptMovStep.insert(std::make_pair(SERIE_PP, 1.0f));
 m_ptMovStep.insert(std::make_pair(SERIE_PM, 0.1f));
 m_ptMovStep.insert(std::make_pair(SERIE_PA, 10.0f));

 mp_chart->AddSerie(16);       //peak on serie (see also defines)
 mp_chart->AddSerie(16);       //peak duty serie
 mp_chart->AddSerie(16);       //hold duty serie
 mp_chart->AddSerie(2);        //voltage serie (vert. line)
 mp_chart->AddSerie(16);       //peak full time
 mp_chart->AddSerie(16);       //peak-to-hold pause
 mp_chart->AddSerie(5);        //PW multiplication
 mp_chart->AddSerie(5);        //PW addition


 for (int i = 0; i < 2; ++i)
 {
  m_params[i].m_pwm_period = .0f;
  m_params[i].m_peak_duty = .0f;
  m_params[i].m_hold_duty = .0f;
  m_params[i].m_peak_on_time = .0f;
  m_params[i].m_peak_pwm_time = .0f;
  m_params[i].m_pth_pause = .0f;
  m_params[i].m_tst_peak_pwm = false;
  m_params[i].m_tst_hold_pwm = false;
  std::fill(m_params[i].m_peak_on_tab, m_params[i].m_peak_on_tab + SECU3IO::LUTABSIZE, .0f);
  std::fill(m_params[i].m_peak_duty_tab, m_params[i].m_peak_duty_tab + SECU3IO::LUTABSIZE, .0f);
  std::fill(m_params[i].m_hold_duty_tab, m_params[i].m_hold_duty_tab + SECU3IO::LUTABSIZE, .0f);
  std::fill(m_params[i].m_peak_full_tab, m_params[i].m_peak_full_tab + SECU3IO::LUTABSIZE, .0f);
  std::fill(m_params[i].m_pth_pause_tab, m_params[i].m_pth_pause_tab + SECU3IO::LUTABSIZE, .0f);
  std::fill(m_params[i].m_pwmul_tab, m_params[i].m_pwmul_tab + SECU3IO::PWCORSIZE, .0f);
  std::fill(m_params[i].m_pwadd_tab, m_params[i].m_pwadd_tab + SECU3IO::PWCORSIZE, .0f);
  m_params[i].m_peak_on_usetab = false;
  m_params[i].m_peak_duty_usetab = false;
  m_params[i].m_hold_duty_usetab = false;
  m_params[i].m_peak_full_usetab = false;
  m_params[i].m_pth_pause_usetab = false;
  m_params[i].m_tst_peak_pwm = false;
  m_params[i].m_tst_hold_pwm = false;
  m_params[i].m_testch_sel = 0;
  m_params[i].m_testch_frq = 30.0f;
  m_params[i].m_testch_duty = 33.0f;
 }
 mp_curr_curve = m_params[m_set_of_sett_idx].m_peak_on_tab;
}

void CInjDriverTabDlg::DoDataExchange(CDataExchange* pDX)
{
 DDX_CBIndex(pDX, IDC_SET_OF_SETT_COMBO, m_set_of_sett_idx); //must be first!

 DDX_CBIndex(pDX, IDC_INJDRV_TESTCH_COMBO, m_params[m_set_of_sett_idx].m_testch_sel);

 DDX_Control(pDX, IDC_PWM_PERIOD_EDIT, m_pwm_period_edit);
 DDX_Control(pDX, IDC_PWM_PERIOD_SPIN, m_pwm_period_spin);

 DDX_Control(pDX, IDC_PEAK_DUTY_EDIT, m_peak_duty_edit);
 DDX_Control(pDX, IDC_PEAK_DUTY_SPIN, m_peak_duty_spin);

 DDX_Control(pDX, IDC_HOLD_DUTY_EDIT, m_hold_duty_edit);
 DDX_Control(pDX, IDC_HOLD_DUTY_SPIN, m_hold_duty_spin);

 DDX_Control(pDX, IDC_PEAK_ON_TIME_EDIT, m_peak_on_time_edit);
 DDX_Control(pDX, IDC_PEAK_ON_TIME_SPIN, m_peak_on_time_spin);

 DDX_Control(pDX, IDC_PEAK_PWM_TIME_EDIT, m_peak_pwm_time_edit);
 DDX_Control(pDX, IDC_PEAK_PWM_TIME_SPIN, m_peak_pwm_time_spin);

 DDX_Control(pDX, IDC_PTH_PAUSE_EDIT, m_pth_pause_edit);
 DDX_Control(pDX, IDC_PTH_PAUSE_SPIN, m_pth_pause_spin);

 DDX_Control(pDX, IDC_INJDRV_TESTCH_FRQ_EDIT, m_testch_frq_edit);
 DDX_Control(pDX, IDC_INJDRV_TESTCH_FRQ_SPIN, m_testch_frq_spin);

 DDX_Control(pDX, IDC_INJDRV_TESTCH_DUTY_EDIT, m_testch_duty_edit);
 DDX_Control(pDX, IDC_INJDRV_TESTCH_DUTY_SPIN, m_testch_duty_spin);

 DDX_Control(pDX, IDC_EEPROM_SAVE_BUTTON, m_eeprom_save_btn);

 DDX_Control(pDX, IDC_VOLTAGE_PANE, m_voltage_pane);
 DDX_Control(pDX, IDC_INJDRV_GAS_V_PANE, m_gas_v_pane);
 DDX_Control(pDX, IDC_CRC0_PANE, m_crc0_pane);
 DDX_Control(pDX, IDC_CRC1_PANE, m_crc1_pane);

 DDX_Control(pDX, IDC_PEAK_ON_TABSEL_CHECK, m_peak_on_tabsel_check);
 DDX_Control(pDX, IDC_PEAK_DUTY_TABSEL_CHECK, m_peak_duty_tabsel_check);
 DDX_Control(pDX, IDC_HOLD_DUTY_TABSEL_CHECK, m_hold_duty_tabsel_check);
 DDX_Control(pDX, IDC_PEAK_FULL_TABSEL_CHECK, m_peak_full_tabsel_check);
 DDX_Control(pDX, IDC_PTH_PAUSE_TABSEL_CHECK, m_pth_pause_tabsel_check);
 DDX_Control(pDX, IDC_PWMUL_TABSEL_CHECK, m_pwmul_tabsel_check);
 DDX_Control(pDX, IDC_PWADD_TABSEL_CHECK, m_pwadd_tabsel_check);

 DDX_Control(pDX, IDC_PEAK_ON_CHECK, m_peak_on_check);
 DDX_Control(pDX, IDC_PEAK_DUTY_CHECK, m_peak_duty_check);
 DDX_Control(pDX, IDC_HOLD_DUTY_CHECK, m_hold_duty_check);
 DDX_Control(pDX, IDC_PEAK_FULL_CHECK, m_peak_full_check);
 DDX_Control(pDX, IDC_PTH_PAUSE_CHECK, m_pth_pause_check);

 DDX_Control(pDX, IDC_CURVE_UP_BUTTON, m_up_arrow);
 DDX_Control(pDX, IDC_CURVE_DOWN_BUTTON, m_down_arrow);

 DDX_Control(pDX, IDC_TST_PEAK_PWM_CHECK, m_tst_peak_pwm_check);
 DDX_Control(pDX, IDC_TST_HOLD_PWM_CHECK, m_tst_hold_pwm_check);

 DDX_Control(pDX, IDC_SET_OF_SETT_COMBO, m_set_of_sett_combo);
 DDX_Control(pDX, IDC_INJDRV_TESTCH_COMBO, m_testch_combo);

 DDX_Control(pDX, IDC_INJDRV_COMBO, m_injdrv_combo);

 DDX_Control(pDX,IDC_SECU3ORG_LINK, *mp_secu3orgLink);
 DDX_Control(pDX, IDC_SECU3LOGO_BITMAP, *mp_secu3logo);
 DDX_Control(pDX, IDC_INJDRV_TIP_LINK, *mp_TipLink);
 DDX_Control(pDX, IDC_INJDRV_BLRECOVER_LINK, *mp_BLRecLink);

 DDX_Control(pDX, IDC_LOAD_FLASH_CHECK, m_load_flash_check);

 DDX_Control(pDX, IDC_INJDRV_SIMULTAN_CHECK, m_edit_simultan_check);

 m_pwm_period_edit.DDX_Value(pDX, IDC_PWM_PERIOD_EDIT, m_params[m_set_of_sett_idx].m_pwm_period);
 m_peak_duty_edit.DDX_Value(pDX, IDC_PEAK_DUTY_EDIT, m_params[m_set_of_sett_idx].m_peak_duty);
 m_hold_duty_edit.DDX_Value(pDX, IDC_HOLD_DUTY_EDIT, m_params[m_set_of_sett_idx].m_hold_duty);
 m_peak_on_time_edit.DDX_Value(pDX, IDC_PEAK_ON_TIME_EDIT, m_params[m_set_of_sett_idx].m_peak_on_time);
 m_peak_pwm_time_edit.DDX_Value(pDX, IDC_PEAK_PWM_TIME_EDIT, m_params[m_set_of_sett_idx].m_peak_pwm_time);
 m_pth_pause_edit.DDX_Value(pDX, IDC_PTH_PAUSE_EDIT, m_params[m_set_of_sett_idx].m_pth_pause);
 m_testch_frq_edit.DDX_Value(pDX, IDC_INJDRV_TESTCH_FRQ_EDIT, m_params[m_set_of_sett_idx].m_testch_frq);
 m_testch_duty_edit.DDX_Value(pDX, IDC_INJDRV_TESTCH_DUTY_EDIT, m_params[m_set_of_sett_idx].m_testch_duty);

 DDX_Check_bool(pDX, IDC_PEAK_ON_CHECK, m_params[m_set_of_sett_idx].m_peak_on_usetab);
 DDX_Check_bool(pDX, IDC_PEAK_DUTY_CHECK, m_params[m_set_of_sett_idx].m_peak_duty_usetab);
 DDX_Check_bool(pDX, IDC_HOLD_DUTY_CHECK, m_params[m_set_of_sett_idx].m_hold_duty_usetab);
 DDX_Check_bool(pDX, IDC_PEAK_FULL_CHECK, m_params[m_set_of_sett_idx].m_peak_full_usetab);
 DDX_Check_bool(pDX, IDC_PTH_PAUSE_CHECK, m_params[m_set_of_sett_idx].m_pth_pause_usetab);

 DDX_Check_bool(pDX, IDC_TST_PEAK_PWM_CHECK, m_params[m_set_of_sett_idx].m_tst_peak_pwm);
 DDX_Check_bool(pDX, IDC_TST_HOLD_PWM_CHECK, m_params[m_set_of_sett_idx].m_tst_hold_pwm);

 bool fw_opt = CHECKBIT8(m_params[m_set_of_sett_idx].fw_opt, 5);
 DDX_Check_bool(pDX, IDC_LOAD_FLASH_CHECK, fw_opt);
 WRITEBIT8(m_params[m_set_of_sett_idx].fw_opt, 5, fw_opt);

 Super::DoDataExchange(pDX);
}

LPCTSTR CInjDriverTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_INJ_DRIVER;
}

BEGIN_MESSAGE_MAP(CInjDriverTabDlg, Super)
 ON_WM_DESTROY()
 ON_WM_CONTEXTMENU()
 ON_WM_SIZE()

 ON_BN_CLICKED(IDC_EEPROM_SAVE_BUTTON, OnEepromSaveButton)
 ON_BN_CLICKED(IDC_PEAK_ON_TABSEL_CHECK, OnPeakOnTabSelCheck)
 ON_BN_CLICKED(IDC_PEAK_DUTY_TABSEL_CHECK, OnPeakDutyTabSelCheck)
 ON_BN_CLICKED(IDC_HOLD_DUTY_TABSEL_CHECK, OnHoldDutyTabSelCheck)
 ON_BN_CLICKED(IDC_PEAK_FULL_TABSEL_CHECK, OnPeakFullTabSelCheck)
 ON_BN_CLICKED(IDC_PTH_PAUSE_TABSEL_CHECK, OnPthPauseTabSelCheck)
 ON_BN_CLICKED(IDC_PWMUL_TABSEL_CHECK, OnPWMulTabSelCheck)
 ON_BN_CLICKED(IDC_PWADD_TABSEL_CHECK, OnPWAddTabSelCheck)
 ON_BN_CLICKED(IDC_CURVE_UP_BUTTON, OnUpArrowButton)
 ON_BN_CLICKED(IDC_CURVE_DOWN_BUTTON, OnDownArrowButton)
 ON_BN_CLICKED(IDC_PEAK_ON_CHECK, OnChangeDataChecks)
 ON_BN_CLICKED(IDC_PEAK_DUTY_CHECK, OnChangeDataChecks)
 ON_BN_CLICKED(IDC_HOLD_DUTY_CHECK, OnChangeDataChecks)
 ON_BN_CLICKED(IDC_PEAK_FULL_CHECK, OnChangeDataChecks)
 ON_BN_CLICKED(IDC_PTH_PAUSE_CHECK, OnChangeDataChecks)
 ON_BN_CLICKED(IDC_TST_PEAK_PWM_CHECK, OnChangeDataTstPeakPwm)
 ON_BN_CLICKED(IDC_TST_HOLD_PWM_CHECK, OnChangeDataTstHoldPwm)
 ON_BN_CLICKED(IDC_LOAD_FLASH_CHECK, OnChangeDataChecks)

 ON_CBN_SELENDOK(IDC_SET_OF_SETT_COMBO, OnSelendokSetOfSett)
 ON_CBN_SELENDOK(IDC_INJDRV_TESTCH_COMBO, OnSelendokTestCh)
 ON_CBN_SELENDOK(IDC_INJDRV_COMBO, OnSelendokInjDrv)

 ON_EN_CHANGE(IDC_PWM_PERIOD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PEAK_DUTY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_HOLD_DUTY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PEAK_ON_TIME_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PEAK_PWM_TIME_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PTH_PAUSE_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_INJDRV_TESTCH_FRQ_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_INJDRV_TESTCH_DUTY_EDIT, OnChangeData)

 ON_COMMAND(ID_INJDRV_POPUP_EXPORTTOAFILE, OnExportToAFile)
 ON_COMMAND(ID_INJDRV_POPUP_IMPORTFROMAFILE, OnImportFromAFile)
 ON_COMMAND(ID_INJDRV_POPUP_IMPORTFROMSTOCK, OnImportFromStock)
 ON_COMMAND(ID_INJDRV_POPUP_SAVETOFIRMWARE, OnSaveToFirmware)
 ON_COMMAND(ID_INJDRV_POPUP_LOADFROMFIRMWARE, OnLoadFromFirmware)
 ON_COMMAND(ID_INJDRV_POPUP_SHOWFIRMWAREINFO, OnShowFirmwareInfo)
 ON_COMMAND(ID_INJDRV_POPUP_OFFLINEMODE, OnOfflineModeExit)
 ON_COMMAND(ID_INJDRV_POPUP_WRITEFIRMWAREFROMFILE, OnWriteFirmwareFromFile)
 ON_COMMAND(ID_INJDRV_POPUP_READFIRMWARETOFILE, OnReadFirmwareToFile)
 ON_COMMAND(ID_INJDRV_POPUP_READLZBLINFO, OnReadLzblInfo)
 ON_COMMAND(ID_INJDRV_POPUP_WRITEEEPROMFROMFILE, OnWriteEEPROMFromFile)
 ON_COMMAND(ID_INJDRV_POPUP_READEEPROMTOFILE, OnReadEEPROMToFile)
 ON_COMMAND(ID_INJDRV_POPUP_RESETEEPROM, OnResetEEPROM)
 ON_COMMAND(ID_INJDRV_POPUP_FIRMWAREMASTER, OnFirmwareMaster)

 ON_UPDATE_COMMAND_UI(IDC_PEAK_DUTY_EDIT,OnUpdateControlsPD)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_DUTY_SPIN,OnUpdateControlsPD)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_DUTY_CAPTION,OnUpdateControlsPD)

 ON_UPDATE_COMMAND_UI(IDC_HOLD_DUTY_EDIT,OnUpdateControlsHD)
 ON_UPDATE_COMMAND_UI(IDC_HOLD_DUTY_SPIN,OnUpdateControlsHD)
 ON_UPDATE_COMMAND_UI(IDC_HOLD_DUTY_CAPTION,OnUpdateControlsHD)

 ON_UPDATE_COMMAND_UI(IDC_PEAK_ON_TIME_EDIT,OnUpdateControlsPO)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_ON_TIME_SPIN,OnUpdateControlsPO)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_ON_TIME_CAPTION,OnUpdateControlsPO)

 ON_UPDATE_COMMAND_UI(IDC_CURVE_UP_BUTTON, OnUpdateControlsArrowBtns)
 ON_UPDATE_COMMAND_UI(IDC_CURVE_DOWN_BUTTON, OnUpdateControlsArrowBtns)

 ON_UPDATE_COMMAND_UI(IDC_PWM_PERIOD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PWM_PERIOD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PWM_PERIOD_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PEAK_PWM_TIME_EDIT,OnUpdateControlsPF)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_PWM_TIME_SPIN,OnUpdateControlsPF)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_PWM_TIME_CAPTION,OnUpdateControlsPF)

 ON_UPDATE_COMMAND_UI(IDC_PTH_PAUSE_EDIT,OnUpdateControlsPP)
 ON_UPDATE_COMMAND_UI(IDC_PTH_PAUSE_SPIN,OnUpdateControlsPP)
 ON_UPDATE_COMMAND_UI(IDC_PTH_PAUSE_CAPTION,OnUpdateControlsPP)

 ON_UPDATE_COMMAND_UI(IDC_INJDRV_TESTCH_FRQ_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_INJDRV_TESTCH_FRQ_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_INJDRV_TESTCH_FRQ_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_INJDRV_TESTCH_DUTY_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_INJDRV_TESTCH_DUTY_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_INJDRV_TESTCH_DUTY_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_VCURVES_CHART, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_ON_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_DUTY_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_HOLD_DUTY_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PEAK_FULL_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PTH_PAUSE_CHECK, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_TST_PEAK_PWM_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TST_HOLD_PWM_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_SET_OF_SETT_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_SET_OF_SETT_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_INJDRV_TESTCH_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_INJDRV_TESTCH_CAPTION, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_VOLTAGE_PANE,OnUpdateControlsVoltage)
 ON_UPDATE_COMMAND_UI(IDC_VOLTAGE_CAPTION,OnUpdateControlsVoltage)
 ON_UPDATE_COMMAND_UI(IDC_INJDRV_GAS_V_PANE,OnUpdateControlsVoltage)
 ON_UPDATE_COMMAND_UI(IDC_INJDRV_GAS_V_CAPTION,OnUpdateControlsVoltage)
 ON_UPDATE_COMMAND_UI(IDC_CRC0_PANE,OnUpdateControlsVoltage)
 ON_UPDATE_COMMAND_UI(IDC_CRC0_CAPTION,OnUpdateControlsVoltage)
 ON_UPDATE_COMMAND_UI(IDC_CRC1_PANE,OnUpdateControlsVoltage)
 ON_UPDATE_COMMAND_UI(IDC_CRC1_CAPTION,OnUpdateControlsVoltage)

 ON_UPDATE_COMMAND_UI(IDC_EEPROM_SAVE_BUTTON,OnUpdateControlsEESave)

 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_EXPORTTOAFILE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_IMPORTFROMAFILE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_IMPORTFROMSTOCK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_SAVETOFIRMWARE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_SHOWFIRMWAREINFO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_OFFLINEMODE, OnUpdateOfflineModeExit)

 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_WRITEFIRMWAREFROMFILE, OnUpdateBLItems)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_READFIRMWARETOFILE, OnUpdateBLItems)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_READLZBLINFO, OnUpdateBLItems)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_WRITEEEPROMFROMFILE, OnUpdateBLItems)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_READEEPROMTOFILE, OnUpdateBLItems)
 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_FIRMWAREMASTER, OnUpdateBLItems)

 ON_UPDATE_COMMAND_UI(ID_INJDRV_POPUP_RESETEEPROM, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_LOAD_FLASH_CHECK, OnUpdateControls)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInjDriverTabDlg message handlers

BOOL CInjDriverTabDlg::OnInitDialog()
{
 m_x_axis_min = x_axis_min;
 m_x_axis_max = x_axis_max;
 m_x_axis_step = x_axis_step;
 m_y_axis_min = 100.0f;
 m_y_axis_max = 10000.0f;

 Super::OnInitDialog();

 m_crc0_pane_f = m_crc1_pane_f = false; //Ok, not corrupted
 m_crc0_pane.SetWindowText(_T("OK"));
 m_crc1_pane.SetWindowText(_T("OK"));

 m_injdrv_combo.AddString(_T("1"));
 m_injdrv_combo.AddString(_T("2"));
 m_injdrv_combo.SetCurSel(0);

 //init HTTP link
 mp_secu3orgLink->SetLink(true);
 mp_secu3orgLink->SetTextColor(RGB(0, 0, 255));
 mp_secu3orgLink->SetFontUnderline(true);
 mp_secu3orgLink->SetLinkCursor((HCURSOR)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));
 CString str;
 mp_secu3orgLink->GetWindowText(str);
 if (str[0] != _T('h') || str[1] != _T('t') || str[2] != _T('t') || str[3] != _T('p') || str[4] != _T(':') || str[5] != _T('/') || str[6] != _T('/') || str[7] != _T('s') || str[8] != _T('e') || str[9] != _T('c') || str[10] != _T('u') || str[11] != _T('3') || str[12] != _T('.') || str[13] != _T('o') || str[14] != _T('r') || str[15] != _T('g'))
  return FALSE;

 //init tip link
 mp_TipLink->SetLink(true);
 mp_TipLink->SetTextColor(RGB(0, 0, 255));
 mp_TipLink->SetFontUnderline(true);
 mp_TipLink->SetLinkCursor((HCURSOR)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));
 mp_TipLink->SetOnClick(fastdelegate::MakeDelegate(this, &CInjDriverTabDlg::OnTipLinkClick));

 mp_BLRecLink->SetLink(true);
 mp_BLRecLink->SetTextColor(RGB(0, 0, 255));
 mp_BLRecLink->SetFontUnderline(true);
 mp_BLRecLink->SetLinkCursor((HCURSOR)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));
 mp_BLRecLink->SetOnClick(fastdelegate::MakeDelegate(this, &CInjDriverTabDlg::OnBLRecoverLinkClick));

 //init edit boxes and spin buttons
 m_pwm_period_spin.SetBuddy(&m_pwm_period_edit);
 m_pwm_period_edit.SetLimitText(6);
 m_pwm_period_edit.SetDecimalPlaces(1);
 m_pwm_period_spin.SetRangeAndDelta(20.0f, 3000.0f, 0.1f);
 m_pwm_period_edit.SetRange(20.0f, 9999.0f);

 m_peak_duty_spin.SetBuddy(&m_peak_duty_edit);
 m_peak_duty_edit.SetLimitText(5);
 m_peak_duty_edit.SetDecimalPlaces(1);
 m_peak_duty_spin.SetRangeAndDelta(0.0f, 100.0f, 0.1f);
 m_peak_duty_edit.SetRange(0.0f, 100.0f);

 m_hold_duty_spin.SetBuddy(&m_hold_duty_edit);
 m_hold_duty_edit.SetLimitText(5);
 m_hold_duty_edit.SetDecimalPlaces(1);
 m_hold_duty_spin.SetRangeAndDelta(0.0f, 100.0f, 0.1f);
 m_hold_duty_edit.SetRange(0.0f, 100.0f);

 m_peak_on_time_spin.SetBuddy(&m_peak_on_time_edit);
 m_peak_on_time_edit.SetLimitText(4);
 m_peak_on_time_edit.SetDecimalPlaces(0);
 m_peak_on_time_spin.SetRangeAndDelta(100.0f, 9999.0f, 1.0f);
 m_peak_on_time_edit.SetRange(100.0f, 9999.0f);

 m_peak_pwm_time_spin.SetBuddy(&m_peak_pwm_time_edit);
 m_peak_pwm_time_edit.SetLimitText(4);
 m_peak_pwm_time_edit.SetDecimalPlaces(0);
 m_peak_pwm_time_spin.SetRangeAndDelta(100.0f, 9999.0f, 1.0f);
 m_peak_pwm_time_edit.SetRange(100.0f, 9999.0f);

 m_pth_pause_spin.SetBuddy(&m_pth_pause_edit);
 m_pth_pause_edit.SetLimitText(4);
 m_pth_pause_edit.SetDecimalPlaces(0);
 m_pth_pause_spin.SetRangeAndDelta(1.0f, 1000.0f, 1.0f);
 m_pth_pause_edit.SetRange(1.0f, 1000.0f);

 m_testch_frq_spin.SetBuddy(&m_testch_frq_edit);
 m_testch_frq_edit.SetLimitText(6);
 m_testch_frq_edit.SetDecimalPlaces(1);
 m_testch_frq_spin.SetRangeAndDelta(2.5f, 200.0f, 0.1f);
 m_testch_frq_edit.SetRange(2.5f, 200.0f);

 m_testch_duty_spin.SetBuddy(&m_testch_duty_edit);
 m_testch_duty_edit.SetLimitText(6);
 m_testch_duty_edit.SetDecimalPlaces(1);
 m_testch_duty_spin.SetRangeAndDelta(0.0f, 100.0f, 0.5f);
 m_testch_duty_edit.SetRange(0.0f, 100.0f);

 //Init "set of settings" combo
 m_set_of_sett_combo.AddString(MLL::LoadString(IDS_INJDRV_PETROL_SET));
 m_set_of_sett_combo.AddString(MLL::LoadString(IDS_INJDRV_GAS_SET));

 m_testch_combo.AddString(_T(" OFF"));
 m_testch_combo.AddString(_T(" 1"));
 m_testch_combo.AddString(_T(" 2"));
 m_testch_combo.AddString(_T(" 3"));
 m_testch_combo.AddString(_T(" 4"));
 m_testch_combo.AddString(_T(" 5"));

 //init chart
 CRect rect;
 GetDlgItem(IDC_VCURVES_CHART_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_chart->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, rect, this, IDC_VCURVES_CHART);
 mp_chart->SetOnChange(fastdelegate::MakeDelegate(this, &CInjDriverTabDlg::OnChangeChart));
 mp_chart->SetWindowPos(GetDlgItem(IDC_CURVE_DOWN_BUTTON), 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

 mp_chart->SetXAxisValueFormat(_T("%.01f"));
 mp_chart->SetYAxisValueFormat(_T("%.0f")); //integer
 mp_chart->SetChartTitle(MLL::LoadString(IDS_PEAK_ON_TIME_TITLE));
 mp_chart->SetGridXYNumber(x_axis_points-1, 9);
 mp_chart->SetRange(m_x_axis_min, m_x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetBackColor(RGB(180, 200, 180));
 mp_chart->SetSerieColor(SERIE_PO, RGB(80, 80, 200));
 mp_chart->SetSerieColor(SERIE_PD, RGB(80, 80, 200));
 mp_chart->SetSerieColor(SERIE_HD, RGB(80, 80, 200));
 mp_chart->SetSerieColor(SERIE_PF, RGB(80, 80, 200));
 mp_chart->SetSerieColor(SERIE_PP, RGB(80, 80, 200));
 mp_chart->SetSerieColor(SERIE_PM, RGB(80, 80, 200));
 mp_chart->SetSerieColor(SERIE_PA, RGB(80, 80, 200));
 mp_chart->SetSerieHandle(SERIE_PO, true);
 mp_chart->SetSerieHandle(SERIE_PD, true);
 mp_chart->SetSerieHandle(SERIE_HD, true);
 mp_chart->SetSerieHandle(SERIE_PF, true);
 mp_chart->SetSerieHandle(SERIE_PP, true);
 mp_chart->SetSerieHandle(SERIE_PM, true);
 mp_chart->SetSerieHandle(SERIE_PA, true);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetSerieVisibility(SERIE_PF, false);
 mp_chart->SetSerieVisibility(SERIE_PP, false);
 mp_chart->SetSerieVisibility(SERIE_PM, false);
 mp_chart->SetSerieVisibility(SERIE_PA, false);
 mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));

 mp_chart->SetXYValue(SERIE_VT, m_x_axis_min, m_y_axis_min, 0);
 mp_chart->SetXYValue(SERIE_VT, m_x_axis_min, m_y_axis_max, 1);

 UpdateChartValues();
 m_peak_on_tabsel_check.SetCheck(BST_CHECKED);

 //arrow buttons
 HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_UP_ARROW), IMAGE_ICON, 0,0, LR_DEFAULTSIZE);
 m_up_arrow.SetIcon(hIcon);
 hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_DOWN_ARROW), IMAGE_ICON, 0,0, LR_DEFAULTSIZE);
 m_down_arrow.SetIcon(hIcon);

 //init tool tips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 VERIFY(mp_ttc->AddWindow(&m_peak_on_check, MLL::GetString(IDS_PEAK_ON_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_duty_check, MLL::GetString(IDS_PEAK_DUTY_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_hold_duty_check, MLL::GetString(IDS_HOLD_DUTY_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tst_peak_pwm_check, MLL::GetString(IDS_TST_PEAK_PWM_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tst_hold_pwm_check, MLL::GetString(IDS_TST_HOLD_PWM_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_set_of_sett_combo, MLL::GetString(IDS_SET_OF_SETT_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_eeprom_save_btn, MLL::GetString(IDS_EEPROM_SAVE_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_full_check, MLL::GetString(IDS_PEAK_FULL_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_pth_pause_check, MLL::GetString(IDS_PTH_PAUSE_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_pwm_period_edit, MLL::GetString(IDS_PWM_PERIOD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_pwm_period_spin, MLL::GetString(IDS_PWM_PERIOD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_duty_edit, MLL::GetString(IDS_PEAK_DUTY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_duty_spin, MLL::GetString(IDS_PEAK_DUTY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_hold_duty_edit, MLL::GetString(IDS_HOLD_DUTY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_hold_duty_spin, MLL::GetString(IDS_HOLD_DUTY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_on_time_edit, MLL::GetString(IDS_PEAK_ON_TIME_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_on_time_spin, MLL::GetString(IDS_PEAK_ON_TIME_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_pwm_time_edit, MLL::GetString(IDS_PEAK_PWM_TIME_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_pwm_time_spin, MLL::GetString(IDS_PEAK_PWM_TIME_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_pth_pause_edit, MLL::GetString(IDS_PTH_PAUSE_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_pth_pause_spin, MLL::GetString(IDS_PTH_PAUSE_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_voltage_pane, MLL::GetString(IDS_VOLTAGE_PANE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gas_v_pane, MLL::GetString(IDS_INJDRV_GAS_V_PANE_TT)));

 VERIFY(mp_ttc->AddWindow(&m_testch_combo, MLL::GetString(IDS_INJDRV_TESTCH_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_testch_frq_edit, MLL::GetString(IDS_INJDRV_TESTCH_FRQ_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_testch_frq_spin, MLL::GetString(IDS_INJDRV_TESTCH_FRQ_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_testch_duty_edit, MLL::GetString(IDS_INJDRV_TESTCH_DUTY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_testch_duty_spin, MLL::GetString(IDS_INJDRV_TESTCH_DUTY_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_crc0_pane, MLL::GetString(IDS_CRC0_PANE_TT)));
 VERIFY(mp_ttc->AddWindow(&m_crc1_pane, MLL::GetString(IDS_CRC1_PANE_TT)));

 VERIFY(mp_ttc->AddWindow(&m_load_flash_check, MLL::GetString(IDS_LOAD_FLASH_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_pwmul_tabsel_check, MLL::GetString(IDS_PWMUL_TABSEL_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_pwadd_tabsel_check, MLL::GetString(IDS_PWADD_TABSEL_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_injdrv_combo, MLL::GetString(IDS_INJDRV_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_peak_on_tabsel_check, MLL::GetString(IDS_PEAK_ON_TABSEL_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_duty_tabsel_check, MLL::GetString(IDS_PEAK_DUTY_TABSEL_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_hold_duty_tabsel_check, MLL::GetString(IDS_HOLD_DUTY_TABSEL_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_peak_full_tabsel_check, MLL::GetString(IDS_PEAK_FULL_TABSEL_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_pth_pause_tabsel_check, MLL::GetString(IDS_PTH_PAUSE_TABSEL_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_edit_simultan_check, MLL::GetString(IDS_INJDRV_SIMULTAN_CHECK_TT)));

 mp_ttc->SetMaxTipWidth(150); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 m_initialized = true;

 mp_secu3logo->SetBitmapID(IDB_SECU3LOGO_BITMAP);
 mp_secu3logo->SetClickCursor((HCURSOR)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));
 mp_secu3logo->SetOnClick(fastdelegate::MakeDelegate(this, &CInjDriverTabDlg::OnLogoClick));

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);

 //initialize window scroller
 mp_scr->Init(this);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CInjDriverTabDlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
 m_enable = false;
 mp_scr->Close();
}

void CInjDriverTabDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 CMenu menu;
 VERIFY(menu.LoadMenu(IDR_INJDRV_POPUP_MENU));
 CMenu *pSub = menu.GetSubMenu(0);
 pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CInjDriverTabDlg::OnSize( UINT nType, int cx, int cy )
{
 if (m_initialized)
 {
  CRect rc1, rc2;
  DPIAware da;

  rc1 = GDIHelpers::GetChildWndRect(&m_up_arrow);
  GetClientRect(&rc2);
  m_up_arrow.SetWindowPos(NULL, rc2.right - rc1.Width() - da.ScaleX(3), rc1.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(&m_down_arrow);
  int buttonPos = rc2.right - rc1.Width() - da.ScaleX(3);
  m_down_arrow.SetWindowPos(NULL, buttonPos, rc1.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(mp_chart.get());
  mp_chart->SetWindowPos(NULL, 0, 0, buttonPos - rc1.left - da.ScaleX(3) , rc2.bottom - rc1.top - da.ScaleY(5), SWP_NOMOVE | SWP_NOZORDER);
 }

 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(da.ScaleX(1120), da.ScaleY(580));
}

void CInjDriverTabDlg::SetVoltLineColor(DWORD color)
{
 mp_chart->SetSerieColor(SERIE_VT, color);
}

void CInjDriverTabDlg::SetPtMovStep(float peakOn, float peakDuty, float holdDuty, float peakFull, float pthPause, float pwMul, float pwAdd)
{
 //update internally stored values
 m_ptMovStep[SERIE_PO] = peakOn;
 m_ptMovStep[SERIE_PD] = peakDuty;
 m_ptMovStep[SERIE_HD] = holdDuty;
 m_ptMovStep[SERIE_PF] = peakFull;
 m_ptMovStep[SERIE_PP] = pthPause;
 m_ptMovStep[SERIE_PM] = pwMul;
 m_ptMovStep[SERIE_PA] = pwAdd;

 //update chart
 if (m_peak_on_tabsel_check.GetCheck() == BST_CHECKED)  
  mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));
 else if (m_peak_duty_tabsel_check.GetCheck() == BST_CHECKED)
  mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));
 else if (m_hold_duty_tabsel_check.GetCheck() == BST_CHECKED)
  mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));
 else if (m_peak_full_tabsel_check.GetCheck() == BST_CHECKED)
  mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));
 else if (m_pth_pause_tabsel_check.GetCheck() == BST_CHECKED)
  mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));
 else if (m_pwmul_tabsel_check.GetCheck() == BST_CHECKED)
  mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));
 else if (m_pwadd_tabsel_check.GetCheck() == BST_CHECKED)
  mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));
}

void CInjDriverTabDlg::OnChangeChart(int serieIdx)
{
 const double *x = mp_chart->GetYValues(serieIdx);
 if (serieIdx == SERIE_PO)
 {
  for(int i = 0; i < x_axis_points; ++i)
   m_params[m_set_of_sett_idx].m_peak_on_tab[i] = (float)x[i];
 }
 else if (serieIdx == SERIE_PD)
 {
  for(int i = 0; i < x_axis_points; ++i)
   m_params[m_set_of_sett_idx].m_peak_duty_tab[i] = (float)x[i]; 
 }
 else if (serieIdx == SERIE_HD)
 {
  for(int i = 0; i < x_axis_points; ++i)
   m_params[m_set_of_sett_idx].m_hold_duty_tab[i] = (float)x[i];
 }
 else if (serieIdx == SERIE_PF)
 {
  for(int i = 0; i < x_axis_points; ++i)
   m_params[m_set_of_sett_idx].m_peak_full_tab[i] = (float)x[i];
 }
 else if (serieIdx == SERIE_PP)
 {
  for(int i = 0; i < x_axis_points; ++i)
   m_params[m_set_of_sett_idx].m_pth_pause_tab[i] = (float)x[i];
 }
 else if (serieIdx == SERIE_PM)
 {
  for(int i = 0; i < x_axis_points1; ++i)
   m_params[m_set_of_sett_idx].m_pwmul_tab[i] = (float)x[i];
 }
 else if (serieIdx == SERIE_PA)
 {
  for(int i = 0; i < x_axis_points1; ++i)
   m_params[m_set_of_sett_idx].m_pwadd_tab[i] = (float)x[i];
 }
 else
  return; //error

 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::UpdateChartValues(void)
{
 for (int i = 0; i < x_axis_points; ++i)
 {
  mp_chart->SetXYValue(SERIE_PO, m_x_axis_min + (m_x_axis_step * i), m_params[m_set_of_sett_idx].m_peak_on_tab[i], i);
  mp_chart->SetXYValue(SERIE_PD, m_x_axis_min + (m_x_axis_step * i), m_params[m_set_of_sett_idx].m_peak_duty_tab[i], i);
  mp_chart->SetXYValue(SERIE_HD, m_x_axis_min + (m_x_axis_step * i), m_params[m_set_of_sett_idx].m_hold_duty_tab[i], i);
  mp_chart->SetXYValue(SERIE_PF, m_x_axis_min + (m_x_axis_step * i), m_params[m_set_of_sett_idx].m_peak_full_tab[i], i);
  mp_chart->SetXYValue(SERIE_PP, m_x_axis_min + (m_x_axis_step * i), m_params[m_set_of_sett_idx].m_pth_pause_tab[i], i);
 }
 for (int i = 0; i < x_axis_points1; ++i)
 {
  mp_chart->SetXYValue(SERIE_PM, m_x_axis_min + (m_x_axis_step * i), m_params[m_set_of_sett_idx].m_pwmul_tab[i], i);
  mp_chart->SetXYValue(SERIE_PA, m_x_axis_min + (m_x_axis_step * i), m_params[m_set_of_sett_idx].m_pwadd_tab[i], i);
 }
}

void CInjDriverTabDlg::OnEepromSaveButton()
{
 if (m_onSaveButton)
  m_onSaveButton();
}

void CInjDriverTabDlg::OnPeakOnTabSelCheck()
{
 m_peak_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_hold_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_full_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pth_pause_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwmul_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwadd_tabsel_check.SetCheck(BST_UNCHECKED);
 m_curve_idx = SERIE_PO;

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, true, true);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetSerieVisibility(SERIE_PF, false);
 mp_chart->SetSerieVisibility(SERIE_PP, false);
 mp_chart->SetSerieVisibility(SERIE_PM, false);
 mp_chart->SetSerieVisibility(SERIE_PA, false);
 mp_chart->SetSerieVisibility(SERIE_VT, true);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_PEAK_ON_TIME_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.0f")); //integer
 mp_chart->SetXAxisValueFormat(_T("%.01f"));
 mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));

 m_y_axis_min = 100.0f;
 m_y_axis_max = 10000.0f;
 m_x_axis_min = x_axis_min;
 m_x_axis_max = x_axis_max;
 m_x_axis_step = x_axis_step;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_peak_on_tab;
 mp_chart->SetRange(m_x_axis_min, m_x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points-1, 9);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnPeakDutyTabSelCheck()
{
 m_peak_on_tabsel_check.SetCheck(BST_UNCHECKED);
 m_hold_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_full_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pth_pause_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwmul_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwadd_tabsel_check.SetCheck(BST_UNCHECKED);
 m_curve_idx = SERIE_PD;

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, false);
 mp_chart->SetSerieVisibility(SERIE_PD, true, true);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetSerieVisibility(SERIE_PF, false);
 mp_chart->SetSerieVisibility(SERIE_PP, false);
 mp_chart->SetSerieVisibility(SERIE_PM, false);
 mp_chart->SetSerieVisibility(SERIE_PA, false);
 mp_chart->SetSerieVisibility(SERIE_VT, true);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_PEAK_DUTY_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.01f")); //fractional
 mp_chart->SetXAxisValueFormat(_T("%.01f"));
 mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));

 m_y_axis_min = 0.0f;
 m_y_axis_max = 100.0f;
 m_x_axis_min = x_axis_min;
 m_x_axis_max = x_axis_max;
 m_x_axis_step = x_axis_step;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_peak_duty_tab;
 mp_chart->SetRange(m_x_axis_min, m_x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points-1, 10);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnHoldDutyTabSelCheck()
{
 m_peak_on_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_full_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pth_pause_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwmul_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwadd_tabsel_check.SetCheck(BST_UNCHECKED);
 m_curve_idx = SERIE_HD;

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, false);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, true, true);
 mp_chart->SetSerieVisibility(SERIE_PF, false);
 mp_chart->SetSerieVisibility(SERIE_PP, false);
 mp_chart->SetSerieVisibility(SERIE_PM, false);
 mp_chart->SetSerieVisibility(SERIE_PA, false);
 mp_chart->SetSerieVisibility(SERIE_VT, true);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_HOLD_DUTY_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.01f")); //fractional
 mp_chart->SetXAxisValueFormat(_T("%.01f"));
 mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));

 m_y_axis_min = 0.0f;
 m_y_axis_max = 100.0f;
 m_x_axis_min = x_axis_min;
 m_x_axis_max = x_axis_max;
 m_x_axis_step = x_axis_step;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_hold_duty_tab; 
 mp_chart->SetRange(m_x_axis_min, m_x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points-1, 10);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnPeakFullTabSelCheck()
{
 m_peak_on_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_hold_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pth_pause_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwmul_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwadd_tabsel_check.SetCheck(BST_UNCHECKED);
 m_curve_idx = SERIE_PF;

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, false);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetSerieVisibility(SERIE_PF, true, true);
 mp_chart->SetSerieVisibility(SERIE_PP, false);
 mp_chart->SetSerieVisibility(SERIE_PM, false);
 mp_chart->SetSerieVisibility(SERIE_PA, false);
 mp_chart->SetSerieVisibility(SERIE_VT, true);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_PEAK_FULL_TIME_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.0f")); //integer
 mp_chart->SetXAxisValueFormat(_T("%.01f"));
 mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));

 m_y_axis_min = 100.0f;
 m_y_axis_max = 10000.0f;
 m_x_axis_min = x_axis_min;
 m_x_axis_max = x_axis_max;
 m_x_axis_step = x_axis_step;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_peak_full_tab; 
 mp_chart->SetRange(m_x_axis_min, m_x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points-1, 9);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnPthPauseTabSelCheck()
{
 m_peak_on_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_hold_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_full_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwmul_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwadd_tabsel_check.SetCheck(BST_UNCHECKED);
 m_curve_idx = SERIE_PP;

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, false);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetSerieVisibility(SERIE_PF, false);
 mp_chart->SetSerieVisibility(SERIE_PP, true, true);
 mp_chart->SetSerieVisibility(SERIE_PM, false);
 mp_chart->SetSerieVisibility(SERIE_PA, false);
 mp_chart->SetSerieVisibility(SERIE_VT, true);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_PTH_PAUSE_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.0f")); //integer
 mp_chart->SetXAxisValueFormat(_T("%.01f"));
 mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));

 m_y_axis_min = 1.0f;
 m_y_axis_max = 1000.0f;
 m_x_axis_min = x_axis_min;
 m_x_axis_max = x_axis_max;
 m_x_axis_step = x_axis_step;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_pth_pause_tab;
 mp_chart->SetRange(m_x_axis_min, m_x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points-1, 10);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnPWMulTabSelCheck()
{
 m_pth_pause_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_on_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_hold_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_full_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwadd_tabsel_check.SetCheck(BST_UNCHECKED);
 m_curve_idx = SERIE_PM;

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, false);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetSerieVisibility(SERIE_PF, false);
 mp_chart->SetSerieVisibility(SERIE_PP, false);
 mp_chart->SetSerieVisibility(SERIE_PM, true, true);
 mp_chart->SetSerieVisibility(SERIE_PA, false);
 mp_chart->SetSerieVisibility(SERIE_VT, false);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_INJDRV_PWMUL_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.02f")); //float
 mp_chart->SetXAxisValueFormat(_T("%.0f"));  //integer
 mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));

 m_y_axis_min = 0.0f;
 m_y_axis_max = 25.0f; //25%
 m_x_axis_min = 1;
 m_x_axis_max = 5;
 m_x_axis_step = 1;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_pwmul_tab;
 mp_chart->SetRange(m_x_axis_min, m_x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points1-1, 10);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnPWAddTabSelCheck()
{
 m_pth_pause_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_on_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_hold_duty_tabsel_check.SetCheck(BST_UNCHECKED);
 m_peak_full_tabsel_check.SetCheck(BST_UNCHECKED);
 m_pwmul_tabsel_check.SetCheck(BST_UNCHECKED);
 m_curve_idx = SERIE_PA;

 mp_chart->ClearChart();
 mp_chart->SetSerieVisibility(SERIE_PO, false);
 mp_chart->SetSerieVisibility(SERIE_PD, false);
 mp_chart->SetSerieVisibility(SERIE_HD, false);
 mp_chart->SetSerieVisibility(SERIE_PF, false);
 mp_chart->SetSerieVisibility(SERIE_PP, false);
 mp_chart->SetSerieVisibility(SERIE_PM, false);
 mp_chart->SetSerieVisibility(SERIE_PA, true, true);
 mp_chart->SetSerieVisibility(SERIE_VT, false);
 mp_chart->SetChartTitle(MLL::LoadString(IDS_INJDRV_PWADD_TITLE));
 mp_chart->SetYAxisValueFormat(_T("%.0f")); //integer
 mp_chart->SetXAxisValueFormat(_T("%.0f")); //integer
 mp_chart->SetPtMovStep(static_cast<double>(m_ptMovStep[m_curve_idx]));

 m_y_axis_min = 0.0f;
 m_y_axis_max = 2500.0f;
 m_x_axis_min = 1;
 m_x_axis_max = 5;
 m_x_axis_step = 1;
 mp_curr_curve = m_params[m_set_of_sett_idx].m_pwadd_tab;
 mp_chart->SetRange(m_x_axis_min, m_x_axis_max, m_y_axis_min, m_y_axis_max);
 mp_chart->SetGridXYNumber(x_axis_points1-1, 10);

 UpdateChartValues();
 mp_chart->Invalidate(TRUE);
}

void CInjDriverTabDlg::OnUpArrowButton()
{
 if (!mp_curr_curve)
  return;
 for (int i = 0; i < x_axis_points; ++i)
 {
  mp_curr_curve[i]+=m_ptMovStep[m_curve_idx];
  if (mp_curr_curve[i] > m_y_axis_max)
   mp_curr_curve[i] = m_y_axis_max;
  mp_chart->SetXYValue(m_curve_idx, m_x_axis_min + (m_x_axis_step * i), mp_curr_curve[i], i);
 }

 mp_chart->Invalidate();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnDownArrowButton()
{
 if (!mp_curr_curve)
  return;
 for (int i = 0; i < x_axis_points; ++i)
 {
  mp_curr_curve[i]-=m_ptMovStep[m_curve_idx];
  if (mp_curr_curve[i] < m_y_axis_min)
   mp_curr_curve[i] = m_y_axis_min;
  mp_chart->SetXYValue(m_curve_idx, m_x_axis_min + (m_x_axis_step * i), mp_curr_curve[i], i);
 }

 mp_chart->Invalidate();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnSelendokSetOfSett()
{
 m_set_of_sett_idx = m_set_of_sett_combo.GetCurSel();

 if (m_curve_idx == SERIE_PO)
  OnPeakOnTabSelCheck();
 else if (m_curve_idx == SERIE_PD)
  OnPeakDutyTabSelCheck();
 else if (m_curve_idx == SERIE_HD)
  OnHoldDutyTabSelCheck();
 else if (m_curve_idx == SERIE_PF)
  OnPeakFullTabSelCheck();
 else if (m_curve_idx == SERIE_PP)
  OnPthPauseTabSelCheck();
 else if (m_curve_idx == SERIE_PM)
  OnPWMulTabSelCheck();
 else if (m_curve_idx == SERIE_PA)
  OnPWAddTabSelCheck();

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::OnSelendokTestCh()
{
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnSelendokInjDrv()
{
 if (m_onSelInjDrv)
  m_onSelInjDrv();
}

void CInjDriverTabDlg::setOnChange(EventHandler onCB)
{
 m_onChange = onCB;
}

void CInjDriverTabDlg::setOnSaveButton(EventHandler onCB)
{
 m_onSaveButton = onCB;
}

void CInjDriverTabDlg::setOnExportToAFile(EventHandler onCB)
{
 m_onExportToAFile = onCB;
}

void CInjDriverTabDlg::setOnImportFromAFile(EventHandler onCB)
{
 m_onImportFromAFile = onCB;
}

void CInjDriverTabDlg::setOnSaveToFirmware(EventHandler onCB)
{
 m_onSaveToFirmware = onCB;
}

void CInjDriverTabDlg::setOnLoadFromFirmware(EventHandler onCB)
{
 m_onLoadFromFirmware = onCB;
}

void CInjDriverTabDlg::setOnShowFirmwareInfo(EventHandler onCB)
{
 m_onShowFirmwareInfo = onCB;
}

void CInjDriverTabDlg::setOnExitOfflineMode(EventHandler onCB)
{
 m_onExitOfflineMode = onCB;
}

void CInjDriverTabDlg::setOnWriteFirmwareFromFile(EventHandler onCB)
{
 m_onWriteFirmwareFromFile = onCB;
}

void CInjDriverTabDlg::setOnReadFirmwareToFile(EventHandler onCB)
{
 m_onReadFirmwareToFile = onCB;
}

void CInjDriverTabDlg::setOnReadLzblInfo(EventHandler onCB)
{
 m_onReadLzblInfo = onCB;
}

void CInjDriverTabDlg::setOnWriteEEPROMFromFile(EventHandler onCB)
{
 m_onWriteEEPROMFromFile = onCB;
}

void CInjDriverTabDlg::setOnReadEEPROMToFile(EventHandler onCB)
{
 m_onReadEEPROMToFile = onCB;
}

void CInjDriverTabDlg::setOnResetEEPROM(EventHandler onCB)
{
 m_onResetEEPROM = onCB;
}

void CInjDriverTabDlg::setOnFirmwareMaster(EventHandler onCB)
{
 m_onFirmwareMaster = onCB;
}

void CInjDriverTabDlg::setOnSelInjDrv(EventHandler onCB)
{
 m_onSelInjDrv = onCB;
}

void CInjDriverTabDlg::setOnImportFromStock(EventHandler onCB)
{
 m_onImportFromStock = onCB;
}

void CInjDriverTabDlg::SetValues(SECU3IO::InjDrvPar* ip_data, bool voltage_only /*= false*/)
{
 if (voltage_only)
 {
  CString str;
  str.Format("%5.2f", ip_data->voltage);
  m_voltage_pane.SetWindowText(str);
  m_gas_v_pane.SetWindowText(ip_data->gas_v ? _T("1") : _T("0"));
  SetChartVoltageValue(ip_data->voltage);
  mp_chart->Invalidate(TRUE);

  //update CRC panes
  if (ip_data->set0_corrupted != m_crc0_pane_f)
  {
   m_crc0_pane.SetWindowText(ip_data->set0_corrupted ? _T("NOK") : _T("OK"));
   m_crc0_pane_f = ip_data->set0_corrupted;
  }
  if (ip_data->set1_corrupted != m_crc1_pane_f)
  {
   m_crc1_pane.SetWindowText(ip_data->set1_corrupted ? _T("NOK") : _T("OK"));
   m_crc1_pane_f = ip_data->set1_corrupted;
  }
  return;
 }
 int set_idx = ip_data->set_idx;
 m_params[set_idx] = *ip_data; //copy

 CString str;
 str.Format("%5.2f", ip_data->voltage);
 m_voltage_pane.SetWindowText(str);
 m_gas_v_pane.SetWindowText(ip_data->gas_v ? _T("1") : _T("0"));
 SetChartVoltageValue(ip_data->voltage);

 //update CRC panes
 if (ip_data->set0_corrupted != m_crc0_pane_f)
 {
  m_crc0_pane.SetWindowText(ip_data->set0_corrupted ? _T("NOK") : _T("OK"));
  m_crc0_pane_f = ip_data->set0_corrupted;
 }
 if (ip_data->set1_corrupted != m_crc1_pane_f)
 {
  m_crc1_pane.SetWindowText(ip_data->set1_corrupted ? _T("NOK") : _T("OK"));
  m_crc1_pane_f = ip_data->set1_corrupted;
 }

 if (set_idx == m_set_of_sett_idx)
 {
  UpdateChartValues();
  mp_chart->Invalidate(TRUE);
  UpdateData(FALSE);
 }
}

void CInjDriverTabDlg::GetValues(SECU3IO::InjDrvPar* op_data)
{
 UpdateData(TRUE);
 int set_idx = op_data->set_idx;

 *op_data = m_params[set_idx]; //copy
}

void CInjDriverTabDlg::SetChartVoltageValue(float voltage)
{
 if (voltage < m_x_axis_min)
  voltage = m_x_axis_min;
 if (voltage > m_x_axis_max)
  voltage = m_x_axis_max;

 mp_chart->SetXYValue(SERIE_VT, voltage, m_y_axis_min, 0);
 mp_chart->SetXYValue(SERIE_VT, voltage, m_y_axis_max, 1);
}

void CInjDriverTabDlg::OnChangeData()
{
 UpdateData();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnChangeDataChecks()
{
 OnChangeData();
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::OnChangeDataTstPeakPwm()
{
 if (m_tst_peak_pwm_check.GetCheck()==BST_CHECKED)
  m_tst_hold_pwm_check.SetCheck(BST_UNCHECKED);

 UpdateData();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnChangeDataTstHoldPwm()
{
 if (m_tst_hold_pwm_check.GetCheck()==BST_CHECKED)
  m_tst_peak_pwm_check.SetCheck(BST_UNCHECKED);

 UpdateData();
 if (m_onChange)
  m_onChange();
}

void CInjDriverTabDlg::OnExportToAFile()
{
 if (m_onExportToAFile)
  m_onExportToAFile();
}

void CInjDriverTabDlg::OnImportFromAFile()
{
 if (m_onImportFromAFile)
  m_onImportFromAFile();
}

void CInjDriverTabDlg::OnImportFromStock()
{
 if (m_onImportFromStock)
  m_onImportFromStock();
}

void CInjDriverTabDlg::OnSaveToFirmware()
{
 if (m_onSaveToFirmware)
  m_onSaveToFirmware();
}

void CInjDriverTabDlg::OnLoadFromFirmware()
{
 if (m_onLoadFromFirmware)
  m_onLoadFromFirmware();
}

void CInjDriverTabDlg::OnShowFirmwareInfo()
{
 if (m_onShowFirmwareInfo)
  m_onShowFirmwareInfo();
}

void CInjDriverTabDlg::OnWriteEEPROMFromFile(void)
{
 if (m_onWriteEEPROMFromFile)
  m_onWriteEEPROMFromFile();
}

void CInjDriverTabDlg::OnReadEEPROMToFile(void)
{
 if (m_onReadEEPROMToFile)
  m_onReadEEPROMToFile();
}

void CInjDriverTabDlg::OnUpdateControlsPO(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_peak_on_check.GetCheck() == BST_UNCHECKED);
}

void CInjDriverTabDlg::OnUpdateControlsPD(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_peak_duty_check.GetCheck() == BST_UNCHECKED);
}

void CInjDriverTabDlg::OnUpdateControlsHD(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_hold_duty_check.GetCheck() == BST_UNCHECKED);
}

void CInjDriverTabDlg::OnUpdateControlsPF(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_peak_full_check.GetCheck() == BST_UNCHECKED);
}

void CInjDriverTabDlg::OnUpdateControlsPP(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_pth_pause_check.GetCheck() == BST_UNCHECKED);
}

void CInjDriverTabDlg::OnUpdateControlsEESave(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_enable_ee_save && m_initialized);
}

void CInjDriverTabDlg::OnUpdateControlsArrowBtns(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && mp_curr_curve);
}

void CInjDriverTabDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_initialized);
}

void CInjDriverTabDlg::OnUpdateControlsVoltage(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable && m_initialized && m_enable_voltage);
}

void CInjDriverTabDlg::OnUpdateOfflineModeExit(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_offline);
 pCmdUI->SetCheck(m_offline);
}

void CInjDriverTabDlg::OnUpdateBLItems(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_is_bl_items_available);
}

void CInjDriverTabDlg::EnableAll(bool enable)
{
 m_enable = enable;
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::EnableVoltage(bool enable)
{
 m_enable_voltage = enable;
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::EnableSaveBtn(bool enable)
{
 m_enable_ee_save = enable;
 UpdateDialogControls(this, TRUE);
}

int CInjDriverTabDlg::GetCurrSetIdx(void)
{
 return m_set_of_sett_idx;
}

void CInjDriverTabDlg::OnOfflineModeExit()
{
 m_offline = false;
 UpdateDialogControls(this, TRUE);
 if (m_onExitOfflineMode)
  m_onExitOfflineMode();
}

void CInjDriverTabDlg::SetOfflineCheck(void)
{
 m_offline = true;
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::OnLogoClick(void)
{
 ShellExecute(NULL, _T("open"), _T("http://secu3.org"), NULL, NULL, SW_SHOWNORMAL);
}

void CInjDriverTabDlg::OnWriteFirmwareFromFile(void)
{
 if (m_onWriteFirmwareFromFile)
  m_onWriteFirmwareFromFile();
}

void CInjDriverTabDlg::OnReadFirmwareToFile(void)
{
 if (m_onReadFirmwareToFile)
  m_onReadFirmwareToFile();
}

void CInjDriverTabDlg::OnReadLzblInfo(void)
{
 if (m_onReadLzblInfo)
  m_onReadLzblInfo();
}

void CInjDriverTabDlg::EnableBLItems(bool enable)
{
 m_is_bl_items_available = enable;
 UpdateDialogControls(this, TRUE);
}

void CInjDriverTabDlg::OnTipLinkClick(void)
{
 CInjDriverTipDlg tipDlg;
 tipDlg.DoModal();
}

void CInjDriverTabDlg::SetShowGraphLabels(bool show)
{
 mp_chart->SetSerieLabels(SERIE_PO, show, false);
 mp_chart->SetSerieLabels(SERIE_PD, show, false);
 mp_chart->SetSerieLabels(SERIE_HD, show, false);
 mp_chart->SetSerieLabels(SERIE_PF, show, false);
 mp_chart->SetSerieLabels(SERIE_PP, show, false);
 mp_chart->SetSerieLabels(SERIE_PM, show, false);
 mp_chart->SetSerieLabels(SERIE_PA, show, true); //<--redraw
}

void CInjDriverTabDlg::OnResetEEPROM()
{
 if (m_onResetEEPROM)
  m_onResetEEPROM();
}

int CInjDriverTabDlg::GetInjDrvSel(void)
{
 int sel = m_injdrv_combo.GetCurSel();
 ASSERT(sel != CB_ERR);
 return sel;
}

void CInjDriverTabDlg::OnFirmwareMaster()
{
 if (m_onFirmwareMaster)
  m_onFirmwareMaster();
}

void CInjDriverTabDlg::OnBLRecoverLinkClick()
{
 SECUMessageBox(IDS_INJDRV_BLRECOVER_TEXT, MB_OK | MB_ICONINFORMATION);
}

bool CInjDriverTabDlg::GetEditSimultanCheck(void)
{
 return (m_edit_simultan_check.GetCheck()==BST_CHECKED);
}
