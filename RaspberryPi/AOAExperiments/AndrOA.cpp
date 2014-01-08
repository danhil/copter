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
    context(NULL){}

/* Destructor, release interface
 * exit the context
 */
AndrOA::~AndrOA()
{
    if(device_handle != NULL )
    {
        libusb_release_interface(device_handle, 0);
        libusb_close(device_handle);
    }
    if( context != NULL )
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
    protocol = search_for_device(context, &idVendor, &idProduct);

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
    send_string(ACCESSORY_STRING_MANUFACTURER, (char*)manufacturer);
    send_string(ACCESSORY_STRING_MODEL, (char*)model);
    send_string(ACCESSORY_STRING_DESCRIPTION, (char*)description);
    send_string(ACCESSORY_STRING_VERSION, (char*)version);
    send_string(ACCESSORY_STRING_URI, (char*)url);
    send_string(ACCESSORY_STRING_SERIAL, (char*)serial);

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
        int tmpRes = search_for_device(context, &idVendor, &idProduct);
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

/*
 * getProtocol() - retrieve AOA protocol version
 * @ret
 * -1 : Error (libusb_bulk_transfer error code)
 *  >0 : Protocol version
 */
int AndrOA::get_protocol()
{
    unsigned short protocol;
    unsigned char buffer[2];
    int tmpRes;

    tmpRes = libusb_control_transfer(device_handle, 0xc0,
            ACCESSORY_GET_PROTOCOL, 0, 0, buffer, 2, 0);
    if(tmpRes < 0){
        return -1;
    }
    protocol = buffer[1] << 8 | buffer[0];
    return((int)protocol);
}

/*
 * send_string() - send accessory identifications
 * @args
 * index   : string ID
 * str     : identification string(zero terminated UTF8 string)
 *
 * @ret
 * <0 : Error (libusb_bulk_transfer error code)
 * 0 : Success
 */
int AndrOA::send_string(int index, const char *str)
{
    int tmpRes;
    tmpRes = libusb_control_transfer(device_handle, 0x40,
            ACCESSORY_SEND_STRING, 0, index,
            (unsigned char*)str, strlen(str) + 1, 0);
    return(tmpRes);
}

/*
 * search_for_device() -  Search for AOA support device in the all USB devices
 * @args
 * ctx      : libusb_context
 * idVendor : return buffer for USB Vendor ID
 * idProduc : return buffer for USB Product ID
 *
 * @ret
 * -1 : AOA device not found
 *  >0 : AOA Version
 */
int AndrOA::search_for_device(libusb_context *context, uint16_t *idVendor, uint16_t *idProduct)
{
    int tmpRes = -1;
    int i;
    libusb_device **devices;
    libusb_device *device;
    struct libusb_device_descriptor desc;
    ssize_t device_count;

    *idVendor = *idProduct = 0;
    device_count = libusb_get_device_list(ctx, &devices);
    if(device_count < 0){
        printf("Get device error.\n");
        return -1;
    }

    //Go thorugh and enumerate devices
    for(i=0; i<device_count; i++){
        device = devices[i];
        libusb_get_device_descriptor(device, &desc);
#ifdef DEBUG
        printf("VID:%04X, PID:%04X Class:%02X\n", desc.idVendor, desc.idProduct, desc.bDeviceClass);
#endif
        //Ignore non target device
        if( desc.bDeviceClass != 0 ){
            continue;
        }

        //Already Android accessory mode ?
        if(desc.idVendor == USB_ACCESSORY_VENDOR_ID &&
                (desc.idProduct >= USB_ACCESSORY_PRODUCT_ID &&
                 desc.idProduct <= USB_ACCESSORY_AUDIO_ADB_PRODUCT_ID)
          ){
#ifdef DEBUG
            printf("already in accessory mode.\n");
#endif
            tmpRes = 0;
            break;
        }

        //Checking the android accessory capability.
        if((device_handle = libusb_open_device_with_vid_pid(context, desc.idVendor,  desc.idProduct)) == NULL) {
            printf("Device open error.\n");
        } else {
            libusb_claim_interface(device_handle, 0);
            tmpRes = get_protocol();
            libusb_release_interface (device_handle, 0);
            libusb_close(device_handle);
            device_handle = NULL;
            if( tmpRes != -1 ){
#ifdef DEBUG
                printf("Android accessory protocol version: %d\n", verProtocol);
#endif
                break; //AOA found.
            }
        }
    }

    // find end point number
    if( find_end_point(device) < 0 ){
        printf("Endpoint not found.\n");
        tmpRes = -1;
    }
    *idVendor = desc.idVendor;
    *idProduct = desc.idProduct;
#ifdef DEBUG
    printf("VID:%04X, PID:%04X\n", *idVendor, *idProduct);
#endif
    return tmpRes;
}

/* findEndPoint() -  find end point number
 * arg
 * device : libusb_device
 * return
 * 0 : Success
 * -1 : Valid end point not found
 */
int AndrOA::find_end_point(libusb_device *device)
{
    struct libusb_config_descriptor *config;
    libusb_get_config_descriptor (device, 0, &config);

    //initialize end point number
    inEP = outEP = 0;

    //Evaluate first interface and endpoint descriptor
#ifdef DEBUG
    printf("bNumInterfaces: %d\n", config->bNumInterfaces);
#endif
    const struct libusb_interface *itf = &config->interface[0];
    struct libusb_interface_descriptor ifd = itf->altsetting[0];
#ifdef DEBUG
    printf("bNumEndpoints: %d\n", ifd.bNumEndpoints);
#endif
    for(int i=0; i<ifd.bNumEndpoints; i++){
        struct libusb_endpoint_descriptor epd;
        epd = ifd.endpoint[i];
        if( epd.bmAttributes == 2 ) { //Bulk Transfer ?
            if( epd.bEndpointAddress & 0x80){ //IN
                if( inEP == 0 )
                    inEP = epd.bEndpointAddress;
            }else{                            //OUT
                if( outEP == 0 )
                    outEP = epd.bEndpointAddress;
            }
        }
#ifdef DEBUG
        printf(" bEndpointAddress: %02X, bmAttributes:%02X\n", epd.bEndpointAddress, epd.bmAttributes);
#endif
    }
    if( outEP == 0 || inEP == 0) {
        return -1;
    }else{
        return 0;
    }
}
