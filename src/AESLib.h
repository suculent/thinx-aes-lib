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

    int get_cipher_length(int msg_len);
    int get_cipher64_length(int msg_len);

    void set_paddingmode(paddingMode mode);
    paddingMode get_paddingmode();

    uint16_t encrypt64(char * input, char * output, byte key[],int bits, byte my_iv[]); // base64 encode, encrypt and base64 encode again; will deprecate
    uint16_t encrypt(byte input[], uint16_t input_length, char * output, byte key[],int bits, byte my_iv[]); // base64 encode and encrypt; should encode on output only (if)

    uint16_t decrypt64(char * input, char * output, byte key[],int bits, byte my_iv[]); // decode, decrypt and decode
    uint16_t decrypt(byte input[], uint16_t input_length, char * output, byte key[], int bits, byte my_iv[]); // decrypts and decodes (expects encoded)

    String decrypt(String msg, byte key[],int bits, byte my_iv[]) __attribute__((deprecated)); // decode, decrypt, decode and return as String
    String encrypt(String msg, byte key[], int bits, byte my_iv[]) __attribute__((deprecated)); // encode, encrypt, encode and return as String

  private:
    uint8_t getrnd();
    void clean();
    AES aes;
};

#endif // AESLib_h
