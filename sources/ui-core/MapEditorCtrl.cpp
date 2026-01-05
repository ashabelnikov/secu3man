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
#include "resource.h"
#include "MapEditorCtrl.h"
#include "common/GDIHelpers.h"
#include "common/MathHelpers.h"
#include "common/fastdelegate.h"
#include "common/LangLayer.h"
#include "common/StrUtils.h"
#include "EditEx.h"
#include "FileDialogEx.h"
#include "DynFieldsDialog.h"
#include "MsgBox.h"
#include "InitMenuPopup.h"
#include "Undo.h"

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
   if (nChar == VK_RETURN)
    m_onChar(nChar, this);
  }

  void IncrementEdit(float incVal)
  {
   float value = 0;
   if (GetValue(value))
   {   
    value+=incVal;
    value = MathHelpers::RestrictValue(value, m_min, m_max);
    SetValue(value);
   }
   else
    SetValue(0.0f);
  }

  void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
   if (nChar==VK_OEM_6 || nChar==0x57)         // "]" or "W"
    IncrementEdit(m_increment);
   else if (nChar==VK_OEM_5 || nChar==0x51)    // "\" or "Q"
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
   const MSG *pMsg = GetCurrentMessage();
   pMsg = ((const MSG *)pMsg->lParam);

   if (pMsg != NULL)
   {
    if (pMsg->message == WM_KEYDOWN)
    {
     if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_TAB) //skip Esc and TAB keys, so parent dialog will be able to close by Esc and change focus of controls using TAB key
      return 0;
    }
   }

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
CMapEditorCtrl* CMapEditorCtrl::mp_other = NULL;
char CMapEditorCtrl::m_csvsep_symb = ';';
bool CMapEditorCtrl::m_useMarkers = true;
 
void CMapEditorCtrl::SetSettings(int gradSat, int gradBrt, bool boldFont, bool spotMarkers, float spotMarkersSize, bool useMarkers)
{
 CMapEditorCtrl::m_gradSaturation = gradSat;
 CMapEditorCtrl::m_gradBrightness = gradBrt;
 CMapEditorCtrl::m_boldFont = boldFont;
 CMapEditorCtrl::m_spotMarkers = spotMarkers;
 CMapEditorCtrl::m_spotMarkersSize = spotMarkersSize;
 CMapEditorCtrl::m_useMarkers = useMarkers;
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
, mp_mapOrig(NULL)
, mp_horizLabels(NULL)
, mp_vertLabels(NULL)
, m_decPlaces(2)
, m_decPlacesH(0)
, m_decPlacesV(0)
, m_cur_i(0)
, m_cur_j(0)
, m_end_i(0)
, m_end_j(0)
, m_tp_i(0)
, m_tp_j(0)
, m_sel_changed(true) //need to update list of selection
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
, m_lbuttondown(false)
, mp_tooltip(NULL)
, mp_undo(new UndoCntr())
, m_rof_setto(false)
, m_rof_import(false)
{
 m_horizLabels.reserve(16);
 m_vertLabels.reserve(16);
 m_markedItems.reserve(256);
 m_gradColor.reserve(256);

 _RegisterWindowClass(hMod);
 m_gradColor = GDIHelpers::GenerateGradientList(0, 511, 256, m_gradSaturation, m_gradBrightness);
 std::fill(mp_itemColors, mp_itemColors + (rows*cols), 0);
 mp_mapTr = new float[m_rows * m_cols];
}

CMapEditorCtrl::~CMapEditorCtrl()
{
 delete[] mp_itemColors;
 delete[] mp_mapTr;
 delete mp_tooltip;
 delete mp_undo;
}

BEGIN_MESSAGE_MAP(CMapEditorCtrl, Super)
 ON_WM_ERASEBKGND()
 ON_WM_PAINT()
 ON_WM_DESTROY()
 ON_WM_LBUTTONDOWN()
 ON_WM_LBUTTONUP()
 ON_WM_LBUTTONDBLCLK()
 ON_WM_MOUSEMOVE()
 ON_WM_MOUSEWHEEL()
 ON_WM_SETFOCUS()
 ON_WM_KILLFOCUS()
 ON_WM_ENABLE()
 ON_MESSAGE(WM_SETFONT, OnWMSetFont)
 ON_MESSAGE(WM_GETFONT, OnWMGetFont)
 ON_WM_SIZE()
 ON_WM_KEYDOWN()
 ON_WM_GETDLGCODE()
 ON_WM_CONTEXTMENU()
 ON_WM_INITMENUPOPUP()
 ON_COMMAND(ID_MAPED_POPUP_INC, OnInc)
 ON_COMMAND(ID_MAPED_POPUP_DEC, OnDec)
 ON_COMMAND(ID_MAPED_POPUP_SETTO, OnSetTo)
 ON_COMMAND(ID_MAPED_POPUP_SUB, OnSub)
 ON_COMMAND(ID_MAPED_POPUP_ADD, OnAdd)
 ON_COMMAND(ID_MAPED_POPUP_MUL, OnMul)
 ON_COMMAND(ID_MAPED_POPUP_SMOOTH3X3, OnSmoothing3x3)
 ON_COMMAND(ID_MAPED_POPUP_SMOOTH5X5, OnSmoothing5x5)
 ON_COMMAND(ID_MAPED_POPUP_REV, OnRevert)
 ON_COMMAND(ID_MAPED_POPUP_EXPORTCSV, OnExportCsv)
 ON_COMMAND(ID_MAPED_POPUP_IMPORTCSV, OnImportCsv)
 ON_COMMAND(ID_MAPED_POPUP_COPY, OnClipboardCopy)
 ON_COMMAND(ID_MAPED_POPUP_PASTE, OnClipboardPaste)
 ON_COMMAND(ID_MAPED_POPUP_INTERPOL, OnInterpolate)
 ON_COMMAND(ID_MAPED_POPUP_UNDO, OnUndo)
 ON_COMMAND(ID_MAPED_POPUP_REDO, OnRedo)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_INC, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_DEC, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_SETTO, OnUpdateSetTo)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_SUB, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_ADD, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_MUL, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_REV, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_SMOOTH3X3, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_SMOOTH5X5, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_IMPORTCSV, OnUpdateImportCsv)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_INTERPOL, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_UNDO, OnUpdateUndo)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_REDO, OnUpdateRedo)
 ON_UPDATE_COMMAND_UI(ID_MAPED_POPUP_PASTE, OnUpdateControls)
END_MESSAGE_MAP()

UINT CMapEditorCtrl::OnGetDlgCode()
{
 const MSG *pMsg = GetCurrentMessage();
 pMsg = ((const MSG *)pMsg->lParam);

 if (pMsg != NULL)
 {
  if (pMsg->message == WM_KEYDOWN)
  {
   if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_TAB) //don't eat Esc and TAB keys, so parent dialog will be able to process them
    return 0;
  }
 }

 return Super::OnGetDlgCode() | DLGC_WANTALLKEYS;
}

BOOL CMapEditorCtrl::Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id)
{
 BOOL result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, NULL, NULL, dwStyle,
               rect.left, rect.top, rect.Width(), rect.Height(), pParent->GetSafeHwnd(), (HMENU)((UINT_PTR)id));
 if (!result)
  SECUMessageBox(_T("Error creating MapEditorCtrl control's window!"));

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

 m_forceRedraw = true;
}

