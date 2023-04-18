#include "eeprom.h"
#include "uart.h"
#include "sfr_defs.h"

#define EEARH  *((volatile char*) 0x42)
#define EEARL  *((volatile char*) 0x41)
#define EEDR  *((volatile char*) 0x40)
#define EECR  *((volatile char*) 0x3F)
#define SREG    *((volatile char*) 0x5F)
#define SPMCSR *((volatile char*) 0x57)

//#define __SFR_OFFSET 0
//#define _SFR_IO16(io_addr) ((io_addr) + __SFR_OFFSET)

#define EEAR _SFR_IO16(0x21)

//#define EEARL _SFR_IO8(0x21)
#define EEAR0 0
#define EEAR1 1
#define EEAR2 2
#define EEAR3 3
#define EEAR4 4
#define EEAR5 5
#define EEAR6 6
#define EEAR7 7

//#define EEARH _SFR_IO8(0x22)
#define EEAR8 0
#define EEAR9 1

#define I_SREG 7

#define EEPM1 5
#define EEPM0 4
#define EERIE 3         //EEPROM ready interrupt enable
#define EEMPE 2         //EEPROM master write enable
#define EEPE 1          //EEPROM write enable
#define EERE 0
#define SELFPRGEN 0

volatile unsigned char write_busy=0;      //Set to 1 if there is data in the write buffer that needs to be written. Cleared to 0 by the interrupt service routine when the last byte of the data has been written.
unsigned char writebuf[64];   //A 64 byte write buffer used by writebuf() and the interrupt service routine.
unsigned char bufidx;           //The index of the next writebuf character the interrupt service routine should write.
unsigned char writesize;        //The size of the data (in bytes) within the write buffer that must be written.
unsigned int writeaddr;         //Used by the interrupt service routine, specifies the next EEPROM address with which to write the data to.

void EEPROM_set_write_mode();

#pragma GCC push_options
#pragma GCC optimize ("Os")
void eeprom_unlock()
{
    //EECR &= ~(1<<EEPE);
    EECR |= (1<<EEMPE);                     //enable master write
    EECR |= (1<<EEPE);                      //enable write
}
#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize ("Os")
void eeprom_readenable()
{
    EECR |= (1<<EERE);
}
#pragma GCC pop_options

int eeprom_isbusy(){
    //if(write_busy==1)
    //{
    //    int i=0;
    //}
    return write_busy==1;
    //return 0;
}

void eeprom_writebuf(unsigned int addr, unsigned char* buf, unsigned char size){
    writeaddr = addr;
    write_busy = 1;
    bufidx = 0;

    //Copying
    for (int i=0;i<size;i++)
    {
        writebuf[i] = buf[i];
    }

    //uart_writestr(writebuf);


    writesize = size;

    //init EEPROM
    EECR |= (1<<EEPM1);
    EECR &= ~(1<<EEPM0);

    //EEARH &= ~(1<<EEAR8);

    SREG |= (1<<I_SREG);        //Global interrupt enable
    EECR |= (1<<EERIE);         //Enabling EEPROM ready interrupt

    while(write_busy);
}

void eeprom_readbuf(unsigned int addr, unsigned char* buf, unsigned char size)
{
    unsigned int read_addr=addr;
    for(int i=0;i<size;i++)
    {
        //Read from EEPROM and store in buf[0:size-1]
        while(EECR & (1<<EEPE));     //waiting until EEPE becomes 0

        EEAR = read_addr;

        eeprom_readenable();

        //Wait for 4 cycles
        __builtin_avr_delay_cycles(4);

        buf[i] = EEDR;
        read_addr++;
    }
}

void __vector_22(void) __attribute__ ((signal, used, externally_visible));

void __vector_22(){
    //EEPROM ready
    if(bufidx<writesize)
    {
        //Clear the global interrupt flag
        SREG &= ~(1<<I_SREG);

        //Write writebuf[bufidx] to EEPROM writeaddr

        while (EECR & (1<<EEPE));     //waiting until EEPE becomes 0

        //while (SPMCSR & (1<<SELFPRGEN));     //waiting until SELFPRGEN becomes 0

        EEAR = writeaddr;                      //Writes address to EEARL register
        EEDR = writebuf[bufidx];                //Writes data to EEDR register

        eeprom_unlock();

        writeaddr++;
        bufidx++;

        //Set the interrupt flag
        //SREG |= (1<<I_SREG);

        //Re-enable eeprom ready interrupt (just in case)
        //EECR |= (1<<EERIE);

        //while(EECR & (1<<EEPE)); //wait until EEPE becomes 0
    }
    else if (bufidx>=writesize)
    {
        //Disable EEPROM ready interrupts
        EECR &= ~(1<<EERIE);
        write_busy=0;
    }
}

void EEPROM_set_write_mode(){
    EECR |= (1<<EEPM1);
}




