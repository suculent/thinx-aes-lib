# THiNX AESLib (ESP32, ESP8266, Arduino, NRF5x)

An ESP32/ESP8266 library for Arduino IDE to wrap AES encryption with Base64 support. This project is originally based on [AESLib by kakopappa](https://github.com/kakopappa/arduino-esp8266-aes-lib). This fork actually works, will be maintained at least for a while, and provides optimised methods that do not require using Arduino's flawed String objects (even though those are still in examples).

AESLib provides convenience methods for encrypting data to byte arrays and Strings, with optional additional base64 encoding to return strings instead of bare data.

Since ESP8266 Arduino Core 2.6.2 is already out, this might be updated to use AES implementation from BearSSL (to save more RAM in larger projects). But it would loose compatibility with AVR so this is a NO for now.

*Note:* You will have to provide your own IV in all calls. If you want to use gen_iv(), you will also have to seed the PRNG properly using srand() - notice that initialization typically takes the same time on every startup, so just using millis() will probably not result in a good seed.

## Tested on

* ESP8266 (OK)
* Arduino Uno (OK)
* Arduino Mega 2560 (OK)

## Changes

`2.3.5` - Fixed a memory leak in `decrypt64` (via [@motniemtin](https://github.com/motniemtin))

`2.3.4` - Buildable for [NRF5x Core by Sandeep Mistry](https://github.com/sandeepmistry/arduino-nRF5)

`2.3.3` - Reviewed/fixed examples in Arduino IDE (ESP8266 Core v3.0.2)

`2.3.2` - Reviewed and simplified example `base_64iv`, added example `platformio/aes-test-3-base64_iv` for that purpose

`2.3.1` - Restored functionality of B64 functions, fixed examples `complex` and `base64_iv`

`2.3.0` - Interface change (encode function does not do base64 by default anymore), fixed tests to pass

`2.2.6` - Fixed invalid decode crash on ESP32 (via [@Emanavas](https://github.com/emanavas))

`2.2.5` - Library manager update fixed

`2.2.4` - Removed unneeded and non-functional IV code (via [@hansliss](https://github.com/hansliss))

`2.2.1` - Major decryption fix

`2.2.0` - Improved examples

`2.1.8` - Improved examples

`2.0.8` - Input buffer reuse optimizations by (via [@ElMohamed](https://github.com/ElMohamed))

`2.0.7` – Applied `const` specifiers throughout the library (via [@kenkendk](https://github.com/kenkendk))

`2.0.6` – Added Travis CI unit and platform tests; getrnd() is mocked on platforms without time() or millis() is used instead

`2.0.5` – Restored backwards compatibility with AVR; updated Simple and Medium examples

`2.0.3` – Added unit tests; thus fixed getrnd()

`2.0.1` - Cleaner implementation, dropping Arduino framework in favour of testability and portability

`2.0` - Fixed padding, added parametrisation (via [@kavers1](https://github.com/kavers1)), restored Arduino compatibility, memory optimisations

`1.0.5` - Fixed generating random IV; fixed #include directive filename case

`1.0.4` - Fixed simple example

`1.0.3` - Fixed padding (after encoding, not before)

## Client Example

See `examples`.

## Server Example

Requires node.js and npm.

Enter the `nodejs` folder in Terminal and install required npm packages with `npm install .` command.

You can run the example with `node index.js` as you know it, and then dig into the source code to adjust for your purposes.

See `node/index.js` for implementation details.

## References

This is an AES library for the ESP8266, based on tzikis's AES library for Arduino, was previously [here](https://github.com/tzikis/arduino). Tzikis library was based on scottmac's library, which was previously [here](https://github.com/scottmac/arduino), but now seems to be removed. The library is code-wise compatible with Arduino AVR, but it requires more RAM than it is usually available on Arduino boards.

[AES by spaniakos](https://github.com/spaniakos/AES/)

[Primal Cortex](https://primalcortex.wordpress.com/2016/06/17/esp8266-logging-data-in-a-backend-aes-and-crypto-js/)

[arduino-esp8266-aes-encryption-with-nodejs](https://github.com/kakopappa/arduino-esp8266-aes-encryption-with-nodejs)

[arduino-esp8266-aes-lib](https://github.com/kakopappa/arduino-esp8266-aes-lib)
