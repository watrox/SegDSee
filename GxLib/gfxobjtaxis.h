#ifndef GFXOBJTAXIS_H
#define GFXOBJTAXIS_H

#include "gfxobj.h"

class GfxObjTAxis : public GfxObj
{
protected:
    double m_Ti;

    virtual void DoDraw();


public:
    GfxObjTAxis(QObject *parent = 0);

    void  setTi(double v)
    {
        m_Ti=v; Update();
    }


    virtual double X1() {return 0; }
    virtual double X2() {return pix2x(gfx->W()); }


};

#endif // GFXOBJTAXIS_H
