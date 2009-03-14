 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#pragma warning (disable:C4786)

#include "stdafx.h"
#include <map>
#include "ParamDeskDlg.h"

#include "StarterPageDlg.h" 
#include "CarburPageDlg.h" 
#include "AnglesPageDlg.h" 
#include "IdlRegPageDlg.h" 
#include "FunSetPageDlg.h" 
#include "TemperPageDlg.h"
#include "ADCCompenPageDlg.h"
#include "CKPSPageDlg.h"
#include "KnockPageDlg.h"
#include "MiscPageDlg.h"

#include "io-core/ufcodes.h" 
#include "io-core/SECU3IO.h"

#include "common/FastDelegate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define TAB_CTRL_BITMAPS_COLOR_MASK RGB(192,192,192)

using namespace std;
using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CParamDeskDlg dialog


CParamDeskDlg::CParamDeskDlg(CWnd* pParent /*=NULL*/, bool i_show_knock_page /* = false*/)
: CUpdatableDialog(CParamDeskDlg::IDD, pParent)
, m_pImgList(NULL)
, m_enabled(FALSE)
, m_show_knock_page(i_show_knock_page)
{   
 //создаем image list для TabCtrl
 m_pImgList = new CImageList(); 
 m_pImgList->Create((LPCTSTR)IDB_TAB_CTRL_BITMAPS,16,4,TAB_CTRL_BITMAPS_COLOR_MASK);

 //их надо создать только один раз
 m_pStarterPageDlg = new CStarterPageDlg();
 m_pStarterPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pAnglesPageDlg = new CAnglesPageDlg();		 
 m_pAnglesPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pIdlRegPageDlg = new CIdlRegPageDlg();	  
 m_pIdlRegPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pFunSetPageDlg = new CFunSetPageDlg();		 
 m_pFunSetPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pTemperPageDlg = new CTemperPageDlg();		  
 m_pTemperPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pCarburPageDlg = new CCarburPageDlg();
 m_pCarburPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pADCCompenPageDlg = new CADCCompenPageDlg();
 m_pADCCompenPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pCKPSPageDlg = new CCKPSPageDlg();
 m_pCKPSPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 if (m_show_knock_page)
 {
  m_pKnockPageDlg = new CKnockPageDlg();
  m_pKnockPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));
 }

 m_pMiscPageDlg = new CMiscPageDlg();
 m_pMiscPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));
}


CParamDeskDlg::~CParamDeskDlg()
{  
 delete m_pImgList;	

 delete m_pStarterPageDlg; 		 
 delete m_pAnglesPageDlg; 		 
 delete m_pIdlRegPageDlg;	  
 delete m_pFunSetPageDlg;		 
 delete m_pTemperPageDlg;		  
 delete m_pCarburPageDlg;
 delete m_pADCCompenPageDlg;
 delete m_pCKPSPageDlg;
 if (m_show_knock_page)  
  delete m_pKnockPageDlg;
 delete m_pMiscPageDlg;
}


void CParamDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PARAMETERS_DESK_TITLE, m_pd_title);
 DDX_Control(pDX, IDC_PD_TAB_CTRL, m_tab_control);
 DDX_Control(pDX, IDC_PD_SAVE_BUTTON, m_save_button);
}


BEGIN_MESSAGE_MAP(CParamDeskDlg, CDialog)
 ON_WM_DESTROY()
 ON_UPDATE_COMMAND_UI(IDC_PARAMETERS_DESK_TITLE,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TAB_CTRL,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SAVE_BUTTON,OnUpdateControls)
 ON_BN_CLICKED(IDC_PD_SAVE_BUTTON,OnSaveButton)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CParamDeskDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CParamDeskDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CParamDeskDlg::OnInitDialog() 
{
 CDialog::OnInitDialog();
	
 m_tab_control.SetStyle(WS_VISIBLE | WS_CHILD | TCS_OWNERDRAWFIXED/*| TCS_VERTICAL | TCS_BOTTOM*/);
 m_tab_control.SetImageList(m_pImgList);
 m_tab_control.SetResourceModule(::GetModuleHandle(_T("paramdesk.dll")));
 m_tab_control.Init();
	
 m_tab_descriptors.clear();
 //наполняем Tab control вкладками	
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("Запуск",m_pStarterPageDlg,0), STARTR_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("УОЗ",m_pAnglesPageDlg,1), ANGLES_PAR));	
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("ХХ",m_pIdlRegPageDlg,2), IDLREG_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("Функции",m_pFunSetPageDlg,3), FUNSET_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("Температура",m_pTemperPageDlg,4), TEMPER_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("Карбюратор",m_pCarburPageDlg,5), CARBUR_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("Компенсация погрешностей АЦП",m_pADCCompenPageDlg,6), ADCCOR_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("ДПКВ",m_pCKPSPageDlg,7), CKPS_PAR));
 if (m_show_knock_page)
  m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("Детонация",m_pKnockPageDlg,8), KNOCK_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage("Прочее",m_pMiscPageDlg,7), MISCEL_PAR));
	
 //ВНИМАНИЕ! SetEventListener должен быть вызван раньше чем SetCurSel, т.к. SetCurSel 
 //уже использует обработчики сообщений!

 //this будет получать события от Tab control-а и делегировать их указанным обработчикам
 m_tab_control.SetEventListener(this);   

 //устанавливаем предыдущее значение (разрешены вкладки или нет)
 m_tab_control.EnableItem(-1,m_enabled ? true : false);

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
	            // EXCEPTION: OCX Property Pages should return FALSE
}

