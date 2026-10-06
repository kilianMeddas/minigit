#ifndef PRINT_HELPER_H
#define PRINT_HELPER_H

#include <stddef.h>
#include <sys/stat.h>

typedef void (*deserializer)(const void *);

void inc_padding();

void dec_padding();

void print_str_item(const char *key, const char *value, size_t n);

void print_num_item(const char *key, size_t value);

void print_stat_item(const char *key, const struct stat *value);

void print_obj_start(const char *key);

void print_obj_end(void);

void print_array_start(const char *key);

void print_array_end(void);

#endif /* !PRINT_HELPER_H */
