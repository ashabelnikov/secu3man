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
END_MESSAGE_MAP()

CInjectorPageDlg::CInjectorPageDlg(CWnd* pParent /*=NULL*/)
: Super(CInjectorPageDlg::IDD, pParent)
, m_enabled(false)
{
 m_params.inj_config = 0;
 m_params.inj_flow_rate = 200.0f;
 m_params.inj_cyl_disp = 0.75f;
 m_params.inj_sd_igl_const = 0; 
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
 memcpy(o_values,&m_params, sizeof(SECU3IO::InjctrPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CInjectorPageDlg::SetValues(const SECU3IO::InjctrPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::InjctrPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
