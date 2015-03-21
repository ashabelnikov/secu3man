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

/** \file BootLoaderAdapter.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "BootLoader.h"

//Этот адаптер необходим для разрешения проблемы вызова MFC функций из "неродного" потока

//ACHTUNG! CWnd должен наследоваться первым, иначе класс будет неправильно работать!
class AFX_EXT_CLASS CBootLoaderAdapter  : public CWnd, public IBLDEventHandler
{
 public:
  CBootLoaderAdapter();
  virtual ~CBootLoaderAdapter();

  //невидимое окно необходимо нам только для того чтобы посылать самим себе сообщения!
  BOOL Create(CWnd* pParentWnd);

  //установка обработчика событий
  void SetEventHandler(IBLDEventHandler* ip_destination_handler);

 private:
  IBLDEventHandler* m_destination_handler;

  virtual void OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data);
  virtual void OnBegin(const int opcode,const int status);
  virtual void OnEnd(const int opcode,const int status);

 protected:
  afx_msg LRESULT msgOnUpdateUI(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT msgOnBegin(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT msgOnEnd(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP();
};
