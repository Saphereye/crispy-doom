#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#define CALLFUNCTION(script_name) callfunction(__func__, script_name)

bool callfunction(const char *function_name, const char *script_name)
{
    int result = 0;
    char *script_path = NULL;

    const char *PWD = getenv("PWD");
    if (PWD == NULL)
    {
        PWD = "/home/lh2043f3/Coding/crispy-doom";
    }

    script_path = (char *) calloc(sizeof(char), 1024);
    if (script_path == NULL)
    {
        perror("calloc");
        return true;
    }

    sprintf(script_path, "%s/actionscripts/%s/%s.sh", PWD, function_name,
            script_name);
    result = system(script_path);

    free(script_path);

    // Check the result of the execution
    if (result == -1)
    {
        perror("system");
        return true;
    }

    // Check the exit status of the command
    if (WIFEXITED(result) && WEXITSTATUS(result) != 0)
    {
        printf("Script exited with status %d\n", WEXITSTATUS(result));
        return WEXITSTATUS(result);
    }

    printf("Script %s/%s.sh executed successfully\n", function_name, script_name);
    return false;
}