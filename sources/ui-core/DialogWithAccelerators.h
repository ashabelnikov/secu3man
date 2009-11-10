
#pragma once

// Класс добавляет акселераторы (из главного окна)
class AFX_EXT_CLASS CDialogWithAccelerators : public CDialog
{
 typedef CDialog Super;

 public:
  CDialogWithAccelerators(UINT nIDTemplate, CWnd* pParentWnd = NULL );      

 protected:
  //здесь будет грязный хак! 
  virtual BOOL PreTranslateMessage(MSG* pMsg);
};
