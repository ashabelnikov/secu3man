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
#include "ParamsIO.h"

struct CESettingsData;
namespace SECU3IO { struct FwConstsData; }

//Этот класс необходим для абстракции над форматом хранения данных в прошивке
//(памяти программ микроконтроллера), а также для хранения этих данных.
struct PPFlashParam;
class IOCORE_API CFirmwareDataMediator : public ParamsIO
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

  void GetSetMap(int i_index, int id, float* op_values, bool i_original = false);
  void SetSetMap(int i_index, int id, const float* ip_values);
  void GetSepMap(int id, float* op_values, bool i_original = false);
  void SetSepMap(int id, const float* ip_values);

  void GetMapsData(struct FWMapsDataHolder* op_fwd);
  void SetMapsData(const struct FWMapsDataHolder* ip_fwd);
  bool CheckRPMGridsCompatibility(const float* rpmGrid);
  bool CheckCLTGridsCompatibility(const float* rpmGrid);
  bool CheckLoadGridsCompatibility(const float* rpmGrid);

  std::vector<_TSTRING> GetFunctionsSetNames(void);
  void SetFunctionsSetName(int i_index, _TSTRING i_new_name);

  void SetFWFileName(const _TSTRING i_fw_file_name);
  _TSTRING GetFWFileName(void);

  void GetRPMGridMap(float* op_values);
  void SetRPMGridMap(const float* i_values);
  void GetCLTGridMap(float* op_values);
  void SetCLTGridMap(const float* i_values);
  void GetLoadGridMap(float* op_values);
  void SetLoadGridMap(const float* i_values);
  void GetCESettingsData(CESettingsData& o_data) const;
  void SetCESettingsData(const CESettingsData& i_data);
  void GetFwConstsData(SECU3IO::FwConstsData& o_data) const;
  void SetFwConstsData(const SECU3IO::FwConstsData& i_data);

  //Types of slots/plugs
  enum IOXtype
  {
   IOX_INIT = 0,             //initialization slot
   IOX_DATA = 1              //data slot
  };

  //IDs of plugs
  enum IOPid
  {
   IOP_START        =   0,
   //SECU-3i:
   IOP3I_IGN_O1     =   0,
   IOP3I_IGN_O2     =   1,
   IOP3I_IGN_O3     =   2,
   IOP3I_IGN_O4     =   3,
   IOP3I_IGN_O5     =   4,
   IOP3I_ECF        =   5,
   IOP3I_INJ_O1     =   6,
   IOP3I_INJ_O2     =   7,
   IOP3I_INJ_O3     =   8,
   IOP3I_INJ_O4     =   9,
   IOP3I_INJ_O5     =  10,
   IOP3I_BL         =  11,
   IOP3I_DE         =  12,
   IOP3I_STBL_O     =  13,
   IOP3I_CEL_O      =  14,
   IOP3I_FPMP_O     =  15,
   IOP3I_PWRR_O     =  16,
   IOP3I_EVAP_O     =  17,
   IOP3I_O2SH_O     =  18,
   IOP3I_COND_O     =  19,
   IOP3I_ADD_O2     =  20,
   IOP3I_PS         =  21,     // PS          (input)
   IOP3I_REF_S      =  22,     // REF_S       (input)
   IOP3I_CKPS       =  23,     // CKPS        (input)
   IOP3I_ADD_I1     =  24,     // ADD_I1      (input)
   IOP3I_ADD_I2     =  25,     // ADD_I2      (input)
   IOP3I_ADD_I3     =  26,     // ADD_I3      (input)
   IOP3I_GAS_V      =  27,     // GAS_V       (input)
   IOP3I_IGN_I      =  28,     // IGN_I       (input)
   IOP3I_COND_I     =  29,     // COND_I      (input)
   IOP3I_EPAS_I     =  30,     // EPAS_I      (input)
   IOP3I_ADD_I4     =  31,     // ADD_I4      (input)
   IOP3I_TACH_O     =  32,     // TACH_O      (output)
   IOP3I_KSP_CS     =  33,     // KSP_CS      (output)
   IOP3I_ADD_I5     =  34,     // ADD_I5      (input)
   IOP3I_ADD_I6     =  35,     // ADD_I6      (input)
   IOP3I_ADD_I7     =  36,     // ADD_I7      (input)
   IOP3I_ADD_I8     =  37,     // ADD_I8      (input)
   IOP3I_MAP_I      =  38,     // MAP_I       (input)
   IOP3I_OILP_I     =  39,     // OILP_I      (input)
   IOP3I_GENS_I     =  40,     // GENS_I      (input)   
   //reserved slots are here
   IOP3I_RESERVED4  =  41,     // Reserved      ()
   IOP3I_RESERVED5  =  42,     // Reserved      ()
   IOP3I_RESERVED6  =  43,     // Reserved      ()
   IOP3I_RESERVED7  =  44,     // Reserved      ()
   IOP3I_RESERVED8  =  45,     // Reserved      ()
   IOP3I_RESERVED9  =  46,     // Reserved      ()
   IOP3I_RESERVED10 =  47,     // Reserved      ()
   IOP3I_RESERVED11 =  48,     // Reserved      ()

   IOP3I_IGN_OUT6   =  49,     // IGN_O6    (output)
   IOP3I_IGN_OUT7   =  50,     // IGN_O7    (output)
   IOP3I_IGN_OUT8   =  51,     // IGN_O8    (output)
   IOP3I_SM_DIR     =  52,     // SM_DIR    (output)
   IOP3I_SM_STP     =  53,     // SM_STP    (output)
   IOP3I_GD_DIR     =  54,     // GD_DIR    (output)
   IOP3I_GD_STP     =  55,     // GD_STP    (output)
   IOP3I_GD_PWM     =  56,     // GD_PWM    (output)
   IOP3I_IAC_PWM    =  57,     // IAC_PWM   (output)
   IOP3I_HALL_OUT   =  58,     // HALL_OUT  (output)
   IOP3I_STROBE     =  59,     // STROBE    (output)
   IOP3I_INTK_HEAT  =  60,     // INTK_HEAT (output)
   IOP3I_UNI_OUT0   =  61,     // UNI_OUT0  (output)
   IOP3I_UNI_OUT1   =  62,     // UNI_OUT1  (output)
   IOP3I_UNI_OUT2   =  63,     // UNI_OUT2  (output)
   IOP3I_UNI_OUT3   =  64,     // UNI_OUT3  (output)
   IOP3I_UNI_OUT4   =  65,     // UNI_OUT4  (output)
   IOP3I_UNI_OUT5   =  66,     // UNI_OUT5  (output)
   IOP3I_IE         =  67,     // IE        (output)
   IOP3I_FE         =  68,     // FE        (output)
   IOP3I_BC_INPUT   =  69,     // BC_INPUT  (input)
   IOP3I_MAPSEL0    =  70,     // MAPSEL0   (input)
   IOP3I_SPDSENS    =  71,     // SPD_SENS  (input)
   IOP3I_LAMBDA     =  72,     // LAMBDA    (input)
   IOP3I_AIR_TEMP   =  73,     // AIR_TEMP  (input)
   IOP3I_CAN_CS     =  74,     // CAN_CS    (output)
   IOP3I_INJ_O6     =  75,     // INJ_O6    (output)
   IOP3I_INJ_O7     =  76,     // INJ_O7    (output)
   IOP3I_INJ_O8     =  77,     // INJ_O8    (output)
   IOP3I_IGNTIM     =  78,     // IGNTIM    (input)
   IOP3I_MAP2       =  79,     // MAP2      (input)
   IOP3I_TMP2       =  80,     // TEMP2     (input)
   IOP3I_PWM1       =  81,     // PWM1      (output)
   IOP3I_PWM2       =  82,     // PWM2      (output)
   IOP3I_FL_CONS    =  83,     // FL_CONS   (output)
   IOP3I_AUTO_I     =  84,     // AUTO_I    (input)
   IOP3I_VTACHOM    =  85,     // VTACHOM   (output)
   IOP3I_GRTEMP     =  86,     // GRTEMP    (input)
   IOP3I_GRHEAT     =  87,     // GRHEAT    (output)
   IOP3I_GASVAL_O   =  88,     // GASVAL_O  (output)
   IOP3I_FTLS_I     =  89,     // FTLS_I    (input)
   IOP3I_EGTS_I     =  90,     // EGTS_I    (input)
   IOP3I_OPS_I      =  91,     // OPS_I     (input)
   IOP3I_INJPWC_I   =  92,     // INJPWC_I  (input)
   IOP3I_INPUT1     =  93,     // INPUT1    (input)
   IOP3I_INPUT2     =  94,     // INPUT2    (input)
   IOP3I_REFPRS_I   =  95,     // REFPRS_I  (input)
   IOP3I_MAF        =  96,     // MAF       (input)
   IOP3I_FTS_I      =  97,     // FTS_I     (input)
   IOP3I_LAMBDA2    =  98,     // LAMBDA2   (input)
// IOP3I_RESERVED46 =  99,     // Reserved  ()
   IOP3I_COUNT      =  99,     //DON'T forget to update this value

   //SECU-3T:
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
   IOP_MAP_S      =  19,     // MAP_S       (input)
   IOP_ADD_I4     =  20,     // ADD_I4      (input)
   IOP_RESERVED2  =  21,     // Reserved    ()
   IOP_RESERVED3  =  22,     // Reserved    ()
   IOP_RESERVED4  =  23,     // Reserved    ()
   IOP_RESERVED5  =  24,     // Reserved    ()
   IOP_RESERVED6  =  25,     // Reserved    ()
   IOP_RESERVED7  =  26,     // Reserved    ()
   IOP_RESERVED8  =  27,     // Reserved    ()
   IOP_RESERVED9  =  28,     // Reserved    ()
   IOP_RESERVED10 =  29,     // Reserved    ()
   IOP_RESERVED11 =  30,     // Reserved    ()
   IOP_RESERVED12 =  31,     // Reserved    ()
   IOP_RESERVED13 =  32,     // Reserved    ()
   IOP_RESERVED14 =  33,     // Reserved    ()
   IOP_RESERVED15 =  34,     // Reserved    ()
   IOP_RESERVED16 =  35,     // Reserved    ()
   IOP_RESERVED17 =  36,     // Reserved    ()
   IOP_RESERVED18 =  37,     // Reserved    ()
   IOP_RESERVED19 =  38,     // Reserved    ()
   IOP_RESERVED20 =  39,     // Reserved    ()
   IOP_RESERVED21 =  40,     // Reserved    ()
   IOP_RESERVED22 =  41,     // Reserved    ()
   IOP_RESERVED23 =  42,     // Reserved    ()
   IOP_RESERVED24 =  43,     // Reserved    ()
   IOP_RESERVED25 =  44,     // Reserved    ()
   IOP_RESERVED26 =  45,     // Reserved    ()
   IOP_RESERVED27 =  46,     // Reserved    ()
   IOP_RESERVED28 =  47,     // Reserved    ()
   IOP_RESERVED29 =  48,     // Reserved    ()

   IOP_FL_PUMP    =  49,     // FL_PUMP     (output)
   IOP_HALL_OUT   =  50,     // HALL_OUT    (output)
   IOP_STROBE     =  51,     // STROBE      (output)
   IOP_PWRRELAY   =  52,     // PWRRELAY    (output)
   IOP_IGN        =  53,     // IGN         (input)
   IOP_IGN_OUT7   =  54,     // IGN_OUT7    (output)
   IOP_IGN_OUT8   =  55,     // IGN_OUT8    (output)
   IOP_BC_INPUT   =  56,     // BC_INPUT    (input)
   IOP_SM_DIR     =  57,     // SM_DIR      (output)
   IOP_SM_STP     =  58,     // SM_STP      (output)
   IOP_MAPSEL0    =  59,     // MAPSEL0     (input)
   IOP_SPDSENS    =  60,     // SPD_SENS    (input)
   IOP_INTK_HEAT  =  61,     // INTK_HEAT   (output)
   IOP_LAMBDA     =  62,     // LAMBDA      (input)
   IOP_AIR_TEMP   =  63,     // AIR_TEMP    (input)
   IOP_UNI_OUT0   =  64,     // UNI_OUT0    (output)
   IOP_UNI_OUT1   =  65,     // UNI_OUT1    (output)
   IOP_UNI_OUT2   =  66,     // UNI_OUT2    (output)
   IOP_UNI_OUT3   =  67,     // UNI_OUT3    (output)
   IOP_UNI_OUT4   =  68,     // UNI_OUT4    (output)
   IOP_UNI_OUT5   =  69,     // UNI_OUT5    (output)
   IOP_INJ_OUT1   =  70,     // INJ_OUT1    (output)
   IOP_INJ_OUT2   =  71,     // INJ_OUT2    (output)
   IOP_INJ_OUT3   =  72,     // INJ_OUT3    (output)
   IOP_INJ_OUT4   =  73,     // INJ_OUT4    (output)
   IOP_IAC_PWM    =  74,     // IAC_PWM     (output)
   IOP_GD_DIR     =  75,     // GD_DIR      (output)
   IOP_GD_STP     =  76,     // GD_STP      (output)
   IOP_GD_PWM     =  77,     // GD_PWM      (output)
   IOP_INJ_OUT5   =  78,     // INJ_OUT5    (output)
   IOP_INJ_OUT6   =  79,     // INJ_OUT6    (output)
   IOP_COND_I     =  80,     // COND_I      (input)
   IOP_O2SH_O     =  81,     // O2SH_O      (output)
   IOP_PWM1       =  82,     // PWM1        (output)
   IOP_PWM2       =  83,     // PWM2        (output)
   IOP_FL_CONS    =  84,     // FL_CONS     (output)
   IOP_AUTO_I     =  85,     // AUTO_I      (input)
   IOP_VTACHOM    =  86,     // VTACHOM     (output)
   IOP_INPUT1     =  87,     // INPUT1      (input)
   IOP_INPUT2     =  88,     // INPUT2      (input)
   IOP_MAF        =  89,     // MAF         (input)
   IOP_LAMBDA2    =  90,     // LAMBDA2     (input)
   IOP_IGNTIM     =  91,     // IOP_IGNTIM  (input)
   IOP_MAP2       =  92,     // MAP2        (input)
// IOP_RESERVED36 =  93,     // Reserved    ()
// IOP_RESERVED37 =  94,     // Reserved    ()
// IOP_RESERVED38 =  95,     // Reserved    ()
// IOP_RESERVED39 =  96,     // Reserved    ()
// IOP_RESERVED40 =  97,     // Reserved    ()
// IOP_RESERVED41 =  98,     // Reserved    ()
// IOP_RESERVED42 =  99,     // Reserved    ()
   IOP_COUNT      =  93,     // Number of plugs used in I/O remapping
   IOP_NA         =  255     //
  };

  //IDs of slots
  enum IOSid
  {
   IOS_START        =   0,
   //SECU-3i:
   IOS3I_IGN_O1     =   0,
   IOS3I_IGN_O2     =   1,
   IOS3I_IGN_O3     =   2,
   IOS3I_IGN_O4     =   3,
   IOS3I_IGN_O5     =   4,
   IOS3I_ECF        =   5,
   IOS3I_INJ_O1     =   6,
   IOS3I_INJ_O2     =   7,
   IOS3I_INJ_O3     =   8,
   IOS3I_INJ_O4     =   9,
   IOS3I_INJ_O5     =  10,
   IOS3I_BL         =  11,
   IOS3I_DE         =  12,
   IOS3I_STBL_O     =  13,
   IOS3I_CEL_O      =  14,
   IOS3I_FPMP_O     =  15,
   IOS3I_PWRR_O     =  16,
   IOS3I_EVAP_O     =  17,
   IOS3I_O2SH_O     =  18,
   IOS3I_COND_O     =  19,
   IOS3I_ADD_O2     =  20,
   IOS3I_PS         =  21,     // PS          (input)
   IOS3I_REF_S      =  22,     // REF_S       (input)
   IOS3I_CKPS       =  23,     // CKPS        (input)
   IOS3I_ADD_I1     =  24,     // ADD_IO1     (input)
   IOS3I_ADD_I2     =  25,     // ADD_IO2     (input)
   IOS3I_ADD_I3     =  26,     // ADD_IO3     (input)
   IOS3I_GAS_V      =  27,     // GAS_V       (input)
   IOS3I_IGN_I      =  28,     // IGN_I       (input)
   IOS3I_COND_I     =  29,     // COND_I      (input)
   IOS3I_EPAS_I     =  30,     // EPAS_I      (input)
   IOS3I_ADD_I4     =  31,     // ADD_I4      (input)
   IOS3I_TACH_O     =  32,     // TACH_O      (output)
   IOS3I_KSP_CS     =  33,     // KSP_CS      (output)
   IOS3I_ADD_I5     =  34,     // ADD_I5      (input)
   IOS3I_ADD_I6     =  35,     // ADD_I6      (input)
   IOS3I_ADD_I7     =  36,     // ADD_I7      (input)
   IOS3I_ADD_I8     =  37,     // ADD_I8      (input)
   IOS3I_MAP_I      =  38,     // MAP_I       (input)
   IOS3I_OILP_I     =  39,     // OILP_I      (input)
   IOS3I_GENS_I     =  40,     // GENS_I      (input)
// IOS_RESERVED4    =  41,     // Reserved    ()
// IOS_RESERVED5    =  42,     // Reserved    ()
// IOS_RESERVED6    =  43,     // Reserved    ()
// IOS_RESERVED7    =  44,     // Reserved    ()
// IOS_RESERVED8    =  45,     // Reserved    ()
// IOS_RESERVED9    =  46,     // Reserved    ()
// IOS_RESERVED10   =  47,     // Reserved    ()
// IOS_RESERVED11   =  48,     // Reserved    ()
   //reserved slots are here
   IOS3I_COUNT      =  39,
   //SECU-3T:   
   IOS_IGN_OUT1     =   0,     // IGN_OUT1
   IOS_IGN_OUT2     =   1,     // IGN_OUT2
   IOS_IGN_OUT3     =   2,     // IGN_OUT3
   IOS_IGN_OUT4     =   3,     // IGN_OUT4
   IOS_ADD_IO1      =   4,     // ADD_IO1
   IOS_ADD_IO2      =   5,     // ADD_IO2
   IOS_ECF          =   6,     // ECF
   IOS_ST_BLOCK     =   7,     // ST_BLOCK
   IOS_IE           =   8,     // IE
   IOS_FE           =   9,     // FE
   IOS_PS           =  10,     // PS          (input)
   IOS_ADD_I1       =  11,     // ADD_IO1     (input)
   IOS_ADD_I2       =  12,     // ADD_IO2     (input)
   IOS_CE           =  13,     // CE
   IOS_BL           =  14,     // Bootloader  (output)
   IOS_DE           =  15,     // Def. EEPROM (output)
   IOS_GAS_V        =  16,     // GAS_V       (input)
   IOS_REF_S        =  17,     // REF_S       (input)
   IOS_CKPS         =  18,     // CKPS        (input)
   IOS_MAP_S        =  19,     // MAP_S       (input)
   IOS_ADD_I4       =  20,     // ADD_I4      (input)
// IOS_RESERVED2    =  21,     // Reserved    ()
// IOS_RESERVED3    =  22,     // Reserved    ()
// IOS_RESERVED4    =  23,     // Reserved    ()
// IOS_RESERVED5    =  24,     // Reserved    ()
// IOS_RESERVED6    =  25,     // Reserved    ()
// IOS_RESERVED7    =  26,     // Reserved    ()
// IOS_RESERVED8    =  27,     // Reserved    ()
// IOS_RESERVED9    =  28,     // Reserved    ()
// IOS_RESERVED10   =  29,     // Reserved    ()
// IOS_RESERVED11   =  30,     // Reserved    ()
// IOS_RESERVED12   =  31,     // Reserved    ()
// IOS_RESERVED13   =  32,     // Reserved    ()
// IOS_RESERVED14   =  33,     // Reserved    ()
// IOS_RESERVED15   =  34,     // Reserved    ()
// IOS_RESERVED16   =  35,     // Reserved    ()
// IOS_RESERVED17   =  36,     // Reserved    ()
// IOS_RESERVED18   =  37,     // Reserved    ()
// IOS_RESERVED18   =  38,     // Reserved    ()
// IOS_RESERVED18   =  39,     // Reserved    ()
// IOS_RESERVED18   =  40,     // Reserved    ()
// IOS_RESERVED18   =  41,     // Reserved    ()
// IOS_RESERVED18   =  42,     // Reserved    ()
// IOS_RESERVED18   =  43,     // Reserved    ()
// IOS_RESERVED18   =  44,     // Reserved    ()
// IOS_RESERVED18   =  45,     // Reserved    ()
// IOS_RESERVED18   =  46,     // Reserved    ()
// IOS_RESERVED18   =  47,     // Reserved    ()
// IOS_RESERVED18   =  48,     // Reserved    ()
   IOS_COUNT        =  21,     // Number of slots used for I/O remapping
   IOS_NA           =  255     //
  };

  //Version of I/O remapping logic. Note that major version number changes only
  //in case of changing of data layout.
  enum IORemVer
  {
   IOV_V23 = 0x23,           //V2.3
   IOV_V24 = 0x24,           //V2.4  CKPS remapping added, gas dose outputs
   IOV_V25 = 0x25,           //V2.5  GD_PWM plug added
   IOV_V26 = 0x26,           //V2.6  Support of SECU-3i added
   IOV_V27 = 0x27,           //V2.7  additional injection channels were added (5,6 for SECU-3T and 6,7,8 for SECU-3i)
   IOV_V28 = 0x28,           //V2.8  TACH_O and KSP_CS outputs were added
   IOV_V29 = 0x29,           //V2.9  ADD_I5 input has been added
   IOV_V30 = 0x30,           //V3.0  New reserved slots and plugs were added
   IOV_V31 = 0x31,           //V3.1  New plugs were added (PWM1, PWM2, FL_CONS, AUTO_I, UNI_OUT3,4,5, VTACHOM)
   IOV_V32 = 0x32,           //V3.2  New slots were added (ADD_I5,6,7,8, GRTEMP, GRHEAT)
   IOV_V33 = 0x33,           //V3.3  New slots were added (added possibility to remap ADD_I5/7 as MAP2, GASVAL_O plug added)
   IOV_V34 = 0x34,           //V3.4  New plugs: FTLS_I, EGTS_I, OPS_I, INJPWC_I; Plus extra reserved plugs
   IOV_V35 = 0x35,           //v3.5  New plugs: INPUT1, INPUT2, REFPRS_I
   IOV_V36 = 0x36,           //v3.6  New slots: MAP_x; new plugs: MAF
   IOV_V37 = 0x37,           //v3.7  ADD_I3 --> FTLS_I
   IOV_V38 = 0x38,           //v3.8  FTS_I added
   IOV_V39 = 0x39            //v3.9  LAMBDA2 added, ADD_I4, IGNTIM and MAP2 for SECU3T added
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

  const PPFlashParam& GetPlatformParams(void) const;

protected:
  //ignition
  void GetStartMap(int i_index, float* op_values, bool i_original = false);
  void SetStartMap(int i_index,const float* i_values);
  void GetIdleMap(int i_index,  float* op_values, bool i_original = false);
  void SetIdleMap(int i_index, const float* i_values);
  void GetWorkMap(int i_index, float* op_values, bool i_original = false);
  void SetWorkMap(int i_index, const float* i_values);
  void GetTempMap(int i_index, float* op_values, bool i_original = false);
  void SetTempMap(int i_index, const float* i_values);
  void GetTempIdlMap(int i_index, float* op_values, bool i_original = false);
  void SetTempIdlMap(int i_index, const float* i_values);
  //fuel injection
  void GetVEMap(int i_index, float* op_values, bool i_original = false);
  void SetVEMap(int i_index, const float* i_values);
  void GetVE2Map(int i_index, float* op_values, bool i_original = false);
  void SetVE2Map(int i_index, const float* i_values);
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
  void GetThrassMap(int i_index, float* op_values, bool i_original = false);
  void SetThrassMap(int i_index, const float* i_values);
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
  void GetIACCorrWMap(int i_index, float* op_values, bool i_original = false);
  void SetIACCorrWMap(int i_index, const float* i_values);
  void GetIACCorrMap(int i_index, float* op_values, bool i_original = false);
  void SetIACCorrMap(int i_index, const float* i_values);
  void GetIATCLTMap(int i_index, float* op_values, bool i_original = false);
  void SetIATCLTMap(int i_index, const float* i_values);
  void GetTpsswtMap(int i_index, float* op_values, bool i_original = false);
  void SetTpsswtMap(int i_index, const float* i_values);
  void GetGtscMap(int i_index, float* op_values, bool i_original = false);
  void SetGtscMap(int i_index, const float* i_values);
  void GetGpscMap(int i_index, float* op_values, bool i_original = false);
  void SetGpscMap(int i_index, const float* i_values);
  void GetAtscMap(int i_index, float* op_values, bool i_original = false);
  void SetAtscMap(int i_index, const float* i_values);
  void GetPwm1Map(int i_index, float* op_values, bool i_original = false);
  void SetPwm1Map(int i_index, const float* i_values);
  void GetPwm2Map(int i_index, float* op_values, bool i_original = false);
  void SetPwm2Map(int i_index, const float* i_values);
  void SetIACMATMap(int i_index, const float* i_values);
  void GetIACMATMap(int i_index, float* op_values, bool i_original = false);
  void GetTpszonMap(int i_index, float* op_values, bool i_original = false);
  void SetTpszonMap(int i_index, const float* i_values);
  void GetInjCylMultMap(int i_index, float* op_values, bool i_original = false);
  void SetInjCylMultMap(int i_index, const float* i_values);
  void GetInjCylAddMap(int i_index, float* op_values, bool i_original = false);
  void SetInjCylAddMap(int i_index, const float* i_values);
  void GetAEMAPMap(int i_index, float* op_values, bool i_original = false);
  void SetAEMAPMap(int i_index, const float* i_values);

  void GetAttenuatorMap(float* op_values, bool i_original = false);
  void SetAttenuatorMap(const float* i_values);
  void GetDwellCntrlMap(float* op_values, bool i_original = false);
  void SetDwellCntrlMap(const float* i_values);
  void GetCTSCurveMap(float* op_values, bool i_original = false);
  void SetCTSCurveMap(const float* i_values);
  void GetATSCurveMap(float* op_values, bool i_original = false);
  void SetATSCurveMap(const float* i_values);
  void GetATSAACMap(float* op_values, bool i_original = false);
  void SetATSAACMap(const float* i_values);
  void GetGasdosePosMap(float* op_values, bool i_original = false);
  void SetGasdosePosMap(const float* i_values);
  void GetBarocorrMap(float* op_values, bool i_original = false);
  void SetBarocorrMap(const float* i_values);
  void GetManIgntimMap(float* op_values, bool i_original = false);
  void SetManIgntimMap(const float* i_values);
  void GetTmp2CurveMap(float* op_values, bool i_original = false);
  void SetTmp2CurveMap(const float* i_values);
  void GetGrtsCurveMap(float* op_values, bool i_original = false);
  void SetGrtsCurveMap(const float* i_values);
  void GetCrkTempMap(float* op_values, bool i_original = false);
  void SetCrkTempMap(const float* i_values);
  void GetEHPauseMap(float* op_values, bool i_original = false);
  void SetEHPauseMap(const float* i_values);
  void GetCrankingThrdMap(float* op_values, bool i_original = false);
  void SetCrankingThrdMap(const float* ip_values);
  void GetCrankingTimeMap(float* op_values, bool i_original = false);
  void SetCrankingTimeMap(const float* ip_values);
  void GetSmapabanThrdMap(float* op_values, bool i_original = false);
  void SetSmapabanThrdMap(const float* ip_values);
  void GetKnockZoneMap(float* op_values, bool i_original = false);
  void SetKnockZoneMap(const float* i_values);
  void GetLambdaZoneMap(float* op_values, bool i_original = false);
  void SetLambdaZoneMap(const float* i_values);
  void GetGrHeatDutyMap(float* op_values, bool i_original = false);
  void SetGrHeatDutyMap(const float* i_values);
  void GetPwmIacUCoefMap(float* op_values, bool i_original = false);
  void SetPwmIacUCoefMap(const float* i_values);
  void GetAftstrStrk0Map(float* op_values, bool i_original = false);
  void SetAftstrStrk0Map(const float* i_values);
  void GetAftstrStrk1Map(float* op_values, bool i_original = false);
  void SetAftstrStrk1Map(const float* i_values);
  void GetGrValDelMap(float* op_values, bool i_original = false);
  void SetGrValDelMap(const float* i_values);
  void GetFtlsCurveMap(float* op_values, bool i_original = false);
  void SetFtlsCurveMap(const float* i_values);
  void GetFtlsCorMap(float* op_values, bool i_original = false);
  void SetFtlsCorMap(const float* i_values);
  void GetEgtsCurveMap(float* op_values, bool i_original = false);
  void SetEgtsCurveMap(const float* i_values);
  void GetOpsCurveMap(float* op_values, bool i_original = false);
  void SetOpsCurveMap(const float* i_values);
  void GetManInjPwcMap(float* op_values, bool i_original = false);
  void SetManInjPwcMap(const float* i_values);
  void GetMAFCurveMap(float* op_values, bool i_original = false);
  void SetMAFCurveMap(const float* i_values);
  void GetFtsCurveMap(float* op_values, bool i_original = false);
  void SetFtsCurveMap(const float* i_values);
  void GetFuelDensCorrMap(float* op_values, bool i_original = false);
  void SetFuelDensCorrMap(const float* i_values);
  void GetXtauXfAccMap(float* op_values, bool i_original = false);
  void SetXtauXfAccMap(const float* i_values);
  void GetXtauXfDecMap(float* op_values, bool i_original = false);
  void SetXtauXfDecMap(const float* i_values);
  void GetXtauTfAccMap(float* op_values, bool i_original = false);
  void SetXtauTfAccMap(const float* i_values);
  void GetXtauTfDecMap(float* op_values, bool i_original = false);
  void SetXtauTfDecMap(const float* i_values);
  void GetInjNonLinPMap(float* op_values, bool i_original = false);
  void SetInjNonLinPMap(const float* i_values);
  void GetInjNonLinGMap(float* op_values, bool i_original = false);
  void SetInjNonLinGMap(const float* i_values);
  void GetEGODelayMap(float* op_values, bool i_original = false);
  void SetEGODelayMap(const float* i_values);

  virtual SECU3IO::params_t* GetParamsPtr(void);
  virtual EECUPlatform GetPlatformId(void);

 private:
  CFirmwareDataMediator(const CFirmwareDataMediator& i);
  struct cd_data_t* _FindCodeData(const BYTE* ip_bytes = NULL, const PPFlashParam* ip_fpp = NULL);

  BYTE* getBytes(bool i_original = false);
  const BYTE* getBytes(bool i_original = false) const;

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
