#include "gfxsrcseistest.h"

GfxSrcSeisTest::GfxSrcSeisTest(QObject *parent) : GfxSrcSeis(parent)
{
    _tr=NULL;
    m_Nh   = 1;
    m_Nt   = 1000;
    m_Si   = 0.004;
    m_Ti   = 25;

    setNs(1000);
}

GfxSrcSeisTest::~GfxSrcSeisTest()
{
    if(_tr) delete [] _tr;
}

