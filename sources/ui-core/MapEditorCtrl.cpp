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

/** \file MapEditorCtrl.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "MapEditorCtrl.h"
#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
#include "common/fastdelegate.h"
#include "ui-core/EditEx.h"

static TCHAR templateStr[] = _T("88888888888888");

static const COLORREF itemErrColor = RGB(255,120,120);
CBrush redBrush(itemErrColor);
CPen redPen(PS_SOLID, 3, RGB(255, 64, 64));

//We need this descendant to have ability to handle some keyboard keys
class CEditExCustomKeys : public CEditEx
{
  typedef fastdelegate::FastDelegate2<UINT, CEditExCustomKeys*> EventHandler2;
  typedef fastdelegate::FastDelegate1<CEditExCustomKeys*> EventHandler1;

  void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
   CEditEx::OnChar(nChar, nRepCnt, nFlags);
   if (nChar==VK_RETURN)
    m_onChar(nChar, this);
   
   else if (nChar==VK_TAB) //DLGC_WANTALLKEYS brake TABSTOP and we make it to work again
   {
    CWnd* pWnd = GetParent()->GetParent()->GetNextDlgTabItem(GetParent(), (GetKeyState(VK_SHIFT)&0x8000)?TRUE:FALSE);
    //deactivate edit box in previous control
    CMapEditorCtrl* pParent = dynamic_cast<CMapEditorCtrl*>(GetParent());
    if (pParent && pParent != pWnd)
     pParent->_DeactivateEdit();
    //activate next control
    if (pWnd)
     pWnd->SetFocus();
   }
  }

  void IncrementEdit(float incVal)
  {   
   float value = 0;
   if (GetValue(value))
   {   
    value+=incVal;
    if (value < m_min)
     value = m_min;
    else if  (value > m_max)
     value = m_max;
    SetValue(value);
   }
   else
    SetValue(0.0f);
  }

  void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
   if (nChar==VK_LEFT || nChar==VK_RIGHT || nChar==VK_UP || nChar==VK_DOWN)
   {
    int nStartChar, nEndChar;
    GetSel(nStartChar, nEndChar);
    if (nChar==VK_LEFT)
    {
     if (nStartChar <= 0)
     {
      m_onChar(nChar, this);
      return;
     }
    }
    else if (nChar==VK_RIGHT)
    {
     CString str;
     GetWindowText(str);
     if (nStartChar >= str.GetLength())
     {
      m_onChar(nChar, this);
      return;
     }
    }
    else
    {
     m_onChar(nChar, this);
     return;
    }
   }
   else if (nChar==VK_OEM_6)
    IncrementEdit(m_increment);
   else if (nChar==VK_OEM_5)
    IncrementEdit(-m_increment);

   CEditEx::OnKeyDown(nChar, nRepCnt, nFlags);
  }

  void OnKillFocus(CWnd* cwnd)
  {
   CEditEx::OnKillFocus(cwnd);
   m_onKillFocus(this);
  }

  UINT OnGetDlgCode()
  {
   return CEditEx::OnGetDlgCode() | DLGC_WANTALLKEYS;
  }

  HBRUSH CtlColor(CDC* pDC, UINT nCtlColor)
  {
   HBRUSH hbr = NULL;
   if (nCtlColor == CTLCOLOR_EDIT)
   {
    if (m_error)
    { 
     pDC->SetBkColor(itemErrColor);
     hbr = redBrush;
    }
   }
   return hbr;
  }

  EventHandler2 m_onChar;
  EventHandler1 m_onKillFocus;
  DECLARE_MESSAGE_MAP()

 public:
  CEditExCustomKeys(const EventHandler2& onChar, const EventHandler1& onKillFocus, float i_min, float i_max, float inc)
  : CEditEx(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
  , m_onChar(onChar)
  , m_onKillFocus(onKillFocus)
  , m_min(i_min)
  , m_max(i_max)
  , m_increment(inc)
  , m_error(false){}

  bool m_error;
  int m_mapType;
  float m_min;
  float m_max;
  float m_increment;
};

BEGIN_MESSAGE_MAP(CEditExCustomKeys, CEditEx)
 ON_WM_CHAR()
 ON_WM_KEYDOWN()
 ON_WM_GETDLGCODE()
 ON_WM_KILLFOCUS()
 ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



//if you create control via resource editor, you must specify this class name in the control's properties
#define MAPEDITORCTRL_CLASSNAME  _T("MFCMapEditorCtrl")  // Window class name

CMapEditorCtrl::CMapEditorCtrl(int rows, int cols, bool invDataRowsOrder /*= false*/, HMODULE hMod /*= NULL*/, int minLabelWidthInChars /*= 0*/)
: m_rows(rows)
, m_cols(cols)
, m_minVal(0.0f)
, m_maxVal(10.0f)
, mp_map(NULL)
, mp_horizLabels(NULL)
, mp_vertLabels(NULL)
, m_decPlaces(2)
, m_decPlacesH(0)
, m_decPlacesV(0)
, m_cur_i(0)
, m_cur_j(0)
, m_label_width(-1) //not initialized
, m_label_height(-1) //not initialized
, m_horizShow(false)
, m_vertShow(false)
, m_enAbroadUp(false)
, m_enAbroadDown(false)
, m_showMarkers(false)
, m_increment(0.5f)
, m_invDataRowsOrder(invDataRowsOrder)
, m_minLabelWidthInChars(minLabelWidthInChars)
{
 _RegisterWindowClass(hMod);
 m_gradColor = GDIHelpers::GenerateGradientList(0, 511, 256, 110, 230);
}

