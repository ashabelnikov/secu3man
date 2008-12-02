// ListDynDialogEx.h: interface for the CListDynDialogEx class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "dyndialogex.h"

class CListDynDialogEx : public CDynDialogEx  
{
public:
  CListDynDialogEx(CWnd* pParent = NULL);
  virtual ~CListDynDialogEx();

  CListBox m_lstBox;

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CListDynDialogEx)
public:
  virtual int DoModal();
  //}}AFX_VIRTUAL

  // Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CListDynDialogEx)
  virtual BOOL OnInitDialog();
  afx_msg void OnDblclkList();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


