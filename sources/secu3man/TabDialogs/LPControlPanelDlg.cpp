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
#include "LPControlPanelDlg.h"

#include <limits>
#include "common/FastDelegate.h"
#include "ui-core/ToolTipCtrlEx.h"

using namespace std;

#undef max //numeric_limits<>::max() cases warning C4003: not enough actual parameters for macro...

using namespace fastdelegate;

const UINT CLPControlPanelDlg::IDD = IDD_LOG_PLAYER_CONTROL_PANEL;

CLPControlPanelDlg::CLPControlPanelDlg(CWnd* pParent /*=NULL*/)
: Super(CLPControlPanelDlg::IDD, pParent)
, m_next_button_state(false)
, m_prev_button_state(false)
, m_play_button_state(false)
, m_slider_state(false)
, m_all_enabled(false)
{
 //empty
}

void CLPControlPanelDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_LOG_PLAYER_TIME_FACTOR_COMBO, m_time_factor_combo);
 DDX_Control(pDX, IDC_LOG_PLAYER_PLAY_BUTTON, m_play_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_NEXT_BUTTON, m_next_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_PREV_BUTTON, m_prev_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_FILE_NAME_INDICATOR, m_file_indicator);
 DDX_Control(pDX, IDC_LOG_PLAYER_POSITION_INDICATOR, m_position_indicator);
 DDX_Control(pDX, IDC_LOG_PLAYER_OPEN_FILE_BUTTON, m_open_file_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_POSITION_SLIDER, m_slider);
 DDX_Control(pDX, IDC_LOG_PLAYER_STOPONMARKS_CHECKBOX, m_stoponmarks_check);
}

BEGIN_MESSAGE_MAP(CLPControlPanelDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_PREV_BUTTON, OnUpdatePrevButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_NEXT_BUTTON, OnUpdateNextButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_PLAY_BUTTON, OnUpdatePlayButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_POSITION_SLIDER, OnUpdateSlider)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_TIME_FACTOR_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_TIME_FACTOR_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_STOPONMARKS_CHECKBOX, OnUpdateControls)
 ON_BN_CLICKED(IDC_LOG_PLAYER_OPEN_FILE_BUTTON, OnOpenFileButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_PLAY_BUTTON, OnPlayButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_NEXT_BUTTON, OnNextButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_PREV_BUTTON, OnPrevButton)
 ON_WM_HSCROLL()
 ON_CBN_SELCHANGE(IDC_LOG_PLAYER_TIME_FACTOR_COMBO, OnSelchangeTimeFactorCombo)
END_MESSAGE_MAP()

BOOL CLPControlPanelDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_prev_button, MLL::GetString(IDS_LOG_PLAYER_PREV_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_play_button, MLL::GetString(IDS_LOG_PLAYER_PLAY_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_next_button, MLL::GetString(IDS_LOG_PLAYER_NEXT_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_time_factor_combo, MLL::GetString(IDS_LOG_PLAYER_TIME_FACTOR_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_slider, MLL::GetString(IDS_LOG_PLAYER_POSITION_SLIDER_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this,TRUE);
 return TRUE;
}

void CLPControlPanelDlg::FillTimeFactorCombo(std::vector<_TSTRING>& i_factor_strings)
{
 for(std::vector<_TSTRING>::iterator it = i_factor_strings.begin(); it != i_factor_strings.end(); ++it)
 {
  int index = m_time_factor_combo.AddString((*it).c_str());
  ASSERT(index != LB_ERR);
  m_time_factor_combo.SetItemData(index, std::distance(i_factor_strings.begin(), it));
 }
}

size_t CLPControlPanelDlg::GetTimeFactor(void) const
{
 int index = m_time_factor_combo.GetCurSel();
 return (index != LB_ERR ? m_time_factor_combo.GetItemData(index) : numeric_limits<size_t>::max());
}

void CLPControlPanelDlg::SetTimeFactor(size_t i_factor_id)
{
 int count = m_time_factor_combo.GetCount();
 for(int i = 0; i < count; ++i)
 {
  size_t id = m_time_factor_combo.GetItemData(i);
  if (id==i_factor_id)
  {
   m_time_factor_combo.SetCurSel(i);
   return;
  }
 }
 ASSERT(0); //unknown item id!
}

void CLPControlPanelDlg::SetPlayButtonState(bool i_state)
{
 m_play_button.SetCheck(i_state ? BST_CHECKED : BST_UNCHECKED);
}

bool CLPControlPanelDlg::GetPlayButtonState(void) const
{
 return m_play_button.GetCheck() == BST_CHECKED;
}

void CLPControlPanelDlg::EnableNextButton(bool i_enable)
{
 m_next_button_state = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CLPControlPanelDlg::EnablePrevButton(bool i_enable)
{
 m_prev_button_state = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CLPControlPanelDlg::SetSliderPosition(unsigned long i_position)
{
 m_slider.SetPos(i_position);
}

unsigned long CLPControlPanelDlg::GetSliderPosition(void) const
{
 return m_slider.GetPos();
}

void CLPControlPanelDlg::SetSliderRange(unsigned long i_begin, unsigned long i_end)
{
 m_slider.SetRangeMin(i_begin);
 m_slider.SetRangeMax(i_end);
}

void CLPControlPanelDlg::SetFileIndicator(const _TSTRING& i_string)
{
 m_file_indicator.SetWindowText(i_string.c_str());
}

void CLPControlPanelDlg::SetPositionIndicator(const _TSTRING& i_string)
{
 m_position_indicator.SetWindowText(i_string.c_str());
}

void CLPControlPanelDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_all_enabled);
}

void CLPControlPanelDlg::OnUpdateNextButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_next_button_state && m_all_enabled);
}

void CLPControlPanelDlg::OnUpdatePrevButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_prev_button_state && m_all_enabled);
}

