#ifndef _THREADED_H_
#define _THREADED_H_

#include <stdint.h>
#include <machine/_threads.h>

typedef struct
{
    thrd_t t;
} ThreadStruct_t;

typedef struct
{
    void *data;
    size_t data_size;
    size_t data_written;
    size_t file_size;
} ThreadDataStruct_t;


//
void thread_mutex_init();

//
void thread_mutex_close();

//
void thread_spawner(ThreadStruct_t *t_struct, void *func, void *in);

//
void thread_wait_until_exit(ThreadStruct_t *t_struct);

//
int thread_read_func(void *in);

//
int thread_write_func(void *in);

#endif