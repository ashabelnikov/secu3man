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

/** \file SpinButtonCtrlEx.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <math.h>
#include "SpinButtonCtrlEx.h"
#include "EditEx.h"

#define EPS 1.0e-9

//-------------------------------------------------------------
CSpinButtonCtrlEx::CSpinButtonCtrlEx()
: m_Delta(1)
, m_MinVal(0)
, m_MaxVal(100)
, m_IntRange(100)
{
 //empty
}

//-------------------------------------------------------------
CSpinButtonCtrlEx::~CSpinButtonCtrlEx()
{
 //empty
}

//-------------------------------------------------------------
BEGIN_MESSAGE_MAP(CSpinButtonCtrlEx, Super)
 ON_NOTIFY_REFLECT_EX(UDN_DELTAPOS, OnDeltapos)
 ON_WM_CREATE()
 ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//-------------------------------------------------------------
void CSpinButtonCtrlEx::SetBuddyValue(double val)
{
 CEditEx*	p_edit = static_cast<CEditEx*>(GetBuddy());
 ASSERT(p_edit);
 if (p_edit)
 {
  p_edit->SetValue((float)val);
 }
 return;
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::SetPos(double value)
{
 SetBuddyValue(value);
 SetIntegerPos(value);
}

//-------------------------------------------------------------
double CSpinButtonCtrlEx::GetPos()
{
 CEditEx* p_edit = static_cast<CEditEx*>(GetBuddy());
 if (p_edit)
 {
  float val;
  p_edit->GetValue(val);
  return (float)val;
 }
 else
  return 0.0f;
}

//---------------------------------------------------------------------------------
void CSpinButtonCtrlEx::SetRangeAndDelta(double lower, double upper, double delta)
{
 m_MinVal = lower;
 m_MaxVal = upper;
 m_Delta = delta;

 // reversed min/max is not implemented, although it's probably easy
 ASSERT (m_MaxVal > m_MinVal);

 //предотвращаем деление на 0
 if (m_Delta == 0.0)
  return;

 // Figure out the integer range to use, so that acceleration can work properly
 double range = fabs ((m_MaxVal - m_MinVal) / m_Delta);
 if (range > (double)UD_MAXVAL)
  m_IntRange = UD_MAXVAL;
 else
  m_IntRange = (int) range;
 Super::SetRange32 (0, m_IntRange);

 // Set integer position
 SetIntegerPos (GetPos());
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::SetIntegerPos (double pos)
{
 //предотвращаем деление на 0
 if (m_MaxVal == m_MinVal)
  return;

 int int_pos;

 if (pos < m_MinVal)
  int_pos = 0;
 else if (pos > m_MaxVal)
  int_pos = m_IntRange;
 else
 {
  // where in the range is current position?
  double pos_in_range = (pos - m_MinVal) / (m_MaxVal - m_MinVal);
  int_pos = (int)(m_IntRange * pos_in_range + 0.5);
 }
 Super::SetPos (int_pos);
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::GetRangeAndDelta(double& lower, double& upper, double& delta)
{
 lower = m_MinVal;
 upper = m_MaxVal;
 delta = m_Delta;
}

//-------------------------------------------------------------
BOOL CSpinButtonCtrlEx::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
 NM_UPDOWN* pUD = (NM_UPDOWN*)pNMHDR;

 double value = GetPos () + m_Delta * pUD->iDelta;
 const bool can_wrap = (UDS_WRAP & GetStyle());

 if (pUD->iDelta < 0) // spin down
 {
  double abs_eps = fabs(EPS * max (value, m_MinVal));
  if (abs_eps < EPS)
   abs_eps = EPS;

  if (m_MinVal - value > abs_eps)
  {
   if(can_wrap)
	value = m_MaxVal;
   else
    value = m_MinVal;
  }
 }
 else  // spin up
 {
  double abs_eps = fabs(EPS * max (value, m_MaxVal));
  if (abs_eps < EPS) abs_eps = EPS;

  if (value - m_MaxVal > abs_eps)
  {
   if(can_wrap)
    value = m_MinVal;
   else
    value = m_MaxVal;
  }
 }

 SetBuddyValue (value);

 *pResult = 0;

 return FALSE; // let parent process this notification too.
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::PreSubclassWindow()
{
 Super::PreSubclassWindow();
 InitSpinButtonCtrl();
}

//-------------------------------------------------------------
int CSpinButtonCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 if (Super::OnCreate(lpCreateStruct) == -1)
  return -1;
 InitSpinButtonCtrl();
 return 0;
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::InitSpinButtonCtrl()
{
 ASSERT ((GetStyle () & UDS_SETBUDDYINT) != UDS_SETBUDDYINT); // Otherwise control won't work properly!
 SetRangeAndDelta (m_MinVal, m_MaxVal, m_Delta); // set default values
}

//-------------------------------------------------------------
BOOL CSpinButtonCtrlEx::OnEraseBkgnd(CDC* pDC) 
{
 return TRUE; //prevent flickering
}

//-------------------------------------------------------------
