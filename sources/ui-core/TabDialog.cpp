 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "TabDialog.h"

CTabDialog::CTabDialog(UINT nIDTemplate, CWnd* pParentWnd)
: Super(nIDTemplate, pParentWnd)
{
 //empty
}

CTabDialog::~CTabDialog()
{
 //empty
}

void CTabDialog::OnOK()
{
 UpdateData(); //for DDX/DDV
 //не вызываем реализацию базового класса чтобы диалог нельзя было закрыть
}

void CTabDialog::OnCancel()
{
 //не вызываем реализацию базового класса чтобы диалог нельзя было закрыть
}
