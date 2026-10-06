#define _POSIX_C_SOURCE 200112L

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Print a file or folder name with a certain number of \t depending of the
 * depth
 */
static void print_element(const char *name, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf("|   ");
    }
    printf("%s\n", name);
}

/*
 * Recursive function that print the folder_name and each files contained in
 * said folder. Then do a recursive call for each sub-folder
 */
static void print_folder(const char *path, int depth, const char *folder_name)
{
    DIR *d = opendir(path);

    if (!d)
        return;

    print_element(folder_name, depth - 1);

    struct dirent *entry;

    while ((entry = readdir(d)))
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")
            || !strcmp(entry->d_name, ".mgit"))
            continue;

        size_t len = strlen(path) + strlen(entry->d_name) + 2;
        char *new_path = malloc(len);

        snprintf(new_path, len, "%s/%s", path, entry->d_name);

        struct stat st;

        if (lstat(new_path, &st) == -1)
        {
            free(new_path);
            continue;
        }

        if (S_ISDIR(st.st_mode))
            print_folder(new_path, depth + 1, entry->d_name);
        else
            print_element(entry->d_name, depth);

        free(new_path);
    }

    closedir(d);
}

void tree_pretty_print(const char *filename)
{
    print_folder(filename, 1, filename);
}
