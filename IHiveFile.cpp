#include "IHiveFile.h"
#include "IHiveMmap.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#define BLOCK_SIZE_MASK 0xFFF

int IHiveFile::open(RegFile& regf, const char* from, int flags, int mode)
{
    int fd = ::open(from, flags, mode);
    if (fd == -1) {
        syslog(LOG_DEBUG, "Failed in open:%s", strerror(errno));
        return -1;
    }

    regf.handle = fd;
    regf.flags = flags;

    return gRegMmap->map(regf);
}

int IHiveFile::close(RegFile& regf)
{
    if (regf.handle < 0)
        return regf.handle;

    int ret = gRegMmap->umap(regf);
    ret = ret < 0 ? ret : ::close(regf.handle);

    return ret;
}

int IHiveFile::extend(RegFile& regf, size_t nblocks)
{
    if (regf.handle < 0)
        return regf.handle;

    if ((regf.flags & O_ACCMODE) == O_RDONLY)
        return -1;

    size_t old_len = regf.stop - regf.start;

    int ret = ftruncate(regf.handle, old_len + nblocks * BLOCK_SIZE_);
    if (ret == -1) {
        syslog(LOG_DEBUG, "Failed in ftruncate:%s", strerror(errno));
        return -1;
    }

    return gRegMmap->extend(regf);
}

IHiveFile gIHiveFile;
IRegFile* gRegFile = &gIHiveFile;
