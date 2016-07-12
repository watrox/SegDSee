#ifndef GFXOBJSEISHDRS_H
#define GFXOBJSEISHDRS_H

#include <QString>
#include <QList>

#include "gfxobjseis.h"

class GfxObjSeisHdrs : public GfxObjSeis
{
protected:

    QList<QString> m_hdrs;

    virtual void DoDraw();

public:

    virtual double Y1() {return 0; }
    virtual double Y2() {return pix2y(gfx->H()); }

    GfxObjSeisHdrs(QObject *parent = 0);

    void setHdrs(QList<QString> v);
};

#endif // GFXOBJSEISHDRS_H
