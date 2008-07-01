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


CParamDeskDlg::CParamDeskDlg(CWnd* pParent /*=NULL*/)
: CUpdatableDialog(CParamDeskDlg::IDD, pParent)
, m_pImgList(NULL)
//, m_lastSel(0)
, m_enabled(FALSE)
{
  //{{AFX_DATA_INIT(CParamDeskDlg)
	// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
   
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
}


void CParamDeskDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CParamDeskDlg)
  DDX_Control(pDX, IDC_PARAMETERS_DESK_TITLE, m_pd_title);
  DDX_Control(pDX, IDC_PD_TAB_CTRL, m_tab_control);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParamDeskDlg, CDialog)
  //{{AFX_MSG_MAP(CParamDeskDlg)
  ON_WM_DESTROY()
  ON_UPDATE_COMMAND_UI(IDC_PARAMETERS_DESK_TITLE,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_TAB_CTRL,OnUpdateControls)
  //}}AFX_MSG_MAP
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
	
  //наполняем Tab control вкладками	
  m_tab_control.AddPage("Запуск",m_pStarterPageDlg,0);
  m_tab_control.AddPage("УОЗ",m_pAnglesPageDlg,1);	
  m_tab_control.AddPage("ХХ",m_pIdlRegPageDlg,2);
  m_tab_control.AddPage("Функции",m_pFunSetPageDlg,3);
  m_tab_control.AddPage("Температура",m_pTemperPageDlg,4);
  m_tab_control.AddPage("Карбюратор",m_pCarburPageDlg,5);
  m_tab_control.AddPage("Компенсация погрешностей АЦП",m_pADCCompenPageDlg,6);
	
  //ВНИМАНИЕ! SetEventListener должен быть вызван раньше чем SetCurSel, т.к. SetCurSel 
  //уже использует обработчики сообщений!

  //this будет получать события от Tab control-а и делегировать их указанным обработчикам
  m_tab_control.SetEventListener(this);  

  //выбираем ранее выбранную вкладку
 // m_tab_control.SetCurSel(m_lastSel);

  //устанавливаем предыдущее значение (разрешены вкладки или нет)
  m_tab_control.EnableItem(-1,m_enabled ? true : false);

  UpdateDialogControls(this,TRUE);
  return TRUE;  // return TRUE unless you set the focus to a control
	            // EXCEPTION: OCX Property Pages should return FALSE
}

BYTE CParamDeskDlg::GetCurrentDescriptor(void)
{
  switch(m_tab_control.GetCurSel())
  {
    case 0:	
     return STARTR_PAR;
    case 1:	
     return ANGLES_PAR;
    case 2:	
     return IDLREG_PAR;
    case 3:	
     return FUNSET_PAR;
    case 4:	
     return TEMPER_PAR;
    case 5:	
     return CARBUR_PAR;	
    case 6:	
     return ADCCOR_PAR;	
    default:
     return 0; //invalid case
  }
}


void CParamDeskDlg::OnDestroy() 
{
  //запоминаем какая вкладка была выбрана
  //m_lastSel = m_tab_control.GetCurSel();

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

