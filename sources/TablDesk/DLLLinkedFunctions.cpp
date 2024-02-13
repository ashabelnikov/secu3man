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

/** \file DLLLinkedFunctions.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "common/ModuleName.h"
#include "common/UnicodeSupport.h"
#include "DLLLinkedFunctions.h"
#include "ui-core/MsgBox.h"

namespace DLL
{
 //Global table of functions from DLLs
 Chart2DCreate_Addr              Chart2DCreate = NULL;
 Chart2DUpdate_Addr              Chart2DUpdate = NULL;
 Chart2DSetOnChange_Addr         Chart2DSetOnChange = NULL;
 Chart2DSetOnChangeSettings_Addr Chart2DSetOnChangeSettings = NULL;
 Chart2DSetOnClose_Addr          Chart2DSetOnClose = NULL;
 Chart2DSetMarksVisible_Addr     Chart2DSetMarksVisible = NULL;
 Chart2DSetAxisValuesFormat_Addr Chart2DSetAxisValuesFormat = NULL;
 Chart2DSetOnGetAxisLabel_Addr   Chart2DSetOnGetAxisLabel = NULL;
 Chart2DInverseAxis_Addr         Chart2DInverseAxis = NULL;
 Chart2DShow_Addr                Chart2DShow = NULL;
 Chart2DSetOnWndActivation_Addr  Chart2DSetOnWndActivation = NULL;
 Chart2DEnable_Addr              Chart2DEnable = NULL;
 Chart2DSetPtValuesFormat_Addr   Chart2DSetPtValuesFormat = NULL;
 Chart2DSetPtMovingStep_Addr     Chart2DSetPtMovingStep = NULL;
 Chart2DGetPtMovingStep_Addr     Chart2DGetPtMovingStep = NULL;
 Chart2DUpdateYRange_Addr        Chart2DUpdateYRange = NULL;
 Chart2DSetCSVSepSymbol_Addr     Chart2DSetCSVSepSymbol = NULL;
 Chart2DSetAxisTitle_Addr        Chart2DSetAxisTitle = NULL;
 Chart2DRefresh_Addr             Chart2DRefresh = NULL;

 Chart3DCreate_Addr              Chart3DCreate = NULL;
 Chart3DUpdate_Addr              Chart3DUpdate = NULL;
 Chart3DSetOnChange_Addr         Chart3DSetOnChange = NULL;
 Chart3DSetOnChangeSettings_Addr Chart3DSetOnChangeSettings = NULL;
 Chart3DSetOnClose_Addr          Chart3DSetOnClose = NULL;
 Chart3DSetOnGetAxisLabel_Addr   Chart3DSetOnGetAxisLabel = NULL;
 Chart3DShow_Addr                Chart3DShow = NULL;
 Chart3DSetOnWndActivation_Addr  Chart3DSetOnWndActivation = NULL;
 Chart3DEnable_Addr              Chart3DEnable = NULL;
 Chart3DSetPtValuesFormat_Addr   Chart3DSetPtValuesFormat = NULL;
 Chart3DSetPtMovingStep_Addr     Chart3DSetPtMovingStep = NULL;
 Chart3DGetPtMovingStep_Addr     Chart3DGetPtMovingStep = NULL;
 Chart3DSetFncRange_Addr         Chart3DSetFncRange = NULL; 
 Chart3DSetOnValueTransform_Addr Chart3DSetOnValueTransform = NULL;
 Chart3DSetAxisTitle_Addr        Chart3DSetAxisTitle = NULL;
 Chart3DSetTitle_Addr            Chart3DSetTitle = NULL;
 Chart3DSetCSVSepSymbol_Addr     Chart3DSetCSVSepSymbol = NULL;
 Chart3DSetClassic2DKeys_Addr    Chart3DSetClassic2DKeys = NULL;

 ChartxDSetLanguage_Addr         ChartxDSetLanguage = NULL;
 ChartxDShowHints_Addr           ChartxDShowHints = NULL;

 //---------------------------------------------------------
 //Loads one function
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
   SECUMessageBox(str.c_str(),MB_OK|MB_ICONSTOP);
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

  hModule = LoadLibrary(ModuleName::chartxd);
  if (hModule==NULL)
  {
   SECUMessageBox(_T("Can't load library ChartxD.dll"), MB_OK|MB_ICONSTOP);
   Chart2DCreate = NULL;
   Chart2DUpdate = NULL;
   Chart2DSetOnChange = NULL;
   Chart2DSetOnChangeSettings = NULL;
   Chart2DSetOnClose = NULL;
   Chart2DSetMarksVisible = NULL;
   Chart2DSetAxisValuesFormat = NULL;
   Chart2DSetOnGetAxisLabel = NULL;
   Chart2DInverseAxis = NULL;
   Chart2DShow = NULL;
   Chart2DSetOnWndActivation = NULL;
   Chart2DEnable = NULL;
   Chart2DSetPtValuesFormat = NULL;
   Chart2DSetPtMovingStep = NULL;
   Chart2DUpdateYRange = NULL;
   Chart2DSetCSVSepSymbol = NULL;
   Chart2DSetAxisTitle = NULL;
   Chart2DRefresh = NULL;

   //3D
   Chart3DCreate = NULL;
   Chart3DUpdate = NULL;
   Chart3DSetOnChange = NULL;
   Chart3DSetOnChangeSettings = NULL;
   Chart3DSetOnClose = NULL;
   Chart3DSetOnGetAxisLabel = NULL;
   Chart3DShow = NULL;
   Chart3DSetOnWndActivation = NULL;
   Chart3DEnable = NULL;
   Chart3DSetPtValuesFormat = NULL;
   Chart3DSetPtMovingStep = NULL;
   Chart3DSetFncRange = NULL; 
   Chart3DSetOnValueTransform = NULL;
   Chart3DSetAxisTitle = NULL;
   Chart3DSetTitle = NULL;
   Chart3DSetCSVSepSymbol = NULL;
   Chart3DSetClassic2DKeys = NULL;
   //common
   ChartxDSetLanguage = NULL;
   ChartxDShowHints = NULL;
   status = false;
  }
  else
  {
   LoadFunction(hModule, Chart2DCreate, "Chart2DCreate", status);
   LoadFunction(hModule, Chart2DUpdate, "Chart2DUpdate", status);
   LoadFunction(hModule, Chart2DSetOnChange, "Chart2DSetOnChange", status);
   LoadFunction(hModule, Chart2DSetOnChangeSettings, "Chart2DSetOnChangeSettings", status);
   LoadFunction(hModule, Chart2DSetOnClose, "Chart2DSetOnClose", status);
   LoadFunction(hModule, Chart2DSetMarksVisible, "Chart2DSetMarksVisible", status);
   LoadFunction(hModule, Chart2DSetAxisValuesFormat, "Chart2DSetAxisValuesFormat", status);
   LoadFunction(hModule, Chart2DSetOnGetAxisLabel, "Chart2DSetOnGetAxisLabel", status);
   LoadFunction(hModule, Chart2DInverseAxis, "Chart2DInverseAxis", status);
   LoadFunction(hModule, Chart2DShow, "Chart2DShow", status);
   LoadFunction(hModule, Chart2DSetOnWndActivation, "Chart2DSetOnWndActivation", status);
   LoadFunction(hModule, Chart2DEnable, "Chart2DEnable", status);
   LoadFunction(hModule, Chart2DSetPtValuesFormat, "Chart2DSetPtValuesFormat", status);
   LoadFunction(hModule, Chart2DSetPtMovingStep, "Chart2DSetPtMovingStep", status);
   LoadFunction(hModule, Chart2DGetPtMovingStep, "Chart2DGetPtMovingStep", status);
   LoadFunction(hModule, Chart2DUpdateYRange, "Chart2DUpdateYRange", status);
   LoadFunction(hModule, Chart2DSetCSVSepSymbol, "Chart2DSetCSVSepSymbol", status);
   LoadFunction(hModule, Chart2DSetAxisTitle, "Chart2DSetAxisTitle", status);
   LoadFunction(hModule, Chart2DRefresh, "Chart2DRefresh", status);

   //3D
   LoadFunction(hModule, Chart3DCreate, "Chart3DCreate", status);
   LoadFunction(hModule, Chart3DUpdate, "Chart3DUpdate", status);
   LoadFunction(hModule, Chart3DSetOnChange, "Chart3DSetOnChange", status);
   LoadFunction(hModule, Chart3DSetOnChangeSettings, "Chart3DSetOnChangeSettings", status);
   LoadFunction(hModule, Chart3DSetOnClose, "Chart3DSetOnClose", status);
   LoadFunction(hModule, Chart3DSetOnGetAxisLabel, "Chart3DSetOnGetAxisLabel", status);
   LoadFunction(hModule, Chart3DShow, "Chart3DShow", status);
   LoadFunction(hModule, Chart3DSetOnWndActivation, "Chart3DSetOnWndActivation", status);
   LoadFunction(hModule, Chart3DEnable, "Chart3DEnable", status);
   LoadFunction(hModule, Chart3DSetPtValuesFormat, "Chart3DSetPtValuesFormat", status);
   LoadFunction(hModule, Chart3DSetPtMovingStep, "Chart3DSetPtMovingStep", status);
   LoadFunction(hModule, Chart3DGetPtMovingStep, "Chart3DGetPtMovingStep", status);
   LoadFunction(hModule, Chart3DSetFncRange, "Chart3DSetFncRange", status);
   LoadFunction(hModule, Chart3DSetOnValueTransform, "Chart3DSetOnValueTransform", status);
   LoadFunction(hModule, Chart3DSetAxisTitle, "Chart3DSetAxisTitle", status);
   LoadFunction(hModule, Chart3DSetTitle, "Chart3DSetTitle", status);
   LoadFunction(hModule, Chart3DSetCSVSepSymbol, "Chart3DSetCSVSepSymbol", status);
   LoadFunction(hModule, Chart3DSetClassic2DKeys, "Chart3DSetClassic2DKeys", status);
   //common
   LoadFunction(hModule, ChartxDSetLanguage, "ChartxDSetLanguage", status);
   LoadFunction(hModule, ChartxDShowHints, "ChartxDShowHints", status);
  }

  return status;
 }

 void SetLanguage(int language)
 {
  if (ChartxDSetLanguage)
   DLL::ChartxDSetLanguage(language);
 }

 void ShowHints(bool i_show, int timeToShow)
 {
  if (ChartxDShowHints)
   DLL::ChartxDShowHints(i_show, timeToShow);
 }

};//namespace
