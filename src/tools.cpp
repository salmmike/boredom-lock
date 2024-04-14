#include "tools.h"

#include <sstream>
#include <stdio.h>
#include <usb.h>

std::vector<usb_id>
list_usb()
{
    std::vector<usb_id> found_devices{};

    struct usb_bus* bus;
    struct usb_device* dev;
    usb_init();
    usb_find_busses();
    usb_find_devices();

    for (bus = usb_busses; bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            found_devices.push_back(
              { dev->descriptor.idVendor, dev->descriptor.idProduct });
            found_devices.back().print();
        }
    }

    return found_devices;
}