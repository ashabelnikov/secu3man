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

  //NOTE: we break compatibility with v0.x and v1.x because of support complexity
  //IDs of plugs
  enum IOPid
  {
   IOP_START      =   0,
   IOP_IGN_OUT1   =   0,     // IGN_OUT1    (output)
   IOP_IGN_OUT2   =   1,     // IGN_OUT2    (output)
   IOP_IGN_OUT3   =   2,     // IGN_OUT3    (output)
   IOP_IGN_OUT4   =   3,     // IGN_OUT4    (output)
   IOP_ADD_IO1    =   4,     // ADD_IO1     (output)  (applicable only in SECU-3T)
   IOP_ADD_IO2    =   5,     // ADD_IO2     (output)  (applicable only in SECU-3T)
   IOP_ECF        =   6,     // ECF         (output)
   IOP_ST_BLOCK   =   7,     // ST_BLOCK    (output)
   IOP_IE         =   8,     // IE          (output)
   IOP_FE         =   9,     // FE          (output)
   IOP_PS         =  10,     // PS          (input)
   IOP_ADD_I1     =  11,     // ADD_IO1     (input)   (applicable only in SECU-3T)
   IOP_ADD_I2     =  12,     // ADD_IO2     (input)   (applicable only in SECU-3T)
   IOP_CE         =  13,     // CE          (output)
   IOP_BL         =  14,     // Bootloader  (output)
   IOP_DE         =  15,     // Def. EEPROM (output)
   IOP_GAS_V      =  16,     // GAS_V       (input)
   IOP_REF_S      =  17,     // REF_S       (input)   (applicable only in SECU-3T)
   IOP_RESERVED1  =  18,     // Reserved    ()
   IOP_RESERVED2  =  19,     // Reserved    ()
   IOP_RESERVED3  =  20,     // Reserved    ()
   IOP_RESERVED4  =  21,     // Reserved    ()
   IOP_RESERVED5  =  22,     // Reserved    ()
   IOP_FL_PUMP    =  23,     // FL_PUMP     (output)
   IOP_HALL_OUT   =  24,     // HALL_OUT    (output)
   IOP_STROBE     =  25,     // STROBE      (output)
   IOP_PWRRELAY   =  26,     // PWRRELAY    (output)
   IOP_IGN        =  27,     // IGN         (input)
   IOP_IGN_OUT7   =  28,     // IGN_OUT7    (output)
   IOP_IGN_OUT8   =  29,     // IGN_OUT8    (output)
   IOP_BC_INPUT   =  30,     // BC_INPUT    (input)
   IOP_SM_DIR     =  31,     // SM_DIR      (output)
   IOP_SM_STP     =  32,     // SM_STP      (output)
   IOP_MAPSEL0    =  33,     // MAPSEL0     (input)
   IOP_SPDSENS    =  34,     // SPD_SENS    (input)
   IOP_INTK_HEAT  =  35,     // INTK_HEAT   (output)
   IOP_LAMBDA     =  36,     // LAMBDA      (input)
   IOP_AIR_TEMP   =  37,     // AIR_TEMP    (input)
   IOP_UNI_OUT0   =  38,     // UNI_OUT0    (output)
   IOP_UNI_OUT1   =  39,     // UNI_OUT1    (output)
   IOP_UNI_OUT2   =  40,     // UNI_OUT2    (output)
   IOP_INJ_OUT0   =  41,     // INJ_OUT0    (output)
// IOP_RESERVED8  =  42,     // Reserved    ()
// IOP_RESERVED9  =  43,     // Reserved    ()
// IOP_RESERVED10 =  44,     // Reserved    ()
// IOP_RESERVED11 =  45,     // Reserved    ()
// IOP_RESERVED12 =  46,     // Reserved    ()
// IOP_RESERVED13 =  47,     // Reserved    ()
// IOP_RESERVED14 =  48,     // Reserved    ()
// IOP_RESERVED15 =  49,     // Reserved    ()
   IOP_COUNT,                // Number of plugs used in I/O remapping
   IOP_NA         =  255     //
  };

  //IDs of slots
  enum IOSid
  {
   IOS_START      =   0,
   IOS_IGN_OUT1   =   0,     // IGN_OUT1
   IOS_IGN_OUT2   =   1,     // IGN_OUT2
   IOS_IGN_OUT3   =   2,     // IGN_OUT3
   IOS_IGN_OUT4   =   3,     // IGN_OUT4
   IOS_ADD_IO1    =   4,     // ADD_IO1              (applicable only in SECU-3T)
   IOS_ADD_IO2    =   5,     // ADD_IO2              (applicable only in SECU-3T)
   IOS_ECF        =   6,     // ECF
   IOS_ST_BLOCK   =   7,     // ST_BLOCK
   IOS_IE         =   8,     // IE
   IOS_FE         =   9,     // FE
   IOS_PS         =  10,     // PS          (input)
   IOS_ADD_I1     =  11,     // ADD_IO1     (input)
   IOS_ADD_I2     =  12,     // ADD_IO2     (input)
   IOS_CE         =  13,     // CE
   IOS_BL         =  14,     // Bootloader  (output)
   IOS_DE         =  15,     // Def. EEPROM (output)
   IOS_GAS_V      =  16,     // GAS_V       (input)
   IOS_REF_S      =  17,     // REF_S       (input)  (applicable only in SECU-3T)
// IOS_RESERVED1  =  18,     // Reserved    ()
// IOS_RESERVED2  =  19,     // Reserved    ()
// IOS_RESERVED3  =  20,     // Reserved    ()
// IOS_RESERVED4  =  21,     // Reserved    ()
// IOS_RESERVED5  =  22,     // Reserved    ()
   IOS_COUNT,                // Number of slots used for I/O remapping
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
   IOV_V16 = 0x16,           //V1.6 (minor changes - INTK_HEAT output added)
   IOV_V20 = 0x20            //V2.0 (major changes)
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

  void SetNumPulsesPer1Km(int pp1km);
  void SetQuartzFrq(long frq);

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

  float m_period_distance;              //distance of one period in meters (speed sensor), used in calculations
  long m_quartz_frq;                    //MCU clock frequency
};
