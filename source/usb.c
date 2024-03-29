#include <switch/runtime/devices/usb_comms.h>
#include "usb.h"


size_t usb_read(void *out, size_t len, u64 offset)
{
    usb_poll(UsbMode_Read, offset, len);
    return usbCommsRead(out, len);
}

size_t usb_write(const void *in, size_t len)
{
    return usbCommsWrite(in, len);
}

void usb_poll(uint8_t mode, size_t offset, size_t data_size)
{
    usb_struct_t usb_struct = { mode, {0}, offset, data_size, {0} };
    usb_write(&usb_struct, sizeof(usb_struct_t));
}

void usb_exit()
{
    usb_poll(UsbMode_Exit, 0, 0);
}