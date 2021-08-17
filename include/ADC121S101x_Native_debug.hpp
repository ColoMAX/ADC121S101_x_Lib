#ifndef ADCS101x_NATIVE_DEBUG_HPP_18_08_2021
#define ADCS101x_NATIVE_DEBUG_HPP_18_08_2021
/* Copyright Max Suurland 2021, all rights reserved*/
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#define EXT_ADC_DEBUG
#define F(x) x
#define __FlashStringHelper char
#define myPinBit(myPin) myPin

#define D3 3
//#define OUTPUT 4
//#define HIGH 1
#define ON HIGH
#define OFF LOW

#define pinMode(pin, mode) printf("pin %u changed mode to %s\n", pin, #mode)
#define digitalWrite(pin, mode) printf("pin %u changed state to %s\n", pin, #mode)

#define os_intr_lock() printf("entering locked region\n")
#define os_intr_unlock() printf("leaving locked region\n");

#define WRITE_PERI_REG(state, pin) printf("pin %u changed state to %s (FAST)\n", pin, #state)

#define os_delay_us(x) printf("delaying for %u us\n", x)
#define SPI1CMD asdfgh
#define SPIBUSY !printf("SPI clock active\n")

class SPI_{
    #define SPISettings(a,b,c)
    #define SPICLCK
    #define BITORDER
    #define SPI_MODE2
    public:
    SPI_(){}

    void begin(){
        printf("starting SPI\n");
    }
    void end(){
        printf("stopping SPI\n");
    }
    void transferBytes(uint8_t * send, uint8_t * receive, size_t size){
        printf("SPI transfer %lu bytes\n", size);
    }
    void beginTransaction(){
        printf("SPI starting send block\n");
    }
    void endTransaction(){
        printf("SPI ending send block\n");
    }
};



class SER{
    public:
    SER(){}
    void begin(long int baud){
        printf("Serial init baud %lu\n", baud);
    }

    void print(const char* fmt, ...){
        va_list args;
        va_start(args, fmt);
        vprintf (fmt, args);
        va_end(args);
    }
    void printf(const char* fmt, ...){
        va_list args;
        va_start(args, fmt);
        vprintf (fmt, args);
        va_end(args);
    }
};

extern SER Serial;
extern SPI_ SPI;

#define DEBUGPORT Serial

#endif
