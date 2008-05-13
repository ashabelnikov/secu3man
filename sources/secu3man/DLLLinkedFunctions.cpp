 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "secu3man.h"
#include "DLLLinkedFunctions.h"

namespace DLL
{
  //глобальная таблица функций из DLL
  DisplayChart_UOZ1_Addr  DisplayChart_UOZ1 = NULL;
  DisplayChart_UOZ2_Addr  DisplayChart_UOZ2 = NULL;
  //---------------------------------------------------------

  //Заполняет таблицу функций (загружает необходимые библиотеки и получает адреса функций, сохраняя их в таблицу)
  bool LoadDLLsAndLinkToFunctions(void)
  {
    HMODULE hModule;
    FARPROC addr;
    bool status = true;

    hModule = LoadLibrary(_T("UOZ1.dll"));
    if (hModule==NULL)
	{
	  AfxMessageBox(_T("Can't load library UOZ1.dll"),MB_OK|MB_ICONSTOP);
	  DisplayChart_UOZ1 = NULL; 
	  status = false;
	}
    else
	{
      addr = GetProcAddress(hModule,"DisplayChart_AAoI_2D");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    DisplayChart_UOZ1 = NULL;
	    status = false;
	  }
      else
        DisplayChart_UOZ1 = (DisplayChart_UOZ1_Addr)addr;
	}



    hModule = LoadLibrary(_T("UOZ2.dll"));
    if (hModule==NULL)
	{
  	  AfxMessageBox(_T("Can't load library UOZ2.dll"),MB_OK|MB_ICONSTOP);
	  DisplayChart_UOZ2 = NULL;
	  status = false;
	}
    else
	{
      addr = GetProcAddress(hModule,"DisplayChart_AAoI_3D");
	  if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    DisplayChart_UOZ2 = NULL;
	    status = false;
	  }
      else
	    DisplayChart_UOZ2 = (DisplayChart_UOZ2_Addr)addr;
	}

  return status;
  }

};//namespace