 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "TabController.h"
#include "TabDialog.h"
#include "ITabControllerEvent.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTabController, CTabCtrl)


/////////////////////////////////////////////////////////////////////////////
// CTabController

CTabController::CTabController()
: m_tab_item_index(0)
, m_CurDlg(NULL)
, m_msg_reflect(TRUE)
, m_style(WS_VISIBLE | WS_CHILD)
, m_tcmn(3) //magic number
, m_pEventHandler(NULL)
, m_hResourceModule(NULL)
{
  m_hResourceModule = AfxGetInstanceHandle();
}

CTabController::~CTabController()
{ 
}

CTabController::TabPageData* CTabController::GetItemData(int item) const
{
  TCITEM tci;
  tci.mask = TCIF_PARAM;
  GetItem(item,&tci);    
  return reinterpret_cast<TabPageData*>(tci.lParam);	
}

void CTabController::CalculatePageRect(int nItem, CRect& o_rect)
{
  DWORD style = CTabCtrl::GetStyle();

  RECT c_rc;
  RECT i_rc;

  //необходимо получить размер для вкладки
  GetClientRect(&c_rc); 
  GetItemRect(nItem,&i_rc);

  if ((style & TCS_BOTTOM) && !(style & TCS_VERTICAL)) //bottom
  {
   c_rc.bottom-= (i_rc.bottom - i_rc.top) + m_tcmn;        
   c_rc.top+=m_tcmn;
   c_rc.right-=m_tcmn;
   c_rc.left+=m_tcmn;
  }
  if (!(style & TCS_BOTTOM) && (style & TCS_VERTICAL)) //left
  {  
   c_rc.left+= (i_rc.right - i_rc.left) + m_tcmn;    
   c_rc.top+=m_tcmn;
   c_rc.bottom-=m_tcmn;
   c_rc.right-=m_tcmn;
  }
  if ((style & TCS_BOTTOM) && (style & TCS_VERTICAL)) //right
  {
   c_rc.right-= (i_rc.right - i_rc.left) + m_tcmn;  
   c_rc.top+=m_tcmn;
   c_rc.left+=m_tcmn;
   c_rc.bottom-=m_tcmn;
  }
  if (!(style & TCS_BOTTOM) && !(style & TCS_VERTICAL))//top
  {
   c_rc.top+= (i_rc.bottom - i_rc.top) + m_tcmn;    
   c_rc.left+=m_tcmn;
   c_rc.bottom-=m_tcmn;
   c_rc.right-=m_tcmn;
  }
  o_rect = c_rc;
}


void CTabController::CreateTabPage()
{
  int selection = GetCurSel(); 
 
  //создание окна вкладки и ее отображение
  TabPageData* pPageData = GetItemData(selection);
  if (::IsWindow(pPageData->pDialogClass->m_hWnd))
	return; //предотвращаем повторное создание вкладки

  ASSERT(pPageData);
  BOOL result = pPageData->pDialogClass->CreateIndirect(pPageData->pDialogTemplate,(CWnd*)this);	  

  if (0==result)
  {
    AfxMessageBox(_T("Error creating Tab control page dialog!"));
	return;
  }
 
  CRect c_rc(0,0,0,0);
 
  CalculatePageRect(selection,c_rc);

  pPageData->pDialogClass->MoveWindow(&c_rc);
  //подогнали размер диалога
  pPageData->pDialogClass->ShowWindow(SW_SHOWNORMAL);

  m_CurDlg = pPageData->pDialogClass;
}


void CTabController::DestroyTabPage()
{  
  if (m_CurDlg) 
  {
    if (IsWindow(m_CurDlg->m_hWnd)) //только если окно было создано (предотвращаем повторное закрытие окна)
      m_CurDlg->DestroyWindow();
  }
}


bool CTabController::Create(CWnd* pParentWnd,const CRect& rect,UINT nID,const bool i_msg_reflect)
{
  if (FALSE == CTabCtrl::Create(m_style,rect ,pParentWnd,nID))
    return false;

  m_msg_reflect = (i_msg_reflect) ? FALSE : TRUE; //отражать сообщения родительскому окну или нет?
  return true;
}

