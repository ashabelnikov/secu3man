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
  typedef bool (*EventHandler)(void);
 public:
  CFirmwareDataMediator(const PPFlashParam& i_fpp);
 ~CFirmwareDataMediator();

  //загрузка байтов прошивки из указанного буфера
  void LoadBytes(const BYTE* ip_bytes);
  void LoadDataBytesFromAnotherFirmware(const BYTE* ip_source_bytes, const PPFlashParam* ip_fpp = NULL);
  void LoadDefParametersFromBuffer(const BYTE* ip_source_bytes, EventHandler onVrefUsrConfirm = NULL);

  //сохранение байтов прошивки в указанный буфер
  void StoreBytes(BYTE* op_bytes);

  //была ли прошивка изменена после последней загрузки
  bool IsModified(void);

  //возвращает true если прошивка загружена (есть что редактировать)
  bool IsLoaded(void) { return m_is_opened;}

  //вызывать эту функцию для сброса признака модификации, только
  //после того как данные были успешно сохранены.
  void ResetModified(void);

  void CalculateAndPlaceFirmwareCRC(BYTE* iop_data);

  //checks compatibility of specified firmware with this version of management software
  //returns false if incompatible
  bool CheckCompatibility(const BYTE* ip_data, const PPFlashParam* ip_fpp = NULL) const;

  //-----------------------------------------------------------------------
  _TSTRING GetSignatureInfo(void);
  void SetSignatureInfo(const _TSTRING& i_string);

  DWORD GetFWOptions(void);
  DWORD GetFWOptions(const BYTE* ip_source_bytes, const PPFlashParam* ip_fpp);

  void GetStartMap(int i_index, float* op_values, bool i_original = false);
  void SetStartMap(int i_index,const float* i_values);
  void GetIdleMap(int i_index,  float* op_values, bool i_original = false);
  void SetIdleMap(int i_index, const float* i_values);
  void GetWorkMap(int i_index, float* op_values, bool i_original = false);
  void SetWorkMap(int i_index, const float* i_values);
  void GetTempMap(int i_index, float* op_values, bool i_original = false);
  void SetTempMap(int i_index, const float* i_values);

  void GetMapsData(struct FWMapsDataHolder* op_fwd);
  void SetMapsData(const struct FWMapsDataHolder* ip_fwd);
  bool CheckRPMGridsCompatibility(const float* rpmGrid);

  std::vector<_TSTRING> GetFunctionsSetNames(void);
  void SetFunctionsSetName(int i_index, _TSTRING i_new_name);

  void SetFWFileName(const _TSTRING i_fw_file_name);
  _TSTRING GetFWFileName(void);

  void GetAttenuatorMap(float* op_values, bool i_original = false);
  void SetAttenuatorMap(const float* i_values);

  void GetDwellCntrlMap(float* op_values, bool i_original = false);
  void SetDwellCntrlMap(const float* i_values);

  void GetCTSCurveMap(float* op_values, bool i_original = false);
  void SetCTSCurveMap(const float* i_values);

  float GetCTSMapVoltageLimit(int i_type);
  void  SetCTSMapVoltageLimit(int i_type, float i_value);

  void GetChokeOpMap(float* op_values, bool i_original = false);
  void SetChokeOpMap(const float* i_values);

  void GetRPMGridMap(float* op_values);
  void SetRPMGridMap(const float* i_values);

  //Types of slots/plugs
  enum IOXtype
  {
   IOX_INIT = 0,             //initialization slot
   IOX_DATA = 1              //data slot
  };

  //IDs of plugs
  enum IOPid
  {
   IOP_START      =   0,
   IOP_STARTv0    =   0,
   IOP_IGN_OUT1   =   0,     // IGN_OUT1    (output)                                 v1.0+
   IOP_IGN_OUT2   =   1,     // IGN_OUT2    (output)                                 v1.0+
   IOP_IGN_OUT3   =   2,     // IGN_OUT3    (output)
   IOP_IGN_OUT3v0 =   2,     // IGN_OUT3    (output)
   IOP_IGN_OUT4   =   3,     // IGN_OUT4    (output)
   IOP_IGN_OUT4v0 =   3,     // IGN_OUT4    (output)
   IOP_ADD_IO1    =   4,     // ADD_IO1     (output)  (applicable only in SECU-3T)
   IOP_ADD_IO1v0  =   4,     // ADD_IO1     (output)  (applicable only in SECU-3T)
   IOP_ADD_IO2    =   5,     // ADD_IO2     (output)  (applicable only in SECU-3T)
   IOP_ADD_IO2v0  =   5,     // ADD_IO2     (output)  (applicable only in SECU-3T)
   IOP_ECF        =   6,     // ECF         (output)
   IOP_ECFv0      =   0,     // ECF         (output)
   IOP_ST_BLOCK   =   7,     // ST_BLOCK    (output)
   IOP_ST_BLOCKv0 =   1,     // ST_BLOCK    (output)
   IOP_IE         =   8,     // IE          (output)
   IOP_IEv0       =   6,     // IE          (output)
   IOP_FE         =   9,     // FE          (output)
   IOP_FEv0       =   7,     // FE          (output)
   IOP_PS         =  10,     // PS          (input)                                  v1.0+
   IOP_ADD_I1     =  11,     // ADD_IO1     (input)   (applicable only in SECU-3T)   v1.0+
   IOP_ADD_I2     =  12,     // ADD_IO2     (input)   (applicable only in SECU-3T)   v1.0+
   IOP_CE         =  13,     // CE          (output)                                 v1.4+
   IOP_BL         =  14,     // Bootloader  (output)                                 v1.4+
   IOP_DE         =  15,     // Def. EEPROM (output)                                 v1.4+
   IOP_FL_PUMP    =  16,     // FL_PUMP     (output)
   IOP_FL_PUMPv0  =   8,     // FL_PUMP     (output)
   IOP_HALL_OUT   =  17,     // HALL_OUT    (output)
   IOP_HALL_OUTv0 =   9,     // HALL_OUT    (output)
   IOP_STROBE     =  18,     // STROBE      (output)
   IOP_STROBEv0   =  10,     // STROBE      (output)
   IOP_PWRRELAY   =  19,     // PWRRELAY    (output)
   IOP_PWRRELAYv0 =  11,     // PWRRELAY    (output)
   IOP_IGN        =  20,     // IGN         (input)                                  v1.0+
   IOP_IGN_OUT7   =  21,     // IGN_OUT7    (output)                                 v1.0+
   IOP_IGN_OUT8   =  22,     // IGN_OUT8    (output)                                 v1.0+
   IOP_BC_INPUT   =  23,     // BC_INPUT    (input)                                  v1.1+
   IOP_SM_DIR     =  24,     // SM_DIR      (output)                                 v1.2+
   IOP_SM_STP     =  25,     // SM_STP      (output)                                 v1.2+
   IOP_MAPSEL0    =  26,     // MAPSEL0     (input)                                  v1.3+
   IOP_SPDSENS    =  27,     // SPD_SENS    (input)                                  v1.5+
   IOP_INTK_HEAT  =  28,     // INTK_HEAT   (output)                                 v1.6+
// IOP_RESERVED14 =  29,     // Reserved    ()
// IOP_RESERVED15 =  30,     // Reserved    ()
// IOP_RESERVED16 =  31,     // Reserved    ()
   IOP_COUNT,                // Number of plugs used in I/O remapping
   IOP_COUNTv0    =  12,     // Number of plugs used in I/O remapping (v0.0)
   IOP_NA         =  255     //
  };

  //IDs of slots
  enum IOSid
  {
   IOS_START      =   0,
   IOS_STARTv0    =   0,
   IOS_IGN_OUT1   =   0,     // IGN_OUT1                                             v1.0+
   IOS_IGN_OUT2   =   1,     // IGN_OUT2                                             v1.0+
   IOS_IGN_OUT3   =   2,     // IGN_OUT3
   IOS_IGN_OUT3v0 =   2,     // IGN_OUT3
   IOS_IGN_OUT4   =   3,     // IGN_OUT4
   IOS_IGN_OUT4v0 =   3,     // IGN_OUT4
   IOS_ADD_IO1    =   4,     // ADD_IO1     (applicable only in SECU-3T)
   IOS_ADD_IO1v0  =   4,     // ADD_IO1     (applicable only in SECU-3T)
   IOS_ADD_IO2    =   5,     // ADD_IO2     (applicable only in SECU-3T)
   IOS_ADD_IO2v0  =   5,     // ADD_IO2     (applicable only in SECU-3T)
   IOS_ECF        =   6,     // ECF
   IOS_ECFv0      =   0,     // ECF
   IOS_ST_BLOCK   =   7,     // ST_BLOCK
   IOS_ST_BLOCKv0 =   1,     // ST_BLOCK
   IOS_IE         =   8,     // IE
   IOS_IEv0       =   6,     // IE
   IOS_FE         =   9,     // FE
   IOS_FEv0       =   7,     // FE
   //Added in V1.0:
   IOS_PS         =  10,     // PS          (input)
   IOS_ADD_I1     =  11,     // ADD_IO1     (input)
   IOS_ADD_I2     =  12,     // ADD_IO2     (input)
   IOS_CE         =  13,     // CE                                                   v1.4+
   IOS_BL         =  14,     // Bootloader  (output)                                 v1.4+
   IOS_DE         =  15,     // Def. EEPROM (output)                                 v1.4+
   IOS_COUNT,                // Number of slots used for I/O remapping
   IOS_COUNTv0    =  8,      // Number of slots used for I/O remapping (v0.0)
   IOS_NA         =  255     //
  };

  //Version of I/O remapping logic. Note that major version number changes only
  //in case of changing of data layout.
  enum IORemVer
  {
   IOV_V00 = 0,              //V0.0
   IOV_V10 = 0x10,           //V1.0 (major changes)
   IOV_V11 = 0x11,           //V1.1 (minor changes)
   IOV_V12 = 0x12,           //V1.2 (minor changes)
   IOV_V13 = 0x13,           //V1.3 (minor changes)
   IOV_V14 = 0x14,           //V1.4 (minor changes)
   IOV_V15 = 0x15,           //V1.5 (minor changes - SPD_SENS input added)
   IOV_V16 = 0x16            //V1.6 (minor changes - INTK_HEAT output added)
  };

  DWORD GetIOPlug(IOXtype type, IOPid id);
  DWORD GetIOSlot(IOXtype type, IOSid id, bool inv = false);
  DWORD GetSStub(void) const;
  DWORD GetGStub(void) const;
  void  SetIOPlug(IOXtype type, IOPid id, DWORD value);
  IORemVer GetIORemVersion(void) const;

  bool HasCodeData(const BYTE* ip_source_bytes = NULL) const;
  size_t GetOnlyCodeSize(const BYTE* ip_bytes) const;
  void LoadCodeData(const BYTE* ip_source_bytes, size_t i_srcSize, BYTE* op_destin_bytes = NULL);

  unsigned int CalculateCRC16OfActiveFirmware(void);
  unsigned int GetCRC16StoredInActiveFirmware(void);
  void CalculateAndPlaceFirmwareCRC(void);

  //-----------------------------------------------------------------------

  bool SetDefParamValues(BYTE i_descriptor, const void* ip_values);   //загнать указанные резервн.парам. в прошивку
  bool GetDefParamValues(BYTE i_descriptor, void* op_values);         //извлечь указанные резервн.парам. из прошивки

  //-----------------------------------------------------------------------

  const PPFlashParam& GetPlatformParams(void) const;

 private:
  CFirmwareDataMediator(const CFirmwareDataMediator& i);
  struct cd_data_t* _FindCodeData(const BYTE* ip_bytes = NULL, const PPFlashParam* ip_fpp = NULL);

 private:
  const std::auto_ptr<PPFlashParam> m_fpp;
  const std::auto_ptr<LocInfoProvider> m_lip;
  const size_t m_firmware_size;
  BYTE* mp_bytes_active;   //байты прошивки (копия для модификации)
  BYTE* mp_bytes_original; //байты прошивки (копия для сравнения)
  bool m_is_opened;
  _TSTRING m_fw_file_name;
  struct cd_data_t* mp_cddata;
};
