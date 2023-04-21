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

/** \file DevDiagInpsDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "DevDiagInpsDlg.h"

#include "common/DPIAware.h"
#include "common/FastDelegate.h"
#include "common/GDIHelpers.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ToolTipCtrlEx.h"

using namespace fastdelegate;

const UINT CDevDiagInpsDlg::IDD = IDD_DEVDIAG_INPUTS;

const UINT InputsTextStart = IDC_DEV_DIAG_VOLTAGE;
const UINT InputsTextEnd = IDC_DEV_DIAG_DE;
const UINT InputsTextStart1 = IDC_DEV_DIAG_VOLTAGE;            //secu-3i
const UINT InputsTextEnd1 = IDC_DEV_DIAG_ADD_I8;               //secu-3i

const UINT InputsCaptionStart = IDC_DEV_DIAG_VOLTAGE_CAPTION;
const UINT InputsCaptionEnd = IDC_DEV_DIAG_DE_CAPTION;
const UINT InputsCaptionStart1 = IDC_DEV_DIAG_VOLTAGE_CAPTION; //secu-3i
const UINT InputsCaptionEnd1 = IDC_DEV_DIAG_ADD_I8_CAPTION;    //secu-3i

BEGIN_MESSAGE_MAP(CDevDiagInpsDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_UPDATE_COMMAND_UI_RANGE(InputsTextStart, InputsTextEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI_RANGE(InputsCaptionStart, InputsCaptionEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI_RANGE(InputsTextStart1, InputsTextEnd1, OnUpdateDiagControls)        //secu-3i
 ON_UPDATE_COMMAND_UI_RANGE(InputsCaptionStart1, InputsCaptionEnd1, OnUpdateDiagControls)  //secu-3i
END_MESSAGE_MAP()

CDevDiagInpsDlg::CDevDiagInpsDlg(CWnd* pParent /*=NULL*/)
: Super(CDevDiagInpsDlg::IDD, pParent)
, m_enable_diag_controls(false)
, m_enable_secu3t_features(false)
, m_enable_extraio(false)
, mp_scr(new CWndScroller)
{
 memset(&m_inputValues, 0, sizeof(SECU3IO::DiagInpDat));
}

void CDevDiagInpsDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_VOLTAGE, m_inputValues.voltage, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_MAP_S, m_inputValues.map, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_TEMP, m_inputValues.temp, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I1, m_inputValues.add_i1, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I2, m_inputValues.add_i2, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_CARB, m_inputValues.carb,_T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_GAS_V, m_inputValues.gas, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_CKPS, m_inputValues.ckps, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_REF_S, m_inputValues.ref_s, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_PS, m_inputValues.ps, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_BL, m_inputValues.bl, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_DE, m_inputValues.de, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I4, m_inputValues.add_i4, _T("%.3f"));

 if (!m_enable_secu3t_features) //SECU-3i
 {
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I3, m_inputValues.add_i3, _T("%.3f"));
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_IGN_I, m_inputValues.ign_i, _T("%d"));
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_COND_I, m_inputValues.cond_i, _T("%d"));
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_EPAS_I, m_inputValues.epas_i, _T("%d"));
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I5, m_inputValues.add_i5, _T("%.3f"));
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I6, m_inputValues.add_i6, _T("%.3f"));
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I7, m_inputValues.add_i7, _T("%.3f"));
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I8, m_inputValues.add_i8, _T("%.3f"));
 }
}

