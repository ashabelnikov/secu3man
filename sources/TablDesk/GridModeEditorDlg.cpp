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

#include "stdafx.h"
#include "resource.h"
#include "common/Dll.h"
#include "common/MathHelpers.h"
#include "GridModeEditorDlg.h"
#include "io-core/SECU3IO.h"
#include "MapIds.h"
#include "ui-core/EditEx.h"
#include "ui-core/fnt_helpers.h"

static const COLORREF itemErrColor = RGB(255,120,120);
static const float wrkMinVal = -15.0f;
static const float wrkMaxVal = 55.0f;
static const COLORREF gradColor[16] ={0xA88CD5, 0xD26EDC, 0xC38CBE, 0xCB9491, 0xC8AA85, 0xCDC38F, 0xD3D48F, 0xB2D573,
                                      0x87DCA3, 0x87e4A3, 0x99E9A3, 0x5DF3DF, 0x3ACDE9, 0x78AFE9, 0x5D94EB, 0x555AFD};

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
     return wrkMinVal;
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
     return wrkMaxVal;
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
 ON_WM_PAINT()
 ON_WM_CLOSE()
 ON_UPDATE_COMMAND_UI_RANGE(IDC_GME_WRK_START, IDC_GME_WRK_START+16*16, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_GME_IDL_0, IDC_GME_IDL_15, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_GME_STR_0, IDC_GME_STR_15, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDC_GME_TMP_0, IDC_GME_TMP_15, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AA_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_WM_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_OC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_TC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_KC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IM_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AC_CAPTION, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AA_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AAE_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_WM_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_WMP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_OC_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_OCP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_TC_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_TCP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_KC_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_KCP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IM_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IMP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IC_VALUE, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_ICP_TEXT, OnUpdateAAControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_AC_VALUE, OnUpdateAAControls)   //air temper. corr.
 ON_UPDATE_COMMAND_UI(IDC_GME_AA_UNIT, OnUpdateAAControls)
 ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CGridModeEditorDlg::CGridModeEditorDlg(CWnd* pParent /*=NULL*/)
: Super(CGridModeEditorDlg::IDD, pParent)
, m_wpiPen(PS_SOLID, 3, RGB(255, 64, 64))
, mp_startMap(NULL)
, mp_idleMap(NULL)
, mp_workMap(NULL)
, mp_tempMap(NULL)
, mp_rpmGrid(NULL)
, m_redBrush(itemErrColor)
, m_closing_wnd(false)
, m_en_aa_indication(false)
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

 _ResetUseFlags();

 //Create gradient brushes
 for(i = 0; i < 16; ++i)
  m_gradBrush[i].CreateSolidBrush(gradColor[i]);
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
    if (!m_wrk_grid[i][j]->m_error)
     m_wrk_grid[i][j]->DDX_Value(pDX, IDC_GME_WRK_START+i*16+j, mp_workMap[i*16+j]);
 }

 for(int i = 0; i < 16; ++i)
 {
  DDX_Control(pDX, IDC_GME_IDL_0 + i, *m_idl_grid[i]);
  if (!m_idl_grid[i]->m_error)
   m_idl_grid[i]->DDX_Value(pDX, IDC_GME_IDL_0 + i, mp_idleMap[i]);
  DDX_Control(pDX, IDC_GME_STR_0 + i, *m_str_grid[i]);
  if (!m_str_grid[i]->m_error)
   m_str_grid[i]->DDX_Value(pDX, IDC_GME_STR_0 + i, mp_startMap[i]);
  DDX_Control(pDX, IDC_GME_TMP_0 + i, *m_tmp_grid[i]);
  if (!m_tmp_grid[i]->m_error)
   m_tmp_grid[i]->DDX_Value(pDX, IDC_GME_TMP_0 + i, mp_tempMap[i]);
 }

 //RPM grid
 for(int i = 0; i < 16; ++i)
 {
  int value = MathHelpers::Round(mp_rpmGrid[i]);
  DDX_Text(pDX, IDC_GME_WRKL_0 + i, value);
 }

 //Advance angle indication controls
 DDX_Control(pDX, IDC_GME_AA_VALUE, m_aa_value);
 DDX_Control(pDX, IDC_GME_WM_VALUE, m_wm_value);
 DDX_Control(pDX, IDC_GME_OC_VALUE, m_oc_value);
 DDX_Control(pDX, IDC_GME_TC_VALUE, m_tc_value);
 DDX_Control(pDX, IDC_GME_KC_VALUE, m_kc_value);
 DDX_Control(pDX, IDC_GME_IM_VALUE, m_im_value);
 DDX_Control(pDX, IDC_GME_IC_VALUE, m_ic_value);
 DDX_Control(pDX, IDC_GME_AC_VALUE, m_ac_value);
}

