
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTabController window

class CTabDialog;

#include <vector>
#include "ITabControllerEvent.h"



// используйте стиль TCS_OWNERDRAWFIXED!

class AFX_EXT_CLASS CTabController : public CTabCtrl
{
   DECLARE_DYNAMIC(CTabController)

	class TabPageData
	{		
	public:
	 TabPageData() : pDialogTemplate(NULL),pDialogClass(NULL) {};        
	 DLGTEMPLATE* pDialogTemplate;
     CTabDialog*    pDialogClass;	
	 bool is_enabled;  //определяет текущее состояние Item-a
	};


	int m_tab_item_index;
    CTabDialog*  m_CurDlg;
	BOOL m_msg_reflect;
	DWORD m_style;
	int   m_tcmn;
	HMODULE m_hResourceModule;

	ITabControllerEvent* m_pEventHandler;

//Operations
private:
    TabPageData* GetItemData(int item) const;
    void CreateTabPage();
    void DestroyTabPage();
	void CalculatePageRect(int nItem,CRect& o_rect);

	 
public:
	void SetResourceModule(HMODULE hModule);
	void SetMsgReflection(BOOL reflect);
	CTabController();
	virtual ~CTabController();
    bool Create(CWnd* pParentWnd,const CRect& rect,UINT nID,const bool);
    int AddPage(CString name,CTabDialog* pPageDlg);
    int AddPage(CString name,CTabDialog* pPageDlg,const int nImage);
	void SetStyle(const DWORD style);
    bool SetCurSel(UINT iNewTab);            // set tab (fail if disabled)

	void EnableItem(int ITab, bool enable);
	void Init();

	void SetEventListener(ITabControllerEvent* i_listener) 
	{
	  ASSERT(i_listener); m_pEventHandler = i_listener;
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabController)
	afx_msg BOOL OnSelchangeTabctl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSelchangingTabctl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
   virtual  BOOL IsTabEnabled(int iTab); 
   int      NextEnabledTab(int iTab, BOOL bWrap);  // get next enabled tab
   int      PrevEnabledTab(int iTab, BOOL bWrap);  // get prev enabled tab


   // MFC overrides
   virtual  BOOL PreTranslateMessage(MSG* pMsg);
   virtual  void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

};

/////////////////////////////////////////////////////////////////////////////
