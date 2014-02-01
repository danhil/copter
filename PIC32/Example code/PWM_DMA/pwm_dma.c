/***********************************************************************************
	
			PIC32MX Starter Kit Example Code - PWM and DMA

 ***********************************************************************************
 * FileName:        pwm_dma.c
 * Dependencies:    None
 * Company:         Microchip Technology, Inc.
 *
 * Copyright (c) 2008 Microchip Technology, Inc.
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ***************************************************************************
 * Description:
 *			The example code demonstrates varying LED1 and LED2 intensity
 *          using DMA to update PWM outputs with no CPU overhead.
 *
 * Additional Info:
 *			Refer to readme.txt
 *
 * Platforms:
 *			PIC32MX Starter Kit DM320001
 *			PIC32MX USB Starter Kit DM320003
 *
 * Tools:
 *			1. MPLAB IDE 8.20 or higher
 *			2. MPLAB C Compiler for PIC32 v1.04 or higher
 *			3. General Purpose Starter Kit DM320001 or USB Starter board DM320003
 *			4. USB Cable
 *
 * Debug Print Library: SUPPORTED IN DEBUG MODE ONLY
 *			The Debug Print Library adds user debugging input/output capability
 *          to the MPLAB IDE Starter Kit console window using DBPRINTF, DPUTS,
 *          DBGETC and DBGETS.
 *
 *          To use the Debug Print Library with the PIC32 Starter Kit, do the following:
 *			1. Add '#include "db_utils.h"' to the source code.
 *			   Note: you may need to copy db_utils.h into your working directory or
 *			   specify	a path to its location.
 *			   It is located in "<install dir>\PIC32 Starter Kits\Microchip\Include".
 *			2. Add 'db_utils.a' to the library branch of MPLAB IDE project tree.
 *			   It is located in "<install dir>\PIC32 Starter Kits\Microchip\Common"
 *			3. Add "PIC32_STARTER_KIT" macro definition to the project build options
 *			   for the C compiler.
 *			4. Be sure to call DBINIT() first before any other Debug Print Library macros.
 *
 * Starter Board Resources:
 *			LED1 (RED)		= PORTD.RD0
 *			LED2 (YELLOW)	= PORTD.RD1
 *			LED3 (GREEN)	= PORTD.RD2
 *			SW1				= PORTD.RD6  (change notice pin CN15)
 *			SW2				= PORTD.RD7	 (change notice pin CN16)
 *			SW3				= PORTD.RD13 (change notice pin CN19)
 *			JTAG.TMS		= PORTA.RA0  (used by debugger)
 *			JTAG.TCK		= PORTA.RA1  (used by debugger)
 *			JTAG.TDO		= PORTA.RA5  (used by debugger)
 *			JTAG.TDI		= PORTA.RA4  (used by debugger)
 *			PGC2			= PORTB.RB6	 (used by debugger)
 *			PGD2			= PORTB.RB7  (used by debugger)
 *
 * Starter Board Notes:
 *			1.	There are no pullup resistors on SW1, SW2 or SW3.
 *				To enable the PORTD pullups for SW1, SW2 and SW3, use the following:
 *				CNPUE = (CN15_PULLUP_ENABLE | CN16_PULLUP_ENABLE | CN19_PULLUP_ENABLE);
 *			2.  Do not disable the PIC32MX JTAG.  This will prevent the PIC32MX Starter Kit
 *				debugger(PIC18F4550) from communicating with the PIC32MX device.
 *			3.  Do not configure the SYSTEM CLOCK to operate faster than 80MHz.
 *************************************************************************************
 * Change History:
 * ID			Date            Changes
 * C11842		12/01/2008		Modified for 80MHz operation, Updated comments.
 *************************************************************************************/

/* PIC32 Peripheral library functions and macros */
#include <plib.h> 

