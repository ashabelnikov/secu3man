
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

   virtual void SetMAPCurveSlope(float i_slope) 
   {
     m_map_slopping = i_slope;
   }

   virtual void SetMAPCurveOffset(float i_offset) 
   {
     m_map_offset = i_offset;
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

   virtual float GetMAPCurveSlope(void) 
   {
     return m_map_slopping;
   }

   virtual float GetMAPCurveOffset(void) 
   {
     return m_map_offset;
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
	CSpinButtonCtrlEx	m_map_slopping_spin;
	CEditEx	m_map_slopping_edit;
	CSpinButtonCtrlEx	m_map_offset_spin;
	CEditEx	m_map_offset_edit;
	CComboBox	m_bl_baudrate_selection_combo;
	CComboBox	m_app_baudrate_selection_combo;
	float	m_map_offset;
	float	m_map_slopping;
	int		m_app_baudrate;
	int		m_bl_baudrate;
	int		m_port_number;
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

