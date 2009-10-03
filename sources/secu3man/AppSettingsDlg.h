
#pragma once

#include "ui-core/editex.h"
#include "ui-core/spinbuttonctrlex.h"
#include "IAppSettingsDlg.h"
#include "common/unicodesupport.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"

class CAppSettingsDlg : public CDialog, public IAppSettingsDlg 
{
 public:
  CAppSettingsDlg(CWnd* pParent = NULL);   // standard constructor
  enum { IDD = IDD_APP_SETTINGS };

 //////////////////Interface//////////////////////////
  virtual void FillCtrlsWithAllowableBaudRates(std::vector<DWORD> i_AllowableBaudRates);
  virtual void FillCtrlsWithAllowablePorts(std::vector<_TSTRING> i_AllowablePorts);
  virtual void FillCtrlsWithAllowableCSVSepSymbols(std::vector<std::pair<_TSTRING, char> >  i_AllowableCSVSepSymbols);

  //"Set" methods (model => view data transfer)
  virtual void SetPortName(_TSTRING i_PortName);
  virtual void SetBaudRateApplication(DWORD i_app_baud);
  virtual void SetBaudRateBootloader(DWORD i_bl_baud);
  virtual void SetCSVSepSymbol(size_t i_index);
  virtual void SetMIDeskUpdatePeriod(int i_period);
   
  //"Get" methods (view => model data transfer)
  virtual _TSTRING GetPortName(void); 
  virtual DWORD GetBaudRateApplication(void);
  virtual DWORD GetBaudRateBootloader(void);
  virtual int GetMIDeskUpdatePeriod(void);
 
  //logs
  virtual void SetLogFilesFolder(const CString& i_folder);
  virtual CString GetLogFilesFolder(void) const;
  virtual void SetUseAppFolder(bool i_use);
  virtual bool GetUseAppFolder(void) const;
  virtual size_t GetCSVSepSymbol(void);

  virtual void setFunctionOnOk(EventHandler OnOk);
  virtual void setFunctionOnCancel(EventHandler OnCancel);
  virtual void setFunctionOnActivate(EventHandler OnActivate);

  virtual int ShowDialog(void);
 /////////////////////////////////////////////////////

 // Implementation
 protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  virtual void OnOK();
  afx_msg void OnAppSettingsLogfolderButton();
  afx_msg void OnAppSettingsLogfolderUseappfolder();
  DECLARE_MESSAGE_MAP()

 private:
  EventHandler m_OnOk;
  EventHandler m_OnCancel;
  EventHandler m_OnActivate;

  CComboBox	m_port_selection_combo;
  CComboBox	m_bl_baudrate_selection_combo;
  CComboBox	m_app_baudrate_selection_combo;
  CComboBox m_log_csv_sepsymbol_combo;
  CButton   m_log_files_folder_button;
  CButton   m_use_app_folder_button;
  CEdit     m_log_files_folder_edit;
  CEditEx   m_midesk_update_period_edit;
  CSpinButtonCtrlEx m_midesk_update_period_spin;
  int m_app_baudrate;
  int m_bl_baudrate;
  int m_port_number;
  CString m_log_files_folder;
  int m_use_app_folder;
  int m_log_csv_sepsymbol_index;
  int m_midesk_update_period;
};
