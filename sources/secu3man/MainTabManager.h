
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
