
#pragma once

#include <string>
#include "io-core\SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core\EditEx.h"
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\TabDialog.h"

class CStarterPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CStarterPageDlg(CWnd* pParent = NULL);   // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::StartrPar* o_values);
  void SetValues(const SECU3IO::StartrPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangePdStarterOffRpmEdit();
  afx_msg void OnChangePdStarterSmapAbandonRpmEdit();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::StartrPar m_params;
  BOOL m_enabled;

  CSpinButtonCtrlEx m_smap_abandon_rpm_spin;
  CSpinButtonCtrlEx m_starter_off_rpm_spin;
  CEditEx m_smap_abandon_rpm_edit;
  CEditEx m_starter_off_rpm_edit;
};
