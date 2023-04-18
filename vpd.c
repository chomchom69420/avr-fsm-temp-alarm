#include "vpd.h"
#include "eeprom.h"
#include "util.h"


/*
*   x1 = 0x4D (M) & 0xFE = 0x4C
*   x2 = 0x41 (A)
*   x3 = 0x4E (N)
*   y1 = 0x4D (M)
*   y2 = 0x41 (A)
*   y3 = 0x4E (N)
*/

//vpd_struct vpd_defaults = {"SER", "Manish", "Mani", "abcd1234", 0, {0x4C,0x41, 0x4E, 0x4D, 0x41, 0x4E}, "USA", 0};

//For the other guy --> delete this and the following comments

/*
*   x1 = 0x56 (V) & 0xFE = 0x56
*   x2 = 0x45 (E)
*   x3 = 0x4E (N)
*   y1 = 0x4B (K)
*   y2 = 0x41 (A)
*   y3 = 0x4E (N)
*/


vpd_struct vpd_defaults = {"SER", "Venkata", "Kanaka", "pqrs1234", 0, {0x56,0x45, 0x4E, 0x4B, 0x41, 0x4E}, "USA", 0};

int compare(char a[],char b[])
{
    int flag=0,i=0;  // integer variables declaration
    while(a[i]!='\0' &&b[i]!='\0')  // while loop
    {
       if(a[i]!=b[i])
       {
           flag=1;
           break;
       }
       i++;
    }
    if(flag==0)
    return 0;
    else
    return 1;
}

void vpd_init()
{
    while(eeprom_isbusy());  //waits for EEPROM to get out of busy state

    //Read 43 bytes from 0x000
    unsigned int base_addr=0x000;
    int size = sizeof(vpd_struct);
    unsigned char buf[size+10];
    unsigned char* p = (unsigned char*)&vpd;


    eeprom_readbuf(base_addr, (unsigned char*)buf, size);
    //memcpy((unsigned char*)&vpd, (unsigned char*)buf, size);

    for(int i=0;i<size;i++)
    {
        p[i]=buf[i];
    }


    //eeprom_readbuf(base_addr, (unsigned char*)&vpd, size);

    if(!vpd_is_data_valid())
    {
        vpd_write_defaults();

        //Initialize data by reading default values
        eeprom_readbuf(base_addr, (unsigned char*)buf, size);
        //memcpy((unsigned char*)&vpd, buf, size);
        for(int i=0;i<size;i++)
        {
            p[i]=buf[i];
        }

    }
}

int vpd_is_data_valid()
{
    return compare(vpd.token, vpd_defaults.token) && is_checksum_valid((unsigned char*)&vpd, sizeof(vpd_struct));
}

void vpd_write_defaults()
{
    update_checksum((unsigned char*)&vpd, sizeof(vpd));

    //Writing defaults to eeprom
    unsigned int base_addr = 0x000;
    int size = sizeof(vpd_struct);
    //unsigned char buf[size+10];
    unsigned char* p = (unsigned char*)&vpd_defaults;

    //memcpy((unsigned char*)buf, (unsigned char*)&vpd_defaults, size);
    eeprom_writebuf(base_addr, p, size);


    //eeprom_writebuf(base_addr, (unsigned char*)&vpd, size);
}



