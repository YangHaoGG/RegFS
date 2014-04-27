/**
 * @file IRegFile.h
 * @brief
 * declare the interface of regfile
 *
 * @author niexw
 * @email xiaowen.nie.cn@gmail.com
 */
#ifndef _REGFS_REGFILE_H_
#define _REGFS_REGFILE_H_

//#include <ocf/ComDefines.h>
#include "hive_type.h"
#include <stddef.h>
typedef int HANDLE;

#define DEFAULT_HIVE_FILE_MODE S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH

#if defined(__cplusplus)
extern "C" {
struct RegFile {
	HANDLE handle;
	uint start;
	uint stop;
	int flags;
	RegFile() : handle(-1), start(0), stop(0), flags(0) {}
	~RegFile();
};
}
struct IRegFile
{
	virtual int open(RegFile &regf, const char *from, int flags, int mode) = 0;
	virtual int close(RegFile &regf) = 0;
	virtual int extend(RegFile &regf, size_t length) = 0;

	virtual int lock(RegFile &regf, int flags, char *f, int l) = 0;
	virtual void unlock(RegFile &regf, int flags) = 0;
	virtual int upgrade(RegFile &regf) = 0;
	virtual int downgrade(RegFile &regf) = 0;
	virtual int isLocked(RegFile &regf) = 0;

	virtual int name(RegFile &regf, char *name, int len) = 0;
	virtual int size(RegFile &regf, size_t *size) = 0;
	virtual int mode(RegFile &regf, unsigned int *mode) = 0;
	virtual int uid(RegFile &regf, unsigned int *uid) = 0;
	virtual int gid(RegFile &regf, unsigned int *gid) = 0;

  protected:
	~IRegFile() {}
};
extern "C" IRegFile* gRegFile;
inline RegFile::~RegFile() { if (handle > 0) gRegFile->close(*this); }
#else /* __cplusplus */

struct RegFile {
	HANDLE handle;
	char *start;
	char *stop;
	int flags;
};
struct IRegFile {
	int (*open)(struct IRegFile *This, struct RegFile *regf, const char *from, int flags, int mode);
	int (*close)(struct IRegFile *This, struct RegFile *regf);
	int (*extend)(struct IRegFile *This, struct RegFile *regf, size_t length);

	int (*lock)(struct IRegFile *This, struct RegFile *regf, int flags, char *f, int l);
	void (*unlock)(struct IRegFile *This, struct RegFile *regf, int flags);
	int (*upgrade)(struct IRegFile *This, struct RegFile *regf);
	int (*downgrade)(struct IRegFile *This, struct RegFile *regf);
	int (*isLocked)(struct IRegFile *This, struct RegFile *regf);

	int (*name)(struct IRegFile *This, struct RegFile *regf, char *name, int len);
	int (*size)(struct IRegFile *This, struct RegFile *regf, size_t *size);
	int (*mode)(struct IRegFile *This, struct RegFile *regf, unsigned int *mode);
	int (*uid)(struct IRegFile *This, struct RegFile *regf, unsigned int *uid);
	int (*gid)(struct IRegFile *This, struct RegFile *regf, unsigned int *gid);
};
extern struct IRegFile *gRegFile;

#endif /* __cplusplus */

#endif /* _REGFS_REGFILE_H_ */

