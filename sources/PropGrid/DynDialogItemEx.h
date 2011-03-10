// DynDialogItemEx.h: interface for the CDynDialogItemEx class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

typedef enum
{
 NOCONTROL    = 0,
 BUTTON       = 0x0080,
 EDITCONTROL  = 0x0081,
 STATICTEXT   = 0x0082,
 LISTBOX      = 0x0083,
 HSCROLL      = 0x0084,
 COMBOBOX     = 0x0085,
 SPIN,
 PROGRES,
 SLIDER,
 HOTKEY,
 LISTCTRL,
 TREECTRL,
 TABCTRL,
 ANIMATE,
 RICHEDIT,
 DATETIMEPICKER,
 MONTHCALENDER,
 IPADRESS,
 COMBOBOXEX
} DLGITEMTEMPLATECONTROLS;

class CDynDialogItemEx : public CWnd
{
 public:
  // Construction
  CDynDialogItemEx();
//virtual ~CDynDialogItemEx();

  // Operations
  UINT InitDialogItem(DLGITEMTEMPLATECONTROLS TypeControl,
    DWORD dwStyle,
    DWORD dwExtendedStyle,
    LPRECT pRect,
    LPCTSTR lpszCaption,
    UINT nID = 0,
    BOOL bSubclassed = FALSE,
    void *pData = NULL);
  UINT InitDialogItem(LPCTSTR lpszClassName,
    DWORD dwStyle,
    DWORD dwExtendedStyle,
    LPRECT pRect,
    LPCTSTR lpszCaption,
    UINT nID = 0,
    BOOL bSubclassed = FALSE,
    void *pData = NULL);
  BOOL CreateEx(CWnd *pParent);
  void SetFont(CFont* pFont, BOOL bRedraw = TRUE);

 public:
  BOOL SetWindowPos(CWnd *pParent);
  PBYTE FillBufferWithItemTemplate(BYTE* pdest);

  UINT GetControlID()                       {return m_ControlID;}
  DLGITEMTEMPLATECONTROLS GetControlType()  {return m_eTypeControl;}
  long GetCaptionLength()                   {return m_strCaption.GetLength();}
  BOOL IsDlgItemSubclassed()                {return m_bSubclassed;}
  CString GetClassName()                    {return m_strClassName;}

  virtual void DoDataExchange(CDataExchange* pDX);
  // Generated message map functions
 protected:
  DECLARE_MESSAGE_MAP()

  BOOL IsDataMemberPointerToWnd();
  BSTR GetRuntimeLicense(CString &strControlName);
  TCHAR* GetClassNameByType(DLGITEMTEMPLATECONTROLS TypeControl);

 protected:
  DLGITEMTEMPLATECONTROLS GetClassTypeByName(LPCTSTR lpszClassName);

  CString                  m_strClassName;
  DLGITEMTEMPLATECONTROLS  m_eTypeControl;
  UINT                     m_ControlID;
  UINT                     m_dwStyle;
  UINT                     m_dwStyleEx;
  CRect                    m_Rect;
  CString                  m_strCaption;
  void                    *m_pData;
  BOOL                     m_bSubclassed;
};
