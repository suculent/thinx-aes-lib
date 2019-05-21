/* Minimalistic example for Readme */

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
  // workaround for incorrect B64 functionality on first run...
  Serial.println("encrypt()");
  Serial.println(encrypt(strdup(plaintext.c_str()), aes_iv));
}

String encrypt(char * msg, byte iv[]) {  
  int msgLen = strlen(msg);
  Serial.print("msglen = "); Serial.println(msgLen);
  // msglen --> base64_enc_len
  // base64_enc_len --> cipher_len 
  // cipher_len --> base64_len
  int cipherlength = aesLib.get_cipher64_length(msgLen);
  char encrypted[cipherlength]; // AHA! needs to be large, 2x is not enough
  aesLib.encrypt64(msg, encrypted, aes_key,sizeof(aes_key), iv);
  Serial.print("encrypted = "); Serial.println(encrypted);
 // for(int i = 0;i<cipherlength;i++)
 //   Serial.print(encrypted[i],HEX);
  Serial.print("encrypted length :");Serial.println(cipherlength);

  return String(encrypted);
}

String decrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);  // 
  char decrypted[msgLen]; // base64_dec_len(msglen); half may be enough
  aesLib.decrypt64(msg, decrypted, aes_key, sizeof(aes_key),iv);
  return String(decrypted);
}

void setup() {
  Serial.begin(77800);
  while (!Serial); // wait for serial port
  delay(2000);
  Serial.print("[0]size of a byte = ");Serial.println((int)sizeof(byte));
  Serial.print("[0]size of a char = ");Serial.println((int)sizeof(char));
  Serial.print("[0]size of a aes_iv = ");Serial.println((int)sizeof(aes_iv));
  Serial.println("[1]aes_init()");
  aes_init();
  aesLib.set_paddingmode(paddingMode::Array);
  //
  // verify with https://gchq.github.io/CyberChef/#recipe=To_Base64('A-Za-z0-9%2B/%3D')
  //
  Serial.print("[1]free heap: "); Serial.println(ESP.getFreeHeap());
  char b64in[16] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  char b64out[base64_enc_len(sizeof(aes_iv))];
  Serial.print("[2]Base64 encode length for b64in (16 ascii 0 characters) :"); Serial.println(base64_enc_len(sizeof(b64in)));
  base64_encode(b64out,b64in,16);
  Serial.print("[2]Base64 encode aes_iv :"); Serial.println(b64out);
  Serial.print("[2]free heap: "); Serial.println(ESP.getFreeHeap());
  delay(1000);
  char b64enc[base64_enc_len(10)];
  Serial.print("[3]Base64 encode length for '0123456789' :"); Serial.println(base64_enc_len(10)); // MDEyMzQ1Njc4OQ==
  base64_encode(b64enc,"0123456789",10);
  Serial.print("[3]Base64 encode '0123456789' :"); Serial.println(b64enc);
  char b64dec[ base64_dec_len(b64enc,sizeof(b64enc))];
  Serial.print("[3]Base64 decode length for '0123456789' :"); Serial.println(base64_dec_len(b64enc,sizeof(b64enc)));
  base64_decode(b64dec,b64enc,sizeof(b64enc));
  Serial.print("Base64 decode '0123456789' :"); Serial.println(b64dec);
  Serial.print("free heap: "); Serial.println(ESP.getFreeHeap());
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
    
  if (Serial.available() > 0) {

    loopcount++; Serial.println(loopcount); // entry counter
    
    String readBuffer = Serial.readStringUntil('\n');
    Serial.print("free heap: "); Serial.println(ESP.getFreeHeap());
    Serial.println("INPUT:" + readBuffer);    
    
    sprintf(cleartext, "%s", readBuffer.c_str()); // must not exceed 255 bytes; may contain a newline

    // Encrypt
    String encrypted = encrypt(cleartext, enc_iv);
    sprintf(ciphertext, "%s", encrypted.c_str());
    Serial.print("Ciphertext: ");
    Serial.println(encrypted);  
    delay(1000);
    Serial.print("free heap: "); Serial.println(ESP.getFreeHeap());
     // Decrypt
     delay(1000);
    String decrypted = decrypt( ciphertext, dec_iv);
    Serial.print("Cleartext: ");
    Serial.println(decrypted);
    if (decrypted.equals(cleartext)){
      Serial.println("SUCCES");
    }
    else
    {
      Serial.println("FAILURE");
    
    }
  Serial.print(ESP.getFreeHeap());
  for(int i = 0;i<16;i++){
    enc_iv[i]= 0;
    dec_iv[i]= 0;
  }
  } 
}