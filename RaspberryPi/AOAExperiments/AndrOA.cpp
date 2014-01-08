#include "AndrOA.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Constructor assign */
AndrOA::AndrOA(const char *manufacturer,
        const char *model,
        const char *description,
        const char *version,
        const char *url,
        const char *serial):
    manufacturer(manufacturer),
    model(model),
    description(description),
    version(version),
    url(url),
    serial(serial),
    device_handle(NULL),
    ctx(NULL){}

/* Destructor, release interface
 * exit the context
 */
AndrOA::~AndrOA()
{
    if(handle != NULL )
    {
        libusb_release_interface(device_handle, 0);
        libusb_close(device_handle);
    }
    if( ctx != NULL )
    {
        libusb_exit(context);
    }
}

/*
 *
 * Functons
 *
 */

/*
 * connect_to_accessory() - connect to a android accessory device
 * @return
 * 0 : connection success
 * 1 : connection failed.(AOA device not found)
 * 2 : connection failed.(Accessory mode switch failed)
 * 3 : connection failed.(libusb_claim_interface failed)
 */

int AndrOA::connect_to_accessory(void)
{
    int returned = libusb_init(&context);
    if(returned != 0){
        printf("ERROR libusb_init failed\n");
        return 1;
    }

    unsigned char ioBuffer[2];
    int protocol;
    int res;
    int tries = 10;
    uint16_t idVendor, idProduct;

    /* Search for connected devices */
    protocol = searchDevice(context, &idVendor, &idProduct);

    if ( protocol < 0 )
    {
        printf("ERROR Android accessory device not found.\n");
        return -1;
    } else if( protocol == 0 )
    { // Check if we are already in accessory mode
        device_handle = libusb_open_device_with_vid_pid(context,
                idVendor, idProduct);
        libusb_claim_interface(device_handle, 0);
        return 0;
    }

    versionProtocol = protocol;
    device_handle = libusb_open_device_with_vid_pid(context,
            idVendor, idProduct);
    libusb_claim_interface(device_handle, 0);
    usleep(800);

    /* Send the accessory info */
    sendString(ACCESSORY_STRING_MANUFACTURER, (char*)manufacturer);
    sendString(ACCESSORY_STRING_MODEL, (char*)model);
    sendString(ACCESSORY_STRING_DESCRIPTION, (char*)description);
    sendString(ACCESSORY_STRING_VERSION, (char*)version);
    sendString(ACCESSORY_STRING_URI, (char*)url);
    sendString(ACCESSORY_STRING_SERIAL, (char*)serial);

    /* Swich to accessory mode */
    res = libusb_control_transfer(device_handle, 0x40,
            ACCESSORY_START, 0, 0, NULL, 0, 0);
    if(res < 0){
        libusb_close(device_handle);
        device_handle = NULL;
        return -2;
    }

    /* Check that the devicehandle is reall NULLED */
    if(device_handle != NULL){
        libusb_close(device_handle);
        device_handle = NULL;
    }

    usleep(8000);

    printf("connecting to a new ProductID...\n");
    //attempt to connect to new PID,
    //if that doesn't work try ACCESSORY_PID_ALT
    for(;;){
        --tries;
        int tmpRes = searchDevice(context, &idVendor, &idProduct);
        if(tmpRes != 0){
            continue;
        }
        device_handle = libusb_open_device_with_vid_pid(context, idVendor,
                idProduct);
        if( device_handle == NULL ){
            if( tries < 0 ){
                return -1;
            }
        } else{
            break;
        }
        sleep(1);
    }

    res = libusb_claim_interface(device_handle, 0);
    if(res < 0){
        return -3;
    }

    printf("Established Android accessory connection.\n");
    return 0;
}

/* read() - read data from accessory device
 * @args
 * buf     : data buffer
 * len     : data length
 * timeout : wait time(ms)
 * @return
 * < 0 : Error (libusb_bulk_transfer error code)
 * >=0 : Succes(received bytes)
 */

int AndrOA::read(unsigned char *buffer, int len, unsigned int timeout)
{
    int xferred;
    int tmpRes = libusb_bulk_transfer(device_handle, inEP,
            buffer, len, &xferred, timeout);
    if(tmpRes == 0){
        tmpRes = xferred;
    }
    return(tmpRes);
}

/*  write() - write data to accessory device
 *  @args
 *  *buf    : data buffer
 *  len     : data length
 *  timeout : wait time(ms)
 *
 * @ret
 * <0 : Error (libusb_bulk_transfer error code)
 * >=0 : Succes(received bytes)
 */

int AndrOA::write(unsigned char *buffer, int len, unsigned int timeout)
{
    int xferred;
    int tmpRes = libusb_bulk_transfer(device_handle, outEP, buffer,
            len, &xferred, timeout);
    if(tmpRes == 0)
    {
        tmpRes = xferred;
    }
    return(tmpRes);
}

