
#pragma once

#include "common\unicodesupport.h"
#include "SECU3IO.h"

//Все записи в файле должны иметь одинаковую длину!
class AFX_EXT_CLASS LogReader
{
public:
	LogReader();
   ~LogReader();

    enum FileError
    {
    FE_OPEN,
    FE_FORMAT,
	FE_NA
    };

    //открывает файл
    bool OpenFile(const _TSTRING& i_file_name, FileError& o_error);

	//закрывает файл
    bool CloseFile(void);

	//возвращает true если в данный момент открыт файл
	bool IsOpened(void) const;

	//получает данные из текущей записи
	bool GetRecord(SYSTEMTIME& o_time, SECU3IO::SensorDat *op_data);

	//Перемещение к следующей записи
	//возвращает false если достигнут конец записей
	bool Next(void);

	//Перемещение к предыдущей записи
	//возвращает false если достигнуто начало записей
	bool Prev(void);

	//возвращает кол-во записей в файле
	unsigned long GetCount(void) const;

	//Separating symbol for CSV 
	void SetSeparatingSymbol(char i_sep_symbol);
private:
   
	_TSTRING m_file_name;         //имя открытого файла
	unsigned long m_record_count; //кол-во записей в файле
	FILE* m_file_handle;          //указатель на файл

	//номер текущей записи
	unsigned long m_current_record;
	//кол-во символов в одной записи, включая символы перевода строки 
	unsigned long m_record_size;

	char m_csv_separating_symbol;
	char m_csv_data_template[1024];
};