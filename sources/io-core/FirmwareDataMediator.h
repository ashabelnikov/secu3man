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

/** \file FirmwareDataMediator.h
 * \author Alexey A. Shabelnikov
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
  BYTE* GetFWVersion(BYTE* fw_version) const;

  //ignition
  void GetStartMap(int i_index, float* op_values, bool i_original = false);
  void SetStartMap(int i_index,const float* i_values);
  void GetIdleMap(int i_index,  float* op_values, bool i_original = false);
  void SetIdleMap(int i_index, const float* i_values);
  void GetWorkMap(int i_index, float* op_values, bool i_original = false);
  void SetWorkMap(int i_index, const float* i_values);
  void GetTempMap(int i_index, float* op_values, bool i_original = false);
  void SetTempMap(int i_index, const float* i_values);
  //fuel injection
  void GetVEMap(int i_index, float* op_values, bool i_original = false);
  void SetVEMap(int i_index, const float* i_values);
  void GetAFRMap(int i_index, float* op_values, bool i_original = false);
  void SetAFRMap(int i_index, const float* i_values);
  void GetCrnkMap(int i_index, float* op_values, bool i_original = false);
  void SetCrnkMap(int i_index, const float* i_values);
  void GetWrmpMap(int i_index, float* op_values, bool i_original = false);
  void SetWrmpMap(int i_index, const float* i_values);
  void GetDeadMap(int i_index, float* op_values, bool i_original = false);
  void SetDeadMap(int i_index, const float* i_values);
  void GetIdlrMap(int i_index, float* op_values, bool i_original = false);
  void SetIdlrMap(int i_index, const float* i_values);
  void GetIdlcMap(int i_index, float* op_values, bool i_original = false);
  void SetIdlcMap(int i_index, const float* i_values);
  void GetAETPSMap(int i_index, float* op_values, bool i_original = false);
  void SetAETPSMap(int i_index, const float* i_values);
  void GetAERPMMap(int i_index, float* op_values, bool i_original = false);
  void SetAERPMMap(int i_index, const float* i_values);
  void GetAftstrMap(int i_index, float* op_values, bool i_original = false);
  void SetAftstrMap(int i_index, const float* i_values);
  void GetITMap(int i_index, float* op_values, bool i_original = false);
  void SetITMap(int i_index, const float* i_values);
  void GetITRPMMap(int i_index, float* op_values, bool i_original = false);
  void SetITRPMMap(int i_index, const float* i_values);
  void GetRigidMap(int i_index, float* op_values, bool i_original = false);
  void SetRigidMap(int i_index, const float* i_values);
  void GetEGOCurveMap(int i_index, float* op_values, bool i_original = false);
  void SetEGOCurveMap(int i_index, const float* i_values);

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

  void GetATSCurveMap(float* op_values, bool i_original = false);
  void SetATSCurveMap(const float* i_values);

  float GetATSMapVoltageLimit(int i_type);
  void  SetATSMapVoltageLimit(int i_type, float i_value);

  void GetChokeOpMap(float* op_values, bool i_original = false);
  void SetChokeOpMap(const float* i_values);

  void GetRPMGridMap(float* op_values);
  void SetRPMGridMap(const float* i_values);

  void GetATSAACMap(float* op_values, bool i_original = false);
  void SetATSAACMap(const float* i_values);

  void GetGasdosePosMap(float* op_values, bool i_original = false);
  void SetGasdosePosMap(const float* i_values);

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
   IOP_ADD_IO1    =   4,     // ADD_IO1     (output)
   IOP_ADD_IO2    =   5,     // ADD_IO2     (output)
   IOP_ECF        =   6,     // ECF         (output)
   IOP_ST_BLOCK   =   7,     // ST_BLOCK    (output)
   IOP_IE         =   8,     // IE          (output)
   IOP_FE         =   9,     // FE          (output)
   IOP_PS         =  10,     // PS          (input)
   IOP_ADD_I1     =  11,     // ADD_IO1     (input)
   IOP_ADD_I2     =  12,     // ADD_IO2     (input)
   IOP_CE         =  13,     // CE          (output)
   IOP_BL         =  14,     // Bootloader  (output)
   IOP_DE         =  15,     // Def. EEPROM (output)
   IOP_GAS_V      =  16,     // GAS_V       (input)
   IOP_REF_S      =  17,     // REF_S       (input)
   IOP_CKPS       =  18,     // CKPS        (input)
   IOP_RESERVED2  =  19,     // Reserved    ()
   IOP_RESERVED3  =  20,     // Reserved    ()
   IOP_RESERVED4  =  21,     // Reserved    ()
   IOP_RESERVED5  =  22,     // Reserved    ()
   IOP_RESERVED6  =  23,     // Reserved    ()
   IOP_RESERVED7  =  24,     // Reserved    ()
   IOP_RESERVED8  =  25,     // Reserved    ()
   IOP_RESERVED9  =  26,     // Reserved    ()
   IOP_RESERVED10 =  27,     // Reserved    ()
   IOP_RESERVED11 =  28,     // Reserved    ()
   IOP_RESERVED12 =  29,     // Reserved    ()
   IOP_RESERVED13 =  30,     // Reserved    ()
   IOP_RESERVED14 =  31,     // Reserved    ()
   IOP_RESERVED15 =  32,     // Reserved    ()
   IOP_RESERVED16 =  33,     // Reserved    ()
   IOP_RESERVED17 =  34,     // Reserved    ()
   IOP_RESERVED18 =  35,     // Reserved    ()
   IOP_RESERVED19 =  36,     // Reserved    ()
   IOP_FL_PUMP    =  37,     // FL_PUMP     (output)
   IOP_HALL_OUT   =  38,     // HALL_OUT    (output)
   IOP_STROBE     =  39,     // STROBE      (output)
   IOP_PWRRELAY   =  40,     // PWRRELAY    (output)
   IOP_IGN        =  41,     // IGN         (input)
   IOP_IGN_OUT7   =  42,     // IGN_OUT7    (output)
   IOP_IGN_OUT8   =  43,     // IGN_OUT8    (output)
   IOP_BC_INPUT   =  44,     // BC_INPUT    (input)
   IOP_SM_DIR     =  45,     // SM_DIR      (output)
   IOP_SM_STP     =  46,     // SM_STP      (output)
   IOP_MAPSEL0    =  47,     // MAPSEL0     (input)
   IOP_SPDSENS    =  48,     // SPD_SENS    (input)
   IOP_INTK_HEAT  =  49,     // INTK_HEAT   (output)
   IOP_LAMBDA     =  50,     // LAMBDA      (input)
   IOP_AIR_TEMP   =  51,     // AIR_TEMP    (input)
   IOP_UNI_OUT0   =  52,     // UNI_OUT0    (output)
   IOP_UNI_OUT1   =  53,     // UNI_OUT1    (output)
   IOP_UNI_OUT2   =  54,     // UNI_OUT2    (output)
   IOP_INJ_OUT0   =  55,     // INJ_OUT0    (output)
   IOP_INJ_OUT1   =  56,     // INJ_OUT1    (output)
   IOP_INJ_OUT2   =  57,     // INJ_OUT2    (output)
   IOP_INJ_OUT3   =  58,     // INJ_OUT3    (output)
   IOP_IAC_PWM    =  59,     // IAC_PWM     (output)
   IOP_GD_DIR     =  60,     // GD_DIR      (output)
   IOP_GD_STP     =  61,     // GD_STP      (output)
   IOP_GD_PWM     =  62,     // GD_PWM      (output)
// IOP_RESERVED24 =  63,     // Reserved    ()
// IOP_RESERVED25 =  64,     // Reserved    ()
// IOP_RESERVED26 =  65,     // Reserved    ()
// IOP_RESERVED27 =  66,     // Reserved    ()
// IOP_RESERVED28 =  67,     // Reserved    ()
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
   IOS_ADD_IO1    =   4,     // ADD_IO1
   IOS_ADD_IO2    =   5,     // ADD_IO2
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
   IOS_REF_S      =  17,     // REF_S       (input)
   IOS_CKPS       =  18,     // CKPS        (input)
// IOS_RESERVED2  =  19,     // Reserved    ()
// IOS_RESERVED3  =  20,     // Reserved    ()
// IOS_RESERVED4  =  21,     // Reserved    ()
// IOS_RESERVED5  =  22,     // Reserved    ()
// IOS_RESERVED6  =  23,     // Reserved    ()
// IOS_RESERVED7  =  24,     // Reserved    ()
// IOS_RESERVED8  =  25,     // Reserved    ()
// IOS_RESERVED9  =  26,     // Reserved    ()
// IOS_RESERVED10 =  27,     // Reserved    ()
// IOS_RESERVED11 =  28,     // Reserved    ()
// IOS_RESERVED12 =  29,     // Reserved    ()
// IOS_RESERVED13 =  30,     // Reserved    ()
// IOS_RESERVED14 =  31,     // Reserved    ()
// IOS_RESERVED15 =  32,     // Reserved    ()
// IOS_RESERVED16 =  33,     // Reserved    ()
// IOS_RESERVED17 =  34,     // Reserved    ()
// IOS_RESERVED18 =  35,     // Reserved    ()
// IOS_RESERVED19 =  36,     // Reserved    ()
   IOS_COUNT,                // Number of slots used for I/O remapping
   IOS_NA         =  255     //
  };

  //Version of I/O remapping logic. Note that major version number changes only
  //in case of changing of data layout.
  enum IORemVer
  {
   IOV_V23 = 0x23,           //V2.3
   IOV_V24 = 0x24,           //V2.4  CKPS remapping added, gas dose outputs
   IOV_V25 = 0x25,           //V2.5  GD_PWM plug added
  };

  DWORD GetIOPlug(IOXtype type, IOPid id);
  DWORD GetIOSlot(IOXtype type, IOSid id, bool inv = false);
  DWORD GetSStub(void) const;
  DWORD GetGStub(void) const;
  void  SetIOPlug(IOXtype type, IOPid id, DWORD value);
  IORemVer GetIORemVersion(void) const;

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

  BYTE* getBytes(bool i_original = false);

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
