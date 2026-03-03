#pragma once

#include <stdbool.h>

#define INPUT_QUEUE "/monitor"

#ifdef __cplusplus
extern "C"
{
#endif

    void control_loop (bool *exit_flag);

#ifdef __cplusplus
}
#endif
