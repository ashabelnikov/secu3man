//
// MPCStatusBar.cpp
//
// (c) Pierre MELLINAND
//
// History
// --------
// v 1.0
//  June 1998:    Created classes with names starting with XFX...
// v 1.1
//  October 19, 1999  void MPCStatusBar::RemovePane(int nPaneID) added
//        BOOL MPCStatusBar::AddIndicator( int position, UINT paneID )
//         corrected bug (std text disappeared when AddIndicator called)
//        BOOL MPCStatusBar::GetStatusPane(int nIndex, MPCStatusBarPane & xfxpane)
//         GetPaneText( ) wasnt called
//        Renamed classes with prefix MPC..
//        Added parameter 'BOOL bAutodeleteControl' to method AddControl()
//
#ifndef MPCStatusBar_h_
#define MPCStatusBar_h_

#include <afxtempl.h>

///////////////////////////////////////////////////////////////////////////////////
// MPCStatusBarPaneControlInfo
class MPCStatusBarPaneControlInfo : public CObject
{
  DECLARE_DYNCREATE(MPCStatusBarPaneControlInfo)
 public:
  CWnd * m_hWnd;
  int  m_nPaneID;
  BOOL m_bAutodelete;
  MPCStatusBarPaneControlInfo();
};


///////////////////////////////////////////////////////////////////////////////////
// MPCStatusBarPaneControlInfo
class MPCStatusBarPane : public CObject
{
 public:
  UINT nID;
  int  cxText;
  UINT nStyle;
  UINT nFlags;
  CString strText;
  MPCStatusBarPane ( MPCStatusBarPane & xfxpane );
  MPCStatusBarPane& operator=(MPCStatusBarPane& xfxpane);
  MPCStatusBarPane();
};

///////////////////////////////////////////////////////////////////////////////////
// MPCStatusBar
class AFX_EXT_CLASS MPCStatusBar : public CStatusBar
{
  DECLARE_DYNCREATE(MPCStatusBar)
 private:
  CArray<MPCStatusBarPaneControlInfo*, MPCStatusBarPaneControlInfo*> m_aPans;

 public:
  void PositionControls();
  ~MPCStatusBar();
  MPCStatusBarPaneControlInfo * GetPanControl(int nPaneID);
  BOOL AddControl(CWnd * pWnd, int nPaneID, BOOL bAutodeleteControl = TRUE );
  void SetPaneWidth(int index, int cxWidth);
  BOOL AddIndicator( int position, UINT paneID);
  BOOL GetStatusPane(int nIndex, MPCStatusBarPane & xfxpane);
  void RemovePane(int nPaneID);

  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()
};

#endif