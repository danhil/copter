/*********************************************************************
 *
 *    PIC32MX RTCC API Example
 *
 *********************************************************************
 * FileName:        alarm_interrupts.c
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
 * Sets the alarm time and checks that we actually get an alarm. Then
 * sets the alarm time and enables/disables the RTCC alarm interrupts.
 *
 * Platform: Explorer-16 with PIC32MX PIM
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

static volatile int	rtccIntCount=0;			// ISR counter 

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
	rtccTime	tm, tAlrm;		// time structure
	rtccDate	dt, dAlrm;		// date structure
	int		success=0;			// return code
	int		waitIntCount=20;	// how many ints to wait for
	
	

	// Configure the device for maximum performance but do not change the PBDIV
	// Given the options, this function will change the flash wait states, RAM
	// wait state and enable prefetch cache but will not change the PBDIV.
	// The PBDIV value is already set via the pragma FPBDIV option above..
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	RtccInit();			// init the RTCC
	while(RtccGetClkStat()!=RTCC_CLK_ON);	// wait for the SOSC to be actually running and RTCC to have its clock source
							// could wait here at most 32ms

	// set time and date
	RtccOpen(0x17040000, 0x08100604, 0);	// time is MSb: hour, min, sec, rsvd. date is MSb: year, mon, mday, wday.
						// please note that the rsvd field has to be 0 in the time field!

	INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
	INTEnableInterrupts();
	
	// let's set the alarm time and check that we actually get an alarm
	do
	{
		RtccGetTimeDate(&tm, &dt);			// get current time and date
	}while((tm.sec&0xf)>0x7);				// don't want to have minute or BCD rollover

	tAlrm.l=tm.l;
	dAlrm.l=dt.l;

	tAlrm.sec+=2;				// alarm due in 2 secs

	RtccChimeEnable();			// rollover
	RtccSetAlarmRptCount(0);		// we'll get more than one alarm
	RtccSetAlarmRpt(RTCC_RPT_SEC);		// one alarm every second
	RtccSetAlarmTimeDate(tAlrm.l, dAlrm.l);	// set the alarm time
	RtccAlarmEnable();			// enable the alarm

	if(RtccGetAlarmEnable())	// check that the alarm is enabled
	{
		// enabling/disabling the RTCC alarm interrupts

		INTSetVectorPriority(INT_RTCC_VECTOR, INT_PRIORITY_LEVEL_4);			// set the RTCC priority in the INT controller
		INTSetVectorSubPriority(INT_RTCC_VECTOR, INT_SUB_PRIORITY_LEVEL_1);		// set the RTCC sub-priority in the INT controller
		INTEnable(INT_RTCC, INT_ENABLED);					// enable the RTCC event interrupts in the INT controller.

		while(rtccIntCount<waitIntCount);	// wait to get those interrupts and for time to pass by
		// of course we could do some other useful things...
		
		// ok, we're done
		INTEnable(INT_RTCC, INT_DISABLED);	// disable further RTCC interrupts
		success=1;
	}


	return success;
	
}

void __ISR(_RTCC_VECTOR, ipl4) RtccIsr(void)
{
	// once we get in the RTCC ISR we have to clear the RTCC int flag
	INTClearFlag(INT_RTCC);

	rtccIntCount++;	// show we're progressing somehow...

}





