
#include <fcntl.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "helpers.h"
#include "minigit.h"
#include "index.h"

int add(int argc, char *path[])
{
    int len = argc;

    for (int i = 0; i < len; i++)
    {
        if (check_file(path[i])!=0)
            return 1;
    }

    for (int i = 0; i < len; i++)
    {
        int final_size;
        char sha[SHA_DIGEST_LENGTH * 2 + 1];
        unsigned char *final = make_object(path[i], sha, &final_size);
        if (final == NULL)
            return 1;
        char *str = malloc(sizeof(char) * (2 + 1));

        if (str == NULL)
        {
            free(final);
            return 1;
        }

        str[0] = sha[0];
        str[1] = sha[1];
        str[2] = '\0';

        char *name_file = sha + 2;

        char final_str[100];

        sprintf(final_str, ".mgit/objects/%s", str);

        if (mkdir(final_str, 0755) == -1)
        {
            /* Le dossier existe peut-être déjà */
        }

        char fileName[1024];

        sprintf(fileName, "%s/%s", final_str, name_file);

        FILE *fptr;
        fptr = fopen(fileName, "w");

        if (fptr == NULL)
        {
            free(str);
            free(final);
            return 1;
        }

        fwrite(final, sizeof(char), final_size, fptr);
        fclose(fptr);

        free(str);
        free(final);
    }

    // MAKE_INDEX
    make_index(path, argc);

    return 0;
}
