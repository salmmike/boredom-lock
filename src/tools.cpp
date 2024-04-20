#include "tools.h"

#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <usb.h>

bool
usb_id_is_connected(const usb_id& device_id)
{
    const auto devices = list_usb();
    return std::find(std::begin(devices), std::end(devices), device_id) !=
           std::end(devices);
}

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
            found_devices.push_back({
              dev->descriptor.idVendor,
              dev->descriptor.idProduct,
            });
        }
    }

    return found_devices;
}

usb_id
usb_id_from_string(const std::string& id)
{
    std::stringstream ss;
    std::string_view id_v(id);
    uint16_t vid;
    uint16_t pid;

    auto delimiter = id_v.find(':');

    if (delimiter == std::string::npos) {
        return usb_id{};
    }

    ss << std::hex << id_v.substr(0, delimiter);
    ss >> vid;
    ss.clear();

    ss << std::hex << id_v.substr(delimiter + 1);
    ss >> pid;

    return usb_id(vid, pid);
}

std::vector<usb_id>
init_find_device()
{
    return list_usb();
}

std::vector<usb_id>
find_device(std::vector<usb_id> previous)
{
    const auto current = list_usb();
    std::vector<usb_id> not_found;

    for (const auto& dev : current) {
        if (std::find(std::begin(previous), std::end(previous), dev) ==
            std::end(previous)) {
            not_found.push_back(dev);
        }
    }

    return not_found;
}
