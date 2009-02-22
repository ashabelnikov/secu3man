/****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "StdAfx.h"
#include "CustValSelector.h"

CCustValSelector::CCustValSelector(void)
: m_selected_value_index(0) // <--by default item with zero index
{
  m_nButtonPushed = 0;
  m_unit_string = _T("");
}

CPropertyGrid::EEditMode CCustValSelector::GetEditMode()
{
  return CPropertyGrid::EM_CUSTOM;
}

void CCustValSelector::DrawItem(CDC& dc, CRect rc, bool focused)
{
  CRect rect = rc;
  rect.DeflateRect(focused?rc.Height()+2:2, 2);

  COLORREF color_normal	= GetSysColor(COLOR_BTNTEXT);
  COLORREF color_disabled = GetSysColor(COLOR_GRAYTEXT);

  if (focused)
  {
    if (m_values.size() > 0)
	{
      const TCHAR* string = m_values[m_selected_value_index].c_str();
      dc.DrawText(string, rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
	}

    // for propert clean up
    CFont* pOldFont = dc.SelectObject(m_pGrid->GetFontBold());

	if (IsLeftBoundReached() || !m_pGrid->IsFocusedItemEnabled())
     dc.SetTextColor(color_disabled);
	else
     dc.SetTextColor(color_normal);


    // the left button
    CRect rc1 = rc;
    rc1.right = rc1.left + rc1.Height();
    dc.DrawFrameControl(rc1, DFC_BUTTON, DFCS_BUTTONPUSH|((m_nButtonPushed==1 && !IsLeftBoundReached())?DFCS_PUSHED:0));
    dc.SelectObject(m_pGrid->GetFontBold());
    dc.DrawText("<", rc1, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);


    if (IsRightBoundReached() || !m_pGrid->IsFocusedItemEnabled())
     dc.SetTextColor(color_disabled);
	else
     dc.SetTextColor(color_normal);

    // the right button
    CRect rc2 = rc;
    rc2.left = rc2.right - rc2.Height();
    dc.DrawFrameControl(rc2, DFC_BUTTON, DFCS_BUTTONPUSH|((m_nButtonPushed==2 && !IsRightBoundReached()) ? DFCS_PUSHED:0));
    dc.SelectObject(m_pGrid->GetFontBold());
    dc.DrawText(">", rc2, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);

    // clean up
    dc.SelectObject(pOldFont);
  }
  else
  {
    if (m_values.size() > 0)
	{
      _TSTRING value_with_unit = m_values[m_selected_value_index] + _T(" ") + m_unit_string;
	  rc.OffsetRect(2,0);
      dc.DrawText(value_with_unit.c_str(), rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
	}  
  }
}

bool CCustValSelector::OnLButtonDown(CRect rc, CPoint pt)
{
  m_nButtonPushed = 0;
  if (rc.PtInRect(pt))
  {
    if (pt.x<=rc.left+rc.Height())
    {
      m_nButtonPushed = 1;
      return true;
    }
    if (pt.x>=rc.right-rc.Height())
    {
      m_nButtonPushed = 2;
      return true;
    }
  }
  return false;
}

void CCustValSelector::OnMouseMove(CRect rc, CPoint pt)
{
  OnLButtonDown(rc, pt);
}

void CCustValSelector::OnLButtonUp(CRect rc, CPoint pt)
{
  if (m_nButtonPushed==1 && !IsLeftBoundReached())
  {
	m_selected_value_index--;
	NotifyAboutChange();
  }
    
  if (m_nButtonPushed!=1 && !IsRightBoundReached())
  {
    m_selected_value_index++;
	NotifyAboutChange();
  }

  if (NULL!=m_pGrid && ::IsWindow(m_pGrid->m_hWnd))
   m_pGrid->Invalidate();

  m_nButtonPushed = 0;  
}

void CCustValSelector::SetValuesList(const std::vector<_TSTRING> &i_values)
{
 m_values = i_values; 
 if (NULL!=m_pGrid && ::IsWindow(m_pGrid->m_hWnd))
   m_pGrid->Invalidate();
}

void CCustValSelector::SetSelectedIndex(size_t i_value_index)
{
 if (!IsSelectedIndexInRange(i_value_index))
 {
  ASSERT(0);
  return;
 }

 m_selected_value_index = i_value_index;

 if (NULL!=m_pGrid && ::IsWindow(m_pGrid->m_hWnd))
   m_pGrid->Invalidate();
}

size_t CCustValSelector::GetSelectedIndex(void) const
{
 return m_selected_value_index;
}


void CCustValSelector::SetUnitString(const _TSTRING& i_unit_string)
{
 m_unit_string = i_unit_string;
 if (NULL!=m_pGrid && ::IsWindow(m_pGrid->m_hWnd))
   m_pGrid->Invalidate();
}


bool CCustValSelector::IsLeftBoundReached() const
{
 if (0==m_values.size()) 
   return true;
 return (m_selected_value_index <= 0);
}

bool CCustValSelector::IsRightBoundReached() const
{
 if (0==m_values.size()) 
   return true;
 return (m_selected_value_index >= m_values.size()-1);
}

bool CCustValSelector::IsSelectedIndexInRange(size_t i_index /* = INT_MAX */) const
{
 if (0==m_values.size()) 
   return false;

 if (i_index != INT_MAX)  
  return (i_index >= 0) && (i_index < m_values.size());
 else
  return (m_selected_value_index >= 0) && (m_selected_value_index < m_values.size());
}

void CCustValSelector::NotifyAboutChange(void) const
{
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