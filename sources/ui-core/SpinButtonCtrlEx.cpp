 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include <math.h>
#include "SpinButtonCtrlEx.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EPS 1.0e-9

//-------------------------------------------------------------
CSpinButtonCtrlEx::CSpinButtonCtrlEx()
: m_Delta(1)
, m_MinVal(0)
, m_MaxVal(100)
, m_IntRange(100)
{
 //na
}

//-------------------------------------------------------------
CSpinButtonCtrlEx::~CSpinButtonCtrlEx()
{
 //na
}

//-------------------------------------------------------------
BEGIN_MESSAGE_MAP(CSpinButtonCtrlEx, CSpinButtonCtrl)
 ON_NOTIFY_REFLECT_EX(UDN_DELTAPOS, OnDeltapos)
 ON_WM_CREATE()
END_MESSAGE_MAP()

//-------------------------------------------------------------
void CSpinButtonCtrlEx::SetBuddyValue (double val)
{
 CEditEx*	p_edit = static_cast<CEditEx*>(GetBuddy());
 ASSERT(p_edit);
 if (p_edit)
 {
  p_edit->SetValue((float)val);
 }
 return;
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::SetPos(double value)
{
 SetBuddyValue(value);
 SetIntegerPos(value);
}

//-------------------------------------------------------------
double CSpinButtonCtrlEx::GetPos()
{
 CEditEx* p_edit = static_cast<CEditEx*>(GetBuddy());
 if (p_edit)
 {
  float val;
  p_edit->GetValue(val);
  return (float)val;
 }
 else
  return 0.0f;
}

//---------------------------------------------------------------------------------
void CSpinButtonCtrlEx::SetRangeAndDelta(double lower, double upper, double delta)
{
 m_MinVal = lower;
 m_MaxVal = upper;
 m_Delta = delta;

 // reversed min/max is not implemented, although it's probably easy
 ASSERT (m_MaxVal > m_MinVal); 

 //предотвращаем деление на 0
 if (m_Delta == 0.0)
  return;

 // Figure out the integer range to use, so that acceleration can work properly
 double range = fabs ((m_MaxVal - m_MinVal) / m_Delta);
 if (range > (double)UD_MAXVAL)
  m_IntRange = UD_MAXVAL;
 else
  m_IntRange = (int) range;
 CSpinButtonCtrl::SetRange32 (0, m_IntRange);

 // Set integer position
 SetIntegerPos (GetPos());
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::SetIntegerPos (double pos)
{
 //предотвращаем деление на 0
 if (m_MaxVal == m_MinVal)
  return;

 int int_pos;
	
 if (pos < m_MinVal)
  int_pos = 0;
 else if (pos > m_MaxVal)
  int_pos = m_IntRange;
 else
 {
  // where in the range is current position?
  double pos_in_range = (pos - m_MinVal) / (m_MaxVal - m_MinVal);
  int_pos = (int)(m_IntRange * pos_in_range + 0.5);
 }
 CSpinButtonCtrl::SetPos (int_pos);
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::GetRangeAndDelta(double& lower, double& upper, double& delta)
{
 lower = m_MinVal;
 upper = m_MaxVal;
 delta = m_Delta;
}

//-------------------------------------------------------------
BOOL CSpinButtonCtrlEx::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
 NM_UPDOWN* pUD = (NM_UPDOWN*)pNMHDR;
	
 double value = GetPos () + m_Delta * pUD->iDelta;
 const bool can_wrap = (UDS_WRAP & GetStyle());

 if (pUD->iDelta < 0) // spin down
 {
  double abs_eps = fabs(EPS * max (value, m_MinVal));
  if (abs_eps < EPS)
   abs_eps = EPS;

  if (m_MinVal - value > abs_eps)
  {  
   if(can_wrap)
	value = m_MaxVal;
   else
    value = m_MinVal;
  }
 }
 else  // spin up
 {
  double abs_eps = fabs(EPS * max (value, m_MaxVal));
  if (abs_eps < EPS) abs_eps = EPS;
		
  if (value - m_MaxVal > abs_eps)
  {   		
   if(can_wrap)
    value = m_MinVal;
   else
    value = m_MaxVal;
  }
 }

 SetBuddyValue (value);

 *pResult = 0;
 
 return FALSE; // let parent process this notification too.
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::PreSubclassWindow() 
{
 CSpinButtonCtrl::PreSubclassWindow();
 InitSpinButtonCtrl();
}

//-------------------------------------------------------------
int CSpinButtonCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
 if (CSpinButtonCtrl::OnCreate(lpCreateStruct) == -1)
  return -1;	
 InitSpinButtonCtrl();	
 return 0;
}

//-------------------------------------------------------------
void CSpinButtonCtrlEx::InitSpinButtonCtrl()
{
 ASSERT ((GetStyle () & UDS_SETBUDDYINT) != UDS_SETBUDDYINT); // Otherwise control won't work properly!
 SetRangeAndDelta (m_MinVal, m_MaxVal, m_Delta); // set default values
}

//-------------------------------------------------------------
