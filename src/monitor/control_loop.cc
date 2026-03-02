#include "control_loop.h"
#include "fifo.h"

#include "ipc_protocol_generated.h"

#include <limits.h>

#include <iostream>
#include <iomanip>

using namespace core;

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

        memset (buffer, 0, PIPE_BUF);

        if (read_fifo (input_fifo, buffer, PIPE_BUF, &size))
        {
            flatbuffers::Verifier verifier((uint8_t*) buffer, size);

            const core::Message* message = core::GetMessage(buffer);
            printf ("%p\n", message);
            printf ("%s\n", message->return_path()->str());
        }
exit(0);
    }

    close_fifo (INPUT_FIFO, input_fifo);
}
