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
#include "LambdaPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CLambdaPageDlg::IDD = IDD_PD_LAMBDA_PAGE;

BEGIN_MESSAGE_MAP(CLambdaPageDlg, Super)
END_MESSAGE_MAP()

CLambdaPageDlg::CLambdaPageDlg(CWnd* pParent /*=NULL*/)
: Super(CLambdaPageDlg::IDD, pParent)
, m_enabled(false)
{
 m_params.lam_str_per_stp = 10;
 m_params.lam_step_size = 0.25f;
 m_params.lam_corr_limit = 30.0f;
 m_params.lam_swt_point = 0.50f;
 m_params.lam_temp_thrd = 70.0f;
 m_params.lam_rpm_thrd = 800;
}

CLambdaPageDlg::~CLambdaPageDlg()
{
 //empty
}

LPCTSTR CLambdaPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CLambdaPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CLambdaPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CLambdaPageDlg message handlers

BOOL CLambdaPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

//разрешение/запрещение контроллов (всех поголовно)
void CLambdaPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CLambdaPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CLambdaPageDlg::GetValues(SECU3IO::LambdaPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::LambdaPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CLambdaPageDlg::SetValues(const SECU3IO::LambdaPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::LambdaPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
