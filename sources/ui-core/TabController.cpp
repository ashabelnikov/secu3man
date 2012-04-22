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

enum tcOrient
{
 TCO_LEFT, TCO_TOP, TCO_RIGHT, TCO_BOTTOM
};

IMPLEMENT_DYNAMIC(CTabController, CTabCtrl)

BEGIN_MESSAGE_MAP(CTabController, CTabCtrl)
 ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchangeTabctl)
 ON_NOTIFY_REFLECT_EX(TCN_SELCHANGING, OnSelchangingTabctl)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabController

CTabController::CTabController()
: m_tab_item_index(0)
, mp_CurDlg(NULL)
, m_msg_reflect(TRUE)
, m_style(WS_VISIBLE | WS_CHILD)
, m_tcmn(4) //magic number
, m_pEventHandler(NULL)
, m_hResourceModule(NULL)
{
 //empty
}

CTabController::~CTabController()
{
 //empty
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

 RECT c_rc, i_rc;

 //необходимо получить размер дл€ вкладки
 GetClientRect(&c_rc);
 GetItemRect(nItem,&i_rc);

 if (TCO_BOTTOM == GetTCOrientation()) //bottom
 {
  c_rc.bottom-= (i_rc.bottom - i_rc.top) + m_tcmn;
  c_rc.top+=m_tcmn;
  c_rc.right-=m_tcmn;
  c_rc.left+=m_tcmn;
 }
 if (TCO_LEFT == GetTCOrientation()) //left
 {
  c_rc.left+= (i_rc.right - i_rc.left) + m_tcmn;
  c_rc.top+=m_tcmn;
  c_rc.bottom-=m_tcmn;
  c_rc.right-=m_tcmn;
 }
 if (TCO_RIGHT == GetTCOrientation()) //right
 {
  c_rc.right-= (i_rc.right - i_rc.left) + m_tcmn;
  c_rc.top+=m_tcmn;
  c_rc.left+=m_tcmn;
  c_rc.bottom-=m_tcmn;
 }
 if (TCO_TOP == GetTCOrientation())//top
 {
  c_rc.top+= (i_rc.bottom - i_rc.top) + m_tcmn;
  c_rc.left+=m_tcmn;
  c_rc.bottom-=m_tcmn;
  c_rc.right-=m_tcmn;
 }
 o_rect = c_rc;
}

int CTabController::GetTCOrientation(void)
{
 DWORD style = CTabCtrl::GetStyle();
 if ((style & TCS_BOTTOM) && !(style & TCS_VERTICAL)) //bottom
  return TCO_BOTTOM;
 if (!(style & TCS_BOTTOM) && (style & TCS_VERTICAL)) //left
  return TCO_LEFT;
 if ((style & TCS_BOTTOM) && (style & TCS_VERTICAL)) //right
  return TCO_RIGHT;
 if (!(style & TCS_BOTTOM) && !(style & TCS_VERTICAL))//top
  return TCO_TOP;
 return TCO_TOP; //default
}

void CTabController::CreateTabPage(void)
{
 int selection = GetCurSel();

 //создание окна вкладки и ее отображение
 TabPageData* pPageData = GetItemData(selection);
 if (!pPageData->pDialogClass || ::IsWindow(pPageData->pDialogClass->m_hWnd))
  return; //предотвращаем повторное создание вкладки

 ASSERT(pPageData);
 BOOL result = pPageData->pDialogClass->CreateIndirect(pPageData->pDialogTemplate, this->GetParent());

 if (0==result)
 {
  AfxMessageBox(_T("Error creating Tab control page dialog!"));
  return;
 }

 CRect c_rc(0,0,0,0);
 CalculatePageRect(selection, c_rc);

 //вкладка диалога находитс€ в одной системе координат с таб-контроллом (у них общий parent!)
 CRect parent_rect;
 GetWindowRect(parent_rect);
 this->GetParent()->ScreenToClient(parent_rect);
 c_rc.OffsetRect(parent_rect.left, parent_rect.top);

 //подогнали размер диалога и сохранили указатель на текущий диалог
 pPageData->pDialogClass->SetWindowPos(&wndTop, c_rc.left, c_rc.top, c_rc.Width(), c_rc.Height(), SWP_SHOWWINDOW);
 mp_CurDlg = pPageData->pDialogClass;
}

