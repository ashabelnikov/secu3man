
#pragma once

#include "io-core\SECU3IO.h"
#include "IRSView.h"

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg dialog

class AFX_EXT_CLASS CRSDeskDlg : public CDialog, public IRSView
{
 public:
  CRSDeskDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;
    
  //--------interface implementation---------------
  virtual void Show(bool show);
  virtual void Enable(bool enable);
  virtual void SetValues(const SECU3IO::RawSensDat* i_values);
  virtual void GetValues(SECU3IO::RawSensDat* o_values);
  //-----------------------------------------------

  //изменение размеров окна
  void Resize(const CRect& i_rect);

  // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()

 private:
  float m_map_value;
  float m_ubat_value;
  float m_temp_value;
  float m_knock_value;

  CStatic m_map_field;
  CStatic m_ubat_field;
  CStatic m_temp_field;
  CStatic m_knock_field;

  CStatic m_map_caption;
  CStatic m_ubat_caption;
  CStatic m_temp_caption;
  CStatic m_knock_caption;

  CStatic m_map_unit;
  CStatic m_ubat_unit;
  CStatic m_temp_unit;
  CStatic m_knock_unit;
};

/////////////////////////////////////////////////////////////////////////////
