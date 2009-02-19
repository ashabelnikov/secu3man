
#pragma once

class AFX_EXT_CLASS CHeaderCtrlEx : public CHeaderCtrl
{

public:
	CHeaderCtrlEx();


public:
	BOOL Init(CHeaderCtrl *pHeader);
	virtual ~CHeaderCtrlEx();

    void SetTextColor(COLORREF i_color); 

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()

private:
	COLORREF m_text_color;
};

