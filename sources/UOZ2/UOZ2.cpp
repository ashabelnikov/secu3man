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
  HWND  __declspec(dllexport)  __cdecl DisplayChart_AAoI_3D(float *original_function, float *modified_function,int *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float aai_min,float aai_max, LPCTSTR x_axis_title, LPCTSTR chart_title, int *close_feedback);
}

#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
  return 1;
}

//---------------------------------------------------------------------------
// y = F(x,z)

HWND __cdecl DisplayChart_AAoI_3D(float *original_function, float *modified_function,int *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float aai_min,float aai_max, LPCTSTR x_axis_title, LPCTSTR chart_title, int *close_feedback)
{
  TForm1 *Form1 = new TForm1((TComponent *)NULL);
  Form1->count_x    = x_count_of_points;
  Form1->count_z    = z_count_of_points;
  Form1->u_title    = chart_title;
  Form1->modified_function = modified_function;
  Form1->original_function = original_function;
  Form1->x_title    = x_axis_title;
  Form1->aai_min    = aai_min;
  Form1->aai_max    = aai_max;
  Form1->close_flag = close_feedback;
  memcpy(Form1->u_slots,x_axis_grid_values,sizeof(int)*x_count_of_points);
  Form1->Show();
  Form1->DataPrepare();
  return Form1->Handle;
}

//---------------------------------------------------------------------------

