#ifdef CRC_EXPORTS
#define CRC_API __declspec(dllexport)
#else
#define CRC_API __declspec(dllimport)
#endif

unsigned short CRC_API crc16(unsigned char *buf, unsigned short num);
