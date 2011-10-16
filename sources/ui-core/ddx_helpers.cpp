 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "ddx_helpers.h"

void DDX_HELPERS_API DDX_Check_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value )
{
 BOOL b_value;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_Check(pDX, nIDC, b_value);
  value = (b_value) ? 1 : 0;
 }
 else
 {
  b_value = (value) ? TRUE : FALSE;
  DDX_Check(pDX, nIDC, b_value);
 }
}

void DDX_HELPERS_API DDX_CBIndex_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& index)
{
 int i_index;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_CBIndex(pDX, nIDC, i_index);
  index = (UCHAR)i_index;
 }
 else
 {
  i_index = (int)index;
  DDX_CBIndex(pDX, nIDC, i_index);
 }
}

void DDX_HELPERS_API DDX_Radio_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value )
{
 int b_value;
 if (pDX->m_bSaveAndValidate)
 {
  DDX_Radio(pDX, nIDC, b_value);
  value = (b_value) ? 1 : 0;
 }
 else
 {
  b_value = (value) ? TRUE : FALSE;
  DDX_Radio(pDX, nIDC, b_value);
 }
}

void DDX_HELPERS_API DDX_Text_Fmt(CDataExchange* pDX, int nIDC, float& value, LPCTSTR OutFmt, LPCTSTR InFmt)
{
 CString temp;
 if (!pDX->m_bSaveAndValidate)
  temp.Format(OutFmt, value);

 DDX_Text(pDX, nIDC, temp);

 if (pDX->m_bSaveAndValidate)
  _stscanf(temp, InFmt, &value);
}

void DDX_HELPERS_API DDX_Text_Fmt(CDataExchange* pDX, int nIDC, int& value, LPCTSTR OutFmt, LPCTSTR InFmt)
{
 CString temp;
 if (!pDX->m_bSaveAndValidate)
  temp.Format(OutFmt, value);

 DDX_Text(pDX, nIDC, temp);

 if (pDX->m_bSaveAndValidate)
  _stscanf(temp, InFmt, &value);
}
