/* Simple buffer example for low-memory conditions (Arduino) */

#include <AESLib.h>

#define BAUD 230400

AESLib aesLib;

#define INPUT_BUFFER_LIMIT (16 + 1) // TEST + \0

unsigned char cleartext[INPUT_BUFFER_LIMIT] = {0};    // THIS IS INPUT BUFFER (FOR TEXT)

byte dec_iv[N_BLOCK]        = {0};

// AES Encryption Key (same as in node-js example)
static byte aes_key[]       = { 0xdf, 0x4f, 0x66, 0x80, 0x9b, 0xaf, 0xe5, 0x8f, 0xe1, 0x6a, 0xba, 0x7f, 0x4c, 0xb3, 0xe8, 0xbc };

static byte aes_iv[N_BLOCK] = { 0x3b, 0x69, 0x88, 0x60, 0x53, 0x84, 0x01, 0xcd, 0xd1, 0x46, 0x50, 0x42, 0x78, 0xae, 0xec, 0xc9 };

unsigned char data[]        = { 0x56, 0x0d, 0x5e, 0xca, 0xe1, 0x44, 0x04, 0xf4, 0xd1, 0x43, 0x93, 0x1d, 0x6d, 0x4e, 0x47, 0x05 };


int16_t decrypt_to_cleartext(byte msg[], uint16_t msgLen, byte iv[]) {
  Serial.print("Calling decrypt... "); Serial.print(msgLen); Serial.println(" Bytes");
  uint16_t dec_bytes = aesLib.decrypt(msg, msgLen, (char*)cleartext, aes_key, sizeof(aes_key), iv);
  Serial.print("Decrypted bytes: "); Serial.println(dec_bytes);
  return dec_bytes;
}

void setup() {
  Serial.begin(BAUD);
  Serial.setTimeout(60000);
  delay(2000);
}

unsigned long loopcount = 0;

void export_cleartext(int encLen) {
  int l = strlen((const char*)cleartext);
  Serial.print("cleartext BYTES: "); Serial.println(l);
  //if (l < encLen) encLen = l;
  for (int i = 0; i < encLen; i++) {
    Serial.print(cleartext[i], HEX); 
    Serial.print(", "); 
  }
  Serial.println();
}

int padding = 0;

void loop() {

  //memset(cleartext, 0, 16);

  //export_cleartext(16);

  Serial.print("Current padding mode: "); Serial.println(padding);

  aesLib.set_paddingmode((paddingMode)padding); // <-- added

  padding++; if (padding > 6) padding = 0;

  // reset initialization vetor
  memcpy(dec_iv, aes_iv, sizeof(aes_iv));

  size_t data_size = sizeof(data); // sizeof(data);
  Serial.print("Decrypting data of size "); Serial.println(data_size);
  int16_t decLen = decrypt_to_cleartext(data, data_size, dec_iv);

  if (decLen > 16) decLen = 16;
  
  export_cleartext(decLen);
 
  Serial.print("Decrypted cleartext of length: "); Serial.println(decLen);
  Serial.print("Decrypted cleartext:\n"); Serial.println((char*)cleartext);

  Serial.println("---");
  Serial.println();
  delay(1000);

}