#include "AESLib.h"

//#define AES_DEBUG

#ifndef __AVR__
#ifndef NRF5
#ifndef ARDUINO_ARCH_SAMD
std::string AESLib::intToHex(uint8_t intValue) {
    std::string hexStr;
    std::stringstream sstream;
    sstream << std::setfill ('0') << std::setw(2)
    << std::hex << (int)intValue;
    hexStr = sstream.str();
    sstream.clear();
    return hexStr;
}
#endif
#endif
#endif
//AESLib::AESLib(void){};

uint8_t AESLib::getrnd()
{
   return aes.getrandom();
}

void AESLib::gen_iv(uint8_t * iv) {
    #ifdef ARDUINO_ARCH_ESP32
        esp_fill_random(iv, N_BLOCK);
    #else
        for (int i = 0 ; i < N_BLOCK ; i++ ) {
            iv[i]= getrnd();
        }
    #endif
}

void AESLib::set_paddingmode(paddingMode mode){
  aes.setPadMode(mode);
}

paddingMode AESLib::get_paddingmode(){
  return aes.getPadMode();
}


int AESLib::get_cipher_length(int msglen){
  return aes.get_padded_len(msglen);
}


int AESLib::get_cipher64_length(int msglen){
  return base64_enc_len(aes.get_padded_len(msglen));
}

void AESLib::clean() {
  aes.clean();
}

//
// Basic de/encryption for char* and byte[] of known length
//

/* Returns message encrypted only to be used as byte array. TODO: Refactor to byte[] */
uint16_t AESLib::encrypt(const byte input[], uint16_t input_length, byte *output, const byte key[], int bits, byte my_iv[]) {

  aes.set_key(key, bits);
  aes.do_aes_encrypt((byte *)input, input_length, (byte*)output, key, bits, my_iv);

  uint16_t enc_len = aes.get_size();

  return enc_len;
}


/* Returns byte array decoded and decrypted. TODO: Refactor to byte[] */
uint16_t AESLib::decrypt(byte input[], uint16_t input_length, byte *plain, const byte key[], int bits, byte my_iv[]) {

  int dec_len = aes.do_aes_decrypt((byte *)input, input_length, (byte *)plain, key, bits, (byte *)my_iv);

#ifndef __AVR__
#ifndef NRF5
#ifndef ARDUINO_ARCH_SAMD
#ifndef __x86_64
#ifdef AES_DEBUG
  Serial.printf("[AESLib::decrypt] Decrypted bytes = ");
  for (uint8_t pos = 0; pos < dec_len; pos++) {
    Serial.printf("%s ", intToHex(plain[pos]).c_str());
  }
  Serial.printf("\n");
#endif
#endif
#endif
#endif
#endif

  return dec_len;
}

//
// Deprecated de/encryption for Arduino Strings
//

#ifndef __x86_64

/* Returns message encrypted and base64 encoded to be used as string. */
uint16_t AESLib::encrypt64(const byte *msg, uint16_t msgLen, char *output, const byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int paddedLen = aes.get_padded_len(msgLen);

  // Serial.print("- Expected padded length "); Serial.print(paddedLen); Serial.println(" bytes");

  byte cipher[paddedLen];
  aes.do_aes_encrypt((byte *)msg, msgLen, cipher, key, bits, my_iv);

  uint16_t encrypted_length = aes.get_size();

  // only this method can return b64
  encrypted_length = base64_encode(output, (char *)cipher, aes.get_size() );

  return encrypted_length;
}

/* Suggested size for the plaintext buffer is 1/2 length of `msg`. Refactor! */
uint16_t AESLib::decrypt64(char *msg, uint16_t msgLen, byte *plain, const byte key[],int bits, byte my_iv[]) {

#ifdef AES_DEBUG
  Serial.println("[decrypt64] decrypting message:  ");
#endif

  aes.set_key(key, bits);

#ifdef AES_DEBUG
  Serial.print("[decrypt64] msgLen (strlen msg):  "); Serial.println(msgLen);
#endif

  // decoded base64 takes some space, but less than original...
  // it should therefore never overfill when reusing the msg-text buffer
  char *msgOut = (char*)malloc(msgLen);
  int b64len = base64_decode(msgOut, msg, msgLen);

  // decrypting will keep the message length
#ifdef AES_DEBUG
  Serial.print("[decrypt64] base64_decode allocating decrypt buffer len:  "); Serial.println(b64len);
#endif

#ifdef AES_DEBUG
#ifdef ESP8266
  Serial.print("[decrypt64] free heap: "); Serial.println(ESP.getFreeHeap());
#endif
#endif

  int plain_len = aes.do_aes_decrypt((byte *)msgOut, b64len, (byte*)plain, key, bits, (byte *)my_iv);

  free(msgOut);
  
  // ToWI: 2021-01-22: Check the padding length, negative value means deciphering error and cause ESP restarts due to stack smashing error
  if (plain_len < 0)
      return 0;
    
#ifdef AES_DEBUG
  Serial.print("[decrypt64] do_aes_decrypt->plain_len =  "); Serial.println(plain_len);
#endif

  return plain_len;
}
#endif
