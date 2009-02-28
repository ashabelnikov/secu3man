
#pragma once


#include "ChildView.h"
#include "MainFrame.h"
#include "ChildViewManager.h"

class CMainFrame;
class CChildViewManager;
class MainFrameController;

class CMainFrameManager  
{
 public:
  CMainFrameManager();
  virtual ~CMainFrameManager();
  
  //Создает пустое главное окно и возвращает указатель не него
  CMainFrame* GreateMainWindow(void);
  
  //инициализирует содержимое окна и дочерние контроллеры
  bool Init(CWnd* &o_pMainWnd);

  virtual bool OnClose(void);

  CMainFrame* m_pMainFrame;	//view 
  MainFrameController* m_pMainFrameController; //controller
  CChildViewManager* m_pChildViewManager;
  CStatusBarManager* m_pStatusBarManager;
};
