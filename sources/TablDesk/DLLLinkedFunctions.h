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

#pragma once

#ifdef DLLLF_EXPORTS
 #define DLLLF_API __declspec(dllexport)
#else
 #define DLLLF_API __declspec(dllimport)
#endif

namespace DLL
{
 typedef void (__cdecl *EventHandler)(void* i_param);
 typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, void* i_param);
 typedef void (__cdecl *OnWndActivation)(void* i_param, long cmd);

 typedef HWND (__cdecl *Chart2DCreate_Addr) (float *original_function, float *modified_function,float aai_min,float aai_max,const float *x_axis_grid_values, int count_of_points, LPCTSTR x_axis_title, LPCTSTR y_axis_title, LPCTSTR chart_title);
 typedef void (__cdecl *Chart2DUpdate_Addr) (HWND hWnd, float *original_function, float *modified_function);
 typedef void (__cdecl *Chart2DSetOnChange_Addr) (HWND hWnd, EventHandler i_pOnChange, void* i_param);
 typedef void (__cdecl *Chart2DSetOnClose_Addr) (HWND hWnd, EventHandler i_pOnClose, void* i_param);
 typedef void (__cdecl *Chart2DSetMarksVisible_Addr) (HWND hWnd, int i_series_index, bool i_visible);
 typedef void (__cdecl *Chart2DSetAxisValuesFormat_Addr) (HWND hWnd, int i_axis, LPCTSTR i_format_string);
 typedef void (__cdecl *Chart2DSetOnGetAxisLabel_Addr) (HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
 typedef void (__cdecl *Chart2DInverseAxis_Addr) (HWND hWnd, int i_axis, bool i_inverted);
 typedef void (__cdecl *Chart2DShow_Addr) (HWND hWnd, int i_show);
 typedef void (__cdecl *Chart2DSetLanguage_Addr) (int i_language);
 typedef void (__cdecl *Chart2DSetOnWndActivation_Addr)(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param);

 typedef HWND (__cdecl *Chart3DCreate_Addr) (float *original_function, float *modified_function,const int *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float aai_min,float aai_max, LPCTSTR x_axis_title, LPCTSTR chart_title);
 typedef void (__cdecl *Chart3DUpdate_Addr) (HWND hWnd, float *original_function, float *modified_function);
 typedef void (__cdecl *Chart3DSetOnChange_Addr) (HWND hWnd, EventHandler i_pOnChange, void* i_param);
 typedef void (__cdecl *Chart3DSetOnClose_Addr) (HWND hWnd, EventHandler i_pOnClose, void* i_param);
 typedef void (__cdecl *Chart3DShow_Addr) (HWND hWnd, int i_show);
 typedef void (__cdecl *Chart3DSetLanguage_Addr) (int i_language);
 typedef void (__cdecl *Chart3DSetOnWndActivation_Addr)(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param);

 ///////////////////////////////////////////////////////////////////
 extern Chart2DCreate_Addr              Chart2DCreate;
 extern Chart2DUpdate_Addr              Chart2DUpdate;
 extern Chart2DSetOnChange_Addr         Chart2DSetOnChange;
 extern Chart2DSetOnClose_Addr          Chart2DSetOnClose;
 extern Chart2DSetMarksVisible_Addr     Chart2DSetMarksVisible;
 extern Chart2DSetAxisValuesFormat_Addr Chart2DSetAxisValuesFormat;
 extern Chart2DSetOnGetAxisLabel_Addr   Chart2DSetOnGetAxisLabel;
 extern Chart2DInverseAxis_Addr         Chart2DInverseAxis;
 extern Chart2DShow_Addr                Chart2DShow;
 extern Chart2DSetLanguage_Addr         Chart2DSetLanguage;
 extern Chart2DSetOnWndActivation_Addr  Chart2DSetOnWndActivation;

 extern Chart3DCreate_Addr              Chart3DCreate;
 extern Chart3DUpdate_Addr              Chart3DUpdate;
 extern Chart3DSetOnChange_Addr         Chart3DSetOnChange;
 extern Chart3DSetOnClose_Addr          Chart3DSetOnClose;
 extern Chart3DShow_Addr                Chart3DShow;
 extern Chart3DSetLanguage_Addr         Chart3DSetLanguage;
 extern Chart3DSetOnWndActivation_Addr  Chart3DSetOnWndActivation;

 //Set language
 DLLLF_API void SetLanguage(int language);

 //Does all work
 DLLLF_API bool LoadDLLsAndLinkToFunctions(void);
};
