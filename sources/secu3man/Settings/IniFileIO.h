/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file IniFileIO.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include <algorithm>
#include <vector>
#include <limits>
#include "common/StrUtils.h"
#include "common/GDIHelpers.h"
#include "ui-core/MsgBox.h"

#undef max //avoid conflicts with C++

template <class T> struct OptField_t
{
 OptField_t() {}
 OptField_t(const _TSTRING& n) : name(n) {}
 _TSTRING name;
 T value;
};

//Special wrapper function for skipping of line comments
static DWORD GetPrivateProfileStringCT(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpFileName)
{
 DWORD result = GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
 if (result)
 {
  TCHAR* p =_tcschr(lpReturnedString, _T(';'));
  if (p != NULL)
  {
   do
   { *p = NULL; p--; }
   while(p >= lpReturnedString && *p == _T(' '));
  }
 }
 return result;
}

class IniIO
{
 public:
  IniIO(const CString& fileName, const CString& sectionName)
  : m_sectionName(sectionName)
  , m_fileName(fileName)
  , m_commentsIndent(30)
  , m_fh(NULL)
  , m_curLocale(NULL)
  {
   m_curLocale = _tsetlocale(LC_ALL, NULL); //remember current locale
   _tsetlocale(LC_ALL, _T("English_USA.1252"));
  }

  IniIO(const CString& fileName)
  : m_fileName(fileName)
  , m_commentsIndent(30)
  , m_fh(NULL)
  , m_curLocale(NULL)
  {
   m_curLocale = _tsetlocale(LC_ALL, NULL); //remember current locale
   _tsetlocale(LC_ALL, _T("English_USA.1252"));

   m_fh = _tfopen(m_fileName.c_str(), _T("wb"));
   if (NULL==m_fh)
   {
    _TSTRING str = _T("Can't open file: ");
    str+=m_fileName;
    SECUMessageBox(str.c_str());
   }
  }

  ~IniIO()
  {
   if (m_fh)
   {
    fclose(m_fh);
   }

   _tsetlocale(LC_ALL, m_curLocale); //restore previous locale
  }

