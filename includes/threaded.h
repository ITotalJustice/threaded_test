#ifndef _THREADED_H_
#define _THREADED_H_

#include <stdio.h>
#include <stdint.h>


typedef enum
{
    ThreadState_InProgress = 0x0,   // lock.
    ThreadState_Waiting = 0x1,      // waiting for data.
} ThreadState;

typedef enum
{
    ThreadStart_On,
    ThreadStart_Off
} ThreadStart;

typedef struct
{
    void *data;
    size_t data_size;
    size_t data_written;
    size_t file_size;

    FILE *in_file;
    FILE *out_file;
} ThreadDataStruct_t;


//
int thread_read_func(void *in);

//
int thread_write_func(void *in);

#endif