#pragma once

#include <stdbool.h>

#define INPUT_FIFO "/tmp/monitor.fifo"

extern "C"
{
    void control_loop (bool *exit_flag);
}
