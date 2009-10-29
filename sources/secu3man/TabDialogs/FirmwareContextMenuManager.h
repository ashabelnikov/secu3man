
#pragma once

#include <vector>

class CFirmwareModeContextMenuManager  
{
 public:
  CFirmwareModeContextMenuManager(CWnd* pWnd = NULL);
  virtual ~CFirmwareModeContextMenuManager();

  void Attach(CWnd* pWnd);
  void CreateContent(void);
  void TrackPopupMenu(int x, int y);
  void EnableBLMenuItems(bool i_enable);
  void EnableAppMenuItems(bool i_enable);

  //Надо вызывать когда окну приходит WM_INITMENUPOPUP 
  void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);

 private:
  CWnd* m_pWnd;        //Identifies the window that owns the pop-up menu
  CMenu m_ParentMenu;  //Top level menu
  CMenu m_ImportMenu;
  CMenu m_ExportMenu;

  std::vector<UINT> m_bl_menu_items_IDs;
  std::vector<UINT> m_app_menu_items_IDs;
};
