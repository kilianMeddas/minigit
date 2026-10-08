#define _POSIX_C_SOURCE 200809L
#include "helpers.h"

int count_char(const char *file_in)
{
    FILE *fd = fopen(file_in, "r");
    if (fd == NULL)
        return -1;

    int count = 0;
    int c;

    while ((c = fgetc(fd)) != EOF)
    {
        count++;
    }

    if (fclose(fd) != 0)
    {
        return -1;
    }

    return count;
}

char *get_content(const char *file_in)
{
    long numbytes;
    char *buffer;
    FILE *fd = fopen(file_in, "r");

    if (fd == NULL)
        return NULL;

    fseek(fd, 0L, SEEK_END);
    numbytes = ftell(fd);
    fseek(fd, 0L, SEEK_SET);

    buffer = calloc(numbytes + 1, sizeof(char));

    if (buffer == NULL)
    {
        fclose(fd);
        return NULL;
    }

    fread(buffer, sizeof(char), numbytes, fd);
    fclose(fd);

    return buffer;
}

void make_sha(unsigned char *final, int final_size, char *sha)
{
    unsigned char hash[SHA_DIGEST_LENGTH];

    SHA1(final, final_size, hash);

    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        sprintf(sha + i * 2, "%02x", hash[i]);
    }

    sha[SHA_DIGEST_LENGTH * 2] = '\0';
}

unsigned char *make_object(char *path, char *sha, int *final_size)
{
    char *type = "blob ";
    unsigned char *final;
    char content_size[20];
    char *content = get_content(path);

    if (content == NULL)
        return NULL;

    sprintf(content_size, "%d", count_char(path));

    *final_size = strlen(type) + strlen(content_size) + 1 + strlen(content);

    final = malloc(*final_size);

    if (!final)
    {
        free(content);
        return NULL;
    }

    char header[100];

    int pos = sprintf(header, "%s%s", type, content_size);

    memcpy(final, header, pos);
    final[pos] = '\0';
    pos++;

    memcpy(final + pos, content, strlen(content));

    make_sha(final, *final_size, sha);

    free(content);

    return final;
}

int check_file(char *path)
{
    struct stat path_stat;

    if (lstat(path, &path_stat) != 0)
    {
        perror("One or more path does not exist");
        return 1;
    }

    if (!S_ISREG(path_stat.st_mode))
    {
        perror("One or more path are not a regular file");
        return 1;
    }

    return 0;
}

struct entry make_entry(char *path, char *sha, int *final_size)
{
    struct entry e = { 0 };
    struct stat st;

    if (lstat(path, &st) == -1)
    {
        perror("Failed to get file stats");
        e.path = NULL;
        return e;
    }

    unsigned char *final = make_object(path, sha, final_size);

    if (final == NULL)
    {
        e.path = NULL;
        return e;
    }

    free(final);

    e.st = st;

    size_t str_len = strlen(path);
    e.path_length = str_len;
    e.path = path;

    strcpy(e.hash, sha);

    return e;
}
