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

#include <memory> //std::auto_ptr<>

class CChildView;
class CMainTabController;
class CTabController;

class CMainTabManager  
{
 public:
  CMainTabManager();
  virtual ~CMainTabManager();  
   
  bool Init(CChildView* i_pChildView);
  void OnAfterCreate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what);

 private:
  //контроллеры вкладок
  class CParamMonTabController* m_pParamMonTabController;
  class CFirmwareTabController* m_pFirmwareTabController;
  class CCheckEngineTabController* m_pCheckEngineTabController;
  class CKnockChannelTabController* m_pKnockChannelTabController;
  class CLogPlayerTabController* m_pLogPlayerTabController;

  //диалоги вкладок
  class CParamMonTabDlg* m_pParamMonTabDlg;
  class CFirmwareTabDlg* m_pFirmwareTabDlg;
  class CCheckEngineTabDlg* m_pCheckEngineTabDlg;
  class CKnockChannelTabDlg* m_pKnockChannelTabDlg;
  class CLogPlayerTabDlg* m_pLogPlayerTabDlg;

 private:
  std::auto_ptr<CMainTabController> mp_MainTabController; 
  std::auto_ptr<CTabController> mp_tab_control;

  CChildView* m_pParent;
  CImageList* m_pImgList;
};
