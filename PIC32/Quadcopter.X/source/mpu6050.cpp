#include "mpu6050.h"

/*
   Rasperry Pi + MPU6050
   _______________________
   |:::::::               |
   |      __              |
   |     |  |             |
   |     |  |             |
   |     |__|             |
   |                      |
   |______________________|

   Acc X + \/
   Acc Y + ->
   Acc Z + down

   Gyro X + ->
   Gyro Y + /\
   Gyro Z + counterclockwise


*/

/*
int mSleep( int milliseconds )
{
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000 * milliseconds;

    if(nanosleep(&tim , &tim2) < 0 )
    {
        printf("Nano sleep system call failed \n");
        return -1;
    }

    return 0;
}
*/
/*****************************************************************
 * This is the default constructor for the class. It assigns
 * all private variables to default values and calls the openI2C()
 * function to open the default I2C device "/dev/i2c-1".
 *****************************************************************/
MPU6050::MPU6050(void)
{
    gyro_xsensitivity = 1.0;
    gyro_ysensitivity = 65.5;
    gyro_zsensitivity = 1.0;

    GYRO_XANGLE = 0.0;
    GYRO_YANGLE = 0.0;
    GYRO_ZANGLE = 0.0;

    // Set the I2C interface used
    this->i2cBusId = I2C1;
    // Set the device address
    this->deviceAddress = 0x68;

    //cout << " Opening I2C Device" << endl;

    // TODO: Fault handling! What to do if function fails?
    I2CInit();
}

/*******************************************************************
 * This is the overloaded constructor. It allows the programmer to
 * specify a custom I2C device & device address
 * The device descriptor is determined by the openI2C() private member
 * function call.
 * *****************************************************************/
MPU6050::MPU6050( I2C_MODULE new_i2cBusId, UINT8 new_devAddress )
{
    gyro_xsensitivity = 1.0;
    gyro_ysensitivity = 65.5;
    gyro_zsensitivity = 1.0;

    GYRO_XANGLE = 0.0;
    GYRO_YANGLE = 0.0;
    GYRO_ZANGLE = 0.0;

    // Set the I2C interface used
    this->i2cBusId = new_i2cBusId;
    // Set the device address
    this->deviceAddress = new_devAddress;

    //cout << " Opening I2C Device" << endl;    
    
    // TODO: Fault handling! What to do if function fails?
    I2CInit();
}

/**********************************************************************
 * This is the class destructor it simply closes the open I2C device
 * by calling the closeI2C() which in turn calls the close() system call
 * *********************************************************************/
MPU6050::~MPU6050(void)
{
    //cout << " Closing I2C Device" << endl;
    //this->closeI2C();
}

void MPU6050::MPU6050_Test_I2C()
{
    UINT8 Data = 0x00;

    readReg( MPU6050_RA_WHO_AM_I, Data );

    if(Data == 0x68)
    {
        //cout << " I2C Read Test Passed, MPU6050 Address: 0x" << hex << (int)Data << endl;

    }
    else
    {
        //cout << " ERROR: I2C Read Test Failed. Data received: 0x" << hex << (int)Data << endl;
    }
}

