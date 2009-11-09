
#pragma once

#include <vector>
#include "common\unicodesupport.h"
#include "io-core\SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core\EditEx.h"
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\TabDialog.h"

class CFunSetPageDlg : public CTabDialog, public ParamPageEvents
{
 public:
  CFunSetPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;
    
  void Enable(bool enable);
  bool IsEnabled(void);

  void FillCBByFunNames(void);
  std::vector<_TSTRING>& AccessFunNames(void);

  void GetValues(SECU3IO::FunSetPar* o_values);
  void SetValues(const SECU3IO::FunSetPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::FunSetPar m_params;
  std::vector<_TSTRING> m_fun_names;
  BOOL m_enabled;

  CComboBox m_gas_maps_combo;
  CComboBox m_benzin_maps_combo;
  CSpinButtonCtrlEx m_press_swing_spin;
  CEditEx m_press_swing_edit;
  CSpinButtonCtrlEx m_map_grad_spin;
  CEditEx m_map_grad_edit;
  CSpinButtonCtrlEx m_map_curve_offset_spin;
  CEditEx m_map_curve_offset_edit;
  CSpinButtonCtrlEx m_map_curve_gradient_spin;
  CEditEx m_map_curve_gradient_edit;
};
