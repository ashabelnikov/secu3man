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
#include "KnockPageDlg.h"

#include <vector>
#include "common/MathHelpers.h"
#include "propgrid/wm_messages.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT CKnockPageDlg::IDD = IDD_PD_KNOCK_PAGE;

BEGIN_MESSAGE_MAP(CKnockPageDlg, Super)
 ON_WM_DESTROY()
 ON_MESSAGE(WM_PG_ITEMCHANGED, OnItemChanged)
 ON_UPDATE_COMMAND_UI(IDC_PROPERTY_GRID,OnUpdateControls)
END_MESSAGE_MAP()

CKnockPageDlg::CKnockPageDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockPageDlg::IDD, pParent)
, m_enabled(false)
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
}

LPCTSTR CKnockPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CKnockPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PROPERTY_GRID, m_ctrlGrid);
}

/////////////////////////////////////////////////////////////////////////////
// CCarburPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CKnockPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CKnockPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 HSECTION hs = m_ctrlGrid.AddSection(MLL::GetString(IDS_PD_KNOCK_PARAMETERS));
 //-----------------------------------------------------------------
 m_knock_use_knock_channel_item = m_ctrlGrid.AddBoolItem(hs, MLL::GetString(IDS_PD_KNOCK_ENABLE_KC), m_params.knock_use_knock_channel);
 //-----------------------------------------------------------------
 CPropertyGridInPlaceEdit::InplaceEditParamsEx ex1;
 ex1.m_decimal_places = 2;
 ex1.m_spin = true;
 ex1.m_delta = 1.0; //шаг
 ex1.m_mode = CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED;
 ex1.m_lower = -12.0;
 ex1.m_upper = 54.0;
 ex1.m_limit_text = 6;
 m_knock_k_wnd_begin_angle_item = m_ctrlGrid.AddDoubleItem(hs, MLL::GetString(IDS_PD_KNOCK_BEGIN_KWND), m_params.knock_k_wnd_begin_angle,_T("%g°"),true,false,-1,&ex1);
 //-----------------------------------------------------------------
 CPropertyGridInPlaceEdit::InplaceEditParamsEx ex2;
 ex2.m_decimal_places = 2;
 ex2.m_spin = true;
 ex2.m_delta = 1.0; //шаг
 ex2.m_mode = CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED;
 ex2.m_lower = -12.0;
 ex2.m_upper = 54.0;
 ex2.m_limit_text = 6;
 m_knock_k_wnd_end_angle_item = m_ctrlGrid.AddDoubleItem(hs, MLL::GetString(IDS_PD_KNOCK_END_KWND),
     m_params.knock_k_wnd_end_angle,_T("%g°"),true,false,-1,&ex2);
 //-----------------------------------------------------------------
 vector<_TSTRING> bpf_freqs;
 for (size_t i = 0; i < SECU3IO::GAIN_FREQUENCES_SIZE; i++) //заполняем комбо бокс частот ПФ
 {
  CString string;
  string.Format(_T("%.2f"),SECU3IO::hip9011_gain_frequences[i]);
  bpf_freqs.push_back(_TSTRING(string));
 }
 m_knock_bpf_frequency_item = m_ctrlGrid.AddSelectorItem(hs,MLL::GetString(IDS_PD_KNOCK_BPF_FREQ), bpf_freqs,
     MathHelpers::Round(m_params.knock_bpf_frequency), MLL::GetString(IDS_PD_KNOCK_BPF_UNIT));

 //-----------------------------------------------------------------
 vector<_TSTRING> int_conts;
 for (i = 0; i < SECU3IO::INTEGRATOR_LEVELS_SIZE; i++) //заполняем комбо бокс постоянных времени интегрирования
 {
  CString string;
  string.Format(_T("%d"), (int)SECU3IO::hip9011_integrator_const[i]);
  int_conts.push_back(_TSTRING(string));
 }
 m_knock_integrator_const_item = m_ctrlGrid.AddSelectorItem(hs,MLL::GetString(IDS_PD_KNOCK_INT_TIME_CONSTANT), int_conts,
     m_params.knock_int_time_const, MLL::GetString(IDS_PD_KNOCK_INT_TIME_UNIT));
 //-----------------------------------------------------------------

 CPropertyGridInPlaceEdit::InplaceEditParamsEx ex3;
 ex3.m_decimal_places = 2;
 ex3.m_spin = true;
 ex3.m_delta = 0.25; //шаг
 ex3.m_mode = CEditEx::MODE_FLOAT /*| CEditEx::MODE_SIGNED*/;
 ex3.m_lower = 0.0;
 ex3.m_upper = 20.0;
 ex3.m_limit_text = 5;
 m_knock_retard_step_item = m_ctrlGrid.AddDoubleItem(hs, MLL::GetString(IDS_PD_KNOCK_RETARD_STEP), m_params.knock_retard_step,_T("%g°"),true,false,-1,&ex3);
 //-----------------------------------------------------------------
 CPropertyGridInPlaceEdit::InplaceEditParamsEx ex4;
 ex4.m_decimal_places = 2;
 ex4.m_spin = true;
 ex4.m_delta = 0.02; //шаг
 ex4.m_mode = CEditEx::MODE_FLOAT /*| CEditEx::MODE_SIGNED*/;
 ex4.m_lower = 0.0;
 ex4.m_upper = 5.0;
 ex4.m_limit_text = 5;
 m_knock_advance_step_item = m_ctrlGrid.AddDoubleItem(hs, MLL::GetString(IDS_PD_KNOCK_ADVANCE_STEP), m_params.knock_advance_step,_T("%g°"),true,false,-1,&ex4);
 //-----------------------------------------------------------------
 CPropertyGridInPlaceEdit::InplaceEditParamsEx ex5;
 ex5.m_decimal_places = 2;
 ex5.m_spin = true;
 ex5.m_delta = 0.25; //шаг
 ex5.m_mode = CEditEx::MODE_FLOAT /*| CEditEx::MODE_SIGNED*/;
 ex5.m_lower = 0.0;
 ex5.m_upper = 25.0;
 ex5.m_limit_text = 5;
 m_knock_max_retard_item = m_ctrlGrid.AddDoubleItem(hs, MLL::GetString(IDS_PD_KNOCK_MAX_RETARD), m_params.knock_max_retard,_T("%g°"),true,false,-1,&ex5);
 //-----------------------------------------------------------------
 CPropertyGridInPlaceEdit::InplaceEditParamsEx ex6;
 ex6.m_decimal_places = 2;
 ex6.m_spin = true;
 ex6.m_delta = 0.01; //шаг
 ex6.m_mode = CEditEx::MODE_FLOAT /*| CEditEx::MODE_SIGNED*/;
 ex6.m_lower = 0.1;
 ex6.m_upper = 5.0;
 ex6.m_limit_text = 5;
 m_knock_threshold_item = m_ctrlGrid.AddDoubleItem(hs, MLL::GetString(IDS_PD_KNOCK_THRESHOLD), m_params.knock_threshold,_T("%g V"),true,false,-1,&ex6);
 //-----------------------------------------------------------------
 CPropertyGridInPlaceEdit::InplaceEditParamsEx ex7;
 ex7.m_decimal_places = 2;
 ex7.m_spin = true;
 ex7.m_delta = 1; //шаг
 ex7.m_mode = CEditEx::MODE_INT /*| CEditEx::MODE_SIGNED*/;
 ex7.m_lower = 1;
 ex7.m_upper = 99;
 ex7.m_limit_text = 2;
 m_knock_recovery_delay_item = m_ctrlGrid.AddIntegerItem(hs, MLL::GetString(IDS_PD_KNOCK_RECOVERY_DELAY), m_params.knock_recovery_delay,_TSTRING(_T("%d ")) + MLL::GetString(IDS_PD_KNOCK_RECOVERY_DELAY_UNIT),true,false,-1,&ex7);
 //-----------------------------------------------------------------

 m_ctrlGrid.SetGutterWidth(180);
 m_ctrlGrid.SetEditable(m_enabled);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
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
  m_ctrlGrid.SetEditable(m_enabled);
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

 bool status = true;

 bool knock_use_knock_channel;
 if (!m_ctrlGrid.GetItemValue(m_knock_use_knock_channel_item,knock_use_knock_channel))
  status = false;
 m_params.knock_use_knock_channel = knock_use_knock_channel;

 double knock_k_wnd_begin_angle;
 if (!m_ctrlGrid.GetItemValue(m_knock_k_wnd_begin_angle_item,knock_k_wnd_begin_angle))
  status = false;
 m_params.knock_k_wnd_begin_angle = (float)knock_k_wnd_begin_angle;

 double knock_k_wnd_end_angle;
 if (!m_ctrlGrid.GetItemValue(m_knock_k_wnd_end_angle_item,knock_k_wnd_end_angle))
  status = false;
 m_params.knock_k_wnd_end_angle = (float)knock_k_wnd_end_angle;

 int knock_bpf_frequency;
 if (!m_ctrlGrid.GetItemValue(m_knock_bpf_frequency_item,knock_bpf_frequency))
   status = false;
 m_params.knock_bpf_frequency = (float)knock_bpf_frequency;

 int int_time_constant;
 if (!m_ctrlGrid.GetItemValue(m_knock_integrator_const_item,int_time_constant))
     status = false;
 m_params.knock_int_time_const = int_time_constant;

