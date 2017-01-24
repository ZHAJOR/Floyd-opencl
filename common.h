#ifndef COMMON_H
#define COMMON_H

#define _POSIX_C_SOURCE 199309L
#include <time.h>

struct timespec get_time(void);
struct timespec time_diff(struct timespec* ts1, struct timespec* ts2);
struct timespec get_duration(struct timespec* ts);
void print_time(struct timespec* ts);

int* read_int_array(unsigned n);
void read_input(int** array, unsigned* n);
void print_array(int* array, unsigned n);

#endif
