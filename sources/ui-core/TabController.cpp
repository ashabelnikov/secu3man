/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file TabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "TabController.h"
#include "TabDialog.h"
#include "ITabControllerEvent.h"
#include "common/MathHelpers.h"

enum tcOrient
{
 TCO_LEFT, TCO_TOP, TCO_RIGHT, TCO_BOTTOM
};

class CTabController::TabPageData
{
 public:
  TabPageData() : pDialogTemplate(NULL), pDialogClass(NULL) {};
  DLGTEMPLATE* pDialogTemplate;
  CTabDialog*  pDialogClass;
  bool is_enabled;  //определяет текущее состояние Item-a
};

IMPLEMENT_DYNAMIC(CTabController, Super)

BEGIN_MESSAGE_MAP(CTabController, Super)
 ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchangeTabctl)
 ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchangingTabctl)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabController

bool CTabController::m_liveWnd = true;

void CTabController::SetSettings(bool liveWnd)
{
 CTabController::m_liveWnd = liveWnd;
}

CTabController::CTabController()
: m_tab_item_index(0)
, mp_CurDlg(NULL)
, m_tcmn(4) //magic number
, m_pEventHandler(NULL)
, m_hResourceModule(NULL)
, m_tabNavWrap(true)
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
 DWORD style = Super::GetStyle();

 RECT c_rc, i_rc;

 //необходимо получить размер для вкладки
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
 DWORD style = Super::GetStyle();
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

void CTabController::CreateTabPage(bool create /* = false*/)
{
 if (m_liveWnd && !create)
 {
  int selection = GetCurSel();
  TabPageData* pPageData = GetItemData(selection);

  mp_CurDlg = pPageData->pDialogClass;  
  if (mp_CurDlg && IsWindow(mp_CurDlg->m_hWnd))
  {
   FitDialogSize(selection, true);
   mp_CurDlg->OnShow(true);
  }
 }
 else
 {
  int selection = m_liveWnd ? m_tab_item_index : GetCurSel();
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

  CTabDialog *p_CurDlg = pPageData->pDialogClass;
  FitDialogSize(selection, !m_liveWnd);
  if (!create)
   p_CurDlg->OnShow(true);

  if (!m_liveWnd)
   mp_CurDlg = p_CurDlg;
 }
}

void CTabController::DestroyTabPage(void)
{
 if (mp_CurDlg && IsWindow(mp_CurDlg->m_hWnd))
 {
  if (m_liveWnd)
  {
   mp_CurDlg->ShowWindow(SW_HIDE);
   mp_CurDlg->OnShow(false);
  }
  else
  {
   mp_CurDlg->OnShow(false);
   mp_CurDlg->DestroyWindow();
   mp_CurDlg = NULL;
  }
 }
}

//добавление вкладки, !!! ВНИЗУ смотреть похожую функцию !!!
int CTabController::AddPage(CString name, CTabDialog* pPageDlg)
{
 TabPageData* pPageData = new TabPageData;
 pPageData->pDialogClass = pPageDlg;

 pPageData->is_enabled = true; //by default - item has been enabled

 if (pPageDlg) //our control is able to work without dialogs
 {
  //создание шаблона диалога
  HRSRC hrsrc  = FindResource(_GetResourceModule(), pPageDlg->GetDialogID(), RT_DIALOG);
  if (NULL==hrsrc)
  {
   AfxMessageBox(_T("Resource not found!"));
   return -1;  //error
  }

  HGLOBAL hglb = LoadResource(_GetResourceModule(), hrsrc);
  if (NULL==hglb)
  {
   AfxMessageBox(_T("Resource load failed!"));
   return -1;  //error
  }
  pPageData->pDialogTemplate = (DLGTEMPLATE*)LockResource(hglb);
 }
 //добавление непосредственно вкладки
 InsertItem(TCIF_TEXT|TCIF_PARAM,m_tab_item_index,name,0,(LPARAM)pPageData);

 if (m_liveWnd)
  CreateTabPage(true); //create window, do not call OnShow()

 return m_tab_item_index++;
}

//!!! ВВЕРХУ смотреть похожую функцию !!!
int CTabController::AddPage(CString name, CTabDialog* pPageDlg, const int nImage)
{
 TabPageData* pPageData = new TabPageData;
 pPageData->pDialogClass = pPageDlg;

 pPageData->is_enabled = true; //by default - item has been enabled

 if (pPageDlg) //our control is able to work without dialogs
 {
  //создание шаблона диалога
  HRSRC hrsrc  = FindResource(_GetResourceModule(), pPageDlg->GetDialogID(), RT_DIALOG);
  if (NULL==hrsrc)
  {
   AfxMessageBox(_T("Resource not found!"));
   return -1; //error
  }

  HGLOBAL hglb = LoadResource(_GetResourceModule(), hrsrc);
  if (NULL==hglb)
  {
   AfxMessageBox(_T("Resource load failed!"));
   return -1; //error
  }

  pPageData->pDialogTemplate = (DLGTEMPLATE*)LockResource(hglb);
 }

 //добавление непосредственно вкладки
 InsertItem(TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE,m_tab_item_index,name,nImage,(LPARAM)pPageData);

 if (m_liveWnd)
  CreateTabPage(true); //create window, do not call OnShow()

 return m_tab_item_index++;
}

