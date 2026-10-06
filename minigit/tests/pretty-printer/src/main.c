#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "index_printer.h"
#include "tree_printer.h"

static void err_usage()
{
    errx(EXIT_FAILURE, "Usage: ./printer < -i | -t > path");
}

int main(int argc, char **argv)
{
    if (argc != 3)
        err_usage();

    if (!strcmp(argv[1], "-i"))
        index_pretty_print(argv[2]);
    else if (!strcmp(argv[1], "-t"))
        tree_pretty_print(argv[2]);
    else
        err_usage();

    return 0;
}