  bool ReadString(OptField_t<_TSTRING>& field, const _TSTRING& defVal)
  {
   TCHAR read_str[1024];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 1023, m_fileName.c_str());
   field.value = read_str;
   return true;
  }

  bool ReadDword(OptField_t<DWORD>& field, const _TSTRING& defVal, const std::vector<DWORD>& patterns)
  {
   TCHAR read_str[256];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   field.value = _ttoi(read_str);
   if (patterns.end() == std::find(patterns.begin(), patterns.end(), field.value))
   {
    field.value = _ttoi(defVal.c_str());
    return false;
   }
   return true; //ok  
  }

  bool WriteDword(const OptField_t<DWORD>& field, const _TSTRING& comment = _T(""))
  {
   CString str;
   str.Format(_T("%d"), field.value);
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);   
   return true;
  }

  bool ReadColor(OptField_t<DWORD>& field, const _TSTRING& defVal)
  {
   bool status = true;
   TCHAR read_str[256];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   field.value = _tcstol(read_str, NULL, 16);
   if (field.value > 0xFFFFFF)
   {
    field.value = 0xFFFFFF;
    status = false;
   }
   field.value = GDIHelpers::swapRB(field.value); //swap R and B components
   return status;
  }

  bool WriteColor(const OptField_t<DWORD>& field, const _TSTRING& comment = _T(""))
  {
   CString str;
   str.Format(_T("%06X"), (int)GDIHelpers::swapRB(field.value));
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);
   return true;
  }

  bool WriteString(const OptField_t<_TSTRING>& field, const _TSTRING& comment = _T(""))
  {
   CString str = field.value.c_str();
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);
   return true;
  }

  bool CreateSection(const CString& sectionName)
  {
   if (NULL==m_fh)
    return false;
   _ftprintf(m_fh, _T("[%s]\r\n"), sectionName);
   return true;
  }

  bool ReadEnum(OptField_t<int>& field, size_t defValIdx, const std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> >& patterns)
  {
   TCHAR read_str[256];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), patterns[defValIdx].first.second.c_str(), read_str, 255, m_fileName.c_str());
   size_t count = patterns.size();
   for(size_t i = 0; i < count; ++i)
   {
    if (read_str == patterns[i].first.second)
    {
     field.value = patterns[i].second;
     return true;
    }
   }
   field.value = patterns[defValIdx].second; //zero item by default
   return false;
  }

  bool WriteEnum(const OptField_t<int>& field, const std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> >& patterns, const _TSTRING& comment = _T(""))
  {
   CString write_str;
   for(size_t i = 0; i < patterns.size(); ++i)
   {
    if (field.value == patterns[i].second)
     write_str = patterns[i].first.second.c_str();
   }
   AddComment(write_str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), write_str);
   return true;
  }

  template <class T>
  bool ReadInt(OptField_t<T>& field, const _TSTRING& defVal, const std::vector<std::pair<_TSTRING, T> >& patterns)
  {
   TCHAR read_str[256];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   field.value = _ttoi(read_str);
   size_t count = patterns.size();
   for(size_t i = 0; i < count; i++)
   {
    if (field.value == patterns[i].second)
     return true;
   }
   field.value = _ttoi(defVal.c_str()); //error: use default
   return false;
  }

  //Result will be std::numeric_limits<int>::max() if field is empty and stubIfEmpty == true
  template <class T>
  bool ReadInt(OptField_t<T>& field, const _TSTRING& defVal, int minVal, int maxVal, bool stubIfEmpty = false)
  {
   TCHAR read_str[256];
   int value = 0;
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   if (StrUtils::ltrim(read_str).empty() && stubIfEmpty)
   {
    field.value = std::numeric_limits<int>::max();
    return true; //ok
   }
   if (_stscanf(read_str, _T("%d"), &value) != 1 || ((int)value) < minVal || ((int)value) > maxVal)
   {
    field.value = _ttoi(defVal.c_str());
    return false;
   }
   field.value = value;
   return true;
  }

  template <class T>
  bool WriteInt(const OptField_t<T>& field, const _TSTRING& comment = _T(""))
  {
   CString str;
   if ((int)field.value != std::numeric_limits<int>::max())
    str.Format(_T("%d"), (int)field.value);
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);
   return true;
  }

  //Result will be std::numeric_limits<int>::max() if field is empty or value is incorrect
  bool ReadWndPos(OptField_t<POINT>& field, int minVal = -10000, int maxVal = 10000)
  {
   int value_x = 0, value_y = 0;
   TCHAR read_str[256];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), _T(""), read_str, 255, m_fileName.c_str());
   int result = _stscanf(read_str, _T("%d,%d"), &value_x, &value_y);
   if (result != 2 || (value_x < minVal) || (value_x > maxVal) || (value_y < minVal) || (value_y > maxVal))
   {
    field.value.x = std::numeric_limits<int>::max();
    field.value.y = std::numeric_limits<int>::max();
    return false;
   }
   field.value.x = value_x;
   field.value.y = value_y;
   return true;
  }

  bool WriteWndPos(const OptField_t<POINT>& field, const _TSTRING& comment = _T(""))
  {
   CString str;
   if (field.value.x != std::numeric_limits<int>::max() && field.value.y != std::numeric_limits<int>::max())
    str.Format(_T("%d,%d"), (int)field.value.x, (int)field.value.y);
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);
   return true;
  }

  bool ReadFlt(OptField_t<float>& field, const _TSTRING& defVal, float minVal, float maxVal)
  {
   TCHAR read_str[256];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   if (_stscanf(read_str, _T("%f"), &field.value) != 1 || field.value < minVal || field.value > maxVal)
   {
    _stscanf(defVal.c_str(), _T("%f"), &field.value);
    return false;
   }
   return true;
  }

  bool WriteFlt(const OptField_t<float>& field, int decPlaces, const _TSTRING& comment = _T(""))
  {
   CString str;
   str.Format(_T("%.*f"), decPlaces, field.value);
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);
   return true;
  }

  bool ReadVector(OptField_t<std::vector<int> >& field, const _TSTRING& defVal, int minVal, int maxVal, int num)
  {
   int value = 0;
   TCHAR read_str[1024];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 1023, m_fileName.c_str());
   std::vector<int> vect;
   std::vector<_TSTRING> tokens = StrUtils::TokenizeStr(read_str, _T(','));
   for (size_t i = 0; i < tokens.size(); ++i)
   {
    int result = _stscanf(tokens[i].c_str(), _T("%d"), &value);
    vect.push_back(value);
    if (result != 1 || (value < minVal) || (value > maxVal) || (num != -1 && tokens.size() != num))
    {
     std::vector<_TSTRING> tokens = StrUtils::TokenizeStr(defVal.c_str(), _T(','));
     field.value.clear(); 
     for (size_t i = 0; i < tokens.size(); ++i)
     {
      _stscanf(tokens[i].c_str(), _T("%d"), &value);
      field.value.push_back(value);
     }
     return false;
    }
   }
   field.value = vect;
   return true;
  }

  bool WriteVector(const OptField_t<std::vector<int> >& field, const _TSTRING& comment = _T(""))
  {
   CString str;
   for(size_t i = 0; i < field.value.size(); ++i)
   {
    CString s;
    if (i!=0) str+=_T(",");
    s.Format(_T("%d"), field.value[i]);
    str+=s;
   }
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);
   return true;
  }

  bool ReadVector(OptField_t<std::vector<float> >& field, const _TSTRING& defVal, float minVal, float maxVal, int num)
  {
   float value = 0;
   TCHAR read_str[1024];
   GetPrivateProfileStringCT(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 1023, m_fileName.c_str());
   std::vector<float> vect;
   std::vector<_TSTRING> tokens = StrUtils::TokenizeStr(read_str, _T(','));
   for (size_t i = 0; i < tokens.size(); ++i)
   {
    int result = _stscanf(tokens[i].c_str(), _T("%f"), &value);
    vect.push_back(value);
    if (result != 1 || (value < minVal) || (value > maxVal) || (num != -1 && tokens.size() != num))
    {
     std::vector<_TSTRING> tokens = StrUtils::TokenizeStr(defVal.c_str(), _T(','));
     field.value.clear(); 
     for (size_t i = 0; i < tokens.size(); ++i)
     {
      _stscanf(tokens[i].c_str(), _T("%f"), &value);
      field.value.push_back(value);
     }
     return false;
    }
   }
   field.value = vect;
   return true;
  }

  bool WriteVector(const OptField_t<std::vector<float> >& field, int decPlaces, const _TSTRING& comment = _T(""))
  {
   CString str;
   for(size_t i = 0; i < field.value.size(); ++i)
   {
    CString s;
    if (i!=0) str+=_T(",");
    s.Format(_T("%.*f"), decPlaces, field.value[i]);
    str+=s;
   }
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);
   return true;
  }

  //Add to file a full line comment
  bool WriteComment(const _TSTRING& text, bool precedEmptyLine = false)
  {
   if (precedEmptyLine)
    _ftprintf(m_fh, _T("\r\n"));
   if (_ftprintf(m_fh, _T("; %s\r\n"), text.c_str()) < 0)
    return false;
   return true;
  }

  bool ReadChar(OptField_t<char>& field, const _TSTRING& defVal, const std::vector<std::pair<_TSTRING, char> >& patterns)
  {
   TCHAR read_str[256];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   TCHAR* p = read_str;
   while(*p==_T(' ') || *p==_T('\'')) ++p; //skip symbols
   if (*p)
   {
    field.value = *p;
    size_t count = patterns.size();
    for(size_t i = 0; i < count; i++)
    {
     if (field.value == patterns[i].second)
      return true;
    }
   }
   field.value = defVal.c_str()[0]; //error: use default
   return false;
  }

  bool WriteChar(const OptField_t<char>& field, const _TSTRING& comment = _T(""))
  {
   CString str;
   str+=_T('\'');
   str+= field.value;
   str+=_T('\'');
   AddComment(str, field.name, comment); //add optional comment
   _ftprintf(m_fh, _T("%s=%s\r\n"), field.name.c_str(), str);
   return true;
  }

 private:
  void AddComment(CString& str, const _TSTRING& fieldName, const _TSTRING& comment)
  {
   if (!comment.empty())
   {
    while((str.GetLength() + fieldName.size() + 3) < m_commentsIndent)
     str+=_T(" ");
    str = str + _T(" ;") + comment.c_str(); // append comment if exists
   }
  }

  const _TSTRING m_sectionName;
  const _TSTRING m_fileName;
  size_t m_commentsIndent;
  FILE* m_fh;
  TCHAR* m_curLocale;
};
