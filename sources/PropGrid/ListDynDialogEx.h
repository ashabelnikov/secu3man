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

 public:
  virtual int DoModal();

  // Implementation
 protected:
  virtual BOOL OnInitDialog();
  afx_msg void OnDblclkList();
  DECLARE_MESSAGE_MAP()
};
