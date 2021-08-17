#include <Arduino.h>

#include <ADC121S101x.hpp>

constexpr const uint16_t ADC_SUPPLY_VOLTAGE = 5000U; //milli Volt

//initilize    CS pin, start chip in sleep
ADC121S101x ADC_ext(D3, false);
//mutiple ADC's should be supported (NOT TESTED)
void setup(){
    Serial.begin(115200);
    /* Chip will be set into sleepmode determend by contructor*/
    ADC_ext.begin();
    
    Serial.printf("Wake up, and read: %u\n", ADC_ext.analogRead());
    // stays awake afterwards
    //Similair functions:
    ADC_ext.shutdown(); // but we can make it sleep manually
    ADC_ext.wake_up(); // or wake up
    // now it is tracking the voltage
    uint16_t raw = ADC_ext.conversion()// will know it is awake and just do start to do the converion
    if(raw == ADC_ext.measurement.rawADC){ // results of each command are stored, but may be unusable, because...
        ADC_ext.printConversion(ADC_SUPPLY_VOLTAGE);
    }
    raw = ADC_ext.send_cmd(/*first command (after possible wakeup)*/ SINGLE_SHOT, /*go to sleep afterwards*/ true); // SINGLE shot does a single conversion and send shutdown, so this is equvant to ADC_ext.send_cmd( SINGLE_SHOT,false);

    uint16_t raw_avr = ADC_ext.conversion_avg(/*samples to average*/ 10);

    Serial.printf("is device still (presumably) awake? %s", ADC_ext.get_shutdown_state()?"No in shutdown mode.\n":"Yes\n");
    
    //but there are a few more:
    ADC_ext.VREF = ADC_SUPPLY_VOLTAGE;

    float volt = ADC_ext.to_voltage_f(ADC_ext.send_cmd(MULTI, true, 15));
    Serial.printf("Voltage of avergae %f\n", volt); 
}

void loop(){

}