#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef RPI
#include <bcm2835.h>
#endif
#include "AndrOA.h"


// Define the Raspberry pi pins of interest
#define LED1 RPI_V2_GPIO_P1_11
#define SW1  RPI_V2_GPIO_P1_12

AndrOA acc("daniel.hilton",
        "RpiADK",
        "RpiDev",
        "0.0",
        "http://github.com/danhil",
        "0000000012345678") ;

void signal_callback_handler(int signum);

int main()
{
    printf("Inside Main");
    int res;
    int read_length = 8;
    int read_timeout;
    unsigned char buf[read_length];

#ifdef RPI
    if (!bcm2835_init())
    {
        printf("Cound not init Rpi gpio");
        return 1;
    }

    bcm2835_gpio_fsel(LED1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SW1, BCM2835_GPIO_FSEL_INPT);
    //Input pull-up
    bcm2835_gpio_set_pud(SW1, BCM2835_GPIO_PUD_UP);
#endif

    signal(SIGINT, signal_callback_handler);
    printf("press ^C to exit program ...\n");
    printf("connecting to the accessory");
    acc.connect_to_accessory();
    printf("connected to the accessory");
    // Protocol: 1B:type,2Blength;lengthB data;
    double output;
    while(1){
        res = acc.read(buf, read_length, read_timeout);
        if(res > 0){
            printf("%d bytes rcvd : %C %C %C %C\n", res, buf[0], buf[1], buf[2], buf[3]);
            const char * dest_buf = (const char *)buf;
            char *pNext;
            output = strtod (dest_buf, &pNext);
            printf("This is the recieved value: %f\n", output);
#ifdef RPI
            if(buf[0] == 0x01){
                if(buf[1] == 1){
                    bcm2835_gpio_write(LED1, HIGH);
                }else{
                    bcm2835_gpio_write(LED1, LOW);
                }
            }
#endif
        }else if(res == LIBUSB_ERROR_TIMEOUT ){
            printf("The USB connection timed out");
        }else{
            break;
        }

#ifdef RPI
        buf[0] = 1;
        //read switch status
        uint8_t value = bcm2835_gpio_lev(SW1);
        if( value == 0 ){
            buf[1] = 1;
        }else{
            buf[1] = 0;
        }
        // Transmit logical pin in status.
        acc.write(buf, 2, 10);
#endif
    }

#ifdef RPI
    bcm2835_close();
#endif
    return 0;
}

// Signal handeler for the interrupt signal
void signal_callback_handler(int signum)
{
    printf("\ndetect key interrupt\n",signum);
#ifdef RPI
    bcm2835_close();
#endif
    printf("Program exit\n");
    exit(0);
}


