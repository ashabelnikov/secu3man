 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "CarburPageDlg.h"
#include "ui-core/ddx_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCarburPageDlg::CCarburPageDlg(CWnd* pParent /*=NULL*/)
	: CTabDialog(CCarburPageDlg::IDD, pParent)
	, m_enabled(FALSE)
{
    m_params.ephh_lot = 1250;
	m_params.ephh_hit = 1500;
	m_params.carb_invers = 0;

	//{{AFX_DATA_INIT(CCarburPageDlg)
	//}}AFX_DATA_INIT
}

LPCTSTR CCarburPageDlg::GetDialogID(void) const
{
	return (LPCTSTR)IDD; 
}

void CCarburPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCarburPageDlg)
	DDX_Control(pDX, IDC_PD_CARBUR_INVERSE_SWITCH, m_inverse_throttle_switch);
	DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN, m_shutoff_lo_threshold_spin);
	DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN, m_shutoff_hi_threshold_spin);
	DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, m_shutoff_lo_threshold_edit);
	DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, m_shutoff_hi_threshold_edit);	
	//}}AFX_DATA_MAP

    DDX_Text(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, m_params.ephh_lot);
	DDX_Text(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, m_params.ephh_hit);
    DDX_Check_UCHAR(pDX, IDC_PD_CARBUR_INVERSE_SWITCH, m_params.carb_invers);
}


BEGIN_MESSAGE_MAP(CCarburPageDlg, CDialog)
	//{{AFX_MSG_MAP(CCarburPageDlg)
	ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, OnChangePdCarburShutoffHiThresholdEdit)
	ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, OnChangePdCarburShutoffLoThresholdEdit)
	ON_BN_CLICKED(IDC_PD_CARBUR_INVERSE_SWITCH, OnPdCarburInverseSwitch)

	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_CAPTION,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_UNIT,OnUpdateControls)

	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_CAPTION,OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_UNIT,OnUpdateControls)

	ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_INVERSE_SWITCH,OnUpdateControls)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCarburPageDlg message handlers


//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CCarburPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_enabled);
}

BOOL CCarburPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_shutoff_lo_threshold_edit.SetLimitText(4);
	m_shutoff_lo_threshold_spin.SetBuddy(&m_shutoff_lo_threshold_edit);
	m_shutoff_lo_threshold_spin.SetRangeAndDelta(250,7500,10);

	m_shutoff_hi_threshold_edit.SetLimitText(4);
	m_shutoff_hi_threshold_spin.SetBuddy(&m_shutoff_hi_threshold_edit);
	m_shutoff_hi_threshold_spin.SetRangeAndDelta(250,7500,10);
	
	UpdateDialogControls(this,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCarburPageDlg::OnChangePdCarburShutoffHiThresholdEdit() 
{
  UpdateData();	
  OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)  
}

void CCarburPageDlg::OnChangePdCarburShutoffLoThresholdEdit() 
{
  UpdateData();			
  OnChangeNotify();   
}

void CCarburPageDlg::OnPdCarburInverseSwitch() 
{
  UpdateData();			
  OnChangeNotify();   
}

//разрешение/запрещение контроллов (всех поголовно)
void CCarburPageDlg::Enable(bool enable)
{
  m_enabled = (enable) ? TRUE : FALSE;  
  if (::IsWindow(m_hWnd))
    UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CCarburPageDlg::IsEnabled(void)
{
  return (m_enabled) ? true : false;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CCarburPageDlg::GetValues(SECU3IO::CarburPar* o_values)
{
  ASSERT(o_values);
  UpdateData(TRUE); //копируем данные из диалога в переменные
  memcpy(o_values,&m_params,sizeof(SECU3IO::CarburPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CCarburPageDlg::SetValues(const SECU3IO::CarburPar* i_values)
{
  ASSERT(i_values);
  memcpy(&m_params,i_values,sizeof(SECU3IO::CarburPar));
  UpdateData(FALSE); //копируем данные из переменных в диалог
}
