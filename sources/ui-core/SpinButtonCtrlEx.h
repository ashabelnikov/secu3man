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

/** \file SpinButtonCtrlEx.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

// Note! control must be with UDS_AUTOBUDDY style and without UDS_SETBUDDYINT style!

class AFX_EXT_CLASS CSpinButtonCtrlEx : public CSpinButtonCtrl
{
  typedef CSpinButtonCtrl Super;
 public:
  CSpinButtonCtrlEx();
  virtual ~CSpinButtonCtrlEx();

  double GetPos();
  void   SetPos(double val);

  void SetRangeAndDelta(double lower, double upper, double delta);
  void GetRangeAndDelta(double &lower, double& upper, double& delta);

 protected:
  void InitSpinButtonCtrl();
  void SetIntegerPos(double pos);
  void SetBuddyValue(double value);

 protected:
  virtual void PreSubclassWindow();
  afx_msg BOOL OnDeltapos (NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  DECLARE_MESSAGE_MAP()

 private:
  UINT   m_IntRange;
  double m_MinVal;
  double m_MaxVal;
  double m_Delta;
};
