#ifndef HELPERS_H
#define HELPERS_H

#include <fcntl.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct entry
{
    struct stat st;
    char hash[41];
    size_t path_length;
    char *path;
};

int count_char(const char *file_in);
char *get_content(const char *file_in);

void make_sha(unsigned char *final, int final_size, char *sha);

unsigned char *make_object(char *path, char *sha, int *final_size);

int check_file(char *path);

struct entry make_entry(char *path, char *sha, int *final_size);

#endif // HELPERS_H
