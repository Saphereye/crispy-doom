#pragma once

#include "doomdef.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define G_CALLFUNCTION(script_name) G_CallFunction(__func__, script_name)

bool G_CallFunction(const char *function_name, const char *script_name)
{
    int result = 0;
    char *script_file_path = NULL;

    // use PWD to access the scripts
    const char *PWD = getenv("PWD");
    if (PWD == NULL)
    {
        const char *HOME = getenv("HOME");
        PWD = (char*)calloc(sizeof(char), 1024);
        M_snprintf(PWD, "%s/Coding/crispy-doom", HOME);
    }

    // Reserve size for script file path string
    script_file_path = (char *) calloc(sizeof(char), 1024);
    if (script_file_path == NULL)
    {
        perror("calloc");
        return true;
    }

    // Build the script file path
    M_snprintf(script_file_path, "%s/actionscripts/%s/%s", PWD, function_name,
               script_name);
    
    free(PWD);

    // Check if scripts exists, if not just make one
    FILE *script_file = fopen(script_file_path, "a+");
    if (script_file == NULL)
    {
        perror("fopen");
        return true;
    }

    // Call the scritps (Is it async?)
    result = system(script_file_path);

    free(script_file_path);

    // Check the result of the script execution
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

    printf("Script %s/%s.sh executed successfully\n", function_name,
           script_name);
    return false;
}