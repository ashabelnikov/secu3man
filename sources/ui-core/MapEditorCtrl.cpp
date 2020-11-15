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
static CBrush redBrush(itemErrColor);
static CPen redPen(PS_SOLID, 3, GDIHelpers::InvColor(RGB(255, 127, 127)));

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


int CMapEditorCtrl::m_gradSaturation = 120;
int CMapEditorCtrl::m_gradBrightness = 255;
bool CMapEditorCtrl::m_boldFont = false;
bool CMapEditorCtrl::m_spotMarkers = true;
float CMapEditorCtrl::m_spotMarkersSize = 1.0f;
 
CMapEditorCtrl::SetSettings(int gradSat, int gradBrt, bool boldFont, bool spotMarkers, float spotMarkersSize)
{
 CMapEditorCtrl::m_gradSaturation = gradSat;
 CMapEditorCtrl::m_gradBrightness = gradBrt;
 CMapEditorCtrl::m_boldFont = boldFont;
 CMapEditorCtrl::m_spotMarkers = spotMarkers;
 CMapEditorCtrl::m_spotMarkersSize = spotMarkersSize;
}

//if you create control via resource editor, you must specify this class name in the control's properties
#define MAPEDITORCTRL_CLASSNAME  _T("MFCMapEditorCtrl")  // Window class name

CMapEditorCtrl::CMapEditorCtrl(int rows, int cols, bool invDataRowsOrder /*= false*/, bool invDataVGridOrder /*= false*/, HMODULE hMod /*= NULL*/, int minLabelWidthInChars /*= 0*/, bool readOnly /*= false*/, bool absGrad /*= false*/)
: m_rows(rows)
, m_cols(cols)
, m_minVal(0.0f)
, m_maxVal(10.0f)
, m_vMinVal(0.0f)
, m_vMaxVal(10.0f)
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
, m_invDataVGridOrder(invDataVGridOrder)
, m_minLabelWidthInChars(minLabelWidthInChars)
, m_readOnly(readOnly)
, mp_itemColors(new DWORD[rows*cols])
, m_absGrad(absGrad)
, mp_bmOldGrid(NULL)
, mp_bmOldMark(NULL)
, m_blackBrush(RGB(0, 0, 0))            //black color brush
, m_forceRedraw(true)
, m_bgrdBrushColor(GetSysColor(COLOR_3DFACE))
, m_bgrdBrush(GetSysColor(COLOR_3DFACE))
, m_ballCoord(0,0)
, m_ballBrush(GDIHelpers::InvColor(RGB(255, 64, 64)))
{
 m_horizLabels.reserve(16);
 m_vertLabels.reserve(16);
 m_markedItems.reserve(256);
 m_gradColor.reserve(256);

 _RegisterWindowClass(hMod);
 m_gradColor = GDIHelpers::GenerateGradientList(0, 511, 256, m_gradSaturation, m_gradBrightness);
 for(int i = 0; i < (rows*cols); ++i)
  mp_itemColors[i] = 0;

 mp_mapTr = new float[m_rows * m_cols];
}

CMapEditorCtrl::~CMapEditorCtrl()
{
 delete[] mp_itemColors;
 delete[] mp_mapTr;
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

 if (NULL!=m_clipRgn.GetSafeHandle())
  m_clipRgn.DeleteObject(); 
}

void CMapEditorCtrl::OnPaint()
{
 CPaintDC dc(this);

 if (m_bgrdBrushColor != GetSysColor(COLOR_3DFACE))
 { //system colors have changed, we need to redraw entire control
  m_bgrdBrushColor = GetSysColor(COLOR_3DFACE);
  m_bgrdBrush.DeleteObject();
  m_bgrdBrush.CreateSolidBrush(m_bgrdBrushColor);
  m_forceRedraw = true;
 }

 if (m_forceRedraw)
 {
  m_forceRedraw = false;

  CRect rc;
  GetClientRect(&rc);

  if (m_dcGrid.GetSafeHdc() == NULL)
   m_dcGrid.CreateCompatibleDC(&dc);

  if (m_dcMark.GetSafeHdc() == NULL)
   m_dcMark.CreateCompatibleDC(&dc);

  if (m_bmGrid.GetSafeHandle() == NULL)
  {
   m_bmGrid.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
   mp_bmOldGrid = m_dcGrid.SelectObject(&m_bmGrid);
  }

  if (m_bmMark.GetSafeHandle() == NULL)
  {
   m_bmMark.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
   mp_bmOldMark = m_dcMark.SelectObject(&m_bmMark);
  }

  //draw grid
  _DrawGrid();

  //Draw horizontal and vertical labels
  _DrawLabels();

  //draw marker(s)
  _DrawMarkers();
 }

 //Draw all data from memory DCs to the real DC
 _ShowImage(&dc);
}

