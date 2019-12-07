import usb
import struct
import sys
import os
from pathlib import Path

file_name = ""

SEEK_SET = 0
SEEK_CUR = 1
SEEK_END = 2

USB_EXIT = 0
USB_READ = 1
USB_WRITE = 2


def read_nsp(range_size, range_offset, out_ep):
    with open(file_name, "rb") as f:
        f.seek(range_offset)

        curr_off = 0x0
        end_off = range_size
        read_size = 0x800000 # 1MiB

        while curr_off < end_off:
            if curr_off + read_size >= end_off:
                read_size = end_off - curr_off

            buf = f.read(read_size)
            out_ep.write(data=buf, timeout=0)
            curr_off += read_size
            print("reading {} - {}".format(curr_off, end_off))


def get_file_size(file):
    with open(file, "rb") as f:
        f.seek(0, SEEK_END)
        return f.tell


def send_file_info(in_ep, out_ep):
    get_file_size(file_name)


def wait_for_input(in_ep, out_ep):
    print("now waiting for intput\n")

    while True:
        # read-in data sent from switch.
        file_range_header = in_ep.read(0x30, timeout=0)

        mode = struct.unpack('<B', file_range_header[0:1])[0]
        range_offset = struct.unpack('<Q', file_range_header[8:0x10])[0]
        range_size = struct.unpack('<Q', file_range_header[0x10:0x18])[0]
        print("mode = {} | range offset = {} | range size = {}".format(mode, range_offset, range_size))

        if (mode == USB_EXIT):
            break
        elif (mode == USB_READ):
            read_nsp(range_size, range_offset, out_ep)


if __name__ == '__main__':

    args = len(sys.argv)
    if (args != 2):
        print("not enough args")
        sys.exit(1)

    file_name = Path(sys.argv[1])

    if not file_name.is_file():
        raise ValueError('must be a file!!!!!')

    # Find the switch
    print("waiting for to find the switch...\n")
    dev = usb.core.find(idVendor=0x057E, idProduct=0x3000)

    while (dev is None):
        dev = usb.core.find(idVendor=0x057E, idProduct=0x3000)

    print("found the switch!\n")

    dev.reset()
    dev.set_configuration()
    cfg = dev.get_active_configuration()

    is_out_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_OUT
    is_in_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_IN
    out_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_out_ep)
    in_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_in_ep)

    assert out_ep is not None
    assert in_ep is not None

    wait_for_input(in_ep, out_ep)