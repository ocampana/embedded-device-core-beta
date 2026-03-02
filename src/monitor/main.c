#include "configuration.h"
#include "control_loop.h"

int main (int argc, char *argv[])
{
    bool exit_flag;

    mount_configuration ();

    exit_flag = false;
    control_loop (&exit_flag);

    return 0;
}
