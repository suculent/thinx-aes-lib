var CryptoJS = require("crypto-js");

// INIT VECTOR /////////////////////////////////////////////////////////////////

// null_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
var base64_iv  = 'AAAAAAAAAAAAAAAAAAAAAA==';
var plain_iv =  new Buffer(base64_iv, 'base64').toString('hex');
var iv  = CryptoJS.enc.Hex.parse(plain_iv);

// console.log("plain_iv: ", plain_iv);

// KEY /////////////////////////////////////////////////////////////////////////

// 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
var AESKey = '2B7E151628AED2A6ABF7158809CF4F3C';
var key = CryptoJS.enc.Hex.parse(AESKey);

// MESSAGE /////////////////////////////////////////////////////////////////////

var message = "Looks like key but it's not me.";

// ENCRYPT /////////////////////////////////////////////////////////////////////

// Encrypt
iv = CryptoJS.enc.Hex.parse(plain_iv); // resets IV back to initial state
var ebytes = CryptoJS.AES.encrypt( message, key, {
    iv: iv,
    mode: CryptoJS.mode.CBC,
    padding: CryptoJS.pad.ZeroPadding
});
var ciphertext = ebytes.toString();
console.log("ciphertext: ", ciphertext);

// DECRYPT /////////////////////////////////////////////////////////////////////

// Decrypt
var bytes  = CryptoJS.AES.decrypt( ciphertext, key, {
  iv: iv,
  mode: CryptoJS.mode.CBC,
  padding: CryptoJS.pad.ZeroPadding
});
var plaintext = bytes.toString(CryptoJS.enc.Base64);
var decoded_b64msg =  new Buffer(plaintext, 'base64').toString('ascii');

console.log("Decrypted message: ", decoded_b64msg);

if (decoded_b64msg == message) {
  console.log("Test passed.\n");
  process.exit(0);
} else {
  console.log("Test failed.\n");
  process.exit(1);
}
