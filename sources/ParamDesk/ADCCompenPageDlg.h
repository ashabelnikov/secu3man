
#pragma once

#include "io-core\SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core\EditEx.h"
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\TabDialog.h"

class CADCCompenPageDlg : public CTabDialog, public ParamPageEvents
{
 public:
  CADCCompenPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::ADCCompenPar* o_values);
  void SetValues(const SECU3IO::ADCCompenPar* i_values);
		
 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangePdMAPFactorEdit();
  afx_msg void OnChangePdMAPCorrectionEdit();
  afx_msg void OnChangePdUBATFactorEdit();
  afx_msg void OnChangePdUBATCorrectionEdit();
  afx_msg void OnChangePdTEMPFactorEdit();
  afx_msg void OnChangePdTEMPCorrectionEdit();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::ADCCompenPar m_params;
  BOOL m_enabled;

  CSpinButtonCtrlEx m_map_factor_spin;
  CSpinButtonCtrlEx m_map_correction_spin;
  CEditEx m_map_factor_edit;
  CEditEx m_map_correction_edit;

  CSpinButtonCtrlEx m_ubat_factor_spin;
  CSpinButtonCtrlEx m_ubat_correction_spin;
  CEditEx m_ubat_factor_edit;
  CEditEx m_ubat_correction_edit;

  CSpinButtonCtrlEx m_temp_factor_spin;
  CSpinButtonCtrlEx m_temp_correction_spin;
  CEditEx m_temp_factor_edit;
  CEditEx m_temp_correction_edit;
};
