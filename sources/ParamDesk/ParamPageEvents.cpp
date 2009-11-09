 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "ParamPageEvents.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParamPageEvents::ParamPageEvents()
: m_OnChange(NULL)
{
 //empty
}

ParamPageEvents::~ParamPageEvents()
{
 //empty
}

void ParamPageEvents::setFunctionOnChange(EventHandler OnChange)
{
 m_OnChange = OnChange;
}

void ParamPageEvents::OnChangeNotify(void)
{
 if (m_OnChange)  //call only if handler present!
  m_OnChange();
}
