#pragma once

// CPropertyGridMonthCalCtrl

class AFX_EXT_CLASS CPropertyGridMonthCalCtrl : public CMonthCalCtrl
{
  typedef CMonthCalCtrl Super;

 public:
  CPropertyGridMonthCalCtrl();
  virtual ~CPropertyGridMonthCalCtrl();

 protected:
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnMcnSelect(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg UINT OnGetDlgCode();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  DECLARE_MESSAGE_MAP()
};
