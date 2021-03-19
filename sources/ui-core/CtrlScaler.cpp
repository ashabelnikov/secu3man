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

/** \file CtrlScaler.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "common\GDIHelpers.h"
#include "CtrlScaler.h"

CtrlScaler::CtrlScaler()
: mp_parent(NULL)
{
 m_scaleInfo.reserve(64);
}

CtrlScaler::~CtrlScaler()
{
 //empty
}

void CtrlScaler::Init(CWnd* p_parent)
{
 ASSERT(p_parent);
 p_parent->GetClientRect(m_origRect);
 mp_parent = p_parent;
 m_scaleInfo.clear();
}

void CtrlScaler::Add(CWnd* p_ctrl)
{
 m_scaleInfo.push_back(std::make_pair(p_ctrl, GDIHelpers::GetChildWndRect(p_ctrl)));
}

void CtrlScaler::Add(CStatic* p_ctrl, int num)
{
 for(int i = 0; i < num; ++i)
  m_scaleInfo.push_back(std::make_pair(&p_ctrl[i], GDIHelpers::GetChildWndRect(&p_ctrl[i])));
}

void CtrlScaler::Scale(bool invalidate /*= true*/)
{
 if (NULL==mp_parent)
  return;
 CRect rect;
 mp_parent->GetClientRect(&rect);
 if (rect.IsRectNull())
  return;

 //Calculate scale factors basing on the previous size of window
 float Xf, Yf;
 GDIHelpers::CalcRectToRectRatio(rect, m_origRect, Xf, Yf);

 //Resize controls
 for(size_t i = 0; i < m_scaleInfo.size(); ++i)
 {
  CRect rc = m_scaleInfo[i].second;
  GDIHelpers::ScaleRect(rc, Xf, Yf);
  m_scaleInfo[i].first->MoveWindow(rc);
  if (invalidate)
   m_scaleInfo[i].first->Invalidate();
 }

 if (invalidate)
  mp_parent->UpdateWindow();
}
