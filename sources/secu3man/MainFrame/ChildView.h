
#pragma once

// CChildView window
class CChildView : public CWnd
{
 public:
  CChildView();
  virtual ~CChildView();

  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
      const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

 protected:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void PostNcDestroy();
  virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
  virtual void PreSubclassWindow();
 
  // Generated message map functions
 protected:
  afx_msg void OnPaint();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
