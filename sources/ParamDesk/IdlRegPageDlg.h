
#pragma once

#include "io-core\SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core\EditEx.h"
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\TabDialog.h"

class CIdlRegPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CIdlRegPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;
    
  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::IdlRegPar* o_values);
  void SetValues(const SECU3IO::IdlRegPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::IdlRegPar m_params;
  BOOL m_enabled;

  CButton m_use_regulator;
  CSpinButtonCtrlEx m_goal_rpm_spin;
  CEditEx m_goal_rpm_edit;
  CSpinButtonCtrlEx m_factor_pos_spin;
  CEditEx m_factor_pos_edit;
  CSpinButtonCtrlEx m_factor_neg_spin;
  CEditEx m_factor_neg_edit;
  CSpinButtonCtrlEx m_dead_band_rpm_spin;
  CEditEx m_dead_band_rpm_edit;
  CSpinButtonCtrlEx m_restriction_min_spin;
  CEditEx m_restriction_min_edit;
  CSpinButtonCtrlEx m_restriction_max_spin;
  CEditEx m_restriction_max_edit;
};