CMapEditorCtrl::~CMapEditorCtrl()
{
 //empty
}

BEGIN_MESSAGE_MAP(CMapEditorCtrl, Super)
 ON_WM_PAINT()
 ON_WM_DESTROY()
 ON_WM_LBUTTONDOWN()
 ON_WM_SETFOCUS()
 ON_WM_KILLFOCUS()
 ON_WM_ENABLE()
 ON_MESSAGE(WM_SETFONT, OnWMSetFont)
 ON_MESSAGE(WM_GETFONT, OnWMGetFont)
END_MESSAGE_MAP()

BOOL CMapEditorCtrl::Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id)
{
 BOOL result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, NULL, NULL, dwStyle,
               rect.left, rect.top, rect.Width(), rect.Height(), pParent->GetSafeHwnd(), (HMENU)((UINT_PTR)id));
 if (!result)
  AfxMessageBox(_T("Error creating MapEditorCtrl control's window!"));

 return result;
}

bool CMapEditorCtrl::_RegisterWindowClass(HMODULE hMod /*= NULL*/)
{
 WNDCLASS wndcls;
 HINSTANCE hInst = hMod ? hMod : AfxGetResourceHandle();

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

void CMapEditorCtrl::OnDestroy()
{
 Super::OnDestroy();
 if (mp_edit.get())
  mp_edit.reset();
}

void CMapEditorCtrl::OnPaint()
{
 CPaintDC dc(this); // device context for painting

 for(int i = 0; i < m_rows; ++i)
 {
  for(int j = 0; j < m_cols; ++j)
  {
   float value = _GetItem(i,j);
   CRect rect = _GetItemRect(i, j);
   int index = _GetGradIndex(value);
   dc.SetBkColor(IsWindowEnabled() ? m_gradColor[index] : GetSysColor(COLOR_3DFACE));
   dc.SetBkMode(TRANSPARENT);
   dc.FillSolidRect(rect, IsWindowEnabled() ? m_gradColor[index] : GetSysColor(COLOR_3DFACE));
   _DrawItem(dc, rect, _FloatToStr(value, m_decPlaces));
  }
 }


 if (mp_vertLabels && m_vertShow)
 {
  for(int i = 0; i < m_rows; ++i)
  {
   CRect rect = _GetItemRect(i, 0);
   CFont* oldFont = dc.SelectObject(&m_cFont);
   dc.TextOut(0, rect.top, m_vertLabels[i]);
   dc.SelectObject(oldFont); 
  }
 }

 if (mp_horizLabels && m_horizShow)
 {
  CRect clrc;
  GetClientRect(&clrc);
  for(int j = 0; j < m_cols; ++j)
  {
   CRect rect = _GetItemRect(0, j);
   CFont* oldFont = dc.SelectObject(&m_cFont);
   dc.TextOut(rect.left, clrc.bottom - m_label_height, m_horizLabels[j]);
   dc.SelectObject(oldFont); 
  }
 }
 
 //draw marker(s)
 if (IsWindowEnabled() && m_showMarkers)
 {
  if (mp_horizLabels || mp_vertLabels)
  {
   for (size_t i = 0; i < m_markedItems.size(); ++i)
    _DrawMarker(&dc, m_markedItems[i].x, m_markedItems[i].y);
  }
 }
}

void CMapEditorCtrl::_DrawItem(CDC& dc, const CRect& rect, LPCTSTR text)
{
 dc.Draw3dRect(&rect, RGB(0,0,0), RGB(0,0,0));
 CFont* oldFont = dc.SelectObject(&m_cFont);
 dc.SetTextColor(GetSysColor(IsWindowEnabled() ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
 dc.TextOut(rect.left, rect.top, text);
 dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
 dc.SelectObject(oldFont);
}

void CMapEditorCtrl::SetRange(float i_min, float i_max)
{
 m_minVal = i_min;
 m_maxVal = i_max;
}

int CMapEditorCtrl::_GetGradIndex(float value)
{
 int index = MathHelpers::Round((value - (m_minVal)) / ((m_maxVal - m_minVal)/((float)m_gradColor.size())));
 if (index < 0) index = 0;
 if (index > (int)m_gradColor.size()-1) index = (int)m_gradColor.size()-1;
 return index;
}

void CMapEditorCtrl::AttachMap(float* p_map)
{
 ASSERT(p_map);
 mp_map = p_map;
}

float CMapEditorCtrl::_GetItem(int i, int j)
{
 ASSERT(mp_map);
 int ii = m_invDataRowsOrder ? (m_rows - 1) - i : i;
 return mp_map[(ii*m_cols)+j];
}

void CMapEditorCtrl::_SetItem(int i, int j, float value)
{
 ASSERT(mp_map);
 int ii = m_invDataRowsOrder ? (m_rows - 1) - i : i;
 mp_map[(ii*m_cols)+j] = value;
}

void CMapEditorCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
 for(int i = 0; i < m_rows; ++i)
 {
  for(int j = 0; j < m_cols; ++j)
  {   
   CRect rect = _GetItemRect(i, j);
   if (rect.PtInRect(point))
   {
    if ((mp_edit.get() && mp_edit->GetSafeHwnd()))
    {
     float value = 0;
     if (_ValidateItem(mp_edit.get(), &value))
     {
      if (m_OnChange)
      {
       float previousValue = _GetItem(m_cur_i, m_cur_j);
       _SetItem(m_cur_i, m_cur_j, value); //save result
       if (previousValue != value)
        m_OnChange();
      }
      _DeactivateEdit();
      m_cur_i = i; m_cur_j = j;
      _ActivateEdit();       
     }
    }
    else
    { //no previous controls created, just create new control
     m_cur_i = i; m_cur_j = j;
     _ActivateEdit();    
    }
   }
  }
 }

 return Super::OnLButtonDown(nFlags, point);
}

CRect CMapEditorCtrl::_GetItemRect(int i , int j)
{
 CRect rc;
 GetClientRect(&rc);
 int w_off = _GetLabelWidth();
 int h_off = _GetLabelHeight();
 rc.left+=w_off;
 rc.bottom-=h_off;
 int space  = 2;
 float width  = ((float)(rc.right - rc.left) - ((float)m_cols)*space) / ((float)m_cols);
 float height = ((float)(rc.bottom - rc.top) - ((float)m_rows)*space) / ((float)m_rows);
 return CRect(MathHelpers::Round(j*(width+space))+w_off, MathHelpers::Round((m_rows-1-i)*(height+space)), MathHelpers::Round(j*(width+space) + width)+w_off, MathHelpers::Round((m_rows-1-i)*(height+space) + height));
}

CRect CMapEditorCtrl::_GetMarkerRect(int i, int j, int inflate /*=2*/)
{
 CRect rect = _GetItemRect(i, j);
 rect.InflateRect(inflate,inflate,inflate,inflate);
 return rect;
}


CString CMapEditorCtrl::_FloatToStr(float value, int decPlaces)
{
 TCHAR buff[16];
  _stprintf(buff, "%.*f", decPlaces, value);
  return buff;
}

void CMapEditorCtrl::OnEditChar(UINT nChar, CEditExCustomKeys* pSender)
{
 if (NULL==pSender)
  return;
 float value = 0;
 if (!_ValidateItem(pSender, &value))
  return;

 if (m_OnChange)
 {
  float previousValue = _GetItem(m_cur_i, m_cur_j);
  _SetItem(m_cur_i, m_cur_j, value); //save result
  if (previousValue != value)
   m_OnChange();
 }

 switch(nChar)
 {
  case VK_UP:
   if (m_cur_i < m_rows-1)
   {
    _DeactivateEdit();
    m_cur_i++;
    _ActivateEdit();
   }
   else
   {
    if (m_enAbroadUp)
    {
     _DeactivateEdit();
     if (m_OnAbroadMove)
      m_OnAbroadMove(ABROAD_UP, m_cur_j);
    }
   }
   break;
  case VK_RETURN:                 //move down if user pressed Enter key
  case VK_DOWN:
   if (m_cur_i > 0)
   {
    _DeactivateEdit();
    m_cur_i--;
    _ActivateEdit();
   }
   else
   {
    if (m_enAbroadDown)
    {
     _DeactivateEdit();
     if (m_OnAbroadMove)
      m_OnAbroadMove(ABROAD_DOWN, m_cur_j);
    }
   }
   break;
  case VK_LEFT:
   if (m_cur_j > 0)
   {
    _DeactivateEdit();
    m_cur_j--;
    _ActivateEdit();
   }
   break;
  case VK_RIGHT:
   if (m_cur_j < m_cols-1)
   {
    _DeactivateEdit();
    m_cur_j++;
    _ActivateEdit();
   }
   break;
 } 
}

void CMapEditorCtrl::OnEditKill(CEditExCustomKeys* pSender)
{
 float value = 0;
 if (!_ValidateItem(pSender, &value))
 {
  pSender->SetFocus();
  return; //validation failed
 }

 if (m_OnChange)
 {
  float previousValue = _GetItem(m_cur_i, m_cur_j);
  _SetItem(m_cur_i, m_cur_j, value); //save result
  if (previousValue != value)
   m_OnChange();
 }
}

bool CMapEditorCtrl::_ValidateItem(CEditExCustomKeys* pItem, float* p_value /*= NULL*/)
{
 float value = 0;
 bool convres = pItem->GetValue(value); 
 pItem->m_error = !(convres && value >= m_minVal && value <= m_maxVal);
 pItem->Invalidate();
 if (p_value && !pItem->m_error)
  *p_value = value; //save result of thre are no errors
 return !pItem->m_error;
}

void CMapEditorCtrl::OnSetFocus(CWnd* pLast)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
  mp_edit->SetFocus(); //already exists, just set focus
 else
  _ActivateEdit(); //create and initialize edit box

 Super::OnSetFocus(pLast);
}

void CMapEditorCtrl::OnKillFocus(CWnd* pNewWnd)
{
 Super::OnKillFocus(pNewWnd);
}

void CMapEditorCtrl::_ActivateEdit(void)
{ 
//ASSERT(!mp_edit.get());
 mp_edit.reset(new CEditExCustomKeys(fastdelegate::MakeDelegate(this, &CMapEditorCtrl::OnEditChar), fastdelegate::MakeDelegate(this, &CMapEditorCtrl::OnEditKill), m_minVal, m_maxVal, m_increment));
 CRect rect = _GetItemRect(m_cur_i, m_cur_j);
 mp_edit->Create(WS_BORDER | WS_CHILD | WS_VISIBLE, rect, this, 0);
 mp_edit->SetDecimalPlaces(m_decPlaces);
 mp_edit->SetValue(_GetItem(m_cur_i, m_cur_j));
 mp_edit->SetFocus();
 mp_edit->SetSel(0, -1); //select all text
 mp_edit->SetFont(GetFont(), TRUE);
}

void CMapEditorCtrl::_DeactivateEdit(void)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
  mp_edit->DestroyWindow();
 mp_edit.reset();
}