/* Configuration Bits */
#pragma config FNOSC    = PRIPLL        // Oscillator Selection
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (PIC32 Starter Kit: use divide by 2 only)
#pragma config FPLLMUL  = MUL_20        // PLL Multiplier
#pragma config FPLLODIV = DIV_1         // PLL Output Divider
#pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
#pragma config FWDTEN   = OFF           // Watchdog Timer 
#pragma config WDTPS    = PS1           // Watchdog Timer Postscale
#pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
#pragma config OSCIOFNC = OFF           // CLKO Enable
#pragma config POSCMOD  = XT            // Primary Oscillator
#pragma config IESO     = OFF           // Internal/External Switch-over
#pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable
#pragma config CP       = OFF           // Code Protect
#pragma config BWP      = OFF           // Boot Flash Write Protect
#pragma config PWP      = OFF           // Program Flash Write Protect
#pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#pragma config DEBUG    = OFF           // Debugger Disabled for Starter Kit


/*  The following PWM duty cycles determine the intensity of the LEDS */	
const unsigned short pwm_duty_cycles[] = {2500,5000,7500,10000,12500,15000,17500,20000, \
					22500,25000,22500,20000,17500,15000,12500,10000,7500,5000,2500,0};
					
#define ARRAY_SIZE			            (sizeof(pwm_duty_cycles))
#define DMA0				            (0)
#define DMA1				            (1)

// specify the number of bytes for each transfer
unsigned char srcSize = ARRAY_SIZE;

// specify size of (bytes) per transaction (16-bit PWM values requires 2 bytes)
unsigned char cellSize = 2;		

// specify the size (bytes) of the destination (16-bit OC1RS register)
unsigned int dstSize = 2;

// assign destination pointers to the OC1RS and 0C2RS registers
unsigned short* volatile pDma_0_Dst = (void*) &OC1RS;
unsigned short* volatile pDma_1_Dst = (void*) &OC2RS;

// assign source pointer to the patterns stored in program FLASH
const unsigned short* pDmaSrc = pwm_duty_cycles;		


//  The following is used by the main application
#define SYS_FREQ			(80000000)
#define PWM_PERIOD			(25000)

//  pwm_dma application code
int main(void)
{
    // Configure the device for maximum performance, but do not change the PBDIV clock divisor.
	// Given the options, this function will change the program Flash wait states,
	// RAM wait state and enable prefetch cache, but will not change the PBDIV.
    // The PBDIV value is already set via the pragma FPBDIV option above..
   	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	//Initialize the DB_UTILS IO channel
	DBINIT();

	// Display a message
	DBPRINTF("Welcome to the PIC32 PWM using DMA example. \n");
	DBPRINTF("The build date and time is ... (" __DATE__ "," __TIME__ ")\n");
	DBPRINTF("This example demonstrates using DMA to load PWM to vary LED intensity \n");
		
	// Step #1 - open DMA channel_0 and channel_1
	// Use channel chaining mode to start next channel when previous channel is finished
	DmaChnOpen(DMA0, 0, DMA_OPEN_CHAIN_LOW);
	DmaChnOpen(DMA1, 0, DMA_OPEN_CHAIN_HI);
	
	
	// Step #2 - setup DMA event triggers using TIMER 4 period match interrupt
	DmaChnSetEventControl(DMA0, DMA_EV_START_IRQ(_TIMER_4_IRQ));
	DmaChnSetEventControl(DMA1, DMA_EV_START_IRQ(_TIMER_4_IRQ));
	
		
	// Step #3 - setup the transfer pointers and transfer sizes
	DmaChnSetTxfer(DMA0, pDmaSrc, pDma_0_Dst, srcSize, dstSize, cellSize);
	DmaChnSetTxfer(DMA1, pDmaSrc, pDma_1_Dst, srcSize, dstSize, cellSize);


    // Step #4 - Configure the Output Compare channels for PWM mode using Timer3
	// setup output compare channel #1 - RD0
   	OpenOC1(OC_ON | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
   
	// setup output compare channel #2 - RD1
   	OpenOC2(OC_ON | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
   
   
    // Step #5 - Configure Timer3 and Timer4
   // Timer3 generates 20ms period for PWMs
	OpenTimer3(T3_ON | T3_PS_1_64, PWM_PERIOD);
	
	// Timer4 generates 32ms triggers for DMA transfer
	OpenTimer4(T4_ON | T4_PS_1_256, 10000);
	
	
    // Step #6 - Enable DMA channel 0 to start the transfers.
    // Channel chaining will take over automatically 
	DmaChnEnable(DMA0);
	
    // sit back and enjoy the led light show!!
    // The CPU is not involved in the PWM updates so free to do other things...
	while(1);

}
