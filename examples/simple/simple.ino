/* Simple buffer example for low-memory conditions (Arduino) */

#include "AESLib.h"

#define BAUD 9600

AESLib aesLib;

#define INPUT_BUFFER_LIMIT (128 + 1) // designed for Arduino UNO, not stress-tested anymore (this works with readBuffer[129])

unsigned char cleartext[INPUT_BUFFER_LIMIT] = {0}; // THIS IS INPUT BUFFER (FOR TEXT)
unsigned char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)

unsigned char readBuffer[18] = "username:password";

unsigned char zmi_username[6] = "username";
unsigned char zmi_password[11] = "password";

// AES Encryption Key (same as in node-js example)
byte aes_key[] = { 0x04, 0x0B, 0x04, 0x05, 0x05, 0x09, 0x04, 0x07, 0x05, 0x05, 0x05, 0x02, 0x05, 0x05, 0x01, 0x00 };

// General initialization vector (same as in node-js example) (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0x04, 0x0B, 0x04, 0x05, 0x05, 0x09, 0x04, 0x07, 0x05, 0x05, 0x05, 0x02, 0x05, 0x05, 0x01, 0x00 };

// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
}

uint16_t encrypt_to_ciphertext(char * msg, uint16_t msgLen, byte iv[]) {
  Serial.println("Calling encrypt (string)...");
  // aesLib.get_cipher64_length(msgLen);
  int cipherlength = aesLib.encrypt((byte*)msg, msgLen, (char*)ciphertext, aes_key, sizeof(aes_key), iv);
                   // uint16_t encrypt(byte input[], uint16_t input_length, char * output, byte key[],int bits, byte my_iv[]);
  return cipherlength;
}

uint16_t decrypt_to_cleartext(byte msg[], uint16_t msgLen, byte iv[]) {
  Serial.print("Calling decrypt...; ");
  uint16_t dec_bytes = aesLib.decrypt(msg, msgLen, (char*)cleartext, aes_key, sizeof(aes_key), iv);  
  Serial.print("Decrypted bytes: "); Serial.println(dec_bytes);
  return dec_bytes;
}

void setup() {
  Serial.begin(BAUD);
  Serial.setTimeout(60000);
  delay(2000);

  aes_init(); // generate random IV, should be called only once? causes crash if repeated...

}

/* non-blocking wait function */
void wait(unsigned long milliseconds) {
  unsigned long timeout = millis() + milliseconds;
  while (millis() < timeout) {
    yield();
  }
}

unsigned long loopcount = 0;

byte enc_iv_to[N_BLOCK] = { 0x04, 0x0B, 0x04, 0x05, 0x05, 0x09, 0x04, 0x07, 0x05, 0x05, 0x05, 0x02, 0x05, 0x05, 0x01, 0x00 };
byte enc_iv_from[N_BLOCK] = { 0x04, 0x0B, 0x04, 0x05, 0x05, 0x09, 0x04, 0x07, 0x05, 0x05, 0x05, 0x02, 0x05, 0x05, 0x01, 0x00 };

void loop() {

  Serial.print("readBuffer length: "); Serial.println(sizeof(readBuffer));

   // must not exceed INPUT_BUFFER_LIMIT bytes; may contain a newline
  sprintf((char*)cleartext, "%s", readBuffer);

  // Encrypt
  // iv_block gets written to, provide own fresh copy... so each iteration of encryption will be the same.
  uint16_t msgLen = sizeof(readBuffer);
  uint16_t encLen = encrypt_to_ciphertext((char*)cleartext, msgLen, enc_iv_to);
  Serial.print("Encrypted length = "); Serial.println(encLen );
  memset(enc_iv_to, 0, sizeof(enc_iv_to));

  Serial.println("Encrypted. Decrypting..."); Serial.println(encLen ); Serial.flush();
  uint16_t decLen = decrypt_to_cleartext(ciphertext, encLen , enc_iv_from);  
  Serial.print("Decrypted cleartext:\n"); Serial.printf("%s\n", (char*)cleartext);
  memset(enc_iv_from, 0, sizeof(enc_iv_from));

  if (strcmp((char*)readBuffer, (char*)cleartext) == 0) {
    Serial.println("Decrypted correctly.");
  } else {
    Serial.println("Decryption test failed.");
  }

  Serial.println("---");

}
