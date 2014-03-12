/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include "stdafx.h"
#include <algorithm>
#include <shlwapi.h>
#include <vector>
#include "io-core/FirmwareDataMediator.h"
#include "HexUtils/readhex.h"
#include "Resources/resource.h"

namespace FirmwareFileUtils { 

//Save EEPROM to file
void SaveEEPROMToFile(const BYTE* p_data, const int size)
{
 HANDLE   hFile=0;
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(save.GetPathName(),CFile::modeWrite|CFile::modeCreate,&ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return;
  }
  f.Write(p_data,size);
  f.Close();
  return;
 }
 else
  return;
}

//Save FLASH to file
bool SaveFLASHToFile(const BYTE* p_data, const int size, CFirmwareDataMediator* p_fwdm, CString* o_file_name /* = NULL*/, bool calculate_and_place_crc16/* = false*/)
{
 HANDLE   hFile=0;
 BYTE *save_buff = NULL;

 if (!p_fwdm) return false;

 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(save.GetPathName(),CFile::modeWrite|CFile::modeCreate,&ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false; //ошибка - данные не сохранены
  }

  save_buff = new BYTE[size];
  memcpy(save_buff, p_data,size);

  //вычисляем контрольную сумму и сохраняем ее в массив с прошивкой
  if (calculate_and_place_crc16)
   p_fwdm->CalculateAndPlaceFirmwareCRC(save_buff);

  f.Write(save_buff,size);
  f.Close();
  delete save_buff;

  if (o_file_name!=NULL)
   *o_file_name = save.GetFileName();
  return true; //подтверждение пользователя
 }
 else
  return false; //отказ пользователя
}


namespace {
   struct GenMessage
   {
    CString operator()(const std::vector<int>& sizes, UINT msgRegId)
    {
     CString string;
     CString size_str;
     for(size_t i = 0; i < sizes.size(); ++i)
     {
      if ((sizes.size() - 1 == i) && (sizes.size() > 1))
       size_str = size_str + _T(" ") + MLL::LoadString(IDS_FW_OR) + _T(" ");
      else
       if (0 != i)
        size_str+=(", ");
      CString value;
      value.Format(_T("%d"), sizes[i]);
      size_str+=(value);
     }
     string.Format(MLL::LoadString(msgRegId), size_str);
     return string;
    }
   }; }

//Load EEPROM form a file
//мы заранее знаем размер файла с EEPROM
bool LoadEEPROMFromFile(BYTE* p_data, const std::vector<int>& sizes, int* o_selected_size /*= NULL*/, _TSTRING* o_file_name /*= NULL*/)
{
 HANDLE hFile = 0;
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);
 CString cs;

 if (sizes.empty())
  return false; //error, at least one size must be specified
 std::vector<int>::const_iterator p_size_max = std::max_element(sizes.begin(), sizes.end());

 if (open.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(open.GetPathName(), CFile::modeRead, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false; //error, can't open file
  }

  //Проверка на размер файла (его размер должен соответствовать одному из разрешенных размеров EEPROM)
  std::vector<int>::const_iterator p_size = std::find(sizes.begin(), sizes.end(), f.GetLength());
  if (p_size==sizes.end())
  {
   AfxMessageBox(GenMessage()(sizes, IDS_FW_WRONG_EE_FILE_SIZE));
   f.Close();
   return false; //ошибка
  }

  f.Read(p_data, *p_size);
  f.Close();

  if (NULL != o_selected_size)
   *o_selected_size = *p_size; //save selected size

  if (NULL != o_file_name)
   *o_file_name = open.GetFileName();

  return true; //подтверждение пользователя
 }
 else
  return false; //отказ пользователя
}

