#include <stdio.h>
#include <string.h>

#include "minigit.h"

/**
** \brief Execute the requested MiniGit command.
** \param argc Number of command-line arguments
** \param argv Array of command-line arguments
** \return Command exit status, 1 if no action is provided, or 128
**         if the init command receives an invalid number of arguments
*/
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
