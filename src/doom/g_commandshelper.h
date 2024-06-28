#pragma once

#include "doomdef.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>     // For errno
#include <fcntl.h>     // For open
#include <unistd.h>    // For write, close
#include <sys/types.h> // For open, mkdir
#include <sys/stat.h>  // For mkdir
#include <string.h>    // For strerror

#define G_CALLFUNCTION(script_name) G_CallFunction(__func__, script_name)

bool _CreateDirectories(const char *path, int log_fd);

bool G_CallFunction(const char *function_name, const char *script_name)
{
    int result = 0;
    char *script_file_path = NULL;
    char *script_dir_path = NULL;
    int log_fd;

    // Open the log file
    log_fd = open("script.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1)
    {
        dprintf(log_fd, "Error opening log file: %s\n", strerror(errno));
        return true;
    }

    dprintf(log_fd, "Called script %s/%s\n", function_name, script_name);

    // Use DOOMPWD to access the scripts
    const char *DOOMPWD = "/home/lh2043f3/Coding/crispy-doom";

    // Reserve size for script file path string
    script_file_path = (char *) calloc(1024, sizeof(char));
    if (script_file_path == NULL)
    {
        dprintf(log_fd, "Error allocating memory for script file path: %s\n",
                strerror(errno));
        close(log_fd);
        return true;
    }

    // Build the script file path
    snprintf(script_file_path, 1024, "%s/actionscripts/%s/%s", DOOMPWD,
             function_name, script_name);
    dprintf(log_fd, "Script file path: %s\n", script_file_path);

    // Create the directory path without the script file name
    script_dir_path = strdup(script_file_path);
    if (script_dir_path == NULL)
    {
        dprintf(log_fd,
                "Error allocating memory for script directory path: %s\n",
                strerror(errno));
        free(script_file_path);
        close(log_fd);
        return true;
    }

    char *last_slash = strrchr(script_dir_path, '/');
    if (last_slash != NULL)
    {
        *last_slash =
            '\0'; // Remove the script file name to get the directory path

        // Create directories if they don't exist
        if (_CreateDirectories(script_dir_path, log_fd))
        {
            free(script_file_path);
            free(script_dir_path);
            close(log_fd);
            return true;
        }
    }

    free(script_dir_path);

    // Check if script exists, if not just make one
    FILE *script_file = fopen(script_file_path, "a+");
    if (script_file == NULL)
    {
        dprintf(log_fd, "Error opening script file: %s\n", strerror(errno));
        free(script_file_path);
        close(log_fd);
        return true;
    }
    fclose(script_file);

    // Call the script (Is it async?)
    result = system(script_file_path);

    free(script_file_path);

    // Check the result of the script execution
    if (result == -1)
    {
        dprintf(log_fd, "Error running system script: %s\n", strerror(errno));
        close(log_fd);
        return true;
    }

    // Check the exit status of the command
    if (WIFEXITED(result) && WEXITSTATUS(result) != 0)
    {
        dprintf(log_fd, "Script exited with status %d\n", WEXITSTATUS(result));
        close(log_fd);
        return WEXITSTATUS(result);
    }

    dprintf(log_fd, "Script %s/%s executed successfully\n", function_name,
            script_name);
    close(log_fd);
    return false;
}

/*
================
Helper Functions
================
*/

bool _CreateDirectories(const char *path, int log_fd)
{
    char *subpath = strdup(path);
    char *p = NULL;
    size_t len;

    if (subpath == NULL)
    {
        dprintf(log_fd, "Error allocating memory for subpath: %s\n",
                strerror(errno));
        return true;
    }

    len = strlen(subpath);
    if (subpath[len - 1] == '/')
    {
        subpath[len - 1] = 0;
    }

    for (p = subpath + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            if (mkdir(subpath, S_IRWXU) != 0 && errno != EEXIST)
            {
                dprintf(log_fd, "Error creating directory %s: %s\n", subpath,
                        strerror(errno));
                free(subpath);
                return true;
            }
            *p = '/';
        }
    }

    if (mkdir(subpath, S_IRWXU) != 0 && errno != EEXIST)
    {
        dprintf(log_fd, "Error creating directory %s: %s\n", subpath,
                strerror(errno));
        free(subpath);
        return true;
    }

    free(subpath);
    return false;
}