void CMapEditorCtrl::_DrawGrid(int ii /*=-1*/, int jj /*=-1*/)
{
 bool upd_all = (ii == -1 || jj == -1);
 if (NULL==m_dcGrid.GetSafeHdc())
  return;

 if (upd_all)
 { //draw all cells
  CRect rc;
  GetClientRect(&rc);
  m_dcGrid.FillRect(rc, &m_bgrdBrush); //fill client rect with background
  for(int i = 0; i < m_rows; ++i)
  {
   for(int j = 0; j < m_cols; ++j)
   {
    float value = _GetItemTr(i,j);
    CRect rect = _GetItemRect(&m_dcGrid, i, j);
    int index = _GetGradIndex(value);
    COLORREF customColor = _GetItem<COLORREF>(mp_itemColors, i, j);
    m_dcGrid.SetBkColor(IsWindowEnabled() ? (customColor ? customColor : m_gradColor[index]) : GetSysColor(COLOR_3DFACE));
    m_dcGrid.SetBkMode(TRANSPARENT);
    m_dcGrid.FillSolidRect(rect, IsWindowEnabled() ? (customColor ? customColor : m_gradColor[index]) : GetSysColor(COLOR_3DFACE));
    _DrawItem(m_dcGrid, rect, _FloatToStr(value, m_decPlaces));
   }
  }
 }
 else
 { //draw only certain cell
  CRect rect = _GetItemRect(&m_dcGrid, ii, jj);
  m_dcGrid.FillRect(rect, &m_bgrdBrush); //fill cell rect with background
  float value = _GetItemTr(ii,jj);
  int index = _GetGradIndex(value);
  COLORREF customColor = _GetItem<COLORREF>(mp_itemColors, ii, jj);
  m_dcGrid.SetBkColor(IsWindowEnabled() ? (customColor ? customColor : m_gradColor[index]) : GetSysColor(COLOR_3DFACE));
  m_dcGrid.SetBkMode(TRANSPARENT);
  m_dcGrid.FillSolidRect(rect, IsWindowEnabled() ? (customColor ? customColor : m_gradColor[index]) : GetSysColor(COLOR_3DFACE));
  _DrawItem(m_dcGrid, rect, _FloatToStr(value, m_decPlaces));
 }
}

void CMapEditorCtrl::_DrawLabels(void)
{
 if (NULL==m_dcGrid.GetSafeHdc())
  return;
 //draw vertical labels
 if (mp_vertLabels && m_vertShow)
 {
  for(int i = 0; i < m_rows; ++i)
  {
   CRect rect = _GetItemRect(&m_dcGrid, i, 0);
   CFont* oldFont = m_dcGrid.SelectObject(&m_cFont);
   m_dcGrid.TextOut(0, rect.top, m_vertLabels[i]);
   m_dcGrid.SelectObject(oldFont); 
  }
 }
 
 //draw horizontal labels
 if (mp_horizLabels && m_horizShow)
 {
  CRect clrc;
  GetClientRect(&clrc);
  for(int j = 0; j < m_cols; ++j)
  {
   CRect rect = _GetItemRect(&m_dcGrid, 0, j);
   CFont* oldFont = m_dcGrid.SelectObject(&m_cFont);
   m_dcGrid.TextOut(rect.left, clrc.bottom - m_label_height, m_horizLabels[j]);
   m_dcGrid.SelectObject(oldFont); 
  }
 }
}