/////////////////////////////////////////////////////////////////////////////
// CDwellCalcDlg message handlers

BOOL CGridModeEditorDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //Set bold font to advance angle indication controls
 CloneWndFont(&m_aa_value, &m_fieldFont, 11, true);
 m_aa_value.SetFont(&m_fieldFont);
 m_wm_value.SetFont(&m_fieldFont);
 m_oc_value.SetFont(&m_fieldFont);
 m_tc_value.SetFont(&m_fieldFont);
 m_kc_value.SetFont(&m_fieldFont);
 m_im_value.SetFont(&m_fieldFont);
 m_ic_value.SetFont(&m_fieldFont);
 m_ac_value.SetFont(&m_fieldFont);

 LOGFONT LogFont;
 memset(&LogFont, 0x00, sizeof(LogFont));
 _tcsncpy(LogFont.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE);
 LogFont.lfHeight = 8;
 m_font.CreateFontIndirect(&LogFont);

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
   m_wrk_grid[i][j]->SetFont(&m_font);
  }

 //set position of idle, temperature and start map edits
 CRect rc_idle,rc_strt,rc_temp;
 m_idl_grid[0]->GetWindowRect(rc_idle); ScreenToClient(rc_idle);
 m_str_grid[0]->GetWindowRect(rc_strt); ScreenToClient(rc_strt);
 m_tmp_grid[0]->GetWindowRect(rc_temp); ScreenToClient(rc_temp);
 for(j = 0; j < 16; ++j)
 {
  m_idl_grid[j]->SetWindowPos(NULL, rc_idle.left + (j*(width+space)), rc_idle.top, width, height, SWP_NOZORDER);
  m_str_grid[j]->SetWindowPos(NULL, rc_strt.left + (j*(width+space)), rc_strt.top, width, height, SWP_NOZORDER);
  m_tmp_grid[j]->SetWindowPos(NULL, rc_temp.left + (j*(width+space)), rc_temp.top, width, height, SWP_NOZORDER);
 }

 //map labels
 for(i = 0; i < 16; ++i)
 {
  CRect lab_rect(rc.left - width, rc.top+(i*(height+space)), rc.left, rc.top + (i*(height+space) + height));
  CString cs; cs.Format(_T("%d "), 16-i);
  m_wrk_map_labels[i].Create(cs.GetBuffer(0),WS_CHILD | WS_VISIBLE | SS_RIGHT, lab_rect, this);
  m_wrk_map_labels[i].SetFont(&m_font);
 }

 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(this->m_hWnd, TYPE_MAP_GME_WND);

 SetIcon(::LoadIcon(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDI_GRAPH)), TRUE);

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGridModeEditorDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 bool allowed = m_IsAllowed ? m_IsAllowed() : false;
 if (!allowed)
  m_wrk_grid[15][15]->SetFocus(); //hack

 pCmdUI->Enable(allowed);
}