void MPU6050::Setup_MPU6050()
{
    //Sets sample rate to 8000/1+7 = 1000Hz
    writeReg(MPU6050_RA_SMPLRT_DIV, 0x07);
    //Disable FSync, 256Hz DLPF
    writeReg(MPU6050_RA_CONFIG, 0x00);
    //Disable gyro self tests, scale of 500 degrees/s
    writeReg(MPU6050_RA_GYRO_CONFIG, 0b00001000);
    //Disable accel self tests, scale of +-2g, no DHPF
    writeReg(MPU6050_RA_ACCEL_CONFIG, 0x00);
    //Freefall threshold of |0mg|
    writeReg(MPU6050_RA_FF_THR, 0x00);
    //Freefall duration limit of 0
    writeReg(MPU6050_RA_FF_DUR, 0x00);
    //Motion threshold of 0mg
    writeReg(MPU6050_RA_MOT_THR, 0x00);
    //Motion duration of 0s
    writeReg(MPU6050_RA_MOT_DUR, 0x00);
    //Zero motion threshold
    writeReg(MPU6050_RA_ZRMOT_THR, 0x00);
    //Zero motion duration threshold
    writeReg(MPU6050_RA_ZRMOT_DUR, 0x00);
    //Disable sensor output to FIFO buffer
    writeReg(MPU6050_RA_FIFO_EN, 0x00);

    //AUX I2C setup
    //Sets AUX I2C to single master control, plus other config
    writeReg(MPU6050_RA_I2C_MST_CTRL, 0x00);
    //Setup AUX I2C slaves
    writeReg(MPU6050_RA_I2C_SLV0_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV0_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV0_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV1_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV1_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV1_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV2_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV2_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV2_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV3_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV3_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV3_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_DO, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_DI, 0x00);

    //MPU6050_RA_I2C_MST_STATUS //Read-only
    //Setup INT pin and AUX I2C pass through
    writeReg(MPU6050_RA_INT_PIN_CFG, 0x00);
    //Enable data ready interrupt
    writeReg(MPU6050_RA_INT_ENABLE, 0x00);

    //MPU6050_RA_DMP_INT_STATUS        //Read-only
    //MPU6050_RA_INT_STATUS 3A        //Read-only
    //MPU6050_RA_ACCEL_XOUT_H         //Read-only
    //MPU6050_RA_ACCEL_XOUT_L         //Read-only
    //MPU6050_RA_ACCEL_YOUT_H         //Read-only
    //MPU6050_RA_ACCEL_YOUT_L         //Read-only
    //MPU6050_RA_ACCEL_ZOUT_H         //Read-only
    //MPU6050_RA_ACCEL_ZOUT_L         //Read-only
    //MPU6050_RA_TEMP_OUT_H         //Read-only
    //MPU6050_RA_TEMP_OUT_L         //Read-only
    //MPU6050_RA_GYRO_XOUT_H         //Read-only
    //MPU6050_RA_GYRO_XOUT_L         //Read-only
    //MPU6050_RA_GYRO_YOUT_H         //Read-only
    //MPU6050_RA_GYRO_YOUT_L         //Read-only
    //MPU6050_RA_GYRO_ZOUT_H         //Read-only
    //MPU6050_RA_GYRO_ZOUT_L         //Read-only
    //MPU6050_RA_EXT_SENS_DATA_00     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_01     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_02     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_03     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_04     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_05     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_06     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_07     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_08     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_09     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_10     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_11     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_12     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_13     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_14     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_15     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_16     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_17     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_18     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_19     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_20     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_21     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_22     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_23     //Read-only
    //MPU6050_RA_MOT_DETECT_STATUS     //Read-only

    //Slave out, dont care
    writeReg(MPU6050_RA_I2C_SLV0_DO, 0x00);
    writeReg(MPU6050_RA_I2C_SLV1_DO, 0x00);
    writeReg(MPU6050_RA_I2C_SLV2_DO, 0x00);
    writeReg(MPU6050_RA_I2C_SLV3_DO, 0x00);
    //More slave config
    writeReg(MPU6050_RA_I2C_MST_DELAY_CTRL, 0x00);
    //Reset sensor signal paths
    writeReg(MPU6050_RA_SIGNAL_PATH_RESET, 0x00);
    //Motion detection control
    writeReg(MPU6050_RA_MOT_DETECT_CTRL, 0x00);
    //Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0
    writeReg(MPU6050_RA_USER_CTRL, 0x00);
    //Sets clock source to gyro reference w/ PLL
    writeReg(MPU6050_RA_PWR_MGMT_1, 0b00000010);
    //Controls frequency of wakeups in accel low power mode plus the sensor standby modes
    writeReg(MPU6050_RA_PWR_MGMT_2, 0x00);
    //MPU6050_RA_BANK_SEL            //Not in datasheet
    //MPU6050_RA_MEM_START_ADDR        //Not in datasheet
    //MPU6050_RA_MEM_R_W            //Not in datasheet
    //MPU6050_RA_DMP_CFG_1            //Not in datasheet
    //MPU6050_RA_DMP_CFG_2            //Not in datasheet
    //MPU6050_RA_FIFO_COUNTH        //Read-only
    //MPU6050_RA_FIFO_COUNTL        //Read-only
    //Data transfer to and from the FIFO buffer
    writeReg(MPU6050_RA_FIFO_R_W, 0x00);
    //MPU6050_RA_WHO_AM_I             //Read-only, I2C address

    //cout << "MPU6050 Setup Complete" << endl;
}

