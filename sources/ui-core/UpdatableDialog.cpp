 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "UpdatableDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CUpdatableDialog::CUpdatableDialog(UINT nIDTemplate, CWnd* pParentWnd /* = NULL*/)
: CDialog(nIDTemplate, pParentWnd)
{
}


BOOL CUpdatableDialog::PreTranslateMessage(MSG* pMsg) 
{
  static BOOL bDoIdle = TRUE;

  MSG msg;
  if(!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) && bDoIdle)
  {
    //вызывается один раз только когда нет сообщений в очереди
    UpdateDialogControls(this,TRUE); 
	bDoIdle = FALSE;
  }
  else
  {
    if(AfxGetApp()->IsIdleMessage(pMsg) && pMsg->message != 0x3FC)
	{
	  bDoIdle = TRUE;
	}
  }
	
  return CDialog::PreTranslateMessage(pMsg);
}
