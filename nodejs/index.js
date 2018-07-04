var CryptoJS = require("crypto-js");

// Sample encrypted text "START; 380"
//var esp8266_msg = 'ei6NxsBeWk7hj41eia3S0Od26goTtxHvwO6V27LwSW4=';
var esp8266_msg = 'ei6NxsBeWk7hj41eia3S0Od26goTtxHvwO6V27LwSW4=';


// null_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
var esp8266_iv  = 'AAAAAAAAAAAAAAAAAAAAAA==';

// 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
var AESKey = '2B7E151628AED2A6ABF7158809CF4F3C';

var plain_iv =  new Buffer(esp8266_iv, 'base64').toString('hex');
var iv = CryptoJS.enc.Hex.parse(plain_iv);
var key= CryptoJS.enc.Hex.parse(AESKey);

// Decrypt
var bytes  = CryptoJS.AES.decrypt( esp8266_msg, key, { iv: iv } );
var plaintext = bytes.toString(CryptoJS.enc.Base64);
var decoded_b64msg =  new Buffer(plaintext, 'base64').toString('ascii');
var decoded_msg =     new Buffer(decoded_b64msg, 'base64').toString('ascii');

console.log("Decrypted message: ", decoded_msg);

// Re-encrypt
var plaintext_b64 = Buffer.from(decoded_msg).toString('base64');
iv = CryptoJS.enc.Hex.parse(plain_iv); // reset IV back to 0
var ebytes = CryptoJS.AES.encrypt( plaintext_b64, key, { iv: iv } );
var ciphertext = ebytes.toString();
console.log("ciphertext: ", ciphertext);
