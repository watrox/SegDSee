#ifndef GFXOBJ_H
#define GFXOBJ_H

#include <QObject>

#include <math.h>

#include "gfxutil.h"
#include "gfx.h"


class GfxView;
class GfxSrc;

class GfxObj  : public QObject
{
    Q_OBJECT
protected:

    int   L;
    int   R;
    int   W;
    int   H;
    int   T;
    int   B;

    Gfx* gfx;

    double xpmm;
    double ypmm;

    bool   _preset;

    bool   m_trim;

    double m_Xo;
    double m_Yo;

    double m_X1;
    double m_X2;
    double m_Xs;

    double m_Y1;
    double m_Y2;
    double m_Ys;

    int    m_Layer;

    GfxView* m_view;
    GfxSrc*  m_src;

public:
    explicit  GfxObj(QObject *parent = 0);

    virtual ~GfxObj();

    double xSign(double x)
    {
        if(x<0) return -1;
        return 1;
    }

    double x2mm(double x)
    {
      double s = xSign(m_X2-m_X1);
      double xx = s*(x-m_X1);
      return xx*m_Xs+m_Xo;
    }

    double mm2x(double mm)
    {
      double s  = xSign(m_X2-m_X1);
      double xx = (mm-m_Xo)/m_Xs;
      double x = xx*s+m_X1;
      return x;
    }

    double y2mm(double y)
    {
      double s = xSign(m_Y2-m_Y1);
      double yy = s*(y-m_Y1);
      return yy*m_Ys+m_Yo;
    }

    double mm2y(double mm)
    {
      double s  = xSign(m_Y2-m_Y1);
      double yy = (mm-m_Yo)/m_Ys;
      double y = yy*s+m_Y1;
      return y;
    }

    double x2pix(double x)
    {
        return round(x2mm(x)*xpmm);
    }

    double y2pix(double y)
    {
        return round(y2mm(y)*ypmm);
    }

    double pix2x(int xx)
    {
        return mm2x(xx/xpmm);
    }

    double pix2y(int yy)
    {
        return mm2y(yy/ypmm);
    }

    double x2fpix(double x)
    {
        return x2mm(x)*xpmm;
    }

    double y2fpix(double y)
    {
        return y2mm(y)*ypmm;
    }

    virtual void setXo(double v);
    virtual void setYo(double v);

    virtual void setX1(double v);
    virtual void setX2(double v);
    virtual void setXs(double v);

    virtual void setY1(double v);
    virtual void setY2(double v);
    virtual void setYs(double v);

    virtual double Xo() {return m_Xo; }
    virtual double Yo() {return m_Yo; }

    virtual double X1() {return m_X1; }
    virtual double X2() {return m_X2; }
    virtual double Xs() {return m_Xs; }

    virtual double Y1() {return m_Y1; }
    virtual double Y2() {return m_Y2; }
    virtual double Ys() {return m_Ys; }

    int    WidthView ();
    int    HeightView();

    double WidthMM ();
    double HeightMM();

    double WidthPix ();
    double HeightPix();

    void setView(GfxView* v);
    void setSrc (GfxSrc*  v);

    void setLayer(int layer);
    int     Layer() { return m_Layer; }

    bool Trim() { return m_trim; }
    void setTrim(bool v);

    void Draw();

    virtual void Update();
    void Preset();

    virtual void  DoPreset() { _preset=false; }
    virtual void  DoDraw();

/*
    bool Hit(int x, int y)
    {
        //return (PointInRange(Left,Right,x) && PointInRange(Top,Bottom,y));
        return false;
    }
*/

signals:
    
public slots:
    
};

#endif // GFXOBJ_H
