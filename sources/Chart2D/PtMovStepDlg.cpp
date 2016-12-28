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

/** \file PtMovStepDlg.cpp
 * \author Alexey A. Shabelnikov
 */

//--------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include "resource.h"
#pragma hdrstop

#include "PtMovStepDlg.h"
#include "TFloatUpDown.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPtMovStepDlg *PtMovStepDlg;

//---------------------------------------------------------------------------
__fastcall TPtMovStepDlg::TPtMovStepDlg(TComponent* Owner)
: TForm(Owner)
, m_value(0)
, m_valFmt("%0.2f")
{
 //empty
}

//---------------------------------------------------------------------------
void __fastcall TPtMovStepDlg::FormCreate(TObject *Sender)
{
 char string[1024 + 1];
 ::LoadString((HINSTANCE)::GetWindowLong(Handle, GWL_HINSTANCE), IDS_DLG_PTMOV_STEP_CAPT, string, 1024);
 Caption = string;
 
 AnsiString as;
 as.sprintf(m_valFmt.c_str(), m_value);
 EditStep->Text = as;
}

//---------------------------------------------------------------------------
void __fastcall TPtMovStepDlg::ButtonOkClick(TObject *Sender)
{
 ModalResult = mrOk;        
}

//---------------------------------------------------------------------------
void __fastcall TPtMovStepDlg::EditStepChange(TObject *Sender)
{
 double value = 0;
 if (1!=sscanf(EditStep->Text.c_str(), "%lf", &value))
 {
  ButtonOk->Enabled = false;
  return;
 }

 m_value = value; 
 ButtonOk->Enabled = true;
}

//---------------------------------------------------------------------------
void TPtMovStepDlg::SetValue(float val)
{
 m_value = val;
 AnsiString as;
 as.sprintf(m_valFmt.c_str(), m_value);
 EditStep->Text = as;
}

//---------------------------------------------------------------------------
float TPtMovStepDlg::GetValue(void) const
{
 return m_value;
}

//---------------------------------------------------------------------------
