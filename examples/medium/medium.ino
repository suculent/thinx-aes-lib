/* Minimalistic example for Readme with heavy load (>1K) */

#include "AESLib.h"

#define BAUD 9600

AESLib aesLib;

#ifndef __AVR__
#define INPUT_BUFFER_LIMIT 2048
#else
#define INPUT_BUFFER_LIMIT 32
#endif

char cleartext[INPUT_BUFFER_LIMIT] = {0}; // THIS IS INPUT BUFFER (FOR TEXT)
char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)

// 887 bytes (<1K)
// String readBuffer = "123456789123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G\n887";

// 1072 bytes (~1K) works with 2048K+4096K buffer
#ifndef __AVR__
String readBuffer = "123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B12345678\nXXX1072";
#else
String readBuffer = "123456789";
#endif

// 1522 bytes does not work with 3072K buffers, crashes because of dual base64... use standard encrypt() instead; see large example

// AES Encryption Key (same as in node-js example)
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (same as in node-js example) (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
  aesLib.set_paddingmode((paddingMode)0);
}

uint16_t encrypt_to_ciphertext(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  int cipherlength = aesLib.get_cipher64_length(msgLen);
  char encrypted_bytes[cipherlength];
  uint16_t enc_length = aesLib.encrypt64((byte*)msg, msgLen, encrypted_bytes, aes_key, sizeof(aes_key), iv);
  sprintf(ciphertext, "%s", encrypted_bytes);
  return enc_length;
}

void decrypt_to_cleartext(char * msg, uint16_t msgLen, byte iv[]) {
  Serial.println("Calling decrypt64...");
#ifdef ESP8266
  Serial.print("[decrypt_to_cleartext] free heap: "); Serial.println(ESP.getFreeHeap());
#endif
  uint16_t decLen = aesLib.decrypt64(msg, msgLen, (byte*)cleartext, aes_key, sizeof(aes_key), iv);
  Serial.print("Decrypted bytes: "); Serial.println(decLen);
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

byte enc_iv_to[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte enc_iv_from[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void loop() {

#ifdef ESP8266
  Serial.print("»» LOOP BEGINS. Free heap: "); Serial.println(ESP.getFreeHeap());
#endif

  Serial.print("readBuffer length: "); Serial.println(readBuffer.length());

   // must not exceed INPUT_BUFFER_LIMIT bytes; may contain a newline
  sprintf(cleartext, "%s", readBuffer.c_str());

  memcpy(enc_iv_to, aes_iv, sizeof(aes_iv));
  memcpy(enc_iv_from, aes_iv, sizeof(aes_iv));

  // Encrypt
  // iv_block gets written to, provide own fresh copy... so each iteration of encryption will be the same.
  uint16_t len = encrypt_to_ciphertext(cleartext, enc_iv_to);

  memset(cleartext, 0, sizeof(cleartext));

  Serial.print("Encrypted length = "); Serial.println(len);

  Serial.println("Encrypted. Decrypting..."); Serial.flush();
  decrypt_to_cleartext(ciphertext, len, enc_iv_from);
  Serial.print("Decrypted cleartext:\n"); Serial.println(cleartext);

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