LRESULT CMapEditorCtrl::OnWMSetFont(WPARAM wParam, LPARAM lParam)
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

LRESULT CMapEditorCtrl::OnWMGetFont(WPARAM wParam, LPARAM lParam)
{
 return (LRESULT)((HFONT)m_cFont);
}

void CMapEditorCtrl::setOnAbroadMove(EventHandler2 OnCB)
{
 m_OnAbroadMove = OnCB;
}

void CMapEditorCtrl::setOnChange(EventHandler OnCB)
{
 m_OnChange = OnCB;
}

void CMapEditorCtrl::AttachLabels(const float* horizLabels, const float* vertLabels)
{
 mp_horizLabels = const_cast<float*>(horizLabels);
 mp_vertLabels = const_cast<float*>(vertLabels);
 //reset precalculated values
 m_label_width = -1;
 m_label_height = -1;

 m_horizLabels.clear();
 if (mp_horizLabels)
 {
  for (int i = 0; i < m_cols; ++i)
   m_horizLabels.push_back(_FloatToStr(mp_horizLabels[i], m_decPlacesH));
 }

 m_vertLabels.clear();
 if (mp_vertLabels)
 {
  for (int i = 0; i < m_rows; ++i)
   m_vertLabels.push_back(_FloatToStr(mp_vertLabels[i], m_decPlacesV));
 }
}

