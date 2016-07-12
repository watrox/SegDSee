#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>

#include "gfxview.h"
#include "gfx.h"

GfxView::GfxView(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(5000, 5000);

    QDesktopWidget desk;

    int dpix = desk.logicalDpiX();
    int dpiy = desk.logicalDpiY();
    m_Xpmm=dpix/25.5;
    m_Ypmm=dpiy/25.5;

    m_Xs=1;
    m_Ys=1;
    setMouseTracking(true);
}

GfxView::~GfxView()
{
    int n;

    for(n=0; n<m_links.count();n++) m_links[n]->setView(NULL);
}
// ----------------------------------------------------------------------

void GfxView::RegisterLink(GfxObj* v)
{
    m_links.append(v);
    _nlinks = m_links.count();
    update();
}

void  GfxView::UnRegisterLink(GfxObj* v)
{
    _nlinks = m_links.count();
    m_links.removeAll(v);
    _nlinks = m_links.count();
    update();
}
// ----------------------------------------------------------------------

void  GfxView::Preset()
{
    int n;
    int nobj = m_links.count();

    for(n=0; n<nobj;n++)
    {
        m_links[n]->Preset();
    }
}


// ----------------------------------------------------------------------


static bool myLessThan( GfxObj *a, GfxObj *b )
{
    return a->Layer() < b->Layer();
}


void GfxView::paintEvent(QPaintEvent* pe)
{
    int n, nobj;

    QPainter painter;

    QRect r = pe->rect();

    QPaintEngine* engine;

    engine = painter.paintEngine();

    painter.begin(this);

    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing,false);
    painter.setRenderHint(QPainter::SmoothPixmapTransform,false);

    engine = painter.paintEngine();

    int dpix = engine->paintDevice()->logicalDpiX();
    int dpiy = engine->paintDevice()->logicalDpiY();

     m_Xpmm = dpix/25.5;
     m_Ypmm = dpiy/25.5;

//   qDebug() << "m_Ypmm:" << m_Ypmm;

     m_gfx.SetViewPort(&painter,&r);

    int rc = receivers(SIGNAL(OnPrevDraw(GfxView*)));
    if(rc>0)
     {
        emit OnPrevDraw(this);
     }

     nobj = m_links.count();

     if(m_links.count()>0)
     {
         qSort( m_links.begin(), m_links.end(), myLessThan );
     }


     for(n=0; n<nobj;n++)
     {
         m_links[n]->Draw();
     }

     rc = receivers(SIGNAL(OnPostDraw(GfxView*)));
     if(rc>0)
      {
         emit OnPostDraw(this);
      }

     m_gfx.Paint();

//   painter.drawRect(r);
//   painter.drawLine(r.left(),r.top(),r.right(),r.bottom());

}

/*
GfxObj* GfxView::ObjHit(int x, int y)
{
    GfxObj*  obj_hit = NULL;
    //int      layer   = -1;
    //int      n;

    if(m_links.count()>0)
    {
      for(n=0;n<m_links.count();n++)
      {
          GfxObj* obj = m_links[n];

          if(obj->Hit(x,y))
          {
            //if(obj->gxLayer>=layer)
            //{
                obj_hit = obj;
            //    layer   = obj->gxLayer;
            //}
          }
      }
    }
    return obj_hit;
}
*/
void GfxView::mousePressEvent(QMouseEvent * event)
{
    emit mouseEvent(event);
    QWidget::mousePressEvent(event);
}


void GfxView::wheelEvent(QWheelEvent* event)
{
/*
    double d = event->delta();
    double z = 1+0.002*abs(d); if(d<0) z = 1/z;

    int xm = event->pos().x();
    int ym = event->pos().y();
*/
    emit wheel_Event(event);

    QWidget::wheelEvent(event);
}
