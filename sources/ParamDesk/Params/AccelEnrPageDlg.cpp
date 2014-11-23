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
#include "AccelEnrPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CAccelEnrPageDlg::IDD = IDD_PD_ACCELENR_PAGE;

BEGIN_MESSAGE_MAP(CAccelEnrPageDlg, Super)
END_MESSAGE_MAP()

CAccelEnrPageDlg::CAccelEnrPageDlg(CWnd* pParent /*=NULL*/)
: Super(CAccelEnrPageDlg::IDD, pParent)
, m_enabled(false)
{

}

LPCTSTR CAccelEnrPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CAccelEnrPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

}

void CAccelEnrPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CAccelEnrPageDlg message handlers

BOOL CAccelEnrPageDlg::OnInitDialog()
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

void CAccelEnrPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//разрешение/запрещение контроллов (всех поголовно)
void CAccelEnrPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//get state of controls
bool CAccelEnrPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//For getting values from dialog
void CAccelEnrPageDlg::GetValues(SECU3IO::AccelPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::AccelPar));
}

//For setting values to dialog
void CAccelEnrPageDlg::SetValues(const SECU3IO::AccelPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::AccelPar));
 UpdateData(FALSE); //copy data from variables to dialog
}
