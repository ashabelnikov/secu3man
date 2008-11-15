
#ifdef HEXUTILS_EXPORTS
#define HEXUTILS_API __declspec(dllexport)
#else
#define HEXUTILS_API __declspec(dllimport)
#endif


enum EReadHexStatus
{
 RH_SUCCESS = 0,
 RH_INCORRECT_CHKSUM = 1,
 RH_UNEXPECTED_SYMBOL = 2,
};

// Intel HEX 80 не содержит адресов больше 0xFFFF поэтому подразумевается
// что выходной буфер имеет размер не менее 0xFFFF
//
// Переводит данные из формата Intel HEX 80 в бинарный

//ip_buff - Входной буфер содержащий смволы HEX-файла
//i_size - размер входного буфера в символах
//op_buff - Выходной буфер в который будут помещены преобразованные бинарные данные
//o_size - размер выходного буфера
//Возвращает результат определяющий на сколько успешно бало произведено преобразование.
HEXUTILS_API EReadHexStatus HexUtils_ConvertHexToBin(BYTE* ip_buff, size_t i_size, BYTE* op_buff, size_t& o_size);
