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
#include <mem.h>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <map>
#include "Unit1.h"
#include "resource.h"
#pragma hdrstop

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
 TForm1* pForm1;
 pForm1 = new TForm1((TComponent *)NULL);
 pForm1->count_of_function_points    = count_of_points;
 pForm1->chart_title_text = chart_title;
 pForm1->x_axis_title = x_axis_title;
 pForm1->y_axis_title = y_axis_title;
 pForm1->original_function   = original_function;
 pForm1->modified_function   = modified_function;
 pForm1->aai_min    = aai_min;
 pForm1->aai_max    = aai_max;

 pForm1->Caption = MLL::LoadString(IDS_EDITING_MAPS);

 //сохраняем значения сетки по горизонтальной оси
 memcpy(pForm1->horizontal_axis_grid_values, x_axis_grid_values, sizeof(float) * count_of_points);
 AddInstanceByHWND(pForm1->Handle, pForm1);
 return pForm1->Handle;
}

//---------------------------------------------------------------------------
void __cdecl Chart2DUpdate(HWND hWnd, float *original_function, float *modified_function)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
   return;

 if (original_function && modified_function)
 {
  //удаляем старые значения, а потом вновь заполняем серии
  for (;pForm1->Series1->Count() > 0;)
   pForm1->Series1->Delete(pForm1->Series1->Count()-1);
  for (;pForm1->Series2->Count() > 0;)
   pForm1->Series2->Delete(pForm1->Series2->Count()-1);

  pForm1->original_function   = original_function;
  pForm1->modified_function   = modified_function;
 }
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
void __cdecl Chart2DSetMarksVisible(HWND hWnd, int i_series_index, bool i_visible)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
  return;
 switch(i_series_index)
 {
  case 0:
   pForm1->Series1->Marks->Visible = i_visible;
   break;
  case 1:
   pForm1->Series2->Marks->Visible = i_visible;
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetMarksVisible: Unsupported \"i_series_index\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetAxisValuesFormat(HWND hWnd, int i_axis, LPCTSTR i_format_string)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
  return;
 switch(i_axis)
 {
  case 0: //Y
   pForm1->Chart1->LeftAxis->AxisValuesFormat = i_format_string;
   break;
  case 1: //X
   pForm1->horizontal_axis_values_format = i_format_string;
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetAxisValuesFormat: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DSetOnGetAxisLabel(HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
  return;
 switch(i_axis)
 {
  case 0: //Y
   pForm1->SetOnGetYAxisLabel(i_pOnGetAxisLabel, i_param);
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetOnGetAxisLabel: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DInverseAxis(HWND hWnd, int i_axis, bool i_inverted)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
  return;
 switch(i_axis)
 {
  case 0: //Y
   pForm1->Chart1->LeftAxis->Inverted = i_inverted;
   break;
  default:
   MessageBox(hWnd, _T("Chart2DSetInverted: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart2DShow(HWND hWnd, int i_show)
{
 TForm1* pForm1 = static_cast<TForm1*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm1)
  return;
 if (1 == i_show)
   pForm1->Show();
 else if  (0 == i_show)
   pForm1->Hide();
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

