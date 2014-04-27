////
// @file IHiveMmap.h
// @brief
// inherit from IRegMmap
//

#ifndef _IHIVEMMAP_H_INCLUDED_
#define _IHIVEMMAP_H_INCLUDED_

#include "RegMmap.h"

struct IHiveMmap:public IRegMmap
{
    int map(RegFile &regf);
    int umap(RegFile &regf);
    int extend(RegFile &regf);
};


#endif // _IHIVEMMAP_H_INCLUDED_
