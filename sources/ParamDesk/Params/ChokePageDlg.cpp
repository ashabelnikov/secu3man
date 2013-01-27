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
#include "Resources/resource.h"
#include "ChokePageDlg.h"

const UINT CChokePageDlg::IDD = IDD_PD_CHOKE_PAGE;

BEGIN_MESSAGE_MAP(CChokePageDlg, Super)
 ON_EN_CHANGE(IDC_PD_CHOKE_SM_STEPS_NUM_EDIT, OnChangePdChokeSMStepsNumEdit)

 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_CHOKE_SM_STEPS_NUM_EDIT,OnUpdateControls)

END_MESSAGE_MAP()

CChokePageDlg::CChokePageDlg(CWnd* pParent /*=NULL*/)
: Super(CChokePageDlg::IDD, pParent)
, m_enabled(false)
, m_sm_steps_num_edit(CEditEx::MODE_INT)
{
 m_params.sm_steps = 700;
}

LPCTSTR CChokePageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CChokePageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_CHOKE_SM_STEPS_NUM_SPIN, m_sm_steps_num_spin);
 DDX_Control(pDX, IDC_PD_CHOKE_SM_STEPS_NUM_EDIT, m_sm_steps_num_edit);

 m_sm_steps_num_edit.DDX_Value(pDX, IDC_PD_CHOKE_SM_STEPS_NUM_EDIT, m_params.sm_steps);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CChokePageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CStarterPageDlg message handlers

BOOL CChokePageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_sm_steps_num_edit.SetLimitText(4);
 m_sm_steps_num_spin.SetBuddy(&m_sm_steps_num_edit);
 m_sm_steps_num_spin.SetRangeAndDelta(50, 1000, 1);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CChokePageDlg::OnChangePdChokeSMStepsNumEdit()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//разрешение/запрещение контроллов (всех поголовно)
void CChokePageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CChokePageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CChokePageDlg::GetValues(SECU3IO::ChokePar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::ChokePar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CChokePageDlg::SetValues(const SECU3IO::ChokePar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::ChokePar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
