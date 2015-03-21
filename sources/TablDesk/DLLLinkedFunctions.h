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

/** \file DLLLinkedFunctions.h
 * \author Alexey A. Shabelnikov
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
 typedef void (__cdecl *OnGetAxisLabel)(LPTSTR io_label_string, int index, void* i_param);
 typedef void (__cdecl *OnWndActivation)(void* i_param, long i_cmd);
 typedef void (__cdecl *OnChangeValue)(void* i_param, int i_type, float i_value);
 //2D
 typedef HWND (__cdecl *Chart2DCreate_Addr) (const float *ip_original_function, float *iop_modified_function, float i_aai_min, float i_aai_max, const float *ip_x_axis_grid_values, int i_count_of_points, LPCTSTR i_x_axis_title, LPCTSTR i_y_axis_title, LPCTSTR i_chart_title, int bins_mode);
 typedef void (__cdecl *Chart2DUpdate_Addr) (HWND hWnd, const float *ip_original_function, float *iop_modified_function);
 typedef void (__cdecl *Chart2DSetOnChange_Addr) (HWND hWnd, EventHandler i_pOnChange, void* i_param);
 typedef void (__cdecl *Chart2DSetOnClose_Addr) (HWND hWnd, EventHandler i_pOnClose, void* i_param);
 typedef void (__cdecl *Chart2DSetMarksVisible_Addr) (HWND hWnd, int i_series_index, bool i_visible);
 typedef void (__cdecl *Chart2DSetAxisValuesFormat_Addr) (HWND hWnd, int i_axis, LPCTSTR i_format_string);
 typedef void (__cdecl *Chart2DSetOnGetAxisLabel_Addr) (HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
 typedef void (__cdecl *Chart2DInverseAxis_Addr) (HWND hWnd, int i_axis, bool i_inverted);
 typedef void (__cdecl *Chart2DShow_Addr) (HWND hWnd, int i_show);
 typedef void (__cdecl *Chart2DSetLanguage_Addr) (int i_language);
 typedef void (__cdecl *Chart2DSetOnWndActivation_Addr)(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param);
 typedef void (__cdecl *Chart2DEnable_Addr)(HWND hWnd, bool i_enable);
 typedef void (__cdecl *Chart2DSetAxisEdits_Addr)(HWND hWnd, int i_axis, int i_show, float i_beginLo, float i_beginHi, float i_endLo, float i_endHi, float i_step, OnChangeValue i_pOnChangeValue, void* i_param);
 typedef void (__cdecl *Chart2DUpdateAxisEdits_Addr)(HWND hWnd, int i_axis, float i_begin, float i_end);
 typedef void (__cdecl *Chart2DShowHints_Addr) (int i_show);
 typedef void (__cdecl *Chart2DSetPtValuesFormat_Addr) (HWND hWnd, LPCTSTR ptValFormat);
 typedef void (__cdecl *Chart2DSetPtMovingStep_Addr) (HWND hWnd, float step);

 //3D
 typedef HWND (__cdecl *Chart3DCreate_Addr) (const float *ip_original_function, float *iop_modified_function, const float *ip_x_axis_grid_values, int i_x_count_of_points, int i_z_count_of_points, float i_aai_min, float i_aai_max, LPCTSTR i_x_axis_title, LPCTSTR i_y_axis_title, LPCTSTR i_chart_title);
 typedef void (__cdecl *Chart3DUpdate_Addr) (HWND hWnd, const float *ip_original_function, float *iop_modified_function);
 typedef void (__cdecl *Chart3DSetOnChange_Addr) (HWND hWnd, EventHandler i_pOnChange, void* i_param);
 typedef void (__cdecl *Chart3DSetOnClose_Addr) (HWND hWnd, EventHandler i_pOnClose, void* i_param);
 typedef void (__cdecl *Chart3DSetOnGetAxisLabel_Addr) (HWND hWnd, int i_axis, OnGetAxisLabel i_pOnGetAxisLabel, void* i_param);
 typedef void (__cdecl *Chart3DShow_Addr) (HWND hWnd, int i_show);
 typedef void (__cdecl *Chart3DSetLanguage_Addr) (int i_language);
 typedef void (__cdecl *Chart3DSetOnWndActivation_Addr)(HWND hWnd, OnWndActivation i_pOnWndActivation, void* i_param);
 typedef void (__cdecl *Chart3DEnable_Addr)(HWND hWnd, bool i_enable);
 typedef void (__cdecl *Chart3DShowHints_Addr) (int i_show);
 typedef void (__cdecl *Chart3DSetPtValuesFormat_Addr) (HWND hWnd, LPCTSTR ptValFormat);
 typedef void (__cdecl *Chart3DSetPtMovingStep_Addr) (HWND hWnd, float step);

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
 extern Chart2DEnable_Addr              Chart2DEnable;
 extern Chart2DSetAxisEdits_Addr        Chart2DSetAxisEdits;
 extern Chart2DUpdateAxisEdits_Addr     Chart2DUpdateAxisEdits;
 extern Chart2DShowHints_Addr           Chart2DShowHints;
 extern Chart2DSetPtValuesFormat_Addr   Chart2DSetPtValuesFormat;
 extern Chart2DSetPtMovingStep_Addr     Chart2DSetPtMovingStep;

 extern Chart3DCreate_Addr              Chart3DCreate;
 extern Chart3DUpdate_Addr              Chart3DUpdate;
 extern Chart3DSetOnChange_Addr         Chart3DSetOnChange;
 extern Chart3DSetOnClose_Addr          Chart3DSetOnClose;
 extern Chart3DSetOnGetAxisLabel_Addr   Chart3DSetOnGetAxisLabel;
 extern Chart3DShow_Addr                Chart3DShow;
 extern Chart3DSetLanguage_Addr         Chart3DSetLanguage;
 extern Chart3DSetOnWndActivation_Addr  Chart3DSetOnWndActivation;
 extern Chart3DEnable_Addr              Chart3DEnable;
 extern Chart3DShowHints_Addr           Chart3DShowHints;
 extern Chart3DSetPtValuesFormat_Addr   Chart3DSetPtValuesFormat;
 extern Chart3DSetPtMovingStep_Addr     Chart3DSetPtMovingStep;

 //Set language
 DLLLF_API void SetLanguage(int language);

 //Does all work
 DLLLF_API bool LoadDLLsAndLinkToFunctions(void);

 //Set hints' showing mode
 DLLLF_API void ShowHints(bool i_show);
};
