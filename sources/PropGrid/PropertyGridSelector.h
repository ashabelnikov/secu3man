#pragma once
#include <string>
#include <vector>
#include "common\unicodesupport.h"

// CPropertyGridSelector frame

class CPropertyGridSelector : public CWnd
{
  DECLARE_DYNCREATE(CPropertyGridSelector)
  typedef CWnd Super;

 public:
  CPropertyGridSelector();
  virtual ~CPropertyGridSelector();

  BOOL Create(DWORD dwStyle, CRect& rc, CWnd* pParent, int nId);
  void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
  void SetColors(COLORREF clrBack, COLORREF clrText);

  void AddString(_TSTRING strItem);
  void SetCurSel(int nItem);

 protected:
  bool IsLeftBoundReached(void) const;
  bool IsRightBoundReached(void) const;

 protected:
  afx_msg void OnPaint();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnDestroy();
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg UINT OnGetDlgCode();
  DECLARE_MESSAGE_MAP()

  //Data
 private:
  bool m_lbutton_pushed;
  bool m_lbutton_depressed;
  bool m_rbutton_pushed;
  bool m_rbutton_depressed;

  CRect m_rc_lbutton;
  CRect m_rc_rbutton;
  CRect m_rcRect;

  std::vector<_TSTRING> m_Items;
  int m_nSelected;

  CFont* m_pFont;
  COLORREF m_clrBack;
  COLORREF m_clrText;
};


