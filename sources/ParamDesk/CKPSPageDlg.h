
#pragma once

#include "ui-core/TabDialog.h"
#include "resource.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"
#include "io-core/SECU3IO.h"

#include <vector>
#include "ParamPageEvents.h"
#include "common/unicodesupport.h"

class CCKPSPageDlg : public CTabDialog, public ParamPageEvents
{
public:
	CCKPSPageDlg(CWnd* pParent = NULL);

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);
	
	void GetValues(SECU3IO::CKPSPar* o_values);
    void SetValues(const SECU3IO::CKPSPar* i_values);

	enum { IDD = IDD_PD_CKPS_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnSelchangePdCogsBTDCCombo();
	afx_msg void OnChangePdIgnitionCogsEdit();
	afx_msg void OnClickedPdPosFrontRadio();
	afx_msg void OnClickedPdNegFrontRadio();
	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

    void _FillCKPSTeethBTDCComboBox(void); 
	int  _GetCKPSTeethBTDCComboBoxSelection(void);
	void _SetCKPSTeethBTDCComboBoxSelection(int i_sel);

private:
	//<cog number, UI name>
	std::vector<std::pair<int, _TSTRING> > m_cogs_numbers;
	
	SECU3IO::CKPSPar m_params;	
	BOOL m_enabled;

	CStatic	m_ckps_front_groupbox;
	CButton m_ckps_posfront_radio;
	CButton m_ckps_negfront_radio;
	
	CStatic m_teeth_before_tdc_label;
	CComboBox m_teeth_before_tdc_combo;

	CSpinButtonCtrlEx m_ignition_cogs_spin;
	CEditEx	m_ignition_cogs_edit;
	CStatic m_ignition_cogs_label;
};
