# TI ADC121S101x Driver for ESP8266

#include ADC121S101x.hpp

12bit single SPI ADC.

Has not been elaboratly tested!
SPI might have weird behaviour during startup, so delay actions from startup.

The devices have 2 basic modes: shutdown and normal.
This depends on the Chip select (CS) and SPI clock (CLK).

enable debug by uncommenting the define in ADC121S101x_debug.h

There is also a pure C variant, but is not recommended ADC121S101x.h

Caution this lib was made in a hurry and with very limited knowledge of C++11 (and C++ in general).