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

/** \file LogConverterDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "LogConverterDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/FileDialogEx.h"
#include "io-core/LogReader.h"
#include "io-core/LogWriter.h"
#include "ui-core/MsgBox.h"
#include "io-core/SECU3IO.h"
#include "common/UnicodeSupport.h"

CLogConverterDlg::CLogConverterDlg(CWnd* pParent, LogWriter *ip_logWriter, int fffConst)
: Super(IDD_LOGCONV_DIALOG, pParent)
, m_csvsep_symb(';')
, mp_logWriter(ip_logWriter)
, m_fffConst(fffConst)
{
 //empty
}

void CLogConverterDlg::DoDataExchange(CDataExchange* pDX)
{
 DDX_Control(pDX, IDC_LOGCONV_INPUT_EDIT, m_inp_edit);
 DDX_Control(pDX, IDC_LOGCONV_INPUT_FILE_BTN, m_inp_file_btn);
 DDX_Control(pDX, IDC_LOGCONV_OUTPUT_EDIT, m_out_edit);
 DDX_Control(pDX, IDC_LOGCONV_OUTPUT_FILE_BTN, m_out_file_btn);
 DDX_Control(pDX, IDC_LOGCONV_FORMAT_CHECK, m_fmt_check);
 DDX_Control(pDX, IDC_LOGCONV_TITLE_CHECK, m_tit_check);
 DDX_Control(pDX, IDC_LOGCONV_DECPT_COMBO, m_decpt_combo);
 DDX_Control(pDX, IDC_LOGCONV_CSVSEP_COMBO, m_csvsep_combo);
 DDX_Control(pDX, IDC_LOGCONV_DECPT_CAPTION, m_decpt_caption);
 DDX_Control(pDX, IDC_LOGCONV_CSVSEP_CAPTION, m_csvsep_caption);
 DDX_Control(pDX, IDC_LOGCONV_PROGRESS_TEXT, m_progress_text);
 DDX_Control(pDX, IDC_LOGCONV_START_BTN, m_start_btn);

 Super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLogConverterDlg, Super)
 ON_BN_CLICKED(IDC_LOGCONV_INPUT_FILE_BTN, OnSelectInputFile)
 ON_BN_CLICKED(IDC_LOGCONV_OUTPUT_FILE_BTN, OnSelectOutputFile)
 ON_BN_CLICKED(IDC_LOGCONV_FORMAT_CHECK, OnLogFormatCheck)
 ON_BN_CLICKED(IDC_LOGCONV_TITLE_CHECK, OnLogTitleCheck)
 ON_BN_CLICKED(IDC_LOGCONV_START_BTN, OnStartConversion)
 ON_CBN_SELENDOK(IDC_LOGCONV_DECPT_COMBO, OnSelendokDecPt)
 ON_CBN_SELENDOK(IDC_LOGCONV_CSVSEP_COMBO, OnSelendokCsvSep)
END_MESSAGE_MAP()

namespace {
 const TCHAR *tdecpt_symb = _T(".,");
 const TCHAR *tcsvsep_symb = _T(";%,+|*=!#$&");
 char decpt_symb[16];
 char csvsep_symb[16];
}

BOOL CLogConverterDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_start_btn.EnableWindow(false);

 CharToOem(tdecpt_symb, decpt_symb);
 CharToOem(tcsvsep_symb, csvsep_symb);

 const TCHAR* p = tdecpt_symb;
 while(*p!=0)
 {
  TCHAR str[2] = {*p, 0};
  m_decpt_combo.AddString(str); 
  p++;
 }
 p = tcsvsep_symb;
 while(*p!=0)
 {
  TCHAR str[2] = {*p, 0};
  m_csvsep_combo.AddString(str); 
  p++;
 }

 for(int i = 0; i < m_csvsep_combo.GetCount(); ++i)
 {
  if (m_csvsep_symb == csvsep_symb[i])
  {
   m_csvsep_combo.SetCurSel(i);
   break;
  }
 }

 TCHAR decPt = _TDECIMAL_POINT(localeconv())[0];
 for(int i = 0; i < m_decpt_combo.GetCount(); ++i)
 {
  if (decPt==tdecpt_symb[i])
  {
   m_decpt_combo.SetCurSel(i);
   break;
  }
 }

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_inp_file_btn, MLL::GetString(IDS_LOGCONV_INPUT_FILE_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_inp_edit, MLL::GetString(IDS_LOGCONV_INPUT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_out_edit, MLL::GetString(IDS_LOGCONV_OUTPUT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_out_file_btn, MLL::GetString(IDS_LOGCONV_OUTPUT_FILE_BTN_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fmt_check, MLL::GetString(IDS_LOGCONV_FORMAT_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tit_check, MLL::GetString(IDS_LOGCONV_TITLE_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_decpt_combo, MLL::GetString(IDS_LOGCONV_DECPT_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_csvsep_combo, MLL::GetString(IDS_LOGCONV_CSVSEP_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_start_btn, MLL::GetString(IDS_LOGCONV_START_BTN_TT)));
 mp_ttc->SetMaxTipWidth(150); //Enable text wrapping
 mp_ttc->ActivateToolTips(true); 

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CLogConverterDlg::OnSelectInputFile()
{
 HANDLE hFile = NULL;
 static TCHAR BASED_CODE szFilter[] = _T("CSV Files (*.csv)|*.csv|S3L Files (*.s3l)|*.s3l|All Files (*.*)|*.*||");
 CFileDialogEx open(TRUE, NULL, NULL, NULL, szFilter, NULL);
 if (open.DoModal() == IDOK)
 {
  CString fileName = open.GetPathName();
  m_inp_edit.SetWindowText(fileName);

  _EnableStartButton();
 }
 m_progress_text.SetWindowText(_T(""));
}

void CLogConverterDlg::OnSelectOutputFile()
{
 static TCHAR BASED_CODE szFilter[] = _T("CSV Files (*.csv)|*.csv|S3L Files (*.s3l)|*.s3l|All Files (*.*)|*.*||");
 CFileDialogEx save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("CSV");
 if (save.DoModal()==IDOK)
 {
  CString fileName = save.GetPathName();
  m_out_edit.SetWindowText(fileName);

  _TSTRING name = fileName;
  size_t pos = name.rfind(_T(".s3l"));
  if (pos!=_TSTRING::npos && pos == name.size()-4) //check file extension
  {//binary format (.s3l)
   m_fmt_check.SetCheck(BST_CHECKED);
   _EnableCsvControls(false);
  }

  pos = name.rfind(_T(".csv"));
  if (pos!=_TSTRING::npos && pos == name.size()-4) //check file extension
  {//text format (.csv)
   m_fmt_check.SetCheck(BST_UNCHECKED);
   _EnableCsvControls(true);
  }
  _EnableStartButton();
 }
 m_progress_text.SetWindowText(_T(""));
}

void CLogConverterDlg::OnLogFormatCheck()
{
 CString fileName;
 m_out_edit.GetWindowText(fileName);
 _TSTRING name = (LPCTSTR)fileName;

 if (m_fmt_check.GetCheck()==BST_CHECKED)
 {//binary format
  _EnableCsvControls(false);

  size_t pos = name.rfind(_T(".csv"));
  if (pos!=_TSTRING::npos && pos == name.size()-4)
  {
   name.resize(name.size()-4);
   name+=_T(".s3l");
   m_out_edit.SetWindowText(name.c_str());
  }
 }
 else
 {
  _EnableCsvControls(true);

  size_t pos = name.rfind(_T(".s3l"));
  if (pos!=_TSTRING::npos && pos == name.size()-4)
  {
   name.resize(name.size()-4);
   name+=_T(".csv");
   m_out_edit.SetWindowText(name.c_str());
  }
 }

 _EnableStartButton();
 m_progress_text.SetWindowText(_T(""));
}

void CLogConverterDlg::OnLogTitleCheck()
{
 m_progress_text.SetWindowText(_T(""));
}

void CLogConverterDlg::OnSelendokDecPt()
{
 _EnableStartButton();
 m_progress_text.SetWindowText(_T(""));
}

void CLogConverterDlg::OnSelendokCsvSep()
{
 _EnableStartButton();
 m_progress_text.SetWindowText(_T(""));
}

void CLogConverterDlg::OnStartConversion()
{
 char decpt = decpt_symb[m_decpt_combo.GetCurSel()];
 char csvsep = csvsep_symb[m_csvsep_combo.GetCurSel()];
 bool binfmt = m_fmt_check.GetCheck()==BST_CHECKED;
 bool wrtit = m_tit_check.GetCheck()==BST_CHECKED;

 CString inputFile;
 m_inp_edit.GetWindowText(inputFile);
 CString outputFile;
 m_out_edit.GetWindowText(outputFile);

 LogReader rd(true); //<--standalone mode
 rd.SetSeparatingSymbol(m_csvsep_symb);
 rd.SetFFFConst(m_fffConst); //<--not used in the stanalone mode, may be excluded
 LogReader::FileError error_id;
 bool result = rd.OpenFile((LPCTSTR)inputFile, error_id, mp_logWriter->GetFileHandle(), false); //do not fully check file
 if (false==result)
 {
  if (error_id==LogReader::FE_OPEN)
   SECUMessageBox(MLL::LoadString(IDS_LP_CANT_OPEN_FILE));
  else if (error_id==LogReader::FE_PENDING)
   SECUMessageBox(MLL::LoadString(IDS_LP_OPEN_PENDING));
  else if (error_id==LogReader::FE_FORMAT)
  {
   SECUMessageBox(MLL::LoadString(IDS_LP_INCORRECT_FILE_FORMAT));
  }
  else
   ASSERT(0);

  rd.CloseFile();
  return; 
 }

 LogWriter wr(true);             //<--standalone mode
 wr.SetSeparatingSymbol(csvsep); //set CSV separating symbol
 wr.SetWriteFields(wrtit);       //set write field flag
 wr.SetFormat(binfmt);           //set binary format flag
 wr.SetDecimalPoint(decpt);      //set decimal point symbol
 wr.SetTitleStr(rd.GetTitleStr());
 wr.SetFieldList(mp_logWriter->GetFieldList());
 wr.BeginLogging((LPCTSTR)outputFile, NULL);

 m_start_btn.EnableWindow(false);
 m_progress_text.SetWindowText(_T("0%"));
 unsigned long recnum = rd.GetCount();
 unsigned int progress = 0;
 
 if (recnum != 0)
 {
  SYSTEMTIME time; SECU3IO::SensorDat data; int marks;
  do 
  {
   if (rd.GetRecord(time, data, marks))
   {
    wr.SetRecord(time, data, marks);
   }
  
   unsigned int new_progress = (unsigned int)((((unsigned long long)rd.GetCurPos()) * 100) / recnum);
   if (new_progress != progress)
   {
    progress = new_progress;
    CString s;
    s.Format("%d%%", progress);
    m_progress_text.SetWindowText(s);
   }
  }
  while(rd.Next());
 }

 m_progress_text.SetWindowText(_T("100%"));
 _EnableStartButton();

 rd.CloseFile();
 wr.FlushFileBuffers();
 wr.EndLogging();
}

void CLogConverterDlg::_EnableStartButton(void)
{
 CString inputFile;
 m_inp_edit.GetWindowText(inputFile);
 CString outputFile;
 m_out_edit.GetWindowText(outputFile);

 bool files_ok = inputFile != outputFile && !inputFile.IsEmpty() && !outputFile.IsEmpty();

 bool cb_ok = (m_csvsep_combo.GetCurSel() != CB_ERR) && (m_decpt_combo.GetCurSel() != CB_ERR) && 
     (decpt_symb[m_decpt_combo.GetCurSel()] != csvsep_symb[m_csvsep_combo.GetCurSel()]);

 if (m_fmt_check.GetCheck()==BST_CHECKED)
  m_start_btn.EnableWindow(files_ok); //binary
 else
  m_start_btn.EnableWindow(files_ok && cb_ok); //text
}

void CLogConverterDlg::SetCSVSepSymbol(char csvsep)
{
 m_csvsep_symb = csvsep;
}

void CLogConverterDlg::_EnableCsvControls(bool en)
{
 m_tit_check.EnableWindow(en);
 m_decpt_combo.EnableWindow(en);
 m_csvsep_combo.EnableWindow(en);
 m_decpt_caption.EnableWindow(en);
 m_csvsep_caption.EnableWindow(en);
}