/*********************************************************************
 *
 *                  OCMP Simple PWM Application
 *
 *********************************************************************
 * FileName:        ocmp_simple_pwm.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE
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
 *
 *
 * $Id: ocmp_simple_pwm.c 7587 2008-01-10 18:16:01Z rajbhartin $
 * $Name: x.x $
 *
 *********************************************************************
 *
 * This Program Demonstrates continuous Pulse generation on
 * Output compare module1 of the PIC32 controller.
 *
 * The H/W setup contains PIC32 placed on an Explorer16 board.
 * Observe the output pulse on OC1 pin using an Oscilloscope.
 *
 **********************************************************************/
#include <plib.h>

// Will use default osc mode with WDT off
// Other options are don't care for this example
//
#pragma config FWDTEN = OFF

int main(void)
{


    /* Peripheral Pin Remapping */
    #if defined(__32MX220F032D) || defined (__32MX250F128D)
    RPB4Rbits.RPB4R = 0b0101;
    #endif

	/* Open Timer2 with Period register value */
	OpenTimer2(T2_ON, 0x550);

	/* Enable OC | 32 bit Mode  | Timer2 is selected | Continuous O/P   | OC Pin High , S Compare value, Compare value*/
	OpenOC1( OC_ON | OC_TIMER_MODE32 | OC_TIMER2_SRC | OC_CONTINUE_PULSE | OC_LOW_HIGH , 0x550, 0x500 );
	/*
	  The expected output looks like the diagram below with approximately 6% duty-cycle

           ||      ||      ||
	 ______||______||______||______

	*/
	while(1); /* can have user code here */

	/* Close Output Compare */
	CloseOC1();

	return (0);
}