
#pragma once

#include "io-core\SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core\EditEx.h"
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\TabDialog.h"

class CAnglesPageDlg : public CTabDialog, public ParamPageEvents
{
 public:
  CAnglesPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::AnglesPar* o_values);
  void SetValues(const SECU3IO::AnglesPar* i_values);

 // Implementation
 protected:	
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::AnglesPar m_params;
  BOOL m_enabled;

  CSpinButtonCtrlEx m_min_angle_spin;
  CSpinButtonCtrlEx m_max_angle_spin;
  CSpinButtonCtrlEx m_correction_spin;
  CSpinButtonCtrlEx m_decrease_spead_spin;
  CSpinButtonCtrlEx m_increase_spead_spin;
  CEditEx m_min_angle_edit;
  CEditEx m_max_angle_edit;
  CEditEx m_correction_edit;
  CEditEx m_decrease_spead_edit;
  CEditEx m_increase_spead_edit;
};
