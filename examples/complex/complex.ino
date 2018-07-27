/* More complex example with additional logging and deprecated methods. */

#include <AESLib.h>

extern "C" {
#include "user_interface.h"
#include <cont.h>
  extern cont_t g_cont;
}

AESLib aesLib;

// AES Encryption Key
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (use your own)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // eq. 'AAAAAAAAAAAAAAAAAAAAAA==';

// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
  encrypt(""); // workaround for incorrect B64 functionality on first run... initing b64 is not enough
}

// Serves for debug logging the case where IV changes after use...

void print_key_iv() {

  int i;

  /*
    Serial.println("AES Key: ");
    for (i = 0; i < sizeof(aes_key); i++) {
     Serial.print(aes_key[i], DEC);
     if ((i+1) < sizeof(aes_key)) {
      Serial.print(",");
     }
    }

    Serial.println("");
  */

  Serial.println("AES IV: ");
  for (i = 0; i < sizeof(aes_iv); i++) {
    Serial.print(aes_iv[i], DEC);
    if ((i + 1) < sizeof(aes_iv)) {
      Serial.print(",");
    }
  }

  Serial.println("");
}

char message[200] = {0};

String encode(String msg) {

  char output[256] = {0};
  char input[256] = {0};
  sprintf(input, msg.c_str());

  int inputLen = strlen(input);
  int enlen = base64_encode(output, input, msg.length());
  
  sprintf(message, output);
  return String(output);
}

void print_iv() {
   byte null_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   char iv[256];
   int ivlen = base64_encode(iv, (char*)null_iv, N_BLOCK);
   Serial.println(iv);
}

String decode() {
  char decoded[200];
  int b64len = base64_decode(decoded, message, strlen(message));
  return String(decoded);
}

// V1
String encrypt(String msg) {
  unsigned long ms = micros();
  byte null_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, reqires always fresh copy.
  String encMsg = aesLib.encrypt(msg, aes_key, null_iv);
  Serial.print("Encryption took: ");
  Serial.print(micros() - ms);
  Serial.println("us");
  return encMsg;
}

// V2
String encrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char encrypted[2 * msgLen];
  aesLib.encrypt(msg, encrypted, aes_key, iv);
  Serial.print("Encryption took: ");
  Serial.print(micros() - ms);
  Serial.println("us");
  return String(encrypted);
}

// V1
String decrypt(String msg) {
  unsigned long ms = micros();
  byte null_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, reqires always fresh copy.
  String decMsg = aesLib.decrypt(msg, aes_key, null_iv);
  Serial.print("Decryption [1] took: ");
  Serial.print(micros() - ms);
  Serial.println("us");
  return decMsg;
}

// V2
String decrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char decrypted[msgLen]; // half may be enough
  aesLib.decrypt(msg, decrypted, aes_key, iv);
  Serial.print("Decryption [2] took: ");
  Serial.print(micros() - ms);
  Serial.println("us");
  return String(decrypted);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nBooting...");
  aes_init();
}

void log_free_stack(String tag) {
  extern cont_t g_cont;
  register uint32_t *sp asm("a1");
  unsigned long heap = system_get_free_heap_size();
  Serial.printf("[%s] STACK U=%4d ", tag.c_str(), cont_get_free_stack(&g_cont));
  Serial.printf("F=%4d ", 4 * (sp - g_cont.stack));
  Serial.print("H="); Serial.println(heap);
}

String plaintext = "12345678;";
int loopcount = 0;

char cleartext[256];
char ciphertext[512];

// zEDQYJuYhIV5lLJeIB3qlQ==

void loop() {

  loopcount++;

  sprintf(cleartext, "START; %i \n", loopcount);

  print_key_iv();

  print_iv();

  // V1
  //Serial.println("ENCRYPTION (String)");
  //String encrypted = encrypt(String(cleartext));

  // V2
  Serial.println("ENCRYPTION (char*)");
  byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, reqires always fresh copy.
  String encrypted = encrypt(cleartext, enc_iv);
  sprintf(ciphertext, "%s", encrypted.c_str());
  //ciphertext = encrypted.c_str();

  Serial.print("Result E: ");
  Serial.println(encrypted);
  Serial.println();

  // V1
  //Serial.println("DECRYPTION (String)");
  //String decrypted = decrypt(encrypted);


  // V2
  Serial.println("DECRYPTION (char*)");
  Serial.println(ciphertext);
  byte dec_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, reqires always fresh copy.
  String decrypted = decrypt(ciphertext, dec_iv);

  Serial.print("Result D: ");
  Serial.println(decrypted);

  String plain = String(cleartext);

  if (plain.indexOf(decrypted) == -1) {
    Serial.println("Decryption FAILED!");
    Serial.printf("At: %i \n", plain.indexOf(decrypted));
    delay(5000);
  } else {
    if (plain.length() == decrypted.length()) {
      Serial.println("Decryption successful.");
    } else {
      Serial.printf("Decryption length incorrect. Plain: %i, Dec: %i", plain.length(), decrypted.length());
    }
  }

  delay(500);
  Serial.println("\n\n");

  log_free_stack("loop-text-after");
}
