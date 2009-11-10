 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "DialogWithAccelerators.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDialogWithAccelerators::CDialogWithAccelerators()
{
 //empty
}

CDialogWithAccelerators::CDialogWithAccelerators(UINT nIDTemplate, CWnd* pParentWnd /* = NULL*/)
: Super(nIDTemplate, pParentWnd)
{
 //empty
}

BOOL CDialogWithAccelerators::PreTranslateMessage(MSG* pMsg) 
{
 //Этот дурацкий код нужен для работы акселераторов, иначе они не будут работать в диалогах!
 HACCEL hAccel = ((CFrameWnd*)AfxGetApp()->m_pMainWnd)->m_hAccelTable;
 if((hAccel && ::TranslateAccelerator(AfxGetApp()->m_pMainWnd->m_hWnd, hAccel, pMsg)))
  return TRUE;
  	
 return Super::PreTranslateMessage(pMsg);
}
