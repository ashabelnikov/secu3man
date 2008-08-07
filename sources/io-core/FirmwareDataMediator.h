

#pragma once

#include "BootLoader.h"
#include "common/unicodesupport.h"

//Этот класс необходим для абстракции над форматом хранения данных в прошивке 
//(памяти программ микроконтроллера), а так же для хранения этих данных.

class AFX_EXT_CLASS CFirmwareDataMediator
{
  public:
     CFirmwareDataMediator();
	~CFirmwareDataMediator();

	//загрузка байтов прошивки из указанного буфера
	void LoadBytes(BYTE* i_bytes);
	
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

    _TSTRING GetSignatureInfo(void);
	void SetSignatureInfo(_TSTRING i_string);
  

  private:
    const size_t m_firmware_size;
	BYTE m_bytes_active[CBootLoader::FLASH_TOTAL_SIZE + 1]; //байты прошивки (копия для модификации)
	BYTE m_bytes_original[CBootLoader::FLASH_TOTAL_SIZE + 1]; //байты прошивки (копия для сравнения) 
	bool m_is_opened;
};