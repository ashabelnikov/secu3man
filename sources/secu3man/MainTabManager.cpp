 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MainTabManager.h"
#include "resource.h"

#include "ParamMonTabController.h"
#include "FirmwareTabController.h"

#include "ParamMonTabDlg.h"
#include "FirmwareTabDlg.h"

#include "secu3man.h"


#define MAIN_TAB_CTRL_BITMAPS_COLOR_MASK RGB(192,192,192)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainTabManager::CMainTabManager()
: m_pImgList(NULL)
, m_pParent(NULL)
, m_pParamMonTabController(NULL)
, m_pFirmwareTabController(NULL)
, m_pParamMonTabDlg(NULL)
, m_pFirmwareTabDlg(NULL)
{

}

CMainTabManager::~CMainTabManager()
{
  if (m_pImgList)
  {
    delete m_pImgList;
	m_pImgList = NULL;
  }

  delete m_pParamMonTabController;
  delete m_pFirmwareTabController;

  delete m_pParamMonTabDlg;
  delete m_pFirmwareTabDlg;
}

bool CMainTabManager::Init(CChildView* i_pChildView)
{
  ASSERT(i_pChildView);
  m_pParent = i_pChildView;
  m_pImgList = new CImageList(); 
  m_pImgList->Create((LPCTSTR)IDB_MAIN_TAB_CTRL_BITMAPS,16,4,MAIN_TAB_CTRL_BITMAPS_COLOR_MASK);

  CRect rect(0,0,300,200);

  m_tab_control.SetStyle(WS_VISIBLE | WS_CHILD  | TCS_BOTTOM | TCS_OWNERDRAWFIXED);
  m_tab_control.SetResourceModule(::GetModuleHandle(NULL));
  BOOL b = m_tab_control.Create(i_pChildView,rect,IDC_MAIN_TAB_CTRL,false);
  m_tab_control.SetImageList(m_pImgList);

  m_tab_control.SetEventListener(&m_MainTabController); //контроллер будет принимать сообщени€ от View
  m_MainTabController.SetTabController(&m_tab_control); //set View


  CSecu3manApp* p_app = static_cast<CSecu3manApp*>(AfxGetApp());

  ASSERT(p_app);
  CCommunicationManager* p_comm = p_app->m_pCommunicationManager;
  CStatusBarManager*     p_sbar = &p_app->m_pMainFrameManager->m_StatusBarManager;

  //создаем контроллеры вкладок и наполн€ем Tab control вкладками
  m_pParamMonTabDlg = new CParamMonTabDlg(); //view
  m_pParamMonTabController = new CParamMonTabController(m_pParamMonTabDlg, p_comm, p_sbar);

  m_MainTabController.AddTabController(m_pParamMonTabController);
  m_tab_control.AddPage("ѕараметры и монитор",m_pParamMonTabDlg,0);

  m_pFirmwareTabDlg = new CFirmwareTabDlg(); //view
  m_pFirmwareTabController = new CFirmwareTabController(m_pFirmwareTabDlg, p_comm, p_sbar);

  m_MainTabController.AddTabController(m_pFirmwareTabController);
  m_tab_control.AddPage("ƒанные прошивки",m_pFirmwareTabDlg,1);	

  //необходимо выбрать любую вкладку вызвать OnActivate дл€ открытой вкладки.
 // m_MainTabController.OnSelchangeTabctl();
  m_tab_control.SetCurSel(0);

  return true;
}

void CMainTabManager::OnAfterCreate(void)
{
  CRect rect;
  m_pParent->GetClientRect(rect);
  rect.right-=0;
  rect.bottom-=0; 
  m_tab_control.MoveWindow(rect);
}