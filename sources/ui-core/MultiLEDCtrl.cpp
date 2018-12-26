/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file MultiLEDCtrl.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "MultiLEDCtrl.h"
#include "common/MathHelpers.h"

//if you create control via resource editor, you must specify this class name in the control's properties
#define MAPEDITORCTRL_CLASSNAME  _T("MultiLEDCtrl")  // Window class name


CMultiLEDCtrl::CMultiLEDCtrl(HMODULE hMod /*= NULL*/)
: m_rows(0)
, m_cols(0)
, m_bkBrushColor(GetSysColor(COLOR_3DFACE))
{
 _RegisterWindowClass(hMod);
 m_bkBrush.CreateSolidBrush(m_bkBrushColor);
}

CMultiLEDCtrl::~CMultiLEDCtrl()
{
 for(size_t i = 0; i < m_items.size(); ++i)
  delete m_items[i].mp_brush;
}

BEGIN_MESSAGE_MAP(CMultiLEDCtrl, Super)
 ON_WM_PAINT()
 ON_WM_DESTROY()
 ON_WM_ENABLE()
 ON_MESSAGE(WM_SETFONT, OnWMSetFont)
 ON_MESSAGE(WM_GETFONT, OnWMGetFont)
 ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CMultiLEDCtrl::Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id)
{
 BOOL result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, NULL, NULL, dwStyle,
               rect.left, rect.top, rect.Width(), rect.Height(), pParent->GetSafeHwnd(), (HMENU)((UINT_PTR)id));
 if (!result)
  AfxMessageBox(_T("Error creating MultiLEDCtrl control's window!"));

 return result;
}

bool CMultiLEDCtrl::_RegisterWindowClass(HMODULE hMod /*= NULL*/)
{
 WNDCLASS wndcls;
 HINSTANCE hInst = hMod ? hMod : AfxGetInstanceHandle();

 if (!(::GetClassInfo(hInst, MAPEDITORCTRL_CLASSNAME, &wndcls)))
 {
  wndcls.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
  wndcls.lpfnWndProc   = ::DefWindowProc;
  wndcls.cbClsExtra    = wndcls.cbWndExtra = 0;
  wndcls.hInstance     = hInst;
  wndcls.hIcon         = NULL;
  wndcls.hCursor       = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
  wndcls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
  wndcls.lpszMenuName  = NULL;
  wndcls.lpszClassName = MAPEDITORCTRL_CLASSNAME;

  if (!AfxRegisterClass(&wndcls))
  {
   AfxThrowResourceException();
   return false;
  }
 }

 return true;
}

void CMultiLEDCtrl::OnDestroy()
{
 Super::OnDestroy();
}

void CMultiLEDCtrl::OnPaint()
{
 CPaintDC dc(this); // device context for painting

 COLORREF newcolor = GetSysColor(COLOR_3DFACE);
 if (m_bkBrushColor != newcolor)
 {
  m_bkBrushColor = newcolor;
  m_bkBrush.DeleteObject();
  m_bkBrush.CreateSolidBrush(m_bkBrushColor);  
 }

 COLORREF text_color = GetSysColor(COLOR_BTNTEXT);
 COLORREF gray_color = GetSysColor(COLOR_GRAYTEXT);
 
 for (size_t i = 0; i < m_items.size(); ++i)
 {
  CRect rc = GetItemRect((int)i);
  if (m_items[i].m_state) //on
   dc.FillRect(rc, IsWindowEnabled() ? m_items[i].mp_brush : &m_bkBrush);
  else //off
   dc.FillRect(rc, IsWindowEnabled() ? &m_bkBrush : &m_bkBrush);
  dc.Draw3dRect(&rc, RGB(255,255,255), RGB(0,0,0));
  dc.SetBkMode(TRANSPARENT);
  dc.SetTextColor(IsWindowEnabled() ? text_color : gray_color);
  CFont* oldFont = dc.SelectObject(&m_cFont);
  dc.DrawText(m_items[i].m_text, &rc, DT_CENTER | DT_END_ELLIPSIS);
  dc.SelectObject(oldFont);
 } 
}

