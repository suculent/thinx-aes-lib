/* Minimalistic example for Readme */

#include "AESLib.h"

#define BAUD 9600

AESLib aesLib;

#define INPUT_BUFFER_LIMIT 2048

char cleartext[INPUT_BUFFER_LIMIT] = {0}; // THIS IS INPUT BUFFER
char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER, TOGETHER IT TAKES 3K but may be reused for output...

// AES Encryption Key (same as in node-js example)
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (same as in node-js example) (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
}

uint16_t encrypt_to_ciphertext(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  int cipherlength = aesLib.get_cipher64_length(msgLen);
  char encrypted_bytes[cipherlength];
  uint16_t enc_length = aesLib.encrypt64(msg, encrypted_bytes, aes_key, sizeof(aes_key), iv);
  sprintf(ciphertext, "%s", encrypted_bytes);
  return enc_length;
}

void decrypt_to_cleartext(char * msg, uint16_t msgLen, byte iv[]) {
  unsigned long ms = micros();
  char decrypted_bytes[msgLen];
  uint16_t decLen = aesLib.decrypt64(msg, decrypted_bytes, aes_key, sizeof(aes_key), iv);
  // Serial.print("Decrypted: "); Serial.println(String(decrypted_bytes));
  sprintf(cleartext, "%s", decrypted_bytes);
}

void setup() {
  Serial.begin(BAUD);
  Serial.setTimeout(60000);
  delay(2000);

  aes_init(); // generate random IV, should be called only once? causes crash if repeated...

#ifdef ESP8266
  Serial.print("[setup] free heap: "); Serial.println(ESP.getFreeHeap());
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

// cca 901 bytes with newline
// String readBuffer = "123456789123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G\n887";
// 1777 bytes
String readBuffer = "123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B12345678\nXXX1072";

byte enc_iv_to[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte enc_iv_from[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void loop() {

#ifdef ESP8266
  Serial.print("»» LOOP BEGINS. Free heap: "); Serial.println(ESP.getFreeHeap());
#endif

  Serial.print("readBuffer length: "); Serial.println(readBuffer.length());

   // must not exceed INPUT_BUFFER_LIMIT bytes; may contain a newline
  sprintf(cleartext, "%s", readBuffer.c_str());

  // Encrypt
  // iv_block gets written to, provide own fresh copy... so each iteration of encryption will be the same.

  uint16_t len = encrypt_to_ciphertext(cleartext, enc_iv_to);
  Serial.print("Encrypted (fake) length = "); Serial.println(strlen(ciphertext));
  Serial.print("Encrypted (real) length = "); Serial.println(len);
  Serial.println("Encrypted. Decrypting..."); Serial.flush();

  decrypt_to_cleartext(ciphertext, len, enc_iv_from);
  Serial.print("Decrypted cleartext:\n"); Serial.println(cleartext);

  // memory intensive test, may fail...
  if (readBuffer == String(cleartext)) {
    Serial.println("Decrypted correctly.");
  } else {
    Serial.println("Decryption test failed.");
  }

#ifdef ESP8266
  Serial.print("»» LOOP ENDED. Free heap: "); Serial.println(ESP.getFreeHeap());
#endif

  Serial.println("---");

}