void CMapEditorCtrl::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(!m_readOnly);
}

void CMapEditorCtrl::OnUpdateSetTo(CCmdUI* pCmdUI)
{
 pCmdUI->Enable((!m_readOnly || m_rof_setto));
}

void CMapEditorCtrl::OnUpdateUndo(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(!m_readOnly && mp_undo->CanUndo());
}

void CMapEditorCtrl::OnUpdateRedo(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(!m_readOnly && mp_undo->CanRedo());
}

void CMapEditorCtrl::OnUpdateImportCsv(CCmdUI* pCmdUI)
{
 pCmdUI->Enable((!m_readOnly || m_rof_import));
}

void CMapEditorCtrl::OnUpdateClipboardPaste(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(!m_readOnly);
}

BOOL CMapEditorCtrl::PreTranslateMessage(MSG * pMsg)
{
 if (mp_tooltip)
  mp_tooltip->RelayEvent(pMsg);

 return Super::PreTranslateMessage(pMsg);
} 

BOOL CMapEditorCtrl::OnEraseBkgnd(CDC* pDC) 
{
 return TRUE; //prevent flickering
}

void CMapEditorCtrl::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 MenuHelpers::InitMenuPopup(this, pMenu, nIndex, bSysMenu);
}

void CMapEditorCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
 CMenu menu;
 VERIFY(menu.LoadMenu(IDR_MAPEDITOR_POPUP_MENU));
 CMenu *pSub = menu.GetSubMenu(0);
 pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CMapEditorCtrl::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 //make program to recreate bitmaps because new size may differ
 if (mp_bmOldGrid && m_dcGrid.GetSafeHdc())
 {
  m_dcGrid.SelectObject(mp_bmOldGrid);
  m_bmGrid.DeleteObject();
 }

 if (mp_bmOldMark && m_dcMark.GetSafeHdc())
 {
  m_dcMark.SelectObject(mp_bmOldMark);
  m_bmMark.DeleteObject();
 }

 //update position of edit if it is active
 if (mp_edit.get())
 {
  CDC* pDC = GetDC();
  CRect rect = _GetItemRect(pDC, m_cur_i, m_cur_j);
  ReleaseDC(pDC);

  if (NULL!=m_clipRgn.GetSafeHandle())
   m_clipRgn.DeleteObject();
  m_clipRgn.CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);

  mp_edit->MoveWindow(&rect);
 }

 Redraw();
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

void CMapEditorCtrl::OnSetFocus(CWnd* pLast)
{
 Super::OnSetFocus(pLast);
 if (m_dcGrid.GetSafeHdc())
 {
  _DrawGrid();
  CClientDC dc(this);
  _ShowImage(&dc);
 }
}

