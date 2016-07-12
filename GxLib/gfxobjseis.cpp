#include "gfxobjseis.h"


GfxObjSeis::GfxObjSeis(QObject *parent) :
    GfxObj(parent)
{
    s_src=(GfxSrcSeis*)m_src;

    m_Gc      = 1;
    m_Gw      = 1;
    m_Tw      = 1;
    m_DispWig = true;
    m_DispCol = false;

    m_SLcolor= Qt::blue;
}


void GfxObjSeis::setSrc(GfxSrcSeis* v)
{
    GfxObj::setSrc(v); s_src=v; Preset();
}

