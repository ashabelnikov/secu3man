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
#include "ui-core/TabDialog.h"

class CCEDeskDlg;
class CLPControlPanelDlg;
class CMIDeskDlg;
class COScopeCtrl;
class CToolTipCtrlEx;

class CLogPlayerTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate1<_TSTRING> EventString;

 public:
  CLogPlayerTabDlg(CWnd* pParent = NULL);
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  void EnlargeMonitor(bool i_enlarge);

  void AppendKnockValue(double i_value, bool i_reverse);
  void ResetKnockOscilloscope(void);

  //Drag and Drop event handler
  void setOnDropFile(EventString OnFunction);

 public:
  std::auto_ptr<CCEDeskDlg> mp_CEDeskDlg;
  std::auto_ptr<CMIDeskDlg> mp_MIDeskDlg;
  std::auto_ptr<CLPControlPanelDlg> mp_LPPanelDlg;

  //additional: for displaying of knock
  std::auto_ptr<COScopeCtrl> mp_OScopeCtrl;

 protected:
  void _ResizeRect(const CRect& i_external, CRect& io_victim);
  void _InitializeOscilloscopeControl(void);

  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  DECLARE_MESSAGE_MAP()

  EventString m_OnDropFile;

  CRect m_original_mi_rect;
  CRect m_original_ce_rect;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
