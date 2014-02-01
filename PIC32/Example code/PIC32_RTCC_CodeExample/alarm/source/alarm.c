/*********************************************************************
 *
 *    PIC32MX RTCC API Example
 *
 *********************************************************************
 * FileName:        alarm.c
 * Dependencies:    plib.h
 *
 *
 * Processor:       PIC32
 *
 * Complier:        MPLAB Cxx
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
 * $Id: rtcc_api_example.c 4288 2007-08-22 23:00:56Z aura $
 *********************************************************************
 * The purpose of this example code is to demonstrate the PIC32MX
 * peripheral library macros and functions supporting the RTCC
 * module and its various features.
 *
 * Platform: Explorer-16 with PIC32MX PIM
 *
 * Features demonstrated:
 *   1. Activating the RTCC and initializing with some time and date
 *   2. Getting the time and date
 *   3. Setting the alarm and enabling it
 *   4. Setting up alarm interrrupts (Enabling/Disabling)
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

/*********************************************************************
 * Function:        int main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          1 if everything went on ok, 0 if failed
 *
 * Side Effects:    None
 *
 * Overview:        The function is an example of using the RTCC device.

 * Note:            None
 ********************************************************************/
int main(void)
{
	int returnValue = 1;
	
	rtccTime	tm, tAlrm;			// time structure
	rtccDate	dt, dAlrm;			// date structure

	int		isRtccIntEn;
	int		isAlrmEn;

	// Configure the device for maximum performance but do not change the PBDIV
	// Given the options, this function will change the flash wait states, RAM
	// wait state and enable prefetch cache but will not change the PBDIV.
	// The PBDIV value is already set via the pragma FPBDIV option above..
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	RtccInit();								// init the RTCC
	while(RtccGetClkStat()!=RTCC_CLK_ON);	// wait for the SOSC to be actually running and RTCC to have its clock source
											// could wait here at most 32ms

	// set time and date
	RtccOpen(0x10073000, 0x07011602, 0);	// time is MSb: hour, min, sec, rsvd. date is MSb: year, mon, mday, wday.
			//16:07:48  //07/01/22 - 2		// please note that the rsvd field has to be 0 in the time field!

	// let's set the alarm time and check that we actually get an alarm
	do
	{
		RtccGetTimeDate(&tm, &dt);			// get current time and date
	} while((tm.sec&0xf)>0x7);				// don't want to have minute or BCD rollover

	tAlrm.l=tm.l;
	dAlrm.l=dt.l;

	tAlrm.sec+=2;								// alarm due in 2 secs

	RtccChimeDisable();							// don't want rollover
	RtccSetAlarmRptCount(0);					// one alarm will do
	RtccSetAlarmRpt(RTCC_RPT_TEN_SEC);			// enable repeat rate, check the second field
	RtccSetAlarmTimeDate(tAlrm.l, dAlrm.l);		// set the alarm time
	RtccAlarmEnable();							// enable the alarm

	while(RtccGetAlarmEnable());				// wait it to be cleared automatically
	
	RtccGetTimeDate(&tm, &dt);					// get current time
	if(tm.sec!=tAlrm.sec || ((dt.l^dAlrm.l)&0x00ffffff)!=0 )			// make sure the alarm time is the right one
	{
		returnValue = 0;								// if failed
	}
	else
	{
		// other things we may do with the alarm...
		RtccChimeEnable();					// enable indefinite repeats
		RtccSetAlarmRptCount(1);			// set the initial repeat count
		RtccSetAlarmRpt(RTCC_RPT_MIN);		// enable repeat rate, every minute, for ex
		RtccAlarmDisable();					// disable the alarm
		isAlrmEn=RtccGetAlarmEnable();		// check that the alarm is enabled
	
		// enabling/disabling the RTCC alarm interrupts
	
		INTSetVectorPriority(INT_RTCC_VECTOR, INT_PRIORITY_LEVEL_4);			// set the RTCC priority in the INT controller
		INTSetVectorSubPriority(INT_RTCC_VECTOR, INT_SUB_PRIORITY_LEVEL_1);		// set the RTCC sub-priority in the INT controller
		INTEnable(INT_RTCC, INT_ENABLED);		// enable the RTCC event interrupts in the INT controller.
	
		INTEnable(INT_RTCC, INT_DISABLED);		// disable the RTCC interrupts
	
		// once we get in the RTCC ISR we have to clear the RTCC int flag
		// but we can do this whenever we see that the interrupt flag is set:
		if(INTGetFlag(INT_RTCC))
		{
			INTClearFlag(INT_RTCC);
		}
	
		// we can check to see if the RTCC interrupts are enabled:
		isRtccIntEn=INTGetEnable(INT_RTCC);
	}

	return returnValue;
}