int CMapEditorCtrl::_GetLabelWidth(void)
{
 if (m_label_width >= 0)
 {
  return m_label_width;
 }

 m_label_width = 0;
 if (mp_vertLabels && m_vertShow)
 {
  CDC* pDC = GetDC();  
  for (size_t i = 0; i < m_vertLabels.size(); ++i)
  {
   CSize sz = pDC->GetTextExtent(m_vertLabels[i]);
   if (sz.cx > m_label_width)
    m_label_width = sz.cx;
  }
 
  //apply mimimum label's width
  CSize sz = pDC->GetTextExtent(templateStr, m_minLabelWidthInChars);
  if (sz.cx > m_label_width)
   m_label_width = sz.cx;
 }
   
 return m_label_width;
}

int CMapEditorCtrl::_GetLabelHeight(void)
{
 if (m_label_height >= 0)
 {
  return m_label_height;
 }

 m_label_height = 0;
 if (mp_horizLabels && m_horizShow)
 {
  CDC* pDC = GetDC();
  CSize sz = pDC->GetTextExtent(m_horizLabels[0]);
  m_label_height = sz.cy;
 }

 return m_label_height;
}

void CMapEditorCtrl::SetDecimalPlaces(int value, int horiz, int vert)
{
 m_decPlaces = value;
 m_decPlacesH = horiz;
 m_decPlacesV = vert;
}