//-----------------------
 double knock_retard_step;
 if (!m_ctrlGrid.GetItemValue(m_knock_retard_step_item, knock_retard_step))
  status = false;
 m_params.knock_retard_step = (float)knock_retard_step;

 double knock_advance_step;
 if (!m_ctrlGrid.GetItemValue(m_knock_advance_step_item, knock_advance_step))
  status = false;
 m_params.knock_advance_step = (float)knock_advance_step;

 double knock_max_retard;
 if (!m_ctrlGrid.GetItemValue(m_knock_max_retard_item, knock_max_retard))
  status = false;
 m_params.knock_max_retard = (float)knock_max_retard;

 double knock_threshold;
 if (!m_ctrlGrid.GetItemValue(m_knock_threshold_item, knock_threshold))
  status = false;
 m_params.knock_threshold = (float)knock_threshold;

 int knock_recovery_delay;
 if (!m_ctrlGrid.GetItemValue(m_knock_recovery_delay_item, knock_recovery_delay))
  status = false;
 m_params.knock_recovery_delay = knock_recovery_delay;
 //-----------------------

 ASSERT(status);

 memcpy(o_values,&m_params, sizeof(SECU3IO::KnockPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CKnockPageDlg::SetValues(const SECU3IO::KnockPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::KnockPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог

 bool status = true;

 bool knock_use_knock_channel = m_params.knock_use_knock_channel;
 if (!m_ctrlGrid.SetItemValue(m_knock_use_knock_channel_item,knock_use_knock_channel))
  status = false;

 double knock_k_wnd_begin_angle = m_params.knock_k_wnd_begin_angle;
 if (!m_ctrlGrid.SetItemValue(m_knock_k_wnd_begin_angle_item,knock_k_wnd_begin_angle))
  status = false;

 double knock_k_wnd_end_angle = m_params.knock_k_wnd_end_angle;
 if (!m_ctrlGrid.SetItemValue(m_knock_k_wnd_end_angle_item,knock_k_wnd_end_angle))
  status = false;

 int knock_bpf_frequency = MathHelpers::Round(m_params.knock_bpf_frequency);
 if (!m_ctrlGrid.SetItemValue(m_knock_bpf_frequency_item, knock_bpf_frequency))
     status = false;

 int int_time_constant = (size_t)m_params.knock_int_time_const;
 if (!m_ctrlGrid.SetItemValue(m_knock_integrator_const_item, int_time_constant))
     status = false;

 //-----------------------
 double knock_retard_step = m_params.knock_retard_step;
 if (!m_ctrlGrid.SetItemValue(m_knock_retard_step_item, knock_retard_step))
  status = false;

 double knock_advance_step = m_params.knock_advance_step;
 if (!m_ctrlGrid.SetItemValue(m_knock_advance_step_item, knock_advance_step))
  status = false;

 double knock_max_retard = m_params.knock_max_retard;
 if (!m_ctrlGrid.SetItemValue(m_knock_max_retard_item, knock_max_retard))
  status = false;

 double knock_threshold = m_params.knock_threshold;
 if (!m_ctrlGrid.SetItemValue(m_knock_threshold_item, knock_threshold))
  status = false;

 int knock_recovery_delay = m_params.knock_recovery_delay;
 if (!m_ctrlGrid.SetItemValue(m_knock_recovery_delay_item, knock_recovery_delay))
  status = false;
 //-----------------------

 ASSERT(status);
}

LRESULT CKnockPageDlg::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
 if (wParam == m_knock_use_knock_channel_item ||
     wParam == m_knock_k_wnd_begin_angle_item ||
     wParam == m_knock_k_wnd_end_angle_item   ||
     wParam == m_knock_bpf_frequency_item ||
     wParam == m_knock_integrator_const_item ||
     wParam == m_knock_retard_step_item   ||
     wParam == m_knock_advance_step_item  ||
     wParam == m_knock_max_retard_item    ||
     wParam == m_knock_threshold_item     ||
     wParam == m_knock_recovery_delay_item)
 {
  OnChangeNotify();
 }
 else
 {
  ASSERT(0);
 }

 return 0;
}

void CKnockPageDlg::OnDestroy()
{
 Super::OnDestroy();
 m_ctrlGrid.ResetContents();
}
