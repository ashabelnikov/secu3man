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

/** \file MIKnockGraph.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIKnockGraph.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIKnockGraph::CMIKnockGraph()
{
 //empty
}

CMIKnockGraph::~CMIKnockGraph()
{
 //empty
}

void CMIKnockGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD, rect, pParent, IDC_MI_KNOCKGRAPH));

 // customize the control
 m_scope.SetRange(0, 5.0, 1);
 m_scope.SetYUnits(MLL::LoadString(IDS_MI_KNOCKGRAPH_V_UNIT));
 m_scope.SetXUnits(MLL::LoadString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIKnockGraph::SetValues(void)
{
 if (m_metVal)
  m_scope.AppendPoint(*m_metVal);
}

 void CMIKnockGraph::Enable(bool enable, bool redraw /*= true*/)
 {
  m_scope.EnableWindow(enable);
 }

void CMIKnockGraph::Resize(const CRect& rect, bool redraw /*= true*/)
{
 if (m_scope.GetSafeHwnd())
  m_scope.MoveWindow(rect, redraw);
}

void CMIKnockGraph::Show(bool show)
{
 m_scope.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}
