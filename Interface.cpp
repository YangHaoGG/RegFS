#define FUSE_USE_VERSION 26

#include "Interface.h"
#include "IHiveFile.h"
#include "hive.h"
#include "KeyEntry.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/xattr.h>

struct RegFile reg;

int regfs_getattr(const char* path, struct stat* st)
{
/*
    int ret = 0;

    ret = get_file_type(path);
    if (ret) {
        if (stat(file_path, st) < 0) {
            syslog(LOG_DEBUG, "Failed in stat:%s", strerror(errno));
            return -1;
        }

        switch (ret) {
            case REG_DIR:
                st->st_mode = S_IFDIR | 0755;
                st->st_nlink = 2;
                st->st_size = 4096;
                break;

            case REG_FILE:
                break;

            default:
                return -1;
        }

        return 0;
    }

    return -1;
*/
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;
    st->st_size = 4096;

    return 0;
}

struct op_rddir
{
    op_rddir(void* buf, fuse_fill_dir_t filler)
        :_buf(buf), _filler(filler) {}

    void operator () (const KeyEntry& ke) {
        if (ke) {
            ke.name(_name, 128);
            _filler(_buf, _name, NULL, 0);
        }
    }

    void* _buf;
    fuse_fill_dir_t _filler;
    static char _name[128];
};
char op_rddir::_name[128];

int regfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t off, struct fuse_file_info* info)
{
    syslog(LOG_DEBUG,"%s",path);
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    KeyEntry ke = KeyEntry::entry(path,&reg);
    if (!ke) return -1;

    ke.for_each_child(op_rddir(buf,filler));

    return 0;
}

int regfs_opendir(const char* path, struct fuse_file_info* info)
{
    syslog(LOG_DEBUG,"opendir %s",path);
    return 0;
}

int regfs_closedir(const char* path, struct fuse_file_info* info)
{
    syslog(LOG_DEBUG,"closedir %s",path);
    return 0;
}

int regfs_setxattr(const char* path, const char* name, const char* value, size_t len, int flag)
{

    return 0;
}

int regfs_getxattr(const char* path, const char* name, char* value, size_t len)
{
    syslog(LOG_DEBUG,"getxattr %s %s %d",path,name,len);
    if (len <= 0)
        return 0;

    const char* p = name;
    while (*p && *p++ != '.');

    KeyEntry ke = KeyEntry::entry(path, &reg);
    if (!ke)
        return 0;

    KeyValue kv = ke.value(p);
    if(!kv)
        return 0;

    size_t len_ = kv.value(value, len);syslog(LOG_DEBUG, "%d", len_);

    return len_;
}

struct op_listxattr
{
    op_listxattr(char* buf, size_t& len)
        :_buf(buf), _len(len) {
        memset(buf, 0, len);
    }

    void operator () (const KeyValue& kv) {
        strncat(_buf, user, 5);
        _buf += 5;
        _len -= 5;

        int len = kv.name(_buf, _len) + 1;

        _buf += len;
        _len -= len;
    }

    char* _buf;
    size_t& _len;
    static const char* user;
};
const char* op_listxattr::user = "user.";

struct size_listxattr
{
    size_listxattr(size_t& size):size_(size) {}
    void operator () (const KeyValue& kv) {
        size_ += kv.name_length() + 6;
    }

    size_t& size_;
};


int regfs_listxattr(const char* path, char* buf, size_t len)
{
    syslog(LOG_DEBUG, "listxattr %s",path);

    size_t len_ = len;
    KeyEntry ke = KeyEntry::entry(path, &reg);
    if (len_) {
        ke.for_each_value(op_listxattr(buf, len_));
        return len - len_;
    }
    else {
        ke.for_each_value(size_listxattr(len_));
        return len_;
    }
}

void regfs_destroy(void* data)
{
    syslog(LOG_DEBUG, "destroy system");
}

int regfs_removexattr(const char* path, const char* name)
{

}

