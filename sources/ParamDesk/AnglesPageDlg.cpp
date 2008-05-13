 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "AnglesPageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAnglesPageDlg::CAnglesPageDlg(CWnd* pParent /*=NULL*/)
	: CTabDialog(CAnglesPageDlg::IDD, pParent)
	, m_enabled(FALSE)
{
    m_params.max_angle = 60.0f;
	m_params.min_angle = -15.0f;
	m_params.angle_corr = 0.0f;

    //{{AFX_DATA_INIT(CAnglesPageDlg)
	//}}AFX_DATA_INIT
}

LPCTSTR CAnglesPageDlg::GetDialogID(void) const
{
	return (LPCTSTR)IDD; 
}

void CAnglesPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnglesPageDlg)
	DDX_Control(pDX, IDC_PD_ANGLES_MIN_ANGLE_SPIN, m_min_angle_spin);
	DDX_Control(pDX, IDC_PD_ANGLES_MAX_ANGLE_SPIN, m_max_angle_spin);
	DDX_Control(pDX, IDC_PD_ANGLES_CORRECTION_SPIN, m_correction_spin);
	DDX_Control(pDX, IDC_PD_ANGLES_MIN_ANGLE_EDIT, m_min_angle_edit);
	DDX_Control(pDX, IDC_PD_ANGLES_MAX_ANGLE_EDIT, m_max_angle_edit);
	DDX_Control(pDX, IDC_PD_ANGLES_CORRECTION_EDIT, m_correction_edit);
	//}}AFX_DATA_MAP

	m_max_angle_edit.DDX_Value(pDX, IDC_PD_ANGLES_MAX_ANGLE_EDIT, m_params.max_angle);
	m_min_angle_edit.DDX_Value(pDX, IDC_PD_ANGLES_MIN_ANGLE_EDIT, m_params.min_angle);
	m_correction_edit.DDX_Value(pDX, IDC_PD_ANGLES_CORRECTION_EDIT, m_params.angle_corr);
}


BEGIN_MESSAGE_MAP(CAnglesPageDlg, CDialog)
	//{{AFX_MSG_MAP(CAnglesPageDlg)
	ON_EN_CHANGE(IDC_PD_ANGLES_MIN_ANGLE_EDIT, OnChangePdAnglesMinAngleEdit)
	ON_EN_CHANGE(IDC_PD_ANGLES_MAX_ANGLE_EDIT, OnChangePdAnglesMaxAngleEdit)
	ON_EN_CHANGE(IDC_PD_ANGLES_CORRECTION_EDIT, OnChangePdAnglesCorrectionEdit)

	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_EDIT, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_SPIN, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_CAPTION, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_UNIT, OnUpdateControls)

	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_EDIT, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_SPIN, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_CAPTION, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_UNIT, OnUpdateControls)

	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_EDIT, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_SPIN, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_CAPTION, OnUpdateControls)
	ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_UNIT, OnUpdateControls)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnglesPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CAnglesPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_enabled);
}



BOOL CAnglesPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_min_angle_spin.SetBuddy(&m_min_angle_edit);
	m_min_angle_edit.SetLimitText(4);
    m_min_angle_edit.SetDecimalPlaces(2);
    m_min_angle_edit.SetMode(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED);
    m_min_angle_spin.SetRangeAndDelta(-15.0f,60.0f,0.25f);

	m_max_angle_spin.SetBuddy(&m_max_angle_edit);
	m_max_angle_edit.SetLimitText(4);
    m_max_angle_edit.SetDecimalPlaces(2);
    m_max_angle_edit.SetMode(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED);
    m_max_angle_spin.SetRangeAndDelta(-15.0f,60.0f,0.25f);
	
	m_correction_spin.SetBuddy(&m_correction_edit);
	m_correction_edit.SetLimitText(4);
    m_correction_edit.SetDecimalPlaces(2);
    m_correction_edit.SetMode(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED);
    m_correction_spin.SetRangeAndDelta(-30.0f,30.0f,0.25f);


	UpdateData(FALSE);
	UpdateDialogControls(this,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnglesPageDlg::OnChangePdAnglesMinAngleEdit() 
{
  UpdateData();	
  OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CAnglesPageDlg::OnChangePdAnglesMaxAngleEdit() 
{
  UpdateData();			
  OnChangeNotify();
}

void CAnglesPageDlg::OnChangePdAnglesCorrectionEdit() 
{
  UpdateData();			
  OnChangeNotify();
}

//разрешение/запрещение контроллов (всех поголовно)
void CAnglesPageDlg::Enable(bool enable)
{
  m_enabled = (enable) ? TRUE : FALSE;  
  if (::IsWindow(m_hWnd))
    UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CAnglesPageDlg::IsEnabled(void)
{
  return (m_enabled) ? true : false;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CAnglesPageDlg::GetValues(SECU3IO::AnglesPar* o_values)
{
  ASSERT(o_values);
  UpdateData(TRUE); //копируем данные из диалога в переменные
  memcpy(o_values,&m_params,sizeof(SECU3IO::AnglesPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CAnglesPageDlg::SetValues(const SECU3IO::AnglesPar* i_values)
{
  ASSERT(i_values);
  memcpy(&m_params,i_values,sizeof(SECU3IO::AnglesPar));
  UpdateData(FALSE); //копируем данные из переменных в диалог
}
