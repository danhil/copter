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



#endif /* __AndrOA_h__ */
