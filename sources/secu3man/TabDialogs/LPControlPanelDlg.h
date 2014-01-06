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

#pragma once

#include <memory>
#include <vector>
#include "common/FastDelegate.h"
#include "ui-core/DialogWithAccelerators.h"

class CToolTipCtrlEx;

class CLPControlPanelDlg : public CModelessDialog
{
  typedef CModelessDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<size_t> EventWithCode;
  typedef fastdelegate::FastDelegate2<UINT, unsigned long> EventHScroll;

 public:
  CLPControlPanelDlg(CWnd* pParent = NULL);
  static const UINT IDD;

  //Time factor combo box
  void FillTimeFactorCombo(std::vector<_TSTRING>& i_factor_strings);
  size_t GetTimeFactor(void) const;
  void SetTimeFactor(size_t i_factor_id);

  //buttons
  void SetPlayButtonState(bool i_state);
  bool GetPlayButtonState(void) const;

  void EnableNextButton(bool i_enable);
  void EnablePrevButton(bool i_enable);
  void EnablePlayButton(bool i_enable);
  bool IsNextButtonEnabled(void) const;
  bool IsPrevButtonEnabled(void) const;
  bool IsPlayButtonEnabled(void) const;

  void SetOpenFileButtonText(const _TSTRING& i_text);

  //slider
  void SetSliderPosition(unsigned long i_position);
  unsigned long GetSliderPosition(void) const;
  void SetSliderRange(unsigned long i_begin, unsigned long i_end);
  void EnableSlider(bool i_enable);
  bool IsSliderEnabled(void) const;
  unsigned long GetSliderLineSize(void) const;
  void SetSliderLineSize(unsigned long i_line_size);
  unsigned long GetSliderPageSize(void) const;
  void SetSliderPageSize(unsigned long i_page_size);
  bool GetStopOnMarksCheck(void);

  //indicators
  void SetFileIndicator(const _TSTRING& i_string);
  void SetPositionIndicator(const _TSTRING& i_string);

  void EnableAll(bool i_enable);

  void setOnPlayButton(EventHandler i_callback);
  void setOnNextButton(EventHandler i_callback);
  void setOnPrevButton(EventHandler i_callback);
  void setOnOpenFileButton(EventHandler i_callback);
  void setOnTimeFactorCombo(EventWithCode i_callback);
  void setOnSliderMoved(EventHScroll i_callback);

 protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateNextButton(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePrevButton(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePlayButton(CCmdUI* pCmdUI);
  afx_msg void OnUpdateSlider(CCmdUI* pCmdUI);
  afx_msg void OnOpenFileButton();
  afx_msg void OnPlayButton();
  afx_msg void OnNextButton();
  afx_msg void OnPrevButton();
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnSelchangeTimeFactorCombo();
  DECLARE_MESSAGE_MAP()

private:
  CComboBox m_time_factor_combo;
  CButton m_play_button;
  CButton m_next_button;
  CButton m_prev_button;
  CButton m_open_file_button;
  CButton m_stoponmarks_check;
  CSliderCtrl m_slider;
  CStatic m_file_indicator;
  CStatic m_position_indicator;

  bool m_next_button_state;
  bool m_prev_button_state;
  bool m_play_button_state;
  bool m_slider_state;
  bool m_all_enabled;

  EventHandler m_on_play_button;
  EventHandler m_on_next_button;
  EventHandler m_on_prev_button;
  EventHandler m_on_open_file_button;
  EventWithCode m_on_time_factor_combo;
  EventHScroll m_on_slider_moved;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
