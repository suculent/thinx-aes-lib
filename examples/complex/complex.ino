/* More complex example with additional logging and deprecated methods. */

#include <AESLib.h>

#define BAUD 230400

AESLib aesLib;

// AES Encryption Key
byte aes_key[] = { 0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30 };

// General initialization vector (use your own)
byte aes_iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
  encrypt("AAAAAAAAAA", aes_iv); // workaround for incorrect B64 functionality on first run... initing b64 is not enough
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

  Serial.printf("Encoded %i bytes to %s \n", enlen, output);
  sprintf(message, output);
  return String(output);
}

void print_iv() {
   byte null_iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   char iv[256];
   int ivlen = base64_encode(iv, (char*)null_iv, 16);
   Serial.println(iv);
}

String decode() {
  char decoded[200];
  int b64len = base64_decode(decoded, message, strlen(message));
  return String(decoded);
}

String encrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char encrypted[2 * msgLen];
  aesLib.encrypt64(msg, encrypted, aes_key, iv);
  Serial.print("Encryption took: ");
  Serial.print(micros() - ms);
  Serial.println("us");
  return String(encrypted);
}

String decrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char decrypted[msgLen]; // half may be enough
  aesLib.decrypt64(msg, decrypted, aes_key, iv);
  Serial.print("Decryption [2] took: ");
  Serial.print(micros() - ms);
  Serial.println("us");
  return String(decrypted);
}

void setup() {
  Serial.begin(BAUD);
  Serial.println("\nBooting...");
  aes_init();
}

void log_free_stack(String tag) {
  Serial.print("["); Serial.print(tag); Serial.print("] "); 
  Serial.print("free heap: "); Serial.println(ESP.getFreeHeap());
}

String plaintext = "12345678;";
int loopcount = 0;

char cleartext[256];
char ciphertext[1024];

// zEDQYJuYhIV5lLJeIB3qlQ==

void loop() {

  loopcount++;

  sprintf(cleartext, "AAAAAAAAA");

  print_key_iv();

  print_iv();

  Serial.println("ENCRYPTION (char*)");
  byte enc_iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, reqires always fresh copy.
  String encrypted = encrypt(cleartext, enc_iv);
  sprintf(ciphertext, "%s", encrypted.c_str());

  Serial.print("Encrypted Result: ");
  Serial.println(encrypted);
  Serial.println();

  Serial.println("DECRYPTION (char*)");
  Serial.println(ciphertext);
  byte dec_iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, reqires always fresh copy.
  String decrypted = decrypt(ciphertext, dec_iv);

  Serial.print("Decrypted Result: ");
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