void CTabController::DestroyTabPage(void)
{
 if (mp_CurDlg)
 {
  if (IsWindow(mp_CurDlg->m_hWnd)) //только если окно было создано (предотвращаем повторное закрытие окна)
   mp_CurDlg->DestroyWindow();
 }
}

bool CTabController::Create(CWnd* pParentWnd,const CRect& rect,UINT nID,const bool i_msg_reflect)
{
 if (FALSE == CTabCtrl::Create(m_style, rect, pParentWnd, nID))
  return false;

 if (NULL==m_hResourceModule)
  m_hResourceModule = AfxGetInstanceHandle();

 m_msg_reflect = (i_msg_reflect) ? FALSE : TRUE; //отражать сообщени€ родительскому окну или нет?
 
 if (TCO_LEFT==GetTCOrientation() || TCO_RIGHT==GetTCOrientation())
 {
  LOGFONT lf;
  CFont* pfont = GetFont();
  if (!pfont)
   pfont = pParentWnd->GetFont();
  ASSERT(pfont);
  if (pfont)
  {
   pfont->GetLogFont(&lf);
   lf.lfEscapement = 2700; //rotation by 270 deg.
   lf.lfOrientation = 2700;
   _tcscpy(lf.lfFaceName, _T("Microsoft Sans Serif"));
   VERIFY(m_vfnt.CreateFontIndirect(&lf));
  }
 }
 return true;
}

//добавление вкладки, !!! ¬Ќ»«” смотреть похожую функцию !!!
int CTabController::AddPage(CString name,CTabDialog* pPageDlg)
{
 TabPageData* pPageData = new TabPageData;
 pPageData->pDialogClass = pPageDlg;

 pPageData->is_enabled = true; //by default - item has been enabled

 if (pPageDlg) //our control is able to work without dialogs
 {
  //создание шаблона диалога
  HRSRC hrsrc  = FindResource(m_hResourceModule,pPageDlg->GetDialogID(), RT_DIALOG);
  if (NULL==hrsrc)
  {
   AfxMessageBox(_T("Resource not found!"));
   return -1;  //error
  }

  HGLOBAL hglb = LoadResource(m_hResourceModule, hrsrc);
  if (NULL==hglb)
  {
   AfxMessageBox(_T("Resource load failed!"));
   return -1;  //error
  }
  pPageData->pDialogTemplate = (DLGTEMPLATE*)LockResource(hglb);
 }
 //добавление непосредственно вкладки
 InsertItem(TCIF_TEXT|TCIF_PARAM,m_tab_item_index,name,0,(LPARAM)pPageData);

 if (0 == m_tab_item_index)
 { //выбираем первую вкладку
  CreateTabPage();
 }

 return m_tab_item_index++;
}

