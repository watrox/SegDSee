#ifndef GFX_H
#define GFX_H

#define SZ 12000

#include <QObject>
#include <QImage>
#include <QPainter>
#include <QRgb>



int Bresenham(int x0, int y0, int x1, int y1, int* x, int *y);

class Gfx
{
protected:

    int m_w, m_ww, m_wb;
    int m_h;
    int m_x;
    int m_y;
    int m_t, m_tx1, m_tx2, m_ty1, m_ty2; // trim

    bool _trim;

    int z;

    int    _cy[SZ];
    int    _cv[SZ];

    QFont      _font;

    QImage     _img;
    QPainter*  _vpainter;

    QPainter   _ipainter;

    QRgb       _pal    [256];

    void mSetPixel(int c, int x, int y);


public:
    explicit Gfx();

    void SetViewPort(QPainter* p, QRect *r);

    void Paint();

    int  GetPalColor(int cidx);

    void DrawHLine(int y, int x0, int x1, int w, int cidx);
    void DrawVLine(int x, int y0, int y1, int w, int cidx);

    void DrawLine(int x0, int y0, int x1, int y1, int cidx);
    void DrawLine(int x0, int y0, int x1, int y1, int cidx, int w, int wc= -1);

    void DrawRampLine(int xa, int ya, int xb, int yb, int ca, int cb);
    void DrawRampLine(int x0, int y0, int x1, int y1, int c0, int c1, int w);

    void DrawPLine(int* x, int* y, int n, int  cidx,  int w=1);
    void DrawPLine(int* x, int* y, int* cidx, int n,  int w=1);
    void DrawPLine(int* x, int* y, int* cidx, int* w, int n, int wc= -1);

    void DrawRect(int x1, int y1, int x2, int y2, int cidx);
    void FillRect(int x1, int y1, int x2, int y2, int cidx);

    void DrawCircle(int xCenter, int yCenter, int r, int cidx, int w, int wc);
    void DrawCircle(int xCenter, int yCenter, int r, int cidx);
    void FillCircle(int xCenter, int yCenter, int r, int cidx);

    inline void DrawHRamp1(int y, int x0, int x1, int v0, int v1, int w);
    inline void DrawHRamp2(int y, int x0, int x1, int v0, int v1, int lx, int rx, int w);

    void DrawWiggleTraceTB
       (int x, int* yi, int* yj, int nny, float* s, int fill,double g, int clp,
        int fc, int wc);

    void Draw2ColorTraceTB
            (int x1, int* yi1, int* yj1, int nny1, float* s1,
             int x2, int* yi2, int* yj2, int nny2, float* s2,
             int lx, int rx,
             double g, int mode);


    inline void PrepareColorTrace
                (int* yi, int* yj, int nny, float* s, double g,
                 int  ya, int  yb, int* yp, int* vp);

    void PrepareVelTrace(int ya, int yb, int ll, int* yy, int* vv, int* v);


    void Draw2VelTraceTB(
                             int xp, int ll1, int* yy1, int* vv1,
                             int xc, int ll2, int* yy2, int* vv2,
                             int xa, int xb, int ya, int yb);

    void DrawText(int x, int y, QString str, int color=0, double rotate=0, int size=11, bool fill_bg = true);

    int Xo() { return m_x; }
    int Yo() { return m_y; }

    int H() { return m_h; }
    int W() { return m_w; }

    void SetPixel(int v, int x, int y);

    int GetTextWidth (QString str);
    int GetTextHeight();

    void SetTrim(int x1, int x2, int y1, int y2);
    void ClrTrim();

    void SetPalette(QList<QRgb> pal);

    
signals:
    
public slots:
    
};

#endif // GFX_H
