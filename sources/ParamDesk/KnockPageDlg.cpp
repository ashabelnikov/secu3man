
#include "stdafx.h"
#include "KnockPageDlg.h"
#include "io-core/numericconv.h"

#include <vector>

using namespace std;
#pragma warning( disable : 4800 ) //: forcing value to bool 'true' or 'false' (performance warning)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CKnockPageDlg::CKnockPageDlg(CWnd* pParent /*=NULL*/)
	: Super(CKnockPageDlg::IDD, pParent)	
{
    m_params.knock_use_knock_channel = 0;
	m_params.knock_bpf_frequency = 40;
	m_params.knock_k_wnd_begin_angle = 0.0f;
	m_params.knock_k_wnd_end_angle = 48.0f;
	
	//{{AFX_DATA_INIT(CKnockPageDlg)
	//}}AFX_DATA_INIT
}

LPCTSTR CKnockPageDlg::GetDialogID(void) const
{
	return (LPCTSTR)IDD; 
}

void CKnockPageDlg::DoDataExchange(CDataExchange* pDX)
{
	Super::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKnockPageDlg)
    DDX_Control(pDX, IDC_PROPERTY_GRID, m_ctrlGrid);		
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(CKnockPageDlg, Super)
	//{{AFX_MSG_MAP(CKnockPageDlg)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PG_ITEMCHANGED, OnItemChanged)
	ON_UPDATE_COMMAND_UI(IDC_PROPERTY_GRID,OnUpdateControls)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCarburPageDlg message handlers


//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CKnockPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_enabled);
}

BOOL CKnockPageDlg::OnInitDialog() 
{
  Super::OnInitDialog();

  HSECTION hs = m_ctrlGrid.AddSection("Параметры");
  //-----------------------------------------------------------------
  m_knock_use_knock_channel_item = m_ctrlGrid.AddBoolItem(hs, "Разрешить контроль детонации", m_params.knock_use_knock_channel);
  //-----------------------------------------------------------------
  CPropertyGridInPlaceEdit::InplaceEditParamsEx ex1;
  ex1.m_decimal_places = 2;
  ex1.m_spin = true;
  ex1.m_delta = 6.0;
  ex1.m_mode = CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED;
  ex1.m_lower = -12.0;
  ex1.m_upper = 54.0;
  ex1.m_limit_text = 6;
  m_knock_k_wnd_begin_angle_item = m_ctrlGrid.AddDoubleItem(hs, "Начало фазового окна", m_params.knock_k_wnd_begin_angle,"%g°",true,false,-1,&ex1);
  //-----------------------------------------------------------------
  CPropertyGridInPlaceEdit::InplaceEditParamsEx ex2;
  ex2.m_decimal_places = 2;
  ex2.m_spin = true;
  ex2.m_delta = 6.0;
  ex2.m_mode = CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED;
  ex2.m_lower = -12.0;
  ex2.m_upper = 54.0;
  ex2.m_limit_text = 6;
  m_knock_k_wnd_end_angle_item = m_ctrlGrid.AddDoubleItem(hs, "Конец фазового окна", m_params.knock_k_wnd_end_angle,"%g°",true,false,-1,&ex2);
  //-----------------------------------------------------------------  
  vector<_TSTRING> bpf_freqs;
  for (size_t i = 0; i < 64; i++) //заполняем комбо бокс частот ПФ
  { 
   CString string;
   string.Format(_T("%.2f"),SECU3IO::hip9011_gain_frequences[i]);
   bpf_freqs.push_back(_TSTRING(string));
  }
  m_freq_selector.SetValuesList(bpf_freqs);
  m_freq_selector.SetSelectedIndex(CNumericConv::Round(m_params.knock_bpf_frequency));
  m_freq_selector.SetUnitString(_T("кГц"));
  m_knock_bpf_frequency_item = m_ctrlGrid.AddCustomItem(hs, _T("Частота ПФ"), &m_freq_selector);
  //-----------------------------------------------------------------

  		
  UpdateDialogControls(this,TRUE);
  return TRUE;  // return TRUE unless you set the focus to a control
	            // EXCEPTION: OCX Property Pages should return FALSE
}

