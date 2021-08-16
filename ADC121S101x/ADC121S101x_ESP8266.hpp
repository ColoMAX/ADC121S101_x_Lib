#ifndef ADC121S101x_ESP8266_16_08_2021
#define ADC121S101x_ESP8266_16_08_2021

#include <Arduino.h>
#include <SPI.h>
#include <esp8266_peri.h>

#ifndef DEBUGPORT
#define DEBUGPORT Serial
#endif 

#define myPinBit(myPin) (1 << myPin)



static constexpr const uint32_t SPICLCK = 1000000;
//static constexpr const uint32_t SPICLCKFAST = 20000000;
static constexpr const uint8_t BITORDER = MSBFIRST; //LSBFIRST;
static constexpr const uint32_t ON = PERIPHS_GPIO_BASEADDR + 4U;
static constexpr const uint32_t OFF = PERIPHS_GPIO_BASEADDR + 8U;


static_assert(SPICLCK <= 20000000);
//static_assert(SPICLCKFAST <= 20000000);

#endif