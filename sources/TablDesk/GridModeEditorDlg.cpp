/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include "stdafx.h"
#include "resource.h"
#include "common/Dll.h"
#include "common/MathHelpers.h"
#include "GridModeEditorDlg.h"
#include "MapIds.h"
#include "ui-core/EditEx.h"

static const COLORREF itemErrColor = RGB(255,120,120);

namespace {
class MapLimits
{
 public:
  MapLimits(int mapType) : m_mapType(mapType) {}

  float getLoLimit(void) const
  {
   switch(m_mapType)
   {
    case TYPE_MAP_DA_START:
    case TYPE_MAP_DA_IDLE:
    case TYPE_MAP_DA_WORK:
     return -15.0f;
    case TYPE_MAP_DA_TEMP_CORR:
     return -15.0f;
    default:
     ASSERT(0);
     return 0;
   }
  }
  
  float getHiLimit(void) const
  {
   switch(m_mapType)
   {
    case TYPE_MAP_DA_START:
    case TYPE_MAP_DA_IDLE:
    case TYPE_MAP_DA_WORK:
     return 55.0f;
    case TYPE_MAP_DA_TEMP_CORR:
     return 25.0f;
    default:
     ASSERT(0);
     return 0;
   }
  }
 private:
  int m_mapType;
};
}
                   
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
    GetNextWindow()->SetFocus();
  }
  void IncrementEdit(float incVal)
  {
   MapLimits lim(m_mapType);
   float value = 0;
   if (GetValue(value))
   {   
    value+=incVal;
    if (value < lim.getLoLimit())
     value = lim.getLoLimit();
    else if  (value > lim.getHiLimit())
     value = lim.getHiLimit();
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
      m_onChar(nChar, this);
    }
    else if (nChar==VK_RIGHT)
    {
     CString str;
     GetWindowText(str);
     if (nStartChar >= str.GetLength())
      m_onChar(nChar, this);
    }
    else
     m_onChar(nChar, this);
   }
   else if (nChar==VK_OEM_6)
    IncrementEdit(0.5f);
   else if (nChar==VK_OEM_5)
    IncrementEdit(-0.5f);

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
  EventHandler2 m_onChar;
  EventHandler1 m_onKillFocus;
  DECLARE_MESSAGE_MAP()
 public:
  CEditExCustomKeys(const EventHandler2& onChar, const EventHandler1& onKillFocus, int mapType) 
  : CEditEx(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED), m_onChar(onChar), m_onKillFocus(onKillFocus),
  m_ln(NULL), m_rn(NULL), m_un(NULL), m_dn(NULL), m_error(false), m_mapType(mapType) {}
  CEditExCustomKeys* m_ln, *m_rn, *m_un, *m_dn; //neighbours
  bool m_error;
  int m_mapType;
};

BEGIN_MESSAGE_MAP(CEditExCustomKeys, CEditEx)
 ON_WM_CHAR()
 ON_WM_KEYDOWN()
 ON_WM_GETDLGCODE()
 ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

const UINT CGridModeEditorDlg::IDD = IDD_GRID_MODE_EDITOR;

/////////////////////////////////////////////////////////////////////////////
// CGridModeEditorDlg dialog

BEGIN_MESSAGE_MAP(CGridModeEditorDlg, Super)
 ON_WM_CLOSE()
 ON_UPDATE_COMMAND_UI_RANGE(IDC_GME_WRK_START, IDC_GME_WRK_START+16*16, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_GME_IDL_0, IDC_GME_IDL_15, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_GME_STR_0, IDC_GME_STR_15, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_GME_TMP_0, IDC_GME_TMP_15, OnUpdateControls)
 ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CGridModeEditorDlg::CGridModeEditorDlg(CWnd* pParent /*=NULL*/)
