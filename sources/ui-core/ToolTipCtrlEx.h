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

  //This method affects all instances of this class
  static void ActivateAllTooltips(bool i_activate);

  //Activates/Deactivates tool tip control. Use this method instead of base implementation
  void ActivateToolTips(bool i_activate);

 private:
  static std::list<CToolTipCtrlEx*> m_insts;
  static bool m_activated_all;
  bool m_activated;
};
