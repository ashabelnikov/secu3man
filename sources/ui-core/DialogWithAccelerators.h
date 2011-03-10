
#pragma once

// Класс добавляет акселераторы (из главного окна)
class AFX_EXT_CLASS CDialogWithAccelerators : public CDialog
{
 typedef CDialog Super;

 public:
  CDialogWithAccelerators(); //modeless
  CDialogWithAccelerators(UINT nIDTemplate, CWnd* pParentWnd = NULL );

 protected:
  //здесь будет грязный хак!
  virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//Диалог с акселераторами который нельзя закрыть по IDOK или IDCANCEL
class AFX_EXT_CLASS CModelessDialog : public CDialogWithAccelerators
{
 typedef CDialogWithAccelerators Super;

 public:
  CModelessDialog(); //modeless
  CModelessDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL );

 protected:
  virtual void OnOK();
  virtual void OnCancel();
};
