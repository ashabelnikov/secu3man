
#pragma once

#include <memory>

class CChildView;
class CMainFrame;
class CMainTabManager;

class CChildViewManager  
{
 public:
  CChildViewManager();
  virtual ~CChildViewManager();

  bool Init(CMainFrame* i_pMainFrame);

  void OnAfterCreate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what);

 private:
  std::auto_ptr<CMainTabManager> mp_MainTabManager;
  std::auto_ptr<CChildView> mp_wndView;
};
