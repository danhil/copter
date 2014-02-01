/*********************************************************************
 *
 *    PIC32MX WDT Example
 *
 *********************************************************************
 * FileName:        wdt_basic.c
 * Dependencies:    plib.h
 *
 *
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE vxx
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *********************************************************************
 * $Id: reset_basic.c 9390 2008-06-16 23:43:04Z rajbhartin $
 *********************************************************************
 * The purpose of this example code is to demonstrate the use of
 * peripheral library macros and functions supporting the PIC32MX
 * Watchdog Timer.
 *
 * Platform: Explorer-16 with PIC32MX PIM
 *
 * Features demonstrated:
 *      - Servicing the WDT event flag
 *      - Enabling / disabling the WDT
 *      - Using peripheral library functions that support the WDT
 *      - Interrupts
 *
 * Description:
 *      The right-most LED on the Explorer will repeatedly blink as long
 *      as the WDT is being serviced. When the right most switch on
 *      the Explorer is pressed, it executes a simple delay routine.
 *      This delay causes the WDT to fail to be serviced in time,
 *      triggering the WDT event flag and resetting the system. The
 *      left-most LED on the explorer will then blink forever to
 *      indicate a timeout reset has occured.
 *
 * Notes:
 *     - PIC32MX 2xx PIMS are unconnected to the Explorer-16 LEDs and 
 *       switches. They must be soldered to the test points on top of
 *       the PIM for proper functionality. The README file contains a
 *       list of the connections that need to be made.
 *     - Debugger operation may interfere with WDT servicing.  This example
 *       may not function as expected while a debugger is in operation.
 ********************************************************************/
#include <plib.h>

#if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__) || (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 80 MHz (SYSCLK / FPBDIV)
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1
#define SYS_FREQ (80000000L)

#elif defined (__32MX220F032D__) || (__32MX250F128D__)
// Configuration Bit settings
// SYSCLK = 48 MHz (8MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 48 MHz (SYSCLK / FPBDIV)
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
#pragma config FPLLMUL = MUL_24, FPLLIDIV = DIV_2, FPLLODIV = DIV_2, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1
#define SYS_FREQ (48000000L)
#endif

// SW4 is connected to PORTD, Bit 13
// which uses change notification input CN19
#if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__)
#define CONFIG          (CN_ON | CN_IDLE_CON)
#define PINS            (CN19_ENABLE)
#define PULLUPS         (CN19_PULLUP_ENABLE)
#define INTERRUPT       (CHANGE_INT_ON | CHANGE_INT_PRI_1)

// SW4 is connected to PORTD, Bit 13
#elif defined (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
#define CONFIG         (CND_ON | CND_IDLE_CON)
#define PINS           (CND13_ENABLE)
#define PULLUPS        (CND13_PULLUP_ENABLE)
#define INTERRUPT      (CHANGE_INT_ON | CHANGE_INT_PRI_1)

// SW4 is connected to PORTC, Bit 0 (by soldering, instructions in README)
#elif defined (__32MX220F032D__) || (__32MX250F128D__)
#define CONFIG         (CNC_ON | CNC_IDLE_CON)
#define PINS           (CNC0_ENABLE)
#define PULLUPS        (CNC0_PULLUP_ENABLE)
#define INTERRUPT      (CHANGE_INT_ON | CHANGE_INT_PRI_1)
#endif

#pragma config WDTPS = PS1 // WDT timeout period = 1ms

