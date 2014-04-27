#ifndef _INTERFACE_H_INCLUDED_
#define _INTERFACE_H_INCLUDED_

#ifndef HAVE_SETXATTR
#define HAVE_SETXATTR 1
#endif

#include <fuse.h>

int regfs_getattr(const char* path, struct stat *st);

int regfs_readdir(const char* path,
                    void* buf,
                    fuse_fill_dir_t filler,
                    off_t off,
                    struct fuse_file_info* info);

int regfs_opendir(const char* path, struct fuse_file_info* info);

int regfs_closedir(const char* path, struct fuse_file_info* info);

int regfs_setxattr(const char* path, const char* name,
                    const char* value, size_t len, int flag);

int regfs_getxattr(const char* path, const char* name, char* value, size_t len);

int regfs_listxattr(const char* path, char* buf, size_t len);

int regfs_removexattr(const char* path, const char* name);

void regfs_destroy(void* data);

extern struct RegFile reg;

#endif // _INTERFACE_H_INCLUDED_
