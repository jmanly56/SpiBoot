#include "bootloader.h"

// InterruptIn irq(D8);
SPISlave slave(PB_15, PB_14, PB_13, PB_12);

int main()
{
        init_slave();
        // irq.rise(await_data);
        // mbed_event_queue()->dispatch_forever();
        process_data();
        mbed_start_application(POST_APPLICATION_ADDR);
        return EXIT_SUCCESS;
}