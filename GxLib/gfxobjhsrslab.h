#ifndef GFXOBJHSRSLAB_H
#define GFXOBJHSRSLAB_H

#include <QObject>
#include <QList>
#include "gfxobj.h"
#include "gfxview.h"

class GfxObjHsrsLab : public GfxObj
{
protected:

    QList<QString> m_hdrs;

    virtual void DoDraw();

public:

    GfxObjHsrsLab(QObject *parent = 0);
    ~GfxObjHsrsLab();

    void setHdrs(QList<QString> v);
};

#endif // GFXOBJHSRSLAB_H
