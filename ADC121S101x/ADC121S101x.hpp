#ifndef ADC121S101x_HPP_15_08_2021
#define ADC121S101x_HPP_15_08_2021

#ifdef ARDUINO
#ifndef ESP8266
#error ONLY ESP8266 SUPPORTED in LIB
#endif
#include <ADC121S101x_ESP8266.hpp>

#else
#include <ADC121S101x_Native_debug.hpp>
#endif


static const constexpr float VERSION = 0.8;

/* uncomment for debug support*/
//#define EXT_ADC_DEBUG

#define BYTE_TO_BINARY(byte)       \
    (byte & 0x80 ? '1' : '0'),     \
        (byte & 0x40 ? '1' : '0'), \
        (byte & 0x20 ? '1' : '0'), \
        (byte & 0x10 ? '1' : '0'), \
        (byte & 0x08 ? '1' : '0'), \
        (byte & 0x04 ? '1' : '0'), \
        (byte & 0x02 ? '1' : '0'), \
        (byte & 0x01 ? '1' : '0')

/* For now sample frequency and measurement time are static because of tight timing contrains

thx to https://arduino.stackexchange.com/questions/44531/arduino-esp8266-direct-fast-control-of-the-digital-pins
and https://forum.arduino.cc/t/arduino-esp8266-faster-direct-port-write/407251/6

DATASHEET https://www.ti.com/lit/ds/symlink/adc121s101.pdf?ts=1629036221757&ref_url=https%253A%252F%252Fwww.google.com%252F


Sample time is time between 13th rising clock and falling of CS
shutdown mode active when CS high between 2-10th falling clock else
normal mode.

get out of shutdown mode needs thus one (invalid) normal mode sample 
get into sleepmode makes current sample invalid.

*/

#define stringify( name ) # name



enum CMD_type
{
    WAKE_UP,
    CONVERSION,
    SHUTDOWN,
    SINGLE_SHOT, //7.375us sample/track time on 500Ohm 26pF internal track cap
    MULTI,
    ADC_NONE
};

/*enum CMD_conversion_type
{
    EXT_ADC12_CONVERSION,
    EXT_ADC12_SINGLE_SHOT, //7.375us sample/track time on 500Ohm 26pF internal track cap
    EXT_ADC12_MULTI,
    EXT_ADC12_CONVERSION_NONE
};

enum CMD_end_type
{
    EXT_ADC12_WAKE_UP,
    EXT_ADC12_SHUTDOWN,
    EXT_ADC12_END_NONE
};*/



class ADC121S101x
{
private:

#ifdef EXT_ADC_DEBUG
#warning using debug, WHICH IS BUGGY BY ITSELF!
    // with help from https://forum.arduino.cc/t/printf-with-f-macro-how-did-i-do/221229
    void _p(const __FlashStringHelper *format, ...)
    {
        va_list args;
        va_start(args, format);
        char buf[128]; // resulting string limited to 128 chars
#ifdef __AVR__
        vsnprintf_P(buf, sizeof(buf), (const char *)format, args); // progmem for AVR
#else
        vsnprintf(buf, sizeof(buf), (const char *)format, args); // for the rest of the world
#endif
        
        DEBUGPORT.print(buf);
        va_end(args);
    }

    void _ph(const __FlashStringHelper *format, ...)
    {
        va_list args;
        va_start(args, format);
        _p(F("<<ADC_EXT %u> "), this->CS_pin);
        _p(format, args);
        DEBUGPORT.print(F(">\n"));
        va_end(args);
    }

#define DBG_PRINTF(format, ...) _p(F(format), ##__VA_ARGS__)
#define DBG_PRINTF_SHORT(format, ...) _ph(F(format), ##__VA_ARGS__)


#else
#define DBG_PRINTF(...)
#define DBG_PRINTF_SHORT(...)
#endif

    /* data */
    const uint8_t CS_pin;
    const uint16_t CS_mode;

    union ADC121S101x_Measurement
    {
        uint8_t buff[2];
        uint16_t rawADC;
    } measurement;

    union ADC121S101x_measure_long
    {
        uint32_t rawADC;
        uint8_t buff[2];
    };
    bool is_shutdown;

    inline void _start_cmds();
    inline void _end_cmds();

    inline uint16_t _send_cmd(enum CMD_type cmdType);

    void swp_buff(uint8_t *buff)
    {

#ifdef __BYTE_ORDER__
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        uint8_t tmp = buff[0];
        buff[0] = buff[1];
        buff[1] = tmp;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                                                                 // do nothing
#else
#error No implemetation yet
#endif
#else
#ifndef ARDUINO
#else
#error DEFINE BYTEORDER
#endif
#endif
    };

public:
    ADC121S101x(const uint8_t CS_pin, bool start_sleep = false);
    void begin();

    uint16_t send_cmd(enum CMD_type cmdType, bool sleep = false, uint8_t numCmds = 1);

    //uint16_t send_cmds(enum CMD_conversion_type CMD_c = EXT_ADC12_CONVERSION_NONE, enum CMD_end_type CMD_e = EXT_ADC12_END_NONE);

    uint16_t single_shot();
    uint16_t conversion();

    void shutdown();
    void wake_up();

    uint16_t conversion_avg(uint8_t samples);
    bool get_shutdown_state();

    void printConversion(uint16_t ref);
    ~ADC121S101x();
};
#endif