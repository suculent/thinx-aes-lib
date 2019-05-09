#include "AESLib.h"

uint8_t AESLib::getrnd()
{
   uint8_t really_random = *(volatile uint8_t *)0x3FF20E44;
   return really_random;
}

void AESLib::gen_iv(byte  *iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= (byte) 0xFF; // getrnd();
    }
}

/* Returns Arduino String decoded and decrypted. */
String AESLib::decrypt(String msg, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int len = msg.length();
  char encrypted[len]; // will be always shorter than Base64
  int b64len = base64_decode(encrypted, (char*)msg.c_str(), msg.length());

  byte out[2*len];
  aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, 128, (byte *)my_iv);

  int outDataLen = strlen((char*)out);
  int outLen = base64_dec_len((char*)out, outDataLen);
  char message[outLen+2]; // trailing zero for cstring

  int baseLen = base64_decode(message, (char *)out, outDataLen);
  message[baseLen] = '\0'; // ensure trailing zero after cstring

  return String(message);
}

/* Suggested size for the plaintext buffer is 1/2 length of `msg` */
void AESLib::decrypt64(char * msg, char * plain, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int msgLen = strlen(msg);
  char encrypted[msgLen]; // will be always shorter than Base64
  int b64len = base64_decode(encrypted, msg, msgLen);

  byte out[2*msgLen];
  aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, bits, (byte *)my_iv);

  int outDataLen = strlen((char*)out);
  int outLen = base64_dec_len((char*)out, outDataLen);
  char message[outLen+2]; // trailing zero for cstring?

  int baseLen = base64_decode(message, (char *)out, outDataLen);
  message[baseLen] = '\0'; // ensure trailing zero after cstring

  strcpy(plain, message);
}

/* Returns byte array decoded and decrypted. */
void AESLib::decrypt(char * msg, char * plain, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int msgLen = strlen(msg);
  char encrypted[msgLen]; // will be always shorter than Base64
  int b64len = base64_decode(encrypted, msg, msgLen);

  byte out[2*msgLen];
  aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, bits, (byte *)my_iv);

  int outDataLen = strlen((char*)out);
  int outLen = base64_dec_len((char*)out, outDataLen);
  char message[outLen+2]; // trailing zero for cstring?

  strcpy(plain, message);
}

/* Returns Arduino string encrypted and encoded with Base64. */
String AESLib::encrypt(String msg, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int msgLen = strlen(msg.c_str());

  char b64data[base64_enc_len(msgLen)];
  int b64len = base64_encode(b64data, (char*)msg.c_str(), msgLen);

  int paddedLen = b64len + (N_BLOCK - (b64len % N_BLOCK)) + 1;
  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);

  char out[b64len];
  byte cipher[2*b64len];
  aes.do_aes_encrypt((byte *)padded, paddedLen, cipher, key, bits, my_iv);

  base64_encode(out, (char *)cipher, aes.get_size() );

  return String((char*)out);
}

/* Returns message encrypted and base64 encoded to be used as string. */
void AESLib::encrypt64(char * msg, char * output, byte key[],int bits, byte my_iv[]) {

#ifdef AES_DEBUG
  Serial.print("incoming msg: "); Serial.println(msg);
  Serial.print("incoming k-size: "); Serial.println(bits);
  Serial.print("incoming v-size: "); Serial.println(16); // initialization vector has the same size as the block size = 16
#endif

  aes.set_key(key, bits);

  int msgLen = strlen(msg);
#ifdef AES_DEBUG
  Serial.print("- msg >>"); Serial.print(msg);Serial.println("<<");
  Serial.print("- msgLen "); Serial.println(msgLen);
#endif

  char b64data[base64_enc_len(msgLen)+1];  // should add 1 character to accomodate the 0x\0 ending character
#ifdef AES_DEBUG
  Serial.print("- b64data [");Serial.print(base64_enc_len(msgLen));Serial.println("]");
#endif

  int b64len = base64_encode(b64data, msg, msgLen);
#ifdef AES_DEBUG
  Serial.print("- b64len ");Serial.println(b64len);
  Serial.print("- b64data ");Serial.println(b64data);
  Serial.print("- b64data ");dumpHex(b64data,b64len)
#endif

  int paddedLen = aes.get_padded_len(b64len);
#ifdef AES_DEBUG
  Serial.print("- paddedLen ");Serial.println(paddedLen);
#endif

  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);
#ifdef AES_DEBUG
  Serial.print("- padPlaintext "); dumpHex(padded,paddedLen)
#endif

  byte cipher[paddedLen];
  aes.do_aes_encrypt((byte *)padded, paddedLen, cipher, key, bits, my_iv);
#ifdef AES_DEBUG
  Serial.print("- do_aes_encrypt "); dumpHex(cipher,paddedLen)
#endif

  char out2[base64_enc_len(paddedLen)+1];
  base64_encode(out2, (char *)cipher, aes.get_size() );
#ifdef AES_DEBUG
  Serial.println("- base64_encode");
#endif

  strcpy(output, (char*)out2);
#ifdef AES_DEBUG
  Serial.println("- strcpy");
#endif
}

/* Returns message encrypted only to be used as byte array. */
void AESLib::encrypt(char * msg, char * output, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int msgLen = strlen(msg);

  char b64data[base64_enc_len(msgLen)];
  int b64len = base64_encode(b64data, (char*)msg, msgLen);

  int paddedLen =  aes.get_padded_len(b64len);;
  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);
  // encryption will return the same number of bytes as the padded message
  byte cipher[paddedLen];
  aes.do_aes_encrypt((byte *)padded, paddedLen, cipher, key, bits, my_iv);

  strcpy(output, (char*)cipher);
}

void AESLib::clean() {
  aes.clean();
}
