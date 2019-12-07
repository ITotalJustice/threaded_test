#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <threads.h>

#include "threaded.h"
#include "usb.h"
#include "util.h"


// globals
static mtx_t g_mtx;


void thread_mutex_init()
{
    mtx_init(&g_mtx, mtx_plain);
}

void thread_mutex_close()
{
    mtx_destroy(&g_mtx);
}

void thread_spawner(ThreadStruct_t *t_struct, void *func, void *in)
{
    thrd_create(&t_struct->t, func, in);
}

void thread_wait_until_exit(ThreadStruct_t *t_struct)
{
    thrd_join(t_struct->t, NULL);
}

int thread_read_func(void *in)
{
    ThreadDataStruct_t *t_struct = (ThreadDataStruct_t *)in;

    void *buf_temp = memalign(0x1000, _8MiB);

    for (size_t buf_size = _8MiB, offset = 0; offset < t_struct->file_size; offset += buf_size)
    {
        if (buf_size + offset > t_struct->file_size)
            buf_size = t_struct->file_size - offset;
        
        usb_read(buf_temp, buf_size, offset);
        //fread(buf_temp, buf_size, 1, t_struct->in_file);

        mtx_lock(&g_mtx);
        t_struct->data_size = buf_size;
        memcpy(t_struct->data, buf_temp, buf_size);
        mtx_unlock(&g_mtx);
    }

    usb_exit();
    return 0;
}

int thread_write_func(void *in)
{
    ThreadDataStruct_t *t_struct = (ThreadDataStruct_t *)in;

    while (t_struct->data_written != t_struct->file_size)
    {
        mtx_lock(&g_mtx);
        fwrite(t_struct->data, t_struct->data_size, 1, t_struct->out_file);
        t_struct->data_written += t_struct->data_size;
        t_struct->data_size = 0;
        mtx_unlock(&g_mtx);
    }

    return 0;
}