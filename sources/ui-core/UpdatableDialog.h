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

#pragma once

/*--------------------------------------------------------------------------------------

Если пронаследоваться от этого класса то в немодальном диалоге будет работать механизм
ON_UPDATE_COMMAND_UI. Для модальных диалогов можно использовать недокументированное сообщение
WM_KICKIDLE или OnKickIdle()

На некоторых пратформах (например Pocket PC) может не быть сообщения 0x3FC и механизм
станет работать не совсем корректно (с точки зрения производительности). Для устраниия
проблемы необходимо добавить в класс пронаследованный от CWinApp следующий обработчик:

virtual BOOL IsIdleMessage(MSG* pMsg)
{
  if(CWinApp::IsIdleMessage(pMsg) == FALSE)
    return FALSE;

  return (pMsg->message != 0x3FC);
}

---------------------------------------------------------------------------------------*/

// - Класс добавляет акселераторы (из главного окна)
// - Класс добавляет возможность ON_UPDATE_COMMAND_UI
class AFX_EXT_CLASS CUpdatableDialog : public CDialog
{
 typedef CDialog Super;

 public:
  CUpdatableDialog();
  CUpdatableDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL );

 protected:
  //здесь будет грязный хак!
  virtual BOOL PreTranslateMessage(MSG* pMsg);

 private:
  BOOL m_bDoIdle;
};

//CUpdatableDialog который нельзя закрыть.
class AFX_EXT_CLASS CModelessUpdatableDialog : public CUpdatableDialog
{
  typedef CUpdatableDialog Super;

 public:
  CModelessUpdatableDialog();
  CModelessUpdatableDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL );

 protected:
  virtual void OnOK();
  virtual void OnCancel();
};