void CTabController::OnSelchangeTabctl(NMHDR* pNMHDR, LRESULT* pResult)
{
 //отображение выбранной вкладки вкладки
 CreateTabPage();

 if (m_pEventHandler)
  m_pEventHandler->OnSelchangeTabctl();

 *pResult = 0;
}

void CTabController::OnSelchangingTabctl(NMHDR* pNMHDR, LRESULT* pResult)
{
 // Figure out index of new tab we are about to go to, as opposed
 // to the current one we're at. Believe it or not, Windows doesn't
 // pass this info
 TC_HITTESTINFO htinfo;
 GetCursorPos(&htinfo.pt);
 ScreenToClient(&htinfo.pt);
 int iNewTab = HitTest(&htinfo);

 if (iNewTab >= 0 && !GetItemData(iNewTab)->is_enabled)
  *pResult = TRUE; // tab disabled: prevent selection
 else
 {
  bool result = true;
  if (m_pEventHandler)
   result = m_pEventHandler->OnSelchangingTabctl();

  if (result)
  {
   //удаление предыдущей вкладки
   DestroyTabPage();
   *pResult = 0;
  }
  else
   *pResult = TRUE; //not allowed by listener
 }
}

/////////////////////////////////////////////////////////////////////////////
// CTabController message handlers

//при изменении размера CTabCtrl необходимо подгонять размеры текущей вкладки
void CTabController::OnSize( UINT nType, int cx, int cy )
{
 //позволяем родительскому классу тоже изменить размер
 Super::OnSize(nType, cx, cy);

 CRect rect(0,0,0,0);

 int index = GetCurSel();
 if (index==-1)
  return; //не выбрано ни одной вкладки

 TabPageData* pItemData = NULL;
 pItemData = GetItemData(index);
 if (NULL == pItemData)
  return;

 CalculatePageRect(index, rect);
 if (pItemData->pDialogClass->GetSafeHwnd())
  pItemData->pDialogClass->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
}

void CTabController::OnDestroy()
{
 int item_count = Super::GetItemCount();

 //удаляем объекты ассоциированные с вкладками Tab-контролом
 for(int i = 0; i < item_count; ++i)
 {
  TabPageData* pItemData = NULL;
  pItemData = GetItemData(i);
  if (NULL == pItemData)
   continue;

  delete pItemData;
 }

 Super::OnDestroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTabController::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
 CRect rect = lpDrawItemStruct->rcItem;
 rect.DeflateRect(GetTCOrientation()!=TCO_RIGHT ? ::GetSystemMetrics(SM_CXEDGE) : 0,
  GetTCOrientation()==TCO_TOP ? ::GetSystemMetrics(SM_CYEDGE) : 0,
  ::GetSystemMetrics(SM_CXEDGE), GetTCOrientation()==TCO_BOTTOM ? ::GetSystemMetrics(SM_CYEDGE) : 0);

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
 p_dc->FillSolidRect(&rect, ::GetSysColor(COLOR_BTNFACE));

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
  int y_pos = (TCO_BOTTOM==GetTCOrientation() && !selected) ? (rect.CenterPoint().y - MathHelpers::Round(((float)image_rect.Height())/2.0f)) : rect.top;
  int x_pos = rect.left;

  if (TCO_RIGHT==GetTCOrientation())
   x_pos = rect.right - image_rect.Width();

  //рисуем картинку в зависимости от текущего состояния
  if (GetItemData(tab_index)->is_enabled)
   p_imagelist->Draw(p_dc, item.iImage, CPoint(x_pos, y_pos),ILD_TRANSPARENT);
  else
   DrawState(p_dc->m_hDC,NULL,NULL,(LPARAM)p_imagelist->ExtractIcon(item.iImage),
     0, x_pos, y_pos, image_rect.Width(), image_rect.Height(), DST_ICON|DSS_DISABLED);

  //Text will be placed after an image
  if (TCO_TOP==GetTCOrientation() || TCO_BOTTOM==GetTCOrientation())
   rect.left += image_rect.Width();
  else
   rect.top += (image_rect.Height() + 1);
 }

 //Select vertical font if orientation is vertical
 if (TCO_LEFT==GetTCOrientation() || TCO_RIGHT==GetTCOrientation())
  rect.left+=p_dc->GetTextExtent(label).cy;

 //рисуем текст в зависимости от текущего состояния
 if(selected)
 {
  if (TCO_TOP==GetTCOrientation() || TCO_BOTTOM==GetTCOrientation())
   rect.top -= 2 * ::GetSystemMetrics(SM_CYEDGE);
  else
   rect.left += 2 * ::GetSystemMetrics(SM_CYEDGE);
 }
 UINT uFormat = (TCO_TOP==GetTCOrientation() || TCO_BOTTOM==GetTCOrientation()) ? (DT_VCENTER | DT_CENTER) : 0;
 p_dc->SetTextColor(GetItemData(tab_index)->is_enabled ? color_normal : color_disabled);
 p_dc->DrawText(label, rect, DT_SINGLELINE | DT_NOCLIP | uFormat);

 p_dc->RestoreDC(save_dc);
}

