#pragma once
#include "PropertyGridCombo.h"
#include "PropertyGridInPlaceEdit.h"

#include <map>
#include <string>
#include <vector>

// CPropertyGrid

typedef UINT HSECTION;
typedef UINT HITEM;

class AFX_EXT_CLASS CPropertyGrid : public CWnd
{
  DECLARE_DYNAMIC(CPropertyGrid)

 public:
  // display mode
  enum EDisplayMode
  {
   DM_CATEGORIZED = 0,
   DM_ALPHABETICAL,
   DM_NOSORT
  };

  // editing
  enum EEditMode
  {
   EM_INPLACE = 0,
   EM_DROPDOWN,
   EM_MODAL
  };

  enum EItemType
  {
   IT_SELECTOR = 0, //custom selector control (inplace selector)
   IT_STRING,
   IT_TEXT,
   IT_INTEGER,
   IT_DOUBLE,
   IT_COMBO,
   IT_BOOLEAN,
   IT_DATE,
   IT_DATETIME,
   IT_FILE,
   IT_FOLDER,
   IT_COLOR,
   IT_FONT,
   IT_HEX       //Additional mode by A. Shabelnikov
  };

 public:
  CPropertyGrid();
  virtual ~CPropertyGrid();

  // customization
  bool GetShadeTitles();
  void SetShadeTitles(bool shade_titles);
  bool GetDrawLines();
  void SetDrawLines(bool draw_lines);
  bool GetDrawGutter();
  void SetDrawGutter(bool draw_gutter);
  bool GetFocusDisabled();
  void SetFocusDisabled(bool focus_disabled);
  bool GetBoldModified();
  void SetBoldModified(bool bold_modified);
  bool GetBoldEditables();
  void SetBoldEditables(bool bold_editables);

  // gutter width
  int GetGutterWidth();
  void SetGutterWidth(int gutter_width);

  // custom colors
  void SetTextColor(COLORREF clrText);
  void SetTitleColor(COLORREF clrText);
  void SetBackColor(COLORREF clrBack);
  void SetShadeColor(COLORREF clrShade);
  void SetFocusColor(COLORREF clrFocus);
  void SetHiliteColor(COLORREF clrHilite);
  void SetEditableColor(COLORREF clrEditable);
  void SetDisabledColor(COLORREF clrDisabled);

  // localization
  void SetTrueFalseStrings(_TSTRING strTrue, _TSTRING strFalse);
  void SetOkCancelStrings(_TSTRING strOk, _TSTRING strCancel);
  void SetDateTimeStrings(_TSTRING strDate, _TSTRING strTime);
  void SetUndefinedString(_TSTRING strUndefined);
  void SetEmptyString(_TSTRING strEmpty);

  // add a section
  HSECTION AddSection(_TSTRING title, bool collapsed = false, HSECTION after = -1);