LPCTSTR CDevDiagInpsDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CDevDiagInpsDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //Change size of fonts (this will improve user experience)
 CloneWndFont(GetDlgItem(InputsTextStart), &m_textFont, 11, true);
 for(size_t i = InputsTextStart1; i <= InputsTextEnd1; ++i)
  GetDlgItem(i)->SetFont(&m_textFont);
  
 UpdateDialogControls(this,TRUE);

 //initialize window scroller
 mp_scr->Init(this);
 _UpdateScrlViewSize();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_VOLTAGE), MLL::GetString(IDS_DEV_DIAG_VOLTAGE_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_GAS_V), MLL::GetString(IDS_DEV_DIAG_GAS_V_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_MAP_S), MLL::GetString(IDS_DEV_DIAG_MAP_S_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_TEMP), MLL::GetString(IDS_DEV_DIAG_TEMP_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_CKPS), MLL::GetString(IDS_DEV_DIAG_CKPS_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_REF_S), MLL::GetString(IDS_DEV_DIAG_REF_S_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_PS), MLL::GetString(IDS_DEV_DIAG_PS_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_BL), MLL::GetString(IDS_DEV_DIAG_BL_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_DE), MLL::GetString(IDS_DEV_DIAG_DE_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ADD_I1), MLL::GetString(IDS_DEV_DIAG_ADD_I1_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ADD_I2), MLL::GetString(IDS_DEV_DIAG_ADD_I2_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ADD_I3), MLL::GetString(IDS_DEV_DIAG_ADD_I3_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ADD_I4), MLL::GetString(IDS_DEV_DIAG_ADD_I4_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_CARB), MLL::GetString(IDS_DEV_DIAG_CARB_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_IGN_I), MLL::GetString(IDS_DEV_DIAG_IGN_I_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_COND_I), MLL::GetString(IDS_DEV_DIAG_COND_I_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_EPAS_I), MLL::GetString(IDS_DEV_DIAG_EPAS_I_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ADD_I5), MLL::GetString(IDS_DEV_DIAG_ADD_I5_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ADD_I6), MLL::GetString(IDS_DEV_DIAG_ADD_I6_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ADD_I7), MLL::GetString(IDS_DEV_DIAG_ADD_I7_TT)));
 VERIFY(mp_ttc->AddWindow(GetDlgItem(IDC_DEV_DIAG_ADD_I8), MLL::GetString(IDS_DEV_DIAG_ADD_I8_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //remember original position
 m_add_i4_rc[0] = GDIHelpers::GetChildWndRect(GetDlgItem(IDC_DEV_DIAG_ADD_I4));
 m_add_i4_rc[1] = GDIHelpers::GetChildWndRect(GetDlgItem(IDC_DEV_DIAG_ADD_I4_CAPTION));

 return TRUE;
}

void CDevDiagInpsDlg::OnUpdateDiagControls(CCmdUI* pCmdUI)
{
 switch(pCmdUI->m_nID)
 {
 case IDC_DEV_DIAG_ADD_I5:
 case IDC_DEV_DIAG_ADD_I5_CAPTION:
 case IDC_DEV_DIAG_ADD_I6:
 case IDC_DEV_DIAG_ADD_I6_CAPTION:
 case IDC_DEV_DIAG_ADD_I7:
 case IDC_DEV_DIAG_ADD_I7_CAPTION:
 case IDC_DEV_DIAG_ADD_I8:
 case IDC_DEV_DIAG_ADD_I8_CAPTION:

 case IDC_DEV_DIAG_ADD_I3:
 case IDC_DEV_DIAG_IGN_I:
 case IDC_DEV_DIAG_COND_I:
 case IDC_DEV_DIAG_EPAS_I:
 case IDC_DEV_DIAG_ADD_I3_CAPTION:
 case IDC_DEV_DIAG_IGN_I_CAPTION:
 case IDC_DEV_DIAG_COND_I_CAPTION:
 case IDC_DEV_DIAG_EPAS_I_CAPTION:
  pCmdUI->Enable(m_enable_diag_controls && !m_enable_secu3t_features);
  break;
 case IDC_DEV_DIAG_ADD_I4_CAPTION:
 case IDC_DEV_DIAG_ADD_I4:
  pCmdUI->Enable(m_enable_diag_controls && m_enable_extraio);
  break;
 default:
  pCmdUI->Enable(m_enable_diag_controls);
 }
}

void CDevDiagInpsDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CDevDiagInpsDlg::EnableDiagControls(bool i_enable)
{
 m_enable_diag_controls = i_enable;
 UpdateDialogControls(this, TRUE);
}

void CDevDiagInpsDlg::EnableSECU3TFeatures(bool i_enable)
{
 m_enable_secu3t_features = i_enable;

 _UpdateScrlViewSize();

 //Move ADD_I4
 if (m_enable_secu3t_features)
 {
  GetDlgItem(IDC_DEV_DIAG_ADD_I4)->MoveWindow(&GDIHelpers::GetChildWndRect(GetDlgItem(IDC_DEV_DIAG_ADD_I3)));
  GetDlgItem(IDC_DEV_DIAG_ADD_I4_CAPTION)->MoveWindow(&GDIHelpers::GetChildWndRect(GetDlgItem(IDC_DEV_DIAG_ADD_I3_CAPTION)));
 }
 else
 {
  GetDlgItem(IDC_DEV_DIAG_ADD_I4)->MoveWindow(&m_add_i4_rc[0]);
  GetDlgItem(IDC_DEV_DIAG_ADD_I4_CAPTION)->MoveWindow(&m_add_i4_rc[1]);
 }

 GetDlgItem(IDC_DEV_DIAG_ADD_I5)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I5_CAPTION)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I6)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I6_CAPTION)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I7)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I7_CAPTION)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I8)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I8_CAPTION)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I3)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_IGN_I)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_COND_I)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_EPAS_I)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_ADD_I3_CAPTION)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_IGN_I_CAPTION)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_COND_I_CAPTION)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);
 GetDlgItem(IDC_DEV_DIAG_EPAS_I_CAPTION)->ShowWindow(i_enable ? SW_HIDE : SW_SHOW);

 GetDlgItem(IDC_DEV_DIAG_CARB_CAPTION)->SetWindowText(MLL::LoadString(i_enable ? IDS_DEV_DIAG_CARB : IDS_DEV_DIAG_TPS));
 GetDlgItem(IDC_DEV_DIAG_MAP_S_CAPTION)->SetWindowText(MLL::LoadString(i_enable ? IDS_DEV_DIAG_MAP_S : IDS_DEV_DIAG_MAP_I));

 UpdateDialogControls(this,TRUE);
}

void CDevDiagInpsDlg::EnableExtraIO(bool i_enable)
{
 m_enable_extraio = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CDevDiagInpsDlg::SetInputValues(const SECU3IO::DiagInpDat* i_values)
{
 ASSERT(i_values);
 memcpy(&m_inputValues, i_values, sizeof(SECU3IO::DiagInpDat));
 UpdateData(FALSE);
}

void CDevDiagInpsDlg::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after, x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

void CDevDiagInpsDlg::_UpdateScrlViewSize(void)
{
 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(0, m_enable_secu3t_features ? da.ScaleY(250) : da.ScaleY(477));
}

void CDevDiagInpsDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 _UpdateScrlViewSize();
}
