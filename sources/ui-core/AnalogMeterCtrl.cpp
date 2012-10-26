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

#include "stdafx.h"
#include "AnalogMeterCtrl.h"
#include "AnalogMeter.h"

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeterCtrl

CAnalogMeterCtrl::CAnalogMeterCtrl()
{
 //empty
}

CAnalogMeterCtrl::~CAnalogMeterCtrl()
{
 //empty
}

BEGIN_MESSAGE_MAP(CAnalogMeterCtrl, CStatic)
 ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeterCtrl message handlers

void CAnalogMeterCtrl::OnPaint()
{
 CPaintDC dc(this); // device context for painting
 CRect rectClient ;

 GetClientRect (&rectClient);
 ShowMeter (&dc, rectClient) ;
 // Do not call CStatic::OnPaint() for painting messages
}

void CAnalogMeterCtrl::UpdateNeedle(double dPos)
{
 CClientDC dc(this);
 CAnalogMeter::UpdateNeedle(&dc, dPos);
}

void CAnalogMeterCtrl::Redraw(void)
{
 m_boolForceRedraw = true;
 Invalidate();
}