void CTabController::SetStyle(const DWORD style)
{
	m_style = style; 
}

//добавление вкладки, !!! ВНИЗУ смотреть похожую функцию !!!
void CTabController::AddPage(CString name,CTabDialog* pPageDlg)
{
  TabPageData* pPageData = new TabPageData;
  pPageData->pDialogClass = pPageDlg;
  
  pPageData->is_enabled = true; //by default - item has been enabled

  //создание шаблона диалога
  HRSRC hrsrc  = FindResource(m_hResourceModule,pPageDlg->GetDialogID(), RT_DIALOG);
  if (NULL==hrsrc)
  {
   AfxMessageBox(_T("Resource not found!")); 
   return;
  }

  HGLOBAL hglb = LoadResource(m_hResourceModule, hrsrc);
  if (NULL==hglb)
  {
   return;
  } 
  pPageData->pDialogTemplate = (DLGTEMPLATE*)LockResource(hglb); 
 
  //добавление непосредственно вкладки
  InsertItem(TCIF_TEXT|TCIF_PARAM,m_tab_item_index,name,0,(LPARAM)pPageData);
   
  if (0 == m_tab_item_index)
  { //выбираем первую вкладку
   CreateTabPage();
  }

  m_tab_item_index++;
}

//!!! ВВЕРХУ смотреть похожую функцию !!!
void CTabController::AddPage(CString name,CTabDialog* pPageDlg,const int nImage)
{
  TabPageData* pPageData = new TabPageData;
  pPageData->pDialogClass = pPageDlg;
  
  pPageData->is_enabled = true; //by default - item has been enabled

  //создание шаблона диалога
  HRSRC hrsrc  = FindResource(m_hResourceModule,pPageDlg->GetDialogID(), RT_DIALOG); 
  if (NULL==hrsrc)
  {
   AfxMessageBox(_T("Resource not found!")); 
   return;
  }

  HGLOBAL hglb = LoadResource(m_hResourceModule, hrsrc); 
  if (NULL==hglb)
  {
   return;
  } 

  pPageData->pDialogTemplate = (DLGTEMPLATE*)LockResource(hglb); 

  //добавление непосредственно вкладки
  InsertItem(TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE,m_tab_item_index,name,nImage,(LPARAM)pPageData);
   
  if (0 == m_tab_item_index)
  {   	  
   //выбираем первую вкладку
   CreateTabPage();
  }

  m_tab_item_index++;
}


BOOL CTabController::OnSelchangeTabctl(NMHDR* pNMHDR, LRESULT* pResult) 
{
  //отображение выбранной вкладки вкладки
  CreateTabPage();

  if (m_pEventHandler)
	m_pEventHandler->OnSelchangeTabctl();

  *pResult = 0;
  return m_msg_reflect;  
}

BOOL CTabController::OnSelchangingTabctl(NMHDR* pNMHDR, LRESULT* pResult) 
{
  // Figure out index of new tab we are about to go to, as opposed
  // to the current one we're at. Believe it or not, Windows doesn't
  // pass this info
  //
  TC_HITTESTINFO htinfo;
  GetCursorPos(&htinfo.pt);
  ScreenToClient(&htinfo.pt);
  int iNewTab = HitTest(&htinfo);

  if (iNewTab >= 0 && !IsTabEnabled(iNewTab))   
    *pResult = TRUE; // tab disabled: prevent selection
  else
  {
    if (m_pEventHandler)	
      m_pEventHandler->OnSelchangingTabctl();
	
    //удаление предыдущей вкладки
    DestroyTabPage();
    *pResult = 0;
  }
  return m_msg_reflect;
}


BEGIN_MESSAGE_MAP(CTabController, CTabCtrl)
	//{{AFX_MSG_MAP(CTabController)
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchangeTabctl)
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGING, OnSelchangingTabctl)
    ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabController message handlers

