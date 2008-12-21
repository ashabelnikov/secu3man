// Custom check box button control for property grid.
// Alexey A. Shabelnikov. Created at Monday December 8, 2008, 00:46 o'clock (Kiev) 


#include "StdAfx.h"
#include "BtnCheckBox.h"

CBtnCheckBox::CBtnCheckBox(void)
: m_clrLeft(RGB(0,0,0))
, m_clrRight(RGB(0,255,0))
, m_nButtonPushed(0)
, m_state(0)
{
 m_pGrid = NULL; //member of the base class
}

CPropertyGrid::EEditMode CBtnCheckBox::GetEditMode()
{
 return CPropertyGrid::EM_CUSTOM;
}

void CBtnCheckBox::DrawItem(CDC& dc, CRect rc, bool focused)
{
//if (focused)
//{
   // for propert clean up
   CFont* pOldFont = dc.SelectObject(m_pGrid->GetFontBold());

   // the right button
   CRect rc2 = rc;
   rc2.left = rc2.right - rc2.Height();
   dc.DrawFrameControl(rc2, DFC_BUTTON, DFCS_BUTTONPUSH|(m_nButtonPushed==true || m_state==1?DFCS_PUSHED:0));
   dc.SelectObject(m_pGrid->GetFontBold());
   dc.DrawText("...", rc2, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
   // clean up
   dc.SelectObject(pOldFont);

   if (m_nButtonPushed==true)
	dc.DrawText(m_checked_text, rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
   else
	dc.DrawText(m_unchecked_text, rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
//}
}

bool CBtnCheckBox::OnLButtonDown(CRect rc, CPoint pt)
{  
 if (rc.PtInRect(pt) && pt.x >= rc.right-rc.Height())
 {
  m_state = 1; 
  return true;
 }

 m_state = 0;
 return false;
}

void CBtnCheckBox::OnMouseMove(CRect rc, CPoint pt)
{
 OnLButtonDown(rc, pt);
}

void CBtnCheckBox::OnLButtonUp(CRect rc, CPoint pt)
{
 if (rc.PtInRect(pt) && pt.x >= rc.right-rc.Height() && m_state == 1)
 {
  //mouse released in button rect - change button state
  m_nButtonPushed = !m_nButtonPushed;   


  if (NULL==m_pGrid)
  {
   ASSERT(0);
   return; //emergency situation
  }

  //notify using parent object mechanism.
  HITEM this_item_id = 0;
  if (m_pGrid->GetFocusedItemID(this_item_id))
  {
   m_pGrid->GetOwner()->SendMessage(WM_PG_ITEMCHANGED, this_item_id);
   m_pGrid->Invalidate();
  }
  else
   ASSERT(0);
 }

 //mouse has been released outside of button rect - reset state
 m_state = 0;
}

bool CBtnCheckBox::IsButtonPressed(void) const
{
 return m_nButtonPushed;
}

void CBtnCheckBox::SetButtonState(bool i_state)
{
 m_nButtonPushed = i_state;
 if (NULL!=m_pGrid && ::IsWindow(m_pGrid->m_hWnd))
   m_pGrid->Invalidate();    
}

void CBtnCheckBox::SetCheckedText(const CString &i_text)
{
 m_checked_text = i_text;
 if (NULL!=m_pGrid && ::IsWindow(m_pGrid->m_hWnd))
   m_pGrid->Invalidate();    
}

void CBtnCheckBox::SetUncheckedText(const CString &i_text)
{
 m_unchecked_text = i_text;
 if (NULL!=m_pGrid && ::IsWindow(m_pGrid->m_hWnd))
   m_pGrid->Invalidate();    
}
