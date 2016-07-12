#ifndef GFXSRC_H
#define GFXSRC_H

#include <QObject>
#include <QList>

#include "gfxobj.h"

class GfxSrc : public QObject
{
    Q_OBJECT

protected:
    QList<GfxObj*> m_links;

public:
    explicit GfxSrc(QObject *parent = 0);

    virtual ~GfxSrc();

    void RegisterLink(GfxObj* v);

    void  UnRegisterLink(GfxObj* v);

    virtual void Clear ();

    
signals:
    
public slots:
    
};

#endif // GFXSRC_H
