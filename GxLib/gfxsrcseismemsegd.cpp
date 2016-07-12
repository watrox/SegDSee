#include "gfxsrcseismemsegd.h"

GfxSrcSeisMemSegd::GfxSrcSeisMemSegd(QObject *parent) : GfxSrcSeisMem(parent)
{
    _ti = new char  [m_Nt]; memset(_ti,0,m_Nt);
    _cs = new int   [m_Nt]; memset(_cs,0xFF,m_Nt*sizeof(int));
    _ch = new int   [m_Nt]; memset(_ch,0xFF,m_Nt*sizeof(int));
}

GfxSrcSeisMemSegd::~GfxSrcSeisMemSegd()
{
    if(_ti) delete [] _ti;
    if(_cs) delete [] _cs;
    if(_ch) delete [] _ch;
}


void  GfxSrcSeisMemSegd::Reset()
{
    if(m_Nt>0)
    {
        memset(_ti,0,m_Nt);
        Invalidate();
    }
}


void  GfxSrcSeisMemSegd::setNt(int nt)
{
    if(_ti) delete [] _ti; _ti=NULL;
    if(_cs) delete [] _cs; _cs=NULL;
    if(_ch) delete [] _ch; _ch=NULL;

    if(nt>0)
    {
        _ti = new char [nt]; memset(_ti,0,nt);
        _cs = new int  [nt]; memset(_cs,0xFF,nt*sizeof(int));
        _ch = new int  [nt]; memset(_ch,0xFF,nt*sizeof(int));
    }
    else
    {
        _ti=NULL;
        _cs=NULL;
        _ch=NULL;
    }

    GfxSrcSeisMem::setNt(nt);
}

float GfxSrcSeisMemSegd::getSmp(int nt, int ns)
{
    int rc = receivers(SIGNAL(smpRequest(int, int , float*, int&)));
    if(rc<1) return 0;

    if(nt<0 || nt>=m_Nt) return 0;
    if(ns<0 || ns>=m_Ns) return 0;

    float* tt = _tr+nt*m_Ns;

    if(_ti[nt]==0)
    {
        emit smpRequest(Th(nt,0),Th(nt,1),tt,_ti[nt]);
    }

    if(_ti[nt]==0) return 0;

    float* ts = tt+ns;
    return *ts;
}

float* GfxSrcSeisMemSegd::Ts(int tidx)
{
    int rc = receivers(SIGNAL(smpRequest(int , int , float*, char& )));
    if(rc<1) return NULL;

    if(tidx<0 || tidx>=m_Nt) return NULL;

    float* ts = _tr+tidx*m_Ns;


    if(_ti[tidx]==0)
    {
        emit smpRequest(_cs[tidx],_ch[tidx],ts, _ti[tidx]);
    }

    if(_ti[tidx]==0) return NULL;

    return ts;
}

void  GfxSrcSeisMemSegd::setSmp(int, int, float)
{

}
