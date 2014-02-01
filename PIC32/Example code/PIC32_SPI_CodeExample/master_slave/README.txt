	Readme File for Code Example:
--------------------------------------------------

PIC32_SPI\..

--------------------------------------------------
Code Example Description:
--------------------------------------------------
Examples for the usage of the SPI Peripheral Lib for in a simple master/slave transfer mode

This test uses both SPI channels.
The master channel (SPI1) sends data to a slave device (SPI2).
The slave device relays data back to the master.
This way we can verify that the connection to the slave is ok.
Hardware connections have to be made:
	- SCK1 <-> SCK2
	- SDO1 <-> SDI2
	- SDI1 <-> SDO2
	- SS1  <-> SS2 (needed only if we use the framed mode)

Inits the SPI channel 1 to use 16 bit words Performs the device initialization in both master/slave modes.

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