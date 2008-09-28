

#pragma once

#include "BootLoader.h"
#include "common/unicodesupport.h"
#include <vector>

//Этот класс необходим для абстракции над форматом хранения данных в прошивке 
//(памяти программ микроконтроллера), а так же для хранения этих данных.

class AFX_EXT_CLASS CFirmwareDataMediator
{
  public:
     CFirmwareDataMediator();
	~CFirmwareDataMediator();

	//загрузка байтов прошивки из указанного буфера
	void LoadBytes(const BYTE* i_bytes);
	void LoadDataBytesFromAnotherFirmware(const BYTE* i_source_bytes);
	
	//сохранение байтов прошивки в указанный буфер
	void StoreBytes(BYTE* o_bytes);

	//была ли прошивка изменена после последней загрузки 
	bool IsModified(void);

	//возвращает true если прошивка загружена (есть что редактировать)
	bool IsLoaded(void) { return m_is_opened;}

	//вызывать эту функцию для сброса признака модификации, только
	//после того как данные были успешно сохранены.
	void ResetModified(void);

    static void CalculateAndPlaceFirmwareCRC(BYTE* io_data);

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

    unsigned int CalculateCRC16OfActiveFirmware(void);
	unsigned int GetCRC16StoredInActiveFirmware(void);
	void CalculateAndPlaceFirmwareCRC(void);

	//-----------------------------------------------------------------------

    bool SetDefParamValues(BYTE i_descriptor, const void* i_values);   //загнать указанные резервн.парам. в прошивку
    bool GetDefParamValues(BYTE i_descriptor, void* o_values);         //извлечь указанные резервн.парам. из прошивки

	//-----------------------------------------------------------------------

  private:
    const size_t m_firmware_size;
	BYTE m_bytes_active[CBootLoader::FLASH_TOTAL_SIZE + 1]; //байты прошивки (копия для модификации)
	BYTE m_bytes_original[CBootLoader::FLASH_TOTAL_SIZE + 1]; //байты прошивки (копия для сравнения) 
	bool m_is_opened;
	_TSTRING m_fw_file_name;
};