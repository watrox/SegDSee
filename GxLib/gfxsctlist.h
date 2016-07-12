#ifndef SECTLIST_H
#define SECTLIST_H

#include "gfxobjseissect.h"

class GfxSctList : public GfxObjSeisSect
{
protected:

    QVector<GfxObjSeisSect*>items;


public:
    GfxSctList(QObject* parent=0);

    ~GfxSctList();

    virtual void Update()
    {
        setAll();
    }

    void  setItem(int n)
    {
        items[n]->m_Mode     = m_Mode;
        items[n]->m_Gc       = m_Gc;
        items[n]->m_Gw       = m_Gw;
        items[n]->m_Tw       = m_Tw;

        items[n]->m_DispCol  = m_DispCol;
        items[n]->m_DispWig  = m_DispWig;

        items[n]->m_WFill    = m_WFill;

        items[n]->m_WLcolor  = m_WLcolor;
        items[n]->m_WFcolor  = m_WFcolor;

        items[n]->m_Pal     = m_Pal;
        items[n]->Update();
    }

    void  setAll()
    {
        int n;

        for(n=0;n<items.count();n++)
        {
            setItem(n);
        }
    }

    int count() { return items.count(); }

    GfxObjSeisSect* operator[](int i)
    {
        return items.value(i,NULL);
    }

    void append(GfxObjSeisSect* v)
    {
        int n;

        items.append(v); n=items.count()-1; setItem(n);
    }

    void clear()
    {
        int n;

        for(n=0;n<items.count();n++)
        {
            if(items[n]) delete items[n];
        }

        items.clear();
    }
};

#endif // SECTLIST_H
