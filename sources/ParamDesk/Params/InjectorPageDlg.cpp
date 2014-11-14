/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include "Resources/resource.h"
#include "InjectorPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CInjectorPageDlg::IDD = IDD_PD_INJECTOR_PAGE;

BEGIN_MESSAGE_MAP(CInjectorPageDlg, Super)
 ON_EN_CHANGE(IDC_PD_INJECTOR_CYLDISP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_INJECTOR_FLOWRATE_EDIT, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_CYLDISP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_CYLDISP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_CYLDISP_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_CYLDISP_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_INJECTOR_FLOWRATE_UNIT,OnUpdateControls)
END_MESSAGE_MAP()

CInjectorPageDlg::CInjectorPageDlg(CWnd* pParent /*=NULL*/)
: Super(CInjectorPageDlg::IDD, pParent)
, m_enabled(false)
, m_cyldisp_edit(CEditEx::MODE_FLOAT, true)
, m_flowrate_edit(CEditEx::MODE_FLOAT, true)
, m_fuel_density(0.71f) //petrol density (0.71 g/cc)
{
 m_params.inj_config = 0;
 m_params.inj_flow_rate = 200.0f;
 m_params.inj_cyl_disp = 0.375f;
 m_params.inj_sd_igl_const = 0;
 m_params.cyl_num = 4; 
}

CInjectorPageDlg::~CInjectorPageDlg()
{
 //empty
}

LPCTSTR CInjectorPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CInjectorPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX,IDC_PD_INJECTOR_CYLDISP_EDIT, m_cyldisp_edit);
 DDX_Control(pDX,IDC_PD_INJECTOR_CYLDISP_SPIN, m_cyldisp_spin);
 DDX_Control(pDX,IDC_PD_INJECTOR_FLOWRATE_EDIT, m_flowrate_edit);
 DDX_Control(pDX,IDC_PD_INJECTOR_FLOWRATE_SPIN, m_flowrate_spin);

 m_flowrate_edit.DDX_Value(pDX, IDC_PD_INJECTOR_FLOWRATE_EDIT, m_params.inj_flow_rate);
 float engdisp = m_params.inj_cyl_disp * m_params.cyl_num; //convert cyl.disp. to eng.disp
 m_cyldisp_edit.DDX_Value(pDX, IDC_PD_INJECTOR_CYLDISP_EDIT, engdisp);
 m_params.inj_cyl_disp = engdisp / m_params.cyl_num; //convert eng.disp to cyl.disp
}

void CInjectorPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CInjectorPageDlg message handlers

BOOL CInjectorPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_cyldisp_spin.SetBuddy(&m_cyldisp_edit);
 m_cyldisp_edit.SetLimitText(6);
 m_cyldisp_edit.SetDecimalPlaces(4);
 m_cyldisp_spin.SetRangeAndDelta(0.05f, 3.0000f, 0.0001);
 m_cyldisp_edit.SetRange(0.05f, 3.0000f);

 m_flowrate_spin.SetBuddy(&m_flowrate_edit);
 m_flowrate_edit.SetLimitText(6);
 m_flowrate_edit.SetDecimalPlaces(2);
 m_flowrate_spin.SetRangeAndDelta(50.00f, 1000.00f, 0.02f);
 m_flowrate_edit.SetRange(50.00f, 1000.00f);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_cyldisp_edit, MLL::GetString(IDS_PD_INJECTOR_CYLDISP_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CInjectorPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

//разрешение/запрещение контроллов (всех поголовно)
void CInjectorPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CInjectorPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CInjectorPageDlg::GetValues(SECU3IO::InjctrPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные

 //convert inj.flow rate from cc/min to g/min
 float mifr = m_params.inj_flow_rate * m_fuel_density;
 //calculate constant (calculation of this value related to the ideal gas law, see firmware code for more information)
 //todo: injection config
 m_params.inj_sd_igl_const = ((m_params.inj_cyl_disp * 3.482f * 18750000.0f) / mifr) * (float(m_params.cyl_num) / (4.0f * 4.0f));

 memcpy(o_values,&m_params, sizeof(SECU3IO::InjctrPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CInjectorPageDlg::SetValues(const SECU3IO::InjctrPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::InjctrPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
