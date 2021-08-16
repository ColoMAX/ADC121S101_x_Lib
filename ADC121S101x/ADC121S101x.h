#ifndef ADC121S101x_15_08_2021
#define ADC121S101x_15_08_2021

#include <Arduino.h>
#include <SPI.h>
#include <esp8266_peri.h>

ADC_MODE(ADC_VCC);
//CPHA=0: Samples of the falling edge of clock pulse
//Clock Polarity (CPOL) =1
/*
Mode	    Clock Polarity (CPOL)	Clock Phase (CPHA)	Output Edge	Data Capture
SPI_MODE0	        0   	                0   	        Falling	    Rising
SPI_MODE1	        0   	                1	            Rising	    Falling
SPI_MODE2	        1   	                0   	        Rising	    Falling
SPI_MODE3	        1   	                1	            Falling	    Rising
*/
constexpr const int CS_pin = D3;
constexpr const uint32_t SPICLCK = 1000000;
constexpr const uint32_t SPICLCKFAST = 20000000;
constexpr const uint8_t BITORDER = MSBFIRST ;//LSBFIRST; 
constexpr const uint32_t ON = PERIPHS_GPIO_BASEADDR + 4U;
constexpr const uint32_t OFF = PERIPHS_GPIO_BASEADDR + 8U;



#define myPinBit(myPin) (1<<myPin)
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

static_assert(SPICLCK<=20000000);
static_assert(SPICLCKFAST<=20000000);


        union ADC121S101x
    {
        uint8_t buff[2];
        uint16_t rawADC;
    };

void ADC12_init(){
    pinMode(CS_pin, OUTPUT);
    digitalWrite(CS_pin, HIGH); // disable Slave Select
    SPI.begin();
}

static inline void wake_up();
static inline void power_down();
static inline uint16_t conversion_raw();

int ADC12_single_conversion(){
    //union ADC121S101x
    //{
    //    uint8_t buff[2];
    //    uint16_t rawADC;
    //};
    //ADC121S101x meas;
    //SPI.beginTransaction(SPISettings(SPICLCK, BITORDER, SPI_MODE2));
//
    //os_intr_lock();
    //WRITE_PERI_REG(OFF , myPinBit(CS_pin) ); // write CS pin low
    //SPI.transferBytes(nullptr/*(uint8_t *)"124"*/,meas.buff, sizeof(meas.buff) )//;
    //WRITE_PERI_REG(ON , myPinBit(CS_pin) ); // write CS pin high
    //os_delay_us(3);
    //SPI1CMD |= SPIBUSY;
    //os_delay_us(3);
    //WRITE_PERI_REG(ON , myPinBit(CS_pin) );
//
    //os_intr_unlock();
    //SPI.endTransaction();
    //uint8_t tmp = meas.buff[0];
    //meas.buff[0] = meas.buff[1];
    //meas.buff[1] = tmp;
    wake_up();
    uint16_t rawADC = conversion_raw();
    power_down();

    Serial.printf("SPI adc reading = %c%c%c %c %c%c%c%c %c%c%c%c %c%c%c%c\t%u count \t%u mV\n",BYTE_TO_BINARY(rawADC>>8), BYTE_TO_BINARY(rawADC),rawADC,/*ESP.getVcc()*/5000U*rawADC/(2<<12) );
    return rawADC;
}

static inline void wake_up(){
    uint8_t voidbuff[2];
    SPI.beginTransaction(SPISettings(SPICLCK, BITORDER, SPI_MODE2));
    os_intr_lock();
    WRITE_PERI_REG(OFF , myPinBit(CS_pin) ); // write CS pin low
    SPI.transferBytes(nullptr,voidbuff, sizeof(voidbuff));
    WRITE_PERI_REG(ON , myPinBit(CS_pin) ); // write CS pin high
    os_intr_unlock();
    SPI.endTransaction();
}
static inline void power_down(){
    uint8_t voidbuff[2];
    SPI.beginTransaction(SPISettings(SPICLCK, BITORDER, SPI_MODE2));
    os_intr_lock();
    
    WRITE_PERI_REG(OFF , myPinBit(CS_pin) ); // write CS pin low
    //os_delay_us(1);
    SPI1CMD |= SPIBUSY;
    os_delay_us(2);
    WRITE_PERI_REG(ON , myPinBit(CS_pin) );
    //SPI.transferBytes(nullptr,voidbuff, sizeof(voidbuff));
    //WRITE_PERI_REG(ON , myPinBit(CS_pin) ); // write CS pin high
    os_intr_unlock();

    SPI.endTransaction();
}
static inline uint16_t conversion_raw(){
    ADC121S101x meas;

    SPI.beginTransaction(SPISettings(SPICLCK, BITORDER, SPI_MODE2));
    os_intr_lock();
    
    WRITE_PERI_REG(OFF , myPinBit(CS_pin) ); // write CS pin low
    SPI.transferBytes(nullptr,meas.buff, sizeof(meas));
    WRITE_PERI_REG(ON , myPinBit(CS_pin) ); // write CS pin high
    os_intr_unlock();

    SPI.endTransaction();
    uint8_t tmp = meas.buff[0];
    meas.buff[0] = meas.buff[1];
    meas.buff[1] = tmp;
    return meas.rawADC;
}

#endif