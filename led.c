#include "delay.h"
#include "led.h"
#include <string.h>

#define LED_PIN (1 << 1)

#define PORTB   (*((volatile unsigned char *)0x25))
#define DDRB    (*((volatile unsigned char *)0x24))

const char *Blink_msg;
unsigned int Blink_pos;
enum Blink_state { phase1, phase2 } blink_state;

void led_init() {
    DDRB |= LED_PIN; // Set LED pin as output
    PORTB &= ~LED_PIN; // Turn off the LED
}

void led_on() {
    PORTB |= LED_PIN;
}

void led_off() {
    PORTB &= ~LED_PIN;
}

int led_is_on() {
    return (PORTB & LED_PIN) ? 1 : 0;
}

void led_set_blink(char *msg) {
    Blink_msg = msg;
    Blink_pos = 0;
    blink_state = phase1;
    delay_set(0, 0);
    led_off();
}

void led_update() {
    switch (blink_state) {
        case phase1:
            if (delay_isdone(0)) {
                switch (Blink_msg[Blink_pos]) {
                    case '-':
                        led_on();
                        delay_set(0, 750);
                        //delay_set(0,7);
                        blink_state = phase2;
                        break;
                    case '.':
                        led_on();
                        delay_set(0, 250);
                        //delay_set(0,2);
                        blink_state = phase2;
                        break;
                    case ' ':
                        led_off();
                        delay_set(0, 1000);
                        //blink_state = phase2;
                        break;
                    default:
                        led_off();
                        delay_set(0, 0);
                        break;
                }
            }
            break;

        case phase2:
            if (delay_isdone(0)) {
                led_off();
                delay_set(0, 100);
                //delay_set(0, 1);
                Blink_pos++;
                if (Blink_pos >= strlen(Blink_msg)) {
                    Blink_pos = 0;
                }
                blink_state = phase1;
            }
            break;

        default:
            break;
    }
}