int main(void)
{
    unsigned int temp;

    // Configure the device for maximum performance but do not change the PBDIV
    // Given the options, this function will change the flash wait states, RAM
    // wait state and enable prefetch cache but will not change the PBDIV.
    // The PBDIV value is already set via the pragma FPBDIV option above
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);


    // Explorer-16 LEDs are on lower 8-bits of PORTA and to use all LEDs, JTAG port must be disabled.
    mJTAGPortEnable(DEBUG_JTAGPORT_OFF);

    // Clear PORTA bits so there are no unexpected flashes when setting
    // them to output in the next step
    mPORTAClearBits(BIT_7 | BIT_0);

    // Make all lower 8-bits of PORTA as output
    mPORTASetPinsDigitalOut(BIT_7 | BIT_0);

    #if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__)|| (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    // Set RD13 (switch S4, the right-most switch on Explorer 16) as input
    mPORTDSetPinsDigitalIn(BIT_13);
    #elif defined (__32MX220F032D__) || (__32MX250F128D__)
    // Set lowest bit of PORTC to input (for the switch)
    mPORTCSetPinsDigitalIn(BIT_0);
    #endif

    // Enable change notice, enable discrete pins and weak pullups
    #if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__)
    mCNOpen(CONFIG, PINS, PULLUPS);
    #elif defined (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    mCNDOpen(CONFIG, PINS, PULLUPS);
    #elif defined (__32MX220F032D__) || (__32MX250F128D__)
    mCNCOpen(CONFIG, PINS, PULLUPS);
    #endif

    // Read port to clear mismatch on change notice pins
    #if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__) || (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    temp = mPORTDRead();
    #elif defined (__32MX220F032D__) || (__32MX250F128D__)
    temp = mPORTCRead();
    #endif

    // Enable change notice interrupt
    #if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__)
    INTEnable(INT_CN, INT_ENABLED);
    #elif defined (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    INTEnable(INT_CND, INT_ENABLED);
    #elif defined (__32MX220F032D__) || (__32MX250F128D__)
    INTEnable(INT_CNC, INT_ENABLED);
    #endif

    // Set priority levels
    INTSetVectorPriority(INT_CHANGE_NOTICE_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_CHANGE_NOTICE_VECTOR, INT_SUB_PRIORITY_LEVEL_0);

    // Enable multi-vector interrupts
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    // Check for a previous WDT Event.
    // The program won't enter this loop until the first switch on Explorer-16
    // is pressed, causing a change notice interrupt to occur. The interrupt
    // executes a short delay routine which causes the WDT to not be serviced
    // in time, resetting the system and setting the WDT event flag.
    if (ReadEventWDT())
    {
        // A WDT event did occur
        DisableWDT();

        // Clear the WDT event flag so a subsequent event can set the event bit
        ClearEventWDT();

        // Blink left-most LED ON/OFF forever to indicate that we have seen WDT timeout reset.
        while (1)
        {
            int i;

            mPORTAToggleBits(BIT_7);

            // Insert some delay
            i = 1024 * 1024;
            while (i--);
        }
    }

    // WDT timeout period is set in the Device Configuration (WDTPS)
    EnableWDT(); // Enable the WDT

    // If a WDT event has not occured, blink LED0 to indicate everything is
    // running smoothly and the WDT is being serviced on time.
    while (1)
    {
        int i;

        mPORTAToggleBits(BIT_0);

        // Insert some delay
        i = 1024 * 1024;
        while (i--)
        {
            // User code
            ClearWDT(); // Service the WDT
        }
    }
}

// Configure the CN interrupt handler
void __ISR(_CHANGE_NOTICE_VECTOR, ipl2) ChangeNotice_Handler(void)
{
    unsigned int temp;
    int i;

    // Read port to clear mismatch on change notice pins
    #if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__) || (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    temp = mPORTDRead();
    #elif defined (__32MX220F032D__) || (__32MX250F128D__)
    temp = mPORTCRead();
    #endif

    // Clear the interrupt flag
    #if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__)
    INTClearFlag(INT_CN);
    #elif defined (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    INTClearFlag(INT_CND);
    #elif defined (__32MX220F032D__) || (__32MX250F128D__)
    INTClearFlag(INT_CNC);
    #endif

    // Short delay causing the WDT to fail to be serviced in time
    i = 1024 * 100;
    while (i--);
}


