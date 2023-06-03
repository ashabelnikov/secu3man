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

/** \file KnockPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "KnockPageDlg.h"

#include <vector>
#include "common/dpiaware.h"
#include "common/MathHelpers.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

using namespace std;

BEGIN_MESSAGE_MAP(CKnockPageDlg, Super)
 ON_CBN_SELCHANGE(IDC_PD_KNOCK_ENABLE_KC_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_KNOCK_BPF_FREQ_COMBO, OnChangeData)
 ON_CBN_SELCHANGE(IDC_PD_KNOCK_INT_TIME_CONST_COMBO, OnChangeData)

 ON_EN_CHANGE(IDC_PD_KNOCK_BEGIN_KWND_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_KNOCK_END_KWND_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_KNOCK_RETARD_STEP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_KNOCK_ADVANCE_STEP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_KNOCK_MAX_RETARD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_KNOCK_THRESHOLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_KNOCK_RECOVERY_DELAY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_KNOCK_CLT_THRD_EDIT, OnChangeData)

 ON_BN_CLICKED(IDC_PD_KNOCK_SELCH1, OnChangeData)
 ON_BN_CLICKED(IDC_PD_KNOCK_SELCH2, OnChangeData)
 ON_BN_CLICKED(IDC_PD_KNOCK_SELCH3, OnChangeData)
 ON_BN_CLICKED(IDC_PD_KNOCK_SELCH4, OnChangeData)
 ON_BN_CLICKED(IDC_PD_KNOCK_SELCH5, OnChangeData)
 ON_BN_CLICKED(IDC_PD_KNOCK_SELCH6, OnChangeData)
 ON_BN_CLICKED(IDC_PD_KNOCK_SELCH7, OnChangeData)
 ON_BN_CLICKED(IDC_PD_KNOCK_SELCH8, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_ENABLE_KC_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_BPF_FREQ_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_INT_TIME_CONST_COMBO, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_ENABLE_KC_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_BPF_FREQ_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_INT_TIME_CONST_CAPTION, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_BPF_FREQ_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_INT_TIME_CONST_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_BEGIN_KWND_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_END_KWND_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_RETARD_STEP_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_ADVANCE_STEP_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_MAX_RETARD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_THRESHOLD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_RECOVERY_DELAY_EDIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_BEGIN_KWND_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_END_KWND_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_RETARD_STEP_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_ADVANCE_STEP_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_MAX_RETARD_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_THRESHOLD_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_RECOVERY_DELAY_SPIN, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_BEGIN_KWND_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_END_KWND_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_RETARD_STEP_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_ADVANCE_STEP_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_MAX_RETARD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_THRESHOLD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_RECOVERY_DELAY_CAPTION, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_BEGIN_KWND_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_END_KWND_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_RETARD_STEP_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_ADVANCE_STEP_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_MAX_RETARD_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_THRESHOLD_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_RECOVERY_DELAY_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH1, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH2, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH3, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH4, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH5, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH6, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH7, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_SELCH8, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_CLT_THRD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_CLT_THRD_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_CLT_THRD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_KNOCK_CLT_THRD_UNIT, OnUpdateControls)

 ON_WM_DESTROY()
 ON_WM_SIZE()
END_MESSAGE_MAP()

CKnockPageDlg::CKnockPageDlg()
: m_enabled(false)
, mp_scr(new CWndScroller)
, m_wnd_begin_angle_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_wnd_end_angle_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_retard_step_edit(CEditEx::MODE_FLOAT, true)
, m_advance_step_edit(CEditEx::MODE_FLOAT, true)
, m_max_retard_edit(CEditEx::MODE_FLOAT, true)
, m_threshold_edit(CEditEx::MODE_FLOAT, true)
, m_recovery_delay_edit(CEditEx::MODE_INT, true)
, m_knkclt_thrd_edit(CEditEx::MODE_FLOAT, true)
{
 m_params.knock_use_knock_channel = 0;
 m_params.knock_bpf_frequency = 40;
 m_params.knock_k_wnd_begin_angle = 0.0f;
 m_params.knock_k_wnd_end_angle = 48.0f;

 m_params.knock_retard_step = 4.0f;
 m_params.knock_advance_step = 0.25f;
 m_params.knock_max_retard = 16.0f;
 m_params.knock_threshold = 2.5f;
 m_params.knock_recovery_delay = 2;

 m_params.knock_int_time_const = 23; //300us

 for (int i = 0; i < 8; ++i)
  m_params.knock_selch[i] = 0;

 m_params.knkclt_thrd = 70.0f;
}

CKnockPageDlg::~CKnockPageDlg()
{
 //empty
}

LPCTSTR CKnockPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PD_KNOCK_PAGE;
}

BOOL CKnockPageDlg::Create(CWnd* pParentWnd /* = NULL*/)
{
 return Super::Create(GetDialogID(), pParentWnd);
}

void CKnockPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_KNOCK_ENABLE_KC_COMBO, m_use_knock_channel_combo);
 DDX_Control(pDX, IDC_PD_KNOCK_BPF_FREQ_COMBO, m_bpf_frequency_combo);
 DDX_Control(pDX, IDC_PD_KNOCK_INT_TIME_CONST_COMBO, m_integrator_const_combo);

 DDX_Control(pDX, IDC_PD_KNOCK_BEGIN_KWND_EDIT, m_wnd_begin_angle_edit);
 DDX_Control(pDX, IDC_PD_KNOCK_END_KWND_EDIT, m_wnd_end_angle_edit);
 DDX_Control(pDX, IDC_PD_KNOCK_RETARD_STEP_EDIT, m_retard_step_edit);
 DDX_Control(pDX, IDC_PD_KNOCK_ADVANCE_STEP_EDIT, m_advance_step_edit);
 DDX_Control(pDX, IDC_PD_KNOCK_MAX_RETARD_EDIT, m_max_retard_edit);
 DDX_Control(pDX, IDC_PD_KNOCK_THRESHOLD_EDIT, m_threshold_edit);
 DDX_Control(pDX, IDC_PD_KNOCK_RECOVERY_DELAY_EDIT, m_recovery_delay_edit);

 DDX_Control(pDX, IDC_PD_KNOCK_BEGIN_KWND_SPIN, m_wnd_begin_angle_spin);
 DDX_Control(pDX, IDC_PD_KNOCK_END_KWND_SPIN, m_wnd_end_angle_spin);
 DDX_Control(pDX, IDC_PD_KNOCK_RETARD_STEP_SPIN, m_retard_step_spin);
 DDX_Control(pDX, IDC_PD_KNOCK_ADVANCE_STEP_SPIN, m_advance_step_spin);
 DDX_Control(pDX, IDC_PD_KNOCK_MAX_RETARD_SPIN, m_max_retard_spin);
 DDX_Control(pDX, IDC_PD_KNOCK_THRESHOLD_SPIN, m_threshold_spin);
 DDX_Control(pDX, IDC_PD_KNOCK_RECOVERY_DELAY_SPIN, m_recovery_delay_spin);

 for(int i = 0; i < 8; ++i)
  DDX_Control(pDX, IDC_PD_KNOCK_SELCH1+i, m_selch[i]);

 DDX_Control(pDX, IDC_PD_KNOCK_CLT_THRD_EDIT, m_knkclt_thrd_edit);
 DDX_Control(pDX, IDC_PD_KNOCK_CLT_THRD_SPIN, m_knkclt_thrd_spin);

 DDX_CBIndex_UCHAR(pDX, IDC_PD_KNOCK_ENABLE_KC_COMBO, m_params.knock_use_knock_channel);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_KNOCK_BPF_FREQ_COMBO, m_params.knock_bpf_frequency);
 DDX_CBIndex_UCHAR(pDX, IDC_PD_KNOCK_INT_TIME_CONST_COMBO, m_params.knock_int_time_const);

 m_wnd_begin_angle_edit.DDX_Value(pDX, IDC_PD_KNOCK_BEGIN_KWND_EDIT, m_params.knock_k_wnd_begin_angle);
 m_wnd_end_angle_edit.DDX_Value(pDX, IDC_PD_KNOCK_END_KWND_EDIT, m_params.knock_k_wnd_end_angle);
 m_retard_step_edit.DDX_Value(pDX, IDC_PD_KNOCK_RETARD_STEP_EDIT, m_params.knock_retard_step);
 m_advance_step_edit.DDX_Value(pDX, IDC_PD_KNOCK_ADVANCE_STEP_EDIT, m_params.knock_advance_step);
 m_max_retard_edit.DDX_Value(pDX, IDC_PD_KNOCK_MAX_RETARD_EDIT, m_params.knock_max_retard);
 m_threshold_edit.DDX_Value(pDX, IDC_PD_KNOCK_THRESHOLD_EDIT, m_params.knock_threshold);
 m_recovery_delay_edit.DDX_Value(pDX, IDC_PD_KNOCK_RECOVERY_DELAY_EDIT, m_params.knock_recovery_delay);

 for(int i = 0; i < 8; ++i)
  DDX_Check_bool(pDX, IDC_PD_KNOCK_SELCH1+i, m_params.knock_selch[i]);

 m_knkclt_thrd_edit.DDX_Value(pDX, IDC_PD_KNOCK_CLT_THRD_EDIT, m_params.knkclt_thrd);
}

