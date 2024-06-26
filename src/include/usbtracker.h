#ifndef USBTRACKER_H_
#define USBTRACKER_H_

#include "tools.h"
#include <functional>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

class USBTracker
{
  public:
    USBTracker(){};
    ~USBTracker();

    void start_tracking();
    void stop_tracking();

    void handle_device_add_event(const usb_id& dev);
    void handle_device_remove_event(const usb_id& dev);
    bool is_running() const;
    void join_thread();

    /// @brief Returns True if the device is connected via USB.
    /// @param device_id The vid:pid (USB vendor and product ID) of the device.
    /// @return true if the device is connected.
    bool usb_id_is_connected(const usb_id& device_id);

    void set_device_event_cb(std::function<void(void*)> callback);

    void set_event_cb_data(void* data);

  private:
    std::vector<usb_id> m_connected_devices;
    std::thread m_thread;
    std::atomic<bool> m_running;
    void* m_user_data;
    std::function<void(void*)> m_callback;
    std::mutex m_mtx;
};

#endif // USBTRACKER_H_