void CLPControlPanelDlg::OnUpdatePlayButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_play_button_state && m_all_enabled);
}

void CLPControlPanelDlg::OnUpdateSlider(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_slider_state && m_all_enabled);
}

void CLPControlPanelDlg::EnableAll(bool i_enable)
{
 m_all_enabled = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CLPControlPanelDlg::OnOpenFileButton()
{
 if (m_on_open_file_button)
  m_on_open_file_button();
}

void CLPControlPanelDlg::OnPlayButton()
{
 if (m_on_play_button)
  m_on_play_button();
}

void CLPControlPanelDlg::OnNextButton()
{
 if (m_on_next_button)
  m_on_next_button();
}

void CLPControlPanelDlg::OnPrevButton()
{
 if (m_on_prev_button)
  m_on_prev_button();
}

void CLPControlPanelDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
 if (m_on_slider_moved)
  m_on_slider_moved(nSBCode, nPos);
}

void CLPControlPanelDlg::OnSelchangeTimeFactorCombo()
{
 if (m_on_time_factor_combo)
 {
  size_t factor = GetTimeFactor();
  m_on_time_factor_combo(factor);
 }
}

void CLPControlPanelDlg::setOnPlayButton(EventHandler i_callback)
{
 m_on_play_button = i_callback;
}

void CLPControlPanelDlg::setOnNextButton(EventHandler i_callback)
{
 m_on_next_button = i_callback;
}

void CLPControlPanelDlg::setOnPrevButton(EventHandler i_callback)
{
 m_on_prev_button = i_callback;
}

void CLPControlPanelDlg::setOnOpenFileButton(EventHandler i_callback)
{
 m_on_open_file_button = i_callback;
}

void CLPControlPanelDlg::setOnTimeFactorCombo(EventWithCode i_callback)
{
 m_on_time_factor_combo = i_callback;
}

void CLPControlPanelDlg::setOnSliderMoved(EventHScroll i_callback)
{
 m_on_slider_moved = i_callback;
}

void CLPControlPanelDlg::SetOpenFileButtonText(const _TSTRING& i_text)
{
 m_open_file_button.SetWindowText(i_text.c_str());
}

bool CLPControlPanelDlg::IsNextButtonEnabled(void) const
{
 return m_next_button_state;
}

bool CLPControlPanelDlg::IsPrevButtonEnabled(void) const
{
 return m_prev_button_state;
}

void CLPControlPanelDlg::EnablePlayButton(bool i_enable)
{
 m_play_button_state = i_enable;
 UpdateDialogControls(this,TRUE);
}

bool CLPControlPanelDlg::IsPlayButtonEnabled(void) const
{
 return m_play_button_state;
}

void CLPControlPanelDlg::EnableSlider(bool i_enable)
{
 m_slider_state = i_enable;
 UpdateDialogControls(this,TRUE);
}

bool CLPControlPanelDlg::IsSliderEnabled(void) const
{
 return m_slider_state;
}

unsigned long CLPControlPanelDlg::GetSliderLineSize(void) const
{
 return m_slider.GetLineSize();
}

void CLPControlPanelDlg::SetSliderLineSize(unsigned long i_line_size)
{
 m_slider.SetLineSize(i_line_size);
}

unsigned long CLPControlPanelDlg::GetSliderPageSize(void) const
{
 return m_slider.GetPageSize();
}

void CLPControlPanelDlg::SetSliderPageSize(unsigned long i_page_size)
{
 m_slider.SetPageSize(i_page_size);
}

bool CLPControlPanelDlg::GetStopOnMarksCheck(void)
{
 return m_stoponmarks_check.GetCheck() == BST_CHECKED;
}
