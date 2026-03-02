#include "control_loop.h"
#include "fifo.h"

#include "ipc_protocol_generated.h"

#include <limits.h>

void control_loop (bool *exit_flag)
{
    int input_fifo;
    int output_fifo;

    if (!open_fifo (INPUT_FIFO, O_RDONLY, &input_fifo))
        return;

    while (!*exit_flag)
    {
        char buffer[PIPE_BUF];
        size_t size;

        if (read_fifo (input_fifo, buffer, PIPE_BUF, &size))
        {
        }
    }

    close_fifo (INPUT_FIFO, input_fifo);
}
