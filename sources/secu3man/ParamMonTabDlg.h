
#pragma once

#include <memory>
#include "ui-core/TabDialog.h"
#include "common\FastDelegate.h"

class CMIDeskDlg;
class CRSDeskDlg;
class CParamDeskDlg;
class CHotKeysToCmdRouter;

// CParamMonModePageDlg dialog
class CParamMonTabDlg : public CTabDialog
{
  typedef fastdelegate::FastDelegate0<> EventHandler;
 public:
  CParamMonTabDlg(CWnd* pParent = NULL);   // standard constructor   
  enum { IDD = IDD_PARAMETERS_AND_MONITOR };
  virtual LPCTSTR GetDialogID(void) const;

  bool GetRawSensorsCheckState(void);
  void setOnRawSensorsCheck(EventHandler i_Function);

  std::auto_ptr<CMIDeskDlg> mp_MIDeskDlg;
  std::auto_ptr<CRSDeskDlg> mp_RSDeskDlg;
  std::auto_ptr<CParamDeskDlg> mp_ParamDeskDlg;

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnPmShowRawSensors();
  afx_msg void OnClose();
  afx_msg void OnDestroy();
  afx_msg void OnMaximizeWindow();
  afx_msg void OnMinimizeWindow();
  DECLARE_MESSAGE_MAP()

  EventHandler m_OnRawSensorsCheck;
  CButton m_raw_sensors_check;

  void _RegisterHotKeys(void);
  std::auto_ptr<CHotKeysToCmdRouter> m_hot_keys_supplier;
};