: Super(CGridModeEditorDlg::IDD, pParent)
, mp_startMap(NULL)
, mp_idleMap(NULL)
, mp_workMap(NULL)
, mp_tempMap(NULL)
, m_redBrush(itemErrColor)
{
 int i,j;
 for(i = 0; i < 16; ++i)
 {
  for(j = 0; j < 16; ++j)
  {
   m_wrk_grid[i][j].reset(new CEditExCustomKeys(fastdelegate::MakeDelegate(this, &CGridModeEditorDlg::OnEditChar), 
                                                fastdelegate::MakeDelegate(this, &CGridModeEditorDlg::OnEditKill), TYPE_MAP_DA_WORK));
   m_wrk_grid[i][j]->SetDecimalPlaces(2);
  }
 }

 for(i = 0; i < 16; ++i)
 {
  m_idl_grid[i].reset(new CEditExCustomKeys(fastdelegate::MakeDelegate(this, &CGridModeEditorDlg::OnEditChar),
                                            fastdelegate::MakeDelegate(this, &CGridModeEditorDlg::OnEditKill), TYPE_MAP_DA_IDLE));
  m_idl_grid[i]->SetDecimalPlaces(2);
  m_str_grid[i].reset(new CEditExCustomKeys(fastdelegate::MakeDelegate(this, &CGridModeEditorDlg::OnEditChar),
                                            fastdelegate::MakeDelegate(this, &CGridModeEditorDlg::OnEditKill), TYPE_MAP_DA_START));
  m_str_grid[i]->SetDecimalPlaces(2);
  m_tmp_grid[i].reset(new CEditExCustomKeys(fastdelegate::MakeDelegate(this, &CGridModeEditorDlg::OnEditChar),
                                            fastdelegate::MakeDelegate(this, &CGridModeEditorDlg::OnEditKill), TYPE_MAP_DA_TEMP_CORR));
  m_tmp_grid[i]->SetDecimalPlaces(2);
 }

 //set neighbours :-)
 for(i = 0; i < 16; ++i)
 {
  for(j = 0; j < 16; ++j)
  {
   m_wrk_grid[i][j]->m_un = i > 0 ? m_wrk_grid[i-1][j].get() : NULL;
   m_wrk_grid[i][j]->m_dn = i < 15 ? m_wrk_grid[i+1][j].get() : m_idl_grid[j].get();
   m_wrk_grid[i][j]->m_ln = j > 0 ? m_wrk_grid[i][j-1].get() : NULL;
   m_wrk_grid[i][j]->m_rn = j < 15 ? m_wrk_grid[i][j+1].get() : NULL;
  }
 }
 
 for(j = 0; j < 16; ++j)
 {
  m_idl_grid[j]->m_un = m_wrk_grid[15][j].get();
  m_idl_grid[j]->m_dn = m_tmp_grid[j].get();
  m_idl_grid[j]->m_ln = j > 0 ? m_idl_grid[j-1].get() : NULL;
  m_idl_grid[j]->m_rn = j < 15 ? m_idl_grid[j+1].get() : NULL;

  m_tmp_grid[j]->m_un = m_idl_grid[j].get();
  m_tmp_grid[j]->m_dn = m_str_grid[j].get();
  m_tmp_grid[j]->m_ln = j > 0 ? m_tmp_grid[j-1].get() : NULL;
  m_tmp_grid[j]->m_rn = j < 15 ? m_tmp_grid[j+1].get() : NULL;

  m_str_grid[j]->m_un = m_tmp_grid[j].get();
  m_str_grid[j]->m_ln = j > 0 ? m_str_grid[j-1].get() : NULL;
  m_str_grid[j]->m_rn = j < 15 ? m_str_grid[j+1].get() : NULL;
 } 
}

CGridModeEditorDlg::~CGridModeEditorDlg()
{
 //empty
}

void CGridModeEditorDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 if (::IsWindow(m_wrk_grid[0][0]->m_hWnd))
 {
  for(int i = 0; i < 16; ++i)
   for(int j = 0; j < 16; ++j)
    m_wrk_grid[i][j]->DDX_Value(pDX, IDC_GME_WRK_START+i*16+j, mp_workMap[i*16+j]);
 }

 for(int i = 0; i < 16; ++i)
 {
  DDX_Control(pDX, IDC_GME_IDL_0 + i, *m_idl_grid[i]);
  m_idl_grid[i]->DDX_Value(pDX, IDC_GME_IDL_0 + i, mp_idleMap[i]);
  DDX_Control(pDX, IDC_GME_STR_0 + i, *m_str_grid[i]);
  m_str_grid[i]->DDX_Value(pDX, IDC_GME_STR_0 + i, mp_startMap[i]);
  DDX_Control(pDX, IDC_GME_TMP_0 + i, *m_tmp_grid[i]);
  m_tmp_grid[i]->DDX_Value(pDX, IDC_GME_TMP_0 + i, mp_tempMap[i]);
 }
}

/////////////////////////////////////////////////////////////////////////////
// CDwellCalcDlg message handlers

