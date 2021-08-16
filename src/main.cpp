
#ifdef ARDUINO
#include <Arduino.h>
#endif

#define USECPP_

#ifdef USECPP_

#include "ADC121S101x.hpp"
ADC121S101x ADC_ext(D3, false);
#else
#include <ADC121S101x.h>
#endif

#ifdef ARDUINO
void setup()
{
#else
int main()
{
  printf("check\n");
#endif
  // put your setup code here, to run once:
  Serial.begin(250000);
#ifdef USECPP_
  //pinMode(D1, OUTPUT);
  ADC_ext.begin();
  ADC_ext.shutdown();

  for (int ii = 0; ii < 3; ii++)
  {
    uint16_t rawADC = ADC_ext.send_cmd(SINGLE_SHOT); //ADC_ext.single_shot();
    //uint16_t rawADC = ADC_ext.send_cmd(CONVERSION, true);
    ADC_ext.printConversion(5000);
  }
#else
  ADC12_init();
#endif

#ifdef ARDUINO
}
void loop()
{
#else
  while (1)
  {
#endif
// put your main code here, to run repeatedly:
#ifdef USECPP_

#else
    ADC12_single_conversion();
    delay(100);
#endif

#ifdef ARDUINO
#else
  }
  return 0;
#endif
}