void CMapEditorCtrl::OnKillFocus(CWnd* pNewWnd)
{
 Super::OnKillFocus(pNewWnd);
 if (m_dcGrid.GetSafeHdc())
 {
  _DrawGrid();
  CClientDC dc(this);
  _ShowImage(&dc);
 }
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

void CMapEditorCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{  
 if (mp_other && mp_other != this)
 {
  float value = 0;
  if (!mp_other->_ValidateItem(mp_other->mp_edit.get(), &value))
   mp_other->_DeactivateEdit(); 
 }

 int i, j;
 if (_CellByPoint(point, i, j))
 {
  m_lbuttondown = true;
  if ((mp_edit.get() && mp_edit->GetSafeHwnd()))
  {
   float value = 0;
   if (_ValidateItem(mp_edit.get(), &value))
   {
    if (m_OnChange)
    {
     float previousValue = _GetItemTr(m_cur_i, m_cur_j);       
     if (previousValue != value)
     {
      mp_undo->Add();
      _SetItemTr(m_cur_i, m_cur_j, value); //save result
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

    bool redraw = _SelChange(i, j, i, j);
    if (redraw)
    {
     _DrawGrid();
     CClientDC dc(this);
     _ShowImage(&dc); //update only selected cells
    }
   }
  }
  else
  { //no previous controls created, just update position of selection
   SetFocus();
   bool redraw = _SelChange(i, j, i, j);
   if (redraw)
   {
    _DrawGrid();
    _DrawLabels();
    CClientDC dc(this);
    _ShowImage(&dc);
   }
  }
 }

 return Super::OnLButtonDown(nFlags, point);
}

void CMapEditorCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
 _SelChange(m_cur_i, m_cur_j, m_end_i, m_end_j); //notify about change of selection

 m_lbuttondown = false;

 return Super::OnLButtonUp(nFlags, point);
}

void CMapEditorCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
 if (m_readOnly)
 {
  Super::OnLButtonDblClk(nFlags, point);
  return;
 }

 int i, j;
 if (_CellByPoint(point, i, j))
 {
  if ((mp_edit.get() && mp_edit->GetSafeHwnd()))
  {
   float value = 0;
   if (_ValidateItem(mp_edit.get(), &value))
   {
    if (m_OnChange)
    {
     float previousValue = _GetItemTr(m_cur_i, m_cur_j);       
     if (previousValue != value)
     {
      mp_undo->Add();
      _SetItemTr(m_cur_i, m_cur_j, value); //save result
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
    _SelChange(i, j, i, j);
    _ActivateEdit();
   }
  }
  else
  { //no previous controls created, just create new control
   bool redraw = _SelChange(i, j, i, j);
   if (redraw)
   {
    _DrawGrid();
    CClientDC dc(this);
    _ShowImage(&dc); //update only selected cells
   }
   _ActivateEdit();
  }
 }

 Super::OnLButtonDblClk(nFlags, point);
}

void CMapEditorCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
 if (mp_edit.get() || !m_dcGrid.GetSafeHdc())
 {
  Super::OnMouseMove(nFlags, point);
  return;
 }

 int i, j;
 if (_CellByPoint(point, i, j))
 {
  if (m_lbuttondown && (nFlags & MK_LBUTTON))
  {
   bool redraw = _SelChange(m_cur_i, m_cur_j, i, j, false);
   if (redraw)
   {
    _DrawGrid();
    CClientDC dc(this);
    _ShowImage(&dc); //update only selected cells
   }
  }
  else if (mp_mapOrig)
  {
   if (!mp_tooltip)
   { //activate toolttip
    _ActivateTooltip(i, j);
   }
   else if (m_tp_i != i || m_tp_j != j)
   {
    mp_tooltip->Activate(false);
    delete mp_tooltip;
    _ActivateTooltip(i, j);
   }
  }
 }

 Super::OnMouseMove(nFlags, point);
}

BOOL CMapEditorCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
 if (m_readOnly)
 {
  return Super::OnMouseWheel(nFlags, zDelta, pt);
 }

 if (mp_edit.get())
 {
  float value = 0;
  if (mp_edit->GetValue(value))
  {   
   value+= zDelta > 0 ? m_increment : -m_increment;
   value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
   mp_edit->SetValue(value);
  }
  else
   mp_edit->SetValue(0.0f);

  return Super::OnMouseWheel(nFlags, zDelta, pt);
 }

 const std::vector<std::pair<int, int> >& sel = GetSelection();
 if (sel.size() > 0) mp_undo->Add();
 for(size_t i = 0; i < sel.size(); ++i)
 {
  float value = _GetItemTr(sel[i].first, sel[i].second);
  value+= zDelta > 0 ? m_increment : -m_increment;
  value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
  _SetItemTr(sel[i].first, sel[i].second, value);
 }

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid();
 CClientDC dc(this);
 _ShowImage(&dc);
 
 return Super::OnMouseWheel(nFlags, zDelta, pt);
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
  if (previousValue != value)
  {
   mp_undo->Add();
   _SetItemTr(m_cur_i, m_cur_j, value); //save result
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

 if (nChar==VK_RETURN)
 {
  _DeactivateEdit();
 }
}

void CMapEditorCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 if (!m_readOnly && ((nChar >= 0x30 && nChar <= 0x39) || nChar==VK_OEM_MINUS))
 { //activate edit when user start to type a number
  _ActivateEdit();
  CString cs;
  if (nChar==VK_OEM_MINUS)
   cs.Insert(0, '-');
  else
   cs.Insert(0, nChar);

  mp_edit->SetWindowText(cs);

  Super::OnKeyDown(nChar, nRepCnt, nFlags);
  return;
 }

 switch(nChar)
 { 
  case 0x43:   //"C"
   if (GetKeyState(VK_CONTROL) & 0x8000)
   { //Ctrl+C
    _ClipboardCopy();
   }
   break;
   
  case 0x56:   //"V"
   if (GetKeyState(VK_CONTROL) & 0x8000)
   { //Ctrl+V
    _ClipboardPaste();
   }
   break;

  case VK_HOME:
   {
    bool redraw;
    if (GetKeyState(VK_CONTROL) & 0x8000)
     redraw = _SelChange(m_rows-1, m_cur_j, m_rows-1, m_cur_j);
    else
     redraw = _SelChange(m_cur_i, 0, m_cur_i, 0);      
    if (redraw)
    {
     _DrawGrid();
     CClientDC dc(this);
     _ShowImage(&dc); //update only selected cells
    }
   }
   break;
  case VK_END:
   {
    bool redraw;
    if (GetKeyState(VK_CONTROL) & 0x8000)
     redraw = _SelChange(0, m_cur_j, 0, m_cur_j);
    else
     redraw = _SelChange(m_cur_i, m_cols-1, m_cur_i, m_cols-1); 
    if (redraw)
    {
     _DrawGrid();
     CClientDC dc(this);
     _ShowImage(&dc); //update only selected cells
    }
   }
   break;

  case VK_INSERT:
    if (GetKeyState(VK_CONTROL) & 0x8000)
     _ClipboardCopy(); //Ctrl+Insert
    else if (GetKeyState(VK_SHIFT) & 0x8000)
     _ClipboardPaste(); //Shift+Insert
    else
     _ActivateEdit();
   break;

  case VK_RETURN:                 //move down if user pressed Enter key
   if (GetSelection().size() < 2) //ignore Return key if more than one cell is selected
   {
    bool redraw;
    if (m_cur_i > 0)
    {
     redraw = _SelChange(m_cur_i-1, m_cur_j, m_cur_i-1, m_cur_j);
    }
    else
    {
     if (m_cur_j < m_cols-1)   
      redraw = _SelChange(m_rows-1, m_cur_j+1, m_rows-1, m_cur_j+1);
     else
      redraw = _SelChange(m_rows-1, 0, m_rows-1, 0);
    }

    if (redraw)
    {
     _DrawGrid();
     CClientDC dc(this);
     _ShowImage(&dc); //update only selected cells
    }
   }
   break;

  case VK_UP:
   if ((GetKeyState(VK_SHIFT) & 0x8000))
   {
    if (m_end_i < m_rows-1)
    {
     bool redraw = _SelChange(m_cur_i, m_cur_j, m_end_i + 1, m_end_j);
     if (redraw)
     {
      _DrawGrid();
      CClientDC dc(this);
      _ShowImage(&dc); //update only selected cells
     }
    }
   }
   else
   {
    if (m_cur_i < m_rows-1)
    {
     bool redraw = _SelChange(m_cur_i + 1, m_cur_j,  m_cur_i + 1, m_cur_j);
     if (redraw)
     {
      _DrawGrid();
      CClientDC dc(this);
      _ShowImage(&dc); //update only selected cells
     }
    }
    else
    {
     if (m_enAbroadUp)
     {
      bool redraw = _SelChange(m_cur_i, m_cur_j, m_cur_i, m_cur_j);
      if (redraw)
      {
       _DrawGrid();
       CClientDC dc(this);
       _ShowImage(&dc); //update only selected cells
      }

      if (m_OnAbroadMove)
       m_OnAbroadMove(ABROAD_UP, m_cur_j);
     }
    }
   }
   break;
  case VK_DOWN:
   if ((GetKeyState(VK_SHIFT) & 0x8000))
   {
    if (m_end_i > 0)
    {
     bool redraw = _SelChange(m_cur_i, m_cur_j, m_end_i - 1, m_end_j);
     if (redraw)
     {
      _DrawGrid();
      CClientDC dc(this);
      _ShowImage(&dc); //update only selected cells
     }
    }
   }
   else
   {
    if (m_cur_i > 0)
    {
     bool redraw = _SelChange(m_cur_i - 1, m_cur_j, m_cur_i - 1, m_cur_j);
     if (redraw)
     {
      _DrawGrid();
      CClientDC dc(this);
      _ShowImage(&dc); //update only selected cells
     }
    }
    else
    {
     if (m_enAbroadDown)
     {
      bool redraw = _SelChange(m_cur_i, m_cur_j, m_cur_i, m_cur_j);
      if (redraw)
      {
       _DrawGrid();
       CClientDC dc(this);
       _ShowImage(&dc); //update only selected cells
      }

      if (m_OnAbroadMove)
       m_OnAbroadMove(ABROAD_DOWN, m_cur_j);
     }
    }
   }
   break;
  case VK_LEFT:
   if ((GetKeyState(VK_SHIFT) & 0x8000))
   {
    if (m_end_j > 0)
    {
     bool redraw = _SelChange(m_cur_i, m_cur_j, m_end_i, m_end_j - 1);
     if (redraw)
     {
      _DrawGrid();
      CClientDC dc(this);
      _ShowImage(&dc); //update only selected cells
     }
    }
   }
   else
   {
    if (m_cur_j > 0)
    {
     bool redraw = _SelChange(m_cur_i, m_cur_j - 1, m_cur_i, m_cur_j - 1);
     if (redraw)
     {
      _DrawGrid();
      CClientDC dc(this);
      _ShowImage(&dc); //update only selected cells
     }
    }
   }
   break;
  case VK_RIGHT:
   if ((GetKeyState(VK_SHIFT) & 0x8000))
   {
    if (m_end_j < m_cols-1)
    {
     bool redraw = _SelChange(m_cur_i, m_cur_j, m_end_i, m_end_j + 1);
     if (redraw)
     {
      _DrawGrid();
      CClientDC dc(this);
      _ShowImage(&dc); //update only selected cells
     }
    }
   }
   else
   {
    if (m_cur_j < m_cols-1)
    {
     bool redraw = _SelChange(m_cur_i, m_cur_j + 1, m_cur_i, m_cur_j + 1);
     if (redraw)
     {
      _DrawGrid();
      CClientDC dc(this);
      _ShowImage(&dc); //update only selected cells
     }
    }
   }
   break;

  case 0x57:  //"W"
  case VK_OEM_6:   //"]"
  case VK_OEM_PERIOD: //"."
   if (m_readOnly) break;
   {
    const std::vector<std::pair<int, int> >& sel = GetSelection();
    if (sel.size() > 0) mp_undo->Add();
    for(size_t i = 0; i < sel.size(); ++i)
    {
     float value = _GetItemTr(sel[i].first, sel[i].second);
     value+= m_increment;
     value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
     _SetItemTr(sel[i].first, sel[i].second, value);
    }
    m_OnChange();
    if (!m_absGrad)
    {
     _UpdateMinMaxElems();
    }
    _DrawGrid();  //fully redraw grid because gradient should be updated
    CClientDC dc(this);
    _ShowImage(&dc);
   }
   break;

  case 0x51:   //"Q"
  case VK_OEM_5: //"\"
  case VK_OEM_COMMA: //","
   if (m_readOnly) break;
   {
    const std::vector<std::pair<int, int> >& sel = GetSelection();
    if (sel.size() > 0) mp_undo->Add();
    for(size_t i = 0; i < sel.size(); ++i)
    {
     float value = _GetItemTr(sel[i].first, sel[i].second);
     value-= m_increment;
     value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
     _SetItemTr(sel[i].first, sel[i].second, value);
    }
    m_OnChange();
    if (!m_absGrad)
    {
     _UpdateMinMaxElems();
    }
   _DrawGrid(); //fully redraw grid because gradient should be updated
   CClientDC dc(this);
   _ShowImage(&dc);
  }
  break;

 case 'A': //Ctrl+A - select all cells
  if (GetKeyState(VK_CONTROL) & 0x8000)
  {
   bool redraw = _SelChange(0, 0, m_rows-1, m_cols-1);
   if (redraw)
   {
    _DrawGrid();
    _DrawLabels(); 
    CClientDC dc(this);
    _ShowImage(&dc);
   }
  }
  break;

 case 'Z': //Ctrl+Z Undo
  if (GetKeyState(VK_CONTROL) & 0x8000)
  {
   OnUndo();
  }
  break;

 case 'Y': //Ctrl+Y Undo
  if (GetKeyState(VK_CONTROL) & 0x8000)
  {
   OnRedo();
  }
  break;
 }

 Super::OnKeyDown(nChar, nRepCnt, nFlags);
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
  if (previousValue != value)
  {
   mp_undo->Add();
   _SetItemTr(m_cur_i, m_cur_j, value); //save result
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

 _DeactivateEdit();
}

void CMapEditorCtrl::OnClipboardCopy()
{
 _ClipboardCopy();
}

void CMapEditorCtrl::OnClipboardPaste()
{
 _ClipboardPaste();
}

void CMapEditorCtrl::setOnAbroadMove(EventHandler2 OnCB)
{
 m_OnAbroadMove = OnCB;
}

void CMapEditorCtrl::setOnChange(EventHandler OnCB)
{
 m_OnChange = OnCB;
}

void CMapEditorCtrl::setOnSelChange(EventHandler OnCB)
{
 m_OnSelChange = OnCB;
}

void CMapEditorCtrl::setOnValueTransform(EventHandler3 OnCB)
{
 m_OnValueTransform = OnCB;
}

void CMapEditorCtrl::AttachMap(float* p_map, float* p_mapOrig /*= NULL*/)
{
 ASSERT(p_map);
 mp_map = p_map;
 mp_mapOrig = p_mapOrig;
 mp_undo->Attach(mp_map, m_rows*m_cols);
 _UpdateMinMaxElems();
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

void CMapEditorCtrl::SetCSVSepSymbol(char sepsymb)
{
 m_csvsep_symb = sepsymb;
}

void CMapEditorCtrl::SetRange(float i_min, float i_max)
{
 m_minVal = i_min;
 m_maxVal = i_max;
}

void CMapEditorCtrl::SetDecimalPlaces(int value, int horiz, int vert)
{
 m_decPlaces = value;
 m_decPlacesH = horiz;
 m_decPlacesV = vert;
}

void CMapEditorCtrl::SetValueIncrement(float inc)
{
 m_increment = inc;
}

void CMapEditorCtrl::SetArguments(float i_arg, float j_arg)
{
 if (!m_showMarkers)
  return;
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

   float pd = _GetVGrid(p2) - _GetVGrid(p1);
   if (pd != 0) //prevent division by zero
    ball.y = (int)(cp1.y - (((i_arg - _GetVGrid(p1)) / pd) * (cp1.y - cp2.y)));
   else
    ball.y = cp1.y;
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

   float pd = mp_horizLabels[p2] - mp_horizLabels[p1];
   if (pd != 0) //prevent division by zero
    ball.x = (int)(cp1.x + (((j_arg - mp_horizLabels[p1]) / pd) * (cp2.x - cp1.x)));
   else
    ball.x = cp1.x;

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
 SetFocus();
 
 if (i > m_rows-1)
  i = m_rows-1;
 if (j > m_cols-1)
  j = m_cols-1;

 bool redraw = _SelChange(i, j, i, j);
 if (redraw)
 {
  _DrawGrid();
  _DrawLabels(); 
  CClientDC dc(this);
  _ShowImage(&dc);
 }
}

const std::vector<std::pair<int, int> >& CMapEditorCtrl::GetSelection(void)
{
 if (m_sel_changed)
 { //update list of selected cells only if selection actually changed, see also _SelChange() method
  m_sel.clear();
  int bi = (std::max)(m_cur_i, m_end_i), bj = (std::min)(m_cur_j, m_end_j);
  int ei = (std::min)(m_cur_i, m_end_i), ej = (std::max)(m_cur_j, m_end_j);
  for(int i = bi; i >= ei; --i)
   for(int j = bj; j <= ej; ++j)
    m_sel.push_back(std::make_pair(i, j));
  m_sel_changed = false;
 }

 return m_sel;
}

void CMapEditorCtrl::EnableAbroadMove(bool up, bool down)
{
 m_enAbroadUp = up;
 m_enAbroadDown = down;
}

void CMapEditorCtrl::UpdateDisplay(const std::vector<std::pair<int, int> >* p_updList /*= NULL*/)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
 {
  if (!p_updList)
  {
   mp_edit->SetValue(_GetItemTr(m_cur_i, m_cur_j));
  }
  else
  {
   for(size_t i = 0; i < p_updList->size(); ++i)
   {
    if (m_cur_i == (*p_updList)[i].first && m_cur_j == (*p_updList)[i].second)
    { //update value in the active edit box
     mp_edit->SetValue(_GetItemTr(m_cur_i, m_cur_j));
     break;
    }
   }
  }
 }

 _UpdateMinMaxElems();

 CClientDC dc(this);
 if (!p_updList)
 { //update full region
  _DrawGrid();
  _DrawLabels();
  _ShowImage(&dc);
 }
 else
 { //update specified cells only
  _DrawGrid(p_updList);
  CRect sel_rc;
  _GetSelRect(sel_rc);
  _ShowImage(&dc, &sel_rc);
 }
}

void CMapEditorCtrl::OnEnable(BOOL bEnable)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
  mp_edit->EnableWindow(bEnable);
 Redraw(); //redraw all elements
}

void CMapEditorCtrl::ShowMarkers(bool show, bool redraw /*=true*/)
{
 show = show && m_useMarkers;
 if (m_showMarkers == show)
  return;
 m_showMarkers = show;

 if (redraw)
 {
  _DrawMarkers(); //draw marker(s)
  CClientDC dc(this);
  _ShowImage(&dc);
 }
}

void CMapEditorCtrl::SetGradientList(const std::vector<COLORREF>& colors)
{
 m_gradColor = colors;
}

void CMapEditorCtrl::SetItemColor(int i, int j, COLORREF color)
{
 _SetItem<COLORREF>(mp_itemColors, i, j, color);
}

void CMapEditorCtrl::Redraw(void)
{
 m_forceRedraw = true;
 Invalidate();
}

bool CMapEditorCtrl::GetSpotMarkers(void)
{
 return m_spotMarkers;
}

void CMapEditorCtrl::OnInc()
{
 const std::vector<std::pair<int, int> >& sel = GetSelection();
 if (sel.size() > 0) mp_undo->Add();
 for(size_t i = 0; i < sel.size(); ++i)
 {
  float value = _GetItemTr(sel[i].first, sel[i].second);
  value+= m_increment;
  value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
  _SetItemTr(sel[i].first, sel[i].second, value);
 }

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid();
 CClientDC dc(this);
 _ShowImage(&dc);   
}

void CMapEditorCtrl::OnDec()
{
 const std::vector<std::pair<int, int> >& sel = GetSelection();
 if (sel.size() > 0) mp_undo->Add();
 for(size_t i = 0; i < sel.size(); ++i)
 {
  float value = _GetItemTr(sel[i].first, sel[i].second);
  value-= m_increment;
  value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
  _SetItemTr(sel[i].first, sel[i].second, value);
 }

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid();
 CClientDC dc(this);
 _ShowImage(&dc);   
}

void CMapEditorCtrl::OnSetTo()
{
 CDynFieldsContainer dfd(this, _T(""), 120, true);
 float value = .0f;
 dfd.AppendItem(MLL::GetString(IDS_MAPED_POPUP_SETTO), _T(""), m_minVal, m_maxVal, m_increment, m_decPlaces, &value, MLL::GetString(IDS_MAPED_POPUP_SETTO_TT));
 if (dfd.DoModal()==IDOK)
 {
  value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
  const std::vector<std::pair<int, int> >& sel = GetSelection();
  if (sel.size() > 0) mp_undo->Add();
  for(size_t i = 0; i < sel.size(); ++i)
   _SetItemTr(sel[i].first, sel[i].second, value);

  if (m_OnChange)
   m_OnChange();
  if (!m_absGrad)
  {
   _UpdateMinMaxElems();
  }

  _DrawGrid();
  CClientDC dc(this);
  _ShowImage(&dc);   
 }
}

void CMapEditorCtrl::OnSub()
{
 CDynFieldsContainer dfd(this, _T(""), 120, true);
 float sub = .0f;
 dfd.AppendItem(MLL::GetString(IDS_MAPED_POPUP_SUB), _T(""), m_minVal, m_maxVal, m_increment, m_decPlaces, &sub, MLL::GetString(IDS_MAPED_POPUP_SUB_TT));
 if (dfd.DoModal()==IDOK)
 {
  const std::vector<std::pair<int, int> >& sel = GetSelection();
  if (sel.size() > 0) mp_undo->Add();
  for(size_t i = 0; i < sel.size(); ++i)
  {
   float value = _GetItemTr(sel[i].first, sel[i].second);
   value-= sub;
   value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
   _SetItemTr(sel[i].first, sel[i].second, value);
  }

  m_OnChange();
  if (!m_absGrad)
  {
   _UpdateMinMaxElems();
  }

  _DrawGrid();
  CClientDC dc(this);
  _ShowImage(&dc);   
 }
}

void CMapEditorCtrl::OnAdd()
{
 CDynFieldsContainer dfd(this, _T(""), 120, true);
 float add = .0f;
 dfd.AppendItem(MLL::GetString(IDS_MAPED_POPUP_ADD), _T(""), m_minVal, m_maxVal, m_increment, m_decPlaces, &add, MLL::GetString(IDS_MAPED_POPUP_ADD_TT));
 if (dfd.DoModal()==IDOK)
 {
  const std::vector<std::pair<int, int> >& sel = GetSelection();
  if (sel.size() > 0) mp_undo->Add();
  for(size_t i = 0; i < sel.size(); ++i)
  {
   float value = _GetItemTr(sel[i].first, sel[i].second);
   value+= add;
   value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
   _SetItemTr(sel[i].first, sel[i].second, value);
  }

  m_OnChange();
  if (!m_absGrad)
  {
   _UpdateMinMaxElems();
  }

  _DrawGrid();
  CClientDC dc(this);
  _ShowImage(&dc);   
 }
}

void CMapEditorCtrl::OnMul()
{
 CDynFieldsContainer dfd(this, _T(""), 120, true);
 float mul = .0f;
 dfd.AppendItem(MLL::GetString(IDS_MAPED_POPUP_MUL), _T(""), -m_maxVal, m_maxVal, m_increment, m_decPlaces, &mul, MLL::GetString(IDS_MAPED_POPUP_MUL_TT));
 if (dfd.DoModal()==IDOK)
 {
  const std::vector<std::pair<int, int> >& sel = GetSelection();
  if (sel.size() > 0) mp_undo->Add();
  for(size_t i = 0; i < sel.size(); ++i)
  {
   float value = _GetItemTr(sel[i].first, sel[i].second);
   value*= mul;
   value = MathHelpers::RestrictValue(value, m_minVal, m_maxVal);
   _SetItemTr(sel[i].first, sel[i].second, value);
  }

  m_OnChange();
  if (!m_absGrad)
  {
   _UpdateMinMaxElems();
  }

  _DrawGrid();
  CClientDC dc(this);
  _ShowImage(&dc);   
 }
}

void CMapEditorCtrl::OnSmoothing3x3()
{
 mp_undo->Add();
 float* p_source_function = new float[m_rows * m_cols];
 bool* p_mask = new bool[m_rows * m_cols];
 for(int i = 0; i < m_rows; ++i)
  for(int j = 0; j < m_cols; ++j)
   p_mask[i * m_cols + j] = _isCellInSelRect(i, j);
 std::copy(mp_map, mp_map + (m_rows * m_cols), p_source_function);
 MathHelpers::Smooth2D(p_source_function, mp_map, m_rows, m_cols, 3, p_mask);
 delete[] p_source_function;
 delete[] p_mask;

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid();
 CClientDC dc(this);
 _ShowImage(&dc);   
}

void CMapEditorCtrl::OnSmoothing5x5()
{
 mp_undo->Add();
 float* p_source_function = new float[m_rows * m_cols];
 bool* p_mask = new bool[m_rows * m_cols];
 for(int i = 0; i < m_rows; ++i)
  for(int j = 0; j < m_cols; ++j)
   p_mask[i * m_cols + j] = _isCellInSelRect(i, j);
 std::copy(mp_map, mp_map + (m_rows * m_cols), p_source_function);
 MathHelpers::Smooth2D(p_source_function, mp_map, m_rows, m_cols, 5, p_mask);
 delete[] p_source_function;
 delete[] p_mask;

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid();
 CClientDC dc(this);
 _ShowImage(&dc);   
}

void CMapEditorCtrl::OnInterpolate()
{
 int bi = (std::min)(m_cur_i, m_end_i), bj = (std::min)(m_cur_j, m_end_j);
 int ei = (std::max)(m_cur_i, m_end_i), ej = (std::max)(m_cur_j, m_end_j);

 float func[2][2] =  {{_GetItemTr(bi, bj), _GetItemTr(bi, ej)},
                      {_GetItemTr(ei, bj), _GetItemTr(ei, ej)}};

 float xBins[2] = {(float)bj, (float)ej};
 float yBins[2] = {(float)bi, (float)ei};

 mp_undo->Add();

 for(int i = bi; i <= ei; ++i)
 {
  for(int j = bj; j <= ej; ++j)
  {
   float value = MathHelpers::BilinearInterpolation<2, 2>((float)j, (float)i, func, xBins, yBins);
   _SetItemTr(i, j, value);
  }
 }

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid();
 CClientDC dc(this);
 _ShowImage(&dc);   
}

void CMapEditorCtrl::OnRevert()
{
 const std::vector<std::pair<int, int> >& sel = GetSelection();
 if (sel.size() > 0) mp_undo->Add();
 for(size_t i = 0; i < sel.size(); ++i)
 {
  float value = _GetItemTrO(sel[i].first, sel[i].second);
  _SetItemTr(sel[i].first, sel[i].second, value);
 }

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid(); //update all cells because if at least one cell is changed, we should update gradient of all cells
 CClientDC dc(this);
 _ShowImage(&dc);   
}

void CMapEditorCtrl::OnExportCsv()
{
 static TCHAR BASED_CODE szFilter[] = _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
 CFileDialogEx save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("CSV");
 if (save.DoModal()==IDOK)
 {
  FILE* fh = _tfopen(save.GetPathName(), _T("wb+"));
  if (NULL == fh)
  {
   SECUMessageBox(_T("Can't open file for writing!"), MB_OK | MB_ICONERROR);
   return;
  }

  char temp[16];
  sprintf(temp, "%%s%c", m_csvsep_symb);

  //write function's values
  for(int i = m_rows-1; i >= 0; --i)
  {
   for(int j = 0; j < m_cols; ++j)
   {
    float value = _GetItemTr(i, j);
    CString s;
    s.Format(_T("%.*f"), m_decPlaces, (float)value);

    if (j == m_cols-1)
     fprintf(fh, "%s\r\n", (LPCTSTR)s);    
    else
     fprintf(fh, temp, (LPCTSTR)s);
   }   
  }

  //write horizontal axis's grid
  for(int j = 0; j < m_cols; ++j)
  {   
   if (j == m_cols-1)
    fprintf(fh, "%s", (LPCTSTR)m_horizLabels[j]);    
   else
    fprintf(fh, temp, (LPCTSTR)m_horizLabels[j]);
  }

  fclose(fh);
 }
}

void CMapEditorCtrl::OnImportCsv()
{
 static TCHAR BASED_CODE szFilter[] = _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
 CFileDialogEx open(TRUE,NULL,NULL,NULL,szFilter,NULL);
 if (open.DoModal()==IDOK)
 {
  FILE* file = fopen(open.GetPathName(), "r");
  if (NULL == file)
  {
   SECUMessageBox("Can't open file for reading!", MB_OK | MB_ICONERROR);
   return;
  }
  //parse file
  std::vector<std::vector<std::string> > csv;
  char line[8192+1];
  while (fgets(line, 8192, file))
  {
   csv.push_back(StrUtils::TokenizeStr(line, m_csvsep_symb));
  }

  fclose(file);

  if (csv.size() != (size_t)(m_rows+1))
  {
   SECUMessageBox("Error reading csv file! Incorrect number of rows.", MB_OK | MB_ICONERROR);
   return;
  }
  
  for(size_t i = 0; i < csv.size(); ++i)
  {
   if (csv[i].size() != (size_t)m_cols)
   {
    SECUMessageBox("Error reading csv file! Incorrect number of values in row.", MB_OK | MB_ICONERROR);
    return;
   }
  }

  mp_undo->Add();

  //set read values
  for(int i = 0; i < m_rows; ++i)
   for(int j = 0; j < m_cols; ++j)
    _SetItemTr((m_rows-1)-i, j, (float)atof(csv[i][j].c_str())); //save result

  if (m_OnChange)
   m_OnChange();
  if (!m_absGrad)
  {
   _UpdateMinMaxElems();
  }

  _DrawGrid();
  _DrawLabels();
  CClientDC dc(this);
  _ShowImage(&dc);   
 }
}

bool CMapEditorCtrl::_CellByPoint(CPoint point, int& oi, int& oj)
{
 int i = 0;
 bool found = false;
 for(; i < m_rows; ++i)
 { //find row
  CRect rect = _GetItemRect(&m_dcGrid, i, 0);
  if (point.y >= rect.top && point.y <= rect.bottom)
  {
   found = true;
   break;
  }
 }

 if (found)
 { //find cell
  found = false;
  int j = 0;
  for(; j < m_cols; ++j)
  {   
   CRect rect = _GetItemRect(&m_dcGrid, 0, j);
   if (point.x >= rect.left && point.x <= rect.right)
   {
    oi = i, oj = j; //save result
    return true;
   }
  }
 }

 return false;
}

bool CMapEditorCtrl::_isCellInSelRect(int i, int j)
{
 int bi = (std::max)(m_cur_i, m_end_i), bj = (std::min)(m_cur_j, m_end_j);
 int ei = (std::min)(m_cur_i, m_end_i), ej = (std::max)(m_cur_j, m_end_j);
 return (i <= bi) && (i >= ei) && (j >= bj) && (j <= ej);
}

void CMapEditorCtrl::_GetSelRect(CRect& o_rc)
{
 CRect rc_lt = _GetItemRect(&m_dcGrid, (std::max)(m_cur_i, m_end_i), (std::min)(m_cur_j, m_end_j));
 CRect rc_rb = _GetItemRect(&m_dcGrid, (std::min)(m_cur_i, m_end_i), (std::max)(m_cur_j, m_end_j));
 o_rc.left = rc_lt.left, o_rc.top =  rc_lt.top;
 o_rc.right = rc_rb.right, o_rc.bottom = rc_rb.bottom;
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
 return CRect(MathHelpers::Round(j*(width+space))+w_off, MathHelpers::Round(((m_rows-1)-i)*(height+space)),
              MathHelpers::Round(j*(width+space) + width)+w_off, MathHelpers::Round(((m_rows-1)-i)*(height+space) + height));
}

CRect CMapEditorCtrl::_GetMarkerRect(int i, int j, int inflate /*=2*/)
{
 CRect rect = _GetItemRect(&m_dcMark, i, j);
 rect.InflateRect(inflate,inflate,inflate,inflate);
 return rect;
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

void CMapEditorCtrl::_DrawItem(CDC& dc, int i, int j, const CRect* p_rect /*=NULL*/)
{
 float value = _GetItemTr(i, j);
 CRect rect;
 if (!p_rect)
 {
  rect = _GetItemRect(&dc, i, j);
  p_rect = &rect;
 }
 int index = _GetGradIndex(value);
 COLORREF customColor = _GetItem<COLORREF>(mp_itemColors, i, j);
 COLORREF gradColor = customColor ? customColor : m_gradColor[index];
 if (_isCellInSelRect(i, j))
 {
  if (::GetFocus()==m_hWnd)      
   gradColor = RGB(220,220,220);
  else
   gradColor = GDIHelpers::Brighten(gradColor, 80);
 }
 dc.SetBkColor(IsWindowEnabled() ? gradColor : GetSysColor(COLOR_3DFACE));
 dc.FillSolidRect(p_rect, IsWindowEnabled() ? gradColor : GetSysColor(COLOR_3DFACE));
 dc.Draw3dRect(p_rect, RGB(0,0,0), RGB(0,0,0));
 dc.DrawText(_FloatToStr(value, m_decPlaces), (LPRECT)p_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
}

void CMapEditorCtrl::_DrawGrid(const std::vector<std::pair<int, int> >* p_updList /*= NULL*/)
{
 if (NULL==m_dcGrid.GetSafeHdc())
  return;

 if (!p_updList)
 { //draw all cells
  CRect rc;
  GetClientRect(&rc);
  rc.left+=_GetLabelWidth(&m_dcGrid);
  rc.bottom-=_GetLabelHeight(&m_dcGrid);
  m_dcGrid.FillRect(rc, &m_bgrdBrush); //fill client rect with background
  m_dcGrid.SetBkMode(TRANSPARENT);
  m_dcGrid.SetTextColor(GetSysColor(IsWindowEnabled() ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
  CFont* oldFont = m_dcGrid.SelectObject(&m_cFont);
  for(int i = 0; i < m_rows; ++i)
   for(int j = 0; j < m_cols; ++j)
    _DrawItem(m_dcGrid, i, j);
  m_dcGrid.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
  m_dcGrid.SelectObject(oldFont);
 }
 else
 { //draw only certain cell(s)
  m_dcGrid.SetBkMode(TRANSPARENT);
  m_dcGrid.SetTextColor(GetSysColor(IsWindowEnabled() ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
  CFont* oldFont = m_dcGrid.SelectObject(&m_cFont);
  for(size_t s = 0; s < p_updList->size(); ++s)
  {
   int i = (*p_updList)[s].first, j = (*p_updList)[s].second;
   CRect rc = _GetItemRect(&m_dcGrid, i, j);
   m_dcGrid.FillRect(rc, &m_bgrdBrush); //erase (fill cell rect with background)
   _DrawItem(m_dcGrid, i, j, &rc); //draw
  }
  m_dcGrid.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
  m_dcGrid.SelectObject(oldFont);
 }
}

void CMapEditorCtrl::_DrawLabels(void)
{
 if (NULL==m_dcGrid.GetSafeHdc())
  return;
 //draw vertical labels
 if (mp_vertLabels && m_vertShow)
 {
  CRect rc;
  GetClientRect(&rc);
  rc.right = rc.left + _GetLabelWidth(&m_dcGrid);
  m_dcGrid.FillRect(rc, &m_bgrdBrush); //fill rect with background
  m_dcGrid.SetTextAlign(TA_LEFT|TA_TOP|TA_NOUPDATECP);
  CFont* oldFont = m_dcGrid.SelectObject(&m_cFont);
  for(int i = 0; i < m_rows; ++i)
  {
   CRect rect = _GetItemRect(&m_dcGrid, i, 0);
   m_dcGrid.TextOut(0, rect.top + (rect.Height() - m_label_height) / 2, m_vertLabels[i]);
  }
  m_dcGrid.SelectObject(oldFont); 
  m_dcGrid.SetTextAlign(TA_LEFT|TA_TOP|TA_NOUPDATECP);
 }
 
 //draw horizontal labels
 if (mp_horizLabels && m_horizShow)
 {
  CRect clrc;
  GetClientRect(&clrc);
  CRect rc = clrc;
  rc.top = rc.bottom - _GetLabelHeight(&m_dcGrid);
  m_dcGrid.FillRect(rc, &m_bgrdBrush); //fill rect with background
  m_dcGrid.SetTextAlign(TA_CENTER|TA_TOP|TA_NOUPDATECP);
  CFont* oldFont = m_dcGrid.SelectObject(&m_cFont);
  for(int j = 0; j < m_cols; ++j)
  {
   CRect rect = _GetItemRect(&m_dcGrid, 0, j);
   m_dcGrid.TextOut(rect.CenterPoint().x, clrc.bottom - m_label_height, m_horizLabels[j]);
  }
  m_dcGrid.SelectObject(oldFont); 
  m_dcGrid.SetTextAlign(TA_LEFT|TA_TOP|TA_NOUPDATECP);
 }
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

void CMapEditorCtrl::_DrawMarkers(void)
{
 if (m_showMarkers && IsWindowEnabled() && m_dcMark.GetSafeHdc())
 {
  CRect rc;
  GetClientRect(&rc);
  m_dcMark.FillRect(rc, &m_blackBrush);
 
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
 if (NULL==pDC || NULL==pDC->GetSafeHdc() || NULL==m_dcGrid.GetSafeHdc())
  return;
 CRect rc;
 GetClientRect(&rc);
 
 if (p_rect)
 { //show specified rect only
  if (m_showMarkers && m_dcMark.GetSafeHdc())
  { //with markers
   CDC memDC;
   CBitmap memBmp;
   memDC.CreateCompatibleDC(pDC);
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
  { //without markers
   if (NULL!=m_clipRgn.GetSafeHandle())
    pDC->SelectClipRgn(&m_clipRgn, RGN_XOR);
   pDC->BitBlt(p_rect->left, p_rect->top, rc.Width(), rc.Height(), &m_dcGrid, p_rect->left, p_rect->top, SRCCOPY);
   if (NULL!=m_clipRgn.GetSafeHandle())
    pDC->SelectClipRgn(NULL, RGN_COPY);
  }
 }
 else
 { //show full size image
  if (m_showMarkers && m_dcMark.GetSafeHdc())
  { //with markers
   CDC memDC;
   CBitmap memBmp;
   memDC.CreateCompatibleDC(pDC);
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
  else
  { //without markers
   if (NULL!=m_clipRgn.GetSafeHandle())
    pDC->SelectClipRgn(&m_clipRgn, RGN_XOR);
   pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &m_dcGrid, 0, 0, SRCCOPY);
   if (NULL!=m_clipRgn.GetSafeHandle())
    pDC->SelectClipRgn(NULL, RGN_COPY);
  }
 }
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

bool CMapEditorCtrl::_SelChange(int cur_i, int cur_j, int end_i, int end_j, bool notify /*= true*/)
{
 bool changed = (m_cur_i != cur_i) | (m_cur_j != cur_j) | (m_end_i != end_i) | (m_end_j != end_j);
 if (changed)
  m_sel_changed = true;
 m_cur_i = cur_i, m_cur_j = cur_j;
 m_end_i = end_i, m_end_j = end_j;
 if (m_OnSelChange && changed && notify)
  m_OnSelChange();
 return changed;
}

void CMapEditorCtrl::_ActivateTooltip(int i, int j)
{
 mp_tooltip = new CToolTipCtrl();
 mp_tooltip->Create(this);
 mp_tooltip->AddTool(this);
 mp_tooltip->Activate(true);
 mp_tooltip->SetDelayTime(10, TTDT_INITIAL); // length of time the pointer must remain stationary within a tool's bounding rectangle before the tool tip window appears.
 mp_tooltip->SetDelayTime(10, TTDT_AUTOPOP); // amount of time the tooltip window remains visible if the pointer is stationary within a tool's bounding rectangle
 mp_tooltip->SetDelayTime(10, TTDT_RESHOW);  // amount of time it takes for subsequent tooltip windows to appear as the pointer moves from one tool to another.
 float value = _GetItemTrO(i, j);
 CString s = MLL::LoadString(IDS_ORIG_TOOLTIP);
 s+= _FloatToStr(value, m_decPlaces);
 mp_tooltip->UpdateTipText(s, this);
 m_tp_i = i, m_tp_j = j;
}

void CMapEditorCtrl::_ActivateEdit()
{ 
 if (m_readOnly) return;
 CDC* pDC = GetDC();
 CRect rect = _GetItemRect(pDC, m_cur_i, m_cur_j);
 ReleaseDC(pDC);
 m_clipRgn.CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
 mp_edit.reset(new CEditExCustomKeys(fastdelegate::MakeDelegate(this, &CMapEditorCtrl::OnEditChar), fastdelegate::MakeDelegate(this, &CMapEditorCtrl::OnEditKill), m_minVal, m_maxVal, m_increment));
 mp_edit->Create(WS_BORDER | WS_CHILD | WS_VISIBLE | ES_CENTER, rect, this, 0);
 mp_edit->SetDecimalPlaces(m_decPlaces);
 mp_edit->SetValue(_GetItemTr(m_cur_i, m_cur_j));
 mp_edit->SetFocus();
 //set cursor to the right 
 CString str;
 mp_edit->GetWindowText(str);
 mp_edit->SetSel(str.GetLength(), str.GetLength());
 mp_edit->SetFont(GetFont(), TRUE);

 mp_other = this;
}

void CMapEditorCtrl::_DeactivateEdit(void)
{
 if (mp_edit.get() && mp_edit->GetSafeHwnd())
  mp_edit->DestroyWindow();
 mp_edit.reset();
 mp_other = NULL;

 if (NULL!=m_clipRgn.GetSafeHandle())
  m_clipRgn.DeleteObject();
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

float CMapEditorCtrl::_GetItemTrO(int i, int j)
{
 if (m_OnValueTransform)
  return m_OnValueTransform(_GetItem<float>(mp_mapOrig, i,j), 0); //to chart
 else
  return _GetItem<float>(mp_mapOrig, i,j);
}

void CMapEditorCtrl::_ClipboardCopy(void)
{
 int bi = (std::max)(m_cur_i, m_end_i), bj = (std::min)(m_cur_j, m_end_j);
 int ei = (std::min)(m_cur_i, m_end_i), ej = (std::max)(m_cur_j, m_end_j);
 CString str;
 for(int i = bi; i >= ei; --i)
 {
  for(int j = bj; j <= ej; ++j)
  {      
   float value = _GetItemTr(i, j);
   CString s;
   if (j == ej)
    s.Format(_T("%.*f\r\n"), m_decPlaces, (float)value);
   else
    s.Format(_T("%.*f\t"), m_decPlaces, (float)value);
   str+=s;
  }
 }

 // Open the clipboard, empty it and place new data
 if(::OpenClipboard(this->m_hWnd))
 {
  ::EmptyClipboard();

  // Allocate a global memory object for the text.  
  HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (str.GetLength() + 1) * sizeof(TCHAR)); 
  if (hglbCopy == NULL) 
  { 
   CloseClipboard(); 
   return; 
  } 
 
  // Lock the handle and copy the text to the buffer.  
  LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
  _tcscpy(lptstrCopy, (LPCTSTR)str); 
  GlobalUnlock(hglbCopy); 
 
  // Place the handle on the clipboard.  
  ::SetClipboardData(CF_TEXT, hglbCopy); 
  /*
  CRect src;
  _GetSelRect(src);  
  CDC* pWndDC  = GetWindowDC();
  CDC bmDC;
  bmDC.CreateCompatibleDC(pWndDC);
  CBitmap bitmap;
  bitmap.CreateCompatibleBitmap(pWndDC, src.Width(), src.Height());
  CBitmap* oldBitmap = bmDC.SelectObject(&bitmap);
  bmDC.BitBlt(0,0, src.Width(), src.Height(), pWndDC, src.left+1, src.top+1, SRCCOPY);
  bmDC.SelectObject(oldBitmap);
  HBITMAP hBitmap = (HBITMAP)bitmap.Detach();
  ::SetClipboardData (CF_BITMAP, hBitmap);
  */
  ::CloseClipboard();
 }
}

void CMapEditorCtrl::_ClipboardPaste(void)
{
 if (::IsClipboardFormatAvailable(CF_TEXT) && ::OpenClipboard(this->m_hWnd) && !m_readOnly) 
 {
  HGLOBAL hglb = GetClipboardData(CF_TEXT); 
  if (hglb != NULL) 
  { 
   LPTSTR lptstr = (LPTSTR)GlobalLock(hglb); 
   if (lptstr != NULL) 
   {     
    //parse string
    _TSSTREAM ss(lptstr);
    std::vector<std::vector<_TSTRING> > csv;
    _TSTRING line;
    while (std::getline(ss, line, _T('\n')))
    {
     line = StrUtils::rtrim(line, _T("\t\n\r"));
     csv.push_back(StrUtils::TokenizeStr(line.c_str(), _T('\t')));
    }

    GlobalUnlock(hglb); 

    mp_undo->Add();

    TCHAR decPt = _TDECIMAL_POINT(localeconv())[0]; //symbol of the decimal point used by current locale

    int bi = (std::max)(m_cur_i, m_end_i), bj = (std::min)(m_cur_j, m_end_j);
    for(size_t i = 0; i < csv.size(); ++i)
    {
     for(size_t j = 0; j < csv[i].size(); ++j)
     {
      int ii = bi - i, jj = bj + j;  
      if (ii < 0)
       ii = 0;
      if (jj >= m_cols)
       jj = m_cols-1;

       //change decimal point, thus it will be compatible with our current locale
      _TSTRING& s = csv[i][j];
       std::size_t pos = s.find_first_of(_T(".,"));
       if (pos != _TSTRING::npos)
        s[pos] = decPt;
       
      _SetItemTr(ii, jj, (float)atof(s.c_str())); //save result
     }     
    }

    m_OnChange();
    if (!m_absGrad)
    {
     _UpdateMinMaxElems();
    }
 
    _DrawGrid();  //fully redraw grid because gradient should be updated
    CClientDC dc(this);
    _ShowImage(&dc);
   } 
  } 
  CloseClipboard();     
 }
}

void CMapEditorCtrl::OnUndo()
{
 if (!mp_undo->CanUndo()) return;
 mp_undo->DoUndo();

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid();
 CClientDC dc(this);
 _ShowImage(&dc);
}

void CMapEditorCtrl::OnRedo()
{
 if (!mp_undo->CanRedo()) return;
 mp_undo->DoRedo();

 m_OnChange();
 if (!m_absGrad)
 {
  _UpdateMinMaxElems();
 }

 _DrawGrid();
 CClientDC dc(this);
 _ShowImage(&dc);
}

void CMapEditorCtrl::EnableReadonlyFeatures(bool setto, bool import)
{
 m_rof_setto = setto;
 m_rof_import = import;
}
