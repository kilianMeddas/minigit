#include "index.h"

int compare(const void *a, const void *b)
{
    const struct entry *entry_a = a;
    const struct entry *entry_b = b;

    return strcmp(entry_a->path, entry_b->path);
}

void add_index_sha(void)
{
    FILE *file = fopen(".mgit/index", "rb");

    if (file == NULL)
        return;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *content = malloc(file_size);

    if (content == NULL)
    {
        fclose(file);
        return;
    }

    fread(content, 1, file_size, file);
    fclose(file);

    char sha[SHA_DIGEST_LENGTH * 2 + 1];

    make_sha(content, file_size, sha);

    free(content);

    file = fopen(".mgit/index", "ab");

    if (file == NULL)
        return;

    fwrite(sha, sizeof(char), 40, file);

    fclose(file);
}
void create_index(char *path[], int argc)
{
    uint64_t number_entries = argc;

    struct entry *entries = malloc(sizeof(struct entry) * argc);

    if (!entries)
        return;

    for (int i = 0; i < argc; i++)
    {
        int final_size;
        char sha[SHA_DIGEST_LENGTH * 2 + 1];

        entries[i] = make_entry(path[i], sha, &final_size);
    }

    size_t header_len = 16;
    char *header = malloc(header_len);

    if (!header)
    {
        free(entries);
        return;
    }

    header[0] = 'D';
    header[1] = 'I';
    header[2] = 'R';
    header[3] = 'C';

    header[4] = 0;
    header[5] = 0;
    header[6] = 0;
    header[7] = 2;

    memcpy(header + 8, &number_entries, sizeof(number_entries));
    qsort(entries, argc, sizeof(struct entry), compare);

    FILE *file = fopen(".mgit/index", "wb");

    if (file == NULL)
    {
        free(header);
        free(entries);
        return;
    }

    fwrite(header, 1, header_len, file);

    for (int i = 0; i < argc; i++)
    {
        fwrite(&entries[i].st, sizeof(entries[i].st), 1, file);

        fwrite(entries[i].hash, SHA_DIGEST_LENGTH * 2, 1, file);

        fwrite(&entries[i].path_length, sizeof(size_t), 1, file);
        fwrite(entries[i].path, sizeof(char), entries[i].path_length, file);
    }
    fclose(file);
    add_index_sha();

    free(header);
    free(entries);
}

void write_entries(FILE *file, struct entry *entries, uint64_t number_entries)
{
    size_t header_len = 16;
    char header[16] = { 0 };

    header[0] = 'D';
    header[1] = 'I';
    header[2] = 'R';
    header[3] = 'C';
    header[7] = 2;

    memcpy(header + 8, &number_entries, sizeof(number_entries));

    fwrite(header, 1, header_len, file);

    for (uint64_t i = 0; i < number_entries; i++)
    {
        fwrite(&entries[i].st, sizeof(entries[i].st), 1, file);
        fwrite(entries[i].hash, SHA_DIGEST_LENGTH * 2, 1, file);
        fwrite(&entries[i].path_length, sizeof(size_t), 1, file);
        fwrite(entries[i].path, sizeof(char), entries[i].path_length, file);
    }
}

void add_new_index(char *path[], int argc, uint64_t number_entries,
                   struct entry *old_entries)
{
    uint64_t temp = number_entries;

    number_entries += argc;

    struct entry *entries = malloc(sizeof(struct entry) * number_entries);

    if (!entries)
        return;

    for (uint64_t i = 0; i < temp; i++)
        entries[i] = old_entries[i];
    for (int i = 0; i < argc; i++)
    {
        int final_size;
        char sha[SHA_DIGEST_LENGTH * 2 + 1];

        entries[i + temp] = make_entry(path[i], sha, &final_size);
    }

    qsort(entries, number_entries, sizeof(struct entry), compare);

    FILE *file = fopen(".mgit/index", "wb");

    if (file == NULL)
    {
        free(entries);
        return;
    }

    write_entries(file, entries, number_entries);

    fclose(file);
    add_index_sha();

    free(entries);
}

void read_entry(FILE *file, struct entry *entry)
{
    fread(&entry->st, sizeof(entry->st), 1, file);
    fread(entry->hash, 40, 1, file);
    entry->hash[40] = '\0';

    fread(&entry->path_length, sizeof(size_t), 1, file);

    entry->path = malloc(entry->path_length + 1);

    if (entry->path == NULL)
        return;

    fread(entry->path, sizeof(char), entry->path_length, file);

    entry->path[entry->path_length] = '\0';
}

void make_index(char *path[], int argc)
{
    FILE *file = fopen(".mgit/index", "rb");

    if (file == NULL)
    {
        create_index(path, argc);
        return;
    }

    char header[16];

    fread(header, sizeof(char), 16, file);

    uint64_t number_entries;

    memcpy(&number_entries, header + 8, sizeof(number_entries));
    struct entry *old_entries = malloc(sizeof(struct entry) * number_entries);

    if (old_entries == NULL)
    {
        fclose(file);
        return;
    }

    for (uint64_t i = 0; i < number_entries; i++)
        read_entry(file, &old_entries[i]);

    fclose(file);

    add_new_index(path, argc, number_entries, old_entries);

    free(old_entries);
}