void CGridModeEditorDlg::OnUpdateAAControls(CCmdUI* pCmdUI)
{
 bool allowed = m_IsAllowed ? m_IsAllowed() : false;
 bool flag = true;

 if (pCmdUI->m_nID == IDC_GME_WM_VALUE) flag = m_curDV.work_use;
 else if (pCmdUI->m_nID == IDC_GME_OC_VALUE) flag = m_curDV.octan_use;
 else if (pCmdUI->m_nID == IDC_GME_TC_VALUE) flag = m_curDV.temp_use;
 else if (pCmdUI->m_nID == IDC_GME_KC_VALUE) flag = m_curDV.knkret_use;
 else if (pCmdUI->m_nID == IDC_GME_IM_VALUE) flag = m_curDV.idle_use;
 else if (pCmdUI->m_nID == IDC_GME_IC_VALUE) flag = m_curDV.idlreg_use;
 else if (pCmdUI->m_nID == IDC_GME_AC_VALUE) flag = m_curDV.airt_use;

 pCmdUI->Enable(allowed && m_en_aa_indication && flag);
}

int CGridModeEditorDlg::_CalcGradIndex(float value)
{
 int index = MathHelpers::Round((value - (wrkMinVal)) / ((wrkMaxVal - wrkMinVal)/16.0f));
 if (index < 0) index = 0;
 if (index > 15) index = 15;
 return index;
}

HBRUSH CGridModeEditorDlg::OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor)
{
 HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
 if (nCtlColor == CTLCOLOR_EDIT)
 {
  for(size_t i = 0; i < 16; ++i)
  {
   //work map
   for(size_t j = 0; j < 16; ++j)
   {
    if (pWnd->m_hWnd == m_wrk_grid[i][j]->m_hWnd)
    {
     if (true==m_wrk_grid[i][j]->m_error)
     {//use error color
      pDC->SetBkColor(itemErrColor);
      hbr = m_redBrush;
     }
     else
     {//use gradient colors
      int index = _CalcGradIndex(mp_workMap[(i*16)+j]);
      pDC->SetBkColor(gradColor[index]);
      hbr = m_gradBrush[index];
     }
    }
   }
   //Idle map
   if (pWnd->m_hWnd == m_idl_grid[i]->m_hWnd)
   {
    if (true==m_idl_grid[i]->m_error)
    { //error
     pDC->SetBkColor(itemErrColor);
     hbr = m_redBrush;
    }
    else
    {//gradient
     int index = _CalcGradIndex(mp_idleMap[i]);
     pDC->SetBkColor(gradColor[index]);
     hbr = m_gradBrush[index];
    }
   }
   //Start map
   if (pWnd->m_hWnd == m_str_grid[i]->m_hWnd)
   {
    if (true==m_str_grid[i]->m_error)
    {//error
     pDC->SetBkColor(itemErrColor);
     hbr = m_redBrush;
    }
    else
    {//gradient
     int index = _CalcGradIndex(mp_startMap[i]);
     pDC->SetBkColor(gradColor[index]);
     hbr = m_gradBrush[index];
    }
   }
   //Coolant temperature map
   if (pWnd->m_hWnd == m_tmp_grid[i]->m_hWnd)
   {
    if (true==m_tmp_grid[i]->m_error)
    {//error
     pDC->SetBkColor(itemErrColor);
     hbr = m_redBrush;
    }
    else
    {//gradient
     int index = _CalcGradIndex(mp_tempMap[i]);
     pDC->SetBkColor(gradColor[index]);
     hbr = m_gradBrush[index];
    }
   }
  }
 }
 return hbr;
}

