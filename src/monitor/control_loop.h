#pragma once

#include <stdbool.h>

#define INPUT_FIFO "/tmp/monitor.fifo"

#ifdef __cplusplus
extern "C"
{
#endif

    void control_loop (bool *exit_flag);

#ifdef __cplusplus
}
#endif
