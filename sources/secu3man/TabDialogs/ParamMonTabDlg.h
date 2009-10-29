
#pragma once

#include <memory>
#include "common\FastDelegate.h"
#include "ui-core\TabDialog.h"

class CMIDeskDlg;
class CParamDeskDlg;
class CRSDeskDlg;

// CParamMonModePageDlg dialog
class CParamMonTabDlg : public CTabDialog
{
  typedef fastdelegate::FastDelegate0<> EventHandler;
 public:
  CParamMonTabDlg(CWnd* pParent = NULL);   // standard constructor   
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  bool GetRawSensorsCheckState(void);
  void setOnRawSensorsCheck(EventHandler i_Function);

  std::auto_ptr<CMIDeskDlg> mp_MIDeskDlg;
  std::auto_ptr<CRSDeskDlg> mp_RSDeskDlg;
  std::auto_ptr<CParamDeskDlg> mp_ParamDeskDlg;

  void MakePDFloating(bool i_floating);
  void EnlargeMonitor(bool i_enlarge);

 // Implementation
 protected:
  void _ResizeRect(const CRect& i_external, CRect& io_victim);

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnPmShowRawSensors();
  afx_msg void OnClose();
  DECLARE_MESSAGE_MAP()

  EventHandler m_OnRawSensorsCheck;
  CButton m_raw_sensors_check;
  CStatic m_save_note_text;

  CRect m_original_mi_rect;
  CRect m_original_rs_rect;
  CPoint m_original_check_pos;
};
