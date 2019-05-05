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

/** \file ClickableBmp.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/fastdelegate.h"

class AFX_EXT_CLASS CClickableBmp : public CStatic
{
  typedef CStatic Super;
  typedef fastdelegate::FastDelegate0<void> EventHandler;

 public:
  CClickableBmp();
  virtual ~CClickableBmp();

  void SetOnClick(const EventHandler& onClickCB);
  void SetClickCursor(HCURSOR hCursor);
  void SetBitmapID(UINT nIDResource);

 protected:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  DECLARE_MESSAGE_MAP()

 private:
  EventHandler m_onClickCB;
  CBitmap m_bitmap;
  HCURSOR m_hCursor;
};
