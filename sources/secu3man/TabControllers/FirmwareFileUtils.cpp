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

/** \file FirmwareFileUtils.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <algorithm>
#include <shlwapi.h>
#include <vector>
#include "io-core/FirmwareDataMediator.h"
#include "io-core/EEPROMDataMediator.h"
#include "HexUtils/readhex.h"
#include "HexUtils/writehex.h"
#include "Resources/resource.h"
#include "ui-core/FileDialogEx.h"
#include "ui-core/MsgBox.h"

namespace FirmwareFileUtils { 

//Save EEPROM to file
bool SaveEEPROMToFile(const BYTE* p_data, const int size, EEPROMDataMediator* p_eedm /*= NULL*/, CString* o_file_name /* = NULL*/, bool calculate_and_place_crc16/* = false*/)
{
 if (!p_eedm && calculate_and_place_crc16) return false;

 BYTE *save_buff = NULL;
 HANDLE   hFile=0;
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialogEx save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(save.GetPathName(),CFile::modeWrite|CFile::modeCreate,&ex))
  {
   ex.GetErrorMessage(szError, 1024);
   SECUMessageBox(szError);
   return false;
  }

  save_buff = new BYTE[size];
  memcpy(save_buff, p_data,size);

  //вычисляем контрольную сумму и сохраняем ее в массив с EEPROM
  if (calculate_and_place_crc16)
  {
   p_eedm->CalculateAndPlaceParamsCRC(save_buff); //for params
   p_eedm->CalculateAndPlaceTablesCRC(save_buff); //for tables
  }

  f.Write(save_buff,size);
  f.Close();
  delete save_buff;

  if (o_file_name!=NULL)
   *o_file_name = save.GetFileName();

  return true;
 }
 else
  return false;
}

