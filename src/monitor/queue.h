#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <fcntl.h>
#include <mqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_QUEUE_MSG_SIZE 2048
#define MAX_QUEUE_MSG_NUMBER 10

/*
 * Direction is O_RDONLY o O_WRONLY
 */
bool open_queue (const char *path, int direction, mqd_t * queue);

bool close_queue (const char *path, mqd_t queue);

bool write_queue (mqd_t queue, void *buffer, size_t size);

bool read_queue (mqd_t queue, void *buffer, size_t buffer_size, size_t * size);

#ifdef __cplusplus
}
#endif
