#include "configuration.h"
#include "panic.h"

#include <blkid/blkid.h>
#include <sys/mount.h>
#include <sys/stat.h>

#include <stdlib.h>

bool mount_configuration ()
{
    struct stat sb;
    char *device;

    /* does the mount point exist? If not, let's create it */
    if (stat (CONFIGURATION_MOUNT_POINT, &sb) == 0 &&
        S_ISDIR(sb.st_mode)) {
        /* present, doing nothing */
    }
    else
    {
        /* missing, let's create it */
        if (mkdir (CONFIGURATION_MOUNT_POINT, 0755) != 0)
        {
            panic ("Unable to create missing mount point");

            return false;
        }
    }

    /* let's retrieve the device based on the partition label */
    device = blkid_get_devname (NULL, "LABEL", CONFIGURATION_LABEL);

    if (!device)
    {
        panic ("Unable to get device by label");

        return false;
    }

    /* here we can finally mount the device */
    mount (device, CONFIGURATION_MOUNT_POINT, "ext4", 0, NULL);

    free (device);

    return true;
}
