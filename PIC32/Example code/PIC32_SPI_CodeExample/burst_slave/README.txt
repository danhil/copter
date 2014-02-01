	Readme File for Code Example:
--------------------------------------------------

PIC32_SPI\burst_slave

--------------------------------------------------
Code Example Description:
--------------------------------------------------
Examples for the usage of the SPI Peripheral Lib

This test assumes that a master device is connected to our test SPI device and that it sends us data packets. The slave device, after receiving the data from the master switches to master mode and relays data back to the master. The master can then verify the integrity of the packet. This is the SPI slave code. The slave has to be launched first and has to be waiting when the master begins transmitting data.

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