//Save FLASH to file
bool SaveFLASHToFile(const BYTE* p_data, const int size, CFirmwareDataMediator* p_fwdm, CString* o_file_name /* = NULL*/, bool calculate_and_place_crc16/* = false*/)
{
 if (!p_fwdm) return false;

 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|HEX Files (*.hex)|*.hex|All Files (*.*)|*.*||");
 CFileDialogEx save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CStdioFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(save.GetPathName(), CFile::modeWrite | CFile::modeCreate | CFile::typeBinary, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   SECUMessageBox(szError);
   return false; //error - data has not been saved
  }

  std::vector<BYTE> save_buff(size);
  memcpy(&save_buff[0], p_data, size);

  //calculate and place check sum into buffer with firmware
  if (calculate_and_place_crc16)
   p_fwdm->CalculateAndPlaceFirmwareCRC(&save_buff[0]);

  if (save.GetFileExt()==_T("hex")) //check, if selected file name is hex, then save it as hex
  {
   f.SetLength(0);
   if (!WriteHexFile(f.m_pStream, save_buff))
   {
    f.Close();
    return false;
   }
  }
  else
   f.Write(&save_buff[0], (UINT)save_buff.size());    

  f.Close();

  if (o_file_name!=NULL)
   *o_file_name = save.GetFileName();
  return true; //user confirm
 }
 else
  return false; //canceled by user
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
bool LoadEEPROMFromFile(BYTE* p_data, const std::vector<int>& sizes, int* o_selected_size /*= NULL*/, _TSTRING* o_file_name /*= NULL*/, _TSTRING* o_file_path /*= NULL*/)
{
 HANDLE hFile = 0;
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialogEx open(TRUE,NULL,NULL,NULL,szFilter,NULL);
 CString cs;

 if (sizes.empty())
  return false; //error, at least one size must be specified
 std::vector<int>::const_iterator p_size_max = std::max_element(sizes.begin(), sizes.end());

 if ((o_file_path && !o_file_path->empty()) || open.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  //obtain file name either from full path (if supplied) or open file dialog
  _TSTRING fileName = (o_file_path && !o_file_path->empty()) ? (*o_file_path) : (LPCTSTR)open.GetPathName();
  if(!f.Open(fileName.c_str(), CFile::modeRead, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   SECUMessageBox(szError);
   return false; //error, can't open file
  }

  //Проверка на размер файла (его размер должен соответствовать одному из разрешенных размеров EEPROM)
  std::vector<int>::const_iterator p_size = std::find(sizes.begin(), sizes.end(), f.GetLength());
  if (p_size==sizes.end())
  {
   SECUMessageBox(GenMessage()(sizes, IDS_FW_WRONG_EE_FILE_SIZE));
   f.Close();
   return false; //ошибка
  }

  f.Read(p_data, *p_size);
  f.Close();

  if (NULL != o_selected_size)
   *o_selected_size = *p_size; //save selected size

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

//Load FLASH from a file
//мы заранее знаем размер файла с FLASH
//p_data - буфер для чтения данных. Должен быть не меньше чем 64кБ
//size  - размер данных для чтения
//o_file_name - указатель на строку в которую будет сохранено имя файла
bool LoadFLASHFromFile(BYTE* p_data, const std::vector<int>& sizes, _TSTRING* i_title /*= NULL*/, int* o_selected_size /*= NULL*/, _TSTRING* o_file_name /*= NULL*/, _TSTRING* o_file_path /*= NULL*/)
{
 HANDLE   hFile=0;
 static TCHAR BASED_CODE szFilter[] = _T("Firmware Files (*.bin;*.hex;*.a90)|*.bin;*.hex;*.a90|BIN Files (*.bin)|*.bin|HEX Files (*.hex;*.a90)|*.hex;*.a90|All Files (*.*)|*.*||");
 CFileDialogEx open(TRUE,NULL,NULL,NULL,szFilter,NULL);
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
  _TSTRING fileName = (o_file_path && !o_file_path->empty()) ? (*o_file_path) : (LPCTSTR)open.GetPathName();
  if(!f.Open(fileName.c_str(), CFile::modeRead, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   SECUMessageBox(szError);
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
   fileExt = (LPCTSTR)open.GetFileExt();
  if (fileExt==_T("hex") || fileExt==_T("a90"))
  {
   ULONGLONG ulonglong_size = f.GetLength();
   if (ulonglong_size > 524288)
   {
    SECUMessageBox(MLL::LoadString(IDS_FW_FILE_IS_TOO_BIG));
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
     SECUMessageBox(MLL::LoadString(IDS_FW_HEX_FILE_CRC_ERROR));
     f.Close();
     return false; //ошибка

    default:
     case RH_UNEXPECTED_SYMBOL:
     SECUMessageBox(MLL::LoadString(IDS_FW_HEX_FILE_STRUCTURE_ERROR));
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
    SECUMessageBox(GenMessage()(sizes, IDS_FW_WRONG_FW_FILE_SIZE));
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
    SECUMessageBox(GenMessage()(sizes, IDS_FW_WRONG_FW_FILE_SIZE));
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

bool CheckFirmwareIntegrity(BYTE* p_data, int size)
{
 #define signLen1 60
 #define signLen2 69

 BYTE sign1[signLen1] =
 { 0x20,0xfd,0x99,0x41,0xfd,0x7f,0x2f,0xc6,0x96,0x63,0x16,0xbb,0x6f,0x03,0xbe,0x5b,
   0xc0,0x72,0x37,0xc7,0x68,0x24,0xcc,0x76,0x27,0xba,0x85,0x1c,0xb8,0x95,0x54,0xf7,
   0xa2,0x4f,0x00,0xac,0x51,0xfd,0xab,0x5c,0xbc,0x5a,0x4c,0x02,0xac,0x52,0xc6,0x65,
   0x0f,0xfd,0x99,0x41,0xfd,0x5f,0x0f,0xb4,0x9f,0x4c,0xeb,0x4e };

 BYTE sign2[signLen2] =
 { 0x53,0xef,0x97,0x53,0xd5,0x85,0x1c,0xec,0xb9,0x6c,0x11,0xc5,0x59,0x14,0xb1,0x22,
   0xc0,0x8d,0x63,0x0e,0xc1,0x64,0x05,0xad,0x58,0x0e,0x64,0x16,0xdb,0x6b,0x0c,0xc8,
   0x70,0x1a,0xcb,0x5e,0x29,0xc0,0x5c,0x39,0xf8,0x9b,0x46,0xf3,0xa4,0x50,0xf5,0xa1,
   0x4f,0x00,0x60,0xfe,0xf0,0xa6,0x50,0xf6,0x6a,0x09,0xb3,0xa1,0x3d,0xe5,0xa1,0x03,
   0xb3,0x58,0x43,0xf0,0x8f };

 for(int i = 0; i < signLen1; ++i)
  sign1[i] = sign1[i] - ((unsigned char)i * (unsigned char)0xAA);

 for(int i = 0; i < signLen2; ++i)
  sign2[i] = sign2[i] - ((unsigned char)i * (unsigned char)0xAA);

 //"Incompatible version of firmware!"
 TCHAR *fwass = _T("IQn3cxolm8p0a!t@i^b+lJe[ $v$e%r(s#i:o<n{ ~o~fW TfBiMrJmJw^a&r&el!|");
 TCHAR buff[128];

 for(int i = 0; i < (int)_tcslen(fwass); ++i)
  buff[i] = fwass[i*2];
 buff[i] = 0;

 BYTE* data_end = p_data + size;
 if (std::search(p_data, p_data + size, sign1, sign1 + signLen1) == data_end)
 {
  if (std::search(p_data, p_data + size, sign2, sign2 + signLen2) == data_end)
  {
   SECUMessageBox(buff, MB_OK | MB_ICONSTOP);
   return false;
  }
 }

 return true;
}

} //ns
