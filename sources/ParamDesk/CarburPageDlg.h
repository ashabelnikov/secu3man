
#pragma once

#include "io-core\SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core\EditEx.h"
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\TabDialog.h"

class CCarburPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CCarburPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::CarburPar* o_values);
  void SetValues(const SECU3IO::CarburPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::CarburPar m_params;
  BOOL m_enabled;

  CButton m_inverse_throttle_switch;
  CSpinButtonCtrlEx m_shutoff_lo_threshold_spin;
  CSpinButtonCtrlEx m_shutoff_hi_threshold_spin;
  CSpinButtonCtrlEx m_epm_on_threshold_spin;
  CSpinButtonCtrlEx m_shutoff_lo_threshold_spin_g;
  CSpinButtonCtrlEx m_shutoff_hi_threshold_spin_g;
  CSpinButtonCtrlEx m_shutoff_delay_spin;
  CEditEx m_shutoff_lo_threshold_edit;
  CEditEx m_shutoff_hi_threshold_edit;
  CEditEx m_epm_on_threshold_edit;
  CEditEx m_shutoff_lo_threshold_edit_g;
  CEditEx m_shutoff_hi_threshold_edit_g;
  CEditEx m_shutoff_delay_edit;
};