void CGridModeEditorDlg::_2DLookup(float x, const float* grid, std::vector<int>& pt)
{
 pt.clear();
 if (x <= grid[0]) { pt.push_back(0); return; }
 if (x >= grid[15]) { pt.push_back(15); return; }

 for(int i = 0; i < 16; ++i)
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

void CGridModeEditorDlg::_DrawRect(std::auto_ptr<CEditExCustomKeys>& wnd, CDC& dc)
{
 CRect rc;
 wnd->GetWindowRect(rc);
 ScreenToClient(rc);
 rc.InflateRect(2,2,2,2);
 dc.Rectangle(rc);
}

void CGridModeEditorDlg::_ResetUseFlags(void)
{
 m_curDV.strt_use = m_curDV.work_use = m_curDV.idle_use = m_curDV.temp_use =
 m_curDV.airt_use = m_curDV.idlreg_use = m_curDV.octan_use = m_curDV.knkret_use = false;
}

void CGridModeEditorDlg::OnPaint()
{
 Super::OnPaint();
 CClientDC dc(this);
 dc.SelectObject(&m_wpiPen);

 std::vector<int> pt;
 if (m_curDV.strt_use && m_curDV.work_use && m_curDV.idle_use && m_curDV.temp_use)
  return;

 if (m_curDV.strt_use)
 {
  _2DLookup((float)m_curDV.rpm, SECU3IO::start_map_rpm_slots, pt);
  for(size_t i = 0; i < pt.size(); ++i)
   _DrawRect(m_str_grid[pt[i]], dc);
 }

 if (m_curDV.work_use && m_curDV.air_flow)
 {
  _2DLookup((float)m_curDV.rpm, mp_rpmGrid, pt);
  for(size_t i = 0; i < pt.size(); ++i)
   _DrawRect(m_wrk_grid[16-m_curDV.air_flow][pt[i]], dc);
 }

 if (m_curDV.idle_use)
 {
  _2DLookup((float)m_curDV.rpm, mp_rpmGrid, pt);
  for(size_t i = 0; i < pt.size(); ++i)
   _DrawRect(m_idl_grid[pt[i]], dc);
 }

 if (m_curDV.temp_use)
 {
  _2DLookup(m_curDV.temp, SECU3IO::temp_map_tmp_slots, pt);
  for(size_t i = 0; i < pt.size(); ++i)
   _DrawRect(m_tmp_grid[pt[i]], dc);
 }
}

void CGridModeEditorDlg::OnClose()
{
 m_closing_wnd = true; //set flag indicating that window is being closing
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

void CGridModeEditorDlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
}

void CGridModeEditorDlg::UpdateView(void)
{
 if (::IsWindow(this->m_hWnd))
  UpdateData(FALSE); //update dialog controls
}

void CGridModeEditorDlg::EnableAdvanceAngleIndication(bool i_enable)
{
 m_en_aa_indication = i_enable;
 bool is_wnd = ::IsWindow(this->m_hWnd);

 if (!i_enable)
 {
  _ResetUseFlags();
  if (is_wnd)
   Invalidate();
 }

 if (is_wnd)
  UpdateDialogControls(this, true);
}

void CGridModeEditorDlg::SetDynamicValues(const DynVal& dv)
{
 CString str;
 str.Format(_T("%0.2f"), dv.adv_ang), m_aa_value.SetWindowText(str);
 str.Format(_T("%0.2f"), dv.knock_retard), m_kc_value.SetWindowText(str);
 str.Format(_T("%0.2f"), dv.idle_aalt), m_im_value.SetWindowText(str);
 str.Format(_T("%0.2f"), dv.work_aalt), m_wm_value.SetWindowText(str);
 str.Format(_T("%0.2f"), dv.temp_aalt), m_tc_value.SetWindowText(str);
 str.Format(_T("%0.2f"), dv.airt_aalt), m_ac_value.SetWindowText(str);
 str.Format(_T("%0.2f"), dv.idlreg_aac), m_ic_value.SetWindowText(str);
 str.Format(_T("%0.2f"), dv.octan_aac), m_oc_value.SetWindowText(str);
 m_curDV = dv;
 UpdateDialogControls(this, true);  //todo: check it for perfomance issues
 Invalidate(); //shit
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
 if (m_closing_wnd)
  return; //do not perform validation and update of values if window is being closed

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

