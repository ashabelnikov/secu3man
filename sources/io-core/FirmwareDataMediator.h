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

#pragma once

#include "iocore_api.h"
#include "common/unicodesupport.h"
#include <vector>

//Этот класс необходим для абстракции над форматом хранения данных в прошивке
//(памяти программ микроконтроллера), а также для хранения этих данных.
struct PPFlashParam;
class IOCORE_API CFirmwareDataMediator
{
  class LocInfoProvider;
 public:
  CFirmwareDataMediator(const PPFlashParam& i_fpp);
 ~CFirmwareDataMediator();

  //загрузка байтов прошивки из указанного буфера
  void LoadBytes(const BYTE* i_bytes);
  void LoadDataBytesFromAnotherFirmware(const BYTE* i_source_bytes, const PPFlashParam* ip_fpp = NULL);
  void LoadDefParametersFromBuffer(const BYTE* i_source_bytes);

  //сохранение байтов прошивки в указанный буфер
  void StoreBytes(BYTE* o_bytes);

  //была ли прошивка изменена после последней загрузки
  bool IsModified(void);

  //возвращает true если прошивка загружена (есть что редактировать)
  bool IsLoaded(void) { return m_is_opened;}

  //вызывать эту функцию для сброса признака модификации, только
  //после того как данные были успешно сохранены.
  void ResetModified(void);

  void CalculateAndPlaceFirmwareCRC(BYTE* io_data);

  //checks compatibility of specified firmware with this version of management software
  //returns false if incompatible
  bool CheckCompatibility(const BYTE* i_data, const PPFlashParam* ip_fpp = NULL) const;

  //-----------------------------------------------------------------------
  _TSTRING GetSignatureInfo(void);
  void SetSignatureInfo(const _TSTRING& i_string);

  DWORD GetFWOptions(void);

  void GetStartMap(int i_index, float* o_values, bool i_original = false);
  void SetStartMap(int i_index,const float* i_values);
  void GetIdleMap(int i_index,  float* o_values, bool i_original = false);
  void SetIdleMap(int i_index, const float* i_values);
  void GetWorkMap(int i_index, float* o_values, bool i_original = false);
  void SetWorkMap(int i_index, const float* i_values);
  void GetTempMap(int i_index, float* o_values, bool i_original = false);
  void SetTempMap(int i_index, const float* i_values);

  void GetMapsData(struct FWMapsDataHolder* op_fwd);
  void SetMapsData(const struct FWMapsDataHolder* ip_fwd);

  std::vector<_TSTRING> GetFunctionsSetNames(void);
  void SetFunctionsSetName(int i_index, _TSTRING i_new_name);

  void SetFWFileName(const _TSTRING i_fw_file_name);
  _TSTRING GetFWFileName(void);

  void GetAttenuatorMap(float* o_values, bool i_original = false);
  void SetAttenuatorMap(const float* i_values);

  void GetDwellCntrlMap(float* o_values, bool i_original = false);
  void SetDwellCntrlMap(const float* i_values);

  void GetCTSCurveMap(float* o_values, bool i_original = false);
  void SetCTSCurveMap(const float* i_values);

  float GetCTSMapVoltageLimit(int i_type);
  void  SetCTSMapVoltageLimit(int i_type, float i_value);

  //Types of slots/plugs
  enum IOXtype
  {
   IOX_INIT = 0,             //initialization slot  
   IOX_DATA = 1              //data slot
  };

  //IDs of plugs 
  enum IOPid
  {
   IOP_ECF        =   0,     // ECF
   IOP_ST_BLOCK   =   1,     // ST_BLOCK
   IOP_IGN_OUT3   =   2,     // IGN_OUT3
   IOP_IGN_OUT4   =   3,     // IGN_OUT4
   IOP_ADD_IO1    =   4,     // ADD_IO1     (applicable only in SECU-3T)
   IOP_ADD_IO2    =   5,     // ADD_IO2     (applicable only in SECU-3T)
   IOP_IE         =   6,     // IE
   IOP_FE         =   7,     // FE
   IOP_FL_PUMP    =   8,     // FL_PUMP
   IOP_HALL_OUT   =   9,     // HALL_OUT
   IOP_STROBE     =  10,     // STROBE
   IOP_PWRRELAY   =  11,     // PWRRELAY
// IOS_RESERVED3  =  11,     // Reserved
// IOS_RESERVED4  =  12,     // Reserved
// IOS_RESERVED5  =  13,     // Reserved
// IOS_RESERVED6  =  14,     // Reserved
// IOS_RESERVED7  =  15,     // Reserved
   IOP_COUNT                 // Number of plugs used in I/O remapping
  };

  //IDs of slots
  enum IOSid
  {
   IOS_ECF        =   0,     // ECF
   IOS_ST_BLOCK   =   1,     // ST_BLOCK
   IOS_IGN_OUT3   =   2,     // IGN_OUT3
   IOS_IGN_OUT4   =   3,     // IGN_OUT4
   IOS_ADD_IO1    =   4,     // ADD_IO1     (applicable only in SECU-3T)
   IOS_ADD_IO2    =   5,     // ADD_IO2     (applicable only in SECU-3T)
   IOS_IE         =   6,     // IE
   IOS_FE         =   7,     // FE
// IOS_RESERVED1  =   8,     // Reserved
// IOS_RESERVED2  =   9,     // Reserved
   IOS_COUNT                 // Number of slots used for I/O remapping
  };

  DWORD GetIOPlug(IOXtype type, IOPid id);
  DWORD GetIOSlot(IOXtype type, IOSid id);
  DWORD GetSStub(void);
  void  SetIOPlug(IOXtype type, IOPid id, DWORD value);

  bool HasCodeData(const BYTE* i_source_bytes = NULL) const;
  size_t GetOnlyCodeSize(const BYTE* i_bytes) const;
  void LoadCodeData(const BYTE* i_source_bytes, size_t i_srcSize, BYTE* o_destin_bytes = NULL);

  unsigned int CalculateCRC16OfActiveFirmware(void);
  unsigned int GetCRC16StoredInActiveFirmware(void);
  void CalculateAndPlaceFirmwareCRC(void);

  //-----------------------------------------------------------------------

  bool SetDefParamValues(BYTE i_descriptor, const void* i_values);   //загнать указанные резервн.парам. в прошивку
  bool GetDefParamValues(BYTE i_descriptor, void* o_values);         //извлечь указанные резервн.парам. из прошивки

  //-----------------------------------------------------------------------

  const PPFlashParam& GetPlatformParams(void) const;

 private:
  CFirmwareDataMediator(const CFirmwareDataMediator& i);
  void _FindCodeData(void);

 private:
  const std::auto_ptr<PPFlashParam> m_fpp;
  const std::auto_ptr<LocInfoProvider> m_lip;
  const size_t m_firmware_size;
  BYTE* m_bytes_active;   //байты прошивки (копия для модификации)
  BYTE* m_bytes_original; //байты прошивки (копия для сравнения)
  bool m_is_opened;
  _TSTRING m_fw_file_name;
  struct cd_data_t* mp_cddata;
};