CRect CMultiLEDCtrl::GetItemRect(int idx)
{
 int i = idx / m_cols;
 int j = idx % m_cols;
 CRect rc;
 GetClientRect(&rc);
 int space  = 2;
 float width  = ((float)(rc.right - rc.left) - ((float)m_cols)*space) / ((float)m_cols);
 float height = ((float)(rc.bottom - rc.top) - ((float)m_rows)*space) / ((float)m_rows);
 return CRect(MathHelpers::Round(j*(width+space)), MathHelpers::Round(i*(height+space)), MathHelpers::Round(j*(width+space) + width), MathHelpers::Round(i*(height+space) + height));
}

LRESULT CMultiLEDCtrl::OnWMSetFont(WPARAM wParam, LPARAM lParam)
{
 if(wParam != NULL)
 {
  CFont *pFont = CFont::FromHandle((HFONT)wParam);
  if(pFont != NULL)
  {
   LOGFONT tLogFont;
   memset(&tLogFont, 0, sizeof(LOGFONT));
   if (pFont->GetLogFont(&tLogFont))
   {
    if((HFONT)m_cFont != NULL)
    {
     m_cFont.DeleteObject();
     ASSERT((HFONT)m_cFont == NULL);
    }	
    m_cFont.CreateFontIndirect(&tLogFont);
   }
  }
 }

 if(lParam && ::IsWindow(m_hWnd))
  Invalidate();

 return 0;
}

LRESULT CMultiLEDCtrl::OnWMGetFont(WPARAM wParam, LPARAM lParam)
{
 return (LRESULT)((HFONT)m_cFont);
}

void CMultiLEDCtrl::OnEnable(BOOL bEnable)
{
 Invalidate();
}

void CMultiLEDCtrl::SetNumRows(int rows)
{
 m_rows = rows;

 //calculate number of columns from number of items and rows
 m_cols = ((int)m_items.size() / m_rows) + ((int)m_items.size() % m_rows ? 1 : 0);
}

void CMultiLEDCtrl::AddItem(const CString& text, COLORREF color /*=RGB(0,255,0)*/)
{
 LEDItem li;
 li.m_text = text;
 li.m_state = false;
 li.m_color = color;
 li.mp_brush = new CBrush(color);

 m_items.push_back(li);
 //calculate number of columns from number of items and rows
 m_cols = ((int)m_items.size() / m_rows) + ((int)m_items.size() % m_rows ? 1 : 0);
}

void CMultiLEDCtrl::SetItemState(int idx, bool state, bool invalidate /*= true*/)
{
 ASSERT(idx < (int)m_items.size());
 if (idx>= (int)m_items.size())
  return; //error: wrong item index

 if (m_items[idx].m_state == state)
  return; //don't redraw if item already has needed state

 m_items[idx].m_state = state;
 if (invalidate)
  InvalidateRect(GetItemRect(idx));  
}

void CMultiLEDCtrl::Clear(void)
{
 for(size_t i = 0; i < m_items.size(); ++i)
  delete m_items[i].mp_brush;
 m_items.clear();
}

#if _MSC_VER >= 1400
LRESULT CMultiLEDCtrl::OnNcHitTest(CPoint point)
#else
UINT CMultiLEDCtrl::OnNcHitTest(CPoint point)
#endif
{
 return HTTRANSPARENT;
}

void CMultiLEDCtrl::SetItemColor(int idx, COLORREF color, bool invalidate /*= true*/)
{
 m_items[idx].m_color = color;
 delete m_items[idx].mp_brush;
 m_items[idx].mp_brush = new CBrush(color);
 if (invalidate)
  InvalidateRect(GetItemRect(idx));  
}
