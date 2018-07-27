/* Example taking test values from node.js server as input as well... */

#include "AESLib.h"

AESLib aesLib;

String plaintext = "12345678;";
int loopcount = 0;

char cleartext[256];
char ciphertext[512];

// AES Encryption Key
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Sample strings as generated by node.js server
String server_b64iv = "AAAAAAAAAAAAAAAAAAAAAAAA=="; // same as aes_iv  but in Base-64 form as received from server
String server_b64msg = "ei6NxsBeWk7hj41eia3S0LdkAlm2qxpRbmcsrd23TTc="; // same as aes_iv  but in Base-64 form as received from server

// Generate IV (once)
void aes_init() {
  // workaround for incorrect B64 functionality on first run...
  encrypt("HELLO WORLD!", aes_iv);

  print_key_iv();

  // reset aes_iv to server-based value
  int ivLen = base64_decode((char*)server_b64iv.c_str(), (char *)aes_iv, server_b64iv.length());
  Serial.print("Decoded IV bytes: ");
  Serial.println(ivLen);
  print_key_iv();
}

String encrypt(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  char encrypted[2 * msgLen];
  aesLib.encrypt(msg, encrypted, aes_key, iv);
  return String(encrypted);
}

String decrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char decrypted[msgLen]; // half may be enough
  aesLib.decrypt(msg, decrypted, aes_key, iv);
  return String(decrypted);
}

void print_key_iv() {

  int i;

  /*

  Serial.println("AES Key: ");
  for (i = 0; i < sizeof(aes_key); i++) {
    Serial.print(aes_key[i], DEC);
    if ((i + 1) < sizeof(aes_key)) {
      Serial.print(",");
    }
  }

  Serial.println(""); */

  Serial.println("AES IV: ");
  for (i = 0; i < sizeof(aes_iv); i++) {
    Serial.print(aes_iv[i], DEC);
    if ((i + 1) < sizeof(aes_iv)) {
      Serial.print(",");
    }
  }

  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  aes_init();

  print_key_iv();

  byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
  // first decrypt after init should use aes_iv as given by server to test bare string first
  String decrypted = decrypt((char*)server_b64msg.c_str(), enc_iv); // aes_iv fails here, incorrectly decoded...
  Serial.print("Server Cleartext: ");
  Serial.println(decrypted);

  print_key_iv();
}

void loop() {

  loopcount++;

  sprintf(cleartext, "START; %i \n", loopcount);

  print_key_iv();

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
