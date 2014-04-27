#include "KeyEntry.h"
#include <ctype.h>
#include <string>
#include <string.h>

RegFile* KeyEntry::_reg = 0;

KeyEntry KeyEntry::entry(const char* path, RegFile* reg)
{
    _reg = reg;

    if (!path)
        return KeyEntry(-1);

    const char* pstart = path;
    if (*pstart++ != '/')
        return KeyEntry(-1);

    uint base = _reg->start + 0x1000;
    uint phead = _reg->start; //HiveHeader*

    uint rt_off = 0; //offset of root cell
    reader(phead, HiveHeader*, root_cell, &rt_off, sizeof(uint)); //rt_off = phead->root_cell;

    KeyEntry ke = KeyEntry(base + rt_off + 4);

    while (*pstart) {
        while (*pstart == '/' && *++pstart);
        if (!*pstart)   break;

        const char* pend = pstart + 1;
        while (*pend != '/' && *++pend);

        std::string str(pstart,pend);
        ke = ke.child(str.c_str());
        if (!ke)
            break;

        if (!*pend) break;
        pstart = pend + 1;
    }

    return ke;
}

KeyEntry::KeyEntry(uint pkn)
{
    _pkn = pkn;
}

KeyEntry::KeyEntry(const KeyEntry& ke)
{
    _pkn = ke._pkn;
}

KeyEntry::~KeyEntry()
{

}

int KeyEntry::name(void* buf, size_t max_len) const
{
    if (_pkn == -1)
        return -1;

    ushort name_len = 0;
    reader(_pkn, HiveKeyNode*, name_len, &name_len, sizeof(ushort)); //name_len = _pkn->name_len;

    if (name_len >= max_len) {
        syslog(LOG_DEBUG,"the length of name is too lang:%d",name_len);
        name_len = max_len - 1;
    }

    reader(_pkn, HiveKeyNode*, name, buf, name_len); //copy _pkn->name to buf
    ((char*)buf)[name_len] = 0;

    return name_len;
}

KeyEntry KeyEntry::child(const char* name)
{
    if (_pkn == -1)
        return KeyEntry(-1);

    uint base = _reg->start + 0x1000;

    int n = 0;
    reader(_pkn, HiveKeyNode*, subkey_counts[0], &n, sizeof(int)); //n = _pkn->subkey_counts[0];

    if (n == 0)
        return KeyEntry(-1);

    uint subkey_off = 0;
    reader(_pkn, HiveKeyNode*, list.subkey_lists[0], &subkey_off, sizeof(uint)); //subkey_off = _pkn->list.subkey_lists[0];

    uint hlist = base + subkey_off + 4; //HiveSubkeyList*

    ushort counts = 0;
    reader(hlist, HiveSubkeyList*, counts, &counts, sizeof(ushort)); //counts = hlist->counts;

    if(counts != n) {
        syslog(LOG_DEBUG,"Record Error:[%d]subkey counts doesn't match",_pkn - _reg->start);
        return KeyEntry(-1);
    }

    ushort signature = 0;
    reader(hlist, HiveSubkeyList*, signature, &signature, sizeof(ushort)); //signature = hlist->signature;

    switch (signature) {
        case MG_HIVE_LF:
        case MG_HIVE_LH:
        {
            uint h = hash(name);
            for (int i=0; i<n; ++i) {
                uint hval = 0;
                reader(hlist, HiveSubkeyList*, data[i].hash_val, &hval, sizeof(uint));

                if (h == hval) {
                    uint offset = 0;
                    reader(hlist, HiveSubkeyList*, data[i].offset, &offset, sizeof(uint)); //offset = hlist->data[i].offset;

                    uint pkn = base + offset +4;
                    return KeyEntry(pkn);
                }
            }
        }
        break;

        case MG_HIVE_LI:
        case MG_HIVE_RI:
            for (int i=0; i<n; ++i) {
                uint offset = 0;
                reader(hlist, HiveSubkeyList*, offset[i], &offset, sizeof(uint)); //offset = hlist->offset[i];

                uint pkn = base + offset +4;

                char pname[128];
                uint name_len = 0;
                reader(pkn, HiveKeyNode*, name_len, &name_len, sizeof(uint)); //name_len = pkn->name_len;

                if (name_len >= 128) {
                    syslog(LOG_DEBUG,"the length of name is too lang:%d",name_len);
                    continue;
                }
                reader(pkn, HiveKeyNode*, name, pname, 128); //copy pkn->name to pname
                if (!strcmp(pname,name))
                    return KeyEntry(pkn);
            }
            break;
    }

    return KeyEntry(-1);
}

