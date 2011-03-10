//
// MPCLogoPane.h
//
// (c) Pierre MELLINAND
//
// History
// --------
// v 1.0
//  June 1998:    Created.
// v 1.01
//  October 19, 1999  Added History
//


#ifndef MPCLogoPane_h_
#define MPCLogoPane_h_

///////////////////////////////////////////////////////////////////////////////////
// MPCLogoPane
class AFX_EXT_CLASS MPCLogoPane : public CWnd
{
  DECLARE_DYNCREATE(MPCLogoPane)
 public:
  CString m_LogoText;
  CFont  m_fontLogo;

  MPCLogoPane();
  virtual BOOL Create(CString sText,DWORD dwStyle, CWnd * pParent,UINT nID);

  void SetLogoText(CString sTxt);
  void SetLogoFont(CString Name, int nHeight = 18, int nWeight = FW_BOLD,
      BYTE bItalic = TRUE, BYTE bUnderline = FALSE);

  afx_msg void OnPaint();
  DECLARE_MESSAGE_MAP()
};

#endif
