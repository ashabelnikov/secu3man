
#pragma once

class CMainFrame;
class CChildViewManager;
class MainFrameController;
class CStatusBarManager;

class CMainFrameManager  
{
 public:
  CMainFrameManager();
  virtual ~CMainFrameManager();
  
  //Создает пустое главное окно и возвращает указатель не него
  CMainFrame* GreateMainWindow(void);
  
  //инициализирует содержимое окна и дочерние контроллеры
  bool Init(CWnd* &o_pMainWnd);

  //возвращает менеджер статусной строки (в нижней части главного окна)
  CStatusBarManager* GetStatusBarManager(void) const;  

 private:
  MainFrameController* m_pMainFrameController; //controller
  CStatusBarManager* m_pStatusBarManager;
  CChildViewManager* m_pChildViewManager;
  CMainFrame* m_pMainFrame;	//view 
};