  // add items
  HITEM AddStringItem(HSECTION section, _TSTRING name, _TSTRING value, bool editable = true, HITEM after = -1);
  HITEM AddTextItem(HSECTION section, _TSTRING name, _TSTRING value, bool editable = true, HITEM after = -1);
  HITEM AddIntegerItem(HSECTION section, _TSTRING name, int value, _TSTRING format = _T(""), bool editable = true, bool undefined = false, HITEM after = -1, const CPropertyGridInPlaceEdit::InplaceEditParamsEx* ip_ex_params = NULL);
  HITEM AddHexItem(HSECTION section, _TSTRING name, int value, _TSTRING format = _T(""), bool editable = true, bool undefined = false, HITEM after = -1, const CPropertyGridInPlaceEdit::InplaceEditParamsEx* ip_ex_params = NULL);
  HITEM AddDoubleItem(HSECTION section, _TSTRING name, double value, _TSTRING format = _T(""), bool editable = true, bool undefined = false, HITEM after = -1, const CPropertyGridInPlaceEdit::InplaceEditParamsEx* ip_ex_params = NULL);
  HITEM AddComboItem(HSECTION section, _TSTRING name, const std::vector<_TSTRING>& values, int cur, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddBoolItem(HSECTION section, _TSTRING name, bool value, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddDateItem(HSECTION section, _TSTRING name, COleDateTime value, _TSTRING format = _T(""), bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddDateTimeItem(HSECTION section, _TSTRING name, COleDateTime value, _TSTRING format = _T(""), bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddFileItem(HSECTION section, _TSTRING name, _TSTRING value, _TSTRING filter = _T(""), bool editable = true, HITEM after = -1);
  HITEM AddFolderItem(HSECTION section, _TSTRING name, _TSTRING value, _TSTRING title = _T(""), bool editable = true, HITEM after = -1);
  HITEM AddColorItem(HSECTION section, _TSTRING name, COLORREF value, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddFontItem(HSECTION section, _TSTRING name, LOGFONT value, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddSelectorItem(HSECTION section, _TSTRING name, const std::vector<_TSTRING>& values, int cur, _TSTRING unit = _T(""), bool editable = true, bool undefined = false, HITEM after = -1);

  // contents
  void ResetContents();
  bool RemoveSection(HSECTION hs);
  bool RemoveItem(HITEM item);
  void ValidateChanges();

  // status
  int GetNumSections();
  int GetSectionSize(HSECTION hs);

  // get item value
  bool GetItemValue(HITEM item, _TSTRING& strValue) const;
  bool GetItemValue(HITEM item, int& nValue) const;
  bool GetItemValue(HITEM item, double& dValue) const;
  bool GetItemValue(HITEM item, bool& bValue) const;
  bool GetItemValue(HITEM item, COleDateTime& dtValue) const;
  bool GetItemValue(HITEM item, COLORREF& clrValue) const;
  bool GetItemValue(HITEM item, LOGFONT& lfValue) const;

  // set item value
  bool SetItemValue(HITEM item, const _TSTRING strValue);
  bool SetItemValue(HITEM item, const int nValue);
  bool SetItemValue(HITEM item, const double nValue);
  bool SetItemValue(HITEM item, const bool bValue);
  bool SetItemValue(HITEM item, const COleDateTime dtValue);
  bool SetItemValue(HITEM item, const COLORREF clrValue);
  bool SetItemValue(HITEM item, const LOGFONT lfValue);

  // for custom items
  int GetTextMargin();
  CFont* GetFontNormal();
  CFont* GetFontBold();

  // appearance stuff
  void SetDisplayMode(EDisplayMode display_mode);
  void ExpandAll(bool expand);
  void ExpandSection(HSECTION hs, bool expand);
  bool IsSectionCollapsed(HSECTION hs);

  bool GetFocusedItemID(HITEM&) const;
  //returns true if currently focused item is enabled, otherwise false. 
  //If there no focused item, this function will return false.
  bool IsFocusedItemEnabled(void) const;
  void SetEditable(bool i_editable);

 protected:
  class CItem
  {
   public:
    CPropertyGridInPlaceEdit::InplaceEditParamsEx m_SpinEdExParams;
    HITEM m_id;
    bool m_editable;
    bool m_undefined;
    EItemType m_type;
    _TSTRING m_name; //Name of field (string)

    std::vector<_TSTRING> m_options;

    int m_nValue;
    double m_dValue;
    _TSTRING m_strValue;
    bool m_bValue;
    COleDateTime m_dtValue;
    COLORREF m_clrValue;
    LOGFONT m_lfValue;    

    bool m_undefined_old;
    int m_nValue_old;
    double m_dValue_old;
    _TSTRING m_strValue_old;
    bool m_bValue_old;
    COleDateTime m_dtValue_old;
    COLORREF m_clrValue_old;
    LOGFONT m_lfValue_old;

    CRect m_rcName;
    CRect m_rcValue;
    
    //Additional field used for IT_SELECTOR
    _TSTRING m_unit;

    bool operator==(const HITEM& item) const;
    bool operator==(const _TSTRING& name) const;

    void ValidateChanges();
  };

  class CSection
  {
   public:
    HSECTION m_id;
    _TSTRING m_title;
    bool m_collapsed;
    std::vector<CItem> m_items;

    CRect m_rcSign;
    CRect m_rcTitle;

    bool operator==(const HSECTION& section) const;
  };

#if (_MSC_VER >=1300) //under VC6 I had a compilation error!
  friend bool item_alpha_sort(std::vector<CPropertyGrid::CItem>::iterator it1, std::vector<CPropertyGrid::CItem>::iterator it2);
#endif

protected:
  virtual void PreSubclassWindow();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nHitTest, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg LRESULT OnComboSelChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnEditChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnDateChanged(WPARAM wParam, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg UINT OnGetDlgCode();
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  DECLARE_MESSAGE_MAP()

 protected:
  // init control
  void InitControl();

  // drawing
  void DrawItem(CDC& dc, int w, int x, int y, std::vector<CItem>::iterator& it);

  // item management
  CSection* FindSection(HSECTION hs) const;
  CItem* FindItem(HITEM hi) const;
  HITEM AddItem(HSECTION hs, EItemType type, _TSTRING name, void* pValue, bool editable, bool undefined, HITEM after);

  // scrolling stuff
  CScrollBar m_scrollbar;
  bool m_scroll_enabled;
  int GetScrollOffset();
  void RecalcLayout();

  // editing
  EEditMode GetEditMode(CItem& item);
  void DeleteEditControl();
  void EditFocusedItem();

  // movement in list
  void MoveForward(HSECTION& focused_section, HITEM& focused_item);

  // keyboard
  void FocusNextItem();
  void FocusPrevItem();

  //Selected item can be activated by space bar key or mouse click
  void _ActivateFocusedItem(void);

  //Data
private:
  std::vector<CSection> m_sections;

  HSECTION m_focused_section;
  HITEM m_focused_item;

  EDisplayMode m_display_mode;

  bool m_shade_titles;
  bool m_draw_lines;
  bool m_draw_gutter;
  bool m_focus_disabled;
  bool m_bold_modified;
  bool m_bold_editables;

  int m_gutter_width;
  bool m_resizing_gutter;
  CPoint m_ptLast;

  CFont m_fntNormal;
  CFont m_fntBold;

  int m_line_height;

  CRect m_rect_button;
  CWnd* m_control;
  bool m_button_pushed;
  bool m_button_depressed;
  bool m_value_clicked;  

  HSECTION m_section_id;
  HITEM m_item_id;

  _TSTRING m_strTrue;
  _TSTRING m_strFalse;
  _TSTRING m_strOk;
  _TSTRING m_strCancel;
  _TSTRING m_strDate;
  _TSTRING m_strTime;
  _TSTRING m_strUndefined;
  _TSTRING m_strEmpty;

  COLORREF m_clrText;
  COLORREF m_clrTitle;
  COLORREF m_clrBack;
  COLORREF m_clrShade;
  COLORREF m_clrFocus;
  COLORREF m_clrHilite;
  COLORREF m_clrEditable;
  COLORREF m_clrDisabled;
};
