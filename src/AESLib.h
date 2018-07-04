#ifndef AESLib_h
#define AESLib_h

#include "Arduino.h"
#include "AES.h"
#include "base64.h"

class AESLib
{
  public:
    void gen_iv(byte  *iv);
    String encrypt(String msg, byte key[], byte my_iv[]);
    void encrypt(char * input, char * output, byte key[], byte my_iv[]);
    String decrypt(String msg, byte key[], byte my_iv[]);
    void decrypt(char * input, char * output, byte key[], byte my_iv[]);

  private:
    uint8_t getrnd();
    void clean();
    AES aes;
};

#endif
