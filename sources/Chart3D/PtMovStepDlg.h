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

/** \file PtMovStepDlg.h
 * \author Alexey A. Shabelnikov
 */

//---------------------------------------------------------------------------

#ifndef _PTMOVSTEP_H_
#define _PTMOVSTEP_H_
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "TFloatUpDown.h"

//---------------------------------------------------------------------------
class TPtMovStepDlg : public TForm
{
__published:	// IDE-managed Components
        TButton *ButtonOk;
        TButton *ButtonCancel;
        TEdit *EditStep;
        TFloatUpDown *SpinStep;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall ButtonOkClick(TObject *Sender);
        void __fastcall EditStepChange(TObject *Sender);
public:		// User declarations
        __fastcall TPtMovStepDlg(TComponent* Owner);
        void SetValue(float val);
        float GetValue(void) const;
private:
        double m_value;
        AnsiString m_valFmt;
};
//---------------------------------------------------------------------------
extern PACKAGE TPtMovStepDlg *PtMovStepDlg;
//---------------------------------------------------------------------------
#endif //_PTMOVSTEP_H_
