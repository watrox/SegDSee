#ifndef GFXOBJSEIS_H
#define GFXOBJSEIS_H

#include <QRgb>

#include "gfxobj.h"
#include "gfxsrcseis.h"
//#include "sectlist.h"

class SectList;

class GfxObjSeis : public GfxObj
{
friend class GfxSctList;

protected:

    GfxSrcSeis* s_src;

    int         m_Mode;

    bool        m_DispWig;
    bool        m_DispCol;
    int         m_WFill;
    QRgb        m_SLcolor;           // Selected trace color
    QRgb        m_WLcolor;           // Wiggle color
    QRgb        m_WFcolor;           // Wiggle fill Color

    double      m_Gc;                // Color Gain
    double      m_Gw;                // Wiggle Gain
    double      m_Tw;                // Trace width (int pos units)

    QList<QRgb> m_Pal; // Color Palette

public:

    void  setMode(int v)
    {
        m_Mode=v; Update();
    }


    virtual void  setTw(double v)
    {
        m_Tw=v; Update();
    }

    virtual void  setGc(double v)
    {
        m_Gc=v*128.; Update();
    }

    virtual double Gc()
    {
        return m_Gc/128.;
    }

    virtual double  Gw()
    {
        return m_Gw;
    }

    virtual void  setGw(double v)
    {
        m_Gw=v; Update();
    }

    virtual void  setDispWig(bool v)
    {
        m_DispWig=v; Update();
    }


    virtual bool  DispWig()
    {
        return m_DispWig;
    }

    virtual void  setDispCol(bool v)
    {
        m_DispCol=v; Update();
    }

    virtual bool  DispCol()
    {
        return m_DispCol;
    }

    virtual void  setWFill(int v)
    {
        m_WFill =  v; //  1 - pos; -1 - neg; 0 - no fill
        Update();
    }

    virtual int  WFill()
    {
        return m_WFill;
    }

    virtual void  setWLcolor(QRgb v)
    {
        m_WLcolor=v; Update();
    }

    virtual void  setWFcolor(QRgb v)
    {
        m_WFcolor=v; Update();
    }

    virtual void  setSLcolor(QRgb v)
    {
        m_SLcolor=v; Update();
    }

    virtual void  copySettings(GfxObjSeis* src)
    {
        m_SLcolor = src->m_SLcolor;
        m_WFcolor = src->m_WFcolor;
        m_WLcolor = src->m_WLcolor;
        m_WFill   = src->m_WFill;
        m_DispCol = src->m_DispCol;
        m_DispWig = src->m_DispWig;
        m_Gw      = src->m_Gw;
        m_Gc      = src->m_Gc;
        m_Pal     = src->m_Pal;

        Update();
    }

    GfxObjSeis(QObject *parent = 0);

    virtual void setPal (QList<QRgb> v)
    {
        m_Pal = v; Update();
    }


    virtual void setPal ()
    {
        QList<QRgb> p; setPal(p);
    }

    QList<QRgb>* Pal()
    {
        return &m_Pal;
    }

    virtual void  setSrc (GfxSrcSeis* v);

};

#endif // GFXOBJSEIS_H
