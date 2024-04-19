#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <string>
#include <vector>

/// @brief Reperesents USB device with product and vendor ID.
struct usb_id
{
    /// @brief vendor ID.
    uint16_t vid;
    /// @brief product ID.
    uint16_t pid;

    void print() const { printf("%04x:%04x\n", vid, pid); }

    bool operator==(const usb_id rhs) const
    {
        return this->vid == rhs.vid && this->pid == rhs.pid;
    }
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

/// @brief Create a USB id from string
/// @param id USB VID_PID string
/// @return usb_id struct
usb_id
usb_id_from_string(const std::string& id);

#endif /* TOOLS_H */
