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