BOOL CGridModeEditorDlg::OnInitDialog()
{
 Super::OnInitDialog();

 LOGFONT LogFont;
 memset(&LogFont, 0x00, sizeof(LogFont));
 _tcsncpy(LogFont.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE);
 LogFont.lfHeight = 8;

 CFont* pFont = new CFont;
 pFont->CreateFontIndirect(&LogFont);

 CRect rc;
 GetDlgItem(IDC_GME_WRK_FRAME)->GetWindowRect(&rc);
 int space  = 2;
 int width  = MathHelpers::Round(((float)(rc.right - rc.left) - 16.0f*space) / 16.0f);
 int height = MathHelpers::Round(((float)(rc.bottom - rc.top) - 16.0f*space) / 16.0f);
 ScreenToClient(&rc);

 //create work map edit boxes
 size_t i, j;
 for(i = 0; i < 16; ++i)
  for(j = 0; j < 16; ++j)
  {
   CRect wrk_rect(j*(width+space), i*(height+space), j*(width+space) + width, i*(height+space) + height);
   wrk_rect.OffsetRect(rc.left, rc.top);
   m_wrk_grid[i][j]->Create(WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP, wrk_rect, this, IDC_GME_WRK_START+i*16+j);
   m_wrk_grid[i][j]->SetFont(pFont);   
  }

 //map labels
 for(i = 0; i < 16; ++i)
 {
  CRect lab_rect(rc.left - width, rc.top+(i*(height+space)), rc.left, rc.top + (i*(height+space) + height));
  CString cs; cs.Format("%d ", 16-i);
  m_wrk_map_labels[i].Create(cs.GetBuffer(0),WS_CHILD | WS_VISIBLE | SS_RIGHT, lab_rect, this);
  m_wrk_map_labels[i].SetFont(pFont);
 }

 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(this->m_hWnd, TYPE_MAP_GME_WND);

 SetIcon(::LoadIcon(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDI_GRAPH)), TRUE);

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CGridModeEditorDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool allowed = m_IsAllowed ? m_IsAllowed() : false;
 pCmdUI->Enable(allowed);
}

HBRUSH CGridModeEditorDlg::OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor)
{
 HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
 if (nCtlColor == CTLCOLOR_EDIT)
 {
  for(size_t i = 0; i < 16; ++i)
  {
   for(size_t j = 0; j < 16; ++j)
    if (pWnd->m_hWnd == m_wrk_grid[i][j]->m_hWnd && true==m_wrk_grid[i][j]->m_error)
    { 
     pDC->SetBkColor(itemErrColor);
     hbr = m_redBrush;
    }
   if ((pWnd->m_hWnd == m_idl_grid[i]->m_hWnd && true==m_idl_grid[i]->m_error)||
      (pWnd->m_hWnd == m_str_grid[i]->m_hWnd && true==m_str_grid[i]->m_error)||
      (pWnd->m_hWnd == m_tmp_grid[i]->m_hWnd && true==m_tmp_grid[i]->m_error))
   { 
    pDC->SetBkColor(itemErrColor);
    hbr = m_redBrush;
   }
  }
 }
 return hbr;
}

void CGridModeEditorDlg::OnClose()
{
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(this->m_hWnd, TYPE_MAP_GME_WND);
 Super::OnClose(); //close window
}

void CGridModeEditorDlg::BindMaps(float* pStart, float* pIdle, float* pWork, float* pTemp)
{
 mp_startMap = pStart;
 mp_idleMap = pIdle;
 mp_workMap = pWork;
 mp_tempMap = pTemp;
}

void CGridModeEditorDlg::UpdateView(void)
{
 if (::IsWindow(this->m_hWnd))
  UpdateData(FALSE);
}

void CGridModeEditorDlg::setIsAllowed(EventResult IsFunction)
{m_IsAllowed = IsFunction;}

void CGridModeEditorDlg::setOnMapChanged(EventWithCode OnFunction)
{m_OnMapChanged = OnFunction;}

void CGridModeEditorDlg::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CGridModeEditorDlg::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CGridModeEditorDlg::OnEditChar(UINT nChar, CEditExCustomKeys* pSender)
{
 if (NULL==pSender)
  return;
 if (!_ValidateItem(pSender))
  return;
 switch(nChar)
 {
  case VK_UP:
   if (pSender->m_un)
    pSender->m_un->SetFocus();
   break;
  case VK_DOWN:
   if (pSender->m_dn)
    pSender->m_dn->SetFocus();
   break;
  case VK_LEFT:
   if (pSender->m_ln)
    pSender->m_ln->SetFocus();
   break;
  case VK_RIGHT:
   if (pSender->m_rn)
    pSender->m_rn->SetFocus();
   break;
 }
 
 if (m_OnMapChanged)
 {
  UpdateData();
  m_OnMapChanged(pSender->m_mapType);
 }
}

void CGridModeEditorDlg::OnEditKill(CEditExCustomKeys* pSender)
{
 if (!_ValidateItem(pSender))
 {
  pSender->SetFocus();
  return; //validation failed
 }
 if (m_OnMapChanged)
 {
  UpdateData();
  m_OnMapChanged(pSender->m_mapType);
 }
}

bool CGridModeEditorDlg::_ValidateItem(CEditExCustomKeys* pItem)
{
 float value = 0;
 bool convres = pItem->GetValue(value); 
 MapLimits lim(pItem->m_mapType);
 pItem->m_error = !(convres && value >= lim.getLoLimit() && value <= lim.getHiLimit());
 pItem->Invalidate();
 return !pItem->m_error;
}
 