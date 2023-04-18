#include "delay.h"
//This function uses timer0

#define TCCR0A  *((volatile char*) 0x44)
#define TCCR0B  *((volatile char*) 0x45)
#define OCR0A   *((volatile char*) 0x47)
#define TIMSK0  *((volatile char*) 0x6E)
#define SREG    *((volatile char*) 0x5F)

#define TIMER1_COMPA_vect _VECTOR(11)  /* Timer/Counter1 Compare Match A */

#define COM0A1 7
#define COM0A0 6

#define WGM01 1
#define WGM00 0

#define CS00 0
#define CS01 1
#define CS02 2

#define OCIE0A 1

#define I_SREG 7

#define F_CPU 16000000UL
#define CPU_FREQUENCY 16000000UL

static volatile unsigned int delay_count[2] = {0, 0};
static unsigned int limit[2] = {0, 0};
static unsigned char initialized = 0;

//typedef void (*func_t)(void);

void delay_init() {
    if (!initialized) {
        // Set timer0 compare for 1ms tick
        // Calculation: (CPU_FREQUENCY / PRESCALER_VALUE / COMPARE_VALUE) = 1000 Hz (1ms)
        // Example: (16,000,000 / 64 / 250) = 1000 Hz
        /*

        unsigned int PRESCALER_VALUE = 64;
        */

        unsigned int COMPARE_VALUE = 250;



        OCR0A = COMPARE_VALUE-1; // Set timer0 compare value to 250-1 = 249

        // Set CTC mode and correct clock divisor
        TCCR0A |= (1 << WGM01); // Set CTC mode //WGM01
        TCCR0B |= (1 << CS01) | (1 << CS00); // Set clock divisor (prescaler value = 64)  CS00/CS01

        // Enable global interrupts
        // __builtin_avr_sei();
        SREG |= (1<<I_SREG);

        // Enable interrupts on output compare A
        TIMSK0 |= (1 << OCIE0A);  //OCIE0A

        // Stop further initialization by setting initialized to 1
        initialized = 1;
    }
}


unsigned int delay_get(unsigned int num) {

    unsigned int count_value;

    // Get global interrupt enable bit state
    unsigned int sreg_state = SREG & (1 << I_SREG);

    // Disable interrupts
    // __builtin_avr_cli(); //
    SREG &= ~(1<<I_SREG);

    // Get the delay_count[num] value
    count_value = delay_count[num];

    // Restore global interrupt state
    if (sreg_state) {
        SREG |= (1<<I_SREG); //Enable interrupts
    }

    // Return the count value
    return count_value;
}


void delay_set(unsigned int num, unsigned int time) {

    // If not initialized, initialize the delay counter
    if (!initialized) {
        delay_init();
    }
    // Get global interrupt enable bit state
    unsigned int sreg_state = SREG & (1 << 7);

    // Disable Interrupts
    SREG &= ~(1<<I_SREG); //cli()

    // Set the limit for delay[num] and clear the count for delay[num]
    limit[num] = time;
    delay_count[num] = 0;

    // Restore global interrupt state
    if (sreg_state) {
        SREG |= (1<<I_SREG);   //sei()
    }
}


unsigned int delay_isdone(unsigned int num) {
    unsigned int result = 0;

    // If delay_count[num] equals limit[num], set result to 1
    if (delay_count[num] == limit[num]) {
        result = 1;
    }

    // Return the result
    return result;
}

//map to ivt

void __vector_14(void) __attribute__ ((signal, used, externally_visible));

void __vector_14(void) {
    for (unsigned int i = 0; i < sizeof(delay_count) / sizeof(delay_count[0]); i++) {
        if (delay_count[i] < limit[i]) {
            delay_count[i]++;
        }
    }
}

//ISR(TIMER0_COMPA_vect){
//    __vector_14();
//}
