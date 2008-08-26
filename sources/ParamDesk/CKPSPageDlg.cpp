 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "CKPSPageDlg.h"
#include "ui-core/ddx_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCKPSPageDlg::CCKPSPageDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CCKPSPageDlg::IDD, pParent)
, m_enabled(FALSE)
, m_teeth_before_tdc_start(18)
{
  m_params.ckps_cogs_btdc = 20;
  m_params.ckps_edge_type = 0;
  m_params.ckps_ignit_cogs = 20;
}

LPCTSTR CCKPSPageDlg::GetDialogID(void) const
{
	return (LPCTSTR)IDD; 
}

void CCKPSPageDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX,IDC_PD_CKPS_FRONT_GROUPBOX, m_ckps_front_groupbox);
  DDX_Control(pDX,IDC_PD_CKPS_POSFRONT_RADIOBOX, m_ckps_posfront_radio);
  DDX_Control(pDX,IDC_PD_CKPS_NEGFRONT_RADIOBOX, m_ckps_negfront_radio);
	
  DDX_Control(pDX,IDC_PD_CKPS_COGS_BEFORE_TDC_LABEL, m_teeth_before_tdc_label);
  DDX_Control(pDX,IDC_PD_CKPS_COGS_BEFORE_TDC_COMBOBOX, m_teeth_before_tdc_combo);

  DDX_Control(pDX,IDC_PD_CKPS_IGNITION_COGS_SPIN, m_ignition_cogs_spin);
  DDX_Control(pDX,IDC_PD_CKPS_IGNITION_COGS_EDIT, m_ignition_cogs_edit);
  DDX_Control(pDX,IDC_PD_CKPS_IGNITION_COGS_UNIT, m_ignition_cogs_label);

  DDX_Text(pDX, IDC_PD_CKPS_IGNITION_COGS_EDIT, m_params.ckps_ignit_cogs);
  DDX_CBIndex_UCHAR(pDX, IDC_PD_CKPS_COGS_BEFORE_TDC_COMBOBOX, m_params.ckps_cogs_btdc);
  DDX_Radio_UCHAR(pDX,IDC_PD_CKPS_NEGFRONT_RADIOBOX,m_params.ckps_edge_type);
}


BEGIN_MESSAGE_MAP(CCKPSPageDlg, CDialog)
  //{{AFX_MSG_MAP(CCKPSPageDlg)
  ON_CBN_SELCHANGE(IDC_PD_CKPS_COGS_BEFORE_TDC_COMBOBOX, OnSelchangePdCogsBTDCCombo)
  ON_EN_CHANGE(IDC_PD_CKPS_IGNITION_COGS_EDIT, OnChangePdIgnitionCogsEdit)
  ON_BN_CLICKED(IDC_PD_CKPS_POSFRONT_RADIOBOX,OnClickedPdFrontRadio)
  ON_BN_CLICKED(IDC_PD_CKPS_NEGFRONT_RADIOBOX,OnClickedPdFrontRadio)

  ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_FRONT_GROUPBOX,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_POSFRONT_RADIOBOX,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_NEGFRONT_RADIOBOX,OnUpdateControls)
  
  ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_COGS_BEFORE_TDC_LABEL,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_COGS_BEFORE_TDC_COMBOBOX,OnUpdateControls)
  
  ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_IGNITION_COGS_SPIN,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_IGNITION_COGS_EDIT,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_CKPS_IGNITION_COGS_UNIT,OnUpdateControls)

  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCKPSPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CCKPSPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_enabled);
}


BOOL CCKPSPageDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
	

  m_ignition_cogs_edit.SetLimitText(2);
  m_ignition_cogs_spin.SetBuddy(&m_ignition_cogs_edit);
  m_ignition_cogs_spin.SetRangeAndDelta(1,55,1);


  FillCBCogsBTDCByNumbers(); //инициализируем комбо бокс числа зубьев до в.м.т.
  UpdateData(FALSE);  //инициализируем контроллы диалога данными    
  UpdateDialogControls(this,TRUE);

  return TRUE;  // return TRUE unless you set the focus to a control
	            // EXCEPTION: OCX Property Pages should return FALSE
}


void CCKPSPageDlg::OnSelchangePdCogsBTDCCombo() 
{
  UpdateData();
  OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)  		
}

void CCKPSPageDlg::OnChangePdIgnitionCogsEdit() 
{
  UpdateData();		
  OnChangeNotify();     
}

void CCKPSPageDlg::OnClickedPdFrontRadio()
{
  UpdateData();		
  OnChangeNotify();     
}

//разрешение/запрещение контроллов (всех поголовно)
void CCKPSPageDlg::Enable(bool enable)
{
  m_enabled = (enable) ? TRUE : FALSE;  
  if (::IsWindow(m_hWnd))
    UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CCKPSPageDlg::IsEnabled(void)
{
  return (m_enabled) ? true : false;
}


void CCKPSPageDlg::FillCBCogsBTDCByNumbers(void)
{
  for(int i = m_teeth_before_tdc_start; i <= m_teeth_before_tdc_start+4; i++)
  {
   CString string;
   string.Format(_T("%d"),i);
   m_teeth_before_tdc_combo.AddString(string);
  }
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CCKPSPageDlg::GetValues(SECU3IO::CKPSPar* o_values)
{
  ASSERT(o_values);
  UpdateData(TRUE); //копируем данные из диалога в переменные
  memcpy(o_values,&m_params,sizeof(SECU3IO::CKPSPar));
  o_values->ckps_cogs_btdc+=m_teeth_before_tdc_start;
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CCKPSPageDlg::SetValues(const SECU3IO::CKPSPar* i_values)
{
  ASSERT(i_values);
  memcpy(&m_params,i_values,sizeof(SECU3IO::CKPSPar));
  m_params.ckps_cogs_btdc-=m_teeth_before_tdc_start;
  UpdateData(FALSE); //копируем данные из переменных в диалог
}


