#include "wdt.h"
#include "log.h"
#include "config.h"
#include "led.h"

#define MCUSR  *((volatile char*) 0x55)
#define WDTCSR *((volatile char*) 0x60)
#define SREG   *((volatile char*) 0x5F)

#define WDRF 3
#define I_SREG 7
#define WDIF 7
#define WDIE 6
#define WDP3 5
#define WDCE 4
#define WDE 3
#define WDP2 2
#define WDP1 1
#define WDP0 0

#pragma GCC push_options
#pragma GCC optimize ("Os")
void wdt_init()
{
    //Clear WDRF in MCUSR
    //MCUSR &= ~(1<<WDRF);

    //Setting the pre-scalar for timeout period of 2s
    WDTCSR |= (1<<WDP0) | (1<<WDP1) | (1<<WDP2);

    //Interrupt and system reset mode
    WDTCSR |= (1<<WDIE) | (1<<WDE);

    //Initizaize global interrupts
    SREG |= (1<<I_SREG);
}
#pragma GCC pop_options

void wdt_reset()
{
    __builtin_avr_wdr();

    //Clear WDRF in MCUSR
    //MCUSR &= ~(1<<WDRF);
}

#pragma GCC push_options
#pragma GCC optimize ("Os")
void wdt_force_restart()
{
    //disables interrupt
    SREG &= ~(1<<I_SREG);

    //Enabling change
    WDTCSR |= (1<<WDCE);

    //Clearing WDIE
    WDTCSR &= ~(1<<WDIE);

    //Wait for reset
    while(1);
}
#pragma GCC pop_options

void __vector_6(void) __attribute__ ((signal, used, externally_visible));

void __vector_6(void)
{
    //Turns on LED
    led_on();

    //Add EVENT_WDT to system event log for wdt timeout
    log_add_record(EVENT_WDT);

    //Write modified log entries to EEPROM
    for(int i=0;i<16;i++)
        log_update_noisr();

    //Write modified config data to EEPROM
    config_update_noisr();

    //Set the interrupt
    //WDTCSR |= (1<<WDIE);
}


