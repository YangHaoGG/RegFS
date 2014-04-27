#ifndef HIVE_DEFINE_H_INCLUDED
#define HIVE_DEFINE_H_INCLUDED

#define MG_HIVE_HEADER  0x66676572 /*regf*/
#define MG_HIVE_BIN     0x6e696268 /*hbin*/
#define MG_HIVE_NK      0x6b6e /*nk*/
#define MG_HIVE_VK      0x6b76 /*vk*/
#define MG_HIVE_SK      0x6b73 /*sk*/
#define MG_HIVE_LF      0x666c /*lf*/
#define MG_HIVE_LH      0x686c /*lh*/
#define MG_HIVE_RI      0x6972 /*ri*/
#define MG_HIVE_LI      0x696c /*li*/

#define REG_NONE        0x0
#define REG_SZ          0x1
#define REG_EXPAND_SZ   0x2
#define REG_BINARY      0x3
#define REG_DWORD_LITTLE_ENDIAN 0x4
#define REG_DWORD_BIG_ENDIAN    0x5
#define REG_LINK        0x6
#define REG_MULTI_SZ    0x7
#define REG_RESOURCE_LIST       0x8
#define REG_FULL_RESOURCE_DESCRIPTOR    0x9
#define REG_RESOURCE_REQUIREMENTS_LIST  0xa
#define REG_QWORD       0xb

#endif // HIVE_DEFINE_H_INCLUDED