void MPU6050::Calibrate_Gyros()
{
    int	GYRO_XOUT_OFFSET_1000SUM = 0;
    int	GYRO_YOUT_OFFSET_1000SUM = 0;
    int	GYRO_ZOUT_OFFSET_1000SUM = 0;

    for(int i = 0; i < 1000; i++)
    {
        readReg((unsigned char)MPU6050_RA_GYRO_XOUT_H, GYRO_XOUT_H);
        readReg((unsigned char)MPU6050_RA_GYRO_XOUT_L, GYRO_XOUT_L);
        readReg((unsigned char)MPU6050_RA_GYRO_YOUT_H, GYRO_YOUT_H);
        readReg((unsigned char)MPU6050_RA_GYRO_YOUT_L, GYRO_YOUT_L);
        readReg((unsigned char)MPU6050_RA_GYRO_ZOUT_H, GYRO_ZOUT_H);
        readReg((unsigned char)MPU6050_RA_GYRO_ZOUT_L, GYRO_ZOUT_L);

        GYRO_XOUT_OFFSET = ((GYRO_XOUT_H<<8)|GYRO_XOUT_L);
        GYRO_YOUT_OFFSET = ((GYRO_YOUT_H<<8)|GYRO_YOUT_L);
        GYRO_ZOUT_OFFSET = ((GYRO_ZOUT_H<<8)|GYRO_ZOUT_L);

        // Two's complement. Needs to be converted to make sense.
        // For info: (http://en.wikipedia.org/wiki/Two's_complement)
        if ( GYRO_XOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767
            GYRO_XOUT_OFFSET = GYRO_XOUT_OFFSET - 0xFFFF;
        if ( GYRO_YOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767
            GYRO_YOUT_OFFSET = GYRO_YOUT_OFFSET - 0xFFFF;
        if ( GYRO_ZOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767
            GYRO_ZOUT_OFFSET = GYRO_ZOUT_OFFSET - 0xFFFF;

        GYRO_XOUT_OFFSET_1000SUM += GYRO_XOUT_OFFSET;
        GYRO_YOUT_OFFSET_1000SUM += GYRO_YOUT_OFFSET;
        GYRO_ZOUT_OFFSET_1000SUM += GYRO_ZOUT_OFFSET;

        //__delay_ms(1);
        //mSleep(1);		// function not accurate but it´s OK for now..
    }
    GYRO_XOUT_OFFSET = GYRO_XOUT_OFFSET_1000SUM / 1000;
    GYRO_YOUT_OFFSET = GYRO_YOUT_OFFSET_1000SUM / 1000;
    GYRO_ZOUT_OFFSET = GYRO_ZOUT_OFFSET_1000SUM / 1000;

    //cout << "Gyro X offset sum: " << GYRO_XOUT_OFFSET_1000SUM << " Gyro X offset: " << GYRO_XOUT_OFFSET << endl;
    //cout << "Gyro Y offset sum: " << GYRO_YOUT_OFFSET_1000SUM << " Gyro Y offset: " << GYRO_YOUT_OFFSET << endl;
    //cout << "Gyro Z offset sum: " << GYRO_ZOUT_OFFSET_1000SUM << " Gyro Z offset: " << GYRO_ZOUT_OFFSET << endl;
}

//Gets raw accelerometer data, performs no processing
void MPU6050::Get_Accel_Values()
{
    readReg((unsigned char)MPU6050_RA_ACCEL_XOUT_H, ACCEL_XOUT_H);
    readReg((unsigned char)MPU6050_RA_ACCEL_XOUT_L, ACCEL_XOUT_L);
    readReg((unsigned char)MPU6050_RA_ACCEL_YOUT_H, ACCEL_YOUT_H);
    readReg((unsigned char)MPU6050_RA_ACCEL_YOUT_L, ACCEL_YOUT_L);
    readReg((unsigned char)MPU6050_RA_ACCEL_ZOUT_H, ACCEL_ZOUT_H);
    readReg((unsigned char)MPU6050_RA_ACCEL_ZOUT_L, ACCEL_ZOUT_L);

    RAW_ACCEL_XOUT = ((ACCEL_XOUT_H<<8)|ACCEL_XOUT_L);
    RAW_ACCEL_YOUT = ((ACCEL_YOUT_H<<8)|ACCEL_YOUT_L);
    RAW_ACCEL_ZOUT = ((ACCEL_ZOUT_H<<8)|ACCEL_ZOUT_L);

    // Two's complement. Needs to be converted to make sense.
    // For info: (http://en.wikipedia.org/wiki/Two's_complement)
    if ( RAW_ACCEL_XOUT > 0x7FFF ) 	// 0x7FFF = 32767
        RAW_ACCEL_XOUT -= 0xFFFF;
    if ( RAW_ACCEL_YOUT > 0x7FFF ) 	// 0x7FFF = 32767
        RAW_ACCEL_YOUT = RAW_ACCEL_YOUT - 0xFFFF;
    if ( RAW_ACCEL_ZOUT > 0x7FFF ) 	// 0x7FFF = 32767
        RAW_ACCEL_ZOUT = RAW_ACCEL_ZOUT - 0xFFFF;

    // Converts raw data into g. 
    // See MPU-6050 Product Specification.pdf page 13 for more information.
    ACCEL_XOUT = (float)RAW_ACCEL_XOUT / 16384;
    ACCEL_YOUT = (float)RAW_ACCEL_YOUT / 16384;
    ACCEL_ZOUT = (float)RAW_ACCEL_ZOUT / 16384;
}

//Converts the already acquired accelerometer data into 3D euler angles
void MPU6050::Get_Accel_Angles()
{
    ACCEL_XANGLE = 57.295*atan((float)ACCEL_YOUT/ sqrt(pow((float)ACCEL_ZOUT,2)+pow((float)ACCEL_XOUT,2)));
    ACCEL_YANGLE = 57.295*atan((float)-ACCEL_XOUT/ sqrt(pow((float)ACCEL_ZOUT,2)+pow((float)ACCEL_YOUT,2)));
}

//Function to read the gyroscope rate data and convert it into degrees/s
void MPU6050::Get_Gyro_Rates()
{
    int GYRO_XOUT;
    int GYRO_YOUT;
    int GYRO_ZOUT;

    readReg((unsigned char)MPU6050_RA_GYRO_XOUT_H, GYRO_XOUT_H);
    readReg((unsigned char)MPU6050_RA_GYRO_XOUT_L, GYRO_XOUT_L);
    readReg((unsigned char)MPU6050_RA_GYRO_YOUT_H, GYRO_YOUT_H);
    readReg((unsigned char)MPU6050_RA_GYRO_YOUT_L, GYRO_YOUT_L);
    readReg((unsigned char)MPU6050_RA_GYRO_ZOUT_H, GYRO_ZOUT_H);
    readReg((unsigned char)MPU6050_RA_GYRO_ZOUT_L, GYRO_ZOUT_L);

    GYRO_XOUT = ((GYRO_XOUT_H<<8)|GYRO_XOUT_L);
    GYRO_YOUT = ((GYRO_YOUT_H<<8)|GYRO_YOUT_L);
    GYRO_ZOUT = ((GYRO_ZOUT_H<<8)|GYRO_ZOUT_L);

    // Two's complement. Needs to be converted to make sense.
    // For info: (http://en.wikipedia.org/wiki/Two's_complement)
    if ( GYRO_XOUT > 0x7FFF ) 	// 0x7FFF = 32767
        GYRO_XOUT = GYRO_XOUT - 0xFFFF;
    if ( GYRO_YOUT > 0x7FFF ) 	// 0x7FFF = 32767
        GYRO_YOUT = GYRO_YOUT - 0xFFFF;
    if ( GYRO_ZOUT > 0x7FFF ) 	// 0x7FFF = 32767
        GYRO_ZOUT = GYRO_ZOUT - 0xFFFF;

    GYRO_XOUT -= GYRO_XOUT_OFFSET;
    GYRO_YOUT -= GYRO_YOUT_OFFSET;
    GYRO_ZOUT -= GYRO_ZOUT_OFFSET;

    GYRO_XRATE = (float)GYRO_XOUT / gyro_xsensitivity;
    GYRO_YRATE = (float)GYRO_YOUT / gyro_ysensitivity;
    GYRO_ZRATE = (float)GYRO_ZOUT / gyro_zsensitivity;


    //GYRO_XANGLE += GYRO_XRATE * 0.01;


}

/**********************************************************************
 * This function opens the I2C device by simply calling the open system
 * call on the I2C device specified in the i2cFileName string. The I2C
 * device is opened for writing and reading. The i2cDescriptor private
 * variable is set by the return value of the open() system call.
 * This variable will be used to reference the opened I2C device by the
 * ioctl() & close() system calls.
 * ********************************************************************/
/*int MPU6050::openI2C()
{
    this->i2cDescriptor = open(i2cFileName.c_str(), O_RDWR);
    if(this->i2cDescriptor < 0)
    {

        perror("Could not open file (1)");

        exit(1);
    }

    return i2cDescriptor;
}

*/

BOOL MPU6050::I2CInit()
{
    UINT32 actualClock;
    BOOL Success = TRUE;    

    // Set the I2C baudrate
    actualClock = I2CSetFrequency( i2cBusId, GetPeripheralClock(), I2C_CLOCK_FREQ );
    if ( abs(actualClock-I2C_CLOCK_FREQ) > I2C_CLOCK_FREQ/10 )
    {
        //DBPRINTF("Error: I2C1 clock frequency (%u) error exceeds 10%%.\n", (unsigned)actualClock);
        Success = FALSE;
    }

    // Enable the I2C bus
    I2CEnable( i2cBusId, TRUE );

    return Success;
}


/*********************************************************************
 * This function closes the I2C device by calling the close() system call
 * on the I2C device descriptor.
 *
 *******************************************************************/
/*int MPU6050::closeI2C()
{

    int retVal = -1;

    retVal = close(this->i2cDescriptor);
    if(retVal < 0)
    {

        perror("Could not close file (1)");

        exit(1);
    }
    return retVal;
}
*/
/********************************************************************
 *This function writes a byte of data "data" to a specific register
 *"reg_addr" in the I2C device This involves sending these two bytes
 *in order to the i2C device by means of the ioctl() command. Since
 *both bytes are written (no read/write switch), both pieces
 *of information can be sent in a single message (i2c_msg structure)
 ********************************************************************/
/*int MPU6050::writeReg(unsigned char reg_addr, unsigned char data)
{

    unsigned char buff[2];
    int retVal = -1;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    buff[0] = reg_addr;
    buff[1] = data;

    messages[0].addr = deviceAddress;
    messages[0].flags = 0;
    messages[0].len = sizeof(buff);
    messages[0].buf = buff;

    packets.msgs = messages;
    packets.nmsgs = 1;

    retVal = ioctl(this->i2cDescriptor, I2C_RDWR, &packets);
    if(retVal < 0)

        perror("Write to I2C Device failed");

    return retVal;
}
*/
/********************************************************************
 *This function reads a byte of data "data" from a specific register
 *"reg_addr" in the I2C device. This involves sending the register address
 *byte "reg_Addr" with "write" asserted and then instructing the
 *I2C device to read a byte of data from that address ("read asserted").
 *This necessitates the use of two i2c_msg structs. One for the register
 *address write and another for the read from the I2C device i.e.
 *I2C_M_RD flag is set. The read data is then saved into the reference *variable "data".
 ********************************************************************/
/*int MPU6050::readReg(unsigned char reg_addr, unsigned char &data)
{

    unsigned char *inbuff, outbuff;
    int retVal = -1;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    outbuff = reg_addr;
    messages[0].addr = deviceAddress;
    messages[0].flags= 0;
    messages[0].len = sizeof(outbuff);
    messages[0].buf = &outbuff;

    inbuff = &data;
    messages[1].addr = deviceAddress;
    messages[1].flags = I2C_M_RD;
    messages[1].len = sizeof(inbuff);
    messages[1].buf = inbuff;

    packets.msgs = messages;
    packets.nmsgs = 2;

    retVal = ioctl(this->i2cDescriptor, I2C_RDWR, &packets);
    if(retVal < 0)

        perror("Read from I2C Device failed");

    return retVal;
}
*/


BOOL MPU6050::readReg( UINT8 regAddress, UINT8 &data )
{
    // Variable declarations
    UINT8               i2cData[10];
    I2C_7_BIT_ADDRESS   SlaveAddress;
    int                 Index;
    int                 DataSz;
    BOOL                Acknowledged;
    BOOL                Success = TRUE;
    UINT8               i2cbyte;


    // Initialize the data buffer
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, this->deviceAddress, I2C_WRITE);
    i2cData[0] = SlaveAddress.byte;
    i2cData[1] = regAddress;                 // MPU6050 data address to read (0x75 = WHO_AM_I which contains 0x68)
    DataSz = 2;


    // Start the transfer
    if( !StartTransfer(FALSE) )
    {
        //while(1);
        Success = FALSE;
    }

    // Address the device.
    Index = 0;
    while( Success & (Index < DataSz) )
    {
        // Transmit a byte
        if (TransmitOneByte(i2cData[Index]))
            Index++;
        else
            Success = FALSE;

        // Verify that the byte was acknowledged
        if(!I2CByteWasAcknowledged( i2cBusId ))
        {
            //DBPRINTF("Error: Sent byte was not acknowledged\n");
            Success = FALSE;
        }
    }

    // Restart and send the device's internal address to switch to a read transfer
    if(Success)
    {
        // Send a Repeated Started condition
        if( !StartTransfer(TRUE) )
        {
            //while(1);
            Success = FALSE;
        }

        // Transmit the address with the READ bit set
        I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, this->deviceAddress, I2C_READ);
        if (TransmitOneByte(SlaveAddress.byte))
        {
            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged( i2cBusId ))
            {
                //DBPRINTF("Error: Sent byte was not acknowledged\n");
                Success = FALSE;
            }
        }
        else
        {
            Success = FALSE;
        }
    }

    i2cbyte = 9;

    // Read the data from the desired address
    if(Success)
    {
        if(I2CReceiverEnable( i2cBusId , TRUE) == I2C_RECEIVE_OVERFLOW)
        {
            //DBPRINTF("Error: I2C Receive Overflow\n");
            Success = FALSE;
        }
        else
        {
            while(!I2CReceivedDataIsAvailable( i2cBusId ));
            i2cbyte = I2CGetByte( i2cBusId );
        }
    }

    // End the transfer
    StopTransfer();

    data = i2cbyte;

    if(!Success)
    {
        //while(1);

        mPORTBSetBits(BIT_2);
        mPORTBClearBits(BIT_3);
    }

    if( i2cbyte == 0x68 )
    {
        mPORTBSetBits(BIT_2);
        mPORTBSetBits(BIT_3);
    }
    if( i2cbyte == 9 )
    {
        //mPORTBSetBits(BIT_2);
        //mPORTBClearBits(BIT_3);
    }

    return Success;
}

