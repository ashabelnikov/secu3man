 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "FunSetPageDlg.h"
#include "ui-core/ddx_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CFunSetPageDlg::CFunSetPageDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CFunSetPageDlg::IDD, pParent)
, m_enabled(FALSE)
{
  m_params.map_grad = 4.5f;
  m_params.press_swing = 10.0f;
  m_params.fn_benzin = 0;
  m_params.fn_gas = 1;
  //{{AFX_DATA_INIT(CFunSetPageDlg)
	// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

LPCTSTR CFunSetPageDlg::GetDialogID(void) const
{
	return (LPCTSTR)IDD; 
}

void CFunSetPageDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFunSetPageDlg)
  DDX_Control(pDX, IDC_PD_FUNSET_GAS_MAPS_COMBO, m_gas_maps_combo);
  DDX_Control(pDX, IDC_PD_FUNSET_BENZIN_MAPS_COMBO, m_benzin_maps_combo);
  DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_SPIN, m_press_swing_spin);
  DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_press_swing_edit);
  DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_SPIN, m_map_grad_spin);
  DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_map_grad_edit);
  //}}AFX_DATA_MAP

  m_map_grad_edit.DDX_Value(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_params.map_grad);
  m_press_swing_edit.DDX_Value(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_params.press_swing);
  DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_BENZIN_MAPS_COMBO, m_params.fn_benzin);
  DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_GAS_MAPS_COMBO, m_params.fn_gas);
}


BEGIN_MESSAGE_MAP(CFunSetPageDlg, CDialog)
  //{{AFX_MSG_MAP(CFunSetPageDlg)
  ON_CBN_SELCHANGE(IDC_PD_FUNSET_BENZIN_MAPS_COMBO, OnSelchangePdFunsetBenzinMapsCombo)
  ON_CBN_SELCHANGE(IDC_PD_FUNSET_GAS_MAPS_COMBO, OnSelchangePdFunsetGasMapsCombo)
  ON_EN_CHANGE(IDC_PD_FUNSET_MAP_GRAD_EDIT, OnChangePdFunsetMapGradEdit)
  ON_EN_CHANGE(IDC_PD_FUNSET_PRESS_SWING_EDIT, OnChangePdFunsetPressSwingEdit)

  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_EDIT,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_SPIN,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_CAPTION,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_GRAD_UNIT,OnUpdateControls)

  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_EDIT,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_SPIN,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_CAPTION,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_PRESS_SWING_UNIT,OnUpdateControls)

  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_BENZIN_MAPS_COMBO,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_BENZIN_MAPS_COMBO_CAPTION,OnUpdateControls)

  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_GAS_MAPS_COMBO,OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_GAS_MAPS_COMBO_CAPTION,OnUpdateControls)

  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFunSetPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CFunSetPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_enabled);
}


BOOL CFunSetPageDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
	
  m_map_grad_spin.SetBuddy(&m_map_grad_edit);
  m_map_grad_edit.SetLimitText(4);
  m_map_grad_edit.SetDecimalPlaces(2);
  m_map_grad_edit.SetMode(CEditEx::MODE_FLOAT);
  m_map_grad_spin.SetRangeAndDelta(0.25,25.0f,0.125f);
	
  m_press_swing_spin.SetBuddy(&m_press_swing_edit);
  m_press_swing_edit.SetLimitText(4);
  m_press_swing_edit.SetDecimalPlaces(2);
  m_press_swing_edit.SetMode(CEditEx::MODE_FLOAT);
  m_press_swing_spin.SetRangeAndDelta(0.25,50.0f,0.25f);

  FillCBByFunNames(); //инициализируем контент ComboBox-ов семейств характеристик
  UpdateData(FALSE);  //инициализируем контроллы диалога данными    
  UpdateDialogControls(this,TRUE);

  return TRUE;  // return TRUE unless you set the focus to a control
	            // EXCEPTION: OCX Property Pages should return FALSE
}


void CFunSetPageDlg::OnSelchangePdFunsetBenzinMapsCombo() 
{
  UpdateData();
  OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)  		
}

void CFunSetPageDlg::OnSelchangePdFunsetGasMapsCombo() 
{
  UpdateData();	
  OnChangeNotify();     
}

void CFunSetPageDlg::OnChangePdFunsetMapGradEdit() 
{
  UpdateData();	
  OnChangeNotify();     
}

void CFunSetPageDlg::OnChangePdFunsetPressSwingEdit() 
{
  UpdateData();		
  OnChangeNotify();     
}

//разрешение/запрещение контроллов (всех поголовно)
void CFunSetPageDlg::Enable(bool enable)
{
  m_enabled = (enable) ? TRUE : FALSE;  
  if (::IsWindow(m_hWnd))
    UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CFunSetPageDlg::IsEnabled(void)
{
  return (m_enabled) ? true : false;
}



//TODO: Если будет необходимость использовать режим сортировки, то для корректного
//ассоциирования элементов можно использовать CComboBox::SetItemData. В любом случае
//должна измениться только внутренняя архитектура этого диалога

//заполняет ComboBox-ы имен семейств характеристик
void CFunSetPageDlg::FillCBByFunNames(void)
{
  if (!::IsWindow(m_hWnd))
	return;

  //удаляем предыдущий контент
  m_benzin_maps_combo.ResetContent();
  m_gas_maps_combo.ResetContent();
	  
  //добавляем новый контент
  for(size_t i = 0; i < m_fun_names.size(); i++)
  {
	m_benzin_maps_combo.AddString(m_fun_names[i].c_str());
    m_gas_maps_combo.AddString(m_fun_names[i].c_str());
  }

  //после удаления всех элементов текущее выделение в ComboBox-ах
  //теряется и мы должны вернуть старое выделение.

  //для газа
  if (m_params.fn_gas < m_fun_names.size())
    m_gas_maps_combo.SetCurSel(m_params.fn_gas);
  else
    m_gas_maps_combo.SetCurSel(0);

  //для бензиа
  if (m_params.fn_benzin < m_fun_names.size())
    m_benzin_maps_combo.SetCurSel(m_params.fn_benzin);
  else
    m_benzin_maps_combo.SetCurSel(0);
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CFunSetPageDlg::GetValues(SECU3IO::FunSetPar* o_values)
{
  ASSERT(o_values);
  UpdateData(TRUE); //копируем данные из диалога в переменные
  memcpy(o_values,&m_params,sizeof(SECU3IO::FunSetPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CFunSetPageDlg::SetValues(const SECU3IO::FunSetPar* i_values)
{
  ASSERT(i_values);
  memcpy(&m_params,i_values,sizeof(SECU3IO::FunSetPar));
  UpdateData(FALSE); //копируем данные из переменных в диалог
}


