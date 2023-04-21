#include "tempfsm.h"
#include "alarm.h"
#include "led.h"
#include "log.h"

enum Temp_State { NORM1, NORM2, NORM3, WARN_HI1, WARN_HI2, CRITICAL_HI, WARN_LO1, WARN_LO2, CRITICAL_LO} temp_state;

void tempfsm_init()
{
    temp_state = NORM1;
}

void tempfsm_reset()
{
    temp_state = NORM1;
}

void tempfsm_update(int current, int hicrit, int hiwarn, int locrit, int lowarn)
{
    switch(temp_state)
    {
    case NORM1:
        if(current >= hiwarn)
        {
            alarm_send(EVENT_HI_WARN);
            log_add_record(EVENT_HI_WARN);
            led_set_blink("-");     //Next state is a warning state
            led_update();
            temp_state = WARN_HI1;
        }
        if (current<= lowarn)
        {
            alarm_send(EVENT_LO_WARN);
            log_add_record(EVENT_LO_WARN);
            led_set_blink("-");     //Next state is a warning state
            led_update();
            temp_state = WARN_LO1;
        }
        break;

    case WARN_HI1:
        if(current >= hicrit)
        {
            alarm_send(EVENT_HI_ALARM);
            log_add_record(EVENT_HI_ALARM);
            led_set_blink(".");     //Next state is a critical state
            led_update();
            temp_state = CRITICAL_HI;
        }
        if(current < hiwarn)
        {
            led_set_blink(" ");     //Next state is a normal state
            led_update();
            temp_state = NORM3;
        }
        break;

    case WARN_LO1:
        if(current<=locrit)
        {
            alarm_send(EVENT_LO_ALARM);
            log_add_record(EVENT_LO_ALARM);
            led_set_blink(".");     //Next state is a critical state
            led_update();
            temp_state = CRITICAL_LO;
        }
        if(current > lowarn)
        {
            led_set_blink(" ");     //Next state is a normal state
            led_update();
            temp_state = NORM2;
        }
        break;

    case NORM3:
        if(current>=hiwarn)
        {
            alarm_send(EVENT_HI_WARN);
            log_add_record(EVENT_HI_WARN);
            led_set_blink("-");     //Next state is a warning state
            led_update();
            temp_state = WARN_HI1;
        }
        if (current<=lowarn)
        {
            alarm_send(EVENT_LO_WARN);
            log_add_record(EVENT_LO_WARN);
            led_set_blink("-");     //Next state is a warning state
            led_update();
            temp_state = WARN_LO1;
        }
        break;

    case NORM2:
        if(current>=hiwarn)
        {
            alarm_send(EVENT_HI_WARN);
            log_add_record(EVENT_HI_WARN);
            led_set_blink("-");     //Next state is a warning state
            led_update();
            temp_state = WARN_HI1;
        }
        if (current<=lowarn)
        {
            alarm_send(EVENT_LO_WARN);
            log_add_record(EVENT_LO_WARN);
            led_set_blink("-");     //Next state is a warning state
            led_update();
            temp_state = WARN_LO1;
        }
        break;

    case CRITICAL_LO:
        if(current>locrit)
        {
            led_set_blink("-");     //Next state is a warning state
            led_update();
            temp_state = WARN_LO2;
        }
        break;

    case CRITICAL_HI:
        if(current<hicrit)
        {
            led_set_blink("-");     //Next state is a warning state
            led_update();
            temp_state = WARN_HI2;
        }
        break;

    case WARN_LO2:
        if(current <= locrit)
        {
            led_set_blink(".");     //Next state is a critical state
            led_update();
            temp_state = CRITICAL_LO;
        }
        if(current > lowarn)
        {
            led_set_blink(" ");     //Next state is a normal state
            led_update();
            temp_state = NORM2;
        }
        break;

    case WARN_HI2:
        if(current < hiwarn)
        {
            led_set_blink(" ");     //Next state is a normal state
            led_update();
            temp_state = NORM3;
        }
        if(current >= hicrit)
        {
            led_set_blink(".");     //Next state is a critical state
            led_update();
            temp_state = CRITICAL_HI;
        }
        break;

    }

}
