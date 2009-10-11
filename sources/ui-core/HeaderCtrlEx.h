
#pragma once

class AFX_EXT_CLASS CHeaderCtrlEx : public CHeaderCtrl
{
 public:
  CHeaderCtrlEx();
  virtual ~CHeaderCtrlEx();

  BOOL Init(CHeaderCtrl *pHeader);

  void SetTextColor(COLORREF i_color); 

 protected:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  DECLARE_MESSAGE_MAP()

 private:
  COLORREF m_text_color;
};