//Load FLASH from a file
//мы заранее знаем размер файла с FLASH
//p_data - буфер для чтения данных. Должен быть не меньше чем 64кБ
//size  - размер данных для чтения
//o_file_name - указатель на строку в которую будет сохранено имя файла
bool LoadFLASHFromFile(BYTE* p_data, const std::vector<int>& sizes, _TSTRING* i_title /*= NULL*/, int* o_selected_size /*= NULL*/, _TSTRING* o_file_name /*= NULL*/, _TSTRING* o_file_path /*= NULL*/)
{
 HANDLE   hFile=0;
 static TCHAR BASED_CODE szFilter[] = _T("Firmware Files (*.bin;*.hex;*.a90)|*.bin;*.hex;*.a90|BIN Files (*.bin)|*.bin|HEX Files (*.hex;*.a90)|*.hex;*.a90|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);
 if (i_title)
  open.m_ofn.lpstrTitle = i_title->c_str();
 CString cs;

 if (sizes.empty())
  return false; //error
 std::vector<int>::const_iterator p_size_max = std::max_element(sizes.begin(), sizes.end());

 if ((o_file_path && !o_file_path->empty()) || open.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  //obtain file name either from full path (if supplied) or open file dialog
  _TSTRING fileName = (o_file_path && !o_file_path->empty()) ? (*o_file_path) : open.GetPathName().GetBuffer(256);
  if(!f.Open(fileName.c_str(), CFile::modeRead, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false; //ошибка
  }

  //----------------------------------------------------------------------------
  _TSTRING fileExt;
  if (o_file_path && !o_file_path->empty())
  { //obtain file extension from full path
   TCHAR path[MAX_PATH+1] = {0};
   o_file_path->copy(path, o_file_path->size());
   fileExt = PathFindExtension(path);
   if (fileExt[0] == _T('.'))
    fileExt.erase(0, 1);  //Remove dot
  }
  else //obtain extension from open file dialog
   fileExt = open.GetFileExt().GetBuffer(256);
  if (fileExt==_T("hex") || fileExt==_T("a90"))
  {
   ULONGLONG ulonglong_size = f.GetLength();
   if (ulonglong_size > 524288)
   {
    AfxMessageBox(MLL::LoadString(IDS_FW_FILE_IS_TOO_BIG));
    f.Close();
    return false; //ошибка
   }

   int hex_file_length = static_cast<int>(ulonglong_size);
   BYTE* p_hex_buff = new BYTE[hex_file_length];
   f.Read(p_hex_buff, hex_file_length);
   size_t bin_size = 0;
   EReadHexStatus status = HexUtils_ConvertHexToBin(p_hex_buff, hex_file_length, p_data, bin_size, *p_size_max);
   delete p_hex_buff;

   switch(status)
   {
    case RH_INCORRECT_CHKSUM:
     AfxMessageBox(MLL::LoadString(IDS_FW_HEX_FILE_CRC_ERROR));
     f.Close();
     return false; //ошибка

    default:
     case RH_UNEXPECTED_SYMBOL:
     AfxMessageBox(MLL::LoadString(IDS_FW_HEX_FILE_STRUCTURE_ERROR));
     f.Close();
     return false; //ошибка

    case RH_ADDRESS_EXCEDED:
     break;

    case RH_SUCCESS:
     break;
   }

   //find appropriate size and check
   std::vector<int>::const_iterator p_size = std::find(sizes.begin(), sizes.end(), bin_size);
   if ((p_size==sizes.end()) || (status == RH_ADDRESS_EXCEDED))
   {
    AfxMessageBox(GenMessage()(sizes, IDS_FW_WRONG_FW_FILE_SIZE));
    f.Close();
    return false; //ошибка
   }

   if (NULL != o_selected_size)
    *o_selected_size = *p_size; //save selected size
  }
  else //если у файла расширение bin или нет расширения или оно другое, то по умолчанию bin
  {
   std::vector<int>::const_iterator p_size = std::find(sizes.begin(), sizes.end(), f.GetLength());
   if (p_size==sizes.end())
   {
    AfxMessageBox(GenMessage()(sizes, IDS_FW_WRONG_FW_FILE_SIZE));
    f.Close();
    return false; //ошибка
   }
   f.Read(p_data, *p_size);

   if (NULL != o_selected_size)
    *o_selected_size = *p_size; //save selected size
  }
  //----------------------------------------------------------------------------

  f.Close();
  if (NULL != o_file_name)
  {
   if (o_file_path && !o_file_path->empty())
   { //obtain file name from full path
    TCHAR path[MAX_PATH+1] = {0};
    o_file_path->copy(path, o_file_path->size());
    PathStripPath(path);
    *o_file_name = path;
   }
   else //obtain file name from open file dialog
    *o_file_name = open.GetFileName();
  }

  //Save full path only if it is not supplied
  if (NULL != o_file_path && o_file_path->empty())
   *o_file_path = open.GetPathName();

  return true; //подтверждение пользователя
 }
 else
  return false; //отказ пользователя
}

} //ns
