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

/** \file LPControlPanelDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "LPControlPanelDlg.h"

#include <limits>
#include "common/FastDelegate.h"
#include "common/GDIHelpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/HatchDispCtrl.h"

using namespace std;

#undef max //numeric_limits<>::max() cases warning C4003: not enough actual parameters for macro...

using namespace fastdelegate;

CLPControlPanelDlg::CLPControlPanelDlg()
: m_next_button_state(false)
, m_prev_button_state(false)
, m_play_button_state(false)
, m_slider_state(false)
, m_all_enabled(false)
, m_marksFrame(new CHatchDispCtrl())
{
 //empty
}

BOOL CLPControlPanelDlg::Create(CWnd* pParentWnd /*= NULL*/)
{
 return Super::Create(IDD_LOG_PLAYER_CONTROL_PANEL, pParentWnd);
}

void CLPControlPanelDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_LOG_PLAYER_TIME_FACTOR_COMBO, m_time_factor_combo);
 DDX_Control(pDX, IDC_LOG_PLAYER_MAPSET_COMBO, m_mapset_combo);
 DDX_Control(pDX, IDC_LOG_PLAYER_GME_INJ_CHECK, m_gmeinj_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_GME_IGN_CHECK, m_gmeign_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_PLAY_BUTTON, m_play_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_NEXT_BUTTON, m_next_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_PREV_BUTTON, m_prev_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_FILE_NAME_INDICATOR, m_file_indicator);
 DDX_Control(pDX, IDC_LOG_PLAYER_POSITION_INDICATOR, m_position_indicator);
 DDX_Control(pDX, IDC_LOG_PLAYER_OPEN_FILE_BUTTON, m_open_file_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_POSITION_SLIDER, m_slider);
 DDX_Control(pDX, IDC_LOG_PLAYER_STOPONMARKS_CHECKBOX, m_stoponmarks_check);
 DDX_Control(pDX, IDC_LOG_PLAYER_MAPSET_CAPTION, m_mapset_caption);
 DDX_Control(pDX, IDC_LOG_PLAYER_STOPONERRORS_CHECKBOX, m_stoponerrors_check);
 DDX_Control(pDX, IDC_LP_LOGCONTROL_MARKS_FRAME, *m_marksFrame.get());
}

