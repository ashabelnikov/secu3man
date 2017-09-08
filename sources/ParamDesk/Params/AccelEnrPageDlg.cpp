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

/** \file AccelEnrPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "AccelEnrPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CAccelEnrPageDlg::IDD = IDD_PD_ACCELENR_PAGE;

BEGIN_MESSAGE_MAP(CAccelEnrPageDlg, Super)
 ON_EN_CHANGE(IDC_PD_ACCELENR_TPSTHRD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ACCELENR_MULTCOLDENR_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ACCELENR_DECAYTIME_EDIT, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_TPSTHRD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_TPSTHRD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_TPSTHRD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_TPSTHRD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_MULTCOLDENR_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_MULTCOLDENR_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_MULTCOLDENR_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_MULTCOLDENR_UNIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_DECAYTIME_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_DECAYTIME_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_DECAYTIME_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ACCELENR_DECAYTIME_UNIT,OnUpdateFuelInjectionControls)
END_MESSAGE_MAP()

CAccelEnrPageDlg::CAccelEnrPageDlg(CWnd* pParent /*=NULL*/)
: Super(CAccelEnrPageDlg::IDD, pParent)
, m_enabled(false)
, m_fuel_injection(false)
, m_tpsdot_thrd_edit(CEditEx::MODE_FLOAT, true)
, m_coldacc_mult_edit(CEditEx::MODE_FLOAT, true)
, m_decaytime_edit(CEditEx::MODE_INT, true)
{
 m_params.ae_tpsdot_thrd = 50.0f;   //50%/sec
 m_params.ae_coldacc_mult = 150.0f; //*150% at -30°C
 m_params.ae_decay_time = 50; //strokes
}

LPCTSTR CAccelEnrPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CAccelEnrPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX,IDC_PD_ACCELENR_TPSTHRD_EDIT, m_tpsdot_thrd_edit);
 DDX_Control(pDX,IDC_PD_ACCELENR_TPSTHRD_SPIN, m_tpsdot_thrd_spin);
 DDX_Control(pDX,IDC_PD_ACCELENR_MULTCOLDENR_EDIT, m_coldacc_mult_edit);
 DDX_Control(pDX,IDC_PD_ACCELENR_MULTCOLDENR_SPIN, m_coldacc_mult_spin);
 DDX_Control(pDX,IDC_PD_ACCELENR_DECAYTIME_EDIT, m_decaytime_edit);
 DDX_Control(pDX,IDC_PD_ACCELENR_DECAYTIME_SPIN, m_decaytime_spin);

 m_tpsdot_thrd_edit.DDX_Value(pDX, IDC_PD_ACCELENR_TPSTHRD_EDIT, m_params.ae_tpsdot_thrd);
 m_coldacc_mult_edit.DDX_Value(pDX, IDC_PD_ACCELENR_MULTCOLDENR_EDIT, m_params.ae_coldacc_mult);
 m_decaytime_edit.DDX_Value(pDX, IDC_PD_ACCELENR_DECAYTIME_EDIT, m_params.ae_decay_time);
}

void CAccelEnrPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CAccelEnrPageDlg::OnUpdateFuelInjectionControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_fuel_injection);
}

/////////////////////////////////////////////////////////////////////////////
// CAccelEnrPageDlg message handlers

BOOL CAccelEnrPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_tpsdot_thrd_spin.SetBuddy(&m_tpsdot_thrd_edit);
 m_tpsdot_thrd_edit.SetLimitText(3);
 m_tpsdot_thrd_edit.SetDecimalPlaces(0);
 m_tpsdot_thrd_spin.SetRangeAndDelta(0, 255, 1);
 m_tpsdot_thrd_edit.SetRange(0, 255);

 m_coldacc_mult_spin.SetBuddy(&m_coldacc_mult_edit);
 m_coldacc_mult_edit.SetLimitText(3);
 m_coldacc_mult_edit.SetDecimalPlaces(0);
 m_coldacc_mult_spin.SetRangeAndDelta(100, 299, 1);
 m_coldacc_mult_edit.SetRange(100, 299);

 m_decaytime_spin.SetBuddy(&m_decaytime_edit);
 m_decaytime_edit.SetLimitText(3);
 m_decaytime_edit.SetDecimalPlaces(3);
 m_decaytime_spin.SetRangeAndDelta(1, 255, 1);
 m_decaytime_edit.SetRange(1, 255);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_tpsdot_thrd_edit, MLL::GetString(IDS_PD_ACCELENR_TPSTHRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tpsdot_thrd_spin, MLL::GetString(IDS_PD_ACCELENR_TPSTHRD_EDIT_TT)));

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

//Enable/disable all controls
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

void CAccelEnrPageDlg::EnableFuelInjection(bool i_enable)
{
 if (m_fuel_injection == i_enable)
  return; //already has needed state
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}
