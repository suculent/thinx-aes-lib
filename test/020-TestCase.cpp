// 020-TestCase.cpp

// Run with:
// # g++ -std=c++14 -Wall -o 020-TestCase 020-TestCase.cpp ../src/*.cpp && ./020-TestCase --success

//
// Configuration
//

#include "../src/AESLib.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iomanip>
#include <sstream>
#include <string>
#include <cstdint>

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

std::string intToHexString(uint8_t intValue) {
    std::string hexStr;
    std::stringstream sstream;
    sstream << std::setfill ('0') << std::setw(2)
    << std::hex << (int)intValue;
    hexStr = sstream.str();
    sstream.clear();
    return hexStr;
}

AESLib aesLib;

#define INPUT_BUFFER_LIMIT (128 + 1) // designed for Arduino UNO, not stress-tested anymore (this works with readBuffer[129])

char cleartext[INPUT_BUFFER_LIMIT] = {0};
char ciphertext[2*INPUT_BUFFER_LIMIT] = {0};
char readBuffer[33] = "Looks like key but it's not me.";

byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
byte aes_iv[N_BLOCK] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 2 bytes (16 bits)
byte enc_iv_to[N_BLOCK] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 2 bytes (16 bits)
byte enc_iv_from[N_BLOCK] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 2 bytes (16 bits)

// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
}

// must not be in production code, ever
uint16_t encrypt_to_ciphertext(char * msg, byte iv[]) {
  uint16_t msgLen = strlen(msg);
  memcpy(aes_iv, iv, N_BLOCK);
  uint16_t cipherLength = aesLib.encrypt((byte*)msg, msgLen, ciphertext, aes_key, sizeof(aes_key), iv);
  return cipherLength;
}

uint16_t decrypt_to_cleartext(byte msg[], uint16_t msgLen, byte iv[]) {
  memcpy(aes_iv, iv, N_BLOCK);
  uint16_t dec_len = aesLib.decrypt(msg, msgLen, cleartext, aes_key, sizeof(aes_key), iv);
  return dec_len;
}

//
// Test
//

#define CATCH_CONFIG_MAIN

#include "catch2.hpp"

static char test_string[] = "Teghesu Geleghesu";
uint8_t new_iv[N_BLOCK] = { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 };

TEST_CASE( "AES Random Generator must be properly initialized.", "[single-file]" ) {
    AESLib aesLib;
    REQUIRE( aesLib.getrnd() > 0 );
}

TEST_CASE( "AES IV must be non-null after initialization.", "[single-file]" ) {
    aesLib.gen_iv(new_iv); // crashes?
    REQUIRE( new_iv[0] != 0 );
}

TEST_CASE( "Encrypted test string has expected length.", "[single-file]" ) {
    REQUIRE( encrypt_to_ciphertext(test_string, enc_iv_to) == 44 );
}

TEST_CASE( "Decrypted test string has expected contents.", "[single-file]" ) {
    REQUIRE( decrypt_to_cleartext((byte*)ciphertext, 44, enc_iv_from) == 17 );
}
