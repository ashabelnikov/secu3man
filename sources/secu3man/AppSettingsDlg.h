
#pragma once

#include "ui-core/editex.h"
#include "ui-core/spinbuttonctrlex.h"
#include "IAppSettingsDlg.h"
#include "common/unicodesupport.h"

class CAppSettingsDlg : public CDialog, public IAppSettingsDlg 
{
//////////////////Interface//////////////////////////
public:

   virtual void FillCtrlsWithAllowableBaudRates(std::vector<DWORD> i_AllowableBaudRates)
   {
	 for(size_t i = 0; i < i_AllowableBaudRates.size(); i++)
	 {
       CString str;
	   str.Format(_T("%d"),i_AllowableBaudRates[i]);
	   m_bl_baudrate_selection_combo.AddString(str);
	   m_app_baudrate_selection_combo.AddString(str);
	 } 
   }
   
   virtual void FillCtrlsWithAllowablePorts(std::vector<_TSTRING> i_AllowablePorts)
   {
	 for(size_t i = 0; i < i_AllowablePorts.size(); i++)
	 {
       m_port_selection_combo.AddString(i_AllowablePorts[i].c_str());
	 }
   }

   virtual void FillCtrlsWithAllowableCSVSepSymbols(std::vector<std::pair<_TSTRING, char> >  i_AllowableCSVSepSymbols)
   {
	 for(size_t i = 0; i < i_AllowableCSVSepSymbols.size(); i++)
	 {
	  int index = m_log_csv_sepsymbol_combo.AddString(i_AllowableCSVSepSymbols[i].first.c_str());
	  ASSERT(index != LB_ERR);
      m_log_csv_sepsymbol_combo.SetItemData(index, i); 
	 }
   }


   //"Set" methods (model => view data transfer)
   virtual void SetPortName(_TSTRING i_PortName)
   {
     int result = m_port_selection_combo.FindString(-1,i_PortName.c_str());
	 if (result!=LB_ERR)
	   //m_port_selection_combo.SetCurSel(result);
	   m_port_number = result;
   }

   virtual void SetBaudRateApplication(DWORD i_app_baud) 
   {
     CString str;
	 str.Format(_T("%d"),i_app_baud);
     int result = m_app_baudrate_selection_combo.FindString(-1,str);
	 if (result!=LB_ERR)
	   // m_app_baudrate_selection_combo.SetCurSel(result);
	   m_app_baudrate = result;
   }

   virtual void SetBaudRateBootloader(DWORD i_bl_baud) 
   {
     CString str;
	 str.Format(_T("%d"),i_bl_baud);
     int result = m_bl_baudrate_selection_combo.FindString(-1,str);
	 if (result!=LB_ERR)
	   //m_bl_baudrate_selection_combo.SetCurSel(result); 
	   m_bl_baudrate = result;
   }

   virtual void SetCSVSepSymbol(size_t i_index)
   {
     int count = m_log_csv_sepsymbol_combo.GetCount();
	 for (int i = 0; i < count; i++)
	 {
	  size_t index = m_log_csv_sepsymbol_combo.GetItemData(i);
	  if (index == i_index)
	  { //found!
       m_log_csv_sepsymbol_index = i; 
	   return;
	  }
	 }
	 ASSERT(0); //WTF...
   }
   
   //"Get" methods (view => model data transfer)
   virtual _TSTRING GetPortName(void) 
   {
     CString str;
     m_port_selection_combo.GetLBText(m_port_number,str);
	 return _TSTRING(str);
   }

   virtual DWORD GetBaudRateApplication(void) 
   {
     CString str;
	 m_app_baudrate_selection_combo.GetLBText(m_app_baudrate,str);
     return _ttoi(str);
   }

   virtual DWORD GetBaudRateBootloader(void) 
   {
     CString str;
	 m_bl_baudrate_selection_combo.GetLBText(m_bl_baudrate,str);
     return _ttoi(str);  
   }
   
   virtual void SetLogFilesFolder(const CString& i_folder)
   {
    m_log_files_folder = i_folder;
   }

   virtual CString GetLogFilesFolder(void) const
   {
    return m_log_files_folder; 
   }

   virtual void SetUseAppFolder(bool i_use)
   {
    m_use_app_folder = i_use;
   }

   virtual bool GetUseAppFolder(void) const
   {
	return m_use_app_folder ? true : false;
   }

   virtual size_t GetCSVSepSymbol(void) 
   {
	 return m_log_csv_sepsymbol_combo.GetItemData(m_log_csv_sepsymbol_index); 
   }

   virtual void setFunctionOnOk(EventHandler OnOk)
   {
     m_OnOk = OnOk;
   }

   virtual void setFunctionOnCancel(EventHandler OnCancel)
   {
     m_OnCancel = OnCancel;
   }

   virtual void setFunctionOnActivate(EventHandler OnActivate)
   {
     m_OnActivate = OnActivate;
   }

   virtual int ShowDialog(void)
   {
     return DoModal();   
   }

/////////////////////////////////////////////////////

EventHandler m_OnOk;
EventHandler m_OnCancel;
EventHandler m_OnActivate;

// Construction
public:
	CAppSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAppSettingsDlg)
	enum { IDD = IDD_APP_SETTINGS };
	CComboBox	m_port_selection_combo;
	CComboBox	m_bl_baudrate_selection_combo;
	CComboBox	m_app_baudrate_selection_combo;
	CComboBox   m_log_csv_sepsymbol_combo;
	CEdit       m_log_files_folder_edit;
	CButton     m_log_files_folder_button;
	CButton     m_use_app_folder_button;
	int		m_app_baudrate;
	int		m_bl_baudrate;
	int		m_port_number;
    CString m_log_files_folder;
	int     m_use_app_folder;
	int     m_log_csv_sepsymbol_index;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAppSettingsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnAppSettingsLogfolderButton();
	afx_msg void OnAppSettingsLogfolderUseappfolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

