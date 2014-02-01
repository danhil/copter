	Readme File for Code Example:
--------------------------------------------------

PIC32_WDT\wdt_basic

--------------------------------------------------
Code Example Description:
--------------------------------------------------
The purpose of this example code is to demonstrate the use of peripheral library macros and functions supporting the PIC32MX Watchdog Timer.

The first LED on the Explorer will repeatedly blink as long as the WDT is being serviced. When the right most switch on the Explorer is pressed, 
it executes a simple delay routine. This delay causes the WDT to fail to be serviced in time, triggering the WDT event flag and resetting the system. The
left-most LED on the explorer will then blink forever to indicate a timeout reset has occured.
--------------------------------------------------
Suggested Development Resources:
--------------------------------------------------
Explorer-16 demo board with 

Processors:	PIC32MX###F512L
		       360
		       460
		       795
		PIC32MX430F064L 
		PIC32MX450F256L 
		PIC32MX470F512L
            	PIC32MX220F032D (LEDs and switches must be soldered to the PIM)
		PIC32MX250F128D (LEDs and switches must be soldered to the PIM)
		

Complier:	MPLAB C32 v1 or higher
		MPLAB XC32 

IDE:		MPLAB IDE v8 or higher
            	MPLAB X IDE

--------------------------------------------------
Notes:
--------------------------------------------------
For PIC32MX 360/460/795/430 PIMS, Explorer-16 LEDs are on lower 8-bits of PORTA and to use all LEDs, JTAG port must be disabled.

For PIC32MX 220/250 PIMS, Explorer-16 LEDs and switches are unconnected and must be soldered to the test points on top of the PIM as stated below.

Debugger operation may interfere with WDT servicing.  This example may not function as expected while a debugger is in operation.

--------------------------------------------------
Soldering Instructions for PIC32MX 220/250 PIMS:
--------------------------------------------------
The test points (TP) are located on top of the PIM and must be connected as follows to the Explorer-16 for proper LED  and switch functionality.

TP19 to D3  (RA0 to LED0)
TP13 to D6  (RA7 to LED3)

TP25 to SW4 (RC0 to Switch 4)
