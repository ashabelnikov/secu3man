/****************************************************************
 *               SECU-3  - engine control unit
 *    Designed by Aleksey Shabelnikov. Ukraine, Gorlovka 2007.
 *    STc - Microprocessors systems: design & programming
 *    contacts:
 *              shabelnikov-stc@mail.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include <vcl.h>
#include <windows.h>
#include "Unit1.h"
#include <mem.h>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <map>
#pragma hdrstop

extern "C"
{
 HWND  __declspec(dllexport)  __cdecl Chart2DCreate(float *original_function, float *modified_function,float aai_min,float aai_max,int *x_axis_grid_values, int count_of_points, LPCTSTR x_axis_title,LPCTSTR chart_title);
 void  __declspec(dllexport)  __cdecl Chart2DUpdate(HWND hWnd, float *original_function, float *modified_function);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnChange(HWND hWnd, EventHandler i_pOnChange, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart2DSetOnClose(HWND hWnd, EventHandler i_pOnClose, void* i_param);
}

std::map<HWND,TForm*> g_form_instances;

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
 return 1;
}

//---------------------------------------------------------------------------
HWND __cdecl Chart2DCreate(float *original_function, float *modified_function,float aai_min,float aai_max,int *x_axis_grid_values, int count_of_points, LPCTSTR x_axis_title, LPCTSTR chart_title)
{
 TForm1* pForm1;
 pForm1 = new TForm1((TComponent *)NULL);
 pForm1->count_of_function_points    = count_of_points;
 pForm1->chart_title_text = chart_title;
 pForm1->x_axis_title = x_axis_title;
 pForm1->original_function   = original_function;
 pForm1->modified_function   = modified_function;
 pForm1->aai_min    = aai_min;
 pForm1->aai_max    = aai_max;
 //сохраняем значения сетки по горизонтальной оси
 memcpy(pForm1->horizontal_axis_grid_values,x_axis_grid_values,sizeof(int)*count_of_points);
 pForm1->Show();
 pForm1->DataPrepare();
 AddInstanceByHWND(pForm1->Handle,pForm1);
 return pForm1->Handle;
}

//---------------------------------------------------------------------------
void __cdecl Chart2DUpdate(HWND hWnd, float *original_function, float *modified_function)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
   return;
  //удаляем старые значения, а потом вновь заполняем серии
  for (;pForm1->Series1->Count() > 0;)
   pForm1->Series1->Delete(pForm1->Series1->Count()-1);
  for (;pForm1->Series2->Count() > 0;)
   pForm1->Series2->Delete(pForm1->Series2->Count()-1);

  pForm1->original_function   = original_function;
  pForm1->modified_function   = modified_function;
  pForm1->DataPrepare();
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetOnChange(HWND hWnd, EventHandler i_pOnChange, void* i_param)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
   return;
 pForm1->SetOnChange(i_pOnChange, i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetOnClose(HWND hWnd, EventHandler i_pOnClose, void* i_param)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
   return;
 pForm1->SetOnClose(i_pOnClose,i_param);
}

//---------------------------------------------------------------------------

