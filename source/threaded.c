#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <threads.h>

#include "threaded.h"
#include "util.h"


// globals
static mtx_t g_mtx;
static bool g_write = false;


void thread_mutex_init()
{
    mtx_init(&g_mtx, mtx_plain);
}

void thread_mutex_close()
{
    mtx_destroy(&g_mtx);
}

int thread_read_func(void *in)
{
    ThreadDataStruct_t *t_struct = (ThreadDataStruct_t *)in;

    void *buf_temp = malloc(_8MiB);

    for (size_t buf_size = _8MiB, offset = 0; offset < t_struct->file_size; offset += buf_size)
    {
        if (buf_size + offset > t_struct->file_size)
            buf_size = t_struct->file_size - offset;
        
        fread(buf_temp, buf_size, 1, t_struct->in_file);

        mtx_lock(&g_mtx);
        t_struct->data_size = buf_size;
        memcpy(t_struct->data, buf_temp, buf_size);
        g_write = true;
        mtx_unlock(&g_mtx);
    }

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
        g_write = false;
        mtx_unlock(&g_mtx);
    }
    return 0;
}