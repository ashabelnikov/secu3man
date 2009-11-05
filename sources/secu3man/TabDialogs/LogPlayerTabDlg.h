
#pragma once

#include <memory>
#include <vector>
#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"

class CLPControlPanelDlg;
class CMIDeskDlg;

class CLogPlayerTabDlg : public CTabDialog
{
 public:
  CLogPlayerTabDlg(CWnd* pParent = NULL);  
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  void EnlargeMonitor(bool i_enlarge);

 public:
  std::auto_ptr<CMIDeskDlg> mp_MIDeskDlg;
  std::auto_ptr<CLPControlPanelDlg> mp_LPPanelDlg;
  
 protected:
  void _ResizeRect(const CRect& i_external, CRect& io_victim);

  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  DECLARE_MESSAGE_MAP()

  CRect m_original_mi_rect;
};
