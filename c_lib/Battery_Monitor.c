#include "Battery_Monitor.h"

static const float BITS_TO_BATTERY_VOLTS = 0;

/**
 * Function Battery_Monitor_Init initializes the Battery Monitor to record the current battery voltages.
 */
void Battery_Monitor_Init()
{

ADCSRA |= (1 << ADPS0);
ADCSRA |= (1 << ADPS1);
ADCSRA |= (1 << ADPS2);
// 128 prescaler

   ADMUX |= (1 << REFS1);
   ADMUX |= (1 << REFS0);
// configures for internal 2.56V Voltage Reference with external capacitor on AREF pin

  // SET input voltage for ADC be from ADC6 (000110 for MUX5..0 in ADMUX register)
   ADMUX |= (1 << MUX2);
   ADMUX |= (1 << MUX1);

   float BITS_TO_BATTERY_VOLTS = 1024;


}

/**
 * Function Battery_Voltage initiates the A/D measurement and returns the result for the battery voltage.
 */
float Battery_Voltage()
{
    // A Union to assist with reading the LSB and MSB in the  16 bit register
   union { struct {uint8_t LSB; uint8_t MSB; } split; uint16_t value;} data;


   // enables the ADC
ADCSRA |= (1 << ADEN);
ADCSRA |= (1 << ADSC);
   while(ADSC == 1) { // this won't work?
       // the conversion is running….
       // when while loop ends, conversion has completed
   }

// ADC conversion result: ADC = Vin * 1023 / Vref

   split.LSB = ADCL;  // (ADC Data Register Low byte)
   split.MSB = ADCH;// (ADC Data Register High byte)
   data.value = split.LSB + split.MSB * 256;
// “ADCL must be read first, then ADCH.”

   return data.value * BITS_TO_BATTERY_VOLTS;


}


