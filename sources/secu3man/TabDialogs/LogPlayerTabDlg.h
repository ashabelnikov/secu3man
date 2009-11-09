
#pragma once

#include <memory>
#include <vector>
#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"

class CLPControlPanelDlg;
class CMIDeskDlg;
class COScopeCtrl;

class CLogPlayerTabDlg : public CTabDialog
{
 public:
  CLogPlayerTabDlg(CWnd* pParent = NULL);  
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  void EnlargeMonitor(bool i_enlarge);

  void AppendKnockValue(double i_value, bool i_reverse);
  void ResetKnockOscilloscope(void);

 public:
  std::auto_ptr<CMIDeskDlg> mp_MIDeskDlg;
  std::auto_ptr<CLPControlPanelDlg> mp_LPPanelDlg;

  //additional: for displaying of knock
  std::auto_ptr<COScopeCtrl> mp_OScopeCtrl;
  
 protected:
  void _ResizeRect(const CRect& i_external, CRect& io_victim);
  void _InitializeOscilloscopeControl(void);

  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  DECLARE_MESSAGE_MAP()

  CRect m_original_mi_rect;
};
