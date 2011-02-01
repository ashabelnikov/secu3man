/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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
#include "resource.h"
#include "CarburPageDlg.h"
#include "ui-core\ddx_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT CCarburPageDlg::IDD = IDD_PD_CARBUR_PAGE;

BEGIN_MESSAGE_MAP(CCarburPageDlg, Super)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_EPM_ON_THRESHOLD_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_CARBUR_INVERSE_SWITCH, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT_G, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT_G, OnChangeData)
 ON_EN_CHANGE(IDC_PD_CARBUR_SHUTOFF_DELAY_EDIT, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_INVERSE_SWITCH,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_EPM_ON_THRESHOLD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_EPM_ON_THRESHOLD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_EPM_ON_THRESHOLD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_EPM_ON_THRESHOLD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT_G,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN_G,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_CAPTION_G,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_UNIT_G,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT_G,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN_G,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_CAPTION_G,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_UNIT_G,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_DELAY_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_DELAY_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_DELAY_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CARBUR_SHUTOFF_DELAY_UNIT,OnUpdateControls)
END_MESSAGE_MAP()

CCarburPageDlg::CCarburPageDlg(CWnd* pParent /*=NULL*/)
: Super(CCarburPageDlg::IDD, pParent)
, m_enabled(FALSE)
, m_shutoff_lo_threshold_edit(CEditEx::MODE_INT)
, m_shutoff_hi_threshold_edit(CEditEx::MODE_INT)
, m_epm_on_threshold_edit(CEditEx::MODE_FLOAT)
, m_shutoff_lo_threshold_edit_g(CEditEx::MODE_INT)
, m_shutoff_hi_threshold_edit_g(CEditEx::MODE_INT)
, m_shutoff_delay_edit(CEditEx::MODE_FLOAT)
{
 m_params.ephh_lot = 1250;
 m_params.ephh_hit = 1500;
 m_params.carb_invers = 0;
 m_params.epm_ont = 6.25f;
 m_params.ephh_lot_g = 1250;
 m_params.ephh_hit_g = 1500;
 m_params.shutoff_delay = 0.0f;
}

LPCTSTR CCarburPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CCarburPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_CARBUR_INVERSE_SWITCH, m_inverse_throttle_switch);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN, m_shutoff_lo_threshold_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN, m_shutoff_hi_threshold_spin);
 DDX_Control(pDX, IDC_PD_CARBUR_EPM_ON_THRESHOLD_SPIN, m_epm_on_threshold_spin);

 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, m_shutoff_lo_threshold_edit);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, m_shutoff_hi_threshold_edit);	
 DDX_Control(pDX, IDC_PD_CARBUR_EPM_ON_THRESHOLD_EDIT, m_epm_on_threshold_edit);	

 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_SPIN_G, m_shutoff_lo_threshold_spin_g);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_SPIN_G, m_shutoff_hi_threshold_spin_g);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_DELAY_SPIN, m_shutoff_delay_spin);

 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT_G, m_shutoff_lo_threshold_edit_g);
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT_G, m_shutoff_hi_threshold_edit_g);	
 DDX_Control(pDX, IDC_PD_CARBUR_SHUTOFF_DELAY_EDIT, m_shutoff_delay_edit);	

 m_shutoff_lo_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT, m_params.ephh_lot);
 m_shutoff_hi_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT, m_params.ephh_hit);

 m_shutoff_lo_threshold_edit_g.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_LO_THRESHOLD_EDIT_G, m_params.ephh_lot_g);
 m_shutoff_hi_threshold_edit_g.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_HI_THRESHOLD_EDIT_G, m_params.ephh_hit_g);
 m_shutoff_delay_edit.DDX_Value(pDX, IDC_PD_CARBUR_SHUTOFF_DELAY_EDIT, m_params.shutoff_delay);
    
 DDX_Check_UCHAR(pDX, IDC_PD_CARBUR_INVERSE_SWITCH, m_params.carb_invers);
 m_epm_on_threshold_edit.DDX_Value(pDX, IDC_PD_CARBUR_EPM_ON_THRESHOLD_EDIT, m_params.epm_ont);
}

/////////////////////////////////////////////////////////////////////////////
// CCarburPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CCarburPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CCarburPageDlg::OnInitDialog()
{
 Super::OnInitDialog();
	
 m_shutoff_lo_threshold_edit.SetLimitText(4);
 m_shutoff_lo_threshold_spin.SetBuddy(&m_shutoff_lo_threshold_edit);
 m_shutoff_lo_threshold_spin.SetRangeAndDelta(250,7500,10);

 m_shutoff_hi_threshold_edit.SetLimitText(4);
 m_shutoff_hi_threshold_spin.SetBuddy(&m_shutoff_hi_threshold_edit);
 m_shutoff_hi_threshold_spin.SetRangeAndDelta(250,7500,10);

 m_epm_on_threshold_spin.SetBuddy(&m_epm_on_threshold_edit);
 m_epm_on_threshold_edit.SetLimitText(4);
 m_epm_on_threshold_edit.SetDecimalPlaces(2);
 m_epm_on_threshold_spin.SetRangeAndDelta(0.0f,50.0f,0.1f);
	
 m_shutoff_lo_threshold_edit_g.SetLimitText(4);
 m_shutoff_lo_threshold_spin_g.SetBuddy(&m_shutoff_lo_threshold_edit_g);
 m_shutoff_lo_threshold_spin_g.SetRangeAndDelta(250,7500,10);

 m_shutoff_hi_threshold_edit_g.SetLimitText(4);
 m_shutoff_hi_threshold_spin_g.SetBuddy(&m_shutoff_hi_threshold_edit_g);
 m_shutoff_hi_threshold_spin_g.SetRangeAndDelta(250,7500,10);

 m_shutoff_delay_spin.SetBuddy(&m_shutoff_delay_edit);
 m_shutoff_delay_edit.SetLimitText(4);
 m_shutoff_delay_edit.SetDecimalPlaces(2);
 m_shutoff_delay_spin.SetRangeAndDelta(0.0f,2.5f,0.01f);

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CCarburPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
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
 memcpy(o_values,&m_params, sizeof(SECU3IO::CarburPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CCarburPageDlg::SetValues(const SECU3IO::CarburPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::CarburPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
