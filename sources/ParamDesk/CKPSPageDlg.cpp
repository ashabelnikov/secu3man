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
#include "CKPSPageDlg.h"
#include "ui-core/ddx_helpers.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT CCKPSPageDlg::IDD = IDD_PD_CKPS_PAGE;

CCKPSPageDlg::CCKPSPageDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CCKPSPageDlg::IDD, pParent)
, m_enabled(FALSE)
, m_ignition_cogs_edit(CEditEx::MODE_INT)
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
 DDX_Radio_UCHAR(pDX,IDC_PD_CKPS_NEGFRONT_RADIOBOX,m_params.ckps_edge_type);
}


BEGIN_MESSAGE_MAP(CCKPSPageDlg, CDialog)
 //{{AFX_MSG_MAP(CCKPSPageDlg)
 ON_CBN_SELCHANGE(IDC_PD_CKPS_COGS_BEFORE_TDC_COMBOBOX, OnSelchangePdCogsBTDCCombo)
 ON_EN_CHANGE(IDC_PD_CKPS_IGNITION_COGS_EDIT, OnChangePdIgnitionCogsEdit)
 ON_BN_CLICKED(IDC_PD_CKPS_POSFRONT_RADIOBOX,OnClickedPdPosFrontRadio)
 ON_BN_CLICKED(IDC_PD_CKPS_NEGFRONT_RADIOBOX,OnClickedPdNegFrontRadio)

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
 m_ignition_cogs_edit.SetDecimalPlaces(2);
 m_ignition_cogs_spin.SetBuddy(&m_ignition_cogs_edit);
 m_ignition_cogs_spin.SetRangeAndDelta(1,55,1);

 _FillCKPSTeethBTDCComboBox(); //инициализируем комбо бокс числа зубьев до в.м.т.
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

void CCKPSPageDlg::OnClickedPdPosFrontRadio()
{  
 m_ckps_negfront_radio.SetCheck(0);  
 UpdateData();		
 OnChangeNotify();     
}

void CCKPSPageDlg::OnClickedPdNegFrontRadio()
{  
 m_ckps_posfront_radio.SetCheck(0);
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

//эту функцию необходимо использовать когда надо получить данные из диалога
void CCKPSPageDlg::GetValues(SECU3IO::CKPSPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 m_params.ckps_cogs_btdc = _GetCKPSTeethBTDCComboBoxSelection();
 memcpy(o_values,&m_params,sizeof(SECU3IO::CKPSPar)); 
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CCKPSPageDlg::SetValues(const SECU3IO::CKPSPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values,sizeof(SECU3IO::CKPSPar));

 //комбо бокс
 _SetCKPSTeethBTDCComboBoxSelection(m_params.ckps_cogs_btdc);
   
 //устанавливаем состояние контлоллов фронта ДПКВ
 m_ckps_posfront_radio.SetCheck(m_params.ckps_edge_type);
 m_ckps_negfront_radio.SetCheck(~m_params.ckps_edge_type);

 UpdateData(FALSE); //копируем данные из переменных в диалог
}


void CCKPSPageDlg::_FillCKPSTeethBTDCComboBox(void)
{
 m_cogs_numbers.push_back(std::make_pair(1,_TSTRING(_T("1"))));
 m_cogs_numbers.push_back(std::make_pair(2,_TSTRING(_T("2"))));
 m_cogs_numbers.push_back(std::make_pair(3,_TSTRING(_T("3"))));
 m_cogs_numbers.push_back(std::make_pair(4,_TSTRING(_T("4"))));
 m_cogs_numbers.push_back(std::make_pair(5,_TSTRING(_T("5"))));
 m_cogs_numbers.push_back(std::make_pair(6,_TSTRING(_T("6"))));
 m_cogs_numbers.push_back(std::make_pair(7,_TSTRING(_T("7"))));
 m_cogs_numbers.push_back(std::make_pair(8,_TSTRING(_T("8"))));
 m_cogs_numbers.push_back(std::make_pair(9,_TSTRING(_T("9"))));
 m_cogs_numbers.push_back(std::make_pair(10,_TSTRING(_T("10"))));
 m_cogs_numbers.push_back(std::make_pair(11,_TSTRING(_T("11"))));
 m_cogs_numbers.push_back(std::make_pair(12,_TSTRING(_T("12"))));
 m_cogs_numbers.push_back(std::make_pair(13,_TSTRING(_T("13"))));
 m_cogs_numbers.push_back(std::make_pair(14,_TSTRING(_T("14"))));
 m_cogs_numbers.push_back(std::make_pair(15,_TSTRING(_T("15"))));
 m_cogs_numbers.push_back(std::make_pair(16,_TSTRING(_T("16"))));
 m_cogs_numbers.push_back(std::make_pair(17,_TSTRING(_T("17"))));
 m_cogs_numbers.push_back(std::make_pair(18,_TSTRING(_T("18"))));
 m_cogs_numbers.push_back(std::make_pair(19,_TSTRING(_T("19"))));
 m_cogs_numbers.push_back(std::make_pair(20,_TSTRING(_T("20"))));
 m_cogs_numbers.push_back(std::make_pair(21,_TSTRING(_T("21"))));
 m_cogs_numbers.push_back(std::make_pair(22,_TSTRING(_T("22"))));
 m_cogs_numbers.push_back(std::make_pair(23,_TSTRING(_T("23"))));
 m_cogs_numbers.push_back(std::make_pair(24,_TSTRING(_T("24"))));
 m_cogs_numbers.push_back(std::make_pair(25,_TSTRING(_T("25"))));


 m_teeth_before_tdc_combo.ResetContent();
 for(size_t i = 0; i < m_cogs_numbers.size(); i++)
 {
  int index = m_teeth_before_tdc_combo.AddString(m_cogs_numbers[i].second.c_str());
  if (index==CB_ERR)
  {
   ASSERT(0);
   continue;
  }
  m_teeth_before_tdc_combo.SetItemData(index, i);
 }
}

int CCKPSPageDlg::_GetCKPSTeethBTDCComboBoxSelection(void)
{
 int index = m_teeth_before_tdc_combo.GetCurSel();
 if (index==CB_ERR)
 {
  ASSERT(0);
  return 0;
 }
 int cog_index = m_teeth_before_tdc_combo.GetItemData(index);
 return m_cogs_numbers[cog_index].first;
}

void CCKPSPageDlg::_SetCKPSTeethBTDCComboBoxSelection(int i_sel)
{
 for(size_t i = 0; i < m_cogs_numbers.size(); i++)
 {
  if (m_cogs_numbers[i].first != i_sel) //find index in conntainer for cog number
   continue;
  //find related index and select corresponding item
  int count = m_teeth_before_tdc_combo.GetCount();
  for(int ii = 0; ii < count; ii++)
  if (m_teeth_before_tdc_combo.GetItemData(ii) == i)
  {
   m_teeth_before_tdc_combo.SetCurSel(ii);	
   return;
  }  
 }
 ASSERT(0);
}
