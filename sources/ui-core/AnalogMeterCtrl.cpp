 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "AnalogMeterCtrl.h"
#include "AnalogMeter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeterCtrl

CAnalogMeterCtrl::CAnalogMeterCtrl()
{
}

CAnalogMeterCtrl::~CAnalogMeterCtrl()
{
}


BEGIN_MESSAGE_MAP(CAnalogMeterCtrl, CStatic)
	//{{AFX_MSG_MAP(CAnalogMeterCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeterCtrl message handlers

void CAnalogMeterCtrl::OnPaint() 
{
  CPaintDC dc(this); // device context for painting
  CRect rectClient ;
	
  GetClientRect (&rectClient);	
  ShowMeter (&dc, rectClient) ;
  // Do not call CStatic::OnPaint() for painting messages
}

void CAnalogMeterCtrl::UpdateNeedle(double dPos)
{
  CClientDC dc(this);
  CAnalogMeter::UpdateNeedle(&dc, dPos);
}