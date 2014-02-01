	Readme File for Code Example:
--------------------------------------------------

PIC32_SPI\loopback

--------------------------------------------------
Code Example Description:
--------------------------------------------------
Examples for the usage of the SPI Peripheral Lib for in a simple loopback mode.

This test assumes that the SPI SDO output is connected to the SDI input. Inits the SPI channel 1 to use 16 bit words. Performs the device initialization in both master/slave modes. 

--------------------------------------------------
Suggested Development Resources:
--------------------------------------------------
Explorer 16 demo board with 

Processors:	PIC32MX###F512L
		       360
		       460
		       795
            PIC32MX220F032D (some LEDs not supported on Explorer 16)
		PIC32MX250F128D (some LEDs not supported on Explorer 16)

Complier:	MPLAB C32 v1 or higher

IDE:
		MPLAB IDE v8 or higher
            MPLAB X IDE

--------------------------------------------------
Further Documentation:
--------------------------------------------------
For more information on this topic see Family Reference Manual (FRM)

FRM PDF can be found on Microchip website by searching for: 
"PIC32 Family Reference Manual, Sect. 23 Serial Peripheral Interface.pdf"