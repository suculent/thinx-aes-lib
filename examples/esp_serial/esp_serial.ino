/* Medium example for ESP8266 (not for Arduino, uses additional Base64 layer) */

#include "AESLib.h"

AESLib aesLib;

String plaintext = "HELLO WORLD!";

char cleartext[256];
char ciphertext[512];

// AES Encryption Key
byte aes_key[] = { 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30 };

// General initialization vector (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Generate IV (once)
void aes_init() {
  Serial.println("gen_iv()");
  aesLib.gen_iv(aes_iv);
  Serial.println("encrypt()");
  Serial.println(encrypt(strdup(plaintext.c_str()), plaintext.length(), aes_iv));
}

String encrypt(char * msg, uint16_t msgLen, byte iv[]) {  
  int cipherlength = aesLib.get_cipher64_length(msgLen);
  char encrypted[cipherlength]; // AHA! needs to be large, 2x is not enough
  aesLib.encrypt64(msg, encrypted, aes_key, sizeof(aes_key), iv);
  Serial.print("encrypted = "); Serial.println(encrypted);
  return String(encrypted);
}

String decrypt(char * msg, uint16_t msgLen, byte iv[]) {
  unsigned long ms = micros();
  char decrypted[msgLen];
  aesLib.decrypt64(msg, decrypted, aes_key, sizeof(aes_key), iv);
  return String(decrypted);
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // wait for serial port
  delay(2000);
  //Serial.println("[1]aes_init()");
  aes_init();
  aesLib.set_paddingmode(paddingMode::Array);
  //
  // verify with https://cryptii.com
  // previously: verify with https://gchq.github.io/CyberChef/#recipe=To_Base64('A-Za-z0-9%2B/%3D')
  //
  char b64in[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char b64out[base64_enc_len(sizeof(aes_iv))];
  //Serial.print("[2]Base64 encode length for b64in (16 ascii 0 characters) :"); Serial.println(base64_enc_len(sizeof(b64in)));
  base64_encode(b64out, b64in, 16);
  //Serial.print("[2]Base64 encode aes_iv :"); Serial.println(b64out);
  delay(1000);
  char b64enc[base64_enc_len(10)];
  //Serial.print("[3]Base64 encode length for '0123456789' :"); Serial.println(base64_enc_len(10)); // MDEyMzQ1Njc4OQ==
  base64_encode(b64enc, "0123456789", 10);
  //Serial.print("[3]Base64 encode '0123456789' :"); Serial.println(b64enc);
  char b64dec[ base64_dec_len(b64enc, sizeof(b64enc))];
  //Serial.print("[3]Base64 decode length for '0123456789' :"); Serial.println(base64_dec_len(b64enc,sizeof(b64enc)));
  base64_decode(b64dec, b64enc, sizeof(b64enc));
  //Serial.print("Base64 decode '0123456789' :"); Serial.println(b64dec);
  delay(1000);

  Serial.println("Enter text to be encrypted into console (no feedback) and press ENTER (newline):");
}

/* non-blocking wait function */
void wait(unsigned long milliseconds) {
  unsigned long timeout = millis() + milliseconds;
  while (millis() < timeout) {
    yield();
  }
}

unsigned long loopcount = 0;
byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
byte dec_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


void loop() {

  if (Serial.available()) {
    loopcount++; Serial.println(loopcount); // entry counter

    String readBuffer = Serial.readStringUntil('\n');
    Serial.println("INPUT:" + readBuffer);

    sprintf(cleartext, "%s", readBuffer.c_str()); // must not exceed 255 bytes; may contain a newline

    // Encrypt
    uint16_t clen = String(cleartext).length();
    String encrypted = encrypt(cleartext, clen, enc_iv);
    sprintf(ciphertext, "%s", encrypted.c_str());
    Serial.print("Ciphertext: ");
    Serial.println(encrypted);
    delay(1000);
    // Decrypt
    delay(1000);
    uint16_t dlen = encrypted.length();
    String decrypted = decrypt( ciphertext, dlen, dec_iv);
    Serial.print("Cleartext: ");
    Serial.println(decrypted);
    if (decrypted.equals(cleartext)) {
      Serial.println("SUCCES");
    }
    else
    {
      Serial.println("FAILURE");

    }
    for (int i = 0; i < 16; i++) {
      enc_iv[i] = 0;
      dec_iv[i] = 0;
    }
  }
}
