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

String AESLib::decrypt(String msg, byte key[], byte my_iv[]) {

  // Set decryption key
  aes.set_key(key, sizeof(key));

  // Don't be silly here.
  int len = msg.length();
  if (len > 200) {
    return String("Failed to decrypt. Message too long.");
  } else {
    Serial.printf("Decrypting message %s of length %i \n", msg.c_str(), len);
  }

  // Copy message to temporary char array
  char temp[200] = {0};
  sprintf(temp, msg.c_str());
  //msg.toCharArray(temp, len);

  // Decode char array to `decoded array` which is a ciphertext
  char encrypted[256] = {0}; // MAX_BLOCK * 2
  // int base64_decode(char * output, char * input, int inputLen)
  int b64len = base64_decode(encrypted, temp, strlen(temp));

  printf("Decoded %i bytes as %s \n", b64len, encrypted);

  byte out[512] = {0};
  aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, 128, (byte *)my_iv);

  int outDataLen = strlen((char*)out);
  int outLen = base64_dec_len((char*)out, outDataLen);
  char message[outLen+2]; // trailing zero for cstring?
  int baseLen = base64_decode(message, (char *)out, outDataLen);
  message[baseLen] = '\0'; // adds trailing zero for cstring
  printf("Out o:%i/b:%i bytes: %s \n", outDataLen, baseLen, message);

  String result = String(message);
  return result;
}

/*
void decrypt(String b64data, String IV_base64, int size)
{
  char data_decoded[200];
  char iv_decoded[200];
  byte out[200];
  char temp[200];
  b64data.toCharArray(temp, 200);
  base64_decode(data_decoded, temp, b64data.length());
  IV_base64.toCharArray(temp, 200);

  base64_decode(iv_decoded, temp, IV_base64.length());

  // void do_aes_decrypt(byte *cipher,int size_c,byte *plain,byte *key, int bits, byte ivl [N_BLOCK]);
  aes.do_aes_decrypt((byte *)data_decoded, size, out, key, 128, (byte *)iv_decoded);
  char message[msg.length()];
  base64_decode(message, (char *)out, b64data.length());
  printf("Out %s \n", message);
}*/

String AESLib::encrypt(String msg, byte key[], byte my_iv[]) {

  aes.set_key(key, sizeof(key));

  // Calculate required length and pad the plaintext for 16bit AES
  int msgLen = sizeof(msg.c_str());
  int paddedLen = msg.length() + (8 - msg.length() % 8) + 1; // ??? just a test...
  byte padded[paddedLen];
  aes.padPlaintext((char*)msg.c_str(), padded);
  aes.padPlaintext((char*)msg.c_str(), padded);

  int baseLength = base64_enc_len(msg.length());
  char b64data[baseLength];

  // debug print b64data array that is supposed to be empty (filled with NULLs)...
  for (int j = 0; j < baseLength; j++) {
    b64data[j] = '\0';
    Serial.printf("%i ", b64data[j]);
  }
  Serial.println();

  char out[200] = {0};
  byte cipher[1000] = {0};

  int b64len = base64_encode(b64data, (char*)padded, paddedLen);
  if (b64len > 200) {
    Serial.println("B64 too long for output array!");
    return String("");
  }

  for (int j = 0; j < b64len; j++) {
    Serial.printf("%i ", b64data[j]);
  }
  Serial.println();

  Serial.printf("Encrypting %s\n", b64data);
  aes.do_aes_encrypt((byte *)b64data, b64len, cipher, key, 128, my_iv);
  base64_encode(out, (char *)cipher, aes.get_size() );
  Serial.printf("Encrypted/encoded: %s\n", out);
  return String((char*)out);
}