//!!! ¬¬≈–’” смотреть похожую функцию !!!
int CTabController::AddPage(CString name,CTabDialog* pPageDlg,const int nImage)
{
 TabPageData* pPageData = new TabPageData;
 pPageData->pDialogClass = pPageDlg;

 pPageData->is_enabled = true; //by default - item has been enabled

 //создание шаблона диалога
 HRSRC hrsrc  = FindResource(m_hResourceModule,pPageDlg->GetDialogID(), RT_DIALOG);
 if (NULL==hrsrc)
 {
  AfxMessageBox(_T("Resource not found!"));
  return -1; //error
 }

 HGLOBAL hglb = LoadResource(m_hResourceModule, hrsrc);
 if (NULL==hglb)
 {
  AfxMessageBox(_T("Resource load failed!"));
  return -1; //error
 }

 pPageData->pDialogTemplate = (DLGTEMPLATE*)LockResource(hglb);

 //добавление непосредственно вкладки
 InsertItem(TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE,m_tab_item_index,name,nImage,(LPARAM)pPageData);

 if (0 == m_tab_item_index)
 {
  //выбираем первую вкладку
  CreateTabPage();
 }

 return m_tab_item_index++;
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

/////////////////////////////////////////////////////////////////////////////
// CTabController message handlers

//при изменении размера CTabCtrl необходимо подгон€ть размеры текущей вкладки
void CTabController::OnSize( UINT nType, int cx, int cy )
{
 CRect rect(0,0,0,0);

 int index = GetCurSel();
 if (index==-1)
  return; //не выбрано ни одной вкладки

 TabPageData* pItemData = NULL;
 pItemData = GetItemData(index);
 if (NULL == pItemData)
  return;

 CalculatePageRect(index, rect);
 pItemData->pDialogClass->MoveWindow(rect);

 //позвол€ем родительскому классу тоже изменить размер
 CTabCtrl::OnSize(nType, cx, cy);
}

void CTabController::OnDestroy()
{
 int item_count = CTabCtrl::GetItemCount();

 //удал€ем объекты ассоциированные с вкладками Tab-контролом
 for(int i = 0; i < item_count; ++i)
 {
  TabPageData* pItemData = NULL;
  pItemData = GetItemData(i);
  if (NULL == pItemData)
   continue;

  delete pItemData;
 }

 if (m_vfnt.m_hObject)
  m_vfnt.DeleteObject();

 CTabCtrl::OnDestroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTabController::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
 CRect rect = lpDrawItemStruct->rcItem;
 rect.DeflateRect(GetTCOrientation()!=TCO_RIGHT ? ::GetSystemMetrics(SM_CXEDGE) : 0,
  GetTCOrientation()==TCO_TOP ? ::GetSystemMetrics(SM_CYEDGE) : 0,
  ::GetSystemMetrics(SM_CXEDGE), 0);

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
 p_dc->FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_BTNFACE));

 //Add a space frpm left (or top) side
 if (TCO_TOP==GetTCOrientation() || TCO_BOTTOM==GetTCOrientation())
  rect.left += p_dc->GetTextExtent(_T(" ")).cx;
 else
  rect.top += p_dc->GetTextExtent(_T(" ")).cx;

 // draw the image
 CImageList* p_imagelist = GetImageList();
 if(p_imagelist && item.iImage >= 0)
 {
  IMAGEINFO info;
  p_imagelist->GetImageInfo(item.iImage,&info);
  CRect image_rect(info.rcImage);
  int y_pos = rect.top;
  int x_pos = rect.left;

  if (TCO_RIGHT==GetTCOrientation())
   x_pos = rect.right-image_rect.Width();

  //рисуем картинку в зависимости от текущего состо€ни€
  if (IsTabEnabled(tab_index))
   p_imagelist->Draw(p_dc, item.iImage, CPoint(x_pos, y_pos),ILD_TRANSPARENT);
  else  
   DrawState(p_dc->m_hDC,NULL,NULL,(LPARAM)p_imagelist->ExtractIcon(item.iImage),
     0, x_pos, y_pos, image_rect.Width(), image_rect.Height(), DST_ICON|DSS_DISABLED);
  
  //Text will be placed after an image
  if (TCO_TOP==GetTCOrientation() || TCO_BOTTOM==GetTCOrientation())
   rect.left += image_rect.Width();
  else
   rect.top += image_rect.Height();
 }

 //Select vertical font if orientation is vertical
 CFont* oldFont = NULL;
 if (TCO_LEFT==GetTCOrientation() || TCO_RIGHT==GetTCOrientation())
 {
  oldFont = p_dc->SelectObject(&m_vfnt);
  rect.left+=p_dc->GetTextExtent(label).cy;
 }

 //рисуем текст в зависимости от текущего состо€ни€
 if(selected)
 {
  if (TCO_TOP==GetTCOrientation() || TCO_BOTTOM==GetTCOrientation())
   rect.top -= 2 * ::GetSystemMetrics(SM_CYEDGE);
  else
   rect.left += 2 * ::GetSystemMetrics(SM_CYEDGE);
 } 
 UINT uFormat = (TCO_TOP==GetTCOrientation() || TCO_BOTTOM==GetTCOrientation()) ? (DT_VCENTER | DT_CENTER) : 0;
 p_dc->SetTextColor(IsTabEnabled(tab_index) ? color_normal : color_disabled);
 p_dc->DrawText(label, rect, DT_SINGLELINE | DT_NOCLIP | uFormat);

 if (oldFont)
  p_dc->SelectObject(&oldFont);
 p_dc->RestoreDC(save_dc);
}

