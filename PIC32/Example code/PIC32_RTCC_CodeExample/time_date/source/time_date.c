/*********************************************************************
 *
 *    PIC32MX RTCC API Example
 *
 *********************************************************************
 * FileName:        time_date.c
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
 * This example demonstrates multiple ways to initialize the RTCC
 * with the current date. It then shows how to read that date and
 * time with multiple methods. Then it configures a function for
 * checking that the RTCC has a clock enabled and counts the time.
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

//	local function prototypes
int CheckRtccRunning(int secWait);

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
	rtccTime	tm, tm1;			// time structure
	rtccDate	dt, dt1;			// date structure

	// Configure the device for maximum performance but do not change the PBDIV
	// Given the options, this function will change the flash wait states, RAM
	// wait state and enable prefetch cache but will not change the PBDIV.
	// The PBDIV value is already set via the pragma FPBDIV option above..
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	RtccInit();			// init the RTCC
	while(RtccGetClkStat()!=RTCC_CLK_ON);	// wait for the SOSC to be actually running and RTCC to have its clock source
							// could wait here at most 32ms

	// when using the RtccSetTimeDate() function, the write operation is enabled if needed and then restored to the initial value
	// so that we don't have to worry about calling RtccWrEnable()/RtccWrDisable() functions

	// let's start setting the current date
	{
		// one way to do it
		tm.l=0;
		tm.sec=0x30;
		tm.min=0x07;
		tm.hour=0x10;

		dt.wday=2;
		dt.mday=0x16;
		dt.mon=0x01;
		dt.year=0x07;
		RtccSetTimeDate(tm.l, dt.l);
	}
	// however, much easier to do it should be:
	RtccSetTimeDate(0x10073000, 0x07011602);	// time is MSb: hour, min, sec, rsvd. date is MSb: year, mon, mday, wday.
												// please note that the rsvd field has to be 0 in the time field!

	// NOTE: at this point the writes to the RTCC time and date registers are disabled

	// we can also read the time and date
	tm1.l=RtccGetTime();
	dt1.l=RtccGetDate();
	// can check that the time and date are the ones we just set
	if(tm.hour!=tm1.hour ||tm.min!=tm1.min)
	{
		return 0;
	}
	if(dt.l!=dt1.l)		// check for proper date
	{
		return 0;
	}
	// we can read the time and date in a single operation
	RtccGetTimeDate(&tm1, &dt1);

	// now that we know the RTCC clock is up and running, it's easier to start from fresh:
	RtccOpen(tm.l, dt.l, 0);	// set time, date and calibration in a single operation

	// check that the RTCC is running
	if(!CheckRtccRunning(3))
	{
		return 0;
	}

	// another way to see the RTCC is tunning: check the SYNC bit
	while(RtccGetSync());	// wait sync to be low
	while(!RtccGetSync());	// wait to be high
	while(RtccGetSync());	// wait sync to be low again


	// other RTCC operations

	// adjust the RTCC timing
	RtccSetCalibration(200);	// value to calibrate with at each minute

	// enabling the RTCC output pin
	RtccSelectPulseOutput(1);		// select the seconds clock pulse as the function of the RTCC output pin
	RtccSelectPulseOutput(0);		// select the alarm pulse as the function of the RTCC output pin
	RtccOutputEnable(1);			// enable the Output pin of the RTCC


	return 1;
}



/*********************************************************************
 * Function:        int CheckRtccRunning(int secWait)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          1(true) if test succeeded, 0(FALSE) otherwise
 *
 * Side Effects:    None
 *
 * Overview:        The function checks that the RTCC has the clock enabled and counts the time.
 *
 * Note:            None
 ********************************************************************/
int CheckRtccRunning(int secWait)
{
	#define	WAIT_FOR_SEC_TMO	1100			// how many ms to wait for the RTCC seconds count to change

	rtccTime	t0, t1;
	int		fail;
	int		secCnt;
	unsigned int	tStart;


	
	for(secCnt=0, fail=0; secCnt<secWait; secCnt++)
	{
		tStart=ReadCoreTimer();
		t0.l=RtccGetTime();
		do
		{
			t1.l=RtccGetTime();
		}while((t1.sec == t0.sec) && (ReadCoreTimer()-tStart) < (SYS_FREQ/2000)*WAIT_FOR_SEC_TMO);	// wait seconds change

		if(t1.sec==t0.sec)
		{
			fail=1;
			break;	// failed
		}
	}
	
	return !fail;
}