//при изменении размера CTabCtrl необходимо подгонять размеры текущей вкладки
void CTabController::OnSize( UINT nType, int cx, int cy )
{
  CRect rect(0,0,0,0);

  int index = GetCurSel();

  if (index==-1)
    return; //не выбрано ниодной вкладки

  TabPageData* pItemData = NULL;
  pItemData = GetItemData(index);
  if (NULL == pItemData) 
    return;

  CalculatePageRect(index, rect);
  pItemData->pDialogClass->MoveWindow(rect);		

  //позволяем родительскому классу тоже изменить размер
  CTabCtrl::OnSize(nType, cx, cy);
}

void CTabController::SetMsgReflection(BOOL reflect)
{
  m_msg_reflect = reflect;
}

void CTabController::SetResourceModule(HMODULE hModule)
{
  m_hResourceModule = hModule;
}


void CTabController::OnDestroy() 
{
 int item_count = CTabCtrl::GetItemCount();

 //удаляем объекты ассоциированные с вкладками Tab-контролом
  for(int i = 0; i < item_count; ++i)
  {  
    TabPageData* pItemData = NULL;
	pItemData = GetItemData(i);
	if (NULL == pItemData) 
	  continue;

	delete pItemData;		
  }  

  CTabCtrl::OnDestroy();		
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTabController::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
  CRect rect = lpDrawItemStruct->rcItem;
 
  rect.DeflateRect(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE), ::GetSystemMetrics(SM_CXEDGE), 0);

  int tab_index = lpDrawItemStruct->itemID;

  if(tab_index < 0)
    return;  //---------------------------

  bool selected = (tab_index == GetCurSel());

  COLORREF color_normal	= GetSysColor(COLOR_BTNTEXT);
  COLORREF color_disabled = GetSysColor(COLOR_GRAYTEXT);

  TCHAR label[64];
  TC_ITEM item;
  item.mask = TCIF_TEXT |TCIF_IMAGE;
  item.pszText = label;
  item.cchTextMax = 63;  

  if(!GetItem(tab_index,&item))
    return;  //---------------------------

  CDC* p_dc = CDC::FromHandle(lpDrawItemStruct->hDC);
  if(!p_dc)
    return;  //---------------------------

  int save_dc = p_dc->SaveDC();

  p_dc->SetBkMode(TRANSPARENT);
  p_dc->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));


  // draw the image
  CImageList* p_imagelist = GetImageList();
  if(p_imagelist && item.iImage >= 0)
  {
    rect.left += p_dc->GetTextExtent(_T(" ")).cx;

    IMAGEINFO info;
    p_imagelist->GetImageInfo(item.iImage,&info);
    CRect image_rect(info.rcImage);
    int y_pos = rect.top;

	//рисуем картинку в зависимости от текущего состояния
	if (IsTabEnabled(tab_index))
      p_imagelist->Draw(p_dc, item.iImage, CPoint(rect.left,y_pos),ILD_TRANSPARENT);
    else
	{	
	  DrawState(p_dc->m_hDC,NULL,NULL,(LPARAM)p_imagelist->ExtractIcon(item.iImage),
	    0,rect.left,y_pos,image_rect.Width(),image_rect.Height(),DST_ICON|DSS_DISABLED);
	}

    rect.left += image_rect.Width();
  }

  //рисуем текст в зависимости от текущего состояния
  if(!IsTabEnabled(tab_index))
  {
    p_dc->SetTextColor(color_disabled);
    if(selected)
      rect.top -= 3 * ::GetSystemMetrics(SM_CYEDGE);

    p_dc->DrawText(label, rect,DT_SINGLELINE | DT_VCENTER | DT_CENTER);
  }
  else
  {
    p_dc->SetTextColor(color_normal);
    if(selected)
      rect.top -= 3*::GetSystemMetrics(SM_CYEDGE);
    
    p_dc->DrawText(label, rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
  }

  p_dc->RestoreDC(save_dc);
}


////////////////////////////////////////////////////////////////////
// Trap arrow-left key to skip disabled tabs.
// This is the only way to know where we're coming from--ie from
// arrow-left (prev) or arrow-right (next).
//
BOOL CTabController::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)) 
  {
    int iNewTab = (pMsg->wParam == VK_LEFT) ?
       PrevEnabledTab(GetCurSel(), FALSE) : 
	   NextEnabledTab(GetCurSel(), FALSE);
    if (iNewTab >= 0)
      SetCurSel(iNewTab);
    return TRUE;
  }
   return CTabCtrl::PreTranslateMessage(pMsg);
}


