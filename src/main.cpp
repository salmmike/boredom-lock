#include "scheduler.h"
#include "tools.h"
#include <iostream>
#include <thread>

int
main()
{
    BoredomScheduler test("/opt/boredom-lock/config.ini");
    test.init();
    test.is_alarm();
    for (size_t i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        test.is_alarm();
    }
    return 0;
}
