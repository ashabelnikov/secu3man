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

/** \file AppSettingsDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/fastdelegate.h"
#include "common/unicodesupport.h"

class CToolTipCtrlEx;

class CEditSettingsDlg : public CDialog
{
  typedef CDialog Super;

  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CEditSettingsDlg(const _TSTRING& filename, bool syntaxHighlight = true, CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

 // Implementation
 protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()

 private:
  void _HighlightSyntax(void);
  CRichEditCtrl m_edit;
  _TSTRING m_filename;
  bool m_syntaxHighlight;
};