void CMapEditorCtrl::_DrawMarkers(void)
{
 if (m_dcMark.GetSafeHdc()==NULL)
  return;
 CRect rc;
 GetClientRect(&rc);
 m_dcMark.FillRect(rc, &m_blackBrush);

 if (IsWindowEnabled() && m_showMarkers)
 { 
  if (mp_horizLabels || mp_vertLabels)
  {
   if (m_spotMarkers)
   {//ball
    CRect rect = _GetItemRect(&m_dcMark, 0, 0);
    int r = MathHelpers::Round((((float)rect.Height())/2)*m_spotMarkersSize);
    CObject* oldBrush =  m_dcMark.SelectObject(&m_ballBrush); 
    m_dcMark.Ellipse(m_ballCoord.x - r, m_ballCoord.y - r, m_ballCoord.x + r, m_ballCoord.y + r);
    m_dcMark.SelectObject(oldBrush);
   }
   else
   {//rectangle
    for (size_t i = 0; i < m_markedItems.size(); ++i)
     _DrawMarker(&m_dcMark, m_markedItems[i].x, m_markedItems[i].y);
   }
  }
 }
}

void CMapEditorCtrl::_ShowImage(CDC* pDC, CRect* p_rect /*=NULL*/)
{
 if (NULL==pDC || NULL==pDC->GetSafeHdc())
  return;
 CDC memDC;
 CBitmap memBmp;
 CRect rc;
 GetClientRect(&rc);

 memDC.CreateCompatibleDC(pDC);
 
 if (p_rect)
 { //show specified rect only
  memBmp.CreateCompatibleBitmap(pDC, p_rect->Width(), p_rect->Height());
  CBitmap* oldBmp = (CBitmap*)memDC.SelectObject(&memBmp);
  if (memDC.GetSafeHdc() != NULL)
  {
   memDC.BitBlt(p_rect->left, p_rect->top, rc.Width(), rc.Height(), &m_dcGrid, p_rect->left, p_rect->top, SRCCOPY);
   memDC.BitBlt(p_rect->left, p_rect->top, rc.Width(), rc.Height(), &m_dcMark, p_rect->left, p_rect->top, SRCINVERT);
   if (NULL!=m_clipRgn.GetSafeHandle())
    pDC->SelectClipRgn(&m_clipRgn, RGN_XOR);
   pDC->BitBlt(p_rect->left, p_rect->top, rc.Width(), rc.Height(), &memDC, p_rect->left, p_rect->top, SRCCOPY);
   if (NULL!=m_clipRgn.GetSafeHandle())
    pDC->SelectClipRgn(NULL, RGN_COPY);
  }
  memDC.SelectObject(oldBmp);
 }
 else
 { //show full size image
  memBmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
  CBitmap* oldBmp = (CBitmap*)memDC.SelectObject(&memBmp);
  if (memDC.GetSafeHdc() != NULL)
  {
   memDC.BitBlt(0, 0, rc.Width(), rc.Height(), &m_dcGrid, 0, 0, SRCCOPY);
   memDC.BitBlt(0, 0, rc.Width(), rc.Height(), &m_dcMark, 0, 0, SRCINVERT);
   if (NULL!=m_clipRgn.GetSafeHandle())
    pDC->SelectClipRgn(&m_clipRgn, RGN_XOR);
   pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
   if (NULL!=m_clipRgn.GetSafeHandle())
    pDC->SelectClipRgn(NULL, RGN_COPY);
  }
  memDC.SelectObject(oldBmp);
 }
}

