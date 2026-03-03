#include "control_loop.h"
#include "queue.h"

#include "ipc_protocol_generated.h"

#include <limits.h>

#include <iostream>
#include <iomanip>

using namespace core;

static void reply_ok (const char *fifo_name)
{
    mqd_t output_queue;

    if (!open_queue (fifo_name, O_WRONLY, &output_queue))
        return;

    flatbuffers::FlatBufferBuilder builder (1024);

    // 1. Create the Ok
    // Even if it's empty, we call the Create function to get an offset
    auto ok_offset = CreateOk (builder);

    // 2. Crea le stringhe necessarie
    auto return_path = builder.CreateString (INPUT_QUEUE);

    // 3. Create the root Message table
    // We pass Payload_Ok as the type and the ok_offset as the value
    auto message_offset = CreateMessage (builder, return_path,
                                         Payload_Ok,
                                         ok_offset.Union ());

    // 4. Finalize the buffer
    builder.Finish (message_offset);

    uint8_t *buf = builder.GetBufferPointer ();
    size_t size = builder.GetSize ();

    write_queue (output_queue, buf, size);

    close_queue (fifo_name, output_queue);
}

static void reply_error (const char *fifo_name)
{
    int output_queue;

    if (!open_queue (fifo_name, O_WRONLY, &output_queue))
        return;

    flatbuffers::FlatBufferBuilder builder (1024);

    // 1. Create the Error
    // Even if it's empty, we call the Create function to get an offset
    auto error_offset = CreateError (builder);

    // 2. Crea le stringhe necessarie
    auto return_path = builder.CreateString (INPUT_QUEUE);

    // 3. Create the root Message table
    // We pass Payload_Error as the type and the ok_offset as the value
    auto message_offset = CreateMessage (builder, return_path,
                                         Payload_Error,
                                         error_offset.Union());

    // 4. Finalize the buffer
    builder.Finish (message_offset);

    uint8_t *buf = builder.GetBufferPointer ();
    size_t size = builder.GetSize ();

    write_queue (output_queue, buf, size);

    close_queue (fifo_name, output_queue);
}

static void send_message (const char *fifo_name, uint8_t * buf, size_t size)
{
    int output_queue;

    if (!open_queue (fifo_name, O_WRONLY, &output_queue))
        return;

    write_queue (output_queue, buf, size);

    close_queue (fifo_name, output_queue);
}

void control_loop (bool *exit_flag)
{
    int input_queue;

    if (!open_queue (INPUT_QUEUE, O_RDONLY, &input_queue))
        return;

    while (!*exit_flag)
    {
        std::vector<uint8_t> buffer(MAX_QUEUE_MSG_SIZE);
        size_t size = 0;

        if (read_queue (input_queue, buffer.data(), buffer.size(), &size))
        {
            if (size == 0)
            {
                /* empty message */
                continue;
            }

            auto message = GetMessage(buffer.data());

            switch (message->payload_type ())
            {
                case Payload_GetRunningStateRequest:
                {
		    std::cout << "Received Payload_GetRunningStateRequest" << std::endl;
                    flatbuffers::FlatBufferBuilder builder (1024);

                    auto response =
                        CreateGetRunningStateResponse (builder, 
                                                       RunningState_FactoryDefault);

                    auto return_path = builder.CreateString (INPUT_QUEUE);
                    auto payload_type = Payload_GetRunningStateResponse;

                    auto m = CreateMessage (builder, return_path,
                                            payload_type, response.Union ());

                    builder.Finish (m);

                    uint8_t* buf = builder.GetBufferPointer ();
                    size_t size = builder.GetSize ();

                    send_message (message->return_path ()->c_str (), buf, size);

                    break;
                }

                case Payload_CreateInitialCredentialsRequest:
                    reply_ok (message->return_path ()->c_str ());
                    break;

                default:
                    reply_error (message->return_path ()->c_str ());
                    break;
            }
        }
    }

    close_queue (INPUT_QUEUE, input_queue);
}
