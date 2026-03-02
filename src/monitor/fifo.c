#include "fifo.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <limits.h>

bool open_fifo (const char *path, int direction, int *fd)
{
    int i;
    struct stat *st;

    unlink (path);

    if (mkfifo (path,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1)
        return false;

    *fd = open (path, direction, 0);

    return true;
}

bool close_fifo (const char *path, int fd)
{
    close (fd);
    unlink (path);

    return true;
}

bool write_fifo (int fd, void *buffer, size_t size)
{
    if (size > PIPE_BUF)
        /* we are not guaranteed to write atomically */
        return false;

    if (write (fd, buffer, size) == size)
        return true;

    return false;
}

bool read_fifo (int fd, void *buffer, size_t buffer_size, size_t * size)
{
    if (buffer_size < PIPE_BUF)
        /* we may not be able to store the whole message */
        return false;

    *size = read (fd, buffer, buffer_size);

    if (*size == -1)
        return false;

    return true;
}
