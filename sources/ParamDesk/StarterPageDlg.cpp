 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "StarterPageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CStarterPageDlg::CStarterPageDlg(CWnd* pParent /*=NULL*/)
	: CTabDialog(CStarterPageDlg::IDD, pParent)
	, m_enabled(FALSE)
{
	//{{AFX_DATA_INIT(CStarterPageDlg)
	//}}AFX_DATA_INIT
    m_params.starter_off  = 600;
	m_params.smap_abandon = 700;	
}

LPCTSTR CStarterPageDlg::GetDialogID(void) const
{
	return (LPCTSTR)IDD; 
}

void CStarterPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStarterPageDlg)
	DDX_Control(pDX, IDC_PD_STARTER_SMAP_ABANDON_RPM_SPIN, m_smap_abandon_rpm_spin);
	DDX_Control(pDX, IDC_PD_STARTER_OFF_RPM_SPIN, m_starter_off_rpm_spin);
	DDX_Control(pDX, IDC_PD_STARTER_SMAP_ABANDON_RPM_EDIT, m_smap_abandon_rpm_edit);
	DDX_Control(pDX, IDC_PD_STARTER_OFF_RPM_EDIT, m_starter_off_rpm_edit);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_PD_STARTER_OFF_RPM_EDIT, m_params.starter_off);
	DDX_Text(pDX, IDC_PD_STARTER_SMAP_ABANDON_RPM_EDIT, m_params.smap_abandon);
}


BEGIN_MESSAGE_MAP(CStarterPageDlg, CDialog)
	//{{AFX_MSG_MAP(CStarterPageDlg)
	ON_EN_CHANGE(IDC_PD_STARTER_OFF_RPM_EDIT, OnChangePdStarterOffRpmEdit)
	ON_EN_CHANGE(IDC_PD_STARTER_SMAP_ABANDON_RPM_EDIT, OnChangePdStarterSmapAbandonRpmEdit)
	
	ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_SMAP_ABANDON_RPM_SPIN,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_SMAP_ABANDON_RPM_CAPTION,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_SMAP_ABANDON_RPM_UNIT,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_SMAP_ABANDON_RPM_EDIT,OnUpdateControls)

	ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_OFF_RPM_SPIN,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_OFF_RPM_CAPTION,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_OFF_RPM_UNIT,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_OFF_RPM_EDIT,OnUpdateControls)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CStarterPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_enabled);
}


/////////////////////////////////////////////////////////////////////////////
// CStarterPageDlg message handlers

BOOL CStarterPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_starter_off_rpm_edit.SetLimitText(4);
	m_smap_abandon_rpm_spin.SetBuddy(&m_smap_abandon_rpm_edit);
	m_smap_abandon_rpm_spin.SetRangeAndDelta(40,1000,10);

	m_smap_abandon_rpm_edit.SetLimitText(4);
	m_starter_off_rpm_spin.SetBuddy(&m_starter_off_rpm_edit);
    m_starter_off_rpm_spin.SetRangeAndDelta(40,1000,10);

	UpdateData(FALSE);
	UpdateDialogControls(this,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStarterPageDlg::OnChangePdStarterOffRpmEdit() 
{
  UpdateData();
  OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)  		  
}

void CStarterPageDlg::OnChangePdStarterSmapAbandonRpmEdit() 
{
  UpdateData();	
  OnChangeNotify();  		  
}

//разрешение/запрещение контроллов (всех поголовно)
void CStarterPageDlg::Enable(bool enable)
{
  m_enabled = (enable) ? TRUE : FALSE;  
  if (::IsWindow(m_hWnd))
    UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CStarterPageDlg::IsEnabled(void)
{
  return (m_enabled) ? true : false;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CStarterPageDlg::GetValues(SECU3IO::StartrPar* o_values)
{
  ASSERT(o_values);
  UpdateData(TRUE); //копируем данные из диалога в переменные
  memcpy(o_values,&m_params,sizeof(SECU3IO::StartrPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CStarterPageDlg::SetValues(const SECU3IO::StartrPar* i_values)
{
  ASSERT(i_values);
  memcpy(&m_params,i_values,sizeof(SECU3IO::StartrPar));
  UpdateData(FALSE); //копируем данные из переменных в диалог
}
