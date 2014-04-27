#ifndef KEYENTRY_H_INCLUDED
#define KEYENTRY_H_INCLUDED

#include "hive.h"
#include "RegFile.h"
#include "MemManager.h"
#include <syslog.h>
#include <string>

class KeyValue
{
    public:
        KeyValue(uint pkv);
        KeyValue(const KeyValue& kv);
        ~KeyValue();

        int name(void* buf, size_t len) const;
        int value(void* buf, size_t len) const;
        operator bool () const;
        int name_length() const;
    protected:

    private:
        uint _pkv;
};


class KeyEntry
{
    public:
        KeyEntry(uint pkn);
        KeyEntry(const KeyEntry& ke);
        ~KeyEntry();

        template <class T>
        int for_each_child(T f);

        template <class T>
        int for_each_value(T f);

        int name(void* buf, size_t len) const;
        operator bool () const;
        KeyEntry child(const char* name);
        KeyValue value(const char* name);

        static KeyEntry entry (const char* path, RegFile* reg);

    protected:
        unsigned int hash (const char* str);

    private:
        uint _pkn;
        static RegFile* _reg;
};

template <class T>
int KeyEntry::for_each_child(T f)
{
    if (_pkn == -1)
        return -1;

    int n = 0;
    reader(_pkn, HiveKeyNode*, subkey_counts[0], &n, sizeof(int));

    if (!n) return 0;

    uint base = _reg->start + 0x1000;

    uint subkey_off = 0;
    reader(_pkn, HiveKeyNode*, list.subkey_lists[0], &subkey_off, sizeof(uint)); //subkey_off = _pkn->list.subkey_lists[0];

    uint hlist = base + subkey_off + 4; //HiveSubkeyList*

    ushort counts = 0;
    reader(hlist, HiveSubkeyList*, counts, &counts, sizeof(ushort)); //counts = hlist->counts;

    if (counts != n) {
        syslog(LOG_DEBUG,"Record Error:[%d]subkey counts doesn't match",_pkn - _reg->start);
        return -1;
    }

    ushort signature = 0;
    reader(hlist, HiveSubkeyList*, signature, &signature, sizeof(ushort)); //signature = hlist->signature;

    switch (signature) {
        case MG_HIVE_LF:
        case MG_HIVE_LH:
            for (int i=0; i<n; ++i) {
                uint offset = 0;
                reader(hlist, HiveSubkeyList*, data[i].offset, &offset, sizeof(uint)); //offset = hlist->data[i].offset;

                uint pkn = base + offset +4;
                f(KeyEntry(pkn));
            }
        break;

        case MG_HIVE_LI:
        case MG_HIVE_RI:
            for (int i=0; i<n; ++i) {
                uint offset = 0;
                reader(hlist, HiveSubkeyList*, offset[i], &offset, sizeof(uint)); //offset = hlist->offset[i]

                uint pkn = base + offset +4;
                f(KeyEntry(pkn));
            }
            break;
    }

    return n;
}

template <class T>
int KeyEntry::for_each_value(T f)
{
    if (_pkn == -1)
        return -1;

    uint nval = 0; //value counts of _pkn
    reader(_pkn, HiveKeyNode*, list.value_counts, &nval, sizeof(uint)); //nval = _pkn->list.value_counts;

    if (nval == 0)
        return 0;

    uint base = _reg->start + 0x1000;
    uint off_list = 0;
    reader(_pkn, HiveKeyNode*, list.value_list, &off_list, sizeof(uint)); //off_list = _pkn->list.value_list;

    uint vlist = base + off_list + 4; //value list of _pkn
    for (int i=0; i<nval; ++i) {
        uint voff = 0;
        reader(vlist, HiveValueList*, offset[i], &voff, sizeof(uint)); //voff = vlist->offset[i];

        uint pkv = base + voff + 4;
        f(KeyValue(pkv));
    }

    return nval;
}

#endif // KEYENTRY_H_INCLUDED
