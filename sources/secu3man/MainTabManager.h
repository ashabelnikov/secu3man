
#pragma once

#include "ui-core/TabController.h"
#include "ChildView.h"
#include "MainTabController.h"

class CMainTabManager  
{
public:
	CTabController m_tab_control;
	CImageList* m_pImgList;

	CMainTabController m_MainTabController; 

    class CParamMonTabController* m_pParamMonTabController;
    class CFirmwareTabController* m_pFirmwareTabController;

    class CParamMonTabDlg* m_pParamMonTabDlg;
    class CFirmwareTabDlg* m_pFirmwareTabDlg;

	CChildView* m_pParent;
    
    bool Init(CChildView* i_pChildView);
	void OnAfterCreate(void);
	virtual bool OnClose(void);

	CMainTabManager();
	virtual ~CMainTabManager();

    

};

