#ifndef _USB_H_
#define _USB_H_

#include <stdint.h>


typedef enum
{
    UsbMode_Exit,
    UsbMode_Read,
    UsbMode_Write
} UsbMode;

typedef struct
{
    uint8_t mode;
    uint8_t padding[0x7];
    uint64_t offset_start;
    uint64_t data_size;
    uint8_t unused[0x18];
} usb_struct_t;


// read into void *out return size of data read.
size_t usb_read(void *out, size_t len, uint64_t offset);

// write from void *in, return the size of the data written.
size_t usb_write(const void *in, size_t len);

// call this before every read.
void usb_poll(uint8_t mode, size_t offset, size_t data_size);

// call this to exit usb comms.
void usb_exit(void);

#endif