void CKnockPageDlg::OnChangeData() 
{
  UpdateData();	
  OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)  
}

//разрешение/запрещение контроллов (всех поголовно)
void CKnockPageDlg::Enable(bool enable)
{
  m_enabled = (enable) ? TRUE : FALSE;  
  if (::IsWindow(m_hWnd))
  {
   UpdateDialogControls(this,TRUE);
   m_ctrlGrid.SetEditable(m_enabled);
  }
}

//что с контроллами?
bool CKnockPageDlg::IsEnabled(void)
{
  return (m_enabled) ? true : false;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CKnockPageDlg::GetValues(SECU3IO::KnockPar* o_values)
{
  ASSERT(o_values);
  UpdateData(TRUE); //копируем данные из диалога в переменные

  bool status = true;

  bool knock_use_knock_channel;
  if (!m_ctrlGrid.GetItemValue(m_knock_use_knock_channel_item,knock_use_knock_channel))
   status = false;
  m_params.knock_use_knock_channel = knock_use_knock_channel;

  double knock_k_wnd_begin_angle;
  if (!m_ctrlGrid.GetItemValue(m_knock_k_wnd_begin_angle_item,knock_k_wnd_begin_angle))
   status = false;
  m_params.knock_k_wnd_begin_angle = (float)knock_k_wnd_begin_angle;

  double knock_k_wnd_end_angle;
  if (!m_ctrlGrid.GetItemValue(m_knock_k_wnd_end_angle_item,knock_k_wnd_end_angle))
   status = false;
  m_params.knock_k_wnd_end_angle = (float)knock_k_wnd_end_angle;

  size_t knock_bpf_frequency  = m_freq_selector.GetSelectedIndex();
  m_params.knock_bpf_frequency = (float)knock_bpf_frequency;

  ASSERT(status);

  memcpy(o_values,&m_params,sizeof(SECU3IO::KnockPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CKnockPageDlg::SetValues(const SECU3IO::KnockPar* i_values)
{
  ASSERT(i_values);
  memcpy(&m_params,i_values,sizeof(SECU3IO::KnockPar));
  UpdateData(FALSE); //копируем данные из переменных в диалог

  bool status = true;

  bool knock_use_knock_channel = m_params.knock_use_knock_channel;
  if (!m_ctrlGrid.SetItemValue(m_knock_use_knock_channel_item,knock_use_knock_channel))
   status = false;

  double knock_k_wnd_begin_angle = m_params.knock_k_wnd_begin_angle;
  if (!m_ctrlGrid.SetItemValue(m_knock_k_wnd_begin_angle_item,knock_k_wnd_begin_angle))
   status = false;

  double knock_k_wnd_end_angle = m_params.knock_k_wnd_end_angle;
  if (!m_ctrlGrid.SetItemValue(m_knock_k_wnd_end_angle_item,knock_k_wnd_end_angle))
   status = false;

  size_t knock_bpf_frequency = CNumericConv::Round(m_params.knock_bpf_frequency);
  m_freq_selector.SetSelectedIndex(knock_bpf_frequency);

  ASSERT(status);
}


LRESULT CKnockPageDlg::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
  if (wParam == m_knock_use_knock_channel_item ||
	  wParam == m_knock_k_wnd_begin_angle_item ||
	  wParam == m_knock_k_wnd_end_angle_item   ||
	  wParam == m_knock_bpf_frequency_item)
  {
   OnChangeNotify();
  }
  else
  {
  ASSERT(0);
  }

  return 0;
}

void CKnockPageDlg::OnDestroy() 
{
  Super::OnDestroy();
  m_ctrlGrid.ResetContents();
}