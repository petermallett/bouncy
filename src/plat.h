#pragma once

#include <assert.h>
#include <linux/limits.h>

#define KB(x) ((size_t) (x) << 10)
#define MB(x) ((size_t) (x) << 20)

struct proc_info {
    char cwd[PATH_MAX];
    char procName[NAME_MAX];
};

struct text_file {
    char path[PATH_MAX];
    char name[NAME_MAX];
    size_t size;
    FILE *fp;

    ~text_file(void)
    {
        assert(!this->fp);
    }
};

struct text_file_buf {
    text_file *fileInfo;
    char *data;

    ~text_file_buf(void)
    {
        assert(!this->data);
    }
};

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

inline proc_info
platGetProcInfo(char *argv0)
{
    proc_info result = {};
    char *getcwd_result = getcwd(result.cwd, PATH_MAX);
    assert(getcwd_result == result.cwd);
    strcpy(result.procName, basename(argv0));

    return result;
}

inline bool
platFileExists(const char *filePath)
{
    return(access(filePath, F_OK) != -1);
}

inline text_file
platBeginTextFileRead(const char *filePath)
{
    text_file result;
    struct stat buffer;

    result.fp = 0;

    strcpy(result.path, filePath);
    strcpy(result.name, basename((char *)filePath));

    if (stat(filePath, &buffer) == 0)
    {
        result.size = buffer.st_size;
        FILE *fp = fopen(filePath, "r");
        if (fp)
        {
            result.fp = fp;
        }
    }
    else
    {
        result.size = 0;
    }

    return result;
}

inline void
platEndTextFileRead(text_file *fileInfo)
{
    if(fileInfo->fp)
    {
        fclose(fileInfo->fp);
        fileInfo->fp = 0;
    }
}

inline text_file_buf
platReadEntireTextFile(text_file *fileInfo)
{
    text_file_buf result = {};
    result.fileInfo = fileInfo;

    result.data = (char *)malloc(fileInfo->size);

    return result;
}

inline void
platFreeFileBuf(text_file_buf *buf_info)
{
    assert(buf_info->data);
    free(buf_info->data);
    buf_info->data = 0;
    buf_info->fileInfo = 0;
}
