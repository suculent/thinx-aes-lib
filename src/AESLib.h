#ifndef AESLib_h
#define AESLib_h

#include "Arduino.h"
#include "AES.h"
#include "base64.h"

#define AES_DEBUG

class AESLib
{
  public:
    void gen_iv(byte  *iv);

    String encrypt(String msg, byte key[], byte my_iv[]);                  // encode, encrypt, encode and return as String
    void encrypt64(char * input, char * output, byte key[], byte my_iv[]); // encode, encrypt and encode
    void encrypt(char * input, char * output, byte key[], byte my_iv[]);   // encode and encrypt

    String decrypt(String msg, byte key[], byte my_iv[]);                  // decode, decrypt, decode and return as String
    void decrypt64(char * input, char * output, byte key[], byte my_iv[]); // decode, decrypt and decode
    void decrypt(char * input, char * output, byte key[], byte my_iv[]);   // decode and decrypt

  private:
    uint8_t getrnd();
    void clean();
    AES aes;
};

#endif