////////////////////////////////////////////////////////////////////
// Trap arrow-left key to skip disabled tabs.
// This is the only way to know where we're coming from--ie from
// arrow-left (prev) or arrow-right (next).
//
BOOL CTabController::PreTranslateMessage(MSG* pMsg)
{
 if (pMsg->message == WM_KEYDOWN)
 {
  int orientation = GetTCOrientation();
  if (TCO_TOP==orientation || TCO_BOTTOM==orientation)
  {
   if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)
   {
    int iNewTab = (pMsg->wParam == VK_LEFT) ?
    PrevEnabledTab(GetCurSel(), FALSE) :
    NextEnabledTab(GetCurSel(), FALSE);
    if (iNewTab >= 0)
     SetCurSel(iNewTab);
    return TRUE;
   }
  }
  else
  {
   if (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN)
   {
    int iNewTab = (pMsg->wParam == VK_UP) ?
    PrevEnabledTab(GetCurSel(), FALSE) :
    NextEnabledTab(GetCurSel(), FALSE);
    if (iNewTab >= 0)
     SetCurSel(iNewTab);
    return TRUE;
   }
  }
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
 //=====================================================================
 if (m_pEventHandler&&(!m_msg_reflect))  //если не поставить проверку !m_msg_reflect то хэндлер будет вызыватьс€ второй раз
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

 // send to parent TCN_SELCHANGE
 nmh.code = TCN_SELCHANGE;

 if (m_msg_reflect)
  GetParent()->SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);

 return true;
}

//
BOOL CTabController::IsTabEnabled(int iTab) const
{
 TabPageData* pPageData = GetItemData(iTab);
 return pPageData->is_enabled ? TRUE: FALSE;
}

//Achtung! Only Item, not a dialog incapsulated by item!
//if iTab==-1, then all items will be enabled/disabled
void CTabController::EnableItem(int iTab, bool enable)
{
 bool repaint = false;
 int count_of_items = GetItemCount();

 if ((iTab >= count_of_items) && (iTab!=-1))
  return; //invalid iTab parameter!

 if (iTab==-1) //implement changes for all items
 {
  for(int i = 0; i < count_of_items; i++)
  {
   TabPageData* pPageData = GetItemData(i);
   if (pPageData->is_enabled != enable)
    repaint = true;
   pPageData->is_enabled = enable;
  }
 }
 else //concrete item
 {
  TabPageData* pPageData = GetItemData(iTab);
  if (pPageData->is_enabled != enable)
   repaint = true;
  pPageData->is_enabled = enable;
 }

 //Tab control should be repainted!
 if (repaint)
  Invalidate();
}

void CTabController::SetStyle(const DWORD style)
{
 m_style = style;
}

void CTabController::Init(void)
{
 m_tab_item_index = 0;
}

void CTabController::SetEventListener(ITabControllerEvent* i_listener)
{
 ASSERT(i_listener);
 m_pEventHandler = i_listener;
}

void CTabController::SetMsgReflection(BOOL reflect)
{
 m_msg_reflect = reflect;
}

void CTabController::SetResourceModule(HMODULE hModule)
{
 m_hResourceModule = hModule;
}

CTabDialog* CTabController::GetCurrentPage(void) const
{
 return mp_CurDlg;
}