unsigned int KeyEntry::hash(const char* str)
{
    unsigned int h = 0;
    const char* p = str;

    while (*p) {
        h *= 37;
        h += toupper(*p);
        ++p;
    }
    return h;
}

KeyEntry::operator bool () const
{
    return _pkn != -1;
}

KeyValue::KeyValue(uint pkv)
{
    _pkv = pkv;
}

KeyValue::KeyValue(const KeyValue& kv)
{
    _pkv = kv._pkv;
}

KeyValue::~KeyValue()
{

}

int KeyValue::name(void* buf, size_t len) const
{
    if (_pkv == -1)
        return 0;

    ushort len_ = name_length();

    if (len_ >= len) {
        len_ = len - 1;
    }

    reader(_pkv, HiveKeyValue*, name, buf, len_); //copy _pkv->name to buf
    ((char*)buf)[len_] = 0;

    return len_;
}

int KeyValue::value(void* buf, size_t len) const
{
    if (_pkv == -1)
        return -1;

    if (len <= 4)
        return 0;

    void* buf_ = buf;
    reader(_pkv, HiveKeyValue*, type, buf_, 4); //copy _pkv->type to buf
    buf_ += 4;

    int len_ = 0;
    reader(_pkv, HiveKeyValue*, data_len, &len_, sizeof(int)); //len_ = _pkv->data_len;

    if (len_ < 0) {
        len_ = -len_;
        reader(_pkv, HiveKeyValue*, data, buf_, len_); //copy _pkv->data to buf

        ((char*)buf_)[len_] = 0;
    } else {
        uint doff = 0;
        reader(_pkv, HiveKeyValue*, data, &doff, sizeof(uint)); //doff = _pkv->data;

        uint data_ = 0x1000 + doff + 4;
        if (len_ + 4 >= len) {
            len_ = len - 4;
        }
        reader(data_, HiveData*, data, buf_, len_); //copy data_->data to buf
        ((char*)buf_)[len_] = 0;
    }

    return len_ + 4;
}

KeyValue::operator bool() const
{
    return _pkv != -1;
}

KeyValue KeyEntry::value(const char* name)
{
    if (_pkn == -1)
        return KeyValue(-1);

    uint nval = 0; //value counts of _pkn
    reader(_pkn, HiveKeyNode*, list.value_counts, &nval, sizeof(uint)); //nval = _pkn->list.value_counts;

    if (nval == 0)
        return 0;

    uint base = _reg->start + 0x1000;
    uint off_list = 0;
    reader(_pkn, HiveKeyNode*, list.value_list, &off_list, sizeof(uint)); //off_list = _pkn->list.value_list;

    char name_[128];
    uint vlist = base + off_list + 4; //value list of _pkn
    for (int i=0; i<nval; ++i) {
        uint voff = 0;
        reader(vlist, HiveValueList*, offset[i], &voff, sizeof(uint)); //voff = vlist->offset[i];

        uint pkv = base + voff + 4;
        KeyValue kv(pkv);
        kv.name(name_, 128);

        if(!strcmp(name_,name))
            return kv;
    }

    return KeyValue(-1);
}
int KeyValue::name_length() const
{
    if (_pkv == -1)
        return 0;

    ushort len_ = 0;
    reader(_pkv, HiveKeyValue*, name_len, &len_, sizeof(ushort)); //len_ = _pkv->name_len;

    return len_;
}


