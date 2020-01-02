/* Minimalistic example for Readme */

#include "AESLib.h"

#define BAUD 9600

AESLib aesLib;

String plaintext = "HELLO WORLD!";

char cleartext[256] = {0};
char ciphertext[1024] = {0};

// AES Encryption Key (same as in node-js example)
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (same as in node-js example) (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Generate IV (once)
void aes_init() {
  Serial.println("aes_init() calling gen_iv()");
  aesLib.gen_iv(aes_iv);
}

String encrypt(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  Serial.print("msglen = "); Serial.println(msgLen);
  int cipherlength = aesLib.get_cipher64_length(msgLen);
  char encrypted[cipherlength];
  aesLib.encrypt64(msg, encrypted, aes_key, sizeof(aes_key), iv);
  Serial.print("encrypted = "); Serial.println(encrypted);
  return String(encrypted);
}

String decrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char decrypted[msgLen]; // half may be enough
  aesLib.decrypt64(msg, decrypted, aes_key, sizeof(aes_key), iv);
  return String(decrypted);
}

void setup() {
  Serial.begin(BAUD);
  Serial.setTimeout(60000);
  delay(2000);
  Serial.println("aes_init()");
  aes_init();

#ifdef ESP8266
  Serial.print("free heap: "); Serial.println(ESP.getFreeHeap());
#endif

  //Serial.println("Enter text to be encrypted into console (no feedback) and press ENTER (newline):");
}

/* non-blocking wait function */
void wait(unsigned long milliseconds) {
  unsigned long timeout = millis() + milliseconds;
  while (millis() < timeout) {
    yield();
  }
}

unsigned long loopcount = 0;

String readBuffer = "HELLO WORLD!";
String encrypted;

void loop() {

  sprintf(cleartext, "%s", readBuffer.c_str()); // must not exceed 255 bytes; may contain a newline
  Serial.print("CLEAR:"); Serial.println(cleartext);

  // Encrypt
  // iv_block gets written to, provide own fresh copy... so each iteration of encryption will be the same.
  byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  encrypted = encrypt(cleartext, enc_iv);
  sprintf(ciphertext, "%s", encrypted.c_str());

#ifdef ESP8266
  Serial.print("free heap: "); Serial.println(ESP.getFreeHeap());
#endif

  delay(10000);
}
