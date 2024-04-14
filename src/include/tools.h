#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <string>
#include <vector>

struct usb_id
{
    int vid;
    int pid;

    void print() { printf("%04x:%04x\n", vid, pid); }
};

/// @brief Returns True if the device is connected via USB.
/// @param device_id The vid:pid (USB vendor and product ID) of the device.
/// @return true if the device is connected.
bool
usb_id_is_connected(const usb_id& device_id);

/// @brief List plugged USB devices.
/// @return List of USB vid:pid values of plugged devices.
std::vector<usb_id>
list_usb();

#endif /* TOOLS_H */
