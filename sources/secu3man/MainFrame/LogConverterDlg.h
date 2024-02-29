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

/** \file LogConverterDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>

class CToolTipCtrlEx;
class LogWriter;


class CLogConverterDlg : public CDialog
{
  typedef CDialog Super;

 public:
  CLogConverterDlg(CWnd* pParent, LogWriter* ip_logWriter, int fffConst);

  void SetCSVSepSymbol(char csvsep);

 protected:
  virtual BOOL OnInitDialog();
  afx_msg void OnSelectInputFile();
  afx_msg void OnSelectOutputFile();
  afx_msg void OnLogFormatCheck();
  afx_msg void OnLogTitleCheck();
  afx_msg void OnStartConversion();  
  afx_msg void OnSelendokDecPt();
  afx_msg void OnSelendokCsvSep();  
  virtual void DoDataExchange(CDataExchange* pDX);
  DECLARE_MESSAGE_MAP()

 private:
  void _EnableStartButton(void);
  void _EnableCsvControls(bool en);

 private:
  CEdit m_inp_edit;
  CEdit m_out_edit;
  CButton m_inp_file_btn;
  CButton m_out_file_btn;
  CButton m_fmt_check;
  CButton m_tit_check;
  CComboBox m_decpt_combo;
  CComboBox m_csvsep_combo;
  CButton m_start_btn;
  CStatic m_progress_text;
  CStatic m_decpt_caption;
  CStatic m_csvsep_caption;
  
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  char m_csvsep_symb;

  LogWriter* mp_logWriter;
  int m_fffConst;
};
