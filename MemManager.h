#ifndef _MEMMANAGER_H_INCLUDED_
#define _MEMMANAGER_H_INCLUDED_

#include "RegFile.h"
#include "hive_type.h"
#include <list>
#include <syslog.h>

struct MapInfo
{
    void*   _addr;
    size_t  _len;

    explicit MapInfo(void* addr, size_t len)
        :_addr(addr), _len(len) {}
};

class MapTable
{
    public:
        MapTable();
        ~MapTable();

        int extend(RegFile& regf, const MapInfo& info);
        int write_back();
        int set_dirty(void** pg);
        int clean_up();

        uint end() {
            return _end;
        }

        void* address(uint virt) {
            if (virt >= _end) {
                syslog(LOG_DEBUG,"Segment fault:[%d]",virt);
                return 0;
            }

            uint g_ = pgd_index(virt);
            uint m_ = pmd_index(virt);
            void* addr_ = _pgd[g_][m_];

            uint offset = virt & OFF_MASK;
            return addr_ + offset;
        }

    protected:
        void** pmd_alloc();

        uint pgd_index(uint virt) {
            return (virt >> PGD_SHF) & PD_MASK;
        }

        uint pmd_index(uint virt) {
            return (virt >> PMD_SHF) & PD_MASK;
        }

    private:
        void**  _pgd[512];
        uint    _end;
        static const uint PMD_SIZE = 512;
        static const uint PD_MASK  = (512 - 1);
        static const uint PMD_SHF  = 12;
        static const uint PGD_SHF  = 21;
        static const uint OFF_MASK = 4096 - 1;
        static const uint DIRTY    = 0x1;
};

class MemManager
{
    public:
        ~MemManager();

        int extend(RegFile& regf, const size_t len);
        int map(RegFile& regf);
        int unmap(RegFile& regf);

        int read(uint virt, void* dst, size_t len);
        int write(uint virt, void* src, size_t len);

        static MemManager* instance() {
            static MemManager* manager_ = new MemManager();
            return manager_;
        }

    protected:
        size_t fixed_length(size_t len);
        int fixed_flags(int flags);
        MemManager();

    private:
        MapTable            _table;
        std::list<MapInfo>  _list;
        static const uint   pg_size = 4096;
};

#define reader(virt, T, elem, dst, len) do {\
    uint virt_ = virt + (unsigned long)&(((T)0)->elem);\
    MemManager::instance()->read(virt_, dst, len);\
}while(0)

#define writer(virt, T, elem, src, len) do { \
    uint virt_ = virt + (unsigned long)&(((T)0)->elem); \
    MemManager::instance()->write(virt_, src, len); \
}while(0)

#endif // _MEMMANAGER_H_INCLUDED_
