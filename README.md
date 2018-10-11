# THiNX AESLib (ESP32, ESP8266)

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/8dded023f3d14a69b3c38c9f5fd66a40)](https://www.codacy.com/app/suculent/thinx-aes-lib?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=suculent/thinx-aes-lib&amp;utm_campaign=Badge_Grade)

An ESP32/ESP8266/Arduino library for Arduino IDE to wrap AES encryption with Base64 support. This project is originally based on [AESLib by kakopappa](https://github.com/kakopappa/arduino-esp8266-aes-lib). This fork actually works, will be maintained at least for a while, and provides optimized methods that do not require using Arduino's flawed String objects (even though those are still in examples).

AESLib provides convenience methods for encrypting data to byte arrays and Strings, with optional additional base64 encoding to return strings instead of bare data.

In future this should use AES implementation from BearSSL, when available (to save more RAM in larger projects).

# Changes

`1.0.4` - fixed simple example
`1.0.3` - fixed padding (after encoding, not before) 

# Client Example

```

#include "AESLib.h"

AESLib aesLib;

int loopcount = 0;

char cleartext[256];
char ciphertext[512];

// AES Encryption Key
byte aes_key[] = { 0x15, 0x2B, 0x7E, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

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
  char encrypted[2 * msgLen];
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

  sprintf(cleartext, "START; %i \n", loopcount);  

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

```

# Server Example

Requires node.js and npm.

Enter the `nodejs` folder in Terminal and install required npm packages with `npm install .` command.

You can run the example with `node index.js` as you know it, and then dig into the source code to adjust for your purposes.

```
// Setup CryptoJS
var CryptoJS = require("crypto-js");
var esp8266_msg = 'ei6NxsBeWk7hj41eia3S0Od26goTtxHvwO6V27LwSW4='; // = "START; 380"
var esp8266_iv  = 'AAAAAAAAAAAAAAAAAAAAAA==';
var AESKey = '2B7E151628AED2A6ABF7158809CF4F3C';
var plain_iv = new Buffer(esp8266_iv, 'base64').toString('hex');
var iv = CryptoJS.enc.Hex.parse(plain_iv);
var key = CryptoJS.enc.Hex.parse(AESKey);

// Decrypt
var bytes  = CryptoJS.AES.decrypt( esp8266_msg, key, { iv: iv } );
var plaintext = bytes.toString(CryptoJS.enc.Base64);
var decoded_b64msg = new Buffer(plaintext, 'base64').toString('ascii');
var decoded_msg = new Buffer(decoded_b64msg, 'base64').toString('ascii');

console.log("Decrypted message: ", decoded_msg);
```


# References

This is an AES library for the Arduino, based on tzikis's AES library, was previously [here](https://github.com/tzikis/arduino). Tzikis library was based on scottmac's library, which was previously [here](https://github.com/scottmac/arduino), but now seems to be removed.

[AES by spaniakos](https://github.com/spaniakos/AES/)

[Primal Cortex](https://primalcortex.wordpress.com/2016/06/17/esp8266-logging-data-in-a-backend-aes-and-crypto-js/)

[arduino-esp8266-aes-encryption-with-nodejs](https://github.com/kakopappa/arduino-esp8266-aes-encryption-with-nodejs)

[arduino-esp8266-aes-lib](https://github.com/kakopappa/arduino-esp8266-aes-lib)
