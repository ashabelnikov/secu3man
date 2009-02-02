 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "DLLLinkedFunctions.h"

namespace DLL
{
  //глобальная таблица функций из DLL
  UOZ1_Chart2DCreate_Addr        UOZ1_Chart2DCreate = NULL;
  UOZ1_Chart2DUpdate_Addr        UOZ1_Chart2DUpdate = NULL;
  UOZ1_Chart2DSetOnChange_Addr   UOZ1_Chart2DSetOnChange = NULL;
  UOZ1_Chart2DSetOnClose_Addr    UOZ1_Chart2DSetOnClose = NULL;
  UOZ1_Chart2DSetMarksVisible_Addr UOZ1_Chart2DSetMarksVisible = NULL;

  UOZ2_Chart3DCreate_Addr        UOZ2_Chart3DCreate = NULL;
  UOZ2_Chart3DUpdate_Addr        UOZ2_Chart3DUpdate = NULL;
  UOZ2_Chart3DSetOnChange_Addr   UOZ2_Chart3DSetOnChange = NULL;
  UOZ2_Chart3DSetOnClose_Addr    UOZ2_Chart3DSetOnClose = NULL;
	
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
	  UOZ1_Chart2DCreate = NULL; 
	  UOZ1_Chart2DUpdate = NULL;
	  UOZ1_Chart2DSetOnChange = NULL;
	  UOZ1_Chart2DSetOnClose = NULL;
	  UOZ1_Chart2DSetMarksVisible = NULL;
	  status = false;
	}
    else
	{
      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart2DCreate");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ1_Chart2DCreate = NULL;
	    status = false;
	  }
      else
        UOZ1_Chart2DCreate = (UOZ1_Chart2DCreate_Addr)addr;

      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart2DUpdate");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ1_Chart2DUpdate = NULL;
	    status = false;
	  }
      else
        UOZ1_Chart2DUpdate = (UOZ1_Chart2DUpdate_Addr)addr;	
      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart2DSetOnChange");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ1_Chart2DSetOnChange = NULL;
	    status = false;
	  }
      else
        UOZ1_Chart2DSetOnChange = (UOZ1_Chart2DSetOnChange_Addr)addr;	
      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart2DSetOnClose");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ1_Chart2DSetOnClose = NULL;
	    status = false;
	  }
      else
        UOZ1_Chart2DSetOnClose = (UOZ1_Chart2DSetOnClose_Addr)addr;	
      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart2DSetMarksVisible");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ1_Chart2DSetMarksVisible = NULL;
	    status = false;
	  }
      else
        UOZ1_Chart2DSetMarksVisible = (UOZ1_Chart2DSetMarksVisible_Addr)addr;	
      //-------------------------------------------------------------------------- 	
	}



    hModule = LoadLibrary(_T("UOZ2.dll"));
    if (hModule==NULL)
	{
  	  AfxMessageBox(_T("Can't load library UOZ2.dll"),MB_OK|MB_ICONSTOP);
	  UOZ2_Chart3DCreate = NULL; 
	  UOZ2_Chart3DUpdate = NULL;
	  UOZ2_Chart3DSetOnChange = NULL;
	  UOZ2_Chart3DSetOnClose = NULL;
	  status = false;
	}
    else
	{
      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart3DCreate");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ2_Chart3DCreate = NULL;
	    status = false;
	  }
      else
        UOZ2_Chart3DCreate = (UOZ2_Chart3DCreate_Addr)addr;

      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart3DUpdate");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ2_Chart3DUpdate = NULL;
	    status = false;
	  }
      else
        UOZ2_Chart3DUpdate = (UOZ2_Chart3DUpdate_Addr)addr;	
      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart3DSetOnChange");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ2_Chart3DSetOnChange = NULL;
	    status = false;
	  }
      else
        UOZ2_Chart3DSetOnChange = (UOZ2_Chart3DSetOnChange_Addr)addr;	
      //-------------------------------------------------------------------------- 
      addr = GetProcAddress(hModule,"Chart3DSetOnClose");
      if (addr==NULL)
	  {
	    AfxMessageBox(_T("Error: GetProcAddress"),MB_OK|MB_ICONSTOP);	
	    UOZ2_Chart3DSetOnClose = NULL;
	    status = false;
	  }
      else
        UOZ2_Chart3DSetOnClose = (UOZ2_Chart3DSetOnClose_Addr)addr;	
      //--------------------------------------------------------------------------       
	}

  return status;
  }

};//namespace