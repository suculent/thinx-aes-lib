#ifndef AESLib_h
#define AESLib_h

#define __STDC_WANT_LIB_EXT1__ 1

#include "AES.h"
#include "xbase64.h"

#if !(defined(__AVR__) || defined(ARDUINO_ARCH_SAMD) || defined(NRF5))
#include <iomanip> // provides std::setfill and setw:: (only for intToHex debugging)
#include <sstream>
#include <cstdint>
#include <iostream>
#include <string>
#endif

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef __x86_64
#include "Arduino.h"
#define debug(format, ...) if (Serial) Serial.printf ( format, __VA_ARGS__)

#define dumpHex(arr, count) if (Serial) { for(int kkk =0;kkk< count;kkk++) \
                                      Serial.printf ("%x " ,arr[kkk]); \
                                      Serial.printf ("\n"); \
                          }

#define dump(arr, count) if (Serial) { for(int kkk =0;kkk< count;kkk++) \
                                      Serial.printf ("%s," ,arr[kkk]); \
                                      Serial.printf ("\n"); \
                          }
//#define AES_DEBUG
#endif

class AESLib
{
  public:

    void gen_iv(byte *iv);

    int get_cipher_length(int msg_len);
    int get_cipher64_length(int msg_len);

    void set_paddingmode(paddingMode mode);
    paddingMode get_paddingmode();

    uint16_t encrypt64(const byte *input, uint16_t input_length, char *output, const byte key[],int bits, byte my_iv[]); // base64 encode, encrypt and base64 encode again; will deprecate
    uint16_t encrypt(const byte input[], uint16_t input_length, byte *output, const byte key[],int bits, byte my_iv[]); // base64 encode and encrypt; should encode on output only (if)

    uint16_t decrypt64(char *input, uint16_t input_length, byte *output, const byte key[],int bits, byte my_iv[]); // decode, decrypt and decode
    uint16_t decrypt(byte input[], uint16_t input_length, byte *output, const byte key[], int bits, byte my_iv[]); // decrypts and decodes (expects encoded)

#ifndef __x86_64
    String decrypt(String msg, byte key[],int bits, byte my_iv[]) __attribute__((deprecated)); // decode, decrypt, decode and return as String
    String encrypt(String msg, byte key[], int bits, byte my_iv[]) __attribute__((deprecated)); // encode, encrypt, encode and return as String
#endif

    uint8_t getrnd();


#ifndef __AVR__
#ifndef NRF5
    std::string intToHex(uint8_t intValue);
#endif
#endif

  private:
    void clean();
    AES aes;
};

#endif // AESLib_h
