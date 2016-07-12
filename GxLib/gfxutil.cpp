#include "gfxutil.h"


//---------------------------------------------------------------------------

char* q2s(QString qs)
{
    static char s[2048];

//  QByteArray qfn = qs.toLocal8Bit();
    QByteArray qfn = qs.toUtf8();


    strncpy(s,qfn.data(),2048);

    return s;
}

string q2S(QString qs)
{
    string S;

    QByteArray qfn = qs.toUtf8();
//  QByteArray qfn = qs.toLocal8Bit();

    S =qfn.data();

    return S;
}

//---------------------------------------------------------------------------


void CreatePalette24(QRgb* mrkc, int* mrkv, QRgb* rgb, int nmrk)
{
    int    n,m,i,j;

    double r1,b1,g1;
    double r2,b2,g2;
    double rc,bc,gc;
    int    n1,n2;
    QRgb   colors[255]; QRgb  ctmp;
    int    values[255]; int   vtmp;

    gc=bc=rc=0;

    for(n=0;n<nmrk;n++)
    {
        colors[n]=mrkc[n];
        values[n]=mrkv[n];
    }

    for(i=0;i<nmrk-1;i++)
    {
        for(j=nmrk-1;j>i;j--)
        {
            if( values[j] < values[j-1] )
            {
                ctmp=colors[j]; colors[j]=colors[j+1]; colors[j+1]=ctmp;
                vtmp=values[j]; values[j]=values[j+1]; values[j+1]=vtmp;
            }
        }
    }

    for(i=j=1;i<nmrk;i++)
    {
        if(values[i]>values[j-1])
        {
            values[j]=values[i];
            colors[j]=colors[i];
            j++;
        }
    }

    nmrk=j;
    if(nmrk<2) nmrk=2;

    values[0]=0;
    values[nmrk-1]=255;

    for(m=0;m<nmrk-1;m++)
    {
        r1=qRed  (colors[m]);
        g1=qGreen(colors[m]);
        b1=qBlue (colors[m]);

        r2=qRed  (colors[m+1]);
        g2=qGreen(colors[m+1]);
        b2=qBlue (colors[m+1]);

        n1=values[m];
        n2=values[m+1];

        for(n=n1;n<=n2;n++)
        {
            if(n2==n1) rc = r1;
            else       rc = r1 + (r2-r1) / (n2-n1) * (n-n1);

            if(n2==n1) gc = g1;
            else       gc = g1 + (g2-g1) / (n2-n1) * (n-n1);

            if(n2==n1) bc = b1;
            else       bc = b1 + (b2-b1) / (n2-n1) * (n-n1);


            //rgb[n]=RGB24((int)rc,(int)gc,(int)bc);
            rgb[n] = QColor(rc,gc,bc).rgb();

        }
    }
    //rgb[255]=RGB24((int)rc,(int)gc,(int)bc);
    rgb[255] = QColor(rc,gc,bc).rgb();
}

void CreatePalette24(QRgb* mrkc, double* v, double vmin, double vmax, QRgb* rgb, int nmrk)
{
    int* mrkv = new int[nmrk];

    int  vv,n;

    for(n=0;n<nmrk;n++)
    {
        vv = (v[n]-vmin)/(vmax-vmin)*255;
        if(vv<0) vv=0; if(vv>255) vv=255;
        mrkv[n]=vv;
    }

    CreatePalette24(mrkc, mrkv, rgb, nmrk);

    delete [] mrkv;
}

QList<QRgb> CreatePalette24(QRgb* mrkc, double* v, double vmin, double vmax, int nmrk)
{
    QRgb rgb[256];

    int* mrkv = new int[nmrk];

    int  vv,n;

    for(n=0;n<nmrk;n++)
    {
        vv = (v[n]-vmin)/(vmax-vmin)*256;
        if(vv<0) vv=0; if(vv>255) vv=255;
        mrkv[n]=vv;
    }

    CreatePalette24(mrkc, mrkv, rgb, nmrk);

    delete [] mrkv;


    QList<QRgb> RGB;

    for(n=0;n<256;n++)
    {
        RGB.append(rgb[n]);
    }

    return RGB;

}

QRgb RGB24(int r, int g, int b)
{
    return QColor(r,g,b).rgb(); //RGB24(r,g,b);
}
