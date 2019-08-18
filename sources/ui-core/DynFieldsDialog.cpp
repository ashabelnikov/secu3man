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

/** \file DynFieldsDialog.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "DynFieldsDialog.h"
#include "common/DPIAware.h"
#include "common/GDIHelpers.h"
#include "ui-core/WndScroller.h"

int CDynFieldsDialog::ItemData::idCntr = IDC_DYNFLDDLG_START;
int CDynFieldsDialog::ItemData::groupIdx = 0;

BEGIN_MESSAGE_MAP(CDynFieldsDialog, Super)
 ON_WM_PAINT()
 ON_WM_SIZE()
 ON_WM_GETMINMAXINFO()
 ON_CONTROL_RANGE(EN_CHANGE, IDC_DYNFLDDLG_START, IDC_DYNFLDDLG_END, OnChangeData)
END_MESSAGE_MAP()

void CDynFieldsDialog::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 for(std::list<ItemData>::iterator it = m_fl.begin(); it != m_fl.end(); ++it)
  it->DDX_Value(pDX);
}

CDynFieldsDialog::CDynFieldsDialog(CWnd* pParentWnd, const _TSTRING& caption, int height)
: Super(IDD_DYNFLDDLG, pParentWnd)
, m_caption(caption)
, m_initialized(false)
, mp_scr(new CWndScroller)
, m_height(height)
{
 CDynFieldsDialog::ItemData::idCntr = IDC_DYNFLDDLG_START; //reset counter
 CDynFieldsDialog::ItemData::groupIdx = 0; //reset group index
}

CDynFieldsDialog::~CDynFieldsDialog()
{
 //empty
}

BOOL CDynFieldsDialog::OnInitDialog()
{
 for(std::list<ItemData>::iterator it = m_fl.begin(); it != m_fl.end(); ++it)
  it->Init();

 for(std::list<ItemData>::iterator it = m_fl.begin(); it != m_fl.end(); ++it)
  it->Create(this);

 Super::OnInitDialog();

 SetWindowText(m_caption.c_str());
 
 //adjust vertical size of dialog
 CRect lr;
 m_fl.back().p_edit->GetWindowRect(&lr);
 DPIAware dpi;
 ScreenToClient(&lr);
 CRect rcok = GDIHelpers::GetChildWndRect(GetDlgItem(IDOK));
 rcok.MoveToY(lr.bottom + dpi.ScaleY(3));
 GetDlgItem(IDOK)->MoveWindow(rcok);

 m_contentHeight = lr.bottom + dpi.ScaleY(3) + rcok.Height()+ dpi.ScaleY(3);

 //Resize vertical size of dialog to the cvalue specified by user
 CRect dr;
 GetWindowRect(&dr);

 CSize ncs = GDIHelpers::GetNonClientSize(this);
 if ((ncs.cy + m_contentHeight) >  dpi.ScaleY(m_height))
 { //content doesn't fit into the specified height, so scroller will be used
  dr.bottom = dr.top + dpi.ScaleY(m_height);
  MoveWindow(&dr);
 }
 else
 { //scroller is not needed
  dr.bottom = dr.top + (ncs.cy + m_contentHeight);
  MoveWindow(&dr);
 }

 //remember original size of window
 CRect rc;
 GetWindowRect(&rc);
 m_createSize = rc.Size();

 m_initialized = true;

 //initialize window scroller
 mp_scr->Init(this);

 GetClientRect(&rc);
 _UpdateScrlViewSize(rc.Width(), rc.Height());

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CDynFieldsDialog::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 if (m_initialized)
 {
  _UpdateScrlViewSize(cx, cy);
 }
}

void CDynFieldsDialog::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CDynFieldsDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
 if (m_initialized)
 {
  lpMMI->ptMinTrackSize.x = m_createSize.cx;
  lpMMI->ptMinTrackSize.y = m_createSize.cy;

  lpMMI->ptMaxTrackSize.x = m_createSize.cx;
  DPIAware dpi;
  CSize ncs = GDIHelpers::GetNonClientSize(this);
  lpMMI->ptMaxTrackSize.y = (LONG)(m_contentHeight + ncs.cy + dpi.ScaleY(10));
 }
}

void CDynFieldsDialog::_UpdateScrlViewSize(int cx, int cy)
{
 DPIAware dpi;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, dpi.ScaleY(m_contentHeight));
}

bool CDynFieldsDialog::AppendItem(const _TSTRING& caption, const _TSTRING& unit, int vMin, int vMax, int vStp, int decPls, int* p_value)
{
 ItemData id;
 id.caption = caption.c_str();
 id.unit = unit.c_str();
 id.vMin = (float)vMin;
 id.vMax = (float)vMax;
 id.vStp = (float)vStp;
 id.decPls = decPls;
 id.fltVal = NULL;
 id.intVal = p_value;
 m_fl.push_back(id);
 return true;
}

bool CDynFieldsDialog::AppendItem(const _TSTRING& caption, const _TSTRING& unit, float vMin, float vMax, float vStp, int decPls, float* p_value)
{
 ItemData id;
 id.caption = caption.c_str();
 id.unit = unit.c_str();
 id.vMin = vMin;
 id.vMax = vMax;
 id.vStp = vStp;
 id.decPls = decPls;
 id.fltVal = p_value;
 id.intVal = NULL;
 m_fl.push_back(id);
 return true;
}

void CDynFieldsDialog::OnOK()
{ 
 bool valid = UpdateData(true);
 if (!valid)
  return;

 for(std::list<ItemData>::iterator it = m_fl.begin(); it != m_fl.end(); ++it)
  it->Apply();

 Super::OnOK();
}

void CDynFieldsDialog::OnChangeData(UINT nID)
{
 for(std::list<ItemData>::iterator it = m_fl.begin(); it != m_fl.end(); ++it)
 {
  if (nID == it->p_edit->GetDlgCtrlID())
   UpdateData();
 }
}

