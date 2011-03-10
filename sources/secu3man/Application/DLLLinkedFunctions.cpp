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
#include "DLLLinkedFunctions.h"

namespace DLL
{
 //глобальная таблица функций из DLL
 UOZ1_Chart2DCreate_Addr        UOZ1_Chart2DCreate = NULL;
 UOZ1_Chart2DUpdate_Addr        UOZ1_Chart2DUpdate = NULL;
 UOZ1_Chart2DSetOnChange_Addr   UOZ1_Chart2DSetOnChange = NULL;
 UOZ1_Chart2DSetOnClose_Addr    UOZ1_Chart2DSetOnClose = NULL;
 UOZ1_Chart2DSetMarksVisible_Addr UOZ1_Chart2DSetMarksVisible = NULL;
 UOZ1_Chart2DSetAxisValuesFormat_Addr UOZ1_Chart2DSetAxisValuesFormat = NULL;
 UOZ1_Chart2DSetOnGetAxisLabel_Addr UOZ1_Chart2DSetOnGetAxisLabel = NULL;
 UOZ1_Chart2DInverseAxis_Addr   UOZ1_Chart2DInverseAxis = NULL;
 UOZ1_Chart2DShow_Addr          UOZ1_Chart2DShow = NULL;
 UOZ1_Chart2DSetLanguage_Addr   UOZ1_Chart2DSetLanguage = NULL;

 UOZ2_Chart3DCreate_Addr        UOZ2_Chart3DCreate = NULL;
 UOZ2_Chart3DUpdate_Addr        UOZ2_Chart3DUpdate = NULL;
 UOZ2_Chart3DSetOnChange_Addr   UOZ2_Chart3DSetOnChange = NULL;
 UOZ2_Chart3DSetOnClose_Addr    UOZ2_Chart3DSetOnClose = NULL;
 UOZ2_Chart3DShow_Addr          UOZ2_Chart3DShow = NULL;
 UOZ2_Chart3DSetLanguage_Addr   UOZ2_Chart3DSetLanguage = NULL;

 //---------------------------------------------------------

 //Заполняет таблицу функций (загружает необходимые библиотеки и получает адреса функций, сохраняя их в таблицу)
 bool LoadDLLsAndLinkToFunctions(void)
 {
  HMODULE hModule;
  FARPROC addr;
  bool status = true;

  hModule = LoadLibrary(_T("UOZ1.dll"));
  if (hModule==NULL)
  {
   AfxMessageBox(_T("Can't load library UOZ1.dll"),MB_OK|MB_ICONSTOP);
   UOZ1_Chart2DCreate = NULL;
   UOZ1_Chart2DUpdate = NULL;
   UOZ1_Chart2DSetOnChange = NULL;
   UOZ1_Chart2DSetOnClose = NULL;
   UOZ1_Chart2DSetMarksVisible = NULL;
   UOZ1_Chart2DSetAxisValuesFormat = NULL;
   UOZ1_Chart2DSetOnGetAxisLabel = NULL;
   UOZ1_Chart2DInverseAxis = NULL;
   UOZ1_Chart2DShow = NULL;
   UOZ1_Chart2DSetLanguage = NULL;
   status = false;
  }
  else
  {
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DCreate");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DCreate = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DCreate = (UOZ1_Chart2DCreate_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DUpdate");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DUpdate = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DUpdate = (UOZ1_Chart2DUpdate_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DSetOnChange");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DSetOnChange = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DSetOnChange = (UOZ1_Chart2DSetOnChange_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DSetOnClose");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DSetOnClose = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DSetOnClose = (UOZ1_Chart2DSetOnClose_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DSetMarksVisible");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DSetMarksVisible = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DSetMarksVisible = (UOZ1_Chart2DSetMarksVisible_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DSetAxisValuesFormat");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DSetAxisValuesFormat = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DSetAxisValuesFormat = (UOZ1_Chart2DSetAxisValuesFormat_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DSetOnGetAxisLabel");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DSetOnGetAxisLabel = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DSetOnGetAxisLabel = (UOZ1_Chart2DSetOnGetAxisLabel_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DInverseAxis");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DInverseAxis = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DInverseAxis = (UOZ1_Chart2DInverseAxis_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DShow");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DShow = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DShow = (UOZ1_Chart2DShow_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart2DSetLanguage");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ1_Chart2DSetLanguage = NULL;
    status = false;
   }
   else
    UOZ1_Chart2DSetLanguage = (UOZ1_Chart2DSetLanguage_Addr)addr;
   //--------------------------------------------------------------------------
  }

  hModule = LoadLibrary(_T("UOZ2.dll"));
  if (hModule==NULL)
  {
   AfxMessageBox(_T("Can't load library UOZ2.dll"),MB_OK|MB_ICONSTOP);
   UOZ2_Chart3DCreate = NULL;
   UOZ2_Chart3DUpdate = NULL;
   UOZ2_Chart3DSetOnChange = NULL;
   UOZ2_Chart3DSetOnClose = NULL;
   UOZ2_Chart3DShow = NULL;
   UOZ2_Chart3DSetLanguage = NULL;
  status = false;
  }
  else
  {
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart3DCreate");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ2_Chart3DCreate = NULL;
    status = false;
   }
   else
    UOZ2_Chart3DCreate = (UOZ2_Chart3DCreate_Addr)addr;

   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart3DUpdate");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ2_Chart3DUpdate = NULL;
    status = false;
   }
   else
    UOZ2_Chart3DUpdate = (UOZ2_Chart3DUpdate_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart3DSetOnChange");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ2_Chart3DSetOnChange = NULL;
    status = false;
   }
   else
    UOZ2_Chart3DSetOnChange = (UOZ2_Chart3DSetOnChange_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart3DSetOnClose");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ2_Chart3DSetOnClose = NULL;
    status = false;
   }
   else
    UOZ2_Chart3DSetOnClose = (UOZ2_Chart3DSetOnClose_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart3DShow");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ2_Chart3DShow = NULL;
    status = false;
   }
   else
    UOZ2_Chart3DShow = (UOZ2_Chart3DShow_Addr)addr;
   //--------------------------------------------------------------------------
   addr = GetProcAddress(hModule,"Chart3DSetLanguage");
   if (addr==NULL)
   {
    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);
    UOZ2_Chart3DSetLanguage = NULL;
    status = false;
   }
   else
    UOZ2_Chart3DSetLanguage = (UOZ2_Chart3DSetLanguage_Addr)addr;
   //--------------------------------------------------------------------------
  }

  return status;
 }

};//namespace
