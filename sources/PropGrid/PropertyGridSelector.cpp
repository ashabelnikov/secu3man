 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.org
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "PropertyGridSelector.h"
#include "wm_messages.h"

using namespace std;

static const int margin = 2;

// CPropertyGridSelector

IMPLEMENT_DYNCREATE(CPropertyGridSelector, CWnd)

CPropertyGridSelector::CPropertyGridSelector()
: m_pFont(NULL)
, m_nSelected(-1)
, m_lbutton_pushed(false)
, m_lbutton_depressed(false)
, m_rbutton_pushed(false)
, m_rbutton_depressed(false)
{
 m_clrBack = GetSysColor(COLOR_WINDOW);
 m_clrText = GetSysColor(COLOR_WINDOWTEXT);
}

CPropertyGridSelector::~CPropertyGridSelector()
{
 //empty
}

//
// content management
//

void CPropertyGridSelector::AddString(_TSTRING strItem)
{
 m_Items.push_back(strItem);
}

void CPropertyGridSelector::SetCurSel(int nItem)
{
 m_nSelected = nItem;
}

BEGIN_MESSAGE_MAP(CPropertyGridSelector, CWnd)
 ON_WM_PAINT()
 ON_WM_SHOWWINDOW()
 ON_WM_LBUTTONDOWN()
 ON_WM_MOUSEMOVE()
 ON_WM_LBUTTONUP()
 ON_WM_DESTROY()
 ON_WM_KILLFOCUS()
 ON_WM_KEYDOWN()
 ON_WM_KEYUP()
 ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

//
// creation
//

BOOL CPropertyGridSelector::Create(DWORD dwStyle, CRect& rc, CWnd* pParent, int nId)
{
 pParent->ClientToScreen(&rc);
 BOOL ret = Super::CreateEx(0, AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW), _T(""), dwStyle|WS_POPUP, rc, pParent->GetParent(), nId);
 if (ret) SetOwner(pParent);
 return ret;
}

void CPropertyGridSelector::SetFont(CFont* pFont, BOOL bRedraw)
{
 m_pFont = pFont;
 CWnd::SetFont(pFont, bRedraw);
}

void CPropertyGridSelector::SetColors(COLORREF clrBack, COLORREF clrText)
{
 m_clrBack = clrBack;
 m_clrText = clrText;
}

void CPropertyGridSelector::OnShowWindow(BOOL bShow, UINT nStatus)
{
 if (bShow)
 {
  //empty
 }

 CWnd::OnShowWindow(bShow, nStatus);
}

void CPropertyGridSelector::OnDestroy()
{
 CWnd::OnDestroy();
}

void CPropertyGridSelector::OnKillFocus(CWnd* pNewWnd)
{
 CWnd::OnKillFocus(pNewWnd);
 DestroyWindow();
}

//
// painting
//

