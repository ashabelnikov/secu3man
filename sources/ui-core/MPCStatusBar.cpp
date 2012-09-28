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
#include "StdAfx.h"
#include <afxtempl.h>
#include "MPCStatusBar.h"

////////////////////////////////////////////////////////////////////////
// MPCStatusBarPaneControlInfo
IMPLEMENT_DYNCREATE(MPCStatusBarPaneControlInfo,CObject)
MPCStatusBarPaneControlInfo::MPCStatusBarPaneControlInfo()
{
 m_hWnd = NULL;
 m_bAutodelete=TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
// MPCStatusBarPane
MPCStatusBarPane::MPCStatusBarPane(MPCStatusBarPane & xfxpane )
{
 *this = xfxpane;
}
MPCStatusBarPane& MPCStatusBarPane::operator=(MPCStatusBarPane & xfxpane )
{
 nID  = xfxpane.nID;
 cxText = xfxpane.cxText;
 nStyle = xfxpane.nStyle;
 strText = xfxpane.strText;
 return *this;
}

MPCStatusBarPane::MPCStatusBarPane()
{
 //empty
}

////////////////////////////////////////////////////////////////////////
// MPCStatusBar
IMPLEMENT_DYNCREATE(MPCStatusBar,CStatusBar)
BEGIN_MESSAGE_MAP(MPCStatusBar, CStatusBar)
 ON_WM_SIZE()
END_MESSAGE_MAP()

MPCStatusBar::~MPCStatusBar()
{
 for( int i = 0; i < m_aPans.GetSize(); i++ )
 {
  if( m_aPans[i]->m_hWnd && ::IsWindow(m_aPans[i]->m_hWnd->m_hWnd) )
   m_aPans[i]->m_hWnd->CloseWindow();
  if( m_aPans[i]->m_bAutodelete )
  {
   delete m_aPans[i]->m_hWnd;
  }
  delete m_aPans[i];
 }
}

void MPCStatusBar::OnSize(UINT nType, int cx, int cy)
{
 CStatusBar::OnSize(nType, cx, cy);
 PositionControls();
}

void MPCStatusBar::PositionControls()
{
 int h,v,s;
 GetStatusBarCtrl( ).GetBorders( h, v, s ) ;


 for(int i = 0; i < m_aPans.GetSize(); i++ )
 {
  CRect rect;

  int index = CommandToIndex( m_aPans[i]->m_nPaneID );
  GetItemRect(index,rect);
  if( (GetPaneStyle(index) & SBPS_NOBORDERS) == SBPS_NOBORDERS)
   m_aPans[i]->m_hWnd->MoveWindow(rect);
  else
  {
   rect.top+=v;
   rect.bottom-=v;
   rect.left+=h+s;
   rect.right-=(h+s);
   m_aPans[i]->m_hWnd->MoveWindow(rect);
  }

 }
}


MPCStatusBarPaneControlInfo * MPCStatusBar::GetPanControl(int nPaneID)
{
 for(int i = 0; i < m_aPans.GetSize(); i++ )
 {
  if( m_aPans[i]->m_nPaneID == nPaneID )
   return m_aPans[i];
 }
 return NULL;
}


BOOL MPCStatusBar::AddControl(CWnd * pWnd, int paneID, BOOL bAutodeleteControl)
{

 MPCStatusBarPaneControlInfo * pPanInfo = GetPanControl(paneID);
 if( pPanInfo )
  return FALSE;

 int idx = CommandToIndex( paneID ) ;
 if( idx == -1 )
  return FALSE;

 MPCStatusBarPaneControlInfo * pPan = new MPCStatusBarPaneControlInfo;
 pPan->m_nPaneID =  paneID;
 pPan->m_hWnd    =  pWnd;
 pPan->m_bAutodelete = bAutodeleteControl;

 m_aPans.Add(pPan);
 PositionControls();
 Invalidate(TRUE);
 return TRUE;
}

void MPCStatusBar::SetPaneWidth(int index, int cxWidth)
{
 UINT nID,nStyle;
 int width;
 GetPaneInfo(index,nID,nStyle,width);
 SetPaneInfo(index,nID,nStyle,cxWidth);
}




BOOL MPCStatusBar::GetStatusPane(int nIndex, MPCStatusBarPane & xfxpane)
{
 if( nIndex < m_nCount  && nIndex >= 0 )
 {
  GetPaneInfo( nIndex,  xfxpane.nID, xfxpane.nStyle, xfxpane.cxText ) ;
  GetPaneText( nIndex , xfxpane.strText );
  return TRUE;
 }
 return FALSE;
}

BOOL MPCStatusBar::AddIndicator( int position, UINT paneID )
{
 CArray<MPCStatusBarPane,MPCStatusBarPane> arPanes;

 MPCStatusBarPane statusPane;
 int i;

 i = 0;
 while( GetStatusPane(i,statusPane) )
 {
  arPanes.Add( statusPane );
  i++;
 }
 if( position < 0 )
  position = 0;
 if( position > arPanes.GetSize() )
  position = arPanes.GetSize()-1;

 for(i = 0; i < arPanes.GetSize(); i ++ )
 {
  if( paneID == arPanes[i].nID )
  {
   TRACE(_T("XFXStatusBar::AddIndicator(): Pane ID already exists \n"));
   return FALSE;
  }
 }

 MPCStatusBarPane new_statusPane;
 new_statusPane.nID = paneID;
 if( arPanes.GetSize() )
  arPanes.InsertAt(position,new_statusPane);
 else
  arPanes.Add(new_statusPane);

 UINT * pIndicators = new UINT[arPanes.GetSize()];
 for(i=0;i<arPanes.GetSize();i++)
  pIndicators[i]=arPanes[i].nID;

 SetRedraw(FALSE);
 SetIndicators(pIndicators,arPanes.GetSize());

 // Replace infos of indicators ..
 for(i = 0; i < arPanes.GetSize(); i++ )
 {
  if( arPanes[i].nID != paneID )
  {
   SetPaneInfo(i,arPanes[i].nID,arPanes[i].nStyle,arPanes[i].cxText);
   SetPaneText(i,arPanes[i].strText);
  }
  else
  {
   SetPaneWidth(i,50);
   SetPaneText(i,arPanes[i].strText);
  }

 }
 delete pIndicators;
 SetRedraw(TRUE);
 PositionControls();
 Invalidate(TRUE);
 return TRUE;
}




void MPCStatusBar::RemovePane(int nPaneID)
{
 SetRedraw(FALSE);
 CWnd * pwnd = NULL;
  int i;
 for( i = 0; i < m_aPans.GetSize(); i++ )
 {
  if( m_aPans[i]->m_nPaneID == nPaneID )
  {
   if( m_aPans[i]->m_bAutodelete )
    pwnd = m_aPans[i]->m_hWnd;
   delete m_aPans[i];
   m_aPans.RemoveAt(i);
   break;
  }
 }
 if( pwnd )
 {
  pwnd->DestroyWindow();
  delete pwnd;
 }

 CArray<MPCStatusBarPane,MPCStatusBarPane> arPanes;
 MPCStatusBarPane statusPane;
 i = 0;
 while( GetStatusPane(i,statusPane) )
 {
  if( statusPane.nID != (UINT)nPaneID )
   arPanes.Add( statusPane );
  i++;
 }
 // Now remove the indicator
 UINT * pIndicators = new UINT[arPanes.GetSize()];
 for(i=0;i<arPanes.GetSize();i++)
  pIndicators[i]=arPanes[i].nID;
 SetIndicators(pIndicators,arPanes.GetSize());

 // Replace infos of indicators ..
 for(i = 0; i < arPanes.GetSize(); i++ )
 {
  SetPaneInfo(i,arPanes[i].nID,arPanes[i].nStyle,arPanes[i].cxText);
  SetPaneText(i,arPanes[i].strText);
 }
 delete pIndicators;
 SetRedraw(TRUE);
 PositionControls();
 Invalidate(TRUE);

}


// End of file ///////////////////////////////////////////////////////////////
