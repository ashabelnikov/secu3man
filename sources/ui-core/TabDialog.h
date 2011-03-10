
#pragma once

#include "UpdatableDialog.h"


//Этот класс добавляет функциональность заключающуюся в том, чтобы диалог нельзя было
//закрыть по IDOK или IDCANCEL.
class AFX_EXT_CLASS CTabDialog : public CUpdatableDialog
{
  typedef CUpdatableDialog Super;

 public:
  CTabDialog(UINT nIDTemplate, CWnd* pParentWnd);
  virtual ~CTabDialog();

  //возвращает идентификатор диалога
  virtual LPCTSTR GetDialogID(void) const = 0;

  //диалоговые вкладки в Tab-контроле нельзя закрывать по IDOK или IDCANCEL
  virtual void OnOK();
  virtual void OnCancel();
};