void CPropertyGridSelector::OnPaint()
{
 // check
 if (m_nSelected < 0) m_nSelected = 0;
 if (m_nSelected > int(m_Items.size()) - 1) m_nSelected = int(m_Items.size()) - 1;

 {
  // client rect
  CRect rc;
  GetClientRect(&rc);   //calculations
  m_rc_lbutton = rc, m_rc_rbutton = rc;
  m_rc_lbutton.right = m_rc_lbutton.left + m_rc_lbutton.Height(); //квадратная кнопочка
  m_rc_rbutton.left  = m_rc_rbutton.right - m_rc_rbutton.Height();
  m_rcRect = CRect(m_rc_lbutton.right, rc.top, m_rc_rbutton.left, rc.bottom);
 }

 // creation of brush and pen
 CBrush brush;
 brush.CreateSolidBrush(m_clrBack);

 // the dc
 CPaintDC dc(this);
 CBrush* pOldBrush = dc.SelectObject(&brush);
 CFont* pOldFont = dc.SelectObject(m_pFont);

 // draw
 dc.FillRect(m_rcRect,&brush);
 dc.SetBkMode(TRANSPARENT);
 dc.SetTextColor(m_clrText);
 dc.DrawText(m_Items[m_nSelected].c_str(), m_rcRect, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_NOPREFIX);

 // clean up
 dc.SelectObject(pOldFont);
 dc.SelectObject(pOldBrush);

 //draw buttons
 COLORREF color_normal	= GetSysColor(COLOR_BTNTEXT);
 COLORREF color_disabled = GetSysColor(COLOR_GRAYTEXT);

 if (IsLeftBoundReached())
  dc.SetTextColor(color_disabled);
 else
  dc.SetTextColor(color_normal);
 // the left button
 dc.DrawFrameControl(m_rc_lbutton, DFC_BUTTON, DFCS_BUTTONPUSH|((m_lbutton_depressed && !IsLeftBoundReached())?DFCS_PUSHED:0));
 dc.DrawText("<", m_rc_lbutton, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);

 if (IsRightBoundReached())
  dc.SetTextColor(color_disabled);
 else
  dc.SetTextColor(color_normal);
 // the right button
 dc.DrawFrameControl(m_rc_rbutton, DFC_BUTTON, DFCS_BUTTONPUSH|((m_rbutton_depressed && !IsRightBoundReached()) ? DFCS_PUSHED:0));
 dc.DrawText(">", m_rc_rbutton, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
}

//
// mouse interaction
//

void CPropertyGridSelector::OnLButtonDown(UINT nFlags, CPoint point)
{
 if (m_rc_lbutton.PtInRect(point))
 {
  m_lbutton_pushed = true;
  m_lbutton_depressed = true;
  SetCapture();
  Invalidate();
  return;
 }
 if (m_rc_rbutton.PtInRect(point))
 {
  m_rbutton_pushed = true;
  m_rbutton_depressed = true;
  SetCapture();
  Invalidate();
  return;
 }

 CWnd::OnLButtonDown(nFlags, point);
}

void CPropertyGridSelector::OnMouseMove(UINT nFlags, CPoint point)
{
 if (m_lbutton_pushed)
 {
  m_lbutton_depressed = m_rc_lbutton.PtInRect(point)?true:false;
  Invalidate();
 }
 if (m_rbutton_pushed)
 {
  m_rbutton_depressed = m_rc_rbutton.PtInRect(point)?true:false;
  Invalidate();
 }

 CWnd::OnMouseMove(nFlags, point);
}

void CPropertyGridSelector::OnLButtonUp(UINT nFlags, CPoint point)
{
 if (m_lbutton_pushed)
 {
  m_lbutton_pushed = false;
  m_lbutton_depressed = false;
  ReleaseCapture();
  Invalidate();

  if (m_rc_lbutton.PtInRect(point))
   m_nSelected = max(0, m_nSelected-1);
  Invalidate();
 }

 if (m_rbutton_pushed)
 {
  m_rbutton_pushed = false;
  m_rbutton_depressed = false;
  ReleaseCapture();
  Invalidate();

  if (m_rc_rbutton.PtInRect(point))
   m_nSelected = min(int(m_Items.size())-1, m_nSelected+1);
  Invalidate();
 }
}

//
// keyboard interaction
//

UINT CPropertyGridSelector::OnGetDlgCode()
{
 return DLGC_WANTALLKEYS;
}

void CPropertyGridSelector::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 if (nChar == VK_LEFT || nChar == VK_UP)
 {
  m_nSelected = max(0, m_nSelected-1);
  m_lbutton_depressed = true;
  Invalidate();
 }
 else if (nChar == VK_RIGHT || nChar == VK_DOWN)
 {
  m_nSelected = min(int(m_Items.size())-1, m_nSelected+1);
  m_rbutton_depressed = true;
  Invalidate();
 }
 else if (nChar == VK_ESCAPE)
 {
  DestroyWindow();
  return;
 }
 else if (nChar == VK_RETURN || nChar == VK_EXECUTE)
 {
  GetOwner()->SetFocus();
  GetOwner()->SendMessage(WM_PG_SELECTORSELCHANGED, m_nSelected, 0);
  return;
 }

 CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CPropertyGridSelector::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 if (nChar == VK_LEFT || nChar == VK_UP)
 {
  m_lbutton_depressed = false;
  Invalidate();
 }
 else if (nChar == VK_RIGHT || nChar == VK_DOWN)
 {
  m_rbutton_depressed = false;
  Invalidate();
 }

 CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

bool CPropertyGridSelector::IsLeftBoundReached() const
{
 if (0==m_Items.size())
  return true;
 return (m_nSelected <= 0);
}

bool CPropertyGridSelector::IsRightBoundReached() const
{
 if (0==m_Items.size())
  return true;
 return (m_nSelected >= static_cast<int>(m_Items.size()-1));
}
