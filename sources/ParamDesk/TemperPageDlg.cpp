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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#include "stdafx.h"
#include "resource.h"
#include "TemperPageDlg.h"
#include "ui-core\ddx_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT CTemperPageDlg::IDD = IDD_PD_TEMPER_PAGE;

BEGIN_MESSAGE_MAP(CTemperPageDlg, Super)
 ON_EN_CHANGE(IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, OnChangePdTemperVentOnThresholdEdit)
 ON_EN_CHANGE(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, OnChangePdTemperVentOffThresholdEdit)
 ON_BN_CLICKED(IDC_PD_TEMPER_USE_TEMP_SENSOR, OnPdTemperUseTempSensor)
 ON_BN_CLICKED(IDC_PD_TEMPER_USE_VENT_PWM, OnPdTemperUseVentPwm)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_ON_THRESHOLD_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_ON_THRESHOLD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_ON_THRESHOLD_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_USE_TEMP_SENSOR, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_USE_VENT_PWM, OnUpdateControls)
END_MESSAGE_MAP()

CTemperPageDlg::CTemperPageDlg(CWnd* pParent /*=NULL*/)
: Super(CTemperPageDlg::IDD, pParent)
, m_enabled(FALSE)
, m_vent_on_threshold_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_vent_off_threshold_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
{
 m_params.vent_on = 95.0f;
 m_params.vent_off = 98.0f;
 m_params.tmp_use = 1;
 m_params.vent_pwm = 0;
}

LPCTSTR CTemperPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CTemperPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_TEMPER_USE_TEMP_SENSOR, m_use_temp_sensor);
 DDX_Control(pDX, IDC_PD_TEMPER_USE_VENT_PWM, m_use_vent_pwm);
 DDX_Control(pDX, IDC_PD_TEMPER_VENT_ON_THRESHOLD_SPIN, m_vent_on_threshold_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_VENT_OFF_THRESHOLD_SPIN, m_vent_off_threshold_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, m_vent_off_threshold_edit);
 DDX_Control(pDX, IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, m_vent_on_threshold_edit);

 m_vent_on_threshold_edit.DDX_Value(pDX, IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, m_params.vent_on);
 m_vent_off_threshold_edit.DDX_Value(pDX, IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, m_params.vent_off);
 DDX_Check_UCHAR(pDX, IDC_PD_TEMPER_USE_TEMP_SENSOR, m_params.tmp_use);
 DDX_Check_UCHAR(pDX, IDC_PD_TEMPER_USE_VENT_PWM, m_params.vent_pwm);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CTemperPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CTemperPageDlg message handlers

BOOL CTemperPageDlg::OnInitDialog() 
{
 Super::OnInitDialog();
		
 m_vent_on_threshold_spin.SetBuddy(&m_vent_on_threshold_edit);
 m_vent_on_threshold_edit.SetLimitText(4);
 m_vent_on_threshold_edit.SetDecimalPlaces(2);
 m_vent_on_threshold_spin.SetRangeAndDelta(-10.0f,125.0f,0.2f);

 m_vent_off_threshold_spin.SetBuddy(&m_vent_off_threshold_edit);
 m_vent_off_threshold_edit.SetLimitText(4);
 m_vent_off_threshold_edit.SetDecimalPlaces(2);
 m_vent_off_threshold_spin.SetRangeAndDelta(-10.0f,125.0f,0.2f);

 UpdateData(FALSE);
 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemperPageDlg::OnChangePdTemperVentOnThresholdEdit()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CTemperPageDlg::OnChangePdTemperVentOffThresholdEdit()
{
 UpdateData();
 OnChangeNotify();
}

void CTemperPageDlg::OnPdTemperUseTempSensor()
{
 UpdateData();		
 OnChangeNotify();
}

void CTemperPageDlg::OnPdTemperUseVentPwm()
{
 UpdateData();		
 OnChangeNotify();
}

//разрешение/запрещение контроллов (всех поголовно)
void CTemperPageDlg::Enable(bool enable)
{
 m_enabled = (enable) ? TRUE : FALSE;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CTemperPageDlg::IsEnabled(void)
{
 return (m_enabled) ? true : false;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CTemperPageDlg::GetValues(SECU3IO::TemperPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::TemperPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CTemperPageDlg::SetValues(const SECU3IO::TemperPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::TemperPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
