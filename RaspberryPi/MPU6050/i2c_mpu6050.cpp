/*********************************************************************
 * Main function *
 *******************************************************************/
/*
   Här är jag:
   I main-loopen har jag 1ms delay och skärmuppdatering var 100 cykel.
   Dock går skärmuppdateringen mycket långsammare än 100ms!!
   Detta påverkar säkert filtret också.
   Filtret är trögt. Vinkelvärdet är ca 2 sek efter vid snabba rörelser.
   */

#include "mpu6050.h"
//#include <iostream>
//using namespace std;

void changemode(int);
int  kbhit(void);
void gotoxy();

int main(void)
{
    //unsigned char Data = 0;
    //float value;

    float AccX, AccY, AccZ;
    float GyroX, GyroY, GyroZ;

    //float AngleX = 0;
    float AngleY = 0;
    //float AngleZ = 0;

    int i = 0;


    MPU6050 MPU6050dev( 0x68, string("/dev/i2c-1") );

    MPU6050dev.Setup_MPU6050();

    MPU6050dev.Calibrate_Gyros();

    //cout << "Press a key: " << endl;

    //getchar();

    //cout << "Procceding.." << endl;

    changemode(1);

    while ( !kbhit() )
    {
        i++;

        //mSleep(1);

        MPU6050dev.Get_Accel_Values();
        //MPU6050dev.Get_Accel_Angles();

        MPU6050dev.Get_Gyro_Rates();

        //value = MPU6050dev.GetAngle();

        AccX = MPU6050dev.GetAccX();
        AccY = MPU6050dev.GetAccY();
        AccZ = MPU6050dev.GetAccZ();

        GyroX = MPU6050dev.GetGyroX();
        GyroY = MPU6050dev.GetGyroY();	// 177	// ska inte vara 177 längre. Har ändra skalningen så att GyroY ska vara grader/s
        GyroZ = MPU6050dev.GetGyroZ();

        //AccY = ( -AccX + 800 ) / 177;		// 177 	// 93.6
        AccY = ( -AccX );

        AngleY = (0.98)*(AngleY + GyroY/117) + (0.02)*( (AccY * 90) + 5.7 );	// Loopen går i ca 117 Hz

        //AngleY = AngleY / 177;

        //cout << "X Value: " << AccX << "   Y Value: " << AccY << "   Z Value: " << AccZ << endl;
        //cout << "X Value: " << GyroX << "   Y Value: " << GyroY << "   Z Value: " << GyroZ << endl;

        if ( i == 20 )
        {
            cout << "Angle Y:  " << AngleY << "   Gyro Y: " << GyroY << "   Acc Y: " << AccY << endl;
            i = 0;
        }
    }

    changemode(0);

    return 0;
}

void gotoxy(int x,int y)
{
    printf( "%c[%d;%df", 0x1B, y, x );
}

/*
   int main(void)
   {
   int ch;
   changemode(1);
   while ( !kbhit() )
   {
   putchar('.');
   }

   ch = getchar();

   printf("\nGot %c\n", ch);

   changemode(0);
   return 0;
   }
   */

void changemode(int dir)
{
    static struct termios oldt, newt;

    if ( dir == 1 )
    {
        tcgetattr( STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~( ICANON | ECHO );
        tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    }
    else
        tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit (void)
{
    struct timeval tv;
    fd_set rdfs;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET (STDIN_FILENO, &rdfs);

    select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &rdfs);

}
