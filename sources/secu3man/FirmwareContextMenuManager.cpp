 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "FirmwareContextMenuManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFirmwareModeContextMenuManager::CFirmwareModeContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{

}

CFirmwareModeContextMenuManager::~CFirmwareModeContextMenuManager()
{

}

void CFirmwareModeContextMenuManager::Attach(CWnd* pWnd)
{
  ASSERT(pWnd);
  m_pWnd = pWnd;
}


//Fill menu with items and submenus
void CFirmwareModeContextMenuManager::CreateContent(void)
{
  CString str;

  ///////////////////////////////////////////////////////////////////////////
  //Popup menu "»мпорт данных"
  m_ImportMenu.CreatePopupMenu();
  str.LoadString(IDS_IMPORT_IMPORT_FROM_MPSZ);
  m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_IMPORT_FROM_MPSZ,str);

  str.LoadString(IDS_IMPORT_FW_DATA_FROM_ANOTHER_FW);
  m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_FW_DATA_FROM_ANOTHER_FW,str);	

  str.LoadString(IDS_IMPORT_FW_DATA_FROM_SECU3);
  m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_FW_DATA_FROM_SECU3,str);	


  ///////////////////////////////////////////////////////////////////////////
  //Popup menu "Ёкспорт данных"
  m_ExportMenu.CreatePopupMenu();
  str.LoadString(IDS_EXPORT_EXPORT_TO_MPSZ);
  m_ExportMenu.AppendMenu(MF_STRING,IDM_EXPORT_EXPORT_TO_MPSZ,str);
  ///////////////////////////////////////////////////////////////////////////
  m_ParentMenu.CreatePopupMenu();
  
  str.LoadString(IDS_READ_FLASH);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_FLASH,str);	

  str.LoadString(IDS_WRITE_FLASH);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_WRITE_FLASH,str);	

  str.LoadString(IDS_OPEN_FLASH);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_OPEN_FLASH,str);	

  str.LoadString(IDS_SAVE_FLASH);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_SAVE_FLASH,str);	

  ///////////////////////////////////////////////////////////////////////////
  m_ParentMenu.AppendMenu(MF_SEPARATOR);
  str.LoadString(IDS_IMPORT_POPUP_MENU);
  m_ParentMenu.AppendMenu(MF_POPUP | MF_STRING,(UINT)m_ImportMenu.m_hMenu,str);

  str.LoadString(IDS_EXPORT_POPUP_MENU);
  m_ParentMenu.AppendMenu(MF_POPUP | MF_STRING,(UINT)m_ExportMenu.m_hMenu,str);
  ///////////////////////////////////////////////////////////////////////////
  m_ParentMenu.AppendMenu(MF_SEPARATOR);

  str.LoadString(IDS_READ_FLASH_TO_FILE);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_FLASH_TO_FILE,str);
  m_bl_menu_items_IDs.push_back(IDM_READ_FLASH_TO_FILE);

  str.LoadString(IDS_WRITE_FLASH_FROM_FILE);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_WRITE_FLASH_FROM_FILE,str);
  m_bl_menu_items_IDs.push_back(IDM_WRITE_FLASH_FROM_FILE);

  m_ParentMenu.AppendMenu(MF_SEPARATOR);

  str.LoadString(IDS_READ_EEPROM_TO_FILE);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_EEPROM_TO_FILE,str);	
  m_bl_menu_items_IDs.push_back(IDM_READ_EEPROM_TO_FILE);

  str.LoadString(IDS_WRITE_EEPROM_FROM_FILE);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_WRITE_EEPROM_FROM_FILE,str);
  m_bl_menu_items_IDs.push_back(IDM_WRITE_EEPROM_FROM_FILE);

  m_ParentMenu.AppendMenu(MF_SEPARATOR);

  str.LoadString(IDS_READ_BOOTLOADER_SIGNATURE);
  m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_BOOTLOADER_SIGNATURE,str);
  m_bl_menu_items_IDs.push_back(IDM_READ_BOOTLOADER_SIGNATURE);

}

//показывает контекстное меню
void CFirmwareModeContextMenuManager::TrackPopupMenu(int x, int y)
{
  ASSERT(m_pWnd);
  m_ParentMenu.TrackPopupMenu(TPM_LEFTALIGN,x,y,m_pWnd);			   			 			
}


//This method should be called when a pop-up menu is about to become active
void CFirmwareModeContextMenuManager::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu) 
{
  ASSERT(m_pWnd);

  if (bSysMenu)
	return;     // don't support system menu

  ASSERT(pMenu != NULL);
  // check the enabled state of various menu items

  CCmdUI state;
  state.m_pMenu = pMenu;
  ASSERT(state.m_pOther == NULL);
  ASSERT(state.m_pParentMenu == NULL);

  // determine if menu is popup in top-level menu and set m_pOther to
  //  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
  HMENU hParentMenu;
  if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
	state.m_pParentMenu = pMenu;    // parent == child for tracking popup
  else if ((hParentMenu = ::GetMenu(m_pWnd->m_hWnd)) != NULL)
  {
	CWnd* pParent = m_pWnd->GetTopLevelParent();
	// child windows don't have menus -- need to go to the top!
	if (pParent != NULL && (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
	{
	  int nIndexMax = ::GetMenuItemCount(hParentMenu);
	  for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
	  {
		if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
		{
		  // when popup is found, m_pParentMenu is containing menu
		  state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
		  break;
		}
	  }
	}
  }

  state.m_nIndexMax = pMenu->GetMenuItemCount();
  for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
  {
    state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
	if (state.m_nID == 0)
	  continue; // menu separator or invalid cmd - ignore it

	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pMenu != NULL);
	if (state.m_nID == (UINT)-1)
	{
	  // possibly a popup menu, route to first item of that popup
	  state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
	  if (state.m_pSubMenu == NULL || 
		 (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 || state.m_nID == (UINT)-1)
		{
		  continue;       // first item of popup can't be routed to
		}
		state.DoUpdate(m_pWnd, FALSE);    // popups are never auto disabled
	}
	else
	{
	  // normal menu item
	  // Auto enable/disable if frame window has 'm_bAutoMenuEnable'
	  // set and command is _not_ a system command.
	  state.m_pSubMenu = NULL;
	  state.DoUpdate(m_pWnd, TRUE);
	}

	// adjust for menu deletions and additions
	UINT nCount = pMenu->GetMenuItemCount();
	if (nCount < state.m_nIndexMax)
	{
	  state.m_nIndex -= (state.m_nIndexMax - nCount);
	  while (state.m_nIndex < nCount && pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
	  {
	    state.m_nIndex++;
	  }
	}
	state.m_nIndexMax = nCount;
  }//for
}


