#include "index_printer.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "print_helper.h"

#define HEADER_SIZE 8
#define HASH_SIZE 40

static void read_exact(void *buf, size_t size, FILE *f, const char *what)
{
    if (fread(buf, 1, size, f) != size)
        errx(EXIT_FAILURE, "Error while reading %s!", what);
}

static void print_entry_item(FILE *f)
{
    struct stat st;
    char hash[HASH_SIZE];
    size_t path_len;

    print_obj_start(NULL);

    // READ FILE'S STATS
    read_exact(&st, sizeof(st), f, "entry stat");
    print_stat_item("st", &st);

    // READ HASH
    read_exact(hash, HASH_SIZE, f, "entry hash");
    print_str_item("hash", hash, HASH_SIZE);

    // READ PATH LENGHT
    read_exact(&path_len, sizeof(path_len), f, "entry path length");
    print_num_item("path_len", path_len);

    char *path = malloc(path_len ? path_len : 1);
    if (!path)
        err(EXIT_FAILURE, "malloc");

    // READ FILENAME
    read_exact(path, path_len, f, "entry path");
    print_str_item("path", path, path_len);
    free(path);

    print_obj_end();
}

void index_pretty_print(const char *filename)
{
    FILE *f = fopen(filename, "r");

    if (!f)
        errx(EXIT_FAILURE, "%s", filename);

    char header[HEADER_SIZE];
    size_t nb_entries;
    char checksum[HASH_SIZE];

    print_obj_start(NULL);

    // READ HEADER
    read_exact(header, HEADER_SIZE, f, "header");
    print_str_item("header", header, HEADER_SIZE);

    // READ ENTRY NUMBER
    read_exact(&nb_entries, sizeof(nb_entries), f, "entries number");
    print_num_item("entries_number", nb_entries);

    // READ ENTRIES
    print_array_start("entries");
    for (size_t i = 0; i < nb_entries; ++i)
        print_entry_item(f);
    print_array_end();

    // READ CHECKSUM
    read_exact(checksum, HASH_SIZE, f, "checksum");
    print_str_item("checksum", checksum, HASH_SIZE);

    print_obj_end();

    fclose(f);
}
