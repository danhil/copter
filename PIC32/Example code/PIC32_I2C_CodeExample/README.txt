	Readme File for Code Example:
--------------------------------------------------

PIC32_I2C\i2c_master

--------------------------------------------------
Code Example Description:
--------------------------------------------------
This example interfaces with a 24LC256 serial eeprom on I2C channel 1. The code writes to a location and then verifies the contents of the eeprom. I2C Channel 1 is
hooked up to 24LC256, while the address lines of the eeprom are all tied to Vss.

--------------------------------------------------
Suggested Development Resources:
--------------------------------------------------
Complier:	MPLAB C32 v1 or higher
		MPLAB XC32 

IDE:		MPLAB IDE v8 or higher
            	MPLAB X IDE

--------------------------------------------------
Further Documentation:
--------------------------------------------------
For more information on this topic see the Family Reference Manual (FRM)

The FRM PDF can be found on the Microchip website by searching for: 
"PIC32 Family Reference Manual, Sect. 24 Inter-Integrated Circuit"