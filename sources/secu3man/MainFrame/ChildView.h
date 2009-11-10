
#pragma once

#include "ui-core\DialogWithAccelerators.h"

// CChildView window
class CChildView : public CDialogWithAccelerators
{
 typedef CDialogWithAccelerators Super;

 public:
  CChildView();
  virtual ~CChildView();

  //Возвращает false если ошибка
  bool Create(CWnd* ip_parent);

  // Generated message map functions
 protected:
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
