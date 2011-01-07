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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#pragma once

typedef void (__cdecl *EventHandler)(void* i_param);
typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, void* i_param);

typedef HWND (__cdecl *UOZ1_Chart2DCreate_Addr) (float *original_function, float *modified_function,float aai_min,float aai_max,const float *x_axis_grid_values, int count_of_points, LPCTSTR x_axis_title, LPCTSTR y_axis_title, LPCTSTR chart_title);
typedef void (__cdecl *UOZ1_Chart2DUpdate_Addr) (HWND hWnd, float *original_function, float *modified_function);
typedef void (__cdecl *UOZ1_Chart2DSetOnChange_Addr) (HWND hWnd, EventHandler i_pOnChange, void* i_param);
typedef void (__cdecl *UOZ1_Chart2DSetOnClose_Addr) (HWND hWnd, EventHandler i_pOnClose, void* i_param);
typedef void (__cdecl *UOZ1_Chart2DSetMarksVisible_Addr) (HWND hWnd, int i_series_index, bool i_visible);
typedef void (__cdecl *UOZ1_Chart2DSetAxisValuesFormat_Addr) (HWND hWnd, int i_axis, LPCTSTR i_format_string);
typedef void (__cdecl *UOZ1_Chart2DSetOnGetAxisLabel_Addr) (HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
typedef void (__cdecl *UOZ1_Chart2DInverseAxis_Addr) (HWND hWnd, int i_axis, bool i_inverted);
typedef void (__cdecl *UOZ1_Chart2DShow_Addr) (HWND hWnd, int i_show);
typedef void (__cdecl *UOZ1_Chart2DSetLanguage_Addr) (int i_language);

typedef HWND (__cdecl *UOZ2_Chart3DCreate_Addr) (float *original_function, float *modified_function,const int *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float aai_min,float aai_max, LPCTSTR x_axis_title, LPCTSTR chart_title);
typedef void (__cdecl *UOZ2_Chart3DUpdate_Addr) (HWND hWnd, float *original_function, float *modified_function);
typedef void (__cdecl *UOZ2_Chart3DSetOnChange_Addr) (HWND hWnd, EventHandler i_pOnChange, void* i_param);
typedef void (__cdecl *UOZ2_Chart3DSetOnClose_Addr) (HWND hWnd, EventHandler i_pOnClose, void* i_param);
typedef void (__cdecl *UOZ2_Chart3DShow_Addr) (HWND hWnd, int i_show);
typedef void (__cdecl *UOZ2_Chart3DSetLanguage_Addr) (int i_language);

namespace DLL
{
 ///////////////////////////////////////////////////////////////////
 extern UOZ1_Chart2DCreate_Addr        UOZ1_Chart2DCreate;
 extern UOZ1_Chart2DUpdate_Addr        UOZ1_Chart2DUpdate;
 extern UOZ1_Chart2DSetOnChange_Addr   UOZ1_Chart2DSetOnChange;
 extern UOZ1_Chart2DSetOnClose_Addr    UOZ1_Chart2DSetOnClose;
 extern UOZ1_Chart2DSetMarksVisible_Addr UOZ1_Chart2DSetMarksVisible;
 extern UOZ1_Chart2DSetAxisValuesFormat_Addr UOZ1_Chart2DSetAxisValuesFormat;
 extern UOZ1_Chart2DSetOnGetAxisLabel_Addr UOZ1_Chart2DSetOnGetAxisLabel;
 extern UOZ1_Chart2DInverseAxis_Addr   UOZ1_Chart2DInverseAxis;
 extern UOZ1_Chart2DShow_Addr          UOZ1_Chart2DShow;
 extern UOZ1_Chart2DSetLanguage_Addr   UOZ1_Chart2DSetLanguage;

 extern UOZ2_Chart3DCreate_Addr        UOZ2_Chart3DCreate;
 extern UOZ2_Chart3DUpdate_Addr        UOZ2_Chart3DUpdate;
 extern UOZ2_Chart3DSetOnChange_Addr   UOZ2_Chart3DSetOnChange;
 extern UOZ2_Chart3DSetOnClose_Addr    UOZ2_Chart3DSetOnClose;
 extern UOZ2_Chart3DShow_Addr          UOZ2_Chart3DShow;
 extern UOZ2_Chart3DSetLanguage_Addr   UOZ2_Chart3DSetLanguage;

 ///////////////////////////////////////////////////////////////////

 bool LoadDLLsAndLinkToFunctions(void);
};
