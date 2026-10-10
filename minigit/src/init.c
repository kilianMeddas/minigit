#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

/**
** \brief Initialize the MiniGit repository structure.
** \return 0 on success, 128 on failure
*/
int init(void)
{
    int head;

    if (mkdir(".mgit", 0755) == -1)
        return 128;

    if (mkdir(".mgit/objects", 0755) == -1)
        return 128;

    if (mkdir(".mgit/refs", 0755) == -1)
        return 128;

    if (mkdir(".mgit/refs/heads", 0755) == -1)
        return 128;

    if (mkdir(".mgit/refs/remotes", 0755) == -1)
        return 128;

    head = open(".mgit/HEAD", O_CREAT | O_WRONLY, 0644);
    if (head == -1)
        return 128;

    if (write(head, "ref: refs/heads/master\n", 23) != 23)
    {
        close(head);
        return 128;
    }

    if (close(head) == -1)
        return 128;

    return 0;
}
