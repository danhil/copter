/*********************************************************************
 *
 *                  SPI API Example file
 *
 *********************************************************************
 * FileName:        burst_slave.c
 * Dependencies:	Spi.h
 *
 * Processor:       PIC32
 *
 * Complier:        MPLAB Cxx
 *                  MPLAB IDE
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC Microcontroller products.
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
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: spi_api_example.c 4172 2007-08-14 22:17:03Z aura $
 *
 ********************************************************************/
#include <stdlib.h>

#include <plib.h>



// example functions prototype
void	SpiDoBurstExampleSlave(SpiChannel chn);

void	SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster);



// some definitions

#define	MIN_SPI_TXFER_SIZE		8		// min number of words per transfer
#define	MAX_SPI_TXFER_SIZE		512		// max number of words per transfer

typedef struct
{
	unsigned short	nChars;		// number of characters in the packet
	unsigned short	data[1];	// data itself
}SpiPkt;	// a transferred SPI packet. assumes we're working in 16 bits mode



// Configuration Bit settings
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

#if defined (__32MX460F512L__) || defined (__32MX360F512L__) || defined (__32MX795F512L__)
#define SYS_FREQ (80000000L)
#elif defined (__32MX220F032D__) || defined (__32MX250F128D__)
#define SYS_FREQ (40000000L)
#endif

/*********************************************************************
 * Function:        int main(void)
 *
 * PreCondition:    None
 *
 * Input:			None
 *
 * Output:          	None
 *
 * Side Effects:    None
 *
 * Overview:		Examples for the usage of the SPI Peripheral Lib
 *
 * Note:            None.
 ********************************************************************/
int	main(void)
{
      #if defined (__32MX460F512L__) || defined (__32MX360F512L__) || defined (__32MX795F512L__)
      #define SYS_FREQ (80000000L)
      #elif defined (__32MX220F032D__) || defined (__32MX250F128D__)
      #define SYS_FREQ (40000000L)
      #endif

	SpiChannel spiChn=SPI_CHANNEL1;	// the SPI channel to use

	// Configure the device for maximum performance but do not change the PBDIV
	// Given the options, this function will change the flash wait states, RAM
	// wait state and enable prefetch cache but will not change the PBDIV.
	// The PBDIV value is already set via the pragma FPBDIV option above..
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);


	SpiDoBurstExampleSlave(spiChn);

	return 1;

}




/*********************************************************************
 * Function:        void SpiDoBurstExampleSlave(SpiChannel chn)
 *
 * PreCondition:    None
 *
 * Input:           chn	- the SPI channel to use
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Examples for the usage of the SPI Peripheral Lib for in a master/slave switch transfer mode
 *
 * Note:            This test assumes that a master device is connected to our test SPI device and that
 * 					it sends us data packets.
 * 					The slave device, after receiving the data from the master switches to master mode and
 * 					relays data back to the master.
 * 					The master can then verify the integrity of the packet.
 * 					This is the SPI slave code.
 * 					The slave has to be launched first and has to be waiting when the master begins transmitting data
 ********************************************************************/
void SpiDoBurstExampleSlave(SpiChannel chn)
{

	static unsigned short slaveBuff[MAX_SPI_TXFER_SIZE];	// buffer for the slave to receive the data

	SpiInitDevice(chn, 0, 1, 0);	// initialize the SPI channel as slave, frame slave

	while(1)
	{
		int		txferSize;

		SpiChnChangeMode(chn, 0, 0, 1);	// set slave mode
		txferSize=SpiChnGetC(chn);	// receive the packet header
		SpiChnGetS(chn, (unsigned int*)slaveBuff, txferSize);	// get the rest of the packet


		// switch to master mode
		SpiChnChangeMode(chn, 1, 1, 1);
		SpiChnPutC(chn, txferSize);	// transmit the packet header
		SpiChnPutS(chn, (unsigned int*)slaveBuff, txferSize);	// write the rest of the packet

	}

}

/*********************************************************************
 * Function:        void	SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster)
 *
 * PreCondition:    None
 *
 * Input:           chn			- the SPI channel to use
 * 					isMaster	-	1: the device is to act as a bus master
 * 									0: the device is an SPI slave
 * 					frmEn		-	1: frame mode is enabled
 * 								-	0: frame mode is disabled
 * 					frmMaster	-	0: if frame mode is enabled, the device is a frame slave (FRMSYNC is input)
 * 									1: if frame mode is enabled, the device is a frame master (FRMSYNC is output)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Inits the SPI channel 1 to use 16 bit words
 * 					Performs the device initialization in both master/slave modes.
 *
 * Note:            None
 ********************************************************************/
void SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster)
{
	SpiOpenFlags	oFlags=SPI_OPEN_MODE16|SPI_OPEN_SMP_END|SPI_OPEN_ON;	// SPI open mode
	if(isMaster)
	{
		oFlags|=SPI_OPEN_MSTEN;
	}
	if(frmEn)
	{
		oFlags|=SPI_OPEN_FRMEN;
		if(!frmMaster)
		{
			oFlags|=SPI_OPEN_FSP_IN;
		}
	}


	SpiChnOpen(chn, oFlags, 4);	// divide fpb by 4, configure the I/O ports. Not using SS in this example

}


