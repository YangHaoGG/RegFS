#ifndef HIVE_STRUCTURE_H_INCLUDED
#define HIVE_STRUCTURE_H_INCLUDED

#include "hive_type.h"

struct HiveHeader
{
    uint signature;
    uint seq1;
    uint seq2;
    uint time_stamp[2];
    uint major;
    uint minor;
    uint type;
    uint format;
    uint root_cell;/*offset*/
    uint last_bin;/*pointer to start of last hbin in file*/
    uint cluster;
    unsigned char name[64];
    uint reserved1[99];
    uint check_sum;
    uint reserved2[894];
    uint boot_type;
    uint boot_recover;
};

struct HiveBin
{
    uint signature;
    uint offset;/*distance from the first hive bin*/
    uint size;
    uint reserved1[2];
    uint time_stamp[2];
    uint spare;
    uint data[1];
};

struct HiveKeyNode
{
    ushort signature;
    ushort flag;/*root key:0x2c,others:0x20*/
    uint time_stamp[2];
    uint spare;
    uint parent;/*offset of parent key*/
    uint subkey_counts[2];/*use subkey_counts[0]*/
    union
    {
        struct
        {
            uint subkey_lists[2];/*pointer to subkey_list(use subkey_list[0])*/
            uint value_counts;
            uint value_list;
        }list;
        uint child_hive_reference[4];
    };
    uint security;/*offset*/
    uint class_name;/*offset*/
    uint max_name_len:16;
    uint user_flags:4;
    uint vir_ctl_flags:4;
    uint debug:8;
    uint max_class_len;
    uint max_value_name_len;
    uint max_value_data_len;
    uint work_var;
    ushort name_len;
    ushort class_len;
    char name[1];
};

struct HiveKeyValue
{
    ushort signature;
    ushort name_len;
    int data_len;
    uint data;/*offset or data(if data_len's highest bit has been set to 1)*/
    uint type;/*value type*/
    ushort flags;/*if the 0 bit is set, the value name is in ASCII, other-wise it is in UTF-16LE*/
    ushort spare;
    char name[1];
};

struct HiveKeySecurity
{
    ushort signature;
    ushort reserved;
    uint flink;/*pointer to previous sk record*/
    uint blink;/*pointer to next sk record*/
    uint reference_count;
    uint descriptor_len;
    uint data[1];
};

struct HiveCell
{
    int cell_len;/*negative if allocated, positive if free*/
    ushort data[1];
};

struct HiveSubkeyList
{
    ushort signature;/*MG_HIVE_LF,MG_HIVE_LH,MG_HIVE_RI,MG_HIVE_LI*/
    ushort counts;
    union
    {
        struct
        {
            uint offset;
            uint hash_val;
        }data[1];/*signature == MG_HIVE_LF or signature == MG_HIVE_LH*/
        uint offset[1];
    };
};

struct HiveValueList
{
    uint offset[1];
};

struct HiveData
{
    char data[1];
};

#endif // HIVE_STRUCTURE_H_INCLUDED