void CMapEditorCtrl::_DrawMarker(CDC* pDC, int i, int j)
{
 CRect rect = _GetMarkerRect(i, j, 2);
 CBrush * oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
 CPen* oldPen = pDC->SelectObject(&redPen);
 pDC->Rectangle(rect);
 pDC->SelectObject(oldBrush);
 pDC->SelectObject(oldPen);
}

static void _2DLookup(float x, const float* grid, int ptNum, std::vector<int>& pt)
{
 pt.clear();
 if (x <= grid[0]) { pt.push_back(0); return; }
 if (x >= grid[ptNum-1]) { pt.push_back(ptNum-1); return; }

 for(int i = 0; i < ptNum; ++i)
 {
  float d = grid[i] - grid[i-1];
  if (x <= grid[i])
  {
   if (x < (grid[i-1] + d/5)) pt.push_back(i-1);
   else if (x > (grid[i] - d/5)) pt.push_back(i);
   else { pt.push_back(i-1); pt.push_back(i); }
   break;
  }
 }
}

void CMapEditorCtrl::SetArguments(float i_arg, float j_arg)
{
 std::vector<CPoint> marks;
 std::vector<int> pt_i, pt_j;

 //find where we are for each argument
 if (mp_vertLabels)
  _2DLookup(i_arg, mp_vertLabels, m_rows, pt_i);
 if (mp_horizLabels)
  _2DLookup(j_arg, mp_horizLabels, m_cols, pt_j);

 //build list of cells to be marked
 if (mp_vertLabels && !mp_horizLabels)
 {
  for(size_t i = 0; i < pt_i.size(); ++i)
   marks.push_back(CPoint(pt_i[i], 0));
 }
 else if (!mp_vertLabels && mp_horizLabels)
 {
  for(size_t j = 0; j < pt_j.size(); ++j)
   marks.push_back(CPoint(0, pt_j[j])); 
 }
 else
 {
  for(size_t i = 0; i < pt_i.size(); ++i)
  {
   for(size_t j = 0; j < pt_j.size(); ++j)
    marks.push_back(CPoint(pt_i[i], pt_j[j]));
  } 
 }

 if (m_markedItems.size() != marks.size() || !std::equal(m_markedItems.begin(), m_markedItems.end(), marks.begin()))
 {
  //erase old mark(s)
  for (size_t i = 0; i < m_markedItems.size(); ++i)
  {
   CRect rect = _GetMarkerRect(m_markedItems[i].x, m_markedItems[i].y, 3);
   InvalidateRect(&rect);
  }

  m_markedItems = marks;

  //invalidate to draw new mark(s)
  for (size_t i = 0; i < m_markedItems.size(); ++i)
  {
   CRect rect = _GetMarkerRect(m_markedItems[i].x, m_markedItems[i].y, 3);
   InvalidateRect(&rect);
  }
 }
}

