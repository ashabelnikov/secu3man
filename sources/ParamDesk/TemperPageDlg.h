
#pragma once

#include "io-core\SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core\EditEx.h"
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\TabDialog.h"

class CTemperPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CTemperPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::TemperPar* o_values);
  void SetValues(const SECU3IO::TemperPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangePdTemperVentOnThresholdEdit();
  afx_msg void OnChangePdTemperVentOffThresholdEdit();
  afx_msg void OnPdTemperUseTempSensor();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::TemperPar m_params;
  BOOL m_enabled;

  CButton m_use_temp_sensor;
  CSpinButtonCtrlEx m_vent_on_threshold_spin;
  CSpinButtonCtrlEx m_vent_off_threshold_spin;
  CEditEx m_vent_off_threshold_edit;
  CEditEx m_vent_on_threshold_edit;
};
