
#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>

#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

//SHA256 calculation algorithm
class Sha256
{
 public:
  typedef unsigned char BYTE;             // 8-bit byte
  typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

  Sha256();

  void init(void);
  void update(const BYTE data[], size_t len);
  void final(BYTE hash[]);

 private:
  void sha256_transform(const BYTE data[]);

 private: //context data
  BYTE m_data[64];
  WORD m_datalen;
  unsigned long long m_bitlen;
  WORD m_state[8];
};

#endif   // SHA256_H
