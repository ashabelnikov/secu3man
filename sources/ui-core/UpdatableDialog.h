
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

class AFX_EXT_CLASS CUpdatableDialog : public CDialog
{
  // Construction
  public:
    // standard constructor
	CUpdatableDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL );      

	//здесь будет грязный хак! 
	virtual BOOL PreTranslateMessage(MSG* pMsg); 
};

