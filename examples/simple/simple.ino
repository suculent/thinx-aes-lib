/* Minimalistic example for Readme */

#include "AESLib.h"

AESLib aesLib;

String plaintext = "AAAAAAA";
int loopcount = 0;

char cleartext[256];
char ciphertext[512];

// AES Encryption Key
byte aes_key[] = { 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30 };

// General initialization vector (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
  // workaround for incorrect B64 functionality on first run...
  encrypt("HELLO WORLD!", aes_iv);
}

String encrypt(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  char encrypted[4 * msgLen]; // AHA! needs to be large, 2x is not enough
  aesLib.encrypt64(msg, encrypted, aes_key, iv);
  return String(encrypted);
}

String decrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char decrypted[msgLen]; // half may be enough
  aesLib.decrypt64(msg, decrypted, aes_key, iv);
  return String(decrypted);
}

void setup() {
  Serial.begin(115200);
  aes_init();
}

void loop() {

  loopcount++;

  sprintf(cleartext, "%s", plaintext.c_str());

  // Encrypt
  byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
  String encrypted = encrypt(cleartext, enc_iv);
  sprintf(ciphertext, "%s", encrypted.c_str());
  Serial.print("Ciphertext: ");
  Serial.println(encrypted);

  // Decrypt
  byte dec_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
  String decrypted = decrypt(ciphertext, dec_iv);
  Serial.print("Cleartext: ");
  Serial.println(decrypted);

  delay(500);
}
