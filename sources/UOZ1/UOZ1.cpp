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
#pragma hdrstop

extern "C"
{
  HWND  __declspec(dllexport)  __cdecl DisplayChart_AAoI_2D(float *original_function, float *modified_function,float aai_min,float aai_max,int *x_axis_grid_values, int count_of_points, LPCTSTR chart_title, int *close_feedback);
}


#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
  return 1;   
}
//---------------------------------------------------------------------------

HWND __cdecl DisplayChart_AAoI_2D(float *original_function, float *modified_function,float aai_min,float aai_max,int *x_axis_grid_values, int count_of_points, LPCTSTR chart_title, int *close_feedback)
{
  TForm1 *Form1 = new TForm1((TComponent *)NULL);
  Form1->v_count    = count_of_points;
  Form1->u_title    = chart_title;
  Form1->original_function   = original_function;
  Form1->modified_function   = modified_function;
  Form1->close_flag = close_feedback;
  Form1->aai_min    = aai_min;
  Form1->aai_max    = aai_max;
  //сохраняем значения сетки по горизонтальной оси
  memcpy(Form1->u_slots,x_axis_grid_values,sizeof(int)*count_of_points);
  Form1->Show();
  Form1->DataPrepare();
  return Form1->Handle;
}
