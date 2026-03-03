import flatbuffers
import os, fcntl, posix_ipc
from core import Message
from core import Payload
from core import GetRunningStateRequest, GetRunningStateResponse, RunningState

WEBUI_QUEUE = "/webui"
MONITOR_QUEUE = "/monitor"

def send_and_receive (output_buffer: str) -> Message:
    message = None

    input_queue = posix_ipc.MessageQueue (WEBUI_QUEUE,
                                          posix_ipc.O_CREAT | posix_ipc.O_RDONLY,
                                          mode=0o666) 

    output_queue = posix_ipc.MessageQueue (MONITOR_QUEUE,posix_ipc.O_CREAT)

    output_queue.send (output_buffer.decode('utf-8'))

    input_buffer, priority = input_queue.receive ()

    if not input_buffer:
        message = None

    message = Message.Message.GetRootAsMessage (input_buffer, 0)

    return message;

def is_factory_default_state () -> bool:
    builder = flatbuffers.Builder(1024)

    GetRunningStateRequest.Start (builder)
    request_payload = GetRunningStateRequest.End (builder)

    path = builder.CreateString (WEBUI_QUEUE)

    Message.Start (builder)
    Message.AddReturnPath (builder, path)
    Message.AddPayloadType (builder,
                            Payload.Payload ().GetRunningStateRequest)
    Message.AddPayload (builder, request_payload)
    request = Message.End (builder)

    builder.Finish (request)
    buffer = builder.Output ()

    response = send_and_receive (buffer)

    if not response:
        return True

    if response.PayloadType () == Payload.Payload ().GetRunningStateResponse:
        r = GetRunningStateResponse.GetRunningStateResponse ()

        union_obj = response.Payload()
        r.Init (union_obj.Bytes, union_obj.Pos)

        state = r.State ()

        return state == RunningState.RunningState ().FactoryDefault