////////////////////////////////////////////////////////////////////////////////
// Return the index of the next enabled tab after a given index, or -1 if none
// (0 = first tab).
// If bWrap is TRUE, wrap from beginning to end; otherwise stop at zero.
//
int CTabController::NextEnabledTab(int iCurrentTab, BOOL bWrap)
{
  int nTabs = GetItemCount();
  for (int iTab = iCurrentTab+1; iTab != iCurrentTab; iTab++) 
  {
    if (iTab >= nTabs) 
	{
      if (!bWrap)
        return -1;
      iTab = 0;
    }
    if (IsTabEnabled(iTab)) 
      return iTab;
   }
   return -1;
}

/////////////////////////////////////////////////////////////////////////////
// Return the index of the previous enabled tab before a given index, or -1.
// (0 = first tab).
// If bWrap is TRUE, wrap from beginning to end; otherwise stop at zero.
//
int CTabController::PrevEnabledTab(int iCurrentTab, BOOL bWrap)
{
  for (int iTab = iCurrentTab-1; iTab != iCurrentTab; iTab--) 
  {
    if (iTab < 0) 
	{
      if (!bWrap)
        return -1;
      iTab = GetItemCount() - 1;
    }
    if (IsTabEnabled(iTab)) 
      return iTab;
  }
  return -1;
}


///////////////////////////////////////////////////////////////////////////////
// Helper to set the active page, when moving backwards. Must simulate Windows 
// messages to tell parent I am changing the tab; SetCurSel does not do this!
//
// In normal operation, this fn will always succeed, because I don't call it
// unless I already know IsTabEnabled() = TRUE; but if you call SetActiveTab
// with a random value, it could fail.
//
bool CTabController::SetCurSel(UINT iNewTab)
{
  //if (!IsTabEnabled(iNewTab))
    //return false;

  //===================================================================== 
  if (m_pEventHandler&&(!m_msg_reflect))  //если не поставить проверку !m_msg_reflect то хэндлер будет вызываться второй раз	
    m_pEventHandler->OnSelchangingTabctl(); //Send event!
  //===================================================================== 

  // send the parent TCN_SELCHANGING
  NMHDR nmh;
  nmh.hwndFrom = m_hWnd;
  nmh.idFrom = GetDlgCtrlID();
  nmh.code = TCN_SELCHANGING;

  if (m_msg_reflect)
    GetParent()->SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);  

  DestroyTabPage(); //удаление предыдущей вкладки
  int previos_selected_item = CTabCtrl::SetCurSel(iNewTab); //выбор новой вкладки
  CreateTabPage(); //отображение новой - выбранной вкладки

  //===================================================================== 
  if (m_pEventHandler&&(!m_msg_reflect))
	m_pEventHandler->OnSelchangeTabctl(); //Send event!
  //===================================================================== 

  // send parent TCN_SELCHANGE
  nmh.code = TCN_SELCHANGE;

  if (m_msg_reflect)
    GetParent()->SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);

  return true;
}

//
BOOL CTabController::IsTabEnabled(int iTab)
{
  TabPageData* pPageData = GetItemData(iTab);

  return pPageData->is_enabled ? TRUE: FALSE;
}

//Achtung! Only Item, not a dialog incapsulated by item!
//if iTab==-1, then all items will be enabled/disabled 
void CTabController::EnableItem(int iTab, bool enable)
{
  int count_of_items = GetItemCount();

  if ((iTab >= count_of_items) && (iTab!=-1))
	return; //invalid iTab parameter!
  
  if (iTab==-1) //implement changes for all items
  {
    for(int i = 0; i < count_of_items; i++)
    {
      TabPageData* pPageData = GetItemData(i);
      pPageData->is_enabled = enable;
    }
  }
  else //concrete item
  {
    TabPageData* pPageData = GetItemData(iTab);
    pPageData->is_enabled = enable;  
  }

  //Tab control should be repainted!
  Invalidate();
}