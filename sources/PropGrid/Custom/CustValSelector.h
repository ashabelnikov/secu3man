/****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#pragma once
#include "..\CustomItem.h"
#include "..\..\common\unicodesupport.h"

class AFX_EXT_CLASS CCustValSelector : public ICustomItem
{
public:
  CCustValSelector(void);

public:
  virtual CPropertyGrid::EEditMode GetEditMode();
  virtual void DrawItem(CDC& dc, CRect rc, bool focused);
  virtual bool OnLButtonDown(CRect rc, CPoint pt);
  virtual void OnMouseMove(CRect rc, CPoint pt);
  virtual void OnLButtonUp(CRect rc, CPoint pt);


  void SetValuesList(const std::vector<_TSTRING> &i_values);
  void SetSelectedIndex(size_t i_value_index);
  size_t GetSelectedIndex(void) const;
  void SetUnitString(const _TSTRING& i_unit_string);

protected:
  bool IsLeftBoundReached(void) const;
  bool IsRightBoundReached(void) const;
  bool IsSelectedIndexInRange(size_t i_index = INT_MAX) const;
  void NotifyAboutChange(void) const;


  std::vector<_TSTRING> m_values;
  size_t m_selected_value_index;
  _TSTRING m_unit_string;

  int m_nButtonPushed; //0: none, 1: left, 2:right
};