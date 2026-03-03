#include "queue.h"

#include <limits.h>

bool open_queue (const char *name, int direction, mqd_t* queue)
{
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_QUEUE_MSG_NUMBER;
    attr.mq_msgsize = MAX_QUEUE_MSG_SIZE;
    attr.mq_curmsgs = 0;

    *queue = mq_open (name, direction | O_CREAT, 
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
		      &attr);

    return *queue != -1;
}

bool close_queue (const char *name, mqd_t queue)
{
    mq_close (queue);

    return true;
}

bool write_queue (mqd_t queue, void *buffer, size_t size)
{
    int status;
   
    status = mq_send (queue, buffer, size, 0);

    return status == 0;
}

bool read_queue (mqd_t queue, void *buffer, size_t buffer_size, size_t * size)
{
    ssize_t s;
    s = mq_receive (queue, buffer, buffer_size, NULL);

    *size = (size_t) s;

    return s != -1;
}
