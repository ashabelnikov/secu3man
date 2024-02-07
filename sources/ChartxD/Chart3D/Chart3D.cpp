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

/** \file Chart3D.cpp
 * \author Alexey A. Shabelnikov
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
#include "../ManageFrm.h"
#include "../common/Chartxdid.h"
#pragma hdrstop

extern "C"
{
 HWND  __declspec(dllexport)  __cdecl Chart3DCreate(HWND parent, float *original_function, float *modified_function,const float *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float fnc_min,float fnc_max, LPCTSTR x_axis_title, LPCTSTR y_axis_title, LPCTSTR chart_title);
 void  __declspec(dllexport)  __cdecl Chart3DUpdate(HWND hWnd, float *original_function, float *modified_function);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnGetAxisLabel(HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnChange(HWND hWnd, EventHandler i_pOnChange, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnChangeSettings(HWND hWnd, EventHandler i_pOnChange, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnClose(HWND hWnd, EventHandler i_pOnClose, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DShow(HWND hWnd, int i_show);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnWndActivation(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DEnable(HWND hWnd, bool enable);
 void  __declspec(dllexport)  __cdecl Chart3DSetPtValuesFormat(HWND hWnd, LPCTSTR ptValFormat);
 void  __declspec(dllexport)  __cdecl Chart3DSetPtMovingStep(HWND hWnd, float step);
 float __declspec(dllexport)  __cdecl Chart3DGetPtMovingStep(HWND hWnd);
 void  __declspec(dllexport)  __cdecl Chart3DSetFncRange(HWND hWnd, float fnc_min, float fnc_max);
 void  __declspec(dllexport)  __cdecl Chart3DSetOnValueTransform(HWND hWnd, OnValueTransform i_pOnValueTransform, void* i_param);
 void  __declspec(dllexport)  __cdecl Chart3DSetAxisTitle(HWND hWnd, int i_axis, LPCTSTR axisTitle);
 void  __declspec(dllexport)  __cdecl Chart3DSetTitle(HWND hWnd, LPCTSTR title);
 void  __declspec(dllexport)  __cdecl Chart3DSetCSVSepSymbol(char sepsymb);
 void  __declspec(dllexport)  __cdecl Chart3DSetClassic2DKeys(bool classic);
}

extern HINSTANCE hInst;
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
// y = F(x,z)

//original_function and modified_function - addresses of 2D arrays containing values of functions
HWND __cdecl Chart3DCreate(HWND parent, float *original_function, float *modified_function, const float *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float fnc_min,float fnc_max, LPCTSTR x_axis_title, LPCTSTR y_axis_title, LPCTSTR chart_title)
{
 if (info[0]!=0x53)
  return NULL;

 //Create a form
 TForm3D *pForm = new TForm3D(parent);
 pForm->SetDimentions(x_count_of_points, z_count_of_points);
 pForm->SetRange(fnc_min, fnc_max);
 pForm->SetChartTitle(chart_title);
 pForm->SetXAxisTitle(x_axis_title);
 pForm->SetYAxisTitle(y_axis_title);
 pForm->mp_modified_function = modified_function;
 pForm->mp_original_function = original_function;

 pForm->Caption = MLL::LoadString(IDS_EDITING_MAPS);
 pForm->LabelAfc->Caption = MLL::LoadString(IDS_AIR_FLOW_CAPTION_TEXT);
 pForm->CheckBoxBv->Caption = MLL::LoadString(IDS_BACK_SIDE_VIEW_CB);
 pForm->InitPopupMenu(hInst);
 pForm->InitHints(hInst); //Set hints' text

 std::copy(x_axis_grid_values, x_axis_grid_values + x_count_of_points, pForm->m_u_slots);
 pForm->DataPrepare(true);
 AddInstanceByHWND(pForm->Handle, pForm);
 return pForm->Handle;
}

//---------------------------------------------------------------------------
void __cdecl Chart3DUpdate(HWND hWnd, float *original_function, float *modified_function)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;

 if (original_function && modified_function)
 {
  pForm->mp_original_function = original_function;
  pForm->mp_modified_function = modified_function;
 }

 pForm->DataPrepare(false);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetOnGetAxisLabel(HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 switch(i_axis)
 {
  case CXD_X_AXIS: //X
   pForm->SetOnGetXAxisLabel(i_pOnGetAxisLabel, i_param);
   break;
  default:
   MessageBox(hWnd, _T("Chart3DSetOnGetAxisLabel: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
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
 {
  pForm->Show();
  pForm->Chart1->SetFocus();
 }
 else if  (0 == i_show)
  pForm->Hide();
 else
  MessageBox(hWnd, _T("Chart3DShow: Unsupported \"i_show\" argument!"), _T("Error"), MB_OK);
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
void __cdecl Chart3DSetPtValuesFormat(HWND hWnd, LPCTSTR ptValFormat)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetPtValuesFormat(ptValFormat);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetPtMovingStep(HWND hWnd, float step)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->m_pt_moving_step = step;
}

//---------------------------------------------------------------------------
float __cdecl Chart3DGetPtMovingStep(HWND hWnd)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return 0;
 return pForm->m_pt_moving_step;
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetFncRange(HWND hWnd, float fnc_min, float fnc_max)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetRange(fnc_min, fnc_max);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetOnValueTransform(HWND hWnd, OnValueTransform i_pOnValueTransform, void* i_param)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetOnValueTransform(i_pOnValueTransform, i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetAxisTitle(HWND hWnd, int i_axis, LPCTSTR axisTitle)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 switch(i_axis)
 {
  case CXD_Y_AXIS: //Y
   pForm->SetYAxisTitle(axisTitle);
   break;
  default:
   MessageBox(hWnd, _T("Chart3DSetAxisTitle: Unsupported \"i_axis\" argument!"), _T("Error"), MB_OK);
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetOnChangeSettings(HWND hWnd, EventHandler i_pOnChange, void* i_param)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;
 pForm->SetOnChangeSettings(i_pOnChange, i_param);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetTitle(HWND hWnd, LPCTSTR title)
{
 TForm3D* pForm = static_cast<TForm3D*>(GetInstanceByHWND(hWnd));
 if (NULL==pForm)
  return;

 pForm->SetChartTitle(title);
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetCSVSepSymbol(char sepsymb)
{
 TForm3D::m_csvsep_symb = sepsymb;
}

//---------------------------------------------------------------------------
void __cdecl Chart3DSetClassic2DKeys(bool classic)
{
 TForm3D::m_classic2DKeys = classic;
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
