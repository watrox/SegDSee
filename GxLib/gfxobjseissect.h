#ifndef GFXOBJSEISSECT_H
#define GFXOBJSEISSECT_H

#include <QRgb>

#include "gfxobjseis.h"
#include "gfxsrcseis.h"

class GfxObjSeisSect : public GfxObjSeis
{
protected:

    int         m_SelTr;              // Selected trace index

    int*   _si;
    int*   _sj;
    int    _nj;
    int    _ns;


public:

    GfxObjSeisSect(QObject *parent = 0);

    virtual ~GfxObjSeisSect();

    virtual void   DoDraw()  ;
    virtual void   DoPreset();

    void  setSelTr(int v)
    {
        m_SelTr=v; Update();
    }

    int SelTr()
    {
        return m_SelTr;
    }


    float getSelMaxAmp();

};

#endif // GFXOBJSEISSECT_H
