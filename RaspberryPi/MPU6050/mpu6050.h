//#include <string>
#include <stdio.h>
//#include <conio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <math.h>
#include <time.h>

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <iostream>
using namespace std;

class MPU6050
{
    public:
        MPU6050(void); 													// default constructor
        MPU6050(unsigned char dev_addr, std::string i2cfilename);		//over loaded constructor
        ~MPU6050(void);

        void MPU6050_Test_I2C();
        void Setup_MPU6050();
        void Calibrate_Gyros();
        void Get_Accel_Values();
        void Get_Accel_Angles();
        void Get_Gyro_Rates();
        float GetAngle() {return ACCEL_XANGLE;}

        float GetAccX() {return ACCEL_XOUT;}
        float GetAccY() {return ACCEL_YOUT;}
        float GetAccZ() {return ACCEL_ZOUT;}

        float GetGyroX() {return GYRO_XRATE;}
        float GetGyroY() {return GYRO_YRATE;}
        float GetGyroZ() {return GYRO_ZRATE;}

    private:

        int openI2C(); 				// Open an I2C device. Called only in constructors.
        int closeI2C(); 			// Close an I2C device. Called only in destructor.

        int writeReg(unsigned char reg_addr, unsigned char data);		// function to write byte data into a register of an I2C device
        int readReg(unsigned char reg_addr, unsigned char &data);		// function to read byte data from a register of an I2C device

        // private member variables
        std::string  i2cFileName; 		//i2c device name e.g."/dev/i2c-0" or "/dev/i2c-1"
        int i2cDescriptor;  			// i2c device descriptor
        unsigned char deviceAddress; 		// i2c device address


        // Offset values used internally
        int GYRO_XOUT_OFFSET;
        int	GYRO_YOUT_OFFSET;
        int	GYRO_ZOUT_OFFSET;

        int ACCEL_XOUT_OFFSET;
        int	ACCEL_YOUT_OFFSET;
        int	ACCEL_ZOUT_OFFSET;

        // Temporary values used internally
        unsigned char	GYRO_XOUT_H;
        unsigned char	GYRO_XOUT_L;
        unsigned char	GYRO_YOUT_H;
        unsigned char	GYRO_YOUT_L;
        unsigned char	GYRO_ZOUT_H;
        unsigned char	GYRO_ZOUT_L;

        unsigned char	ACCEL_XOUT_H;
        unsigned char	ACCEL_XOUT_L;
        unsigned char	ACCEL_YOUT_H;
        unsigned char	ACCEL_YOUT_L;
        unsigned char	ACCEL_ZOUT_H;
        unsigned char	ACCEL_ZOUT_L;




        // Degrees/s
        float GYRO_XRATE;
        float GYRO_YRATE;
        float GYRO_ZRATE;

        // Gyro Angles
        float GYRO_XANGLE;
        float GYRO_YANGLE;
        float GYRO_ZANGLE;



        // Adjustment values for the gyro values
        float gyro_xsensitivity;
        float gyro_ysensitivity;
        float gyro_zsensitivity;

        // Raw data
        int RAW_ACCEL_XOUT;
        int RAW_ACCEL_YOUT;
        int RAW_ACCEL_ZOUT;

        // Raw data
        float ACCEL_XOUT;
        float ACCEL_YOUT;
        float ACCEL_ZOUT;

        // Euler angels
        float ACCEL_XANGLE;
        float ACCEL_YANGLE;
};

int mSleep( int milliseconds );
