#include "AESLib.h"

uint8_t AESLib::getrnd()
{
   // if analog input pin 0 is unconnected, random analog
   // noise will cause the call to randomSeed() to generate
   // different seed numbers each time the sketch runs.
   // randomSeed() will then shuffle the random function.
   randomSeed(analogRead(0));

   return random(256); // return a random number from 0 to 255
}

void AESLib::gen_iv(byte  *iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= (byte)getrnd();
    }
}

/* Returns Arduino String decoded and decrypted. */
String AESLib::decrypt(String msg, byte key[], byte my_iv[]) {

  aes.set_key(key, sizeof(key));

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
void AESLib::decrypt64(char * msg, char * plain, byte key[], byte my_iv[]) {

  aes.set_key(key, sizeof(key));

  int msgLen = strlen(msg);
  char encrypted[msgLen]; // will be always shorter than Base64
  int b64len = base64_decode(encrypted, msg, msgLen);

  byte out[2*msgLen];
  aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, 128, (byte *)my_iv);

  int outDataLen = strlen((char*)out);
  int outLen = base64_dec_len((char*)out, outDataLen);
  char message[outLen+2]; // trailing zero for cstring?

  int baseLen = base64_decode(message, (char *)out, outDataLen);
  message[baseLen] = '\0'; // ensure trailing zero after cstring

  strcpy(plain, message);
}

/* Returns byte array decoded and decrypted. */
void AESLib::decrypt(char * msg, char * plain, byte key[], byte my_iv[]) {

  aes.set_key(key, sizeof(key));

  int msgLen = strlen(msg);
  char encrypted[msgLen]; // will be always shorter than Base64
  int b64len = base64_decode(encrypted, msg, msgLen);

  byte out[2*msgLen];
  aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, 128, (byte *)my_iv);

  int outDataLen = strlen((char*)out);
  int outLen = base64_dec_len((char*)out, outDataLen);
  char message[outLen+2]; // trailing zero for cstring?

  strcpy(plain, message);
}

/* Returns Arduino string encrypted and encoded with Base64. */
String AESLib::encrypt(String msg, byte key[], byte my_iv[]) {

  aes.set_key(key, sizeof(key));

  int msgLen = strlen(msg.c_str());

  char b64data[base64_enc_len(msgLen)];
  int b64len = base64_encode(b64data, (char*)msg.c_str(), msgLen);

  int paddedLen = b64len + (N_BLOCK - (b64len % N_BLOCK)) + 1;
  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);

  char out[b64len];
  byte cipher[2*b64len];
  aes.do_aes_encrypt((byte *)padded, paddedLen, cipher, key, 128, my_iv);

  base64_encode(out, (char *)cipher, aes.get_size() );

  return String((char*)out);
}

/* Returns message encrypted and base64 encoded to be used as string. */
void AESLib::encrypt64(char * msg, char * output, byte key[], byte my_iv[]) {

#ifdef AES_DEBUG
  Serial.print("incoming msg: "); Serial.println(msg);
  Serial.print("incoming k-size: "); Serial.println(sizeof(key));
  Serial.print("incoming v-size: "); Serial.println(sizeof(my_iv));
#endif

  aes.set_key(key, sizeof(key));

  int msgLen = strlen(msg);
#ifdef AES_DEBUG
  Serial.println("- msgLen");
#endif

  char b64data[base64_enc_len(msgLen)];
#ifdef AES_DEBUG
  Serial.println("- b64data");
#endif

  int b64len = base64_encode(b64data, (char*)msg, msgLen);
#ifdef AES_DEBUG
  Serial.println("- b64len");
#endif

  int paddedLen = b64len + (N_BLOCK - (b64len % N_BLOCK)) + 1;
#ifdef AES_DEBUG
  Serial.println("- paddedLen");
#endif

  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);
#ifdef AES_DEBUG
  Serial.println("- padPlaintext");
#endif

  byte cipher[2*b64len];
  aes.do_aes_encrypt((byte *)padded, paddedLen, cipher, key, 128, my_iv);
#ifdef AES_DEBUG
  Serial.println("- do_aes_encrypt");
#endif

  char out2[4*b64len];
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
void AESLib::encrypt(char * msg, char * output, byte key[], byte my_iv[]) {

  aes.set_key(key, sizeof(key));

  int msgLen = strlen(msg);

  char b64data[base64_enc_len(msgLen)];
  int b64len = base64_encode(b64data, (char*)msg, msgLen);

  int paddedLen = b64len + (N_BLOCK - (b64len % N_BLOCK)) + 1;
  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);

  byte cipher[2*b64len];
  aes.do_aes_encrypt((byte *)padded, paddedLen, cipher, key, 128, my_iv);

  strcpy(output, (char*)cipher);
}

void AESLib::clean() {
  aes.clean();
}