void CMapEditorCtrl::ShowLabels(bool horizShow, bool vertShow)
{
 m_horizShow = horizShow;
 m_vertShow = vertShow;
}

void CMapEditorCtrl::SetSelection(int i, int j)
{
 _DeactivateEdit();
 
 if (i >= m_rows-1)
  i = m_rows-1;
 m_cur_i = i;

 if (j >= m_cols-1)
  j = m_cols-1;
 m_cur_j = j;

 _ActivateEdit();
}

void CMapEditorCtrl::EnableAbroadMove(bool up, bool down)
{
 m_enAbroadUp = up;
 m_enAbroadDown = down;
}

void CMapEditorCtrl::UpdateDisplay(void)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
  mp_edit->SetValue(_GetItem(m_cur_i, m_cur_j));

 Invalidate();
}

void CMapEditorCtrl::OnEnable(BOOL bEnable)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
  mp_edit->EnableWindow(bEnable);
 Invalidate();
}

void CMapEditorCtrl::ShowMarkers(bool show, bool invalidate /*=true*/)
{
 if (m_showMarkers == show)
  return;

 m_showMarkers = show;
 //invalidate
 for (size_t i = 0; i < m_markedItems.size(); ++i)
 {
  CRect rect = _GetMarkerRect(m_markedItems[i].x, m_markedItems[i].y, 3);
  InvalidateRect(&rect);
 }
}

void CMapEditorCtrl::SetValueIncrement(float inc)
{
 m_increment = inc;
}

void CMapEditorCtrl::SetGradientList(const std::vector<COLORREF>& colors)
{
 m_gradColor = colors;
}

