#ifndef INDEX_H
#define INDEX_H

#include "helpers.h"

void create_index(char *path[], int argc);

void write_entries(FILE *file, struct entry *entries, uint64_t number_entries);

void add_new_index(char *path[], int argc, uint64_t number_entries,
                   struct entry *old_entries);

void read_entry(FILE *file, struct entry *entry);

void add_index_sha(void);

void make_index(char *path[], int argc);

int compare(const void *a, const void *b);

#endif // INDEX_H
