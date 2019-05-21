#include "AESLib.h"

uint8_t AESLib::getrnd()
{
   return aes.getrandom();
}

void AESLib::gen_iv(byte  *iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= (byte)getrnd();
    }
}

void AESLib::set_paddingmode(paddingMode mode){
  aes.setPadMode(mode);
}

paddingMode AESLib::get_paddingmode(){
  return aes.getPadMode();
}


int AESLib::get_cipher_length(int msglen){
  return aes.get_padded_len(msglen);
}


int AESLib::get_cipher64_length(int msglen){
  return base64_enc_len(aes.get_padded_len(base64_enc_len(msglen)));
}

/* Returns Arduino String decoded and decrypted. */
String AESLib::decrypt(String msg, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int len = msg.length();
  char encrypted[base64_dec_len((char*)msg.c_str(),len)]; 
  int b64len = base64_decode(encrypted, (char*)msg.c_str(), msg.length());

  byte out[b64len];
  int plain_len = aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, bits, (byte *)my_iv);
  // unpad the string
  out[plain_len] = 0; // add string termination

  int outLen = base64_dec_len((char*)out, plain_len);
  char message[outLen+1]; // trailing zero for cstring

  int baseLen = base64_decode(message, (char *)out, plain_len);
  //message[baseLen] = '\0'; // ensure trailing zero after cstring <--not needed is already done in base64_decode

  return String(message);
}

/* Suggested size for the plaintext buffer is 1/2 length of `msg` */
void AESLib::decrypt64(char * msg, char * plain, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int msgLen = strlen(msg);
  char encrypted[base64_dec_len(msg,msgLen)+1]; 
  int b64len = base64_decode(encrypted, msg, msgLen);
  // decrypting will keep the message length
  byte out[b64len]; 
  int plain_len = aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, bits, (byte *)my_iv);
  // unpad the string
  out[plain_len ] = 0; // add string termination
  Serial.print("- Decrypt paddedtext    ");dumpHex(out,b64len)
  Serial.print("- Decrypt plain length  ");Serial.println(plain_len);
  int outLen = base64_dec_len((char*)out, plain_len);
  char message[outLen+1]; // trailing zero for cstring?

  int baseLen = base64_decode(message, (char *)out, plain_len);
  //message[baseLen] = '\0'; // ensure trailing zero after cstring

  strcpy(plain, message);
}

/* Returns byte array decoded and decrypted. */
void AESLib::decrypt(char * msg, char * plain, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int msgLen = strlen(msg);
  char encrypted[msgLen]; // will be always shorter than Base64
  int b64len = base64_decode(encrypted, msg, msgLen);

  byte out[2*msgLen];
  int plain_len = aes.do_aes_decrypt((byte *)encrypted, b64len, out, key, bits, (byte *)my_iv);
  // unpad the string
  out[plain_len ] = 0; // add string termination
  
  int outLen = base64_dec_len((char*)out, plain_len);
  char message[outLen+1]; // trailing zero for cstring?
  
  strcpy(plain, message);
}

/* Returns Arduino string encrypted and encoded with Base64. */
String AESLib::encrypt(String msg, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int msgLen = strlen(msg.c_str());

  char b64data[base64_enc_len(msgLen)+1]; //+1 to store \0 at the end
  int b64len = base64_encode(b64data, (char*)msg.c_str(), msgLen);

  // paddedLen is a mutiple of the N_BLOCK size
  // paddedLen = (((int)(b64len/N_BLOCK) + 1)*N_BLOCK
  // KOV the +1 is not needed since this is no string anymore so no \0 character needed for ending
  //     depending on the padding strategy an additional N_BLOCK bytes will be added
  int paddedLen =  aes.get_padded_len(b64len);
  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);
  
  // cipher will keep the length of the padded message
  // do_aes_encrypt will pad the message so use the unpadded source
  byte cipher[paddedLen];
  aes.do_aes_encrypt((byte *)b64data, b64len, cipher, key, bits, my_iv);

  char out[base64_enc_len(paddedLen)+1];
  base64_encode(out, (char *)cipher, paddedLen );

  return String((char*)out);
}

/* Returns message encrypted and base64 encoded to be used as string. */
void AESLib::encrypt64(char * msg, char * output, byte key[],int bits, byte my_iv[]) {

#ifdef AES_DEBUG
  Serial.print("incoming msg: "); Serial.println(msg);
  Serial.print("incoming k-size: "); Serial.println(bits);
  Serial.print("incoming v-size: "); Serial.println(16); // initialization vector has the same size as the block size = 16
#endif

  aes.set_key(key, bits);

  int msgLen = strlen(msg);
#ifdef AES_DEBUG
  Serial.print("- msg >>"); Serial.print(msg);Serial.println("<<");
  Serial.print("- msgLen "); Serial.println(msgLen);
  Serial.print("- bits :");Serial.println(bits);
#endif

  char b64data[base64_enc_len(msgLen)+1];  // should add 1 character to accomodate the 0x\0 ending character
#ifdef AES_DEBUG
  Serial.print("- b64data [");Serial.print(base64_enc_len(msgLen));Serial.println("]");
#endif

  int b64len = base64_encode(b64data, msg, msgLen);
#ifdef AES_DEBUG
  Serial.print("- b64len ");Serial.println(b64len);
  Serial.print("- b64data ");Serial.println(b64data);
  Serial.print("- b64data ");dumpHex(b64data,b64len)
#endif

  int paddedLen = aes.get_padded_len(b64len);
#ifdef AES_DEBUG
  Serial.print("- paddedLen ");Serial.println(paddedLen);
  Serial.print("- cipherLen ");Serial.println(aes.get_padded_len(b64len));
#endif
  // this padding is not needed. The do_aes_encrypt will pad anyhow
  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);
#ifdef AES_DEBUG
  Serial.print("- padPlaintext "); dumpHex(padded,paddedLen)
#endif

  byte cipher[paddedLen];
  aes.do_aes_encrypt((byte *)b64data, b64len, cipher, key, bits, my_iv);
#ifdef AES_DEBUG
  Serial.print("- do_aes_encrypt "); dumpHex(cipher,paddedLen)
#endif

  char out2[base64_enc_len(paddedLen)+1];
  base64_encode(out2, (char *)cipher, aes.get_size() );
#ifdef AES_DEBUG
  Serial.println("- base64_encode");
#endif

  strcpy(output, (char*)out2);
#ifdef AES_DEBUG
  Serial.println("- strcpy");
#endif
}

/* Returns message encrypted only to be used as byte array. */
void AESLib::encrypt(char * msg, char * output, byte key[],int bits, byte my_iv[]) {

  aes.set_key(key, bits);

  int msgLen = strlen(msg);

  char b64data[base64_enc_len(msgLen)+1];
  int b64len = base64_encode(b64data, (char*)msg, msgLen);

  int paddedLen =  aes.get_padded_len(b64len);;
  byte padded[paddedLen];
  aes.padPlaintext(b64data, padded);
  // encryption will return the same number of bytes as the padded message
  // do_aes_encrypt will pad the message so use the unpadded source
  byte cipher[paddedLen];
  aes.do_aes_encrypt((byte *)b64data, b64len, cipher, key, bits, my_iv);

  strcpy(output, (char*)cipher);
}

void AESLib::clean() {
  aes.clean();
}
