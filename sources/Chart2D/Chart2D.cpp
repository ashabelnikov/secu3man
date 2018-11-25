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

/** \file Chart2D.cpp
 * \author Alexey A. Shabelnikov
 */

#include <map>
#include <mem.h>
#include <vector>
#include <vcl.h>
#include <windows.h>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#pragma hdrstop

#include "Form2D.h"
#include "resource.h"

extern "C"
{
 HWND  __declspec(dllexport)  __cdecl Chart2DCreate(HWND parent, const float *ip_original_function, float *iop_modified_function, float i_fnc_min, float i_fnc_max, const float *ip_x_axis_grid_values, int i_count_of_points, LPCTSTR i_x_axis_title, LPCTSTR i_y_axis_title, LPCTSTR i_chart_title, int i_bins_mode);
 void  __declspec(dllexport)  __cdecl Chart2DUpdate(HWND hWnd, const float *ip_original_function, float *iop_modified_function);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnChange(HWND hWnd, EventHandler i_pOnChange, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnClose(HWND hWnd, EventHandler i_pOnClose, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DSetMarksVisible(HWND hWnd, int i_series_index, bool i_visible);
 void  __declspec(dllexport)  __cdecl Chart2DSetAxisValuesFormat(HWND hWnd, int i_axis, LPCTSTR i_format_string);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnGetAxisLabel(HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DInverseAxis(HWND hWnd, int i_axis, bool i_inverted);
 void  __declspec(dllexport)  __cdecl Chart2DShow(HWND hWnd, int i_show);
 void  __declspec(dllexport)  __cdecl Chart2DSetLanguage(int i_language);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnWndActivation(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DEnable(HWND hWnd, bool i_enable);
 void  __declspec(dllexport)  __cdecl Chart2DSetAxisEdits(HWND hWnd, int i_axis, int i_show, float i_beginMin, float i_beginMax, float i_endMin, float i_endMax, float i_spinStep, int limitText, int spinDecimalPlaces, OnChangeValue i_pOnChangeValue, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DUpdateAxisEdits(HWND hWnd, int i_axis, float i_begin, float i_end);
 void  __declspec(dllexport)  __cdecl Chart2DShowHints(int i_show);
 void  __declspec(dllexport)  __cdecl Chart2DSetPtValuesFormat(HWND hWnd, LPCTSTR ptValFormat);
 void  __declspec(dllexport)  __cdecl Chart2DSetPtMovingStep(HWND hWnd, float step);
}

std::map<HWND, TForm*> g_form_instances; //form instance DB
std::vector<TForm*> g_form_delete;       //for garbage collection

enum ELanguage
{
 IL_ENGLISH = 0,
 IL_RUSSIAN = 1
};

HINSTANCE hInst = NULL;
extern volatile BYTE info[116];

namespace MLL
{
 AnsiString LoadString(unsigned int id)
 {
  char string[1024 + 1];
  ::LoadString(hInst, id, string, 1024);
  return AnsiString(string);
 }
}

//---------------------------------------------------------------------------
bool RemoveInstanceByHWND(HWND hWnd)
{
 if (g_form_instances.find(hWnd)!= g_form_instances.end())
 {
  g_form_delete.push_back(g_form_instances[hWnd]);
  g_form_instances.erase(hWnd);
  return true;
 }
 return false;
}

//---------------------------------------------------------------------------
bool AddInstanceByHWND(HWND hWnd, TForm* i_pForm)
{
 if (g_form_instances.find(hWnd)!= g_form_instances.end())
 {
  return false; //why do you want to add redundant references?
 }
 g_form_instances[hWnd] = i_pForm;
 return true;
}

//---------------------------------------------------------------------------
TForm* GetInstanceByHWND(HWND hWnd)
{
 if (g_form_instances.find(hWnd)!= g_form_instances.end())
 {
  return g_form_instances[hWnd];
 }
 return NULL;
}

//---------------------------------------------------------------------------
#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
 hInst = hinstDLL;
 return 1;
}

//---------------------------------------------------------------------------
HWND __cdecl Chart2DCreate(HWND parent, const float *ip_original_function, float *iop_modified_function, float i_fnc_min, float i_fnc_max, const float *ip_x_axis_grid_values, int i_count_of_points, LPCTSTR i_x_axis_title, LPCTSTR i_y_axis_title, LPCTSTR i_chart_title, int i_bins_mode)
{
  if (info[0]!=0x53)
   return NULL;

 //Clean up previous instances of forms
 for(size_t i = 0; i < g_form_delete.size(); ++i)
  delete g_form_delete[i];
 g_form_delete.clear();

 //Create a form
 TForm2D* pForm;
 pForm = new TForm2D(parent);
 pForm->m_count_of_function_points = i_count_of_points;
 pForm->m_chart_title_text = i_chart_title;
 pForm->m_x_axis_title = i_x_axis_title;
 pForm->m_y_axis_title = i_y_axis_title;
 pForm->mp_original_function = ip_original_function;
 pForm->mp_modified_function = iop_modified_function;
 pForm->m_fnc_min = i_fnc_min;
 pForm->m_fnc_max = i_fnc_max;

 //save horizontal axis grid values
 if (ip_x_axis_grid_values)
 {
  if (0==i_bins_mode) //0,1 modes
   memcpy(pForm->m_horizontal_axis_grid_values, ip_x_axis_grid_values, sizeof(float) * i_count_of_points);
  else //mode 2
   memcpy(pForm->m_horizontal_axis_grid_values, ip_x_axis_grid_values, sizeof(float) * 5); //5 values: min,max,inc,dec places,min diff
 }

 pForm->Caption = MLL::LoadString(IDS_EDITING_MAPS);
 pForm->InitPopupMenu(hInst);
 pForm->InitHints(hInst); //Set hints' text
 if (i_bins_mode)
  pForm->InitBins();

 AddInstanceByHWND(pForm->Handle, pForm);
 return pForm->Handle;
}

//---------------------------------------------------------------------------
void __cdecl Chart2DUpdate(HWND hWnd, const float *ip_original_function, float *iop_modified_function)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;

 if (ip_original_function && iop_modified_function)
 {
  //delete old values and fill series again
  for (;pForm->Series1->Count() > 0;)
   pForm->Series1->Delete(pForm->Series1->Count()-1);
  for (;pForm->Series2->Count() > 0;)
   pForm->Series2->Delete(pForm->Series2->Count()-1);

  pForm->mp_original_function = ip_original_function;
  pForm->mp_modified_function = iop_modified_function;
 }
 pForm->DataPrepare();
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetOnChange(HWND hWnd, EventHandler i_pOnChange, void* i_param)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetOnChange(i_pOnChange, i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetOnClose(HWND hWnd, EventHandler i_pOnClose, void* i_param)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetOnClose(i_pOnClose,i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetMarksVisible(HWND hWnd, int i_series_index, bool i_visible)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 switch(i_series_index)
 {
  case 0:
   pForm->Series1->Marks->Visible = i_visible;
   break;
  case 1:
   pForm->Series2->Marks->Visible = i_visible;
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetMarksVisible: Unsupported \"i_series_index\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetAxisValuesFormat(HWND hWnd, int i_axis, LPCTSTR i_format_string)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 switch(i_axis)
 {
  case 0: //Y
   pForm->Chart1->LeftAxis->AxisValuesFormat = i_format_string;
   break;
  case 1: //X
   pForm->m_horizontal_axis_values_format = i_format_string;
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetAxisValuesFormat: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetOnGetAxisLabel(HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 switch(i_axis)
 {
  case 0: //Y
   pForm->SetOnGetYAxisLabel(i_pOnGetAxisLabel, i_param);
   break;
  case 1: //X
   pForm->SetOnGetXAxisLabel(i_pOnGetAxisLabel, i_param);
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetOnGetAxisLabel: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DInverseAxis(HWND hWnd, int i_axis, bool i_inverted)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 switch(i_axis)
 {
  case 0: //Y
   pForm->Chart1->LeftAxis->Inverted = i_inverted;
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetInverted: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DShow(HWND hWnd, int i_show)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 if (1 == i_show)
   pForm->Show();
 else if  (0 == i_show)
   pForm->Hide();
 else
   MessageBox(hWnd, _T("Chart2DShow: Unsupported \"i_show\" argument!"), _T("Error"), MB_OK);
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetLanguage(int i_language)
{
 switch(i_language)
 {
  case IL_ENGLISH:
   ::SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
   break;
  case IL_RUSSIAN:
   ::SetThreadLocale(MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_ENGLISH_US), SORT_DEFAULT));
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetOnWndActivation(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetOnWndActivation(i_pOnWndActivation, i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart2DEnable(HWND hWnd, bool i_enable)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->Enable(i_enable);
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetAxisEdits(HWND hWnd, int i_axis, int i_show, float i_beginMin, float i_beginMax, float i_endMin, float i_endMax, float i_spinStep, int limitText, int spinDecimalPlaces, OnChangeValue i_pOnChangeValue, void* i_param)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;

 switch(i_axis)
 {
  case 1: //X
   pForm->ShowXEdits(i_show);
   pForm->CfgXEdits(0, i_beginMin, i_beginMax, i_spinStep, limitText, spinDecimalPlaces); //begin
   pForm->CfgXEdits(1, i_endMin, i_endMax, i_spinStep, limitText, spinDecimalPlaces);     //end
   pForm->SetXEditsCB(i_pOnChangeValue, i_param); //important: set callbacks after configuration, in opposite case we may get fake OnChange() event from non-initialized edits
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetAxisEdits: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DUpdateAxisEdits(HWND hWnd, int i_axis, float i_begin, float i_end)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;

 switch(i_axis)
 {
  case 1: //X
   pForm->SetXEditVal(0, i_begin);
   pForm->SetXEditVal(1, i_end);
   break;
  default:
   MessageBox(hWnd, _T("Chart2DUpdateAxisEdits: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DShowHints(int i_show)
{
 Application->ShowHint = i_show;
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetPtValuesFormat(HWND hWnd, LPCTSTR ptValFormat)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetPtValuesFormat(ptValFormat);
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetPtMovingStep(HWND hWnd, float step)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->m_pt_moving_step = step;
}

//---------------------------------------------------------------------------

//App. info
static volatile BYTE info[116] = {0x53,0x45,0x43,0x55,0x2d,0x33,0x20,0x4d,0x61,0x6e,0x61,0x67,0x65,0x72,0x2c,0x20,
                                  0x43,0x6f,0x70,0x79,0x72,0x69,0x67,0x68,0x74,0x20,0x28,0x43,0x29,0x20,0x32,0x30,
                                  0x30,0x37,0x20,0x41,0x6c,0x65,0x78,0x65,0x79,0x20,0x41,0x2e,0x20,0x53,0x68,0x61,
                                  0x62,0x65,0x6c,0x6e,0x69,0x6b,0x6f,0x76,0x2c,0x20,0x55,0x41,0x2c,0x20,0x4b,0x69,
                                  0x65,0x76,0x2c,0x20,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x73,0x65,0x63,0x75,0x2d,
                                  0x33,0x2e,0x6f,0x72,0x67,0x2c,0x20,0x65,0x6d,0x61,0x69,0x6c,0x3a,0x20,0x73,0x68,
                                  0x61,0x62,0x65,0x6c,0x6e,0x69,0x6b,0x6f,0x76,0x40,0x73,0x65,0x63,0x75,0x2d,0x33,
                                  0x2e,0x6f,0x72,0x67};
