#pragma once

#include <assert.h>
#include <stdio.h>
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
