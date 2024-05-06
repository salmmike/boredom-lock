#ifndef TOOLS_H
#define TOOLS_H

#include <cstdint>
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
    std::string to_string() const
    {
        char buffer[11];
        snprintf(buffer, sizeof(buffer), "%04x:%04x", vid, pid);
        return std::string{ buffer };
    }
};

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
