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
#include "common/MathHelpers.h"
#include "GridModeEditorDlg.h"

const UINT CGridModeEditorDlg::IDD = IDD_GRID_MODE_EDITOR;

/////////////////////////////////////////////////////////////////////////////
// CDwellCalcDlg dialog

BEGIN_MESSAGE_MAP(CGridModeEditorDlg, Super)
END_MESSAGE_MAP()

CGridModeEditorDlg::CGridModeEditorDlg(CWnd* pParent /*=NULL*/)
: Super(CGridModeEditorDlg::IDD, pParent)
{
 //empty
}

CGridModeEditorDlg::~CGridModeEditorDlg()
{
 //empty
}

void CGridModeEditorDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

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

 size_t i, j;
 for(j = 0; j < 16; ++j)
  for(i = 0; i < 16; ++i)
  {
   CRect wrk_rect(i*(width+space), j*(height+space), i*(width+space) + width, j*(height+space) + height);
   wrk_rect.OffsetRect(rc.left, rc.top);
   m_wrk_grid[i][j].Create(WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP, wrk_rect, this, IDC_GME_WRK_START+j*16+i);
   m_wrk_grid[i][j].SetFont(pFont);   
  }

 //map labels
 for(i = 0; i < 16; ++i)
 {
  CRect lab_rect(rc.left - width, rc.top+(i*(height+space)), rc.left, rc.top + (i*(height+space) + height));
  CString cs; cs.Format("%d ", 16-i);
  m_wrk_map_labels[i].Create(cs.GetBuffer(0),WS_CHILD | WS_VISIBLE | SS_RIGHT, lab_rect, this);
  m_wrk_map_labels[i].SetFont(pFont);
 }

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}
