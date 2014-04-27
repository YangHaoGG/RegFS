#define FUSE_USE_VERSION 26

#ifndef HAVE_SETXATTR
#define HAVE_SETXATTR 1
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include "Interface.h"
#include "IHiveFile.h"
#include "IHiveMmap.h"
#include <syslog.h>
#include <unistd.h>


#define MAX_SIZE    1024

char file_path[MAX_SIZE] = {0};  // hive file's absolute path

static struct fuse_operations regfs_op;

int get_absolute_path (const char* path,char* buf, size_t size);

int main(int argc, char* argv[])
{
    if(argc < 3) {
        printf("Usage:regfs mount_point [mount options] file_path");
        return -1;
    }

    openlog("regfs:", LOG_CONS|LOG_PID,0);
    syslog(LOG_DEBUG,"%s mounted on %s", argv[0], argv[1]);

    syslog(LOG_DEBUG, "path:%s", argv[argc - 1]);

    /* init the fuse_operations */
    {
        regfs_op.getattr    = regfs_getattr;
        regfs_op.opendir    = regfs_opendir;
        regfs_op.releasedir = regfs_closedir;
        regfs_op.readdir    = regfs_readdir;
        regfs_op.setxattr   = regfs_setxattr;
        regfs_op.getxattr   = regfs_getxattr;
        regfs_op.listxattr  = regfs_listxattr;
        regfs_op.removexattr= regfs_removexattr;
        regfs_op.destroy    = regfs_destroy;
    }

    int ret = gRegFile->open(reg, argv[argc - 1], O_RDWR, 0);
    if (ret == -1) {
        syslog(LOG_DEBUG, "open error");
        return -1;
    }

    return fuse_main(argc-1, argv, &regfs_op, 0);
}

int get_absolute_path (const char* path, char* buf, size_t size)
{
    switch (*path) {
        case '/':
            strncat(buf, path, strlen(path));
            break;
        default:
            getcwd(buf, size);
            strncat(buf, "/", 1);
            strncat(buf, path, strlen(path));
            break;
    }

    return 0;
}
