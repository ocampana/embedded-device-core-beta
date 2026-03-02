#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Direction is O_RDONLY o O_WRONLY
 */
bool open_fifo (const char *path, int direction, int *fd);

bool close_fifo (const char *path, int fd);

bool write_fifo (int fd, void *buffer, size_t size);

bool read_fifo (int fd, void *buffer, size_t buffer_size, size_t * size);

#ifdef __cplusplus
}
#endif
