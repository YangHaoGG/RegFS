#define __USE_GNU

#include "IHiveMmap.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include "hive.h"
#include "MemManager.h"


int IHiveMmap::map(RegFile& regf)
{
    return MemManager::instance()->map(regf);
}

int IHiveMmap::umap(RegFile& regf)
{
    return MemManager::instance()->unmap(regf);
}

int IHiveMmap::extend(RegFile& regf)
{
    struct stat st;
    if (fstat(regf.handle,&st) == -1) {
        syslog(LOG_DEBUG, "Failed in fstat:%s", strerror(errno));
        return -1;
    }

    size_t old_size = regf.stop - regf.start;
    size_t len = st.st_size - old_size;

    return MemManager::instance()->extend(regf, len);
}

IHiveMmap gIHiveMmap;
IRegMmap* gRegMmap = &gIHiveMmap;