// Trap arrow-left/right keys to skip disabled tabs.
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
    PrevEnabledTab(GetCurSel(), m_tabNavWrap) :
    NextEnabledTab(GetCurSel(), m_tabNavWrap);
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
    PrevEnabledTab(GetCurSel(), m_tabNavWrap) :
    NextEnabledTab(GetCurSel(), m_tabNavWrap);
    if (iNewTab >= 0)
     SetCurSel(iNewTab);
    return TRUE;
   }
  }
  //Behaviour of Home and End keys doesn't depend on orientation
  if (pMsg->wParam == VK_END)
  { //select last enabled tab
   for (int iTab = GetItemCount()-1; iTab > 0; iTab--)
    if (GetItemData(iTab)->is_enabled)
    {
     SetCurSel(iTab);
     break;
    }
   return TRUE;
  }   
  if (pMsg->wParam == VK_HOME)
  { //select first enabled tab
   for (int iTab = 0; iTab < GetItemCount(); iTab++)
    if (GetItemData(iTab)->is_enabled)
    {
     SetCurSel(iTab);
     break;
    }
   return TRUE;
  }
 }
 return Super::PreTranslateMessage(pMsg);
}

// Return the index of the next enabled tab after a given index, or -1 if none (0 = first tab).
// If bWrap is true, wrap from beginning to end; otherwise stop at zero.
int CTabController::NextEnabledTab(int iCurrentTab, bool bWrap)
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
  if (GetItemData(iTab)->is_enabled)
   return iTab;
 }
 return -1;
}

// Return the index of the previous enabled tab before a given index, or -1 (0 = first tab).
// If bWrap is true, wrap from beginning to end; otherwise stop at zero.
int CTabController::PrevEnabledTab(int iCurrentTab, bool bWrap)
{
 for (int iTab = iCurrentTab-1; iTab != iCurrentTab; iTab--)
 {
  if (iTab < 0)
  {
   if (!bWrap)
    return -1;
   iTab = GetItemCount() - 1;
  }
  if (GetItemData(iTab)->is_enabled)
   return iTab;
 }
 return -1;
}

// Helper to set the active page. Must simulate Windows
// messages to tell parent I am changing the tab; SetCurSel does not do this!
bool CTabController::SetCurSel(UINT iNewTab)
{
 if (mp_CurDlg)
 {
  //=====================================================================
  if (m_pEventHandler)
   if (!m_pEventHandler->OnSelchangingTabctl()) //Send event!
    return false; //not allowed by listener
  //=====================================================================

  DestroyTabPage(); //удаление предыдущей вкладки
 }

 Super::SetCurSel(iNewTab); //выбор новой вкладки
 CreateTabPage(); //отображение новой - выбранной вкладки

 //=====================================================================
 if (m_pEventHandler)
  m_pEventHandler->OnSelchangeTabctl(); //Send event!
 //=====================================================================

 return true;
}

//Note! Only Item, not a dialog incapsulated by item!
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

void CTabController::Init(void)
{
 m_tab_item_index = 0;
}

void CTabController::SetEventListener(ITabControllerEvent* i_listener)
{
 ASSERT(i_listener);
 m_pEventHandler = i_listener;
}

void CTabController::SetResourceModule(HMODULE hModule)
{
 m_hResourceModule = hModule;
}

HMODULE CTabController::_GetResourceModule(void)
{
 if (!m_hResourceModule)
  m_hResourceModule = AfxGetInstanceHandle();
 return m_hResourceModule;
}

CTabDialog* CTabController::GetCurrentPage(void) const
{
 return mp_CurDlg;
}

void CTabController::SetPageCaption(int iTab, CString text)
{
 TCITEM tcItem;
 int count_of_items = GetItemCount();
 if (iTab >= count_of_items)
  return; //invalid iTab parameter!
 TCHAR buffer[256] = {0};
 tcItem.pszText = buffer;
 tcItem.cchTextMax = 256;
 tcItem.mask = TCIF_TEXT;
 GetItem(0, &tcItem);
 tcItem.pszText = text.LockBuffer();
 SetItem(iTab, &tcItem);
 text.UnlockBuffer();
}

void CTabController::FitDialogSize(int sel, bool show)
{
 CRect c_rc(0,0,0,0);
 CalculatePageRect(sel, c_rc);

 //вкладка диалога находится в одной системе координат с таб-контроллом (у них общий parent!)
 CRect parent_rect;
 GetWindowRect(parent_rect);
 this->GetParent()->ScreenToClient(parent_rect);
 c_rc.OffsetRect(parent_rect.left, parent_rect.top);

 //подогнали размер диалога и сохранили указатель на текущий диалог
 TabPageData* pPageData = GetItemData(sel);
 pPageData->pDialogClass->SetWindowPos(&wndTop, c_rc.left, c_rc.top, c_rc.Width(), c_rc.Height(), show ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
}
