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

#pragma once

#include <memory>
#include <vector>
#include "common/fastdelegate.h"
#include "ui-core/TabDialog.h"

class CChartCtrl;
class CChartLineSerie;
class CChartPointsSerie;
class CKnockContextMenuManager;
class CKnockFrqCalcDlg;
class CKnockPageDlg;
class COScopeCtrl;

class CKnockChannelTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<const std::vector<int>&> EventIndexes;

 public:
  CKnockChannelTabDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  void setOnSaveParameters(EventHandler OnFunction);
  void setOnCopyToAttenuatorTable(EventHandler OnFunction);
  void setOnClearFunction(EventHandler OnFunction);
  void setOnResetPoints(EventIndexes OnFunction);
  void setOnNeighbourMiddle(EventIndexes OnFunction);
  void setOnSigmaFilter(EventHandler OnFunction);
  void setOnLoadPoints(EventHandler OnFunction);
  void setOnSavePoints(EventHandler OnFunction);

  void EnableAll(bool i_enable);

  //Добавляет новую выборку в осциллограф
  void AppendPoint(float value);

  //Установка значений функции для графика который показывает зависимость сигнала от оборотов
  void SetRPMKnockSignal(const std::vector<float> &i_values);

  void EnableCopyToAttenuatorTableButton(bool i_enable);
  void EnableClearFunctionButton(bool i_enable);

  void SetDesiredLevel(float i_level);
  float GetDesiredLevel(void);
  bool GetDLSMCheckboxState(void) const;
  void SetDesiredLevelColor(bool color_ok);

  //установка значения оборотов, отображаемых на графике в виде вертикальной линии
  void SetRPMValue(int rpm);
  //установка видимости вертикальной линии, отображающей обороты на графике
  void SetRPMVisibility(bool visible);

  enum { RPM_KNOCK_SIGNAL_POINTS = 128 };
  enum { RPM_AXIS_MIN = 200, RPM_AXIS_STEP = 60 };

  std::auto_ptr<CKnockPageDlg> mp_knock_parameters_dlg;

 protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg void OnDestroy();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnSaveParameters();
  afx_msg void OnCopyToAttenuatorTable();
  afx_msg void OnClearFunction();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateCopyToAttenuatorTable(CCmdUI* pCmdUI);
  afx_msg void OnUpdateClearFunction(CCmdUI* pCmdUI);
  afx_msg void OnUpdateListResetPoints(CCmdUI* pCmdUI);
  afx_msg void OnUpdateListNeighbourMiddle(CCmdUI* pCmdUI);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDLSMCheckbox();
  afx_msg void OnListCheckbox();
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnListResetPoints();
  afx_msg void OnListNeighbourMiddle();
  afx_msg void OnListSigmaFilter();
  afx_msg void OnListLoadPoints();
  afx_msg void OnListSavePoints();
  DECLARE_MESSAGE_MAP()

  void _InitializeOscilloscopeControl(void);
  void _InitializeRPMKnockSignalControl(void);
  void _InitializeRPMKnockSignalList(void);

  void OnFrqCalculate(float frq);

 private:
  CButton  m_param_save_button;
  CChartCtrl* mp_RTChart;
  CChartPointsSerie* m_pPointSerie;
  CChartLineSerie* m_pLineSerie;
  CChartLineSerie* m_pLineSerieLevel;
  CChartLineSerie* m_pLineSerieRPM;
  CListCtrl m_RTList;
  std::auto_ptr<COScopeCtrl> mp_OScopeCtrl;
  std::auto_ptr<CKnockFrqCalcDlg> mp_knock_frq_calc_dlg;
  std::auto_ptr<CKnockContextMenuManager> mp_ContextMenuManager;
  CButton m_copy_to_attenuator_table_button;
  CButton m_clear_function_button;
  CButton m_dlsm_checkbox;
  CButton m_list_checkbox;
  CSliderCtrl m_level_slider;
  CStatic m_level_text;

  EventHandler  m_OnSaveParameters;
  EventHandler  m_OnCopyToAttenuatorTable;
  EventHandler  m_OnClearFunction;
  EventIndexes  m_OnResetPoints;
  EventIndexes  m_OnNeighbourMiddle;
  EventHandler  m_OnSigmaFilter;
  EventHandler  m_OnLoadPoints;
  EventHandler  m_OnSavePoints;
  bool m_all_enabled;
  bool m_copy_to_attenuator_table_button_state;
  bool m_clear_function_button_state;
  bool m_dlsm_checkbox_state;
};
