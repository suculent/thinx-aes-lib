#include "AESLib.h"

#ifndef __AVR__
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

uint8_t AESLib::getrnd()
{
   return aes.getrandom();
}

void AESLib::gen_iv(uint8_t * iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= getrnd();
    }
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
  return base64_enc_len(aes.get_padded_len(base64_enc_len(msglen)));
}

void AESLib::clean() {
  aes.clean();
}

//
// Basic de/encryption for char* and byte[] of known length
//

/* Returns message encrypted only to be used as byte array. TODO: Refactor to byte[] */
uint16_t AESLib::encrypt(const byte input[], uint16_t input_length, char * output, const byte key[], int bits, byte my_iv[]) {

  aes.set_key(key, bits);
  aes.do_aes_encrypt((byte *)input, input_length, (byte*)output, key, bits, my_iv);

  uint16_t enc_len = aes.get_size();
  uint16_t base64_len = base64_enc_len(input_length);

  char b64data[base64_len];
  // Note: arg order is base64_encode(output, input);
  base64_len = base64_encode(b64data, (char*)output, enc_len);
  memcpy(output, b64data, base64_len);

  /*
#ifdef AES_DEBUG
  printf("[AESLib::encrypt] Encoded %u bytes = ", base64_len);
  for (uint8_t pos = 0; pos < base64_len; pos++) {
    if (pos < base64_len) {
      printf("%c", output[pos]);
    }
  }
#endif
  */

#ifndef __x86_64
#ifdef AES_DEBUG
  Serial.printf("[AESLib::encrypt] Encoded %u bytes = ", base64_len);
  for (uint8_t pos = 0; pos < base64_len; pos++) {
    if (pos < base64_len) {
      Serial.printf("%c", output[pos]);
    }
  }
  Serial.println("");
#endif
#endif

  return base64_len;
}


/* Returns byte array decoded and decrypted. TODO: Refactor to byte[] */
uint16_t AESLib::decrypt(byte input[], uint16_t input_length, char * plain, const byte key[], int bits, byte my_iv[]) {

  int dec_len = aes.do_aes_decrypt((byte *)input, input_length, (byte *)plain, key, bits, (byte *)my_iv);

#ifndef __x86_64
#ifndef __AVR__
#ifdef AES_DEBUG
  Serial.printf("[AESLib::decrypt] Decrypted bytes = ");
  for (uint8_t pos = 0; pos < dec_len; pos++) {
    Serial.printf("%s ", intToHex(plain[pos]).c_str());
  }
  Serial.printf("\n");
#endif
#endif
#endif

  return dec_len;
}

//
// Deprecated de/encryption for Arduino Strings
//

#ifndef __x86_64

//
// Encryption with added base64 layer on input, seems useless with known lengths.
// Will probably deprecate soon as well to keep the wrapper as slim as possible.
//

/* Returns message encrypted and base64 encoded to be used as string. */
uint16_t AESLib::encrypt64(const char * msg, uint16_t msgLen, char * output, const byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  char b64data[base64_enc_len(msgLen)+1];  // should add 1 character to accomodate the 0x\0 ending character

  // thanks to this, method can consume byte[] and not just char* (!)
  int b64len = base64_encode(b64data, msg, msgLen);
  int paddedLen = aes.get_padded_len(b64len);

  byte cipher[paddedLen];
  aes.do_aes_encrypt((byte *)b64data, b64len, cipher, key, bits, my_iv);

  // only this method can return b64
  uint16_t encrypted_length = aes.get_size();
  base64_encode(output, (char *)cipher, aes.get_size() );

  return encrypted_length;
}

/* Suggested size for the plaintext buffer is 1/2 length of `msg`. Refactor! */
uint16_t AESLib::decrypt64(char * msg, uint16_t msgLen, char * plain, const byte key[],int bits, byte my_iv[]) {

#ifdef AES_DEBUG
  Serial.println("[decrypt64] decrypting message:  ");
#endif

  aes.set_key(key, bits);

#ifdef AES_DEBUG
  Serial.print("[decrypt64] msgLen (strlen msg):  "); Serial.println(msgLen);
#endif

  // decoded base64 takes some space, but less than original...
  // it should therefore never overfill when reusing the msg-text buffer
  int b64len = base64_decode(msg, msg, msgLen);
  // decrypting will keep the message length
#ifdef AES_DEBUG
  Serial.print("[decrypt64] base64_decode allocating decrypt buffer len:  "); Serial.println(b64len);
#endif

  byte out[b64len]; // unfortunately this needs to fit to stack... that's hard limit for chunk

#ifdef AES_DEBUG
  // Serial.print("[decrypt64] Clearing-out buffer to allow safe strlen (zero-in-the-middle will still fail)...");
#endif

  if (b64len > 0) {
    // void * memset ( void * ptr, int value, size_t num );
    memset( out, 0x00, b64len );
  }

#ifdef AES_DEBUG
#ifdef ESP8266
  Serial.print("[decrypt64] free heap: "); Serial.println(ESP.getFreeHeap());
#endif
#endif

  int b64_len = aes.do_aes_decrypt((byte *)msg, b64len, (byte*)out, key, bits, (byte *)my_iv);
  // ToWI: 2021-01-22: Check the padding length, negative value means deciphering error and cause ESP restarts due to stack smashing error
  if (b64_len < 0)
      return 0;
    
  out[b64_len+1] = 0;

#ifdef AES_DEBUG
  Serial.print("[decrypt64] aes_decrypt length before b64-decode:  "); Serial.println(b64_len);
#endif

  // calculate required output length
  uint16_t outLen = base64_dec_len((char*)out, b64_len);

#ifdef AES_DEBUG
  Serial.print("[decrypt64] expected base64_dec_len after b64-decode:  "); Serial.println(outLen);
#endif

  // decode buffer to output plain-text, output buffer will never overfill...
  outLen = base64_decode(plain, (char *)out, b64_len);
  // plain[outLen+1] = 0; // trailing zero for safety?

#ifdef AES_DEBUG
  Serial.print("[decrypt64] base64_decode->outLen =  "); Serial.println(outLen);
#endif

  return outLen;
}
#endif
