#include "delay.h"
#include "led.h"
#include "timer1.h"
#include "rtc.h"
#include "uart.h"

int main_proj1(void) {
    /* Initialize uart, led, and rtc hardware */
    uart_init();
    led_init();
    rtc_init();
    timer1_init();
    delay_init();

    /* Initialize led blink pattern to "--- -.-" in Morse Code */
    led_set_blink("-");

    /* Set RTC date/time to "01/01/2019 00:00:00" */
    rtc_set_by_datestr("01/01/2019 00:00:00");

    /* Output project information to the console */
    uart_writestr("SER 486 Project 1 - Manish Mysore Rajesh Gangadhar\n\r");

    /* Profile the led_update() function */
    signed long c1 = 0;
    delay_set(1, 1);
    while (!delay_isdone(1)) {
        c1++;
    }

    signed long c2 = 0;
    delay_set(1, 1);
    while (!delay_isdone(1)) {
        led_update();
        c2++;
    }

    /* Calculate average time spent in led_update() function */

    /*

    long Ttotal = 10000;
    long Nled = c2;
    long Nbaseline = c1;
    long Tled_update = Ttotal * (1.0/Nled - 1.0/Nbaseline);

    */

    /* Display baseline and led_update() performance */
    uart_writedec32(c1);
    uart_writestr(" ");
    uart_writedec32(c2);
    uart_writestr("\r\n");

    /* Main loop */
    while (1) {
        /* Update led blink FSM */
        led_update();

        /* Display RTC date string every 500ms */
        delay_set(1, 500);
        while(!delay_isdone(1));
        char date_str[20];
        char* p = date_str;

        /*

        unsigned long date_num;
        date_num = rtc_get_date();

        */

        p = rtc_get_date_string();
        uart_writestr(p);

        uart_writestr("\r");
    }
}
