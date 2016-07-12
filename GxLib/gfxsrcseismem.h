#ifndef GFXSRCSEISMEM_H
#define GFXSRCSEISMEM_H

#include "gfxsrcseis.h"

class GfxSrcSeisMem : public GfxSrcSeis
{
protected:

    float*  _tr;  // Trace samples
    double* _th;  // Trace headers


public:
    GfxSrcSeisMem(QObject *parent=0);

    void  SetSmp(int nt, float* v)
    {
        if(nt<0 || nt>=m_Nt) return;
        float* ts = _tr+nt*m_Ns;
        memcpy(ts,v,m_Ns*sizeof(float));
        Invalidate();
    }

    void  Zero()
    {
        if(_tr==NULL) return;
        memset(_tr,0,m_Ns*m_Ns*sizeof(float));
        Invalidate();
    }

    virtual ~GfxSrcSeisMem();

    virtual void  setNt(int nt);
    virtual void  setNs(int ns);
    virtual void  setNh(int nh);

    virtual void  setSi(double si)
    {
        m_Si=si;
        Invalidate();
    }

    virtual void  setTi(double ti)
    {
        m_Ti=ti;
        Invalidate();
    }

    virtual float*    Ts(int tidx)
    {
        if(tidx<0 || tidx>=m_Nt) return NULL;
        float* ts = _tr+tidx*m_Ns; return ts;
    }


    virtual double  Th(int tidx, int nitm)
    {
        if(tidx<0 || tidx>=m_Nt) return 0;
        if(nitm<0 || nitm>=m_Nh) return 0;
        int nn = tidx*m_Nh+nitm;
        return _th[nn];
    }


    virtual void  setTh(int tidx, int nitm, double v)
    {
        if(tidx<0 || tidx>=m_Nt) return;
        if(nitm<0 || nitm>=m_Nh) return;
        int nn = tidx*m_Nh+nitm;
        _th[nn]=v;
    }

    virtual double   Tp(int tidx)
    {
        return tidx;
    }

    virtual void  setSmp(int nt, int ns, float v)
    {
        if(nt<0 || nt>=m_Nt) return;
        if(ns<0 || ns>=m_Ns) return;
        float* ts = _tr+nt*m_Ns+ns;
        *ts = v;
        Invalidate();
    }

    virtual float  Smp(int nt, int ns)
    {
        if(nt<0 || nt>=m_Nt) return 0;
        if(ns<0 || ns>=m_Ns) return 0;
        float* ts = _tr+nt*m_Ns+ns;
        return *ts;
    }
};

#endif // GFXSRCSEISMEM_H
