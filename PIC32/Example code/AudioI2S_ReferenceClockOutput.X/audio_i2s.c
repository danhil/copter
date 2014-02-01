/*********************************************************************
 *
 *      PIC32MX Audio I2S Communication and Reference Clock Output
 *
 *********************************************************************
 * FileName:        audio_i2s.c
 * Dependencies:    plib.h
 *                  test_sine.c
 *
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE v8+
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?Company?) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company?s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ******************************************************************
 * Platform: Explorer-16 with PIC32MX PIM
 *
 * Description:
 *      The example generates 12.288MHz Reference Clock Output and
 *      is configured for 48kHz sample rate 24-bit I2S stereo
 *      communication. A sinusoid tone sampled at 48kHz is transmitted
 *      over I2S output. REFOCLK, SS, SDO and SDI pins use the
 *      Peripheral Pin Select functionality for flexible mapping of I2S
 *      functionality on desired pins. The readme file states the pins
 *      that can be probed to observe the I2S and Reference Clock Output
 *      signals.
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

//Definitions.
#define SAMPLE_RATE 48000
#define FRAME_SIZE 	(SAMPLE_RATE/1000)
#define RODIV		3
#define REFTRIM		464

//Stereo audio structure for 24-bit I2S data.

typedef union {

    struct {
        INT32 rightChannel;
        INT32 leftChannel;
    };
    UINT64 audioWord;
} AudioStereo;
AudioStereo* txBuffer;

/* Main entry function */
int main(void) {
    UINT spi_con1 = 0, spi_con2 = 0;
    UINT baud_rate = SAMPLE_RATE * 64;
    UINT mclk = SAMPLE_RATE * 256;
    extern AudioStereo test_sine[];

    //Congigure MIPS, Prefetch Cache module.
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    INTEnableSystemMultiVectoredInt();

    //Test tone vector sampled at 48kHz.
    txBuffer = test_sine;

    //Configure the direction of used pins and
    //configure as digital pins.
    #if defined (__32MX220F032D__) || defined (__32MX250F128D__)
    PORTSetPinsDigitalOut(IOPORT_A, BIT_1);
    PORTSetPinsDigitalOut(IOPORT_B, BIT_4);
    PORTSetPinsDigitalIn(IOPORT_B, BIT_5);
    PORTSetPinsDigitalOut(IOPORT_C, BIT_3);
    #elif defined (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    PORTSetPinsDigitalOut(IOPORT_D, BIT_9);
    PORTSetPinsDigitalOut(IOPORT_D, BIT_15);
    PORTSetPinsDigitalIn(IOPORT_D, BIT_3);
    PORTSetPinsDigitalOut(IOPORT_D, BIT_4);
    #endif

    /*------------------------------------------------
    Pin mapping for PIC32MX220F032D and PIC32MX250F128D:
    --------------------------------------------------
    Function	PPS	Pin#	Test Point#	: Comment
    __________________________________________________________________________
    REFOCLK(o)	RPC3	(36)	TP36		: Reference Clock Output
    SCK1(o)	-	(14)	TP14            : SPI-I2S SCLK Output
    SS(o)	RPB4	(33)	TP33            : SPI-I2S LRCK Output
    SDO(o)	RPA1	(20)	TP20            : SPI-I2S SDO Output
    SDI(i)	RPB5	(41)	-               : SPI-I2S SDI Input */
    #if defined (__32MX220F032D__) || defined (__32MX250F128D__)
    PPSOutput(3, RPC3, REFCLKO); //REFCLK0: RPC3 - Out
    PPSInput(2, SDI1, RPB5); //SDI: RPB5 - In
    PPSOutput(2, RPA1, SDO1); //SDO: RPA1 - Out
    PPSOutput(1, RPB4, SS1); //SS: RPB4 - Out

    /*------------------------------------------------
    Pin mapping for PIC32MX430F064L, PIC32MX450F256L and PIC32MX470F512L:
    --------------------------------------------------
    Function	PPS	Pin#	Test Point#	: Comment
    __________________________________________________________________________
    REFOCLK(o)	RPD9	(69)	-		: Reference Clock Output
    SCK1(o)	-	(70)	-               : SPI-I2S SCLK Output
    SS(o)	RPD4	(81)	-               : SPI-I2S LRCK Output
    SDO(o)	RPD15	(48)	-               : SPI-I2S SDO Output
    SDI(i)	RPD3	(78)	-               : SPI-I2S SDI Input */
    #elif defined (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    PPSOutput(3, RPD9, REFCLKO); //REFCLK0: RPD9 - Out
    PPSInput(1, SDI1, RPD3); //SDI: RPD3 - In
    PPSOutput(2, RPD15, SDO1); //SDO: RPD15 - Out
    PPSOutput(3, RPD4, SS1); //SS: RPD4 - Out
    #endif

    //Configure Reference Clock Output to 12.288MHz.
    mOSCREFOTRIMSet(REFTRIM);
    OSCREFConfig(OSC_REFOCON_USBPLL, //USB-PLL clock output used as REFCLKO source
            OSC_REFOCON_OE | OSC_REFOCON_ON, //Enable and turn on the REFCLKO
            RODIV);

    //Configure SPI in I2S mode with 24-bit stereo audio.
    spi_con1 = SPI_OPEN_MSTEN | //Master mode enable
            SPI_OPEN_SSEN | //Enable slave select function
            SPI_OPEN_CKP_HIGH | //Clock polarity Idle High Actie Low
            SPI_OPEN_MODE16 | //Data mode: 24b
            SPI_OPEN_MODE32 | //Data mode: 24b
            SPI_OPEN_MCLKSEL | //Clock selected is reference clock
            SPI_OPEN_FSP_HIGH; //Frame Sync Pulse is active high

    spi_con2 = SPI_OPEN2_AUDEN | //Enable Audio mode
            SPI_OPEN2_AUDMOD_I2S; //Enable I2S mode



    //Configure and turn on the SPI1 module.
    SpiChnOpenEx(SPI_CHANNEL1, spi_con1, spi_con2, (mclk / baud_rate));

    //Enable SPI1 interrupt.
    INTSetVectorPriority(INT_SPI_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_SPI_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_SPI1, INT_ENABLED);

    SpiChnPutC(SPI_CHANNEL1, 0); //Dummy write to start the SPI

    //Idle.
    while (1);

    return 1;
}

/* SPI1 ISR */
void __ISR(_SPI_1_VECTOR, ipl4) SPI1InterruptHandler(void) {
    static UINT ptrIndex = 0;
    static BOOL toggleData = TRUE;

    SpiChnPutC(SPI_CHANNEL1,
            (toggleData ? txBuffer[ptrIndex].leftChannel : txBuffer[ptrIndex++].rightChannel));
    toggleData = !toggleData;
    if (ptrIndex >= FRAME_SIZE)
        ptrIndex = 0;
    INTClearFlag(INT_SPI1TX);
}