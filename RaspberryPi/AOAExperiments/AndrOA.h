/*
 * In using libusb to communicate with a usbdevice the following steps are performed:
 *
 ****Initialize the library by calling the function libusb_init and creating a session
 ****Call the function libusb_get_device_list to get a list of connected devices.
 * This creates an array of libusb_device containing all usb devices connected to the system.
 **** Loop through all these devices and check their options
 ***** Discover the one and open the device either by libusb_open or
 * libusb_open_device_with_vid_pid(when you know vendor and product id of the device) to open the device
 ***** Clear the list you got from libusb_get_device_list by using libusb_free_device_list
 ***** Claim the interface with libusb_claim_interface (requires you to know the interface numbers of device)
 **** I/O through bulk transfers etc.
 ***** Release the device by using libusb_release_interface
 ***** Close the device you openedbefore, by using libusb_close
 ***** Close the session by using libusb_exit
 */
#ifndef __AndrOA_h__
#define __AndrOA_h__

// #include <usb.h>
#include <libusb.h>

/* Product and vendor ids */
#define USB_AOA_VENDOR_ID             0x18D1 /* GOOGLE */
#define USB_AOA_PRODUCT_ID            0x2D00 /* AOA */
#define USB_AOA_ADB_PRODUCT_ID        0x2D01 /* AOA +ADB */
#define USB_AUDIO_PRODUCT_ID                0x2D02 /*AUDIO */
#define USB_AUDIO_ADB_PRODUCT_ID            0x2D03 /* AUDIO + ADB */
#define USB_AOA_AUDIO_PRODUCT_ID      0x2D04 /* AOA + AUDIO */
#define USB_AOA_AUDIO_ADB_PRODUCT_ID  0x2D05 /* AOA + AUDIO + ADB */

/* String IDs */
#define AOA_STRING_MANUFACTURER       0
#define AOA_STRING_MODEL              1
#define AOA_STRING_DESCRIPTION        2
#define AOA_STRING_VERSION            3
#define AOA_STRING_URI                4
#define AOA_STRING_SERIAL             5

/* AndrOA protocol defines */
#define AOA_GET_PROTOCOL              51
#define AOA_SEND_STRING               52
#define AOA_START                     53
#define AOA_REGISTER_HID              54
#define AOA_UNREGISTER_HID            55
#define AOA_SET_HID_REPORT_DESC       56
#define AOA_SEND_HID_EVENT            57
#define AOA_AUDIO_SUPPORT             58

#define VERSION                     "0.0"
#define BUGREPORT                   "hilton.daniel0@gmail.com"

class AndrOA {
    private:
        const char *manufacturer;
        const char *model;
        const char *description;
        const char *version;
        const char *url;
        const char *serial;
        uint8_t inEP;
        uint8_t outEP;
        int versionProtocol;

        libusb_context *context;
        struct libusb_device_handle* device_handle;

        /* Private- Functions to be implemented */
        int search_for_device(libusb_context *context, uint16_t *idVendor,
                uint16_t *idProduct);
        int find_end_point(libusb_device *device);
        int is_an_accessory_device (libusb_context *context);
        int get_protocol(void);
        int send_string(int index, const char *str);

    public:
        AndrOA(const char *manufacturer,
                const char *model,
                const char *description,
                const char *version,
                const char *url,
                const char *serial);

        ~AndrOA();

        /* Public fuctions */
        int connect_to_accessory(void);
        int read(unsigned char *buffer, int len, unsigned int timeout);
        int write(unsigned char *buffer, int len, unsigned int timeout);
};



#endif /* __AndrOA_h__ */
