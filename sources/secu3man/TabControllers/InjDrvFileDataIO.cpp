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

/** \file InjDrvFileDataIO.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "InjDrvFileDataIO.h"
#include "io-core/CRC16.h"
#include "common/MathHelpers.h"
#include "io-core/Bitmask.h"
#include <algorithm>
#include "HexUtils/readhex.h"
#include "HexUtils/writehex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LZIDSIGSIZE 18

const char* strSettSign = ".lzid-def-settings";
const char* strHeader = "http://secu-3.org";

//types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#pragma pack( push, enter_LzidSettingsDef )
#pragma pack(1) //<--LZID

//old implementation (v2.3 and older)
struct lzid_sett_t_v23
{
 char sign[LZIDSIGSIZE];          //!< text signature, thus this structure can be found in the raw code

 uint8_t type;                    //!< type of driver (model ID), 0 - Unknown; 1 - LZID4, mega1284;
 uint8_t version;                 //!< version of this structure/system
 uint8_t fw_opt;                  //!< Firmware compilation options

 uint16_t pwm_period;             //!< value in 0.05us units
 uint16_t peak_duty;              //!< coefficient in range 0...1.0, value * 4096

 uint16_t reserved;               //!< reserved for future use (peak PWM period)
 uint16_t hold_duty;              //!< coefficient in range 0...1.0, value * 4096

 uint16_t peak_on_time;           //!< peak time withput PWM (full on), value in 0.4us units
 uint16_t peak_full_time;         //!< full peak time (ON+PWM), value in 0.4us units
 uint16_t pth_pause;              //!< peak-to-hold pause, value in 0.4us units

 uint8_t direct_flags;            //!< flags, enabling direct input-to-output control (one bit per channel)
 uint8_t lutabl_flags;            //!< look up tables related flags
 uint8_t gen_flags;               //!< general flags

 uint16_t peak_on_tab[SECU3IO::LUTABSIZE]; //!< peak full on time vs board voltage
 uint16_t peak_duty_tab[SECU3IO::LUTABSIZE];//!< peak duty vs board voltage
 uint16_t hold_duty_tab[SECU3IO::LUTABSIZE];//!< hold duty vs board voltage
 uint16_t peak_full_tab[SECU3IO::LUTABSIZE];//!< full peak time (ON+PWM) vs board voltage
 uint16_t pth_pause_tab[SECU3IO::LUTABSIZE];//!< peak-to-hold pause vs board voltage

 uint8_t testch_sel;
 uint16_t testch_frq;
 uint8_t testch_duty;

 uint8_t reserved_bytes[188];     //!< reserved bytes

 uint16_t crc;                    //!< CRC16 of this structure (excluding last two bytes)
};

//v2.4+ implementations:
struct lzid_sett_t
{
 char sign[LZIDSIGSIZE];          //!< text signature, thus this structure can be found in the raw code

 uint8_t type;                    //!< type of driver (model ID), 0 - Unknown; 1 - LZID4, mega1284;
 uint8_t version;                 //!< version of this structure/system
 uint8_t fw_opt;                  //!< Firmware compilation options

 uint16_t pwm_period;             //!< value in 0.05us units
 uint16_t peak_duty;              //!< coefficient in range 0...1.0, value * 4096

 uint16_t reserved;               //!< reserved for future use (peak PWM period)
 uint16_t hold_duty;              //!< coefficient in range 0...1.0, value * 4096

 uint16_t peak_on_time;           //!< peak time withput PWM (full on), value in 0.4us units
 uint16_t peak_full_time;         //!< full peak time (ON+PWM), value in 0.4us units
 uint16_t pth_pause;              //!< peak-to-hold pause, value in 0.4us units

 uint8_t direct_flags;            //!< flags, enabling direct input-to-output control (one bit per channel)
 uint8_t lutabl_flags;            //!< look up tables related flags
 uint8_t gen_flags;               //!< general flags

 uint16_t peak_on_tab[SECU3IO::LUTABSIZE]; //!< peak full on time vs board voltage
 uint16_t peak_duty_tab[SECU3IO::LUTABSIZE];//!< peak duty vs board voltage
 uint16_t hold_duty_tab[SECU3IO::LUTABSIZE];//!< hold duty vs board voltage
 uint16_t peak_full_tab[SECU3IO::LUTABSIZE];//!< full peak time (ON+PWM) vs board voltage
 uint16_t pth_pause_tab[SECU3IO::LUTABSIZE];//!< peak-to-hold pause vs board voltage

 uint8_t testch_sel;
 uint16_t testch_frq;
 uint8_t testch_duty;

 uint8_t reserved_bytes[38];     //!< reserved bytes

 uint16_t crc;                    //!< CRC16 of this structure (excluding last two bytes)
};

#pragma pack( pop, enter_LzidSettingsDef )



void ConvertToFirmwareData(const SECU3IO::InjDrvPar& ms, lzid_sett_t& fs)
{
 memset(&fs, 0, sizeof(lzid_sett_t));

 strncpy(fs.sign, strSettSign, LZIDSIGSIZE);
 fs.type = ms.type;
 fs.version = ms.version;
 fs.fw_opt = ms.fw_opt;

 fs.pwm_period = MathHelpers::Round(ms.m_pwm_period * 20.0f);
 fs.peak_duty = MathHelpers::Round((ms.m_peak_duty / 100.0f) * 4096.0f);
 fs.reserved = 0;
 fs.hold_duty = MathHelpers::Round((ms.m_hold_duty / 100.0f) * 4096.0f);
 fs.peak_on_time = MathHelpers::Round(ms.m_peak_on_time * 2.5f);
 fs.peak_full_time = MathHelpers::Round(ms.m_peak_pwm_time * 2.5f);
 fs.pth_pause = MathHelpers::Round(ms.m_pth_pause * 2.5f);

 fs.direct_flags = 0;
 for(int i = 0; i < 8; ++i)
  WRITEBIT8(fs.direct_flags, i, ms.direct_flags[i]);

 fs.lutabl_flags = 0;
 WRITEBIT8(fs.lutabl_flags, 0, ms.m_peak_on_usetab);
 WRITEBIT8(fs.lutabl_flags, 1, ms.m_peak_duty_usetab);
 WRITEBIT8(fs.lutabl_flags, 2, ms.m_hold_duty_usetab);
 WRITEBIT8(fs.lutabl_flags, 3, ms.m_peak_full_usetab);
 WRITEBIT8(fs.lutabl_flags, 4, ms.m_pth_pause_usetab);

 fs.gen_flags = 0;
 WRITEBIT8(fs.gen_flags, 0, ms.m_tst_peak_pwm);
 WRITEBIT8(fs.gen_flags, 1, ms.m_tst_hold_pwm);

 fs.testch_sel = ms.m_testch_sel;
 fs.testch_frq = MathHelpers::Round((1.0/ms.m_testch_frq) * 131072.0);
 fs.testch_duty = MathHelpers::Round((ms.m_testch_duty / 100.0) * 255);

 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  fs.peak_on_tab[i] = MathHelpers::Round(ms.m_peak_on_tab[i] * 2.5f);
 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  fs.peak_duty_tab[i] = MathHelpers::Round(ms.m_peak_duty_tab[i] * 2.5f);
 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  fs.hold_duty_tab[i] = MathHelpers::Round(ms.m_hold_duty_tab[i] * 2.5f);
 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  fs.peak_full_tab[i] = MathHelpers::Round(ms.m_peak_full_tab[i] * 2.5f);
 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  fs.pth_pause_tab[i] = MathHelpers::Round(ms.m_pth_pause_tab[i] * 2.5f);

 //finally, calculate and store CRC
 fs.crc = crc16((BYTE*)&fs, sizeof(lzid_sett_t) - sizeof(WORD));
}

template <class T>
bool ConvertFromFirmwareData(SECU3IO::InjDrvPar& ms, const T& fs, bool ignoreCRC = false)
{
 if (!ignoreCRC && fs.crc != crc16((BYTE*)&fs, sizeof(T) - sizeof(WORD)))
  return false; //data corrupted

 ms.ee_status = false;
 ms.start_bldr = false;
 ms.reset_eeprom = false;
 ms.voltage = .0f;

 ms.type = fs.type;
 ms.version = fs.version;
 ms.fw_opt = fs.fw_opt;

 ms.m_pwm_period = ((float)fs.pwm_period) / 20.0f;
 ms.m_peak_duty = (((float)fs.peak_duty) / 4096.0f) * 100.0f;
 ms.m_hold_duty = (((float)fs.hold_duty) / 4096.0f) * 100.0f;

 ms.m_peak_on_time = ((float)fs.peak_on_time) / 2.5f;
 ms.m_peak_pwm_time = ((float)fs.peak_full_time) / 2.5f;
 ms.m_pth_pause = ((float)fs.pth_pause) / 2.5f;

 for (int i = 0; i < 8; i++)
  ms.direct_flags[i] = CHECKBIT8(fs.direct_flags, i);

 ms.m_peak_on_usetab = CHECKBIT8(fs.lutabl_flags, 0);
 ms.m_peak_duty_usetab = CHECKBIT8(fs.lutabl_flags, 1);
 ms.m_hold_duty_usetab = CHECKBIT8(fs.lutabl_flags, 2);
 ms.m_peak_full_usetab = CHECKBIT8(fs.lutabl_flags, 3);
 ms.m_pth_pause_usetab = CHECKBIT8(fs.lutabl_flags, 4);

 ms.m_tst_peak_pwm = CHECKBIT8(fs.gen_flags, 0);
 ms.m_tst_hold_pwm = CHECKBIT8(fs.gen_flags, 1);

 ms.m_testch_sel = fs.testch_sel;  
 ms.m_testch_frq = (float)(1.0/(((double)fs.testch_frq) / 131072.0));
 ms.m_testch_duty = (float)((((double)fs.testch_duty) / 255.0) * 100.0f);

 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  ms.m_peak_on_tab[i] = ((float)fs.peak_on_tab[i]) / 2.5f;
 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  ms.m_peak_duty_tab[i] = ((float)fs.peak_duty_tab[i]) / 2.5f;
 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  ms.m_hold_duty_tab[i] = ((float)fs.hold_duty_tab[i]) / 2.5f;
 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  ms.m_peak_full_tab[i] = ((float)fs.peak_full_tab[i]) / 2.5f;
 for (int i = 0; i < SECU3IO::LUTABSIZE; ++i)
  ms.m_pth_pause_tab[i] = ((float)fs.pth_pause_tab[i]) / 2.5f;

 return true; //ok
}


bool CInjDrvFileDataIO::ExportSet(SECU3IO::InjDrvPar* ip_set)
{
 FILE* fout = NULL;
 static TCHAR BASED_CODE szFilter[] = _T("LZID Files (*.lzid)|*.lzid|All Files (*.*)|*.*||");
 CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("lzid");
 if (save.DoModal()==IDOK)
 {
  fout = _tfopen(save.GetPathName(),_T("wb+"));
  if (NULL == fout)
  {
   AfxMessageBox("Error opening file for saving!", MB_ICONSTOP);
   return false;
  }
  lzid_sett_t fwd;  
  ConvertToFirmwareData(*ip_set, fwd);
  fwrite(strHeader, sizeof(char), strlen(strHeader), fout);
  fwrite(&fwd, sizeof(BYTE), sizeof(lzid_sett_t), fout);
  fclose(fout);
 }
 return true;
}

bool CInjDrvFileDataIO::ImportSet(SECU3IO::InjDrvPar* op_set)
{
 FILE* fin = NULL;
 static TCHAR BASED_CODE szFilter[] = _T("LZID Files (*.lzid)|*.lzid|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);

 if (open.DoModal()==IDOK)
 {
  fin = _tfopen(open.GetPathName(),_T("rb"));
  if (NULL == fin)
  {
   AfxMessageBox("Error opening file for read!", MB_ICONSTOP);
   return false;
  }

  fseek(fin, 0L, SEEK_END);
  long int fsize = ftell(fin);
  fseek(fin, 0L, SEEK_SET);

  char buff[32];
  fread(buff, sizeof(char), strlen(strHeader), fin);

  bool result = false;
  if (fsize == (sizeof(lzid_sett_t) + strlen(strHeader)))
  {
   lzid_sett_t fwd;
   fread(&fwd, sizeof(BYTE), sizeof(lzid_sett_t), fin);
   result = ConvertFromFirmwareData<lzid_sett_t>(*op_set, fwd);
  }
  else if (fsize == (sizeof(lzid_sett_t_v23) + strlen(strHeader)))
  { //old
   lzid_sett_t_v23 fwd;
   fread(&fwd, sizeof(BYTE), sizeof(lzid_sett_t_v23), fin);
   result = ConvertFromFirmwareData<lzid_sett_t_v23>(*op_set, fwd);
  }
  else
  {
   ASSERT(0);
  }

  if (!result || 0 != strncmp(buff, strHeader, strlen(strHeader)))
  { //data corrupted
   AfxMessageBox("Error! Input file corrupted!", MB_ICONSTOP);
   fclose(fin);
   return false;
  }
  fclose(fin);
  return true;
 }
 return false; //canceled by user
}

template <class T>
bool getSettAddresses(std::vector<BYTE>& buff, T** op_set)
{
 std::vector<BYTE>::iterator it;
 it = std::search(buff.begin(), buff.end(), strSettSign, strSettSign + LZIDSIGSIZE);
 if (it == buff.end() || (size_t)((it + LZIDSIGSIZE) - buff.begin()) >= buff.size())
  return false;
 T* p_set0 = (T*)&buff[it - buff.begin()];
 it = std::search(it + LZIDSIGSIZE, buff.end(), strSettSign, strSettSign + LZIDSIGSIZE);
 if (it == buff.end())
  return false;
 T* p_set1 = (T*)&buff[it - buff.begin()];

 op_set[0] = p_set0;
 op_set[1] = p_set1;
 return true;
}


bool CInjDrvFileDataIO::LoadSetsFromFirmware(SECU3IO::InjDrvPar* op_set)
{
 static TCHAR BASED_CODE szFilter[] = _T("Firmware Files (*.bin;*.hex;*.a90)|*.bin;*.hex;*.a90|BIN Files (*.bin)|*.bin|HEX Files (*.hex;*.a90)|*.hex;*.a90|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);

 if (open.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(open.GetPathName(), CFile::modeRead, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false;
  }

  ULONGLONG size = f.GetLength();
  if (size > 1048576) //max. 1MB
  {
   f.Close();
   return false; //file is too big
  }
  std::vector<BYTE> buff((size_t)size, 0);
  f.Read(&buff[0], (UINT)size);
  f.Close();

  //check, if input file is hex, then convert it to raw bin
  CString fileExt = open.GetFileExt();
  if (fileExt==_T("hex") || fileExt==_T("a90"))
  {
   int max_size = 1048576;
   std::vector<BYTE> hex_buff(max_size, 0);
   size_t resulting_bin_size = 0;
   EReadHexStatus status = HexUtils_ConvertHexToBin(&buff[0], (size_t)size, &hex_buff[0], resulting_bin_size, max_size);
   switch(status)
   {
    case RH_INCORRECT_CHKSUM:
     AfxMessageBox("Hex file CRC error! File corrupted!", MB_ICONSTOP);
     return false;
    default:
     case RH_UNEXPECTED_SYMBOL:
     AfxMessageBox("Hex file structure error!", MB_ICONSTOP);
     return false;
    case RH_ADDRESS_EXCEDED:
     AfxMessageBox("Hex file is too big!", MB_ICONSTOP);
     break;
    case RH_SUCCESS:
     break;
   }
   //save data
   hex_buff.resize(resulting_bin_size);
   buff = hex_buff;
  }

  //find signature and calculate addresses
  lzid_sett_t* p_set[2];
  if (!getSettAddresses<lzid_sett_t>(buff, p_set))
   return false;

  bool result1 = false, result2 = false;
  if (p_set[0]->version >= 0x24)
   result1 = ConvertFromFirmwareData<lzid_sett_t>(op_set[0], *p_set[0], true);
  else
   result1 = ConvertFromFirmwareData<lzid_sett_t_v23>(op_set[0], *((lzid_sett_t_v23*)p_set[0]), true);
  if (p_set[1]->version >= 0x24)
   result2 = ConvertFromFirmwareData<lzid_sett_t>(op_set[1], *p_set[1], true);
  else
   result2 = ConvertFromFirmwareData<lzid_sett_t_v23>(op_set[1], *((lzid_sett_t_v23*)p_set[1]), true);

  //For now we ignore check sum, because it is not calculated in the firmware
  if (!result1 || !result2)
  { //data corrupted
   AfxMessageBox("Error! Input file corrupted!", MB_ICONSTOP);
   return false;
  }
  return true;
 }
 return false; //canceled by user
}

bool CInjDrvFileDataIO::SaveSetsToFirmware(SECU3IO::InjDrvPar* ip_set)
{
 static TCHAR BASED_CODE szFilter[] = _T("Firmware Files (*.bin;*.hex;*.a90)|*.bin;*.hex;*.a90|BIN Files (*.bin)|*.bin|HEX Files (*.hex;*.a90)|*.hex;*.a90|All Files (*.*)|*.*||");
 CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CStdioFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(save.GetPathName(), CFile::modeReadWrite | CFile::typeBinary, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false;
  }

  ULONGLONG size = f.GetLength();
  if (size > 1048576) //max. 1MB
  {
   f.Close();
   return false; //file is too big
  }
  std::vector<BYTE> buff((size_t)size, 0);
  f.Read(&buff[0], (UINT)size);

  //check, if input file is hex, then convert it to raw bin
  bool hex_file = save.GetFileExt()==_T("hex") || save.GetFileExt()==_T("a90");
  if (hex_file)
  {
   int max_size = 1048576;
   std::vector<BYTE> hex_buff(max_size, 0);
   size_t resulting_bin_size = 0;
   EReadHexStatus status = HexUtils_ConvertHexToBin(&buff[0], (size_t)size, &hex_buff[0], resulting_bin_size, max_size);
   switch(status)
   {
    case RH_INCORRECT_CHKSUM:
     AfxMessageBox("Hex file CRC error! File corrupted!", MB_ICONSTOP);
     f.Close();
     return false;
    default:
     case RH_UNEXPECTED_SYMBOL:
     AfxMessageBox("Hex file structure error!", MB_ICONSTOP);
     f.Close();
     return false;
    case RH_ADDRESS_EXCEDED:
     AfxMessageBox("Hex file is too big!", MB_ICONSTOP);
     f.Close();
     return false;
    case RH_SUCCESS:
     break;
   }
   //save data
   hex_buff.resize(resulting_bin_size);
   buff = hex_buff;
  }

  //find signature and calculate addresses
  lzid_sett_t* p_set[2];
  if (!getSettAddresses(buff, p_set))
  {
   f.Close();
   return false;
  }
  //save modified data
  ConvertToFirmwareData(ip_set[0], *p_set[0]);
  ConvertToFirmwareData(ip_set[1], *p_set[1]);

  f.Seek(0, CFile::begin);

  //again, if file is hex, then save it in hex format
  if (hex_file)
  {
   f.SetLength(0);
   if (!WriteHexFile(f.m_pStream, buff))
   {
    f.Close();
    return false;
   }
  }
  else
   f.Write(&buff[0], (UINT)buff.size());    

  f.Close();
 }
 return true;
}

bool CInjDrvFileDataIO::SaveFirmware(const std::vector<BYTE>& buffer)
{
 static TCHAR BASED_CODE szFilter[] = _T("Firmware Files (*.bin;*.hex;*.a90)|*.bin;*.hex;*.a90|BIN Files (*.bin)|*.bin|HEX Files (*.hex)|*.hex|HEX Files (*.a90)|*.a90|All Files (*.*)|*.*||");
 CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CStdioFile f;
  CFileException ex;
  TCHAR szError[1024];  
  if(!f.Open(save.GetPathName(), CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false;
  }

  f.Seek(0, CFile::begin);

  //check, if selected file name is hex, then save it as hex
  bool hex_file = save.GetFileExt()==_T("hex") || save.GetFileExt()==_T("a90");

  if (hex_file)
  {
   f.SetLength(0);
   if (!WriteHexFile(f.m_pStream, buffer))
   {
    f.Close();
    return false;
   }
  }
  else
   f.Write(&buffer[0], (UINT)buffer.size());    

  f.Close();
 }
 return true;
}

bool CInjDrvFileDataIO::LoadFirmware(std::vector<BYTE>& buffer)
{
 static TCHAR BASED_CODE szFilter[] = _T("Firmware Files (*.bin;*.hex;*.a90)|*.bin;*.hex;*.a90|BIN Files (*.bin)|*.bin|HEX Files (*.hex)|*.hex|HEX Files (*.a90)|*.a90|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);

 if (open.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(open.GetPathName(), CFile::modeRead, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false;
  }

  ULONGLONG size = f.GetLength();
  if (size > 1048576) //max. 1MB
  {
   f.Close();
   return false; //file is too big
  }
  std::vector<BYTE> buff((size_t)size, 0);
  f.Read(&buff[0], (UINT)size);
  f.Close();

  //check, if input file is hex, then convert it to raw bin
  CString fileExt = open.GetFileExt();
  if (fileExt==_T("hex") || fileExt==_T("a90"))
  {
   int max_size = 1048576;
   std::vector<BYTE> hex_buff(max_size, 0);
   size_t resulting_bin_size = 0;
   EReadHexStatus status = HexUtils_ConvertHexToBin(&buff[0], (size_t)size, &hex_buff[0], resulting_bin_size, max_size);
   switch(status)
   {
    case RH_INCORRECT_CHKSUM:
     AfxMessageBox("Hex file CRC error! File corrupted!", MB_ICONSTOP);
     return false;
    default:
     case RH_UNEXPECTED_SYMBOL:
     AfxMessageBox("Hex file structure error!", MB_ICONSTOP);
     return false;
    case RH_ADDRESS_EXCEDED:
     AfxMessageBox("Hex file is too big!", MB_ICONSTOP);
     break;
    case RH_SUCCESS:
     break;
   }
   //save data
   hex_buff.resize(resulting_bin_size);
   if (resulting_bin_size != 32768)
    return false;
   buffer = hex_buff;
  }
  else
  { //binary file
   if (size != 32768)
    return false;
   buffer = buff;
  }
  return true;
 }
 return false; //canceled by user
}

bool CInjDrvFileDataIO::SaveEEPROM(const std::vector<BYTE>& buffer, size_t size)
{
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CStdioFile f;
  CFileException ex;
  TCHAR szError[1024];  
  if(!f.Open(save.GetPathName(), CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false;
  }

  f.Seek(0, CFile::begin);
  f.Write(&buffer[0], size);    
  f.Close();
 }
 return true;
}

bool CInjDrvFileDataIO::LoadEEPROM(std::vector<BYTE>& buffer, size_t size)
{
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);

 if (open.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(open.GetPathName(), CFile::modeRead, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false;
  }

  if (f.GetLength() != size) //wrong size
  {
   f.Close();
   return false; //file is too big
  }
  f.Read(&buffer[0], (UINT)size);
  f.Close();
  return true;
 }
 return false; //canceled by user
}
