#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <switch.h>

#include "threaded.h"
#include "util.h"


static bool wait = false;
static bool write = false;


int thread_read_func(void *in)
{
    ThreadDataStruct_t *t_struct = (ThreadDataStruct_t *)in;

    for (size_t buf_size = _8MiB, offset = 0; offset < t_struct->file_size; offset += buf_size)
    {
        if (buf_size + offset > t_struct->file_size)
            buf_size = t_struct->file_size - offset;
        
        void *buf_temp = memalign(0x1000, buf_size);
        fread(buf_temp, buf_size, 1, t_struct->in_file);

        while (wait == true);
        
        t_struct->data = malloc(buf_size);
        t_struct->data_size = buf_size;
        memcpy(t_struct->data, buf_temp, buf_size);
        free(buf_temp);

        wait = true;
        write = true;
    }
    return 0;
}

int thread_write_func(void *in)
{
    ThreadDataStruct_t *t_struct = (ThreadDataStruct_t *)in;

    while (t_struct->data_written != t_struct->file_size)
    {
        if (write == true)
        {
            fwrite(t_struct->data, t_struct->data_size, 1, t_struct->out_file);
            t_struct->data_written += t_struct->data_size;

            free(t_struct->data);
            write = false;
            wait = false;
        }
    }
    return 0;
}