PIC32 Ethernet Starter Kit: “PWM_DMA” demo
---------------------------------------------------------
 
Required Development Resources:
-------------------------------
a. PIC32 Ethernet Starter Kit (DM320004)
b. MPLAB IDE.
c. C Compiler.


Compiling and Downloading the Demo:
-----------------------------------
1. Load the demo code into MPLAB by double clicking the *.mcp project file.
2. Connect the mini-B debugger port on-board the Starter board to an USB port on the development computer using the USB cable provided in the kit.
3. Choose the PIC32 Starter Kit debugger tool in MPLAB IDE by selecting Debugger>Select Tool and then click on PIC32 Starter Kit.
4. Build the project by selecting Project>Build All.
5. Download your code into the evaluation board microcontroller by selecting Debugger>Programming>Program All Memories.
6. Run the demo code by selecting Debugger>Run.

Running the Demo:
-----------------
This program uses 2 PWM channels to vary the intensity of LED1 (PORTD.RD0) and LED2 (PORTD.RD1) on the Starter Kit.

PWM1 output drives LED1 and PWM2 output drives LED2.
Timer3 generates 20msec (50Hz) PWM period for PWM1 and PWM2 outputs.
Timer4 generates 32msec event triggers for DMA0 and DMA1.

A PWM channel is updated every 32msec automatically with a new duty cycle using a DMA transfer from an array of duty cycle values stored in program memory. When all the duty cycle values have been transferred to PWM1 by DMA0, DMA1 starts transferring duty cycle values to PWM2. When DMA1 finishes, DMA0 is started again, and this process repeats. This "ping-pong" process continues automatically and does not require any bandwidth from the cpu.

Also demonstrated is the use of the Debug Print Library macros to send user messages to the MPLAB IDE Starter Kit console window
