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

/** \file LPControlPanelDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <vector>
#include "common/FastDelegate.h"
#include "ui-core/DialogWithAccelerators.h"

class CToolTipCtrlEx;
class CHatchDispCtrl;

class CLPControlPanelDlg : public CModelessDialog
{
  typedef CModelessDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<size_t> EventWithCode;
  typedef fastdelegate::FastDelegate2<UINT, unsigned long> EventHScroll;

 public:
  CLPControlPanelDlg();
  virtual BOOL Create(CWnd* pParentWnd = NULL);

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
  bool GetStopOnErrorsCheck(void);

  //indicators
  void SetFileIndicator(const _TSTRING& i_string);
  void SetPositionIndicator(const _TSTRING& i_string);

  void EnableAll(bool i_enable);

  void ResetHatch(void);
  void DrawHatch(unsigned long pos, COLORREF color);
  void InvalidateHatch(void);

  void setOnPlayButton(EventHandler i_callback);
  void setOnNextButton(EventHandler i_callback);
  void setOnPrevButton(EventHandler i_callback);
  void setOnOpenFileButton(EventHandler i_callback);
  void setOnTimeFactorCombo(EventWithCode i_callback);
  void setOnSliderMoved(EventHScroll i_callback);
  void setOnStopOnMarksCheck(EventHandler i_callback);
  void setOnStopOnErrorsCheck(EventHandler i_callback);

  //related to maps
  void FillMapSetCombo(std::vector<_TSTRING>& mapsets);
  void EnableMapSetCombo(bool enable);
  void EnableGmeButtons(bool enable);
  void setOnSelectMapset(EventHandler i_callback);
  void setOnGmeIgnButton(EventHandler i_callback);
  void setOnGmeInjButton(EventHandler i_callback);
  bool GetGmeIgnCheck(void);
  bool GetGmeInjCheck(void);
  void SetGmeIgnCheck(bool checked);
  void SetGmeInjCheck(bool checked);
  int GetMapSetSelection(void);
  void SetMapSetSelection(int sel);

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
  afx_msg void OnSelchangeMapSetCombo();
  afx_msg void OnGmeIgnButton();
  afx_msg void OnGmeInjButton();
  afx_msg void OnStopOnMarksCheck();
  afx_msg void OnStopOnErrorsCheck();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnDestroy();
  DECLARE_MESSAGE_MAP()

private:
  void _AllignMarksFrameToSlider(void);
  CComboBox m_time_factor_combo;
  CButton m_play_button;
  CButton m_next_button;
  CButton m_prev_button;
  CButton m_open_file_button;
  CButton m_stoponmarks_check;
  CButton m_stoponerrors_check;
  CSliderCtrl m_slider;
  CStatic m_file_indicator;
  CStatic m_position_indicator;
  CButton m_gmeinj_button;
  CButton m_gmeign_button;
  CComboBox m_mapset_combo;
  CStatic m_mapset_caption;

  bool m_initialized;
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
  EventHandler m_on_selmapset_combo;
  EventHandler m_on_gmeign_button;
  EventHandler m_on_gmeinj_button;
  EventHandler m_on_stoponmarks_check;
  EventHandler m_on_stoponerrors_check;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CHatchDispCtrl> m_marksFrame;
};
