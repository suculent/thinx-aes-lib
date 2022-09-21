/* Large buffer example for Readme with heavy load (1.5K) */

#include "AESLib.h"

#define BAUD 9600

AESLib aesLib;

#define INPUT_BUFFER_LIMIT 3072

char cleartext[INPUT_BUFFER_LIMIT] = {0}; // THIS IS INPUT BUFFER (FOR TEXT)
char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)

// 1072 bytes (>1K) works with 2048K+4096K buffer
// char readBuffer[1074] = "123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B12345678\nXXX1072\0";

// 1522 bytes (<2K) works with 3072K buffers, works without additional base64 pre-coding
char readBuffer[1522] = "123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0296123456789A123456789B123456789C123456789D123456789E123456789F123456789G0064123456789A123456789B123456789C123456789D123456789E123456789F123456789G0128123456789A123456789B123456789D123456789E123456789F123456789G0128123456789A123456789B123456789D123456789E123456789F123456789G0128123456789A123456789B123456789D123456789E123456789F123456789G0128123456789A123456789B123456789D123456789E123456789F123456789G0128123456789A123456789B123456789D123456789E123456789F123456789G0128123456789A123456789B123456789D123456789E123456789F123456789G0128123456789A123456789B123456789D123456789E123456789F123456789G0128123456789A123456789B12345678\nXXX1522\0";

// AES Encryption Key (same as in node-js example)
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (same as in node-js example) (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Generate IV (once)
void aes_init() {
  // aesLib.gen_iv(aes_iv); // would generate random IV for forward encryption, but we set own from above anyway...
  aesLib.set_paddingmode((paddingMode)0);
}

uint16_t encrypt_to_ciphertext(char * msg, uint16_t msgLen, byte iv[]) {
  Serial.println("Calling encrypt (string)...");
  int cipherlength = aesLib.get_cipher64_length(msgLen);
  aesLib.encrypt((byte*)msg, msgLen, (byte*)ciphertext, aes_key, sizeof(aes_key), iv);
  return cipherlength;
}

uint16_t decrypt_to_cleartext(char * msg, uint16_t msgLen, byte iv[]) {
  Serial.print("Calling decrypt...; ");
  Serial.print("[decrypt_to_cleartext] free heap: "); Serial.println(ESP.getFreeHeap());
  uint16_t dec_len = aesLib.decrypt((byte*)msg, msgLen, (byte*)cleartext, aes_key, sizeof(aes_key), iv);
  Serial.print("Decrypted bytes: "); Serial.println(dec_len);
  return dec_len;
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

byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

byte enc_iv_to[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte enc_iv_from[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void loop() {

#ifdef ESP8266
  Serial.print("»» LOOP BEGINS. Free heap: "); Serial.println(ESP.getFreeHeap());
#endif

  Serial.print("readBuffer length: "); Serial.println(strlen(readBuffer));

   // must not exceed INPUT_BUFFER_LIMIT bytes; may contain a newline. Encoded will be a bit more than 1.5x long as original.
  sprintf(cleartext, "%s", readBuffer);

  // Encrypt
  // iv_block gets written to, provide own fresh copy... so each iteration of encryption will be the same.
  memcpy(enc_iv, enc_iv_to, sizeof(enc_iv_to));
  uint16_t len = encrypt_to_ciphertext(cleartext, sizeof(readBuffer), enc_iv_to);
  Serial.print("Encrypted length = "); Serial.println(len);

  Serial.println("Encrypted. Decrypting..."); Serial.println(len); Serial.flush();
  memcpy(enc_iv, enc_iv_from, sizeof(enc_iv_from));
  uint16_t dlen = decrypt_to_cleartext(ciphertext, len, enc_iv_from);
  Serial.print("Decrypted cleartext in length of:\n"); Serial.println(dlen);
  Serial.print("Decrypted cleartext:\n"); Serial.println(cleartext);

  if (strcmp(readBuffer, cleartext) == 0) {
    Serial.println("Decrypted correctly.");
  } else {
    Serial.println("Decryption test failed.");
  }

#ifdef ESP8266
  Serial.print("»» LOOP ENDED. Free heap: "); Serial.println(ESP.getFreeHeap());
#endif

  Serial.println("---");

}
