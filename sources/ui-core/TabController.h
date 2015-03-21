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

/** \file TabController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

/////////////////////////////////////////////////////////////////////////////

#include <vector>

class CTabDialog;
class ITabControllerEvent;

// использовать стиль TCS_OWNERDRAWFIXED!

class AFX_EXT_CLASS CTabController : public CTabCtrl
{
  DECLARE_DYNAMIC(CTabController)

 public:
  CTabController();
  virtual ~CTabController();

  //Устанавливает модуль, который содержит ресурсы необходимые для работы контролла.
  void SetResourceModule(HMODULE hModule);

  //Добавление вкладок
  int  AddPage(CString name, CTabDialog* pPageDlg);
  int  AddPage(CString name, CTabDialog* pPageDlg, const int nImage);

  // set tab (fail if disabled)
  bool SetCurSel(UINT iNewTab);

  //Разрешение/запрещение указанной вкладки
  void EnableItem(int ITab, bool enable);

  //Инициализация. Обязательно нужно вызвать перед началом работы!
  void Init(void);

  //Установка обработчика событий. Смотрите описание ITabControllerEvent.
  void SetEventListener(ITabControllerEvent* i_listener);

  CTabDialog* GetCurrentPage(void) const;

 protected:
  // Generated message map functions
  afx_msg void OnSelchangeTabctl(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSelchangingTabctl(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnDestroy();
  DECLARE_MESSAGE_MAP()

  int NextEnabledTab(int iTab, bool bWrap);  // get next enabled tab
  int PrevEnabledTab(int iTab, bool bWrap);  // get prev enabled tab

  // MFC overrides
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

 private:
  class TabPageData;
  TabPageData* GetItemData(int item) const;
  void CreateTabPage(void);
  void DestroyTabPage(void);
  void CalculatePageRect(int nItem, CRect& o_rect);
  int GetTCOrientation(void);

  CTabDialog*  mp_CurDlg;
  HMODULE m_hResourceModule;
  HMODULE _GetResourceModule(void);
  ITabControllerEvent* m_pEventHandler;

  int   m_tab_item_index;
  int   m_tcmn;
  bool  m_tabNavWrap;
};

/////////////////////////////////////////////////////////////////////////////
