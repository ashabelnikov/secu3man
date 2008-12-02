#pragma once

#define WM_PG_DATESELCHANGED WM_USER+489

// CPropertyGridMonthCalCtrl

class AFX_EXT_CLASS CPropertyGridMonthCalCtrl : public CMonthCalCtrl
{
  typedef CMonthCalCtrl Super;
  /*DECLARE_DYNAMIC(CPropertyGridMonthCalCtrl)*/

public:
  CPropertyGridMonthCalCtrl();
  virtual ~CPropertyGridMonthCalCtrl();

protected:
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnMcnSelect(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg UINT OnGetDlgCode();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


