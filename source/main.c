#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <switch.h>

#include <threads.h>

#include "usb.h"
#include "threaded.h"
#include "util.h"


void app_init()
{
    consoleInit(NULL);
    //usbCommsInitialize(); for when i try usb multi threading.
}

void app_exit()
{
    consoleExit(NULL);
    //usbCommsExit();
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
    ptr.file_size = get_filesize("in_file");

    thread_mutex_init();

    thrd_t t_1;
    thrd_t t_2;
    thrd_create(&t_1, thread_read_func, &ptr);
    thrd_create(&t_2, thread_write_func, &ptr);

    print_message_display("waiting for thread 1 to finish\n");
    thrd_join(t_1, NULL);
    print_message_display("waiting for thread 2 to finish %d\n", ptr.data_written);
    thrd_join(t_2, NULL);

    fclose(ptr.in_file);
    fclose(ptr.out_file);
    free(ptr.data);
    thread_mutex_close();

    print_message_loop_lock("exiting\n\n");
    app_exit();
    return 0;
}