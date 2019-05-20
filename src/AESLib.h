#ifndef AESLib_h
#define AESLib_h

#include "Arduino.h"
#include "AES.h"
#include "base64.h"

#define AES_DEBUG
#define debug(format, ...) if (Serial) Serial.printf ( format, __VA_ARGS__)
#define dumpHex(arr, count) if (Serial) { for(int kkk =0;kkk< count;kkk++) \
                                      Serial.printf ("%x " ,arr[kkk]); \
                                      Serial.printf ("\n"); \
                          }
#define dump(arr, count) if (Serial) { for(int kkk =0;kkk< count;kkk++) \
                                      Serial.printf ("%s," ,arr[kkk]); \
                                      Serial.printf ("\n"); \
                          }
class AESLib
{
  public:
    void gen_iv(byte  *iv);

    String encrypt(String msg, byte key[],int bits, byte my_iv[]);                  // encode, encrypt, encode and return as String
    void encrypt64(char * input, char * output, byte key[],int bits, byte my_iv[]); // encode, encrypt and encode
    void encrypt(char * input, char * output, byte key[],int bits, byte my_iv[]);   // encode and encrypt

    String decrypt(String msg, byte key[],int bits, byte my_iv[]);                  // decode, decrypt, decode and return as String
    void decrypt64(char * input, char * output, byte key[],int bits, byte my_iv[]); // decode, decrypt and decode
    void decrypt(char * input, char * output, byte key[],int bits, byte my_iv[]);   // decode and decrypt

    int get_cipher_length(int msg_len);
    int get_cipher64_length(int msg_len);
    void set_paddingmode(paddingMode mode);
    paddingMode get_paddingmode();

  private:
    uint8_t getrnd();
    void clean();
    AES aes;
};

#endif
