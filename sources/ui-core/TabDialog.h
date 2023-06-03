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

/** \file TabDialog.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "UpdatableDialog.h"


//Этот класс добавляет функциональность заключающуюся в том, чтобы диалог нельзя было
//закрыть по IDOK или IDCANCEL.
class AFX_EXT_CLASS CTabDialog : public CUpdatableDialog
{
  typedef CUpdatableDialog Super;

 public:
  CTabDialog();
  virtual ~CTabDialog();

  //возвращает идентификатор диалога
  virtual LPCTSTR GetDialogID(void) const = 0;

  virtual void OnShow(bool show) {};

  //диалоговые вкладки в Tab-контроле нельзя закрывать по IDOK или IDCANCEL
  virtual void OnOK();
  virtual void OnCancel();
};
