#include "usbtracker.h"
#include <filesystem>
#include <fstream>

#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <usb.h>

int
hotplug_callback(struct libusb_context* ctx [[maybe_unused]],
                 struct libusb_device* dev,
                 libusb_hotplug_event event,
                 void* user_data)
{
    static libusb_device_handle* dev_handle = NULL;
    struct libusb_device_descriptor desc;

    (void)libusb_get_device_descriptor(dev, &desc);

    usb_id new_id{
        .vid = desc.idVendor,
        .pid = desc.idProduct,
    };

    auto tracker = static_cast<USBTracker*>(user_data);

    if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event) {
        tracker->handle_device_add_event(new_id);

    } else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event) {
        tracker->handle_device_remove_event(new_id);
        if (dev_handle) {
            libusb_close(dev_handle);
            dev_handle = NULL;
        }
    } else {
        std::cerr << "Unhandled event" << event << "\n";
    }
    return 0;
}

int
track_usb_devices(USBTracker* tracker)
{
    libusb_hotplug_callback_handle callback_handle;
    int rc;
    struct timeval blocktime
    {
        .tv_sec = 0, .tv_usec = 500,
    };

    libusb_init_context(NULL, NULL, 0);

    rc = libusb_hotplug_register_callback(NULL,
                                          LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
                                            LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                          0,
                                          LIBUSB_HOTPLUG_MATCH_ANY,
                                          LIBUSB_HOTPLUG_MATCH_ANY,
                                          LIBUSB_HOTPLUG_MATCH_ANY,
                                          hotplug_callback,
                                          tracker,
                                          &callback_handle);
    if (LIBUSB_SUCCESS != rc) {
        std::cerr << "Error creating a hotplug callback\n";
        libusb_exit(NULL);
        return EXIT_FAILURE;
    }

    while (tracker->is_running()) {
        libusb_handle_events_timeout_completed(NULL, &blocktime, NULL);
        usleep(1000UL);
    }

    libusb_hotplug_deregister_callback(NULL, callback_handle);
    libusb_exit(NULL);

    return 0;
}

USBTracker::~USBTracker()
{
    stop_tracking();
}

void
USBTracker::start_tracking()
{
    m_running = true;
    m_connected_devices = list_usb();
    m_thread = std::thread(track_usb_devices, this);
}

void
USBTracker::stop_tracking()
{
    m_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void
USBTracker::handle_device_add_event(const usb_id& dev)
{
    m_mtx.lock();
    m_connected_devices.push_back(dev);
    m_callback(m_user_data);
    m_mtx.unlock();
}

void
USBTracker::handle_device_remove_event(const usb_id& dev)
{
    m_mtx.lock();
    m_connected_devices.erase(std::remove(std::begin(m_connected_devices),
                                          std::end(m_connected_devices),
                                          dev),
                              std::end(m_connected_devices));
    m_callback(m_user_data);
    m_mtx.unlock();
}

bool
USBTracker::is_running() const
{
    return m_running;
}

void
USBTracker::join_thread()
{
    m_thread.join();
}

bool
USBTracker::usb_id_is_connected(const usb_id& device_id)
{
    m_mtx.lock();
    bool result = std::find(std::begin(m_connected_devices),
                            std::end(m_connected_devices),
                            device_id) != std::end(m_connected_devices);
    m_mtx.unlock();
    return result;
}

void
USBTracker::set_device_event_cb(std::function<void(void*)> callback)
{
    m_callback = callback;
}

void
USBTracker::set_event_cb_data(void* data)
{
    m_user_data = data;
}
