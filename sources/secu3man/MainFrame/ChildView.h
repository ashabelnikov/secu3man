/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#pragma once

#include "ui-core/DialogWithAccelerators.h"
#include "ui-core/TabController.h"

// CChildView window
class CChildView : public CModelessDialog
{
 typedef CModelessDialog Super;

 public:
  CChildView();
  virtual ~CChildView();

  //Возвращает false если ошибка
  bool Create(CWnd* ip_parent);

  //Add view tab
  void AddView(const _TSTRING& title, CTabDialog* ip_dlg, const int nImage);

  //Set Listener
  void SetEventListener(ITabControllerEvent* ip_listener);

  //Get current selection of a tab control
  int GetCurSel(void) const;

  //Set current selection of a tab control
  void SetCurSel(int index);

  //Set focus to default control (tab control)
  void SetFocus(bool curPage = false);

  //Enable/Disable tab control (only tab control, not its content)
  void EnableTabControl(bool i_enable);

  // Generated message map functions
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);// DDX/DDV support
  afx_msg void OnSize(UINT, int, int);
  DECLARE_MESSAGE_MAP()

 private:
  CTabController m_tab_control;
  CImageList* m_pImgList;
};

/////////////////////////////////////////////////////////////////////////////
