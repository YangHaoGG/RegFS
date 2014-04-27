////
// @file IHiveFile.h
// @brief
// interface of regfile, inherit from IRegFile
//
// @email gm.yanghao@gmail.com
//

#ifndef _HIVEFILE_H_INCLUDED_
#define _HIVEFILE_H_INCLUDED_

#include "RegFile.h"

#define BLOCK_SIZE_      4096
#define BLOCK_SIZE_MASK_ (BLOCK_SIZE_ - 1)

struct IHiveFile:public IRegFile
{
    int open(RegFile &regf, const char *from, int flags, int mode);
    int close(RegFile &regf);
    int extend(RegFile &regf, size_t nblocks);

    int lock(RegFile &regf, int flags, char *f, int l) {}
    void unlock(RegFile &regf, int flags) {}
    int upgrade(RegFile &regf) {}
    int downgrade(RegFile &regf) {}
    int isLocked(RegFile &regf) {}

    int name(RegFile &regf, char *name, int len) {}
    int size(RegFile &regf, size_t *size) {}
    int mode(RegFile &regf, unsigned int *mode) {}
    int uid(RegFile &regf, unsigned int *uid) {}
    int gid(RegFile &regf, unsigned int *gid) {}

    ~IHiveFile(){}
};

#endif // _HIVEFILE_H_INCLUDED_