BOOL MPU6050::writeReg(UINT8 regAddress, UINT8 data)
{
    UINT8               i2cData[10];
    I2C_7_BIT_ADDRESS   SlaveAddress;
    int                 Index;
    int                 DataSz;
    BOOL                Acknowledged = FALSE;
    BOOL                Success = TRUE;

    // Initialize the data buffer
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, this->deviceAddress, I2C_WRITE);
    i2cData[0] = SlaveAddress.byte;
    i2cData[1] = regAddress;        // Register Address to write
    i2cData[3] = data;              // Data to write
    DataSz = 3;

    // Start the transfer
    if( !StartTransfer(FALSE) )
    {
        //while(1);
        Success = FALSE;
    }

    // Transmit all data
    Index = 0;
    while( Success && (Index < DataSz) )
    {
        // Transmit a byte
        if (TransmitOneByte(i2cData[Index]))
        {
            // Advance to the next byte
            Index++;

            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged( i2cBusId ))
            {
                //DBPRINTF("Error: Sent byte was not acknowledged\n");
                Success = FALSE;
            }
        }
        else
        {
            Success = FALSE;
        }
    }

    // End the transfer
    StopTransfer();

    // Wait for device to complete write process, by polling the ack status.
    while (Acknowledged != TRUE && Success != FALSE);
    {
        // Start the transfer
        if( StartTransfer(FALSE) )
        {
            // Transmit just the device's address
            if (TransmitOneByte(SlaveAddress.byte))
            {
                // Check to see if the byte was acknowledged
                Acknowledged = I2CByteWasAcknowledged( i2cBusId );
            }
            else
                Success = FALSE;

            // End the transfer
            StopTransfer();
        }
        else
          Success = FALSE;
    }
    return Success;
}