BYTE CParamDeskDlg::GetCurrentDescriptor(void)
{
 int selection = m_tab_control.GetCurSel();
 if (m_tab_descriptors.find(selection)==m_tab_descriptors.end())
 {
  ASSERT(0); //unexpected selection index!
  return 0;
 }
 
 return m_tab_descriptors[selection];
}


void CParamDeskDlg::OnDestroy() 
{  
 CDialog::OnDestroy();	  
}

void CParamDeskDlg::SetPosition(int x_pos, int y_pos)
{
 SetWindowPos(NULL,x_pos,y_pos,0,0,SWP_NOZORDER|SWP_NOSIZE);
}

void CParamDeskDlg::SetTitle(const CString& i_str)
{
 m_pd_title.SetWindowText(i_str);
}

void CParamDeskDlg::GetTitle(CString& o_str)
{
 m_pd_title.GetWindowText(o_str);
}

//разрешение/запрещение
void CParamDeskDlg::Enable(bool enable)
{
 m_enabled = (enable) ? TRUE : FALSE;
 m_pStarterPageDlg->Enable(enable);		 
 m_pAnglesPageDlg->Enable(enable);
 m_pIdlRegPageDlg->Enable(enable);
 m_pFunSetPageDlg->Enable(enable);
 m_pTemperPageDlg->Enable(enable);
 m_pCarburPageDlg->Enable(enable);
 m_pADCCompenPageDlg->Enable(enable);
 m_pCKPSPageDlg->Enable(enable);
 if (m_show_knock_page)
  m_pKnockPageDlg->Enable(enable);
 m_pMiscPageDlg->Enable(enable);

 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);

 m_tab_control.EnableItem(-1,enable); //all items  
}

//спрятать/показать все
void CParamDeskDlg::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 m_tab_control.ShowWindow(nCmdShow);
}

// Устанавливает данные указанной вкладки, вкладка определяется дескриптором
bool CParamDeskDlg::SetValues(BYTE i_descriptor, const void* i_values)
{
 using namespace SECU3IO;
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   m_pTemperPageDlg->SetValues((TemperPar*)i_values);
   break;
  case CARBUR_PAR:
   m_pCarburPageDlg->SetValues((CarburPar*)i_values);
   break;
  case IDLREG_PAR: 
   m_pIdlRegPageDlg->SetValues((IdlRegPar*)i_values);
   break;
  case ANGLES_PAR:
   m_pAnglesPageDlg->SetValues((AnglesPar*)i_values);
   break;
  case FUNSET_PAR:
   m_pFunSetPageDlg->SetValues((FunSetPar*)i_values);
   break;
  case STARTR_PAR:
   m_pStarterPageDlg->SetValues((StartrPar*)i_values);
   break;
  case ADCCOR_PAR:
   m_pADCCompenPageDlg->SetValues((ADCCompenPar*)i_values);
   break;
  case CKPS_PAR:
   m_pCKPSPageDlg->SetValues((CKPSPar*)i_values);
   break;
  case KNOCK_PAR:
   if (!m_show_knock_page)
    return false;
   m_pKnockPageDlg->SetValues((KnockPar*)i_values);
   break;
  case MISCEL_PAR:
   m_pMiscPageDlg->SetValues((MiscelPar*)i_values);
   break;
  case FNNAME_DAT:     
  case SENSOR_DAT:					      
  default:
   return false; //неизвестный или неподдерживаемый дескриптор
 }//switch 
  
 return true;
}

// Получает данные из указанной вкладки, вкладка определяется дескриптором
bool CParamDeskDlg::GetValues(BYTE i_descriptor, void* o_values)
{
 using namespace SECU3IO;
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   m_pTemperPageDlg->GetValues((TemperPar*)o_values);
   break;
  case CARBUR_PAR:
   m_pCarburPageDlg->GetValues((CarburPar*)o_values);
   break;
  case IDLREG_PAR: 
   m_pIdlRegPageDlg->GetValues((IdlRegPar*)o_values);
   break;
  case ANGLES_PAR:
   m_pAnglesPageDlg->GetValues((AnglesPar*)o_values);
   break;
  case FUNSET_PAR:
   m_pFunSetPageDlg->GetValues((FunSetPar*)o_values);
   break;
  case STARTR_PAR:
   m_pStarterPageDlg->GetValues((StartrPar*)o_values);
   break;
  case ADCCOR_PAR:
   m_pADCCompenPageDlg->GetValues((ADCCompenPar*)o_values);
   break;
  case CKPS_PAR:
   m_pCKPSPageDlg->GetValues((CKPSPar*)o_values);
   break;
  case KNOCK_PAR:
   if (!m_show_knock_page)
    return false;
   m_pKnockPageDlg->GetValues((KnockPar*)o_values);
   break;
  case MISCEL_PAR:
   m_pMiscPageDlg->GetValues((MiscelPar*)o_values);
   break;
  case FNNAME_DAT:     
  case SENSOR_DAT:					      
  default:
   return false; //неизвестный или неподдерживаемый дескриптор
 }//switch        

 return true;
}


//Устанавливает имена семейств характеристик
void CParamDeskDlg::SetFunctionsNames(const std::vector<_TSTRING>& i_names)
{
 m_pFunSetPageDlg->AccessFunNames() = i_names;
 m_pFunSetPageDlg->FillCBByFunNames();
}

void CParamDeskDlg::OnSaveButton()
{
 if (m_OnSaveButton)
  m_OnSaveButton();
}

void CParamDeskDlg::ShowSaveButton(bool i_show)
{
 m_save_button.ShowWindow(i_show ? SW_SHOW : SW_HIDE);
}

