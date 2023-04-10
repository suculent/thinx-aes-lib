# THiNX AESLib (ESP32, ESP8266, Arduino, NRF5x)

**Security Professional Advisory**

> Please, do not use this library in your new project. It's designed for legacy devices with minimal memory footprint (or not enough memory to perform a HTTPS network connection). If you're in need of up-to date encryption system, refer to [Arduino Cryptography Library](https://rweather.github.io/arduinolibs/crypto.html) which supports CBC as well.
Also, the library's security theoretically depends on having a CSPRNG (Cryptographically Secure Prime Rundom Number Generator), where the easiest solution is to use external memory chip with such features in I2C/SPI bus which would also allow to store keys securely without having to be worried of someone hacking out your Arduino/ESP's flash chip out.

> *Note:* From a security point of view, you have to provide your own Initialization Vector (IV) to a sequence using specific Key that you use for encryption/decryption. In case of a received message, the initialization vector should be part of that message, as well as you should include that when you're sending the message.  You will have to provide your own IV in all calls. If you want to use gen_iv(), you will also have to seed the Random Generator (PRNG) properly using srand() - notice that initialization typically takes the same time on every startup, so just using millis() will probably not result in a good seed. Make sure you contact a NTP time server or have another seriously random input as a seed before requesting a time-based random number.
This may be not an issue in cases where seed is provided by "server" (or master MCU).
**That's why all the examples always reset IV to initial value, which is not common in a secure workflow, where the encryption/decryption function does not only output encrypted/decrypted value, but also updates IV in memory for the next step, so it can be used to encrypt streams starting with a single IV for each input/output stream.**

_This is originally a library for Arduino IDE (supporting Arduino, ESP32, ESP8266 and now NRF5x as well) to wrap AES encryption with optional Base64 support. This project is based on [AESLib by kakopappa](https://github.com/kakopappa/arduino-esp8266-aes-lib), where this fork has been actively maintained for a while (however it's now suggestedly replaced with [Arduino Cryptography Library](https://rweather.github.io/arduinolibs/crypto.html) and provides optimised methods that do not require using Arduino's flawed String objects (unless you'll have to use Base64)._

AESLib provides convenience methods for encrypting data to byte arrays and Strings, with optional additional base64 encoding to return strings instead of bare data.

## Tested on

* ESP8266 – Wemos D1 Mini & Mini Pro, and other variants (OK)
* Arduino Uno (OK)
* Arduino Mega 2560 (OK)
* Arduino SAMD (to be confirmed)
* nRF5x (to be confirmed)

## Changes

`2.3.6` - Added compatibility/buildability for Arduino SAMD architecture

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
