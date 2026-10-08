#include <stdio.h>
#include <string.h>

#include "minigit.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        puts("Please provide an action to do (init, add or commit)");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0)
    {
        if (argc != 2)
            return 128;

        return init();
    }
    else if (strcmp(argv[1], "add") == 0)
    {
        return add(argc - 2, argv + 2);
    }
    return 0;
}
