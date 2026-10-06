#include "print_helper.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DEPTH 64
#define PADDING_STR "   "

enum container
{
    CONTAINER_OBJ,
    CONTAINER_ARRAY
};

struct container_stack
{
    enum container items[MAX_DEPTH];
    size_t size;
};

struct printer
{
    size_t padding;
    struct container_stack stack;
};

static struct printer printer;

static void print_padding()
{
    for (size_t i = 0; i < printer.padding; ++i)
        printf(PADDING_STR);
}

static void print_key(const char *key)
{
    print_padding();

    if (key)
        printf("%s : ", key);
}

static void print_content(const char *buf, size_t n)
{
    printf("\"");

    for (size_t i = 0; i < n; ++i)
    {
        unsigned char c = buf[i];

        switch (c)
        {
        case '\0':
            printf("\\0");
            break;
        case '\a':
            printf("\\a");
            break;
        case '\b':
            printf("\\b");
            break;
        case '\t':
            printf("\\t");
            break;
        case '\n':
            printf("\\n");
            break;
        case '\v':
            printf("\\v");
            break;
        case '\f':
            printf("\\f");
            break;
        case '\r':
            printf("\\r");
            break;
        case '\\':
            printf("\\\\");
            break;
        case '"':
            printf("\\\"");
            break;
        default:
            if (isprint(c))
                printf("%c", c);
            else
                printf("\\%03o", c);
            break;
        }
    }

    printf("\"");
}

void inc_padding()
{
    ++printer.padding;
}

void dec_padding()
{
    --printer.padding;
}

void print_str_item(const char *key, const char *value, size_t n)
{
    print_key(key);

    if (value)
        print_content(value, n);
    else
        printf("null");

    printf("\n");
}

void print_num_item(const char *key, size_t value)
{
    print_key(key);
    printf("%ld\n", value);
}

static void print_unsigned_long_item(const char *key, unsigned long value)
{
    print_key(key);
    printf("%lu\n", value);
}

static void print_octal_item(const char *key, unsigned int value)
{
    print_key(key);
    printf("%o\n", value);
}

static void print_unsigned_item(const char *key, unsigned value)
{
    print_key(key);
    printf("%u\n", value);
}

static void print_time_item(const char *key, const struct timespec *value)
{
    char date[64] = "invalid";
    struct tm tm;

    if (localtime_r(&value->tv_sec, &tm))
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &tm);

    print_key(key);
    printf("%ld.%09ld (%s)\n", value->tv_sec, value->tv_nsec, date);
}

static void stack_push(struct container_stack *s, enum container c)
{
    assert(s->size < MAX_DEPTH && "nesting too deep");

    if (s->size < MAX_DEPTH)
        s->items[s->size++] = c;
}

static void stack_pop(struct container_stack *s, enum container c)
{
    assert(s->size > 0 && "closure without an opening");
    assert(s->items[s->size - 1] == c
           && "closure that does not match the opening");

    if (s->size > 0)
        --s->size;
}

static void open_contrainer(const char *key, enum container c)
{
    stack_push(&printer.stack, c);

    print_key(key);
    switch (c)
    {
    case CONTAINER_ARRAY:
        printf("%c\n", '[');
        break;
    case CONTAINER_OBJ:
        printf("%c\n", '{');
        break;
    default:
        abort();
    };
    inc_padding();
}

static void close_contrainer(enum container c)
{
    stack_pop(&printer.stack, c);

    dec_padding();
    print_padding();
    switch (c)
    {
    case CONTAINER_ARRAY:
        printf("%c\n", ']');
        break;
    case CONTAINER_OBJ:
        printf("%c\n", '}');
        break;
    default:
        abort();
    };
}

void print_obj_start(const char *key)
{
    open_contrainer(key, CONTAINER_OBJ);
}

void print_obj_end(void)
{
    close_contrainer(CONTAINER_OBJ);
}

void print_array_start(const char *key)
{
    open_contrainer(key, CONTAINER_ARRAY);
}

void print_array_end(void)
{
    close_contrainer(CONTAINER_ARRAY);
}

void print_stat_item(const char *key, const struct stat *st)
{
    if (!st)
    {
        print_key(key);
        printf("null\n");
        return;
    }

    print_obj_start(key);

    print_unsigned_long_item("st_dev", st->st_dev);
    print_unsigned_long_item("st_ino", st->st_ino);
    print_octal_item("st_mode", st->st_mode);
    print_unsigned_long_item("st_nlink", st->st_nlink);
    print_unsigned_item("st_uid", st->st_uid);
    print_unsigned_item("st_gid", st->st_gid);
    print_unsigned_long_item("st_rdev", st->st_rdev);
    print_num_item("st_size", st->st_size);

    print_time_item("st_atim", &st->st_atim);
    print_time_item("st_mtim", &st->st_mtim);
    print_time_item("st_ctim", &st->st_ctim);

    print_num_item("st_blksize", st->st_blksize);
    print_num_item("st_blocks", st->st_blocks);

    print_obj_end();
}
