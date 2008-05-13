
#include "ui-core/TabDialog.h"
#include "ParamDesk/ParamDeskDlg.h"
#include "FirmwareContextMenuManager.h"
#include "common/FastDelegate.h"

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg dialog

class CFirmwareTabDlg : public CTabDialog
{
public:
   typedef fastdelegate::FastDelegate0<> EventHandler;


// Construction
public:
	CFirmwareTabDlg(CWnd* pParent = NULL);   // standard constructor
	
public:
	CParamDeskDlg m_ParamDeskDlg;
	CFirmwareModeContextMenuManager m_ContextMenuManager;

	bool IsBLStartedEmergency(void);
    void SetBLStartedEmergency(bool state)
	{ 
	  m_bl_started_emergency.SetCheck(state ? BST_CHECKED : BST_UNCHECKED);
	};

	void EnableBLStartedEmergency(bool enable);
	bool IsBLStartedEmergencyEnabled(void) {return m_is_bl_started_emergency_available;};

	void EnableBLItems(bool enable);
	bool IsBLItemsEnabled(void) {return m_is_bl_items_available;};


// Dialog Data
	//{{AFX_DATA(CFirmwareTabDlg)
	enum { IDD = IDD_FIRMWARE_SUPPORT };
	CButton	m_bl_started_emergency;
	CButton	m_view_work_map_btn;
	CButton	m_view_temp_map_btn;
	CButton	m_view_start_map_btn;
	CButton	m_view_idle_map_btn;
	//}}AFX_DATA


    int m_work_map_chart_state;
    int m_temp_map_chart_state;
    int m_start_map_chart_state;
    int m_idle_map_chart_state;

	virtual LPCTSTR GetDialogID(void) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFirmwareTabDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFirmwareTabDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdatePopupMenu_bl(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupMenu_file(CCmdUI* pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnFirmwareSupportViewStartMap();
	afx_msg void OnFirmwareSupportViewIdleMap();
	afx_msg void OnFirmwareSupportViewWorkMap();
	afx_msg void OnFirmwareSupportViewTempMap();
	afx_msg void OnUpdateFirmwareSupportViewStartMap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFirmwareSupportViewIdleMap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFirmwareSupportViewWorkMap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFirmwareSupportViewTempMap(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
    afx_msg void OnBootLoaderInfo();
	afx_msg void OnReadEepromToFile();
    afx_msg void OnWriteEepromFromFile();
    afx_msg void OnReadFlashToFile();
    afx_msg void OnWriteFlashFromFile();
	afx_msg void OnUpdateBLStartedEmergency(CCmdUI* pCmdUI);
	afx_msg void OnFirmwareSupportBlStartedEmergency();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public: //события от меню
    void setOnBootLoaderInfo(EventHandler OnFunction) {m_OnBootLoaderInfo = OnFunction;};
    void setOnReadEepromToFile(EventHandler OnFunction) {m_OnReadEepromToFile = OnFunction;};
    void setOnWriteEepromFromFile(EventHandler OnFunction) {m_OnWriteEepromFromFile = OnFunction;};
    void setOnReadFlashToFile(EventHandler OnFunction) {m_OnReadFlashToFile = OnFunction;};
    void setOnWriteFlashFromFile(EventHandler OnFunction) {m_OnWriteFlashFromFile = OnFunction;};

public: //события от кнопок и чекбоксов
    void setOnBLStartedEmergency(EventHandler OnFunction) {m_OnBLStartedEmergency = OnFunction;}

private:
    EventHandler m_OnBootLoaderInfo;
    EventHandler m_OnReadEepromToFile;
    EventHandler m_OnWriteEepromFromFile;
    EventHandler m_OnReadFlashToFile;
    EventHandler m_OnWriteFlashFromFile;
	EventHandler m_OnBLStartedEmergency;

private:
	bool m_is_bl_started_emergency_available;
	bool m_is_bl_items_available;

};