/////////////////////////////////////////////////////////////////////////////
// CKnockPageDlg message handlers

//Update controls
void CKnockPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CKnockPageDlg::OnInitDialog()
{
 size_t i;
 Super::OnInitDialog();

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 VERIFY(mp_ttc->AddWindow(&m_wnd_begin_angle_edit, MLL::GetString(IDS_PD_KNOCK_BEGIN_KWND_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_wnd_end_angle_edit, MLL::GetString(IDS_PD_KNOCK_END_KWND_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_retard_step_edit, MLL::GetString(IDS_PD_KNOCK_RETARD_STEP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_advance_step_edit, MLL::GetString(IDS_PD_KNOCK_ADVANCE_STEP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_max_retard_edit, MLL::GetString(IDS_PD_KNOCK_MAX_RETARD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_threshold_edit, MLL::GetString(IDS_PD_KNOCK_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_recovery_delay_edit, MLL::GetString(IDS_PD_KNOCK_RECOVERY_DELAY_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_wnd_begin_angle_spin, MLL::GetString(IDS_PD_KNOCK_BEGIN_KWND_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_wnd_end_angle_spin, MLL::GetString(IDS_PD_KNOCK_END_KWND_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_retard_step_spin, MLL::GetString(IDS_PD_KNOCK_RETARD_STEP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_advance_step_spin, MLL::GetString(IDS_PD_KNOCK_ADVANCE_STEP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_max_retard_spin, MLL::GetString(IDS_PD_KNOCK_MAX_RETARD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_threshold_spin, MLL::GetString(IDS_PD_KNOCK_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_recovery_delay_spin, MLL::GetString(IDS_PD_KNOCK_RECOVERY_DELAY_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_integrator_const_combo, MLL::GetString(IDS_PD_KNOCK_INT_TIME_CONST_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_bpf_frequency_combo, MLL::GetString(IDS_PD_KNOCK_BPF_FREQ_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_use_knock_channel_combo, MLL::GetString(IDS_PD_KNOCK_ENABLE_KC_COMBO_TT)));

 for (i = 0; i < 8; ++i)
 VERIFY(mp_ttc->AddWindow(&m_selch[i], MLL::GetString(IDS_PD_KNOCK_SELCH1_TT)));

 VERIFY(mp_ttc->AddWindow(&m_knkclt_thrd_edit, MLL::GetString(IDS_PD_KNOCK_CLT_THRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_knkclt_thrd_spin, MLL::GetString(IDS_PD_KNOCK_CLT_THRD_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //-----------------------------------------------------------------
 m_use_knock_channel_combo.AddString(MLL::LoadString(IDS_PD_NO));
 m_use_knock_channel_combo.AddString(MLL::LoadString(IDS_PD_YES));
 //-----------------------------------------------------------------
 m_bpf_frequency_combo.SetRedraw(false);
 for (i = 0; i < SECU3IO::GAIN_FREQUENCES_SIZE; i++) //fill combobox containing BPF frequencies
 {
  CString string;
  string.Format(_T("%.2f"),SECU3IO::hip9011_gain_frequences[i]);
  m_bpf_frequency_combo.AddString(string);
 }
 m_bpf_frequency_combo.SetRedraw(true);
 //-----------------------------------------------------------------
 m_integrator_const_combo.SetRedraw(false);
 for (i = 0; i < SECU3IO::INTEGRATOR_LEVELS_SIZE; i++) //Fill combobox containing integration time constants
 {
  CString string;
  string.Format(_T("%d"), (int)SECU3IO::hip9011_integrator_const[i]);
  m_integrator_const_combo.AddString(string);
 }
 m_integrator_const_combo.SetRedraw(true);
 //-----------------------------------------------------------------
 m_wnd_begin_angle_edit.SetLimitText(6);
 m_wnd_begin_angle_edit.SetDecimalPlaces(2);
 m_wnd_begin_angle_spin.SetBuddy(&m_wnd_begin_angle_edit);
 m_wnd_begin_angle_spin.SetRangeAndDelta(-12.0f, 54.0f, 1.0f);
 m_wnd_begin_angle_edit.SetRange(-12.0f, 54.0f);
 //-----------------------------------------------------------------
 m_wnd_end_angle_edit.SetLimitText(6);
 m_wnd_end_angle_edit.SetDecimalPlaces(2);
 m_wnd_end_angle_spin.SetBuddy(&m_wnd_end_angle_edit);
 m_wnd_end_angle_spin.SetRangeAndDelta(-12.0f, 54.0f, 1.0f);
 m_wnd_end_angle_edit.SetRange(-12.0f, 54.0f);
 //-----------------------------------------------------------------
 m_retard_step_edit.SetLimitText(5);
 m_retard_step_edit.SetDecimalPlaces(2);
 m_retard_step_spin.SetBuddy(&m_retard_step_edit);
 m_retard_step_spin.SetRangeAndDelta(0.0f, 20.0f, 0.25f);
 m_retard_step_edit.SetRange(0.0f, 20.0f);
 //-----------------------------------------------------------------
 m_advance_step_edit.SetLimitText(5);
 m_advance_step_edit.SetDecimalPlaces(2);
 m_advance_step_spin.SetBuddy(&m_advance_step_edit);
 m_advance_step_spin.SetRangeAndDelta(0.0f, 5.0f, 0.02f);
 m_advance_step_edit.SetRange(0.0f, 5.0f);
 //-----------------------------------------------------------------
 m_max_retard_edit.SetLimitText(5);
 m_max_retard_edit.SetDecimalPlaces(2);
 m_max_retard_spin.SetBuddy(&m_max_retard_edit);
 m_max_retard_spin.SetRangeAndDelta(0.0f, 25.0f, 0.25f);
 m_max_retard_edit.SetRange(0.0f, 25.0f);
 //-----------------------------------------------------------------
 m_threshold_edit.SetLimitText(5);
 m_threshold_edit.SetDecimalPlaces(2);
 m_threshold_spin.SetBuddy(&m_threshold_edit);
 m_threshold_spin.SetRangeAndDelta(0.1f, 5.0f, 0.01f);
 m_threshold_edit.SetRange(0.1f, 5.0f);
 //-----------------------------------------------------------------
 m_recovery_delay_edit.SetLimitText(2);
 m_recovery_delay_edit.SetDecimalPlaces(2);
 m_recovery_delay_spin.SetBuddy(&m_recovery_delay_edit);
 m_recovery_delay_spin.SetRangeAndDelta(1, 99, 1);
 m_recovery_delay_edit.SetRange(1, 99);
 //-----------------------------------------------------------------
 m_knkclt_thrd_edit.SetLimitText(6);
 m_knkclt_thrd_edit.SetDecimalPlaces(2);
 m_knkclt_thrd_spin.SetBuddy(&m_knkclt_thrd_edit);
 m_knkclt_thrd_spin.SetRangeAndDelta(-40.0f, 120.0f, 0.25f);
 m_knkclt_thrd_edit.SetRange(-40.0f, 120.0f);
 //-----------------------------------------------------------------


 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CKnockPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//разрешение/запрещение контроллов (всех поголовно)
void CKnockPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
 {
  UpdateDialogControls(this, TRUE);
 }
}

//что с контроллами?
bool CKnockPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CKnockPageDlg::GetValues(SECU3IO::KnockPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::KnockPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CKnockPageDlg::SetValues(const SECU3IO::KnockPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::KnockPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}

void CKnockPageDlg::ForceOnChangeNotify(void)
{
 OnChangeNotify();
}

void CKnockPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CKnockPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(483));
}

