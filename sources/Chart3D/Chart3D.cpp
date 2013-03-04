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
#include <vector>
#include <vcl.h>
#include <windows.h>
#include <mem.h>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include "Form3D.h"
#include "resource.h"
#pragma hdrstop

extern "C"
{
 HWND  __declspec(dllexport)  __cdecl Chart3DCreate(float *original_function, float *modified_function,const float *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float aai_min,float aai_max, LPCTSTR x_axis_title, LPCTSTR chart_title);
 void  __declspec(dllexport)  __cdecl Chart3DUpdate(HWND hWnd, float *original_function, float *modified_function);
 void  __declspec(dllexport)  __cdecl Chart3DUpdateAxisLabels(HWND hWnd, int i_axis, const float *ip_labels_values);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnChange(HWND hWnd, EventHandler i_pOnChange, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnClose(HWND hWnd, EventHandler i_pOnClose, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DShow(HWND hWnd, int i_show);
 void  __declspec(dllexport)  __cdecl Chart3DSetLanguage(int i_language);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnWndActivation(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DEnable(HWND hWnd, bool enable);
 void  __declspec(dllexport)  __cdecl Chart3DShowHints(int i_show);
}

std::map<HWND,TForm*> g_form_instances;  //form instance DB
std::vector<TForm*> g_form_delete;       //for garbage collection

enum ELanguage
{
 IL_ENGLISH,
 IL_RUSSIAN
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
// y = F(x,z)

//original_function и modified_function - адреса двухмерных массивов содержащих
//значения функций
HWND __cdecl Chart3DCreate(float *original_function, float *modified_function, const float *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float aai_min,float aai_max, LPCTSTR x_axis_title, LPCTSTR chart_title)
{
 //Clean up previous instances of forms
 for(size_t i = 0; i < g_form_delete.size(); ++i)
  delete g_form_delete[i];
 g_form_delete.clear();

 //Create a form
 TForm3D *pForm = new TForm3D((TComponent *)NULL);
 pForm->count_x = x_count_of_points;
 pForm->count_z = z_count_of_points;
 pForm->u_title = chart_title;
 pForm->modified_function = modified_function;
 pForm->original_function = original_function;
 pForm->x_title = x_axis_title;
 pForm->aai_min = aai_min;
 pForm->aai_max = aai_max;

 pForm->Caption = MLL::LoadString(IDS_EDITING_MAPS);
 pForm->Chart1->LeftAxis->Title->Caption = MLL::LoadString(IDS_LEFT_AXIS_TITLE);
 pForm->Label2->Caption = MLL::LoadString(IDS_AIR_FLOW_CAPTION_TEXT);
 pForm->CheckBox2->Caption = MLL::LoadString(IDS_BACK_SIDE_VIEW_CB);
 pForm->InitPopupMenu(hInst);
 pForm->InitHints(hInst); //Set hints' text

 memcpy(pForm->u_slots, x_axis_grid_values, sizeof(float) * x_count_of_points);
 pForm->DataPrepare();
 AddInstanceByHWND(pForm->Handle, pForm);
 return pForm->Handle;
}

//---------------------------------------------------------------------------
void __cdecl Chart3DUpdate(HWND hWnd, float *original_function, float *modified_function)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;

 //удаляем старые значения, а потом вновь заполняем серии
 for(int i = 0; i < pForm->Chart1->SeriesList->Count; i++)
  for (;pForm->Chart1->Series[i]->Count() > 0;)
   pForm->Chart1->Series[i]->Delete(pForm->Chart1->Series[i]->Count()-1);

 pForm->original_function = original_function;
 pForm->modified_function = modified_function;
 pForm->DataPrepare();
}

//---------------------------------------------------------------------------
void __cdecl Chart3DUpdateAxisLabels(HWND hWnd, int i_axis, const float *ip_labels_values)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm || !ip_labels_values)
  return;

 switch(i_axis)
 {
  case 1: //X
  {
   for(int i = 0; i < pForm->count_x; ++i)
   {
    for(int s = 0; s < pForm->count_z; ++s)
    {
     pForm->Chart1->Series[s]->XLabel[i] = ip_labels_values[i];
     pForm->Chart1->Series[s+pForm->count_z]->XLabel[i] = ip_labels_values[i];
    }
   }
  }
   break;
  default:
   MessageBox(hWnd, _T("Chart3DUpdateLabels: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetOnChange(HWND hWnd, EventHandler i_pOnChange, void* i_param)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetOnChange(i_pOnChange, i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetOnClose(HWND hWnd, EventHandler i_pOnClose, void* i_param)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetOnClose(i_pOnClose,i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetOnWndActivation(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetOnWndActivation(i_pOnWndActivation, i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DShow(HWND hWnd, int i_show)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 if (1 == i_show)
  pForm->Show();
 else if  (0 == i_show)
  pForm->Hide();
 else
  MessageBox(hWnd, _T("Chart3DShow: Unsupported \"i_show\" argument!"), _T("Error"), MB_OK);
}
//---------------------------------------------------------------------------
void __cdecl Chart3DSetLanguage(int i_language)
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
void __cdecl Chart3DEnable(HWND hWnd, bool enable)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->Enable(enable);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DShowHints(int i_show)
{
 Application->ShowHint = i_show;
}

//---------------------------------------------------------------------------
