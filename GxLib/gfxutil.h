#ifndef GFXUTIL_H
#define GFXUTIL_H


#include <QRgb>
#include<QColor>

#include <string>

using namespace std;

class zdata
{
public:
    int    xo, yo;
    double xs, ys;

    zdata(double xxs, double yys, int xxo, int yyo)
    {
        xo=xxo; yo=yyo; xs=xxs; ys=yys;
    }

    zdata() {}
};


char   *q2s(QString qs);
string  q2S(QString qs);

void CreatePalette24(QRgb *mrkc, int* mrkv, QRgb* rgb, int nmrk);
void CreatePalette24(QRgb* mrkc, double* v, double vmin, double vmax, QRgb* rgb, int nmrk);

QList<QRgb> CreatePalette24 (QRgb* mrkc, double* v, double vmin, double vmax, int nmrk);


QRgb RGB24(int r, int g, int b);

#endif // GFXUTIL_H
