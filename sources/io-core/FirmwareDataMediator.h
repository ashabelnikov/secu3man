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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#pragma once

#include "iocore_api.h"
#include "common/unicodesupport.h"
#include "PlatformParamHolder.h"
#include <vector>

//Этот класс необходим для абстракции над форматом хранения данных в прошивке 
//(памяти программ микроконтроллера), а также для хранения этих данных.

class IOCORE_API CFirmwareDataMediator
{
 public:
  CFirmwareDataMediator(const PPFlashParam& i_fpp);
 ~CFirmwareDataMediator();

  //загрузка байтов прошивки из указанного буфера
  void LoadBytes(const BYTE* i_bytes);
  void LoadDataBytesFromAnotherFirmware(const BYTE* i_source_bytes);
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
  bool CheckCompatibility(const BYTE* i_data) const;

	//-----------------------------------------------------------------------
  _TSTRING GetSignatureInfo(void);
  void SetSignatureInfo(_TSTRING i_string);
  
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

  void GetCoilRegulMap(float* o_values, bool i_original = false);
  void SetCoilRegulMap(const float* i_values);

  unsigned int CalculateCRC16OfActiveFirmware(void);
  unsigned int GetCRC16StoredInActiveFirmware(void);
  void CalculateAndPlaceFirmwareCRC(void);

  //-----------------------------------------------------------------------

  bool SetDefParamValues(BYTE i_descriptor, const void* i_values);   //загнать указанные резервн.парам. в прошивку
  bool GetDefParamValues(BYTE i_descriptor, void* o_values);         //извлечь указанные резервн.парам. из прошивки

  //-----------------------------------------------------------------------

 private:
  PPFlashParam m_fpp;
  const size_t m_firmware_size;
  BYTE* m_bytes_active;   //байты прошивки (копия для модификации)
  BYTE* m_bytes_original; //байты прошивки (копия для сравнения) 
  bool m_is_opened;
  _TSTRING m_fw_file_name;
};
