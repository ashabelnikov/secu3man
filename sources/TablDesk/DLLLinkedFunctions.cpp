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
#include "common/UnicodeSupport.h"
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
 //загружает одну функцию
 template <class T>
 void LoadFunction(HMODULE hModule, T& ptr, const char* name, bool& status)
 {
  FARPROC addr;
  addr = GetProcAddress(hModule, name);
  if (addr==NULL)
  {
   _TSTRING str(_T("Error: GetProcAddress() for "));
   TCHAR string[256];
   OemToChar(name, string);
   str+= string;
   str+= _T(" symbol");
   AfxMessageBox(str.c_str(),MB_OK|MB_ICONSTOP);
   ptr = NULL;
   status = false;
  }
  else
   ptr = (T)addr;
 }

 //Заполняет таблицу функций (загружает необходимые библиотеки и получает адреса функций, сохраняя их в таблицу)
 bool LoadDLLsAndLinkToFunctions(void)
 {
  HMODULE hModule;
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
   LoadFunction(hModule, UOZ1_Chart2DCreate, "Chart2DCreate", status);
   LoadFunction(hModule, UOZ1_Chart2DUpdate, "Chart2DUpdate", status);
   LoadFunction(hModule, UOZ1_Chart2DSetOnChange, "Chart2DSetOnChange", status);
   LoadFunction(hModule, UOZ1_Chart2DSetOnClose, "Chart2DSetOnClose", status);
   LoadFunction(hModule, UOZ1_Chart2DSetMarksVisible, "Chart2DSetMarksVisible", status);
   LoadFunction(hModule, UOZ1_Chart2DSetAxisValuesFormat, "Chart2DSetAxisValuesFormat", status);
   LoadFunction(hModule, UOZ1_Chart2DSetOnGetAxisLabel, "Chart2DSetOnGetAxisLabel", status);
   LoadFunction(hModule, UOZ1_Chart2DInverseAxis, "Chart2DInverseAxis", status);
   LoadFunction(hModule, UOZ1_Chart2DShow, "Chart2DShow", status);
   LoadFunction(hModule, UOZ1_Chart2DSetLanguage, "Chart2DSetLanguage", status);
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
   LoadFunction(hModule, UOZ2_Chart3DCreate, "Chart3DCreate", status);
   LoadFunction(hModule, UOZ2_Chart3DUpdate, "Chart3DUpdate", status);
   LoadFunction(hModule, UOZ2_Chart3DSetOnChange, "Chart3DSetOnChange", status);
   LoadFunction(hModule, UOZ2_Chart3DSetOnClose, "Chart3DSetOnClose", status);
   LoadFunction(hModule, UOZ2_Chart3DShow, "Chart3DShow", status);
   LoadFunction(hModule, UOZ2_Chart3DSetLanguage, "Chart3DSetLanguage", status);
  }

  return status;
 }

 void SetLanguage(int language)
 {
  if (UOZ1_Chart2DSetLanguage)
   DLL::UOZ1_Chart2DSetLanguage(language);
  if (UOZ2_Chart3DSetLanguage)
   DLL::UOZ2_Chart3DSetLanguage(language);
 }

};//namespace
