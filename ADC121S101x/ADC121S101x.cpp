#include <ADC121S101x.hpp>

#ifndef ARDUINO
int asdfgh = 0;
SPI_ SPI;
SER Serial;
#endif

#ifdef EXT_ADC_DEBUG
const char * CMDSTRING[] ={ // TODO SHOUDL BE SAVED IN PROGMEM
stringify(WAKE_UP),
stringify(CONVERSION),
stringify(SHUTDOWN),
stringify(SINGLE_SHOT), 
stringify(MULTI),
stringify(ADC_NONE)
};
#endif

/*TODO SHOULD READ CURRENT MODE, NOW FORCES INPUT on CS_MODE IN DESTRUCTOR*/
ADC121S101x::ADC121S101x(const uint8_t CS_pin, bool start_sleep) : CS_pin(CS_pin), CS_mode(CS_pin & 0x0), is_shutdown(start_sleep)
{
    #ifndef ARDUINO
    printf("current constructor: pin: %u : %u Mode: %u : %u is_shutdown %u : %u\n", CS_pin, this->CS_pin, CS_pin & 0x0, this->CS_mode, start_sleep, is_shutdown);
    #endif
}

void ADC121S101x::begin()
{
    pinMode(this->CS_pin, OUTPUT);
    digitalWrite(this->CS_pin, HIGH); // disable Slave Select

    //DBG_PRINTF("Initialized EXT_ADC on pin: %u\n", this->CS_pin);
    DBG_PRINTF_SHORT("Initialized");

    SPI.begin();
    if (this->is_shutdown)
    {
        DBG_PRINTF_SHORT("Initial Shutdown");
        this->_send_cmd(SHUTDOWN); // BUG when only sending SHUTDOWN it actually sends WAKEUP
    }
    else
    {
        DBG_PRINTF_SHORT("Initial Wakeup");
        this->_send_cmd(WAKE_UP);
    }
}
bool ADC121S101x::get_shutdown_state()
{
    return this->is_shutdown;
}

ADC121S101x::~ADC121S101x()
{
    send_cmd(SHUTDOWN);
    pinMode(this->CS_pin, this->CS_mode); // restore
    SPI.end();                            // is this a good idea?
}

uint16_t ADC121S101x::send_cmd(enum CMD_type cmdType, bool go_shutdown, uint8_t numCmds)
{
    //ADC121S101x::measurement
    //SPI.beginTransaction(SPISettings(SPICLCK, BITORDER, SPI_MODE2));
        _start_cmds();
    DBG_PRINTF_SHORT("State: %s", this->is_shutdown?"Shutdown":"awake");
    DBG_PRINTF_SHORT("Starting general command: %s", CMDSTRING[cmdType]);

    if (cmdType != ADC_NONE && cmdType != SHUTDOWN && this->is_shutdown)
    {
        DBG_PRINTF_SHORT("Waking up");
        this->_send_cmd(WAKE_UP);
        this->is_shutdown = false;
    }
    switch (cmdType)
    {
    case ADC_NONE:
    case WAKE_UP:
        break;
    case SHUTDOWN:
        go_shutdown = true;
        break;
    case SINGLE_SHOT:
        go_shutdown = true;
    case CONVERSION:
        this->measurement.rawADC = _send_cmd(CONVERSION);
        break;
    case MULTI:
    {
        uint32_t meas = 0U;

        for (uint8_t ii = 0; ii < numCmds; ii++)
        {
            this->measurement.rawADC = _send_cmd(CONVERSION);
            swp_buff(this->measurement.buff);
            meas += this->measurement.rawADC;
            //swp_buff_long();
            //meas.rawADC
        }
        this->measurement.rawADC = meas / numCmds;
    }
    break;
    }
    if (go_shutdown)
    {
        _send_cmd(SHUTDOWN);
        this->is_shutdown = true;
    }
    _end_cmds();

    //post processing
    switch (cmdType)
    {

    case SINGLE_SHOT:
    case CONVERSION:
        swp_buff(this->measurement.buff);
        break;
    default:
        break;
    }

    /*
    switch (cmdType)
    {
    case SHUTDOWN:
        this->_send_cmd(cmdType);
        this->is_shutdown = true;
        break;
    case CONVERSION:
    case SINGLE_SHOT:
        this->measurement.rawADC = this->_send_cmd(CONVERSION);
        //break;
        if (cmdType == SINGLE_SHOT)
        {
            go_shutdown = true;
        }
        break;
    case MULTI:
    {
        uint32_t meas = 0U;

        for (uint8_t ii = 0; ii < numCmds; ii++)
        {
            this->measurement.rawADC = _send_cmd(CONVERSION);
            swp_buff(this->measurement.buff);
            meas += this->measurement.rawADC;
            //swp_buff_long();
            //meas.rawADC
        }
        this->measurement.rawADC = meas / numCmds;
        break;
    }
    case WAKE_UP:
        break;
    }
    if ((this->is_shutdown = go_shutdown))
        this->_send_cmd(SHUTDOWN);
    SPI.endTransaction();
    //post processing
    switch (cmdType)
    {

    case SINGLE_SHOT:
    case CONVERSION:
        swp_buff(this->measurement.buff);
        break;
    default:
        break;
    }
    */
    return this->measurement.rawADC;
}

// VERY RAW OUTPUT, byte order is even wrong
uint16_t ADC121S101x::_send_cmd(enum CMD_type cmdType)
{
    union ADC121S101x_Measurement Measure;
    const uint32_t PINBIT = myPinBit(this->CS_pin);
    DBG_PRINTF_SHORT("cmd send: %s", CMDSTRING[cmdType] );

    os_intr_lock();
    //WRITE_PERI_REG(ON, myPinBit(D1));
    WRITE_PERI_REG(OFF, PINBIT); // write CS pin low
    //WRITE_PERI_REG(OFF, myPinBit(D1));
    switch (cmdType)
    {
    case SHUTDOWN:
        SPI1CMD |= SPIBUSY;
        os_delay_us(2);
        WRITE_PERI_REG(ON, PINBIT);
        break;
    default:
        SPI.transferBytes(nullptr, Measure.buff, sizeof(Measure));
        WRITE_PERI_REG(ON, PINBIT); // write CS pin high
        break;
    }
    os_intr_unlock();

    if (cmdType == SHUTDOWN) // not nessasary but safer
        while ((SPI1CMD & SPIBUSY))
        {
        };

    return Measure.rawADC;
}

uint16_t ADC121S101x::single_shot()
{
    return send_cmd(SINGLE_SHOT);
}
uint16_t ADC121S101x::conversion() { return send_cmd(CONVERSION); }

void ADC121S101x::shutdown()
{
    send_cmd(SHUTDOWN);
}
void ADC121S101x::wake_up()
{
    send_cmd(WAKE_UP);
}

uint16_t ADC121S101x::conversion_avg(uint8_t samples)
{
    return send_cmd(MULTI, false, samples);
}

inline void ADC121S101x::_start_cmds()
{
    SPI.beginTransaction(SPISettings(SPICLCK, BITORDER, SPI_MODE2));
};

inline void ADC121S101x::_end_cmds()
{
    SPI.endTransaction();
};


void ADC121S101x::printConversion(uint16_t ref){
    uint16_t rawADC = this->measurement.rawADC;
    DEBUGPORT.printf("SPI adc reading = %c%c%c %c %c%c%c%c %c%c%c%c %c%c%c%c\t%u count \t%u mV\n",BYTE_TO_BINARY(rawADC>>8), BYTE_TO_BINARY(rawADC),rawADC,/*ESP.getVcc()*/ref*rawADC/(2<<12) );
}
