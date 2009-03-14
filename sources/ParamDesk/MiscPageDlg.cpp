 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MiscPageDlg.h"
#include "ui-core/ddx_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMiscPageDlg::CMiscPageDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CMiscPageDlg::IDD, pParent)
, m_enabled(FALSE)
, m_uart_speed_cb_index(0)
, m_packet_period_edit(CEditEx::MODE_INT)
{
 m_params.baud_rate = CBR_9600;
 m_params.period_ms = 0;
}

LPCTSTR CMiscPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD; 
}

void CMiscPageDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);

 DDX_Control(pDX,IDC_PD_MISC_UART_SPEED_COMBO_CAPTION, m_uart_speed_label);
 DDX_Control(pDX,IDC_PD_MISC_UART_SPEED_COMBO, m_uart_speed_combo);

 DDX_Control(pDX,IDC_PD_MISC_PACKET_PERIOD_EDIT, m_packet_period_edit);
 DDX_Control(pDX,IDC_PD_MISC_PACKET_PERIOD_CAPTION, m_packet_period_label);
 DDX_Control(pDX,IDC_PD_MISC_PACKET_PERIOD_UNIT, m_packet_period_unit);
 DDX_Control(pDX,IDC_PD_MISC_PACKET_PERIOD_SPIN, m_packet_period_spin);

 DDX_CBIndex(pDX, IDC_PD_MISC_UART_SPEED_COMBO, m_uart_speed_cb_index);
 m_packet_period_edit.DDX_Value(pDX, IDC_PD_MISC_PACKET_PERIOD_EDIT, m_params.period_ms);
}


BEGIN_MESSAGE_MAP(CMiscPageDlg, CDialog)
 ON_EN_CHANGE(IDC_PD_MISC_PACKET_PERIOD_EDIT, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_MISC_UART_SPEED_COMBO, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_UART_SPEED_COMBO_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_UART_SPEED_COMBO, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PACKET_PERIOD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PACKET_PERIOD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PACKET_PERIOD_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PACKET_PERIOD_SPIN, OnUpdateControls)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCKPSPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CMiscPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CMiscPageDlg::OnChangeData() 
{
 UpdateData();	
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)  		  
}

BOOL CMiscPageDlg::OnInitDialog() 
{
 CDialog::OnInitDialog();
	
 m_packet_period_edit.SetLimitText(3);
 m_packet_period_edit.SetDecimalPlaces(3);
 m_packet_period_spin.SetBuddy(&m_packet_period_edit);
 m_packet_period_spin.SetRangeAndDelta(0,500,10);

 BRCType br;
 for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
  br.push_back(SECU3IO::secu3_allowable_uart_divisors[i].first);
 FillUARTSpeedComboBox(br); //инициализируем комбо бокс

 UpdateData(FALSE);  //инициализируем контроллы диалога данными    
 UpdateDialogControls(this,TRUE);

 return TRUE;  // return TRUE unless you set the focus to a control
	           // EXCEPTION: OCX Property Pages should return FALSE
}

//разрешение/запрещение контроллов (всех поголовно)
void CMiscPageDlg::Enable(bool enable)
{
 m_enabled = (enable) ? TRUE : FALSE;  
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CMiscPageDlg::IsEnabled(void)
{
 return (m_enabled) ? true : false;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CMiscPageDlg::GetValues(SECU3IO::MiscelPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 m_params.baud_rate = _GetBRFromComboBoxByIndex(m_uart_speed_cb_index);
 memcpy(o_values,&m_params,sizeof(SECU3IO::MiscelPar));  
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CMiscPageDlg::SetValues(const SECU3IO::MiscelPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values,sizeof(SECU3IO::MiscelPar));  
 m_uart_speed_cb_index = _GetIndexFromComboBoxByBR(m_params.baud_rate);
 UpdateData(FALSE); //копируем данные из переменных в диалог
}

void CMiscPageDlg::FillUARTSpeedComboBox(const BRCType& i_baudrates)
{
 m_baudrates.clear();
 m_baudrates = i_baudrates; //make a copy

 m_uart_speed_combo.ResetContent();
 for(size_t i = 0; i < m_baudrates.size(); i++)
 {
  CString string;
  string.Format(_T("%d"), m_baudrates[i]);
  int index = m_uart_speed_combo.AddString(string);
  m_uart_speed_combo.SetItemData(index, static_cast<DWORD>(i));
 }
}

int CMiscPageDlg::_GetBRFromComboBoxByIndex(int i_cb_index)
{
 DWORD item_data = m_uart_speed_combo.GetItemData(i_cb_index);
 ASSERT(item_data < m_baudrates.size()); //invalid index?
 return m_baudrates[static_cast<size_t>(item_data)];
}

int CMiscPageDlg::_GetIndexFromComboBoxByBR(int i_baudrate)
{
 int count = m_uart_speed_combo.GetCount();
 for (int i = 0; i < count; i++)
 {
  DWORD item_data = m_uart_speed_combo.GetItemData(i);
  int baud = m_baudrates[static_cast<size_t>(item_data)];
  if (i_baudrate == baud)
   return i;
 }
 ASSERT(0); //WOW! Unknown baud rate?
 return 0;
}
