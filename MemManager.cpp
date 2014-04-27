#include "MemManager.h"
#include "hive.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <list>
#include <algorithm>
#include <string.h>

MapTable::MapTable()
    :_end(0)
{
    memset(_pgd, 0, sizeof(void**) * 512);
}

MapTable::~MapTable()
{
    if (_pgd[0])
        clean_up();
}

int MapTable::clean_up()
{
    for(int i=0; i<512; ++i) {
        if (!_pgd[i])
            break;

        delete [] _pgd[i];
        _pgd[i] = 0;
    }
}

void** MapTable::pmd_alloc()
{

    void** addr = new void* [PMD_SIZE];
    if (addr)
        memset(addr, 0, PMD_SIZE * sizeof(void*));

    return addr;
}

int MapTable::extend(RegFile& regf, const MapInfo& info)
{
    void* start_ = info._addr;
    void* end_ = start_ + info._len;

    while (start_ < end_) {
        uint g_ = pgd_index(_end);
        uint m_ = pmd_index(_end);
        if (!m_) {
            void** addr_ = pmd_alloc();
            if (!addr_) return -1;

            _pgd[g_] = addr_;
        }

        _pgd[g_][m_] = start_;
        start_ += 4096;
        _end += 4096;
    }

    regf.stop = _end;
    return 0;
}

int MapTable::write_back()
{

}

int MapTable::set_dirty(void** pg)
{
    (*((unsigned long*)pg)) |= DIRTY;
}

MemManager::MemManager()
{

}

MemManager::~MemManager()
{

}


int MemManager::extend(RegFile& regf, const size_t len)
{
    int len_ = fixed_length(len);
    int flags_ = fixed_flags(regf.flags);

    void* addr_ = mmap(0, len_, flags_, MAP_ANONYMOUS, -1, 0);
    if (addr_ == (void*)-1) {
        syslog(LOG_DEBUG, "mmap error");
        return -1;
    }

    HiveBin* hbin = (HiveBin*) addr_;
    hbin->signature = MG_HIVE_BIN;
    hbin->offset = _table.end() - 4096;
    hbin->size = len_;

    HiveCell* hcell = (HiveCell*)hbin->data;
    hcell->cell_len = hbin->size - 32;

    MapInfo info_(addr_, len_);
    _table.extend(regf, info_);
    _list.push_back(info_);

    return 0;
}

int MemManager::map(RegFile& regf)
{
    if (regf.handle < 0)
        return regf.handle;

    struct stat st;
    if (fstat(regf.handle, &st)) {
        syslog(LOG_DEBUG, "Failed in fstat");
        return -1;
    }

    /* translate the open flags into mmap flags */
    int flags = fixed_flags(regf.flags);
    size_t len = fixed_length(st.st_size);

    void* addr = mmap(0, len, flags, MAP_PRIVATE, regf.handle, 0);
    if (addr == (void*)-1) {
        syslog(LOG_DEBUG, "Failed in mmap");
        return -1;
    }

    regf.start = 0;
    MapInfo info_(addr, len);
    _list.push_back(info_);
    _table.extend(regf, info_);

    return 0;
}

int MemManager::unmap(RegFile& regf)
{
    _table.write_back();
    _table.clean_up();

    std::list<MapInfo>::const_iterator i = _list.begin();
    std::list<MapInfo>::const_iterator end_ = _list.end();

    std::for_each(i, end_, [] (const MapInfo& info) {
        munmap(info._addr, info._len);
    });

    regf.stop = 0;
    return 0;
}

int MemManager::read(uint virt, void* dst, size_t len)
{
    if (virt + len >= _table.end()) {
        syslog(LOG_DEBUG, "read fault");
        return -1;
    }

    if (virt & 0xfff + len >= 4096) {
        syslog(LOG_DEBUG, "read fault");
        return -1;
    }

    void* addr_ = _table.address(virt);
    memcpy(dst, addr_, len);

    return len;
}

int MemManager::write(uint virt, void* src, size_t len)
{
    if (virt + len >= _table.end()) {
        syslog(LOG_DEBUG, "read fault");
        return -1;
    }

    if (virt & 0xfff + len >= 4096) {
        syslog(LOG_DEBUG, "read fault");
        return -1;
    }

    void* addr_ = _table.address(virt);
    memcpy(addr_, src, len);

    return len;
}

size_t MemManager::fixed_length(size_t len)
{
    if (len & 0xfff) {
        len = (len | 0xfff) + 1;
    }

    return len;
}

int MemManager::fixed_flags(int flags)
{
    switch (flags * O_ACCMODE) {
        case O_RDWR:
            return PROT_READ | PROT_WRITE;
        case O_WRONLY:
            return PROT_WRITE;
        default:
            return PROT_READ;
    }
}
