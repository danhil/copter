#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>
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
	unsigned char buf[3];

	if (!bcm2835_init())
	{
		printf("Cound not init Rpid gpio");
		return 1;
	}

	bcm2835_gpio_fsel(LED1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(SW1, BCM2835_GPIO_FSEL_INPT);
	//Input pull-up
	bcm2835_gpio_set_pud(SW1, BCM2835_GPIO_PUD_UP);

	signal(SIGINT, signal_callback_handler);
	printf("press ^C to exit program ...\n");
	printf("connecting to the accessory");
	acc.connect_to_accessory();

	while(1){
		res = acc.read(buf, 2, 10);
		if(res > 0){
			printf("%d bytes rcvd : %02X %02X\n", res, buf[0], buf[1]);
			if(buf[0] == 0x01){
				if(buf[1] == 1){
					bcm2835_gpio_write(LED1, HIGH);
				}else{
					bcm2835_gpio_write(LED1, LOW);
				}
			}
		}else if(res == LIBUSB_ERROR_TIMEOUT ){
		}else{
			break;
		}

		buf[0] = 1;
		//read switch status
		uint8_t value = bcm2835_gpio_lev(SW1);
		if( value == 0 ){
			buf[1] = 1;
		}else{
			buf[1] = 0;
		}
		acc.write(buf, 2, 10);

	}
	bcm2835_close();
	return 0;
}

// Signal handeler for the interrupt signal
void signal_callback_handler(int signum)
{
	printf("\ndetect key interrupt\n",signum);
	bcm2835_close();
	printf("Program exit\n");
	exit(0);
}


