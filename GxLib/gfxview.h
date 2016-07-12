#ifndef Gm_XVIEW_H
#define Gm_XVIEW_H

#include <QWidget>
#include <QList>
#include <QPaintEvent>
#include <QPainter>
#include <QPaintEngine>

#include <math.h>

#include "gfx.h"
#include "gfxobj.h"


class GfxView : public QWidget
{
    Q_OBJECT
protected:

    Gfx   m_gfx;

    QList<GfxObj*> m_links;

    double  m_Xpmm;
    double  m_Ypmm;
    double  m_Xs;
    double  m_Ys;
    
    int    _preset;
    int    _nlinks;

    virtual void paintEvent(QPaintEvent* pe);

    virtual void mousePressEvent(QMouseEvent * event);

    virtual void wheelEvent(QWheelEvent* event);

    void Preset(void);

//  GfxObj* ObjHit(int x, int y);

public:
    explicit GfxView(QWidget *parent = 0);

    ~GfxView();

    double Xpmm() {return m_Xpmm; }
    double Ypmm() {return m_Ypmm; }

    double Xs() { return m_Xs; }
    double Ys() { return m_Ys; }

    void  setXs (double v)
    {
        m_Xs=v; _preset=true; Preset(); update();
    }

    void  setYs (double v)
    {
        m_Ys=v; _preset=true; Preset(); update();
    }

    void RegisterLink(GfxObj* v);

    void  UnRegisterLink(GfxObj* v);

    Gfx* getGfx()
    {
        return &m_gfx;
    }


    
signals:
    void OnPrevDraw       (GfxView* view);
    void OnPostDraw       (GfxView* view);
    void mouseEvent       (QMouseEvent* event);
    void wheel_Event      (QWheelEvent* event);
    void timeSetEvent(double time);
    
public slots:
    
};

#endif // Gm_XVIEW_H
