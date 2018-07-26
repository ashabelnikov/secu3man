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

#undef max //avoid conflicts with C++

template <class T> struct OptField_t
{
 OptField_t() {}
 OptField_t(const _TSTRING& n) : name(n) {}
 _TSTRING name;
 T value;
};

class IniIO
{
 public:
  IniIO(const _TSTRING& fileName, const _TSTRING& sectionName)
  : m_sectionName(sectionName)
  , m_fileName(fileName)
  {}
  IniIO(const CString& fileName, const CString& sectionName)
  : m_sectionName(sectionName)
  , m_fileName(fileName)
  {}

  bool ReadString(OptField_t<_TSTRING>& field, const _TSTRING& defVal)
  {
   TCHAR read_str[1024];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 1023, m_fileName.c_str());
   field.value = read_str;
   return true;
  }

  bool ReadDword(OptField_t<DWORD>& field, const _TSTRING& defVal, const std::vector<DWORD>& patterns)
  {
   TCHAR read_str[256];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   field.value = _ttoi(read_str);
   if (patterns.end() == std::find(patterns.begin(), patterns.end(), field.value))
   {
    field.value = _ttoi(defVal.c_str());
    return false;
   }
   return true; //ok  
  }

  bool WriteDword(const OptField_t<DWORD>& field)
  {
   CString str;
   str.Format(_T("%d"), field.value);
   WritePrivateProfileString(m_sectionName.c_str(), field.name.c_str(), str, m_fileName.c_str());
   return true;
  }

  bool ReadColor(OptField_t<DWORD>& field, const _TSTRING& defVal)
  {
   bool status = true;
   TCHAR read_str[256];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   field.value = _tcstol(read_str, NULL, 16);
   if (field.value > 0xFFFFFF)
   {
    field.value = 0xFFFFFF;
    status = false;
   }
   field.value = swapRB(field.value); //swap R and B components
   return status;
  }

  bool WriteColor(const OptField_t<DWORD>& field)
  {
   CString str;
   str.Format(_T("%06X"), (int)swapRB(field.value));
   WritePrivateProfileString(m_sectionName.c_str(), field.name.c_str(), str, m_fileName.c_str());
   return true;
  }

  bool WriteString(const OptField_t<_TSTRING>& field)
  {
   WritePrivateProfileString(m_sectionName.c_str(), field.name.c_str(), field.value.c_str(), m_fileName.c_str());
   return true;
  }

  bool CreateSection(void)
  {
   WritePrivateProfileSection(m_sectionName.c_str(), _T(""), m_fileName.c_str());
   return true;
  }

  bool ReadEnum(OptField_t<int>& field, size_t defValIdx, const std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> >& patterns)
  {
   TCHAR read_str[256];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), patterns[defValIdx].first.second.c_str(), read_str, 255, m_fileName.c_str());
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

  bool WriteEnum(const OptField_t<int>& field, const std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> >& patterns)
  {
   _TSTRING write_str;
   for(size_t i = 0; i < patterns.size(); ++i)
   {
    if (field.value == patterns[i].second)
     write_str = patterns[i].first.second.c_str();
   }
   WritePrivateProfileString(m_sectionName.c_str(), field.name.c_str(), write_str.c_str(), m_fileName.c_str());
   return true;
  }

  template <class T>
  bool ReadInt(OptField_t<T>& field, const _TSTRING& defVal, const std::vector<std::pair<_TSTRING, T> >& patterns)
  {
   TCHAR read_str[256];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
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
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   if (ltrim(read_str).empty() && stubIfEmpty)
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
  bool WriteInt(const OptField_t<T>& field)
  {
   CString str;
   if ((int)field.value != std::numeric_limits<int>::max())
    str.Format(_T("%d"), (int)field.value);
   WritePrivateProfileString(m_sectionName.c_str(), field.name.c_str(), str, m_fileName.c_str());
   return true;
  }

  //Result will be std::numeric_limits<int>::max() if field is empty or value is incorrect
  bool ReadWndPos(OptField_t<POINT>& field, int minVal = -10000, int maxVal = 10000)
  {
   int value_x = 0, value_y = 0;
   TCHAR read_str[256];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), _T(""), read_str, 255, m_fileName.c_str());
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

  bool WriteWndPos(const OptField_t<POINT>& field)
  {
   CString str;
   if (field.value.x != std::numeric_limits<int>::max() && field.value.y != std::numeric_limits<int>::max())
    str.Format(_T("%d,%d"), (int)field.value.x, (int)field.value.y);
   WritePrivateProfileString(m_sectionName.c_str(), field.name.c_str(), str, m_fileName.c_str());
   return true;
  }

  bool ReadFlt(OptField_t<float>& field, const _TSTRING& defVal, float minVal, float maxVal)
  {
   TCHAR read_str[256];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 255, m_fileName.c_str());
   if (_stscanf(read_str, _T("%f"), &field.value) != 1 || field.value < minVal || field.value > maxVal)
   {
    _stscanf(defVal.c_str(), _T("%f"), &field.value);
    return false;
   }
   return true;
  }

  bool WriteFlt(const OptField_t<float>& field, int decPlaces)
  {
   CString str;
   str.Format(_T("%.*f"), decPlaces, field.value);
   WritePrivateProfileString(m_sectionName.c_str(), field.name.c_str(), str, m_fileName.c_str());
   return true;
  }

  bool ReadVector(OptField_t<std::vector<int> >& field, const _TSTRING& defVal, int minVal, int maxVal, int num)
  {
   int value = 0;
   TCHAR read_str[1024];
   GetPrivateProfileString(m_sectionName.c_str(), field.name.c_str(), defVal.c_str(), read_str, 1023, m_fileName.c_str());
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

  bool WriteVector(const OptField_t<std::vector<int> >& field)
  {
   CString str;
   for(size_t i = 0; i < field.value.size(); ++i)
   {
    CString s;
    if (i!=0) str+=_T(",");
    s.Format(_T("%d"), field.value[i]);
    str+=s;
   }
   WritePrivateProfileString(m_sectionName.c_str(), field.name.c_str(), str, m_fileName.c_str());
   return true;
  }

 private:
  _TSTRING ltrim(const _TSTRING& str)
  {
   size_t first = str.find_first_not_of(_T(' '));
   if (_TSTRING::npos == first)
    return str;
   return str.substr(first, _TSTRING::npos);
  }

  COLORREF swapRB(DWORD rgb)
  {
   return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
  }

  const _TSTRING m_sectionName;
  const _TSTRING m_fileName;
};
