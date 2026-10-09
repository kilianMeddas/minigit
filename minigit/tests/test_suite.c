
#define _XOPEN_SOURCE 700

#include <criterion/criterion.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../src/minigit.h"

static char original_dir[PATH_MAX];
static char test_dir[PATH_MAX];

static void setup_test_directory(void)
{
    cr_assert_not_null(getcwd(original_dir, sizeof(original_dir)));

    strcpy(test_dir, "/tmp/minigit-test-XXXXXX");
    cr_assert_not_null(mkdtemp(test_dir));
    cr_assert_eq(chdir(test_dir), 0);
}

static void cleanup_test_directory(void)
{
    system("rm -rf .mgit test_file.txt");
    cr_assert_eq(chdir(original_dir), 0);
    cr_assert_eq(rmdir(test_dir), 0);
}

Test(minigit_init, creates_repository, .init = setup_test_directory,
     .fini = cleanup_test_directory)
{
    cr_assert_eq(init(), 0);

    cr_assert_eq(access(".mgit", F_OK), 0);
    cr_assert_eq(access(".mgit/objects", F_OK), 0);
    cr_assert_eq(access(".mgit/refs", F_OK), 0);
    cr_assert_eq(access(".mgit/HEAD", F_OK), 0);
}

Test(minigit_init, fails_if_repository_exists, .init = setup_test_directory,
     .fini = cleanup_test_directory)
{
    cr_assert_eq(init(), 0);
    cr_assert_eq(init(), 128);
}

Test(minigit_add, adds_existing_file, .init = setup_test_directory,
     .fini = cleanup_test_directory)
{
    FILE *file;

    cr_assert_eq(init(), 0);

    file = fopen("test_file.txt", "w");
    cr_assert_not_null(file);
    cr_assert(fputs("Hello MiniGit!", file) >= 0);
    cr_assert_eq(fclose(file), 0);

    char *paths[] = { "test_file.txt" };

    cr_assert_eq(add(1, paths), 0);
    cr_assert_eq(access(".mgit/index", F_OK), 0);
}

Test(minigit_add, fails_if_file_does_not_exist, .init = setup_test_directory,
     .fini = cleanup_test_directory)
{
    cr_assert_eq(init(), 0);

    char *paths[] = { "missing_file.txt" };

    cr_assert_eq(add(1, paths), 1);
}
