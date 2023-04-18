 #include "util.h"
 #include "timer1.h"
 #include "delay.h"
 #include "led.h"
 #include "rtc.h"
 #include "eeprom.h"
 #include "config.h"
 #include "vpd.h"
 #include "uart.h"
 #include "log.h"

int main(void)
{
    /* Initialize uart, led, and rtc hardware */
    uart_init();
    config_init();
    led_init();
    log_init();
    rtc_init();
    vpd_init();
    //timer1_init();
    delay_init();

    /* Initialize led blink pattern to "--- -.-" in Morse Code */
    led_set_blink("-");

    /* Set RTC date/time to "01/01/2019 00:00:00" */
    rtc_set_by_datestr("01/01/2019 00:00:00");

    /* Output project information to the console */
    //  uart_writestr("SER 486 Project 2 - Manish Mysore Rajesh Gangadhar\n\r");

    uart_writestr("SER 486 Project 2 - Venkata Kanaka Rama Meher Virinchi\n\r");

    uart_writestr(vpd.model);
    uart_writestr("\n\r");
    uart_writestr(vpd.manufacturer);
    uart_writestr("\n\r");
    uart_writestr(config.token);
    uart_writestr("\n\r");

    //Set the config.use_static_ip=1 and set modified
    config.use_static_ip=1;
    config_set_modified();

    //Clear event log and add three records to it
    log_clear();
    log_add_record(0xaa);
    log_add_record(0xbb);
    log_add_record(0xcc);

    log_update();
    log_update();
    log_update();

    int dumped=0;

    //uart_writehex8(config.checksum);
    //uart_writestr("\n\r");


    while(1)
    {
        led_update();
        log_update();
        config_update();

        if(!eeprom_isbusy() && (!dumped))
        {
            dump_eeprom(0, 0x100);
            dumped=1;
        }
    }

    return 0;
}
