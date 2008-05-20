
#pragma once

#include "ui-core/TabDialog.h"
#include "resource.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"
#include "io-core/SECU3IO.h"

#include <vector>
#include "ParamPageEvents.h"
#include "common/unicodesupport.h"

class CFunSetPageDlg : public CTabDialog, public ParamPageEvents
{
// Construction
public:
	CFunSetPageDlg(CWnd* pParent = NULL);   // standard constructor

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);

	void FillCBByFunNames(void);
	std::vector<_TSTRING>& AccessFunNames(void) { return m_fun_names;}

	void GetValues(SECU3IO::FunSetPar* o_values);
    void SetValues(const SECU3IO::FunSetPar* i_values);

// Dialog Data
	//{{AFX_DATA(CFunSetPageDlg)
	enum { IDD = IDD_PD_FUNSET_PAGE };
	CComboBox	m_gas_maps_combo;
	CComboBox	m_benzin_maps_combo;
	CSpinButtonCtrlEx	m_press_swing_spin;
	CEditEx	m_press_swing_edit;
	CSpinButtonCtrlEx	m_map_grad_spin;
	CEditEx	m_map_grad_edit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFunSetPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SECU3IO::FunSetPar m_params;
	std::vector<_TSTRING> m_fun_names;
	BOOL m_enabled;

	// Generated message map functions
	//{{AFX_MSG(CFunSetPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePdFunsetBenzinMapsCombo();
	afx_msg void OnSelchangePdFunsetGasMapsCombo();
	afx_msg void OnChangePdFunsetMapGradEdit();
	afx_msg void OnChangePdFunsetPressSwingEdit();

	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


