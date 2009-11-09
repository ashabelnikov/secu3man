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
#include "FunSetPageDlg.h"
#include "ui-core\ddx_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT CFunSetPageDlg::IDD = IDD_PD_FUNSET_PAGE;

BEGIN_MESSAGE_MAP(CFunSetPageDlg, CDialog)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_BENZIN_MAPS_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_FUNSET_GAS_MAPS_COMBO, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_MAP_GRAD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_PRESS_SWING_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_OFFSET_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, OnChangeData)

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

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_OFFSET_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_CURVE_GRADIENT_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_FUNSET_MAP_SENSOR_GROUP,OnUpdateControls)
END_MESSAGE_MAP()

CFunSetPageDlg::CFunSetPageDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CFunSetPageDlg::IDD, pParent)
, m_enabled(FALSE)
, m_map_grad_edit(CEditEx::MODE_FLOAT)
, m_press_swing_edit(CEditEx::MODE_FLOAT)
, m_map_curve_offset_edit(CEditEx::MODE_FLOAT)
, m_map_curve_gradient_edit(CEditEx::MODE_FLOAT)

{
 m_params.map_lower_pressure = 4.5f;
 m_params.map_upper_pressure = 10.0f;
 m_params.fn_benzin = 0;
 m_params.fn_gas = 1;
 m_params.map_curve_offset = 0.547f;
 m_params.map_curve_gradient = 20.9f;
}

LPCTSTR CFunSetPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CFunSetPageDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_FUNSET_GAS_MAPS_COMBO, m_gas_maps_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_BENZIN_MAPS_COMBO, m_benzin_maps_combo);
 DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_SPIN, m_press_swing_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_press_swing_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_SPIN, m_map_grad_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_map_grad_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET_EDIT, m_map_curve_offset_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_OFFSET_SPIN, m_map_curve_offset_spin);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, m_map_curve_gradient_edit);
 DDX_Control(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_SPIN, m_map_curve_gradient_spin);
 
 m_map_grad_edit.DDX_Value(pDX, IDC_PD_FUNSET_MAP_GRAD_EDIT, m_params.map_lower_pressure);
 m_press_swing_edit.DDX_Value(pDX, IDC_PD_FUNSET_PRESS_SWING_EDIT, m_params.map_upper_pressure);
 m_map_curve_offset_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_OFFSET_EDIT, m_params.map_curve_offset);
 m_map_curve_gradient_edit.DDX_Value(pDX, IDC_PD_FUNSET_CURVE_GRADIENT_EDIT, m_params.map_curve_gradient);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_BENZIN_MAPS_COMBO, m_params.fn_benzin);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_FUNSET_GAS_MAPS_COMBO, m_params.fn_gas);
}

/////////////////////////////////////////////////////////////////////////////
// CFunSetPageDlg message handlers

//���� ���� ��������� ��������� ���������, �� ���� ����� ������� �������
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
 m_map_grad_spin.SetRangeAndDelta(0.25f,105.0f,0.25f);
	
 m_press_swing_spin.SetBuddy(&m_press_swing_edit);
 m_press_swing_edit.SetLimitText(4);
 m_press_swing_edit.SetDecimalPlaces(2);
 m_press_swing_spin.SetRangeAndDelta(0.25,105.0f,0.25f);

 m_map_curve_offset_spin.SetBuddy(&m_map_curve_offset_edit);
 m_map_curve_offset_edit.SetLimitText(5);
 m_map_curve_offset_edit.SetDecimalPlaces(3);
 m_map_curve_offset_spin.SetRangeAndDelta(0.0f,5.0f,0.0025f);

 m_map_curve_gradient_spin.SetBuddy(&m_map_curve_gradient_edit);
 m_map_curve_gradient_edit.SetLimitText(5);
 m_map_curve_gradient_edit.SetDecimalPlaces(3);
 m_map_curve_gradient_spin.SetRangeAndDelta(1.0f,150.0f,0.01f);

 FillCBByFunNames(); //�������������� ������� ComboBox-�� �������� �������������
 UpdateData(FALSE);  //�������������� ��������� ������� �������    
 UpdateDialogControls(this,TRUE);

 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CFunSetPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//����������/���������� ���������� (���� ���������)
void CFunSetPageDlg::Enable(bool enable)
{
 m_enabled = (enable) ? TRUE : FALSE;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//��� � �����������?
bool CFunSetPageDlg::IsEnabled(void)
{
 return (m_enabled) ? true : false;
}

//TODO: ���� ����� ������������� ������������ ����� ����������, �� ��� �����������
//�������������� ��������� ����� ������������ CComboBox::SetItemData. � ����� ������
//������ ���������� ������ ���������� ����������� ����� �������

//��������� ComboBox-� ���� �������� �������������
void CFunSetPageDlg::FillCBByFunNames(void)
{
 if (!::IsWindow(m_hWnd))
  return;

 //������� ���������� �������
 m_benzin_maps_combo.ResetContent();
 m_gas_maps_combo.ResetContent();
	  
 //��������� ����� �������
 for(size_t i = 0; i < m_fun_names.size(); i++)
 {
  m_benzin_maps_combo.AddString(m_fun_names[i].c_str());
  m_gas_maps_combo.AddString(m_fun_names[i].c_str());
 }

 //����� �������� ���� ��������� ������� ��������� � ComboBox-��
 //�������� � �� ������ ������� ������ ���������.

 //��� ����
 if (m_params.fn_gas < m_fun_names.size())
  m_gas_maps_combo.SetCurSel(m_params.fn_gas);
 else
  m_gas_maps_combo.SetCurSel(0);

 //��� ������
 if (m_params.fn_benzin < m_fun_names.size())
  m_benzin_maps_combo.SetCurSel(m_params.fn_benzin);
 else
  m_benzin_maps_combo.SetCurSel(0);
}

std::vector<_TSTRING>& CFunSetPageDlg::AccessFunNames(void)
{ 
 return m_fun_names;
}

//��� ������� ���������� ������������ ����� ���� �������� ������ �� �������
void CFunSetPageDlg::GetValues(SECU3IO::FunSetPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //�������� ������ �� ������� � ����������
 memcpy(o_values,&m_params, sizeof(SECU3IO::FunSetPar));
}

//��� ������� ���������� ������������ ����� ���� ������� ������ � ������
void CFunSetPageDlg::SetValues(const SECU3IO::FunSetPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::FunSetPar));
 UpdateData(FALSE); //�������� ������ �� ���������� � ������
}
