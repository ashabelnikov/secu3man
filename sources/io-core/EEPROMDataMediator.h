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

/** \file EEPROMDataMediator.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "iocore_api.h"
#include "common/unicodesupport.h"
#include "PlatformParamHolder.h"
#include "ParamsIO.h"
#include <set>

//Аппаратно-независимая прослойка над данными EEPROM.
class IOCORE_API EEPROMDataMediator : public ParamsIO
{
 public:
  EEPROMDataMediator(const PPEepromParam& i_epp);
 ~EEPROMDataMediator();

  //загрузка байтов EEPROM из указанного буфера
  void LoadBytes(const BYTE* i_bytes);

  //сохранение байтов EEPROM в указанный буфер
  void StoreBytes(BYTE* op_bytes);

  //была ли прошивка изменена после последней загрузки
  bool IsModified(void);

  //возвращает true если прошивка загружена (есть что редактировать)
  bool IsLoaded(void);

  //вызывать эту функцию для сброса признака модификации, только
  //после того как данные были успешно сохранены.
  void ResetModified(void);

  //Calculates CRC16 of parameters for given buffer with EEPROM
  //ip_eeprom_buffer - contains EEPROM
  //return true - OK, false - check sum is invalid
  bool VerifyParamsCheckSum(const BYTE* ip_eeprom_buffer);

  //Same for set of maps
  bool VerifyTablesCheckSum(const BYTE* ip_eeprom_buffer);

  //returns address of parameters in EEPROM
  size_t GetParamsStartAddr(void) const;   //note: redundant to GetParamsPtr()

  void CalculateAndPlaceParamsCRC(BYTE* iop_data);
  void CalculateAndPlaceParamsCRC(void);

  void CalculateAndPlaceTablesCRC(BYTE* iop_data);
  void CalculateAndPlaceTablesCRC(void);

  void GetSetMap(int i_index, int id, float* op_values, bool i_original = false);
  void SetSetMap(int i_index, int id, const float* ip_values);

  float GetOdometer(void); //TODO: use double for better accuracy
  void SetOdometer(float odv); //TODO: use double for better accuracy

  float GetConsFuel(void); //TODO: use double for better accuracy
  void SetConsFuel(float cf); //TODO: use double for better accuracy

  std::vector<_TSTRING> GetFunctionsSetNames(void);
  void SetFunctionsSetName(int i_index, _TSTRING i_new_name);

  void SetEEFileName(const _TSTRING i_ee_file_name);
  _TSTRING GetEEFileName(void);

  std::set<int> GetCEErrorsList(void);
  void ResetCEErrorsList(void);

  bool GetSplitAngMode(int i_index);

protected:
  void GetStartMap(int i_index, float* o_values, bool i_original = false);
  void GetIdleMap(int i_index,  float* o_values, bool i_original = false);
  void GetWorkMap(int i_index, float* o_values, bool i_original = false);
  void GetTempMap(int i_index, float* o_values, bool i_original = false);
  void GetTempIdlMap(int i_index, float* o_values, bool i_original = false);

  void GetVEMap(int i_index, float* o_values, bool i_original = false);
  void GetVE2Map(int i_index, float* o_values, bool i_original = false);
  void GetIVEMap(int i_index, float* o_values, bool i_original = false);
  void GetAFRMap(int i_index, float* o_values, bool i_original = false);
  void GetCrnkMap(int i_index, float* o_values, bool i_original = false);
  void GetWrmpMap(int i_index, float* o_values, bool i_original = false);
  void GetDeadMap(int i_index, float* o_values, bool i_original = false);
  void GetIdlrMap(int i_index, float* o_values, bool i_original = false);
  void GetIdlcMap(int i_index, float* o_values, bool i_original = false);
  void GetThrassMap(int i_index, float* o_values, bool i_original = false);
  void GetAETPSMap(int i_index, float* o_values, bool i_original = false);
  void GetAERPMMap(int i_index, float* o_values, bool i_original = false);
  void GetAftstrMap(int i_index, float* o_values, bool i_original = false);
  void GetITMap(int i_index, float* op_values, bool i_original = false);
  void GetITRPMMap(int i_index, float* op_values, bool i_original = false);
  void GetRigidMap(int i_index, float* op_values, bool i_original = false);
  void GetEGOCurveMap(int i_index, float* op_values, bool i_original = false);
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
  void GetIACMATMap(int i_index, float* op_values, bool i_original = false);
  void SetIACMATMap(int i_index, const float* i_values);
  void GetTpszonMap(int i_index, float* op_values, bool i_original = false);
  void SetTpszonMap(int i_index, const float* i_values);
  void GetInjCylMultMap(int i_index, float* op_values, bool i_original = false);
  void SetInjCylMultMap(int i_index, const float* i_values);
  void GetInjCylAddMap(int i_index, float* op_values, bool i_original = false);
  void SetInjCylAddMap(int i_index, const float* i_values);
  void GetAEMAPMap(int i_index, float* o_values, bool i_original = false);
  void SetAEMAPMap(int i_index, const float* i_values);

  void SetStartMap(int i_index,const float* i_values);
  void SetIdleMap(int i_index, const float* i_values);
  void SetWorkMap(int i_index, const float* i_values);
  void SetTempMap(int i_index, const float* i_values);
  void SetTempIdlMap(int i_index, const float* i_values);
  //fuel injection
  void SetVEMap(int i_index, const float* i_values);
  void SetVE2Map(int i_index, const float* i_values);
  void SetIVEMap(int i_index, const float* i_values);
  void SetAFRMap(int i_index, const float* i_values);
  void SetCrnkMap(int i_index, const float* i_values);
  void SetWrmpMap(int i_index, const float* i_values);
  void SetDeadMap(int i_index, const float* i_values);
  void SetIdlrMap(int i_index, const float* i_values);
  void SetIdlcMap(int i_index, const float* i_values);
  void SetThrassMap(int i_index, const float* i_values);
  void SetAETPSMap(int i_index, const float* i_values);
  void SetAERPMMap(int i_index, const float* i_values);
  void SetAftstrMap(int i_index, const float* i_values);
  void SetITMap(int i_index, const float* i_values);
  void SetITRPMMap(int i_index, const float* i_values);
  void SetRigidMap(int i_index, const float* i_values);
  void SetEGOCurveMap(int i_index, const float* i_values);
  void GetPwm1Map(int i_index, float* o_values, bool i_original = false);
  void SetPwm1Map(int i_index, const float* i_values);
  void GetPwm2Map(int i_index, float* o_values, bool i_original = false);
  void SetPwm2Map(int i_index, const float* i_values);

  virtual SECU3IO::params_t* GetParamsPtr(void);
  virtual EECUPlatform GetPlatformId(void);

 private:
  BYTE* getBytes(bool i_original = false);

  PPEepromParam m_epp;
  const size_t m_eeprom_size;
  BYTE* m_bytes_active;   //байты EEPROM (копия для модификации)
  BYTE* m_bytes_original; //байты EEPROM (копия для сравнения)
  bool m_is_opened;
  _TSTRING m_ee_file_name;
};
