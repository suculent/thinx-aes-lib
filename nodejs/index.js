var CryptoJS = require("crypto-js");

var base64_iv  = 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA';
var iv  = CryptoJS.enc.Hex.parse(base64_iv);

var AESKey = '2B7E151628AED2A6ABF7158809CF4F3C';
var key = CryptoJS.enc.Hex.parse(AESKey);

var message = "username:password";

const password = AESKey;
console.log(password); // added vulnerability

// ENCRYPT /////////////////////////////////////////////////////////////////////

// Encrypt
iv = CryptoJS.enc.Hex.parse(base64_iv); // resets IV back to initial state
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
