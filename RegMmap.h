/**
 * @file RegMmap.h
 * @brief
 * the policy to implement the regfs is simple: map the whole regfs file into
 * memory. normally, the size of regfs is small, so for the 32bits operating
 * systems, regfs impact the os very little. but for heavy loaded os, we must
 * use 64 bits.
 *
 * @author niexw
 * @email xiaowen.nie.cn@gmail.com
 */
#ifndef _REGFS_REGMMAP_H_
#define _REGFS_REGMMAP_H_

#include "RegFile.h"

#if defined(__cplusplus)
struct IRegMmap
{
	virtual int map(RegFile &regf) = 0;
	virtual int umap(RegFile &regf) = 0;
	virtual int extend(RegFile &regf) = 0;
};
extern "C" IRegMmap *gRegMmap;
#else /* __cplusplus */

BEGIN_DECLARE_C_INTERFACE(IRegMmap)
	int (*map)(struct IRegMmap *This, struct RegFile *regf);
	int (*umap)(struct IRegMmap *This, struct RegFile *regf);
	int (*extend)(struct IRegMmap *This, struct RegFile *regf);
END_DECLARE_C_INTERFACE(IRegMmap)
extern struct IRegMmap *gRegMmap;

#endif /* __cplusplus */

#endif /* _REGFS_REGMMAP_H_ */

