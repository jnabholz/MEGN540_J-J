#include "Encoder.h"

/**
* Internal counters for the Interrupts to increment or decrement as necessary.
*/
static volatile bool _last_right_A;  // Static limits it's use to this file
static volatile bool _last_right_B;  // Static limits it's use to this file

static volatile bool _last_left_A;   // Static limits it's use to this file
static volatile bool _last_left_B;   // Static limits it's use to this file
static volatile bool _last_left_XOR; // Necessary to check if PB4 triggered the ISR or not

static volatile int32_t _left_counts;   // Static limits it's use to this file
static volatile int32_t _right_counts;  // Static limits it's use to this file

/** Helper Funcions for Accessing Bit Information */
// *** MEGN540 Lab 3 TODO ***
// Hint, use avr's bit_is_set function to help
static inline bool Right_XOR() { return INT6; } // MEGN540 Lab 3 TODO
static inline bool Right_B()   { return PF0; } // MEGN540 Lab 3 TODO
static inline bool Right_A()   { return INT6^PF0; } // MEGN540 Lab 3 TODO

static inline bool Left_XOR() { return PCINT4; } // MEGN540 Lab 3 TODO
static inline bool Left_B()   { return PE2; } // MEGN540 Lab 3 TODO
static inline bool Left_A()   { return PCINT4^PE2; } // MEGN540 Lab 3 TODO

/**
 * Function Encoders_Init initializes the encoders, sets up the pin change interrupts, and zeros the initial encoder
 * counts.
 */
void Encoders_Init()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE

    // Left encoder uses PB4 and PE2 pins as digital inputs. External interrupt PCINT4 is necessary to detect
    // the change in XOR flag. You'll need to see Section 11.1.5 - 11.1.7 for setup and use.
    // Note that the PCINT interrupt is triggered by any PCINT pin. In the ISR you should check to make sure
    // the interrupt triggered is the one you intend on processing.


    // Right encoder uses PE6 and PF0 as digital inputs. External interrupt INT6 is necessary to detect
    // the changes in XOR flag. You'll need to see Sections 11.1.2-11.1.4 for setup and use.
    // You'll use the INT6_vect ISR flag.


    // Initialize static file variables. These probably need to be updated.
    _last_right_A = 0;  // MEGN540 Lab 3 TODO
    _last_right_B = 0;  // MEGN540 Lab 3 TODO

    _last_left_A = 0;  // MEGN540 Lab 3 TODO
    _last_left_B = 0;  // MEGN540 Lab 3 TODO
    _last_left_XOR = 0;  // MEGN540 Lab 3 TODO

    _left_counts = 0;  // MEGN540 Lab 3 TODO
    _right_counts = 0;  // MEGN540 Lab 3 TODO
    
    /*Set ISC61 to 0 and ISC60 to 1 in EICRB so any logical change on INT6 will trigger an
    ISR*/
    EICRB|=(1<<ISC60);
    EICRB&=(0<<ISC61);
    
    //Set EIMSK bit 6 equal to 1 to enable INT6
    EIMSK|=(1<<INT6);
    
    //Set PCIE0 to 1 in PCICR so any logical change on PCINT4 will trigger an ISR
    PCICR|=(1<<PCIE0);
    
    //Set PCINT4 to 1 in PCMSK0 to enable PCINT4
    PCMSK0|=(1<<PCINT4);
}


/**
 * Function Counts_Left returns the number of counts from the left encoder.
 * @return [int32_t] The count number.
 */
int32_t Counts_Left()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE
    // Note: Interrupts can trigger during a function call and an int32 requires
    // multiple clock cycles to read/save. You may want to stop interrupts, copy the value,
    // and re-enable interrupts to prevent this from corrupting your read/write.
    
    //Initialize the count
    int32_t i=0;
    //Disable the global interrupt (IF WE HAVE PROBLEMS ANYWHERE, IT’S PROBABLY HERE)
    __disable_interrupt();
    //Store the number of counts
    i=_left_counts;
    //Restore interrupts
    GlobalInterruptEnable();
    return i;
}

/**
 * Function Counts_Right returns the number of counts from the right encoder.
 * @return [int32_t] The count number.
 */
int32_t Counts_Right()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE
    // Note: Interrupts can trigger during a function call and an int32 requires
    // multiple clock cycles to read/save. You may want to stop interrupts, copy the value,
    // and re-enable interrupts to prevent this from corrupting your read/write.

    //Initialize the count
    int32_t i=0;
    //Disable the global interrupt (IF WE HAVE PROBLEMS ANYWHERE, IT’S PROBABLY HERE)
    __disable_interrupt();
    //Store the number of counts
    i=_right_counts;
    //Restore interrupts
    GlobalInterruptEnable();
    return i;
}

/**
 * Function Rad_Left returns the number of radians for the left encoder.
 * @return [float] Encoder angle in radians
 */
float Rad_Left()
{

static float countsPerRevMotorShaftL = 12;
static float: gearRatioL = 75.81;
float countsPerRevSprocketL;
countsPerRevSprocketL = (countsPerRevMotorShaftL * gearRatioL);
float radSprocketPerCountL;
radSprocketPerCountL = 2*(3.14150) / (countsPerRevSprocketL);
float numRadL;
numRadL = (Counts_Left()) * radSprocketPerCountL;
return numRadL;

}

/**
 * Function Rad_Right returns the number of radians for the left encoder.
 * @return [float] Encoder angle in radians
 */
float Rad_Right()
{
static float countsPerRevMotorShaftR = 12;
static float: gearRatioR = 75.81;
float countsPerRevSprocketR;
countsPerRevSprocketR = (countsPerRevMotorShaftR * gearRatioR);
float radSprocketPerCountR;
radSprocketPerCountR = 2*(3.14150) / (countsPerRevSprocketR);
float numRadR;
numRadL = (Counts_Right()) * radSprocketPerCountR;
return numRadR;
}

/**
 * Interrupt Service Routine for the left Encoder. Note: May need to check that it is actually PCINT4 that triggered, as
 * the Pin Change Interrupts can trigger for multiple pins.
 * @return
 */
ISR(PCINT0_vect,ISR_BLOCK)
{
    /*Create the booleans Current_A and Current_B to get the current values of the encoder*/
    bool Current_A=Left_A();
    bool Current_B=Left_B();
    //Increment _left_counts by the number of counterclockwise turns and decrement it by the number of clockwise turns
    _left_counts+=-(_last_left_B^Current_A)+(_last_left_A^Current_B);
    //Set the previous values equal to the current ones
    _last_left_B=Current_B;
    _last_left_A=Current_A;
}


/**
 * Interrupt Service Routine for the right Encoder.
 * @return
 */
ISR(INT6_vect,ISR_BLOCK)
{
    /*Create the booleans Current_A and Current_B to get the current values of the encoder*/
    bool Current_A=Right_A();
    bool Current_B=Right_B();
    //Increment _right_counts by the number of clockwise turns and decrement it by the number of counterclockwise turns
    _right_counts+=(_last_right_B^Current_A)-(_last_right_A^Current_B);
    //Set the previous values equal to the current ones
    _last_right_B=Current_B;
    _last_right_A=Current_A;
}
