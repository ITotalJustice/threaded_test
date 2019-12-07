#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <switch.h>

#include "usb.h"
#include "threaded.h"
#include "util.h"


void app_init()
{
    consoleInit(NULL);
    usbCommsInitialize();
}

void app_exit()
{
    consoleExit(NULL);
    usbCommsExit();
}

int main(int argc, char *argv[])
{
    app_init();

    ThreadDataStruct_t ptr;
    ptr.out_file = open_file("out_file", "wb");
    ptr.in_file = open_file("in_file", "rb");
    ptr.data = malloc(_8MiB);
    ptr.data_size = 0;
    ptr.data_written = 0;
    ptr.file_size = 0x100000000;

    thread_mutex_init();

    ThreadStruct_t t_1;
    ThreadStruct_t t_2;

    thread_spawner(&t_1, thread_read_func, &ptr);
    thread_spawner(&t_2, thread_write_func, &ptr);

    print_message_display("waiting for thread 1 to finish\n");
    thread_wait_until_exit(&t_1);
    print_message_display("waiting for thread 2 to finish %d\n");
    thread_wait_until_exit(&t_2);

    fclose(ptr.in_file);
    fclose(ptr.out_file);
    free(ptr.data);
    thread_mutex_close();

    print_message_loop_lock("exiting\n\n");
    app_exit();
    return 0;
}