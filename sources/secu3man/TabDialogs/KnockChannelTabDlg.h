
#pragma once

#include <memory>
#include <vector>
#include "common\fastdelegate.h"
#include "ui-core\TabDialog.h"

class CChartCtrl;
class CChartLineSerie;
class CChartPointsSerie;
class CKnockPageDlg;
class COScopeCtrl;

class CKnockChannelTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CKnockChannelTabDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  void setOnSaveParameters(EventHandler OnFunction);
  void setOnCopyToAttenuatorTable(EventHandler OnFunction);
  void setOnClearFunction(EventHandler OnFunction);

  void EnableAll(bool i_enable);

  //Добавляет новую выборку в осциллограф
  void AppendPoint(float value);

  //Установка значений функции для графика который показывает зависимость сигнала от оборотов
  void SetRPMKnockSignal(const std::vector<float> &i_values);

  void EnableCopyToAttenuatorTableButton(bool i_enable);
  void EnableClearFunctionButton(bool i_enable);

  void SetDesiredLevel(float i_level);
  float GetDesiredLevel(void);

  enum { RPM_KNOCK_SIGNAL_POINTS = 128 };

  std::auto_ptr<CKnockPageDlg> mp_knock_parameters_dlg;

 protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg void OnDestroy();
  afx_msg void OnSaveParameters();
  afx_msg void OnCopyToAttenuatorTable();
  afx_msg void OnClearFunction();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateCopyToAttenuatorTable(CCmdUI* pCmdUI);
  afx_msg void OnUpdateClearFunction(CCmdUI* pCmdUI);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

  void _InitializeOscilloscopeControl(void);
  void _InitializeRPMKnockSignalControl(void);

 private:
  CButton  m_param_save_button;
  CChartCtrl* mp_RTChart;
  CChartPointsSerie* m_pPointSerie;
  CChartLineSerie* m_pLineSerie;
  CChartLineSerie* m_pLineSerieLevel;
  std::auto_ptr<COScopeCtrl> mp_OScopeCtrl;
  CButton m_copy_to_attenuator_table_button;
  CButton m_clear_function_button;
  CSliderCtrl m_level_slider;

  EventHandler  m_OnSaveParameters; 
  EventHandler  m_OnCopyToAttenuatorTable;
  EventHandler  m_OnClearFunction;
  bool m_all_enabled;
  bool m_copy_to_attenuator_table_button_state;
  bool m_clear_function_button_state;
};
