#ifndef Gm_XSRCSEIS_H
#define Gm_XSRCSEIS_H

#include "gfxsrc.h"


class GfxSrcSeis : public GfxSrc
{
protected:

    int      m_Nh;   // Number of trace headers
    int      m_Nt;   // Number of traces
    int      m_Ns;   // Number of trace samples

    double   m_To;   // Time 0
    double   m_Si;   // Sample interval
    double   m_Ti;   // Meters per 1 P

    float*   m_Tp;   // Traces positions
    float*   m_Ts;

//  HdrDef m_TrHdrDef;

    virtual void  setNh(int nh)
    {
        m_Nh=nh; Invalidate();
    }

public:
     GfxSrcSeis(QObject *parent=0);

    ~GfxSrcSeis();

    virtual void Invalidate(void);
    virtual void Clear ();


     virtual void setTo(double v)
     {
         m_To=v; Invalidate();
     }

     virtual double Tx(double pos)
     {
         if(pos<0)    return 0;
         if(pos>=m_Nt) return m_Nt-1;
         return pos;
     }

     virtual double Tp(int tidx)
     {
         if(m_Tp && m_Nt>tidx) return m_Tp[tidx];
         else                  return 0;
     }

     virtual float*  Ts(int)
     {
         return NULL;
     }

     virtual double  To()
     {
         return m_To;
     }

     virtual double  Th(int, int)
     {
         return 0;
     }


     virtual double  Si()
     {
         return m_Si;
     }

     virtual double  Ti()
     {
         return m_Ti;
     }

     virtual int  Nt()
     {
         return m_Nt;
     }

     virtual int  Ns()
     {
         return m_Ns;
     }


     virtual void  setTh(int, int, double)
     {
     }


     virtual void  setNt(int)
     {
     }

     virtual void  setNs(int)
     {
     }

     void  setSi(double si)
     {
         m_Si = si; Invalidate();
     }

     void  setTi(double ti)
     {
         m_Ti = ti; Invalidate();
     }
};

#endif // Gm_XSRCSEIS_H