BEGIN_MESSAGE_MAP(CLPControlPanelDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_PREV_BUTTON, OnUpdatePrevButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_NEXT_BUTTON, OnUpdateNextButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_PLAY_BUTTON, OnUpdatePlayButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_POSITION_SLIDER, OnUpdateSlider)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_TIME_FACTOR_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_TIME_FACTOR_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_STOPONMARKS_CHECKBOX, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_STOPONERRORS_CHECKBOX, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LP_LOGCONTROL_MARKS_FRAME, OnUpdateControls)
 ON_BN_CLICKED(IDC_LOG_PLAYER_OPEN_FILE_BUTTON, OnOpenFileButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_PLAY_BUTTON, OnPlayButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_NEXT_BUTTON, OnNextButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_PREV_BUTTON, OnPrevButton)
 ON_WM_HSCROLL()
 ON_CBN_SELCHANGE(IDC_LOG_PLAYER_TIME_FACTOR_COMBO, OnSelchangeTimeFactorCombo)
 ON_CBN_SELCHANGE(IDC_LOG_PLAYER_MAPSET_COMBO, OnSelchangeMapSetCombo)
 ON_BN_CLICKED(IDC_LOG_PLAYER_GME_INJ_CHECK, OnGmeInjButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_GME_IGN_CHECK, OnGmeIgnButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_STOPONMARKS_CHECKBOX, OnStopOnMarksCheck)
 ON_BN_CLICKED(IDC_LOG_PLAYER_STOPONERRORS_CHECKBOX, OnStopOnErrorsCheck)
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
 VERIFY(mp_ttc->AddWindow(&m_open_file_button, MLL::GetString(IDS_LOG_PLAYER_OPEN_FILE_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_stoponmarks_check, MLL::GetString(IDS_LOG_PLAYER_STOPONMARKS_CHECKBOX_TT)));
 VERIFY(mp_ttc->AddWindow(&m_file_indicator, MLL::GetString(IDS_LOG_PLAYER_FILE_NAME_INDICATOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_position_indicator, MLL::GetString(IDS_LOG_PLAYER_POSITION_INDICATOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_mapset_combo, MLL::GetString(IDS_LOG_PLAYER_MAPSET_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gmeinj_button, MLL::GetString(IDS_LOG_PLAYER_GME_INJ_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_gmeign_button, MLL::GetString(IDS_LOG_PLAYER_GME_IGN_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_stoponerrors_check, MLL::GetString(IDS_LOG_PLAYER_STOPONERRORS_CHECKBOX_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //allign marks' frame to slider's channel, so their grids will match
 CRect chrc;
 m_slider.GetChannelRect(&chrc);
 CRect thrc;
 m_slider.GetThumbRect(&thrc);
 CRect mrrc = GDIHelpers::GetChildWndRect(m_marksFrame.get());
 m_marksFrame->MoveWindow(mrrc.left + chrc.left + thrc.Width()/2, mrrc.top, chrc.Width() - thrc.Width(), mrrc.Height());

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
 m_marksFrame->SetRange(i_end - i_begin);
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
 if (pScrollBar) //process messages from controls only
 {
  //use GetPos() instead of nPos, because nPos is limited to 32767!
  if (m_on_slider_moved)
   m_on_slider_moved(nSBCode, /*nPos*/m_slider.GetPos());
 }
 Super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLPControlPanelDlg::OnSelchangeTimeFactorCombo()
{
 if (m_on_time_factor_combo)
 {
  size_t factor = GetTimeFactor();
  m_on_time_factor_combo(factor);
 }
}

void CLPControlPanelDlg::OnSelchangeMapSetCombo()
{
 if (m_on_selmapset_combo)
  m_on_selmapset_combo();
}

void CLPControlPanelDlg::OnGmeIgnButton()
{
 if (m_on_gmeign_button)
  m_on_gmeign_button();
}

void CLPControlPanelDlg::OnGmeInjButton()
{
 if (m_on_gmeinj_button)
  m_on_gmeinj_button();
}

void CLPControlPanelDlg::OnStopOnMarksCheck()
{
 if (m_on_stoponmarks_check)
  m_on_stoponmarks_check();
}

void CLPControlPanelDlg::OnStopOnErrorsCheck()
{
 if (m_on_stoponerrors_check)
  m_on_stoponerrors_check();
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

bool CLPControlPanelDlg::GetStopOnErrorsCheck(void)
{
 return m_stoponerrors_check.GetCheck() == BST_CHECKED;
}

void CLPControlPanelDlg::FillMapSetCombo(std::vector<_TSTRING>& mapsets)
{
 m_mapset_combo.ResetContent();
 for(size_t i = 0; i < mapsets.size(); ++i)
  m_mapset_combo.AddString(mapsets[i].c_str());
}

void CLPControlPanelDlg::EnableMapSetCombo(bool enable)
{
 m_mapset_combo.EnableWindow(enable);
 m_mapset_caption.EnableWindow(enable);
}

void CLPControlPanelDlg::EnableGmeButtons(bool enable)
{
 m_gmeinj_button.EnableWindow(enable);
 m_gmeign_button.EnableWindow(enable);
}

void CLPControlPanelDlg::setOnSelectMapset(EventHandler i_callback)
{
 m_on_selmapset_combo = i_callback;
}

void CLPControlPanelDlg::setOnGmeIgnButton(EventHandler i_callback)
{
 m_on_gmeign_button = i_callback;
}

void CLPControlPanelDlg::setOnGmeInjButton(EventHandler i_callback)
{
 m_on_gmeinj_button = i_callback;
}

bool CLPControlPanelDlg::GetGmeIgnCheck(void)
{
 return m_gmeign_button.GetCheck()==BST_CHECKED;
}

bool CLPControlPanelDlg::GetGmeInjCheck(void)
{
 return m_gmeinj_button.GetCheck()==BST_CHECKED;
}

void CLPControlPanelDlg::SetGmeIgnCheck(bool checked)
{
 m_gmeign_button.SetCheck(checked ? BST_CHECKED : BST_UNCHECKED);
}

void CLPControlPanelDlg::SetGmeInjCheck(bool checked)
{
 m_gmeinj_button.SetCheck(checked ? BST_CHECKED : BST_UNCHECKED);
}

int CLPControlPanelDlg::GetMapSetSelection(void)
{
 int sel = m_mapset_combo.GetCurSel();
 return (sel == CB_ERR) ? -1 : sel;
}

void CLPControlPanelDlg::SetMapSetSelection(int sel)
{
 if (sel == -1)
  return;
 m_mapset_combo.SetCurSel(sel);
}

void CLPControlPanelDlg::setOnStopOnMarksCheck(EventHandler i_callback)
{
 m_on_stoponmarks_check = i_callback;
}

void CLPControlPanelDlg::setOnStopOnErrorsCheck(EventHandler i_callback)
{
 m_on_stoponerrors_check = i_callback;
}

void CLPControlPanelDlg::ResetHatch(void)
{
 m_marksFrame->Init();
}

void CLPControlPanelDlg::DrawHatch(unsigned long pos, COLORREF color)
{
 m_marksFrame->DrawHatch(pos, color);
}

void CLPControlPanelDlg::InvalidateHatch(void)
{
 m_marksFrame->DrawBitmap();
}
