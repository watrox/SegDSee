#ifndef GFXSRCSEISTEST_H
#define GFXSRCSEISTEST_H

#include "gfxsrcseis.h"

class GfxSrcSeisTest : public GfxSrcSeis
{
protected:
    float* _tr;

public:
    GfxSrcSeisTest(QObject *parent = 0);

    virtual ~GfxSrcSeisTest();

    virtual double   Tp(int tidx)
    {
        if(m_Nt>tidx) return tidx;
        return 0;
    }

    virtual double  Th(int tidx, int)
    {
        if(m_Nt>tidx) return tidx;
        return 0;
    }


    virtual void  setTh(int, int, double)
    {
    }


//   virtual double  GetNTraceByPos(double p);

    virtual void  setNt(int nt)
    {
        m_Nt=nt;
        Invalidate();
    }

    virtual void  setNs(int ns)
    {
        m_Ns=ns;

        if(_tr) delete [] _tr; _tr=NULL;

        if(m_Ns>0)
        {
            int n2=m_Ns*2;
            _tr = new float [n2];
            for(int n=0; n<n2; n++) _tr[n]=sin(n/10.);
            //if(n2>100) _tr[100]=2;
        }

        Invalidate();
    };

    virtual void  setSi(double si)
    {
        m_Si=si;
        Invalidate();
    };

    virtual void  setTi(double ti)
    {
        m_Ti=ti;
        Invalidate();
    };

    virtual float*    Ts(int tidx)
    {
        int no=tidx%m_Ns;
        if(tidx<0 || tidx>=m_Nt) return NULL;
        float* ts = _tr+no;
        return ts;
    };


};

#endif // GFXSRCSEISTEST_H
