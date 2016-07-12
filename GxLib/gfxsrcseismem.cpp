#include "gfxsrcseismem.h"

GfxSrcSeisMem::GfxSrcSeisMem(QObject *parent) : GfxSrcSeis(parent)
{
    m_Nt   = 1;
    m_Nh   = 1;
    m_Ns   = 1000;
    m_Si   = 0.004;
    m_Ti   = 25;

    _tr = new float  [m_Nt*m_Ns]; memset(_tr,0,m_Nt*m_Ns*sizeof(float));
    _th = new double [m_Nt*m_Nh]; memset(_th,0,m_Nt*m_Nh*sizeof(double));

}

GfxSrcSeisMem::~GfxSrcSeisMem()
{
    if(_tr) delete [] _tr;
    if(_th) delete [] _th;
}

void GfxSrcSeisMem::setNt(int nt)
{
    m_Nt=nt;

    if(_tr) delete [] _tr; _tr=NULL;
    if(_th) delete [] _th; _th=NULL;

    if(m_Nt*m_Ns>0)
    {
        _tr = new float[m_Nt*m_Ns];
        memset(_tr,0,m_Nt*m_Ns*sizeof(float));
    }
    else
    {
        _tr=NULL;
    }

    if(m_Nt*m_Nh>0)
    {
        _th = new double [m_Nt*m_Nh];
        memset(_th,0,m_Nt*m_Nh*sizeof(double));
    }
    else
    {
        _th=NULL;
    }

    Invalidate();
}

void GfxSrcSeisMem::setNs(int ns)
{
    m_Ns=ns;

    if(_tr) delete [] _tr; _tr=NULL;

    m_Ns=ns;

    if(m_Nt*m_Ns>0)
    {
        _tr = new float [m_Nt*m_Ns]; memset(_tr,0,m_Nt*m_Ns*sizeof(float));
    }
    else
    {
        _tr=NULL;
    }

    Invalidate();
}

void GfxSrcSeisMem::setNh(int nh)
{
    m_Nh=nh;

    if(_th) delete [] _th; _th=NULL;

    if(m_Nt*m_Nh>0)
    {
        _th = new double [m_Nt*m_Nh]; memset(_th,0,m_Nt*m_Nh*sizeof(double));
    }
    else
    {
        _th=NULL;
    }

    Invalidate();
}

/*
void  GfxSrcSeisMem::setTh(int tidx, int nitm, double v)
{
    if(tidx<0 || tidx>=m_Nt) return;
    if(nitm<0 || nitm>=m_Nh) return;
    double *h = _th+tidx*m_Nh+nitm; *h = v;
}

double  GfxSrcSeisMem::Th(int tidx, int nitm)
{
    if(tidx<0 || tidx>=m_Nt) return 0;
    if(nitm<0 || nitm>=m_Nh) return 0;
    double* h = _th+tidx*m_Nh+nitm;
    return *h;
}
*/