void CMapEditorCtrl::_DrawItem(CDC& dc, const CRect& rect, LPCTSTR text)
{
 dc.Draw3dRect(&rect, RGB(0,0,0), RGB(0,0,0));
 CFont* oldFont = dc.SelectObject(&m_cFont);
 dc.SetTextColor(GetSysColor(IsWindowEnabled() ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
 dc.DrawText(text, (LPRECT)&rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
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
 float minVal = m_absGrad ? m_minVal : m_vMinVal;
 float maxVal = m_absGrad ? m_maxVal : m_vMaxVal;
 float div = ((maxVal - minVal)/((float)m_gradColor.size()));
 int index = MathHelpers::Round((value - (minVal)) / (div == 0 ? 0.000001f : div));
 if (index < 0) index = 0;
 if (index > (int)m_gradColor.size()-1) index = (int)m_gradColor.size()-1;
 return index;
}

void CMapEditorCtrl::AttachMap(float* p_map)
{
 ASSERT(p_map);
 mp_map = p_map;
 _UpdateMinMaxElems();
}

void CMapEditorCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
 for(int i = 0; i < m_rows; ++i)
 {
  for(int j = 0; j < m_cols; ++j)
  {   
   CRect rect = _GetItemRect(&m_dcGrid, i, j);
   if (rect.PtInRect(point))
   {
    if ((mp_edit.get() && mp_edit->GetSafeHwnd()))
    {
     float value = 0;
     if (_ValidateItem(mp_edit.get(), &value))
     {
      if (m_OnChange)
      {
       float previousValue = _GetItemTr(m_cur_i, m_cur_j);       
       _SetItemTr(m_cur_i, m_cur_j, value); //save result
       if (previousValue != value)
       {
        m_OnChange();
        if (!m_absGrad)
        {
         _UpdateMinMaxElems();
         _DrawGrid();    //redraw grid because gradient should be updated
         CClientDC dc(this);
         _ShowImage(&dc);
        }
       }
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

CRect CMapEditorCtrl::_GetItemRect(CDC* pDC, int i , int j)
{
 CRect rc;
 GetClientRect(&rc);
 int w_off = _GetLabelWidth(pDC);
 int h_off = _GetLabelHeight(pDC);
 rc.left+=w_off;
 rc.bottom-=h_off;
 int space  = 2;
 float width  = ((float)(rc.right - rc.left) - ((float)m_cols)*space) / ((float)m_cols);
 float height = ((float)(rc.bottom - rc.top) - ((float)m_rows)*space) / ((float)m_rows);
 return CRect(MathHelpers::Round(j*(width+space))+w_off, MathHelpers::Round((m_rows-1-i)*(height+space)), MathHelpers::Round(j*(width+space) + width)+w_off, MathHelpers::Round((m_rows-1-i)*(height+space) + height));
}

CRect CMapEditorCtrl::_GetMarkerRect(int i, int j, int inflate /*=2*/)
{
 CRect rect = _GetItemRect(&m_dcMark, i, j);
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
  float previousValue = _GetItemTr(m_cur_i, m_cur_j);
  _SetItemTr(m_cur_i, m_cur_j, value); //save result
  if (previousValue != value)
  {
   m_OnChange();
   if (!m_absGrad)
   {
    _UpdateMinMaxElems();
    _DrawGrid();     //redraw grid because gradient should be updated
    CClientDC dc(this);
    _ShowImage(&dc);
   }
  }
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
  float previousValue = _GetItemTr(m_cur_i, m_cur_j);
  _SetItemTr(m_cur_i, m_cur_j, value); //save result
  if (previousValue != value)
  {
   m_OnChange();
   if (!m_absGrad)
   {
    _UpdateMinMaxElems();
    _DrawGrid();   //redraw grid because gradient should be updated
    CClientDC dc(this);
    _ShowImage(&dc);
   }
  }
 }
}

bool CMapEditorCtrl::_ValidateItem(CEditExCustomKeys* pItem, float* p_value /*= NULL*/)
{
 float value = 0;
 bool convres = pItem->GetValue(value); 
 pItem->m_error = !(convres && value >= m_minVal && value <= m_maxVal);
 pItem->Invalidate();
 if (p_value && !pItem->m_error)
  *p_value = value; //save result of there are no errors
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
 if (m_readOnly) return;
 CDC* pDC = GetDC();
 CRect rect = _GetItemRect(pDC, m_cur_i, m_cur_j);
 ReleaseDC(pDC);
 m_clipRgn.CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
//ASSERT(!mp_edit.get());
 mp_edit.reset(new CEditExCustomKeys(fastdelegate::MakeDelegate(this, &CMapEditorCtrl::OnEditChar), fastdelegate::MakeDelegate(this, &CMapEditorCtrl::OnEditKill), m_minVal, m_maxVal, m_increment));
 mp_edit->Create(WS_BORDER | WS_CHILD | WS_VISIBLE | ES_CENTER, rect, this, 0);
 mp_edit->SetDecimalPlaces(m_decPlaces);
 mp_edit->SetValue(_GetItemTr(m_cur_i, m_cur_j));
 mp_edit->SetFocus();
 mp_edit->SetSel(0, -1); //select all text
 mp_edit->SetFont(GetFont(), TRUE);

 if (m_OnSelChange)
  m_OnSelChange();
}

void CMapEditorCtrl::_DeactivateEdit(void)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
  mp_edit->DestroyWindow();
 mp_edit.reset();

 if (NULL!=m_clipRgn.GetSafeHandle())
  m_clipRgn.DeleteObject();
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
    if (m_boldFont)
     tLogFont.lfWeight = 700;
    m_cFont.CreateFontIndirect(&tLogFont);
   }
  }
 }

 if(lParam && ::IsWindow(m_hWnd))
  Redraw(); //redraw all elements

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
 m_label_width = m_label_height = -1;

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
   m_vertLabels.push_back(_FloatToStr(_GetVGrid(i), m_decPlacesV));
 }
}

int CMapEditorCtrl::_GetLabelWidth(CDC* pDC)
{
 if (m_label_width >= 0)
  return m_label_width;

 m_label_width = 0;
 if (mp_vertLabels && m_vertShow)
 {
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

int CMapEditorCtrl::_GetLabelHeight(CDC* pDC)
{
 if (m_label_height >= 0)
  return m_label_height;

 m_label_height = 0;
 if (mp_horizLabels && m_horizShow)
 {
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

void CMapEditorCtrl::_2DLookupH(float x, std::vector<int>& pt)
{
 pt.clear();
 if (x <= mp_horizLabels[0]) { pt.push_back(0); return; }
 if (x >= mp_horizLabels[m_cols-1]) { pt.push_back(m_cols-1); return; }

 float dv = 3.0f;
 for(int i = 1; i < m_cols; ++i)
 {
  float d = mp_horizLabels[i] - mp_horizLabels[i-1];
  if (x <= mp_horizLabels[i])
  {
   if (x < (mp_horizLabels[i-1] + d/dv)) pt.push_back(i-1);
   else if (x > (mp_horizLabels[i] - d/dv)) pt.push_back(i);
   else { pt.push_back(i-1); pt.push_back(i); }
   break;
  }
 }
}

void CMapEditorCtrl::_2DLookupV(float x, std::vector<int>& pt)
{
 pt.clear();
 if (x <= _GetVGrid(0)) { pt.push_back(0); return; }
 if (x >= _GetVGrid(m_rows-1)) { pt.push_back(m_rows-1); return; }

 float dv = 3.0f;
 for(int i = 1; i < m_rows; ++i)
 {
  float d = _GetVGrid(i) - _GetVGrid(i-1);
  if (x <= _GetVGrid(i))
  {
   if (x < (_GetVGrid(i-1) + d/dv)) pt.push_back(i-1);
   else if (x > (_GetVGrid(i) - d/dv)) pt.push_back(i);
   else { pt.push_back(i-1); pt.push_back(i); }
   break;
  }
 }
}

void CMapEditorCtrl::SetArguments(float i_arg, float j_arg)
{
 if (m_spotMarkers)
 { //spot style markers
  CPoint ball;  
  if (mp_vertLabels)
  { //look up through rows
   int i;
   if (i_arg <= _GetVGrid(0))
    i_arg = _GetVGrid(0);  
   if (i_arg >= _GetVGrid(m_rows-1))
    i_arg = _GetVGrid(m_rows-1);
   for(i = m_rows - 2; i >= 0; --i)
    if (i_arg >= _GetVGrid(i)) break; 
   int p1 = i; int p2 = i + 1;

   CClientDC dc(this);
   CPoint cp1 = _GetItemRect(&dc, p1, 0).CenterPoint();
   CPoint cp2 = _GetItemRect(&dc, p2, 0).CenterPoint();

   if (!mp_horizLabels)
    ball.x = cp1.x;
   ball.y = (int)(cp1.y - (((i_arg - _GetVGrid(p1)) / (_GetVGrid(p2) - _GetVGrid(p1))) * (cp1.y - cp2.y)));
  }
  if (mp_horizLabels)
  { //look up through columns
   int j;
   if (j_arg <= mp_horizLabels[0])
    j_arg = mp_horizLabels[0];  
   if (j_arg >= mp_horizLabels[m_cols-1])
    j_arg = mp_horizLabels[m_cols-1];
   for(j = m_cols - 2; j >= 0; --j)
    if (j_arg >= mp_horizLabels[j]) break; 
   int p1 = j; int p2 = j + 1;

   CClientDC dc(this);
   CPoint cp1 = _GetItemRect(&dc, 0, p1).CenterPoint();
   CPoint cp2 = _GetItemRect(&dc, 0, p2).CenterPoint();

   ball.x = (int)(cp1.x + (((j_arg - mp_horizLabels[p1]) / (mp_horizLabels[p2] - mp_horizLabels[p1])) * (cp2.x - cp1.x)));
   if (!mp_vertLabels)
    ball.y = cp1.y;
  }

  if (ball != m_ballCoord)
  { //draw only if position of ball is changed
   m_ballCoord = ball;
   _DrawMarkers();
   CClientDC dc(this);
   _ShowImage(&dc);
  }
 }
 else
 { //rectangle style markers
  std::vector<CPoint> marks;
  std::vector<int> pt_i, pt_j;

  //find where we are for each argument
  if (mp_vertLabels)
   _2DLookupV(i_arg, pt_i);
  if (mp_horizLabels)
   _2DLookupH(j_arg, pt_j);
 
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
  { //update only if changes are present
   m_markedItems = marks;

   CClientDC dc(this);

   //draw marker(s)
   _DrawMarkers();

   //Draw all data onto memory DC
   _ShowImage(&dc);
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

std::pair<int, int> CMapEditorCtrl::GetSelection(void)
{
 if (mp_edit.get())
  return std::make_pair(m_cur_i, m_cur_j);
 else
  return std::make_pair(-1, -1);
}

void CMapEditorCtrl::EnableAbroadMove(bool up, bool down)
{
 m_enAbroadUp = up;
 m_enAbroadDown = down;
}

void CMapEditorCtrl::UpdateDisplay(int i /*=-1*/, int j /*=-1*/)
{
 bool upd_all = (i == -1 || j == -1);
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
 {
  if (upd_all || (m_cur_i == i && m_cur_j == j))
   mp_edit->SetValue(_GetItemTr(m_cur_i, m_cur_j));
 }

 _UpdateMinMaxElems();

 CClientDC dc(this);
 if (upd_all)
 { //update full region
  _DrawGrid();
  _DrawLabels();
  _ShowImage(&dc);
 }
 else
 { //update specified cell only
  _DrawGrid(i, j);
  _ShowImage(&dc, &_GetItemRect(&m_dcGrid, i, j));
 }
}

void CMapEditorCtrl::OnEnable(BOOL bEnable)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
  mp_edit->EnableWindow(bEnable);
 Redraw(); //redraw all elements
}

void CMapEditorCtrl::ShowMarkers(bool show, bool invalidate /*=true*/)
{
 if (m_showMarkers == show)
  return;
 m_showMarkers = show;

 _DrawMarkers(); //draw marker(s)

 if (invalidate)
  Invalidate();
}

void CMapEditorCtrl::SetValueIncrement(float inc)
{
 m_increment = inc;
}

void CMapEditorCtrl::SetGradientList(const std::vector<COLORREF>& colors)
{
 m_gradColor = colors;
}

void CMapEditorCtrl::SetItemColor(int i, int j, COLORREF color)
{
 _SetItem<COLORREF>(mp_itemColors, i, j, color);
}

void CMapEditorCtrl::setOnSelChange(EventHandler OnCB)
{
 m_OnSelChange = OnCB;
}

void CMapEditorCtrl::_UpdateMinMaxElems(void)
{
 if (m_absGrad)
  return;

 float* ptr = mp_map;
 size_t size = (m_rows * m_cols);

 if (m_OnValueTransform)
 {
  ptr = mp_mapTr;
  for(size_t i = 0; i < size; ++i)  
   ptr[i] = m_OnValueTransform(mp_map[i], 0);
 }

 m_vMinVal = *std::min_element(ptr, ptr + size);
 m_vMaxVal = *std::max_element(ptr, ptr + size);
}

void CMapEditorCtrl::setOnValueTransform(EventHandler3 OnCB)
{
 m_OnValueTransform = OnCB;
}

void CMapEditorCtrl::_SetItemTr(int i, int j, float value)
{
 if (m_OnValueTransform)
  _SetItem<float>(mp_map, i, j, m_OnValueTransform(value, 1)); //from chart
 else
  _SetItem<float>(mp_map, i, j, value);
}

float CMapEditorCtrl::_GetItemTr(int i, int j)
{
 if (m_OnValueTransform)
  return m_OnValueTransform(_GetItem<float>(mp_map, i,j), 0); //to chart
 else
  return _GetItem<float>(mp_map, i,j);
}

void CMapEditorCtrl::Redraw(void)
{
 m_forceRedraw = true;
 Invalidate();
}
