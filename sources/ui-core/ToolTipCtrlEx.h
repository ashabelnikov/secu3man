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

/** \file ToolTipCtrlEx.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <list>
#include "../common/unicodesupport.h"

//Custom tooltip control
class AFX_EXT_CLASS CToolTipCtrlEx : public CToolTipCtrl
{
  typedef CToolTipCtrl Super;

 public:
  CToolTipCtrlEx();
  virtual ~CToolTipCtrlEx();

  //Create custom methods
  bool AddWindow(CWnd* pWnd, const _TSTRING& text);
  bool AddRectangle(CWnd* pWnd, const _TSTRING& text, LPCRECT pRect, UINT nIDTool);
  //updates text on the already created tooltip
  bool UpdateText(CWnd* pWnd, const _TSTRING& text);

  //This method affects all instances of this class
  static void ActivateAllTooltips(bool i_activate, bool update_state = true);

  //Activates/Deactivates tool tip control. Use this method instead of base implementation
  void ActivateToolTips(bool i_activate);

  //Set time during which tool tips will be visible, value in ms
  static void SetVisibleTime(int timems);

  virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = 0);

 private:
  static std::list<CToolTipCtrlEx*> m_insts;
  static bool m_activated_all;
  static int  m_visibilityTime;
  bool m_activated;
};
