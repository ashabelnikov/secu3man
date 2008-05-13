
#pragma once


typedef HWND (__cdecl *DisplayChart_UOZ1_Addr) (float *original_function, float *modified_function,float aai_min,float aai_max,int *x_axis_grid_values, int count_of_points, LPCTSTR chart_title, int *close_feedback);
typedef HWND (__cdecl *DisplayChart_UOZ2_Addr) (float *original_function, float *modified_function,int *x_axis_grid_values, int x_count_of_points, int z_count_of_points,float aai_min,float aai_max, LPCTSTR x_axis_title, LPCTSTR chart_title, int *close_feedback);


namespace DLL
{
  ///////////////////////////////////////////////////////////////////
  extern DisplayChart_UOZ1_Addr  DisplayChart_UOZ1;
  extern DisplayChart_UOZ2_Addr  DisplayChart_UOZ2;

  ///////////////////////////////////////////////////////////////////

  bool LoadDLLsAndLinkToFunctions(void);
};


