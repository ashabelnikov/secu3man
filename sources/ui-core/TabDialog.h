
#pragma once

#include "UpdatableDialog.h"

class AFX_EXT_CLASS CTabDialog : public CUpdatableDialog 
{
	typedef CUpdatableDialog Super;

  public:
  	CTabDialog(UINT nIDTemplate, CWnd* pParentWnd);

	//возвращает идентификатор диалога
	virtual LPCTSTR GetDialogID(void) const = 0;

	virtual ~CTabDialog();

	//диалоговые вкладки в Tab-контроле нельзя закрывать по IDOK или IDCANCEL
	void OnOK();
	void OnCancel();
};


