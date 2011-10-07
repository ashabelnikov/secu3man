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

#include <map>
#include <mem.h>
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
 HWND  __declspec(dllexport)  __cdecl Chart2DCreate(float *original_function, float *modified_function,float aai_min,float aai_max,const float *x_axis_grid_values, int count_of_points, LPCTSTR x_axis_title, LPCTSTR y_axis_title,LPCTSTR chart_title);
 void  __declspec(dllexport)  __cdecl Chart2DUpdate(HWND hWnd, float *original_function, float *modified_function);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnChange(HWND hWnd, EventHandler i_pOnChange, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnClose(HWND hWnd, EventHandler i_pOnClose, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DSetMarksVisible(HWND hWnd, int i_series_index, bool i_visible);
 void  __declspec(dllexport)  __cdecl Chart2DSetAxisValuesFormat(HWND hWnd, int i_axis, LPCTSTR i_format_string);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnGetAxisLabel(HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DInverseAxis(HWND hWnd, int i_axis, bool i_inverted);
 void  __declspec(dllexport)  __cdecl Chart2DShow(HWND hWnd, int i_show);
 void  __declspec(dllexport)  __cdecl Chart2DSetLanguage(int i_language);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnWndActivation(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DEnable(HWND hWnd, bool enable);
}

std::map<HWND, TForm*> g_form_instances;

enum ELanguage
{
 IL_ENGLISH = 0,
 IL_RUSSIAN = 1
};

HINSTANCE hInst = NULL;

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
HWND __cdecl Chart2DCreate(float *original_function, float *modified_function, float aai_min, float aai_max, const float *x_axis_grid_values, int count_of_points, LPCTSTR x_axis_title, LPCTSTR y_axis_title, LPCTSTR chart_title)
{
 TForm2D* pForm;
 pForm = new TForm2D((TComponent *)NULL);
 pForm->count_of_function_points = count_of_points;
 pForm->chart_title_text = chart_title;
 pForm->x_axis_title = x_axis_title;
 pForm->y_axis_title = y_axis_title;
 pForm->original_function = original_function;
 pForm->modified_function = modified_function;
 pForm->aai_min = aai_min;
 pForm->aai_max = aai_max;

 pForm->Caption = MLL::LoadString(IDS_EDITING_MAPS);
 pForm->InitPopupMenu(hInst);

 //сохраняем значения сетки по горизонтальной оси
 memcpy(pForm->horizontal_axis_grid_values, x_axis_grid_values, sizeof(float) * count_of_points);
 AddInstanceByHWND(pForm->Handle, pForm);
 return pForm->Handle;
}

//---------------------------------------------------------------------------
void __cdecl Chart2DUpdate(HWND hWnd, float *original_function, float *modified_function)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;

 if (original_function && modified_function)
 {
  //удаляем старые значения, а потом вновь заполняем серии
  for (;pForm->Series1->Count() > 0;)
   pForm->Series1->Delete(pForm->Series1->Count()-1);
  for (;pForm->Series2->Count() > 0;)
   pForm->Series2->Delete(pForm->Series2->Count()-1);

  pForm->original_function = original_function;
  pForm->modified_function = modified_function;
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
   pForm->horizontal_axis_values_format = i_format_string;
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
void __cdecl Chart2DEnable(HWND hWnd, bool enable)
{
 TForm2D* pForm = static_cast<TForm2D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->Enable(enable);
}

//---------------------------------------------------------------------------

