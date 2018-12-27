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

/** \file MIKnockGraph.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "MeasInstrBase.h"
#include "ui-core/OScopeCtrl.h"

class AFX_EXT_CLASS CMIKnockGraph : public MeasInstrBase
{
 public:
  CMIKnockGraph();
  virtual ~CMIKnockGraph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void SetValues(void);
  virtual void Enable(bool enable, bool redraw = true);
  virtual void Resize(const CRect& rect, bool redraw = true);
  virtual void Show(bool show);
  //---------------------------------------

  virtual CRect GetWindowRect(bool screen = false);

private:
  COScopeCtrl m_scope;
};
