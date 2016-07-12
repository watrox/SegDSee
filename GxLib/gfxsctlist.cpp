#include "gfxsctlist.h"

GfxSctList::GfxSctList(QObject *parent) : GfxObjSeisSect(parent)
{
}

GfxSctList::~GfxSctList()
{
    GfxSctList::clear();
}

